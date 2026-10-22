/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * spell.c: code for spell checking
 *
 * See spellfile.c for the Vim spell file format.
 *
 * The spell checking mechanism uses a tree (aka trie).  Each node in the tree
 * has a list of bytes that can appear (siblings).  For each byte there is a
 * pointer to the node with the byte that follows in the word (child).
 *
 * A NUL byte is used where the word may end.  The bytes are sorted, so that
 * binary searching can be used and the NUL bytes are at the start.  The
 * number of possible bytes is stored before the list of bytes.
 *
 * The tree uses two arrays: "byts" stores the characters, "idxs" stores
 * either the next index or flags.  The tree starts at index 0.  For example,
 * to lookup "vi" this sequence is followed:
 *	i = 0
 *	len = byts[i]
 *	n = where "v" appears in byts[i + 1] to byts[i + len]
 *	i = idxs[n]
 *	len = byts[i]
 *	n = where "i" appears in byts[i + 1] to byts[i + len]
 *	i = idxs[n]
 *	len = byts[i]
 *	find that byts[i + 1] is 0, idxs[i + 1] has flags for "vi".
 *
 * There are two word trees: one with case-folded words and one with words in
 * original case.  The second one is only used for keep-case words and is
 * usually small.
 *
 * There is one additional tree for when not all prefixes are applied when
 * generating the .spl file.  This tree stores all the possible prefixes, as
 * if they were words.  At each word (prefix) end the prefix nr is stored, the
 * following word must support this prefix nr.  And the condition nr is
 * stored, used to lookup the condition that the word must match with.
 *
 * Thanks to Olaf Seibert for providing an example implementation of this tree
 * and the compression mechanism.
 * LZ trie ideas, original link (now dead)
 *	irb.hr/hr/home/ristov/papers/RistovLZtrieRevision1.pdf
 * More papers: http://www-igm.univ-mlv.fr/~laporte/publi_en.html
 *
 * Matching involves checking the caps type: Onecap ALLCAP KeepCap.
 *
 * Why doesn't Vim use aspell/ispell/myspell/etc.?
 * See ":help develop-spell".
 */

#define IN_SPELL_C
#include "vim.h"

#အကယ်၍ defined(FEAT_SPELL)

#include <time.h>

#define REGION_ALL 0xff		// word valid in all regions

// Result values.  Lower number is accepted over higher one.
#define SP_BANNED	(-1)
#define SP_OK		0
#define SP_RARE		1
#define SP_LOCAL	2
#define SP_BAD		3

/*
 * Structure to store info for word matching.
 */
typedef sa_nracanaa matchinf_S
{
    langp_T	*mi_lp;			// info for language and region

    // pointers to original text to be checked
    char_u	*mi_word;		// start of word being checked
    char_u	*mi_end;		// end of matching word so far
    char_u	*mi_fend;		// next char to be added to mi_fword
    char_u	*mi_cend;		// char after what was used for
					// mi_capflags

    // case-folded text
    char_u	mi_fword[MAXWLEN + 1];	// mi_word case-folded
    ကိန်းပြည့်		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    ကိန်းပြည့်		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    ကိန်းပြည့်		mi_prefcnt;		// number of entries at mi_prefarridx
    ကိန်းပြည့်		mi_prefixlen;		// byte length of prefix
    ကိန်းပြည့်		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    ကိန်းပြည့်		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    ကိန်းပြည့်		mi_complen;		// nr of compound words used
    ကိန်းပြည့်		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    ကိန်းပြည့်		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    ကိန်းပြည့်		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    ကိန်းပြည့်		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


static ကိန်းပြည့် spell_mb_isword_class(ကိန်းပြည့် cl, win_T *wp);

// mode values for find_word
#define FIND_FOLDWORD	    0	// find word case-folded
#define FIND_KEEPWORD	    1	// find keep-case word
#define FIND_PREFIX	    2	// find word after prefix
#define FIND_COMPOUND	    3	// find case-folded compound word
#define FIND_KEEPCOMPOUND   4	// find keep-case compound word

// type values for get_char_type
#define CHAR_OTHER	0
#define CHAR_UPPER	1
#define CHAR_DIGIT	2

static void find_word(matchinf_T *mip, ကိန်းပြည့် mode);
static void find_prefix(matchinf_T *mip, ကိန်းပြည့် mode);
static ကိန်းပြည့် fold_more(matchinf_T *mip);
static void spell_load_cb(char_u *fname, void *cookie);
static ကိန်းပြည့် count_syllables(slang_T *slang, char_u *word);
static void clear_midword(win_T *buf);
static void use_midword(slang_T *lp, win_T *buf);
static ကိန်းပြည့် find_region(char_u *rp, char_u *region);
static void spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
static void spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
static void spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
static void dump_word(slang_T *slang, char_u *word, char_u *pat, ကိန်းပြည့် *dir, ကိန်းပြည့် round, ကိန်းပြည့် flags, linenr_T lnum);
static linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, ကိန်းပြည့် *dir, ကိန်းပြည့် round, ကိန်းပြည့် flags, linenr_T startlnum);
static char_u *advance_camelcase_word(char_u *p, win_T *wp, ကိန်းပြည့် *is_camel_case);

/*
 * Main spell-checking function.
 * "ptr" points to a character that could be the start of a word.
 * "*attrp" is set to the highlight index for a badly spelled word.  For a
 * non-word or when it's OK it remains unchanged.
 * This must only be called when 'spelllang' is not empty.
 *
 * "capcol" is used to check for a Capitalised word after the end of a
 * sentence.  If it's zero then perform the check.  Return the column where to
 * check next, or -1 when no sentence end was found.  If it's NULL then don't
 * worry.
 *
 * Returns the length of the word in bytes, also when it's OK, so that the
 * caller can skip over the word.
 */
    ကိန်းပြည့်
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    ကိန်းပြည့်		*capcol,	// column to check for Capital
    ကိန်းပြည့်		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    ကိန်းပြည့်		nrlen = 0;	// found a number first
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		wrongcaplen = 0;
    ကိန်းပြည့်		lpi;
    ကိန်းပြည့်		count_word = docount;
    ကိန်းပြည့်		use_camel_case = *wp->w_s->b_p_spo != NUL;
    ကိန်းပြည့်		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    အကယ်၍ (*ptr <= ' ')
	ပြန်ပေး 1;

    // Return here when loading language files failed.
    အကယ်၍ (wp->w_s->b_langp.ga_len == 0)
	ပြန်ပေး 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    အကယ်၍ (*ptr >= '0' && *ptr <= '9')
    {
	အကယ်၍ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	မဟုတ်ပါက အကယ်၍ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	မဟုတ်ပါက
	    mi.mi_end = skipdigits(ptr);
	nrlen = (ကိန်းပြည့်)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    အကယ်၍ (spell_iswordp(mi.mi_fend, wp))
    {
	အကယ်၍ (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	မဟုတ်ပါက
	{
	    do
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } နေစဉ် (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	အကယ်၍ (capcol != NULL && *capcol == 0 && wp->w_s->b_cap_prog != NULL)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    အကယ်၍ (!SPELL_ISUPPER(c))
		wrongcaplen = (ကိန်းပြည့်)(mi.mi_fend - ptr);
	}
    }
    အကယ်၍ (capcol != NULL)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = NULL;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    အကယ်၍ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (void)spell_casefold(wp, ptr, (ကိန်းပြည့်)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (ကိန်းပြည့်)STRLEN(mi.mi_fword);

    အကယ်၍ (is_camel_case && mi.mi_fwordlen > 0)
	// Introduce a fake word end space into the folded word.
	mi.mi_fword[mi.mi_fwordlen - 1] = ' ';

    // The word is bad unless we recognize it.
    mi.mi_result = SP_BAD;
    mi.mi_result2 = SP_BAD;

    /*
     * Loop over the languages specified in 'spelllang'.
     * We check them all, because a word may be matched longer in another
     * language.
     */
    အတွက် (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	အကယ်၍ (mi.mi_lp->lp_slang->sl_fidxs == NULL)
	    continue;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	အကယ်၍ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	အကယ်၍ (count_word && mi.mi_result == SP_OK)
	{
	    count_common_word(mi.mi_lp->lp_slang, ptr,
						   (ကိန်းပြည့်)(mi.mi_end - ptr), 1);
	    count_word = FALSE;
	}
    }

    အကယ်၍ (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	အကယ်၍ (nrlen > 0)
	{
	    အကယ်၍ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		ပြန်ပေး nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	မဟုတ်ပါက အကယ်၍ (!spell_iswordp_nmw(ptr, wp))
	{
	    အကယ်၍ (capcol != NULL && wp->w_s->b_cap_prog != NULL)
	    {
		regmatch_T	regmatch;
		ကိန်းပြည့်		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		အကယ်၍ (r)
		    *capcol = (ကိန်းပြည့်)(regmatch.endp[0] - ptr);
	    }

	    အကယ်၍ (has_mbyte)
		ပြန်ပေး (*mb_ptr2len)(ptr);
	    ပြန်ပေး 1;
	}
	မဟုတ်ပါက အကယ်၍ (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	မဟုတ်ပါက အကယ်၍ (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    ကိန်းပြည့်		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    အကယ်၍ (mi.mi_lp->lp_slang->sl_fidxs != NULL)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		အတွက် (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    အကယ်၍ (p >= mi.mi_end)
			bha_nga;
		    mi.mi_compoff = (ကိန်းပြည့်)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    အကယ်၍ (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			bha_nga;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	အကယ်၍ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	မဟုတ်ပါက အကယ်၍ (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	မဟုတ်ပါက
	    *attrp = HLF_SPL;
    }

    အကယ်၍ (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ပြန်ပေး wrongcaplen;
    }

    ပြန်ပေး (ကိန်းပြည့်)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    static ကိန်းပြည့်
get_char_type(ကိန်းပြည့် c)
{
    အကယ်၍ (VIM_ISDIGIT(c))
	ပြန်ပေး CHAR_DIGIT;
    အကယ်၍ (SPELL_ISUPPER(c))
	ပြန်ပေး CHAR_UPPER;
    ပြန်ပေး CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    static char_u *
advance_camelcase_word(char_u *str, win_T *wp, ကိန်းပြည့် *is_camel_case)
{
    ကိန်းပြည့် last_type, last_last_type, this_type;
    ကိန်းပြည့် c;
    char_u *end = str;

    *is_camel_case = FALSE;

    အကယ်၍ (*str == NUL)
	ပြန်ပေး str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    နေစဉ် (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	အကယ်၍ (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    bha_nga;
	}
	မဟုတ်ပါက အကယ်၍ ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    bha_nga;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    ပြန်ပေး end;
}

/*
 * Check if the word at "mip->mi_word" is in the tree.
 * When "mode" is FIND_FOLDWORD check in fold-case word tree.
 * When "mode" is FIND_KEEPWORD check in keep-case word tree.
 * When "mode" is FIND_PREFIX check for word after prefix in fold-case word
 * tree.
 *
 * For a match mip->mi_result is updated.
 */
    static void
find_word(matchinf_T *mip, ကိန်းပြည့် mode)
{
    idx_T	arridx = 0;
    ကိန်းပြည့်		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    ကိန်းပြည့်		endidxcnt = 0;
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		wlen = 0;
    ကိန်းပြည့်		flen;
    ကိန်းပြည့်		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    ကိန်းပြည့်		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    ကိန်းပြည့်		word_ends;
    ကိန်းပြည့်		prefix_found;
    ကိန်းပြည့်		nobreak_result;

    အကယ်၍ (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	အကယ်၍ (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    မဟုတ်ပါက
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	အကယ်၍ (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	မဟုတ်ပါက အကယ်၍ (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    အကယ်၍ (byts == NULL)
	ပြန်ပေး;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    အတွက် (;;)
    {
	အကယ်၍ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	အကယ်၍ (byts[arridx] == 0)
	{
	    အကယ်၍ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		ပြန်ပေး;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    နေစဉ် (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    အကယ်၍ (len == 0)
		bha_nga;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	အကယ်၍ (ptr[wlen] == NUL)
	    bha_nga;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	အကယ်၍ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	နေစဉ် (lo < hi)
	{
	    m = (lo + hi) / 2;
	    အကယ်၍ (byts[m] > c)
		hi = m - 1;
	    မဟုတ်ပါက အကယ်၍ (byts[m] < c)
		lo = m + 1;
	    မဟုတ်ပါက
	    {
		lo = hi = m;
		bha_nga;
	    }
	}

	// Stop if there is no matching byte.
	အကယ်၍ (hi < lo || byts[lo] != c)
	    bha_nga;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	အကယ်၍ (c == ' ')
	{
	    အတွက် (;;)
	    {
		အကယ်၍ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		အကယ်၍ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    bha_nga;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    နေစဉ် (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	အကယ်၍ ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    continue;	    // not at first byte of character
	အကယ်၍ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    အကယ်၍ (slang->sl_compprog == NULL && !slang->sl_nobreak)
		continue;	    // next char is a word character
	    word_ends = FALSE;
	}
	မဟုတ်ပါက
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	အကယ်၍ (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    အကယ်၍ (STRNCMP(ptr, p, wlen) != 0)
	    {
		အတွက် (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (ကိန်းပြည့်)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	အတွက် (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    အကယ်၍ (mode == FIND_FOLDWORD)
	    {
		အကယ်၍ (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		အကယ်၍ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    continue;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    မဟုတ်ပါက အကယ်၍ (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		အကယ်၍ (c == 0)
		    continue;

		// Use the WF_RARE flag for a rare prefix.
		အကယ်၍ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    အကယ်၍ (slang->sl_nobreak)
	    {
		အကယ်၍ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    bha_nga;
		}
	    }

	    မဟုတ်ပါက အကယ်၍ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		အကယ်၍ (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    continue;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		အကယ်၍ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			continue;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		အကယ်၍ (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    continue;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		အကယ်၍ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    continue;
		အကယ်၍ (!word_ends && (flags & WF_NOCOMPAFT))
		    continue;

		// Quickly check if compounding is possible with this flag.
		အကယ်၍ (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    continue;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		အကယ်၍ (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    continue;

		အကယ်၍ (mode == FIND_COMPOUND)
		{
		    ကိန်းပြည့်	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    အကယ်၍ (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			အတွက် (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    မဟုတ်ပါက
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    အကယ်၍ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			continue;

		    အကယ်၍ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			အကယ်၍ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    continue;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		အကယ်၍ (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    အကယ်၍ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			အကယ်၍ (ptr == mip->mi_word)
			    (void)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			မဟုတ်ပါက
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    အကယ်၍ (!can_compound(slang, fword, mip->mi_compflags))
			continue;
		}
		မဟုတ်ပါက အကယ်၍ (slang->sl_comprules != NULL
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    continue;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    မဟုတ်ပါက အကယ်၍ (flags & WF_NEEDCOMP)
		continue;

	    nobreak_result = SP_OK;

	    အကယ်၍ (!word_ends)
	    {
		ကိန်းပြည့်	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		ကိန်းပြည့်	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		အကယ်၍ (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		အကယ်၍ (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    အကယ်၍ (STRNCMP(ptr, p, wlen) != 0)
		    {
			အတွက် (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (ကိန်းပြည့်)(p - mip->mi_fword);
		    }
		}
#အကယ်၍ 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		အကယ်၍ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		အတွက် (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    အကယ်၍ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			အကယ်၍ (mip->mi_lp->lp_slang->sl_fidxs == NULL
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    continue;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    အကယ်၍ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#အကယ်၍ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			အကယ်၍ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    အကယ်၍ (!slang->sl_nobreak)
			bha_nga;
		}
		--mip->mi_complen;
		အကယ်၍ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		အကယ်၍ (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ (mip->mi_result == SP_OK)
			bha_nga;
		    continue;
		}
	    }

	    အကယ်၍ (flags & WF_BANNED)
		res = SP_BANNED;
	    မဟုတ်ပါက အကယ်၍ (flags & WF_REGION)
	    {
		// Check region.
		အကယ်၍ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		မဟုတ်ပါက
		    res = SP_LOCAL;
	    }
	    မဟုတ်ပါက အကယ်၍ (flags & WF_RARE)
		res = SP_RARE;
	    မဟုတ်ပါက
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    အကယ်၍ (nobreak_result == SP_BAD)
	    {
		အကယ်၍ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		မဟုတ်ပါက အကယ်၍ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    မဟုတ်ပါက အကယ်၍ (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    မဟုတ်ပါက အကယ်၍ (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    အကယ်၍ (mip->mi_result == SP_OK)
		bha_nga;
	}

	အကယ်၍ (mip->mi_result == SP_OK)
	    bha_nga;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    ကိန်းပြည့်
match_checkcompoundpattern(
    char_u	*ptr,
    ကိန်းပြည့်		wlen,
    garray_T	*gap)  // &sl_comppat
{
    ကိန်းပြည့်		i;
    char_u	*p;
    ကိန်းပြည့်		len;

    အတွက် (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	အကယ်၍ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (ကိန်းပြည့်)STRLEN(p);
	    အကယ်၍ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ပြန်ပေး TRUE;
	}
    }
    ပြန်ပေး FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    ကိန်းပြည့်
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    ကိန်းပြည့်		i;
    char_u	*p;

    အကယ်၍ (slang->sl_compprog == NULL)
	ပြန်ပေး FALSE;
    အကယ်၍ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	အတွက် (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    မဟုတ်ပါက
	p = flags;
    အကယ်၍ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ပြန်ပေး FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    အကယ်၍ (slang->sl_compsylmax < MAXWLEN
		       && count_syllables(slang, word) > slang->sl_compsylmax)
	ပြန်ပေး (ကိန်းပြည့်)STRLEN(flags) < slang->sl_compmax;
    ပြန်ပေး TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    ကိန်းပြည့်
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		c;

    // loop over all the COMPOUNDRULE entries
    အတွက် (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	အတွက် (i = 0; ; ++i)
	{
	    c = compflags[i];
	    အကယ်၍ (c == NUL)
		// found a rule that matches for the flags we have so far
		ပြန်ပေး TRUE;
	    အကယ်၍ (*p == '/' || *p == NUL)
		bha_nga;  // end of rule, it's too short
	    အကယ်၍ (*p == '[')
	    {
		ကိန်းပြည့် match = FALSE;

		// compare against all the flags in []
		++p;
		နေစဉ် (*p != ']' && *p != NUL)
		    အကယ်၍ (*p++ == c)
			match = TRUE;
		အကယ်၍ (!match)
		    bha_nga;  // none matches
	    }
	    မဟုတ်ပါက အကယ်၍ (*p != c)
		bha_nga;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	အကယ်၍ (p == NULL)
	    bha_nga;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ပြန်ပေး FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    ကိန်းပြည့်
valid_word_prefix(
    ကိန်းပြည့်		totprefcnt,	// nr of prefix IDs
    ကိန်းပြည့်		arridx,		// idx in sl_pidxs[]
    ကိန်းပြည့်		flags,
    char_u	*word,
    slang_T	*slang,
    ကိန်းပြည့်		cond_req)	// only use prefixes with a condition
{
    ကိန်းပြည့်		prefcnt;
    ကိန်းပြည့်		pidx;
    regprog_T	**rp;
    ကိန်းပြည့်		prefid;

    prefid = (unsigned)flags >> 24;
    အတွက် (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	အကယ်၍ (prefid != (pidx & 0xff))
	    continue;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	အကယ်၍ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    continue;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	အကယ်၍ (*rp != NULL)
	{
	    အကယ်၍ (!vim_regexec_prog(rp, FALSE, word, 0))
		continue;
	}
	မဟုတ်ပါက အကယ်၍ (cond_req)
	    continue;

	// It's a match!  Return the WF_ flags.
	ပြန်ပေး pidx;
    }
    ပြန်ပေး 0;
}

/*
 * Check if the word at "mip->mi_word" has a matching prefix.
 * If it does, then check the following word.
 *
 * If "mode" is "FIND_COMPOUND" then do the same after another word, find a
 * prefix in a compound word.
 *
 * For a match mip->mi_result is updated.
 */
    static void
find_prefix(matchinf_T *mip, ကိန်းပြည့် mode)
{
    idx_T	arridx = 0;
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		wlen = 0;
    ကိန်းပြည့်		flen;
    ကိန်းပြည့်		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    အကယ်၍ (byts == NULL)
	ပြန်ပေး;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    အကယ်၍ (mode == FIND_COMPOUND)
    {
	// Skip over the previously found word(s).
	ptr += mip->mi_compoff;
	flen -= mip->mi_compoff;
    }
    idxs = slang->sl_pidxs;

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    အတွက် (;;)
    {
	အကယ်၍ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	အကယ်၍ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    နေစဉ် (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    အကယ်၍ (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    အကယ်၍ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    မဟုတ်ပါက
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    အကယ်၍ (len == 0)
		bha_nga;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	အကယ်၍ (ptr[wlen] == NUL)
	    bha_nga;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	နေစဉ် (lo < hi)
	{
	    m = (lo + hi) / 2;
	    အကယ်၍ (byts[m] > c)
		hi = m - 1;
	    မဟုတ်ပါက အကယ်၍ (byts[m] < c)
		lo = m + 1;
	    မဟုတ်ပါက
	    {
		lo = hi = m;
		bha_nga;
	    }
	}

	// Stop if there is no matching byte.
	အကယ်၍ (hi < lo || byts[lo] != c)
	    bha_nga;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;
    }
}

/*
 * Need to fold at least one more character.  Do until next non-word character
 * for efficiency.  Include the non-word character too.
 * Return the length of the folded chars in bytes.
 */
    static ကိန်းပြည့်
fold_more(matchinf_T *mip)
{
    ကိန်းပြည့်		flen;
    char_u	*p;

    p = mip->mi_fend;
    do
	MB_PTR_ADV(mip->mi_fend);
    နေစဉ် (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    အကယ်၍ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (void)spell_casefold(mip->mi_win, p, (ကိန်းပြည့်)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (ကိန်းပြည့်)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ပြန်ပေး flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    ကိန်းပြည့်
spell_valid_case(
    ကိန်းပြည့်	    wordflags,	    // flags for the checked word.
    ကိန်းပြည့်	    treeflags)	    // flags for the word in the spell tree
{
    ပြန်ပေး ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    ကိန်းပြည့်
spell_check_window(win_T *wp)
{
    ပြန်ပေး wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(var_na **)(wp->w_s->b_langp.ga_data) != NULL;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    static ကိန်းပြည့်
no_spell_checking(win_T *wp)
{
    အကယ်၍ (spell_check_window(wp))
	ပြန်ပေး FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ပြန်ပေး TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    ကိန်းပြည့်
spell_move_to(
    win_T	*wp,
    ကိန်းပြည့်		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    ကိန်းပြည့်		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    ကိန်းပြည့်		found_len = 0;
    char_u	*line;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    ကိန်းပြည့်		len;
#ifdef FEAT_SYN_HL
    ကိန်းပြည့်		has_syntax = syntax_present(wp);
#endif
    ကိန်းပြည့်		col;
    ကိန်းပြည့်		can_spell;
    char_u	*buf = NULL;
    ကိန်းပြည့်		buflen = 0;
    ကိန်းပြည့်		skip = 0;
    ကိန်းပြည့်		capcol = -1;
    ကိန်းပြည့်		found_one = FALSE;
    ကိန်းပြည့်		wrapped = FALSE;

    အကယ်၍ (no_spell_checking(wp))
	ပြန်ပေး 0;

    /*
     * Start looking for bad word at the start of the line, because we can't
     * start halfway a word, we don't know where it starts or ends.
     *
     * When searching backwards, we continue in the line to find the last
     * bad word (in the cursor line: before the cursor).
     *
     * We concatenate the start of the next line, so that wrapped words work
     * (e.g. "et<line-break>cetera").  Doesn't work when searching backwards
     * though...
     */
    lnum = wp->w_cursor.lnum;
    CLEAR_POS(&found_pos);

    နေစဉ် (!got_int)
    {
	ကိန်းပြည့် empty_line;

	line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	အကယ်၍ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    အကယ်၍ (buf == NULL)
		bha_nga;
	}

	// In first line check first word for Capital.
	အကယ်၍ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	အကယ်၍ (capcol == 0)
	    capcol = getwhitecols(line);
	မဟုတ်ပါက အကယ်၍ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(line);
	    အကယ်၍ (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_line = *skipwhite(line) == NUL;
	STRCPY(buf, line);
	အကယ်၍ (lnum < wp->w_buffer->b_ml.ml_line_count)
	    spell_cat_line(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	နေစဉ် (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    အကယ်၍ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		bha_nga;

	    // start of word
	    attr = HLF_COUNT;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    အကယ်၍ (attr != HLF_COUNT)
	    {
		// We found a bad word.  Check the attribute.
		အကယ်၍ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    အကယ်၍ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			အကယ်၍ (has_syntax)
			{
			    col = (ကိန်းပြည့်)(p - buf);
			    (void)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    အကယ်၍ (!can_spell)
				attr = HLF_COUNT;
			}
			မဟုတ်ပါက
#endif
			    can_spell = TRUE;

			အကယ်၍ (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (ကိန်းပြည့်)(p - buf);
			    found_pos.coladd = 0;
			    အကယ်၍ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				အကယ်၍ (attrp != NULL)
				    *attrp = attr;
				ပြန်ပေး len;
			    }
			    မဟုတ်ပါက အကယ်၍ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    မဟုတ်ပါက
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	အကယ်၍ (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    ပြန်ပေး found_len;
	}

	အကယ်၍ (curline)
	    bha_nga;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	အကယ်၍ (lnum == wp->w_cursor.lnum && wrapped)
	    bha_nga;

	// Advance to next line.
	အကယ်၍ (dir == BACKWARD)
	{
	    အကယ်၍ (lnum > 1)
		--lnum;
	    မဟုတ်ပါက အကယ်၍ (!p_ws)
		bha_nga;	    // at first line and 'nowrapscan'
	    မဟုတ်ပါက
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_line_count;
		wrapped = TRUE;
		အကယ်၍ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    capcol = -1;
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (lnum < wp->w_buffer->b_ml.ml_line_count)
		++lnum;
	    မဟုတ်ပါက အကယ်၍ (!p_ws)
		bha_nga;	    // at first line and 'nowrapscan'
	    မဟုတ်ပါက
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		အကယ်၍ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    အကယ်၍ (lnum == wp->w_cursor.lnum && !found_one)
		bha_nga;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    အကယ်၍ (attr == HLF_COUNT)
		skip = (ကိန်းပြည့်)(p - endp);
	    မဟုတ်ပါက
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    အကယ်၍ (empty_line)
		capcol = 0;
	}

	line_breakcheck();
    }

    vim_free(buf);
    ပြန်ပေး 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    void
spell_cat_line(char_u *buf, char_u *line, ကိန်းပြည့် maxlen)
{
    char_u	*p;
    ကိန်းပြည့်		n;

    p = skipwhite(line);
    နေစဉ် (vim_strchr((char_u *)"*#/\"\t", *p) != NULL)
	p = skipwhite(p + 1);

    အကယ်၍ (*p == NUL)
	ပြန်ပေး;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (ကိန်းပြည့်)(p - line) + 1;
    အကယ်၍ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
typedef sa_nracanaa spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    ကိန်းပြည့်	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    static void
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    ကိန်းပြည့်		r;
    spelload_T	sl;
    ကိန်းပြည့်		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = NULL;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    အတွက် (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((var_na *)fname_enc, parimaa_na(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#မဟုတ်ပါက
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	အကယ်၍ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((var_na *)fname_enc, parimaa_na(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#မဟုတ်ပါက
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    အကယ်၍ (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		continue;
	    bha_nga;
	}
	bha_nga;
    }

    အကယ်၍ (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#မဟုတ်ပါက
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    မဟုတ်ပါက အကယ်၍ (sl.sl_slang != NULL)
    {
	// At least one file was loaded, now load ALL the additions.
	STRCPY(fname_enc + STRLEN(fname_enc) - 3, "add.spl");
	do_in_runtimepath(fname_enc, DIP_ALL, spell_load_cb, &sl);
    }

    --curbuf->b_locked;
}

/*
 * Return the encoding used for spell checking: Use 'encoding', except that we
 * use "latin1" for "latin9".  And limit to 60 characters (just in case).
 */
    char_u *
spell_enc(void)
{

    အကယ်၍ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ပြန်ပေး p_enc;
    ပြန်ပေး (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    static void
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((var_na *)fname, MAXPATHL, SPL_FNAME_TMPL,
						  int_wordlist, spell_enc());
}

/*
 * Allocate a new slang_T for language "lang".  "lang" can be NULL.
 * Caller must fill "sl_next".
 */
    slang_T *
slang_alloc(char_u *lang)
{
    slang_T *lp;

    lp = ALLOC_CLEAR_ONE(slang_T);
    အကယ်၍ (lp != NULL)
    {
	အကယ်၍ (lang != NULL)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, parimaa_na(fromto_T), 10);
	ga_init2(&lp->sl_repsal, parimaa_na(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ပြန်ပေး lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    void
slang_free(slang_T *lp)
{
    vim_free(lp->sl_name);
    vim_free(lp->sl_fname);
    slang_clear(lp);
    vim_free(lp);
}


/*
 * Free the salitem_T entries in a "sl_sal" garray (the SN_SAL form) and
 * clear the garray.  Used by slang_clear() and when set_sofo() reuses
 * sl_sal for the SN_SOFO form.
 */
    void
free_sal_items(garray_T *gap)
{
    salitem_T	*smp;

    နေစဉ် (gap->ga_len > 0)
    {
	smp = &((salitem_T *)gap->ga_data)[--gap->ga_len];
	vim_free(smp->sm_lead);
	// Don't free sm_oneof and sm_rules, they point into sm_lead.
	vim_free(smp->sm_to);
	vim_free(smp->sm_lead_w);
	vim_free(smp->sm_oneof_w);
	vim_free(smp->sm_to_w);
    }
    ga_clear(gap);
}

/*
 * Clear an slang_T so that the file can be reloaded.
 */
    void
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    အတွက် (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	နေစဉ် (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    အကယ်၍ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	အကယ်၍ (gap->ga_data != NULL)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    အတွက် (i = 0; i < gap->ga_len; ++i)
		vim_free(((ကိန်းပြည့် **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    မဟုတ်ပါက
	free_sal_items(gap);

    အတွက် (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = NULL;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_items);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_KEY_OFF);
    hash_init(&lp->sl_wordcount);

    hash_clear_all(&lp->sl_map_hash, 0);

    // Clear info from .sug file.
    slang_clear_sug(lp);

    lp->sl_compmax = MAXWLEN;
    lp->sl_compminlen = 0;
    lp->sl_compsylmax = MAXWLEN;
    lp->sl_regions[0] = NUL;
}

/*
 * Clear the info from the .sug file in "lp".
 */
    void
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = NULL;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    static void
spell_load_cb(char_u *fname, void *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, NULL, FALSE);
    အကယ်၍ (slang == NULL)
	ပြန်ပေး;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    အကယ်၍ (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    မဟုတ်ပါက အကယ်၍ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    void
count_common_word(
    slang_T	*lp,
    char_u	*word,
    ကိန်းပြည့်		len,	    // word length, -1 for up to NUL
    ကိန်းပြည့်		count)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    အကယ်၍ (len == -1)
	p = word;
    မဟုတ်ပါက အကယ်၍ (len >= MAXWLEN)
	ပြန်ပေး;
    မဟုတ်ပါက
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    အကယ်၍ (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	အကယ်၍ (wc == NULL)
	    ပြန်ပေး;
	STRCPY(wc->wc_word, p);
	wc->wc_count = count;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    မဟုတ်ပါက
    {
	wc = HI2WC(hi);
	အကယ်၍ ((wc->wc_count += count) < (unsigned)count)	// check for overflow
	    wc->wc_count = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    ကိန်းပြည့်
byte_in_str(char_u *str, ကိန်းပြည့် n)
{
    char_u	*p;

    အတွက် (p = str; *p != NUL; ++p)
	အကယ်၍ (*p == n)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

#define SY_MAXLEN   30
typedef sa_nracanaa syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    ကိန်းပြည့်		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    ကိန်းပြည့်
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    ကိန်းပြည့်		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_items, parimaa_na(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    နေစဉ် (p != NULL)
    {
	*p++ = NUL;
	အကယ်၍ (*p == NUL)	    // trailing slash
	    bha_nga;
	s = p;
	p = vim_strchr(p, '/');
	အကယ်၍ (p == NULL)
	    l = (ကိန်းပြည့်)STRLEN(s);
	မဟုတ်ပါက
	    l = (ကိန်းပြည့်)(p - s);
	အကယ်၍ (l >= SY_MAXLEN)
	    ပြန်ပေး SP_FORMERROR;
	အကယ်၍ (ga_grow(&slang->sl_syl_items, 1) == FAIL)
	    ပြန်ပေး SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_items.ga_data)
					       + slang->sl_syl_items.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ပြန်ပေး OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    static ကိန်းပြည့်
count_syllables(slang_T *slang, char_u *word)
{
    ကိန်းပြည့်		cnt = 0;
    ကိန်းပြည့်		skip = FALSE;
    char_u	*p;
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		i;
    syl_item_T	*syl;
    ကိန်းပြည့်		c;

    အကယ်၍ (slang->sl_syllable == NULL)
	ပြန်ပေး 0;

    အတွက် (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	အကယ်၍ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    continue;
	}

	// Find longest match of syllable items.
	len = 0;
	အတွက် (i = 0; i < slang->sl_syl_items.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_items.ga_data) + i;
	    အကယ်၍ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	အကယ်၍ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	မဟုတ်ပါက
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    အကယ်၍ (vim_strchr(slang->sl_syllable, c) == NULL)
		skip = FALSE;	    // No, search for next syllable
	    မဟုတ်ပါက အကယ်၍ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ပြန်ပေး cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    var_na *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    ကိန်းပြည့်		filename;
    ကိန်းပြည့်		region_mask;
    slang_T	*slang;
    ကိန်းပြည့်		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    ကိန်းပြည့်		len;
    char_u	*p;
    ကိန်းပြည့်		round;
    char_u	*spf;
    char_u	*use_region = NULL;
    ကိန်းပြည့်		dont_use_region = FALSE;
    ကိန်းပြည့်		nobreak = FALSE;
    ကိန်းပြည့်		i, j;
    langp_T	*lp, *lp2;
    static ကိန်းပြည့်	recursive = FALSE;
    var_na	*ret_msg = NULL;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    အကယ်၍ (recursive)
	ပြန်ပေး NULL;
    recursive = TRUE;

    ga_init2(&ga, parimaa_na(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    အကယ်၍ (spl_copy == NULL)
	goto theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    အတွက် (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = NULL;

	အကယ်၍ (!valid_spelllang(lang))
	    continue;

	အကယ်၍ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    continue;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	အကယ်၍ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    အကယ်၍ (p != NULL && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    မဟုတ်ပါက
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		အကယ်၍ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    bha_nga;
	}
	မဟုတ်ပါက
	{
	    filename = FALSE;
	    အကယ်၍ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    မဟုတ်ပါက
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		အကယ်၍ (STRICMP(lang, slang->sl_name) == 0)
		    bha_nga;
	}

	အကယ်၍ (region != NULL)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    အကယ်၍ (use_region != NULL && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	အကယ်၍ (slang == NULL)
	{
	    အကယ်၍ (filename)
		(void)spell_load_file(lang, lang, NULL, FALSE);
	    မဟုတ်ပါက
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		အကယ်၍ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_msg = N_(e_spellfilemising_autocommand_deleted_buffer);
		    goto theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    အကယ်၍ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		အကယ်၍ (!filename && region != NULL)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    အကယ်၍ (c == REGION_ALL)
		    {
			အကယ်၍ (slang->sl_add)
			{
			    အကယ်၍ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			မဟုတ်ပါက
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    မဟုတ်ပါက
			region_mask = 1 << c;
		}

		အကယ်၍ (region_mask != 0)
		{
		    အကယ်၍ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_msg = e_out_of_memory;
			goto theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    အကယ်၍ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    အတွက် (round = 0; round == 0 || *spf != NUL; ++round)
    {
	အကယ်၍ (round == 0)
	{
	    // Internal wordlist, if there is one.
	    အကယ်၍ (int_wordlist == NULL)
		continue;
	    int_wordlist_spl(spf_name);
	}
	မဟုတ်ပါက
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    အတွက် (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		အကယ်၍ (p != NULL && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    bha_nga;
	    }
	    အကယ်၍ (c < ga.ga_len)
		continue;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    အကယ်၍ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		bha_nga;
	အကယ်၍ (slang == NULL)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    အကယ်၍ (round == 0)
		STRCPY(lang, "internal wordlist");
	    မဟုတ်ပါက
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		အကယ်၍ (p != NULL)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, NULL, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    အကယ်၍ (slang != NULL && nobreak)
		slang->sl_nobreak = TRUE;
	}
	အကယ်၍ (slang != NULL && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    အကယ်၍ (use_region != NULL && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		အကယ်၍ (c != REGION_ALL)
		    region_mask = 1 << c;
		မဟုတ်ပါက အကယ်၍ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    အကယ်၍ (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = NULL;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = NULL;
		LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		++ga.ga_len;
		use_midword(slang, wp);
	    }
	}
    }

    // Everything is fine, store the new b_langp value.
    ga_clear(&wp->w_s->b_langp);
    wp->w_s->b_langp = ga;

    // For each language figure out what language to use for sound folding and
    // REP items.  If the language doesn't support it itself use another one
    // with the same name.  E.g. for "en-math" use "en".
    အတွက် (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	အကယ်၍ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	မဟုတ်ပါက
	    // find first similar language that does sound folding
	    အတွက် (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		အကယ်၍ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    bha_nga;
		}
	    }

	// REP items
	အကယ်၍ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	မဟုတ်ပါက
	    // find first similar language that has REP items
	    အတွက် (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		အကယ်၍ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    bha_nga;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ပြန်ပေး ret_msg;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    static void
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    static void
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    အကယ်၍ (lp->sl_midword == NULL)	    // there aren't any
	ပြန်ပေး;

    အတွက် (p = lp->sl_midword; *p != NUL; )
	အကယ်၍ (has_mbyte)
	{
	    ကိန်းပြည့်	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    အကယ်၍ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    မဟုတ်ပါက အကယ်၍ (wp->w_s->b_spell_ismw_mb == NULL)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    မဟုတ်ပါက
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (ကိန်းပြည့်)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		အကယ်၍ (bp != NULL)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	မဟုတ်ပါက
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    static ကိန်းပြည့်
find_region(char_u *rp, char_u *region)
{
    ကိန်းပြည့်		i;

    အတွက် (i = 0; ; i += 2)
    {
	အကယ်၍ (rp[i] == NUL)
	    ပြန်ပေး REGION_ALL;
	အကယ်၍ (rp[i] == region[0] && rp[i + 1] == region[1])
	    bha_nga;
    }
    ပြန်ပေး i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    ကိန်းပြည့်
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		firstcap;
    ကိန်းပြည့်		allcap;
    ကိန်းပြည့်		past_second = FALSE;	// past second word char

    // find first letter
    အတွက် (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	အကယ်၍ (end == NULL ? *p == NUL : p >= end)
	    ပြန်ပေး 0;	    // only non-word characters, illegal word
    အကယ်၍ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    မဟုတ်ပါက
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    အတွက် ( ; end == NULL ? *p != NUL : p < end; MB_PTR_ADV(p))
	အကယ်၍ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    အကယ်၍ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		အကယ်၍ (past_second && allcap)
		    ပြန်ပေး WF_KEEPCAP;
		allcap = FALSE;
	    }
	    မဟုတ်ပါက အကယ်၍ (!allcap)
		// UlU -> KEEPCAP
		ပြန်ပေး WF_KEEPCAP;
	    past_second = TRUE;
	}

    အကယ်၍ (allcap)
	ပြန်ပေး WF_ALLCAP;
    အကယ်၍ (firstcap)
	ပြန်ပေး WF_ONECAP;
    ပြန်ပေး 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    void
spell_delete_wordlist(void)
{
    char_u	fname[MAXPATHL];

    အကယ်၍ (int_wordlist == NULL)
	ပြန်ပေး;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    void
spell_free_all(void)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    နေစဉ် (first_lang != NULL)
    {
	slang = first_lang;
	first_lang = slang->sl_next;
	slang_free(slang);
    }

    spell_delete_wordlist();

    VIM_CLEAR(repl_to);
    VIM_CLEAR(repl_from);
}

/*
 * Clear all spelling tables and reload them.
 * Used after 'encoding' is set and when ":mkspell" was used.
 */
    void
spell_reload(void)
{
    win_T	*wp;

    // Initialize the table for spell_iswordp().
    init_spell_chartab();

    // Unload all allocated memory.
    spell_free_all();

    // Go through all buffers and handle 'spelllang'.
    FOR_ALL_WINDOWS(wp)
    {
	// Only load the wordlists when 'spelllang' is set and there is a
	// window for this buffer in which 'spell' is set.
	အကယ်၍ (*wp->w_s->b_p_spl != NUL)
	{
		အကယ်၍ (wp->w_p_spell)
		{
		    (void)parse_spelllang(wp);
		    bha_nga;
		}
	}
    }
}

/*
 * Open a spell buffer.  This is a nameless buffer that is not in the buffer
 * list and only contains text lines.  Can use a swapfile to reduce memory
 * use.
 * Most other fields are invalid!  Esp. watch out for string options being
 * NULL and there is no undo info.
 * Returns NULL when out of memory.
 */
    buf_T *
open_spellbuf(void)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    အကယ်၍ (buf == NULL)
	ပြန်ပေး NULL;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_key = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ပြန်ပေး buf;
}

/*
 * Close the buffer used for spell info.
 */
    void
close_spellbuf(buf_T *buf)
{
    အကယ်၍ (buf == NULL)
	ပြန်ပေး;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    void
clear_spell_chartab(spelltab_T *sp)
{
    ကိန်းပြည့်		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    အတွက် (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    အတွက် (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    အတွက် (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    အတွက် (i = 'a'; i <= 'z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_upper[i] = i - 0x20;
    }
}

/*
 * Init the chartab used for spelling.  Only depends on 'encoding'.
 * Called once while starting up and when 'encoding' changes.
 * The default is to use isalpha(), but the spell file should define the word
 * characters to make it possible that 'encoding' differs from the current
 * locale.  For utf-8 we don't use isalpha() but our own functions.
 */
    void
init_spell_chartab(void)
{
    ကိန်းပြည့်	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    အကယ်၍ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	အတွက် (i = 128; i <= 255; ++i)
	    အကယ်၍ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    မဟုတ်ပါက အကယ်၍ (enc_utf8)
    {
	အတွက် (i = 128; i < 256; ++i)
	{
	    ကိန်းပြည့် f = utf_fold(i);
	    ကိန်းပြည့် u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    မဟုတ်ပါက
    {
	// Rough guess: use locale-dependent library functions.
	အတွက် (i = 128; i < 256; ++i)
	{
	    အကယ်၍ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    မဟုတ်ပါက အကယ်၍ (MB_ISLOWER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_upper[i] = MB_TOUPPER(i);
	    }
	}
    }
}


/*
 * Return TRUE if "p" points to a word character.
 * As a special case we see "midword" characters as word character when it is
 * followed by a word character.  This finds they'there but not 'they there'.
 * Thus this only works properly when past the first character of the word.
 */
    ကိန်းပြည့်
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    ကိန်းပြည့်		l;
    ကိန်းပြည့်		c;

    အကယ်၍ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	အကယ်၍ (l == 1)
	{
	    // be quick for ASCII
	    အကယ်၍ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	မဟုတ်ပါက
	{
	    c = mb_ptr2char(p);
	    အကယ်၍ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != NULL
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != NULL))
		s = p + l;
	}

	c = mb_ptr2char(s);
	အကယ်၍ (c > 255)
	    ပြန်ပေး spell_mb_isword_class(mb_get_class(s), wp);
	ပြန်ပေး spelltab.st_isw[c];
    }

    ပြန်ပေး spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    ကိန်းပြည့်
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    ကိန်းပြည့်		c;

    အကယ်၍ (has_mbyte)
    {
	c = mb_ptr2char(p);
	အကယ်၍ (c > 255)
	    ပြန်ပေး spell_mb_isword_class(mb_get_class(p), wp);
	ပြန်ပေး spelltab.st_isw[c];
    }
    ပြန်ပေး spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    static ကိန်းပြည့်
spell_mb_isword_class(ကိန်းပြည့် cl, win_T *wp)
{
    အကယ်၍ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ပြန်ပေး cl == 2 || cl == 0x2800;
    ပြန်ပေး cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    static ကိန်းပြည့်
spell_iswordp_w(ကိန်းပြည့် *p, win_T *wp)
{
    ကိန်းပြည့်		*s;

    အကယ်၍ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != NULL
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != NULL))
	s = p + 1;
    မဟုတ်ပါက
	s = p;

    အကယ်၍ (*s > 255)
    {
	အကယ်၍ (enc_utf8)
	    ပြန်ပေး spell_mb_isword_class(utf_class(*s), wp);
	အကယ်၍ (enc_dbcs)
	    ပြန်ပေး spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	ပြန်ပေး 0;
    }
    ပြန်ပေး spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    ကိန်းပြည့်
spell_casefold(
    win_T	*wp,
    char_u	*str,
    ကိန်းပြည့်		len,
    char_u	*buf,
    ကိန်းပြည့်		buflen)
{
    ကိန်းပြည့်		i;

    အကယ်၍ (len >= buflen)
    {
	buf[0] = NUL;
	ပြန်ပေး FAIL;		// result will not fit
    }

    အကယ်၍ (has_mbyte)
    {
	ကိန်းပြည့်	outi = 0;
	char_u	*p;
	ကိန်းပြည့်	c;

	// Fold one character at a time.
	အတွက် (p = str; p < str + len; )
	{
	    အကယ်၍ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ပြန်ပေး FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    အကယ်၍ (c == 0x03a3 || c == 0x03c2)
	    {
		အကယ်၍ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		မဟုတ်ပါက
		    c = 0x03c3;
	    }
	    မဟုတ်ပါက
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    မဟုတ်ပါက
    {
	// Be quick for non-multibyte encodings.
	အတွက် (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ပြန်ပေး OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    ကိန်းပြည့်
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    အကယ်၍ (wp->w_s->b_cap_prog == NULL)
	ပြန်ပေး FALSE;

    ကိန်းပြည့်		need_cap = FALSE;
    char_u	*line = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : NULL;
    char_u	*line_copy = NULL;
    colnr_T	endcol = 0;

    အကယ်၍ (col == 0 || getwhitecols(line) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	အကယ်၍ (lnum == 1)
	    need_cap = TRUE;
	မဟုတ်ပါက
	{
	    line = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    အကယ်၍ (*skipwhite(line) == NUL)
		need_cap = TRUE;
	    မဟုတ်ပါက
	    {
		// Append a space in place of the line break.
		line_copy = concat_str(line, (char_u *)" ");
		အကယ်၍ (line_copy == NULL)
		    ပြန်ပေး FALSE;

		line = line_copy;
		endcol = (colnr_T)STRLEN(line);
	    }
	}
    }
    မဟုတ်ပါက
	endcol = col;

    အကယ်၍ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = line + endcol;
	အတွက် (;;)
	{
	    MB_PTR_BACK(line, p);
	    အကယ်၍ (p == line || spell_iswordp_nmw(p, wp))
		bha_nga;
	    အကယ်၍ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == line + endcol)
	    {
		need_cap = TRUE;
		bha_nga;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(line_copy);

    ပြန်ပေး need_cap;
}


/*
 * ":spellrepall"
 */
    void
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*line;
    char_u	*p;
    ကိန်းပြည့်		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    အကယ်၍ (repl_from == NULL || repl_to == NULL)
    {
	emsg(_(e_no_previous_spell_replacement));
	ပြန်ပေး;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    အကယ်၍ (frompat == NULL)
	ပြန်ပေး;
    frompatlen = vim_snprintf((var_na *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    နေစဉ် (!got_int)
    {
	အကယ်၍ (do_search(NULL, '/', '/', frompat, frompatlen, 1L, SEARCH_KEEP, NULL) == 0
						   || u_save_cursor() == FAIL)
	    bha_nga;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	line = ml_get_curline();
	အကယ်၍ (addlen <= 0 || STRNCMP(line + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    အကယ်၍ (p == NULL)
		bha_nga;
	    mch_memmove(p, line, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, line + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#အကယ်၍ defined(FEAT_PROP_POPUP)
	    အကယ်၍ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (ကိန်းပြည့်)addlen, APC_SUBSTITUTE);
#endif

	    အကယ်၍ (curwin->w_cursor.lnum != prev_lnum)
	    {
		++sub_nlines;
		prev_lnum = curwin->w_cursor.lnum;
	    }
	    ++sub_nsubs;
	}
	curwin->w_cursor.col += (colnr_T)repl_to_len;
    }

    p_ws = save_ws;
    curwin->w_cursor = pos;
    vim_free(frompat);

    အကယ်၍ (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    မဟုတ်ပါက
	do_sub_msg(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    void
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    ကိန်းပြည့်		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		l;

    p = word;
    အကယ်၍ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    မဟုတ်ပါက
	c = *p++;
    အကယ်၍ (upper)
	c = SPELL_TOUPPER(c);
    မဟုတ်ပါက
	c = SPELL_TOFOLD(c);
    အကယ်၍ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    မဟုတ်ပါက
    {
	l = 1;
	wcopy[0] = c;
    }
    vim_strncpy(wcopy + l, p, MAXWLEN - l - 1);
}

/*
 * Make a copy of "word" with all the letters upper cased into
 * "wcopy[MAXWLEN]".  The result is NUL terminated.
 */
    void
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    ကိန်းပြည့်		c;

    d = wcopy;
    အတွက် (s = word; *s != NUL; )
    {
	အကယ်၍ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	မဟုတ်ပါက
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	အကယ်၍ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    အကယ်၍ (d - wcopy >= MAXWLEN - 1)
		bha_nga;
	    *d++ = c;
	}
	မဟုတ်ပါက
	    c = SPELL_TOUPPER(c);

	အကယ်၍ (has_mbyte)
	{
	    အကယ်၍ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		bha_nga;
	    d += mb_char2bytes(c, d);
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (d - wcopy >= MAXWLEN - 1)
		bha_nga;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    ကိန်းပြည့်
nofold_len(char_u *fword, ကိန်းပြည့် flen, char_u *word)
{
    char_u	*p;
    ကိန်းပြည့်		i = 0;

    အတွက် (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    အတွက် (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    ပြန်ပေး (ကိန်းပြည့်)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    void
make_case_word(char_u *fword, char_u *cword, ကိန်းပြည့် flags)
{
    အကယ်၍ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    မဟုတ်ပါက အကယ်၍ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    မဟုတ်ပါက
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    ကိန်းပြည့်		lpi;

    အကယ်၍ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	အတွက် (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    အကယ်၍ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		ပြန်ပေး vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ပြန်ပေး vim_strsave(word);
}
#endif

/*
 * Turn "inword" into its sound-a-like equivalent in "res[MAXWLEN]".
 *
 * There are many ways to turn a word into a sound-a-like representation.  The
 * oldest is Soundex (1918!).   A nice overview can be found in "Approximate
 * swedish name matching - survey and test of different algorithms" by Klas
 * Erikson.
 *
 * We support two methods:
 * 1. SOFOFROM/SOFOTO do a simple character mapping.
 * 2. SAL items define a more advanced sound-folding (and much slower).
 */
    void
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    ကိန်းပြည့်		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    အကယ်၍ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    မဟုတ်ပါက
    {
	// SAL items used.  Requires the word to be case-folded.
	အကယ်၍ (folded)
	    word = inword;
	မဟုတ်ပါက
	{
	    (void)spell_casefold(curwin,
				  inword, (ကိန်းပြည့်)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	အကယ်၍ (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	မဟုတ်ပါက
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    static void
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    ကိန်းပြည့်		ri = 0;
    ကိန်းပြည့်		c;

    အကယ်၍ (has_mbyte)
    {
	ကိန်းပြည့်	prevc = 0;
	ကိန်းပြည့်	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	အတွက် (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    အကယ်၍ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    မဟုတ်ပါက အကယ်၍ (c < 256)
		c = slang->sl_sal_first[c];
	    မဟုတ်ပါက
	    {
		ip = ((ကိန်းပြည့် **)slang->sl_sal.ga_data)[c & 0xff];
		အကယ်၍ (ip == NULL)		// empty list, can't match
		    c = NUL;
		မဟုတ်ပါက
		    အတွက် (;;)		// find "c" in the list
		    {
			အကယ်၍ (*ip == 0)	// not found
			{
			    c = NUL;
			    bha_nga;
			}
			အကယ်၍ (*ip == c)	// match!
			{
			    c = ip[1];
			    bha_nga;
			}
			ip += 2;
		    }
	    }

	    အကယ်၍ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		အကယ်၍ (ri + MB_MAXBYTES > MAXWLEN)
		    bha_nga;
		prevc = c;
	    }
	}
    }
    မဟုတ်ပါက
    {
	// The sl_sal_first[] table contains the translation.
	အတွက် (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    အကယ်၍ (VIM_ISWHITE(c))
		c = ' ';
	    မဟုတ်ပါက
		c = slang->sl_sal_first[c];
	    အကယ်၍ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    static void
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    ကိန်းပြည့်		i, j, z;
    ကိန်းပြည့်		reslen;
    ကိန်းပြည့်		n, k = 0;
    ကိန်းပြည့်		z0;
    ကိန်းပြည့်		k0;
    ကိန်းပြည့်		n0;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		pri;
    ကိန်းပြည့်		p0 = -333;
    ကိန်းပြည့်		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    အကယ်၍ (slang->sl_rem_accents)
    {
	t = word;
	နေစဉ် (*s != NUL)
	{
	    အကယ်၍ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    မဟုတ်ပါက
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    နေစဉ် ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	အကယ်၍ (n >= 0)
	{
	    // check all rules for the same letter
	    အတွက် (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		အကယ်၍ (k > 1)
		{
		    အကယ်၍ (word[i + 1] != s[1])
			continue;
		    အကယ်၍ (k > 2)
		    {
			အတွက် (j = 2; j < k; ++j)
			    အကယ်၍ (word[i + j] != s[j])
				bha_nga;
			အကယ်၍ (j < k)
			    continue;
		    }
		}

		အကယ်၍ ((pf = smp[n].sm_oneof) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    နေစဉ် (*pf != NUL && *pf != word[i + k])
			++pf;
		    အကယ်၍ (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		နေစဉ် (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		အကယ်၍ (*s == '<')
		    s++;
		အကယ်၍ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		အကယ်၍ (*s == '^' && *(s + 1) == '^')
		    s++;

		အကယ်၍ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(word[i - 1] == ' '
				      || spell_iswordp(word + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp(word + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp(word + i - 1, curwin)
			    && (!spell_iswordp(word + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = word[i + k - 1];
		    n0 = slang->sl_sal_first[c0];

		    အကယ်၍ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			အတွက် ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    အကယ်၍ (k0 > 1)
			    {
				အကယ်၍ (word[i + k] != s[1])
				    continue;
				အကယ်၍ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    အတွက် (j = 2; j < k0; ++j)
					အကယ်၍ (*pf++ != s[j])
					    bha_nga;
				    အကယ်၍ (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    အကယ်၍ ((pf = smp[n0].sm_oneof) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				နေစဉ် (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				အကယ်၍ (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    နေစဉ် (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    အကယ်၍ (*s == '<')
				s++;
			    အကယ်၍ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    အကယ်၍ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				အကယ်၍ (k0 == k)
				    // this is just a piece of the string
				    continue;

				အကယ်၍ (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				bha_nga;
			    }
			}

			အကယ်၍ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    continue;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    အကယ်၍ (s == NULL)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != NULL) ? 1 : 0;
		    အကယ်၍ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			အကယ်၍ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			နေစဉ် (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			အကယ်၍ (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    မဟုတ်ပါက
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			နေစဉ် (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    အကယ်၍ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			အကယ်၍ (strstr((var_na *)pf, "^^") != NULL)
			{
			    အကယ်၍ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    bha_nga;
		}
	    }
	}
	မဟုတ်ပါက အကယ်၍ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	အကယ်၍ (z0 == 0)
	{
	    အကယ်၍ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
		    && (!slang->sl_collapse || reslen == 0
						     || res[reslen - 1] != c))
		// condense only double letters
		res[reslen++] = c;

	    i++;
	    z = 0;
	    k = 0;
	}
    }

    res[reslen] = NUL;
}

/*
 * Turn "inword" into its sound-a-like equivalent in "res[MAXWLEN]".
 * Multi-byte version of spell_soundfold().
 */
    static void
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    ကိန်းပြည့်		word[MAXWLEN];
    ကိန်းပြည့်		wres[MAXWLEN];
    ကိန်းပြည့်		l;
    char_u	*s;
    ကိန်းပြည့်		*ws;
    char_u	*t;
    ကိန်းပြည့်		*pf;
    ကိန်းပြည့်		i, j, z;
    ကိန်းပြည့်		reslen;
    ကိန်းပြည့်		n, k = 0;
    ကိန်းပြည့်		z0;
    ကိန်းပြည့်		k0;
    ကိန်းပြည့်		n0;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		pri;
    ကိန်းပြည့်		p0 = -333;
    ကိန်းပြည့်		c0;
    ကိန်းပြည့်		did_white = FALSE;
    ကိန်းပြည့်		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    အတွက် (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	အကယ်၍ (slang->sl_rem_accents)
	{
	    အကယ်၍ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		အကယ်၍ (did_white)
		    continue;
		c = ' ';
		did_white = TRUE;
	    }
	    မဟုတ်ပါက
	    {
		did_white = FALSE;
		အကယ်၍ (!spell_iswordp_nmw(t, curwin))
		    continue;
	    }
	}
	word[wordlen++] = c;
    }
    word[wordlen] = NUL;

    /*
     * This algorithm comes from Aspell phonet.cpp.
     * Converted from C++ to C.  Added support for multi-byte chars.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    နေစဉ် ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	အကယ်၍ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    အတွက် (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		အကယ်၍ (c != ws[0])
		    continue;
		k = smp[n].sm_leadlen;
		အကယ်၍ (k > 1)
		{
		    အကယ်၍ (word[i + 1] != ws[1])
			continue;
		    အကယ်၍ (k > 2)
		    {
			အတွက် (j = 2; j < k; ++j)
			    အကယ်၍ (word[i + j] != ws[j])
				bha_nga;
			အကယ်၍ (j < k)
			    continue;
		    }
		}

		အကယ်၍ ((pf = smp[n].sm_oneof_w) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    နေစဉ် (*pf != NUL && *pf != word[i + k])
			++pf;
		    အကယ်၍ (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		နေစဉ် (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		အကယ်၍ (*s == '<')
		    s++;
		အကယ်၍ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		အကယ်၍ (*s == '^' && *(s + 1) == '^')
		    s++;

		အကယ်၍ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(word[i - 1] == ' '
				    || spell_iswordp_w(word + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp_w(word + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp_w(word + i - 1, curwin)
			    && (!spell_iswordp_w(word + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = word[i + k - 1];
		    n0 = slang->sl_sal_first[c0 & 0xff];

		    အကယ်၍ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			အတွက် ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    အကယ်၍ (c0 != ws[0])
				continue;
			    k0 = smp[n0].sm_leadlen;
			    အကယ်၍ (k0 > 1)
			    {
				အကယ်၍ (word[i + k] != ws[1])
				    continue;
				အကယ်၍ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    အတွက် (j = 2; j < k0; ++j)
					အကယ်၍ (*pf++ != ws[j])
					    bha_nga;
				    အကယ်၍ (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    အကယ်၍ ((pf = smp[n0].sm_oneof_w) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				နေစဉ် (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				အကယ်၍ (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    နေစဉ် (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    အကယ်၍ (*s == '<')
				s++;
			    အကယ်၍ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    အကယ်၍ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				အကယ်၍ (k0 == k)
				    // this is just a piece of the string
				    continue;

				အကယ်၍ (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				bha_nga;
			    }
			}

			အကယ်၍ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    continue;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != NULL) ? 1 : 0;
		    အကယ်၍ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			အကယ်၍ (reslen > 0 && ws != NULL && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			အကယ်၍ (ws != NULL)
			    နေစဉ် (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			အကယ်၍ (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    parimaa_na(ကိန်းပြည့်) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    မဟုတ်ပါက
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			အကယ်၍ (ws != NULL)
			    နေစဉ် (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				အကယ်၍ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			အကယ်၍ (ws == NULL)
			    c = NUL;
			မဟုတ်ပါက
			    c = *ws;
			အကယ်၍ (strstr((var_na *)s, "^^") != NULL)
			{
			    အကယ်၍ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       parimaa_na(ကိန်းပြည့်) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    bha_nga;
		}
	    }
	}
	မဟုတ်ပါက အကယ်၍ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	အကယ်၍ (z0 == 0)
	{
	    အကယ်၍ (k && !p0 && reslen < MAXWLEN && c != NUL
		    && (!slang->sl_collapse || reslen == 0
						     || wres[reslen - 1] != c))
		// condense only double letters
		wres[reslen++] = c;

	    i++;
	    z = 0;
	    k = 0;
	}
    }

    // Convert wide characters in "wres" to a multi-byte string in "res".
    l = 0;
    အတွက် (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	အကယ်၍ (l + MB_MAXBYTES > MAXWLEN)
	    bha_nga;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    void
ex_spellinfo(exarg_T *eap UNUSED)
{
    ကိန်းပြည့်		lpi;
    langp_T	*lp;
    char_u	*p;

    အကယ်၍ (no_spell_checking(curwin))
	ပြန်ပေး;

    msg_start();
    အတွက် (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	msg_puts("file: ");
	msg_puts((var_na *)lp->lp_slang->sl_fname);
	msg_putchar('\n');
	p = lp->lp_slang->sl_info;
	အကယ်၍ (p != NULL)
	{
	    msg_puts((var_na *)p);
	    msg_putchar('\n');
	}
    }
    msg_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_COUNT	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    void
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    အကယ်၍ (no_spell_checking(curwin))
	ပြန်ပေး;
    (void)get_option_value((char_u*)"spl", &dummy, &spl, NULL, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_value_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_value_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    အကယ်၍ (!BUFEMPTY())
	ပြန်ပေး;

    spell_dump_compl(NULL, 0, NULL, eap->forceit ? DUMPFLAG_COUNT : 0);

    // Delete the empty line that we started with.
    အကယ်၍ (curbuf->b_ml.ml_line_count > 1)
	ml_delete(curbuf->b_ml.ml_line_count);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    void
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    ကိန်းပြည့်		ic,	    // ignore case
    ကိန်းပြည့်		*dir,	    // direction for adding matches
    ကိန်းပြည့်		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    ကိန်းပြည့်		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    ကိန်းပြည့်		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    ကိန်းပြည့်		round;
    ကိန်းပြည့်		depth;
    ကိန်းပြည့်		n;
    ကိန်းပြည့်		flags;
    char_u	*region_names = NULL;	    // region names being used
    ကိန်းပြည့်		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    ကိန်းပြည့်		lpi;
    ကိန်းပြည့်		dumpflags = dumpflags_arg;
    ကိန်းပြည့်		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    အကယ်၍ (pat != NULL)
    {
	အကယ်၍ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	မဟုတ်ပါက
	{
	    n = captype(pat, NULL);
	    အကယ်၍ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    မဟုတ်ပါက အကယ်၍ (n == WF_ALLCAP && (ကိန်းပြည့်)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    အတွက် (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	အကယ်၍ (p[0] != 0)
	{
	    အကယ်၍ (region_names == NULL)	    // first language with regions
		region_names = p;
	    မဟုတ်ပါက အကယ်၍ (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		bha_nga;
	    }
	}
    }

    အကယ်၍ (do_region && region_names != NULL && pat == NULL)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    မဟုတ်ပါက
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    အတွက် (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	အကယ်၍ (slang->sl_fbyts == NULL)	    // reloading failed
	    continue;

	အကယ်၍ (pat == NULL)
	{
	    vim_snprintf((var_na *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	အကယ်၍ (pat != NULL && slang->sl_pbyts == NULL)
	    patlen = (ကိန်းပြည့်)STRLEN(pat);
	မဟုတ်ပါက
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	အတွက် (round = 1; round <= 2; ++round)
	{
	    အကယ်၍ (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    မဟုတ်ပါက
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    အကယ်၍ (byts == NULL)
		continue;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    နေစဉ် (depth >= 0 && !got_int
				  && (pat == NULL || !ins_compl_interrupted()))
	    {
		အကယ်၍ (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    line_breakcheck();
		    ins_compl_check_keys(50, FALSE);
		}
		မဟုတ်ပါက
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    အကယ်၍ (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (ကိန်းပြည့်)idxs[n];
			အကယ်၍ ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    အကယ်၍ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    အကယ်၍ (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				အကယ်၍ (pat == NULL)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    အကယ်၍ (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    မဟုတ်ပါက
		    {
			// Normal char, go one level deeper.
			word[depth++] = c;
			arridx[depth] = idxs[n];
			curi[depth] = 1;

			// Check if this character matches with the pattern.
			// If not skip the whole tree below it.
			// Always ignore case here, dump_word() will check
			// proper case later.  This isn't exactly right when
			// length changes for multi-byte characters with
			// ignore case...
			အကယ်၍ (depth <= patlen
					&& MB_STRNICMP(word, pat, depth) != 0)
			    --depth;
		    }
		}
	    }
	}
    }
}

/*
 * Dump one word: apply case modifications and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 */
    static void
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    ကိန်းပြည့်		*dir,
    ကိန်းပြည့်		dumpflags,
    ကိန်းပြည့်		wordflags,
    linenr_T	lnum)
{
    ကိန်းပြည့်		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		flags = wordflags;

    အကယ်၍ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    အကယ်၍ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    အကယ်၍ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    မဟုတ်ပါက
    {
	p = word;
	အကယ်၍ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, NULL) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    အကယ်၍ (pat == NULL)
    {
	// Add flags and regions after a slash.
	အကယ်၍ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    အကယ်၍ (keepcap)
		STRCAT(badword, "=");
	    အကယ်၍ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    မဟုတ်ပါက အကယ်၍ (flags & WF_RARE)
		STRCAT(badword, "?");
	    အကယ်၍ (flags & WF_REGION)
		အတွက် (i = 0; i < 7; ++i)
		    အကယ်၍ (flags & (0x10000 << i))
			sprintf((var_na *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	အကယ်၍ (dumpflags & DUMPFLAG_COUNT)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    အကယ်၍ (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((var_na *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_count);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    မဟုတ်ပါက အကယ်၍ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (ကိန်းပြည့်)STRLEN(p),
					  p_ic, NULL, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    static linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    ကိန်းပြည့်		*dir,
    ကိန်းပြည့်		dumpflags,
    ကိန်းပြည့်		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    ကိန်းပြည့်		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    ကိန်းပြည့်		has_word_up = FALSE;
    ကိန်းပြည့်		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    ကိန်းပြည့်		depth;
    ကိန်းပြည့်		n;
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    အကယ်၍ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    အကယ်၍ (byts != NULL)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	နေစဉ် (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    အကယ်၍ (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		line_breakcheck();
	    }
	    မဟုတ်ပါက
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		အကယ်၍ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    အတွက် (i = 1; i < len; ++i)
			အကယ်၍ (byts[n + i] != 0)
			    bha_nga;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    အကယ်၍ (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			အကယ်၍ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    အကယ်၍ (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			အကယ်၍ (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    အကယ်၍ (lnum != 0)
				++lnum;
			}
		    }
		}
		မဟုတ်ပါက အကယ်၍ (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    ပြန်ပေး lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    နေစဉ် (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ပြန်ပေး p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    ကိန်းပြည့်
spell_word_start(ကိန်းပြည့် startcol)
{
    char_u	*line;
    char_u	*p;
    ကိန်းပြည့်		col = 0;

    အကယ်၍ (no_spell_checking(curwin))
	ပြန်ပေး startcol;

    // Find a word character before "startcol".
    line = ml_get_curline();
    အတွက် (p = line + startcol; p > line; )
    {
	MB_PTR_BACK(line, p);
	အကယ်၍ (spell_iswordp_nmw(p, curwin))
	    bha_nga;
    }

    // Go back to start of the word.
    နေစဉ် (p > line)
    {
	col = (ကိန်းပြည့်)(p - line);
	MB_PTR_BACK(line, p);
	အကယ်၍ (!spell_iswordp(p, curwin))
	    bha_nga;
	col = 0;
    }

    ပြန်ပေး col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
static ကိန်းပြည့် spell_expand_need_cap;

    void
spell_expand_check_cap(colnr_T col)
{
    spell_expand_need_cap = check_need_cap(curwin, curwin->w_cursor.lnum, col);
}

/*
 * Get list of spelling suggestions.
 * Used for Insert mode completion CTRL-X ?.
 * Returns the number of matches.  The matches are in "matchp[]", array of
 * allocated strings.
 */
    ကိန်းပြည့်
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    ပြန်ပေး ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    ကိန်းပြည့်
valid_spelllang(char_u *val)
{
    ပြန်ပေး valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    ကိန်းပြည့်
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    ကိန်းပြည့်		l;

    spf = val;
    နေစဉ် (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	အကယ်၍ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ပြန်ပေး FALSE;
	အတွက် (s = spf_name; *s != NUL; ++s)
	    အကယ်၍ (!vim_is_fname_char(*s))
		ပြန်ပေး FALSE;
    }
    ပြန်ပေး TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    var_na *
did_set_spell_option(void)
{
    var_na    *errmsg = NULL;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	အကယ်၍ (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    bha_nga;
	}
    ပြန်ပေး errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    var_na *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    အကယ်၍ (synblock->b_p_spc == NULL || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = NULL;
    မဟုတ်ပါက
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	အကယ်၍ (re != NULL)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    အကယ်၍ (synblock->b_cap_prog == NULL)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ပြန်ပေး e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    ပြန်ပေး NULL;
}

#endif  // FEAT_SPELL
