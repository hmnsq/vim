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

#जर defined(FEAT_SPELL)

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
प्रकारव्याख्या संरचना matchinf_S
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
    पूर्णांक		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    पूर्णांक		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    पूर्णांक		mi_prefcnt;		// number of entries at mi_prefarridx
    पूर्णांक		mi_prefixlen;		// byte length of prefix
    पूर्णांक		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    पूर्णांक		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    पूर्णांक		mi_complen;		// nr of compound words used
    पूर्णांक		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    पूर्णांक		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    पूर्णांक		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    पूर्णांक		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


स्थिर पूर्णांक spell_mb_isword_class(पूर्णांक cl, win_T *wp);

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

स्थिर शून्य find_word(matchinf_T *mip, पूर्णांक mode);
स्थिर शून्य find_prefix(matchinf_T *mip, पूर्णांक mode);
स्थिर पूर्णांक fold_more(matchinf_T *mip);
स्थिर शून्य spell_load_cb(char_u *fname, शून्य *cookie);
स्थिर पूर्णांक मोजणी_syllables(slang_T *slang, char_u *word);
स्थिर शून्य clear_midword(win_T *buf);
स्थिर शून्य use_midword(slang_T *lp, win_T *buf);
स्थिर पूर्णांक find_region(char_u *rp, char_u *region);
स्थिर शून्य spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
स्थिर शून्य spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
स्थिर शून्य spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
स्थिर शून्य dump_word(slang_T *slang, char_u *word, char_u *pat, पूर्णांक *dir, पूर्णांक round, पूर्णांक flags, linenr_T lnum);
स्थिर linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, पूर्णांक *dir, पूर्णांक round, पूर्णांक flags, linenr_T startlnum);
स्थिर char_u *advance_camelcase_word(char_u *p, win_T *wp, पूर्णांक *is_camel_case);

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
    पूर्णांक
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    पूर्णांक		*capcol,	// column to check for Capital
    पूर्णांक		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    पूर्णांक		nrlen = 0;	// found a number first
    पूर्णांक		c;
    पूर्णांक		wrongcaplen = 0;
    पूर्णांक		lpi;
    पूर्णांक		मोजणी_word = docount;
    पूर्णांक		use_camel_case = *wp->w_s->b_p_spo != NUL;
    पूर्णांक		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    जर (*ptr <= ' ')
	परत 1;

    // Return here when loading language files failed.
    जर (wp->w_s->b_langp.ga_len == 0)
	परत 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    जर (*ptr >= '0' && *ptr <= '9')
    {
	जर (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	अन्यथा जर (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	अन्यथा
	    mi.mi_end = skipdigits(ptr);
	nrlen = (पूर्णांक)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    जर (spell_iswordp(mi.mi_fend, wp))
    {
	जर (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	अन्यथा
	{
	    करा
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } पर्यंत (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	जर (capcol != रिक्त && *capcol == 0 && wp->w_s->b_cap_prog != रिक्त)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    जर (!SPELL_ISUPPER(c))
		wrongcaplen = (पूर्णांक)(mi.mi_fend - ptr);
	}
    }
    जर (capcol != रिक्त)
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
    जर (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (शून्य)spell_casefold(wp, ptr, (पूर्णांक)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (पूर्णांक)STRLEN(mi.mi_fword);

    जर (is_camel_case && mi.mi_fwordlen > 0)
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
    साठी (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	जर (mi.mi_lp->lp_slang->sl_fidxs == रिक्त)
	    सुरूठेवा;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	जर (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	जर (मोजणी_word && mi.mi_result == SP_OK)
	{
	    मोजणी_common_word(mi.mi_lp->lp_slang, ptr,
						   (पूर्णांक)(mi.mi_end - ptr), 1);
	    मोजणी_word = FALSE;
	}
    }

    जर (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	जर (nrlen > 0)
	{
	    जर (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		परत nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	अन्यथा जर (!spell_iswordp_nmw(ptr, wp))
	{
	    जर (capcol != रिक्त && wp->w_s->b_cap_prog != रिक्त)
	    {
		regmatch_T	regmatch;
		पूर्णांक		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		जर (r)
		    *capcol = (पूर्णांक)(regmatch.endp[0] - ptr);
	    }

	    जर (has_mbyte)
		परत (*mb_ptr2len)(ptr);
	    परत 1;
	}
	अन्यथा जर (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	अन्यथा जर (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    पूर्णांक		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    जर (mi.mi_lp->lp_slang->sl_fidxs != रिक्त)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		साठी (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    जर (p >= mi.mi_end)
			थांबा;
		    mi.mi_compoff = (पूर्णांक)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    जर (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			थांबा;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	जर (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	अन्यथा जर (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	अन्यथा
	    *attrp = HLF_SPL;
    }

    जर (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	परत wrongcaplen;
    }

    परत (पूर्णांक)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    स्थिर पूर्णांक
get_char_type(पूर्णांक c)
{
    जर (VIM_ISDIGIT(c))
	परत CHAR_DIGIT;
    जर (SPELL_ISUPPER(c))
	परत CHAR_UPPER;
    परत CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    स्थिर char_u *
advance_camelcase_word(char_u *str, win_T *wp, पूर्णांक *is_camel_case)
{
    पूर्णांक last_type, last_last_type, this_type;
    पूर्णांक c;
    char_u *end = str;

    *is_camel_case = FALSE;

    जर (*str == NUL)
	परत str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    पर्यंत (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	जर (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    थांबा;
	}
	अन्यथा जर ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    थांबा;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    परत end;
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
find_word(matchinf_T *mip, पूर्णांक mode)
{
    idx_T	arridx = 0;
    पूर्णांक		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    पूर्णांक		endidxcnt = 0;
    पूर्णांक		len;
    पूर्णांक		wlen = 0;
    पूर्णांक		flen;
    पूर्णांक		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    पूर्णांक		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    अचिन्हित	flags;
    char_u	*byts;
    idx_T	*idxs;
    पूर्णांक		word_ends;
    पूर्णांक		prefix_found;
    पूर्णांक		nobreak_result;

    जर (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	जर (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    अन्यथा
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	जर (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	अन्यथा जर (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    जर (byts == रिक्त)
	परत;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    साठी (;;)
    {
	जर (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	जर (byts[arridx] == 0)
	{
	    जर (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		परत;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    पर्यंत (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    जर (len == 0)
		थांबा;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	जर (ptr[wlen] == NUL)
	    थांबा;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	जर (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	पर्यंत (lo < hi)
	{
	    m = (lo + hi) / 2;
	    जर (byts[m] > c)
		hi = m - 1;
	    अन्यथा जर (byts[m] < c)
		lo = m + 1;
	    अन्यथा
	    {
		lo = hi = m;
		थांबा;
	    }
	}

	// Stop if there is no matching byte.
	जर (hi < lo || byts[lo] != c)
	    थांबा;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	जर (c == ' ')
	{
	    साठी (;;)
	    {
		जर (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		जर (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    थांबा;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    पर्यंत (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	जर ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    सुरूठेवा;	    // not at first byte of character
	जर (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    जर (slang->sl_compprog == रिक्त && !slang->sl_nobreak)
		सुरूठेवा;	    // next char is a word character
	    word_ends = FALSE;
	}
	अन्यथा
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	जर (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    जर (STRNCMP(ptr, p, wlen) != 0)
	    {
		साठी (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (पूर्णांक)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	साठी (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    जर (mode == FIND_FOLDWORD)
	    {
		जर (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		जर (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    सुरूठेवा;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    अन्यथा जर (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		जर (c == 0)
		    सुरूठेवा;

		// Use the WF_RARE flag for a rare prefix.
		जर (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    जर (slang->sl_nobreak)
	    {
		जर ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    थांबा;
		}
	    }

	    अन्यथा जर ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		जर (((अचिन्हित)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    सुरूठेवा;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		जर (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			सुरूठेवा;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		जर (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    सुरूठेवा;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		जर (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    सुरूठेवा;
		जर (!word_ends && (flags & WF_NOCOMPAFT))
		    सुरूठेवा;

		// Quickly check if compounding is possible with this flag.
		जर (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((अचिन्हित)flags >> 24)))
		    सुरूठेवा;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		जर (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    सुरूठेवा;

		जर (mode == FIND_COMPOUND)
		{
		    पूर्णांक	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    जर (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			साठी (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    अन्यथा
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    जर (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			सुरूठेवा;

		    जर (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			जर (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    सुरूठेवा;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((अचिन्हित)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		जर (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    जर (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			जर (ptr == mip->mi_word)
			    (शून्य)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			अन्यथा
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    जर (!can_compound(slang, fword, mip->mi_compflags))
			सुरूठेवा;
		}
		अन्यथा जर (slang->sl_comprules != रिक्त
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    सुरूठेवा;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    अन्यथा जर (flags & WF_NEEDCOMP)
		सुरूठेवा;

	    nobreak_result = SP_OK;

	    जर (!word_ends)
	    {
		पूर्णांक	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		पूर्णांक	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		जर (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		जर (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    जर (STRNCMP(ptr, p, wlen) != 0)
		    {
			साठी (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (पूर्णांक)(p - mip->mi_fword);
		    }
		}
#जर 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		जर (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		साठी (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    जर (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			जर (mip->mi_lp->lp_slang->sl_fidxs == रिक्त
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    सुरूठेवा;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    जर (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#जर 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			जर (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    जर (!slang->sl_nobreak)
			थांबा;
		}
		--mip->mi_complen;
		जर (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		जर (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		अन्यथा
		{
		    जर (mip->mi_result == SP_OK)
			थांबा;
		    सुरूठेवा;
		}
	    }

	    जर (flags & WF_BANNED)
		res = SP_BANNED;
	    अन्यथा जर (flags & WF_REGION)
	    {
		// Check region.
		जर ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		अन्यथा
		    res = SP_LOCAL;
	    }
	    अन्यथा जर (flags & WF_RARE)
		res = SP_RARE;
	    अन्यथा
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    जर (nobreak_result == SP_BAD)
	    {
		जर (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		अन्यथा जर (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    अन्यथा जर (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    अन्यथा जर (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    जर (mip->mi_result == SP_OK)
		थांबा;
	}

	जर (mip->mi_result == SP_OK)
	    थांबा;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    पूर्णांक
match_checkcompoundpattern(
    char_u	*ptr,
    पूर्णांक		wlen,
    garray_T	*gap)  // &sl_comppat
{
    पूर्णांक		i;
    char_u	*p;
    पूर्णांक		len;

    साठी (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	जर (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (पूर्णांक)STRLEN(p);
	    जर (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		परत TRUE;
	}
    }
    परत FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    पूर्णांक
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    पूर्णांक		i;
    char_u	*p;

    जर (slang->sl_compprog == रिक्त)
	परत FALSE;
    जर (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	साठी (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    अन्यथा
	p = flags;
    जर (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	परत FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    जर (slang->sl_compsylmax < MAXWLEN
		       && मोजणी_syllables(slang, word) > slang->sl_compsylmax)
	परत (पूर्णांक)STRLEN(flags) < slang->sl_compmax;
    परत TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    पूर्णांक
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		c;

    // loop over all the COMPOUNDRULE entries
    साठी (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	साठी (i = 0; ; ++i)
	{
	    c = compflags[i];
	    जर (c == NUL)
		// found a rule that matches for the flags we have so far
		परत TRUE;
	    जर (*p == '/' || *p == NUL)
		थांबा;  // end of rule, it's too short
	    जर (*p == '[')
	    {
		पूर्णांक match = FALSE;

		// compare against all the flags in []
		++p;
		पर्यंत (*p != ']' && *p != NUL)
		    जर (*p++ == c)
			match = TRUE;
		जर (!match)
		    थांबा;  // none matches
	    }
	    अन्यथा जर (*p != c)
		थांबा;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	जर (p == रिक्त)
	    थांबा;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    परत FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    पूर्णांक
valid_word_prefix(
    पूर्णांक		totprefcnt,	// nr of prefix IDs
    पूर्णांक		arridx,		// idx in sl_pidxs[]
    पूर्णांक		flags,
    char_u	*word,
    slang_T	*slang,
    पूर्णांक		cond_req)	// only use prefixes with a condition
{
    पूर्णांक		prefcnt;
    पूर्णांक		pidx;
    regprog_T	**rp;
    पूर्णांक		prefid;

    prefid = (अचिन्हित)flags >> 24;
    साठी (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	जर (prefid != (pidx & 0xff))
	    सुरूठेवा;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	जर ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    सुरूठेवा;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((अचिन्हित)pidx >> 8) & 0xffff];
	जर (*rp != रिक्त)
	{
	    जर (!vim_regexec_prog(rp, FALSE, word, 0))
		सुरूठेवा;
	}
	अन्यथा जर (cond_req)
	    सुरूठेवा;

	// It's a match!  Return the WF_ flags.
	परत pidx;
    }
    परत 0;
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
find_prefix(matchinf_T *mip, पूर्णांक mode)
{
    idx_T	arridx = 0;
    पूर्णांक		len;
    पूर्णांक		wlen = 0;
    पूर्णांक		flen;
    पूर्णांक		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    जर (byts == रिक्त)
	परत;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    जर (mode == FIND_COMPOUND)
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
    साठी (;;)
    {
	जर (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	जर (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    पर्यंत (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    जर (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    जर (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    अन्यथा
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    जर (len == 0)
		थांबा;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	जर (ptr[wlen] == NUL)
	    थांबा;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	पर्यंत (lo < hi)
	{
	    m = (lo + hi) / 2;
	    जर (byts[m] > c)
		hi = m - 1;
	    अन्यथा जर (byts[m] < c)
		lo = m + 1;
	    अन्यथा
	    {
		lo = hi = m;
		थांबा;
	    }
	}

	// Stop if there is no matching byte.
	जर (hi < lo || byts[lo] != c)
	    थांबा;

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
    स्थिर पूर्णांक
fold_more(matchinf_T *mip)
{
    पूर्णांक		flen;
    char_u	*p;

    p = mip->mi_fend;
    करा
	MB_PTR_ADV(mip->mi_fend);
    पर्यंत (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    जर (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (शून्य)spell_casefold(mip->mi_win, p, (पूर्णांक)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (पूर्णांक)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    परत flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    पूर्णांक
spell_valid_case(
    पूर्णांक	    wordflags,	    // flags for the checked word.
    पूर्णांक	    treeflags)	    // flags for the word in the spell tree
{
    परत ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    पूर्णांक
spell_check_window(win_T *wp)
{
    परत wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(अक्षर **)(wp->w_s->b_langp.ga_data) != रिक्त;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    स्थिर पूर्णांक
no_spell_checking(win_T *wp)
{
    जर (spell_check_window(wp))
	परत FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    परत TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    पूर्णांक
spell_move_to(
    win_T	*wp,
    पूर्णांक		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    पूर्णांक		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    पूर्णांक		found_len = 0;
    char_u	*ओळ;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    पूर्णांक		len;
#ifdef FEAT_SYN_HL
    पूर्णांक		has_syntax = syntax_present(wp);
#endif
    पूर्णांक		col;
    पूर्णांक		can_spell;
    char_u	*buf = रिक्त;
    पूर्णांक		buflen = 0;
    पूर्णांक		skip = 0;
    पूर्णांक		capcol = -1;
    पूर्णांक		found_one = FALSE;
    पूर्णांक		wrapped = FALSE;

    जर (no_spell_checking(wp))
	परत 0;

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

    पर्यंत (!got_int)
    {
	पूर्णांक empty_ओळ;

	ओळ = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	जर (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    जर (buf == रिक्त)
		थांबा;
	}

	// In first line check first word for Capital.
	जर (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	जर (capcol == 0)
	    capcol = getwhitecols(ओळ);
	अन्यथा जर (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(ओळ);
	    जर (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    ओळ = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_ओळ = *skipwhite(ओळ) == NUL;
	STRCPY(buf, ओळ);
	जर (lnum < wp->w_buffer->b_ml.ml_ओळ_मोजणी)
	    spell_cat_ओळ(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	पर्यंत (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    जर (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		थांबा;

	    // start of word
	    attr = HLF_मोजणी;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    जर (attr != HLF_मोजणी)
	    {
		// We found a bad word.  Check the attribute.
		जर (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    जर (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			जर (has_syntax)
			{
			    col = (पूर्णांक)(p - buf);
			    (शून्य)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    जर (!can_spell)
				attr = HLF_मोजणी;
			}
			अन्यथा
#endif
			    can_spell = TRUE;

			जर (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (पूर्णांक)(p - buf);
			    found_pos.coladd = 0;
			    जर (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				जर (attrp != रिक्त)
				    *attrp = attr;
				परत len;
			    }
			    अन्यथा जर (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    अन्यथा
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	जर (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    परत found_len;
	}

	जर (curline)
	    थांबा;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	जर (lnum == wp->w_cursor.lnum && wrapped)
	    थांबा;

	// Advance to next line.
	जर (dir == BACKWARD)
	{
	    जर (lnum > 1)
		--lnum;
	    अन्यथा जर (!p_ws)
		थांबा;	    // at first line and 'nowrapscan'
	    अन्यथा
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_ओळ_मोजणी;
		wrapped = TRUE;
		जर (!shortmess(SHM_शोध))
		    give_warning((char_u *)_(शीर्ष_bot_संदेश), TRUE);
	    }
	    capcol = -1;
	}
	अन्यथा
	{
	    जर (lnum < wp->w_buffer->b_ml.ml_ओळ_मोजणी)
		++lnum;
	    अन्यथा जर (!p_ws)
		थांबा;	    // at first line and 'nowrapscan'
	    अन्यथा
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		जर (!shortmess(SHM_शोध))
		    give_warning((char_u *)_(bot_शीर्ष_संदेश), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    जर (lnum == wp->w_cursor.lnum && !found_one)
		थांबा;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    जर (attr == HLF_मोजणी)
		skip = (पूर्णांक)(p - endp);
	    अन्यथा
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    जर (empty_ओळ)
		capcol = 0;
	}

	ओळ_breakcheck();
    }

    vim_free(buf);
    परत 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    शून्य
spell_cat_ओळ(char_u *buf, char_u *ओळ, पूर्णांक maxlen)
{
    char_u	*p;
    पूर्णांक		n;

    p = skipwhite(ओळ);
    पर्यंत (vim_strchr((char_u *)"*#/\"\t", *p) != रिक्त)
	p = skipwhite(p + 1);

    जर (*p == NUL)
	परत;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (पूर्णांक)(p - ओळ) + 1;
    जर (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
प्रकारव्याख्या संरचना spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    पूर्णांक	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    स्थिर शून्य
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    पूर्णांक		r;
    spelload_T	sl;
    पूर्णांक		round;

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
    साठी (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((अक्षर *)fname_enc, आकार(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#अन्यथा
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	जर (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((अक्षर *)fname_enc, आकार(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#अन्यथा
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    जर (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		सुरूठेवा;
	    थांबा;
	}
	थांबा;
    }

    जर (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#अन्यथा
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    अन्यथा जर (sl.sl_slang != रिक्त)
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

    जर (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	परत p_enc;
    परत (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    स्थिर शून्य
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((अक्षर *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    जर (lp != रिक्त)
    {
	जर (lang != रिक्त)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, आकार(fromto_T), 10);
	ga_init2(&lp->sl_repsal, आकार(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    परत lp;
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
free_sal_वस्तुसंच(garray_T *gap)
{
    salitem_T	*smp;

    पर्यंत (gap->ga_len > 0)
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
    पूर्णांक		i;
    पूर्णांक		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    साठी (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	पर्यंत (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    जर (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	जर (gap->ga_data != रिक्त)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    साठी (i = 0; i < gap->ga_len; ++i)
		vim_free(((पूर्णांक **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    अन्यथा
	free_sal_वस्तुसंच(gap);

    साठी (i = 0; i < lp->sl_prefixcnt; ++i)
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
    ga_clear(&lp->sl_syl_वस्तुसंच);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_किल्ली_OFF);
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
    जर (slang == रिक्त)
	परत;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    जर (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    अन्यथा जर (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    शून्य
मोजणी_common_word(
    slang_T	*lp,
    char_u	*word,
    पूर्णांक		len,	    // word length, -1 for up to NUL
    पूर्णांक		मोजणी)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    जर (len == -1)
	p = word;
    अन्यथा जर (len >= MAXWLEN)
	परत;
    अन्यथा
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    जर (HASHITEM_EMPTY(hi))
    {
	wc = alloc(दुरत्व(wordcount_T, wc_word) + STRLEN(p) + 1);
	जर (wc == रिक्त)
	    परत;
	STRCPY(wc->wc_word, p);
	wc->wc_मोजणी = मोजणी;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    अन्यथा
    {
	wc = HI2WC(hi);
	जर ((wc->wc_मोजणी += मोजणी) < (अचिन्हित)मोजणी)	// check for overflow
	    wc->wc_मोजणी = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    पूर्णांक
byte_in_str(char_u *str, पूर्णांक n)
{
    char_u	*p;

    साठी (p = str; *p != NUL; ++p)
	जर (*p == n)
	    परत TRUE;
    परत FALSE;
}

#define SY_MAXLEN   30
प्रकारव्याख्या संरचना syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    पूर्णांक		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    पूर्णांक
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    पूर्णांक		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_वस्तुसंच, आकार(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    पर्यंत (p != रिक्त)
    {
	*p++ = NUL;
	जर (*p == NUL)	    // trailing slash
	    थांबा;
	s = p;
	p = vim_strchr(p, '/');
	जर (p == रिक्त)
	    l = (पूर्णांक)STRLEN(s);
	अन्यथा
	    l = (पूर्णांक)(p - s);
	जर (l >= SY_MAXLEN)
	    परत SP_FORMERROR;
	जर (ga_grow(&slang->sl_syl_वस्तुसंच, 1) == FAIL)
	    परत SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_वस्तुसंच.ga_data)
					       + slang->sl_syl_वस्तुसंच.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    परत OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    स्थिर पूर्णांक
मोजणी_syllables(slang_T *slang, char_u *word)
{
    पूर्णांक		cnt = 0;
    पूर्णांक		skip = FALSE;
    char_u	*p;
    पूर्णांक		len;
    पूर्णांक		i;
    syl_item_T	*syl;
    पूर्णांक		c;

    जर (slang->sl_syllable == रिक्त)
	परत 0;

    साठी (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	जर (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    सुरूठेवा;
	}

	// Find longest match of syllable items.
	len = 0;
	साठी (i = 0; i < slang->sl_syl_वस्तुसंच.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_वस्तुसंच.ga_data) + i;
	    जर (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	जर (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	अन्यथा
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    जर (vim_strchr(slang->sl_syllable, c) == रिक्त)
		skip = FALSE;	    // No, search for next syllable
	    अन्यथा जर (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    परत cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    अक्षर *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    पूर्णांक		filename;
    पूर्णांक		region_mask;
    slang_T	*slang;
    पूर्णांक		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    पूर्णांक		len;
    char_u	*p;
    पूर्णांक		round;
    char_u	*spf;
    char_u	*use_region = रिक्त;
    पूर्णांक		dont_use_region = FALSE;
    पूर्णांक		nobreak = FALSE;
    पूर्णांक		i, j;
    langp_T	*lp, *lp2;
    स्थिर पूर्णांक	recursive = FALSE;
    अक्षर	*ret_संदेश = रिक्त;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    जर (recursive)
	परत रिक्त;
    recursive = TRUE;

    ga_init2(&ga, आकार(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    जर (spl_copy == रिक्त)
	जा theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    साठी (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = रिक्त;

	जर (!valid_spelllang(lang))
	    सुरूठेवा;

	जर (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    सुरूठेवा;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	जर (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    जर (p != रिक्त && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    अन्यथा
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		जर (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    थांबा;
	}
	अन्यथा
	{
	    filename = FALSE;
	    जर (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    अन्यथा
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		जर (STRICMP(lang, slang->sl_name) == 0)
		    थांबा;
	}

	जर (region != रिक्त)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    जर (use_region != रिक्त && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	जर (slang == रिक्त)
	{
	    जर (filename)
		(शून्य)spell_load_file(lang, lang, रिक्त, FALSE);
	    अन्यथा
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		जर (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_संदेश = N_(e_spellfilemising_autocommand_deleted_buffer);
		    जा theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    जर (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		जर (!filename && region != रिक्त)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    जर (c == REGION_ALL)
		    {
			जर (slang->sl_add)
			{
			    जर (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			अन्यथा
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    अन्यथा
			region_mask = 1 << c;
		}

		जर (region_mask != 0)
		{
		    जर (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_संदेश = e_out_of_memory;
			जा theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    जर (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    साठी (round = 0; round == 0 || *spf != NUL; ++round)
    {
	जर (round == 0)
	{
	    // Internal wordlist, if there is one.
	    जर (int_wordlist == रिक्त)
		सुरूठेवा;
	    int_wordlist_spl(spf_name);
	}
	अन्यथा
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    साठी (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		जर (p != रिक्त && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    थांबा;
	    }
	    जर (c < ga.ga_len)
		सुरूठेवा;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    जर (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		थांबा;
	जर (slang == रिक्त)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    जर (round == 0)
		STRCPY(lang, "internal wordlist");
	    अन्यथा
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		जर (p != रिक्त)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, रिक्त, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    जर (slang != रिक्त && nobreak)
		slang->sl_nobreak = TRUE;
	}
	जर (slang != रिक्त && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    जर (use_region != रिक्त && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		जर (c != REGION_ALL)
		    region_mask = 1 << c;
		अन्यथा जर (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    जर (region_mask != 0)
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
    साठी (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	जर (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	अन्यथा
	    // find first similar language that does sound folding
	    साठी (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		जर (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    थांबा;
		}
	    }

	// REP items
	जर (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	अन्यथा
	    // find first similar language that has REP items
	    साठी (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		जर (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    थांबा;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    परत ret_संदेश;
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

    जर (lp->sl_midword == रिक्त)	    // there aren't any
	परत;

    साठी (p = lp->sl_midword; *p != NUL; )
	जर (has_mbyte)
	{
	    पूर्णांक	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    जर (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    अन्यथा जर (wp->w_s->b_spell_ismw_mb == रिक्त)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    अन्यथा
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (पूर्णांक)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		जर (bp != रिक्त)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	अन्यथा
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    स्थिर पूर्णांक
find_region(char_u *rp, char_u *region)
{
    पूर्णांक		i;

    साठी (i = 0; ; i += 2)
    {
	जर (rp[i] == NUL)
	    परत REGION_ALL;
	जर (rp[i] == region[0] && rp[i + 1] == region[1])
	    थांबा;
    }
    परत i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    पूर्णांक
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    पूर्णांक		c;
    पूर्णांक		firstcap;
    पूर्णांक		allcap;
    पूर्णांक		past_second = FALSE;	// past second word char

    // find first letter
    साठी (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	जर (end == रिक्त ? *p == NUL : p >= end)
	    परत 0;	    // only non-word characters, illegal word
    जर (has_mbyte)
	c = mb_ptr2char_adv(&p);
    अन्यथा
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    साठी ( ; end == रिक्त ? *p != NUL : p < end; MB_PTR_ADV(p))
	जर (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    जर (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		जर (past_second && allcap)
		    परत WF_KEEPCAP;
		allcap = FALSE;
	    }
	    अन्यथा जर (!allcap)
		// UlU -> KEEPCAP
		परत WF_KEEPCAP;
	    past_second = TRUE;
	}

    जर (allcap)
	परत WF_ALLCAP;
    जर (firstcap)
	परत WF_ONECAP;
    परत 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    शून्य
spell_delete_wordlist(शून्य)
{
    char_u	fname[MAXPATHL];

    जर (int_wordlist == रिक्त)
	परत;

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

    पर्यंत (first_lang != रिक्त)
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
	जर (*wp->w_s->b_p_spl != NUL)
	{
		जर (wp->w_p_spell)
		{
		    (शून्य)parse_spelllang(wp);
		    थांबा;
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
    जर (buf == रिक्त)
	परत रिक्त;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_किल्ली = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    परत buf;
}

/*
 * Close the buffer used for spell info.
 */
    शून्य
close_spellbuf(buf_T *buf)
{
    जर (buf == रिक्त)
	परत;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    शून्य
clear_spell_chartab(spelltab_T *sp)
{
    पूर्णांक		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    साठी (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    साठी (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    साठी (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    साठी (i = 'a'; i <= 'z'; ++i)
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
    पूर्णांक	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    जर (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	साठी (i = 128; i <= 255; ++i)
	    जर (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    अन्यथा जर (enc_utf8)
    {
	साठी (i = 128; i < 256; ++i)
	{
	    पूर्णांक f = utf_fold(i);
	    पूर्णांक u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    अन्यथा
    {
	// Rough guess: use locale-dependent library functions.
	साठी (i = 128; i < 256; ++i)
	{
	    जर (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    अन्यथा जर (MB_ISLOWER(i))
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
    पूर्णांक
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    पूर्णांक		l;
    पूर्णांक		c;

    जर (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	जर (l == 1)
	{
	    // be quick for ASCII
	    जर (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	अन्यथा
	{
	    c = mb_ptr2char(p);
	    जर (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != रिक्त
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != रिक्त))
		s = p + l;
	}

	c = mb_ptr2char(s);
	जर (c > 255)
	    परत spell_mb_isword_class(mb_get_class(s), wp);
	परत spelltab.st_isw[c];
    }

    परत spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    पूर्णांक
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    पूर्णांक		c;

    जर (has_mbyte)
    {
	c = mb_ptr2char(p);
	जर (c > 255)
	    परत spell_mb_isword_class(mb_get_class(p), wp);
	परत spelltab.st_isw[c];
    }
    परत spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    स्थिर पूर्णांक
spell_mb_isword_class(पूर्णांक cl, win_T *wp)
{
    जर (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	परत cl == 2 || cl == 0x2800;
    परत cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    स्थिर पूर्णांक
spell_iswordp_w(पूर्णांक *p, win_T *wp)
{
    पूर्णांक		*s;

    जर (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != रिक्त
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != रिक्त))
	s = p + 1;
    अन्यथा
	s = p;

    जर (*s > 255)
    {
	जर (enc_utf8)
	    परत spell_mb_isword_class(utf_class(*s), wp);
	जर (enc_dbcs)
	    परत spell_mb_isword_class(
				dbcs_class((अचिन्हित)*s >> 8, *s & 0xff), wp);
	परत 0;
    }
    परत spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    पूर्णांक
spell_casefold(
    win_T	*wp,
    char_u	*str,
    पूर्णांक		len,
    char_u	*buf,
    पूर्णांक		buflen)
{
    पूर्णांक		i;

    जर (len >= buflen)
    {
	buf[0] = NUL;
	परत FAIL;		// result will not fit
    }

    जर (has_mbyte)
    {
	पूर्णांक	outi = 0;
	char_u	*p;
	पूर्णांक	c;

	// Fold one character at a time.
	साठी (p = str; p < str + len; )
	{
	    जर (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		परत FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    जर (c == 0x03a3 || c == 0x03c2)
	    {
		जर (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		अन्यथा
		    c = 0x03c3;
	    }
	    अन्यथा
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    अन्यथा
    {
	// Be quick for non-multibyte encodings.
	साठी (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    परत OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    पूर्णांक
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    जर (wp->w_s->b_cap_prog == रिक्त)
	परत FALSE;

    पूर्णांक		need_cap = FALSE;
    char_u	*ओळ = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : रिक्त;
    char_u	*ओळ_copy = रिक्त;
    colnr_T	endcol = 0;

    जर (col == 0 || getwhitecols(ओळ) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	जर (lnum == 1)
	    need_cap = TRUE;
	अन्यथा
	{
	    ओळ = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    जर (*skipwhite(ओळ) == NUL)
		need_cap = TRUE;
	    अन्यथा
	    {
		// Append a space in place of the line break.
		ओळ_copy = concat_str(ओळ, (char_u *)" ");
		जर (ओळ_copy == रिक्त)
		    परत FALSE;

		ओळ = ओळ_copy;
		endcol = (colnr_T)STRLEN(ओळ);
	    }
	}
    }
    अन्यथा
	endcol = col;

    जर (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = ओळ + endcol;
	साठी (;;)
	{
	    MB_PTR_BACK(ओळ, p);
	    जर (p == ओळ || spell_iswordp_nmw(p, wp))
		थांबा;
	    जर (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == ओळ + endcol)
	    {
		need_cap = TRUE;
		थांबा;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(ओळ_copy);

    परत need_cap;
}


/*
 * ":spellrepall"
 */
    शून्य
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    माप_प्रकार	frompatlen;
    char_u	*ओळ;
    char_u	*p;
    पूर्णांक		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    जर (repl_from == रिक्त || repl_to == रिक्त)
    {
	emsg(_(e_no_previous_spell_replacement));
	परत;
    }
    माप_प्रकार	repl_from_len = STRLEN(repl_from);
    माप_प्रकार	repl_to_len = STRLEN(repl_to);
    दीर्घ	addlen = (दीर्घ)repl_to_len - (दीर्घ)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    जर (frompat == रिक्त)
	परत;
    frompatlen = vim_snprintf((अक्षर *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    पर्यंत (!got_int)
    {
	जर (do_शोध(रिक्त, '/', '/', frompat, frompatlen, 1L, शोध_KEEP, रिक्त) == 0
						   || u_save_cursor() == FAIL)
	    थांबा;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	ओळ = ml_get_curline();
	जर (addlen <= 0 || STRNCMP(ओळ + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    जर (p == रिक्त)
		थांबा;
	    mch_memmove(p, ओळ, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, ओळ + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#जर defined(FEAT_PROP_POPUP)
	    जर (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (पूर्णांक)addlen, APC_SUBSTITUTE);
#endif

	    जर (curwin->w_cursor.lnum != prev_lnum)
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

    जर (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    अन्यथा
	do_sub_संदेश(FALSE);
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
    पूर्णांक		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    पूर्णांक		c;
    पूर्णांक		l;

    p = word;
    जर (has_mbyte)
	c = mb_cptr2char_adv(&p);
    अन्यथा
	c = *p++;
    जर (upper)
	c = SPELL_TOUPPER(c);
    अन्यथा
	c = SPELL_TOFOLD(c);
    जर (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    अन्यथा
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
    पूर्णांक		c;

    d = wcopy;
    साठी (s = word; *s != NUL; )
    {
	जर (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	अन्यथा
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	जर (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    जर (d - wcopy >= MAXWLEN - 1)
		थांबा;
	    *d++ = c;
	}
	अन्यथा
	    c = SPELL_TOUPPER(c);

	जर (has_mbyte)
	{
	    जर (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		थांबा;
	    d += mb_char2bytes(c, d);
	}
	अन्यथा
	{
	    जर (d - wcopy >= MAXWLEN - 1)
		थांबा;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    पूर्णांक
nofold_len(char_u *fword, पूर्णांक flen, char_u *word)
{
    char_u	*p;
    पूर्णांक		i = 0;

    साठी (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    साठी (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    परत (पूर्णांक)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    शून्य
make_case_word(char_u *fword, char_u *cword, पूर्णांक flags)
{
    जर (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    अन्यथा जर (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    अन्यथा
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#जर defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    पूर्णांक		lpi;

    जर (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	साठी (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    जर (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		परत vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    परत vim_strsave(word);
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
    पूर्णांक		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    जर (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    अन्यथा
    {
	// SAL items used.  Requires the word to be case-folded.
	जर (folded)
	    word = inword;
	अन्यथा
	{
	    (शून्य)spell_casefold(curwin,
				  inword, (पूर्णांक)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	जर (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	अन्यथा
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
    पूर्णांक		ri = 0;
    पूर्णांक		c;

    जर (has_mbyte)
    {
	पूर्णांक	prevc = 0;
	पूर्णांक	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	साठी (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    जर (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    अन्यथा जर (c < 256)
		c = slang->sl_sal_first[c];
	    अन्यथा
	    {
		ip = ((पूर्णांक **)slang->sl_sal.ga_data)[c & 0xff];
		जर (ip == रिक्त)		// empty list, can't match
		    c = NUL;
		अन्यथा
		    साठी (;;)		// find "c" in the list
		    {
			जर (*ip == 0)	// not found
			{
			    c = NUL;
			    थांबा;
			}
			जर (*ip == c)	// match!
			{
			    c = ip[1];
			    थांबा;
			}
			ip += 2;
		    }
	    }

	    जर (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		जर (ri + MB_MAXBYTES > MAXWLEN)
		    थांबा;
		prevc = c;
	    }
	}
    }
    अन्यथा
    {
	// The sl_sal_first[] table contains the translation.
	साठी (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    जर (VIM_ISWHITE(c))
		c = ' ';
	    अन्यथा
		c = slang->sl_sal_first[c];
	    जर (c != NUL && (ri == 0 || res[ri - 1] != c))
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
    पूर्णांक		i, j, z;
    पूर्णांक		reslen;
    पूर्णांक		n, k = 0;
    पूर्णांक		z0;
    पूर्णांक		k0;
    पूर्णांक		n0;
    पूर्णांक		c;
    पूर्णांक		pri;
    पूर्णांक		p0 = -333;
    पूर्णांक		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    जर (slang->sl_rem_accents)
    {
	t = word;
	पर्यंत (*s != NUL)
	{
	    जर (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    अन्यथा
	    {
		जर (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    अन्यथा
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    पर्यंत ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	जर (n >= 0)
	{
	    // check all rules for the same letter
	    साठी (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		जर (k > 1)
		{
		    जर (word[i + 1] != s[1])
			सुरूठेवा;
		    जर (k > 2)
		    {
			साठी (j = 2; j < k; ++j)
			    जर (word[i + j] != s[j])
				थांबा;
			जर (j < k)
			    सुरूठेवा;
		    }
		}

		जर ((pf = smp[n].sm_oneof) != रिक्त)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    पर्यंत (*pf != NUL && *pf != word[i + k])
			++pf;
		    जर (*pf == NUL)
			सुरूठेवा;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		पर्यंत (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		जर (*s == '<')
		    s++;
		जर (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		जर (*s == '^' && *(s + 1) == '^')
		    s++;

		जर (*s == NUL
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

		    जर (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			साठी ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    जर (k0 > 1)
			    {
				जर (word[i + k] != s[1])
				    सुरूठेवा;
				जर (k0 > 2)
				{
				    pf = word + i + k + 1;
				    साठी (j = 2; j < k0; ++j)
					जर (*pf++ != s[j])
					    थांबा;
				    जर (j < k0)
					सुरूठेवा;
				}
			    }
			    k0 += k - 1;

			    जर ((pf = smp[n0].sm_oneof) != रिक्त)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				पर्यंत (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				जर (*pf == NUL)
				    सुरूठेवा;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    पर्यंत (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    जर (*s == '<')
				s++;
			    जर (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    जर (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				जर (k0 == k)
				    // this is just a piece of the string
				    सुरूठेवा;

				जर (p0 < pri)
				    // priority too low
				    सुरूठेवा;
				// rule fits; stop search
				थांबा;
			    }
			}

			जर (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    सुरूठेवा;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    जर (s == रिक्त)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != रिक्त) ? 1 : 0;
		    जर (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			जर (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			पर्यंत (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			जर (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    अन्यथा
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			पर्यंत (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    जर (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			जर (म_माला((अक्षर *)pf, "^^") != रिक्त)
			{
			    जर (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    थांबा;
		}
	    }
	}
	अन्यथा जर (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	जर (z0 == 0)
	{
	    जर (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    पूर्णांक		word[MAXWLEN];
    पूर्णांक		wres[MAXWLEN];
    पूर्णांक		l;
    char_u	*s;
    पूर्णांक		*ws;
    char_u	*t;
    पूर्णांक		*pf;
    पूर्णांक		i, j, z;
    पूर्णांक		reslen;
    पूर्णांक		n, k = 0;
    पूर्णांक		z0;
    पूर्णांक		k0;
    पूर्णांक		n0;
    पूर्णांक		c;
    पूर्णांक		pri;
    पूर्णांक		p0 = -333;
    पूर्णांक		c0;
    पूर्णांक		did_white = FALSE;
    पूर्णांक		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    साठी (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	जर (slang->sl_rem_accents)
	{
	    जर (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		जर (did_white)
		    सुरूठेवा;
		c = ' ';
		did_white = TRUE;
	    }
	    अन्यथा
	    {
		did_white = FALSE;
		जर (!spell_iswordp_nmw(t, curwin))
		    सुरूठेवा;
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
    पर्यंत ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	जर (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    साठी (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		जर (c != ws[0])
		    सुरूठेवा;
		k = smp[n].sm_leadlen;
		जर (k > 1)
		{
		    जर (word[i + 1] != ws[1])
			सुरूठेवा;
		    जर (k > 2)
		    {
			साठी (j = 2; j < k; ++j)
			    जर (word[i + j] != ws[j])
				थांबा;
			जर (j < k)
			    सुरूठेवा;
		    }
		}

		जर ((pf = smp[n].sm_oneof_w) != रिक्त)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    पर्यंत (*pf != NUL && *pf != word[i + k])
			++pf;
		    जर (*pf == NUL)
			सुरूठेवा;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		पर्यंत (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		जर (*s == '<')
		    s++;
		जर (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		जर (*s == '^' && *(s + 1) == '^')
		    s++;

		जर (*s == NUL
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

		    जर (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			साठी ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    जर (c0 != ws[0])
				सुरूठेवा;
			    k0 = smp[n0].sm_leadlen;
			    जर (k0 > 1)
			    {
				जर (word[i + k] != ws[1])
				    सुरूठेवा;
				जर (k0 > 2)
				{
				    pf = word + i + k + 1;
				    साठी (j = 2; j < k0; ++j)
					जर (*pf++ != ws[j])
					    थांबा;
				    जर (j < k0)
					सुरूठेवा;
				}
			    }
			    k0 += k - 1;

			    जर ((pf = smp[n0].sm_oneof_w) != रिक्त)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				पर्यंत (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				जर (*pf == NUL)
				    सुरूठेवा;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    पर्यंत (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    जर (*s == '<')
				s++;
			    जर (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    जर (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				जर (k0 == k)
				    // this is just a piece of the string
				    सुरूठेवा;

				जर (p0 < pri)
				    // priority too low
				    सुरूठेवा;
				// rule fits; stop search
				थांबा;
			    }
			}

			जर (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    सुरूठेवा;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != रिक्त) ? 1 : 0;
		    जर (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			जर (reslen > 0 && ws != रिक्त && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			जर (ws != रिक्त)
			    पर्यंत (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			जर (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    आकार(पूर्णांक) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    अन्यथा
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			जर (ws != रिक्त)
			    पर्यंत (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				जर (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			जर (ws == रिक्त)
			    c = NUL;
			अन्यथा
			    c = *ws;
			जर (म_माला((अक्षर *)s, "^^") != रिक्त)
			{
			    जर (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       आकार(पूर्णांक) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    थांबा;
		}
	    }
	}
	अन्यथा जर (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	जर (z0 == 0)
	{
	    जर (k && !p0 && reslen < MAXWLEN && c != NUL
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
    साठी (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	जर (l + MB_MAXBYTES > MAXWLEN)
	    थांबा;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    शून्य
ex_spellinfo(exarg_T *eap UNUSED)
{
    पूर्णांक		lpi;
    langp_T	*lp;
    char_u	*p;

    जर (no_spell_checking(curwin))
	परत;

    संदेश_start();
    साठी (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	संदेश_puts("file: ");
	संदेश_puts((अक्षर *)lp->lp_slang->sl_fname);
	संदेश_putchar('\n');
	p = lp->lp_slang->sl_info;
	जर (p != रिक्त)
	{
	    संदेश_puts((अक्षर *)p);
	    संदेश_putchar('\n');
	}
    }
    संदेश_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_मोजणी	    2	// include word count
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
    दीर्घ    dummy;

    जर (no_spell_checking(curwin))
	परत;
    (शून्य)get_option_मूल्य((char_u*)"spl", &dummy, &spl, रिक्त, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_मूल्य_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_मूल्य_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    जर (!BUFEMPTY())
	परत;

    spell_dump_compl(रिक्त, 0, रिक्त, eap->forceit ? DUMPFLAG_मोजणी : 0);

    // Delete the empty line that we started with.
    जर (curbuf->b_ml.ml_ओळ_मोजणी > 1)
	ml_delete(curbuf->b_ml.ml_ओळ_मोजणी);

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
    पूर्णांक		ic,	    // ignore case
    पूर्णांक		*dir,	    // direction for adding matches
    पूर्णांक		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    पूर्णांक		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    पूर्णांक		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    पूर्णांक		round;
    पूर्णांक		depth;
    पूर्णांक		n;
    पूर्णांक		flags;
    char_u	*region_names = रिक्त;	    // region names being used
    पूर्णांक		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    पूर्णांक		lpi;
    पूर्णांक		dumpflags = dumpflags_arg;
    पूर्णांक		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    जर (pat != रिक्त)
    {
	जर (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	अन्यथा
	{
	    n = captype(pat, रिक्त);
	    जर (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    अन्यथा जर (n == WF_ALLCAP && (पूर्णांक)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    साठी (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	जर (p[0] != 0)
	{
	    जर (region_names == रिक्त)	    // first language with regions
		region_names = p;
	    अन्यथा जर (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		थांबा;
	    }
	}
    }

    जर (do_region && region_names != रिक्त && pat == रिक्त)
    {
	vim_snprintf((अक्षर *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    अन्यथा
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    साठी (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	जर (slang->sl_fbyts == रिक्त)	    // reloading failed
	    सुरूठेवा;

	जर (pat == रिक्त)
	{
	    vim_snprintf((अक्षर *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	जर (pat != रिक्त && slang->sl_pbyts == रिक्त)
	    patlen = (पूर्णांक)STRLEN(pat);
	अन्यथा
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	साठी (round = 1; round <= 2; ++round)
	{
	    जर (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    अन्यथा
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    जर (byts == रिक्त)
		सुरूठेवा;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    पर्यंत (depth >= 0 && !got_int
				  && (pat == रिक्त || !ins_compl_interrupted()))
	    {
		जर (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    ओळ_breakcheck();
		    ins_compl_check_किल्ल्या(50, FALSE);
		}
		अन्यथा
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    जर (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (पूर्णांक)idxs[n];
			जर ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((अचिन्हित)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    जर (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (अचिन्हित)flags >> 24;
			    जर (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				जर (pat == रिक्त)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    जर (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    अन्यथा
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
			जर (depth <= patlen
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
    पूर्णांक		*dir,
    पूर्णांक		dumpflags,
    पूर्णांक		wordflags,
    linenr_T	lnum)
{
    पूर्णांक		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    पूर्णांक		i;
    पूर्णांक		flags = wordflags;

    जर (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    जर (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    जर ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    अन्यथा
    {
	p = word;
	जर ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, रिक्त) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    जर (pat == रिक्त)
    {
	// Add flags and regions after a slash.
	जर ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    जर (keepcap)
		STRCAT(badword, "=");
	    जर (flags & WF_BANNED)
		STRCAT(badword, "!");
	    अन्यथा जर (flags & WF_RARE)
		STRCAT(badword, "?");
	    जर (flags & WF_REGION)
		साठी (i = 0; i < 7; ++i)
		    जर (flags & (0x10000 << i))
			प्र_लिखो((अक्षर *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	जर (dumpflags & DUMPFLAG_मोजणी)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    जर (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((अक्षर *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_मोजणी);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    अन्यथा जर (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (पूर्णांक)STRLEN(p),
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
    पूर्णांक		*dir,
    पूर्णांक		dumpflags,
    पूर्णांक		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    पूर्णांक		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    पूर्णांक		has_word_up = FALSE;
    पूर्णांक		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    पूर्णांक		depth;
    पूर्णांक		n;
    पूर्णांक		len;
    पूर्णांक		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    जर (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    जर (byts != रिक्त)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	पर्यंत (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    जर (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		ओळ_breakcheck();
	    }
	    अन्यथा
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		जर (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    साठी (i = 1; i < len; ++i)
			जर (byts[n + i] != 0)
			    थांबा;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    जर (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			जर (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    जर (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			जर (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    जर (lnum != 0)
				++lnum;
			}
		    }
		}
		अन्यथा जर (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    परत lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    पर्यंत (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    परत p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    पूर्णांक
spell_word_start(पूर्णांक startcol)
{
    char_u	*ओळ;
    char_u	*p;
    पूर्णांक		col = 0;

    जर (no_spell_checking(curwin))
	परत startcol;

    // Find a word character before "startcol".
    ओळ = ml_get_curline();
    साठी (p = ओळ + startcol; p > ओळ; )
    {
	MB_PTR_BACK(ओळ, p);
	जर (spell_iswordp_nmw(p, curwin))
	    थांबा;
    }

    // Go back to start of the word.
    पर्यंत (p > ओळ)
    {
	col = (पूर्णांक)(p - ओळ);
	MB_PTR_BACK(ओळ, p);
	जर (!spell_iswordp(p, curwin))
	    थांबा;
	col = 0;
    }

    परत col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
स्थिर पूर्णांक spell_expand_need_cap;

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
    पूर्णांक
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    परत ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    पूर्णांक
valid_spelllang(char_u *val)
{
    परत valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    पूर्णांक
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    पूर्णांक		l;

    spf = val;
    पर्यंत (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	जर (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    परत FALSE;
	साठी (s = spf_name; *s != NUL; ++s)
	    जर (!vim_is_fname_char(*s))
		परत FALSE;
    }
    परत TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    अक्षर *
did_set_spell_option(शून्य)
{
    अक्षर    *errmsg = रिक्त;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	जर (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    थांबा;
	}
    परत errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    अक्षर *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    जर (synblock->b_p_spc == रिक्त || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = रिक्त;
    अन्यथा
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	जर (re != रिक्त)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    जर (synblock->b_cap_prog == रिक्त)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		परत e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    परत रिक्त;
}

#endif  // FEAT_SPELL
