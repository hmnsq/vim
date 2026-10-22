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
#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_SPELL)

#অন্তর্ভাৱ <time.h>

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
প্রকারনির্দেশ সংরচনা matchinf_S
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
    পূর্ণাঙ্ক		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    পূর্ণাঙ্ক		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    পূর্ণাঙ্ক		mi_prefcnt;		// number of entries at mi_prefarridx
    পূর্ণাঙ্ক		mi_prefixlen;		// byte length of prefix
    পূর্ণাঙ্ক		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    পূর্ণাঙ্ক		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    পূর্ণাঙ্ক		mi_complen;		// nr of compound words used
    পূর্ণাঙ্ক		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    পূর্ণাঙ্ক		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    পূর্ণাঙ্ক		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    পূর্ণাঙ্ক		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


স্থির পূর্ণাঙ্ক spell_mb_isword_class(পূর্ণাঙ্ক cl, win_T *wp);

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

স্থির শূন্য find_word(matchinf_T *mip, পূর্ণাঙ্ক mode);
স্থির শূন্য find_prefix(matchinf_T *mip, পূর্ণাঙ্ক mode);
স্থির পূর্ণাঙ্ক fold_more(matchinf_T *mip);
স্থির শূন্য spell_load_cb(char_u *fname, শূন্য *cookie);
স্থির পূর্ণাঙ্ক গুনতি_syllables(slang_T *slang, char_u *word);
স্থির শূন্য clear_midword(win_T *buf);
স্থির শূন্য use_midword(slang_T *lp, win_T *buf);
স্থির পূর্ণাঙ্ক find_region(char_u *rp, char_u *region);
স্থির শূন্য spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
স্থির শূন্য spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
স্থির শূন্য spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
স্থির শূন্য dump_word(slang_T *slang, char_u *word, char_u *pat, পূর্ণাঙ্ক *dir, পূর্ণাঙ্ক round, পূর্ণাঙ্ক flags, linenr_T lnum);
স্থির linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, পূর্ণাঙ্ক *dir, পূর্ণাঙ্ক round, পূর্ণাঙ্ক flags, linenr_T startlnum);
স্থির char_u *advance_camelcase_word(char_u *p, win_T *wp, পূর্ণাঙ্ক *is_camel_case);

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
    পূর্ণাঙ্ক
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    পূর্ণাঙ্ক		*capcol,	// column to check for Capital
    পূর্ণাঙ্ক		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    পূর্ণাঙ্ক		nrlen = 0;	// found a number first
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		wrongcaplen = 0;
    পূর্ণাঙ্ক		lpi;
    পূর্ণাঙ্ক		গুনতি_word = docount;
    পূর্ণাঙ্ক		use_camel_case = *wp->w_s->b_p_spo != NUL;
    পূর্ণাঙ্ক		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    যদি (*ptr <= ' ')
	ঘূৰাই 1;

    // Return here when loading language files failed.
    যদি (wp->w_s->b_langp.ga_len == 0)
	ঘূৰাই 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    যদি (*ptr >= '0' && *ptr <= '9')
    {
	যদি (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	অন্যথা যদি (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	অন্যথা
	    mi.mi_end = skipdigits(ptr);
	nrlen = (পূর্ণাঙ্ক)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    যদি (spell_iswordp(mi.mi_fend, wp))
    {
	যদি (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	অন্যথা
	{
	    কৰা
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } যেতিয়ালৈ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	যদি (capcol != রিক্ত && *capcol == 0 && wp->w_s->b_cap_prog != রিক্ত)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    যদি (!SPELL_ISUPPER(c))
		wrongcaplen = (পূর্ণাঙ্ক)(mi.mi_fend - ptr);
	}
    }
    যদি (capcol != রিক্ত)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = রিক্ত;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    যদি (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (শূন্য)spell_casefold(wp, ptr, (পূর্ণাঙ্ক)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (পূর্ণাঙ্ক)STRLEN(mi.mi_fword);

    যদি (is_camel_case && mi.mi_fwordlen > 0)
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
    বাবে (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	যদি (mi.mi_lp->lp_slang->sl_fidxs == রিক্ত)
	    অনুৱর্ত;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	যদি (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	যদি (গুনতি_word && mi.mi_result == SP_OK)
	{
	    গুনতি_common_word(mi.mi_lp->lp_slang, ptr,
						   (পূর্ণাঙ্ক)(mi.mi_end - ptr), 1);
	    গুনতি_word = FALSE;
	}
    }

    যদি (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	যদি (nrlen > 0)
	{
	    যদি (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		ঘূৰাই nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	অন্যথা যদি (!spell_iswordp_nmw(ptr, wp))
	{
	    যদি (capcol != রিক্ত && wp->w_s->b_cap_prog != রিক্ত)
	    {
		regmatch_T	regmatch;
		পূর্ণাঙ্ক		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		যদি (r)
		    *capcol = (পূর্ণাঙ্ক)(regmatch.endp[0] - ptr);
	    }

	    যদি (has_mbyte)
		ঘূৰাই (*mb_ptr2len)(ptr);
	    ঘূৰাই 1;
	}
	অন্যথা যদি (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	অন্যথা যদি (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    পূর্ণাঙ্ক		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    যদি (mi.mi_lp->lp_slang->sl_fidxs != রিক্ত)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		বাবে (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    যদি (p >= mi.mi_end)
			ভঙ্গ;
		    mi.mi_compoff = (পূর্ণাঙ্ক)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    যদি (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			ভঙ্গ;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	যদি (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	অন্যথা যদি (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	অন্যথা
	    *attrp = HLF_SPL;
    }

    যদি (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ঘূৰাই wrongcaplen;
    }

    ঘূৰাই (পূর্ণাঙ্ক)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    স্থির পূর্ণাঙ্ক
get_char_type(পূর্ণাঙ্ক c)
{
    যদি (VIM_ISDIGIT(c))
	ঘূৰাই CHAR_DIGIT;
    যদি (SPELL_ISUPPER(c))
	ঘূৰাই CHAR_UPPER;
    ঘূৰাই CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    স্থির char_u *
advance_camelcase_word(char_u *str, win_T *wp, পূর্ণাঙ্ক *is_camel_case)
{
    পূর্ণাঙ্ক last_type, last_last_type, this_type;
    পূর্ণাঙ্ক c;
    char_u *end = str;

    *is_camel_case = FALSE;

    যদি (*str == NUL)
	ঘূৰাই str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    যেতিয়ালৈ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	যদি (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    ভঙ্গ;
	}
	অন্যথা যদি ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    ভঙ্গ;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    ঘূৰাই end;
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
    স্থির শূন্য
find_word(matchinf_T *mip, পূর্ণাঙ্ক mode)
{
    idx_T	arridx = 0;
    পূর্ণাঙ্ক		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    পূর্ণাঙ্ক		endidxcnt = 0;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		wlen = 0;
    পূর্ণাঙ্ক		flen;
    পূর্ণাঙ্ক		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    পূর্ণাঙ্ক		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    পূর্ণাঙ্ক		word_ends;
    পূর্ণাঙ্ক		prefix_found;
    পূর্ণাঙ্ক		nobreak_result;

    যদি (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	যদি (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    অন্যথা
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	যদি (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	অন্যথা যদি (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    যদি (byts == রিক্ত)
	ঘূৰাই;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    বাবে (;;)
    {
	যদি (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	যদি (byts[arridx] == 0)
	{
	    যদি (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		ঘূৰাই;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    যেতিয়ালৈ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    যদি (len == 0)
		ভঙ্গ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	যদি (ptr[wlen] == NUL)
	    ভঙ্গ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	যদি (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	যেতিয়ালৈ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    যদি (byts[m] > c)
		hi = m - 1;
	    অন্যথা যদি (byts[m] < c)
		lo = m + 1;
	    অন্যথা
	    {
		lo = hi = m;
		ভঙ্গ;
	    }
	}

	// Stop if there is no matching byte.
	যদি (hi < lo || byts[lo] != c)
	    ভঙ্গ;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	যদি (c == ' ')
	{
	    বাবে (;;)
	    {
		যদি (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		যদি (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    ভঙ্গ;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    যেতিয়ালৈ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	যদি ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    অনুৱর্ত;	    // not at first byte of character
	যদি (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    যদি (slang->sl_compprog == রিক্ত && !slang->sl_nobreak)
		অনুৱর্ত;	    // next char is a word character
	    word_ends = FALSE;
	}
	অন্যথা
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	যদি (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    যদি (STRNCMP(ptr, p, wlen) != 0)
	    {
		বাবে (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (পূর্ণাঙ্ক)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	বাবে (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    যদি (mode == FIND_FOLDWORD)
	    {
		যদি (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		যদি (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    অনুৱর্ত;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    অন্যথা যদি (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		যদি (c == 0)
		    অনুৱর্ত;

		// Use the WF_RARE flag for a rare prefix.
		যদি (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    যদি (slang->sl_nobreak)
	    {
		যদি ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    ভঙ্গ;
		}
	    }

	    অন্যথা যদি ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		যদি (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    অনুৱর্ত;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		যদি (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			অনুৱর্ত;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		যদি (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    অনুৱর্ত;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		যদি (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    অনুৱর্ত;
		যদি (!word_ends && (flags & WF_NOCOMPAFT))
		    অনুৱর্ত;

		// Quickly check if compounding is possible with this flag.
		যদি (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    অনুৱর্ত;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		যদি (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    অনুৱর্ত;

		যদি (mode == FIND_COMPOUND)
		{
		    পূর্ণাঙ্ক	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    যদি (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			বাবে (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    অন্যথা
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    যদি (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			অনুৱর্ত;

		    যদি (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			যদি (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    অনুৱর্ত;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		যদি (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    যদি (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			যদি (ptr == mip->mi_word)
			    (শূন্য)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			অন্যথা
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    যদি (!can_compound(slang, fword, mip->mi_compflags))
			অনুৱর্ত;
		}
		অন্যথা যদি (slang->sl_comprules != রিক্ত
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    অনুৱর্ত;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    অন্যথা যদি (flags & WF_NEEDCOMP)
		অনুৱর্ত;

	    nobreak_result = SP_OK;

	    যদি (!word_ends)
	    {
		পূর্ণাঙ্ক	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		পূর্ণাঙ্ক	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		যদি (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		যদি (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    যদি (STRNCMP(ptr, p, wlen) != 0)
		    {
			বাবে (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (পূর্ণাঙ্ক)(p - mip->mi_fword);
		    }
		}
#যদি 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		যদি (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		বাবে (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    যদি (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			যদি (mip->mi_lp->lp_slang->sl_fidxs == রিক্ত
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    অনুৱর্ত;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    যদি (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#যদি 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			যদি (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    যদি (!slang->sl_nobreak)
			ভঙ্গ;
		}
		--mip->mi_complen;
		যদি (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		যদি (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		অন্যথা
		{
		    যদি (mip->mi_result == SP_OK)
			ভঙ্গ;
		    অনুৱর্ত;
		}
	    }

	    যদি (flags & WF_BANNED)
		res = SP_BANNED;
	    অন্যথা যদি (flags & WF_REGION)
	    {
		// Check region.
		যদি ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		অন্যথা
		    res = SP_LOCAL;
	    }
	    অন্যথা যদি (flags & WF_RARE)
		res = SP_RARE;
	    অন্যথা
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    যদি (nobreak_result == SP_BAD)
	    {
		যদি (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		অন্যথা যদি (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    অন্যথা যদি (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    অন্যথা যদি (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    যদি (mip->mi_result == SP_OK)
		ভঙ্গ;
	}

	যদি (mip->mi_result == SP_OK)
	    ভঙ্গ;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    পূর্ণাঙ্ক
match_checkcompoundpattern(
    char_u	*ptr,
    পূর্ণাঙ্ক		wlen,
    garray_T	*gap)  // &sl_comppat
{
    পূর্ণাঙ্ক		i;
    char_u	*p;
    পূর্ণাঙ্ক		len;

    বাবে (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	যদি (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (পূর্ণাঙ্ক)STRLEN(p);
	    যদি (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ঘূৰাই TRUE;
	}
    }
    ঘূৰাই FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    পূর্ণাঙ্ক
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    পূর্ণাঙ্ক		i;
    char_u	*p;

    যদি (slang->sl_compprog == রিক্ত)
	ঘূৰাই FALSE;
    যদি (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	বাবে (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    অন্যথা
	p = flags;
    যদি (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ঘূৰাই FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    যদি (slang->sl_compsylmax < MAXWLEN
		       && গুনতি_syllables(slang, word) > slang->sl_compsylmax)
	ঘূৰাই (পূর্ণাঙ্ক)STRLEN(flags) < slang->sl_compmax;
    ঘূৰাই TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    পূর্ণাঙ্ক
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;

    // loop over all the COMPOUNDRULE entries
    বাবে (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	বাবে (i = 0; ; ++i)
	{
	    c = compflags[i];
	    যদি (c == NUL)
		// found a rule that matches for the flags we have so far
		ঘূৰাই TRUE;
	    যদি (*p == '/' || *p == NUL)
		ভঙ্গ;  // end of rule, it's too short
	    যদি (*p == '[')
	    {
		পূর্ণাঙ্ক match = FALSE;

		// compare against all the flags in []
		++p;
		যেতিয়ালৈ (*p != ']' && *p != NUL)
		    যদি (*p++ == c)
			match = TRUE;
		যদি (!match)
		    ভঙ্গ;  // none matches
	    }
	    অন্যথা যদি (*p != c)
		ভঙ্গ;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	যদি (p == রিক্ত)
	    ভঙ্গ;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ঘূৰাই FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    পূর্ণাঙ্ক
valid_word_prefix(
    পূর্ণাঙ্ক		totprefcnt,	// nr of prefix IDs
    পূর্ণাঙ্ক		arridx,		// idx in sl_pidxs[]
    পূর্ণাঙ্ক		flags,
    char_u	*word,
    slang_T	*slang,
    পূর্ণাঙ্ক		cond_req)	// only use prefixes with a condition
{
    পূর্ণাঙ্ক		prefcnt;
    পূর্ণাঙ্ক		pidx;
    regprog_T	**rp;
    পূর্ণাঙ্ক		prefid;

    prefid = (unsigned)flags >> 24;
    বাবে (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	যদি (prefid != (pidx & 0xff))
	    অনুৱর্ত;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	যদি ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    অনুৱর্ত;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	যদি (*rp != রিক্ত)
	{
	    যদি (!vim_regexec_prog(rp, FALSE, word, 0))
		অনুৱর্ত;
	}
	অন্যথা যদি (cond_req)
	    অনুৱর্ত;

	// It's a match!  Return the WF_ flags.
	ঘূৰাই pidx;
    }
    ঘূৰাই 0;
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
    স্থির শূন্য
find_prefix(matchinf_T *mip, পূর্ণাঙ্ক mode)
{
    idx_T	arridx = 0;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		wlen = 0;
    পূর্ণাঙ্ক		flen;
    পূর্ণাঙ্ক		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    যদি (byts == রিক্ত)
	ঘূৰাই;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    যদি (mode == FIND_COMPOUND)
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
    বাবে (;;)
    {
	যদি (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	যদি (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    যেতিয়ালৈ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    যদি (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    যদি (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    অন্যথা
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    যদি (len == 0)
		ভঙ্গ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	যদি (ptr[wlen] == NUL)
	    ভঙ্গ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	যেতিয়ালৈ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    যদি (byts[m] > c)
		hi = m - 1;
	    অন্যথা যদি (byts[m] < c)
		lo = m + 1;
	    অন্যথা
	    {
		lo = hi = m;
		ভঙ্গ;
	    }
	}

	// Stop if there is no matching byte.
	যদি (hi < lo || byts[lo] != c)
	    ভঙ্গ;

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
    স্থির পূর্ণাঙ্ক
fold_more(matchinf_T *mip)
{
    পূর্ণাঙ্ক		flen;
    char_u	*p;

    p = mip->mi_fend;
    কৰা
	MB_PTR_ADV(mip->mi_fend);
    যেতিয়ালৈ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    যদি (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (শূন্য)spell_casefold(mip->mi_win, p, (পূর্ণাঙ্ক)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (পূর্ণাঙ্ক)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ঘূৰাই flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    পূর্ণাঙ্ক
spell_valid_case(
    পূর্ণাঙ্ক	    wordflags,	    // flags for the checked word.
    পূর্ণাঙ্ক	    treeflags)	    // flags for the word in the spell tree
{
    ঘূৰাই ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    পূর্ণাঙ্ক
spell_check_window(win_T *wp)
{
    ঘূৰাই wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(ৱর্ণ **)(wp->w_s->b_langp.ga_data) != রিক্ত;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    স্থির পূর্ণাঙ্ক
no_spell_checking(win_T *wp)
{
    যদি (spell_check_window(wp))
	ঘূৰাই FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ঘূৰাই TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    পূর্ণাঙ্ক
spell_move_to(
    win_T	*wp,
    পূর্ণাঙ্ক		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    পূর্ণাঙ্ক		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    পূর্ণাঙ্ক		found_len = 0;
    char_u	*সাৰি;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    পূর্ণাঙ্ক		len;
#ifdef FEAT_SYN_HL
    পূর্ণাঙ্ক		has_syntax = syntax_present(wp);
#endif
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		can_spell;
    char_u	*buf = রিক্ত;
    পূর্ণাঙ্ক		buflen = 0;
    পূর্ণাঙ্ক		skip = 0;
    পূর্ণাঙ্ক		capcol = -1;
    পূর্ণাঙ্ক		found_one = FALSE;
    পূর্ণাঙ্ক		wrapped = FALSE;

    যদি (no_spell_checking(wp))
	ঘূৰাই 0;

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

    যেতিয়ালৈ (!got_int)
    {
	পূর্ণাঙ্ক empty_সাৰি;

	সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	যদি (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    যদি (buf == রিক্ত)
		ভঙ্গ;
	}

	// In first line check first word for Capital.
	যদি (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	যদি (capcol == 0)
	    capcol = getwhitecols(সাৰি);
	অন্যথা যদি (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(সাৰি);
	    যদি (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_সাৰি = *skipwhite(সাৰি) == NUL;
	STRCPY(buf, সাৰি);
	যদি (lnum < wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	    spell_cat_সাৰি(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	যেতিয়ালৈ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    যদি (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		ভঙ্গ;

	    // start of word
	    attr = HLF_গুনতি;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    যদি (attr != HLF_গুনতি)
	    {
		// We found a bad word.  Check the attribute.
		যদি (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    যদি (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			যদি (has_syntax)
			{
			    col = (পূর্ণাঙ্ক)(p - buf);
			    (শূন্য)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    যদি (!can_spell)
				attr = HLF_গুনতি;
			}
			অন্যথা
#endif
			    can_spell = TRUE;

			যদি (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (পূর্ণাঙ্ক)(p - buf);
			    found_pos.coladd = 0;
			    যদি (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				যদি (attrp != রিক্ত)
				    *attrp = attr;
				ঘূৰাই len;
			    }
			    অন্যথা যদি (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    অন্যথা
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	যদি (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    ঘূৰাই found_len;
	}

	যদি (curline)
	    ভঙ্গ;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	যদি (lnum == wp->w_cursor.lnum && wrapped)
	    ভঙ্গ;

	// Advance to next line.
	যদি (dir == BACKWARD)
	{
	    যদি (lnum > 1)
		--lnum;
	    অন্যথা যদি (!p_ws)
		ভঙ্গ;	    // at first line and 'nowrapscan'
	    অন্যথা
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
		wrapped = TRUE;
		যদি (!shortmess(SHM_অনুসন্ধান))
		    give_warning((char_u *)_(শীৰ্ষ_bot_বাৰ্তা), TRUE);
	    }
	    capcol = -1;
	}
	অন্যথা
	{
	    যদি (lnum < wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
		++lnum;
	    অন্যথা যদি (!p_ws)
		ভঙ্গ;	    // at first line and 'nowrapscan'
	    অন্যথা
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		যদি (!shortmess(SHM_অনুসন্ধান))
		    give_warning((char_u *)_(bot_শীৰ্ষ_বাৰ্তা), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    যদি (lnum == wp->w_cursor.lnum && !found_one)
		ভঙ্গ;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    যদি (attr == HLF_গুনতি)
		skip = (পূর্ণাঙ্ক)(p - endp);
	    অন্যথা
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    যদি (empty_সাৰি)
		capcol = 0;
	}

	সাৰি_breakcheck();
    }

    vim_free(buf);
    ঘূৰাই 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    শূন্য
spell_cat_সাৰি(char_u *buf, char_u *সাৰি, পূর্ণাঙ্ক maxlen)
{
    char_u	*p;
    পূর্ণাঙ্ক		n;

    p = skipwhite(সাৰি);
    যেতিয়ালৈ (vim_strchr((char_u *)"*#/\"\t", *p) != রিক্ত)
	p = skipwhite(p + 1);

    যদি (*p == NUL)
	ঘূৰাই;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (পূর্ণাঙ্ক)(p - সাৰি) + 1;
    যদি (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
প্রকারনির্দেশ সংরচনা spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    পূর্ণাঙ্ক	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    স্থির শূন্য
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    পূর্ণাঙ্ক		r;
    spelload_T	sl;
    পূর্ণাঙ্ক		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = রিক্ত;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    বাবে (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((ৱর্ণ *)fname_enc, পরিমাণ(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#অন্যথা
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	যদি (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((ৱর্ণ *)fname_enc, পরিমাণ(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#অন্যথা
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    যদি (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		অনুৱর্ত;
	    ভঙ্গ;
	}
	ভঙ্গ;
    }

    যদি (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#অন্যথা
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    অন্যথা যদি (sl.sl_slang != রিক্ত)
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
spell_enc(শূন্য)
{

    যদি (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ঘূৰাই p_enc;
    ঘূৰাই (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    স্থির শূন্য
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((ৱর্ণ *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    যদি (lp != রিক্ত)
    {
	যদি (lang != রিক্ত)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, পরিমাণ(fromto_T), 10);
	ga_init2(&lp->sl_repsal, পরিমাণ(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ঘূৰাই lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    শূন্য
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
    শূন্য
free_sal_বস্তুসমূহ(garray_T *gap)
{
    salitem_T	*smp;

    যেতিয়ালৈ (gap->ga_len > 0)
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
    শূন্য
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    বাবে (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	যেতিয়ালৈ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    যদি (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	যদি (gap->ga_data != রিক্ত)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    বাবে (i = 0; i < gap->ga_len; ++i)
		vim_free(((পূর্ণাঙ্ক **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    অন্যথা
	free_sal_বস্তুসমূহ(gap);

    বাবে (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = রিক্ত;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_বস্তুসমূহ);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_চাবি_OFF);
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
    শূন্য
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = রিক্ত;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    স্থির শূন্য
spell_load_cb(char_u *fname, শূন্য *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, রিক্ত, FALSE);
    যদি (slang == রিক্ত)
	ঘূৰাই;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    যদি (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    অন্যথা যদি (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    শূন্য
গুনতি_common_word(
    slang_T	*lp,
    char_u	*word,
    পূর্ণাঙ্ক		len,	    // word length, -1 for up to NUL
    পূর্ণাঙ্ক		গুনতি)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    যদি (len == -1)
	p = word;
    অন্যথা যদি (len >= MAXWLEN)
	ঘূৰাই;
    অন্যথা
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    যদি (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	যদি (wc == রিক্ত)
	    ঘূৰাই;
	STRCPY(wc->wc_word, p);
	wc->wc_গুনতি = গুনতি;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    অন্যথা
    {
	wc = HI2WC(hi);
	যদি ((wc->wc_গুনতি += গুনতি) < (unsigned)গুনতি)	// check for overflow
	    wc->wc_গুনতি = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    পূর্ণাঙ্ক
byte_in_str(char_u *str, পূর্ণাঙ্ক n)
{
    char_u	*p;

    বাবে (p = str; *p != NUL; ++p)
	যদি (*p == n)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

#define SY_MAXLEN   30
প্রকারনির্দেশ সংরচনা syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    পূর্ণাঙ্ক		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    পূর্ণাঙ্ক
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    পূর্ণাঙ্ক		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_বস্তুসমূহ, পরিমাণ(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    যেতিয়ালৈ (p != রিক্ত)
    {
	*p++ = NUL;
	যদি (*p == NUL)	    // trailing slash
	    ভঙ্গ;
	s = p;
	p = vim_strchr(p, '/');
	যদি (p == রিক্ত)
	    l = (পূর্ণাঙ্ক)STRLEN(s);
	অন্যথা
	    l = (পূর্ণাঙ্ক)(p - s);
	যদি (l >= SY_MAXLEN)
	    ঘূৰাই SP_FORMERROR;
	যদি (ga_grow(&slang->sl_syl_বস্তুসমূহ, 1) == FAIL)
	    ঘূৰাই SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_বস্তুসমূহ.ga_data)
					       + slang->sl_syl_বস্তুসমূহ.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ঘূৰাই OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    স্থির পূর্ণাঙ্ক
গুনতি_syllables(slang_T *slang, char_u *word)
{
    পূর্ণাঙ্ক		cnt = 0;
    পূর্ণাঙ্ক		skip = FALSE;
    char_u	*p;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i;
    syl_item_T	*syl;
    পূর্ণাঙ্ক		c;

    যদি (slang->sl_syllable == রিক্ত)
	ঘূৰাই 0;

    বাবে (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	যদি (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    অনুৱর্ত;
	}

	// Find longest match of syllable items.
	len = 0;
	বাবে (i = 0; i < slang->sl_syl_বস্তুসমূহ.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_বস্তুসমূহ.ga_data) + i;
	    যদি (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	যদি (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	অন্যথা
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    যদি (vim_strchr(slang->sl_syllable, c) == রিক্ত)
		skip = FALSE;	    // No, search for next syllable
	    অন্যথা যদি (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ঘূৰাই cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    ৱর্ণ *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    পূর্ণাঙ্ক		filename;
    পূর্ণাঙ্ক		region_mask;
    slang_T	*slang;
    পূর্ণাঙ্ক		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    পূর্ণাঙ্ক		len;
    char_u	*p;
    পূর্ণাঙ্ক		round;
    char_u	*spf;
    char_u	*use_region = রিক্ত;
    পূর্ণাঙ্ক		dont_use_region = FALSE;
    পূর্ণাঙ্ক		nobreak = FALSE;
    পূর্ণাঙ্ক		i, j;
    langp_T	*lp, *lp2;
    স্থির পূর্ণাঙ্ক	recursive = FALSE;
    ৱর্ণ	*ret_বাৰ্তা = রিক্ত;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    যদি (recursive)
	ঘূৰাই রিক্ত;
    recursive = TRUE;

    ga_init2(&ga, পরিমাণ(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    যদি (spl_copy == রিক্ত)
	গচ্ছ theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    বাবে (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = রিক্ত;

	যদি (!valid_spelllang(lang))
	    অনুৱর্ত;

	যদি (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    অনুৱর্ত;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	যদি (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    যদি (p != রিক্ত && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    অন্যথা
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		যদি (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    ভঙ্গ;
	}
	অন্যথা
	{
	    filename = FALSE;
	    যদি (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    অন্যথা
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		যদি (STRICMP(lang, slang->sl_name) == 0)
		    ভঙ্গ;
	}

	যদি (region != রিক্ত)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    যদি (use_region != রিক্ত && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	যদি (slang == রিক্ত)
	{
	    যদি (filename)
		(শূন্য)spell_load_file(lang, lang, রিক্ত, FALSE);
	    অন্যথা
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		যদি (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_বাৰ্তা = N_(e_spellfilemising_autocommand_deleted_buffer);
		    গচ্ছ theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    যদি (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		যদি (!filename && region != রিক্ত)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    যদি (c == REGION_ALL)
		    {
			যদি (slang->sl_add)
			{
			    যদি (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			অন্যথা
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    অন্যথা
			region_mask = 1 << c;
		}

		যদি (region_mask != 0)
		{
		    যদি (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_বাৰ্তা = e_out_of_memory;
			গচ্ছ theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    যদি (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    বাবে (round = 0; round == 0 || *spf != NUL; ++round)
    {
	যদি (round == 0)
	{
	    // Internal wordlist, if there is one.
	    যদি (int_wordlist == রিক্ত)
		অনুৱর্ত;
	    int_wordlist_spl(spf_name);
	}
	অন্যথা
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    বাবে (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		যদি (p != রিক্ত && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    ভঙ্গ;
	    }
	    যদি (c < ga.ga_len)
		অনুৱর্ত;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    যদি (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		ভঙ্গ;
	যদি (slang == রিক্ত)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    যদি (round == 0)
		STRCPY(lang, "internal wordlist");
	    অন্যথা
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		যদি (p != রিক্ত)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, রিক্ত, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    যদি (slang != রিক্ত && nobreak)
		slang->sl_nobreak = TRUE;
	}
	যদি (slang != রিক্ত && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    যদি (use_region != রিক্ত && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		যদি (c != REGION_ALL)
		    region_mask = 1 << c;
		অন্যথা যদি (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    যদি (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = রিক্ত;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = রিক্ত;
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
    বাবে (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	যদি (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	অন্যথা
	    // find first similar language that does sound folding
	    বাবে (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		যদি (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    ভঙ্গ;
		}
	    }

	// REP items
	যদি (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	অন্যথা
	    // find first similar language that has REP items
	    বাবে (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		যদি (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    ভঙ্গ;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ঘূৰাই ret_বাৰ্তা;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    স্থির শূন্য
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    স্থির শূন্য
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    যদি (lp->sl_midword == রিক্ত)	    // there aren't any
	ঘূৰাই;

    বাবে (p = lp->sl_midword; *p != NUL; )
	যদি (has_mbyte)
	{
	    পূর্ণাঙ্ক	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    যদি (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    অন্যথা যদি (wp->w_s->b_spell_ismw_mb == রিক্ত)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    অন্যথা
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (পূর্ণাঙ্ক)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		যদি (bp != রিক্ত)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	অন্যথা
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    স্থির পূর্ণাঙ্ক
find_region(char_u *rp, char_u *region)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; ; i += 2)
    {
	যদি (rp[i] == NUL)
	    ঘূৰাই REGION_ALL;
	যদি (rp[i] == region[0] && rp[i + 1] == region[1])
	    ভঙ্গ;
    }
    ঘূৰাই i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    পূর্ণাঙ্ক
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		firstcap;
    পূর্ণাঙ্ক		allcap;
    পূর্ণাঙ্ক		past_second = FALSE;	// past second word char

    // find first letter
    বাবে (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	যদি (end == রিক্ত ? *p == NUL : p >= end)
	    ঘূৰাই 0;	    // only non-word characters, illegal word
    যদি (has_mbyte)
	c = mb_ptr2char_adv(&p);
    অন্যথা
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    বাবে ( ; end == রিক্ত ? *p != NUL : p < end; MB_PTR_ADV(p))
	যদি (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    যদি (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		যদি (past_second && allcap)
		    ঘূৰাই WF_KEEPCAP;
		allcap = FALSE;
	    }
	    অন্যথা যদি (!allcap)
		// UlU -> KEEPCAP
		ঘূৰাই WF_KEEPCAP;
	    past_second = TRUE;
	}

    যদি (allcap)
	ঘূৰাই WF_ALLCAP;
    যদি (firstcap)
	ঘূৰাই WF_ONECAP;
    ঘূৰাই 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    শূন্য
spell_delete_wordlist(শূন্য)
{
    char_u	fname[MAXPATHL];

    যদি (int_wordlist == রিক্ত)
	ঘূৰাই;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    শূন্য
spell_free_all(শূন্য)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    যেতিয়ালৈ (first_lang != রিক্ত)
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
    শূন্য
spell_reload(শূন্য)
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
	যদি (*wp->w_s->b_p_spl != NUL)
	{
		যদি (wp->w_p_spell)
		{
		    (শূন্য)parse_spelllang(wp);
		    ভঙ্গ;
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
open_spellbuf(শূন্য)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    যদি (buf == রিক্ত)
	ঘূৰাই রিক্ত;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_চাবি = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ঘূৰাই buf;
}

/*
 * Close the buffer used for spell info.
 */
    শূন্য
close_spellbuf(buf_T *buf)
{
    যদি (buf == রিক্ত)
	ঘূৰাই;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    শূন্য
clear_spell_chartab(spelltab_T *sp)
{
    পূর্ণাঙ্ক		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    বাবে (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    বাবে (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    বাবে (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    বাবে (i = 'a'; i <= 'z'; ++i)
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
    শূন্য
init_spell_chartab(শূন্য)
{
    পূর্ণাঙ্ক	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    যদি (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	বাবে (i = 128; i <= 255; ++i)
	    যদি (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    অন্যথা যদি (enc_utf8)
    {
	বাবে (i = 128; i < 256; ++i)
	{
	    পূর্ণাঙ্ক f = utf_fold(i);
	    পূর্ণাঙ্ক u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    অন্যথা
    {
	// Rough guess: use locale-dependent library functions.
	বাবে (i = 128; i < 256; ++i)
	{
	    যদি (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    অন্যথা যদি (MB_ISLOWER(i))
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
    পূর্ণাঙ্ক
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    পূর্ণাঙ্ক		l;
    পূর্ণাঙ্ক		c;

    যদি (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	যদি (l == 1)
	{
	    // be quick for ASCII
	    যদি (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	অন্যথা
	{
	    c = mb_ptr2char(p);
	    যদি (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != রিক্ত
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != রিক্ত))
		s = p + l;
	}

	c = mb_ptr2char(s);
	যদি (c > 255)
	    ঘূৰাই spell_mb_isword_class(mb_get_class(s), wp);
	ঘূৰাই spelltab.st_isw[c];
    }

    ঘূৰাই spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    পূর্ণাঙ্ক
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    পূর্ণাঙ্ক		c;

    যদি (has_mbyte)
    {
	c = mb_ptr2char(p);
	যদি (c > 255)
	    ঘূৰাই spell_mb_isword_class(mb_get_class(p), wp);
	ঘূৰাই spelltab.st_isw[c];
    }
    ঘূৰাই spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    স্থির পূর্ণাঙ্ক
spell_mb_isword_class(পূর্ণাঙ্ক cl, win_T *wp)
{
    যদি (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ঘূৰাই cl == 2 || cl == 0x2800;
    ঘূৰাই cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    স্থির পূর্ণাঙ্ক
spell_iswordp_w(পূর্ণাঙ্ক *p, win_T *wp)
{
    পূর্ণাঙ্ক		*s;

    যদি (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != রিক্ত
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != রিক্ত))
	s = p + 1;
    অন্যথা
	s = p;

    যদি (*s > 255)
    {
	যদি (enc_utf8)
	    ঘূৰাই spell_mb_isword_class(utf_class(*s), wp);
	যদি (enc_dbcs)
	    ঘূৰাই spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	ঘূৰাই 0;
    }
    ঘূৰাই spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    পূর্ণাঙ্ক
spell_casefold(
    win_T	*wp,
    char_u	*str,
    পূর্ণাঙ্ক		len,
    char_u	*buf,
    পূর্ণাঙ্ক		buflen)
{
    পূর্ণাঙ্ক		i;

    যদি (len >= buflen)
    {
	buf[0] = NUL;
	ঘূৰাই FAIL;		// result will not fit
    }

    যদি (has_mbyte)
    {
	পূর্ণাঙ্ক	outi = 0;
	char_u	*p;
	পূর্ণাঙ্ক	c;

	// Fold one character at a time.
	বাবে (p = str; p < str + len; )
	{
	    যদি (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ঘূৰাই FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    যদি (c == 0x03a3 || c == 0x03c2)
	    {
		যদি (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		অন্যথা
		    c = 0x03c3;
	    }
	    অন্যথা
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    অন্যথা
    {
	// Be quick for non-multibyte encodings.
	বাবে (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ঘূৰাই OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    পূর্ণাঙ্ক
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    যদি (wp->w_s->b_cap_prog == রিক্ত)
	ঘূৰাই FALSE;

    পূর্ণাঙ্ক		need_cap = FALSE;
    char_u	*সাৰি = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : রিক্ত;
    char_u	*সাৰি_copy = রিক্ত;
    colnr_T	endcol = 0;

    যদি (col == 0 || getwhitecols(সাৰি) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	যদি (lnum == 1)
	    need_cap = TRUE;
	অন্যথা
	{
	    সাৰি = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    যদি (*skipwhite(সাৰি) == NUL)
		need_cap = TRUE;
	    অন্যথা
	    {
		// Append a space in place of the line break.
		সাৰি_copy = concat_str(সাৰি, (char_u *)" ");
		যদি (সাৰি_copy == রিক্ত)
		    ঘূৰাই FALSE;

		সাৰি = সাৰি_copy;
		endcol = (colnr_T)STRLEN(সাৰি);
	    }
	}
    }
    অন্যথা
	endcol = col;

    যদি (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = সাৰি + endcol;
	বাবে (;;)
	{
	    MB_PTR_BACK(সাৰি, p);
	    যদি (p == সাৰি || spell_iswordp_nmw(p, wp))
		ভঙ্গ;
	    যদি (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == সাৰি + endcol)
	    {
		need_cap = TRUE;
		ভঙ্গ;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(সাৰি_copy);

    ঘূৰাই need_cap;
}


/*
 * ":spellrepall"
 */
    শূন্য
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*সাৰি;
    char_u	*p;
    পূর্ণাঙ্ক		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    যদি (repl_from == রিক্ত || repl_to == রিক্ত)
    {
	emsg(_(e_no_previous_spell_replacement));
	ঘূৰাই;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    যদি (frompat == রিক্ত)
	ঘূৰাই;
    frompatlen = vim_snprintf((ৱর্ণ *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    যেতিয়ালৈ (!got_int)
    {
	যদি (do_অনুসন্ধান(রিক্ত, '/', '/', frompat, frompatlen, 1L, অনুসন্ধান_KEEP, রিক্ত) == 0
						   || u_save_cursor() == FAIL)
	    ভঙ্গ;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	সাৰি = ml_get_curline();
	যদি (addlen <= 0 || STRNCMP(সাৰি + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    যদি (p == রিক্ত)
		ভঙ্গ;
	    mch_memmove(p, সাৰি, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, সাৰি + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#যদি defined(FEAT_PROP_POPUP)
	    যদি (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (পূর্ণাঙ্ক)addlen, APC_SUBSTITUTE);
#endif

	    যদি (curwin->w_cursor.lnum != prev_lnum)
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

    যদি (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    অন্যথা
	do_sub_বাৰ্তা(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    শূন্য
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    পূর্ণাঙ্ক		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		l;

    p = word;
    যদি (has_mbyte)
	c = mb_cptr2char_adv(&p);
    অন্যথা
	c = *p++;
    যদি (upper)
	c = SPELL_TOUPPER(c);
    অন্যথা
	c = SPELL_TOFOLD(c);
    যদি (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    অন্যথা
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
    শূন্য
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    পূর্ণাঙ্ক		c;

    d = wcopy;
    বাবে (s = word; *s != NUL; )
    {
	যদি (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	অন্যথা
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	যদি (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    যদি (d - wcopy >= MAXWLEN - 1)
		ভঙ্গ;
	    *d++ = c;
	}
	অন্যথা
	    c = SPELL_TOUPPER(c);

	যদি (has_mbyte)
	{
	    যদি (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		ভঙ্গ;
	    d += mb_char2bytes(c, d);
	}
	অন্যথা
	{
	    যদি (d - wcopy >= MAXWLEN - 1)
		ভঙ্গ;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    পূর্ণাঙ্ক
nofold_len(char_u *fword, পূর্ণাঙ্ক flen, char_u *word)
{
    char_u	*p;
    পূর্ণাঙ্ক		i = 0;

    বাবে (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    বাবে (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    ঘূৰাই (পূর্ণাঙ্ক)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    শূন্য
make_case_word(char_u *fword, char_u *cword, পূর্ণাঙ্ক flags)
{
    যদি (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    অন্যথা যদি (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    অন্যথা
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#যদি defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    পূর্ণাঙ্ক		lpi;

    যদি (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	বাবে (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    যদি (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		ঘূৰাই vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ঘূৰাই vim_strsave(word);
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
    শূন্য
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    পূর্ণাঙ্ক		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    যদি (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    অন্যথা
    {
	// SAL items used.  Requires the word to be case-folded.
	যদি (folded)
	    word = inword;
	অন্যথা
	{
	    (শূন্য)spell_casefold(curwin,
				  inword, (পূর্ণাঙ্ক)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	যদি (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	অন্যথা
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    স্থির শূন্য
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    পূর্ণাঙ্ক		ri = 0;
    পূর্ণাঙ্ক		c;

    যদি (has_mbyte)
    {
	পূর্ণাঙ্ক	prevc = 0;
	পূর্ণাঙ্ক	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	বাবে (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    যদি (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    অন্যথা যদি (c < 256)
		c = slang->sl_sal_first[c];
	    অন্যথা
	    {
		ip = ((পূর্ণাঙ্ক **)slang->sl_sal.ga_data)[c & 0xff];
		যদি (ip == রিক্ত)		// empty list, can't match
		    c = NUL;
		অন্যথা
		    বাবে (;;)		// find "c" in the list
		    {
			যদি (*ip == 0)	// not found
			{
			    c = NUL;
			    ভঙ্গ;
			}
			যদি (*ip == c)	// match!
			{
			    c = ip[1];
			    ভঙ্গ;
			}
			ip += 2;
		    }
	    }

	    যদি (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		যদি (ri + MB_MAXBYTES > MAXWLEN)
		    ভঙ্গ;
		prevc = c;
	    }
	}
    }
    অন্যথা
    {
	// The sl_sal_first[] table contains the translation.
	বাবে (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    যদি (VIM_ISWHITE(c))
		c = ' ';
	    অন্যথা
		c = slang->sl_sal_first[c];
	    যদি (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    স্থির শূন্য
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    পূর্ণাঙ্ক		i, j, z;
    পূর্ণাঙ্ক		reslen;
    পূর্ণাঙ্ক		n, k = 0;
    পূর্ণাঙ্ক		z0;
    পূর্ণাঙ্ক		k0;
    পূর্ণাঙ্ক		n0;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		pri;
    পূর্ণাঙ্ক		p0 = -333;
    পূর্ণাঙ্ক		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    যদি (slang->sl_rem_accents)
    {
	t = word;
	যেতিয়ালৈ (*s != NUL)
	{
	    যদি (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    অন্যথা
	    {
		যদি (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    অন্যথা
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    যেতিয়ালৈ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	যদি (n >= 0)
	{
	    // check all rules for the same letter
	    বাবে (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		যদি (k > 1)
		{
		    যদি (word[i + 1] != s[1])
			অনুৱর্ত;
		    যদি (k > 2)
		    {
			বাবে (j = 2; j < k; ++j)
			    যদি (word[i + j] != s[j])
				ভঙ্গ;
			যদি (j < k)
			    অনুৱর্ত;
		    }
		}

		যদি ((pf = smp[n].sm_oneof) != রিক্ত)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    যেতিয়ালৈ (*pf != NUL && *pf != word[i + k])
			++pf;
		    যদি (*pf == NUL)
			অনুৱর্ত;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		যেতিয়ালৈ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		যদি (*s == '<')
		    s++;
		যদি (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		যদি (*s == '^' && *(s + 1) == '^')
		    s++;

		যদি (*s == NUL
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

		    যদি (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			বাবে ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    যদি (k0 > 1)
			    {
				যদি (word[i + k] != s[1])
				    অনুৱর্ত;
				যদি (k0 > 2)
				{
				    pf = word + i + k + 1;
				    বাবে (j = 2; j < k0; ++j)
					যদি (*pf++ != s[j])
					    ভঙ্গ;
				    যদি (j < k0)
					অনুৱর্ত;
				}
			    }
			    k0 += k - 1;

			    যদি ((pf = smp[n0].sm_oneof) != রিক্ত)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				যেতিয়ালৈ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				যদি (*pf == NUL)
				    অনুৱর্ত;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    যেতিয়ালৈ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    যদি (*s == '<')
				s++;
			    যদি (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    যদি (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				যদি (k0 == k)
				    // this is just a piece of the string
				    অনুৱর্ত;

				যদি (p0 < pri)
				    // priority too low
				    অনুৱর্ত;
				// rule fits; stop search
				ভঙ্গ;
			    }
			}

			যদি (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    অনুৱর্ত;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    যদি (s == রিক্ত)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != রিক্ত) ? 1 : 0;
		    যদি (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			যদি (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			যেতিয়ালৈ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			যদি (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    অন্যথা
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			যেতিয়ালৈ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    যদি (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			যদি (strstr((ৱর্ণ *)pf, "^^") != রিক্ত)
			{
			    যদি (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    ভঙ্গ;
		}
	    }
	}
	অন্যথা যদি (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	যদি (z0 == 0)
	{
	    যদি (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    স্থির শূন্য
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    পূর্ণাঙ্ক		word[MAXWLEN];
    পূর্ণাঙ্ক		wres[MAXWLEN];
    পূর্ণাঙ্ক		l;
    char_u	*s;
    পূর্ণাঙ্ক		*ws;
    char_u	*t;
    পূর্ণাঙ্ক		*pf;
    পূর্ণাঙ্ক		i, j, z;
    পূর্ণাঙ্ক		reslen;
    পূর্ণাঙ্ক		n, k = 0;
    পূর্ণাঙ্ক		z0;
    পূর্ণাঙ্ক		k0;
    পূর্ণাঙ্ক		n0;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		pri;
    পূর্ণাঙ্ক		p0 = -333;
    পূর্ণাঙ্ক		c0;
    পূর্ণাঙ্ক		did_white = FALSE;
    পূর্ণাঙ্ক		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    বাবে (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	যদি (slang->sl_rem_accents)
	{
	    যদি (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		যদি (did_white)
		    অনুৱর্ত;
		c = ' ';
		did_white = TRUE;
	    }
	    অন্যথা
	    {
		did_white = FALSE;
		যদি (!spell_iswordp_nmw(t, curwin))
		    অনুৱর্ত;
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
    যেতিয়ালৈ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	যদি (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    বাবে (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		যদি (c != ws[0])
		    অনুৱর্ত;
		k = smp[n].sm_leadlen;
		যদি (k > 1)
		{
		    যদি (word[i + 1] != ws[1])
			অনুৱর্ত;
		    যদি (k > 2)
		    {
			বাবে (j = 2; j < k; ++j)
			    যদি (word[i + j] != ws[j])
				ভঙ্গ;
			যদি (j < k)
			    অনুৱর্ত;
		    }
		}

		যদি ((pf = smp[n].sm_oneof_w) != রিক্ত)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    যেতিয়ালৈ (*pf != NUL && *pf != word[i + k])
			++pf;
		    যদি (*pf == NUL)
			অনুৱর্ত;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		যেতিয়ালৈ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		যদি (*s == '<')
		    s++;
		যদি (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		যদি (*s == '^' && *(s + 1) == '^')
		    s++;

		যদি (*s == NUL
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

		    যদি (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			বাবে ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    যদি (c0 != ws[0])
				অনুৱর্ত;
			    k0 = smp[n0].sm_leadlen;
			    যদি (k0 > 1)
			    {
				যদি (word[i + k] != ws[1])
				    অনুৱর্ত;
				যদি (k0 > 2)
				{
				    pf = word + i + k + 1;
				    বাবে (j = 2; j < k0; ++j)
					যদি (*pf++ != ws[j])
					    ভঙ্গ;
				    যদি (j < k0)
					অনুৱর্ত;
				}
			    }
			    k0 += k - 1;

			    যদি ((pf = smp[n0].sm_oneof_w) != রিক্ত)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				যেতিয়ালৈ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				যদি (*pf == NUL)
				    অনুৱর্ত;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    যেতিয়ালৈ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    যদি (*s == '<')
				s++;
			    যদি (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    যদি (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				যদি (k0 == k)
				    // this is just a piece of the string
				    অনুৱর্ত;

				যদি (p0 < pri)
				    // priority too low
				    অনুৱর্ত;
				// rule fits; stop search
				ভঙ্গ;
			    }
			}

			যদি (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    অনুৱর্ত;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != রিক্ত) ? 1 : 0;
		    যদি (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			যদি (reslen > 0 && ws != রিক্ত && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			যদি (ws != রিক্ত)
			    যেতিয়ালৈ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			যদি (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    পরিমাণ(পূর্ণাঙ্ক) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    অন্যথা
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			যদি (ws != রিক্ত)
			    যেতিয়ালৈ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				যদি (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			যদি (ws == রিক্ত)
			    c = NUL;
			অন্যথা
			    c = *ws;
			যদি (strstr((ৱর্ণ *)s, "^^") != রিক্ত)
			{
			    যদি (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       পরিমাণ(পূর্ণাঙ্ক) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    ভঙ্গ;
		}
	    }
	}
	অন্যথা যদি (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	যদি (z0 == 0)
	{
	    যদি (k && !p0 && reslen < MAXWLEN && c != NUL
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
    বাবে (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	যদি (l + MB_MAXBYTES > MAXWLEN)
	    ভঙ্গ;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    শূন্য
ex_spellinfo(exarg_T *eap UNUSED)
{
    পূর্ণাঙ্ক		lpi;
    langp_T	*lp;
    char_u	*p;

    যদি (no_spell_checking(curwin))
	ঘূৰাই;

    বাৰ্তা_start();
    বাবে (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	বাৰ্তা_puts("file: ");
	বাৰ্তা_puts((ৱর্ণ *)lp->lp_slang->sl_fname);
	বাৰ্তা_putchar('\n');
	p = lp->lp_slang->sl_info;
	যদি (p != রিক্ত)
	{
	    বাৰ্তা_puts((ৱর্ণ *)p);
	    বাৰ্তা_putchar('\n');
	}
    }
    বাৰ্তা_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_গুনতি	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    শূন্য
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    যদি (no_spell_checking(curwin))
	ঘূৰাই;
    (শূন্য)get_option_মান((char_u*)"spl", &dummy, &spl, রিক্ত, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_মান_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_মান_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    যদি (!BUFEMPTY())
	ঘূৰাই;

    spell_dump_compl(রিক্ত, 0, রিক্ত, eap->forceit ? DUMPFLAG_গুনতি : 0);

    // Delete the empty line that we started with.
    যদি (curbuf->b_ml.ml_সাৰি_গুনতি > 1)
	ml_delete(curbuf->b_ml.ml_সাৰি_গুনতি);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    শূন্য
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    পূর্ণাঙ্ক		ic,	    // ignore case
    পূর্ণাঙ্ক		*dir,	    // direction for adding matches
    পূর্ণাঙ্ক		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    পূর্ণাঙ্ক		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    পূর্ণাঙ্ক		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		depth;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		flags;
    char_u	*region_names = রিক্ত;	    // region names being used
    পূর্ণাঙ্ক		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    পূর্ণাঙ্ক		lpi;
    পূর্ণাঙ্ক		dumpflags = dumpflags_arg;
    পূর্ণাঙ্ক		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    যদি (pat != রিক্ত)
    {
	যদি (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	অন্যথা
	{
	    n = captype(pat, রিক্ত);
	    যদি (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    অন্যথা যদি (n == WF_ALLCAP && (পূর্ণাঙ্ক)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    বাবে (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	যদি (p[0] != 0)
	{
	    যদি (region_names == রিক্ত)	    // first language with regions
		region_names = p;
	    অন্যথা যদি (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		ভঙ্গ;
	    }
	}
    }

    যদি (do_region && region_names != রিক্ত && pat == রিক্ত)
    {
	vim_snprintf((ৱর্ণ *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    অন্যথা
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    বাবে (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	যদি (slang->sl_fbyts == রিক্ত)	    // reloading failed
	    অনুৱর্ত;

	যদি (pat == রিক্ত)
	{
	    vim_snprintf((ৱর্ণ *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	যদি (pat != রিক্ত && slang->sl_pbyts == রিক্ত)
	    patlen = (পূর্ণাঙ্ক)STRLEN(pat);
	অন্যথা
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	বাবে (round = 1; round <= 2; ++round)
	{
	    যদি (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    অন্যথা
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    যদি (byts == রিক্ত)
		অনুৱর্ত;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    যেতিয়ালৈ (depth >= 0 && !got_int
				  && (pat == রিক্ত || !ins_compl_interrupted()))
	    {
		যদি (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    সাৰি_breakcheck();
		    ins_compl_check_চাবিগুলি(50, FALSE);
		}
		অন্যথা
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    যদি (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (পূর্ণাঙ্ক)idxs[n];
			যদি ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    যদি (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    যদি (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				যদি (pat == রিক্ত)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    যদি (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    অন্যথা
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
			যদি (depth <= patlen
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
    স্থির শূন্য
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    পূর্ণাঙ্ক		*dir,
    পূর্ণাঙ্ক		dumpflags,
    পূর্ণাঙ্ক		wordflags,
    linenr_T	lnum)
{
    পূর্ণাঙ্ক		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		flags = wordflags;

    যদি (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    যদি (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    যদি ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    অন্যথা
    {
	p = word;
	যদি ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, রিক্ত) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    যদি (pat == রিক্ত)
    {
	// Add flags and regions after a slash.
	যদি ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    যদি (keepcap)
		STRCAT(badword, "=");
	    যদি (flags & WF_BANNED)
		STRCAT(badword, "!");
	    অন্যথা যদি (flags & WF_RARE)
		STRCAT(badword, "?");
	    যদি (flags & WF_REGION)
		বাবে (i = 0; i < 7; ++i)
		    যদি (flags & (0x10000 << i))
			sprintf((ৱর্ণ *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	যদি (dumpflags & DUMPFLAG_গুনতি)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((ৱর্ণ *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_গুনতি);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    অন্যথা যদি (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (পূর্ণাঙ্ক)STRLEN(p),
					  p_ic, রিক্ত, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    স্থির linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    পূর্ণাঙ্ক		*dir,
    পূর্ণাঙ্ক		dumpflags,
    পূর্ণাঙ্ক		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    পূর্ণাঙ্ক		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    পূর্ণাঙ্ক		has_word_up = FALSE;
    পূর্ণাঙ্ক		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    পূর্ণাঙ্ক		depth;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    যদি (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    যদি (byts != রিক্ত)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	যেতিয়ালৈ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    যদি (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		সাৰি_breakcheck();
	    }
	    অন্যথা
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		যদি (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    বাবে (i = 1; i < len; ++i)
			যদি (byts[n + i] != 0)
			    ভঙ্গ;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    যদি (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			যদি (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    যদি (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			যদি (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    যদি (lnum != 0)
				++lnum;
			}
		    }
		}
		অন্যথা যদি (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    ঘূৰাই lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    যেতিয়ালৈ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ঘূৰাই p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    পূর্ণাঙ্ক
spell_word_start(পূর্ণাঙ্ক startcol)
{
    char_u	*সাৰি;
    char_u	*p;
    পূর্ণাঙ্ক		col = 0;

    যদি (no_spell_checking(curwin))
	ঘূৰাই startcol;

    // Find a word character before "startcol".
    সাৰি = ml_get_curline();
    বাবে (p = সাৰি + startcol; p > সাৰি; )
    {
	MB_PTR_BACK(সাৰি, p);
	যদি (spell_iswordp_nmw(p, curwin))
	    ভঙ্গ;
    }

    // Go back to start of the word.
    যেতিয়ালৈ (p > সাৰি)
    {
	col = (পূর্ণাঙ্ক)(p - সাৰি);
	MB_PTR_BACK(সাৰি, p);
	যদি (!spell_iswordp(p, curwin))
	    ভঙ্গ;
	col = 0;
    }

    ঘূৰাই col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
স্থির পূর্ণাঙ্ক spell_expand_need_cap;

    শূন্য
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
    পূর্ণাঙ্ক
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    ঘূৰাই ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    পূর্ণাঙ্ক
valid_spelllang(char_u *val)
{
    ঘূৰাই valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    পূর্ণাঙ্ক
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    পূর্ণাঙ্ক		l;

    spf = val;
    যেতিয়ালৈ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	যদি (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ঘূৰাই FALSE;
	বাবে (s = spf_name; *s != NUL; ++s)
	    যদি (!vim_is_fname_char(*s))
		ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    ৱর্ণ *
did_set_spell_option(শূন্য)
{
    ৱর্ণ    *errmsg = রিক্ত;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    ভঙ্গ;
	}
    ঘূৰাই errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    ৱর্ণ *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    যদি (synblock->b_p_spc == রিক্ত || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = রিক্ত;
    অন্যথা
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	যদি (re != রিক্ত)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    যদি (synblock->b_cap_prog == রিক্ত)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ঘূৰাই e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    ঘূৰাই রিক্ত;
}

#endif  // FEAT_SPELL
