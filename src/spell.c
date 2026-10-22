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
#अन्तर्भाव "vim.h"

#सचे defined(FEAT_SPELL)

#अन्तर्भाव <time.h>

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
प्रकारनिर्देश संरचना matchinf_S
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
    पूर्णाङ्क		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    पूर्णाङ्क		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    पूर्णाङ्क		mi_prefcnt;		// number of entries at mi_prefarridx
    पूर्णाङ्क		mi_prefixlen;		// byte length of prefix
    पूर्णाङ्क		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    पूर्णाङ्क		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    पूर्णाङ्क		mi_complen;		// nr of compound words used
    पूर्णाङ्क		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    पूर्णाङ्क		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    पूर्णाङ्क		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    पूर्णाङ्क		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


स्थिर पूर्णाङ्क spell_mb_isword_class(पूर्णाङ्क cl, win_T *wp);

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

स्थिर शून्य find_word(matchinf_T *mip, पूर्णाङ्क mode);
स्थिर शून्य find_prefix(matchinf_T *mip, पूर्णाङ्क mode);
स्थिर पूर्णाङ्क fold_more(matchinf_T *mip);
स्थिर शून्य spell_load_cb(char_u *fname, शून्य *cookie);
स्थिर पूर्णाङ्क गणनम्_syllables(slang_T *slang, char_u *word);
स्थिर शून्य clear_midword(win_T *buf);
स्थिर शून्य use_midword(slang_T *lp, win_T *buf);
स्थिर पूर्णाङ्क find_region(char_u *rp, char_u *region);
स्थिर शून्य spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
स्थिर शून्य spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
स्थिर शून्य spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
स्थिर शून्य dump_word(slang_T *slang, char_u *word, char_u *pat, पूर्णाङ्क *dir, पूर्णाङ्क round, पूर्णाङ्क flags, linenr_T lnum);
स्थिर linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, पूर्णाङ्क *dir, पूर्णाङ्क round, पूर्णाङ्क flags, linenr_T startlnum);
स्थिर char_u *advance_camelcase_word(char_u *p, win_T *wp, पूर्णाङ्क *is_camel_case);

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
    पूर्णाङ्क
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    पूर्णाङ्क		*capcol,	// column to check for Capital
    पूर्णाङ्क		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    पूर्णाङ्क		nrlen = 0;	// found a number first
    पूर्णाङ्क		c;
    पूर्णाङ्क		wrongcaplen = 0;
    पूर्णाङ्क		lpi;
    पूर्णाङ्क		गणनम्_word = docount;
    पूर्णाङ्क		use_camel_case = *wp->w_s->b_p_spo != NUL;
    पूर्णाङ्क		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    सचे (*ptr <= ' ')
	पटिनिवत्त 1;

    // Return here when loading language files failed.
    सचे (wp->w_s->b_langp.ga_len == 0)
	पटिनिवत्त 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    सचे (*ptr >= '0' && *ptr <= '9')
    {
	सचे (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	अञ्ञथा सचे (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	अञ्ञथा
	    mi.mi_end = skipdigits(ptr);
	nrlen = (पूर्णाङ्क)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    सचे (spell_iswordp(mi.mi_fend, wp))
    {
	सचे (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	अञ्ञथा
	{
	    करोहि
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } याव (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	सचे (capcol != रिक्त && *capcol == 0 && wp->w_s->b_cap_prog != रिक्त)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    सचे (!SPELL_ISUPPER(c))
		wrongcaplen = (पूर्णाङ्क)(mi.mi_fend - ptr);
	}
    }
    सचे (capcol != रिक्त)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = रिक्त;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    सचे (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (शून्य)spell_casefold(wp, ptr, (पूर्णाङ्क)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (पूर्णाङ्क)STRLEN(mi.mi_fword);

    सचे (is_camel_case && mi.mi_fwordlen > 0)
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
    अत्थाय (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	सचे (mi.mi_lp->lp_slang->sl_fidxs == रिक्त)
	    अनुवर्त;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	सचे (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	सचे (गणनम्_word && mi.mi_result == SP_OK)
	{
	    गणनम्_common_word(mi.mi_lp->lp_slang, ptr,
						   (पूर्णाङ्क)(mi.mi_end - ptr), 1);
	    गणनम्_word = FALSE;
	}
    }

    सचे (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	सचे (nrlen > 0)
	{
	    सचे (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		पटिनिवत्त nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	अञ्ञथा सचे (!spell_iswordp_nmw(ptr, wp))
	{
	    सचे (capcol != रिक्त && wp->w_s->b_cap_prog != रिक्त)
	    {
		regmatch_T	regmatch;
		पूर्णाङ्क		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		सचे (r)
		    *capcol = (पूर्णाङ्क)(regmatch.endp[0] - ptr);
	    }

	    सचे (has_mbyte)
		पटिनिवत्त (*mb_ptr2len)(ptr);
	    पटिनिवत्त 1;
	}
	अञ्ञथा सचे (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	अञ्ञथा सचे (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    पूर्णाङ्क		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    सचे (mi.mi_lp->lp_slang->sl_fidxs != रिक्त)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		अत्थाय (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    सचे (p >= mi.mi_end)
			भङ्ग;
		    mi.mi_compoff = (पूर्णाङ्क)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    सचे (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			भङ्ग;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	सचे (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	अञ्ञथा सचे (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	अञ्ञथा
	    *attrp = HLF_SPL;
    }

    सचे (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	पटिनिवत्त wrongcaplen;
    }

    पटिनिवत्त (पूर्णाङ्क)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    स्थिर पूर्णाङ्क
get_char_type(पूर्णाङ्क c)
{
    सचे (VIM_ISDIGIT(c))
	पटिनिवत्त CHAR_DIGIT;
    सचे (SPELL_ISUPPER(c))
	पटिनिवत्त CHAR_UPPER;
    पटिनिवत्त CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    स्थिर char_u *
advance_camelcase_word(char_u *str, win_T *wp, पूर्णाङ्क *is_camel_case)
{
    पूर्णाङ्क last_type, last_last_type, this_type;
    पूर्णाङ्क c;
    char_u *end = str;

    *is_camel_case = FALSE;

    सचे (*str == NUL)
	पटिनिवत्त str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    याव (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	सचे (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    भङ्ग;
	}
	अञ्ञथा सचे ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    भङ्ग;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    पटिनिवत्त end;
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
    स्थिर शून्य
find_word(matchinf_T *mip, पूर्णाङ्क mode)
{
    idx_T	arridx = 0;
    पूर्णाङ्क		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    पूर्णाङ्क		endidxcnt = 0;
    पूर्णाङ्क		len;
    पूर्णाङ्क		wlen = 0;
    पूर्णाङ्क		flen;
    पूर्णाङ्क		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    पूर्णाङ्क		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    पूर्णाङ्क		word_ends;
    पूर्णाङ्क		prefix_found;
    पूर्णाङ्क		nobreak_result;

    सचे (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	सचे (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    अञ्ञथा
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	सचे (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	अञ्ञथा सचे (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    सचे (byts == रिक्त)
	पटिनिवत्त;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    अत्थाय (;;)
    {
	सचे (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	सचे (byts[arridx] == 0)
	{
	    सचे (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		पटिनिवत्त;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    याव (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    सचे (len == 0)
		भङ्ग;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	सचे (ptr[wlen] == NUL)
	    भङ्ग;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	सचे (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	याव (lo < hi)
	{
	    m = (lo + hi) / 2;
	    सचे (byts[m] > c)
		hi = m - 1;
	    अञ्ञथा सचे (byts[m] < c)
		lo = m + 1;
	    अञ्ञथा
	    {
		lo = hi = m;
		भङ्ग;
	    }
	}

	// Stop if there is no matching byte.
	सचे (hi < lo || byts[lo] != c)
	    भङ्ग;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	सचे (c == ' ')
	{
	    अत्थाय (;;)
	    {
		सचे (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		सचे (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    भङ्ग;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    याव (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	सचे ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    अनुवर्त;	    // not at first byte of character
	सचे (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    सचे (slang->sl_compprog == रिक्त && !slang->sl_nobreak)
		अनुवर्त;	    // next char is a word character
	    word_ends = FALSE;
	}
	अञ्ञथा
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	सचे (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    सचे (STRNCMP(ptr, p, wlen) != 0)
	    {
		अत्थाय (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (पूर्णाङ्क)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	अत्थाय (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    सचे (mode == FIND_FOLDWORD)
	    {
		सचे (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		सचे (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    अनुवर्त;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    अञ्ञथा सचे (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		सचे (c == 0)
		    अनुवर्त;

		// Use the WF_RARE flag for a rare prefix.
		सचे (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    सचे (slang->sl_nobreak)
	    {
		सचे ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    भङ्ग;
		}
	    }

	    अञ्ञथा सचे ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		सचे (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    अनुवर्त;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		सचे (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			अनुवर्त;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		सचे (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    अनुवर्त;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		सचे (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    अनुवर्त;
		सचे (!word_ends && (flags & WF_NOCOMPAFT))
		    अनुवर्त;

		// Quickly check if compounding is possible with this flag.
		सचे (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    अनुवर्त;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		सचे (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    अनुवर्त;

		सचे (mode == FIND_COMPOUND)
		{
		    पूर्णाङ्क	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    सचे (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			अत्थाय (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    अञ्ञथा
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    सचे (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			अनुवर्त;

		    सचे (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			सचे (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    अनुवर्त;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		सचे (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    सचे (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			सचे (ptr == mip->mi_word)
			    (शून्य)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			अञ्ञथा
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    सचे (!can_compound(slang, fword, mip->mi_compflags))
			अनुवर्त;
		}
		अञ्ञथा सचे (slang->sl_comprules != रिक्त
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    अनुवर्त;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    अञ्ञथा सचे (flags & WF_NEEDCOMP)
		अनुवर्त;

	    nobreak_result = SP_OK;

	    सचे (!word_ends)
	    {
		पूर्णाङ्क	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		पूर्णाङ्क	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		सचे (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		सचे (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    सचे (STRNCMP(ptr, p, wlen) != 0)
		    {
			अत्थाय (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (पूर्णाङ्क)(p - mip->mi_fword);
		    }
		}
#सचे 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		सचे (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		अत्थाय (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    सचे (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			सचे (mip->mi_lp->lp_slang->sl_fidxs == रिक्त
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    अनुवर्त;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    सचे (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#सचे 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			सचे (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    सचे (!slang->sl_nobreak)
			भङ्ग;
		}
		--mip->mi_complen;
		सचे (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		सचे (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		अञ्ञथा
		{
		    सचे (mip->mi_result == SP_OK)
			भङ्ग;
		    अनुवर्त;
		}
	    }

	    सचे (flags & WF_BANNED)
		res = SP_BANNED;
	    अञ्ञथा सचे (flags & WF_REGION)
	    {
		// Check region.
		सचे ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		अञ्ञथा
		    res = SP_LOCAL;
	    }
	    अञ्ञथा सचे (flags & WF_RARE)
		res = SP_RARE;
	    अञ्ञथा
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    सचे (nobreak_result == SP_BAD)
	    {
		सचे (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		अञ्ञथा सचे (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    अञ्ञथा सचे (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    अञ्ञथा सचे (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    सचे (mip->mi_result == SP_OK)
		भङ्ग;
	}

	सचे (mip->mi_result == SP_OK)
	    भङ्ग;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    पूर्णाङ्क
match_checkcompoundpattern(
    char_u	*ptr,
    पूर्णाङ्क		wlen,
    garray_T	*gap)  // &sl_comppat
{
    पूर्णाङ्क		i;
    char_u	*p;
    पूर्णाङ्क		len;

    अत्थाय (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	सचे (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (पूर्णाङ्क)STRLEN(p);
	    सचे (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		पटिनिवत्त TRUE;
	}
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    पूर्णाङ्क
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    पूर्णाङ्क		i;
    char_u	*p;

    सचे (slang->sl_compprog == रिक्त)
	पटिनिवत्त FALSE;
    सचे (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	अत्थाय (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    अञ्ञथा
	p = flags;
    सचे (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	पटिनिवत्त FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    सचे (slang->sl_compsylmax < MAXWLEN
		       && गणनम्_syllables(slang, word) > slang->sl_compsylmax)
	पटिनिवत्त (पूर्णाङ्क)STRLEN(flags) < slang->sl_compmax;
    पटिनिवत्त TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    पूर्णाङ्क
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;

    // loop over all the COMPOUNDRULE entries
    अत्थाय (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	अत्थाय (i = 0; ; ++i)
	{
	    c = compflags[i];
	    सचे (c == NUL)
		// found a rule that matches for the flags we have so far
		पटिनिवत्त TRUE;
	    सचे (*p == '/' || *p == NUL)
		भङ्ग;  // end of rule, it's too short
	    सचे (*p == '[')
	    {
		पूर्णाङ्क match = FALSE;

		// compare against all the flags in []
		++p;
		याव (*p != ']' && *p != NUL)
		    सचे (*p++ == c)
			match = TRUE;
		सचे (!match)
		    भङ्ग;  // none matches
	    }
	    अञ्ञथा सचे (*p != c)
		भङ्ग;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	सचे (p == रिक्त)
	    भङ्ग;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    पटिनिवत्त FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    पूर्णाङ्क
valid_word_prefix(
    पूर्णाङ्क		totprefcnt,	// nr of prefix IDs
    पूर्णाङ्क		arridx,		// idx in sl_pidxs[]
    पूर्णाङ्क		flags,
    char_u	*word,
    slang_T	*slang,
    पूर्णाङ्क		cond_req)	// only use prefixes with a condition
{
    पूर्णाङ्क		prefcnt;
    पूर्णाङ्क		pidx;
    regprog_T	**rp;
    पूर्णाङ्क		prefid;

    prefid = (unsigned)flags >> 24;
    अत्थाय (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	सचे (prefid != (pidx & 0xff))
	    अनुवर्त;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	सचे ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    अनुवर्त;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	सचे (*rp != रिक्त)
	{
	    सचे (!vim_regexec_prog(rp, FALSE, word, 0))
		अनुवर्त;
	}
	अञ्ञथा सचे (cond_req)
	    अनुवर्त;

	// It's a match!  Return the WF_ flags.
	पटिनिवत्त pidx;
    }
    पटिनिवत्त 0;
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
    स्थिर शून्य
find_prefix(matchinf_T *mip, पूर्णाङ्क mode)
{
    idx_T	arridx = 0;
    पूर्णाङ्क		len;
    पूर्णाङ्क		wlen = 0;
    पूर्णाङ्क		flen;
    पूर्णाङ्क		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    सचे (byts == रिक्त)
	पटिनिवत्त;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    सचे (mode == FIND_COMPOUND)
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
    अत्थाय (;;)
    {
	सचे (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	सचे (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    याव (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    सचे (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    सचे (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    अञ्ञथा
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    सचे (len == 0)
		भङ्ग;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	सचे (ptr[wlen] == NUL)
	    भङ्ग;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	याव (lo < hi)
	{
	    m = (lo + hi) / 2;
	    सचे (byts[m] > c)
		hi = m - 1;
	    अञ्ञथा सचे (byts[m] < c)
		lo = m + 1;
	    अञ्ञथा
	    {
		lo = hi = m;
		भङ्ग;
	    }
	}

	// Stop if there is no matching byte.
	सचे (hi < lo || byts[lo] != c)
	    भङ्ग;

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
    स्थिर पूर्णाङ्क
fold_more(matchinf_T *mip)
{
    पूर्णाङ्क		flen;
    char_u	*p;

    p = mip->mi_fend;
    करोहि
	MB_PTR_ADV(mip->mi_fend);
    याव (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    सचे (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (शून्य)spell_casefold(mip->mi_win, p, (पूर्णाङ्क)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (पूर्णाङ्क)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    पटिनिवत्त flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    पूर्णाङ्क
spell_valid_case(
    पूर्णाङ्क	    wordflags,	    // flags for the checked word.
    पूर्णाङ्क	    treeflags)	    // flags for the word in the spell tree
{
    पटिनिवत्त ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    पूर्णाङ्क
spell_check_window(win_T *wp)
{
    पटिनिवत्त wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(वर्ण **)(wp->w_s->b_langp.ga_data) != रिक्त;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    स्थिर पूर्णाङ्क
no_spell_checking(win_T *wp)
{
    सचे (spell_check_window(wp))
	पटिनिवत्त FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    पटिनिवत्त TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    पूर्णाङ्क
spell_move_to(
    win_T	*wp,
    पूर्णाङ्क		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    पूर्णाङ्क		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    पूर्णाङ्क		found_len = 0;
    char_u	*पङ्क्तिः;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    पूर्णाङ्क		len;
#ifdef FEAT_SYN_HL
    पूर्णाङ्क		has_syntax = syntax_present(wp);
#endif
    पूर्णाङ्क		col;
    पूर्णाङ्क		can_spell;
    char_u	*buf = रिक्त;
    पूर्णाङ्क		buflen = 0;
    पूर्णाङ्क		skip = 0;
    पूर्णाङ्क		capcol = -1;
    पूर्णाङ्क		found_one = FALSE;
    पूर्णाङ्क		wrapped = FALSE;

    सचे (no_spell_checking(wp))
	पटिनिवत्त 0;

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

    याव (!got_int)
    {
	पूर्णाङ्क empty_पङ्क्तिः;

	पङ्क्तिः = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	सचे (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    सचे (buf == रिक्त)
		भङ्ग;
	}

	// In first line check first word for Capital.
	सचे (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	सचे (capcol == 0)
	    capcol = getwhitecols(पङ्क्तिः);
	अञ्ञथा सचे (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(पङ्क्तिः);
	    सचे (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    पङ्क्तिः = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_पङ्क्तिः = *skipwhite(पङ्क्तिः) == NUL;
	STRCPY(buf, पङ्क्तिः);
	सचे (lnum < wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	    spell_cat_पङ्क्तिः(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	याव (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    सचे (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		भङ्ग;

	    // start of word
	    attr = HLF_गणनम्;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    सचे (attr != HLF_गणनम्)
	    {
		// We found a bad word.  Check the attribute.
		सचे (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    सचे (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			सचे (has_syntax)
			{
			    col = (पूर्णाङ्क)(p - buf);
			    (शून्य)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    सचे (!can_spell)
				attr = HLF_गणनम्;
			}
			अञ्ञथा
#endif
			    can_spell = TRUE;

			सचे (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (पूर्णाङ्क)(p - buf);
			    found_pos.coladd = 0;
			    सचे (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				सचे (attrp != रिक्त)
				    *attrp = attr;
				पटिनिवत्त len;
			    }
			    अञ्ञथा सचे (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    अञ्ञथा
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	सचे (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    पटिनिवत्त found_len;
	}

	सचे (curline)
	    भङ्ग;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	सचे (lnum == wp->w_cursor.lnum && wrapped)
	    भङ्ग;

	// Advance to next line.
	सचे (dir == BACKWARD)
	{
	    सचे (lnum > 1)
		--lnum;
	    अञ्ञथा सचे (!p_ws)
		भङ्ग;	    // at first line and 'nowrapscan'
	    अञ्ञथा
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
		wrapped = TRUE;
		सचे (!shortmess(SHM_अन्वेषणम्))
		    give_warning((char_u *)_(शिखरम्_bot_सन्देशः), TRUE);
	    }
	    capcol = -1;
	}
	अञ्ञथा
	{
	    सचे (lnum < wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
		++lnum;
	    अञ्ञथा सचे (!p_ws)
		भङ्ग;	    // at first line and 'nowrapscan'
	    अञ्ञथा
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		सचे (!shortmess(SHM_अन्वेषणम्))
		    give_warning((char_u *)_(bot_शिखरम्_सन्देशः), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    सचे (lnum == wp->w_cursor.lnum && !found_one)
		भङ्ग;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    सचे (attr == HLF_गणनम्)
		skip = (पूर्णाङ्क)(p - endp);
	    अञ्ञथा
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    सचे (empty_पङ्क्तिः)
		capcol = 0;
	}

	पङ्क्तिः_breakcheck();
    }

    vim_free(buf);
    पटिनिवत्त 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    शून्य
spell_cat_पङ्क्तिः(char_u *buf, char_u *पङ्क्तिः, पूर्णाङ्क maxlen)
{
    char_u	*p;
    पूर्णाङ्क		n;

    p = skipwhite(पङ्क्तिः);
    याव (vim_strchr((char_u *)"*#/\"\t", *p) != रिक्त)
	p = skipwhite(p + 1);

    सचे (*p == NUL)
	पटिनिवत्त;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (पूर्णाङ्क)(p - पङ्क्तिः) + 1;
    सचे (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
प्रकारनिर्देश संरचना spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    पूर्णाङ्क	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    स्थिर शून्य
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    पूर्णाङ्क		r;
    spelload_T	sl;
    पूर्णाङ्क		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = रिक्त;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    अत्थाय (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((वर्ण *)fname_enc, परिमाण(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#अञ्ञथा
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	सचे (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((वर्ण *)fname_enc, परिमाण(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#अञ्ञथा
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    सचे (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		अनुवर्त;
	    भङ्ग;
	}
	भङ्ग;
    }

    सचे (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#अञ्ञथा
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    अञ्ञथा सचे (sl.sl_slang != रिक्त)
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
spell_enc(शून्य)
{

    सचे (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	पटिनिवत्त p_enc;
    पटिनिवत्त (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    स्थिर शून्य
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((वर्ण *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    सचे (lp != रिक्त)
    {
	सचे (lang != रिक्त)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, परिमाण(fromto_T), 10);
	ga_init2(&lp->sl_repsal, परिमाण(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    पटिनिवत्त lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    शून्य
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
    शून्य
free_sal_वस्तूनि(garray_T *gap)
{
    salitem_T	*smp;

    याव (gap->ga_len > 0)
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
    शून्य
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    पूर्णाङ्क		i;
    पूर्णाङ्क		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    अत्थाय (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	याव (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    सचे (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	सचे (gap->ga_data != रिक्त)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    अत्थाय (i = 0; i < gap->ga_len; ++i)
		vim_free(((पूर्णाङ्क **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    अञ्ञथा
	free_sal_वस्तूनि(gap);

    अत्थाय (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = रिक्त;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_वस्तूनि);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_कुञ्चिका_OFF);
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
    शून्य
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = रिक्त;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    स्थिर शून्य
spell_load_cb(char_u *fname, शून्य *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, रिक्त, FALSE);
    सचे (slang == रिक्त)
	पटिनिवत्त;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    सचे (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    अञ्ञथा सचे (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    शून्य
गणनम्_common_word(
    slang_T	*lp,
    char_u	*word,
    पूर्णाङ्क		len,	    // word length, -1 for up to NUL
    पूर्णाङ्क		गणनम्)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    सचे (len == -1)
	p = word;
    अञ्ञथा सचे (len >= MAXWLEN)
	पटिनिवत्त;
    अञ्ञथा
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    सचे (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	सचे (wc == रिक्त)
	    पटिनिवत्त;
	STRCPY(wc->wc_word, p);
	wc->wc_गणनम् = गणनम्;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    अञ्ञथा
    {
	wc = HI2WC(hi);
	सचे ((wc->wc_गणनम् += गणनम्) < (unsigned)गणनम्)	// check for overflow
	    wc->wc_गणनम् = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    पूर्णाङ्क
byte_in_str(char_u *str, पूर्णाङ्क n)
{
    char_u	*p;

    अत्थाय (p = str; *p != NUL; ++p)
	सचे (*p == n)
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

#define SY_MAXLEN   30
प्रकारनिर्देश संरचना syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    पूर्णाङ्क		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    पूर्णाङ्क
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    पूर्णाङ्क		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_वस्तूनि, परिमाण(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    याव (p != रिक्त)
    {
	*p++ = NUL;
	सचे (*p == NUL)	    // trailing slash
	    भङ्ग;
	s = p;
	p = vim_strchr(p, '/');
	सचे (p == रिक्त)
	    l = (पूर्णाङ्क)STRLEN(s);
	अञ्ञथा
	    l = (पूर्णाङ्क)(p - s);
	सचे (l >= SY_MAXLEN)
	    पटिनिवत्त SP_FORMERROR;
	सचे (ga_grow(&slang->sl_syl_वस्तूनि, 1) == FAIL)
	    पटिनिवत्त SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_वस्तूनि.ga_data)
					       + slang->sl_syl_वस्तूनि.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    पटिनिवत्त OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    स्थिर पूर्णाङ्क
गणनम्_syllables(slang_T *slang, char_u *word)
{
    पूर्णाङ्क		cnt = 0;
    पूर्णाङ्क		skip = FALSE;
    char_u	*p;
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;
    syl_item_T	*syl;
    पूर्णाङ्क		c;

    सचे (slang->sl_syllable == रिक्त)
	पटिनिवत्त 0;

    अत्थाय (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	सचे (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    अनुवर्त;
	}

	// Find longest match of syllable items.
	len = 0;
	अत्थाय (i = 0; i < slang->sl_syl_वस्तूनि.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_वस्तूनि.ga_data) + i;
	    सचे (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	सचे (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	अञ्ञथा
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    सचे (vim_strchr(slang->sl_syllable, c) == रिक्त)
		skip = FALSE;	    // No, search for next syllable
	    अञ्ञथा सचे (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    पटिनिवत्त cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    वर्ण *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    पूर्णाङ्क		filename;
    पूर्णाङ्क		region_mask;
    slang_T	*slang;
    पूर्णाङ्क		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    पूर्णाङ्क		len;
    char_u	*p;
    पूर्णाङ्क		round;
    char_u	*spf;
    char_u	*use_region = रिक्त;
    पूर्णाङ्क		dont_use_region = FALSE;
    पूर्णाङ्क		nobreak = FALSE;
    पूर्णाङ्क		i, j;
    langp_T	*lp, *lp2;
    स्थिर पूर्णाङ्क	recursive = FALSE;
    वर्ण	*ret_सन्देशः = रिक्त;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    सचे (recursive)
	पटिनिवत्त रिक्त;
    recursive = TRUE;

    ga_init2(&ga, परिमाण(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    सचे (spl_copy == रिक्त)
	गच्छ theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    अत्थाय (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = रिक्त;

	सचे (!valid_spelllang(lang))
	    अनुवर्त;

	सचे (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    अनुवर्त;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	सचे (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    सचे (p != रिक्त && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    अञ्ञथा
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		सचे (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    भङ्ग;
	}
	अञ्ञथा
	{
	    filename = FALSE;
	    सचे (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    अञ्ञथा
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		सचे (STRICMP(lang, slang->sl_name) == 0)
		    भङ्ग;
	}

	सचे (region != रिक्त)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    सचे (use_region != रिक्त && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	सचे (slang == रिक्त)
	{
	    सचे (filename)
		(शून्य)spell_load_file(lang, lang, रिक्त, FALSE);
	    अञ्ञथा
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		सचे (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_सन्देशः = N_(e_spellfilemising_autocommand_deleted_buffer);
		    गच्छ theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    सचे (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		सचे (!filename && region != रिक्त)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    सचे (c == REGION_ALL)
		    {
			सचे (slang->sl_add)
			{
			    सचे (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			अञ्ञथा
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    अञ्ञथा
			region_mask = 1 << c;
		}

		सचे (region_mask != 0)
		{
		    सचे (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_सन्देशः = e_out_of_memory;
			गच्छ theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    सचे (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    अत्थाय (round = 0; round == 0 || *spf != NUL; ++round)
    {
	सचे (round == 0)
	{
	    // Internal wordlist, if there is one.
	    सचे (int_wordlist == रिक्त)
		अनुवर्त;
	    int_wordlist_spl(spf_name);
	}
	अञ्ञथा
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    अत्थाय (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		सचे (p != रिक्त && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    भङ्ग;
	    }
	    सचे (c < ga.ga_len)
		अनुवर्त;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    सचे (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		भङ्ग;
	सचे (slang == रिक्त)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    सचे (round == 0)
		STRCPY(lang, "internal wordlist");
	    अञ्ञथा
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		सचे (p != रिक्त)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, रिक्त, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    सचे (slang != रिक्त && nobreak)
		slang->sl_nobreak = TRUE;
	}
	सचे (slang != रिक्त && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    सचे (use_region != रिक्त && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		सचे (c != REGION_ALL)
		    region_mask = 1 << c;
		अञ्ञथा सचे (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    सचे (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = रिक्त;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = रिक्त;
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
    अत्थाय (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	सचे (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	अञ्ञथा
	    // find first similar language that does sound folding
	    अत्थाय (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		सचे (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    भङ्ग;
		}
	    }

	// REP items
	सचे (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	अञ्ञथा
	    // find first similar language that has REP items
	    अत्थाय (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		सचे (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    भङ्ग;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    पटिनिवत्त ret_सन्देशः;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    स्थिर शून्य
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    स्थिर शून्य
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    सचे (lp->sl_midword == रिक्त)	    // there aren't any
	पटिनिवत्त;

    अत्थाय (p = lp->sl_midword; *p != NUL; )
	सचे (has_mbyte)
	{
	    पूर्णाङ्क	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    सचे (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    अञ्ञथा सचे (wp->w_s->b_spell_ismw_mb == रिक्त)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    अञ्ञथा
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (पूर्णाङ्क)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		सचे (bp != रिक्त)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	अञ्ञथा
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    स्थिर पूर्णाङ्क
find_region(char_u *rp, char_u *region)
{
    पूर्णाङ्क		i;

    अत्थाय (i = 0; ; i += 2)
    {
	सचे (rp[i] == NUL)
	    पटिनिवत्त REGION_ALL;
	सचे (rp[i] == region[0] && rp[i + 1] == region[1])
	    भङ्ग;
    }
    पटिनिवत्त i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    पूर्णाङ्क
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		firstcap;
    पूर्णाङ्क		allcap;
    पूर्णाङ्क		past_second = FALSE;	// past second word char

    // find first letter
    अत्थाय (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	सचे (end == रिक्त ? *p == NUL : p >= end)
	    पटिनिवत्त 0;	    // only non-word characters, illegal word
    सचे (has_mbyte)
	c = mb_ptr2char_adv(&p);
    अञ्ञथा
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    अत्थाय ( ; end == रिक्त ? *p != NUL : p < end; MB_PTR_ADV(p))
	सचे (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    सचे (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		सचे (past_second && allcap)
		    पटिनिवत्त WF_KEEPCAP;
		allcap = FALSE;
	    }
	    अञ्ञथा सचे (!allcap)
		// UlU -> KEEPCAP
		पटिनिवत्त WF_KEEPCAP;
	    past_second = TRUE;
	}

    सचे (allcap)
	पटिनिवत्त WF_ALLCAP;
    सचे (firstcap)
	पटिनिवत्त WF_ONECAP;
    पटिनिवत्त 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    शून्य
spell_delete_wordlist(शून्य)
{
    char_u	fname[MAXPATHL];

    सचे (int_wordlist == रिक्त)
	पटिनिवत्त;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    शून्य
spell_free_all(शून्य)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    याव (first_lang != रिक्त)
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
    शून्य
spell_reload(शून्य)
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
	सचे (*wp->w_s->b_p_spl != NUL)
	{
		सचे (wp->w_p_spell)
		{
		    (शून्य)parse_spelllang(wp);
		    भङ्ग;
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
open_spellbuf(शून्य)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    सचे (buf == रिक्त)
	पटिनिवत्त रिक्त;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_कुञ्चिका = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    पटिनिवत्त buf;
}

/*
 * Close the buffer used for spell info.
 */
    शून्य
close_spellbuf(buf_T *buf)
{
    सचे (buf == रिक्त)
	पटिनिवत्त;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    शून्य
clear_spell_chartab(spelltab_T *sp)
{
    पूर्णाङ्क		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    अत्थाय (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    अत्थाय (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    अत्थाय (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    अत्थाय (i = 'a'; i <= 'z'; ++i)
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
    शून्य
init_spell_chartab(शून्य)
{
    पूर्णाङ्क	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    सचे (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	अत्थाय (i = 128; i <= 255; ++i)
	    सचे (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    अञ्ञथा सचे (enc_utf8)
    {
	अत्थाय (i = 128; i < 256; ++i)
	{
	    पूर्णाङ्क f = utf_fold(i);
	    पूर्णाङ्क u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    अञ्ञथा
    {
	// Rough guess: use locale-dependent library functions.
	अत्थाय (i = 128; i < 256; ++i)
	{
	    सचे (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    अञ्ञथा सचे (MB_ISLOWER(i))
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
    पूर्णाङ्क
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    पूर्णाङ्क		l;
    पूर्णाङ्क		c;

    सचे (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	सचे (l == 1)
	{
	    // be quick for ASCII
	    सचे (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	अञ्ञथा
	{
	    c = mb_ptr2char(p);
	    सचे (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != रिक्त
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != रिक्त))
		s = p + l;
	}

	c = mb_ptr2char(s);
	सचे (c > 255)
	    पटिनिवत्त spell_mb_isword_class(mb_get_class(s), wp);
	पटिनिवत्त spelltab.st_isw[c];
    }

    पटिनिवत्त spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    पूर्णाङ्क
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    पूर्णाङ्क		c;

    सचे (has_mbyte)
    {
	c = mb_ptr2char(p);
	सचे (c > 255)
	    पटिनिवत्त spell_mb_isword_class(mb_get_class(p), wp);
	पटिनिवत्त spelltab.st_isw[c];
    }
    पटिनिवत्त spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    स्थिर पूर्णाङ्क
spell_mb_isword_class(पूर्णाङ्क cl, win_T *wp)
{
    सचे (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	पटिनिवत्त cl == 2 || cl == 0x2800;
    पटिनिवत्त cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    स्थिर पूर्णाङ्क
spell_iswordp_w(पूर्णाङ्क *p, win_T *wp)
{
    पूर्णाङ्क		*s;

    सचे (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != रिक्त
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != रिक्त))
	s = p + 1;
    अञ्ञथा
	s = p;

    सचे (*s > 255)
    {
	सचे (enc_utf8)
	    पटिनिवत्त spell_mb_isword_class(utf_class(*s), wp);
	सचे (enc_dbcs)
	    पटिनिवत्त spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	पटिनिवत्त 0;
    }
    पटिनिवत्त spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    पूर्णाङ्क
spell_casefold(
    win_T	*wp,
    char_u	*str,
    पूर्णाङ्क		len,
    char_u	*buf,
    पूर्णाङ्क		buflen)
{
    पूर्णाङ्क		i;

    सचे (len >= buflen)
    {
	buf[0] = NUL;
	पटिनिवत्त FAIL;		// result will not fit
    }

    सचे (has_mbyte)
    {
	पूर्णाङ्क	outi = 0;
	char_u	*p;
	पूर्णाङ्क	c;

	// Fold one character at a time.
	अत्थाय (p = str; p < str + len; )
	{
	    सचे (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		पटिनिवत्त FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    सचे (c == 0x03a3 || c == 0x03c2)
	    {
		सचे (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		अञ्ञथा
		    c = 0x03c3;
	    }
	    अञ्ञथा
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    अञ्ञथा
    {
	// Be quick for non-multibyte encodings.
	अत्थाय (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    पटिनिवत्त OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    पूर्णाङ्क
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    सचे (wp->w_s->b_cap_prog == रिक्त)
	पटिनिवत्त FALSE;

    पूर्णाङ्क		need_cap = FALSE;
    char_u	*पङ्क्तिः = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : रिक्त;
    char_u	*पङ्क्तिः_copy = रिक्त;
    colnr_T	endcol = 0;

    सचे (col == 0 || getwhitecols(पङ्क्तिः) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	सचे (lnum == 1)
	    need_cap = TRUE;
	अञ्ञथा
	{
	    पङ्क्तिः = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    सचे (*skipwhite(पङ्क्तिः) == NUL)
		need_cap = TRUE;
	    अञ्ञथा
	    {
		// Append a space in place of the line break.
		पङ्क्तिः_copy = concat_str(पङ्क्तिः, (char_u *)" ");
		सचे (पङ्क्तिः_copy == रिक्त)
		    पटिनिवत्त FALSE;

		पङ्क्तिः = पङ्क्तिः_copy;
		endcol = (colnr_T)STRLEN(पङ्क्तिः);
	    }
	}
    }
    अञ्ञथा
	endcol = col;

    सचे (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = पङ्क्तिः + endcol;
	अत्थाय (;;)
	{
	    MB_PTR_BACK(पङ्क्तिः, p);
	    सचे (p == पङ्क्तिः || spell_iswordp_nmw(p, wp))
		भङ्ग;
	    सचे (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == पङ्क्तिः + endcol)
	    {
		need_cap = TRUE;
		भङ्ग;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(पङ्क्तिः_copy);

    पटिनिवत्त need_cap;
}


/*
 * ":spellrepall"
 */
    शून्य
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*पङ्क्तिः;
    char_u	*p;
    पूर्णाङ्क		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    सचे (repl_from == रिक्त || repl_to == रिक्त)
    {
	emsg(_(e_no_previous_spell_replacement));
	पटिनिवत्त;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    सचे (frompat == रिक्त)
	पटिनिवत्त;
    frompatlen = vim_snprintf((वर्ण *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    याव (!got_int)
    {
	सचे (do_अन्वेषणम्(रिक्त, '/', '/', frompat, frompatlen, 1L, अन्वेषणम्_KEEP, रिक्त) == 0
						   || u_save_cursor() == FAIL)
	    भङ्ग;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	पङ्क्तिः = ml_get_curline();
	सचे (addlen <= 0 || STRNCMP(पङ्क्तिः + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    सचे (p == रिक्त)
		भङ्ग;
	    mch_memmove(p, पङ्क्तिः, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, पङ्क्तिः + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#सचे defined(FEAT_PROP_POPUP)
	    सचे (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (पूर्णाङ्क)addlen, APC_SUBSTITUTE);
#endif

	    सचे (curwin->w_cursor.lnum != prev_lnum)
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

    सचे (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    अञ्ञथा
	do_sub_सन्देशः(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    शून्य
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    पूर्णाङ्क		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		l;

    p = word;
    सचे (has_mbyte)
	c = mb_cptr2char_adv(&p);
    अञ्ञथा
	c = *p++;
    सचे (upper)
	c = SPELL_TOUPPER(c);
    अञ्ञथा
	c = SPELL_TOFOLD(c);
    सचे (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    अञ्ञथा
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
    शून्य
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    पूर्णाङ्क		c;

    d = wcopy;
    अत्थाय (s = word; *s != NUL; )
    {
	सचे (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	अञ्ञथा
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	सचे (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    सचे (d - wcopy >= MAXWLEN - 1)
		भङ्ग;
	    *d++ = c;
	}
	अञ्ञथा
	    c = SPELL_TOUPPER(c);

	सचे (has_mbyte)
	{
	    सचे (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		भङ्ग;
	    d += mb_char2bytes(c, d);
	}
	अञ्ञथा
	{
	    सचे (d - wcopy >= MAXWLEN - 1)
		भङ्ग;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    पूर्णाङ्क
nofold_len(char_u *fword, पूर्णाङ्क flen, char_u *word)
{
    char_u	*p;
    पूर्णाङ्क		i = 0;

    अत्थाय (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    अत्थाय (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    पटिनिवत्त (पूर्णाङ्क)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    शून्य
make_case_word(char_u *fword, char_u *cword, पूर्णाङ्क flags)
{
    सचे (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    अञ्ञथा सचे (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    अञ्ञथा
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#सचे defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    पूर्णाङ्क		lpi;

    सचे (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	अत्थाय (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    सचे (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		पटिनिवत्त vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    पटिनिवत्त vim_strsave(word);
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
    शून्य
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    पूर्णाङ्क		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    सचे (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    अञ्ञथा
    {
	// SAL items used.  Requires the word to be case-folded.
	सचे (folded)
	    word = inword;
	अञ्ञथा
	{
	    (शून्य)spell_casefold(curwin,
				  inword, (पूर्णाङ्क)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	सचे (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	अञ्ञथा
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    स्थिर शून्य
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    पूर्णाङ्क		ri = 0;
    पूर्णाङ्क		c;

    सचे (has_mbyte)
    {
	पूर्णाङ्क	prevc = 0;
	पूर्णाङ्क	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	अत्थाय (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    सचे (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    अञ्ञथा सचे (c < 256)
		c = slang->sl_sal_first[c];
	    अञ्ञथा
	    {
		ip = ((पूर्णाङ्क **)slang->sl_sal.ga_data)[c & 0xff];
		सचे (ip == रिक्त)		// empty list, can't match
		    c = NUL;
		अञ्ञथा
		    अत्थाय (;;)		// find "c" in the list
		    {
			सचे (*ip == 0)	// not found
			{
			    c = NUL;
			    भङ्ग;
			}
			सचे (*ip == c)	// match!
			{
			    c = ip[1];
			    भङ्ग;
			}
			ip += 2;
		    }
	    }

	    सचे (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		सचे (ri + MB_MAXBYTES > MAXWLEN)
		    भङ्ग;
		prevc = c;
	    }
	}
    }
    अञ्ञथा
    {
	// The sl_sal_first[] table contains the translation.
	अत्थाय (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    सचे (VIM_ISWHITE(c))
		c = ' ';
	    अञ्ञथा
		c = slang->sl_sal_first[c];
	    सचे (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    स्थिर शून्य
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    पूर्णाङ्क		i, j, z;
    पूर्णाङ्क		reslen;
    पूर्णाङ्क		n, k = 0;
    पूर्णाङ्क		z0;
    पूर्णाङ्क		k0;
    पूर्णाङ्क		n0;
    पूर्णाङ्क		c;
    पूर्णाङ्क		pri;
    पूर्णाङ्क		p0 = -333;
    पूर्णाङ्क		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    सचे (slang->sl_rem_accents)
    {
	t = word;
	याव (*s != NUL)
	{
	    सचे (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    अञ्ञथा
	    {
		सचे (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    अञ्ञथा
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    याव ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	सचे (n >= 0)
	{
	    // check all rules for the same letter
	    अत्थाय (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		सचे (k > 1)
		{
		    सचे (word[i + 1] != s[1])
			अनुवर्त;
		    सचे (k > 2)
		    {
			अत्थाय (j = 2; j < k; ++j)
			    सचे (word[i + j] != s[j])
				भङ्ग;
			सचे (j < k)
			    अनुवर्त;
		    }
		}

		सचे ((pf = smp[n].sm_oneof) != रिक्त)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    याव (*pf != NUL && *pf != word[i + k])
			++pf;
		    सचे (*pf == NUL)
			अनुवर्त;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		याव (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		सचे (*s == '<')
		    s++;
		सचे (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		सचे (*s == '^' && *(s + 1) == '^')
		    s++;

		सचे (*s == NUL
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

		    सचे (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			अत्थाय ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    सचे (k0 > 1)
			    {
				सचे (word[i + k] != s[1])
				    अनुवर्त;
				सचे (k0 > 2)
				{
				    pf = word + i + k + 1;
				    अत्थाय (j = 2; j < k0; ++j)
					सचे (*pf++ != s[j])
					    भङ्ग;
				    सचे (j < k0)
					अनुवर्त;
				}
			    }
			    k0 += k - 1;

			    सचे ((pf = smp[n0].sm_oneof) != रिक्त)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				याव (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				सचे (*pf == NUL)
				    अनुवर्त;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    याव (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    सचे (*s == '<')
				s++;
			    सचे (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    सचे (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				सचे (k0 == k)
				    // this is just a piece of the string
				    अनुवर्त;

				सचे (p0 < pri)
				    // priority too low
				    अनुवर्त;
				// rule fits; stop search
				भङ्ग;
			    }
			}

			सचे (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    अनुवर्त;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    सचे (s == रिक्त)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != रिक्त) ? 1 : 0;
		    सचे (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			सचे (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			याव (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			सचे (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    अञ्ञथा
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			याव (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    सचे (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			सचे (strstr((वर्ण *)pf, "^^") != रिक्त)
			{
			    सचे (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    भङ्ग;
		}
	    }
	}
	अञ्ञथा सचे (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	सचे (z0 == 0)
	{
	    सचे (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    स्थिर शून्य
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    पूर्णाङ्क		word[MAXWLEN];
    पूर्णाङ्क		wres[MAXWLEN];
    पूर्णाङ्क		l;
    char_u	*s;
    पूर्णाङ्क		*ws;
    char_u	*t;
    पूर्णाङ्क		*pf;
    पूर्णाङ्क		i, j, z;
    पूर्णाङ्क		reslen;
    पूर्णाङ्क		n, k = 0;
    पूर्णाङ्क		z0;
    पूर्णाङ्क		k0;
    पूर्णाङ्क		n0;
    पूर्णाङ्क		c;
    पूर्णाङ्क		pri;
    पूर्णाङ्क		p0 = -333;
    पूर्णाङ्क		c0;
    पूर्णाङ्क		did_white = FALSE;
    पूर्णाङ्क		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    अत्थाय (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	सचे (slang->sl_rem_accents)
	{
	    सचे (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		सचे (did_white)
		    अनुवर्त;
		c = ' ';
		did_white = TRUE;
	    }
	    अञ्ञथा
	    {
		did_white = FALSE;
		सचे (!spell_iswordp_nmw(t, curwin))
		    अनुवर्त;
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
    याव ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	सचे (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    अत्थाय (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		सचे (c != ws[0])
		    अनुवर्त;
		k = smp[n].sm_leadlen;
		सचे (k > 1)
		{
		    सचे (word[i + 1] != ws[1])
			अनुवर्त;
		    सचे (k > 2)
		    {
			अत्थाय (j = 2; j < k; ++j)
			    सचे (word[i + j] != ws[j])
				भङ्ग;
			सचे (j < k)
			    अनुवर्त;
		    }
		}

		सचे ((pf = smp[n].sm_oneof_w) != रिक्त)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    याव (*pf != NUL && *pf != word[i + k])
			++pf;
		    सचे (*pf == NUL)
			अनुवर्त;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		याव (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		सचे (*s == '<')
		    s++;
		सचे (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		सचे (*s == '^' && *(s + 1) == '^')
		    s++;

		सचे (*s == NUL
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

		    सचे (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			अत्थाय ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    सचे (c0 != ws[0])
				अनुवर्त;
			    k0 = smp[n0].sm_leadlen;
			    सचे (k0 > 1)
			    {
				सचे (word[i + k] != ws[1])
				    अनुवर्त;
				सचे (k0 > 2)
				{
				    pf = word + i + k + 1;
				    अत्थाय (j = 2; j < k0; ++j)
					सचे (*pf++ != ws[j])
					    भङ्ग;
				    सचे (j < k0)
					अनुवर्त;
				}
			    }
			    k0 += k - 1;

			    सचे ((pf = smp[n0].sm_oneof_w) != रिक्त)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				याव (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				सचे (*pf == NUL)
				    अनुवर्त;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    याव (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    सचे (*s == '<')
				s++;
			    सचे (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    सचे (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				सचे (k0 == k)
				    // this is just a piece of the string
				    अनुवर्त;

				सचे (p0 < pri)
				    // priority too low
				    अनुवर्त;
				// rule fits; stop search
				भङ्ग;
			    }
			}

			सचे (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    अनुवर्त;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != रिक्त) ? 1 : 0;
		    सचे (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			सचे (reslen > 0 && ws != रिक्त && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			सचे (ws != रिक्त)
			    याव (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			सचे (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    परिमाण(पूर्णाङ्क) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    अञ्ञथा
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			सचे (ws != रिक्त)
			    याव (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				सचे (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			सचे (ws == रिक्त)
			    c = NUL;
			अञ्ञथा
			    c = *ws;
			सचे (strstr((वर्ण *)s, "^^") != रिक्त)
			{
			    सचे (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       परिमाण(पूर्णाङ्क) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    भङ्ग;
		}
	    }
	}
	अञ्ञथा सचे (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	सचे (z0 == 0)
	{
	    सचे (k && !p0 && reslen < MAXWLEN && c != NUL
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
    अत्थाय (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	सचे (l + MB_MAXBYTES > MAXWLEN)
	    भङ्ग;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    शून्य
ex_spellinfo(exarg_T *eap UNUSED)
{
    पूर्णाङ्क		lpi;
    langp_T	*lp;
    char_u	*p;

    सचे (no_spell_checking(curwin))
	पटिनिवत्त;

    सन्देशः_start();
    अत्थाय (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	सन्देशः_puts("file: ");
	सन्देशः_puts((वर्ण *)lp->lp_slang->sl_fname);
	सन्देशः_putchar('\n');
	p = lp->lp_slang->sl_info;
	सचे (p != रिक्त)
	{
	    सन्देशः_puts((वर्ण *)p);
	    सन्देशः_putchar('\n');
	}
    }
    सन्देशः_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_गणनम्	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    शून्य
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    सचे (no_spell_checking(curwin))
	पटिनिवत्त;
    (शून्य)get_option_मानम्((char_u*)"spl", &dummy, &spl, रिक्त, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_मानम्_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_मानम्_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    सचे (!BUFEMPTY())
	पटिनिवत्त;

    spell_dump_compl(रिक्त, 0, रिक्त, eap->forceit ? DUMPFLAG_गणनम् : 0);

    // Delete the empty line that we started with.
    सचे (curbuf->b_ml.ml_पङ्क्तिः_गणनम् > 1)
	ml_delete(curbuf->b_ml.ml_पङ्क्तिः_गणनम्);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    शून्य
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    पूर्णाङ्क		ic,	    // ignore case
    पूर्णाङ्क		*dir,	    // direction for adding matches
    पूर्णाङ्क		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    पूर्णाङ्क		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    पूर्णाङ्क		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    पूर्णाङ्क		round;
    पूर्णाङ्क		depth;
    पूर्णाङ्क		n;
    पूर्णाङ्क		flags;
    char_u	*region_names = रिक्त;	    // region names being used
    पूर्णाङ्क		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    पूर्णाङ्क		lpi;
    पूर्णाङ्क		dumpflags = dumpflags_arg;
    पूर्णाङ्क		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    सचे (pat != रिक्त)
    {
	सचे (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	अञ्ञथा
	{
	    n = captype(pat, रिक्त);
	    सचे (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    अञ्ञथा सचे (n == WF_ALLCAP && (पूर्णाङ्क)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    अत्थाय (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	सचे (p[0] != 0)
	{
	    सचे (region_names == रिक्त)	    // first language with regions
		region_names = p;
	    अञ्ञथा सचे (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		भङ्ग;
	    }
	}
    }

    सचे (do_region && region_names != रिक्त && pat == रिक्त)
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    अञ्ञथा
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    अत्थाय (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	सचे (slang->sl_fbyts == रिक्त)	    // reloading failed
	    अनुवर्त;

	सचे (pat == रिक्त)
	{
	    vim_snprintf((वर्ण *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	सचे (pat != रिक्त && slang->sl_pbyts == रिक्त)
	    patlen = (पूर्णाङ्क)STRLEN(pat);
	अञ्ञथा
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	अत्थाय (round = 1; round <= 2; ++round)
	{
	    सचे (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    अञ्ञथा
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    सचे (byts == रिक्त)
		अनुवर्त;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    याव (depth >= 0 && !got_int
				  && (pat == रिक्त || !ins_compl_interrupted()))
	    {
		सचे (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    पङ्क्तिः_breakcheck();
		    ins_compl_check_कुञ्चिकाः(50, FALSE);
		}
		अञ्ञथा
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    सचे (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (पूर्णाङ्क)idxs[n];
			सचे ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    सचे (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    सचे (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				सचे (pat == रिक्त)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    सचे (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    अञ्ञथा
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
			सचे (depth <= patlen
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
    स्थिर शून्य
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    पूर्णाङ्क		*dir,
    पूर्णाङ्क		dumpflags,
    पूर्णाङ्क		wordflags,
    linenr_T	lnum)
{
    पूर्णाङ्क		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    पूर्णाङ्क		i;
    पूर्णाङ्क		flags = wordflags;

    सचे (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    सचे (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    सचे ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    अञ्ञथा
    {
	p = word;
	सचे ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, रिक्त) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    सचे (pat == रिक्त)
    {
	// Add flags and regions after a slash.
	सचे ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    सचे (keepcap)
		STRCAT(badword, "=");
	    सचे (flags & WF_BANNED)
		STRCAT(badword, "!");
	    अञ्ञथा सचे (flags & WF_RARE)
		STRCAT(badword, "?");
	    सचे (flags & WF_REGION)
		अत्थाय (i = 0; i < 7; ++i)
		    सचे (flags & (0x10000 << i))
			sprintf((वर्ण *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	सचे (dumpflags & DUMPFLAG_गणनम्)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    सचे (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((वर्ण *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_गणनम्);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    अञ्ञथा सचे (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (पूर्णाङ्क)STRLEN(p),
					  p_ic, रिक्त, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    स्थिर linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    पूर्णाङ्क		*dir,
    पूर्णाङ्क		dumpflags,
    पूर्णाङ्क		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    पूर्णाङ्क		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    पूर्णाङ्क		has_word_up = FALSE;
    पूर्णाङ्क		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    पूर्णाङ्क		depth;
    पूर्णाङ्क		n;
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    सचे (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    सचे (byts != रिक्त)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	याव (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    सचे (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		पङ्क्तिः_breakcheck();
	    }
	    अञ्ञथा
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		सचे (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    अत्थाय (i = 1; i < len; ++i)
			सचे (byts[n + i] != 0)
			    भङ्ग;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    सचे (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			सचे (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    सचे (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			सचे (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    सचे (lnum != 0)
				++lnum;
			}
		    }
		}
		अञ्ञथा सचे (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    पटिनिवत्त lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    याव (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    पटिनिवत्त p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    पूर्णाङ्क
spell_word_start(पूर्णाङ्क startcol)
{
    char_u	*पङ्क्तिः;
    char_u	*p;
    पूर्णाङ्क		col = 0;

    सचे (no_spell_checking(curwin))
	पटिनिवत्त startcol;

    // Find a word character before "startcol".
    पङ्क्तिः = ml_get_curline();
    अत्थाय (p = पङ्क्तिः + startcol; p > पङ्क्तिः; )
    {
	MB_PTR_BACK(पङ्क्तिः, p);
	सचे (spell_iswordp_nmw(p, curwin))
	    भङ्ग;
    }

    // Go back to start of the word.
    याव (p > पङ्क्तिः)
    {
	col = (पूर्णाङ्क)(p - पङ्क्तिः);
	MB_PTR_BACK(पङ्क्तिः, p);
	सचे (!spell_iswordp(p, curwin))
	    भङ्ग;
	col = 0;
    }

    पटिनिवत्त col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
स्थिर पूर्णाङ्क spell_expand_need_cap;

    शून्य
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
    पूर्णाङ्क
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    पटिनिवत्त ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    पूर्णाङ्क
valid_spelllang(char_u *val)
{
    पटिनिवत्त valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    पूर्णाङ्क
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    पूर्णाङ्क		l;

    spf = val;
    याव (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	सचे (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    पटिनिवत्त FALSE;
	अत्थाय (s = spf_name; *s != NUL; ++s)
	    सचे (!vim_is_fname_char(*s))
		पटिनिवत्त FALSE;
    }
    पटिनिवत्त TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    वर्ण *
did_set_spell_option(शून्य)
{
    वर्ण    *errmsg = रिक्त;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	सचे (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    भङ्ग;
	}
    पटिनिवत्त errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    वर्ण *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    सचे (synblock->b_p_spc == रिक्त || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = रिक्त;
    अञ्ञथा
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	सचे (re != रिक्त)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    सचे (synblock->b_cap_prog == रिक्त)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		पटिनिवत्त e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    पटिनिवत्त रिक्त;
}

#endif  // FEAT_SPELL
