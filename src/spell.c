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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_SPELL)

#ಅನ್ತರ್ಭಾವ <time.h>

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ matchinf_S
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
    ಪೂರ್ಣಾಙ್ಕ		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    ಪೂರ್ಣಾಙ್ಕ		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    ಪೂರ್ಣಾಙ್ಕ		mi_prefcnt;		// number of entries at mi_prefarridx
    ಪೂರ್ಣಾಙ್ಕ		mi_prefixlen;		// byte length of prefix
    ಪೂರ್ಣಾಙ್ಕ		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    ಪೂರ್ಣಾಙ್ಕ		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    ಪೂರ್ಣಾಙ್ಕ		mi_complen;		// nr of compound words used
    ಪೂರ್ಣಾಙ್ಕ		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    ಪೂರ್ಣಾಙ್ಕ		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    ಪೂರ್ಣಾಙ್ಕ		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    ಪೂರ್ಣಾಙ್ಕ		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spell_mb_isword_class(ಪೂರ್ಣಾಙ್ಕ cl, win_T *wp);

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

ಸ್ಥಿರ ಶೂನ್ಯ find_word(matchinf_T *mip, ಪೂರ್ಣಾಙ್ಕ mode);
ಸ್ಥಿರ ಶೂನ್ಯ find_prefix(matchinf_T *mip, ಪೂರ್ಣಾಙ್ಕ mode);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fold_more(matchinf_T *mip);
ಸ್ಥಿರ ಶೂನ್ಯ spell_load_cb(char_u *fname, ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ_syllables(slang_T *slang, char_u *word);
ಸ್ಥಿರ ಶೂನ್ಯ clear_midword(win_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ use_midword(slang_T *lp, win_T *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_region(char_u *rp, char_u *region);
ಸ್ಥಿರ ಶೂನ್ಯ spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
ಸ್ಥಿರ ಶೂನ್ಯ spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
ಸ್ಥಿರ ಶೂನ್ಯ spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
ಸ್ಥಿರ ಶೂನ್ಯ dump_word(slang_T *slang, char_u *word, char_u *pat, ಪೂರ್ಣಾಙ್ಕ *dir, ಪೂರ್ಣಾಙ್ಕ round, ಪೂರ್ಣಾಙ್ಕ flags, linenr_T lnum);
ಸ್ಥಿರ linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, ಪೂರ್ಣಾಙ್ಕ *dir, ಪೂರ್ಣಾಙ್ಕ round, ಪೂರ್ಣಾಙ್ಕ flags, linenr_T startlnum);
ಸ್ಥಿರ char_u *advance_camelcase_word(char_u *p, win_T *wp, ಪೂರ್ಣಾಙ್ಕ *is_camel_case);

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
    ಪೂರ್ಣಾಙ್ಕ
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    ಪೂರ್ಣಾಙ್ಕ		*capcol,	// column to check for Capital
    ಪೂರ್ಣಾಙ್ಕ		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    ಪೂರ್ಣಾಙ್ಕ		nrlen = 0;	// found a number first
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		wrongcaplen = 0;
    ಪೂರ್ಣಾಙ್ಕ		lpi;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ_word = docount;
    ಪೂರ್ಣಾಙ್ಕ		use_camel_case = *wp->w_s->b_p_spo != NUL;
    ಪೂರ್ಣಾಙ್ಕ		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ಯದಿ (*ptr <= ' ')
	ಪ್ರತ್ಯಾಗಮ 1;

    // Return here when loading language files failed.
    ಯದಿ (wp->w_s->b_langp.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ಯದಿ (*ptr >= '0' && *ptr <= '9')
    {
	ಯದಿ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	ಅನ್ಯಥಾ ಯದಿ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	ಅನ್ಯಥಾ
	    mi.mi_end = skipdigits(ptr);
	nrlen = (ಪೂರ್ಣಾಙ್ಕ)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    ಯದಿ (spell_iswordp(mi.mi_fend, wp))
    {
	ಯದಿ (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	ಅನ್ಯಥಾ
	{
	    ಕುರು
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ಯಾವತ್ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ಯದಿ (capcol != ರಿಕ್ತ && *capcol == 0 && wp->w_s->b_cap_prog != ರಿಕ್ತ)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ಯದಿ (!SPELL_ISUPPER(c))
		wrongcaplen = (ಪೂರ್ಣಾಙ್ಕ)(mi.mi_fend - ptr);
	}
    }
    ಯದಿ (capcol != ರಿಕ್ತ)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ರಿಕ್ತ;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    ಯದಿ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (ಶೂನ್ಯ)spell_casefold(wp, ptr, (ಪೂರ್ಣಾಙ್ಕ)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(mi.mi_fword);

    ಯದಿ (is_camel_case && mi.mi_fwordlen > 0)
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
    ಪ್ರತಿ (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ಯದಿ (mi.mi_lp->lp_slang->sl_fidxs == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ಯದಿ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ಯದಿ (ಎಣಿಕೆ_word && mi.mi_result == SP_OK)
	{
	    ಎಣಿಕೆ_common_word(mi.mi_lp->lp_slang, ptr,
						   (ಪೂರ್ಣಾಙ್ಕ)(mi.mi_end - ptr), 1);
	    ಎಣಿಕೆ_word = FALSE;
	}
    }

    ಯದಿ (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ಯದಿ (nrlen > 0)
	{
	    ಯದಿ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		ಪ್ರತ್ಯಾಗಮ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	ಅನ್ಯಥಾ ಯದಿ (!spell_iswordp_nmw(ptr, wp))
	{
	    ಯದಿ (capcol != ರಿಕ್ತ && wp->w_s->b_cap_prog != ರಿಕ್ತ)
	    {
		regmatch_T	regmatch;
		ಪೂರ್ಣಾಙ್ಕ		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ಯದಿ (r)
		    *capcol = (ಪೂರ್ಣಾಙ್ಕ)(regmatch.endp[0] - ptr);
	    }

	    ಯದಿ (has_mbyte)
		ಪ್ರತ್ಯಾಗಮ (*mb_ptr2len)(ptr);
	    ಪ್ರತ್ಯಾಗಮ 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	ಅನ್ಯಥಾ ಯದಿ (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    ಪೂರ್ಣಾಙ್ಕ		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    ಯದಿ (mi.mi_lp->lp_slang->sl_fidxs != ರಿಕ್ತ)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		ಪ್ರತಿ (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ಯದಿ (p >= mi.mi_end)
			ಭಙ್ಗ;
		    mi.mi_compoff = (ಪೂರ್ಣಾಙ್ಕ)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    ಯದಿ (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			ಭಙ್ಗ;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	ಯದಿ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	ಅನ್ಯಥಾ ಯದಿ (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	ಅನ್ಯಥಾ
	    *attrp = HLF_SPL;
    }

    ಯದಿ (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ಪ್ರತ್ಯಾಗಮ wrongcaplen;
    }

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_char_type(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (VIM_ISDIGIT(c))
	ಪ್ರತ್ಯಾಗಮ CHAR_DIGIT;
    ಯದಿ (SPELL_ISUPPER(c))
	ಪ್ರತ್ಯಾಗಮ CHAR_UPPER;
    ಪ್ರತ್ಯಾಗಮ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    ಸ್ಥಿರ char_u *
advance_camelcase_word(char_u *str, win_T *wp, ಪೂರ್ಣಾಙ್ಕ *is_camel_case)
{
    ಪೂರ್ಣಾಙ್ಕ last_type, last_last_type, this_type;
    ಪೂರ್ಣಾಙ್ಕ c;
    char_u *end = str;

    *is_camel_case = FALSE;

    ಯದಿ (*str == NUL)
	ಪ್ರತ್ಯಾಗಮ str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    ಯಾವತ್ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	ಯದಿ (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    ಭಙ್ಗ;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    ಪ್ರತ್ಯಾಗಮ end;
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
find_word(matchinf_T *mip, ಪೂರ್ಣಾಙ್ಕ mode)
{
    idx_T	arridx = 0;
    ಪೂರ್ಣಾಙ್ಕ		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    ಪೂರ್ಣಾಙ್ಕ		endidxcnt = 0;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		wlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		flen;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    ಪೂರ್ಣಾಙ್ಕ		word_ends;
    ಪೂರ್ಣಾಙ್ಕ		prefix_found;
    ಪೂರ್ಣಾಙ್ಕ		nobreak_result;

    ಯದಿ (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ಯದಿ (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    ಅನ್ಯಥಾ
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ಯದಿ (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	ಅನ್ಯಥಾ ಯದಿ (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ಯದಿ (byts == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ಯದಿ (byts[arridx] == 0)
	{
	    ಯದಿ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ಯಾವತ್ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ಯದಿ (len == 0)
		ಭಙ್ಗ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ಯದಿ (ptr[wlen] == NUL)
	    ಭಙ್ಗ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ಯದಿ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ಯಾವತ್ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ಯದಿ (byts[m] > c)
		hi = m - 1;
	    ಅನ್ಯಥಾ ಯದಿ (byts[m] < c)
		lo = m + 1;
	    ಅನ್ಯಥಾ
	    {
		lo = hi = m;
		ಭಙ್ಗ;
	    }
	}

	// Stop if there is no matching byte.
	ಯದಿ (hi < lo || byts[lo] != c)
	    ಭಙ್ಗ;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ಯದಿ (c == ' ')
	{
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		ಯದಿ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    ಭಙ್ಗ;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    ಯಾವತ್ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ಯದಿ ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    ಅನುವರ್ತ;	    // not at first byte of character
	ಯದಿ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ಯದಿ (slang->sl_compprog == ರಿಕ್ತ && !slang->sl_nobreak)
		ಅನುವರ್ತ;	    // next char is a word character
	    word_ends = FALSE;
	}
	ಅನ್ಯಥಾ
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	ಯದಿ (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    ಯದಿ (STRNCMP(ptr, p, wlen) != 0)
	    {
		ಪ್ರತಿ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (ಪೂರ್ಣಾಙ್ಕ)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	ಪ್ರತಿ (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ಯದಿ (mode == FIND_FOLDWORD)
	    {
		ಯದಿ (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		ಯದಿ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    ಅನುವರ್ತ;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    ಅನ್ಯಥಾ ಯದಿ (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		ಯದಿ (c == 0)
		    ಅನುವರ್ತ;

		// Use the WF_RARE flag for a rare prefix.
		ಯದಿ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    ಯದಿ (slang->sl_nobreak)
	    {
		ಯದಿ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    ಭಙ್ಗ;
		}
	    }

	    ಅನ್ಯಥಾ ಯದಿ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ಯದಿ (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    ಅನುವರ್ತ;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ಯದಿ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			ಅನುವರ್ತ;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ಯದಿ (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    ಅನುವರ್ತ;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ಯದಿ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    ಅನುವರ್ತ;
		ಯದಿ (!word_ends && (flags & WF_NOCOMPAFT))
		    ಅನುವರ್ತ;

		// Quickly check if compounding is possible with this flag.
		ಯದಿ (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    ಅನುವರ್ತ;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ಯದಿ (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    ಅನುವರ್ತ;

		ಯದಿ (mode == FIND_COMPOUND)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ಯದಿ (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			ಪ್ರತಿ (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    ಅನ್ಯಥಾ
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    ಯದಿ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			ಅನುವರ್ತ;

		    ಯದಿ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			ಯದಿ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    ಅನುವರ್ತ;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		ಯದಿ (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    ಯದಿ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ಯದಿ (ptr == mip->mi_word)
			    (ಶೂನ್ಯ)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			ಅನ್ಯಥಾ
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ಯದಿ (!can_compound(slang, fword, mip->mi_compflags))
			ಅನುವರ್ತ;
		}
		ಅನ್ಯಥಾ ಯದಿ (slang->sl_comprules != ರಿಕ್ತ
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    ಅನುವರ್ತ;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    ಅನ್ಯಥಾ ಯದಿ (flags & WF_NEEDCOMP)
		ಅನುವರ್ತ;

	    nobreak_result = SP_OK;

	    ಯದಿ (!word_ends)
	    {
		ಪೂರ್ಣಾಙ್ಕ	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		ಪೂರ್ಣಾಙ್ಕ	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ಯದಿ (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ಯದಿ (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ಯದಿ (STRNCMP(ptr, p, wlen) != 0)
		    {
			ಪ್ರತಿ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (ಪೂರ್ಣಾಙ್ಕ)(p - mip->mi_fword);
		    }
		}
#ಯದಿ 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		ಯದಿ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		ಪ್ರತಿ (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ಯದಿ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			ಯದಿ (mip->mi_lp->lp_slang->sl_fidxs == ರಿಕ್ತ
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    ಅನುವರ್ತ;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ಯದಿ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#ಯದಿ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ಯದಿ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    ಯದಿ (!slang->sl_nobreak)
			ಭಙ್ಗ;
		}
		--mip->mi_complen;
		ಯದಿ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ಯದಿ (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (mip->mi_result == SP_OK)
			ಭಙ್ಗ;
		    ಅನುವರ್ತ;
		}
	    }

	    ಯದಿ (flags & WF_BANNED)
		res = SP_BANNED;
	    ಅನ್ಯಥಾ ಯದಿ (flags & WF_REGION)
	    {
		// Check region.
		ಯದಿ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		ಅನ್ಯಥಾ
		    res = SP_LOCAL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (flags & WF_RARE)
		res = SP_RARE;
	    ಅನ್ಯಥಾ
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ಯದಿ (nobreak_result == SP_BAD)
	    {
		ಯದಿ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		ಅನ್ಯಥಾ ಯದಿ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    ಯದಿ (mip->mi_result == SP_OK)
		ಭಙ್ಗ;
	}

	ಯದಿ (mip->mi_result == SP_OK)
	    ಭಙ್ಗ;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    ಪೂರ್ಣಾಙ್ಕ
match_checkcompoundpattern(
    char_u	*ptr,
    ಪೂರ್ಣಾಙ್ಕ		wlen,
    garray_T	*gap)  // &sl_comppat
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಪ್ರತಿ (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	ಯದಿ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
	    ಯದಿ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    ಪೂರ್ಣಾಙ್ಕ
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;

    ಯದಿ (slang->sl_compprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	ಪ್ರತಿ (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    ಅನ್ಯಥಾ
	p = flags;
    ಯದಿ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ಯದಿ (slang->sl_compsylmax < MAXWLEN
		       && ಎಣಿಕೆ_syllables(slang, word) > slang->sl_compsylmax)
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)STRLEN(flags) < slang->sl_compmax;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // loop over all the COMPOUNDRULE entries
    ಪ್ರತಿ (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	ಪ್ರತಿ (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ಯದಿ (c == NUL)
		// found a rule that matches for the flags we have so far
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಯದಿ (*p == '/' || *p == NUL)
		ಭಙ್ಗ;  // end of rule, it's too short
	    ಯದಿ (*p == '[')
	    {
		ಪೂರ್ಣಾಙ್ಕ match = FALSE;

		// compare against all the flags in []
		++p;
		ಯಾವತ್ (*p != ']' && *p != NUL)
		    ಯದಿ (*p++ == c)
			match = TRUE;
		ಯದಿ (!match)
		    ಭಙ್ಗ;  // none matches
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p != c)
		ಭಙ್ಗ;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ಯದಿ (p == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_word_prefix(
    ಪೂರ್ಣಾಙ್ಕ		totprefcnt,	// nr of prefix IDs
    ಪೂರ್ಣಾಙ್ಕ		arridx,		// idx in sl_pidxs[]
    ಪೂರ್ಣಾಙ್ಕ		flags,
    char_u	*word,
    slang_T	*slang,
    ಪೂರ್ಣಾಙ್ಕ		cond_req)	// only use prefixes with a condition
{
    ಪೂರ್ಣಾಙ್ಕ		prefcnt;
    ಪೂರ್ಣಾಙ್ಕ		pidx;
    regprog_T	**rp;
    ಪೂರ್ಣಾಙ್ಕ		prefid;

    prefid = (unsigned)flags >> 24;
    ಪ್ರತಿ (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ಯದಿ (prefid != (pidx & 0xff))
	    ಅನುವರ್ತ;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ಯದಿ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    ಅನುವರ್ತ;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	ಯದಿ (*rp != ರಿಕ್ತ)
	{
	    ಯದಿ (!vim_regexec_prog(rp, FALSE, word, 0))
		ಅನುವರ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (cond_req)
	    ಅನುವರ್ತ;

	// It's a match!  Return the WF_ flags.
	ಪ್ರತ್ಯಾಗಮ pidx;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
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
find_prefix(matchinf_T *mip, ಪೂರ್ಣಾಙ್ಕ mode)
{
    idx_T	arridx = 0;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		wlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		flen;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ಯದಿ (byts == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ಯದಿ (mode == FIND_COMPOUND)
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
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ಯದಿ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ಯಾವತ್ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ಯದಿ (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ಯದಿ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    ಅನ್ಯಥಾ
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    ಯದಿ (len == 0)
		ಭಙ್ಗ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ಯದಿ (ptr[wlen] == NUL)
	    ಭಙ್ಗ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ಯಾವತ್ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ಯದಿ (byts[m] > c)
		hi = m - 1;
	    ಅನ್ಯಥಾ ಯದಿ (byts[m] < c)
		lo = m + 1;
	    ಅನ್ಯಥಾ
	    {
		lo = hi = m;
		ಭಙ್ಗ;
	    }
	}

	// Stop if there is no matching byte.
	ಯದಿ (hi < lo || byts[lo] != c)
	    ಭಙ್ಗ;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fold_more(matchinf_T *mip)
{
    ಪೂರ್ಣಾಙ್ಕ		flen;
    char_u	*p;

    p = mip->mi_fend;
    ಕುರು
	MB_PTR_ADV(mip->mi_fend);
    ಯಾವತ್ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ಯದಿ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (ಶೂನ್ಯ)spell_casefold(mip->mi_win, p, (ಪೂರ್ಣಾಙ್ಕ)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ಪ್ರತ್ಯಾಗಮ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_valid_case(
    ಪೂರ್ಣಾಙ್ಕ	    wordflags,	    // flags for the checked word.
    ಪೂರ್ಣಾಙ್ಕ	    treeflags)	    // flags for the word in the spell tree
{
    ಪ್ರತ್ಯಾಗಮ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_check_window(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(ವರ್ಣ **)(wp->w_s->b_langp.ga_data) != ರಿಕ್ತ;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
no_spell_checking(win_T *wp)
{
    ಯದಿ (spell_check_window(wp))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_move_to(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    ಪೂರ್ಣಾಙ್ಕ		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    ಪೂರ್ಣಾಙ್ಕ		found_len = 0;
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    ಪೂರ್ಣಾಙ್ಕ		len;
#ifdef FEAT_SYN_HL
    ಪೂರ್ಣಾಙ್ಕ		has_syntax = syntax_present(wp);
#endif
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		can_spell;
    char_u	*buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		buflen = 0;
    ಪೂರ್ಣಾಙ್ಕ		skip = 0;
    ಪೂರ್ಣಾಙ್ಕ		capcol = -1;
    ಪೂರ್ಣಾಙ್ಕ		found_one = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		wrapped = FALSE;

    ಯದಿ (no_spell_checking(wp))
	ಪ್ರತ್ಯಾಗಮ 0;

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

    ಯಾವತ್ (!got_int)
    {
	ಪೂರ್ಣಾಙ್ಕ empty_ಸಾಲು;

	ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	ಯದಿ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ಯದಿ (buf == ರಿಕ್ತ)
		ಭಙ್ಗ;
	}

	// In first line check first word for Capital.
	ಯದಿ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ಯದಿ (capcol == 0)
	    capcol = getwhitecols(ಸಾಲು);
	ಅನ್ಯಥಾ ಯದಿ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(ಸಾಲು);
	    ಯದಿ (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_ಸಾಲು = *skipwhite(ಸಾಲು) == NUL;
	STRCPY(buf, ಸಾಲು);
	ಯದಿ (lnum < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    spell_cat_ಸಾಲು(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ಯಾವತ್ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ಯದಿ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		ಭಙ್ಗ;

	    // start of word
	    attr = HLF_ಎಣಿಕೆ;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ಯದಿ (attr != HLF_ಎಣಿಕೆ)
	    {
		// We found a bad word.  Check the attribute.
		ಯದಿ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ಯದಿ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			ಯದಿ (has_syntax)
			{
			    col = (ಪೂರ್ಣಾಙ್ಕ)(p - buf);
			    (ಶೂನ್ಯ)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    ಯದಿ (!can_spell)
				attr = HLF_ಎಣಿಕೆ;
			}
			ಅನ್ಯಥಾ
#endif
			    can_spell = TRUE;

			ಯದಿ (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (ಪೂರ್ಣಾಙ್ಕ)(p - buf);
			    found_pos.coladd = 0;
			    ಯದಿ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				ಯದಿ (attrp != ರಿಕ್ತ)
				    *attrp = attr;
				ಪ್ರತ್ಯಾಗಮ len;
			    }
			    ಅನ್ಯಥಾ ಯದಿ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    ಅನ್ಯಥಾ
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ಯದಿ (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    ಪ್ರತ್ಯಾಗಮ found_len;
	}

	ಯದಿ (curline)
	    ಭಙ್ಗ;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ಯದಿ (lnum == wp->w_cursor.lnum && wrapped)
	    ಭಙ್ಗ;

	// Advance to next line.
	ಯದಿ (dir == BACKWARD)
	{
	    ಯದಿ (lnum > 1)
		--lnum;
	    ಅನ್ಯಥಾ ಯದಿ (!p_ws)
		ಭಙ್ಗ;	    // at first line and 'nowrapscan'
	    ಅನ್ಯಥಾ
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		wrapped = TRUE;
		ಯದಿ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(ತುದಿ_bot_ಸಂದೇಶ), TRUE);
	    }
	    capcol = -1;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (lnum < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		++lnum;
	    ಅನ್ಯಥಾ ಯದಿ (!p_ws)
		ಭಙ್ಗ;	    // at first line and 'nowrapscan'
	    ಅನ್ಯಥಾ
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ಯದಿ (!shortmess(SHM_ಹುಡುಕು))
		    give_warning((char_u *)_(bot_ತುದಿ_ಸಂದೇಶ), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ಯದಿ (lnum == wp->w_cursor.lnum && !found_one)
		ಭಙ್ಗ;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ಯದಿ (attr == HLF_ಎಣಿಕೆ)
		skip = (ಪೂರ್ಣಾಙ್ಕ)(p - endp);
	    ಅನ್ಯಥಾ
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ಯದಿ (empty_ಸಾಲು)
		capcol = 0;
	}

	ಸಾಲು_breakcheck();
    }

    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    ಶೂನ್ಯ
spell_cat_ಸಾಲು(char_u *buf, char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ maxlen)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		n;

    p = skipwhite(ಸಾಲು);
    ಯಾವತ್ (vim_strchr((char_u *)"*#/\"\t", *p) != ರಿಕ್ತ)
	p = skipwhite(p + 1);

    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು) + 1;
    ಯದಿ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    ಪೂರ್ಣಾಙ್ಕ	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    ಪೂರ್ಣಾಙ್ಕ		r;
    spelload_T	sl;
    ಪೂರ್ಣಾಙ್ಕ		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ರಿಕ್ತ;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((ವರ್ಣ *)fname_enc, ಪರಿಮಾಣ(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#ಅನ್ಯಥಾ
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	ಯದಿ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((ವರ್ಣ *)fname_enc, ಪರಿಮಾಣ(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#ಅನ್ಯಥಾ
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    ಯದಿ (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		ಅನುವರ್ತ;
	    ಭಙ್ಗ;
	}
	ಭಙ್ಗ;
    }

    ಯದಿ (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#ಅನ್ಯಥಾ
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    ಅನ್ಯಥಾ ಯದಿ (sl.sl_slang != ರಿಕ್ತ)
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
spell_enc(ಶೂನ್ಯ)
{

    ಯದಿ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ಪ್ರತ್ಯಾಗಮ p_enc;
    ಪ್ರತ್ಯಾಗಮ (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((ವರ್ಣ *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    ಯದಿ (lp != ರಿಕ್ತ)
    {
	ಯದಿ (lang != ರಿಕ್ತ)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, ಪರಿಮಾಣ(fromto_T), 10);
	ga_init2(&lp->sl_repsal, ಪರಿಮಾಣ(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ಪ್ರತ್ಯಾಗಮ lp;
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

    ಯಾವತ್ (gap->ga_len > 0)
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
    ಶೂನ್ಯ
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ಯಾವತ್ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ಯದಿ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ಯದಿ (gap->ga_data != ರಿಕ್ತ)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
		vim_free(((ಪೂರ್ಣಾಙ್ಕ **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    ಅನ್ಯಥಾ
	free_sal_ವಸ್ತುಗಳು(gap);

    ಪ್ರತಿ (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ರಿಕ್ತ;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_ವಸ್ತುಗಳು);

    ga_clear_strings(&lp->sl_comppat);

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
    lp->sl_sugbuf = ರಿಕ್ತ;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_load_cb(char_u *fname, ಶೂನ್ಯ *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, ರಿಕ್ತ, FALSE);
    ಯದಿ (slang == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ಯದಿ (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    ಶೂನ್ಯ
ಎಣಿಕೆ_common_word(
    slang_T	*lp,
    char_u	*word,
    ಪೂರ್ಣಾಙ್ಕ		len,	    // word length, -1 for up to NUL
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ಯದಿ (len == -1)
	p = word;
    ಅನ್ಯಥಾ ಯದಿ (len >= MAXWLEN)
	ಪ್ರತ್ಯಾಗಮ;
    ಅನ್ಯಥಾ
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ಯದಿ (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	ಯದಿ (wc == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	STRCPY(wc->wc_word, p);
	wc->wc_ಎಣಿಕೆ = ಎಣಿಕೆ;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    ಅನ್ಯಥಾ
    {
	wc = HI2WC(hi);
	ಯದಿ ((wc->wc_ಎಣಿಕೆ += ಎಣಿಕೆ) < (unsigned)ಎಣಿಕೆ)	// check for overflow
	    wc->wc_ಎಣಿಕೆ = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    ಪೂರ್ಣಾಙ್ಕ
byte_in_str(char_u *str, ಪೂರ್ಣಾಙ್ಕ n)
{
    char_u	*p;

    ಪ್ರತಿ (p = str; *p != NUL; ++p)
	ಯದಿ (*p == n)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#define SY_MAXLEN   30
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    ಪೂರ್ಣಾಙ್ಕ		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    ಪೂರ್ಣಾಙ್ಕ
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_ವಸ್ತುಗಳು, ಪರಿಮಾಣ(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ಯಾವತ್ (p != ರಿಕ್ತ)
    {
	*p++ = NUL;
	ಯದಿ (*p == NUL)	    // trailing slash
	    ಭಙ್ಗ;
	s = p;
	p = vim_strchr(p, '/');
	ಯದಿ (p == ರಿಕ್ತ)
	    l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
	ಅನ್ಯಥಾ
	    l = (ಪೂರ್ಣಾಙ್ಕ)(p - s);
	ಯದಿ (l >= SY_MAXLEN)
	    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
	ಯದಿ (ga_grow(&slang->sl_syl_ವಸ್ತುಗಳು, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_ವಸ್ತುಗಳು.ga_data)
					       + slang->sl_syl_ವಸ್ತುಗಳು.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಎಣಿಕೆ_syllables(slang_T *slang, char_u *word)
{
    ಪೂರ್ಣಾಙ್ಕ		cnt = 0;
    ಪೂರ್ಣಾಙ್ಕ		skip = FALSE;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    syl_item_T	*syl;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (slang->sl_syllable == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತಿ (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ಯದಿ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    ಅನುವರ್ತ;
	}

	// Find longest match of syllable items.
	len = 0;
	ಪ್ರತಿ (i = 0; i < slang->sl_syl_ವಸ್ತುಗಳು.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_ವಸ್ತುಗಳು.ga_data) + i;
	    ಯದಿ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ಯದಿ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ಯದಿ (vim_strchr(slang->sl_syllable, c) == ರಿಕ್ತ)
		skip = FALSE;	    // No, search for next syllable
	    ಅನ್ಯಥಾ ಯದಿ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    ವರ್ಣ *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    ಪೂರ್ಣಾಙ್ಕ		filename;
    ಪೂರ್ಣಾಙ್ಕ		region_mask;
    slang_T	*slang;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		round;
    char_u	*spf;
    char_u	*use_region = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		dont_use_region = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		nobreak = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    langp_T	*lp, *lp2;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;
    ವರ್ಣ	*ret_ಸಂದೇಶ = ರಿಕ್ತ;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    recursive = TRUE;

    ga_init2(&ga, ಪರಿಮಾಣ(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ಯದಿ (spl_copy == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    ಪ್ರತಿ (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ರಿಕ್ತ;

	ಯದಿ (!valid_spelllang(lang))
	    ಅನುವರ್ತ;

	ಯದಿ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    ಅನುವರ್ತ;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ಯದಿ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ಯದಿ (p != ರಿಕ್ತ && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    ಅನ್ಯಥಾ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ಯದಿ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	{
	    filename = FALSE;
	    ಯದಿ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    ಅನ್ಯಥಾ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ಯದಿ (STRICMP(lang, slang->sl_name) == 0)
		    ಭಙ್ಗ;
	}

	ಯದಿ (region != ರಿಕ್ತ)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ಯದಿ (use_region != ರಿಕ್ತ && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ಯದಿ (slang == ರಿಕ್ತ)
	{
	    ಯದಿ (filename)
		(ಶೂನ್ಯ)spell_load_file(lang, lang, ರಿಕ್ತ, FALSE);
	    ಅನ್ಯಥಾ
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ಯದಿ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_ಸಂದೇಶ = N_(e_spellfilemising_autocommand_deleted_buffer);
		    ಗಚ್ಛ theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    ಯದಿ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		ಯದಿ (!filename && region != ರಿಕ್ತ)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    ಯದಿ (c == REGION_ALL)
		    {
			ಯದಿ (slang->sl_add)
			{
			    ಯದಿ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			ಅನ್ಯಥಾ
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    ಅನ್ಯಥಾ
			region_mask = 1 << c;
		}

		ಯದಿ (region_mask != 0)
		{
		    ಯದಿ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_ಸಂದೇಶ = e_out_of_memory;
			ಗಚ್ಛ theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ಯದಿ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    ಪ್ರತಿ (round = 0; round == 0 || *spf != NUL; ++round)
    {
	ಯದಿ (round == 0)
	{
	    // Internal wordlist, if there is one.
	    ಯದಿ (int_wordlist == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    int_wordlist_spl(spf_name);
	}
	ಅನ್ಯಥಾ
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    ಪ್ರತಿ (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		ಯದಿ (p != ರಿಕ್ತ && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (c < ga.ga_len)
		ಅನುವರ್ತ;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ಯದಿ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		ಭಙ್ಗ;
	ಯದಿ (slang == ರಿಕ್ತ)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ಯದಿ (round == 0)
		STRCPY(lang, "internal wordlist");
	    ಅನ್ಯಥಾ
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ಯದಿ (p != ರಿಕ್ತ)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, ರಿಕ್ತ, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ಯದಿ (slang != ರಿಕ್ತ && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ಯದಿ (slang != ರಿಕ್ತ && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    ಯದಿ (use_region != ರಿಕ್ತ && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		ಯದಿ (c != REGION_ALL)
		    region_mask = 1 << c;
		ಅನ್ಯಥಾ ಯದಿ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    ಯದಿ (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = ರಿಕ್ತ;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = ರಿಕ್ತ;
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
    ಪ್ರತಿ (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	ಯದಿ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	ಅನ್ಯಥಾ
	    // find first similar language that does sound folding
	    ಪ್ರತಿ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ಯದಿ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    ಭಙ್ಗ;
		}
	    }

	// REP items
	ಯದಿ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	ಅನ್ಯಥಾ
	    // find first similar language that has REP items
	    ಪ್ರತಿ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ಯದಿ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    ಭಙ್ಗ;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ಪ್ರತ್ಯಾಗಮ ret_ಸಂದೇಶ;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
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

    ಯದಿ (lp->sl_midword == ರಿಕ್ತ)	    // there aren't any
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (p = lp->sl_midword; *p != NUL; )
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ಯದಿ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_s->b_spell_ismw_mb == ರಿಕ್ತ)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    ಅನ್ಯಥಾ
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ಯದಿ (bp != ರಿಕ್ತ)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	ಅನ್ಯಥಾ
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_region(char_u *rp, char_u *region)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; ; i += 2)
    {
	ಯದಿ (rp[i] == NUL)
	    ಪ್ರತ್ಯಾಗಮ REGION_ALL;
	ಯದಿ (rp[i] == region[0] && rp[i + 1] == region[1])
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    ಪೂರ್ಣಾಙ್ಕ
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		firstcap;
    ಪೂರ್ಣಾಙ್ಕ		allcap;
    ಪೂರ್ಣಾಙ್ಕ		past_second = FALSE;	// past second word char

    // find first letter
    ಪ್ರತಿ (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ಯದಿ (end == ರಿಕ್ತ ? *p == NUL : p >= end)
	    ಪ್ರತ್ಯಾಗಮ 0;	    // only non-word characters, illegal word
    ಯದಿ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    ಅನ್ಯಥಾ
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    ಪ್ರತಿ ( ; end == ರಿಕ್ತ ? *p != NUL : p < end; MB_PTR_ADV(p))
	ಯದಿ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ಯದಿ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ಯದಿ (past_second && allcap)
		    ಪ್ರತ್ಯಾಗಮ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!allcap)
		// UlU -> KEEPCAP
		ಪ್ರತ್ಯಾಗಮ WF_KEEPCAP;
	    past_second = TRUE;
	}

    ಯದಿ (allcap)
	ಪ್ರತ್ಯಾಗಮ WF_ALLCAP;
    ಯದಿ (firstcap)
	ಪ್ರತ್ಯಾಗಮ WF_ONECAP;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    ಶೂನ್ಯ
spell_delete_wordlist(ಶೂನ್ಯ)
{
    char_u	fname[MAXPATHL];

    ಯದಿ (int_wordlist == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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

    ಯಾವತ್ (first_lang != ರಿಕ್ತ)
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
	ಯದಿ (*wp->w_s->b_p_spl != NUL)
	{
		ಯದಿ (wp->w_p_spell)
		{
		    (ಶೂನ್ಯ)parse_spelllang(wp);
		    ಭಙ್ಗ;
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
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_ಕೀಲಿ = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Close the buffer used for spell info.
 */
    ಶೂನ್ಯ
close_spellbuf(buf_T *buf)
{
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    ಶೂನ್ಯ
clear_spell_chartab(spelltab_T *sp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    ಪ್ರತಿ (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    ಪ್ರತಿ (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    ಪ್ರತಿ (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    ಪ್ರತಿ (i = 'a'; i <= 'z'; ++i)
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
    ಶೂನ್ಯ
init_spell_chartab(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ಯದಿ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	ಪ್ರತಿ (i = 128; i <= 255; ++i)
	    ಯದಿ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
    {
	ಪ್ರತಿ (i = 128; i < 256; ++i)
	{
	    ಪೂರ್ಣಾಙ್ಕ f = utf_fold(i);
	    ಪೂರ್ಣಾಙ್ಕ u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Rough guess: use locale-dependent library functions.
	ಪ್ರತಿ (i = 128; i < 256; ++i)
	{
	    ಯದಿ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (MB_ISLOWER(i))
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
    ಪೂರ್ಣಾಙ್ಕ
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ಯದಿ (l == 1)
	{
	    // be quick for ASCII
	    ಯದಿ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	ಅನ್ಯಥಾ
	{
	    c = mb_ptr2char(p);
	    ಯದಿ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ರಿಕ್ತ
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ರಿಕ್ತ))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ಯದಿ (c > 255)
	    ಪ್ರತ್ಯಾಗಮ spell_mb_isword_class(mb_get_class(s), wp);
	ಪ್ರತ್ಯಾಗಮ spelltab.st_isw[c];
    }

    ಪ್ರತ್ಯಾಗಮ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (has_mbyte)
    {
	c = mb_ptr2char(p);
	ಯದಿ (c > 255)
	    ಪ್ರತ್ಯಾಗಮ spell_mb_isword_class(mb_get_class(p), wp);
	ಪ್ರತ್ಯಾಗಮ spelltab.st_isw[c];
    }
    ಪ್ರತ್ಯಾಗಮ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
spell_mb_isword_class(ಪೂರ್ಣಾಙ್ಕ cl, win_T *wp)
{
    ಯದಿ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ಪ್ರತ್ಯಾಗಮ cl == 2 || cl == 0x2800;
    ಪ್ರತ್ಯಾಗಮ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
spell_iswordp_w(ಪೂರ್ಣಾಙ್ಕ *p, win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		*s;

    ಯದಿ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ರಿಕ್ತ
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ರಿಕ್ತ))
	s = p + 1;
    ಅನ್ಯಥಾ
	s = p;

    ಯದಿ (*s > 255)
    {
	ಯದಿ (enc_utf8)
	    ಪ್ರತ್ಯಾಗಮ spell_mb_isword_class(utf_class(*s), wp);
	ಯದಿ (enc_dbcs)
	    ಪ್ರತ್ಯಾಗಮ spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_casefold(
    win_T	*wp,
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		len,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		buflen)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (len >= buflen)
    {
	buf[0] = NUL;
	ಪ್ರತ್ಯಾಗಮ FAIL;		// result will not fit
    }

    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ	outi = 0;
	char_u	*p;
	ಪೂರ್ಣಾಙ್ಕ	c;

	// Fold one character at a time.
	ಪ್ರತಿ (p = str; p < str + len; )
	{
	    ಯದಿ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ಯದಿ (c == 0x03a3 || c == 0x03c2)
	    {
		ಯದಿ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		ಅನ್ಯಥಾ
		    c = 0x03c3;
	    }
	    ಅನ್ಯಥಾ
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    ಅನ್ಯಥಾ
    {
	// Be quick for non-multibyte encodings.
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    ಪೂರ್ಣಾಙ್ಕ
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    ಯದಿ (wp->w_s->b_cap_prog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ		need_cap = FALSE;
    char_u	*ಸಾಲು = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : ರಿಕ್ತ;
    char_u	*ಸಾಲು_copy = ರಿಕ್ತ;
    colnr_T	endcol = 0;

    ಯದಿ (col == 0 || getwhitecols(ಸಾಲು) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ಯದಿ (lnum == 1)
	    need_cap = TRUE;
	ಅನ್ಯಥಾ
	{
	    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    ಯದಿ (*skipwhite(ಸಾಲು) == NUL)
		need_cap = TRUE;
	    ಅನ್ಯಥಾ
	    {
		// Append a space in place of the line break.
		ಸಾಲು_copy = concat_str(ಸಾಲು, (char_u *)" ");
		ಯದಿ (ಸಾಲು_copy == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FALSE;

		ಸಾಲು = ಸಾಲು_copy;
		endcol = (colnr_T)STRLEN(ಸಾಲು);
	    }
	}
    }
    ಅನ್ಯಥಾ
	endcol = col;

    ಯದಿ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = ಸಾಲು + endcol;
	ಪ್ರತಿ (;;)
	{
	    MB_PTR_BACK(ಸಾಲು, p);
	    ಯದಿ (p == ಸಾಲು || spell_iswordp_nmw(p, wp))
		ಭಙ್ಗ;
	    ಯದಿ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == ಸಾಲು + endcol)
	    {
		need_cap = TRUE;
		ಭಙ್ಗ;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(ಸಾಲು_copy);

    ಪ್ರತ್ಯಾಗಮ need_cap;
}


/*
 * ":spellrepall"
 */
    ಶೂನ್ಯ
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*ಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ಯದಿ (repl_from == ರಿಕ್ತ || repl_to == ರಿಕ್ತ)
    {
	emsg(_(e_no_previous_spell_replacement));
	ಪ್ರತ್ಯಾಗಮ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ಯದಿ (frompat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    frompatlen = vim_snprintf((ವರ್ಣ *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ಯಾವತ್ (!got_int)
    {
	ಯದಿ (do_ಹುಡುಕು(ರಿಕ್ತ, '/', '/', frompat, frompatlen, 1L, ಹುಡುಕು_KEEP, ರಿಕ್ತ) == 0
						   || u_save_cursor() == FAIL)
	    ಭಙ್ಗ;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	ಸಾಲು = ml_get_curline();
	ಯದಿ (addlen <= 0 || STRNCMP(ಸಾಲು + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ಯದಿ (p == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    mch_memmove(p, ಸಾಲು, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, ಸಾಲು + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#ಯದಿ defined(FEAT_PROP_POPUP)
	    ಯದಿ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (ಪೂರ್ಣಾಙ್ಕ)addlen, APC_SUBSTITUTE);
#endif

	    ಯದಿ (curwin->w_cursor.lnum != prev_lnum)
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

    ಯದಿ (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    ಅನ್ಯಥಾ
	do_sub_ಸಂದೇಶ(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    ಶೂನ್ಯ
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    ಪೂರ್ಣಾಙ್ಕ		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		l;

    p = word;
    ಯದಿ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    ಅನ್ಯಥಾ
	c = *p++;
    ಯದಿ (upper)
	c = SPELL_TOUPPER(c);
    ಅನ್ಯಥಾ
	c = SPELL_TOFOLD(c);
    ಯದಿ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    ಅನ್ಯಥಾ
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
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    ಪೂರ್ಣಾಙ್ಕ		c;

    d = wcopy;
    ಪ್ರತಿ (s = word; *s != NUL; )
    {
	ಯದಿ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	ಅನ್ಯಥಾ
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ಯದಿ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ಯದಿ (d - wcopy >= MAXWLEN - 1)
		ಭಙ್ಗ;
	    *d++ = c;
	}
	ಅನ್ಯಥಾ
	    c = SPELL_TOUPPER(c);

	ಯದಿ (has_mbyte)
	{
	    ಯದಿ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		ಭಙ್ಗ;
	    d += mb_char2bytes(c, d);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (d - wcopy >= MAXWLEN - 1)
		ಭಙ್ಗ;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    ಪೂರ್ಣಾಙ್ಕ
nofold_len(char_u *fword, ಪೂರ್ಣಾಙ್ಕ flen, char_u *word)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    ಪ್ರತಿ (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    ಪ್ರತಿ (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    ಶೂನ್ಯ
make_case_word(char_u *fword, char_u *cword, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಯದಿ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    ಅನ್ಯಥಾ ಯದಿ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    ಅನ್ಯಥಾ
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		lpi;

    ಯದಿ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	ಪ್ರತಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    ಯದಿ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		ಪ್ರತ್ಯಾಗಮ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ಪ್ರತ್ಯಾಗಮ vim_strsave(word);
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
    ಶೂನ್ಯ
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    ಪೂರ್ಣಾಙ್ಕ		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    ಯದಿ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    ಅನ್ಯಥಾ
    {
	// SAL items used.  Requires the word to be case-folded.
	ಯದಿ (folded)
	    word = inword;
	ಅನ್ಯಥಾ
	{
	    (ಶೂನ್ಯ)spell_casefold(curwin,
				  inword, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	ಯದಿ (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	ಅನ್ಯಥಾ
	    spell_soundfold_sal(slang, word, res);
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
    ಪೂರ್ಣಾಙ್ಕ		ri = 0;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ	prevc = 0;
	ಪೂರ್ಣಾಙ್ಕ	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	ಪ್ರತಿ (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ಯದಿ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    ಅನ್ಯಥಾ ಯದಿ (c < 256)
		c = slang->sl_sal_first[c];
	    ಅನ್ಯಥಾ
	    {
		ip = ((ಪೂರ್ಣಾಙ್ಕ **)slang->sl_sal.ga_data)[c & 0xff];
		ಯದಿ (ip == ರಿಕ್ತ)		// empty list, can't match
		    c = NUL;
		ಅನ್ಯಥಾ
		    ಪ್ರತಿ (;;)		// find "c" in the list
		    {
			ಯದಿ (*ip == 0)	// not found
			{
			    c = NUL;
			    ಭಙ್ಗ;
			}
			ಯದಿ (*ip == c)	// match!
			{
			    c = ip[1];
			    ಭಙ್ಗ;
			}
			ip += 2;
		    }
	    }

	    ಯದಿ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ಯದಿ (ri + MB_MAXBYTES > MAXWLEN)
		    ಭಙ್ಗ;
		prevc = c;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	// The sl_sal_first[] table contains the translation.
	ಪ್ರತಿ (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ಯದಿ (VIM_ISWHITE(c))
		c = ' ';
	    ಅನ್ಯಥಾ
		c = slang->sl_sal_first[c];
	    ಯದಿ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    ಪೂರ್ಣಾಙ್ಕ		i, j, z;
    ಪೂರ್ಣಾಙ್ಕ		reslen;
    ಪೂರ್ಣಾಙ್ಕ		n, k = 0;
    ಪೂರ್ಣಾಙ್ಕ		z0;
    ಪೂರ್ಣಾಙ್ಕ		k0;
    ಪೂರ್ಣಾಙ್ಕ		n0;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		pri;
    ಪೂರ್ಣಾಙ್ಕ		p0 = -333;
    ಪೂರ್ಣಾಙ್ಕ		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ಯದಿ (slang->sl_rem_accents)
    {
	t = word;
	ಯಾವತ್ (*s != NUL)
	{
	    ಯದಿ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    ಅನ್ಯಥಾ
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ಯಾವತ್ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	ಯದಿ (n >= 0)
	{
	    // check all rules for the same letter
	    ಪ್ರತಿ (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ಯದಿ (k > 1)
		{
		    ಯದಿ (word[i + 1] != s[1])
			ಅನುವರ್ತ;
		    ಯದಿ (k > 2)
		    {
			ಪ್ರತಿ (j = 2; j < k; ++j)
			    ಯದಿ (word[i + j] != s[j])
				ಭಙ್ಗ;
			ಯದಿ (j < k)
			    ಅನುವರ್ತ;
		    }
		}

		ಯದಿ ((pf = smp[n].sm_oneof) != ರಿಕ್ತ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ಯಾವತ್ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ಯದಿ (*pf == NUL)
			ಅನುವರ್ತ;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ಯಾವತ್ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ಯದಿ (*s == '<')
		    s++;
		ಯದಿ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ಯದಿ (*s == '^' && *(s + 1) == '^')
		    s++;

		ಯದಿ (*s == NUL
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

		    ಯದಿ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			ಪ್ರತಿ ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ಯದಿ (k0 > 1)
			    {
				ಯದಿ (word[i + k] != s[1])
				    ಅನುವರ್ತ;
				ಯದಿ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    ಪ್ರತಿ (j = 2; j < k0; ++j)
					ಯದಿ (*pf++ != s[j])
					    ಭಙ್ಗ;
				    ಯದಿ (j < k0)
					ಅನುವರ್ತ;
				}
			    }
			    k0 += k - 1;

			    ಯದಿ ((pf = smp[n0].sm_oneof) != ರಿಕ್ತ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ಯಾವತ್ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ಯದಿ (*pf == NUL)
				    ಅನುವರ್ತ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ಯಾವತ್ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ಯದಿ (*s == '<')
				s++;
			    ಯದಿ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ಯದಿ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				ಯದಿ (k0 == k)
				    // this is just a piece of the string
				    ಅನುವರ್ತ;

				ಯದಿ (p0 < pri)
				    // priority too low
				    ಅನುವರ್ತ;
				// rule fits; stop search
				ಭಙ್ಗ;
			    }
			}

			ಯದಿ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    ಅನುವರ್ತ;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    ಯದಿ (s == ರಿಕ್ತ)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != ರಿಕ್ತ) ? 1 : 0;
		    ಯದಿ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ಯದಿ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ಯಾವತ್ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			ಯದಿ (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    ಅನ್ಯಥಾ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ಯಾವತ್ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ಯದಿ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ಯದಿ (strstr((ವರ್ಣ *)pf, "^^") != ರಿಕ್ತ)
			{
			    ಯದಿ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ಯದಿ (z0 == 0)
	{
	    ಯದಿ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    ಪೂರ್ಣಾಙ್ಕ		word[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		wres[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		l;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		*ws;
    char_u	*t;
    ಪೂರ್ಣಾಙ್ಕ		*pf;
    ಪೂರ್ಣಾಙ್ಕ		i, j, z;
    ಪೂರ್ಣಾಙ್ಕ		reslen;
    ಪೂರ್ಣಾಙ್ಕ		n, k = 0;
    ಪೂರ್ಣಾಙ್ಕ		z0;
    ಪೂರ್ಣಾಙ್ಕ		k0;
    ಪೂರ್ಣಾಙ್ಕ		n0;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		pri;
    ಪೂರ್ಣಾಙ್ಕ		p0 = -333;
    ಪೂರ್ಣಾಙ್ಕ		c0;
    ಪೂರ್ಣಾಙ್ಕ		did_white = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    ಪ್ರತಿ (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ಯದಿ (slang->sl_rem_accents)
	{
	    ಯದಿ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ಯದಿ (did_white)
		    ಅನುವರ್ತ;
		c = ' ';
		did_white = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		did_white = FALSE;
		ಯದಿ (!spell_iswordp_nmw(t, curwin))
		    ಅನುವರ್ತ;
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
    ಯಾವತ್ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	ಯದಿ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    ಪ್ರತಿ (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ಯದಿ (c != ws[0])
		    ಅನುವರ್ತ;
		k = smp[n].sm_leadlen;
		ಯದಿ (k > 1)
		{
		    ಯದಿ (word[i + 1] != ws[1])
			ಅನುವರ್ತ;
		    ಯದಿ (k > 2)
		    {
			ಪ್ರತಿ (j = 2; j < k; ++j)
			    ಯದಿ (word[i + j] != ws[j])
				ಭಙ್ಗ;
			ಯದಿ (j < k)
			    ಅನುವರ್ತ;
		    }
		}

		ಯದಿ ((pf = smp[n].sm_oneof_w) != ರಿಕ್ತ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ಯಾವತ್ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ಯದಿ (*pf == NUL)
			ಅನುವರ್ತ;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ಯಾವತ್ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ಯದಿ (*s == '<')
		    s++;
		ಯದಿ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ಯದಿ (*s == '^' && *(s + 1) == '^')
		    s++;

		ಯದಿ (*s == NUL
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

		    ಯದಿ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			ಪ್ರತಿ ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ಯದಿ (c0 != ws[0])
				ಅನುವರ್ತ;
			    k0 = smp[n0].sm_leadlen;
			    ಯದಿ (k0 > 1)
			    {
				ಯದಿ (word[i + k] != ws[1])
				    ಅನುವರ್ತ;
				ಯದಿ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    ಪ್ರತಿ (j = 2; j < k0; ++j)
					ಯದಿ (*pf++ != ws[j])
					    ಭಙ್ಗ;
				    ಯದಿ (j < k0)
					ಅನುವರ್ತ;
				}
			    }
			    k0 += k - 1;

			    ಯದಿ ((pf = smp[n0].sm_oneof_w) != ರಿಕ್ತ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ಯಾವತ್ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ಯದಿ (*pf == NUL)
				    ಅನುವರ್ತ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ಯಾವತ್ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ಯದಿ (*s == '<')
				s++;
			    ಯದಿ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ಯದಿ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				ಯದಿ (k0 == k)
				    // this is just a piece of the string
				    ಅನುವರ್ತ;

				ಯದಿ (p0 < pri)
				    // priority too low
				    ಅನುವರ್ತ;
				// rule fits; stop search
				ಭಙ್ಗ;
			    }
			}

			ಯದಿ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    ಅನುವರ್ತ;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != ರಿಕ್ತ) ? 1 : 0;
		    ಯದಿ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ಯದಿ (reslen > 0 && ws != ರಿಕ್ತ && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ಯದಿ (ws != ರಿಕ್ತ)
			    ಯಾವತ್ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ಯದಿ (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    ಅನ್ಯಥಾ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ಯದಿ (ws != ರಿಕ್ತ)
			    ಯಾವತ್ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ಯದಿ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ಯದಿ (ws == ರಿಕ್ತ)
			    c = NUL;
			ಅನ್ಯಥಾ
			    c = *ws;
			ಯದಿ (strstr((ವರ್ಣ *)s, "^^") != ರಿಕ್ತ)
			{
			    ಯದಿ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ಯದಿ (z0 == 0)
	{
	    ಯದಿ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    ಪ್ರತಿ (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ಯದಿ (l + MB_MAXBYTES > MAXWLEN)
	    ಭಙ್ಗ;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    ಶೂನ್ಯ
ex_spellinfo(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		lpi;
    langp_T	*lp;
    char_u	*p;

    ಯದಿ (no_spell_checking(curwin))
	ಪ್ರತ್ಯಾಗಮ;

    ಸಂದೇಶ_start();
    ಪ್ರತಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	ಸಂದೇಶ_puts("file: ");
	ಸಂದೇಶ_puts((ವರ್ಣ *)lp->lp_slang->sl_fname);
	ಸಂದೇಶ_putchar('\n');
	p = lp->lp_slang->sl_info;
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಸಂದೇಶ_puts((ವರ್ಣ *)p);
	    ಸಂದೇಶ_putchar('\n');
	}
    }
    ಸಂದೇಶ_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_ಎಣಿಕೆ	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    ಶೂನ್ಯ
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    ಯದಿ (no_spell_checking(curwin))
	ಪ್ರತ್ಯಾಗಮ;
    (ಶೂನ್ಯ)get_option_ಮೌಲ್ಯ((char_u*)"spl", &dummy, &spl, ರಿಕ್ತ, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_ಮೌಲ್ಯ_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_ಮೌಲ್ಯ_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ಯದಿ (!BUFEMPTY())
	ಪ್ರತ್ಯಾಗಮ;

    spell_dump_compl(ರಿಕ್ತ, 0, ರಿಕ್ತ, eap->forceit ? DUMPFLAG_ಎಣಿಕೆ : 0);

    // Delete the empty line that we started with.
    ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > 1)
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
    ಪೂರ್ಣಾಙ್ಕ		ic,	    // ignore case
    ಪೂರ್ಣಾಙ್ಕ		*dir,	    // direction for adding matches
    ಪೂರ್ಣಾಙ್ಕ		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		round;
    ಪೂರ್ಣಾಙ್ಕ		depth;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    char_u	*region_names = ರಿಕ್ತ;	    // region names being used
    ಪೂರ್ಣಾಙ್ಕ		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		lpi;
    ಪೂರ್ಣಾಙ್ಕ		dumpflags = dumpflags_arg;
    ಪೂರ್ಣಾಙ್ಕ		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ಯದಿ (pat != ರಿಕ್ತ)
    {
	ಯದಿ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	ಅನ್ಯಥಾ
	{
	    n = captype(pat, ರಿಕ್ತ);
	    ಯದಿ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    ಅನ್ಯಥಾ ಯದಿ (n == WF_ALLCAP && (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    ಪ್ರತಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ಯದಿ (p[0] != 0)
	{
	    ಯದಿ (region_names == ರಿಕ್ತ)	    // first language with regions
		region_names = p;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (do_region && region_names != ರಿಕ್ತ && pat == ರಿಕ್ತ)
    {
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    ಅನ್ಯಥಾ
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    ಪ್ರತಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ಯದಿ (slang->sl_fbyts == ರಿಕ್ತ)	    // reloading failed
	    ಅನುವರ್ತ;

	ಯದಿ (pat == ರಿಕ್ತ)
	{
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ಯದಿ (pat != ರಿಕ್ತ && slang->sl_pbyts == ರಿಕ್ತ)
	    patlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
	ಅನ್ಯಥಾ
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	ಪ್ರತಿ (round = 1; round <= 2; ++round)
	{
	    ಯದಿ (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    ಅನ್ಯಥಾ
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ಯದಿ (byts == ರಿಕ್ತ)
		ಅನುವರ್ತ;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ಯಾವತ್ (depth >= 0 && !got_int
				  && (pat == ರಿಕ್ತ || !ins_compl_interrupted()))
	    {
		ಯದಿ (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    ಸಾಲು_breakcheck();
		    ins_compl_check_ಕೀಲಿಗಳು(50, FALSE);
		}
		ಅನ್ಯಥಾ
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    ಯದಿ (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (ಪೂರ್ಣಾಙ್ಕ)idxs[n];
			ಯದಿ ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    ಯದಿ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    ಯದಿ (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				ಯದಿ (pat == ರಿಕ್ತ)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ಯದಿ (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    ಅನ್ಯಥಾ
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
			ಯದಿ (depth <= patlen
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
    ಸ್ಥಿರ ಶೂನ್ಯ
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		*dir,
    ಪೂರ್ಣಾಙ್ಕ		dumpflags,
    ಪೂರ್ಣಾಙ್ಕ		wordflags,
    linenr_T	lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		flags = wordflags;

    ಯದಿ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ಯದಿ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ಯದಿ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    ಅನ್ಯಥಾ
    {
	p = word;
	ಯದಿ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, ರಿಕ್ತ) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ಯದಿ (pat == ರಿಕ್ತ)
    {
	// Add flags and regions after a slash.
	ಯದಿ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ಯದಿ (keepcap)
		STRCAT(badword, "=");
	    ಯದಿ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    ಅನ್ಯಥಾ ಯದಿ (flags & WF_RARE)
		STRCAT(badword, "?");
	    ಯದಿ (flags & WF_REGION)
		ಪ್ರತಿ (i = 0; i < 7; ++i)
		    ಯದಿ (flags & (0x10000 << i))
			sprintf((ವರ್ಣ *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ಯದಿ (dumpflags & DUMPFLAG_ಎಣಿಕೆ)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_ಎಣಿಕೆ);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p),
					  p_ic, ರಿಕ್ತ, *dir, FALSE, 0) == OK)
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
    char_u	*word,	    // case-folded word
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		*dir,
    ಪೂರ್ಣಾಙ್ಕ		dumpflags,
    ಪೂರ್ಣಾಙ್ಕ		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		has_word_up = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    ಪೂರ್ಣಾಙ್ಕ		depth;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    ಯದಿ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ಯದಿ (byts != ರಿಕ್ತ)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ಯಾವತ್ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    ಯದಿ (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		ಸಾಲು_breakcheck();
	    }
	    ಅನ್ಯಥಾ
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		ಯದಿ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    ಪ್ರತಿ (i = 1; i < len; ++i)
			ಯದಿ (byts[n + i] != 0)
			    ಭಙ್ಗ;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    ಯದಿ (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ಯದಿ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ಯದಿ (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			ಯದಿ (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ಯದಿ (lnum != 0)
				++lnum;
			}
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    ಯಾವತ್ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_word_start(ಪೂರ್ಣಾಙ್ಕ startcol)
{
    char_u	*ಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;

    ಯದಿ (no_spell_checking(curwin))
	ಪ್ರತ್ಯಾಗಮ startcol;

    // Find a word character before "startcol".
    ಸಾಲು = ml_get_curline();
    ಪ್ರತಿ (p = ಸಾಲು + startcol; p > ಸಾಲು; )
    {
	MB_PTR_BACK(ಸಾಲು, p);
	ಯದಿ (spell_iswordp_nmw(p, curwin))
	    ಭಙ್ಗ;
    }

    // Go back to start of the word.
    ಯಾವತ್ (p > ಸಾಲು)
    {
	col = (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
	MB_PTR_BACK(ಸಾಲು, p);
	ಯದಿ (!spell_iswordp(p, curwin))
	    ಭಙ್ಗ;
	col = 0;
    }

    ಪ್ರತ್ಯಾಗಮ col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spell_expand_need_cap;

    ಶೂನ್ಯ
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
    ಪೂರ್ಣಾಙ್ಕ
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    ಪ್ರತ್ಯಾಗಮ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_spelllang(char_u *val)
{
    ಪ್ರತ್ಯಾಗಮ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		l;

    spf = val;
    ಯಾವತ್ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ಯದಿ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಪ್ರತಿ (s = spf_name; *s != NUL; ++s)
	    ಯದಿ (!vim_is_fname_char(*s))
		ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    ವರ್ಣ *
did_set_spell_option(ಶೂನ್ಯ)
{
    ವರ್ಣ    *errmsg = ರಿಕ್ತ;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    ಭಙ್ಗ;
	}
    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    ವರ್ಣ *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    ಯದಿ (synblock->b_p_spc == ರಿಕ್ತ || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ಯದಿ (re != ರಿಕ್ತ)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ಯದಿ (synblock->b_cap_prog == ರಿಕ್ತ)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif  // FEAT_SPELL
