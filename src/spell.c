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

#ᮚᮦᮔ᮪ defined(FEAT_SPELL)

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
    puur_naa_nka		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    puur_naa_nka		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    puur_naa_nka		mi_prefcnt;		// number of entries at mi_prefarridx
    puur_naa_nka		mi_prefixlen;		// byte length of prefix
    puur_naa_nka		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    puur_naa_nka		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    puur_naa_nka		mi_complen;		// nr of compound words used
    puur_naa_nka		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    puur_naa_nka		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    puur_naa_nka		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    puur_naa_nka		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


static puur_naa_nka spell_mb_isword_class(puur_naa_nka cl, win_T *wp);

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

static void find_word(matchinf_T *mip, puur_naa_nka mode);
static void find_prefix(matchinf_T *mip, puur_naa_nka mode);
static puur_naa_nka fold_more(matchinf_T *mip);
static void spell_load_cb(char_u *fname, void *cookie);
static puur_naa_nka count_syllables(slang_T *slang, char_u *word);
static void clear_midword(win_T *buf);
static void use_midword(slang_T *lp, win_T *buf);
static puur_naa_nka find_region(char_u *rp, char_u *region);
static void spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
static void spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
static void spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
static void dump_word(slang_T *slang, char_u *word, char_u *pat, puur_naa_nka *dir, puur_naa_nka round, puur_naa_nka flags, linenr_T lnum);
static linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, puur_naa_nka *dir, puur_naa_nka round, puur_naa_nka flags, linenr_T startlnum);
static char_u *advance_camelcase_word(char_u *p, win_T *wp, puur_naa_nka *is_camel_case);

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
    puur_naa_nka
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    puur_naa_nka		*capcol,	// column to check for Capital
    puur_naa_nka		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    puur_naa_nka		nrlen = 0;	// found a number first
    puur_naa_nka		c;
    puur_naa_nka		wrongcaplen = 0;
    puur_naa_nka		lpi;
    puur_naa_nka		count_word = docount;
    puur_naa_nka		use_camel_case = *wp->w_s->b_p_spo != NUL;
    puur_naa_nka		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ᮚᮦᮔ᮪ (*ptr <= ' ')
	ᮝᮜᮤ 1;

    // Return here when loading language files failed.
    ᮚᮦᮔ᮪ (wp->w_s->b_langp.ga_len == 0)
	ᮝᮜᮤ 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ᮚᮦᮔ᮪ (*ptr >= '0' && *ptr <= '9')
    {
	ᮚᮦᮔ᮪ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	ᮜᮤᮚ
	    mi.mi_end = skipdigits(ptr);
	nrlen = (puur_naa_nka)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    ᮚᮦᮔ᮪ (spell_iswordp(mi.mi_fend, wp))
    {
	ᮚᮦᮔ᮪ (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	ᮜᮤᮚ
	{
	    do
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ᮞᮨᮓᮨᮜ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ᮚᮦᮔ᮪ (capcol != NULL && *capcol == 0 && wp->w_s->b_cap_prog != NULL)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ᮚᮦᮔ᮪ (!SPELL_ISUPPER(c))
		wrongcaplen = (puur_naa_nka)(mi.mi_fend - ptr);
	}
    }
    ᮚᮦᮔ᮪ (capcol != NULL)
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
    ᮚᮦᮔ᮪ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (void)spell_casefold(wp, ptr, (puur_naa_nka)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (puur_naa_nka)STRLEN(mi.mi_fword);

    ᮚᮦᮔ᮪ (is_camel_case && mi.mi_fwordlen > 0)
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
    for (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ᮚᮦᮔ᮪ (mi.mi_lp->lp_slang->sl_fidxs == NULL)
	    continue;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ᮚᮦᮔ᮪ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ᮚᮦᮔ᮪ (count_word && mi.mi_result == SP_OK)
	{
	    count_common_word(mi.mi_lp->lp_slang, ptr,
						   (puur_naa_nka)(mi.mi_end - ptr), 1);
	    count_word = FALSE;
	}
    }

    ᮚᮦᮔ᮪ (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ᮚᮦᮔ᮪ (nrlen > 0)
	{
	    ᮚᮦᮔ᮪ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		ᮝᮜᮤ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	ᮜᮤᮚ ᮚᮦᮔ᮪ (!spell_iswordp_nmw(ptr, wp))
	{
	    ᮚᮦᮔ᮪ (capcol != NULL && wp->w_s->b_cap_prog != NULL)
	    {
		regmatch_T	regmatch;
		puur_naa_nka		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ᮚᮦᮔ᮪ (r)
		    *capcol = (puur_naa_nka)(regmatch.endp[0] - ptr);
	    }

	    ᮚᮦᮔ᮪ (has_mbyte)
		ᮝᮜᮤ (*mb_ptr2len)(ptr);
	    ᮝᮜᮤ 1;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    puur_naa_nka		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    ᮚᮦᮔ᮪ (mi.mi_lp->lp_slang->sl_fidxs != NULL)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		for (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ᮚᮦᮔ᮪ (p >= mi.mi_end)
			bha_nga;
		    mi.mi_compoff = (puur_naa_nka)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    ᮚᮦᮔ᮪ (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			bha_nga;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	ᮚᮦᮔ᮪ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	ᮜᮤᮚ
	    *attrp = HLF_SPL;
    }

    ᮚᮦᮔ᮪ (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ᮝᮜᮤ wrongcaplen;
    }

    ᮝᮜᮤ (puur_naa_nka)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    static puur_naa_nka
get_char_type(puur_naa_nka c)
{
    ᮚᮦᮔ᮪ (VIM_ISDIGIT(c))
	ᮝᮜᮤ CHAR_DIGIT;
    ᮚᮦᮔ᮪ (SPELL_ISUPPER(c))
	ᮝᮜᮤ CHAR_UPPER;
    ᮝᮜᮤ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    static char_u *
advance_camelcase_word(char_u *str, win_T *wp, puur_naa_nka *is_camel_case)
{
    puur_naa_nka last_type, last_last_type, this_type;
    puur_naa_nka c;
    char_u *end = str;

    *is_camel_case = FALSE;

    ᮚᮦᮔ᮪ (*str == NUL)
	ᮝᮜᮤ str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    ᮞᮨᮓᮨᮜ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	ᮚᮦᮔ᮪ (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    bha_nga;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
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

    ᮝᮜᮤ end;
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
find_word(matchinf_T *mip, puur_naa_nka mode)
{
    idx_T	arridx = 0;
    puur_naa_nka		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    puur_naa_nka		endidxcnt = 0;
    puur_naa_nka		len;
    puur_naa_nka		wlen = 0;
    puur_naa_nka		flen;
    puur_naa_nka		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    puur_naa_nka		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    puur_naa_nka		word_ends;
    puur_naa_nka		prefix_found;
    puur_naa_nka		nobreak_result;

    ᮚᮦᮔ᮪ (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ᮚᮦᮔ᮪ (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    ᮜᮤᮚ
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ᮚᮦᮔ᮪ (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ᮚᮦᮔ᮪ (byts == NULL)
	ᮝᮜᮤ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    for (;;)
    {
	ᮚᮦᮔ᮪ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ᮚᮦᮔ᮪ (byts[arridx] == 0)
	{
	    ᮚᮦᮔ᮪ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		ᮝᮜᮤ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ᮞᮨᮓᮨᮜ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ᮚᮦᮔ᮪ (len == 0)
		bha_nga;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ᮚᮦᮔ᮪ (ptr[wlen] == NUL)
	    bha_nga;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ᮚᮦᮔ᮪ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ᮞᮨᮓᮨᮜ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ᮚᮦᮔ᮪ (byts[m] > c)
		hi = m - 1;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (byts[m] < c)
		lo = m + 1;
	    ᮜᮤᮚ
	    {
		lo = hi = m;
		bha_nga;
	    }
	}

	// Stop if there is no matching byte.
	ᮚᮦᮔ᮪ (hi < lo || byts[lo] != c)
	    bha_nga;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ᮚᮦᮔ᮪ (c == ' ')
	{
	    for (;;)
	    {
		ᮚᮦᮔ᮪ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		ᮚᮦᮔ᮪ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
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
    ᮞᮨᮓᮨᮜ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ᮚᮦᮔ᮪ ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    continue;	    // not at first byte of character
	ᮚᮦᮔ᮪ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ᮚᮦᮔ᮪ (slang->sl_compprog == NULL && !slang->sl_nobreak)
		continue;	    // next char is a word character
	    word_ends = FALSE;
	}
	ᮜᮤᮚ
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	ᮚᮦᮔ᮪ (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    ᮚᮦᮔ᮪ (STRNCMP(ptr, p, wlen) != 0)
	    {
		for (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (puur_naa_nka)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	for (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ᮚᮦᮔ᮪ (mode == FIND_FOLDWORD)
	    {
		ᮚᮦᮔ᮪ (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		ᮚᮦᮔ᮪ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    continue;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		ᮚᮦᮔ᮪ (c == 0)
		    continue;

		// Use the WF_RARE flag for a rare prefix.
		ᮚᮦᮔ᮪ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    ᮚᮦᮔ᮪ (slang->sl_nobreak)
	    {
		ᮚᮦᮔ᮪ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    bha_nga;
		}
	    }

	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ᮚᮦᮔ᮪ (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    continue;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ᮚᮦᮔ᮪ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			continue;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ᮚᮦᮔ᮪ (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    continue;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ᮚᮦᮔ᮪ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    continue;
		ᮚᮦᮔ᮪ (!word_ends && (flags & WF_NOCOMPAFT))
		    continue;

		// Quickly check if compounding is possible with this flag.
		ᮚᮦᮔ᮪ (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    continue;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ᮚᮦᮔ᮪ (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    continue;

		ᮚᮦᮔ᮪ (mode == FIND_COMPOUND)
		{
		    puur_naa_nka	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ᮚᮦᮔ᮪ (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			for (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    ᮜᮤᮚ
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    ᮚᮦᮔ᮪ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			continue;

		    ᮚᮦᮔ᮪ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			ᮚᮦᮔ᮪ (spell_iswordp_nmw(p, mip->mi_win)
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
		ᮚᮦᮔ᮪ (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    ᮚᮦᮔ᮪ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ᮚᮦᮔ᮪ (ptr == mip->mi_word)
			    (void)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			ᮜᮤᮚ
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ᮚᮦᮔ᮪ (!can_compound(slang, fword, mip->mi_compflags))
			continue;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (slang->sl_comprules != NULL
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    continue;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (flags & WF_NEEDCOMP)
		continue;

	    nobreak_result = SP_OK;

	    ᮚᮦᮔ᮪ (!word_ends)
	    {
		puur_naa_nka	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		puur_naa_nka	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ᮚᮦᮔ᮪ (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ᮚᮦᮔ᮪ (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ᮚᮦᮔ᮪ (STRNCMP(ptr, p, wlen) != 0)
		    {
			for (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (puur_naa_nka)(p - mip->mi_fword);
		    }
		}
#ᮚᮦᮔ᮪ 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		ᮚᮦᮔ᮪ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		for (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ᮚᮦᮔ᮪ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			ᮚᮦᮔ᮪ (mip->mi_lp->lp_slang->sl_fidxs == NULL
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    continue;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ᮚᮦᮔ᮪ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#ᮚᮦᮔ᮪ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ᮚᮦᮔ᮪ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    ᮚᮦᮔ᮪ (!slang->sl_nobreak)
			bha_nga;
		}
		--mip->mi_complen;
		ᮚᮦᮔ᮪ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ᮚᮦᮔ᮪ (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		ᮜᮤᮚ
		{
		    ᮚᮦᮔ᮪ (mip->mi_result == SP_OK)
			bha_nga;
		    continue;
		}
	    }

	    ᮚᮦᮔ᮪ (flags & WF_BANNED)
		res = SP_BANNED;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (flags & WF_REGION)
	    {
		// Check region.
		ᮚᮦᮔ᮪ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		ᮜᮤᮚ
		    res = SP_LOCAL;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (flags & WF_RARE)
		res = SP_RARE;
	    ᮜᮤᮚ
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ᮚᮦᮔ᮪ (nobreak_result == SP_BAD)
	    {
		ᮚᮦᮔ᮪ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    ᮚᮦᮔ᮪ (mip->mi_result == SP_OK)
		bha_nga;
	}

	ᮚᮦᮔ᮪ (mip->mi_result == SP_OK)
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
    puur_naa_nka
match_checkcompoundpattern(
    char_u	*ptr,
    puur_naa_nka		wlen,
    garray_T	*gap)  // &sl_comppat
{
    puur_naa_nka		i;
    char_u	*p;
    puur_naa_nka		len;

    for (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	ᮚᮦᮔ᮪ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (puur_naa_nka)STRLEN(p);
	    ᮚᮦᮔ᮪ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ᮝᮜᮤ TRUE;
	}
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    puur_naa_nka
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    puur_naa_nka		i;
    char_u	*p;

    ᮚᮦᮔ᮪ (slang->sl_compprog == NULL)
	ᮝᮜᮤ FALSE;
    ᮚᮦᮔ᮪ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	for (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    ᮜᮤᮚ
	p = flags;
    ᮚᮦᮔ᮪ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ᮝᮜᮤ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ᮚᮦᮔ᮪ (slang->sl_compsylmax < MAXWLEN
		       && count_syllables(slang, word) > slang->sl_compsylmax)
	ᮝᮜᮤ (puur_naa_nka)STRLEN(flags) < slang->sl_compmax;
    ᮝᮜᮤ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    puur_naa_nka
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    puur_naa_nka		i;
    puur_naa_nka		c;

    // loop over all the COMPOUNDRULE entries
    for (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	for (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ᮚᮦᮔ᮪ (c == NUL)
		// found a rule that matches for the flags we have so far
		ᮝᮜᮤ TRUE;
	    ᮚᮦᮔ᮪ (*p == '/' || *p == NUL)
		bha_nga;  // end of rule, it's too short
	    ᮚᮦᮔ᮪ (*p == '[')
	    {
		puur_naa_nka match = FALSE;

		// compare against all the flags in []
		++p;
		ᮞᮨᮓᮨᮜ (*p != ']' && *p != NUL)
		    ᮚᮦᮔ᮪ (*p++ == c)
			match = TRUE;
		ᮚᮦᮔ᮪ (!match)
		    bha_nga;  // none matches
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (*p != c)
		bha_nga;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ᮚᮦᮔ᮪ (p == NULL)
	    bha_nga;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ᮝᮜᮤ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    puur_naa_nka
valid_word_prefix(
    puur_naa_nka		totprefcnt,	// nr of prefix IDs
    puur_naa_nka		arridx,		// idx in sl_pidxs[]
    puur_naa_nka		flags,
    char_u	*word,
    slang_T	*slang,
    puur_naa_nka		cond_req)	// only use prefixes with a condition
{
    puur_naa_nka		prefcnt;
    puur_naa_nka		pidx;
    regprog_T	**rp;
    puur_naa_nka		prefid;

    prefid = (unsigned)flags >> 24;
    for (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ᮚᮦᮔ᮪ (prefid != (pidx & 0xff))
	    continue;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ᮚᮦᮔ᮪ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    continue;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	ᮚᮦᮔ᮪ (*rp != NULL)
	{
	    ᮚᮦᮔ᮪ (!vim_regexec_prog(rp, FALSE, word, 0))
		continue;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (cond_req)
	    continue;

	// It's a match!  Return the WF_ flags.
	ᮝᮜᮤ pidx;
    }
    ᮝᮜᮤ 0;
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
find_prefix(matchinf_T *mip, puur_naa_nka mode)
{
    idx_T	arridx = 0;
    puur_naa_nka		len;
    puur_naa_nka		wlen = 0;
    puur_naa_nka		flen;
    puur_naa_nka		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ᮚᮦᮔ᮪ (byts == NULL)
	ᮝᮜᮤ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ᮚᮦᮔ᮪ (mode == FIND_COMPOUND)
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
    for (;;)
    {
	ᮚᮦᮔ᮪ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ᮚᮦᮔ᮪ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ᮞᮨᮓᮨᮜ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ᮚᮦᮔ᮪ (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ᮚᮦᮔ᮪ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    ᮜᮤᮚ
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    ᮚᮦᮔ᮪ (len == 0)
		bha_nga;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ᮚᮦᮔ᮪ (ptr[wlen] == NUL)
	    bha_nga;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ᮞᮨᮓᮨᮜ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ᮚᮦᮔ᮪ (byts[m] > c)
		hi = m - 1;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (byts[m] < c)
		lo = m + 1;
	    ᮜᮤᮚ
	    {
		lo = hi = m;
		bha_nga;
	    }
	}

	// Stop if there is no matching byte.
	ᮚᮦᮔ᮪ (hi < lo || byts[lo] != c)
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
    static puur_naa_nka
fold_more(matchinf_T *mip)
{
    puur_naa_nka		flen;
    char_u	*p;

    p = mip->mi_fend;
    do
	MB_PTR_ADV(mip->mi_fend);
    ᮞᮨᮓᮨᮜ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ᮚᮦᮔ᮪ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (void)spell_casefold(mip->mi_win, p, (puur_naa_nka)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (puur_naa_nka)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ᮝᮜᮤ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    puur_naa_nka
spell_valid_case(
    puur_naa_nka	    wordflags,	    // flags for the checked word.
    puur_naa_nka	    treeflags)	    // flags for the word in the spell tree
{
    ᮝᮜᮤ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    puur_naa_nka
spell_check_window(win_T *wp)
{
    ᮝᮜᮤ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(var_na **)(wp->w_s->b_langp.ga_data) != NULL;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    static puur_naa_nka
no_spell_checking(win_T *wp)
{
    ᮚᮦᮔ᮪ (spell_check_window(wp))
	ᮝᮜᮤ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ᮝᮜᮤ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    puur_naa_nka
spell_move_to(
    win_T	*wp,
    puur_naa_nka		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    puur_naa_nka		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    puur_naa_nka		found_len = 0;
    char_u	*line;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    puur_naa_nka		len;
#ifdef FEAT_SYN_HL
    puur_naa_nka		has_syntax = syntax_present(wp);
#endif
    puur_naa_nka		col;
    puur_naa_nka		can_spell;
    char_u	*buf = NULL;
    puur_naa_nka		buflen = 0;
    puur_naa_nka		skip = 0;
    puur_naa_nka		capcol = -1;
    puur_naa_nka		found_one = FALSE;
    puur_naa_nka		wrapped = FALSE;

    ᮚᮦᮔ᮪ (no_spell_checking(wp))
	ᮝᮜᮤ 0;

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

    ᮞᮨᮓᮨᮜ (!got_int)
    {
	puur_naa_nka empty_line;

	line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	ᮚᮦᮔ᮪ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ᮚᮦᮔ᮪ (buf == NULL)
		bha_nga;
	}

	// In first line check first word for Capital.
	ᮚᮦᮔ᮪ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ᮚᮦᮔ᮪ (capcol == 0)
	    capcol = getwhitecols(line);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(line);
	    ᮚᮦᮔ᮪ (check_need_cap(curwin, lnum, col))
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
	ᮚᮦᮔ᮪ (lnum < wp->w_buffer->b_ml.ml_line_count)
	    spell_cat_line(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ᮞᮨᮓᮨᮜ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ᮚᮦᮔ᮪ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		bha_nga;

	    // start of word
	    attr = HLF_COUNT;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ᮚᮦᮔ᮪ (attr != HLF_COUNT)
	    {
		// We found a bad word.  Check the attribute.
		ᮚᮦᮔ᮪ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ᮚᮦᮔ᮪ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			ᮚᮦᮔ᮪ (has_syntax)
			{
			    col = (puur_naa_nka)(p - buf);
			    (void)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    ᮚᮦᮔ᮪ (!can_spell)
				attr = HLF_COUNT;
			}
			ᮜᮤᮚ
#endif
			    can_spell = TRUE;

			ᮚᮦᮔ᮪ (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (puur_naa_nka)(p - buf);
			    found_pos.coladd = 0;
			    ᮚᮦᮔ᮪ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				ᮚᮦᮔ᮪ (attrp != NULL)
				    *attrp = attr;
				ᮝᮜᮤ len;
			    }
			    ᮜᮤᮚ ᮚᮦᮔ᮪ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    ᮜᮤᮚ
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ᮚᮦᮔ᮪ (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    ᮝᮜᮤ found_len;
	}

	ᮚᮦᮔ᮪ (curline)
	    bha_nga;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ᮚᮦᮔ᮪ (lnum == wp->w_cursor.lnum && wrapped)
	    bha_nga;

	// Advance to next line.
	ᮚᮦᮔ᮪ (dir == BACKWARD)
	{
	    ᮚᮦᮔ᮪ (lnum > 1)
		--lnum;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (!p_ws)
		bha_nga;	    // at first line and 'nowrapscan'
	    ᮜᮤᮚ
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_line_count;
		wrapped = TRUE;
		ᮚᮦᮔ᮪ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    capcol = -1;
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (lnum < wp->w_buffer->b_ml.ml_line_count)
		++lnum;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (!p_ws)
		bha_nga;	    // at first line and 'nowrapscan'
	    ᮜᮤᮚ
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ᮚᮦᮔ᮪ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ᮚᮦᮔ᮪ (lnum == wp->w_cursor.lnum && !found_one)
		bha_nga;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ᮚᮦᮔ᮪ (attr == HLF_COUNT)
		skip = (puur_naa_nka)(p - endp);
	    ᮜᮤᮚ
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ᮚᮦᮔ᮪ (empty_line)
		capcol = 0;
	}

	line_breakcheck();
    }

    vim_free(buf);
    ᮝᮜᮤ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    void
spell_cat_line(char_u *buf, char_u *line, puur_naa_nka maxlen)
{
    char_u	*p;
    puur_naa_nka		n;

    p = skipwhite(line);
    ᮞᮨᮓᮨᮜ (vim_strchr((char_u *)"*#/\"\t", *p) != NULL)
	p = skipwhite(p + 1);

    ᮚᮦᮔ᮪ (*p == NUL)
	ᮝᮜᮤ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (puur_naa_nka)(p - line) + 1;
    ᮚᮦᮔ᮪ (n < maxlen - 1)
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
    puur_naa_nka	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    static void
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    puur_naa_nka		r;
    spelload_T	sl;
    puur_naa_nka		round;

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
    for (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((var_na *)fname_enc, parimaa_na(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#ᮜᮤᮚ
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	ᮚᮦᮔ᮪ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((var_na *)fname_enc, parimaa_na(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#ᮜᮤᮚ
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    ᮚᮦᮔ᮪ (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		continue;
	    bha_nga;
	}
	bha_nga;
    }

    ᮚᮦᮔ᮪ (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#ᮜᮤᮚ
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (sl.sl_slang != NULL)
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

    ᮚᮦᮔ᮪ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ᮝᮜᮤ p_enc;
    ᮝᮜᮤ (char_u *)"latin1";
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
    ᮚᮦᮔ᮪ (lp != NULL)
    {
	ᮚᮦᮔ᮪ (lang != NULL)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, parimaa_na(fromto_T), 10);
	ga_init2(&lp->sl_repsal, parimaa_na(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ᮝᮜᮤ lp;
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

    ᮞᮨᮓᮨᮜ (gap->ga_len > 0)
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
    puur_naa_nka		i;
    puur_naa_nka		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    for (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ᮞᮨᮓᮨᮜ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ᮚᮦᮔ᮪ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ᮚᮦᮔ᮪ (gap->ga_data != NULL)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    for (i = 0; i < gap->ga_len; ++i)
		vim_free(((puur_naa_nka **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    ᮜᮤᮚ
	free_sal_items(gap);

    for (i = 0; i < lp->sl_prefixcnt; ++i)
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
    ᮚᮦᮔ᮪ (slang == NULL)
	ᮝᮜᮤ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ᮚᮦᮔ᮪ (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (slang->sl_nobreak)
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
    puur_naa_nka		len,	    // word length, -1 for up to NUL
    puur_naa_nka		count)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ᮚᮦᮔ᮪ (len == -1)
	p = word;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (len >= MAXWLEN)
	ᮝᮜᮤ;
    ᮜᮤᮚ
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	ᮚᮦᮔ᮪ (wc == NULL)
	    ᮝᮜᮤ;
	STRCPY(wc->wc_word, p);
	wc->wc_count = count;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    ᮜᮤᮚ
    {
	wc = HI2WC(hi);
	ᮚᮦᮔ᮪ ((wc->wc_count += count) < (unsigned)count)	// check for overflow
	    wc->wc_count = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    puur_naa_nka
byte_in_str(char_u *str, puur_naa_nka n)
{
    char_u	*p;

    for (p = str; *p != NUL; ++p)
	ᮚᮦᮔ᮪ (*p == n)
	    ᮝᮜᮤ TRUE;
    ᮝᮜᮤ FALSE;
}

#define SY_MAXLEN   30
typedef sa_nracanaa syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    puur_naa_nka		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    puur_naa_nka
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    puur_naa_nka		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_items, parimaa_na(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ᮞᮨᮓᮨᮜ (p != NULL)
    {
	*p++ = NUL;
	ᮚᮦᮔ᮪ (*p == NUL)	    // trailing slash
	    bha_nga;
	s = p;
	p = vim_strchr(p, '/');
	ᮚᮦᮔ᮪ (p == NULL)
	    l = (puur_naa_nka)STRLEN(s);
	ᮜᮤᮚ
	    l = (puur_naa_nka)(p - s);
	ᮚᮦᮔ᮪ (l >= SY_MAXLEN)
	    ᮝᮜᮤ SP_FORMERROR;
	ᮚᮦᮔ᮪ (ga_grow(&slang->sl_syl_items, 1) == FAIL)
	    ᮝᮜᮤ SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_items.ga_data)
					       + slang->sl_syl_items.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ᮝᮜᮤ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    static puur_naa_nka
count_syllables(slang_T *slang, char_u *word)
{
    puur_naa_nka		cnt = 0;
    puur_naa_nka		skip = FALSE;
    char_u	*p;
    puur_naa_nka		len;
    puur_naa_nka		i;
    syl_item_T	*syl;
    puur_naa_nka		c;

    ᮚᮦᮔ᮪ (slang->sl_syllable == NULL)
	ᮝᮜᮤ 0;

    for (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ᮚᮦᮔ᮪ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    continue;
	}

	// Find longest match of syllable items.
	len = 0;
	for (i = 0; i < slang->sl_syl_items.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_items.ga_data) + i;
	    ᮚᮦᮔ᮪ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ᮚᮦᮔ᮪ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	ᮜᮤᮚ
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ᮚᮦᮔ᮪ (vim_strchr(slang->sl_syllable, c) == NULL)
		skip = FALSE;	    // No, search for next syllable
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ᮝᮜᮤ cnt;
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
    puur_naa_nka		filename;
    puur_naa_nka		region_mask;
    slang_T	*slang;
    puur_naa_nka		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    puur_naa_nka		len;
    char_u	*p;
    puur_naa_nka		round;
    char_u	*spf;
    char_u	*use_region = NULL;
    puur_naa_nka		dont_use_region = FALSE;
    puur_naa_nka		nobreak = FALSE;
    puur_naa_nka		i, j;
    langp_T	*lp, *lp2;
    static puur_naa_nka	recursive = FALSE;
    var_na	*ret_msg = NULL;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ᮚᮦᮔ᮪ (recursive)
	ᮝᮜᮤ NULL;
    recursive = TRUE;

    ga_init2(&ga, parimaa_na(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ᮚᮦᮔ᮪ (spl_copy == NULL)
	goto theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    for (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = NULL;

	ᮚᮦᮔ᮪ (!valid_spelllang(lang))
	    continue;

	ᮚᮦᮔ᮪ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    continue;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ᮚᮦᮔ᮪ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ᮚᮦᮔ᮪ (p != NULL && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    ᮜᮤᮚ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ᮚᮦᮔ᮪ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    bha_nga;
	}
	ᮜᮤᮚ
	{
	    filename = FALSE;
	    ᮚᮦᮔ᮪ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    ᮜᮤᮚ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ᮚᮦᮔ᮪ (STRICMP(lang, slang->sl_name) == 0)
		    bha_nga;
	}

	ᮚᮦᮔ᮪ (region != NULL)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ᮚᮦᮔ᮪ (use_region != NULL && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ᮚᮦᮔ᮪ (slang == NULL)
	{
	    ᮚᮦᮔ᮪ (filename)
		(void)spell_load_file(lang, lang, NULL, FALSE);
	    ᮜᮤᮚ
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ᮚᮦᮔ᮪ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
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
	    ᮚᮦᮔ᮪ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		ᮚᮦᮔ᮪ (!filename && region != NULL)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    ᮚᮦᮔ᮪ (c == REGION_ALL)
		    {
			ᮚᮦᮔ᮪ (slang->sl_add)
			{
			    ᮚᮦᮔ᮪ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			ᮜᮤᮚ
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    ᮜᮤᮚ
			region_mask = 1 << c;
		}

		ᮚᮦᮔ᮪ (region_mask != 0)
		{
		    ᮚᮦᮔ᮪ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_msg = e_out_of_memory;
			goto theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ᮚᮦᮔ᮪ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    for (round = 0; round == 0 || *spf != NUL; ++round)
    {
	ᮚᮦᮔ᮪ (round == 0)
	{
	    // Internal wordlist, if there is one.
	    ᮚᮦᮔ᮪ (int_wordlist == NULL)
		continue;
	    int_wordlist_spl(spf_name);
	}
	ᮜᮤᮚ
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    for (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		ᮚᮦᮔ᮪ (p != NULL && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    bha_nga;
	    }
	    ᮚᮦᮔ᮪ (c < ga.ga_len)
		continue;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ᮚᮦᮔ᮪ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		bha_nga;
	ᮚᮦᮔ᮪ (slang == NULL)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ᮚᮦᮔ᮪ (round == 0)
		STRCPY(lang, "internal wordlist");
	    ᮜᮤᮚ
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ᮚᮦᮔ᮪ (p != NULL)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, NULL, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ᮚᮦᮔ᮪ (slang != NULL && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ᮚᮦᮔ᮪ (slang != NULL && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    ᮚᮦᮔ᮪ (use_region != NULL && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		ᮚᮦᮔ᮪ (c != REGION_ALL)
		    region_mask = 1 << c;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    ᮚᮦᮔ᮪ (region_mask != 0)
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
    for (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	ᮚᮦᮔ᮪ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	ᮜᮤᮚ
	    // find first similar language that does sound folding
	    for (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ᮚᮦᮔ᮪ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    bha_nga;
		}
	    }

	// REP items
	ᮚᮦᮔ᮪ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	ᮜᮤᮚ
	    // find first similar language that has REP items
	    for (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ᮚᮦᮔ᮪ (lp2->lp_slang->sl_rep.ga_len > 0
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
    ᮝᮜᮤ ret_msg;
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

    ᮚᮦᮔ᮪ (lp->sl_midword == NULL)	    // there aren't any
	ᮝᮜᮤ;

    for (p = lp->sl_midword; *p != NUL; )
	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    puur_naa_nka	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ᮚᮦᮔ᮪ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wp->w_s->b_spell_ismw_mb == NULL)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    ᮜᮤᮚ
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (puur_naa_nka)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ᮚᮦᮔ᮪ (bp != NULL)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	ᮜᮤᮚ
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    static puur_naa_nka
find_region(char_u *rp, char_u *region)
{
    puur_naa_nka		i;

    for (i = 0; ; i += 2)
    {
	ᮚᮦᮔ᮪ (rp[i] == NUL)
	    ᮝᮜᮤ REGION_ALL;
	ᮚᮦᮔ᮪ (rp[i] == region[0] && rp[i + 1] == region[1])
	    bha_nga;
    }
    ᮝᮜᮤ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    puur_naa_nka
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    puur_naa_nka		c;
    puur_naa_nka		firstcap;
    puur_naa_nka		allcap;
    puur_naa_nka		past_second = FALSE;	// past second word char

    // find first letter
    for (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ᮚᮦᮔ᮪ (end == NULL ? *p == NUL : p >= end)
	    ᮝᮜᮤ 0;	    // only non-word characters, illegal word
    ᮚᮦᮔ᮪ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    ᮜᮤᮚ
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    for ( ; end == NULL ? *p != NUL : p < end; MB_PTR_ADV(p))
	ᮚᮦᮔ᮪ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ᮚᮦᮔ᮪ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ᮚᮦᮔ᮪ (past_second && allcap)
		    ᮝᮜᮤ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (!allcap)
		// UlU -> KEEPCAP
		ᮝᮜᮤ WF_KEEPCAP;
	    past_second = TRUE;
	}

    ᮚᮦᮔ᮪ (allcap)
	ᮝᮜᮤ WF_ALLCAP;
    ᮚᮦᮔ᮪ (firstcap)
	ᮝᮜᮤ WF_ONECAP;
    ᮝᮜᮤ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    void
spell_delete_wordlist(void)
{
    char_u	fname[MAXPATHL];

    ᮚᮦᮔ᮪ (int_wordlist == NULL)
	ᮝᮜᮤ;

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

    ᮞᮨᮓᮨᮜ (first_lang != NULL)
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
	ᮚᮦᮔ᮪ (*wp->w_s->b_p_spl != NUL)
	{
		ᮚᮦᮔ᮪ (wp->w_p_spell)
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
    ᮚᮦᮔ᮪ (buf == NULL)
	ᮝᮜᮤ NULL;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_key = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ᮝᮜᮤ buf;
}

/*
 * Close the buffer used for spell info.
 */
    void
close_spellbuf(buf_T *buf)
{
    ᮚᮦᮔ᮪ (buf == NULL)
	ᮝᮜᮤ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    void
clear_spell_chartab(spelltab_T *sp)
{
    puur_naa_nka		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    for (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    for (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    for (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    for (i = 'a'; i <= 'z'; ++i)
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
    puur_naa_nka	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ᮚᮦᮔ᮪ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	for (i = 128; i <= 255; ++i)
	    ᮚᮦᮔ᮪ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_utf8)
    {
	for (i = 128; i < 256; ++i)
	{
	    puur_naa_nka f = utf_fold(i);
	    puur_naa_nka u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    ᮜᮤᮚ
    {
	// Rough guess: use locale-dependent library functions.
	for (i = 128; i < 256; ++i)
	{
	    ᮚᮦᮔ᮪ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (MB_ISLOWER(i))
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
    puur_naa_nka
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    puur_naa_nka		l;
    puur_naa_nka		c;

    ᮚᮦᮔ᮪ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ᮚᮦᮔ᮪ (l == 1)
	{
	    // be quick for ASCII
	    ᮚᮦᮔ᮪ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	ᮜᮤᮚ
	{
	    c = mb_ptr2char(p);
	    ᮚᮦᮔ᮪ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != NULL
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != NULL))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ᮚᮦᮔ᮪ (c > 255)
	    ᮝᮜᮤ spell_mb_isword_class(mb_get_class(s), wp);
	ᮝᮜᮤ spelltab.st_isw[c];
    }

    ᮝᮜᮤ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    puur_naa_nka
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    puur_naa_nka		c;

    ᮚᮦᮔ᮪ (has_mbyte)
    {
	c = mb_ptr2char(p);
	ᮚᮦᮔ᮪ (c > 255)
	    ᮝᮜᮤ spell_mb_isword_class(mb_get_class(p), wp);
	ᮝᮜᮤ spelltab.st_isw[c];
    }
    ᮝᮜᮤ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    static puur_naa_nka
spell_mb_isword_class(puur_naa_nka cl, win_T *wp)
{
    ᮚᮦᮔ᮪ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ᮝᮜᮤ cl == 2 || cl == 0x2800;
    ᮝᮜᮤ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    static puur_naa_nka
spell_iswordp_w(puur_naa_nka *p, win_T *wp)
{
    puur_naa_nka		*s;

    ᮚᮦᮔ᮪ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != NULL
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != NULL))
	s = p + 1;
    ᮜᮤᮚ
	s = p;

    ᮚᮦᮔ᮪ (*s > 255)
    {
	ᮚᮦᮔ᮪ (enc_utf8)
	    ᮝᮜᮤ spell_mb_isword_class(utf_class(*s), wp);
	ᮚᮦᮔ᮪ (enc_dbcs)
	    ᮝᮜᮤ spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	ᮝᮜᮤ 0;
    }
    ᮝᮜᮤ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    puur_naa_nka
spell_casefold(
    win_T	*wp,
    char_u	*str,
    puur_naa_nka		len,
    char_u	*buf,
    puur_naa_nka		buflen)
{
    puur_naa_nka		i;

    ᮚᮦᮔ᮪ (len >= buflen)
    {
	buf[0] = NUL;
	ᮝᮜᮤ FAIL;		// result will not fit
    }

    ᮚᮦᮔ᮪ (has_mbyte)
    {
	puur_naa_nka	outi = 0;
	char_u	*p;
	puur_naa_nka	c;

	// Fold one character at a time.
	for (p = str; p < str + len; )
	{
	    ᮚᮦᮔ᮪ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ᮝᮜᮤ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ᮚᮦᮔ᮪ (c == 0x03a3 || c == 0x03c2)
	    {
		ᮚᮦᮔ᮪ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		ᮜᮤᮚ
		    c = 0x03c3;
	    }
	    ᮜᮤᮚ
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    ᮜᮤᮚ
    {
	// Be quick for non-multibyte encodings.
	for (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ᮝᮜᮤ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    puur_naa_nka
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    ᮚᮦᮔ᮪ (wp->w_s->b_cap_prog == NULL)
	ᮝᮜᮤ FALSE;

    puur_naa_nka		need_cap = FALSE;
    char_u	*line = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : NULL;
    char_u	*line_copy = NULL;
    colnr_T	endcol = 0;

    ᮚᮦᮔ᮪ (col == 0 || getwhitecols(line) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ᮚᮦᮔ᮪ (lnum == 1)
	    need_cap = TRUE;
	ᮜᮤᮚ
	{
	    line = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    ᮚᮦᮔ᮪ (*skipwhite(line) == NUL)
		need_cap = TRUE;
	    ᮜᮤᮚ
	    {
		// Append a space in place of the line break.
		line_copy = concat_str(line, (char_u *)" ");
		ᮚᮦᮔ᮪ (line_copy == NULL)
		    ᮝᮜᮤ FALSE;

		line = line_copy;
		endcol = (colnr_T)STRLEN(line);
	    }
	}
    }
    ᮜᮤᮚ
	endcol = col;

    ᮚᮦᮔ᮪ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = line + endcol;
	for (;;)
	{
	    MB_PTR_BACK(line, p);
	    ᮚᮦᮔ᮪ (p == line || spell_iswordp_nmw(p, wp))
		bha_nga;
	    ᮚᮦᮔ᮪ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == line + endcol)
	    {
		need_cap = TRUE;
		bha_nga;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(line_copy);

    ᮝᮜᮤ need_cap;
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
    puur_naa_nka		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ᮚᮦᮔ᮪ (repl_from == NULL || repl_to == NULL)
    {
	emsg(_(e_no_previous_spell_replacement));
	ᮝᮜᮤ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ᮚᮦᮔ᮪ (frompat == NULL)
	ᮝᮜᮤ;
    frompatlen = vim_snprintf((var_na *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ᮞᮨᮓᮨᮜ (!got_int)
    {
	ᮚᮦᮔ᮪ (do_search(NULL, '/', '/', frompat, frompatlen, 1L, SEARCH_KEEP, NULL) == 0
						   || u_save_cursor() == FAIL)
	    bha_nga;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	line = ml_get_curline();
	ᮚᮦᮔ᮪ (addlen <= 0 || STRNCMP(line + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ᮚᮦᮔ᮪ (p == NULL)
		bha_nga;
	    mch_memmove(p, line, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, line + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#ᮚᮦᮔ᮪ defined(FEAT_PROP_POPUP)
	    ᮚᮦᮔ᮪ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (puur_naa_nka)addlen, APC_SUBSTITUTE);
#endif

	    ᮚᮦᮔ᮪ (curwin->w_cursor.lnum != prev_lnum)
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

    ᮚᮦᮔ᮪ (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    ᮜᮤᮚ
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
    puur_naa_nka		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    puur_naa_nka		c;
    puur_naa_nka		l;

    p = word;
    ᮚᮦᮔ᮪ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    ᮜᮤᮚ
	c = *p++;
    ᮚᮦᮔ᮪ (upper)
	c = SPELL_TOUPPER(c);
    ᮜᮤᮚ
	c = SPELL_TOFOLD(c);
    ᮚᮦᮔ᮪ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    ᮜᮤᮚ
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
    puur_naa_nka		c;

    d = wcopy;
    for (s = word; *s != NUL; )
    {
	ᮚᮦᮔ᮪ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	ᮜᮤᮚ
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ᮚᮦᮔ᮪ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ᮚᮦᮔ᮪ (d - wcopy >= MAXWLEN - 1)
		bha_nga;
	    *d++ = c;
	}
	ᮜᮤᮚ
	    c = SPELL_TOUPPER(c);

	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    ᮚᮦᮔ᮪ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		bha_nga;
	    d += mb_char2bytes(c, d);
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (d - wcopy >= MAXWLEN - 1)
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
    puur_naa_nka
nofold_len(char_u *fword, puur_naa_nka flen, char_u *word)
{
    char_u	*p;
    puur_naa_nka		i = 0;

    for (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    for (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    ᮝᮜᮤ (puur_naa_nka)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    void
make_case_word(char_u *fword, char_u *cword, puur_naa_nka flags)
{
    ᮚᮦᮔ᮪ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    ᮜᮤᮚ
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    puur_naa_nka		lpi;

    ᮚᮦᮔ᮪ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    ᮚᮦᮔ᮪ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		ᮝᮜᮤ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ᮝᮜᮤ vim_strsave(word);
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
    puur_naa_nka		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    ᮚᮦᮔ᮪ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    ᮜᮤᮚ
    {
	// SAL items used.  Requires the word to be case-folded.
	ᮚᮦᮔ᮪ (folded)
	    word = inword;
	ᮜᮤᮚ
	{
	    (void)spell_casefold(curwin,
				  inword, (puur_naa_nka)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	ᮚᮦᮔ᮪ (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	ᮜᮤᮚ
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
    puur_naa_nka		ri = 0;
    puur_naa_nka		c;

    ᮚᮦᮔ᮪ (has_mbyte)
    {
	puur_naa_nka	prevc = 0;
	puur_naa_nka	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	for (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ᮚᮦᮔ᮪ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (c < 256)
		c = slang->sl_sal_first[c];
	    ᮜᮤᮚ
	    {
		ip = ((puur_naa_nka **)slang->sl_sal.ga_data)[c & 0xff];
		ᮚᮦᮔ᮪ (ip == NULL)		// empty list, can't match
		    c = NUL;
		ᮜᮤᮚ
		    for (;;)		// find "c" in the list
		    {
			ᮚᮦᮔ᮪ (*ip == 0)	// not found
			{
			    c = NUL;
			    bha_nga;
			}
			ᮚᮦᮔ᮪ (*ip == c)	// match!
			{
			    c = ip[1];
			    bha_nga;
			}
			ip += 2;
		    }
	    }

	    ᮚᮦᮔ᮪ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ᮚᮦᮔ᮪ (ri + MB_MAXBYTES > MAXWLEN)
		    bha_nga;
		prevc = c;
	    }
	}
    }
    ᮜᮤᮚ
    {
	// The sl_sal_first[] table contains the translation.
	for (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ᮚᮦᮔ᮪ (VIM_ISWHITE(c))
		c = ' ';
	    ᮜᮤᮚ
		c = slang->sl_sal_first[c];
	    ᮚᮦᮔ᮪ (c != NUL && (ri == 0 || res[ri - 1] != c))
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
    puur_naa_nka		i, j, z;
    puur_naa_nka		reslen;
    puur_naa_nka		n, k = 0;
    puur_naa_nka		z0;
    puur_naa_nka		k0;
    puur_naa_nka		n0;
    puur_naa_nka		c;
    puur_naa_nka		pri;
    puur_naa_nka		p0 = -333;
    puur_naa_nka		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ᮚᮦᮔ᮪ (slang->sl_rem_accents)
    {
	t = word;
	ᮞᮨᮓᮨᮜ (*s != NUL)
	{
	    ᮚᮦᮔ᮪ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    ᮜᮤᮚ
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ᮞᮨᮓᮨᮜ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	ᮚᮦᮔ᮪ (n >= 0)
	{
	    // check all rules for the same letter
	    for (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ᮚᮦᮔ᮪ (k > 1)
		{
		    ᮚᮦᮔ᮪ (word[i + 1] != s[1])
			continue;
		    ᮚᮦᮔ᮪ (k > 2)
		    {
			for (j = 2; j < k; ++j)
			    ᮚᮦᮔ᮪ (word[i + j] != s[j])
				bha_nga;
			ᮚᮦᮔ᮪ (j < k)
			    continue;
		    }
		}

		ᮚᮦᮔ᮪ ((pf = smp[n].sm_oneof) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ᮞᮨᮓᮨᮜ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ᮚᮦᮔ᮪ (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ᮞᮨᮓᮨᮜ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ᮚᮦᮔ᮪ (*s == '<')
		    s++;
		ᮚᮦᮔ᮪ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ᮚᮦᮔ᮪ (*s == '^' && *(s + 1) == '^')
		    s++;

		ᮚᮦᮔ᮪ (*s == NUL
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

		    ᮚᮦᮔ᮪ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			for ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ᮚᮦᮔ᮪ (k0 > 1)
			    {
				ᮚᮦᮔ᮪ (word[i + k] != s[1])
				    continue;
				ᮚᮦᮔ᮪ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    for (j = 2; j < k0; ++j)
					ᮚᮦᮔ᮪ (*pf++ != s[j])
					    bha_nga;
				    ᮚᮦᮔ᮪ (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    ᮚᮦᮔ᮪ ((pf = smp[n0].sm_oneof) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ᮞᮨᮓᮨᮜ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ᮚᮦᮔ᮪ (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ᮞᮨᮓᮨᮜ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ᮚᮦᮔ᮪ (*s == '<')
				s++;
			    ᮚᮦᮔ᮪ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ᮚᮦᮔ᮪ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				ᮚᮦᮔ᮪ (k0 == k)
				    // this is just a piece of the string
				    continue;

				ᮚᮦᮔ᮪ (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				bha_nga;
			    }
			}

			ᮚᮦᮔ᮪ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    continue;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    ᮚᮦᮔ᮪ (s == NULL)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != NULL) ? 1 : 0;
		    ᮚᮦᮔ᮪ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ᮚᮦᮔ᮪ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ᮞᮨᮓᮨᮜ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			ᮚᮦᮔ᮪ (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    ᮜᮤᮚ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ᮞᮨᮓᮨᮜ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ᮚᮦᮔ᮪ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ᮚᮦᮔ᮪ (strstr((var_na *)pf, "^^") != NULL)
			{
			    ᮚᮦᮔ᮪ (c != NUL && reslen < MAXWLEN - 1)
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
	ᮜᮤᮚ ᮚᮦᮔ᮪ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ᮚᮦᮔ᮪ (z0 == 0)
	{
	    ᮚᮦᮔ᮪ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    puur_naa_nka		word[MAXWLEN];
    puur_naa_nka		wres[MAXWLEN];
    puur_naa_nka		l;
    char_u	*s;
    puur_naa_nka		*ws;
    char_u	*t;
    puur_naa_nka		*pf;
    puur_naa_nka		i, j, z;
    puur_naa_nka		reslen;
    puur_naa_nka		n, k = 0;
    puur_naa_nka		z0;
    puur_naa_nka		k0;
    puur_naa_nka		n0;
    puur_naa_nka		c;
    puur_naa_nka		pri;
    puur_naa_nka		p0 = -333;
    puur_naa_nka		c0;
    puur_naa_nka		did_white = FALSE;
    puur_naa_nka		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    for (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ᮚᮦᮔ᮪ (slang->sl_rem_accents)
	{
	    ᮚᮦᮔ᮪ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ᮚᮦᮔ᮪ (did_white)
		    continue;
		c = ' ';
		did_white = TRUE;
	    }
	    ᮜᮤᮚ
	    {
		did_white = FALSE;
		ᮚᮦᮔ᮪ (!spell_iswordp_nmw(t, curwin))
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
    ᮞᮨᮓᮨᮜ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	ᮚᮦᮔ᮪ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    for (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ᮚᮦᮔ᮪ (c != ws[0])
		    continue;
		k = smp[n].sm_leadlen;
		ᮚᮦᮔ᮪ (k > 1)
		{
		    ᮚᮦᮔ᮪ (word[i + 1] != ws[1])
			continue;
		    ᮚᮦᮔ᮪ (k > 2)
		    {
			for (j = 2; j < k; ++j)
			    ᮚᮦᮔ᮪ (word[i + j] != ws[j])
				bha_nga;
			ᮚᮦᮔ᮪ (j < k)
			    continue;
		    }
		}

		ᮚᮦᮔ᮪ ((pf = smp[n].sm_oneof_w) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ᮞᮨᮓᮨᮜ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ᮚᮦᮔ᮪ (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ᮞᮨᮓᮨᮜ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ᮚᮦᮔ᮪ (*s == '<')
		    s++;
		ᮚᮦᮔ᮪ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ᮚᮦᮔ᮪ (*s == '^' && *(s + 1) == '^')
		    s++;

		ᮚᮦᮔ᮪ (*s == NUL
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

		    ᮚᮦᮔ᮪ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			for ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ᮚᮦᮔ᮪ (c0 != ws[0])
				continue;
			    k0 = smp[n0].sm_leadlen;
			    ᮚᮦᮔ᮪ (k0 > 1)
			    {
				ᮚᮦᮔ᮪ (word[i + k] != ws[1])
				    continue;
				ᮚᮦᮔ᮪ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    for (j = 2; j < k0; ++j)
					ᮚᮦᮔ᮪ (*pf++ != ws[j])
					    bha_nga;
				    ᮚᮦᮔ᮪ (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    ᮚᮦᮔ᮪ ((pf = smp[n0].sm_oneof_w) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ᮞᮨᮓᮨᮜ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ᮚᮦᮔ᮪ (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ᮞᮨᮓᮨᮜ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ᮚᮦᮔ᮪ (*s == '<')
				s++;
			    ᮚᮦᮔ᮪ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ᮚᮦᮔ᮪ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				ᮚᮦᮔ᮪ (k0 == k)
				    // this is just a piece of the string
				    continue;

				ᮚᮦᮔ᮪ (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				bha_nga;
			    }
			}

			ᮚᮦᮔ᮪ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    continue;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != NULL) ? 1 : 0;
		    ᮚᮦᮔ᮪ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ᮚᮦᮔ᮪ (reslen > 0 && ws != NULL && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ᮚᮦᮔ᮪ (ws != NULL)
			    ᮞᮨᮓᮨᮜ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ᮚᮦᮔ᮪ (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    parimaa_na(puur_naa_nka) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    ᮜᮤᮚ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ᮚᮦᮔ᮪ (ws != NULL)
			    ᮞᮨᮓᮨᮜ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ᮚᮦᮔ᮪ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ᮚᮦᮔ᮪ (ws == NULL)
			    c = NUL;
			ᮜᮤᮚ
			    c = *ws;
			ᮚᮦᮔ᮪ (strstr((var_na *)s, "^^") != NULL)
			{
			    ᮚᮦᮔ᮪ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       parimaa_na(puur_naa_nka) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    bha_nga;
		}
	    }
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ᮚᮦᮔ᮪ (z0 == 0)
	{
	    ᮚᮦᮔ᮪ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    for (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ᮚᮦᮔ᮪ (l + MB_MAXBYTES > MAXWLEN)
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
    puur_naa_nka		lpi;
    langp_T	*lp;
    char_u	*p;

    ᮚᮦᮔ᮪ (no_spell_checking(curwin))
	ᮝᮜᮤ;

    msg_start();
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	msg_puts("file: ");
	msg_puts((var_na *)lp->lp_slang->sl_fname);
	msg_putchar('\n');
	p = lp->lp_slang->sl_info;
	ᮚᮦᮔ᮪ (p != NULL)
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

    ᮚᮦᮔ᮪ (no_spell_checking(curwin))
	ᮝᮜᮤ;
    (void)get_option_value((char_u*)"spl", &dummy, &spl, NULL, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_value_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_value_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ᮚᮦᮔ᮪ (!BUFEMPTY())
	ᮝᮜᮤ;

    spell_dump_compl(NULL, 0, NULL, eap->forceit ? DUMPFLAG_COUNT : 0);

    // Delete the empty line that we started with.
    ᮚᮦᮔ᮪ (curbuf->b_ml.ml_line_count > 1)
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
    puur_naa_nka		ic,	    // ignore case
    puur_naa_nka		*dir,	    // direction for adding matches
    puur_naa_nka		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    puur_naa_nka		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    puur_naa_nka		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    puur_naa_nka		round;
    puur_naa_nka		depth;
    puur_naa_nka		n;
    puur_naa_nka		flags;
    char_u	*region_names = NULL;	    // region names being used
    puur_naa_nka		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    puur_naa_nka		lpi;
    puur_naa_nka		dumpflags = dumpflags_arg;
    puur_naa_nka		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ᮚᮦᮔ᮪ (pat != NULL)
    {
	ᮚᮦᮔ᮪ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	ᮜᮤᮚ
	{
	    n = captype(pat, NULL);
	    ᮚᮦᮔ᮪ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (n == WF_ALLCAP && (puur_naa_nka)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ᮚᮦᮔ᮪ (p[0] != 0)
	{
	    ᮚᮦᮔ᮪ (region_names == NULL)	    // first language with regions
		region_names = p;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		bha_nga;
	    }
	}
    }

    ᮚᮦᮔ᮪ (do_region && region_names != NULL && pat == NULL)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    ᮜᮤᮚ
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ᮚᮦᮔ᮪ (slang->sl_fbyts == NULL)	    // reloading failed
	    continue;

	ᮚᮦᮔ᮪ (pat == NULL)
	{
	    vim_snprintf((var_na *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ᮚᮦᮔ᮪ (pat != NULL && slang->sl_pbyts == NULL)
	    patlen = (puur_naa_nka)STRLEN(pat);
	ᮜᮤᮚ
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	for (round = 1; round <= 2; ++round)
	{
	    ᮚᮦᮔ᮪ (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    ᮜᮤᮚ
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ᮚᮦᮔ᮪ (byts == NULL)
		continue;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ᮞᮨᮓᮨᮜ (depth >= 0 && !got_int
				  && (pat == NULL || !ins_compl_interrupted()))
	    {
		ᮚᮦᮔ᮪ (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    line_breakcheck();
		    ins_compl_check_keys(50, FALSE);
		}
		ᮜᮤᮚ
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    ᮚᮦᮔ᮪ (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (puur_naa_nka)idxs[n];
			ᮚᮦᮔ᮪ ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    ᮚᮦᮔ᮪ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    ᮚᮦᮔ᮪ (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				ᮚᮦᮔ᮪ (pat == NULL)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ᮚᮦᮔ᮪ (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    ᮜᮤᮚ
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
			ᮚᮦᮔ᮪ (depth <= patlen
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
    puur_naa_nka		*dir,
    puur_naa_nka		dumpflags,
    puur_naa_nka		wordflags,
    linenr_T	lnum)
{
    puur_naa_nka		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    puur_naa_nka		i;
    puur_naa_nka		flags = wordflags;

    ᮚᮦᮔ᮪ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ᮚᮦᮔ᮪ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ᮚᮦᮔ᮪ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    ᮜᮤᮚ
    {
	p = word;
	ᮚᮦᮔ᮪ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, NULL) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ᮚᮦᮔ᮪ (pat == NULL)
    {
	// Add flags and regions after a slash.
	ᮚᮦᮔ᮪ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ᮚᮦᮔ᮪ (keepcap)
		STRCAT(badword, "=");
	    ᮚᮦᮔ᮪ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (flags & WF_RARE)
		STRCAT(badword, "?");
	    ᮚᮦᮔ᮪ (flags & WF_REGION)
		for (i = 0; i < 7; ++i)
		    ᮚᮦᮔ᮪ (flags & (0x10000 << i))
			sprintf((var_na *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ᮚᮦᮔ᮪ (dumpflags & DUMPFLAG_COUNT)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((var_na *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_count);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (puur_naa_nka)STRLEN(p),
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
    puur_naa_nka		*dir,
    puur_naa_nka		dumpflags,
    puur_naa_nka		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    puur_naa_nka		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    puur_naa_nka		has_word_up = FALSE;
    puur_naa_nka		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    puur_naa_nka		depth;
    puur_naa_nka		n;
    puur_naa_nka		len;
    puur_naa_nka		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    ᮚᮦᮔ᮪ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ᮚᮦᮔ᮪ (byts != NULL)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ᮞᮨᮓᮨᮜ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    ᮚᮦᮔ᮪ (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		line_breakcheck();
	    }
	    ᮜᮤᮚ
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		ᮚᮦᮔ᮪ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    for (i = 1; i < len; ++i)
			ᮚᮦᮔ᮪ (byts[n + i] != 0)
			    bha_nga;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    ᮚᮦᮔ᮪ (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ᮚᮦᮔ᮪ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ᮚᮦᮔ᮪ (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			ᮚᮦᮔ᮪ (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ᮚᮦᮔ᮪ (lnum != 0)
				++lnum;
			}
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    ᮝᮜᮤ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    ᮞᮨᮓᮨᮜ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ᮝᮜᮤ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    puur_naa_nka
spell_word_start(puur_naa_nka startcol)
{
    char_u	*line;
    char_u	*p;
    puur_naa_nka		col = 0;

    ᮚᮦᮔ᮪ (no_spell_checking(curwin))
	ᮝᮜᮤ startcol;

    // Find a word character before "startcol".
    line = ml_get_curline();
    for (p = line + startcol; p > line; )
    {
	MB_PTR_BACK(line, p);
	ᮚᮦᮔ᮪ (spell_iswordp_nmw(p, curwin))
	    bha_nga;
    }

    // Go back to start of the word.
    ᮞᮨᮓᮨᮜ (p > line)
    {
	col = (puur_naa_nka)(p - line);
	MB_PTR_BACK(line, p);
	ᮚᮦᮔ᮪ (!spell_iswordp(p, curwin))
	    bha_nga;
	col = 0;
    }

    ᮝᮜᮤ col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
static puur_naa_nka spell_expand_need_cap;

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
    puur_naa_nka
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    ᮝᮜᮤ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    puur_naa_nka
valid_spelllang(char_u *val)
{
    ᮝᮜᮤ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    puur_naa_nka
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    puur_naa_nka		l;

    spf = val;
    ᮞᮨᮓᮨᮜ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ᮚᮦᮔ᮪ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ᮝᮜᮤ FALSE;
	for (s = spf_name; *s != NUL; ++s)
	    ᮚᮦᮔ᮪ (!vim_is_fname_char(*s))
		ᮝᮜᮤ FALSE;
    }
    ᮝᮜᮤ TRUE;
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
	ᮚᮦᮔ᮪ (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    bha_nga;
	}
    ᮝᮜᮤ errmsg;
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

    ᮚᮦᮔ᮪ (synblock->b_p_spc == NULL || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = NULL;
    ᮜᮤᮚ
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ᮚᮦᮔ᮪ (re != NULL)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ᮚᮦᮔ᮪ (synblock->b_cap_prog == NULL)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ᮝᮜᮤ e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    ᮝᮜᮤ NULL;
}

#endif  // FEAT_SPELL
