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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_SPELL)

#അന്തര്ഭാവ <time.h>

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
തരനിർവചനം ഘടന matchinf_S
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
    പൂർണ്ണസംഖ്യ		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    പൂർണ്ണസംഖ്യ		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    പൂർണ്ണസംഖ്യ		mi_prefcnt;		// number of entries at mi_prefarridx
    പൂർണ്ണസംഖ്യ		mi_prefixlen;		// byte length of prefix
    പൂർണ്ണസംഖ്യ		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    പൂർണ്ണസംഖ്യ		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    പൂർണ്ണസംഖ്യ		mi_complen;		// nr of compound words used
    പൂർണ്ണസംഖ്യ		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    പൂർണ്ണസംഖ്യ		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    പൂർണ്ണസംഖ്യ		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    പൂർണ്ണസംഖ്യ		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


സ്ഥിതം പൂർണ്ണസംഖ്യ spell_mb_isword_class(പൂർണ്ണസംഖ്യ cl, win_T *wp);

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

സ്ഥിതം ശൂന്യം find_word(matchinf_T *mip, പൂർണ്ണസംഖ്യ mode);
സ്ഥിതം ശൂന്യം find_prefix(matchinf_T *mip, പൂർണ്ണസംഖ്യ mode);
സ്ഥിതം പൂർണ്ണസംഖ്യ fold_more(matchinf_T *mip);
സ്ഥിതം ശൂന്യം spell_load_cb(char_u *fname, ശൂന്യം *cookie);
സ്ഥിതം പൂർണ്ണസംഖ്യ എണ്ണം_syllables(slang_T *slang, char_u *word);
സ്ഥിതം ശൂന്യം clear_midword(win_T *buf);
സ്ഥിതം ശൂന്യം use_midword(slang_T *lp, win_T *buf);
സ്ഥിതം പൂർണ്ണസംഖ്യ find_region(char_u *rp, char_u *region);
സ്ഥിതം ശൂന്യം spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
സ്ഥിതം ശൂന്യം spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
സ്ഥിതം ശൂന്യം spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
സ്ഥിതം ശൂന്യം dump_word(slang_T *slang, char_u *word, char_u *pat, പൂർണ്ണസംഖ്യ *dir, പൂർണ്ണസംഖ്യ round, പൂർണ്ണസംഖ്യ flags, linenr_T lnum);
സ്ഥിതം linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, പൂർണ്ണസംഖ്യ *dir, പൂർണ്ണസംഖ്യ round, പൂർണ്ണസംഖ്യ flags, linenr_T startlnum);
സ്ഥിതം char_u *advance_camelcase_word(char_u *p, win_T *wp, പൂർണ്ണസംഖ്യ *is_camel_case);

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
    പൂർണ്ണസംഖ്യ
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    പൂർണ്ണസംഖ്യ		*capcol,	// column to check for Capital
    പൂർണ്ണസംഖ്യ		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    പൂർണ്ണസംഖ്യ		nrlen = 0;	// found a number first
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		wrongcaplen = 0;
    പൂർണ്ണസംഖ്യ		lpi;
    പൂർണ്ണസംഖ്യ		എണ്ണം_word = docount;
    പൂർണ്ണസംഖ്യ		use_camel_case = *wp->w_s->b_p_spo != NUL;
    പൂർണ്ണസംഖ്യ		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    എങ്കിൽ (*ptr <= ' ')
	തിരികെ 1;

    // Return here when loading language files failed.
    എങ്കിൽ (wp->w_s->b_langp.ga_len == 0)
	തിരികെ 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    എങ്കിൽ (*ptr >= '0' && *ptr <= '9')
    {
	എങ്കിൽ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	അല്ലെങ്കിൽ എങ്കിൽ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	അല്ലെങ്കിൽ
	    mi.mi_end = skipdigits(ptr);
	nrlen = (പൂർണ്ണസംഖ്യ)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    എങ്കിൽ (spell_iswordp(mi.mi_fend, wp))
    {
	എങ്കിൽ (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	അല്ലെങ്കിൽ
	{
	    ചെയ്യുക
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ആകുമ്പോൾ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	എങ്കിൽ (capcol != ശൂന്യ && *capcol == 0 && wp->w_s->b_cap_prog != ശൂന്യ)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    എങ്കിൽ (!SPELL_ISUPPER(c))
		wrongcaplen = (പൂർണ്ണസംഖ്യ)(mi.mi_fend - ptr);
	}
    }
    എങ്കിൽ (capcol != ശൂന്യ)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ശൂന്യ;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    എങ്കിൽ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (ശൂന്യം)spell_casefold(wp, ptr, (പൂർണ്ണസംഖ്യ)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (പൂർണ്ണസംഖ്യ)STRLEN(mi.mi_fword);

    എങ്കിൽ (is_camel_case && mi.mi_fwordlen > 0)
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
    വേണ്ടി (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	എങ്കിൽ (mi.mi_lp->lp_slang->sl_fidxs == ശൂന്യ)
	    തുടരുക;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	എങ്കിൽ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	എങ്കിൽ (എണ്ണം_word && mi.mi_result == SP_OK)
	{
	    എണ്ണം_common_word(mi.mi_lp->lp_slang, ptr,
						   (പൂർണ്ണസംഖ്യ)(mi.mi_end - ptr), 1);
	    എണ്ണം_word = FALSE;
	}
    }

    എങ്കിൽ (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	എങ്കിൽ (nrlen > 0)
	{
	    എങ്കിൽ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		തിരികെ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	അല്ലെങ്കിൽ എങ്കിൽ (!spell_iswordp_nmw(ptr, wp))
	{
	    എങ്കിൽ (capcol != ശൂന്യ && wp->w_s->b_cap_prog != ശൂന്യ)
	    {
		regmatch_T	regmatch;
		പൂർണ്ണസംഖ്യ		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		എങ്കിൽ (r)
		    *capcol = (പൂർണ്ണസംഖ്യ)(regmatch.endp[0] - ptr);
	    }

	    എങ്കിൽ (has_mbyte)
		തിരികെ (*mb_ptr2len)(ptr);
	    തിരികെ 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	അല്ലെങ്കിൽ എങ്കിൽ (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    പൂർണ്ണസംഖ്യ		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    എങ്കിൽ (mi.mi_lp->lp_slang->sl_fidxs != ശൂന്യ)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		വേണ്ടി (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    എങ്കിൽ (p >= mi.mi_end)
			നിർത്തുക;
		    mi.mi_compoff = (പൂർണ്ണസംഖ്യ)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    എങ്കിൽ (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			നിർത്തുക;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	എങ്കിൽ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	അല്ലെങ്കിൽ എങ്കിൽ (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	അല്ലെങ്കിൽ
	    *attrp = HLF_SPL;
    }

    എങ്കിൽ (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	തിരികെ wrongcaplen;
    }

    തിരികെ (പൂർണ്ണസംഖ്യ)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_char_type(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (VIM_ISDIGIT(c))
	തിരികെ CHAR_DIGIT;
    എങ്കിൽ (SPELL_ISUPPER(c))
	തിരികെ CHAR_UPPER;
    തിരികെ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    സ്ഥിതം char_u *
advance_camelcase_word(char_u *str, win_T *wp, പൂർണ്ണസംഖ്യ *is_camel_case)
{
    പൂർണ്ണസംഖ്യ last_type, last_last_type, this_type;
    പൂർണ്ണസംഖ്യ c;
    char_u *end = str;

    *is_camel_case = FALSE;

    എങ്കിൽ (*str == NUL)
	തിരികെ str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    ആകുമ്പോൾ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	എങ്കിൽ (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    നിർത്തുക;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    തിരികെ end;
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
    സ്ഥിതം ശൂന്യം
find_word(matchinf_T *mip, പൂർണ്ണസംഖ്യ mode)
{
    idx_T	arridx = 0;
    പൂർണ്ണസംഖ്യ		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    പൂർണ്ണസംഖ്യ		endidxcnt = 0;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		wlen = 0;
    പൂർണ്ണസംഖ്യ		flen;
    പൂർണ്ണസംഖ്യ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    പൂർണ്ണസംഖ്യ		word_ends;
    പൂർണ്ണസംഖ്യ		prefix_found;
    പൂർണ്ണസംഖ്യ		nobreak_result;

    എങ്കിൽ (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	എങ്കിൽ (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    അല്ലെങ്കിൽ
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	എങ്കിൽ (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    എങ്കിൽ (byts == ശൂന്യ)
	തിരികെ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    വേണ്ടി (;;)
    {
	എങ്കിൽ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	എങ്കിൽ (byts[arridx] == 0)
	{
	    എങ്കിൽ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		തിരികെ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ആകുമ്പോൾ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    എങ്കിൽ (len == 0)
		നിർത്തുക;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	എങ്കിൽ (ptr[wlen] == NUL)
	    നിർത്തുക;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	എങ്കിൽ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ആകുമ്പോൾ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    എങ്കിൽ (byts[m] > c)
		hi = m - 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (byts[m] < c)
		lo = m + 1;
	    അല്ലെങ്കിൽ
	    {
		lo = hi = m;
		നിർത്തുക;
	    }
	}

	// Stop if there is no matching byte.
	എങ്കിൽ (hi < lo || byts[lo] != c)
	    നിർത്തുക;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	എങ്കിൽ (c == ' ')
	{
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		എങ്കിൽ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    നിർത്തുക;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    ആകുമ്പോൾ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	എങ്കിൽ ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    തുടരുക;	    // not at first byte of character
	എങ്കിൽ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    എങ്കിൽ (slang->sl_compprog == ശൂന്യ && !slang->sl_nobreak)
		തുടരുക;	    // next char is a word character
	    word_ends = FALSE;
	}
	അല്ലെങ്കിൽ
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	എങ്കിൽ (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    എങ്കിൽ (STRNCMP(ptr, p, wlen) != 0)
	    {
		വേണ്ടി (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (പൂർണ്ണസംഖ്യ)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	വേണ്ടി (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    എങ്കിൽ (mode == FIND_FOLDWORD)
	    {
		എങ്കിൽ (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		എങ്കിൽ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    തുടരുക;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    അല്ലെങ്കിൽ എങ്കിൽ (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		എങ്കിൽ (c == 0)
		    തുടരുക;

		// Use the WF_RARE flag for a rare prefix.
		എങ്കിൽ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    എങ്കിൽ (slang->sl_nobreak)
	    {
		എങ്കിൽ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    നിർത്തുക;
		}
	    }

	    അല്ലെങ്കിൽ എങ്കിൽ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		എങ്കിൽ (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    തുടരുക;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		എങ്കിൽ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			തുടരുക;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		എങ്കിൽ (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    തുടരുക;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		എങ്കിൽ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    തുടരുക;
		എങ്കിൽ (!word_ends && (flags & WF_NOCOMPAFT))
		    തുടരുക;

		// Quickly check if compounding is possible with this flag.
		എങ്കിൽ (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    തുടരുക;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		എങ്കിൽ (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    തുടരുക;

		എങ്കിൽ (mode == FIND_COMPOUND)
		{
		    പൂർണ്ണസംഖ്യ	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    എങ്കിൽ (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			വേണ്ടി (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    അല്ലെങ്കിൽ
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    എങ്കിൽ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			തുടരുക;

		    എങ്കിൽ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			എങ്കിൽ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    തുടരുക;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		എങ്കിൽ (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    എങ്കിൽ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			എങ്കിൽ (ptr == mip->mi_word)
			    (ശൂന്യം)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			അല്ലെങ്കിൽ
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    എങ്കിൽ (!can_compound(slang, fword, mip->mi_compflags))
			തുടരുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (slang->sl_comprules != ശൂന്യ
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    തുടരുക;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    അല്ലെങ്കിൽ എങ്കിൽ (flags & WF_NEEDCOMP)
		തുടരുക;

	    nobreak_result = SP_OK;

	    എങ്കിൽ (!word_ends)
	    {
		പൂർണ്ണസംഖ്യ	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		പൂർണ്ണസംഖ്യ	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		എങ്കിൽ (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		എങ്കിൽ (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    എങ്കിൽ (STRNCMP(ptr, p, wlen) != 0)
		    {
			വേണ്ടി (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (പൂർണ്ണസംഖ്യ)(p - mip->mi_fword);
		    }
		}
#എങ്കിൽ 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		എങ്കിൽ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		വേണ്ടി (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    എങ്കിൽ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			എങ്കിൽ (mip->mi_lp->lp_slang->sl_fidxs == ശൂന്യ
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    തുടരുക;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    എങ്കിൽ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#എങ്കിൽ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			എങ്കിൽ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    എങ്കിൽ (!slang->sl_nobreak)
			നിർത്തുക;
		}
		--mip->mi_complen;
		എങ്കിൽ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		എങ്കിൽ (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (mip->mi_result == SP_OK)
			നിർത്തുക;
		    തുടരുക;
		}
	    }

	    എങ്കിൽ (flags & WF_BANNED)
		res = SP_BANNED;
	    അല്ലെങ്കിൽ എങ്കിൽ (flags & WF_REGION)
	    {
		// Check region.
		എങ്കിൽ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		അല്ലെങ്കിൽ
		    res = SP_LOCAL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (flags & WF_RARE)
		res = SP_RARE;
	    അല്ലെങ്കിൽ
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    എങ്കിൽ (nobreak_result == SP_BAD)
	    {
		എങ്കിൽ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    എങ്കിൽ (mip->mi_result == SP_OK)
		നിർത്തുക;
	}

	എങ്കിൽ (mip->mi_result == SP_OK)
	    നിർത്തുക;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    പൂർണ്ണസംഖ്യ
match_checkcompoundpattern(
    char_u	*ptr,
    പൂർണ്ണസംഖ്യ		wlen,
    garray_T	*gap)  // &sl_comppat
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;

    വേണ്ടി (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	എങ്കിൽ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(p);
	    എങ്കിൽ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    പൂർണ്ണസംഖ്യ
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;

    എങ്കിൽ (slang->sl_compprog == ശൂന്യ)
	തിരികെ FALSE;
    എങ്കിൽ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	വേണ്ടി (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    അല്ലെങ്കിൽ
	p = flags;
    എങ്കിൽ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	തിരികെ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    എങ്കിൽ (slang->sl_compsylmax < MAXWLEN
		       && എണ്ണം_syllables(slang, word) > slang->sl_compsylmax)
	തിരികെ (പൂർണ്ണസംഖ്യ)STRLEN(flags) < slang->sl_compmax;
    തിരികെ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    പൂർണ്ണസംഖ്യ
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		c;

    // loop over all the COMPOUNDRULE entries
    വേണ്ടി (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	വേണ്ടി (i = 0; ; ++i)
	{
	    c = compflags[i];
	    എങ്കിൽ (c == NUL)
		// found a rule that matches for the flags we have so far
		തിരികെ TRUE;
	    എങ്കിൽ (*p == '/' || *p == NUL)
		നിർത്തുക;  // end of rule, it's too short
	    എങ്കിൽ (*p == '[')
	    {
		പൂർണ്ണസംഖ്യ match = FALSE;

		// compare against all the flags in []
		++p;
		ആകുമ്പോൾ (*p != ']' && *p != NUL)
		    എങ്കിൽ (*p++ == c)
			match = TRUE;
		എങ്കിൽ (!match)
		    നിർത്തുക;  // none matches
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p != c)
		നിർത്തുക;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	എങ്കിൽ (p == ശൂന്യ)
	    നിർത്തുക;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    തിരികെ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    പൂർണ്ണസംഖ്യ
valid_word_prefix(
    പൂർണ്ണസംഖ്യ		totprefcnt,	// nr of prefix IDs
    പൂർണ്ണസംഖ്യ		arridx,		// idx in sl_pidxs[]
    പൂർണ്ണസംഖ്യ		flags,
    char_u	*word,
    slang_T	*slang,
    പൂർണ്ണസംഖ്യ		cond_req)	// only use prefixes with a condition
{
    പൂർണ്ണസംഖ്യ		prefcnt;
    പൂർണ്ണസംഖ്യ		pidx;
    regprog_T	**rp;
    പൂർണ്ണസംഖ്യ		prefid;

    prefid = (unsigned)flags >> 24;
    വേണ്ടി (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	എങ്കിൽ (prefid != (pidx & 0xff))
	    തുടരുക;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	എങ്കിൽ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    തുടരുക;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	എങ്കിൽ (*rp != ശൂന്യ)
	{
	    എങ്കിൽ (!vim_regexec_prog(rp, FALSE, word, 0))
		തുടരുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cond_req)
	    തുടരുക;

	// It's a match!  Return the WF_ flags.
	തിരികെ pidx;
    }
    തിരികെ 0;
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
    സ്ഥിതം ശൂന്യം
find_prefix(matchinf_T *mip, പൂർണ്ണസംഖ്യ mode)
{
    idx_T	arridx = 0;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		wlen = 0;
    പൂർണ്ണസംഖ്യ		flen;
    പൂർണ്ണസംഖ്യ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    എങ്കിൽ (byts == ശൂന്യ)
	തിരികെ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    എങ്കിൽ (mode == FIND_COMPOUND)
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
    വേണ്ടി (;;)
    {
	എങ്കിൽ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	എങ്കിൽ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ആകുമ്പോൾ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    എങ്കിൽ (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    എങ്കിൽ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    അല്ലെങ്കിൽ
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    എങ്കിൽ (len == 0)
		നിർത്തുക;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	എങ്കിൽ (ptr[wlen] == NUL)
	    നിർത്തുക;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ആകുമ്പോൾ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    എങ്കിൽ (byts[m] > c)
		hi = m - 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (byts[m] < c)
		lo = m + 1;
	    അല്ലെങ്കിൽ
	    {
		lo = hi = m;
		നിർത്തുക;
	    }
	}

	// Stop if there is no matching byte.
	എങ്കിൽ (hi < lo || byts[lo] != c)
	    നിർത്തുക;

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fold_more(matchinf_T *mip)
{
    പൂർണ്ണസംഖ്യ		flen;
    char_u	*p;

    p = mip->mi_fend;
    ചെയ്യുക
	MB_PTR_ADV(mip->mi_fend);
    ആകുമ്പോൾ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    എങ്കിൽ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (ശൂന്യം)spell_casefold(mip->mi_win, p, (പൂർണ്ണസംഖ്യ)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (പൂർണ്ണസംഖ്യ)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    തിരികെ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    പൂർണ്ണസംഖ്യ
spell_valid_case(
    പൂർണ്ണസംഖ്യ	    wordflags,	    // flags for the checked word.
    പൂർണ്ണസംഖ്യ	    treeflags)	    // flags for the word in the spell tree
{
    തിരികെ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    പൂർണ്ണസംഖ്യ
spell_check_window(win_T *wp)
{
    തിരികെ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(അക്ഷരം **)(wp->w_s->b_langp.ga_data) != ശൂന്യ;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
no_spell_checking(win_T *wp)
{
    എങ്കിൽ (spell_check_window(wp))
	തിരികെ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    തിരികെ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    പൂർണ്ണസംഖ്യ
spell_move_to(
    win_T	*wp,
    പൂർണ്ണസംഖ്യ		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    പൂർണ്ണസംഖ്യ		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    പൂർണ്ണസംഖ്യ		found_len = 0;
    char_u	*വരി;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    പൂർണ്ണസംഖ്യ		len;
#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ		has_syntax = syntax_present(wp);
#endif
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		can_spell;
    char_u	*buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		buflen = 0;
    പൂർണ്ണസംഖ്യ		skip = 0;
    പൂർണ്ണസംഖ്യ		capcol = -1;
    പൂർണ്ണസംഖ്യ		found_one = FALSE;
    പൂർണ്ണസംഖ്യ		wrapped = FALSE;

    എങ്കിൽ (no_spell_checking(wp))
	തിരികെ 0;

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

    ആകുമ്പോൾ (!got_int)
    {
	പൂർണ്ണസംഖ്യ empty_വരി;

	വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	എങ്കിൽ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    എങ്കിൽ (buf == ശൂന്യ)
		നിർത്തുക;
	}

	// In first line check first word for Capital.
	എങ്കിൽ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	എങ്കിൽ (capcol == 0)
	    capcol = getwhitecols(വരി);
	അല്ലെങ്കിൽ എങ്കിൽ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(വരി);
	    എങ്കിൽ (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_വരി = *skipwhite(വരി) == NUL;
	STRCPY(buf, വരി);
	എങ്കിൽ (lnum < wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    spell_cat_വരി(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ആകുമ്പോൾ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    എങ്കിൽ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		നിർത്തുക;

	    // start of word
	    attr = HLF_എണ്ണം;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    എങ്കിൽ (attr != HLF_എണ്ണം)
	    {
		// We found a bad word.  Check the attribute.
		എങ്കിൽ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    എങ്കിൽ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			എങ്കിൽ (has_syntax)
			{
			    col = (പൂർണ്ണസംഖ്യ)(p - buf);
			    (ശൂന്യം)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    എങ്കിൽ (!can_spell)
				attr = HLF_എണ്ണം;
			}
			അല്ലെങ്കിൽ
#endif
			    can_spell = TRUE;

			എങ്കിൽ (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (പൂർണ്ണസംഖ്യ)(p - buf);
			    found_pos.coladd = 0;
			    എങ്കിൽ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				എങ്കിൽ (attrp != ശൂന്യ)
				    *attrp = attr;
				തിരികെ len;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    അല്ലെങ്കിൽ
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	എങ്കിൽ (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    തിരികെ found_len;
	}

	എങ്കിൽ (curline)
	    നിർത്തുക;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	എങ്കിൽ (lnum == wp->w_cursor.lnum && wrapped)
	    നിർത്തുക;

	// Advance to next line.
	എങ്കിൽ (dir == BACKWARD)
	{
	    എങ്കിൽ (lnum > 1)
		--lnum;
	    അല്ലെങ്കിൽ എങ്കിൽ (!p_ws)
		നിർത്തുക;	    // at first line and 'nowrapscan'
	    അല്ലെങ്കിൽ
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
		wrapped = TRUE;
		എങ്കിൽ (!shortmess(SHM_തിരയൽ))
		    give_warning((char_u *)_(മുകൾ_bot_സന്ദേശം), TRUE);
	    }
	    capcol = -1;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (lnum < wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
		++lnum;
	    അല്ലെങ്കിൽ എങ്കിൽ (!p_ws)
		നിർത്തുക;	    // at first line and 'nowrapscan'
	    അല്ലെങ്കിൽ
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		എങ്കിൽ (!shortmess(SHM_തിരയൽ))
		    give_warning((char_u *)_(bot_മുകൾ_സന്ദേശം), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    എങ്കിൽ (lnum == wp->w_cursor.lnum && !found_one)
		നിർത്തുക;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    എങ്കിൽ (attr == HLF_എണ്ണം)
		skip = (പൂർണ്ണസംഖ്യ)(p - endp);
	    അല്ലെങ്കിൽ
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    എങ്കിൽ (empty_വരി)
		capcol = 0;
	}

	വരി_breakcheck();
    }

    vim_free(buf);
    തിരികെ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    ശൂന്യം
spell_cat_വരി(char_u *buf, char_u *വരി, പൂർണ്ണസംഖ്യ maxlen)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		n;

    p = skipwhite(വരി);
    ആകുമ്പോൾ (vim_strchr((char_u *)"*#/\"\t", *p) != ശൂന്യ)
	p = skipwhite(p + 1);

    എങ്കിൽ (*p == NUL)
	തിരികെ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (പൂർണ്ണസംഖ്യ)(p - വരി) + 1;
    എങ്കിൽ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
തരനിർവചനം ഘടന spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    പൂർണ്ണസംഖ്യ	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    സ്ഥിതം ശൂന്യം
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    പൂർണ്ണസംഖ്യ		r;
    spelload_T	sl;
    പൂർണ്ണസംഖ്യ		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ശൂന്യ;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    വേണ്ടി (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((അക്ഷരം *)fname_enc, വലുപ്പം(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#അല്ലെങ്കിൽ
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	എങ്കിൽ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((അക്ഷരം *)fname_enc, വലുപ്പം(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#അല്ലെങ്കിൽ
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    എങ്കിൽ (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		തുടരുക;
	    നിർത്തുക;
	}
	നിർത്തുക;
    }

    എങ്കിൽ (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#അല്ലെങ്കിൽ
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (sl.sl_slang != ശൂന്യ)
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
spell_enc(ശൂന്യം)
{

    എങ്കിൽ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	തിരികെ p_enc;
    തിരികെ (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    സ്ഥിതം ശൂന്യം
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((അക്ഷരം *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    എങ്കിൽ (lp != ശൂന്യ)
    {
	എങ്കിൽ (lang != ശൂന്യ)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, വലുപ്പം(fromto_T), 10);
	ga_init2(&lp->sl_repsal, വലുപ്പം(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    തിരികെ lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    ശൂന്യം
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
    ശൂന്യം
free_sal_ഇനങ്ങൾ(garray_T *gap)
{
    salitem_T	*smp;

    ആകുമ്പോൾ (gap->ga_len > 0)
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
    ശൂന്യം
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    വേണ്ടി (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ആകുമ്പോൾ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    എങ്കിൽ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	എങ്കിൽ (gap->ga_data != ശൂന്യ)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
		vim_free(((പൂർണ്ണസംഖ്യ **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    അല്ലെങ്കിൽ
	free_sal_ഇനങ്ങൾ(gap);

    വേണ്ടി (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ശൂന്യ;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_ഇനങ്ങൾ);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_താക്കോൽ_OFF);
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
    ശൂന്യം
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = ശൂന്യ;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    സ്ഥിതം ശൂന്യം
spell_load_cb(char_u *fname, ശൂന്യം *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, ശൂന്യ, FALSE);
    എങ്കിൽ (slang == ശൂന്യ)
	തിരികെ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    എങ്കിൽ (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    ശൂന്യം
എണ്ണം_common_word(
    slang_T	*lp,
    char_u	*word,
    പൂർണ്ണസംഖ്യ		len,	    // word length, -1 for up to NUL
    പൂർണ്ണസംഖ്യ		എണ്ണം)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    എങ്കിൽ (len == -1)
	p = word;
    അല്ലെങ്കിൽ എങ്കിൽ (len >= MAXWLEN)
	തിരികെ;
    അല്ലെങ്കിൽ
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	എങ്കിൽ (wc == ശൂന്യ)
	    തിരികെ;
	STRCPY(wc->wc_word, p);
	wc->wc_എണ്ണം = എണ്ണം;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    അല്ലെങ്കിൽ
    {
	wc = HI2WC(hi);
	എങ്കിൽ ((wc->wc_എണ്ണം += എണ്ണം) < (unsigned)എണ്ണം)	// check for overflow
	    wc->wc_എണ്ണം = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    പൂർണ്ണസംഖ്യ
byte_in_str(char_u *str, പൂർണ്ണസംഖ്യ n)
{
    char_u	*p;

    വേണ്ടി (p = str; *p != NUL; ++p)
	എങ്കിൽ (*p == n)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

#define SY_MAXLEN   30
തരനിർവചനം ഘടന syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    പൂർണ്ണസംഖ്യ		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    പൂർണ്ണസംഖ്യ
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_ഇനങ്ങൾ, വലുപ്പം(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ആകുമ്പോൾ (p != ശൂന്യ)
    {
	*p++ = NUL;
	എങ്കിൽ (*p == NUL)	    // trailing slash
	    നിർത്തുക;
	s = p;
	p = vim_strchr(p, '/');
	എങ്കിൽ (p == ശൂന്യ)
	    l = (പൂർണ്ണസംഖ്യ)STRLEN(s);
	അല്ലെങ്കിൽ
	    l = (പൂർണ്ണസംഖ്യ)(p - s);
	എങ്കിൽ (l >= SY_MAXLEN)
	    തിരികെ SP_FORMERROR;
	എങ്കിൽ (ga_grow(&slang->sl_syl_ഇനങ്ങൾ, 1) == FAIL)
	    തിരികെ SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_ഇനങ്ങൾ.ga_data)
					       + slang->sl_syl_ഇനങ്ങൾ.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    തിരികെ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
എണ്ണം_syllables(slang_T *slang, char_u *word)
{
    പൂർണ്ണസംഖ്യ		cnt = 0;
    പൂർണ്ണസംഖ്യ		skip = FALSE;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    syl_item_T	*syl;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (slang->sl_syllable == ശൂന്യ)
	തിരികെ 0;

    വേണ്ടി (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	എങ്കിൽ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    തുടരുക;
	}

	// Find longest match of syllable items.
	len = 0;
	വേണ്ടി (i = 0; i < slang->sl_syl_ഇനങ്ങൾ.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_ഇനങ്ങൾ.ga_data) + i;
	    എങ്കിൽ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	എങ്കിൽ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    എങ്കിൽ (vim_strchr(slang->sl_syllable, c) == ശൂന്യ)
		skip = FALSE;	    // No, search for next syllable
	    അല്ലെങ്കിൽ എങ്കിൽ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    തിരികെ cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    അക്ഷരം *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    പൂർണ്ണസംഖ്യ		filename;
    പൂർണ്ണസംഖ്യ		region_mask;
    slang_T	*slang;
    പൂർണ്ണസംഖ്യ		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		round;
    char_u	*spf;
    char_u	*use_region = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		dont_use_region = FALSE;
    പൂർണ്ണസംഖ്യ		nobreak = FALSE;
    പൂർണ്ണസംഖ്യ		i, j;
    langp_T	*lp, *lp2;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;
    അക്ഷരം	*ret_സന്ദേശം = ശൂന്യ;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    എങ്കിൽ (recursive)
	തിരികെ ശൂന്യ;
    recursive = TRUE;

    ga_init2(&ga, വലുപ്പം(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    എങ്കിൽ (spl_copy == ശൂന്യ)
	പോകുക theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    വേണ്ടി (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ശൂന്യ;

	എങ്കിൽ (!valid_spelllang(lang))
	    തുടരുക;

	എങ്കിൽ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    തുടരുക;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	എങ്കിൽ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    എങ്കിൽ (p != ശൂന്യ && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    അല്ലെങ്കിൽ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		എങ്കിൽ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	{
	    filename = FALSE;
	    എങ്കിൽ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    അല്ലെങ്കിൽ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		എങ്കിൽ (STRICMP(lang, slang->sl_name) == 0)
		    നിർത്തുക;
	}

	എങ്കിൽ (region != ശൂന്യ)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    എങ്കിൽ (use_region != ശൂന്യ && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	എങ്കിൽ (slang == ശൂന്യ)
	{
	    എങ്കിൽ (filename)
		(ശൂന്യം)spell_load_file(lang, lang, ശൂന്യ, FALSE);
	    അല്ലെങ്കിൽ
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		എങ്കിൽ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_സന്ദേശം = N_(e_spellfilemising_autocommand_deleted_buffer);
		    പോകുക theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    എങ്കിൽ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		എങ്കിൽ (!filename && region != ശൂന്യ)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    എങ്കിൽ (c == REGION_ALL)
		    {
			എങ്കിൽ (slang->sl_add)
			{
			    എങ്കിൽ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			അല്ലെങ്കിൽ
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    അല്ലെങ്കിൽ
			region_mask = 1 << c;
		}

		എങ്കിൽ (region_mask != 0)
		{
		    എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_സന്ദേശം = e_out_of_memory;
			പോകുക theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    എങ്കിൽ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    വേണ്ടി (round = 0; round == 0 || *spf != NUL; ++round)
    {
	എങ്കിൽ (round == 0)
	{
	    // Internal wordlist, if there is one.
	    എങ്കിൽ (int_wordlist == ശൂന്യ)
		തുടരുക;
	    int_wordlist_spl(spf_name);
	}
	അല്ലെങ്കിൽ
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    വേണ്ടി (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		എങ്കിൽ (p != ശൂന്യ && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    നിർത്തുക;
	    }
	    എങ്കിൽ (c < ga.ga_len)
		തുടരുക;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    എങ്കിൽ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		നിർത്തുക;
	എങ്കിൽ (slang == ശൂന്യ)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    എങ്കിൽ (round == 0)
		STRCPY(lang, "internal wordlist");
	    അല്ലെങ്കിൽ
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		എങ്കിൽ (p != ശൂന്യ)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, ശൂന്യ, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    എങ്കിൽ (slang != ശൂന്യ && nobreak)
		slang->sl_nobreak = TRUE;
	}
	എങ്കിൽ (slang != ശൂന്യ && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    എങ്കിൽ (use_region != ശൂന്യ && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		എങ്കിൽ (c != REGION_ALL)
		    region_mask = 1 << c;
		അല്ലെങ്കിൽ എങ്കിൽ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    എങ്കിൽ (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = ശൂന്യ;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = ശൂന്യ;
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
    വേണ്ടി (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	എങ്കിൽ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	അല്ലെങ്കിൽ
	    // find first similar language that does sound folding
	    വേണ്ടി (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		എങ്കിൽ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    നിർത്തുക;
		}
	    }

	// REP items
	എങ്കിൽ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	അല്ലെങ്കിൽ
	    // find first similar language that has REP items
	    വേണ്ടി (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		എങ്കിൽ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    നിർത്തുക;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    തിരികെ ret_സന്ദേശം;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    സ്ഥിതം ശൂന്യം
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    സ്ഥിതം ശൂന്യം
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    എങ്കിൽ (lp->sl_midword == ശൂന്യ)	    // there aren't any
	തിരികെ;

    വേണ്ടി (p = lp->sl_midword; *p != NUL; )
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    എങ്കിൽ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_s->b_spell_ismw_mb == ശൂന്യ)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    അല്ലെങ്കിൽ
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (പൂർണ്ണസംഖ്യ)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		എങ്കിൽ (bp != ശൂന്യ)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	അല്ലെങ്കിൽ
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_region(char_u *rp, char_u *region)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; ; i += 2)
    {
	എങ്കിൽ (rp[i] == NUL)
	    തിരികെ REGION_ALL;
	എങ്കിൽ (rp[i] == region[0] && rp[i + 1] == region[1])
	    നിർത്തുക;
    }
    തിരികെ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    പൂർണ്ണസംഖ്യ
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		firstcap;
    പൂർണ്ണസംഖ്യ		allcap;
    പൂർണ്ണസംഖ്യ		past_second = FALSE;	// past second word char

    // find first letter
    വേണ്ടി (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	എങ്കിൽ (end == ശൂന്യ ? *p == NUL : p >= end)
	    തിരികെ 0;	    // only non-word characters, illegal word
    എങ്കിൽ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    അല്ലെങ്കിൽ
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    വേണ്ടി ( ; end == ശൂന്യ ? *p != NUL : p < end; MB_PTR_ADV(p))
	എങ്കിൽ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    എങ്കിൽ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		എങ്കിൽ (past_second && allcap)
		    തിരികെ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!allcap)
		// UlU -> KEEPCAP
		തിരികെ WF_KEEPCAP;
	    past_second = TRUE;
	}

    എങ്കിൽ (allcap)
	തിരികെ WF_ALLCAP;
    എങ്കിൽ (firstcap)
	തിരികെ WF_ONECAP;
    തിരികെ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    ശൂന്യം
spell_delete_wordlist(ശൂന്യം)
{
    char_u	fname[MAXPATHL];

    എങ്കിൽ (int_wordlist == ശൂന്യ)
	തിരികെ;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    ശൂന്യം
spell_free_all(ശൂന്യം)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    ആകുമ്പോൾ (first_lang != ശൂന്യ)
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
    ശൂന്യം
spell_reload(ശൂന്യം)
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
	എങ്കിൽ (*wp->w_s->b_p_spl != NUL)
	{
		എങ്കിൽ (wp->w_p_spell)
		{
		    (ശൂന്യം)parse_spelllang(wp);
		    നിർത്തുക;
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
open_spellbuf(ശൂന്യം)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_താക്കോൽ = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    തിരികെ buf;
}

/*
 * Close the buffer used for spell info.
 */
    ശൂന്യം
close_spellbuf(buf_T *buf)
{
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    ശൂന്യം
clear_spell_chartab(spelltab_T *sp)
{
    പൂർണ്ണസംഖ്യ		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    വേണ്ടി (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    വേണ്ടി (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    വേണ്ടി (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    വേണ്ടി (i = 'a'; i <= 'z'; ++i)
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
    ശൂന്യം
init_spell_chartab(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    എങ്കിൽ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	വേണ്ടി (i = 128; i <= 255; ++i)
	    എങ്കിൽ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
    {
	വേണ്ടി (i = 128; i < 256; ++i)
	{
	    പൂർണ്ണസംഖ്യ f = utf_fold(i);
	    പൂർണ്ണസംഖ്യ u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Rough guess: use locale-dependent library functions.
	വേണ്ടി (i = 128; i < 256; ++i)
	{
	    എങ്കിൽ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (MB_ISLOWER(i))
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
    പൂർണ്ണസംഖ്യ
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    പൂർണ്ണസംഖ്യ		l;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	എങ്കിൽ (l == 1)
	{
	    // be quick for ASCII
	    എങ്കിൽ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	അല്ലെങ്കിൽ
	{
	    c = mb_ptr2char(p);
	    എങ്കിൽ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ശൂന്യ
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ശൂന്യ))
		s = p + l;
	}

	c = mb_ptr2char(s);
	എങ്കിൽ (c > 255)
	    തിരികെ spell_mb_isword_class(mb_get_class(s), wp);
	തിരികെ spelltab.st_isw[c];
    }

    തിരികെ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    പൂർണ്ണസംഖ്യ
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (has_mbyte)
    {
	c = mb_ptr2char(p);
	എങ്കിൽ (c > 255)
	    തിരികെ spell_mb_isword_class(mb_get_class(p), wp);
	തിരികെ spelltab.st_isw[c];
    }
    തിരികെ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
spell_mb_isword_class(പൂർണ്ണസംഖ്യ cl, win_T *wp)
{
    എങ്കിൽ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	തിരികെ cl == 2 || cl == 0x2800;
    തിരികെ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
spell_iswordp_w(പൂർണ്ണസംഖ്യ *p, win_T *wp)
{
    പൂർണ്ണസംഖ്യ		*s;

    എങ്കിൽ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ശൂന്യ
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ശൂന്യ))
	s = p + 1;
    അല്ലെങ്കിൽ
	s = p;

    എങ്കിൽ (*s > 255)
    {
	എങ്കിൽ (enc_utf8)
	    തിരികെ spell_mb_isword_class(utf_class(*s), wp);
	എങ്കിൽ (enc_dbcs)
	    തിരികെ spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	തിരികെ 0;
    }
    തിരികെ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    പൂർണ്ണസംഖ്യ
spell_casefold(
    win_T	*wp,
    char_u	*str,
    പൂർണ്ണസംഖ്യ		len,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		buflen)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (len >= buflen)
    {
	buf[0] = NUL;
	തിരികെ FAIL;		// result will not fit
    }

    എങ്കിൽ (has_mbyte)
    {
	പൂർണ്ണസംഖ്യ	outi = 0;
	char_u	*p;
	പൂർണ്ണസംഖ്യ	c;

	// Fold one character at a time.
	വേണ്ടി (p = str; p < str + len; )
	{
	    എങ്കിൽ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		തിരികെ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    എങ്കിൽ (c == 0x03a3 || c == 0x03c2)
	    {
		എങ്കിൽ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		അല്ലെങ്കിൽ
		    c = 0x03c3;
	    }
	    അല്ലെങ്കിൽ
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    അല്ലെങ്കിൽ
    {
	// Be quick for non-multibyte encodings.
	വേണ്ടി (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    തിരികെ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    പൂർണ്ണസംഖ്യ
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    എങ്കിൽ (wp->w_s->b_cap_prog == ശൂന്യ)
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ		need_cap = FALSE;
    char_u	*വരി = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : ശൂന്യ;
    char_u	*വരി_copy = ശൂന്യ;
    colnr_T	endcol = 0;

    എങ്കിൽ (col == 0 || getwhitecols(വരി) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	എങ്കിൽ (lnum == 1)
	    need_cap = TRUE;
	അല്ലെങ്കിൽ
	{
	    വരി = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    എങ്കിൽ (*skipwhite(വരി) == NUL)
		need_cap = TRUE;
	    അല്ലെങ്കിൽ
	    {
		// Append a space in place of the line break.
		വരി_copy = concat_str(വരി, (char_u *)" ");
		എങ്കിൽ (വരി_copy == ശൂന്യ)
		    തിരികെ FALSE;

		വരി = വരി_copy;
		endcol = (colnr_T)STRLEN(വരി);
	    }
	}
    }
    അല്ലെങ്കിൽ
	endcol = col;

    എങ്കിൽ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = വരി + endcol;
	വേണ്ടി (;;)
	{
	    MB_PTR_BACK(വരി, p);
	    എങ്കിൽ (p == വരി || spell_iswordp_nmw(p, wp))
		നിർത്തുക;
	    എങ്കിൽ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == വരി + endcol)
	    {
		need_cap = TRUE;
		നിർത്തുക;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(വരി_copy);

    തിരികെ need_cap;
}


/*
 * ":spellrepall"
 */
    ശൂന്യം
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*വരി;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    എങ്കിൽ (repl_from == ശൂന്യ || repl_to == ശൂന്യ)
    {
	emsg(_(e_no_previous_spell_replacement));
	തിരികെ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    എങ്കിൽ (frompat == ശൂന്യ)
	തിരികെ;
    frompatlen = vim_snprintf((അക്ഷരം *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ആകുമ്പോൾ (!got_int)
    {
	എങ്കിൽ (do_തിരയൽ(ശൂന്യ, '/', '/', frompat, frompatlen, 1L, തിരയൽ_KEEP, ശൂന്യ) == 0
						   || u_save_cursor() == FAIL)
	    നിർത്തുക;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	വരി = ml_get_curline();
	എങ്കിൽ (addlen <= 0 || STRNCMP(വരി + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    എങ്കിൽ (p == ശൂന്യ)
		നിർത്തുക;
	    mch_memmove(p, വരി, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, വരി + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#എങ്കിൽ defined(FEAT_PROP_POPUP)
	    എങ്കിൽ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (പൂർണ്ണസംഖ്യ)addlen, APC_SUBSTITUTE);
#endif

	    എങ്കിൽ (curwin->w_cursor.lnum != prev_lnum)
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

    എങ്കിൽ (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    അല്ലെങ്കിൽ
	do_sub_സന്ദേശം(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    ശൂന്യം
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    പൂർണ്ണസംഖ്യ		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		l;

    p = word;
    എങ്കിൽ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    അല്ലെങ്കിൽ
	c = *p++;
    എങ്കിൽ (upper)
	c = SPELL_TOUPPER(c);
    അല്ലെങ്കിൽ
	c = SPELL_TOFOLD(c);
    എങ്കിൽ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    അല്ലെങ്കിൽ
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
    ശൂന്യം
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    പൂർണ്ണസംഖ്യ		c;

    d = wcopy;
    വേണ്ടി (s = word; *s != NUL; )
    {
	എങ്കിൽ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	അല്ലെങ്കിൽ
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	എങ്കിൽ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    എങ്കിൽ (d - wcopy >= MAXWLEN - 1)
		നിർത്തുക;
	    *d++ = c;
	}
	അല്ലെങ്കിൽ
	    c = SPELL_TOUPPER(c);

	എങ്കിൽ (has_mbyte)
	{
	    എങ്കിൽ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		നിർത്തുക;
	    d += mb_char2bytes(c, d);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (d - wcopy >= MAXWLEN - 1)
		നിർത്തുക;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    പൂർണ്ണസംഖ്യ
nofold_len(char_u *fword, പൂർണ്ണസംഖ്യ flen, char_u *word)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i = 0;

    വേണ്ടി (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    വേണ്ടി (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    തിരികെ (പൂർണ്ണസംഖ്യ)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    ശൂന്യം
make_case_word(char_u *fword, char_u *cword, പൂർണ്ണസംഖ്യ flags)
{
    എങ്കിൽ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    അല്ലെങ്കിൽ എങ്കിൽ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    അല്ലെങ്കിൽ
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    പൂർണ്ണസംഖ്യ		lpi;

    എങ്കിൽ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	വേണ്ടി (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    എങ്കിൽ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		തിരികെ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    തിരികെ vim_strsave(word);
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
    ശൂന്യം
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    പൂർണ്ണസംഖ്യ		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    എങ്കിൽ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    അല്ലെങ്കിൽ
    {
	// SAL items used.  Requires the word to be case-folded.
	എങ്കിൽ (folded)
	    word = inword;
	അല്ലെങ്കിൽ
	{
	    (ശൂന്യം)spell_casefold(curwin,
				  inword, (പൂർണ്ണസംഖ്യ)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	എങ്കിൽ (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	അല്ലെങ്കിൽ
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    സ്ഥിതം ശൂന്യം
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    പൂർണ്ണസംഖ്യ		ri = 0;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (has_mbyte)
    {
	പൂർണ്ണസംഖ്യ	prevc = 0;
	പൂർണ്ണസംഖ്യ	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	വേണ്ടി (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    എങ്കിൽ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    അല്ലെങ്കിൽ എങ്കിൽ (c < 256)
		c = slang->sl_sal_first[c];
	    അല്ലെങ്കിൽ
	    {
		ip = ((പൂർണ്ണസംഖ്യ **)slang->sl_sal.ga_data)[c & 0xff];
		എങ്കിൽ (ip == ശൂന്യ)		// empty list, can't match
		    c = NUL;
		അല്ലെങ്കിൽ
		    വേണ്ടി (;;)		// find "c" in the list
		    {
			എങ്കിൽ (*ip == 0)	// not found
			{
			    c = NUL;
			    നിർത്തുക;
			}
			എങ്കിൽ (*ip == c)	// match!
			{
			    c = ip[1];
			    നിർത്തുക;
			}
			ip += 2;
		    }
	    }

	    എങ്കിൽ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		എങ്കിൽ (ri + MB_MAXBYTES > MAXWLEN)
		    നിർത്തുക;
		prevc = c;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	// The sl_sal_first[] table contains the translation.
	വേണ്ടി (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    എങ്കിൽ (VIM_ISWHITE(c))
		c = ' ';
	    അല്ലെങ്കിൽ
		c = slang->sl_sal_first[c];
	    എങ്കിൽ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    സ്ഥിതം ശൂന്യം
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    പൂർണ്ണസംഖ്യ		i, j, z;
    പൂർണ്ണസംഖ്യ		reslen;
    പൂർണ്ണസംഖ്യ		n, k = 0;
    പൂർണ്ണസംഖ്യ		z0;
    പൂർണ്ണസംഖ്യ		k0;
    പൂർണ്ണസംഖ്യ		n0;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		pri;
    പൂർണ്ണസംഖ്യ		p0 = -333;
    പൂർണ്ണസംഖ്യ		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    എങ്കിൽ (slang->sl_rem_accents)
    {
	t = word;
	ആകുമ്പോൾ (*s != NUL)
	{
	    എങ്കിൽ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    അല്ലെങ്കിൽ
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ആകുമ്പോൾ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	എങ്കിൽ (n >= 0)
	{
	    // check all rules for the same letter
	    വേണ്ടി (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		എങ്കിൽ (k > 1)
		{
		    എങ്കിൽ (word[i + 1] != s[1])
			തുടരുക;
		    എങ്കിൽ (k > 2)
		    {
			വേണ്ടി (j = 2; j < k; ++j)
			    എങ്കിൽ (word[i + j] != s[j])
				നിർത്തുക;
			എങ്കിൽ (j < k)
			    തുടരുക;
		    }
		}

		എങ്കിൽ ((pf = smp[n].sm_oneof) != ശൂന്യ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ആകുമ്പോൾ (*pf != NUL && *pf != word[i + k])
			++pf;
		    എങ്കിൽ (*pf == NUL)
			തുടരുക;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ആകുമ്പോൾ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		എങ്കിൽ (*s == '<')
		    s++;
		എങ്കിൽ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		എങ്കിൽ (*s == '^' && *(s + 1) == '^')
		    s++;

		എങ്കിൽ (*s == NUL
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

		    എങ്കിൽ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			വേണ്ടി ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    എങ്കിൽ (k0 > 1)
			    {
				എങ്കിൽ (word[i + k] != s[1])
				    തുടരുക;
				എങ്കിൽ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    വേണ്ടി (j = 2; j < k0; ++j)
					എങ്കിൽ (*pf++ != s[j])
					    നിർത്തുക;
				    എങ്കിൽ (j < k0)
					തുടരുക;
				}
			    }
			    k0 += k - 1;

			    എങ്കിൽ ((pf = smp[n0].sm_oneof) != ശൂന്യ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ആകുമ്പോൾ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				എങ്കിൽ (*pf == NUL)
				    തുടരുക;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ആകുമ്പോൾ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    എങ്കിൽ (*s == '<')
				s++;
			    എങ്കിൽ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    എങ്കിൽ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				എങ്കിൽ (k0 == k)
				    // this is just a piece of the string
				    തുടരുക;

				എങ്കിൽ (p0 < pri)
				    // priority too low
				    തുടരുക;
				// rule fits; stop search
				നിർത്തുക;
			    }
			}

			എങ്കിൽ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    തുടരുക;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    എങ്കിൽ (s == ശൂന്യ)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != ശൂന്യ) ? 1 : 0;
		    എങ്കിൽ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			എങ്കിൽ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ആകുമ്പോൾ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			എങ്കിൽ (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    അല്ലെങ്കിൽ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ആകുമ്പോൾ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    എങ്കിൽ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			എങ്കിൽ (strstr((അക്ഷരം *)pf, "^^") != ശൂന്യ)
			{
			    എങ്കിൽ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    നിർത്തുക;
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	എങ്കിൽ (z0 == 0)
	{
	    എങ്കിൽ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    സ്ഥിതം ശൂന്യം
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    പൂർണ്ണസംഖ്യ		word[MAXWLEN];
    പൂർണ്ണസംഖ്യ		wres[MAXWLEN];
    പൂർണ്ണസംഖ്യ		l;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		*ws;
    char_u	*t;
    പൂർണ്ണസംഖ്യ		*pf;
    പൂർണ്ണസംഖ്യ		i, j, z;
    പൂർണ്ണസംഖ്യ		reslen;
    പൂർണ്ണസംഖ്യ		n, k = 0;
    പൂർണ്ണസംഖ്യ		z0;
    പൂർണ്ണസംഖ്യ		k0;
    പൂർണ്ണസംഖ്യ		n0;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		pri;
    പൂർണ്ണസംഖ്യ		p0 = -333;
    പൂർണ്ണസംഖ്യ		c0;
    പൂർണ്ണസംഖ്യ		did_white = FALSE;
    പൂർണ്ണസംഖ്യ		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    വേണ്ടി (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	എങ്കിൽ (slang->sl_rem_accents)
	{
	    എങ്കിൽ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		എങ്കിൽ (did_white)
		    തുടരുക;
		c = ' ';
		did_white = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		did_white = FALSE;
		എങ്കിൽ (!spell_iswordp_nmw(t, curwin))
		    തുടരുക;
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
    ആകുമ്പോൾ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	എങ്കിൽ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    വേണ്ടി (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		എങ്കിൽ (c != ws[0])
		    തുടരുക;
		k = smp[n].sm_leadlen;
		എങ്കിൽ (k > 1)
		{
		    എങ്കിൽ (word[i + 1] != ws[1])
			തുടരുക;
		    എങ്കിൽ (k > 2)
		    {
			വേണ്ടി (j = 2; j < k; ++j)
			    എങ്കിൽ (word[i + j] != ws[j])
				നിർത്തുക;
			എങ്കിൽ (j < k)
			    തുടരുക;
		    }
		}

		എങ്കിൽ ((pf = smp[n].sm_oneof_w) != ശൂന്യ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ആകുമ്പോൾ (*pf != NUL && *pf != word[i + k])
			++pf;
		    എങ്കിൽ (*pf == NUL)
			തുടരുക;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ആകുമ്പോൾ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		എങ്കിൽ (*s == '<')
		    s++;
		എങ്കിൽ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		എങ്കിൽ (*s == '^' && *(s + 1) == '^')
		    s++;

		എങ്കിൽ (*s == NUL
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

		    എങ്കിൽ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			വേണ്ടി ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    എങ്കിൽ (c0 != ws[0])
				തുടരുക;
			    k0 = smp[n0].sm_leadlen;
			    എങ്കിൽ (k0 > 1)
			    {
				എങ്കിൽ (word[i + k] != ws[1])
				    തുടരുക;
				എങ്കിൽ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    വേണ്ടി (j = 2; j < k0; ++j)
					എങ്കിൽ (*pf++ != ws[j])
					    നിർത്തുക;
				    എങ്കിൽ (j < k0)
					തുടരുക;
				}
			    }
			    k0 += k - 1;

			    എങ്കിൽ ((pf = smp[n0].sm_oneof_w) != ശൂന്യ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ആകുമ്പോൾ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				എങ്കിൽ (*pf == NUL)
				    തുടരുക;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ആകുമ്പോൾ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    എങ്കിൽ (*s == '<')
				s++;
			    എങ്കിൽ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    എങ്കിൽ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				എങ്കിൽ (k0 == k)
				    // this is just a piece of the string
				    തുടരുക;

				എങ്കിൽ (p0 < pri)
				    // priority too low
				    തുടരുക;
				// rule fits; stop search
				നിർത്തുക;
			    }
			}

			എങ്കിൽ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    തുടരുക;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != ശൂന്യ) ? 1 : 0;
		    എങ്കിൽ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			എങ്കിൽ (reslen > 0 && ws != ശൂന്യ && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			എങ്കിൽ (ws != ശൂന്യ)
			    ആകുമ്പോൾ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			എങ്കിൽ (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    വലുപ്പം(പൂർണ്ണസംഖ്യ) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    അല്ലെങ്കിൽ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			എങ്കിൽ (ws != ശൂന്യ)
			    ആകുമ്പോൾ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				എങ്കിൽ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			എങ്കിൽ (ws == ശൂന്യ)
			    c = NUL;
			അല്ലെങ്കിൽ
			    c = *ws;
			എങ്കിൽ (strstr((അക്ഷരം *)s, "^^") != ശൂന്യ)
			{
			    എങ്കിൽ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       വലുപ്പം(പൂർണ്ണസംഖ്യ) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    നിർത്തുക;
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	എങ്കിൽ (z0 == 0)
	{
	    എങ്കിൽ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    വേണ്ടി (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	എങ്കിൽ (l + MB_MAXBYTES > MAXWLEN)
	    നിർത്തുക;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    ശൂന്യം
ex_spellinfo(exarg_T *eap UNUSED)
{
    പൂർണ്ണസംഖ്യ		lpi;
    langp_T	*lp;
    char_u	*p;

    എങ്കിൽ (no_spell_checking(curwin))
	തിരികെ;

    സന്ദേശം_start();
    വേണ്ടി (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	സന്ദേശം_puts("file: ");
	സന്ദേശം_puts((അക്ഷരം *)lp->lp_slang->sl_fname);
	സന്ദേശം_putchar('\n');
	p = lp->lp_slang->sl_info;
	എങ്കിൽ (p != ശൂന്യ)
	{
	    സന്ദേശം_puts((അക്ഷരം *)p);
	    സന്ദേശം_putchar('\n');
	}
    }
    സന്ദേശം_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_എണ്ണം	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    ശൂന്യം
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    എങ്കിൽ (no_spell_checking(curwin))
	തിരികെ;
    (ശൂന്യം)get_option_മൂല്യം((char_u*)"spl", &dummy, &spl, ശൂന്യ, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_മൂല്യം_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_മൂല്യം_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    എങ്കിൽ (!BUFEMPTY())
	തിരികെ;

    spell_dump_compl(ശൂന്യ, 0, ശൂന്യ, eap->forceit ? DUMPFLAG_എണ്ണം : 0);

    // Delete the empty line that we started with.
    എങ്കിൽ (curbuf->b_ml.ml_വരി_എണ്ണം > 1)
	ml_delete(curbuf->b_ml.ml_വരി_എണ്ണം);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    ശൂന്യം
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    പൂർണ്ണസംഖ്യ		ic,	    // ignore case
    പൂർണ്ണസംഖ്യ		*dir,	    // direction for adding matches
    പൂർണ്ണസംഖ്യ		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    പൂർണ്ണസംഖ്യ		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    പൂർണ്ണസംഖ്യ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    പൂർണ്ണസംഖ്യ		round;
    പൂർണ്ണസംഖ്യ		depth;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		flags;
    char_u	*region_names = ശൂന്യ;	    // region names being used
    പൂർണ്ണസംഖ്യ		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    പൂർണ്ണസംഖ്യ		lpi;
    പൂർണ്ണസംഖ്യ		dumpflags = dumpflags_arg;
    പൂർണ്ണസംഖ്യ		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    എങ്കിൽ (pat != ശൂന്യ)
    {
	എങ്കിൽ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	അല്ലെങ്കിൽ
	{
	    n = captype(pat, ശൂന്യ);
	    എങ്കിൽ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    അല്ലെങ്കിൽ എങ്കിൽ (n == WF_ALLCAP && (പൂർണ്ണസംഖ്യ)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    വേണ്ടി (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	എങ്കിൽ (p[0] != 0)
	{
	    എങ്കിൽ (region_names == ശൂന്യ)	    // first language with regions
		region_names = p;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (do_region && region_names != ശൂന്യ && pat == ശൂന്യ)
    {
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    അല്ലെങ്കിൽ
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    വേണ്ടി (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	എങ്കിൽ (slang->sl_fbyts == ശൂന്യ)	    // reloading failed
	    തുടരുക;

	എങ്കിൽ (pat == ശൂന്യ)
	{
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	എങ്കിൽ (pat != ശൂന്യ && slang->sl_pbyts == ശൂന്യ)
	    patlen = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
	അല്ലെങ്കിൽ
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	വേണ്ടി (round = 1; round <= 2; ++round)
	{
	    എങ്കിൽ (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    അല്ലെങ്കിൽ
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    എങ്കിൽ (byts == ശൂന്യ)
		തുടരുക;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ആകുമ്പോൾ (depth >= 0 && !got_int
				  && (pat == ശൂന്യ || !ins_compl_interrupted()))
	    {
		എങ്കിൽ (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    വരി_breakcheck();
		    ins_compl_check_താക്കോലുകൾ(50, FALSE);
		}
		അല്ലെങ്കിൽ
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    എങ്കിൽ (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (പൂർണ്ണസംഖ്യ)idxs[n];
			എങ്കിൽ ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    എങ്കിൽ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    എങ്കിൽ (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				എങ്കിൽ (pat == ശൂന്യ)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    എങ്കിൽ (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    അല്ലെങ്കിൽ
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
			എങ്കിൽ (depth <= patlen
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
    സ്ഥിതം ശൂന്യം
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		*dir,
    പൂർണ്ണസംഖ്യ		dumpflags,
    പൂർണ്ണസംഖ്യ		wordflags,
    linenr_T	lnum)
{
    പൂർണ്ണസംഖ്യ		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		flags = wordflags;

    എങ്കിൽ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    എങ്കിൽ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    എങ്കിൽ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    അല്ലെങ്കിൽ
    {
	p = word;
	എങ്കിൽ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, ശൂന്യ) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    എങ്കിൽ (pat == ശൂന്യ)
    {
	// Add flags and regions after a slash.
	എങ്കിൽ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    എങ്കിൽ (keepcap)
		STRCAT(badword, "=");
	    എങ്കിൽ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    അല്ലെങ്കിൽ എങ്കിൽ (flags & WF_RARE)
		STRCAT(badword, "?");
	    എങ്കിൽ (flags & WF_REGION)
		വേണ്ടി (i = 0; i < 7; ++i)
		    എങ്കിൽ (flags & (0x10000 << i))
			sprintf((അക്ഷരം *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	എങ്കിൽ (dumpflags & DUMPFLAG_എണ്ണം)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_എണ്ണം);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (പൂർണ്ണസംഖ്യ)STRLEN(p),
					  p_ic, ശൂന്യ, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    സ്ഥിതം linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		*dir,
    പൂർണ്ണസംഖ്യ		dumpflags,
    പൂർണ്ണസംഖ്യ		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    പൂർണ്ണസംഖ്യ		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    പൂർണ്ണസംഖ്യ		has_word_up = FALSE;
    പൂർണ്ണസംഖ്യ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    പൂർണ്ണസംഖ്യ		depth;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    എങ്കിൽ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    എങ്കിൽ (byts != ശൂന്യ)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ആകുമ്പോൾ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    എങ്കിൽ (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		വരി_breakcheck();
	    }
	    അല്ലെങ്കിൽ
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		എങ്കിൽ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    വേണ്ടി (i = 1; i < len; ++i)
			എങ്കിൽ (byts[n + i] != 0)
			    നിർത്തുക;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    എങ്കിൽ (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			എങ്കിൽ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    എങ്കിൽ (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			എങ്കിൽ (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    എങ്കിൽ (lnum != 0)
				++lnum;
			}
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    തിരികെ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    ആകുമ്പോൾ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    തിരികെ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    പൂർണ്ണസംഖ്യ
spell_word_start(പൂർണ്ണസംഖ്യ startcol)
{
    char_u	*വരി;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		col = 0;

    എങ്കിൽ (no_spell_checking(curwin))
	തിരികെ startcol;

    // Find a word character before "startcol".
    വരി = ml_get_curline();
    വേണ്ടി (p = വരി + startcol; p > വരി; )
    {
	MB_PTR_BACK(വരി, p);
	എങ്കിൽ (spell_iswordp_nmw(p, curwin))
	    നിർത്തുക;
    }

    // Go back to start of the word.
    ആകുമ്പോൾ (p > വരി)
    {
	col = (പൂർണ്ണസംഖ്യ)(p - വരി);
	MB_PTR_BACK(വരി, p);
	എങ്കിൽ (!spell_iswordp(p, curwin))
	    നിർത്തുക;
	col = 0;
    }

    തിരികെ col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ spell_expand_need_cap;

    ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    തിരികെ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    പൂർണ്ണസംഖ്യ
valid_spelllang(char_u *val)
{
    തിരികെ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    പൂർണ്ണസംഖ്യ
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		l;

    spf = val;
    ആകുമ്പോൾ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	എങ്കിൽ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    തിരികെ FALSE;
	വേണ്ടി (s = spf_name; *s != NUL; ++s)
	    എങ്കിൽ (!vim_is_fname_char(*s))
		തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    അക്ഷരം *
did_set_spell_option(ശൂന്യം)
{
    അക്ഷരം    *errmsg = ശൂന്യ;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    നിർത്തുക;
	}
    തിരികെ errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    അക്ഷരം *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    എങ്കിൽ (synblock->b_p_spc == ശൂന്യ || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	എങ്കിൽ (re != ശൂന്യ)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    എങ്കിൽ (synblock->b_cap_prog == ശൂന്യ)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		തിരികെ e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    തിരികെ ശൂന്യ;
}

#endif  // FEAT_SPELL
