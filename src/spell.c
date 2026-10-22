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

#ถ้า defined(FEAT_SPELL)

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
    จำนวนเต็ม		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    จำนวนเต็ม		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    จำนวนเต็ม		mi_prefcnt;		// number of entries at mi_prefarridx
    จำนวนเต็ม		mi_prefixlen;		// byte length of prefix
    จำนวนเต็ม		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    จำนวนเต็ม		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    จำนวนเต็ม		mi_complen;		// nr of compound words used
    จำนวนเต็ม		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    จำนวนเต็ม		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    จำนวนเต็ม		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    จำนวนเต็ม		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


static จำนวนเต็ม spell_mb_isword_class(จำนวนเต็ม cl, win_T *wp);

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

static void find_word(matchinf_T *mip, จำนวนเต็ม mode);
static void find_prefix(matchinf_T *mip, จำนวนเต็ม mode);
static จำนวนเต็ม fold_more(matchinf_T *mip);
static void spell_load_cb(char_u *fname, void *cookie);
static จำนวนเต็ม count_syllables(slang_T *slang, char_u *word);
static void clear_midword(win_T *buf);
static void use_midword(slang_T *lp, win_T *buf);
static จำนวนเต็ม find_region(char_u *rp, char_u *region);
static void spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
static void spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
static void spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
static void dump_word(slang_T *slang, char_u *word, char_u *pat, จำนวนเต็ม *dir, จำนวนเต็ม round, จำนวนเต็ม flags, linenr_T lnum);
static linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, จำนวนเต็ม *dir, จำนวนเต็ม round, จำนวนเต็ม flags, linenr_T startlnum);
static char_u *advance_camelcase_word(char_u *p, win_T *wp, จำนวนเต็ม *is_camel_case);

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
    จำนวนเต็ม
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    จำนวนเต็ม		*capcol,	// column to check for Capital
    จำนวนเต็ม		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    จำนวนเต็ม		nrlen = 0;	// found a number first
    จำนวนเต็ม		c;
    จำนวนเต็ม		wrongcaplen = 0;
    จำนวนเต็ม		lpi;
    จำนวนเต็ม		count_word = docount;
    จำนวนเต็ม		use_camel_case = *wp->w_s->b_p_spo != NUL;
    จำนวนเต็ม		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ถ้า (*ptr <= ' ')
	คืน 1;

    // Return here when loading language files failed.
    ถ้า (wp->w_s->b_langp.ga_len == 0)
	คืน 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ถ้า (*ptr >= '0' && *ptr <= '9')
    {
	ถ้า (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	มิฉะนั้น ถ้า (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	มิฉะนั้น
	    mi.mi_end = skipdigits(ptr);
	nrlen = (จำนวนเต็ม)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    ถ้า (spell_iswordp(mi.mi_fend, wp))
    {
	ถ้า (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	มิฉะนั้น
	{
	    do
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ขณะ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ถ้า (capcol != NULL && *capcol == 0 && wp->w_s->b_cap_prog != NULL)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ถ้า (!SPELL_ISUPPER(c))
		wrongcaplen = (จำนวนเต็ม)(mi.mi_fend - ptr);
	}
    }
    ถ้า (capcol != NULL)
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
    ถ้า (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (void)spell_casefold(wp, ptr, (จำนวนเต็ม)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (จำนวนเต็ม)STRLEN(mi.mi_fword);

    ถ้า (is_camel_case && mi.mi_fwordlen > 0)
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
    สำหรับ (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ถ้า (mi.mi_lp->lp_slang->sl_fidxs == NULL)
	    continue;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ถ้า (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ถ้า (count_word && mi.mi_result == SP_OK)
	{
	    count_common_word(mi.mi_lp->lp_slang, ptr,
						   (จำนวนเต็ม)(mi.mi_end - ptr), 1);
	    count_word = FALSE;
	}
    }

    ถ้า (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ถ้า (nrlen > 0)
	{
	    ถ้า (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		คืน nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	มิฉะนั้น ถ้า (!spell_iswordp_nmw(ptr, wp))
	{
	    ถ้า (capcol != NULL && wp->w_s->b_cap_prog != NULL)
	    {
		regmatch_T	regmatch;
		จำนวนเต็ม		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ถ้า (r)
		    *capcol = (จำนวนเต็ม)(regmatch.endp[0] - ptr);
	    }

	    ถ้า (has_mbyte)
		คืน (*mb_ptr2len)(ptr);
	    คืน 1;
	}
	มิฉะนั้น ถ้า (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	มิฉะนั้น ถ้า (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    จำนวนเต็ม		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    ถ้า (mi.mi_lp->lp_slang->sl_fidxs != NULL)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		สำหรับ (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ถ้า (p >= mi.mi_end)
			bha_nga;
		    mi.mi_compoff = (จำนวนเต็ม)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    ถ้า (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			bha_nga;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	ถ้า (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	มิฉะนั้น ถ้า (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	มิฉะนั้น
	    *attrp = HLF_SPL;
    }

    ถ้า (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	คืน wrongcaplen;
    }

    คืน (จำนวนเต็ม)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    static จำนวนเต็ม
get_char_type(จำนวนเต็ม c)
{
    ถ้า (VIM_ISDIGIT(c))
	คืน CHAR_DIGIT;
    ถ้า (SPELL_ISUPPER(c))
	คืน CHAR_UPPER;
    คืน CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    static char_u *
advance_camelcase_word(char_u *str, win_T *wp, จำนวนเต็ม *is_camel_case)
{
    จำนวนเต็ม last_type, last_last_type, this_type;
    จำนวนเต็ม c;
    char_u *end = str;

    *is_camel_case = FALSE;

    ถ้า (*str == NUL)
	คืน str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    ขณะ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	ถ้า (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    bha_nga;
	}
	มิฉะนั้น ถ้า ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
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

    คืน end;
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
find_word(matchinf_T *mip, จำนวนเต็ม mode)
{
    idx_T	arridx = 0;
    จำนวนเต็ม		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    จำนวนเต็ม		endidxcnt = 0;
    จำนวนเต็ม		len;
    จำนวนเต็ม		wlen = 0;
    จำนวนเต็ม		flen;
    จำนวนเต็ม		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    จำนวนเต็ม		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    จำนวนเต็ม		word_ends;
    จำนวนเต็ม		prefix_found;
    จำนวนเต็ม		nobreak_result;

    ถ้า (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ถ้า (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    มิฉะนั้น
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ถ้า (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	มิฉะนั้น ถ้า (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ถ้า (byts == NULL)
	คืน;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    สำหรับ (;;)
    {
	ถ้า (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ถ้า (byts[arridx] == 0)
	{
	    ถ้า (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		คืน;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ขณะ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ถ้า (len == 0)
		bha_nga;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ถ้า (ptr[wlen] == NUL)
	    bha_nga;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ถ้า (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ขณะ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ถ้า (byts[m] > c)
		hi = m - 1;
	    มิฉะนั้น ถ้า (byts[m] < c)
		lo = m + 1;
	    มิฉะนั้น
	    {
		lo = hi = m;
		bha_nga;
	    }
	}

	// Stop if there is no matching byte.
	ถ้า (hi < lo || byts[lo] != c)
	    bha_nga;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ถ้า (c == ' ')
	{
	    สำหรับ (;;)
	    {
		ถ้า (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		ถ้า (ptr[wlen] != ' ' && ptr[wlen] != TAB)
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
    ขณะ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ถ้า ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    continue;	    // not at first byte of character
	ถ้า (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ถ้า (slang->sl_compprog == NULL && !slang->sl_nobreak)
		continue;	    // next char is a word character
	    word_ends = FALSE;
	}
	มิฉะนั้น
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	ถ้า (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    ถ้า (STRNCMP(ptr, p, wlen) != 0)
	    {
		สำหรับ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (จำนวนเต็ม)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	สำหรับ (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ถ้า (mode == FIND_FOLDWORD)
	    {
		ถ้า (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		ถ้า (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    continue;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    มิฉะนั้น ถ้า (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		ถ้า (c == 0)
		    continue;

		// Use the WF_RARE flag for a rare prefix.
		ถ้า (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    ถ้า (slang->sl_nobreak)
	    {
		ถ้า ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    bha_nga;
		}
	    }

	    มิฉะนั้น ถ้า ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ถ้า (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    continue;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ถ้า (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			continue;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ถ้า (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    continue;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ถ้า (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    continue;
		ถ้า (!word_ends && (flags & WF_NOCOMPAFT))
		    continue;

		// Quickly check if compounding is possible with this flag.
		ถ้า (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    continue;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ถ้า (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    continue;

		ถ้า (mode == FIND_COMPOUND)
		{
		    จำนวนเต็ม	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ถ้า (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			สำหรับ (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    มิฉะนั้น
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    ถ้า (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			continue;

		    ถ้า (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			ถ้า (spell_iswordp_nmw(p, mip->mi_win)
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
		ถ้า (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    ถ้า (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ถ้า (ptr == mip->mi_word)
			    (void)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			มิฉะนั้น
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ถ้า (!can_compound(slang, fword, mip->mi_compflags))
			continue;
		}
		มิฉะนั้น ถ้า (slang->sl_comprules != NULL
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    continue;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    มิฉะนั้น ถ้า (flags & WF_NEEDCOMP)
		continue;

	    nobreak_result = SP_OK;

	    ถ้า (!word_ends)
	    {
		จำนวนเต็ม	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		จำนวนเต็ม	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ถ้า (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ถ้า (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ถ้า (STRNCMP(ptr, p, wlen) != 0)
		    {
			สำหรับ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (จำนวนเต็ม)(p - mip->mi_fword);
		    }
		}
#ถ้า 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		ถ้า (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		สำหรับ (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ถ้า (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			ถ้า (mip->mi_lp->lp_slang->sl_fidxs == NULL
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    continue;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ถ้า (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#ถ้า 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ถ้า (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    ถ้า (!slang->sl_nobreak)
			bha_nga;
		}
		--mip->mi_complen;
		ถ้า (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ถ้า (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		มิฉะนั้น
		{
		    ถ้า (mip->mi_result == SP_OK)
			bha_nga;
		    continue;
		}
	    }

	    ถ้า (flags & WF_BANNED)
		res = SP_BANNED;
	    มิฉะนั้น ถ้า (flags & WF_REGION)
	    {
		// Check region.
		ถ้า ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		มิฉะนั้น
		    res = SP_LOCAL;
	    }
	    มิฉะนั้น ถ้า (flags & WF_RARE)
		res = SP_RARE;
	    มิฉะนั้น
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ถ้า (nobreak_result == SP_BAD)
	    {
		ถ้า (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		มิฉะนั้น ถ้า (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    มิฉะนั้น ถ้า (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    มิฉะนั้น ถ้า (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    ถ้า (mip->mi_result == SP_OK)
		bha_nga;
	}

	ถ้า (mip->mi_result == SP_OK)
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
    จำนวนเต็ม
match_checkcompoundpattern(
    char_u	*ptr,
    จำนวนเต็ม		wlen,
    garray_T	*gap)  // &sl_comppat
{
    จำนวนเต็ม		i;
    char_u	*p;
    จำนวนเต็ม		len;

    สำหรับ (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	ถ้า (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (จำนวนเต็ม)STRLEN(p);
	    ถ้า (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		คืน TRUE;
	}
    }
    คืน FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    จำนวนเต็ม
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    จำนวนเต็ม		i;
    char_u	*p;

    ถ้า (slang->sl_compprog == NULL)
	คืน FALSE;
    ถ้า (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	สำหรับ (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    มิฉะนั้น
	p = flags;
    ถ้า (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	คืน FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ถ้า (slang->sl_compsylmax < MAXWLEN
		       && count_syllables(slang, word) > slang->sl_compsylmax)
	คืน (จำนวนเต็ม)STRLEN(flags) < slang->sl_compmax;
    คืน TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    จำนวนเต็ม
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    จำนวนเต็ม		i;
    จำนวนเต็ม		c;

    // loop over all the COMPOUNDRULE entries
    สำหรับ (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	สำหรับ (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ถ้า (c == NUL)
		// found a rule that matches for the flags we have so far
		คืน TRUE;
	    ถ้า (*p == '/' || *p == NUL)
		bha_nga;  // end of rule, it's too short
	    ถ้า (*p == '[')
	    {
		จำนวนเต็ม match = FALSE;

		// compare against all the flags in []
		++p;
		ขณะ (*p != ']' && *p != NUL)
		    ถ้า (*p++ == c)
			match = TRUE;
		ถ้า (!match)
		    bha_nga;  // none matches
	    }
	    มิฉะนั้น ถ้า (*p != c)
		bha_nga;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ถ้า (p == NULL)
	    bha_nga;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    คืน FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    จำนวนเต็ม
valid_word_prefix(
    จำนวนเต็ม		totprefcnt,	// nr of prefix IDs
    จำนวนเต็ม		arridx,		// idx in sl_pidxs[]
    จำนวนเต็ม		flags,
    char_u	*word,
    slang_T	*slang,
    จำนวนเต็ม		cond_req)	// only use prefixes with a condition
{
    จำนวนเต็ม		prefcnt;
    จำนวนเต็ม		pidx;
    regprog_T	**rp;
    จำนวนเต็ม		prefid;

    prefid = (unsigned)flags >> 24;
    สำหรับ (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ถ้า (prefid != (pidx & 0xff))
	    continue;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ถ้า ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    continue;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	ถ้า (*rp != NULL)
	{
	    ถ้า (!vim_regexec_prog(rp, FALSE, word, 0))
		continue;
	}
	มิฉะนั้น ถ้า (cond_req)
	    continue;

	// It's a match!  Return the WF_ flags.
	คืน pidx;
    }
    คืน 0;
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
find_prefix(matchinf_T *mip, จำนวนเต็ม mode)
{
    idx_T	arridx = 0;
    จำนวนเต็ม		len;
    จำนวนเต็ม		wlen = 0;
    จำนวนเต็ม		flen;
    จำนวนเต็ม		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ถ้า (byts == NULL)
	คืน;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ถ้า (mode == FIND_COMPOUND)
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
    สำหรับ (;;)
    {
	ถ้า (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ถ้า (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ขณะ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ถ้า (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ถ้า (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    มิฉะนั้น
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    ถ้า (len == 0)
		bha_nga;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ถ้า (ptr[wlen] == NUL)
	    bha_nga;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ขณะ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ถ้า (byts[m] > c)
		hi = m - 1;
	    มิฉะนั้น ถ้า (byts[m] < c)
		lo = m + 1;
	    มิฉะนั้น
	    {
		lo = hi = m;
		bha_nga;
	    }
	}

	// Stop if there is no matching byte.
	ถ้า (hi < lo || byts[lo] != c)
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
    static จำนวนเต็ม
fold_more(matchinf_T *mip)
{
    จำนวนเต็ม		flen;
    char_u	*p;

    p = mip->mi_fend;
    do
	MB_PTR_ADV(mip->mi_fend);
    ขณะ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ถ้า (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (void)spell_casefold(mip->mi_win, p, (จำนวนเต็ม)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (จำนวนเต็ม)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    คืน flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    จำนวนเต็ม
spell_valid_case(
    จำนวนเต็ม	    wordflags,	    // flags for the checked word.
    จำนวนเต็ม	    treeflags)	    // flags for the word in the spell tree
{
    คืน ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    จำนวนเต็ม
spell_check_window(win_T *wp)
{
    คืน wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(var_na **)(wp->w_s->b_langp.ga_data) != NULL;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    static จำนวนเต็ม
no_spell_checking(win_T *wp)
{
    ถ้า (spell_check_window(wp))
	คืน FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    คืน TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    จำนวนเต็ม
spell_move_to(
    win_T	*wp,
    จำนวนเต็ม		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    จำนวนเต็ม		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    จำนวนเต็ม		found_len = 0;
    char_u	*line;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    จำนวนเต็ม		len;
#ifdef FEAT_SYN_HL
    จำนวนเต็ม		has_syntax = syntax_present(wp);
#endif
    จำนวนเต็ม		col;
    จำนวนเต็ม		can_spell;
    char_u	*buf = NULL;
    จำนวนเต็ม		buflen = 0;
    จำนวนเต็ม		skip = 0;
    จำนวนเต็ม		capcol = -1;
    จำนวนเต็ม		found_one = FALSE;
    จำนวนเต็ม		wrapped = FALSE;

    ถ้า (no_spell_checking(wp))
	คืน 0;

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

    ขณะ (!got_int)
    {
	จำนวนเต็ม empty_line;

	line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	ถ้า (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ถ้า (buf == NULL)
		bha_nga;
	}

	// In first line check first word for Capital.
	ถ้า (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ถ้า (capcol == 0)
	    capcol = getwhitecols(line);
	มิฉะนั้น ถ้า (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(line);
	    ถ้า (check_need_cap(curwin, lnum, col))
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
	ถ้า (lnum < wp->w_buffer->b_ml.ml_line_count)
	    spell_cat_line(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ขณะ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ถ้า (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		bha_nga;

	    // start of word
	    attr = HLF_COUNT;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ถ้า (attr != HLF_COUNT)
	    {
		// We found a bad word.  Check the attribute.
		ถ้า (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ถ้า (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			ถ้า (has_syntax)
			{
			    col = (จำนวนเต็ม)(p - buf);
			    (void)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    ถ้า (!can_spell)
				attr = HLF_COUNT;
			}
			มิฉะนั้น
#endif
			    can_spell = TRUE;

			ถ้า (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (จำนวนเต็ม)(p - buf);
			    found_pos.coladd = 0;
			    ถ้า (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				ถ้า (attrp != NULL)
				    *attrp = attr;
				คืน len;
			    }
			    มิฉะนั้น ถ้า (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    มิฉะนั้น
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ถ้า (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    คืน found_len;
	}

	ถ้า (curline)
	    bha_nga;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ถ้า (lnum == wp->w_cursor.lnum && wrapped)
	    bha_nga;

	// Advance to next line.
	ถ้า (dir == BACKWARD)
	{
	    ถ้า (lnum > 1)
		--lnum;
	    มิฉะนั้น ถ้า (!p_ws)
		bha_nga;	    // at first line and 'nowrapscan'
	    มิฉะนั้น
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_line_count;
		wrapped = TRUE;
		ถ้า (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    capcol = -1;
	}
	มิฉะนั้น
	{
	    ถ้า (lnum < wp->w_buffer->b_ml.ml_line_count)
		++lnum;
	    มิฉะนั้น ถ้า (!p_ws)
		bha_nga;	    // at first line and 'nowrapscan'
	    มิฉะนั้น
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ถ้า (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ถ้า (lnum == wp->w_cursor.lnum && !found_one)
		bha_nga;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ถ้า (attr == HLF_COUNT)
		skip = (จำนวนเต็ม)(p - endp);
	    มิฉะนั้น
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ถ้า (empty_line)
		capcol = 0;
	}

	line_breakcheck();
    }

    vim_free(buf);
    คืน 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    void
spell_cat_line(char_u *buf, char_u *line, จำนวนเต็ม maxlen)
{
    char_u	*p;
    จำนวนเต็ม		n;

    p = skipwhite(line);
    ขณะ (vim_strchr((char_u *)"*#/\"\t", *p) != NULL)
	p = skipwhite(p + 1);

    ถ้า (*p == NUL)
	คืน;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (จำนวนเต็ม)(p - line) + 1;
    ถ้า (n < maxlen - 1)
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
    จำนวนเต็ม	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    static void
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    จำนวนเต็ม		r;
    spelload_T	sl;
    จำนวนเต็ม		round;

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
    สำหรับ (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((var_na *)fname_enc, parimaa_na(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#มิฉะนั้น
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	ถ้า (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((var_na *)fname_enc, parimaa_na(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#มิฉะนั้น
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    ถ้า (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		continue;
	    bha_nga;
	}
	bha_nga;
    }

    ถ้า (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#มิฉะนั้น
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    มิฉะนั้น ถ้า (sl.sl_slang != NULL)
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

    ถ้า (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	คืน p_enc;
    คืน (char_u *)"latin1";
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
    ถ้า (lp != NULL)
    {
	ถ้า (lang != NULL)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, parimaa_na(fromto_T), 10);
	ga_init2(&lp->sl_repsal, parimaa_na(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    คืน lp;
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

    ขณะ (gap->ga_len > 0)
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
    จำนวนเต็ม		i;
    จำนวนเต็ม		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    สำหรับ (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ขณะ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ถ้า (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ถ้า (gap->ga_data != NULL)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    สำหรับ (i = 0; i < gap->ga_len; ++i)
		vim_free(((จำนวนเต็ม **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    มิฉะนั้น
	free_sal_items(gap);

    สำหรับ (i = 0; i < lp->sl_prefixcnt; ++i)
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
    ถ้า (slang == NULL)
	คืน;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ถ้า (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    มิฉะนั้น ถ้า (slang->sl_nobreak)
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
    จำนวนเต็ม		len,	    // word length, -1 for up to NUL
    จำนวนเต็ม		count)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ถ้า (len == -1)
	p = word;
    มิฉะนั้น ถ้า (len >= MAXWLEN)
	คืน;
    มิฉะนั้น
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ถ้า (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	ถ้า (wc == NULL)
	    คืน;
	STRCPY(wc->wc_word, p);
	wc->wc_count = count;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    มิฉะนั้น
    {
	wc = HI2WC(hi);
	ถ้า ((wc->wc_count += count) < (unsigned)count)	// check for overflow
	    wc->wc_count = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    จำนวนเต็ม
byte_in_str(char_u *str, จำนวนเต็ม n)
{
    char_u	*p;

    สำหรับ (p = str; *p != NUL; ++p)
	ถ้า (*p == n)
	    คืน TRUE;
    คืน FALSE;
}

#define SY_MAXLEN   30
typedef sa_nracanaa syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    จำนวนเต็ม		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    จำนวนเต็ม
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    จำนวนเต็ม		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_items, parimaa_na(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ขณะ (p != NULL)
    {
	*p++ = NUL;
	ถ้า (*p == NUL)	    // trailing slash
	    bha_nga;
	s = p;
	p = vim_strchr(p, '/');
	ถ้า (p == NULL)
	    l = (จำนวนเต็ม)STRLEN(s);
	มิฉะนั้น
	    l = (จำนวนเต็ม)(p - s);
	ถ้า (l >= SY_MAXLEN)
	    คืน SP_FORMERROR;
	ถ้า (ga_grow(&slang->sl_syl_items, 1) == FAIL)
	    คืน SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_items.ga_data)
					       + slang->sl_syl_items.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    คืน OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    static จำนวนเต็ม
count_syllables(slang_T *slang, char_u *word)
{
    จำนวนเต็ม		cnt = 0;
    จำนวนเต็ม		skip = FALSE;
    char_u	*p;
    จำนวนเต็ม		len;
    จำนวนเต็ม		i;
    syl_item_T	*syl;
    จำนวนเต็ม		c;

    ถ้า (slang->sl_syllable == NULL)
	คืน 0;

    สำหรับ (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ถ้า (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    continue;
	}

	// Find longest match of syllable items.
	len = 0;
	สำหรับ (i = 0; i < slang->sl_syl_items.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_items.ga_data) + i;
	    ถ้า (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ถ้า (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	มิฉะนั้น
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ถ้า (vim_strchr(slang->sl_syllable, c) == NULL)
		skip = FALSE;	    // No, search for next syllable
	    มิฉะนั้น ถ้า (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    คืน cnt;
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
    จำนวนเต็ม		filename;
    จำนวนเต็ม		region_mask;
    slang_T	*slang;
    จำนวนเต็ม		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    จำนวนเต็ม		len;
    char_u	*p;
    จำนวนเต็ม		round;
    char_u	*spf;
    char_u	*use_region = NULL;
    จำนวนเต็ม		dont_use_region = FALSE;
    จำนวนเต็ม		nobreak = FALSE;
    จำนวนเต็ม		i, j;
    langp_T	*lp, *lp2;
    static จำนวนเต็ม	recursive = FALSE;
    var_na	*ret_msg = NULL;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ถ้า (recursive)
	คืน NULL;
    recursive = TRUE;

    ga_init2(&ga, parimaa_na(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ถ้า (spl_copy == NULL)
	goto theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    สำหรับ (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = NULL;

	ถ้า (!valid_spelllang(lang))
	    continue;

	ถ้า (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    continue;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ถ้า (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ถ้า (p != NULL && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    มิฉะนั้น
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ถ้า (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    bha_nga;
	}
	มิฉะนั้น
	{
	    filename = FALSE;
	    ถ้า (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    มิฉะนั้น
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ถ้า (STRICMP(lang, slang->sl_name) == 0)
		    bha_nga;
	}

	ถ้า (region != NULL)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ถ้า (use_region != NULL && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ถ้า (slang == NULL)
	{
	    ถ้า (filename)
		(void)spell_load_file(lang, lang, NULL, FALSE);
	    มิฉะนั้น
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ถ้า (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
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
	    ถ้า (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		ถ้า (!filename && region != NULL)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    ถ้า (c == REGION_ALL)
		    {
			ถ้า (slang->sl_add)
			{
			    ถ้า (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			มิฉะนั้น
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    มิฉะนั้น
			region_mask = 1 << c;
		}

		ถ้า (region_mask != 0)
		{
		    ถ้า (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_msg = e_out_of_memory;
			goto theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ถ้า (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    สำหรับ (round = 0; round == 0 || *spf != NUL; ++round)
    {
	ถ้า (round == 0)
	{
	    // Internal wordlist, if there is one.
	    ถ้า (int_wordlist == NULL)
		continue;
	    int_wordlist_spl(spf_name);
	}
	มิฉะนั้น
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    สำหรับ (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		ถ้า (p != NULL && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    bha_nga;
	    }
	    ถ้า (c < ga.ga_len)
		continue;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ถ้า (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		bha_nga;
	ถ้า (slang == NULL)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ถ้า (round == 0)
		STRCPY(lang, "internal wordlist");
	    มิฉะนั้น
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ถ้า (p != NULL)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, NULL, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ถ้า (slang != NULL && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ถ้า (slang != NULL && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    ถ้า (use_region != NULL && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		ถ้า (c != REGION_ALL)
		    region_mask = 1 << c;
		มิฉะนั้น ถ้า (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    ถ้า (region_mask != 0)
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
    สำหรับ (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	ถ้า (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	มิฉะนั้น
	    // find first similar language that does sound folding
	    สำหรับ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ถ้า (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    bha_nga;
		}
	    }

	// REP items
	ถ้า (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	มิฉะนั้น
	    // find first similar language that has REP items
	    สำหรับ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ถ้า (lp2->lp_slang->sl_rep.ga_len > 0
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
    คืน ret_msg;
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

    ถ้า (lp->sl_midword == NULL)	    // there aren't any
	คืน;

    สำหรับ (p = lp->sl_midword; *p != NUL; )
	ถ้า (has_mbyte)
	{
	    จำนวนเต็ม	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ถ้า (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    มิฉะนั้น ถ้า (wp->w_s->b_spell_ismw_mb == NULL)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    มิฉะนั้น
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (จำนวนเต็ม)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ถ้า (bp != NULL)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	มิฉะนั้น
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    static จำนวนเต็ม
find_region(char_u *rp, char_u *region)
{
    จำนวนเต็ม		i;

    สำหรับ (i = 0; ; i += 2)
    {
	ถ้า (rp[i] == NUL)
	    คืน REGION_ALL;
	ถ้า (rp[i] == region[0] && rp[i + 1] == region[1])
	    bha_nga;
    }
    คืน i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    จำนวนเต็ม
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    จำนวนเต็ม		c;
    จำนวนเต็ม		firstcap;
    จำนวนเต็ม		allcap;
    จำนวนเต็ม		past_second = FALSE;	// past second word char

    // find first letter
    สำหรับ (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ถ้า (end == NULL ? *p == NUL : p >= end)
	    คืน 0;	    // only non-word characters, illegal word
    ถ้า (has_mbyte)
	c = mb_ptr2char_adv(&p);
    มิฉะนั้น
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    สำหรับ ( ; end == NULL ? *p != NUL : p < end; MB_PTR_ADV(p))
	ถ้า (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ถ้า (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ถ้า (past_second && allcap)
		    คืน WF_KEEPCAP;
		allcap = FALSE;
	    }
	    มิฉะนั้น ถ้า (!allcap)
		// UlU -> KEEPCAP
		คืน WF_KEEPCAP;
	    past_second = TRUE;
	}

    ถ้า (allcap)
	คืน WF_ALLCAP;
    ถ้า (firstcap)
	คืน WF_ONECAP;
    คืน 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    void
spell_delete_wordlist(void)
{
    char_u	fname[MAXPATHL];

    ถ้า (int_wordlist == NULL)
	คืน;

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

    ขณะ (first_lang != NULL)
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
	ถ้า (*wp->w_s->b_p_spl != NUL)
	{
		ถ้า (wp->w_p_spell)
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
    ถ้า (buf == NULL)
	คืน NULL;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_key = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    คืน buf;
}

/*
 * Close the buffer used for spell info.
 */
    void
close_spellbuf(buf_T *buf)
{
    ถ้า (buf == NULL)
	คืน;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    void
clear_spell_chartab(spelltab_T *sp)
{
    จำนวนเต็ม		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    สำหรับ (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    สำหรับ (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    สำหรับ (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    สำหรับ (i = 'a'; i <= 'z'; ++i)
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
    จำนวนเต็ม	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ถ้า (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	สำหรับ (i = 128; i <= 255; ++i)
	    ถ้า (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    มิฉะนั้น ถ้า (enc_utf8)
    {
	สำหรับ (i = 128; i < 256; ++i)
	{
	    จำนวนเต็ม f = utf_fold(i);
	    จำนวนเต็ม u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    มิฉะนั้น
    {
	// Rough guess: use locale-dependent library functions.
	สำหรับ (i = 128; i < 256; ++i)
	{
	    ถ้า (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    มิฉะนั้น ถ้า (MB_ISLOWER(i))
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
    จำนวนเต็ม
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    จำนวนเต็ม		l;
    จำนวนเต็ม		c;

    ถ้า (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ถ้า (l == 1)
	{
	    // be quick for ASCII
	    ถ้า (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	มิฉะนั้น
	{
	    c = mb_ptr2char(p);
	    ถ้า (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != NULL
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != NULL))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ถ้า (c > 255)
	    คืน spell_mb_isword_class(mb_get_class(s), wp);
	คืน spelltab.st_isw[c];
    }

    คืน spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    จำนวนเต็ม
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    จำนวนเต็ม		c;

    ถ้า (has_mbyte)
    {
	c = mb_ptr2char(p);
	ถ้า (c > 255)
	    คืน spell_mb_isword_class(mb_get_class(p), wp);
	คืน spelltab.st_isw[c];
    }
    คืน spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    static จำนวนเต็ม
spell_mb_isword_class(จำนวนเต็ม cl, win_T *wp)
{
    ถ้า (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	คืน cl == 2 || cl == 0x2800;
    คืน cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    static จำนวนเต็ม
spell_iswordp_w(จำนวนเต็ม *p, win_T *wp)
{
    จำนวนเต็ม		*s;

    ถ้า (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != NULL
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != NULL))
	s = p + 1;
    มิฉะนั้น
	s = p;

    ถ้า (*s > 255)
    {
	ถ้า (enc_utf8)
	    คืน spell_mb_isword_class(utf_class(*s), wp);
	ถ้า (enc_dbcs)
	    คืน spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	คืน 0;
    }
    คืน spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    จำนวนเต็ม
spell_casefold(
    win_T	*wp,
    char_u	*str,
    จำนวนเต็ม		len,
    char_u	*buf,
    จำนวนเต็ม		buflen)
{
    จำนวนเต็ม		i;

    ถ้า (len >= buflen)
    {
	buf[0] = NUL;
	คืน FAIL;		// result will not fit
    }

    ถ้า (has_mbyte)
    {
	จำนวนเต็ม	outi = 0;
	char_u	*p;
	จำนวนเต็ม	c;

	// Fold one character at a time.
	สำหรับ (p = str; p < str + len; )
	{
	    ถ้า (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		คืน FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ถ้า (c == 0x03a3 || c == 0x03c2)
	    {
		ถ้า (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		มิฉะนั้น
		    c = 0x03c3;
	    }
	    มิฉะนั้น
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    มิฉะนั้น
    {
	// Be quick for non-multibyte encodings.
	สำหรับ (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    คืน OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    จำนวนเต็ม
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    ถ้า (wp->w_s->b_cap_prog == NULL)
	คืน FALSE;

    จำนวนเต็ม		need_cap = FALSE;
    char_u	*line = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : NULL;
    char_u	*line_copy = NULL;
    colnr_T	endcol = 0;

    ถ้า (col == 0 || getwhitecols(line) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ถ้า (lnum == 1)
	    need_cap = TRUE;
	มิฉะนั้น
	{
	    line = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    ถ้า (*skipwhite(line) == NUL)
		need_cap = TRUE;
	    มิฉะนั้น
	    {
		// Append a space in place of the line break.
		line_copy = concat_str(line, (char_u *)" ");
		ถ้า (line_copy == NULL)
		    คืน FALSE;

		line = line_copy;
		endcol = (colnr_T)STRLEN(line);
	    }
	}
    }
    มิฉะนั้น
	endcol = col;

    ถ้า (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = line + endcol;
	สำหรับ (;;)
	{
	    MB_PTR_BACK(line, p);
	    ถ้า (p == line || spell_iswordp_nmw(p, wp))
		bha_nga;
	    ถ้า (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == line + endcol)
	    {
		need_cap = TRUE;
		bha_nga;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(line_copy);

    คืน need_cap;
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
    จำนวนเต็ม		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ถ้า (repl_from == NULL || repl_to == NULL)
    {
	emsg(_(e_no_previous_spell_replacement));
	คืน;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ถ้า (frompat == NULL)
	คืน;
    frompatlen = vim_snprintf((var_na *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ขณะ (!got_int)
    {
	ถ้า (do_search(NULL, '/', '/', frompat, frompatlen, 1L, SEARCH_KEEP, NULL) == 0
						   || u_save_cursor() == FAIL)
	    bha_nga;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	line = ml_get_curline();
	ถ้า (addlen <= 0 || STRNCMP(line + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ถ้า (p == NULL)
		bha_nga;
	    mch_memmove(p, line, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, line + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#ถ้า defined(FEAT_PROP_POPUP)
	    ถ้า (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (จำนวนเต็ม)addlen, APC_SUBSTITUTE);
#endif

	    ถ้า (curwin->w_cursor.lnum != prev_lnum)
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

    ถ้า (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    มิฉะนั้น
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
    จำนวนเต็ม		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    จำนวนเต็ม		c;
    จำนวนเต็ม		l;

    p = word;
    ถ้า (has_mbyte)
	c = mb_cptr2char_adv(&p);
    มิฉะนั้น
	c = *p++;
    ถ้า (upper)
	c = SPELL_TOUPPER(c);
    มิฉะนั้น
	c = SPELL_TOFOLD(c);
    ถ้า (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    มิฉะนั้น
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
    จำนวนเต็ม		c;

    d = wcopy;
    สำหรับ (s = word; *s != NUL; )
    {
	ถ้า (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	มิฉะนั้น
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ถ้า (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ถ้า (d - wcopy >= MAXWLEN - 1)
		bha_nga;
	    *d++ = c;
	}
	มิฉะนั้น
	    c = SPELL_TOUPPER(c);

	ถ้า (has_mbyte)
	{
	    ถ้า (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		bha_nga;
	    d += mb_char2bytes(c, d);
	}
	มิฉะนั้น
	{
	    ถ้า (d - wcopy >= MAXWLEN - 1)
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
    จำนวนเต็ม
nofold_len(char_u *fword, จำนวนเต็ม flen, char_u *word)
{
    char_u	*p;
    จำนวนเต็ม		i = 0;

    สำหรับ (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    สำหรับ (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    คืน (จำนวนเต็ม)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    void
make_case_word(char_u *fword, char_u *cword, จำนวนเต็ม flags)
{
    ถ้า (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    มิฉะนั้น ถ้า (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    มิฉะนั้น
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ถ้า defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    จำนวนเต็ม		lpi;

    ถ้า (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	สำหรับ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    ถ้า (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		คืน vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    คืน vim_strsave(word);
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
    จำนวนเต็ม		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    ถ้า (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    มิฉะนั้น
    {
	// SAL items used.  Requires the word to be case-folded.
	ถ้า (folded)
	    word = inword;
	มิฉะนั้น
	{
	    (void)spell_casefold(curwin,
				  inword, (จำนวนเต็ม)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	ถ้า (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	มิฉะนั้น
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
    จำนวนเต็ม		ri = 0;
    จำนวนเต็ม		c;

    ถ้า (has_mbyte)
    {
	จำนวนเต็ม	prevc = 0;
	จำนวนเต็ม	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	สำหรับ (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ถ้า (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    มิฉะนั้น ถ้า (c < 256)
		c = slang->sl_sal_first[c];
	    มิฉะนั้น
	    {
		ip = ((จำนวนเต็ม **)slang->sl_sal.ga_data)[c & 0xff];
		ถ้า (ip == NULL)		// empty list, can't match
		    c = NUL;
		มิฉะนั้น
		    สำหรับ (;;)		// find "c" in the list
		    {
			ถ้า (*ip == 0)	// not found
			{
			    c = NUL;
			    bha_nga;
			}
			ถ้า (*ip == c)	// match!
			{
			    c = ip[1];
			    bha_nga;
			}
			ip += 2;
		    }
	    }

	    ถ้า (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ถ้า (ri + MB_MAXBYTES > MAXWLEN)
		    bha_nga;
		prevc = c;
	    }
	}
    }
    มิฉะนั้น
    {
	// The sl_sal_first[] table contains the translation.
	สำหรับ (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ถ้า (VIM_ISWHITE(c))
		c = ' ';
	    มิฉะนั้น
		c = slang->sl_sal_first[c];
	    ถ้า (c != NUL && (ri == 0 || res[ri - 1] != c))
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
    จำนวนเต็ม		i, j, z;
    จำนวนเต็ม		reslen;
    จำนวนเต็ม		n, k = 0;
    จำนวนเต็ม		z0;
    จำนวนเต็ม		k0;
    จำนวนเต็ม		n0;
    จำนวนเต็ม		c;
    จำนวนเต็ม		pri;
    จำนวนเต็ม		p0 = -333;
    จำนวนเต็ม		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ถ้า (slang->sl_rem_accents)
    {
	t = word;
	ขณะ (*s != NUL)
	{
	    ถ้า (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    มิฉะนั้น
	    {
		ถ้า (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    มิฉะนั้น
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ขณะ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	ถ้า (n >= 0)
	{
	    // check all rules for the same letter
	    สำหรับ (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ถ้า (k > 1)
		{
		    ถ้า (word[i + 1] != s[1])
			continue;
		    ถ้า (k > 2)
		    {
			สำหรับ (j = 2; j < k; ++j)
			    ถ้า (word[i + j] != s[j])
				bha_nga;
			ถ้า (j < k)
			    continue;
		    }
		}

		ถ้า ((pf = smp[n].sm_oneof) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ขณะ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ถ้า (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ขณะ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ถ้า (*s == '<')
		    s++;
		ถ้า (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ถ้า (*s == '^' && *(s + 1) == '^')
		    s++;

		ถ้า (*s == NUL
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

		    ถ้า (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			สำหรับ ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ถ้า (k0 > 1)
			    {
				ถ้า (word[i + k] != s[1])
				    continue;
				ถ้า (k0 > 2)
				{
				    pf = word + i + k + 1;
				    สำหรับ (j = 2; j < k0; ++j)
					ถ้า (*pf++ != s[j])
					    bha_nga;
				    ถ้า (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    ถ้า ((pf = smp[n0].sm_oneof) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ขณะ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ถ้า (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ขณะ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ถ้า (*s == '<')
				s++;
			    ถ้า (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ถ้า (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				ถ้า (k0 == k)
				    // this is just a piece of the string
				    continue;

				ถ้า (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				bha_nga;
			    }
			}

			ถ้า (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    continue;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    ถ้า (s == NULL)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != NULL) ? 1 : 0;
		    ถ้า (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ถ้า (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ขณะ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			ถ้า (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    มิฉะนั้น
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ขณะ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ถ้า (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ถ้า (strstr((var_na *)pf, "^^") != NULL)
			{
			    ถ้า (c != NUL && reslen < MAXWLEN - 1)
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
	มิฉะนั้น ถ้า (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ถ้า (z0 == 0)
	{
	    ถ้า (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    จำนวนเต็ม		word[MAXWLEN];
    จำนวนเต็ม		wres[MAXWLEN];
    จำนวนเต็ม		l;
    char_u	*s;
    จำนวนเต็ม		*ws;
    char_u	*t;
    จำนวนเต็ม		*pf;
    จำนวนเต็ม		i, j, z;
    จำนวนเต็ม		reslen;
    จำนวนเต็ม		n, k = 0;
    จำนวนเต็ม		z0;
    จำนวนเต็ม		k0;
    จำนวนเต็ม		n0;
    จำนวนเต็ม		c;
    จำนวนเต็ม		pri;
    จำนวนเต็ม		p0 = -333;
    จำนวนเต็ม		c0;
    จำนวนเต็ม		did_white = FALSE;
    จำนวนเต็ม		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    สำหรับ (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ถ้า (slang->sl_rem_accents)
	{
	    ถ้า (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ถ้า (did_white)
		    continue;
		c = ' ';
		did_white = TRUE;
	    }
	    มิฉะนั้น
	    {
		did_white = FALSE;
		ถ้า (!spell_iswordp_nmw(t, curwin))
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
    ขณะ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	ถ้า (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    สำหรับ (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ถ้า (c != ws[0])
		    continue;
		k = smp[n].sm_leadlen;
		ถ้า (k > 1)
		{
		    ถ้า (word[i + 1] != ws[1])
			continue;
		    ถ้า (k > 2)
		    {
			สำหรับ (j = 2; j < k; ++j)
			    ถ้า (word[i + j] != ws[j])
				bha_nga;
			ถ้า (j < k)
			    continue;
		    }
		}

		ถ้า ((pf = smp[n].sm_oneof_w) != NULL)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ขณะ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ถ้า (*pf == NUL)
			continue;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ขณะ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ถ้า (*s == '<')
		    s++;
		ถ้า (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ถ้า (*s == '^' && *(s + 1) == '^')
		    s++;

		ถ้า (*s == NUL
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

		    ถ้า (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			สำหรับ ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ถ้า (c0 != ws[0])
				continue;
			    k0 = smp[n0].sm_leadlen;
			    ถ้า (k0 > 1)
			    {
				ถ้า (word[i + k] != ws[1])
				    continue;
				ถ้า (k0 > 2)
				{
				    pf = word + i + k + 1;
				    สำหรับ (j = 2; j < k0; ++j)
					ถ้า (*pf++ != ws[j])
					    bha_nga;
				    ถ้า (j < k0)
					continue;
				}
			    }
			    k0 += k - 1;

			    ถ้า ((pf = smp[n0].sm_oneof_w) != NULL)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ขณะ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ถ้า (*pf == NUL)
				    continue;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ขณะ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ถ้า (*s == '<')
				s++;
			    ถ้า (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ถ้า (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				ถ้า (k0 == k)
				    // this is just a piece of the string
				    continue;

				ถ้า (p0 < pri)
				    // priority too low
				    continue;
				// rule fits; stop search
				bha_nga;
			    }
			}

			ถ้า (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    continue;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != NULL) ? 1 : 0;
		    ถ้า (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ถ้า (reslen > 0 && ws != NULL && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ถ้า (ws != NULL)
			    ขณะ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ถ้า (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    parimaa_na(จำนวนเต็ม) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    มิฉะนั้น
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ถ้า (ws != NULL)
			    ขณะ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ถ้า (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ถ้า (ws == NULL)
			    c = NUL;
			มิฉะนั้น
			    c = *ws;
			ถ้า (strstr((var_na *)s, "^^") != NULL)
			{
			    ถ้า (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       parimaa_na(จำนวนเต็ม) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    bha_nga;
		}
	    }
	}
	มิฉะนั้น ถ้า (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ถ้า (z0 == 0)
	{
	    ถ้า (k && !p0 && reslen < MAXWLEN && c != NUL
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
    สำหรับ (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ถ้า (l + MB_MAXBYTES > MAXWLEN)
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
    จำนวนเต็ม		lpi;
    langp_T	*lp;
    char_u	*p;

    ถ้า (no_spell_checking(curwin))
	คืน;

    msg_start();
    สำหรับ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	msg_puts("file: ");
	msg_puts((var_na *)lp->lp_slang->sl_fname);
	msg_putchar('\n');
	p = lp->lp_slang->sl_info;
	ถ้า (p != NULL)
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

    ถ้า (no_spell_checking(curwin))
	คืน;
    (void)get_option_value((char_u*)"spl", &dummy, &spl, NULL, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_value_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_value_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ถ้า (!BUFEMPTY())
	คืน;

    spell_dump_compl(NULL, 0, NULL, eap->forceit ? DUMPFLAG_COUNT : 0);

    // Delete the empty line that we started with.
    ถ้า (curbuf->b_ml.ml_line_count > 1)
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
    จำนวนเต็ม		ic,	    // ignore case
    จำนวนเต็ม		*dir,	    // direction for adding matches
    จำนวนเต็ม		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    จำนวนเต็ม		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    จำนวนเต็ม		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    จำนวนเต็ม		round;
    จำนวนเต็ม		depth;
    จำนวนเต็ม		n;
    จำนวนเต็ม		flags;
    char_u	*region_names = NULL;	    // region names being used
    จำนวนเต็ม		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    จำนวนเต็ม		lpi;
    จำนวนเต็ม		dumpflags = dumpflags_arg;
    จำนวนเต็ม		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ถ้า (pat != NULL)
    {
	ถ้า (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	มิฉะนั้น
	{
	    n = captype(pat, NULL);
	    ถ้า (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    มิฉะนั้น ถ้า (n == WF_ALLCAP && (จำนวนเต็ม)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    สำหรับ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ถ้า (p[0] != 0)
	{
	    ถ้า (region_names == NULL)	    // first language with regions
		region_names = p;
	    มิฉะนั้น ถ้า (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		bha_nga;
	    }
	}
    }

    ถ้า (do_region && region_names != NULL && pat == NULL)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    มิฉะนั้น
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    สำหรับ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ถ้า (slang->sl_fbyts == NULL)	    // reloading failed
	    continue;

	ถ้า (pat == NULL)
	{
	    vim_snprintf((var_na *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ถ้า (pat != NULL && slang->sl_pbyts == NULL)
	    patlen = (จำนวนเต็ม)STRLEN(pat);
	มิฉะนั้น
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	สำหรับ (round = 1; round <= 2; ++round)
	{
	    ถ้า (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    มิฉะนั้น
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ถ้า (byts == NULL)
		continue;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ขณะ (depth >= 0 && !got_int
				  && (pat == NULL || !ins_compl_interrupted()))
	    {
		ถ้า (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    line_breakcheck();
		    ins_compl_check_keys(50, FALSE);
		}
		มิฉะนั้น
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    ถ้า (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (จำนวนเต็ม)idxs[n];
			ถ้า ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    ถ้า (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    ถ้า (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				ถ้า (pat == NULL)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ถ้า (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    มิฉะนั้น
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
			ถ้า (depth <= patlen
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
    จำนวนเต็ม		*dir,
    จำนวนเต็ม		dumpflags,
    จำนวนเต็ม		wordflags,
    linenr_T	lnum)
{
    จำนวนเต็ม		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    จำนวนเต็ม		i;
    จำนวนเต็ม		flags = wordflags;

    ถ้า (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ถ้า (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ถ้า ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    มิฉะนั้น
    {
	p = word;
	ถ้า ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, NULL) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ถ้า (pat == NULL)
    {
	// Add flags and regions after a slash.
	ถ้า ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ถ้า (keepcap)
		STRCAT(badword, "=");
	    ถ้า (flags & WF_BANNED)
		STRCAT(badword, "!");
	    มิฉะนั้น ถ้า (flags & WF_RARE)
		STRCAT(badword, "?");
	    ถ้า (flags & WF_REGION)
		สำหรับ (i = 0; i < 7; ++i)
		    ถ้า (flags & (0x10000 << i))
			sprintf((var_na *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ถ้า (dumpflags & DUMPFLAG_COUNT)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    ถ้า (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((var_na *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_count);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    มิฉะนั้น ถ้า (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (จำนวนเต็ม)STRLEN(p),
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
    จำนวนเต็ม		*dir,
    จำนวนเต็ม		dumpflags,
    จำนวนเต็ม		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    จำนวนเต็ม		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    จำนวนเต็ม		has_word_up = FALSE;
    จำนวนเต็ม		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    จำนวนเต็ม		depth;
    จำนวนเต็ม		n;
    จำนวนเต็ม		len;
    จำนวนเต็ม		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    ถ้า (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ถ้า (byts != NULL)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ขณะ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    ถ้า (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		line_breakcheck();
	    }
	    มิฉะนั้น
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		ถ้า (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    สำหรับ (i = 1; i < len; ++i)
			ถ้า (byts[n + i] != 0)
			    bha_nga;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    ถ้า (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ถ้า (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ถ้า (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			ถ้า (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ถ้า (lnum != 0)
				++lnum;
			}
		    }
		}
		มิฉะนั้น ถ้า (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    คืน lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    ขณะ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    คืน p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    จำนวนเต็ม
spell_word_start(จำนวนเต็ม startcol)
{
    char_u	*line;
    char_u	*p;
    จำนวนเต็ม		col = 0;

    ถ้า (no_spell_checking(curwin))
	คืน startcol;

    // Find a word character before "startcol".
    line = ml_get_curline();
    สำหรับ (p = line + startcol; p > line; )
    {
	MB_PTR_BACK(line, p);
	ถ้า (spell_iswordp_nmw(p, curwin))
	    bha_nga;
    }

    // Go back to start of the word.
    ขณะ (p > line)
    {
	col = (จำนวนเต็ม)(p - line);
	MB_PTR_BACK(line, p);
	ถ้า (!spell_iswordp(p, curwin))
	    bha_nga;
	col = 0;
    }

    คืน col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
static จำนวนเต็ม spell_expand_need_cap;

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
    จำนวนเต็ม
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    คืน ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    จำนวนเต็ม
valid_spelllang(char_u *val)
{
    คืน valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    จำนวนเต็ม
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    จำนวนเต็ม		l;

    spf = val;
    ขณะ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ถ้า (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    คืน FALSE;
	สำหรับ (s = spf_name; *s != NUL; ++s)
	    ถ้า (!vim_is_fname_char(*s))
		คืน FALSE;
    }
    คืน TRUE;
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
	ถ้า (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    bha_nga;
	}
    คืน errmsg;
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

    ถ้า (synblock->b_p_spc == NULL || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = NULL;
    มิฉะนั้น
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ถ้า (re != NULL)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ถ้า (synblock->b_cap_prog == NULL)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		คืน e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    คืน NULL;
}

#endif  // FEAT_SPELL
