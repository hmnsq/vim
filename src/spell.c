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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_SPELL_C
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SPELL)

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGION_ALL 0xff		// word valid in all regions

// Result values.  Lower number is accepted over higher one.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_BANNED	(-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_OK		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_RARE		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_LOCAL	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_BAD		3

/*
 * Structure to store info for word matching.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ matchinf_S
{
    langp_T	*mi_lp;			// info for language and region

    // pointers to original text to be checked
    char_u	*mi_ਸ਼ਬਦ;		// start of word being checked
    char_u	*mi_ਸਿਰਾ;		// end of matching word so far
    char_u	*mi_fend;		// next char to be added to mi_fword
    char_u	*mi_cend;		// char after what was used for
					// mi_capflags

    // case-folded text
    char_u	mi_fword[MAXWLEN + 1];	// mi_word case-folded
    ਪੂਰਨ		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    ਪੂਰਨ		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    ਪੂਰਨ		mi_prefcnt;		// number of entries at mi_prefarridx
    ਪੂਰਨ		mi_prefixlen;		// byte length of prefix
    ਪੂਰਨ		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    ਪੂਰਨ		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    ਪੂਰਨ		mi_complen;		// nr of compound words used
    ਪੂਰਨ		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    ਪੂਰਨ		mi_ਨਤੀਜਾ;		// result so far: SP_BAD, SP_OK, etc.
    ਪੂਰਨ		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    ਪੂਰਨ		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


ਸਥਿਰ ਪੂਰਨ spell_mb_isword_class(ਪੂਰਨ cl, win_T *wp);

// mode values for find_word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_FOLDWORD	    0	// find word case-folded
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_KEEPWORD	    1	// find keep-case word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_ਅਗੇਤਰ	    2	// find word after prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_COMPOUND	    3	// find case-folded compound word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_KEEPCOMPOUND   4	// find keep-case compound word

// type values for get_char_type
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHAR_OTHER	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHAR_ਉੱਤਲਾ	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHAR_DIGIT	2

ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਸ਼ਬਦ(matchinf_T *mip, ਪੂਰਨ ਬਹੁਲਕ);
ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_ਅਗੇਤਰ_2(matchinf_T *mip, ਪੂਰਨ ਬਹੁਲਕ);
ਸਥਿਰ ਪੂਰਨ fold_ਹੋਰ(matchinf_T *mip);
ਸਥਿਰ ਖਾਲੀ spell_ਲੱਦੋ_cb(char_u *fname, ਖਾਲੀ *cookie);
ਸਥਿਰ ਪੂਰਨ ਗਿਣਤੀ_syllables(slang_T *slang, char_u *ਸ਼ਬਦ);
ਸਥਿਰ ਖਾਲੀ clear_midword(win_T *buf);
ਸਥਿਰ ਖਾਲੀ use_midword(slang_T *lp, win_T *buf);
ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_region(char_u *rp, char_u *region);
ਸਥਿਰ ਖਾਲੀ spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
ਸਥਿਰ ਖਾਲੀ spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
ਸਥਿਰ ਖਾਲੀ spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
ਸਥਿਰ ਖਾਲੀ dump_ਸ਼ਬਦ(slang_T *slang, char_u *ਸ਼ਬਦ, char_u *pat, ਪੂਰਨ *dir, ਪੂਰਨ ਗੇੜਾ, ਪੂਰਨ flags, linenr_T lnum);
ਸਥਿਰ linenr_T dump_prefixes(slang_T *slang, char_u *ਸ਼ਬਦ, char_u *pat, ਪੂਰਨ *dir, ਪੂਰਨ ਗੇੜਾ, ਪੂਰਨ flags, linenr_T startlnum);
ਸਥਿਰ char_u *advance_camelcase_ਸ਼ਬਦ(char_u *p, win_T *wp, ਪੂਰਨ *is_camel_case);

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
    ਪੂਰਨ
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    ਪੂਰਨ		*capcol,	// column to check for Capital
    ਪੂਰਨ		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    ਪੂਰਨ		nrlen = 0;	// found a number first
    ਪੂਰਨ		c;
    ਪੂਰਨ		wrongcaplen = 0;
    ਪੂਰਨ		lpi;
    ਪੂਰਨ		ਗਿਣਤੀ_ਸ਼ਬਦ = docount;
    ਪੂਰਨ		use_camel_case = *wp->w_s->b_p_spo != NUL;
    ਪੂਰਨ		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ਜੇ (*ptr <= ' ')
	ਵਾਪਸ 1;

    // Return here when loading language files failed.
    ਜੇ (wp->w_s->b_langp.ga_len == 0)
	ਵਾਪਸ 1;

    CLEAR_ਖੇਤਰ(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ਜੇ (*ptr >= '0' && *ptr <= '9')
    {
	ਜੇ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_ਸਿਰਾ = skipbin(ptr + 2);
	ਨਹੀਂਤਾਂ ਜੇ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_ਸਿਰਾ = skiphex(ptr + 2);
	ਨਹੀਂਤਾਂ
	    mi.mi_ਸਿਰਾ = skipdigits(ptr);
	nrlen = (ਪੂਰਨ)(mi.mi_ਸਿਰਾ - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_ਸ਼ਬਦ = ptr;
    mi.mi_fend = ptr;
    ਜੇ (spell_iswordp(mi.mi_fend, wp))
    {
	ਜੇ (use_camel_case)
	    mi.mi_fend = advance_camelcase_ਸ਼ਬਦ(ptr, wp, &is_camel_case);
	ਨਹੀਂਤਾਂ
	{
	    ਕਰੋ
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ਜਦੋਂ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ਜੇ (capcol != ਨਿਰਾ && *capcol == 0 && wp->w_s->b_cap_prog != ਨਿਰਾ)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ਜੇ (!SPELL_ISUPPER(c))
		wrongcaplen = (ਪੂਰਨ)(mi.mi_fend - ptr);
	}
    }
    ਜੇ (capcol != ਨਿਰਾ)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_ਸਿਰਾ = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ਨਿਰਾ;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    ਜੇ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (ਖਾਲੀ)spell_casefold(wp, ptr, (ਪੂਰਨ)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (ਪੂਰਨ)STRLEN(mi.mi_fword);

    ਜੇ (is_camel_case && mi.mi_fwordlen > 0)
	// Introduce a fake word end space into the folded word.
	mi.mi_fword[mi.mi_fwordlen - 1] = ' ';

    // The word is bad unless we recognize it.
    mi.mi_ਨਤੀਜਾ = SP_BAD;
    mi.mi_result2 = SP_BAD;

    /*
     * Loop over the languages specified in 'spelllang'.
     * We check them all, because a word may be matched longer in another
     * language.
     */
    ਲਈ (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ਇੰਦਰਾਜ(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ਜੇ (mi.mi_lp->lp_slang->sl_fidxs == ਨਿਰਾ)
	    ਜਾਰੀ;

	// Check for a matching word in case-folded words.
	ਲੱਭੋ_ਸ਼ਬਦ(&mi, ਲੱਭੋ_FOLDWORD);

	// Check for a matching word in keep-case words.
	ਲੱਭੋ_ਸ਼ਬਦ(&mi, ਲੱਭੋ_KEEPWORD);

	// Check for matching prefixes.
	ਲੱਭੋ_ਅਗੇਤਰ_2(&mi, ਲੱਭੋ_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ਜੇ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_ਨਤੀਜਾ == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_ਨਤੀਜਾ = mi.mi_result2;
	    mi.mi_ਸਿਰਾ = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ਜੇ (ਗਿਣਤੀ_ਸ਼ਬਦ && mi.mi_ਨਤੀਜਾ == SP_OK)
	{
	    ਗਿਣਤੀ_common_ਸ਼ਬਦ(mi.mi_lp->lp_slang, ptr,
						   (ਪੂਰਨ)(mi.mi_ਸਿਰਾ - ptr), 1);
	    ਗਿਣਤੀ_ਸ਼ਬਦ = FALSE;
	}
    }

    ਜੇ (mi.mi_ਨਤੀਜਾ != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ਜੇ (nrlen > 0)
	{
	    ਜੇ (mi.mi_ਨਤੀਜਾ == SP_BAD || mi.mi_ਨਤੀਜਾ == SP_BANNED)
		ਵਾਪਸ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	ਨਹੀਂਤਾਂ ਜੇ (!spell_iswordp_nmw(ptr, wp))
	{
	    ਜੇ (capcol != ਨਿਰਾ && wp->w_s->b_cap_prog != ਨਿਰਾ)
	    {
		regmatch_T	regmatch;
		ਪੂਰਨ		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ਜੇ (r)
		    *capcol = (ਪੂਰਨ)(regmatch.endp[0] - ptr);
	    }

	    ਜੇ (has_mbyte)
		ਵਾਪਸ (*mb_ptr2len)(ptr);
	    ਵਾਪਸ 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (mi.mi_ਸਿਰਾ == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_ਸਿਰਾ);
	ਨਹੀਂਤਾਂ ਜੇ (mi.mi_ਨਤੀਜਾ == SP_BAD
		&& LANGP_ਇੰਦਰਾਜ(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    ਪੂਰਨ		save_ਨਤੀਜਾ = mi.mi_ਨਤੀਜਾ;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ਇੰਦਰਾਜ(wp->w_s->b_langp, 0);
	    ਜੇ (mi.mi_lp->lp_slang->sl_fidxs != ਨਿਰਾ)
	    {
		p = mi.mi_ਸ਼ਬਦ;
		fp = mi.mi_fword;
		ਲਈ (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ਜੇ (p >= mi.mi_ਸਿਰਾ)
			ਰੋਕੋ;
		    mi.mi_compoff = (ਪੂਰਨ)(fp - mi.mi_fword);
		    ਲੱਭੋ_ਸ਼ਬਦ(&mi, ਲੱਭੋ_COMPOUND);
		    ਜੇ (mi.mi_ਨਤੀਜਾ != SP_BAD)
		    {
			mi.mi_ਸਿਰਾ = p;
			ਰੋਕੋ;
		    }
		}
		mi.mi_ਨਤੀਜਾ = save_ਨਤੀਜਾ;
	    }
	}

	ਜੇ (mi.mi_ਨਤੀਜਾ == SP_BAD || mi.mi_ਨਤੀਜਾ == SP_BANNED)
	    *attrp = HLF_SPB;
	ਨਹੀਂਤਾਂ ਜੇ (mi.mi_ਨਤੀਜਾ == SP_RARE)
	    *attrp = HLF_SPR;
	ਨਹੀਂਤਾਂ
	    *attrp = HLF_SPL;
    }

    ਜੇ (wrongcaplen > 0 && (mi.mi_ਨਤੀਜਾ == SP_OK || mi.mi_ਨਤੀਜਾ == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ਵਾਪਸ wrongcaplen;
    }

    ਵਾਪਸ (ਪੂਰਨ)(mi.mi_ਸਿਰਾ - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    ਸਥਿਰ ਪੂਰਨ
get_char_type(ਪੂਰਨ c)
{
    ਜੇ (VIM_ISDIGIT(c))
	ਵਾਪਸ CHAR_DIGIT;
    ਜੇ (SPELL_ISUPPER(c))
	ਵਾਪਸ CHAR_ਉੱਤਲਾ;
    ਵਾਪਸ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    ਸਥਿਰ char_u *
advance_camelcase_ਸ਼ਬਦ(char_u *str, win_T *wp, ਪੂਰਨ *is_camel_case)
{
    ਪੂਰਨ ਆਖਰੀ_type, ਆਖਰੀ_ਆਖਰੀ_type, this_type;
    ਪੂਰਨ c;
    char_u *ਸਿਰਾ = str;

    *is_camel_case = FALSE;

    ਜੇ (*str == NUL)
	ਵਾਪਸ str;

    c = PTR2CHAR(ਸਿਰਾ);
    MB_PTR_ADV(ਸਿਰਾ);
    // We need at most the types of the type of the last two chars.
    ਆਖਰੀ_ਆਖਰੀ_type = -1;
    ਆਖਰੀ_type = get_char_type(c);

    ਜਦੋਂ (*ਸਿਰਾ != NUL && spell_iswordp(ਸਿਰਾ, wp))
    {
	c = PTR2CHAR(ਸਿਰਾ);
	this_type = get_char_type(c);

	ਜੇ (ਆਖਰੀ_ਆਖਰੀ_type == CHAR_ਉੱਤਲਾ && ਆਖਰੀ_type == CHAR_ਉੱਤਲਾ
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_ਪਿੱਠ(str, ਸਿਰਾ);
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ ((this_type == CHAR_ਉੱਤਲਾ && ਆਖਰੀ_type == CHAR_OTHER)
		|| (this_type != ਆਖਰੀ_type
		    && (this_type == CHAR_DIGIT || ਆਖਰੀ_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    ਰੋਕੋ;
	}

	ਆਖਰੀ_ਆਖਰੀ_type = ਆਖਰੀ_type;
	ਆਖਰੀ_type = this_type;

	MB_PTR_ADV(ਸਿਰਾ);
    }

    ਵਾਪਸ ਸਿਰਾ;
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
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਸ਼ਬਦ(matchinf_T *mip, ਪੂਰਨ ਬਹੁਲਕ)
{
    idx_T	arridx = 0;
    ਪੂਰਨ		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    ਪੂਰਨ		endidxcnt = 0;
    ਪੂਰਨ		len;
    ਪੂਰਨ		wlen = 0;
    ਪੂਰਨ		flen;
    ਪੂਰਨ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    ਪੂਰਨ		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	flags;
    char_u	*byts;
    idx_T	*idxs;
    ਪੂਰਨ		ਸ਼ਬਦ_ਸਿਰੇ;
    ਪੂਰਨ		ਅਗੇਤਰ_ਮਿਲਿਆ;
    ਪੂਰਨ		nobreak_ਨਤੀਜਾ;

    ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_KEEPWORD || ਬਹੁਲਕ == ਲੱਭੋ_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_ਸ਼ਬਦ;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    ਨਹੀਂਤਾਂ
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_ਅਗੇਤਰ)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ਜੇ (byts == ਨਿਰਾ)
	ਵਾਪਸ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    ਲਈ (;;)
    {
	ਜੇ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_ਹੋਰ(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ਜੇ (byts[arridx] == 0)
	{
	    ਜੇ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_ਵਿੱਚ_spell_file));
		ਵਾਪਸ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ਜਦੋਂ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ਜੇ (len == 0)
		ਰੋਕੋ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ਜੇ (ptr[wlen] == NUL)
	    ਰੋਕੋ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ਜੇ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ਜਦੋਂ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ਜੇ (byts[m] > c)
		hi = m - 1;
	    ਨਹੀਂਤਾਂ ਜੇ (byts[m] < c)
		lo = m + 1;
	    ਨਹੀਂਤਾਂ
	    {
		lo = hi = m;
		ਰੋਕੋ;
	    }
	}

	// Stop if there is no matching byte.
	ਜੇ (hi < lo || byts[lo] != c)
	    ਰੋਕੋ;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ਜੇ (c == ' ')
	{
	    ਲਈ (;;)
	    {
		ਜੇ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_ਹੋਰ(mip);
		ਜੇ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    ਰੋਕੋ;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    ਜਦੋਂ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ਜੇ ((*mb_ਸਿਰ_off)(ptr, ptr + wlen) > 0)
	    ਜਾਰੀ;	    // not at first byte of character
	ਜੇ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ਜੇ (slang->sl_compprog == ਨਿਰਾ && !slang->sl_nobreak)
		ਜਾਰੀ;	    // next char is a word character
	    ਸ਼ਬਦ_ਸਿਰੇ = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    ਸ਼ਬਦ_ਸਿਰੇ = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	ਅਗੇਤਰ_ਮਿਲਿਆ = FALSE;

	ਜੇ (ਬਹੁਲਕ != ਲੱਭੋ_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_ਸ਼ਬਦ;
	    ਜੇ (STRNCMP(ptr, p, wlen) != 0)
	    {
		ਲਈ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (ਪੂਰਨ)(p - mip->mi_ਸ਼ਬਦ);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	ਲਈ (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_FOLDWORD)
	    {
		ਜੇ (mip->mi_cend != mip->mi_ਸ਼ਬਦ + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_ਸ਼ਬਦ + wlen;
		    mip->mi_capflags = captype(mip->mi_ਸ਼ਬਦ, mip->mi_cend);
		}

		ਜੇ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    ਜਾਰੀ;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_ਅਗੇਤਰ && !ਅਗੇਤਰ_ਮਿਲਿਆ)
	    {
		c = valid_ਸ਼ਬਦ_ਅਗੇਤਰ(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_ਸ਼ਬਦ + mip->mi_cprefixlen, slang,
				    FALSE);
		ਜੇ (c == 0)
		    ਜਾਰੀ;

		// Use the WF_RARE flag for a rare prefix.
		ਜੇ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		ਅਗੇਤਰ_ਮਿਲਿਆ = TRUE;
	    }

	    ਜੇ (slang->sl_nobreak)
	    {
		ਜੇ ((ਬਹੁਲਕ == ਲੱਭੋ_COMPOUND || ਬਹੁਲਕ == ਲੱਭੋ_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_ਨਤੀਜਾ = SP_OK;
		    ਰੋਕੋ;
		}
	    }

	    ਨਹੀਂਤਾਂ ਜੇ ((ਬਹੁਲਕ == ਲੱਭੋ_COMPOUND || ਬਹੁਲਕ == ਲੱਭੋ_KEEPCOMPOUND
								|| !ਸ਼ਬਦ_ਸਿਰੇ))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ਜੇ (((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    ਜਾਰੀ;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ਜੇ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_ਸ਼ਬਦ + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			ਜਾਰੀ;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ਜੇ (!ਸ਼ਬਦ_ਸਿਰੇ && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    ਜਾਰੀ;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ਜੇ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    ਜਾਰੀ;
		ਜੇ (!ਸ਼ਬਦ_ਸਿਰੇ && (flags & WF_NOCOMPAFT))
		    ਜਾਰੀ;

		// Quickly check if compounding is possible with this flag.
		ਜੇ (!byte_ਵਿੱਚ_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24)))
		    ਜਾਰੀ;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ਜੇ (ਮੇਲ_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    ਜਾਰੀ;

		ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_COMPOUND)
		{
		    ਪੂਰਨ	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ਜੇ (has_mbyte && STRNCMP(ptr, mip->mi_ਸ਼ਬਦ,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_ਸ਼ਬਦ;
			ਲਈ (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    ਨਹੀਂਤਾਂ
			p = mip->mi_ਸ਼ਬਦ + mip->mi_compoff;
		    capflags = captype(p, mip->mi_ਸ਼ਬਦ + wlen);
		    ਜੇ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			ਜਾਰੀ;

		    ਜੇ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_ਪਿੱਠ(mip->mi_ਸ਼ਬਦ, p);
			ਜੇ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    ਜਾਰੀ;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		ਜੇ (ਸ਼ਬਦ_ਸਿਰੇ)
		{
		    char_u	fword[MAXWLEN];

		    ਜੇ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ਜੇ (ptr == mip->mi_ਸ਼ਬਦ)
			    (ਖਾਲੀ)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			ਨਹੀਂਤਾਂ
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ਜੇ (!can_compound(slang, fword, mip->mi_compflags))
			ਜਾਰੀ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (slang->sl_comprules != ਨਿਰਾ
			     && !ਮੇਲ_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    ਜਾਰੀ;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    ਨਹੀਂਤਾਂ ਜੇ (flags & WF_NEEDCOMP)
		ਜਾਰੀ;

	    nobreak_ਨਤੀਜਾ = SP_OK;

	    ਜੇ (!ਸ਼ਬਦ_ਸਿਰੇ)
	    {
		ਪੂਰਨ	save_ਨਤੀਜਾ = mip->mi_ਨਤੀਜਾ;
		char_u	*save_ਸਿਰਾ = mip->mi_ਸਿਰਾ;
		langp_T	*save_lp = mip->mi_lp;
		ਪੂਰਨ	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ਜੇ (slang->sl_nobreak)
		    mip->mi_ਨਤੀਜਾ = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ਜੇ (has_mbyte && ਬਹੁਲਕ == ਲੱਭੋ_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ਜੇ (STRNCMP(ptr, p, wlen) != 0)
		    {
			ਲਈ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (ਪੂਰਨ)(p - mip->mi_fword);
		    }
		}
#ਜੇ 0 // Disabled, see below
		c = mip->mi_compoff;
#ਜੇ_ਅੰਤ
		++mip->mi_complen;
		ਜੇ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		ਲਈ (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ਜੇ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ਇੰਦਰਾਜ(mip->mi_win->w_s->b_langp, lpi);
			ਜੇ (mip->mi_lp->lp_slang->sl_fidxs == ਨਿਰਾ
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    ਜਾਰੀ;
		    }

		    ਲੱਭੋ_ਸ਼ਬਦ(mip, ਲੱਭੋ_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ਜੇ (!slang->sl_nobreak || mip->mi_ਨਤੀਜਾ == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			ਲੱਭੋ_ਸ਼ਬਦ(mip, ਲੱਭੋ_KEEPCOMPOUND);

#ਜੇ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ਜੇ (!slang->sl_nobreak || mip->mi_ਨਤੀਜਾ == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    ਲੱਭੋ_ਅਗੇਤਰ_2(mip, ਲੱਭੋ_COMPOUND);
			}
#ਜੇ_ਅੰਤ
		    }

		    ਜੇ (!slang->sl_nobreak)
			ਰੋਕੋ;
		}
		--mip->mi_complen;
		ਜੇ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ਜੇ (slang->sl_nobreak)
		{
		    nobreak_ਨਤੀਜਾ = mip->mi_ਨਤੀਜਾ;
		    mip->mi_ਨਤੀਜਾ = save_ਨਤੀਜਾ;
		    mip->mi_ਸਿਰਾ = save_ਸਿਰਾ;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (mip->mi_ਨਤੀਜਾ == SP_OK)
			ਰੋਕੋ;
		    ਜਾਰੀ;
		}
	    }

	    ਜੇ (flags & WF_BANNED)
		res = SP_BANNED;
	    ਨਹੀਂਤਾਂ ਜੇ (flags & WF_REGION)
	    {
		// Check region.
		ਜੇ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		ਨਹੀਂਤਾਂ
		    res = SP_LOCAL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (flags & WF_RARE)
		res = SP_RARE;
	    ਨਹੀਂਤਾਂ
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ਜੇ (nobreak_ਨਤੀਜਾ == SP_BAD)
	    {
		ਜੇ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_ਸ਼ਬਦ + wlen;
		}
		ਨਹੀਂਤਾਂ ਜੇ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_ਸ਼ਬਦ + wlen)
		    mip->mi_end2 = mip->mi_ਸ਼ਬਦ + wlen;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (mip->mi_ਨਤੀਜਾ > res)
	    {
		mip->mi_ਨਤੀਜਾ = res;
		mip->mi_ਸਿਰਾ = mip->mi_ਸ਼ਬਦ + wlen;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (mip->mi_ਨਤੀਜਾ == res && mip->mi_ਸਿਰਾ < mip->mi_ਸ਼ਬਦ + wlen)
		mip->mi_ਸਿਰਾ = mip->mi_ਸ਼ਬਦ + wlen;

	    ਜੇ (mip->mi_ਨਤੀਜਾ == SP_OK)
		ਰੋਕੋ;
	}

	ਜੇ (mip->mi_ਨਤੀਜਾ == SP_OK)
	    ਰੋਕੋ;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    ਪੂਰਨ
ਮੇਲ_checkcompoundpattern(
    char_u	*ptr,
    ਪੂਰਨ		wlen,
    garray_T	*gap)  // &sl_comppat
{
    ਪੂਰਨ		i;
    char_u	*p;
    ਪੂਰਨ		len;

    ਲਈ (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_ਅੰਕੜੇ)[i + 1];
	ਜੇ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_ਅੰਕੜੇ)[i];
	    len = (ਪੂਰਨ)STRLEN(p);
	    ਜੇ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    ਪੂਰਨ
can_compound(slang_T *slang, char_u *ਸ਼ਬਦ, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    ਪੂਰਨ		i;
    char_u	*p;

    ਜੇ (slang->sl_compprog == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਜੇ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	ਲਈ (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    ਨਹੀਂਤਾਂ
	p = flags;
    ਜੇ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ਵਾਪਸ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ਜੇ (slang->sl_compsylmax < MAXWLEN
		       && ਗਿਣਤੀ_syllables(slang, ਸ਼ਬਦ) > slang->sl_compsylmax)
	ਵਾਪਸ (ਪੂਰਨ)STRLEN(flags) < slang->sl_compmax;
    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    ਪੂਰਨ
ਮੇਲ_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;

    // loop over all the COMPOUNDRULE entries
    ਲਈ (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	ਲਈ (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ਜੇ (c == NUL)
		// found a rule that matches for the flags we have so far
		ਵਾਪਸ TRUE;
	    ਜੇ (*p == '/' || *p == NUL)
		ਰੋਕੋ;  // end of rule, it's too short
	    ਜੇ (*p == '[')
	    {
		ਪੂਰਨ ਮੇਲ = FALSE;

		// compare against all the flags in []
		++p;
		ਜਦੋਂ (*p != ']' && *p != NUL)
		    ਜੇ (*p++ == c)
			ਮੇਲ = TRUE;
		ਜੇ (!ਮੇਲ)
		    ਰੋਕੋ;  // none matches
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p != c)
		ਰੋਕੋ;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ਜੇ (p == ਨਿਰਾ)
	    ਰੋਕੋ;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ਵਾਪਸ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    ਪੂਰਨ
valid_ਸ਼ਬਦ_ਅਗੇਤਰ(
    ਪੂਰਨ		totprefcnt,	// nr of prefix IDs
    ਪੂਰਨ		arridx,		// idx in sl_pidxs[]
    ਪੂਰਨ		flags,
    char_u	*ਸ਼ਬਦ,
    slang_T	*slang,
    ਪੂਰਨ		cond_req)	// only use prefixes with a condition
{
    ਪੂਰਨ		prefcnt;
    ਪੂਰਨ		pidx;
    regprog_T	**rp;
    ਪੂਰਨ		prefid;

    prefid = (ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24;
    ਲਈ (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ਜੇ (prefid != (pidx & 0xff))
	    ਜਾਰੀ;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ਜੇ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    ਜਾਰੀ;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((ਬਿਨਾਨਿਸ਼ਾਨੀ)pidx >> 8) & 0xffff];
	ਜੇ (*rp != ਨਿਰਾ)
	{
	    ਜੇ (!vim_regexec_prog(rp, FALSE, ਸ਼ਬਦ, 0))
		ਜਾਰੀ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (cond_req)
	    ਜਾਰੀ;

	// It's a match!  Return the WF_ flags.
	ਵਾਪਸ pidx;
    }
    ਵਾਪਸ 0;
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
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਅਗੇਤਰ_2(matchinf_T *mip, ਪੂਰਨ ਬਹੁਲਕ)
{
    idx_T	arridx = 0;
    ਪੂਰਨ		len;
    ਪੂਰਨ		wlen = 0;
    ਪੂਰਨ		flen;
    ਪੂਰਨ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ਜੇ (byts == ਨਿਰਾ)
	ਵਾਪਸ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_COMPOUND)
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
    ਲਈ (;;)
    {
	ਜੇ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_ਹੋਰ(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ਜੇ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ਜਦੋਂ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ਜੇ (ਬਹੁਲਕ == ਲੱਭੋ_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ਜੇ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_ਸ਼ਬਦ);
	    }
	    ਨਹੀਂਤਾਂ
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    ਲੱਭੋ_ਸ਼ਬਦ(mip, ਲੱਭੋ_ਅਗੇਤਰ);


	    ਜੇ (len == 0)
		ਰੋਕੋ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ਜੇ (ptr[wlen] == NUL)
	    ਰੋਕੋ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ਜਦੋਂ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ਜੇ (byts[m] > c)
		hi = m - 1;
	    ਨਹੀਂਤਾਂ ਜੇ (byts[m] < c)
		lo = m + 1;
	    ਨਹੀਂਤਾਂ
	    {
		lo = hi = m;
		ਰੋਕੋ;
	    }
	}

	// Stop if there is no matching byte.
	ਜੇ (hi < lo || byts[lo] != c)
	    ਰੋਕੋ;

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
    ਸਥਿਰ ਪੂਰਨ
fold_ਹੋਰ(matchinf_T *mip)
{
    ਪੂਰਨ		flen;
    char_u	*p;

    p = mip->mi_fend;
    ਕਰੋ
	MB_PTR_ADV(mip->mi_fend);
    ਜਦੋਂ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ਜੇ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (ਖਾਲੀ)spell_casefold(mip->mi_win, p, (ਪੂਰਨ)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (ਪੂਰਨ)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ਵਾਪਸ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    ਪੂਰਨ
spell_valid_case(
    ਪੂਰਨ	    wordflags,	    // flags for the checked word.
    ਪੂਰਨ	    treeflags)	    // flags for the word in the spell tree
{
    ਵਾਪਸ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    ਪੂਰਨ
spell_check_ਖਿੜਕੀ(win_T *wp)
{
    ਵਾਪਸ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(ਅੱਖਰ **)(wp->w_s->b_langp.ga_ਅੰਕੜੇ) != ਨਿਰਾ;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    ਸਥਿਰ ਪੂਰਨ
no_spell_checking(win_T *wp)
{
    ਜੇ (spell_check_ਖਿੜਕੀ(wp))
	ਵਾਪਸ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ਵਾਪਸ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    ਪੂਰਨ
spell_move_ਮੰਜ਼ਿਲ(
    win_T	*wp,
    ਪੂਰਨ		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    ਪੂਰਨ		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	ਮਿਲਿਆ_pos;
    ਪੂਰਨ		ਮਿਲਿਆ_len = 0;
    char_u	*ਸਤਰ;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    ਪੂਰਨ		len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		has_syntax = syntax_present(wp);
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		can_spell;
    char_u	*buf = ਨਿਰਾ;
    ਪੂਰਨ		buflen = 0;
    ਪੂਰਨ		skip = 0;
    ਪੂਰਨ		capcol = -1;
    ਪੂਰਨ		ਮਿਲਿਆ_one = FALSE;
    ਪੂਰਨ		wrapped = FALSE;

    ਜੇ (no_spell_checking(wp))
	ਵਾਪਸ 0;

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
    CLEAR_POS(&ਮਿਲਿਆ_pos);

    ਜਦੋਂ (!got_int)
    {
	ਪੂਰਨ ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ;

	ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);

	len = ml_get_buf_len(wp->w_ਬਫਰ, lnum);
	ਜੇ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ਜੇ (buf == ਨਿਰਾ)
		ਰੋਕੋ;
	}

	// In first line check first word for Capital.
	ਜੇ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ਜੇ (capcol == 0)
	    capcol = getwhitecols(ਸਤਰ);
	ਨਹੀਂਤਾਂ ਜੇ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    ਖਾਨਾ = getwhitecols(ਸਤਰ);
	    ਜੇ (check_need_cap(curwin, lnum, ਖਾਨਾ))
		capcol = ਖਾਨਾ;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ = *skipwhite(ਸਤਰ) == NUL;
	STRCPY(buf, ਸਤਰ);
	ਜੇ (lnum < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    spell_cat_ਸਤਰ(buf + STRLEN(buf),
			  ml_get_buf(wp->w_ਬਫਰ, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ਜਦੋਂ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ਜੇ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.ਖਾਨਾ)
		ਰੋਕੋ;

	    // start of word
	    attr = HLF_ਗਿਣਤੀ;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ਜੇ (attr != HLF_ਗਿਣਤੀ)
	    {
		// We found a bad word.  Check the attribute.
		ਜੇ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ਜੇ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.ਖਾਨਾ))
		    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			ਜੇ (has_syntax)
			{
			    ਖਾਨਾ = (ਪੂਰਨ)(p - buf);
			    (ਖਾਲੀ)syn_get_id(wp, lnum, (colnr_T)ਖਾਨਾ,
						    FALSE, &can_spell, FALSE);
			    ਜੇ (!can_spell)
				attr = HLF_ਗਿਣਤੀ;
			}
			ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			    can_spell = TRUE;

			ਜੇ (can_spell)
			{
			    ਮਿਲਿਆ_one = TRUE;
			    ਮਿਲਿਆ_pos.lnum = lnum;
			    ਮਿਲਿਆ_pos.ਖਾਨਾ = (ਪੂਰਨ)(p - buf);
			    ਮਿਲਿਆ_pos.coladd = 0;
			    ਜੇ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = ਮਿਲਿਆ_pos;
				vim_free(buf);
				ਜੇ (attrp != ਨਿਰਾ)
				    *attrp = attr;
				ਵਾਪਸ len;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				ਮਿਲਿਆ_pos.ਖਾਨਾ += len;
			    ਮਿਲਿਆ_len = len;
			}
		    }
		    ਨਹੀਂਤਾਂ
			ਮਿਲਿਆ_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ਜੇ (dir == BACKWARD && ਮਿਲਿਆ_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = ਮਿਲਿਆ_pos;
	    vim_free(buf);
	    ਵਾਪਸ ਮਿਲਿਆ_len;
	}

	ਜੇ (curline)
	    ਰੋਕੋ;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ਜੇ (lnum == wp->w_cursor.lnum && wrapped)
	    ਰੋਕੋ;

	// Advance to next line.
	ਜੇ (dir == BACKWARD)
	{
	    ਜੇ (lnum > 1)
		--lnum;
	    ਨਹੀਂਤਾਂ ਜੇ (!p_ws)
		ਰੋਕੋ;	    // at first line and 'nowrapscan'
	    ਨਹੀਂਤਾਂ
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		wrapped = TRUE;
		ਜੇ (!shortmess(SHM_ਖੋਜ))
		    give_warning((char_u *)_(ਸਿਖਰ_bot_ਸੁਨੇਹਾ), TRUE);
	    }
	    capcol = -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (lnum < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		++lnum;
	    ਨਹੀਂਤਾਂ ਜੇ (!p_ws)
		ਰੋਕੋ;	    // at first line and 'nowrapscan'
	    ਨਹੀਂਤਾਂ
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ਜੇ (!shortmess(SHM_ਖੋਜ))
		    give_warning((char_u *)_(bot_ਸਿਖਰ_ਸੁਨੇਹਾ), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ਜੇ (lnum == wp->w_cursor.lnum && !ਮਿਲਿਆ_one)
		ਰੋਕੋ;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ਜੇ (attr == HLF_ਗਿਣਤੀ)
		skip = (ਪੂਰਨ)(p - endp);
	    ਨਹੀਂਤਾਂ
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ)
		capcol = 0;
	}

	ਸਤਰ_breakcheck();
    }

    vim_free(buf);
    ਵਾਪਸ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    ਖਾਲੀ
spell_cat_ਸਤਰ(char_u *buf, char_u *ਸਤਰ, ਪੂਰਨ maxlen)
{
    char_u	*p;
    ਪੂਰਨ		n;

    p = skipwhite(ਸਤਰ);
    ਜਦੋਂ (vim_strchr((char_u *)"*#/\"\t", *p) != ਨਿਰਾ)
	p = skipwhite(p + 1);

    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (ਪੂਰਨ)(p - ਸਤਰ) + 1;
    ਜੇ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    ਪੂਰਨ	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    ਸਥਿਰ ਖਾਲੀ
spell_ਲੱਦੋ_lang(char_u *lang)
{
    char_u	fname_enc[85];
    ਪੂਰਨ		r;
    spelload_T	sl;
    ਪੂਰਨ		ਗੇੜਾ;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ਨਿਰਾ;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((ਅੱਖਰ *)fname_enc, ਆਕਾਰ(fname_enc) - 5,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
					"spell/%s_%s.spl",
#ਨਹੀਂਤਾਂ
					"spell/%s.%s.spl",
#ਜੇ_ਅੰਤ
							   lang, spell_enc());
	r = do_ਵਿੱਚ_runtimepath(fname_enc, 0, spell_ਲੱਦੋ_cb, &sl);

	ਜੇ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((ਅੱਖਰ *)fname_enc, ਆਕਾਰ(fname_enc) - 5,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
						  "spell/%s_ascii.spl",
#ਨਹੀਂਤਾਂ
						  "spell/%s.ascii.spl",
#ਜੇ_ਅੰਤ
									lang);
	    r = do_ਵਿੱਚ_runtimepath(fname_enc, 0, spell_ਲੱਦੋ_cb, &sl);

	    ਜੇ (r == FAIL && *sl.sl_lang != NUL && ਗੇੜਾ == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		ਜਾਰੀ;
	    ਰੋਕੋ;
	}
	ਰੋਕੋ;
    }

    ਜੇ (r == FAIL)
    {
	smsg(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#ਨਹੀਂਤਾਂ
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#ਜੇ_ਅੰਤ
						     lang, spell_enc(), lang);
    }
    ਨਹੀਂਤਾਂ ਜੇ (sl.sl_slang != ਨਿਰਾ)
    {
	// At least one file was loaded, now load ALL the additions.
	STRCPY(fname_enc + STRLEN(fname_enc) - 3, "add.spl");
	do_ਵਿੱਚ_runtimepath(fname_enc, DIP_ALL, spell_ਲੱਦੋ_cb, &sl);
    }

    --curbuf->b_locked;
}

/*
 * Return the encoding used for spell checking: Use 'encoding', except that we
 * use "latin1" for "latin9".  And limit to 60 characters (just in case).
 */
    char_u *
spell_enc(ਖਾਲੀ)
{

    ਜੇ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ਵਾਪਸ p_enc;
    ਵਾਪਸ (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    ਸਥਿਰ ਖਾਲੀ
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((ਅੱਖਰ *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    ਜੇ (lp != ਨਿਰਾ)
    {
	ਜੇ (lang != ਨਿਰਾ)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, ਆਕਾਰ(fromto_T), 10);
	ga_init2(&lp->sl_repsal, ਆਕਾਰ(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ਵਾਪਸ lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
free_sal_ਚੀਜ਼ਾਂ(garray_T *gap)
{
    salitem_T	*smp;

    ਜਦੋਂ (gap->ga_len > 0)
    {
	smp = &((salitem_T *)gap->ga_ਅੰਕੜੇ)[--gap->ga_len];
	vim_free(smp->sm_lead);
	// Don't free sm_oneof and sm_rules, they point into sm_lead.
	vim_free(smp->sm_ਮੰਜ਼ਿਲ);
	vim_free(smp->sm_lead_w);
	vim_free(smp->sm_oneof_w);
	vim_free(smp->sm_ਮੰਜ਼ਿਲ_w);
    }
    ga_clear(gap);
}

/*
 * Clear an slang_T so that the file can be reloaded.
 */
    ਖਾਲੀ
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਗੇੜਾ;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	gap = ਗੇੜਾ == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ਜਦੋਂ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_ਅੰਕੜੇ)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_ਮੰਜ਼ਿਲ);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ਜੇ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ਜੇ (gap->ga_ਅੰਕੜੇ != ਨਿਰਾ)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    ਲਈ (i = 0; i < gap->ga_len; ++i)
		vim_free(((ਪੂਰਨ **)gap->ga_ਅੰਕੜੇ)[i]);
	ga_clear(gap);
    }
    ਨਹੀਂਤਾਂ
	free_sal_ਚੀਜ਼ਾਂ(gap);

    ਲਈ (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ਨਿਰਾ;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_ਚੀਜ਼ਾਂ);

    ga_clear_ਲੜੀਆਂ(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_ਕੁੰਜੀ_OFF);
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
    ਖਾਲੀ
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = ਨਿਰਾ;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    ਸਥਿਰ ਖਾਲੀ
spell_ਲੱਦੋ_cb(char_u *fname, ਖਾਲੀ *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_ਲੱਦੋ_file(fname, slp->sl_lang, ਨਿਰਾ, FALSE);
    ਜੇ (slang == ਨਿਰਾ)
	ਵਾਪਸ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ਜੇ (slp->sl_nobreak && slang->sl_ਜੋੜੋ)
	slang->sl_nobreak = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    ਖਾਲੀ
ਗਿਣਤੀ_common_ਸ਼ਬਦ(
    slang_T	*lp,
    char_u	*ਸ਼ਬਦ,
    ਪੂਰਨ		len,	    // word length, -1 for up to NUL
    ਪੂਰਨ		ਗਿਣਤੀ_2)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ਜੇ (len == -1)
	p = ਸ਼ਬਦ;
    ਨਹੀਂਤਾਂ ਜੇ (len >= MAXWLEN)
	ਵਾਪਸ;
    ਨਹੀਂਤਾਂ
    {
	vim_strncpy(buf, ਸ਼ਬਦ, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_ਸ਼ਬਦ) + STRLEN(p) + 1);
	ਜੇ (wc == ਨਿਰਾ)
	    ਵਾਪਸ;
	STRCPY(wc->wc_ਸ਼ਬਦ, p);
	wc->wc_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
	hash_ਜੋੜੋ_ਵਸਤੂ(&lp->sl_wordcount, hi, wc->wc_ਸ਼ਬਦ, hash);
    }
    ਨਹੀਂਤਾਂ
    {
	wc = HI2WC(hi);
	ਜੇ ((wc->wc_ਗਿਣਤੀ += ਗਿਣਤੀ_2) < (ਬਿਨਾਨਿਸ਼ਾਨੀ)ਗਿਣਤੀ_2)	// check for overflow
	    wc->wc_ਗਿਣਤੀ = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    ਪੂਰਨ
byte_ਵਿੱਚ_str(char_u *str, ਪੂਰਨ n)
{
    char_u	*p;

    ਲਈ (p = str; *p != NUL; ++p)
	ਜੇ (*p == n)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SY_MAXLEN   30
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ syl_ਵਸਤੂ_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    ਪੂਰਨ		sy_len;
} syl_ਵਸਤੂ_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    ਪੂਰਨ
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    ਪੂਰਨ		l;
    syl_ਵਸਤੂ_T	*syl;

    ga_init2(&slang->sl_syl_ਚੀਜ਼ਾਂ, ਆਕਾਰ(syl_ਵਸਤੂ_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ਜਦੋਂ (p != ਨਿਰਾ)
    {
	*p++ = NUL;
	ਜੇ (*p == NUL)	    // trailing slash
	    ਰੋਕੋ;
	s = p;
	p = vim_strchr(p, '/');
	ਜੇ (p == ਨਿਰਾ)
	    l = (ਪੂਰਨ)STRLEN(s);
	ਨਹੀਂਤਾਂ
	    l = (ਪੂਰਨ)(p - s);
	ਜੇ (l >= SY_MAXLEN)
	    ਵਾਪਸ SP_FORMERROR;
	ਜੇ (ga_grow(&slang->sl_syl_ਚੀਜ਼ਾਂ, 1) == FAIL)
	    ਵਾਪਸ SP_OTHERERROR;
	syl = ((syl_ਵਸਤੂ_T *)slang->sl_syl_ਚੀਜ਼ਾਂ.ga_ਅੰਕੜੇ)
					       + slang->sl_syl_ਚੀਜ਼ਾਂ.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ਵਾਪਸ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    ਸਥਿਰ ਪੂਰਨ
ਗਿਣਤੀ_syllables(slang_T *slang, char_u *ਸ਼ਬਦ)
{
    ਪੂਰਨ		cnt = 0;
    ਪੂਰਨ		skip = FALSE;
    char_u	*p;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    syl_ਵਸਤੂ_T	*syl;
    ਪੂਰਨ		c;

    ਜੇ (slang->sl_syllable == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਲਈ (p = ਸ਼ਬਦ; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ਜੇ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    ਜਾਰੀ;
	}

	// Find longest match of syllable items.
	len = 0;
	ਲਈ (i = 0; i < slang->sl_syl_ਚੀਜ਼ਾਂ.ga_len; ++i)
	{
	    syl = ((syl_ਵਸਤੂ_T *)slang->sl_syl_ਚੀਜ਼ਾਂ.ga_ਅੰਕੜੇ) + i;
	    ਜੇ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ਜੇ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ਜੇ (vim_strchr(slang->sl_syllable, c) == ਨਿਰਾ)
		skip = FALSE;	    // No, search for next syllable
	    ਨਹੀਂਤਾਂ ਜੇ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ਵਾਪਸ cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    ਅੱਖਰ *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    ਪੂਰਨ		filename;
    ਪੂਰਨ		region_ਨਕਾਬ;
    slang_T	*slang;
    ਪੂਰਨ		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    ਪੂਰਨ		len;
    char_u	*p;
    ਪੂਰਨ		ਗੇੜਾ;
    char_u	*spf;
    char_u	*use_region = ਨਿਰਾ;
    ਪੂਰਨ		dont_use_region = FALSE;
    ਪੂਰਨ		nobreak = FALSE;
    ਪੂਰਨ		i, j;
    langp_T	*lp, *lp2;
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;
    ਅੱਖਰ	*ret_ਸੁਨੇਹਾ = ਨਿਰਾ;
    char_u	*spl_copy;
    bufref_T	bufref;

    ਸੈੱਟ_bufref(&bufref, wp->w_ਬਫਰ);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ਜੇ (recursive)
	ਵਾਪਸ ਨਿਰਾ;
    recursive = TRUE;

    ga_init2(&ga, ਆਕਾਰ(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ਜੇ (spl_copy == ਨਿਰਾ)
	ਜਾਓ theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    ਲਈ (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ਨਿਰਾ;

	ਜੇ (!valid_spelllang(lang))
	    ਜਾਰੀ;

	ਜੇ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    ਜਾਰੀ;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ਜੇ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ਜੇ (p != ਨਿਰਾ && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    ਨਹੀਂਤਾਂ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ਜੇ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    filename = FALSE;
	    ਜੇ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    ਨਹੀਂਤਾਂ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ਜੇ (STRICMP(lang, slang->sl_name) == 0)
		    ਰੋਕੋ;
	}

	ਜੇ (region != ਨਿਰਾ)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ਜੇ (use_region != ਨਿਰਾ && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ਜੇ (slang == ਨਿਰਾ)
	{
	    ਜੇ (filename)
		(ਖਾਲੀ)spell_ਲੱਦੋ_file(lang, lang, ਨਿਰਾ, FALSE);
	    ਨਹੀਂਤਾਂ
	    {
		spell_ਲੱਦੋ_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ਜੇ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_ਸੁਨੇਹਾ = N_(e_spellfilemising_autocommand_deleted_ਬਫਰ);
		    ਜਾਓ theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    ਜੇ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_ਨਕਾਬ = REGION_ALL;
		ਜੇ (!filename && region != ਨਿਰਾ)
		{
		    // find region in sl_regions
		    c = ਲੱਭੋ_region(slang->sl_regions, region);
		    ਜੇ (c == REGION_ALL)
		    {
			ਜੇ (slang->sl_ਜੋੜੋ)
			{
			    ਜੇ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_ਨਕਾਬ = 0;
			}
			ਨਹੀਂਤਾਂ
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    ਨਹੀਂਤਾਂ
			region_ਨਕਾਬ = 1 << c;
		}

		ਜੇ (region_ਨਕਾਬ != 0)
		{
		    ਜੇ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_ਸੁਨੇਹਾ = e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
			ਜਾਓ theend;
		    }
		    LANGP_ਇੰਦਰਾਜ(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ਇੰਦਰਾਜ(ga, ga.ga_len)->lp_region = region_ਨਕਾਬ;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ਜੇ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    ਲਈ (ਗੇੜਾ = 0; ਗੇੜਾ == 0 || *spf != NUL; ++ਗੇੜਾ)
    {
	ਜੇ (ਗੇੜਾ == 0)
	{
	    // Internal wordlist, if there is one.
	    ਜੇ (int_wordlist == ਨਿਰਾ)
		ਜਾਰੀ;
	    int_wordlist_spl(spf_name);
	}
	ਨਹੀਂਤਾਂ
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    ਲਈ (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ਇੰਦਰਾਜ(ga, c)->lp_slang->sl_fname;
		ਜੇ (p != ਨਿਰਾ && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    ਰੋਕੋ;
	    }
	    ਜੇ (c < ga.ga_len)
		ਜਾਰੀ;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ਜੇ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		ਰੋਕੋ;
	ਜੇ (slang == ਨਿਰਾ)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ਜੇ (ਗੇੜਾ == 0)
		STRCPY(lang, "internal wordlist");
	    ਨਹੀਂਤਾਂ
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ਜੇ (p != ਨਿਰਾ)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_ਲੱਦੋ_file(spf_name, lang, ਨਿਰਾ, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ਜੇ (slang != ਨਿਰਾ && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ਜੇ (slang != ਨਿਰਾ && ga_grow(&ga, 1) == OK)
	{
	    region_ਨਕਾਬ = REGION_ALL;
	    ਜੇ (use_region != ਨਿਰਾ && !dont_use_region)
	    {
		// find region in sl_regions
		c = ਲੱਭੋ_region(slang->sl_regions, use_region);
		ਜੇ (c != REGION_ALL)
		    region_ਨਕਾਬ = 1 << c;
		ਨਹੀਂਤਾਂ ਜੇ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_ਨਕਾਬ = 0;
	    }

	    ਜੇ (region_ਨਕਾਬ != 0)
	    {
		LANGP_ਇੰਦਰਾਜ(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ਇੰਦਰਾਜ(ga, ga.ga_len)->lp_sallang = ਨਿਰਾ;
		LANGP_ਇੰਦਰਾਜ(ga, ga.ga_len)->lp_replang = ਨਿਰਾ;
		LANGP_ਇੰਦਰਾਜ(ga, ga.ga_len)->lp_region = region_ਨਕਾਬ;
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
    ਲਈ (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ਇੰਦਰਾਜ(ga, i);

	// sound folding
	ਜੇ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	ਨਹੀਂਤਾਂ
	    // find first similar language that does sound folding
	    ਲਈ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ਇੰਦਰਾਜ(ga, j);
		ਜੇ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    ਰੋਕੋ;
		}
	    }

	// REP items
	ਜੇ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	ਨਹੀਂਤਾਂ
	    // find first similar language that has REP items
	    ਲਈ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ਇੰਦਰਾਜ(ga, j);
		ਜੇ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    ਰੋਕੋ;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ਵਾਪਸ ret_ਸੁਨੇਹਾ;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    ਸਥਿਰ ਖਾਲੀ
clear_midword(win_T *wp)
{
    CLEAR_ਖੇਤਰ(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    ਸਥਿਰ ਖਾਲੀ
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    ਜੇ (lp->sl_midword == ਨਿਰਾ)	    // there aren't any
	ਵਾਪਸ;

    ਲਈ (p = lp->sl_midword; *p != NUL; )
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ਜੇ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_s->b_spell_ismw_mb == ਨਿਰਾ)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    ਨਹੀਂਤਾਂ
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (ਪੂਰਨ)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ਜੇ (bp != ਨਿਰਾ)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	ਨਹੀਂਤਾਂ
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_region(char_u *rp, char_u *region)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; ; i += 2)
    {
	ਜੇ (rp[i] == NUL)
	    ਵਾਪਸ REGION_ALL;
	ਜੇ (rp[i] == region[0] && rp[i + 1] == region[1])
	    ਰੋਕੋ;
    }
    ਵਾਪਸ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    ਪੂਰਨ
captype(
    char_u	*ਸ਼ਬਦ,
    char_u	*ਸਿਰਾ)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		firstcap;
    ਪੂਰਨ		allcap;
    ਪੂਰਨ		past_ਦੂਜਾ = FALSE;	// past second word char

    // find first letter
    ਲਈ (p = ਸ਼ਬਦ; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ਜੇ (ਸਿਰਾ == ਨਿਰਾ ? *p == NUL : p >= ਸਿਰਾ)
	    ਵਾਪਸ 0;	    // only non-word characters, illegal word
    ਜੇ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    ਨਹੀਂਤਾਂ
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    ਲਈ ( ; ਸਿਰਾ == ਨਿਰਾ ? *p != NUL : p < ਸਿਰਾ; MB_PTR_ADV(p))
	ਜੇ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ਜੇ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ਜੇ (past_ਦੂਜਾ && allcap)
		    ਵਾਪਸ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!allcap)
		// UlU -> KEEPCAP
		ਵਾਪਸ WF_KEEPCAP;
	    past_ਦੂਜਾ = TRUE;
	}

    ਜੇ (allcap)
	ਵਾਪਸ WF_ALLCAP;
    ਜੇ (firstcap)
	ਵਾਪਸ WF_ONECAP;
    ਵਾਪਸ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    ਖਾਲੀ
spell_delete_wordlist(ਖਾਲੀ)
{
    char_u	fname[MAXPATHL];

    ਜੇ (int_wordlist == ਨਿਰਾ)
	ਵਾਪਸ;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    ਖਾਲੀ
spell_free_all(ਖਾਲੀ)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    ਜਦੋਂ (ਪਹਿਲਾ_lang != ਨਿਰਾ)
    {
	slang = ਪਹਿਲਾ_lang;
	ਪਹਿਲਾ_lang = slang->sl_ਅਗਲਾ;
	slang_free(slang);
    }

    spell_delete_wordlist();

    VIM_CLEAR(repl_ਮੰਜ਼ਿਲ);
    VIM_CLEAR(repl_from);
}

/*
 * Clear all spelling tables and reload them.
 * Used after 'encoding' is set and when ":mkspell" was used.
 */
    ਖਾਲੀ
spell_reload(ਖਾਲੀ)
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
	ਜੇ (*wp->w_s->b_p_spl != NUL)
	{
		ਜੇ (wp->w_p_spell)
		{
		    (ਖਾਲੀ)parse_spelllang(wp);
		    ਰੋਕੋ;
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
open_spellbuf(ਖਾਲੀ)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    buf->b_p_ਕੁੰਜੀ = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ਵਾਪਸ buf;
}

/*
 * Close the buffer used for spell info.
 */
    ਖਾਲੀ
close_spellbuf(buf_T *buf)
{
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    ਖਾਲੀ
clear_spell_chartab(spelltab_T *sp)
{
    ਪੂਰਨ		i;

    // Init everything to FALSE (zero).
    CLEAR_ਖੇਤਰ(sp->st_isw);
    CLEAR_ਖੇਤਰ(sp->st_isu);
    ਲਈ (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_ਉੱਤਲਾ[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    ਲਈ (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    ਲਈ (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    ਲਈ (i = 'a'; i <= 'z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_ਉੱਤਲਾ[i] = i - 0x20;
    }
}

/*
 * Init the chartab used for spelling.  Only depends on 'encoding'.
 * Called once while starting up and when 'encoding' changes.
 * The default is to use isalpha(), but the spell file should define the word
 * characters to make it possible that 'encoding' differs from the current
 * locale.  For utf-8 we don't use isalpha() but our own functions.
 */
    ਖਾਲੀ
init_spell_chartab(ਖਾਲੀ)
{
    ਪੂਰਨ	    i;

    did_ਸੈੱਟ_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ਜੇ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	ਲਈ (i = 128; i <= 255; ++i)
	    ਜੇ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
    {
	ਲਈ (i = 128; i < 256; ++i)
	{
	    ਪੂਰਨ f = utf_fold(i);
	    ਪੂਰਨ u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_ਉੱਤਲਾ[i] = (u < 256) ? u : i;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Rough guess: use locale-dependent library functions.
	ਲਈ (i = 128; i < 256; ++i)
	{
	    ਜੇ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (MB_ISLOWER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_ਉੱਤਲਾ[i] = MB_TOUPPER(i);
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
    ਪੂਰਨ
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    ਪੂਰਨ		l;
    ਪੂਰਨ		c;

    ਜੇ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ਜੇ (l == 1)
	{
	    // be quick for ASCII
	    ਜੇ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	ਨਹੀਂਤਾਂ
	{
	    c = mb_ptr2char(p);
	    ਜੇ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ਨਿਰਾ
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ਨਿਰਾ))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ਜੇ (c > 255)
	    ਵਾਪਸ spell_mb_isword_class(mb_get_class(s), wp);
	ਵਾਪਸ spelltab.st_isw[c];
    }

    ਵਾਪਸ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    ਪੂਰਨ
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    ਪੂਰਨ		c;

    ਜੇ (has_mbyte)
    {
	c = mb_ptr2char(p);
	ਜੇ (c > 255)
	    ਵਾਪਸ spell_mb_isword_class(mb_get_class(p), wp);
	ਵਾਪਸ spelltab.st_isw[c];
    }
    ਵਾਪਸ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    ਸਥਿਰ ਪੂਰਨ
spell_mb_isword_class(ਪੂਰਨ cl, win_T *wp)
{
    ਜੇ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ਵਾਪਸ cl == 2 || cl == 0x2800;
    ਵਾਪਸ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    ਸਥਿਰ ਪੂਰਨ
spell_iswordp_w(ਪੂਰਨ *p, win_T *wp)
{
    ਪੂਰਨ		*s;

    ਜੇ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ਨਿਰਾ
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ਨਿਰਾ))
	s = p + 1;
    ਨਹੀਂਤਾਂ
	s = p;

    ਜੇ (*s > 255)
    {
	ਜੇ (enc_utf8)
	    ਵਾਪਸ spell_mb_isword_class(utf_class(*s), wp);
	ਜੇ (enc_dbcs)
	    ਵਾਪਸ spell_mb_isword_class(
				dbcs_class((ਬਿਨਾਨਿਸ਼ਾਨੀ)*s >> 8, *s & 0xff), wp);
	ਵਾਪਸ 0;
    }
    ਵਾਪਸ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    ਪੂਰਨ
spell_casefold(
    win_T	*wp,
    char_u	*str,
    ਪੂਰਨ		len,
    char_u	*buf,
    ਪੂਰਨ		buflen)
{
    ਪੂਰਨ		i;

    ਜੇ (len >= buflen)
    {
	buf[0] = NUL;
	ਵਾਪਸ FAIL;		// result will not fit
    }

    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ	outi = 0;
	char_u	*p;
	ਪੂਰਨ	c;

	// Fold one character at a time.
	ਲਈ (p = str; p < str + len; )
	{
	    ਜੇ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ਵਾਪਸ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ਜੇ (c == 0x03a3 || c == 0x03c2)
	    {
		ਜੇ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		ਨਹੀਂਤਾਂ
		    c = 0x03c3;
	    }
	    ਨਹੀਂਤਾਂ
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    ਨਹੀਂਤਾਂ
    {
	// Be quick for non-multibyte encodings.
	ਲਈ (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ਵਾਪਸ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    ਪੂਰਨ
check_need_cap(win_T *wp, linenr_T lnum, colnr_T ਖਾਨਾ)
{
    ਜੇ (wp->w_s->b_cap_prog == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਪੂਰਨ		need_cap = FALSE;
    char_u	*ਸਤਰ = ਖਾਨਾ ? ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE) : ਨਿਰਾ;
    char_u	*ਸਤਰ_copy = ਨਿਰਾ;
    colnr_T	endcol = 0;

    ਜੇ (ਖਾਨਾ == 0 || getwhitecols(ਸਤਰ) >= ਖਾਨਾ)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ਜੇ (lnum == 1)
	    need_cap = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum - 1, FALSE);
	    ਜੇ (*skipwhite(ਸਤਰ) == NUL)
		need_cap = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		// Append a space in place of the line break.
		ਸਤਰ_copy = concat_str(ਸਤਰ, (char_u *)" ");
		ਜੇ (ਸਤਰ_copy == ਨਿਰਾ)
		    ਵਾਪਸ FALSE;

		ਸਤਰ = ਸਤਰ_copy;
		endcol = (colnr_T)STRLEN(ਸਤਰ);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	endcol = ਖਾਨਾ;

    ਜੇ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = ਸਤਰ + endcol;
	ਲਈ (;;)
	{
	    MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
	    ਜੇ (p == ਸਤਰ || spell_iswordp_nmw(p, wp))
		ਰੋਕੋ;
	    ਜੇ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == ਸਤਰ + endcol)
	    {
		need_cap = TRUE;
		ਰੋਕੋ;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(ਸਤਰ_copy);

    ਵਾਪਸ need_cap;
}


/*
 * ":spellrepall"
 */
    ਖਾਲੀ
ex_spellrepall(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*ਸਤਰ;
    char_u	*p;
    ਪੂਰਨ		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ਜੇ (repl_from == ਨਿਰਾ || repl_ਮੰਜ਼ਿਲ == ਨਿਰਾ)
    {
	emsg(_(e_no_ਪਿਛਲਾ_spell_replacement));
	ਵਾਪਸ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_ਮੰਜ਼ਿਲ_len = STRLEN(repl_ਮੰਜ਼ਿਲ);
    ਲੰਮਾ	addlen = (ਲੰਮਾ)repl_ਮੰਜ਼ਿਲ_len - (ਲੰਮਾ)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ਜੇ (frompat == ਨਿਰਾ)
	ਵਾਪਸ;
    frompatlen = vim_snprintf((ਅੱਖਰ *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ਜਦੋਂ (!got_int)
    {
	ਜੇ (do_ਖੋਜ(ਨਿਰਾ, '/', '/', frompat, frompatlen, 1L, ਖੋਜ_KEEP, ਨਿਰਾ) == 0
						   || u_save_cursor() == FAIL)
	    ਰੋਕੋ;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	ਸਤਰ = ml_get_curline();
	ਜੇ (addlen <= 0 || STRNCMP(ਸਤਰ + curwin->w_cursor.ਖਾਨਾ,
						   repl_ਮੰਜ਼ਿਲ, repl_ਮੰਜ਼ਿਲ_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ਜੇ (p == ਨਿਰਾ)
		ਰੋਕੋ;
	    mch_memmove(p, ਸਤਰ, curwin->w_cursor.ਖਾਨਾ);
	    STRCPY(p + curwin->w_cursor.ਖਾਨਾ, repl_ਮੰਜ਼ਿਲ);
	    STRCAT(p, ਸਤਰ + curwin->w_cursor.ਖਾਨਾ + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    ਬਦਲਿਆ_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, curwin->w_cursor.ਖਾਨਾ);
#ਜੇ defined(FEAT_PROP_POPUP)
	    ਜੇ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.ਖਾਨਾ, (ਪੂਰਨ)addlen, APC_SUBSTITUTE);
#ਜੇ_ਅੰਤ

	    ਜੇ (curwin->w_cursor.lnum != prev_lnum)
	    {
		++sub_nlines;
		prev_lnum = curwin->w_cursor.lnum;
	    }
	    ++sub_nsubs;
	}
	curwin->w_cursor.ਖਾਨਾ += (colnr_T)repl_ਮੰਜ਼ਿਲ_len;
    }

    p_ws = save_ws;
    curwin->w_cursor = pos;
    vim_free(frompat);

    ਜੇ (sub_nsubs == 0)
	semsg(_(e_not_ਮਿਲਿਆ_str), repl_from);
    ਨਹੀਂਤਾਂ
	do_sub_ਸੁਨੇਹਾ(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    ਖਾਲੀ
onecap_copy(
    char_u	*ਸ਼ਬਦ,
    char_u	*wcopy,
    ਪੂਰਨ		ਉੱਤਲਾ_2)	    // TRUE: first letter made upper case
{
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		l;

    p = ਸ਼ਬਦ;
    ਜੇ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    ਨਹੀਂਤਾਂ
	c = *p++;
    ਜੇ (ਉੱਤਲਾ_2)
	c = SPELL_TOUPPER(c);
    ਨਹੀਂਤਾਂ
	c = SPELL_TOFOLD(c);
    ਜੇ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
allcap_copy(char_u *ਸ਼ਬਦ, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    ਪੂਰਨ		c;

    d = wcopy;
    ਲਈ (s = ਸ਼ਬਦ; *s != NUL; )
    {
	ਜੇ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	ਨਹੀਂਤਾਂ
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ਜੇ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ਜੇ (d - wcopy >= MAXWLEN - 1)
		ਰੋਕੋ;
	    *d++ = c;
	}
	ਨਹੀਂਤਾਂ
	    c = SPELL_TOUPPER(c);

	ਜੇ (has_mbyte)
	{
	    ਜੇ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		ਰੋਕੋ;
	    d += mb_char2bytes(c, d);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (d - wcopy >= MAXWLEN - 1)
		ਰੋਕੋ;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    ਪੂਰਨ
nofold_len(char_u *fword, ਪੂਰਨ flen, char_u *ਸ਼ਬਦ)
{
    char_u	*p;
    ਪੂਰਨ		i = 0;

    ਲਈ (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    ਲਈ (p = ਸ਼ਬਦ; i > 0; MB_PTR_ADV(p))
	--i;
    ਵਾਪਸ (ਪੂਰਨ)(p - ਸ਼ਬਦ);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    ਖਾਲੀ
make_case_ਸ਼ਬਦ(char_u *fword, char_u *cword, ਪੂਰਨ flags)
{
    ਜੇ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    ਨਹੀਂਤਾਂ ਜੇ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    ਨਹੀਂਤਾਂ
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *ਸ਼ਬਦ)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    ਪੂਰਨ		lpi;

    ਜੇ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	    ਜੇ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, ਸ਼ਬਦ, FALSE, sound);
		ਵਾਪਸ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ਵਾਪਸ vim_strsave(ਸ਼ਬਦ);
}
#ਜੇ_ਅੰਤ

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
    ਖਾਲੀ
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    ਪੂਰਨ		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*ਸ਼ਬਦ;

    ਜੇ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    ਨਹੀਂਤਾਂ
    {
	// SAL items used.  Requires the word to be case-folded.
	ਜੇ (folded)
	    ਸ਼ਬਦ = inword;
	ਨਹੀਂਤਾਂ
	{
	    (ਖਾਲੀ)spell_casefold(curwin,
				  inword, (ਪੂਰਨ)STRLEN(inword), fword, MAXWLEN);
	    ਸ਼ਬਦ = fword;
	}

	ਜੇ (has_mbyte)
	    spell_soundfold_wsal(slang, ਸ਼ਬਦ, res);
	ਨਹੀਂਤਾਂ
	    spell_soundfold_sal(slang, ਸ਼ਬਦ, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    ਸਥਿਰ ਖਾਲੀ
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    ਪੂਰਨ		ri = 0;
    ਪੂਰਨ		c;

    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ	prevc = 0;
	ਪੂਰਨ	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	ਲਈ (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ਜੇ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    ਨਹੀਂਤਾਂ ਜੇ (c < 256)
		c = slang->sl_sal_ਪਹਿਲਾ[c];
	    ਨਹੀਂਤਾਂ
	    {
		ip = ((ਪੂਰਨ **)slang->sl_sal.ga_ਅੰਕੜੇ)[c & 0xff];
		ਜੇ (ip == ਨਿਰਾ)		// empty list, can't match
		    c = NUL;
		ਨਹੀਂਤਾਂ
		    ਲਈ (;;)		// find "c" in the list
		    {
			ਜੇ (*ip == 0)	// not found
			{
			    c = NUL;
			    ਰੋਕੋ;
			}
			ਜੇ (*ip == c)	// match!
			{
			    c = ip[1];
			    ਰੋਕੋ;
			}
			ip += 2;
		    }
	    }

	    ਜੇ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ਜੇ (ri + MB_MAXBYTES > MAXWLEN)
		    ਰੋਕੋ;
		prevc = c;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// The sl_sal_first[] table contains the translation.
	ਲਈ (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ਜੇ (VIM_ISWHITE(c))
		c = ' ';
	    ਨਹੀਂਤਾਂ
		c = slang->sl_sal_ਪਹਿਲਾ[c];
	    ਜੇ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    ਸਥਿਰ ਖਾਲੀ
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	ਸ਼ਬਦ[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    ਪੂਰਨ		i, j, z;
    ਪੂਰਨ		reslen;
    ਪੂਰਨ		n, k = 0;
    ਪੂਰਨ		z0;
    ਪੂਰਨ		k0;
    ਪੂਰਨ		n0;
    ਪੂਰਨ		c;
    ਪੂਰਨ		pri;
    ਪੂਰਨ		p0 = -333;
    ਪੂਰਨ		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ਜੇ (slang->sl_rem_accents)
    {
	t = ਸ਼ਬਦ;
	ਜਦੋਂ (*s != NUL)
	{
	    ਜੇ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    ਨਹੀਂਤਾਂ
	vim_strncpy(ਸ਼ਬਦ, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_ਅੰਕੜੇ;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ਜਦੋਂ ((c = ਸ਼ਬਦ[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_ਪਹਿਲਾ[c];
	z0 = 0;

	ਜੇ (n >= 0)
	{
	    // check all rules for the same letter
	    ਲਈ (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ਜੇ (k > 1)
		{
		    ਜੇ (ਸ਼ਬਦ[i + 1] != s[1])
			ਜਾਰੀ;
		    ਜੇ (k > 2)
		    {
			ਲਈ (j = 2; j < k; ++j)
			    ਜੇ (ਸ਼ਬਦ[i + j] != s[j])
				ਰੋਕੋ;
			ਜੇ (j < k)
			    ਜਾਰੀ;
		    }
		}

		ਜੇ ((pf = smp[n].sm_oneof) != ਨਿਰਾ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ਜਦੋਂ (*pf != NUL && *pf != ਸ਼ਬਦ[i + k])
			++pf;
		    ਜੇ (*pf == NUL)
			ਜਾਰੀ;
		    ++k;
		}
		s = smp[n].sm_ਨਿਯਮਾਂ;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ਜਦੋਂ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ਜੇ (*s == '<')
		    s++;
		ਜੇ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ਜੇ (*s == '^' && *(s + 1) == '^')
		    s++;

		ਜੇ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(ਸ਼ਬਦ[i - 1] == ' '
				      || spell_iswordp(ਸ਼ਬਦ + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp(ਸ਼ਬਦ + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp(ਸ਼ਬਦ + i - 1, curwin)
			    && (!spell_iswordp(ਸ਼ਬਦ + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = ਸ਼ਬਦ[i + k - 1];
		    n0 = slang->sl_sal_ਪਹਿਲਾ[c0];

		    ਜੇ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && ਸ਼ਬਦ[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			ਲਈ ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ਜੇ (k0 > 1)
			    {
				ਜੇ (ਸ਼ਬਦ[i + k] != s[1])
				    ਜਾਰੀ;
				ਜੇ (k0 > 2)
				{
				    pf = ਸ਼ਬਦ + i + k + 1;
				    ਲਈ (j = 2; j < k0; ++j)
					ਜੇ (*pf++ != s[j])
					    ਰੋਕੋ;
				    ਜੇ (j < k0)
					ਜਾਰੀ;
				}
			    }
			    k0 += k - 1;

			    ਜੇ ((pf = smp[n0].sm_oneof) != ਨਿਰਾ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ਜਦੋਂ (*pf != NUL && *pf != ਸ਼ਬਦ[i + k0])
				    ++pf;
				ਜੇ (*pf == NUL)
				    ਜਾਰੀ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_ਨਿਯਮਾਂ;
			    ਜਦੋਂ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ਜੇ (*s == '<')
				s++;
			    ਜੇ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ਜੇ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(ਸ਼ਬਦ + i + k0,
								     curwin)))
			    {
				ਜੇ (k0 == k)
				    // this is just a piece of the string
				    ਜਾਰੀ;

				ਜੇ (p0 < pri)
				    // priority too low
				    ਜਾਰੀ;
				// rule fits; stop search
				ਰੋਕੋ;
			    }
			}

			ਜੇ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    ਜਾਰੀ;
		    }

		    // replace string
		    s = smp[n].sm_ਮੰਜ਼ਿਲ;
		    ਜੇ (s == ਨਿਰਾ)
			s = (char_u *)"";
		    pf = smp[n].sm_ਨਿਯਮਾਂ;
		    p0 = (vim_strchr(pf, '<') != ਨਿਰਾ) ? 1 : 0;
		    ਜੇ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ਜੇ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ਜਦੋਂ (*s != NUL && ਸ਼ਬਦ[i + k0] != NUL)
			{
			    ਸ਼ਬਦ[i + k0] = *s;
			    k0++;
			    s++;
			}
			ਜੇ (k > k0)
			    STRMOVE(ਸ਼ਬਦ + i + k0, ਸ਼ਬਦ + i + k);

			// new "actual letter"
			c = ਸ਼ਬਦ[i];
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ਜਦੋਂ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ਜੇ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)pf, "^^") != ਨਿਰਾ)
			{
			    ਜੇ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(ਸ਼ਬਦ, ਸ਼ਬਦ + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ਜੇ (z0 == 0)
	{
	    ਜੇ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    ਸਥਿਰ ਖਾਲੀ
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_ਅੰਕੜੇ;
    ਪੂਰਨ		ਸ਼ਬਦ[MAXWLEN];
    ਪੂਰਨ		wres[MAXWLEN];
    ਪੂਰਨ		l;
    char_u	*s;
    ਪੂਰਨ		*ws;
    char_u	*t;
    ਪੂਰਨ		*pf;
    ਪੂਰਨ		i, j, z;
    ਪੂਰਨ		reslen;
    ਪੂਰਨ		n, k = 0;
    ਪੂਰਨ		z0;
    ਪੂਰਨ		k0;
    ਪੂਰਨ		n0;
    ਪੂਰਨ		c;
    ਪੂਰਨ		pri;
    ਪੂਰਨ		p0 = -333;
    ਪੂਰਨ		c0;
    ਪੂਰਨ		did_white = FALSE;
    ਪੂਰਨ		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    ਲਈ (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ਜੇ (slang->sl_rem_accents)
	{
	    ਜੇ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ਜੇ (did_white)
		    ਜਾਰੀ;
		c = ' ';
		did_white = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		did_white = FALSE;
		ਜੇ (!spell_iswordp_nmw(t, curwin))
		    ਜਾਰੀ;
	    }
	}
	ਸ਼ਬਦ[wordlen++] = c;
    }
    ਸ਼ਬਦ[wordlen] = NUL;

    /*
     * This algorithm comes from Aspell phonet.cpp.
     * Converted from C++ to C.  Added support for multi-byte chars.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ਜਦੋਂ ((c = ਸ਼ਬਦ[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_ਪਹਿਲਾ[c & 0xff];
	z0 = 0;

	ਜੇ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    ਲਈ (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ਜੇ (c != ws[0])
		    ਜਾਰੀ;
		k = smp[n].sm_leadlen;
		ਜੇ (k > 1)
		{
		    ਜੇ (ਸ਼ਬਦ[i + 1] != ws[1])
			ਜਾਰੀ;
		    ਜੇ (k > 2)
		    {
			ਲਈ (j = 2; j < k; ++j)
			    ਜੇ (ਸ਼ਬਦ[i + j] != ws[j])
				ਰੋਕੋ;
			ਜੇ (j < k)
			    ਜਾਰੀ;
		    }
		}

		ਜੇ ((pf = smp[n].sm_oneof_w) != ਨਿਰਾ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ਜਦੋਂ (*pf != NUL && *pf != ਸ਼ਬਦ[i + k])
			++pf;
		    ਜੇ (*pf == NUL)
			ਜਾਰੀ;
		    ++k;
		}
		s = smp[n].sm_ਨਿਯਮਾਂ;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ਜਦੋਂ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ਜੇ (*s == '<')
		    s++;
		ਜੇ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ਜੇ (*s == '^' && *(s + 1) == '^')
		    s++;

		ਜੇ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(ਸ਼ਬਦ[i - 1] == ' '
				    || spell_iswordp_w(ਸ਼ਬਦ + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp_w(ਸ਼ਬਦ + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp_w(ਸ਼ਬਦ + i - 1, curwin)
			    && (!spell_iswordp_w(ਸ਼ਬਦ + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = ਸ਼ਬਦ[i + k - 1];
		    n0 = slang->sl_sal_ਪਹਿਲਾ[c0 & 0xff];

		    ਜੇ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && ਸ਼ਬਦ[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			ਲਈ ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ਜੇ (c0 != ws[0])
				ਜਾਰੀ;
			    k0 = smp[n0].sm_leadlen;
			    ਜੇ (k0 > 1)
			    {
				ਜੇ (ਸ਼ਬਦ[i + k] != ws[1])
				    ਜਾਰੀ;
				ਜੇ (k0 > 2)
				{
				    pf = ਸ਼ਬਦ + i + k + 1;
				    ਲਈ (j = 2; j < k0; ++j)
					ਜੇ (*pf++ != ws[j])
					    ਰੋਕੋ;
				    ਜੇ (j < k0)
					ਜਾਰੀ;
				}
			    }
			    k0 += k - 1;

			    ਜੇ ((pf = smp[n0].sm_oneof_w) != ਨਿਰਾ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ਜਦੋਂ (*pf != NUL && *pf != ਸ਼ਬਦ[i + k0])
				    ++pf;
				ਜੇ (*pf == NUL)
				    ਜਾਰੀ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_ਨਿਯਮਾਂ;
			    ਜਦੋਂ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ਜੇ (*s == '<')
				s++;
			    ਜੇ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ਜੇ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(ਸ਼ਬਦ + i + k0,
								     curwin)))
			    {
				ਜੇ (k0 == k)
				    // this is just a piece of the string
				    ਜਾਰੀ;

				ਜੇ (p0 < pri)
				    // priority too low
				    ਜਾਰੀ;
				// rule fits; stop search
				ਰੋਕੋ;
			    }
			}

			ਜੇ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    ਜਾਰੀ;
		    }

		    // replace string
		    ws = smp[n].sm_ਮੰਜ਼ਿਲ_w;
		    s = smp[n].sm_ਨਿਯਮਾਂ;
		    p0 = (vim_strchr(s, '<') != ਨਿਰਾ) ? 1 : 0;
		    ਜੇ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ਜੇ (reslen > 0 && ws != ਨਿਰਾ && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ਜੇ (ws != ਨਿਰਾ)
			    ਜਦੋਂ (*ws != NUL && ਸ਼ਬਦ[i + k0] != NUL)
			    {
				ਸ਼ਬਦ[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ਜੇ (k > k0)
			    mch_memmove(ਸ਼ਬਦ + i + k0, ਸ਼ਬਦ + i + k,
				    ਆਕਾਰ(ਪੂਰਨ) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = ਸ਼ਬਦ[i];
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ਜੇ (ws != ਨਿਰਾ)
			    ਜਦੋਂ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ਜੇ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ਜੇ (ws == ਨਿਰਾ)
			    c = NUL;
			ਨਹੀਂਤਾਂ
			    c = *ws;
			ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)s, "^^") != ਨਿਰਾ)
			{
			    ਜੇ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(ਸ਼ਬਦ, ਸ਼ਬਦ + i + 1,
				       ਆਕਾਰ(ਪੂਰਨ) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ਜੇ (z0 == 0)
	{
	    ਜੇ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    ਲਈ (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ਜੇ (l + MB_MAXBYTES > MAXWLEN)
	    ਰੋਕੋ;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    ਖਾਲੀ
ex_spellinfo(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		lpi;
    langp_T	*lp;
    char_u	*p;

    ਜੇ (no_spell_checking(curwin))
	ਵਾਪਸ;

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	ਸੁਨੇਹਾ_puts("file: ");
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)lp->lp_slang->sl_fname);
	ਸੁਨੇਹਾ_putchar('\n');
	p = lp->lp_slang->sl_info;
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
	    ਸੁਨੇਹਾ_putchar('\n');
	}
    }
    ਸੁਨੇਹਾ_ਸਿਰਾ();
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUMPFLAG_ਗਿਣਤੀ	    2	// include word count
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUMPFLAG_ICASE	    4	// ignore case when finding matches
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUMPFLAG_ONECAP	    8	// pattern starts with capital
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    ਖਾਲੀ
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    ਲੰਮਾ    dummy;

    ਜੇ (no_spell_checking(curwin))
	ਵਾਪਸ;
    (ਖਾਲੀ)get_option_ਮੁੱਲ((char_u*)"spl", &dummy, &spl, ਨਿਰਾ, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ਜੇ (!BUFEMPTY())
	ਵਾਪਸ;

    spell_dump_compl(ਨਿਰਾ, 0, ਨਿਰਾ, eap->forceit ? DUMPFLAG_ਗਿਣਤੀ : 0);

    // Delete the empty line that we started with.
    ਜੇ (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ > 1)
	ml_delete(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    ਖਾਲੀ
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    ਪੂਰਨ		ic,	    // ignore case
    ਪੂਰਨ		*dir,	    // direction for adding matches
    ਪੂਰਨ		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    ਪੂਰਨ		curi[MAXWLEN];
    char_u	ਸ਼ਬਦ[MAXWLEN];
    ਪੂਰਨ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    ਪੂਰਨ		n;
    ਪੂਰਨ		flags;
    char_u	*region_ਨਾਂ = ਨਿਰਾ;	    // region names being used
    ਪੂਰਨ		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    ਪੂਰਨ		lpi;
    ਪੂਰਨ		dumpflags = dumpflags_arg;
    ਪੂਰਨ		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ਜੇ (pat != ਨਿਰਾ)
    {
	ਜੇ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	ਨਹੀਂਤਾਂ
	{
	    n = captype(pat, ਨਿਰਾ);
	    ਜੇ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    ਨਹੀਂਤਾਂ ਜੇ (n == WF_ALLCAP && (ਪੂਰਨ)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ਜੇ (p[0] != 0)
	{
	    ਜੇ (region_ਨਾਂ == ਨਿਰਾ)	    // first language with regions
		region_ਨਾਂ = p;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(region_ਨਾਂ, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (do_region && region_ਨਾਂ != ਨਿਰਾ && pat == ਨਿਰਾ)
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "/regions=%s", region_ਨਾਂ);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    ਨਹੀਂਤਾਂ
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ਜੇ (slang->sl_fbyts == ਨਿਰਾ)	    // reloading failed
	    ਜਾਰੀ;

	ਜੇ (pat == ਨਿਰਾ)
	{
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ਜੇ (pat != ਨਿਰਾ && slang->sl_pbyts == ਨਿਰਾ)
	    patlen = (ਪੂਰਨ)STRLEN(pat);
	ਨਹੀਂਤਾਂ
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	{
	    ਜੇ (ਗੇੜਾ == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ਜੇ (byts == ਨਿਰਾ)
		ਜਾਰੀ;		// array is empty

	    ਡੂੰਘਾਈ = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ਜਦੋਂ (ਡੂੰਘਾਈ >= 0 && !got_int
				  && (pat == ਨਿਰਾ || !ins_compl_interrupted()))
	    {
		ਜੇ (curi[ਡੂੰਘਾਈ] > byts[arridx[ਡੂੰਘਾਈ]])
		{
		    // Done all bytes at this node, go up one level.
		    --ਡੂੰਘਾਈ;
		    ਸਤਰ_breakcheck();
		    ins_compl_check_ਕੁੰਜੀਆਂ(50, FALSE);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Do one more byte at this node.
		    n = arridx[ਡੂੰਘਾਈ] + curi[ਡੂੰਘਾਈ];
		    ++curi[ਡੂੰਘਾਈ];
		    c = byts[n];
		    ਜੇ (c == 0 || ਡੂੰਘਾਈ >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (ਪੂਰਨ)idxs[n];
			ਜੇ ((ਗੇੜਾ == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    ਸ਼ਬਦ[ਡੂੰਘਾਈ] = NUL;
			    ਜੇ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24;
			    ਜੇ (c == 0 || curi[ਡੂੰਘਾਈ] == 2)
			    {
				dump_ਸ਼ਬਦ(slang, ਸ਼ਬਦ, pat, dir,
						      dumpflags, flags, lnum);
				ਜੇ (pat == ਨਿਰਾ)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ਜੇ (c != 0)
				lnum = dump_prefixes(slang, ਸ਼ਬਦ, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Normal char, go one level deeper.
			ਸ਼ਬਦ[ਡੂੰਘਾਈ++] = c;
			arridx[ਡੂੰਘਾਈ] = idxs[n];
			curi[ਡੂੰਘਾਈ] = 1;

			// Check if this character matches with the pattern.
			// If not skip the whole tree below it.
			// Always ignore case here, dump_word() will check
			// proper case later.  This isn't exactly right when
			// length changes for multi-byte characters with
			// ignore case...
			ਜੇ (ਡੂੰਘਾਈ <= patlen
					&& MB_STRNICMP(ਸ਼ਬਦ, pat, ਡੂੰਘਾਈ) != 0)
			    --ਡੂੰਘਾਈ;
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
    ਸਥਿਰ ਖਾਲੀ
dump_ਸ਼ਬਦ(
    slang_T	*slang,
    char_u	*ਸ਼ਬਦ,
    char_u	*pat,
    ਪੂਰਨ		*dir,
    ਪੂਰਨ		dumpflags,
    ਪੂਰਨ		wordflags,
    linenr_T	lnum)
{
    ਪੂਰਨ		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    ਪੂਰਨ		i;
    ਪੂਰਨ		flags = wordflags;

    ਜੇ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ਜੇ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ਜੇ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_ਸ਼ਬਦ(ਸ਼ਬਦ, cword, flags);
	p = cword;
    }
    ਨਹੀਂਤਾਂ
    {
	p = ਸ਼ਬਦ;
	ਜੇ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(ਸ਼ਬਦ, ਨਿਰਾ) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ਜੇ (pat == ਨਿਰਾ)
    {
	// Add flags and regions after a slash.
	ਜੇ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ਜੇ (keepcap)
		STRCAT(badword, "=");
	    ਜੇ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    ਨਹੀਂਤਾਂ ਜੇ (flags & WF_RARE)
		STRCAT(badword, "?");
	    ਜੇ (flags & WF_REGION)
		ਲਈ (i = 0; i < 7; ++i)
		    ਜੇ (flags & (0x10000 << i))
			sprintf((ਅੱਖਰ *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ਜੇ (dumpflags & DUMPFLAG_ਗਿਣਤੀ)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_ਲੱਭੋ(&slang->sl_wordcount, tw);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_ਗਿਣਤੀ);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_ਜੋੜੋ_infercase(p, (ਪੂਰਨ)STRLEN(p),
					  p_ic, ਨਿਰਾ, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    ਸਥਿਰ linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*ਸ਼ਬਦ,	    // case-folded word
    char_u	*pat,
    ਪੂਰਨ		*dir,
    ਪੂਰਨ		dumpflags,
    ਪੂਰਨ		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    ਪੂਰਨ		curi[MAXWLEN];
    char_u	ਅਗੇਤਰ[MAXWLEN];
    char_u	ਸ਼ਬਦ_ਉੱਪਰ[MAXWLEN];
    ਪੂਰਨ		has_ਸ਼ਬਦ_ਉੱਪਰ = FALSE;
    ਪੂਰਨ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    ਪੂਰਨ		n;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(ਸ਼ਬਦ);
    ਜੇ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(ਸ਼ਬਦ, ਸ਼ਬਦ_ਉੱਪਰ, TRUE);
	has_ਸ਼ਬਦ_ਉੱਪਰ = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ਜੇ (byts != ਨਿਰਾ)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	ਡੂੰਘਾਈ = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ਜਦੋਂ (ਡੂੰਘਾਈ >= 0 && !got_int)
	{
	    n = arridx[ਡੂੰਘਾਈ];
	    len = byts[n];
	    ਜੇ (curi[ਡੂੰਘਾਈ] > len)
	    {
		// Done all bytes at this node, go up one level.
		--ਡੂੰਘਾਈ;
		ਸਤਰ_breakcheck();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Do one more byte at this node.
		n += curi[ਡੂੰਘਾਈ];
		++curi[ਡੂੰਘਾਈ];
		c = byts[n];
		ਜੇ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    ਲਈ (i = 1; i < len; ++i)
			ਜੇ (byts[n + i] != 0)
			    ਰੋਕੋ;
		    curi[ਡੂੰਘਾਈ] += i - 1;

		    c = valid_ਸ਼ਬਦ_ਅਗੇਤਰ(i, n, flags, ਸ਼ਬਦ, slang, FALSE);
		    ਜੇ (c != 0)
		    {
			vim_strncpy(ਅਗੇਤਰ + ਡੂੰਘਾਈ, ਸ਼ਬਦ, MAXWLEN - ਡੂੰਘਾਈ - 1);
			dump_ਸ਼ਬਦ(slang, ਅਗੇਤਰ, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ਜੇ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ਜੇ (has_ਸ਼ਬਦ_ਉੱਪਰ)
		    {
			c = valid_ਸ਼ਬਦ_ਅਗੇਤਰ(i, n, flags, ਸ਼ਬਦ_ਉੱਪਰ, slang,
									TRUE);
			ਜੇ (c != 0)
			{
			    vim_strncpy(ਅਗੇਤਰ + ਡੂੰਘਾਈ, ਸ਼ਬਦ_ਉੱਪਰ,
							 MAXWLEN - ਡੂੰਘਾਈ - 1);
			    dump_ਸ਼ਬਦ(slang, ਅਗੇਤਰ, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ਜੇ (lnum != 0)
				++lnum;
			}
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਡੂੰਘਾਈ < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    ਅਗੇਤਰ[ਡੂੰਘਾਈ++] = c;
		    arridx[ਡੂੰਘਾਈ] = idxs[n];
		    curi[ਡੂੰਘਾਈ] = 1;
		}
	    }
	}
    }

    ਵਾਪਸ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_ਮੰਜ਼ਿਲ_ਸ਼ਬਦ_ਸਿਰਾ(char_u *ਸ਼ੁਰੂ, win_T *win)
{
    char_u  *p = ਸ਼ੁਰੂ;

    ਜਦੋਂ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ਵਾਪਸ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    ਪੂਰਨ
spell_ਸ਼ਬਦ_ਸ਼ੁਰੂ(ਪੂਰਨ startcol)
{
    char_u	*ਸਤਰ;
    char_u	*p;
    ਪੂਰਨ		ਖਾਨਾ = 0;

    ਜੇ (no_spell_checking(curwin))
	ਵਾਪਸ startcol;

    // Find a word character before "startcol".
    ਸਤਰ = ml_get_curline();
    ਲਈ (p = ਸਤਰ + startcol; p > ਸਤਰ; )
    {
	MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
	ਜੇ (spell_iswordp_nmw(p, curwin))
	    ਰੋਕੋ;
    }

    // Go back to start of the word.
    ਜਦੋਂ (p > ਸਤਰ)
    {
	ਖਾਨਾ = (ਪੂਰਨ)(p - ਸਤਰ);
	MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
	ਜੇ (!spell_iswordp(p, curwin))
	    ਰੋਕੋ;
	ਖਾਨਾ = 0;
    }

    ਵਾਪਸ ਖਾਨਾ;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
ਸਥਿਰ ਪੂਰਨ spell_expand_need_cap;

    ਖਾਲੀ
spell_expand_check_cap(colnr_T ਖਾਨਾ)
{
    spell_expand_need_cap = check_need_cap(curwin, curwin->w_cursor.lnum, ਖਾਨਾ);
}

/*
 * Get list of spelling suggestions.
 * Used for Insert mode completion CTRL-X ?.
 * Returns the number of matches.  The matches are in "matchp[]", array of
 * allocated strings.
 */
    ਪੂਰਨ
expand_spelling(
    linenr_T	lnum ਅਣਵਰਤਿਆ,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_ਅੰਕੜੇ;
    ਵਾਪਸ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    ਪੂਰਨ
valid_spelllang(char_u *val)
{
    ਵਾਪਸ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    ਪੂਰਨ
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    ਪੂਰਨ		l;

    spf = val;
    ਜਦੋਂ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ਜੇ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ਵਾਪਸ FALSE;
	ਲਈ (s = spf_name; *s != NUL; ++s)
	    ਜੇ (!vim_is_fname_char(*s))
		ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spell_option(ਖਾਲੀ)
{
    ਅੱਖਰ    *errmsg = ਨਿਰਾ;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    ਰੋਕੋ;
	}
    ਵਾਪਸ errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    ਅੱਖਰ *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    ਜੇ (synblock->b_p_spc == ਨਿਰਾ || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ਜੇ (re != ਨਿਰਾ)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ਜੇ (synblock->b_cap_prog == ਨਿਰਾ)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    }
	}
    }

    vim_regfree(rp);
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ  // FEAT_SPELL
