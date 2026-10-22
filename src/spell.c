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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_SPELL)

#அந்தர்பாவ <time.h>

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
வகைவரை கட்டமைப்பு matchinf_S
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
    முழு		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    முழு		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    முழு		mi_prefcnt;		// number of entries at mi_prefarridx
    முழு		mi_prefixlen;		// byte length of prefix
    முழு		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    முழு		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    முழு		mi_complen;		// nr of compound words used
    முழு		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    முழு		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    முழு		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    முழு		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


நிலையான முழு spell_mb_isword_class(முழு cl, win_T *wp);

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

நிலையான வெறுமை find_word(matchinf_T *mip, முழு mode);
நிலையான வெறுமை find_prefix(matchinf_T *mip, முழு mode);
நிலையான முழு fold_more(matchinf_T *mip);
நிலையான வெறுமை spell_load_cb(char_u *fname, வெறுமை *cookie);
நிலையான முழு மொத்தம்_syllables(slang_T *slang, char_u *word);
நிலையான வெறுமை clear_midword(win_T *buf);
நிலையான வெறுமை use_midword(slang_T *lp, win_T *buf);
நிலையான முழு find_region(char_u *rp, char_u *region);
நிலையான வெறுமை spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
நிலையான வெறுமை spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
நிலையான வெறுமை spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
நிலையான வெறுமை dump_word(slang_T *slang, char_u *word, char_u *pat, முழு *dir, முழு round, முழு flags, linenr_T lnum);
நிலையான linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, முழு *dir, முழு round, முழு flags, linenr_T startlnum);
நிலையான char_u *advance_camelcase_word(char_u *p, win_T *wp, முழு *is_camel_case);

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
    முழு
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    முழு		*capcol,	// column to check for Capital
    முழு		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    முழு		nrlen = 0;	// found a number first
    முழு		c;
    முழு		wrongcaplen = 0;
    முழு		lpi;
    முழு		மொத்தம்_word = docount;
    முழு		use_camel_case = *wp->w_s->b_p_spo != NUL;
    முழு		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    எனில் (*ptr <= ' ')
	திரும்பு 1;

    // Return here when loading language files failed.
    எனில் (wp->w_s->b_langp.ga_len == 0)
	திரும்பு 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    எனில் (*ptr >= '0' && *ptr <= '9')
    {
	எனில் (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	இல்லையெனில் எனில் (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	இல்லையெனில்
	    mi.mi_end = skipdigits(ptr);
	nrlen = (முழு)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    எனில் (spell_iswordp(mi.mi_fend, wp))
    {
	எனில் (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	இல்லையெனில்
	{
	    செய்
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } போது (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	எனில் (capcol != வெற்று && *capcol == 0 && wp->w_s->b_cap_prog != வெற்று)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    எனில் (!SPELL_ISUPPER(c))
		wrongcaplen = (முழு)(mi.mi_fend - ptr);
	}
    }
    எனில் (capcol != வெற்று)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = வெற்று;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    எனில் (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (வெறுமை)spell_casefold(wp, ptr, (முழு)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (முழு)STRLEN(mi.mi_fword);

    எனில் (is_camel_case && mi.mi_fwordlen > 0)
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
    க்கு (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	எனில் (mi.mi_lp->lp_slang->sl_fidxs == வெற்று)
	    தொடர்;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	எனில் (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	எனில் (மொத்தம்_word && mi.mi_result == SP_OK)
	{
	    மொத்தம்_common_word(mi.mi_lp->lp_slang, ptr,
						   (முழு)(mi.mi_end - ptr), 1);
	    மொத்தம்_word = FALSE;
	}
    }

    எனில் (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	எனில் (nrlen > 0)
	{
	    எனில் (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		திரும்பு nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	இல்லையெனில் எனில் (!spell_iswordp_nmw(ptr, wp))
	{
	    எனில் (capcol != வெற்று && wp->w_s->b_cap_prog != வெற்று)
	    {
		regmatch_T	regmatch;
		முழு		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		எனில் (r)
		    *capcol = (முழு)(regmatch.endp[0] - ptr);
	    }

	    எனில் (has_mbyte)
		திரும்பு (*mb_ptr2len)(ptr);
	    திரும்பு 1;
	}
	இல்லையெனில் எனில் (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	இல்லையெனில் எனில் (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    முழு		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    எனில் (mi.mi_lp->lp_slang->sl_fidxs != வெற்று)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		க்கு (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    எனில் (p >= mi.mi_end)
			நிறுத்து;
		    mi.mi_compoff = (முழு)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    எனில் (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			நிறுத்து;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	எனில் (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	இல்லையெனில் எனில் (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	இல்லையெனில்
	    *attrp = HLF_SPL;
    }

    எனில் (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	திரும்பு wrongcaplen;
    }

    திரும்பு (முழு)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    நிலையான முழு
get_char_type(முழு c)
{
    எனில் (VIM_ISDIGIT(c))
	திரும்பு CHAR_DIGIT;
    எனில் (SPELL_ISUPPER(c))
	திரும்பு CHAR_UPPER;
    திரும்பு CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    நிலையான char_u *
advance_camelcase_word(char_u *str, win_T *wp, முழு *is_camel_case)
{
    முழு last_type, last_last_type, this_type;
    முழு c;
    char_u *end = str;

    *is_camel_case = FALSE;

    எனில் (*str == NUL)
	திரும்பு str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    போது (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	எனில் (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    நிறுத்து;
	}
	இல்லையெனில் எனில் ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    நிறுத்து;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    திரும்பு end;
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
    நிலையான வெறுமை
find_word(matchinf_T *mip, முழு mode)
{
    idx_T	arridx = 0;
    முழு		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    முழு		endidxcnt = 0;
    முழு		len;
    முழு		wlen = 0;
    முழு		flen;
    முழு		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    முழு		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    முழு		word_ends;
    முழு		prefix_found;
    முழு		nobreak_result;

    எனில் (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	எனில் (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    இல்லையெனில்
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	எனில் (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	இல்லையெனில் எனில் (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    எனில் (byts == வெற்று)
	திரும்பு;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    க்கு (;;)
    {
	எனில் (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	எனில் (byts[arridx] == 0)
	{
	    எனில் (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		திரும்பு;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    போது (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    எனில் (len == 0)
		நிறுத்து;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	எனில் (ptr[wlen] == NUL)
	    நிறுத்து;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	எனில் (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	போது (lo < hi)
	{
	    m = (lo + hi) / 2;
	    எனில் (byts[m] > c)
		hi = m - 1;
	    இல்லையெனில் எனில் (byts[m] < c)
		lo = m + 1;
	    இல்லையெனில்
	    {
		lo = hi = m;
		நிறுத்து;
	    }
	}

	// Stop if there is no matching byte.
	எனில் (hi < lo || byts[lo] != c)
	    நிறுத்து;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	எனில் (c == ' ')
	{
	    க்கு (;;)
	    {
		எனில் (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		எனில் (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    நிறுத்து;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    போது (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	எனில் ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    தொடர்;	    // not at first byte of character
	எனில் (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    எனில் (slang->sl_compprog == வெற்று && !slang->sl_nobreak)
		தொடர்;	    // next char is a word character
	    word_ends = FALSE;
	}
	இல்லையெனில்
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	எனில் (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    எனில் (STRNCMP(ptr, p, wlen) != 0)
	    {
		க்கு (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (முழு)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	க்கு (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    எனில் (mode == FIND_FOLDWORD)
	    {
		எனில் (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		எனில் (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    தொடர்;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    இல்லையெனில் எனில் (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		எனில் (c == 0)
		    தொடர்;

		// Use the WF_RARE flag for a rare prefix.
		எனில் (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    எனில் (slang->sl_nobreak)
	    {
		எனில் ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    நிறுத்து;
		}
	    }

	    இல்லையெனில் எனில் ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		எனில் (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    தொடர்;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		எனில் (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			தொடர்;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		எனில் (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    தொடர்;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		எனில் (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    தொடர்;
		எனில் (!word_ends && (flags & WF_NOCOMPAFT))
		    தொடர்;

		// Quickly check if compounding is possible with this flag.
		எனில் (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    தொடர்;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		எனில் (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    தொடர்;

		எனில் (mode == FIND_COMPOUND)
		{
		    முழு	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    எனில் (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			க்கு (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    இல்லையெனில்
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    எனில் (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			தொடர்;

		    எனில் (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			எனில் (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    தொடர்;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		எனில் (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    எனில் (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			எனில் (ptr == mip->mi_word)
			    (வெறுமை)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			இல்லையெனில்
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    எனில் (!can_compound(slang, fword, mip->mi_compflags))
			தொடர்;
		}
		இல்லையெனில் எனில் (slang->sl_comprules != வெற்று
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    தொடர்;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    இல்லையெனில் எனில் (flags & WF_NEEDCOMP)
		தொடர்;

	    nobreak_result = SP_OK;

	    எனில் (!word_ends)
	    {
		முழு	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		முழு	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		எனில் (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		எனில் (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    எனில் (STRNCMP(ptr, p, wlen) != 0)
		    {
			க்கு (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (முழு)(p - mip->mi_fword);
		    }
		}
#எனில் 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		எனில் (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		க்கு (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    எனில் (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			எனில் (mip->mi_lp->lp_slang->sl_fidxs == வெற்று
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    தொடர்;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    எனில் (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#எனில் 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			எனில் (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    எனில் (!slang->sl_nobreak)
			நிறுத்து;
		}
		--mip->mi_complen;
		எனில் (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		எனில் (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		இல்லையெனில்
		{
		    எனில் (mip->mi_result == SP_OK)
			நிறுத்து;
		    தொடர்;
		}
	    }

	    எனில் (flags & WF_BANNED)
		res = SP_BANNED;
	    இல்லையெனில் எனில் (flags & WF_REGION)
	    {
		// Check region.
		எனில் ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		இல்லையெனில்
		    res = SP_LOCAL;
	    }
	    இல்லையெனில் எனில் (flags & WF_RARE)
		res = SP_RARE;
	    இல்லையெனில்
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    எனில் (nobreak_result == SP_BAD)
	    {
		எனில் (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		இல்லையெனில் எனில் (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    இல்லையெனில் எனில் (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    இல்லையெனில் எனில் (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    எனில் (mip->mi_result == SP_OK)
		நிறுத்து;
	}

	எனில் (mip->mi_result == SP_OK)
	    நிறுத்து;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    முழு
match_checkcompoundpattern(
    char_u	*ptr,
    முழு		wlen,
    garray_T	*gap)  // &sl_comppat
{
    முழு		i;
    char_u	*p;
    முழு		len;

    க்கு (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	எனில் (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (முழு)STRLEN(p);
	    எனில் (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    முழு
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    முழு		i;
    char_u	*p;

    எனில் (slang->sl_compprog == வெற்று)
	திரும்பு FALSE;
    எனில் (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	க்கு (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    இல்லையெனில்
	p = flags;
    எனில் (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	திரும்பு FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    எனில் (slang->sl_compsylmax < MAXWLEN
		       && மொத்தம்_syllables(slang, word) > slang->sl_compsylmax)
	திரும்பு (முழு)STRLEN(flags) < slang->sl_compmax;
    திரும்பு TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    முழு
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    முழு		i;
    முழு		c;

    // loop over all the COMPOUNDRULE entries
    க்கு (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	க்கு (i = 0; ; ++i)
	{
	    c = compflags[i];
	    எனில் (c == NUL)
		// found a rule that matches for the flags we have so far
		திரும்பு TRUE;
	    எனில் (*p == '/' || *p == NUL)
		நிறுத்து;  // end of rule, it's too short
	    எனில் (*p == '[')
	    {
		முழு match = FALSE;

		// compare against all the flags in []
		++p;
		போது (*p != ']' && *p != NUL)
		    எனில் (*p++ == c)
			match = TRUE;
		எனில் (!match)
		    நிறுத்து;  // none matches
	    }
	    இல்லையெனில் எனில் (*p != c)
		நிறுத்து;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	எனில் (p == வெற்று)
	    நிறுத்து;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    திரும்பு FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    முழு
valid_word_prefix(
    முழு		totprefcnt,	// nr of prefix IDs
    முழு		arridx,		// idx in sl_pidxs[]
    முழு		flags,
    char_u	*word,
    slang_T	*slang,
    முழு		cond_req)	// only use prefixes with a condition
{
    முழு		prefcnt;
    முழு		pidx;
    regprog_T	**rp;
    முழு		prefid;

    prefid = (unsigned)flags >> 24;
    க்கு (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	எனில் (prefid != (pidx & 0xff))
	    தொடர்;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	எனில் ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    தொடர்;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	எனில் (*rp != வெற்று)
	{
	    எனில் (!vim_regexec_prog(rp, FALSE, word, 0))
		தொடர்;
	}
	இல்லையெனில் எனில் (cond_req)
	    தொடர்;

	// It's a match!  Return the WF_ flags.
	திரும்பு pidx;
    }
    திரும்பு 0;
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
    நிலையான வெறுமை
find_prefix(matchinf_T *mip, முழு mode)
{
    idx_T	arridx = 0;
    முழு		len;
    முழு		wlen = 0;
    முழு		flen;
    முழு		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    எனில் (byts == வெற்று)
	திரும்பு;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    எனில் (mode == FIND_COMPOUND)
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
    க்கு (;;)
    {
	எனில் (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	எனில் (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    போது (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    எனில் (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    எனில் (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    இல்லையெனில்
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    எனில் (len == 0)
		நிறுத்து;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	எனில் (ptr[wlen] == NUL)
	    நிறுத்து;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	போது (lo < hi)
	{
	    m = (lo + hi) / 2;
	    எனில் (byts[m] > c)
		hi = m - 1;
	    இல்லையெனில் எனில் (byts[m] < c)
		lo = m + 1;
	    இல்லையெனில்
	    {
		lo = hi = m;
		நிறுத்து;
	    }
	}

	// Stop if there is no matching byte.
	எனில் (hi < lo || byts[lo] != c)
	    நிறுத்து;

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
    நிலையான முழு
fold_more(matchinf_T *mip)
{
    முழு		flen;
    char_u	*p;

    p = mip->mi_fend;
    செய்
	MB_PTR_ADV(mip->mi_fend);
    போது (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    எனில் (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (வெறுமை)spell_casefold(mip->mi_win, p, (முழு)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (முழு)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    திரும்பு flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    முழு
spell_valid_case(
    முழு	    wordflags,	    // flags for the checked word.
    முழு	    treeflags)	    // flags for the word in the spell tree
{
    திரும்பு ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    முழு
spell_check_window(win_T *wp)
{
    திரும்பு wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(எழுத்து **)(wp->w_s->b_langp.ga_data) != வெற்று;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    நிலையான முழு
no_spell_checking(win_T *wp)
{
    எனில் (spell_check_window(wp))
	திரும்பு FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    திரும்பு TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    முழு
spell_move_to(
    win_T	*wp,
    முழு		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    முழு		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    முழு		found_len = 0;
    char_u	*வரி;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    முழு		len;
#ifdef FEAT_SYN_HL
    முழு		has_syntax = syntax_present(wp);
#endif
    முழு		col;
    முழு		can_spell;
    char_u	*buf = வெற்று;
    முழு		buflen = 0;
    முழு		skip = 0;
    முழு		capcol = -1;
    முழு		found_one = FALSE;
    முழு		wrapped = FALSE;

    எனில் (no_spell_checking(wp))
	திரும்பு 0;

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

    போது (!got_int)
    {
	முழு empty_வரி;

	வரி = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	எனில் (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    எனில் (buf == வெற்று)
		நிறுத்து;
	}

	// In first line check first word for Capital.
	எனில் (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	எனில் (capcol == 0)
	    capcol = getwhitecols(வரி);
	இல்லையெனில் எனில் (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(வரி);
	    எனில் (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    வரி = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_வரி = *skipwhite(வரி) == NUL;
	STRCPY(buf, வரி);
	எனில் (lnum < wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    spell_cat_வரி(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	போது (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    எனில் (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		நிறுத்து;

	    // start of word
	    attr = HLF_மொத்தம்;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    எனில் (attr != HLF_மொத்தம்)
	    {
		// We found a bad word.  Check the attribute.
		எனில் (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    எனில் (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			எனில் (has_syntax)
			{
			    col = (முழு)(p - buf);
			    (வெறுமை)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    எனில் (!can_spell)
				attr = HLF_மொத்தம்;
			}
			இல்லையெனில்
#endif
			    can_spell = TRUE;

			எனில் (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (முழு)(p - buf);
			    found_pos.coladd = 0;
			    எனில் (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				எனில் (attrp != வெற்று)
				    *attrp = attr;
				திரும்பு len;
			    }
			    இல்லையெனில் எனில் (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    இல்லையெனில்
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	எனில் (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    திரும்பு found_len;
	}

	எனில் (curline)
	    நிறுத்து;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	எனில் (lnum == wp->w_cursor.lnum && wrapped)
	    நிறுத்து;

	// Advance to next line.
	எனில் (dir == BACKWARD)
	{
	    எனில் (lnum > 1)
		--lnum;
	    இல்லையெனில் எனில் (!p_ws)
		நிறுத்து;	    // at first line and 'nowrapscan'
	    இல்லையெனில்
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
		wrapped = TRUE;
		எனில் (!shortmess(SHM_தேடல்))
		    give_warning((char_u *)_(உச்சி_bot_செய்தி), TRUE);
	    }
	    capcol = -1;
	}
	இல்லையெனில்
	{
	    எனில் (lnum < wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
		++lnum;
	    இல்லையெனில் எனில் (!p_ws)
		நிறுத்து;	    // at first line and 'nowrapscan'
	    இல்லையெனில்
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		எனில் (!shortmess(SHM_தேடல்))
		    give_warning((char_u *)_(bot_உச்சி_செய்தி), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    எனில் (lnum == wp->w_cursor.lnum && !found_one)
		நிறுத்து;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    எனில் (attr == HLF_மொத்தம்)
		skip = (முழு)(p - endp);
	    இல்லையெனில்
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    எனில் (empty_வரி)
		capcol = 0;
	}

	வரி_breakcheck();
    }

    vim_free(buf);
    திரும்பு 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    வெறுமை
spell_cat_வரி(char_u *buf, char_u *வரி, முழு maxlen)
{
    char_u	*p;
    முழு		n;

    p = skipwhite(வரி);
    போது (vim_strchr((char_u *)"*#/\"\t", *p) != வெற்று)
	p = skipwhite(p + 1);

    எனில் (*p == NUL)
	திரும்பு;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (முழு)(p - வரி) + 1;
    எனில் (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
வகைவரை கட்டமைப்பு spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    முழு	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    நிலையான வெறுமை
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    முழு		r;
    spelload_T	sl;
    முழு		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = வெற்று;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    க்கு (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((எழுத்து *)fname_enc, அளவு(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#இல்லையெனில்
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	எனில் (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((எழுத்து *)fname_enc, அளவு(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#இல்லையெனில்
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    எனில் (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		தொடர்;
	    நிறுத்து;
	}
	நிறுத்து;
    }

    எனில் (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#இல்லையெனில்
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    இல்லையெனில் எனில் (sl.sl_slang != வெற்று)
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
spell_enc(வெறுமை)
{

    எனில் (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	திரும்பு p_enc;
    திரும்பு (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    நிலையான வெறுமை
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((எழுத்து *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    எனில் (lp != வெற்று)
    {
	எனில் (lang != வெற்று)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, அளவு(fromto_T), 10);
	ga_init2(&lp->sl_repsal, அளவு(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    திரும்பு lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    வெறுமை
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
    வெறுமை
free_sal_பொருட்கள்(garray_T *gap)
{
    salitem_T	*smp;

    போது (gap->ga_len > 0)
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
    வெறுமை
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    முழு		i;
    முழு		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    க்கு (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	போது (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    எனில் (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	எனில் (gap->ga_data != வெற்று)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    க்கு (i = 0; i < gap->ga_len; ++i)
		vim_free(((முழு **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    இல்லையெனில்
	free_sal_பொருட்கள்(gap);

    க்கு (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = வெற்று;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_பொருட்கள்);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_சாவி_OFF);
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
    வெறுமை
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = வெற்று;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    நிலையான வெறுமை
spell_load_cb(char_u *fname, வெறுமை *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, வெற்று, FALSE);
    எனில் (slang == வெற்று)
	திரும்பு;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    எனில் (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    இல்லையெனில் எனில் (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    வெறுமை
மொத்தம்_common_word(
    slang_T	*lp,
    char_u	*word,
    முழு		len,	    // word length, -1 for up to NUL
    முழு		மொத்தம்)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    எனில் (len == -1)
	p = word;
    இல்லையெனில் எனில் (len >= MAXWLEN)
	திரும்பு;
    இல்லையெனில்
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    எனில் (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	எனில் (wc == வெற்று)
	    திரும்பு;
	STRCPY(wc->wc_word, p);
	wc->wc_மொத்தம் = மொத்தம்;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    இல்லையெனில்
    {
	wc = HI2WC(hi);
	எனில் ((wc->wc_மொத்தம் += மொத்தம்) < (unsigned)மொத்தம்)	// check for overflow
	    wc->wc_மொத்தம் = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    முழு
byte_in_str(char_u *str, முழு n)
{
    char_u	*p;

    க்கு (p = str; *p != NUL; ++p)
	எனில் (*p == n)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

#define SY_MAXLEN   30
வகைவரை கட்டமைப்பு syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    முழு		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    முழு
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    முழு		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_பொருட்கள், அளவு(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    போது (p != வெற்று)
    {
	*p++ = NUL;
	எனில் (*p == NUL)	    // trailing slash
	    நிறுத்து;
	s = p;
	p = vim_strchr(p, '/');
	எனில் (p == வெற்று)
	    l = (முழு)STRLEN(s);
	இல்லையெனில்
	    l = (முழு)(p - s);
	எனில் (l >= SY_MAXLEN)
	    திரும்பு SP_FORMERROR;
	எனில் (ga_grow(&slang->sl_syl_பொருட்கள், 1) == FAIL)
	    திரும்பு SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_பொருட்கள்.ga_data)
					       + slang->sl_syl_பொருட்கள்.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    திரும்பு OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    நிலையான முழு
மொத்தம்_syllables(slang_T *slang, char_u *word)
{
    முழு		cnt = 0;
    முழு		skip = FALSE;
    char_u	*p;
    முழு		len;
    முழு		i;
    syl_item_T	*syl;
    முழு		c;

    எனில் (slang->sl_syllable == வெற்று)
	திரும்பு 0;

    க்கு (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	எனில் (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    தொடர்;
	}

	// Find longest match of syllable items.
	len = 0;
	க்கு (i = 0; i < slang->sl_syl_பொருட்கள்.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_பொருட்கள்.ga_data) + i;
	    எனில் (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	எனில் (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	இல்லையெனில்
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    எனில் (vim_strchr(slang->sl_syllable, c) == வெற்று)
		skip = FALSE;	    // No, search for next syllable
	    இல்லையெனில் எனில் (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    திரும்பு cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    எழுத்து *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    முழு		filename;
    முழு		region_mask;
    slang_T	*slang;
    முழு		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    முழு		len;
    char_u	*p;
    முழு		round;
    char_u	*spf;
    char_u	*use_region = வெற்று;
    முழு		dont_use_region = FALSE;
    முழு		nobreak = FALSE;
    முழு		i, j;
    langp_T	*lp, *lp2;
    நிலையான முழு	recursive = FALSE;
    எழுத்து	*ret_செய்தி = வெற்று;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    எனில் (recursive)
	திரும்பு வெற்று;
    recursive = TRUE;

    ga_init2(&ga, அளவு(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    எனில் (spl_copy == வெற்று)
	செல் theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    க்கு (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = வெற்று;

	எனில் (!valid_spelllang(lang))
	    தொடர்;

	எனில் (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    தொடர்;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	எனில் (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    எனில் (p != வெற்று && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    இல்லையெனில்
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		எனில் (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    நிறுத்து;
	}
	இல்லையெனில்
	{
	    filename = FALSE;
	    எனில் (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    இல்லையெனில்
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		எனில் (STRICMP(lang, slang->sl_name) == 0)
		    நிறுத்து;
	}

	எனில் (region != வெற்று)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    எனில் (use_region != வெற்று && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	எனில் (slang == வெற்று)
	{
	    எனில் (filename)
		(வெறுமை)spell_load_file(lang, lang, வெற்று, FALSE);
	    இல்லையெனில்
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		எனில் (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_செய்தி = N_(e_spellfilemising_autocommand_deleted_buffer);
		    செல் theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    எனில் (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		எனில் (!filename && region != வெற்று)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    எனில் (c == REGION_ALL)
		    {
			எனில் (slang->sl_add)
			{
			    எனில் (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			இல்லையெனில்
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    இல்லையெனில்
			region_mask = 1 << c;
		}

		எனில் (region_mask != 0)
		{
		    எனில் (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_செய்தி = e_out_of_memory;
			செல் theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    எனில் (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    க்கு (round = 0; round == 0 || *spf != NUL; ++round)
    {
	எனில் (round == 0)
	{
	    // Internal wordlist, if there is one.
	    எனில் (int_wordlist == வெற்று)
		தொடர்;
	    int_wordlist_spl(spf_name);
	}
	இல்லையெனில்
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    க்கு (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		எனில் (p != வெற்று && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    நிறுத்து;
	    }
	    எனில் (c < ga.ga_len)
		தொடர்;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    எனில் (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		நிறுத்து;
	எனில் (slang == வெற்று)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    எனில் (round == 0)
		STRCPY(lang, "internal wordlist");
	    இல்லையெனில்
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		எனில் (p != வெற்று)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, வெற்று, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    எனில் (slang != வெற்று && nobreak)
		slang->sl_nobreak = TRUE;
	}
	எனில் (slang != வெற்று && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    எனில் (use_region != வெற்று && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		எனில் (c != REGION_ALL)
		    region_mask = 1 << c;
		இல்லையெனில் எனில் (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    எனில் (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = வெற்று;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = வெற்று;
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
    க்கு (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	எனில் (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	இல்லையெனில்
	    // find first similar language that does sound folding
	    க்கு (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		எனில் (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    நிறுத்து;
		}
	    }

	// REP items
	எனில் (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	இல்லையெனில்
	    // find first similar language that has REP items
	    க்கு (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		எனில் (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    நிறுத்து;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    திரும்பு ret_செய்தி;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    நிலையான வெறுமை
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    நிலையான வெறுமை
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    எனில் (lp->sl_midword == வெற்று)	    // there aren't any
	திரும்பு;

    க்கு (p = lp->sl_midword; *p != NUL; )
	எனில் (has_mbyte)
	{
	    முழு	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    எனில் (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    இல்லையெனில் எனில் (wp->w_s->b_spell_ismw_mb == வெற்று)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    இல்லையெனில்
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (முழு)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		எனில் (bp != வெற்று)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	இல்லையெனில்
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    நிலையான முழு
find_region(char_u *rp, char_u *region)
{
    முழு		i;

    க்கு (i = 0; ; i += 2)
    {
	எனில் (rp[i] == NUL)
	    திரும்பு REGION_ALL;
	எனில் (rp[i] == region[0] && rp[i + 1] == region[1])
	    நிறுத்து;
    }
    திரும்பு i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    முழு
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    முழு		c;
    முழு		firstcap;
    முழு		allcap;
    முழு		past_second = FALSE;	// past second word char

    // find first letter
    க்கு (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	எனில் (end == வெற்று ? *p == NUL : p >= end)
	    திரும்பு 0;	    // only non-word characters, illegal word
    எனில் (has_mbyte)
	c = mb_ptr2char_adv(&p);
    இல்லையெனில்
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    க்கு ( ; end == வெற்று ? *p != NUL : p < end; MB_PTR_ADV(p))
	எனில் (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    எனில் (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		எனில் (past_second && allcap)
		    திரும்பு WF_KEEPCAP;
		allcap = FALSE;
	    }
	    இல்லையெனில் எனில் (!allcap)
		// UlU -> KEEPCAP
		திரும்பு WF_KEEPCAP;
	    past_second = TRUE;
	}

    எனில் (allcap)
	திரும்பு WF_ALLCAP;
    எனில் (firstcap)
	திரும்பு WF_ONECAP;
    திரும்பு 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    வெறுமை
spell_delete_wordlist(வெறுமை)
{
    char_u	fname[MAXPATHL];

    எனில் (int_wordlist == வெற்று)
	திரும்பு;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    வெறுமை
spell_free_all(வெறுமை)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    போது (first_lang != வெற்று)
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
    வெறுமை
spell_reload(வெறுமை)
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
	எனில் (*wp->w_s->b_p_spl != NUL)
	{
		எனில் (wp->w_p_spell)
		{
		    (வெறுமை)parse_spelllang(wp);
		    நிறுத்து;
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
open_spellbuf(வெறுமை)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_சாவி = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    திரும்பு buf;
}

/*
 * Close the buffer used for spell info.
 */
    வெறுமை
close_spellbuf(buf_T *buf)
{
    எனில் (buf == வெற்று)
	திரும்பு;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    வெறுமை
clear_spell_chartab(spelltab_T *sp)
{
    முழு		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    க்கு (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    க்கு (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    க்கு (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    க்கு (i = 'a'; i <= 'z'; ++i)
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
    வெறுமை
init_spell_chartab(வெறுமை)
{
    முழு	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    எனில் (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	க்கு (i = 128; i <= 255; ++i)
	    எனில் (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    இல்லையெனில் எனில் (enc_utf8)
    {
	க்கு (i = 128; i < 256; ++i)
	{
	    முழு f = utf_fold(i);
	    முழு u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    இல்லையெனில்
    {
	// Rough guess: use locale-dependent library functions.
	க்கு (i = 128; i < 256; ++i)
	{
	    எனில் (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    இல்லையெனில் எனில் (MB_ISLOWER(i))
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
    முழு
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    முழு		l;
    முழு		c;

    எனில் (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	எனில் (l == 1)
	{
	    // be quick for ASCII
	    எனில் (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	இல்லையெனில்
	{
	    c = mb_ptr2char(p);
	    எனில் (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != வெற்று
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != வெற்று))
		s = p + l;
	}

	c = mb_ptr2char(s);
	எனில் (c > 255)
	    திரும்பு spell_mb_isword_class(mb_get_class(s), wp);
	திரும்பு spelltab.st_isw[c];
    }

    திரும்பு spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    முழு
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    முழு		c;

    எனில் (has_mbyte)
    {
	c = mb_ptr2char(p);
	எனில் (c > 255)
	    திரும்பு spell_mb_isword_class(mb_get_class(p), wp);
	திரும்பு spelltab.st_isw[c];
    }
    திரும்பு spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    நிலையான முழு
spell_mb_isword_class(முழு cl, win_T *wp)
{
    எனில் (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	திரும்பு cl == 2 || cl == 0x2800;
    திரும்பு cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    நிலையான முழு
spell_iswordp_w(முழு *p, win_T *wp)
{
    முழு		*s;

    எனில் (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != வெற்று
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != வெற்று))
	s = p + 1;
    இல்லையெனில்
	s = p;

    எனில் (*s > 255)
    {
	எனில் (enc_utf8)
	    திரும்பு spell_mb_isword_class(utf_class(*s), wp);
	எனில் (enc_dbcs)
	    திரும்பு spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	திரும்பு 0;
    }
    திரும்பு spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    முழு
spell_casefold(
    win_T	*wp,
    char_u	*str,
    முழு		len,
    char_u	*buf,
    முழு		buflen)
{
    முழு		i;

    எனில் (len >= buflen)
    {
	buf[0] = NUL;
	திரும்பு FAIL;		// result will not fit
    }

    எனில் (has_mbyte)
    {
	முழு	outi = 0;
	char_u	*p;
	முழு	c;

	// Fold one character at a time.
	க்கு (p = str; p < str + len; )
	{
	    எனில் (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		திரும்பு FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    எனில் (c == 0x03a3 || c == 0x03c2)
	    {
		எனில் (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		இல்லையெனில்
		    c = 0x03c3;
	    }
	    இல்லையெனில்
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    இல்லையெனில்
    {
	// Be quick for non-multibyte encodings.
	க்கு (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    திரும்பு OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    முழு
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    எனில் (wp->w_s->b_cap_prog == வெற்று)
	திரும்பு FALSE;

    முழு		need_cap = FALSE;
    char_u	*வரி = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : வெற்று;
    char_u	*வரி_copy = வெற்று;
    colnr_T	endcol = 0;

    எனில் (col == 0 || getwhitecols(வரி) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	எனில் (lnum == 1)
	    need_cap = TRUE;
	இல்லையெனில்
	{
	    வரி = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    எனில் (*skipwhite(வரி) == NUL)
		need_cap = TRUE;
	    இல்லையெனில்
	    {
		// Append a space in place of the line break.
		வரி_copy = concat_str(வரி, (char_u *)" ");
		எனில் (வரி_copy == வெற்று)
		    திரும்பு FALSE;

		வரி = வரி_copy;
		endcol = (colnr_T)STRLEN(வரி);
	    }
	}
    }
    இல்லையெனில்
	endcol = col;

    எனில் (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = வரி + endcol;
	க்கு (;;)
	{
	    MB_PTR_BACK(வரி, p);
	    எனில் (p == வரி || spell_iswordp_nmw(p, wp))
		நிறுத்து;
	    எனில் (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == வரி + endcol)
	    {
		need_cap = TRUE;
		நிறுத்து;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(வரி_copy);

    திரும்பு need_cap;
}


/*
 * ":spellrepall"
 */
    வெறுமை
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*வரி;
    char_u	*p;
    முழு		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    எனில் (repl_from == வெற்று || repl_to == வெற்று)
    {
	emsg(_(e_no_previous_spell_replacement));
	திரும்பு;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    எனில் (frompat == வெற்று)
	திரும்பு;
    frompatlen = vim_snprintf((எழுத்து *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    போது (!got_int)
    {
	எனில் (do_தேடல்(வெற்று, '/', '/', frompat, frompatlen, 1L, தேடல்_KEEP, வெற்று) == 0
						   || u_save_cursor() == FAIL)
	    நிறுத்து;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	வரி = ml_get_curline();
	எனில் (addlen <= 0 || STRNCMP(வரி + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    எனில் (p == வெற்று)
		நிறுத்து;
	    mch_memmove(p, வரி, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, வரி + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#எனில் defined(FEAT_PROP_POPUP)
	    எனில் (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (முழு)addlen, APC_SUBSTITUTE);
#endif

	    எனில் (curwin->w_cursor.lnum != prev_lnum)
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

    எனில் (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    இல்லையெனில்
	do_sub_செய்தி(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    வெறுமை
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    முழு		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    முழு		c;
    முழு		l;

    p = word;
    எனில் (has_mbyte)
	c = mb_cptr2char_adv(&p);
    இல்லையெனில்
	c = *p++;
    எனில் (upper)
	c = SPELL_TOUPPER(c);
    இல்லையெனில்
	c = SPELL_TOFOLD(c);
    எனில் (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    இல்லையெனில்
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
    வெறுமை
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    முழு		c;

    d = wcopy;
    க்கு (s = word; *s != NUL; )
    {
	எனில் (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	இல்லையெனில்
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	எனில் (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    எனில் (d - wcopy >= MAXWLEN - 1)
		நிறுத்து;
	    *d++ = c;
	}
	இல்லையெனில்
	    c = SPELL_TOUPPER(c);

	எனில் (has_mbyte)
	{
	    எனில் (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		நிறுத்து;
	    d += mb_char2bytes(c, d);
	}
	இல்லையெனில்
	{
	    எனில் (d - wcopy >= MAXWLEN - 1)
		நிறுத்து;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    முழு
nofold_len(char_u *fword, முழு flen, char_u *word)
{
    char_u	*p;
    முழு		i = 0;

    க்கு (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    க்கு (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    திரும்பு (முழு)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    வெறுமை
make_case_word(char_u *fword, char_u *cword, முழு flags)
{
    எனில் (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    இல்லையெனில் எனில் (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    இல்லையெனில்
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#எனில் defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    முழு		lpi;

    எனில் (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    எனில் (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		திரும்பு vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    திரும்பு vim_strsave(word);
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
    வெறுமை
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    முழு		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    எனில் (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    இல்லையெனில்
    {
	// SAL items used.  Requires the word to be case-folded.
	எனில் (folded)
	    word = inword;
	இல்லையெனில்
	{
	    (வெறுமை)spell_casefold(curwin,
				  inword, (முழு)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	எனில் (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	இல்லையெனில்
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    நிலையான வெறுமை
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    முழு		ri = 0;
    முழு		c;

    எனில் (has_mbyte)
    {
	முழு	prevc = 0;
	முழு	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	க்கு (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    எனில் (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    இல்லையெனில் எனில் (c < 256)
		c = slang->sl_sal_first[c];
	    இல்லையெனில்
	    {
		ip = ((முழு **)slang->sl_sal.ga_data)[c & 0xff];
		எனில் (ip == வெற்று)		// empty list, can't match
		    c = NUL;
		இல்லையெனில்
		    க்கு (;;)		// find "c" in the list
		    {
			எனில் (*ip == 0)	// not found
			{
			    c = NUL;
			    நிறுத்து;
			}
			எனில் (*ip == c)	// match!
			{
			    c = ip[1];
			    நிறுத்து;
			}
			ip += 2;
		    }
	    }

	    எனில் (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		எனில் (ri + MB_MAXBYTES > MAXWLEN)
		    நிறுத்து;
		prevc = c;
	    }
	}
    }
    இல்லையெனில்
    {
	// The sl_sal_first[] table contains the translation.
	க்கு (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    எனில் (VIM_ISWHITE(c))
		c = ' ';
	    இல்லையெனில்
		c = slang->sl_sal_first[c];
	    எனில் (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    நிலையான வெறுமை
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    முழு		i, j, z;
    முழு		reslen;
    முழு		n, k = 0;
    முழு		z0;
    முழு		k0;
    முழு		n0;
    முழு		c;
    முழு		pri;
    முழு		p0 = -333;
    முழு		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    எனில் (slang->sl_rem_accents)
    {
	t = word;
	போது (*s != NUL)
	{
	    எனில் (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    இல்லையெனில்
	    {
		எனில் (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    இல்லையெனில்
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    போது ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	எனில் (n >= 0)
	{
	    // check all rules for the same letter
	    க்கு (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		எனில் (k > 1)
		{
		    எனில் (word[i + 1] != s[1])
			தொடர்;
		    எனில் (k > 2)
		    {
			க்கு (j = 2; j < k; ++j)
			    எனில் (word[i + j] != s[j])
				நிறுத்து;
			எனில் (j < k)
			    தொடர்;
		    }
		}

		எனில் ((pf = smp[n].sm_oneof) != வெற்று)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    போது (*pf != NUL && *pf != word[i + k])
			++pf;
		    எனில் (*pf == NUL)
			தொடர்;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		போது (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		எனில் (*s == '<')
		    s++;
		எனில் (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		எனில் (*s == '^' && *(s + 1) == '^')
		    s++;

		எனில் (*s == NUL
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

		    எனில் (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			க்கு ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    எனில் (k0 > 1)
			    {
				எனில் (word[i + k] != s[1])
				    தொடர்;
				எனில் (k0 > 2)
				{
				    pf = word + i + k + 1;
				    க்கு (j = 2; j < k0; ++j)
					எனில் (*pf++ != s[j])
					    நிறுத்து;
				    எனில் (j < k0)
					தொடர்;
				}
			    }
			    k0 += k - 1;

			    எனில் ((pf = smp[n0].sm_oneof) != வெற்று)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				போது (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				எனில் (*pf == NUL)
				    தொடர்;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    போது (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    எனில் (*s == '<')
				s++;
			    எனில் (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    எனில் (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				எனில் (k0 == k)
				    // this is just a piece of the string
				    தொடர்;

				எனில் (p0 < pri)
				    // priority too low
				    தொடர்;
				// rule fits; stop search
				நிறுத்து;
			    }
			}

			எனில் (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    தொடர்;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    எனில் (s == வெற்று)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != வெற்று) ? 1 : 0;
		    எனில் (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			எனில் (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			போது (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			எனில் (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    இல்லையெனில்
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			போது (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    எனில் (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			எனில் (strstr((எழுத்து *)pf, "^^") != வெற்று)
			{
			    எனில் (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    நிறுத்து;
		}
	    }
	}
	இல்லையெனில் எனில் (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	எனில் (z0 == 0)
	{
	    எனில் (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    நிலையான வெறுமை
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    முழு		word[MAXWLEN];
    முழு		wres[MAXWLEN];
    முழு		l;
    char_u	*s;
    முழு		*ws;
    char_u	*t;
    முழு		*pf;
    முழு		i, j, z;
    முழு		reslen;
    முழு		n, k = 0;
    முழு		z0;
    முழு		k0;
    முழு		n0;
    முழு		c;
    முழு		pri;
    முழு		p0 = -333;
    முழு		c0;
    முழு		did_white = FALSE;
    முழு		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    க்கு (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	எனில் (slang->sl_rem_accents)
	{
	    எனில் (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		எனில் (did_white)
		    தொடர்;
		c = ' ';
		did_white = TRUE;
	    }
	    இல்லையெனில்
	    {
		did_white = FALSE;
		எனில் (!spell_iswordp_nmw(t, curwin))
		    தொடர்;
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
    போது ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	எனில் (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    க்கு (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		எனில் (c != ws[0])
		    தொடர்;
		k = smp[n].sm_leadlen;
		எனில் (k > 1)
		{
		    எனில் (word[i + 1] != ws[1])
			தொடர்;
		    எனில் (k > 2)
		    {
			க்கு (j = 2; j < k; ++j)
			    எனில் (word[i + j] != ws[j])
				நிறுத்து;
			எனில் (j < k)
			    தொடர்;
		    }
		}

		எனில் ((pf = smp[n].sm_oneof_w) != வெற்று)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    போது (*pf != NUL && *pf != word[i + k])
			++pf;
		    எனில் (*pf == NUL)
			தொடர்;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		போது (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		எனில் (*s == '<')
		    s++;
		எனில் (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		எனில் (*s == '^' && *(s + 1) == '^')
		    s++;

		எனில் (*s == NUL
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

		    எனில் (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			க்கு ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    எனில் (c0 != ws[0])
				தொடர்;
			    k0 = smp[n0].sm_leadlen;
			    எனில் (k0 > 1)
			    {
				எனில் (word[i + k] != ws[1])
				    தொடர்;
				எனில் (k0 > 2)
				{
				    pf = word + i + k + 1;
				    க்கு (j = 2; j < k0; ++j)
					எனில் (*pf++ != ws[j])
					    நிறுத்து;
				    எனில் (j < k0)
					தொடர்;
				}
			    }
			    k0 += k - 1;

			    எனில் ((pf = smp[n0].sm_oneof_w) != வெற்று)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				போது (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				எனில் (*pf == NUL)
				    தொடர்;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    போது (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    எனில் (*s == '<')
				s++;
			    எனில் (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    எனில் (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				எனில் (k0 == k)
				    // this is just a piece of the string
				    தொடர்;

				எனில் (p0 < pri)
				    // priority too low
				    தொடர்;
				// rule fits; stop search
				நிறுத்து;
			    }
			}

			எனில் (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    தொடர்;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != வெற்று) ? 1 : 0;
		    எனில் (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			எனில் (reslen > 0 && ws != வெற்று && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			எனில் (ws != வெற்று)
			    போது (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			எனில் (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    அளவு(முழு) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    இல்லையெனில்
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			எனில் (ws != வெற்று)
			    போது (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				எனில் (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			எனில் (ws == வெற்று)
			    c = NUL;
			இல்லையெனில்
			    c = *ws;
			எனில் (strstr((எழுத்து *)s, "^^") != வெற்று)
			{
			    எனில் (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       அளவு(முழு) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    நிறுத்து;
		}
	    }
	}
	இல்லையெனில் எனில் (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	எனில் (z0 == 0)
	{
	    எனில் (k && !p0 && reslen < MAXWLEN && c != NUL
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
    க்கு (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	எனில் (l + MB_MAXBYTES > MAXWLEN)
	    நிறுத்து;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    வெறுமை
ex_spellinfo(exarg_T *eap UNUSED)
{
    முழு		lpi;
    langp_T	*lp;
    char_u	*p;

    எனில் (no_spell_checking(curwin))
	திரும்பு;

    செய்தி_start();
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	செய்தி_puts("file: ");
	செய்தி_puts((எழுத்து *)lp->lp_slang->sl_fname);
	செய்தி_putchar('\n');
	p = lp->lp_slang->sl_info;
	எனில் (p != வெற்று)
	{
	    செய்தி_puts((எழுத்து *)p);
	    செய்தி_putchar('\n');
	}
    }
    செய்தி_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_மொத்தம்	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    வெறுமை
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    எனில் (no_spell_checking(curwin))
	திரும்பு;
    (வெறுமை)get_option_மதிப்பு((char_u*)"spl", &dummy, &spl, வெற்று, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_மதிப்பு_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_மதிப்பு_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    எனில் (!BUFEMPTY())
	திரும்பு;

    spell_dump_compl(வெற்று, 0, வெற்று, eap->forceit ? DUMPFLAG_மொத்தம் : 0);

    // Delete the empty line that we started with.
    எனில் (curbuf->b_ml.ml_வரி_மொத்தம் > 1)
	ml_delete(curbuf->b_ml.ml_வரி_மொத்தம்);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    வெறுமை
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    முழு		ic,	    // ignore case
    முழு		*dir,	    // direction for adding matches
    முழு		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    முழு		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    முழு		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    முழு		round;
    முழு		depth;
    முழு		n;
    முழு		flags;
    char_u	*region_names = வெற்று;	    // region names being used
    முழு		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    முழு		lpi;
    முழு		dumpflags = dumpflags_arg;
    முழு		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    எனில் (pat != வெற்று)
    {
	எனில் (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	இல்லையெனில்
	{
	    n = captype(pat, வெற்று);
	    எனில் (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    இல்லையெனில் எனில் (n == WF_ALLCAP && (முழு)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	எனில் (p[0] != 0)
	{
	    எனில் (region_names == வெற்று)	    // first language with regions
		region_names = p;
	    இல்லையெனில் எனில் (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		நிறுத்து;
	    }
	}
    }

    எனில் (do_region && region_names != வெற்று && pat == வெற்று)
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    இல்லையெனில்
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	எனில் (slang->sl_fbyts == வெற்று)	    // reloading failed
	    தொடர்;

	எனில் (pat == வெற்று)
	{
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	எனில் (pat != வெற்று && slang->sl_pbyts == வெற்று)
	    patlen = (முழு)STRLEN(pat);
	இல்லையெனில்
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	க்கு (round = 1; round <= 2; ++round)
	{
	    எனில் (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    இல்லையெனில்
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    எனில் (byts == வெற்று)
		தொடர்;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    போது (depth >= 0 && !got_int
				  && (pat == வெற்று || !ins_compl_interrupted()))
	    {
		எனில் (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    வரி_breakcheck();
		    ins_compl_check_சாவிகள்(50, FALSE);
		}
		இல்லையெனில்
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    எனில் (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (முழு)idxs[n];
			எனில் ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    எனில் (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    எனில் (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				எனில் (pat == வெற்று)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    எனில் (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    இல்லையெனில்
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
			எனில் (depth <= patlen
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
    நிலையான வெறுமை
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    முழு		*dir,
    முழு		dumpflags,
    முழு		wordflags,
    linenr_T	lnum)
{
    முழு		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    முழு		i;
    முழு		flags = wordflags;

    எனில் (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    எனில் (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    எனில் ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    இல்லையெனில்
    {
	p = word;
	எனில் ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, வெற்று) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    எனில் (pat == வெற்று)
    {
	// Add flags and regions after a slash.
	எனில் ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    எனில் (keepcap)
		STRCAT(badword, "=");
	    எனில் (flags & WF_BANNED)
		STRCAT(badword, "!");
	    இல்லையெனில் எனில் (flags & WF_RARE)
		STRCAT(badword, "?");
	    எனில் (flags & WF_REGION)
		க்கு (i = 0; i < 7; ++i)
		    எனில் (flags & (0x10000 << i))
			sprintf((எழுத்து *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	எனில் (dumpflags & DUMPFLAG_மொத்தம்)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_மொத்தம்);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    இல்லையெனில் எனில் (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (முழு)STRLEN(p),
					  p_ic, வெற்று, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    நிலையான linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    முழு		*dir,
    முழு		dumpflags,
    முழு		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    முழு		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    முழு		has_word_up = FALSE;
    முழு		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    முழு		depth;
    முழு		n;
    முழு		len;
    முழு		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    எனில் (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    எனில் (byts != வெற்று)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	போது (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    எனில் (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		வரி_breakcheck();
	    }
	    இல்லையெனில்
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		எனில் (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    க்கு (i = 1; i < len; ++i)
			எனில் (byts[n + i] != 0)
			    நிறுத்து;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    எனில் (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			எனில் (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    எனில் (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			எனில் (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    எனில் (lnum != 0)
				++lnum;
			}
		    }
		}
		இல்லையெனில் எனில் (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    திரும்பு lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    போது (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    திரும்பு p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    முழு
spell_word_start(முழு startcol)
{
    char_u	*வரி;
    char_u	*p;
    முழு		col = 0;

    எனில் (no_spell_checking(curwin))
	திரும்பு startcol;

    // Find a word character before "startcol".
    வரி = ml_get_curline();
    க்கு (p = வரி + startcol; p > வரி; )
    {
	MB_PTR_BACK(வரி, p);
	எனில் (spell_iswordp_nmw(p, curwin))
	    நிறுத்து;
    }

    // Go back to start of the word.
    போது (p > வரி)
    {
	col = (முழு)(p - வரி);
	MB_PTR_BACK(வரி, p);
	எனில் (!spell_iswordp(p, curwin))
	    நிறுத்து;
	col = 0;
    }

    திரும்பு col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
நிலையான முழு spell_expand_need_cap;

    வெறுமை
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
    முழு
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    திரும்பு ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    முழு
valid_spelllang(char_u *val)
{
    திரும்பு valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    முழு
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    முழு		l;

    spf = val;
    போது (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	எனில் (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    திரும்பு FALSE;
	க்கு (s = spf_name; *s != NUL; ++s)
	    எனில் (!vim_is_fname_char(*s))
		திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    எழுத்து *
did_set_spell_option(வெறுமை)
{
    எழுத்து    *errmsg = வெற்று;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    நிறுத்து;
	}
    திரும்பு errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    எழுத்து *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    எனில் (synblock->b_p_spc == வெற்று || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = வெற்று;
    இல்லையெனில்
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	எனில் (re != வெற்று)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    எனில் (synblock->b_cap_prog == வெற்று)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		திரும்பு e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    திரும்பு வெற்று;
}

#endif  // FEAT_SPELL
