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

#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_SPELL_C
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_SPELL)

#ಅನ್ತರ್ಭಾವ <time.h>

#ವ್ಯಾಖ್ಯಾನಿಸು REGION_ALL 0xff		// word valid in all regions

// Result values.  Lower number is accepted over higher one.
#ವ್ಯಾಖ್ಯಾನಿಸು SP_BANNED	(-1)
#ವ್ಯಾಖ್ಯಾನಿಸು SP_OK		0
#ವ್ಯಾಖ್ಯಾನಿಸು SP_RARE		1
#ವ್ಯಾಖ್ಯಾನಿಸು SP_LOCAL	2
#ವ್ಯಾಖ್ಯಾನಿಸು SP_BAD		3

/*
 * Structure to store info for word matching.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ matchinf_S
{
    langp_T	*mi_lp;			// info for language and region

    // pointers to original text to be checked
    char_u	*mi_ಶಬ್ದ;		// start of word being checked
    char_u	*mi_ಕೊನೆ;		// end of matching word so far
    char_u	*mi_fend;		// next char to be added to mi_fword
    char_u	*mi_cend;		// char after what was used for
					// mi_capflags

    // case-folded text
    char_u	mi_fword[MAXWLEN + 1];	// mi_word case-folded
    ಪೂರ್ಣಾಂಕ		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    ಪೂರ್ಣಾಂಕ		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    ಪೂರ್ಣಾಂಕ		mi_prefcnt;		// number of entries at mi_prefarridx
    ಪೂರ್ಣಾಂಕ		mi_prefixlen;		// byte length of prefix
    ಪೂರ್ಣಾಂಕ		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    ಪೂರ್ಣಾಂಕ		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    ಪೂರ್ಣಾಂಕ		mi_complen;		// nr of compound words used
    ಪೂರ್ಣಾಂಕ		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    ಪೂರ್ಣಾಂಕ		mi_ಫಲಿತಾಂಶ;		// result so far: SP_BAD, SP_OK, etc.
    ಪೂರ್ಣಾಂಕ		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    ಪೂರ್ಣಾಂಕ		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spell_mb_isword_class(ಪೂರ್ಣಾಂಕ cl, win_T *wp);

// mode values for find_word
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_FOLDWORD	    0	// find word case-folded
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_KEEPWORD	    1	// find keep-case word
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ	    2	// find word after prefix
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_COMPOUND	    3	// find case-folded compound word
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_KEEPCOMPOUND   4	// find keep-case compound word

// type values for get_char_type
#ವ್ಯಾಖ್ಯಾನಿಸು CHAR_OTHER	0
#ವ್ಯಾಖ್ಯಾನಿಸು CHAR_ಊರ್ಧ್ವ	1
#ವ್ಯಾಖ್ಯಾನಿಸು CHAR_DIGIT	2

ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಶಬ್ದ(matchinf_T *mip, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);
ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ_2(matchinf_T *mip, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fold_ಇನ್ನಷ್ಟು(matchinf_T *mip);
ಸ್ಥಿರ ಶೂನ್ಯ spell_ತುಂಬು_cb(char_u *fname, ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_syllables(slang_T *slang, char_u *ಶಬ್ದ);
ಸ್ಥಿರ ಶೂನ್ಯ clear_midword(win_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ use_midword(slang_T *lp, win_T *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_region(char_u *rp, char_u *region);
ಸ್ಥಿರ ಶೂನ್ಯ spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
ಸ್ಥಿರ ಶೂನ್ಯ spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
ಸ್ಥಿರ ಶೂನ್ಯ spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
ಸ್ಥಿರ ಶೂನ್ಯ dump_ಶಬ್ದ(slang_T *slang, char_u *ಶಬ್ದ, char_u *pat, ಪೂರ್ಣಾಂಕ *dir, ಪೂರ್ಣಾಂಕ ಸುತ್ತು, ಪೂರ್ಣಾಂಕ flags, linenr_T lnum);
ಸ್ಥಿರ linenr_T dump_prefixes(slang_T *slang, char_u *ಶಬ್ದ, char_u *pat, ಪೂರ್ಣಾಂಕ *dir, ಪೂರ್ಣಾಂಕ ಸುತ್ತು, ಪೂರ್ಣಾಂಕ flags, linenr_T startlnum);
ಸ್ಥಿರ char_u *advance_camelcase_ಶಬ್ದ(char_u *p, win_T *wp, ಪೂರ್ಣಾಂಕ *is_camel_case);

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
    ಪೂರ್ಣಾಂಕ
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    ಪೂರ್ಣಾಂಕ		*capcol,	// column to check for Capital
    ಪೂರ್ಣಾಂಕ		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    ಪೂರ್ಣಾಂಕ		nrlen = 0;	// found a number first
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		wrongcaplen = 0;
    ಪೂರ್ಣಾಂಕ		lpi;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_ಶಬ್ದ = docount;
    ಪೂರ್ಣಾಂಕ		use_camel_case = *wp->w_s->b_p_spo != NUL;
    ಪೂರ್ಣಾಂಕ		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ಆದರೆ (*ptr <= ' ')
	ಹಿಂತಿರುಗಿ 1;

    // Return here when loading language files failed.
    ಆದರೆ (wp->w_s->b_langp.ga_len == 0)
	ಹಿಂತಿರುಗಿ 1;

    CLEAR_ಕ್ಷೇತ್ರ(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ಆದರೆ (*ptr >= '0' && *ptr <= '9')
    {
	ಆದರೆ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_ಕೊನೆ = skipbin(ptr + 2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_ಕೊನೆ = skiphex(ptr + 2);
	ಇಲ್ಲದಿದ್ದರೆ
	    mi.mi_ಕೊನೆ = skipdigits(ptr);
	nrlen = (ಪೂರ್ಣಾಂಕ)(mi.mi_ಕೊನೆ - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_ಶಬ್ದ = ptr;
    mi.mi_fend = ptr;
    ಆದರೆ (spell_iswordp(mi.mi_fend, wp))
    {
	ಆದರೆ (use_camel_case)
	    mi.mi_fend = advance_camelcase_ಶಬ್ದ(ptr, wp, &is_camel_case);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಮಾಡು
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ಆಗ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ಆದರೆ (capcol != ಖಾಲಿ && *capcol == 0 && wp->w_s->b_cap_prog != ಖಾಲಿ)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ಆದರೆ (!SPELL_ISUPPER(c))
		wrongcaplen = (ಪೂರ್ಣಾಂಕ)(mi.mi_fend - ptr);
	}
    }
    ಆದರೆ (capcol != ಖಾಲಿ)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_ಕೊನೆ = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ಖಾಲಿ;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    ಆದರೆ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (ಶೂನ್ಯ)spell_casefold(wp, ptr, (ಪೂರ್ಣಾಂಕ)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (ಪೂರ್ಣಾಂಕ)STRLEN(mi.mi_fword);

    ಆದರೆ (is_camel_case && mi.mi_fwordlen > 0)
	// Introduce a fake word end space into the folded word.
	mi.mi_fword[mi.mi_fwordlen - 1] = ' ';

    // The word is bad unless we recognize it.
    mi.mi_ಫಲಿತಾಂಶ = SP_BAD;
    mi.mi_result2 = SP_BAD;

    /*
     * Loop over the languages specified in 'spelllang'.
     * We check them all, because a word may be matched longer in another
     * language.
     */
    ಗಾಗಿ (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ನಮೂದು(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ಆದರೆ (mi.mi_lp->lp_slang->sl_fidxs == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// Check for a matching word in case-folded words.
	ಕಂಡುಹಿಡಿ_ಶಬ್ದ(&mi, ಕಂಡುಹಿಡಿ_FOLDWORD);

	// Check for a matching word in keep-case words.
	ಕಂಡುಹಿಡಿ_ಶಬ್ದ(&mi, ಕಂಡುಹಿಡಿ_KEEPWORD);

	// Check for matching prefixes.
	ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ_2(&mi, ಕಂಡುಹಿಡಿ_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ಆದರೆ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_ಫಲಿತಾಂಶ == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_ಫಲಿತಾಂಶ = mi.mi_result2;
	    mi.mi_ಕೊನೆ = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ಆದರೆ (ಎಣಿಕೆ_ಶಬ್ದ && mi.mi_ಫಲಿತಾಂಶ == SP_OK)
	{
	    ಎಣಿಕೆ_common_ಶಬ್ದ(mi.mi_lp->lp_slang, ptr,
						   (ಪೂರ್ಣಾಂಕ)(mi.mi_ಕೊನೆ - ptr), 1);
	    ಎಣಿಕೆ_ಶಬ್ದ = FALSE;
	}
    }

    ಆದರೆ (mi.mi_ಫಲಿತಾಂಶ != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ಆದರೆ (nrlen > 0)
	{
	    ಆದರೆ (mi.mi_ಫಲಿತಾಂಶ == SP_BAD || mi.mi_ಫಲಿತಾಂಶ == SP_BANNED)
		ಹಿಂತಿರುಗಿ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!spell_iswordp_nmw(ptr, wp))
	{
	    ಆದರೆ (capcol != ಖಾಲಿ && wp->w_s->b_cap_prog != ಖಾಲಿ)
	    {
		regmatch_T	regmatch;
		ಪೂರ್ಣಾಂಕ		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ಆದರೆ (r)
		    *capcol = (ಪೂರ್ಣಾಂಕ)(regmatch.endp[0] - ptr);
	    }

	    ಆದರೆ (has_mbyte)
		ಹಿಂತಿರುಗಿ (*mb_ptr2len)(ptr);
	    ಹಿಂತಿರುಗಿ 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mi.mi_ಕೊನೆ == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_ಕೊನೆ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mi.mi_ಫಲಿತಾಂಶ == SP_BAD
		&& LANGP_ನಮೂದು(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    ಪೂರ್ಣಾಂಕ		save_ಫಲಿತಾಂಶ = mi.mi_ಫಲಿತಾಂಶ;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ನಮೂದು(wp->w_s->b_langp, 0);
	    ಆದರೆ (mi.mi_lp->lp_slang->sl_fidxs != ಖಾಲಿ)
	    {
		p = mi.mi_ಶಬ್ದ;
		fp = mi.mi_fword;
		ಗಾಗಿ (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ಆದರೆ (p >= mi.mi_ಕೊನೆ)
			ನಿಲ್ಲಿಸು;
		    mi.mi_compoff = (ಪೂರ್ಣಾಂಕ)(fp - mi.mi_fword);
		    ಕಂಡುಹಿಡಿ_ಶಬ್ದ(&mi, ಕಂಡುಹಿಡಿ_COMPOUND);
		    ಆದರೆ (mi.mi_ಫಲಿತಾಂಶ != SP_BAD)
		    {
			mi.mi_ಕೊನೆ = p;
			ನಿಲ್ಲಿಸು;
		    }
		}
		mi.mi_ಫಲಿತಾಂಶ = save_ಫಲಿತಾಂಶ;
	    }
	}

	ಆದರೆ (mi.mi_ಫಲಿತಾಂಶ == SP_BAD || mi.mi_ಫಲಿತಾಂಶ == SP_BANNED)
	    *attrp = HLF_SPB;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mi.mi_ಫಲಿತಾಂಶ == SP_RARE)
	    *attrp = HLF_SPR;
	ಇಲ್ಲದಿದ್ದರೆ
	    *attrp = HLF_SPL;
    }

    ಆದರೆ (wrongcaplen > 0 && (mi.mi_ಫಲಿತಾಂಶ == SP_OK || mi.mi_ಫಲಿತಾಂಶ == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ಹಿಂತಿರುಗಿ wrongcaplen;
    }

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(mi.mi_ಕೊನೆ - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_char_type(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (VIM_ISDIGIT(c))
	ಹಿಂತಿರುಗಿ CHAR_DIGIT;
    ಆದರೆ (SPELL_ISUPPER(c))
	ಹಿಂತಿರುಗಿ CHAR_ಊರ್ಧ್ವ;
    ಹಿಂತಿರುಗಿ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    ಸ್ಥಿರ char_u *
advance_camelcase_ಶಬ್ದ(char_u *str, win_T *wp, ಪೂರ್ಣಾಂಕ *is_camel_case)
{
    ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_type, ಕೊನೆಯ_ಕೊನೆಯ_type, this_type;
    ಪೂರ್ಣಾಂಕ c;
    char_u *ಕೊನೆ = str;

    *is_camel_case = FALSE;

    ಆದರೆ (*str == NUL)
	ಹಿಂತಿರುಗಿ str;

    c = PTR2CHAR(ಕೊನೆ);
    MB_PTR_ADV(ಕೊನೆ);
    // We need at most the types of the type of the last two chars.
    ಕೊನೆಯ_ಕೊನೆಯ_type = -1;
    ಕೊನೆಯ_type = get_char_type(c);

    ಆಗ (*ಕೊನೆ != NUL && spell_iswordp(ಕೊನೆ, wp))
    {
	c = PTR2CHAR(ಕೊನೆ);
	this_type = get_char_type(c);

	ಆದರೆ (ಕೊನೆಯ_ಕೊನೆಯ_type == CHAR_ಊರ್ಧ್ವ && ಕೊನೆಯ_type == CHAR_ಊರ್ಧ್ವ
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_ಹಿಂಭಾಗ(str, ಕೊನೆ);
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((this_type == CHAR_ಊರ್ಧ್ವ && ಕೊನೆಯ_type == CHAR_OTHER)
		|| (this_type != ಕೊನೆಯ_type
		    && (this_type == CHAR_DIGIT || ಕೊನೆಯ_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	ಕೊನೆಯ_ಕೊನೆಯ_type = ಕೊನೆಯ_type;
	ಕೊನೆಯ_type = this_type;

	MB_PTR_ADV(ಕೊನೆ);
    }

    ಹಿಂತಿರುಗಿ ಕೊನೆ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ಶಬ್ದ(matchinf_T *mip, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
{
    idx_T	arridx = 0;
    ಪೂರ್ಣಾಂಕ		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    ಪೂರ್ಣಾಂಕ		endidxcnt = 0;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		wlen = 0;
    ಪೂರ್ಣಾಂಕ		flen;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    ಚಿಹ್ನೆರಹಿತ	flags;
    char_u	*byts;
    idx_T	*idxs;
    ಪೂರ್ಣಾಂಕ		ಶಬ್ದ_ತುದಿಗಳು;
    ಪೂರ್ಣಾಂಕ		ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಿಕ್ಕಿತು;
    ಪೂರ್ಣಾಂಕ		nobreak_ಫಲಿತಾಂಶ;

    ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_KEEPWORD || ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_ಶಬ್ದ;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ಆದರೆ (byts == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_ಇನ್ನಷ್ಟು(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ಆದರೆ (byts[arridx] == 0)
	{
	    ಆದರೆ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_ಒಳ_spell_file));
		ಹಿಂತಿರುಗಿ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ಆಗ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ಆದರೆ (len == 0)
		ನಿಲ್ಲಿಸು;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ಆದರೆ (ptr[wlen] == NUL)
	    ನಿಲ್ಲಿಸು;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ಆದರೆ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ಆಗ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ಆದರೆ (byts[m] > c)
		hi = m - 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (byts[m] < c)
		lo = m + 1;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		lo = hi = m;
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Stop if there is no matching byte.
	ಆದರೆ (hi < lo || byts[lo] != c)
	    ನಿಲ್ಲಿಸು;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ಆದರೆ (c == ' ')
	{
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_ಇನ್ನಷ್ಟು(mip);
		ಆದರೆ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    ನಿಲ್ಲಿಸು;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    ಆಗ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ಆದರೆ ((*mb_ತಲೆ_off)(ptr, ptr + wlen) > 0)
	    ಮುಂದುವರಿ;	    // not at first byte of character
	ಆದರೆ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ಆದರೆ (slang->sl_compprog == ಖಾಲಿ && !slang->sl_nobreak)
		ಮುಂದುವರಿ;	    // next char is a word character
	    ಶಬ್ದ_ತುದಿಗಳು = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಶಬ್ದ_ತುದಿಗಳು = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಿಕ್ಕಿತು = FALSE;

	ಆದರೆ (ಬಹುಲಕ != ಕಂಡುಹಿಡಿ_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_ಶಬ್ದ;
	    ಆದರೆ (STRNCMP(ptr, p, wlen) != 0)
	    {
		ಗಾಗಿ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (ಪೂರ್ಣಾಂಕ)(p - mip->mi_ಶಬ್ದ);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	ಗಾಗಿ (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_FOLDWORD)
	    {
		ಆದರೆ (mip->mi_cend != mip->mi_ಶಬ್ದ + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_ಶಬ್ದ + wlen;
		    mip->mi_capflags = captype(mip->mi_ಶಬ್ದ, mip->mi_cend);
		}

		ಆದರೆ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    ಮುಂದುವರಿ;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ && !ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಿಕ್ಕಿತು)
	    {
		c = valid_ಶಬ್ದ_ಪೂರ್ವಪ್ರತ್ಯಯ(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_ಶಬ್ದ + mip->mi_cprefixlen, slang,
				    FALSE);
		ಆದರೆ (c == 0)
		    ಮುಂದುವರಿ;

		// Use the WF_RARE flag for a rare prefix.
		ಆದರೆ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಿಕ್ಕಿತು = TRUE;
	    }

	    ಆದರೆ (slang->sl_nobreak)
	    {
		ಆದರೆ ((ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_COMPOUND || ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_ಫಲಿತಾಂಶ = SP_OK;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_COMPOUND || ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_KEEPCOMPOUND
								|| !ಶಬ್ದ_ತುದಿಗಳು))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ಆದರೆ (((ಚಿಹ್ನೆರಹಿತ)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    ಮುಂದುವರಿ;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ಆದರೆ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_ಶಬ್ದ + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			ಮುಂದುವರಿ;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ಆದರೆ (!ಶಬ್ದ_ತುದಿಗಳು && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    ಮುಂದುವರಿ;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ಆದರೆ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    ಮುಂದುವರಿ;
		ಆದರೆ (!ಶಬ್ದ_ತುದಿಗಳು && (flags & WF_NOCOMPAFT))
		    ಮುಂದುವರಿ;

		// Quickly check if compounding is possible with this flag.
		ಆದರೆ (!byte_ಒಳ_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((ಚಿಹ್ನೆರಹಿತ)flags >> 24)))
		    ಮುಂದುವರಿ;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ಆದರೆ (ಹೊಂದಿಕೆ_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    ಮುಂದುವರಿ;

		ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_COMPOUND)
		{
		    ಪೂರ್ಣಾಂಕ	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ಆದರೆ (has_mbyte && STRNCMP(ptr, mip->mi_ಶಬ್ದ,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_ಶಬ್ದ;
			ಗಾಗಿ (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			p = mip->mi_ಶಬ್ದ + mip->mi_compoff;
		    capflags = captype(p, mip->mi_ಶಬ್ದ + wlen);
		    ಆದರೆ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			ಮುಂದುವರಿ;

		    ಆದರೆ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_ಹಿಂಭಾಗ(mip->mi_ಶಬ್ದ, p);
			ಆದರೆ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    ಮುಂದುವರಿ;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((ಚಿಹ್ನೆರಹಿತ)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		ಆದರೆ (ಶಬ್ದ_ತುದಿಗಳು)
		{
		    char_u	fword[MAXWLEN];

		    ಆದರೆ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ಆದರೆ (ptr == mip->mi_ಶಬ್ದ)
			    (ಶೂನ್ಯ)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			ಇಲ್ಲದಿದ್ದರೆ
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ಆದರೆ (!can_compound(slang, fword, mip->mi_compflags))
			ಮುಂದುವರಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (slang->sl_comprules != ಖಾಲಿ
			     && !ಹೊಂದಿಕೆ_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    ಮುಂದುವರಿ;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WF_NEEDCOMP)
		ಮುಂದುವರಿ;

	    nobreak_ಫಲಿತಾಂಶ = SP_OK;

	    ಆದರೆ (!ಶಬ್ದ_ತುದಿಗಳು)
	    {
		ಪೂರ್ಣಾಂಕ	save_ಫಲಿತಾಂಶ = mip->mi_ಫಲಿತಾಂಶ;
		char_u	*save_ಕೊನೆ = mip->mi_ಕೊನೆ;
		langp_T	*save_lp = mip->mi_lp;
		ಪೂರ್ಣಾಂಕ	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ಆದರೆ (slang->sl_nobreak)
		    mip->mi_ಫಲಿತಾಂಶ = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ಆದರೆ (has_mbyte && ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ಆದರೆ (STRNCMP(ptr, p, wlen) != 0)
		    {
			ಗಾಗಿ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (ಪೂರ್ಣಾಂಕ)(p - mip->mi_fword);
		    }
		}
#ಆದರೆ 0 // Disabled, see below
		c = mip->mi_compoff;
#ಆದರೆ_ಅಂತ್ಯ
		++mip->mi_complen;
		ಆದರೆ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		ಗಾಗಿ (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ಆದರೆ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ನಮೂದು(mip->mi_win->w_s->b_langp, lpi);
			ಆದರೆ (mip->mi_lp->lp_slang->sl_fidxs == ಖಾಲಿ
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    ಮುಂದುವರಿ;
		    }

		    ಕಂಡುಹಿಡಿ_ಶಬ್ದ(mip, ಕಂಡುಹಿಡಿ_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ಆದರೆ (!slang->sl_nobreak || mip->mi_ಫಲಿತಾಂಶ == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			ಕಂಡುಹಿಡಿ_ಶಬ್ದ(mip, ಕಂಡುಹಿಡಿ_KEEPCOMPOUND);

#ಆದರೆ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ಆದರೆ (!slang->sl_nobreak || mip->mi_ಫಲಿತಾಂಶ == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ_2(mip, ಕಂಡುಹಿಡಿ_COMPOUND);
			}
#ಆದರೆ_ಅಂತ್ಯ
		    }

		    ಆದರೆ (!slang->sl_nobreak)
			ನಿಲ್ಲಿಸು;
		}
		--mip->mi_complen;
		ಆದರೆ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ಆದರೆ (slang->sl_nobreak)
		{
		    nobreak_ಫಲಿತಾಂಶ = mip->mi_ಫಲಿತಾಂಶ;
		    mip->mi_ಫಲಿತಾಂಶ = save_ಫಲಿತಾಂಶ;
		    mip->mi_ಕೊನೆ = save_ಕೊನೆ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (mip->mi_ಫಲಿತಾಂಶ == SP_OK)
			ನಿಲ್ಲಿಸು;
		    ಮುಂದುವರಿ;
		}
	    }

	    ಆದರೆ (flags & WF_BANNED)
		res = SP_BANNED;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WF_REGION)
	    {
		// Check region.
		ಆದರೆ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		ಇಲ್ಲದಿದ್ದರೆ
		    res = SP_LOCAL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WF_RARE)
		res = SP_RARE;
	    ಇಲ್ಲದಿದ್ದರೆ
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ಆದರೆ (nobreak_ಫಲಿತಾಂಶ == SP_BAD)
	    {
		ಆದರೆ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_ಶಬ್ದ + wlen;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_ಶಬ್ದ + wlen)
		    mip->mi_end2 = mip->mi_ಶಬ್ದ + wlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mip->mi_ಫಲಿತಾಂಶ > res)
	    {
		mip->mi_ಫಲಿತಾಂಶ = res;
		mip->mi_ಕೊನೆ = mip->mi_ಶಬ್ದ + wlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mip->mi_ಫಲಿತಾಂಶ == res && mip->mi_ಕೊನೆ < mip->mi_ಶಬ್ದ + wlen)
		mip->mi_ಕೊನೆ = mip->mi_ಶಬ್ದ + wlen;

	    ಆದರೆ (mip->mi_ಫಲಿತಾಂಶ == SP_OK)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (mip->mi_ಫಲಿತಾಂಶ == SP_OK)
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_checkcompoundpattern(
    char_u	*ptr,
    ಪೂರ್ಣಾಂಕ		wlen,
    garray_T	*gap)  // &sl_comppat
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    ಗಾಗಿ (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_ದತ್ತಾಂಶ)[i + 1];
	ಆದರೆ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_ದತ್ತಾಂಶ)[i];
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
	    ಆದರೆ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    ಪೂರ್ಣಾಂಕ
can_compound(slang_T *slang, char_u *ಶಬ್ದ, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;

    ಆದರೆ (slang->sl_compprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	ಗಾಗಿ (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	p = flags;
    ಆದರೆ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ಹಿಂತಿರುಗಿ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ಆದರೆ (slang->sl_compsylmax < MAXWLEN
		       && ಎಣಿಕೆ_syllables(slang, ಶಬ್ದ) > slang->sl_compsylmax)
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)STRLEN(flags) < slang->sl_compmax;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;

    // loop over all the COMPOUNDRULE entries
    ಗಾಗಿ (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	ಗಾಗಿ (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ಆದರೆ (c == NUL)
		// found a rule that matches for the flags we have so far
		ಹಿಂತಿರುಗಿ TRUE;
	    ಆದರೆ (*p == '/' || *p == NUL)
		ನಿಲ್ಲಿಸು;  // end of rule, it's too short
	    ಆದರೆ (*p == '[')
	    {
		ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ = FALSE;

		// compare against all the flags in []
		++p;
		ಆಗ (*p != ']' && *p != NUL)
		    ಆದರೆ (*p++ == c)
			ಹೊಂದಿಕೆ = TRUE;
		ಆದರೆ (!ಹೊಂದಿಕೆ)
		    ನಿಲ್ಲಿಸು;  // none matches
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != c)
		ನಿಲ್ಲಿಸು;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ಆದರೆ (p == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    ಪೂರ್ಣಾಂಕ
valid_ಶಬ್ದ_ಪೂರ್ವಪ್ರತ್ಯಯ(
    ಪೂರ್ಣಾಂಕ		totprefcnt,	// nr of prefix IDs
    ಪೂರ್ಣಾಂಕ		arridx,		// idx in sl_pidxs[]
    ಪೂರ್ಣಾಂಕ		flags,
    char_u	*ಶಬ್ದ,
    slang_T	*slang,
    ಪೂರ್ಣಾಂಕ		cond_req)	// only use prefixes with a condition
{
    ಪೂರ್ಣಾಂಕ		prefcnt;
    ಪೂರ್ಣಾಂಕ		pidx;
    regprog_T	**rp;
    ಪೂರ್ಣಾಂಕ		prefid;

    prefid = (ಚಿಹ್ನೆರಹಿತ)flags >> 24;
    ಗಾಗಿ (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ಆದರೆ (prefid != (pidx & 0xff))
	    ಮುಂದುವರಿ;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ಆದರೆ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    ಮುಂದುವರಿ;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((ಚಿಹ್ನೆರಹಿತ)pidx >> 8) & 0xffff];
	ಆದರೆ (*rp != ಖಾಲಿ)
	{
	    ಆದರೆ (!vim_regexec_prog(rp, FALSE, ಶಬ್ದ, 0))
		ಮುಂದುವರಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cond_req)
	    ಮುಂದುವರಿ;

	// It's a match!  Return the WF_ flags.
	ಹಿಂತಿರುಗಿ pidx;
    }
    ಹಿಂತಿರುಗಿ 0;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ_2(matchinf_T *mip, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
{
    idx_T	arridx = 0;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		wlen = 0;
    ಪೂರ್ಣಾಂಕ		flen;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ಆದರೆ (byts == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_COMPOUND)
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
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_ಇನ್ನಷ್ಟು(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ಆದರೆ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ಆಗ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ಆದರೆ (ಬಹುಲಕ == ಕಂಡುಹಿಡಿ_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ಆದರೆ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_ಶಬ್ದ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    ಕಂಡುಹಿಡಿ_ಶಬ್ದ(mip, ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ);


	    ಆದರೆ (len == 0)
		ನಿಲ್ಲಿಸು;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ಆದರೆ (ptr[wlen] == NUL)
	    ನಿಲ್ಲಿಸು;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ಆಗ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ಆದರೆ (byts[m] > c)
		hi = m - 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (byts[m] < c)
		lo = m + 1;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		lo = hi = m;
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Stop if there is no matching byte.
	ಆದರೆ (hi < lo || byts[lo] != c)
	    ನಿಲ್ಲಿಸು;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fold_ಇನ್ನಷ್ಟು(matchinf_T *mip)
{
    ಪೂರ್ಣಾಂಕ		flen;
    char_u	*p;

    p = mip->mi_fend;
    ಮಾಡು
	MB_PTR_ADV(mip->mi_fend);
    ಆಗ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ಆದರೆ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (ಶೂನ್ಯ)spell_casefold(mip->mi_win, p, (ಪೂರ್ಣಾಂಕ)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (ಪೂರ್ಣಾಂಕ)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ಹಿಂತಿರುಗಿ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    ಪೂರ್ಣಾಂಕ
spell_valid_case(
    ಪೂರ್ಣಾಂಕ	    wordflags,	    // flags for the checked word.
    ಪೂರ್ಣಾಂಕ	    treeflags)	    // flags for the word in the spell tree
{
    ಹಿಂತಿರುಗಿ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    ಪೂರ್ಣಾಂಕ
spell_check_ಕಿಟಕಿ(win_T *wp)
{
    ಹಿಂತಿರುಗಿ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(ಅಕ್ಷರ **)(wp->w_s->b_langp.ga_ದತ್ತಾಂಶ) != ಖಾಲಿ;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
no_spell_checking(win_T *wp)
{
    ಆದರೆ (spell_check_ಕಿಟಕಿ(wp))
	ಹಿಂತಿರುಗಿ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    ಪೂರ್ಣಾಂಕ
spell_move_ಗಮ್ಯ(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    ಪೂರ್ಣಾಂಕ		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	ಸಿಕ್ಕಿತು_pos;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_len = 0;
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    ಪೂರ್ಣಾಂಕ		len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		has_syntax = syntax_present(wp);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		can_spell;
    char_u	*buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		buflen = 0;
    ಪೂರ್ಣಾಂಕ		skip = 0;
    ಪೂರ್ಣಾಂಕ		capcol = -1;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_one = FALSE;
    ಪೂರ್ಣಾಂಕ		wrapped = FALSE;

    ಆದರೆ (no_spell_checking(wp))
	ಹಿಂತಿರುಗಿ 0;

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
    CLEAR_POS(&ಸಿಕ್ಕಿತು_pos);

    ಆಗ (!got_int)
    {
	ಪೂರ್ಣಾಂಕ ಖಾಲಿಗಳು_ಸಾಲು;

	ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);

	len = ml_get_buf_len(wp->w_ಬಫರ್, lnum);
	ಆದರೆ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ಆದರೆ (buf == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}

	// In first line check first word for Capital.
	ಆದರೆ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ಆದರೆ (capcol == 0)
	    capcol = getwhitecols(ಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    ಲಂಬಸಾಲು = getwhitecols(ಸಾಲು);
	    ಆದರೆ (check_need_cap(curwin, lnum, ಲಂಬಸಾಲು))
		capcol = ಲಂಬಸಾಲು;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	ಖಾಲಿಗಳು_ಸಾಲು = *skipwhite(ಸಾಲು) == NUL;
	STRCPY(buf, ಸಾಲು);
	ಆದರೆ (lnum < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    spell_cat_ಸಾಲು(buf + STRLEN(buf),
			  ml_get_buf(wp->w_ಬಫರ್, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ಆಗ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ಆದರೆ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.ಲಂಬಸಾಲು)
		ನಿಲ್ಲಿಸು;

	    // start of word
	    attr = HLF_ಎಣಿಕೆ;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ಆದರೆ (attr != HLF_ಎಣಿಕೆ)
	    {
		// We found a bad word.  Check the attribute.
		ಆದರೆ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ಆದರೆ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.ಲಂಬಸಾಲು))
		    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			ಆದರೆ (has_syntax)
			{
			    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - buf);
			    (ಶೂನ್ಯ)syn_get_id(wp, lnum, (colnr_T)ಲಂಬಸಾಲು,
						    FALSE, &can_spell, FALSE);
			    ಆದರೆ (!can_spell)
				attr = HLF_ಎಣಿಕೆ;
			}
			ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			    can_spell = TRUE;

			ಆದರೆ (can_spell)
			{
			    ಸಿಕ್ಕಿತು_one = TRUE;
			    ಸಿಕ್ಕಿತು_pos.lnum = lnum;
			    ಸಿಕ್ಕಿತು_pos.ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - buf);
			    ಸಿಕ್ಕಿತು_pos.coladd = 0;
			    ಆದರೆ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = ಸಿಕ್ಕಿತು_pos;
				vim_free(buf);
				ಆದರೆ (attrp != ಖಾಲಿ)
				    *attrp = attr;
				ಹಿಂತಿರುಗಿ len;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				ಸಿಕ್ಕಿತು_pos.ಲಂಬಸಾಲು += len;
			    ಸಿಕ್ಕಿತು_len = len;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸಿಕ್ಕಿತು_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ಆದರೆ (dir == BACKWARD && ಸಿಕ್ಕಿತು_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = ಸಿಕ್ಕಿತು_pos;
	    vim_free(buf);
	    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_len;
	}

	ಆದರೆ (curline)
	    ನಿಲ್ಲಿಸು;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ಆದರೆ (lnum == wp->w_cursor.lnum && wrapped)
	    ನಿಲ್ಲಿಸು;

	// Advance to next line.
	ಆದರೆ (dir == BACKWARD)
	{
	    ಆದರೆ (lnum > 1)
		--lnum;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!p_ws)
		ನಿಲ್ಲಿಸು;	    // at first line and 'nowrapscan'
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		wrapped = TRUE;
		ಆದರೆ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(ತುದಿ_bot_ಸಂದೇಶ), TRUE);
	    }
	    capcol = -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (lnum < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		++lnum;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!p_ws)
		ನಿಲ್ಲಿಸು;	    // at first line and 'nowrapscan'
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ಆದರೆ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(bot_ತುದಿ_ಸಂದೇಶ), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ಆದರೆ (lnum == wp->w_cursor.lnum && !ಸಿಕ್ಕಿತು_one)
		ನಿಲ್ಲಿಸು;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ಆದರೆ (attr == HLF_ಎಣಿಕೆ)
		skip = (ಪೂರ್ಣಾಂಕ)(p - endp);
	    ಇಲ್ಲದಿದ್ದರೆ
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ಆದರೆ (ಖಾಲಿಗಳು_ಸಾಲು)
		capcol = 0;
	}

	ಸಾಲು_breakcheck();
    }

    vim_free(buf);
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    ಶೂನ್ಯ
spell_cat_ಸಾಲು(char_u *buf, char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ maxlen)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		n;

    p = skipwhite(ಸಾಲು);
    ಆಗ (vim_strchr((char_u *)"*#/\"\t", *p) != ಖಾಲಿ)
	p = skipwhite(p + 1);

    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು) + 1;
    ಆದರೆ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    ಪೂರ್ಣಾಂಕ	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_ತುಂಬು_lang(char_u *lang)
{
    char_u	fname_enc[85];
    ಪೂರ್ಣಾಂಕ		r;
    spelload_T	sl;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ಖಾಲಿ;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((ಅಕ್ಷರ *)fname_enc, ಗಾತ್ರ(fname_enc) - 5,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
					"spell/%s_%s.spl",
#ಇಲ್ಲದಿದ್ದರೆ
					"spell/%s.%s.spl",
#ಆದರೆ_ಅಂತ್ಯ
							   lang, spell_enc());
	r = do_ಒಳ_runtimepath(fname_enc, 0, spell_ತುಂಬು_cb, &sl);

	ಆದರೆ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((ಅಕ್ಷರ *)fname_enc, ಗಾತ್ರ(fname_enc) - 5,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
						  "spell/%s_ascii.spl",
#ಇಲ್ಲದಿದ್ದರೆ
						  "spell/%s.ascii.spl",
#ಆದರೆ_ಅಂತ್ಯ
									lang);
	    r = do_ಒಳ_runtimepath(fname_enc, 0, spell_ತುಂಬು_cb, &sl);

	    ಆದರೆ (r == FAIL && *sl.sl_lang != NUL && ಸುತ್ತು == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		ಮುಂದುವರಿ;
	    ನಿಲ್ಲಿಸು;
	}
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (r == FAIL)
    {
	smsg(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#ಇಲ್ಲದಿದ್ದರೆ
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#ಆದರೆ_ಅಂತ್ಯ
						     lang, spell_enc(), lang);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sl.sl_slang != ಖಾಲಿ)
    {
	// At least one file was loaded, now load ALL the additions.
	STRCPY(fname_enc + STRLEN(fname_enc) - 3, "add.spl");
	do_ಒಳ_runtimepath(fname_enc, DIP_ALL, spell_ತುಂಬು_cb, &sl);
    }

    --curbuf->b_locked;
}

/*
 * Return the encoding used for spell checking: Use 'encoding', except that we
 * use "latin1" for "latin9".  And limit to 60 characters (just in case).
 */
    char_u *
spell_enc(ಶೂನ್ಯ)
{

    ಆದರೆ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ಹಿಂತಿರುಗಿ p_enc;
    ಹಿಂತಿರುಗಿ (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((ಅಕ್ಷರ *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    ಆದರೆ (lp != ಖಾಲಿ)
    {
	ಆದರೆ (lang != ಖಾಲಿ)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, ಗಾತ್ರ(fromto_T), 10);
	ga_init2(&lp->sl_repsal, ಗಾತ್ರ(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ಹಿಂತಿರುಗಿ lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
free_sal_ವಸ್ತುಗಳು(garray_T *gap)
{
    salitem_T	*smp;

    ಆಗ (gap->ga_len > 0)
    {
	smp = &((salitem_T *)gap->ga_ದತ್ತಾಂಶ)[--gap->ga_len];
	vim_free(smp->sm_lead);
	// Don't free sm_oneof and sm_rules, they point into sm_lead.
	vim_free(smp->sm_ಗಮ್ಯ);
	vim_free(smp->sm_lead_w);
	vim_free(smp->sm_oneof_w);
	vim_free(smp->sm_ಗಮ್ಯ_w);
    }
    ga_clear(gap);
}

/*
 * Clear an slang_T so that the file can be reloaded.
 */
    ಶೂನ್ಯ
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	gap = ಸುತ್ತು == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ಆಗ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_ದತ್ತಾಂಶ)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_ಗಮ್ಯ);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ಆದರೆ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ಆದರೆ (gap->ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
		vim_free(((ಪೂರ್ಣಾಂಕ **)gap->ga_ದತ್ತಾಂಶ)[i]);
	ga_clear(gap);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	free_sal_ವಸ್ತುಗಳು(gap);

    ಗಾಗಿ (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ಖಾಲಿ;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_ವಸ್ತುಗಳು);

    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_ಕೀಲಿ_OFF);
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
    ಶೂನ್ಯ
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = ಖಾಲಿ;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_ತುಂಬು_cb(char_u *fname, ಶೂನ್ಯ *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_ತುಂಬು_file(fname, slp->sl_lang, ಖಾಲಿ, FALSE);
    ಆದರೆ (slang == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ಆದರೆ (slp->sl_nobreak && slang->sl_ಕೂಡಿಸು)
	slang->sl_nobreak = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    ಶೂನ್ಯ
ಎಣಿಕೆ_common_ಶಬ್ದ(
    slang_T	*lp,
    char_u	*ಶಬ್ದ,
    ಪೂರ್ಣಾಂಕ		len,	    // word length, -1 for up to NUL
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ಆದರೆ (len == -1)
	p = ಶಬ್ದ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len >= MAXWLEN)
	ಹಿಂತಿರುಗಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_strncpy(buf, ಶಬ್ದ, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_ಶಬ್ದ) + STRLEN(p) + 1);
	ಆದರೆ (wc == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	STRCPY(wc->wc_ಶಬ್ದ, p);
	wc->wc_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
	hash_ಕೂಡಿಸು_ಅಂಶ(&lp->sl_wordcount, hi, wc->wc_ಶಬ್ದ, hash);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wc = HI2WC(hi);
	ಆದರೆ ((wc->wc_ಎಣಿಕೆ += ಎಣಿಕೆ_2) < (ಚಿಹ್ನೆರಹಿತ)ಎಣಿಕೆ_2)	// check for overflow
	    wc->wc_ಎಣಿಕೆ = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    ಪೂರ್ಣಾಂಕ
byte_ಒಳ_str(char_u *str, ಪೂರ್ಣಾಂಕ n)
{
    char_u	*p;

    ಗಾಗಿ (p = str; *p != NUL; ++p)
	ಆದರೆ (*p == n)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು SY_MAXLEN   30
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ syl_ಅಂಶ_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    ಪೂರ್ಣಾಂಕ		sy_len;
} syl_ಅಂಶ_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    ಪೂರ್ಣಾಂಕ
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		l;
    syl_ಅಂಶ_T	*syl;

    ga_init2(&slang->sl_syl_ವಸ್ತುಗಳು, ಗಾತ್ರ(syl_ಅಂಶ_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ಆಗ (p != ಖಾಲಿ)
    {
	*p++ = NUL;
	ಆದರೆ (*p == NUL)	    // trailing slash
	    ನಿಲ್ಲಿಸು;
	s = p;
	p = vim_strchr(p, '/');
	ಆದರೆ (p == ಖಾಲಿ)
	    l = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
	ಇಲ್ಲದಿದ್ದರೆ
	    l = (ಪೂರ್ಣಾಂಕ)(p - s);
	ಆದರೆ (l >= SY_MAXLEN)
	    ಹಿಂತಿರುಗಿ SP_FORMERROR;
	ಆದರೆ (ga_grow(&slang->sl_syl_ವಸ್ತುಗಳು, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ SP_OTHERERROR;
	syl = ((syl_ಅಂಶ_T *)slang->sl_syl_ವಸ್ತುಗಳು.ga_ದತ್ತಾಂಶ)
					       + slang->sl_syl_ವಸ್ತುಗಳು.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_syllables(slang_T *slang, char_u *ಶಬ್ದ)
{
    ಪೂರ್ಣಾಂಕ		cnt = 0;
    ಪೂರ್ಣಾಂಕ		skip = FALSE;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    syl_ಅಂಶ_T	*syl;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (slang->sl_syllable == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಗಾಗಿ (p = ಶಬ್ದ; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ಆದರೆ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    ಮುಂದುವರಿ;
	}

	// Find longest match of syllable items.
	len = 0;
	ಗಾಗಿ (i = 0; i < slang->sl_syl_ವಸ್ತುಗಳು.ga_len; ++i)
	{
	    syl = ((syl_ಅಂಶ_T *)slang->sl_syl_ವಸ್ತುಗಳು.ga_ದತ್ತಾಂಶ) + i;
	    ಆದರೆ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ಆದರೆ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ಆದರೆ (vim_strchr(slang->sl_syllable, c) == ಖಾಲಿ)
		skip = FALSE;	    // No, search for next syllable
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ಹಿಂತಿರುಗಿ cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    ಅಕ್ಷರ *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    ಪೂರ್ಣಾಂಕ		filename;
    ಪೂರ್ಣಾಂಕ		region_ಮುಖವಾಡ;
    slang_T	*slang;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    char_u	*spf;
    char_u	*use_region = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		dont_use_region = FALSE;
    ಪೂರ್ಣಾಂಕ		nobreak = FALSE;
    ಪೂರ್ಣಾಂಕ		i, j;
    langp_T	*lp, *lp2;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;
    ಅಕ್ಷರ	*ret_ಸಂದೇಶ = ಖಾಲಿ;
    char_u	*spl_copy;
    bufref_T	bufref;

    ಗಣ_bufref(&bufref, wp->w_ಬಫರ್);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    recursive = TRUE;

    ga_init2(&ga, ಗಾತ್ರ(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ಆದರೆ (spl_copy == ಖಾಲಿ)
	ಹೋಗು theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    ಗಾಗಿ (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ಖಾಲಿ;

	ಆದರೆ (!valid_spelllang(lang))
	    ಮುಂದುವರಿ;

	ಆದರೆ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    ಮುಂದುವರಿ;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ಆದರೆ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ಆದರೆ (p != ಖಾಲಿ && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ಆದರೆ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    filename = FALSE;
	    ಆದರೆ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ಆದರೆ (STRICMP(lang, slang->sl_name) == 0)
		    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (region != ಖಾಲಿ)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ಆದರೆ (use_region != ಖಾಲಿ && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ಆದರೆ (slang == ಖಾಲಿ)
	{
	    ಆದರೆ (filename)
		(ಶೂನ್ಯ)spell_ತುಂಬು_file(lang, lang, ಖಾಲಿ, FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		spell_ತುಂಬು_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ಆದರೆ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_ಸಂದೇಶ = N_(e_spellfilemising_autocommand_deleted_ಬಫರ್);
		    ಹೋಗು theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    ಆದರೆ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_ಮುಖವಾಡ = REGION_ALL;
		ಆದರೆ (!filename && region != ಖಾಲಿ)
		{
		    // find region in sl_regions
		    c = ಕಂಡುಹಿಡಿ_region(slang->sl_regions, region);
		    ಆದರೆ (c == REGION_ALL)
		    {
			ಆದರೆ (slang->sl_ಕೂಡಿಸು)
			{
			    ಆದರೆ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_ಮುಖವಾಡ = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			region_ಮುಖವಾಡ = 1 << c;
		}

		ಆದರೆ (region_ಮುಖವಾಡ != 0)
		{
		    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_ಸಂದೇಶ = e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
			ಹೋಗು theend;
		    }
		    LANGP_ನಮೂದು(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ನಮೂದು(ga, ga.ga_len)->lp_region = region_ಮುಖವಾಡ;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ಆದರೆ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    ಗಾಗಿ (ಸುತ್ತು = 0; ಸುತ್ತು == 0 || *spf != NUL; ++ಸುತ್ತು)
    {
	ಆದರೆ (ಸುತ್ತು == 0)
	{
	    // Internal wordlist, if there is one.
	    ಆದರೆ (int_wordlist == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    int_wordlist_spl(spf_name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    ಗಾಗಿ (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ನಮೂದು(ga, c)->lp_slang->sl_fname;
		ಆದರೆ (p != ಖಾಲಿ && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (c < ga.ga_len)
		ಮುಂದುವರಿ;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ಆದರೆ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (slang == ಖಾಲಿ)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ಆದರೆ (ಸುತ್ತು == 0)
		STRCPY(lang, "internal wordlist");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ಆದರೆ (p != ಖಾಲಿ)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_ತುಂಬು_file(spf_name, lang, ಖಾಲಿ, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ಆದರೆ (slang != ಖಾಲಿ && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ಆದರೆ (slang != ಖಾಲಿ && ga_grow(&ga, 1) == OK)
	{
	    region_ಮುಖವಾಡ = REGION_ALL;
	    ಆದರೆ (use_region != ಖಾಲಿ && !dont_use_region)
	    {
		// find region in sl_regions
		c = ಕಂಡುಹಿಡಿ_region(slang->sl_regions, use_region);
		ಆದರೆ (c != REGION_ALL)
		    region_ಮುಖವಾಡ = 1 << c;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_ಮುಖವಾಡ = 0;
	    }

	    ಆದರೆ (region_ಮುಖವಾಡ != 0)
	    {
		LANGP_ನಮೂದು(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ನಮೂದು(ga, ga.ga_len)->lp_sallang = ಖಾಲಿ;
		LANGP_ನಮೂದು(ga, ga.ga_len)->lp_replang = ಖಾಲಿ;
		LANGP_ನಮೂದು(ga, ga.ga_len)->lp_region = region_ಮುಖವಾಡ;
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
    ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ನಮೂದು(ga, i);

	// sound folding
	ಆದರೆ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	ಇಲ್ಲದಿದ್ದರೆ
	    // find first similar language that does sound folding
	    ಗಾಗಿ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ನಮೂದು(ga, j);
		ಆದರೆ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	// REP items
	ಆದರೆ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	ಇಲ್ಲದಿದ್ದರೆ
	    // find first similar language that has REP items
	    ಗಾಗಿ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ನಮೂದು(ga, j);
		ಆದರೆ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    ನಿಲ್ಲಿಸು;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ಹಿಂತಿರುಗಿ ret_ಸಂದೇಶ;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_midword(win_T *wp)
{
    CLEAR_ಕ್ಷೇತ್ರ(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    ಆದರೆ (lp->sl_midword == ಖಾಲಿ)	    // there aren't any
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (p = lp->sl_midword; *p != NUL; )
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ಆದರೆ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_s->b_spell_ismw_mb == ಖಾಲಿ)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (ಪೂರ್ಣಾಂಕ)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ಆದರೆ (bp != ಖಾಲಿ)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_region(char_u *rp, char_u *region)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; ; i += 2)
    {
	ಆದರೆ (rp[i] == NUL)
	    ಹಿಂತಿರುಗಿ REGION_ALL;
	ಆದರೆ (rp[i] == region[0] && rp[i + 1] == region[1])
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    ಪೂರ್ಣಾಂಕ
captype(
    char_u	*ಶಬ್ದ,
    char_u	*ಕೊನೆ)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		firstcap;
    ಪೂರ್ಣಾಂಕ		allcap;
    ಪೂರ್ಣಾಂಕ		past_ಎರಡನೇ = FALSE;	// past second word char

    // find first letter
    ಗಾಗಿ (p = ಶಬ್ದ; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ಆದರೆ (ಕೊನೆ == ಖಾಲಿ ? *p == NUL : p >= ಕೊನೆ)
	    ಹಿಂತಿರುಗಿ 0;	    // only non-word characters, illegal word
    ಆದರೆ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    ಇಲ್ಲದಿದ್ದರೆ
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    ಗಾಗಿ ( ; ಕೊನೆ == ಖಾಲಿ ? *p != NUL : p < ಕೊನೆ; MB_PTR_ADV(p))
	ಆದರೆ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ಆದರೆ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ಆದರೆ (past_ಎರಡನೇ && allcap)
		    ಹಿಂತಿರುಗಿ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!allcap)
		// UlU -> KEEPCAP
		ಹಿಂತಿರುಗಿ WF_KEEPCAP;
	    past_ಎರಡನೇ = TRUE;
	}

    ಆದರೆ (allcap)
	ಹಿಂತಿರುಗಿ WF_ALLCAP;
    ಆದರೆ (firstcap)
	ಹಿಂತಿರುಗಿ WF_ONECAP;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    ಶೂನ್ಯ
spell_delete_wordlist(ಶೂನ್ಯ)
{
    char_u	fname[MAXPATHL];

    ಆದರೆ (int_wordlist == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    ಶೂನ್ಯ
spell_free_all(ಶೂನ್ಯ)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    ಆಗ (ಮೊದಲ_lang != ಖಾಲಿ)
    {
	slang = ಮೊದಲ_lang;
	ಮೊದಲ_lang = slang->sl_ಮುಂದಿನ;
	slang_free(slang);
    }

    spell_delete_wordlist();

    VIM_CLEAR(repl_ಗಮ್ಯ);
    VIM_CLEAR(repl_from);
}

/*
 * Clear all spelling tables and reload them.
 * Used after 'encoding' is set and when ":mkspell" was used.
 */
    ಶೂನ್ಯ
spell_reload(ಶೂನ್ಯ)
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
	ಆದರೆ (*wp->w_s->b_p_spl != NUL)
	{
		ಆದರೆ (wp->w_p_spell)
		{
		    (ಶೂನ್ಯ)parse_spelllang(wp);
		    ನಿಲ್ಲಿಸು;
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
open_spellbuf(ಶೂನ್ಯ)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    buf->b_p_ಕೀಲಿ = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Close the buffer used for spell info.
 */
    ಶೂನ್ಯ
close_spellbuf(buf_T *buf)
{
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    ಶೂನ್ಯ
clear_spell_chartab(spelltab_T *sp)
{
    ಪೂರ್ಣಾಂಕ		i;

    // Init everything to FALSE (zero).
    CLEAR_ಕ್ಷೇತ್ರ(sp->st_isw);
    CLEAR_ಕ್ಷೇತ್ರ(sp->st_isu);
    ಗಾಗಿ (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_ಊರ್ಧ್ವ[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    ಗಾಗಿ (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    ಗಾಗಿ (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    ಗಾಗಿ (i = 'a'; i <= 'z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_ಊರ್ಧ್ವ[i] = i - 0x20;
    }
}

/*
 * Init the chartab used for spelling.  Only depends on 'encoding'.
 * Called once while starting up and when 'encoding' changes.
 * The default is to use isalpha(), but the spell file should define the word
 * characters to make it possible that 'encoding' differs from the current
 * locale.  For utf-8 we don't use isalpha() but our own functions.
 */
    ಶೂನ್ಯ
init_spell_chartab(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;

    did_ಗಣ_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ಆದರೆ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	ಗಾಗಿ (i = 128; i <= 255; ++i)
	    ಆದರೆ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
    {
	ಗಾಗಿ (i = 128; i < 256; ++i)
	{
	    ಪೂರ್ಣಾಂಕ f = utf_fold(i);
	    ಪೂರ್ಣಾಂಕ u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_ಊರ್ಧ್ವ[i] = (u < 256) ? u : i;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Rough guess: use locale-dependent library functions.
	ಗಾಗಿ (i = 128; i < 256; ++i)
	{
	    ಆದರೆ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_ISLOWER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_ಊರ್ಧ್ವ[i] = MB_TOUPPER(i);
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
    ಪೂರ್ಣಾಂಕ
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ಆದರೆ (l == 1)
	{
	    // be quick for ASCII
	    ಆದರೆ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    c = mb_ptr2char(p);
	    ಆದರೆ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ಖಾಲಿ
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ಖಾಲಿ))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ಆದರೆ (c > 255)
	    ಹಿಂತಿರುಗಿ spell_mb_isword_class(mb_get_class(s), wp);
	ಹಿಂತಿರುಗಿ spelltab.st_isw[c];
    }

    ಹಿಂತಿರುಗಿ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    ಪೂರ್ಣಾಂಕ
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (has_mbyte)
    {
	c = mb_ptr2char(p);
	ಆದರೆ (c > 255)
	    ಹಿಂತಿರುಗಿ spell_mb_isword_class(mb_get_class(p), wp);
	ಹಿಂತಿರುಗಿ spelltab.st_isw[c];
    }
    ಹಿಂತಿರುಗಿ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
spell_mb_isword_class(ಪೂರ್ಣಾಂಕ cl, win_T *wp)
{
    ಆದರೆ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ಹಿಂತಿರುಗಿ cl == 2 || cl == 0x2800;
    ಹಿಂತಿರುಗಿ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
spell_iswordp_w(ಪೂರ್ಣಾಂಕ *p, win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		*s;

    ಆದರೆ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ಖಾಲಿ
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ಖಾಲಿ))
	s = p + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	s = p;

    ಆದರೆ (*s > 255)
    {
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ spell_mb_isword_class(utf_class(*s), wp);
	ಆದರೆ (enc_dbcs)
	    ಹಿಂತಿರುಗಿ spell_mb_isword_class(
				dbcs_class((ಚಿಹ್ನೆರಹಿತ)*s >> 8, *s & 0xff), wp);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    ಪೂರ್ಣಾಂಕ
spell_casefold(
    win_T	*wp,
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		len,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		buflen)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (len >= buflen)
    {
	buf[0] = NUL;
	ಹಿಂತಿರುಗಿ FAIL;		// result will not fit
    }

    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ	outi = 0;
	char_u	*p;
	ಪೂರ್ಣಾಂಕ	c;

	// Fold one character at a time.
	ಗಾಗಿ (p = str; p < str + len; )
	{
	    ಆದರೆ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ಆದರೆ (c == 0x03a3 || c == 0x03c2)
	    {
		ಆದರೆ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		ಇಲ್ಲದಿದ್ದರೆ
		    c = 0x03c3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Be quick for non-multibyte encodings.
	ಗಾಗಿ (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    ಪೂರ್ಣಾಂಕ
check_need_cap(win_T *wp, linenr_T lnum, colnr_T ಲಂಬಸಾಲು)
{
    ಆದರೆ (wp->w_s->b_cap_prog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ		need_cap = FALSE;
    char_u	*ಸಾಲು = ಲಂಬಸಾಲು ? ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE) : ಖಾಲಿ;
    char_u	*ಸಾಲು_copy = ಖಾಲಿ;
    colnr_T	endcol = 0;

    ಆದರೆ (ಲಂಬಸಾಲು == 0 || getwhitecols(ಸಾಲು) >= ಲಂಬಸಾಲು)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ಆದರೆ (lnum == 1)
	    need_cap = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum - 1, FALSE);
	    ಆದರೆ (*skipwhite(ಸಾಲು) == NUL)
		need_cap = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Append a space in place of the line break.
		ಸಾಲು_copy = concat_str(ಸಾಲು, (char_u *)" ");
		ಆದರೆ (ಸಾಲು_copy == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FALSE;

		ಸಾಲು = ಸಾಲು_copy;
		endcol = (colnr_T)STRLEN(ಸಾಲು);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	endcol = ಲಂಬಸಾಲು;

    ಆದರೆ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = ಸಾಲು + endcol;
	ಗಾಗಿ (;;)
	{
	    MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
	    ಆದರೆ (p == ಸಾಲು || spell_iswordp_nmw(p, wp))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == ಸಾಲು + endcol)
	    {
		need_cap = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(ಸಾಲು_copy);

    ಹಿಂತಿರುಗಿ need_cap;
}


/*
 * ":spellrepall"
 */
    ಶೂನ್ಯ
ex_spellrepall(exarg_T *eap ಬಳಸದ)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*ಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ಆದರೆ (repl_from == ಖಾಲಿ || repl_ಗಮ್ಯ == ಖಾಲಿ)
    {
	emsg(_(e_no_ಹಿಂದಿನ_spell_replacement));
	ಹಿಂತಿರುಗಿ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_ಗಮ್ಯ_len = STRLEN(repl_ಗಮ್ಯ);
    ದೀರ್ಘ	addlen = (ದೀರ್ಘ)repl_ಗಮ್ಯ_len - (ದೀರ್ಘ)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ಆದರೆ (frompat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    frompatlen = vim_snprintf((ಅಕ್ಷರ *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ಆಗ (!got_int)
    {
	ಆದರೆ (do_ಹುಡುಕು(ಖಾಲಿ, '/', '/', frompat, frompatlen, 1L, ಹುಡುಕು_KEEP, ಖಾಲಿ) == 0
						   || u_save_cursor() == FAIL)
	    ನಿಲ್ಲಿಸು;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	ಸಾಲು = ml_get_curline();
	ಆದರೆ (addlen <= 0 || STRNCMP(ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು,
						   repl_ಗಮ್ಯ, repl_ಗಮ್ಯ_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ಆದರೆ (p == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    mch_memmove(p, ಸಾಲು, curwin->w_cursor.ಲಂಬಸಾಲು);
	    STRCPY(p + curwin->w_cursor.ಲಂಬಸಾಲು, repl_ಗಮ್ಯ);
	    STRCAT(p, ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curwin->w_cursor.lnum, curwin->w_cursor.ಲಂಬಸಾಲು);
#ಆದರೆ defined(FEAT_PROP_POPUP)
	    ಆದರೆ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.ಲಂಬಸಾಲು, (ಪೂರ್ಣಾಂಕ)addlen, APC_SUBSTITUTE);
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (curwin->w_cursor.lnum != prev_lnum)
	    {
		++sub_nlines;
		prev_lnum = curwin->w_cursor.lnum;
	    }
	    ++sub_nsubs;
	}
	curwin->w_cursor.ಲಂಬಸಾಲು += (colnr_T)repl_ಗಮ್ಯ_len;
    }

    p_ws = save_ws;
    curwin->w_cursor = pos;
    vim_free(frompat);

    ಆದರೆ (sub_nsubs == 0)
	semsg(_(e_not_ಸಿಕ್ಕಿತು_str), repl_from);
    ಇಲ್ಲದಿದ್ದರೆ
	do_sub_ಸಂದೇಶ(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    ಶೂನ್ಯ
onecap_copy(
    char_u	*ಶಬ್ದ,
    char_u	*wcopy,
    ಪೂರ್ಣಾಂಕ		ಊರ್ಧ್ವ_2)	    // TRUE: first letter made upper case
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		l;

    p = ಶಬ್ದ;
    ಆದರೆ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    ಇಲ್ಲದಿದ್ದರೆ
	c = *p++;
    ಆದರೆ (ಊರ್ಧ್ವ_2)
	c = SPELL_TOUPPER(c);
    ಇಲ್ಲದಿದ್ದರೆ
	c = SPELL_TOFOLD(c);
    ಆದರೆ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಶೂನ್ಯ
allcap_copy(char_u *ಶಬ್ದ, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    ಪೂರ್ಣಾಂಕ		c;

    d = wcopy;
    ಗಾಗಿ (s = ಶಬ್ದ; *s != NUL; )
    {
	ಆದರೆ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ಆದರೆ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ಆದರೆ (d - wcopy >= MAXWLEN - 1)
		ನಿಲ್ಲಿಸು;
	    *d++ = c;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    c = SPELL_TOUPPER(c);

	ಆದರೆ (has_mbyte)
	{
	    ಆದರೆ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		ನಿಲ್ಲಿಸು;
	    d += mb_char2bytes(c, d);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (d - wcopy >= MAXWLEN - 1)
		ನಿಲ್ಲಿಸು;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    ಪೂರ್ಣಾಂಕ
nofold_len(char_u *fword, ಪೂರ್ಣಾಂಕ flen, char_u *ಶಬ್ದ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i = 0;

    ಗಾಗಿ (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    ಗಾಗಿ (p = ಶಬ್ದ; i > 0; MB_PTR_ADV(p))
	--i;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(p - ಶಬ್ದ);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    ಶೂನ್ಯ
make_case_ಶಬ್ದ(char_u *fword, char_u *cword, ಪೂರ್ಣಾಂಕ flags)
{
    ಆದರೆ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *ಶಬ್ದ)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		lpi;

    ಆದರೆ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	    ಆದರೆ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, ಶಬ್ದ, FALSE, sound);
		ಹಿಂತಿರುಗಿ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ಹಿಂತಿರುಗಿ vim_strsave(ಶಬ್ದ);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಶೂನ್ಯ
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    ಪೂರ್ಣಾಂಕ		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*ಶಬ್ದ;

    ಆದರೆ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// SAL items used.  Requires the word to be case-folded.
	ಆದರೆ (folded)
	    ಶಬ್ದ = inword;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    (ಶೂನ್ಯ)spell_casefold(curwin,
				  inword, (ಪೂರ್ಣಾಂಕ)STRLEN(inword), fword, MAXWLEN);
	    ಶಬ್ದ = fword;
	}

	ಆದರೆ (has_mbyte)
	    spell_soundfold_wsal(slang, ಶಬ್ದ, res);
	ಇಲ್ಲದಿದ್ದರೆ
	    spell_soundfold_sal(slang, ಶಬ್ದ, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		ri = 0;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ	prevc = 0;
	ಪೂರ್ಣಾಂಕ	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	ಗಾಗಿ (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ಆದರೆ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < 256)
		c = slang->sl_sal_ಮೊದಲ[c];
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ip = ((ಪೂರ್ಣಾಂಕ **)slang->sl_sal.ga_ದತ್ತಾಂಶ)[c & 0xff];
		ಆದರೆ (ip == ಖಾಲಿ)		// empty list, can't match
		    c = NUL;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಗಾಗಿ (;;)		// find "c" in the list
		    {
			ಆದರೆ (*ip == 0)	// not found
			{
			    c = NUL;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (*ip == c)	// match!
			{
			    c = ip[1];
			    ನಿಲ್ಲಿಸು;
			}
			ip += 2;
		    }
	    }

	    ಆದರೆ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ಆದರೆ (ri + MB_MAXBYTES > MAXWLEN)
		    ನಿಲ್ಲಿಸು;
		prevc = c;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// The sl_sal_first[] table contains the translation.
	ಗಾಗಿ (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ಆದರೆ (VIM_ISWHITE(c))
		c = ' ';
	    ಇಲ್ಲದಿದ್ದರೆ
		c = slang->sl_sal_ಮೊದಲ[c];
	    ಆದರೆ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	ಶಬ್ದ[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    ಪೂರ್ಣಾಂಕ		i, j, z;
    ಪೂರ್ಣಾಂಕ		reslen;
    ಪೂರ್ಣಾಂಕ		n, k = 0;
    ಪೂರ್ಣಾಂಕ		z0;
    ಪೂರ್ಣಾಂಕ		k0;
    ಪೂರ್ಣಾಂಕ		n0;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		pri;
    ಪೂರ್ಣಾಂಕ		p0 = -333;
    ಪೂರ್ಣಾಂಕ		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ಆದರೆ (slang->sl_rem_accents)
    {
	t = ಶಬ್ದ;
	ಆಗ (*s != NUL)
	{
	    ಆದರೆ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_strncpy(ಶಬ್ದ, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_ದತ್ತಾಂಶ;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ಆಗ ((c = ಶಬ್ದ[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_ಮೊದಲ[c];
	z0 = 0;

	ಆದರೆ (n >= 0)
	{
	    // check all rules for the same letter
	    ಗಾಗಿ (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ಆದರೆ (k > 1)
		{
		    ಆದರೆ (ಶಬ್ದ[i + 1] != s[1])
			ಮುಂದುವರಿ;
		    ಆದರೆ (k > 2)
		    {
			ಗಾಗಿ (j = 2; j < k; ++j)
			    ಆದರೆ (ಶಬ್ದ[i + j] != s[j])
				ನಿಲ್ಲಿಸು;
			ಆದರೆ (j < k)
			    ಮುಂದುವರಿ;
		    }
		}

		ಆದರೆ ((pf = smp[n].sm_oneof) != ಖಾಲಿ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ಆಗ (*pf != NUL && *pf != ಶಬ್ದ[i + k])
			++pf;
		    ಆದರೆ (*pf == NUL)
			ಮುಂದುವರಿ;
		    ++k;
		}
		s = smp[n].sm_ನಿಯಮಗಳು;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ಆಗ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ಆದರೆ (*s == '<')
		    s++;
		ಆದರೆ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ಆದರೆ (*s == '^' && *(s + 1) == '^')
		    s++;

		ಆದರೆ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(ಶಬ್ದ[i - 1] == ' '
				      || spell_iswordp(ಶಬ್ದ + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp(ಶಬ್ದ + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp(ಶಬ್ದ + i - 1, curwin)
			    && (!spell_iswordp(ಶಬ್ದ + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = ಶಬ್ದ[i + k - 1];
		    n0 = slang->sl_sal_ಮೊದಲ[c0];

		    ಆದರೆ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && ಶಬ್ದ[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			ಗಾಗಿ ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ಆದರೆ (k0 > 1)
			    {
				ಆದರೆ (ಶಬ್ದ[i + k] != s[1])
				    ಮುಂದುವರಿ;
				ಆದರೆ (k0 > 2)
				{
				    pf = ಶಬ್ದ + i + k + 1;
				    ಗಾಗಿ (j = 2; j < k0; ++j)
					ಆದರೆ (*pf++ != s[j])
					    ನಿಲ್ಲಿಸು;
				    ಆದರೆ (j < k0)
					ಮುಂದುವರಿ;
				}
			    }
			    k0 += k - 1;

			    ಆದರೆ ((pf = smp[n0].sm_oneof) != ಖಾಲಿ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ಆಗ (*pf != NUL && *pf != ಶಬ್ದ[i + k0])
				    ++pf;
				ಆದರೆ (*pf == NUL)
				    ಮುಂದುವರಿ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_ನಿಯಮಗಳು;
			    ಆಗ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ಆದರೆ (*s == '<')
				s++;
			    ಆದರೆ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ಆದರೆ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(ಶಬ್ದ + i + k0,
								     curwin)))
			    {
				ಆದರೆ (k0 == k)
				    // this is just a piece of the string
				    ಮುಂದುವರಿ;

				ಆದರೆ (p0 < pri)
				    // priority too low
				    ಮುಂದುವರಿ;
				// rule fits; stop search
				ನಿಲ್ಲಿಸು;
			    }
			}

			ಆದರೆ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    ಮುಂದುವರಿ;
		    }

		    // replace string
		    s = smp[n].sm_ಗಮ್ಯ;
		    ಆದರೆ (s == ಖಾಲಿ)
			s = (char_u *)"";
		    pf = smp[n].sm_ನಿಯಮಗಳು;
		    p0 = (vim_strchr(pf, '<') != ಖಾಲಿ) ? 1 : 0;
		    ಆದರೆ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ಆದರೆ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ಆಗ (*s != NUL && ಶಬ್ದ[i + k0] != NUL)
			{
			    ಶಬ್ದ[i + k0] = *s;
			    k0++;
			    s++;
			}
			ಆದರೆ (k > k0)
			    STRMOVE(ಶಬ್ದ + i + k0, ಶಬ್ದ + i + k);

			// new "actual letter"
			c = ಶಬ್ದ[i];
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ಆಗ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ಆದರೆ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)pf, "^^") != ಖಾಲಿ)
			{
			    ಆದರೆ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(ಶಬ್ದ, ಶಬ್ದ + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ಆದರೆ (z0 == 0)
	{
	    ಆದರೆ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		ಶಬ್ದ[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		wres[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		l;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		*ws;
    char_u	*t;
    ಪೂರ್ಣಾಂಕ		*pf;
    ಪೂರ್ಣಾಂಕ		i, j, z;
    ಪೂರ್ಣಾಂಕ		reslen;
    ಪೂರ್ಣಾಂಕ		n, k = 0;
    ಪೂರ್ಣಾಂಕ		z0;
    ಪೂರ್ಣಾಂಕ		k0;
    ಪೂರ್ಣಾಂಕ		n0;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		pri;
    ಪೂರ್ಣಾಂಕ		p0 = -333;
    ಪೂರ್ಣಾಂಕ		c0;
    ಪೂರ್ಣಾಂಕ		did_white = FALSE;
    ಪೂರ್ಣಾಂಕ		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    ಗಾಗಿ (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ಆದರೆ (slang->sl_rem_accents)
	{
	    ಆದರೆ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ಆದರೆ (did_white)
		    ಮುಂದುವರಿ;
		c = ' ';
		did_white = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		did_white = FALSE;
		ಆದರೆ (!spell_iswordp_nmw(t, curwin))
		    ಮುಂದುವರಿ;
	    }
	}
	ಶಬ್ದ[wordlen++] = c;
    }
    ಶಬ್ದ[wordlen] = NUL;

    /*
     * This algorithm comes from Aspell phonet.cpp.
     * Converted from C++ to C.  Added support for multi-byte chars.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ಆಗ ((c = ಶಬ್ದ[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_ಮೊದಲ[c & 0xff];
	z0 = 0;

	ಆದರೆ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    ಗಾಗಿ (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ಆದರೆ (c != ws[0])
		    ಮುಂದುವರಿ;
		k = smp[n].sm_leadlen;
		ಆದರೆ (k > 1)
		{
		    ಆದರೆ (ಶಬ್ದ[i + 1] != ws[1])
			ಮುಂದುವರಿ;
		    ಆದರೆ (k > 2)
		    {
			ಗಾಗಿ (j = 2; j < k; ++j)
			    ಆದರೆ (ಶಬ್ದ[i + j] != ws[j])
				ನಿಲ್ಲಿಸು;
			ಆದರೆ (j < k)
			    ಮುಂದುವರಿ;
		    }
		}

		ಆದರೆ ((pf = smp[n].sm_oneof_w) != ಖಾಲಿ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ಆಗ (*pf != NUL && *pf != ಶಬ್ದ[i + k])
			++pf;
		    ಆದರೆ (*pf == NUL)
			ಮುಂದುವರಿ;
		    ++k;
		}
		s = smp[n].sm_ನಿಯಮಗಳು;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ಆಗ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ಆದರೆ (*s == '<')
		    s++;
		ಆದರೆ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ಆದರೆ (*s == '^' && *(s + 1) == '^')
		    s++;

		ಆದರೆ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(ಶಬ್ದ[i - 1] == ' '
				    || spell_iswordp_w(ಶಬ್ದ + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp_w(ಶಬ್ದ + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp_w(ಶಬ್ದ + i - 1, curwin)
			    && (!spell_iswordp_w(ಶಬ್ದ + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = ಶಬ್ದ[i + k - 1];
		    n0 = slang->sl_sal_ಮೊದಲ[c0 & 0xff];

		    ಆದರೆ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && ಶಬ್ದ[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			ಗಾಗಿ ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ಆದರೆ (c0 != ws[0])
				ಮುಂದುವರಿ;
			    k0 = smp[n0].sm_leadlen;
			    ಆದರೆ (k0 > 1)
			    {
				ಆದರೆ (ಶಬ್ದ[i + k] != ws[1])
				    ಮುಂದುವರಿ;
				ಆದರೆ (k0 > 2)
				{
				    pf = ಶಬ್ದ + i + k + 1;
				    ಗಾಗಿ (j = 2; j < k0; ++j)
					ಆದರೆ (*pf++ != ws[j])
					    ನಿಲ್ಲಿಸು;
				    ಆದರೆ (j < k0)
					ಮುಂದುವರಿ;
				}
			    }
			    k0 += k - 1;

			    ಆದರೆ ((pf = smp[n0].sm_oneof_w) != ಖಾಲಿ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ಆಗ (*pf != NUL && *pf != ಶಬ್ದ[i + k0])
				    ++pf;
				ಆದರೆ (*pf == NUL)
				    ಮುಂದುವರಿ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_ನಿಯಮಗಳು;
			    ಆಗ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ಆದರೆ (*s == '<')
				s++;
			    ಆದರೆ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ಆದರೆ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(ಶಬ್ದ + i + k0,
								     curwin)))
			    {
				ಆದರೆ (k0 == k)
				    // this is just a piece of the string
				    ಮುಂದುವರಿ;

				ಆದರೆ (p0 < pri)
				    // priority too low
				    ಮುಂದುವರಿ;
				// rule fits; stop search
				ನಿಲ್ಲಿಸು;
			    }
			}

			ಆದರೆ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    ಮುಂದುವರಿ;
		    }

		    // replace string
		    ws = smp[n].sm_ಗಮ್ಯ_w;
		    s = smp[n].sm_ನಿಯಮಗಳು;
		    p0 = (vim_strchr(s, '<') != ಖಾಲಿ) ? 1 : 0;
		    ಆದರೆ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ಆದರೆ (reslen > 0 && ws != ಖಾಲಿ && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ಆದರೆ (ws != ಖಾಲಿ)
			    ಆಗ (*ws != NUL && ಶಬ್ದ[i + k0] != NUL)
			    {
				ಶಬ್ದ[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ಆದರೆ (k > k0)
			    mch_memmove(ಶಬ್ದ + i + k0, ಶಬ್ದ + i + k,
				    ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = ಶಬ್ದ[i];
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ಆದರೆ (ws != ಖಾಲಿ)
			    ಆಗ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ಆದರೆ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ಆದರೆ (ws == ಖಾಲಿ)
			    c = NUL;
			ಇಲ್ಲದಿದ್ದರೆ
			    c = *ws;
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)s, "^^") != ಖಾಲಿ)
			{
			    ಆದರೆ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(ಶಬ್ದ, ಶಬ್ದ + i + 1,
				       ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ಆದರೆ (z0 == 0)
	{
	    ಆದರೆ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    ಗಾಗಿ (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ಆದರೆ (l + MB_MAXBYTES > MAXWLEN)
	    ನಿಲ್ಲಿಸು;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    ಶೂನ್ಯ
ex_spellinfo(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		lpi;
    langp_T	*lp;
    char_u	*p;

    ಆದರೆ (no_spell_checking(curwin))
	ಹಿಂತಿರುಗಿ;

    ಸಂದೇಶ_ಆರಂಭ();
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	ಸಂದೇಶ_puts("file: ");
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)lp->lp_slang->sl_fname);
	ಸಂದೇಶ_putchar('\n');
	p = lp->lp_slang->sl_info;
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
	    ಸಂದೇಶ_putchar('\n');
	}
    }
    ಸಂದೇಶ_ಕೊನೆ();
}

#ವ್ಯಾಖ್ಯಾನಿಸು DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#ವ್ಯಾಖ್ಯಾನಿಸು DUMPFLAG_ಎಣಿಕೆ	    2	// include word count
#ವ್ಯಾಖ್ಯಾನಿಸು DUMPFLAG_ICASE	    4	// ignore case when finding matches
#ವ್ಯಾಖ್ಯಾನಿಸು DUMPFLAG_ONECAP	    8	// pattern starts with capital
#ವ್ಯಾಖ್ಯಾನಿಸು DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    ಶೂನ್ಯ
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    ದೀರ್ಘ    dummy;

    ಆದರೆ (no_spell_checking(curwin))
	ಹಿಂತಿರುಗಿ;
    (ಶೂನ್ಯ)get_option_ಮೌಲ್ಯ((char_u*)"spl", &dummy, &spl, ಖಾಲಿ, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ಆದರೆ (!BUFEMPTY())
	ಹಿಂತಿರುಗಿ;

    spell_dump_compl(ಖಾಲಿ, 0, ಖಾಲಿ, eap->forceit ? DUMPFLAG_ಎಣಿಕೆ : 0);

    // Delete the empty line that we started with.
    ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > 1)
	ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    ಶೂನ್ಯ
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    ಪೂರ್ಣಾಂಕ		ic,	    // ignore case
    ಪೂರ್ಣಾಂಕ		*dir,	    // direction for adding matches
    ಪೂರ್ಣಾಂಕ		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		curi[MAXWLEN];
    char_u	ಶಬ್ದ[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		ಆಳ;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		flags;
    char_u	*region_ಹೆಸರುಗಳು = ಖಾಲಿ;	    // region names being used
    ಪೂರ್ಣಾಂಕ		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		lpi;
    ಪೂರ್ಣಾಂಕ		dumpflags = dumpflags_arg;
    ಪೂರ್ಣಾಂಕ		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ಆದರೆ (pat != ಖಾಲಿ)
    {
	ಆದರೆ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    n = captype(pat, ಖಾಲಿ);
	    ಆದರೆ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == WF_ALLCAP && (ಪೂರ್ಣಾಂಕ)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ಆದರೆ (p[0] != 0)
	{
	    ಆದರೆ (region_ಹೆಸರುಗಳು == ಖಾಲಿ)	    // first language with regions
		region_ಹೆಸರುಗಳು = p;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(region_ಹೆಸರುಗಳು, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (do_region && region_ಹೆಸರುಗಳು != ಖಾಲಿ && pat == ಖಾಲಿ)
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "/regions=%s", region_ಹೆಸರುಗಳು);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ಆದರೆ (slang->sl_fbyts == ಖಾಲಿ)	    // reloading failed
	    ಮುಂದುವರಿ;

	ಆದರೆ (pat == ಖಾಲಿ)
	{
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ಆದರೆ (pat != ಖಾಲಿ && slang->sl_pbyts == ಖಾಲಿ)
	    patlen = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
	ಇಲ್ಲದಿದ್ದರೆ
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	{
	    ಆದರೆ (ಸುತ್ತು == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ಆದರೆ (byts == ಖಾಲಿ)
		ಮುಂದುವರಿ;		// array is empty

	    ಆಳ = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ಆಗ (ಆಳ >= 0 && !got_int
				  && (pat == ಖಾಲಿ || !ins_compl_interrupted()))
	    {
		ಆದರೆ (curi[ಆಳ] > byts[arridx[ಆಳ]])
		{
		    // Done all bytes at this node, go up one level.
		    --ಆಳ;
		    ಸಾಲು_breakcheck();
		    ins_compl_check_ಕೀಲಿಗಳು(50, FALSE);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Do one more byte at this node.
		    n = arridx[ಆಳ] + curi[ಆಳ];
		    ++curi[ಆಳ];
		    c = byts[n];
		    ಆದರೆ (c == 0 || ಆಳ >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (ಪೂರ್ಣಾಂಕ)idxs[n];
			ಆದರೆ ((ಸುತ್ತು == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((ಚಿಹ್ನೆರಹಿತ)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    ಶಬ್ದ[ಆಳ] = NUL;
			    ಆದರೆ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (ಚಿಹ್ನೆರಹಿತ)flags >> 24;
			    ಆದರೆ (c == 0 || curi[ಆಳ] == 2)
			    {
				dump_ಶಬ್ದ(slang, ಶಬ್ದ, pat, dir,
						      dumpflags, flags, lnum);
				ಆದರೆ (pat == ಖಾಲಿ)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ಆದರೆ (c != 0)
				lnum = dump_prefixes(slang, ಶಬ್ದ, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Normal char, go one level deeper.
			ಶಬ್ದ[ಆಳ++] = c;
			arridx[ಆಳ] = idxs[n];
			curi[ಆಳ] = 1;

			// Check if this character matches with the pattern.
			// If not skip the whole tree below it.
			// Always ignore case here, dump_word() will check
			// proper case later.  This isn't exactly right when
			// length changes for multi-byte characters with
			// ignore case...
			ಆದರೆ (ಆಳ <= patlen
					&& MB_STRNICMP(ಶಬ್ದ, pat, ಆಳ) != 0)
			    --ಆಳ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
dump_ಶಬ್ದ(
    slang_T	*slang,
    char_u	*ಶಬ್ದ,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		*dir,
    ಪೂರ್ಣಾಂಕ		dumpflags,
    ಪೂರ್ಣಾಂಕ		wordflags,
    linenr_T	lnum)
{
    ಪೂರ್ಣಾಂಕ		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		flags = wordflags;

    ಆದರೆ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ಆದರೆ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ಆದರೆ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_ಶಬ್ದ(ಶಬ್ದ, cword, flags);
	p = cword;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = ಶಬ್ದ;
	ಆದರೆ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(ಶಬ್ದ, ಖಾಲಿ) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ಆದರೆ (pat == ಖಾಲಿ)
    {
	// Add flags and regions after a slash.
	ಆದರೆ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ಆದರೆ (keepcap)
		STRCAT(badword, "=");
	    ಆದರೆ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WF_RARE)
		STRCAT(badword, "?");
	    ಆದರೆ (flags & WF_REGION)
		ಗಾಗಿ (i = 0; i < 7; ++i)
		    ಆದರೆ (flags & (0x10000 << i))
			sprintf((ಅಕ್ಷರ *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ಆದರೆ (dumpflags & DUMPFLAG_ಎಣಿಕೆ)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_ಕಂಡುಹಿಡಿ(&slang->sl_wordcount, tw);
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_ಎಣಿಕೆ);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_ಕೂಡಿಸು_infercase(p, (ಪೂರ್ಣಾಂಕ)STRLEN(p),
					  p_ic, ಖಾಲಿ, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    ಸ್ಥಿರ linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*ಶಬ್ದ,	    // case-folded word
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		*dir,
    ಪೂರ್ಣಾಂಕ		dumpflags,
    ಪೂರ್ಣಾಂಕ		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		curi[MAXWLEN];
    char_u	ಪೂರ್ವಪ್ರತ್ಯಯ[MAXWLEN];
    char_u	ಶಬ್ದ_ಮೇಲೆ[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		has_ಶಬ್ದ_ಮೇಲೆ = FALSE;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    ಪೂರ್ಣಾಂಕ		ಆಳ;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(ಶಬ್ದ);
    ಆದರೆ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(ಶಬ್ದ, ಶಬ್ದ_ಮೇಲೆ, TRUE);
	has_ಶಬ್ದ_ಮೇಲೆ = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ಆದರೆ (byts != ಖಾಲಿ)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	ಆಳ = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ಆಗ (ಆಳ >= 0 && !got_int)
	{
	    n = arridx[ಆಳ];
	    len = byts[n];
	    ಆದರೆ (curi[ಆಳ] > len)
	    {
		// Done all bytes at this node, go up one level.
		--ಆಳ;
		ಸಾಲು_breakcheck();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Do one more byte at this node.
		n += curi[ಆಳ];
		++curi[ಆಳ];
		c = byts[n];
		ಆದರೆ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    ಗಾಗಿ (i = 1; i < len; ++i)
			ಆದರೆ (byts[n + i] != 0)
			    ನಿಲ್ಲಿಸು;
		    curi[ಆಳ] += i - 1;

		    c = valid_ಶಬ್ದ_ಪೂರ್ವಪ್ರತ್ಯಯ(i, n, flags, ಶಬ್ದ, slang, FALSE);
		    ಆದರೆ (c != 0)
		    {
			vim_strncpy(ಪೂರ್ವಪ್ರತ್ಯಯ + ಆಳ, ಶಬ್ದ, MAXWLEN - ಆಳ - 1);
			dump_ಶಬ್ದ(slang, ಪೂರ್ವಪ್ರತ್ಯಯ, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ಆದರೆ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ಆದರೆ (has_ಶಬ್ದ_ಮೇಲೆ)
		    {
			c = valid_ಶಬ್ದ_ಪೂರ್ವಪ್ರತ್ಯಯ(i, n, flags, ಶಬ್ದ_ಮೇಲೆ, slang,
									TRUE);
			ಆದರೆ (c != 0)
			{
			    vim_strncpy(ಪೂರ್ವಪ್ರತ್ಯಯ + ಆಳ, ಶಬ್ದ_ಮೇಲೆ,
							 MAXWLEN - ಆಳ - 1);
			    dump_ಶಬ್ದ(slang, ಪೂರ್ವಪ್ರತ್ಯಯ, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ಆದರೆ (lnum != 0)
				++lnum;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆಳ < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    ಪೂರ್ವಪ್ರತ್ಯಯ[ಆಳ++] = c;
		    arridx[ಆಳ] = idxs[n];
		    curi[ಆಳ] = 1;
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_ಗಮ್ಯ_ಶಬ್ದ_ಕೊನೆ(char_u *ಆರಂಭ, win_T *win)
{
    char_u  *p = ಆರಂಭ;

    ಆಗ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    ಪೂರ್ಣಾಂಕ
spell_ಶಬ್ದ_ಆರಂಭ(ಪೂರ್ಣಾಂಕ startcol)
{
    char_u	*ಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;

    ಆದರೆ (no_spell_checking(curwin))
	ಹಿಂತಿರುಗಿ startcol;

    // Find a word character before "startcol".
    ಸಾಲು = ml_get_curline();
    ಗಾಗಿ (p = ಸಾಲು + startcol; p > ಸಾಲು; )
    {
	MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
	ಆದರೆ (spell_iswordp_nmw(p, curwin))
	    ನಿಲ್ಲಿಸು;
    }

    // Go back to start of the word.
    ಆಗ (p > ಸಾಲು)
    {
	ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
	MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
	ಆದರೆ (!spell_iswordp(p, curwin))
	    ನಿಲ್ಲಿಸು;
	ಲಂಬಸಾಲು = 0;
    }

    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spell_expand_need_cap;

    ಶೂನ್ಯ
spell_expand_check_cap(colnr_T ಲಂಬಸಾಲು)
{
    spell_expand_need_cap = check_need_cap(curwin, curwin->w_cursor.lnum, ಲಂಬಸಾಲು);
}

/*
 * Get list of spelling suggestions.
 * Used for Insert mode completion CTRL-X ?.
 * Returns the number of matches.  The matches are in "matchp[]", array of
 * allocated strings.
 */
    ಪೂರ್ಣಾಂಕ
expand_spelling(
    linenr_T	lnum ಬಳಸದ,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_ದತ್ತಾಂಶ;
    ಹಿಂತಿರುಗಿ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    ಪೂರ್ಣಾಂಕ
valid_spelllang(char_u *val)
{
    ಹಿಂತಿರುಗಿ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    ಪೂರ್ಣಾಂಕ
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		l;

    spf = val;
    ಆಗ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ಆದರೆ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ಹಿಂತಿರುಗಿ FALSE;
	ಗಾಗಿ (s = spf_name; *s != NUL; ++s)
	    ಆದರೆ (!vim_is_fname_char(*s))
		ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spell_option(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ    *errmsg = ಖಾಲಿ;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    ನಿಲ್ಲಿಸು;
	}
    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    ಅಕ್ಷರ *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    ಆದರೆ (synblock->b_p_spc == ಖಾಲಿ || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ಆದರೆ (re != ಖಾಲಿ)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ಆದರೆ (synblock->b_cap_prog == ಖಾಲಿ)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    }
	}
    }

    vim_regfree(rp);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ  // FEAT_SPELL
