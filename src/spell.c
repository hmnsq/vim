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

#నిర్వచించు లోపల_SPELL_C
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SPELL)

#అన్తర్భావ <time.h>

#నిర్వచించు REGION_ALL 0xff		// word valid in all regions

// Result values.  Lower number is accepted over higher one.
#నిర్వచించు SP_BANNED	(-1)
#నిర్వచించు SP_OK		0
#నిర్వచించు SP_RARE		1
#నిర్వచించు SP_LOCAL	2
#నిర్వచించు SP_BAD		3

/*
 * Structure to store info for word matching.
 */
రకనిర్వచనం నిర్మాణం matchinf_S
{
    langp_T	*mi_lp;			// info for language and region

    // pointers to original text to be checked
    char_u	*mi_మాట;		// start of word being checked
    char_u	*mi_చివర;		// end of matching word so far
    char_u	*mi_fend;		// next char to be added to mi_fword
    char_u	*mi_cend;		// char after what was used for
					// mi_capflags

    // case-folded text
    char_u	mi_fword[MAXWLEN + 1];	// mi_word case-folded
    పూర్ణాంకం		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    పూర్ణాంకం		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    పూర్ణాంకం		mi_prefcnt;		// number of entries at mi_prefarridx
    పూర్ణాంకం		mi_prefixlen;		// byte length of prefix
    పూర్ణాంకం		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    పూర్ణాంకం		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    పూర్ణాంకం		mi_complen;		// nr of compound words used
    పూర్ణాంకం		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    పూర్ణాంకం		mi_ఫలితం;		// result so far: SP_BAD, SP_OK, etc.
    పూర్ణాంకం		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    పూర్ణాంకం		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


స్థిర పూర్ణాంకం spell_mb_isword_class(పూర్ణాంకం cl, win_T *wp);

// mode values for find_word
#నిర్వచించు కనుగొను_FOLDWORD	    0	// find word case-folded
#నిర్వచించు కనుగొను_KEEPWORD	    1	// find keep-case word
#నిర్వచించు కనుగొను_ఉపసర్గ	    2	// find word after prefix
#నిర్వచించు కనుగొను_COMPOUND	    3	// find case-folded compound word
#నిర్వచించు కనుగొను_KEEPCOMPOUND   4	// find keep-case compound word

// type values for get_char_type
#నిర్వచించు CHAR_OTHER	0
#నిర్వచించు CHAR_పై	1
#నిర్వచించు CHAR_DIGIT	2

స్థిర శూన్య కనుగొను_మాట(matchinf_T *mip, పూర్ణాంకం బహుళకం);
స్థిర శూన్య కనుగొను_ఉపసర్గ_2(matchinf_T *mip, పూర్ణాంకం బహుళకం);
స్థిర పూర్ణాంకం fold_మరిన్ని(matchinf_T *mip);
స్థిర శూన్య spell_ఎక్కించు_cb(char_u *fname, శూన్య *cookie);
స్థిర పూర్ణాంకం లెక్క_syllables(slang_T *slang, char_u *మాట);
స్థిర శూన్య clear_midword(win_T *buf);
స్థిర శూన్య use_midword(slang_T *lp, win_T *buf);
స్థిర పూర్ణాంకం కనుగొను_region(char_u *rp, char_u *region);
స్థిర శూన్య spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
స్థిర శూన్య spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
స్థిర శూన్య spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
స్థిర శూన్య dump_మాట(slang_T *slang, char_u *మాట, char_u *pat, పూర్ణాంకం *dir, పూర్ణాంకం విడత, పూర్ణాంకం flags, linenr_T lnum);
స్థిర linenr_T dump_prefixes(slang_T *slang, char_u *మాట, char_u *pat, పూర్ణాంకం *dir, పూర్ణాంకం విడత, పూర్ణాంకం flags, linenr_T startlnum);
స్థిర char_u *advance_camelcase_మాట(char_u *p, win_T *wp, పూర్ణాంకం *is_camel_case);

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
    పూర్ణాంకం
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    పూర్ణాంకం		*capcol,	// column to check for Capital
    పూర్ణాంకం		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    పూర్ణాంకం		nrlen = 0;	// found a number first
    పూర్ణాంకం		c;
    పూర్ణాంకం		wrongcaplen = 0;
    పూర్ణాంకం		lpi;
    పూర్ణాంకం		లెక్క_మాట = docount;
    పూర్ణాంకం		use_camel_case = *wp->w_s->b_p_spo != NUL;
    పూర్ణాంకం		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ఒకవేళ (*ptr <= ' ')
	తిరిగి 1;

    // Return here when loading language files failed.
    ఒకవేళ (wp->w_s->b_langp.ga_len == 0)
	తిరిగి 1;

    CLEAR_క్షేత్రం(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ఒకవేళ (*ptr >= '0' && *ptr <= '9')
    {
	ఒకవేళ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_చివర = skipbin(ptr + 2);
	లేకపోతే ఒకవేళ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_చివర = skiphex(ptr + 2);
	లేకపోతే
	    mi.mi_చివర = skipdigits(ptr);
	nrlen = (పూర్ణాంకం)(mi.mi_చివర - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_మాట = ptr;
    mi.mi_fend = ptr;
    ఒకవేళ (spell_iswordp(mi.mi_fend, wp))
    {
	ఒకవేళ (use_camel_case)
	    mi.mi_fend = advance_camelcase_మాట(ptr, wp, &is_camel_case);
	లేకపోతే
	{
	    చేయి
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } అయితే (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ఒకవేళ (capcol != ఖాళీ && *capcol == 0 && wp->w_s->b_cap_prog != ఖాళీ)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ఒకవేళ (!SPELL_ISUPPER(c))
		wrongcaplen = (పూర్ణాంకం)(mi.mi_fend - ptr);
	}
    }
    ఒకవేళ (capcol != ఖాళీ)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_చివర = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ఖాళీ;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    ఒకవేళ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (శూన్య)spell_casefold(wp, ptr, (పూర్ణాంకం)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (పూర్ణాంకం)STRLEN(mi.mi_fword);

    ఒకవేళ (is_camel_case && mi.mi_fwordlen > 0)
	// Introduce a fake word end space into the folded word.
	mi.mi_fword[mi.mi_fwordlen - 1] = ' ';

    // The word is bad unless we recognize it.
    mi.mi_ఫలితం = SP_BAD;
    mi.mi_result2 = SP_BAD;

    /*
     * Loop over the languages specified in 'spelllang'.
     * We check them all, because a word may be matched longer in another
     * language.
     */
    కొరకు (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_నమోదు(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ఒకవేళ (mi.mi_lp->lp_slang->sl_fidxs == ఖాళీ)
	    కొనసాగు;

	// Check for a matching word in case-folded words.
	కనుగొను_మాట(&mi, కనుగొను_FOLDWORD);

	// Check for a matching word in keep-case words.
	కనుగొను_మాట(&mi, కనుగొను_KEEPWORD);

	// Check for matching prefixes.
	కనుగొను_ఉపసర్గ_2(&mi, కనుగొను_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ఒకవేళ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_ఫలితం == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_ఫలితం = mi.mi_result2;
	    mi.mi_చివర = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ఒకవేళ (లెక్క_మాట && mi.mi_ఫలితం == SP_OK)
	{
	    లెక్క_common_మాట(mi.mi_lp->lp_slang, ptr,
						   (పూర్ణాంకం)(mi.mi_చివర - ptr), 1);
	    లెక్క_మాట = FALSE;
	}
    }

    ఒకవేళ (mi.mi_ఫలితం != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ఒకవేళ (nrlen > 0)
	{
	    ఒకవేళ (mi.mi_ఫలితం == SP_BAD || mi.mi_ఫలితం == SP_BANNED)
		తిరిగి nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	లేకపోతే ఒకవేళ (!spell_iswordp_nmw(ptr, wp))
	{
	    ఒకవేళ (capcol != ఖాళీ && wp->w_s->b_cap_prog != ఖాళీ)
	    {
		regmatch_T	regmatch;
		పూర్ణాంకం		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ఒకవేళ (r)
		    *capcol = (పూర్ణాంకం)(regmatch.endp[0] - ptr);
	    }

	    ఒకవేళ (has_mbyte)
		తిరిగి (*mb_ptr2len)(ptr);
	    తిరిగి 1;
	}
	లేకపోతే ఒకవేళ (mi.mi_చివర == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_చివర);
	లేకపోతే ఒకవేళ (mi.mi_ఫలితం == SP_BAD
		&& LANGP_నమోదు(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    పూర్ణాంకం		save_ఫలితం = mi.mi_ఫలితం;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_నమోదు(wp->w_s->b_langp, 0);
	    ఒకవేళ (mi.mi_lp->lp_slang->sl_fidxs != ఖాళీ)
	    {
		p = mi.mi_మాట;
		fp = mi.mi_fword;
		కొరకు (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ఒకవేళ (p >= mi.mi_చివర)
			ఆపు;
		    mi.mi_compoff = (పూర్ణాంకం)(fp - mi.mi_fword);
		    కనుగొను_మాట(&mi, కనుగొను_COMPOUND);
		    ఒకవేళ (mi.mi_ఫలితం != SP_BAD)
		    {
			mi.mi_చివర = p;
			ఆపు;
		    }
		}
		mi.mi_ఫలితం = save_ఫలితం;
	    }
	}

	ఒకవేళ (mi.mi_ఫలితం == SP_BAD || mi.mi_ఫలితం == SP_BANNED)
	    *attrp = HLF_SPB;
	లేకపోతే ఒకవేళ (mi.mi_ఫలితం == SP_RARE)
	    *attrp = HLF_SPR;
	లేకపోతే
	    *attrp = HLF_SPL;
    }

    ఒకవేళ (wrongcaplen > 0 && (mi.mi_ఫలితం == SP_OK || mi.mi_ఫలితం == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	తిరిగి wrongcaplen;
    }

    తిరిగి (పూర్ణాంకం)(mi.mi_చివర - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    స్థిర పూర్ణాంకం
get_char_type(పూర్ణాంకం c)
{
    ఒకవేళ (VIM_ISDIGIT(c))
	తిరిగి CHAR_DIGIT;
    ఒకవేళ (SPELL_ISUPPER(c))
	తిరిగి CHAR_పై;
    తిరిగి CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    స్థిర char_u *
advance_camelcase_మాట(char_u *str, win_T *wp, పూర్ణాంకం *is_camel_case)
{
    పూర్ణాంకం చివరి_type, చివరి_చివరి_type, this_type;
    పూర్ణాంకం c;
    char_u *చివర = str;

    *is_camel_case = FALSE;

    ఒకవేళ (*str == NUL)
	తిరిగి str;

    c = PTR2CHAR(చివర);
    MB_PTR_ADV(చివర);
    // We need at most the types of the type of the last two chars.
    చివరి_చివరి_type = -1;
    చివరి_type = get_char_type(c);

    అయితే (*చివర != NUL && spell_iswordp(చివర, wp))
    {
	c = PTR2CHAR(చివర);
	this_type = get_char_type(c);

	ఒకవేళ (చివరి_చివరి_type == CHAR_పై && చివరి_type == CHAR_పై
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_వెనుక(str, చివర);
	    ఆపు;
	}
	లేకపోతే ఒకవేళ ((this_type == CHAR_పై && చివరి_type == CHAR_OTHER)
		|| (this_type != చివరి_type
		    && (this_type == CHAR_DIGIT || చివరి_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    ఆపు;
	}

	చివరి_చివరి_type = చివరి_type;
	చివరి_type = this_type;

	MB_PTR_ADV(చివర);
    }

    తిరిగి చివర;
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
    స్థిర శూన్య
కనుగొను_మాట(matchinf_T *mip, పూర్ణాంకం బహుళకం)
{
    idx_T	arridx = 0;
    పూర్ణాంకం		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    పూర్ణాంకం		endidxcnt = 0;
    పూర్ణాంకం		len;
    పూర్ణాంకం		wlen = 0;
    పూర్ణాంకం		flen;
    పూర్ణాంకం		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    పూర్ణాంకం		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    గుర్తులేని	flags;
    char_u	*byts;
    idx_T	*idxs;
    పూర్ణాంకం		మాట_చివర్లు;
    పూర్ణాంకం		ఉపసర్గ_దొరికింది;
    పూర్ణాంకం		nobreak_ఫలితం;

    ఒకవేళ (బహుళకం == కనుగొను_KEEPWORD || బహుళకం == కనుగొను_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_మాట;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ఒకవేళ (బహుళకం == కనుగొను_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    లేకపోతే
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ఒకవేళ (బహుళకం == కనుగొను_ఉపసర్గ)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	లేకపోతే ఒకవేళ (బహుళకం == కనుగొను_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ఒకవేళ (byts == ఖాళీ)
	తిరిగి;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    కొరకు (;;)
    {
	ఒకవేళ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_మరిన్ని(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ఒకవేళ (byts[arridx] == 0)
	{
	    ఒకవేళ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_లోపల_spell_file));
		తిరిగి;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    అయితే (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ఒకవేళ (len == 0)
		ఆపు;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ఒకవేళ (ptr[wlen] == NUL)
	    ఆపు;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ఒకవేళ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	అయితే (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ఒకవేళ (byts[m] > c)
		hi = m - 1;
	    లేకపోతే ఒకవేళ (byts[m] < c)
		lo = m + 1;
	    లేకపోతే
	    {
		lo = hi = m;
		ఆపు;
	    }
	}

	// Stop if there is no matching byte.
	ఒకవేళ (hi < lo || byts[lo] != c)
	    ఆపు;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ఒకవేళ (c == ' ')
	{
	    కొరకు (;;)
	    {
		ఒకవేళ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_మరిన్ని(mip);
		ఒకవేళ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    ఆపు;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    అయితే (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ఒకవేళ ((*mb_తల_off)(ptr, ptr + wlen) > 0)
	    కొనసాగు;	    // not at first byte of character
	ఒకవేళ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ఒకవేళ (slang->sl_compprog == ఖాళీ && !slang->sl_nobreak)
		కొనసాగు;	    // next char is a word character
	    మాట_చివర్లు = FALSE;
	}
	లేకపోతే
	    మాట_చివర్లు = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	ఉపసర్గ_దొరికింది = FALSE;

	ఒకవేళ (బహుళకం != కనుగొను_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_మాట;
	    ఒకవేళ (STRNCMP(ptr, p, wlen) != 0)
	    {
		కొరకు (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (పూర్ణాంకం)(p - mip->mi_మాట);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	కొరకు (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ఒకవేళ (బహుళకం == కనుగొను_FOLDWORD)
	    {
		ఒకవేళ (mip->mi_cend != mip->mi_మాట + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_మాట + wlen;
		    mip->mi_capflags = captype(mip->mi_మాట, mip->mi_cend);
		}

		ఒకవేళ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    కొనసాగు;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    లేకపోతే ఒకవేళ (బహుళకం == కనుగొను_ఉపసర్గ && !ఉపసర్గ_దొరికింది)
	    {
		c = valid_మాట_ఉపసర్గ(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_మాట + mip->mi_cprefixlen, slang,
				    FALSE);
		ఒకవేళ (c == 0)
		    కొనసాగు;

		// Use the WF_RARE flag for a rare prefix.
		ఒకవేళ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		ఉపసర్గ_దొరికింది = TRUE;
	    }

	    ఒకవేళ (slang->sl_nobreak)
	    {
		ఒకవేళ ((బహుళకం == కనుగొను_COMPOUND || బహుళకం == కనుగొను_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_ఫలితం = SP_OK;
		    ఆపు;
		}
	    }

	    లేకపోతే ఒకవేళ ((బహుళకం == కనుగొను_COMPOUND || బహుళకం == కనుగొను_KEEPCOMPOUND
								|| !మాట_చివర్లు))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ఒకవేళ (((గుర్తులేని)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    కొనసాగు;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ఒకవేళ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_మాట + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			కొనసాగు;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ఒకవేళ (!మాట_చివర్లు && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    కొనసాగు;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ఒకవేళ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    కొనసాగు;
		ఒకవేళ (!మాట_చివర్లు && (flags & WF_NOCOMPAFT))
		    కొనసాగు;

		// Quickly check if compounding is possible with this flag.
		ఒకవేళ (!byte_లోపల_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((గుర్తులేని)flags >> 24)))
		    కొనసాగు;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ఒకవేళ (సరిపోలిక_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    కొనసాగు;

		ఒకవేళ (బహుళకం == కనుగొను_COMPOUND)
		{
		    పూర్ణాంకం	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ఒకవేళ (has_mbyte && STRNCMP(ptr, mip->mi_మాట,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_మాట;
			కొరకు (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    లేకపోతే
			p = mip->mi_మాట + mip->mi_compoff;
		    capflags = captype(p, mip->mi_మాట + wlen);
		    ఒకవేళ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			కొనసాగు;

		    ఒకవేళ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_వెనుక(mip->mi_మాట, p);
			ఒకవేళ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    కొనసాగు;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((గుర్తులేని)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		ఒకవేళ (మాట_చివర్లు)
		{
		    char_u	fword[MAXWLEN];

		    ఒకవేళ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ఒకవేళ (ptr == mip->mi_మాట)
			    (శూన్య)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			లేకపోతే
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ఒకవేళ (!can_compound(slang, fword, mip->mi_compflags))
			కొనసాగు;
		}
		లేకపోతే ఒకవేళ (slang->sl_comprules != ఖాళీ
			     && !సరిపోలిక_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    కొనసాగు;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    లేకపోతే ఒకవేళ (flags & WF_NEEDCOMP)
		కొనసాగు;

	    nobreak_ఫలితం = SP_OK;

	    ఒకవేళ (!మాట_చివర్లు)
	    {
		పూర్ణాంకం	save_ఫలితం = mip->mi_ఫలితం;
		char_u	*save_చివర = mip->mi_చివర;
		langp_T	*save_lp = mip->mi_lp;
		పూర్ణాంకం	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ఒకవేళ (slang->sl_nobreak)
		    mip->mi_ఫలితం = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ఒకవేళ (has_mbyte && బహుళకం == కనుగొను_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ఒకవేళ (STRNCMP(ptr, p, wlen) != 0)
		    {
			కొరకు (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (పూర్ణాంకం)(p - mip->mi_fword);
		    }
		}
#ఒకవేళ 0 // Disabled, see below
		c = mip->mi_compoff;
#అయితే_ముగింపు
		++mip->mi_complen;
		ఒకవేళ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		కొరకు (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ఒకవేళ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_నమోదు(mip->mi_win->w_s->b_langp, lpi);
			ఒకవేళ (mip->mi_lp->lp_slang->sl_fidxs == ఖాళీ
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    కొనసాగు;
		    }

		    కనుగొను_మాట(mip, కనుగొను_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ఒకవేళ (!slang->sl_nobreak || mip->mi_ఫలితం == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			కనుగొను_మాట(mip, కనుగొను_KEEPCOMPOUND);

#ఒకవేళ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ఒకవేళ (!slang->sl_nobreak || mip->mi_ఫలితం == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    కనుగొను_ఉపసర్గ_2(mip, కనుగొను_COMPOUND);
			}
#అయితే_ముగింపు
		    }

		    ఒకవేళ (!slang->sl_nobreak)
			ఆపు;
		}
		--mip->mi_complen;
		ఒకవేళ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ఒకవేళ (slang->sl_nobreak)
		{
		    nobreak_ఫలితం = mip->mi_ఫలితం;
		    mip->mi_ఫలితం = save_ఫలితం;
		    mip->mi_చివర = save_చివర;
		}
		లేకపోతే
		{
		    ఒకవేళ (mip->mi_ఫలితం == SP_OK)
			ఆపు;
		    కొనసాగు;
		}
	    }

	    ఒకవేళ (flags & WF_BANNED)
		res = SP_BANNED;
	    లేకపోతే ఒకవేళ (flags & WF_REGION)
	    {
		// Check region.
		ఒకవేళ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		లేకపోతే
		    res = SP_LOCAL;
	    }
	    లేకపోతే ఒకవేళ (flags & WF_RARE)
		res = SP_RARE;
	    లేకపోతే
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ఒకవేళ (nobreak_ఫలితం == SP_BAD)
	    {
		ఒకవేళ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_మాట + wlen;
		}
		లేకపోతే ఒకవేళ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_మాట + wlen)
		    mip->mi_end2 = mip->mi_మాట + wlen;
	    }
	    లేకపోతే ఒకవేళ (mip->mi_ఫలితం > res)
	    {
		mip->mi_ఫలితం = res;
		mip->mi_చివర = mip->mi_మాట + wlen;
	    }
	    లేకపోతే ఒకవేళ (mip->mi_ఫలితం == res && mip->mi_చివర < mip->mi_మాట + wlen)
		mip->mi_చివర = mip->mi_మాట + wlen;

	    ఒకవేళ (mip->mi_ఫలితం == SP_OK)
		ఆపు;
	}

	ఒకవేళ (mip->mi_ఫలితం == SP_OK)
	    ఆపు;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    పూర్ణాంకం
సరిపోలిక_checkcompoundpattern(
    char_u	*ptr,
    పూర్ణాంకం		wlen,
    garray_T	*gap)  // &sl_comppat
{
    పూర్ణాంకం		i;
    char_u	*p;
    పూర్ణాంకం		len;

    కొరకు (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_దత్తాంశం)[i + 1];
	ఒకవేళ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_దత్తాంశం)[i];
	    len = (పూర్ణాంకం)STRLEN(p);
	    ఒకవేళ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    పూర్ణాంకం
can_compound(slang_T *slang, char_u *మాట, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    పూర్ణాంకం		i;
    char_u	*p;

    ఒకవేళ (slang->sl_compprog == ఖాళీ)
	తిరిగి FALSE;
    ఒకవేళ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	కొరకు (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    లేకపోతే
	p = flags;
    ఒకవేళ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	తిరిగి FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ఒకవేళ (slang->sl_compsylmax < MAXWLEN
		       && లెక్క_syllables(slang, మాట) > slang->sl_compsylmax)
	తిరిగి (పూర్ణాంకం)STRLEN(flags) < slang->sl_compmax;
    తిరిగి TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    పూర్ణాంకం
సరిపోలిక_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;

    // loop over all the COMPOUNDRULE entries
    కొరకు (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	కొరకు (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ఒకవేళ (c == NUL)
		// found a rule that matches for the flags we have so far
		తిరిగి TRUE;
	    ఒకవేళ (*p == '/' || *p == NUL)
		ఆపు;  // end of rule, it's too short
	    ఒకవేళ (*p == '[')
	    {
		పూర్ణాంకం సరిపోలిక = FALSE;

		// compare against all the flags in []
		++p;
		అయితే (*p != ']' && *p != NUL)
		    ఒకవేళ (*p++ == c)
			సరిపోలిక = TRUE;
		ఒకవేళ (!సరిపోలిక)
		    ఆపు;  // none matches
	    }
	    లేకపోతే ఒకవేళ (*p != c)
		ఆపు;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ఒకవేళ (p == ఖాళీ)
	    ఆపు;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    తిరిగి FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    పూర్ణాంకం
valid_మాట_ఉపసర్గ(
    పూర్ణాంకం		totprefcnt,	// nr of prefix IDs
    పూర్ణాంకం		arridx,		// idx in sl_pidxs[]
    పూర్ణాంకం		flags,
    char_u	*మాట,
    slang_T	*slang,
    పూర్ణాంకం		cond_req)	// only use prefixes with a condition
{
    పూర్ణాంకం		prefcnt;
    పూర్ణాంకం		pidx;
    regprog_T	**rp;
    పూర్ణాంకం		prefid;

    prefid = (గుర్తులేని)flags >> 24;
    కొరకు (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ఒకవేళ (prefid != (pidx & 0xff))
	    కొనసాగు;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ఒకవేళ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    కొనసాగు;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((గుర్తులేని)pidx >> 8) & 0xffff];
	ఒకవేళ (*rp != ఖాళీ)
	{
	    ఒకవేళ (!vim_regexec_prog(rp, FALSE, మాట, 0))
		కొనసాగు;
	}
	లేకపోతే ఒకవేళ (cond_req)
	    కొనసాగు;

	// It's a match!  Return the WF_ flags.
	తిరిగి pidx;
    }
    తిరిగి 0;
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
    స్థిర శూన్య
కనుగొను_ఉపసర్గ_2(matchinf_T *mip, పూర్ణాంకం బహుళకం)
{
    idx_T	arridx = 0;
    పూర్ణాంకం		len;
    పూర్ణాంకం		wlen = 0;
    పూర్ణాంకం		flen;
    పూర్ణాంకం		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ఒకవేళ (byts == ఖాళీ)
	తిరిగి;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ఒకవేళ (బహుళకం == కనుగొను_COMPOUND)
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
    కొరకు (;;)
    {
	ఒకవేళ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_మరిన్ని(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ఒకవేళ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    అయితే (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ఒకవేళ (బహుళకం == కనుగొను_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ఒకవేళ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_మాట);
	    }
	    లేకపోతే
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    కనుగొను_మాట(mip, కనుగొను_ఉపసర్గ);


	    ఒకవేళ (len == 0)
		ఆపు;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ఒకవేళ (ptr[wlen] == NUL)
	    ఆపు;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	అయితే (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ఒకవేళ (byts[m] > c)
		hi = m - 1;
	    లేకపోతే ఒకవేళ (byts[m] < c)
		lo = m + 1;
	    లేకపోతే
	    {
		lo = hi = m;
		ఆపు;
	    }
	}

	// Stop if there is no matching byte.
	ఒకవేళ (hi < lo || byts[lo] != c)
	    ఆపు;

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
    స్థిర పూర్ణాంకం
fold_మరిన్ని(matchinf_T *mip)
{
    పూర్ణాంకం		flen;
    char_u	*p;

    p = mip->mi_fend;
    చేయి
	MB_PTR_ADV(mip->mi_fend);
    అయితే (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ఒకవేళ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (శూన్య)spell_casefold(mip->mi_win, p, (పూర్ణాంకం)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (పూర్ణాంకం)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    తిరిగి flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    పూర్ణాంకం
spell_valid_case(
    పూర్ణాంకం	    wordflags,	    // flags for the checked word.
    పూర్ణాంకం	    treeflags)	    // flags for the word in the spell tree
{
    తిరిగి ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    పూర్ణాంకం
spell_check_కిటికీ(win_T *wp)
{
    తిరిగి wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(అక్షరం **)(wp->w_s->b_langp.ga_దత్తాంశం) != ఖాళీ;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    స్థిర పూర్ణాంకం
no_spell_checking(win_T *wp)
{
    ఒకవేళ (spell_check_కిటికీ(wp))
	తిరిగి FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    తిరిగి TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    పూర్ణాంకం
spell_move_గమ్యం(
    win_T	*wp,
    పూర్ణాంకం		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    పూర్ణాంకం		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	దొరికింది_pos;
    పూర్ణాంకం		దొరికింది_len = 0;
    char_u	*పంక్తి;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    పూర్ణాంకం		len;
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		has_syntax = syntax_present(wp);
#అయితే_ముగింపు
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		can_spell;
    char_u	*buf = ఖాళీ;
    పూర్ణాంకం		buflen = 0;
    పూర్ణాంకం		skip = 0;
    పూర్ణాంకం		capcol = -1;
    పూర్ణాంకం		దొరికింది_one = FALSE;
    పూర్ణాంకం		wrapped = FALSE;

    ఒకవేళ (no_spell_checking(wp))
	తిరిగి 0;

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
    CLEAR_POS(&దొరికింది_pos);

    అయితే (!got_int)
    {
	పూర్ణాంకం ఖాళీలు_పంక్తి;

	పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);

	len = ml_get_buf_len(wp->w_బఫరు, lnum);
	ఒకవేళ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ఒకవేళ (buf == ఖాళీ)
		ఆపు;
	}

	// In first line check first word for Capital.
	ఒకవేళ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ఒకవేళ (capcol == 0)
	    capcol = getwhitecols(పంక్తి);
	లేకపోతే ఒకవేళ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    నిలువు = getwhitecols(పంక్తి);
	    ఒకవేళ (check_need_cap(curwin, lnum, నిలువు))
		capcol = నిలువు;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	ఖాళీలు_పంక్తి = *skipwhite(పంక్తి) == NUL;
	STRCPY(buf, పంక్తి);
	ఒకవేళ (lnum < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    spell_cat_పంక్తి(buf + STRLEN(buf),
			  ml_get_buf(wp->w_బఫరు, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	అయితే (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ఒకవేళ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.నిలువు)
		ఆపు;

	    // start of word
	    attr = HLF_లెక్క;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ఒకవేళ (attr != HLF_లెక్క)
	    {
		// We found a bad word.  Check the attribute.
		ఒకవేళ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ఒకవేళ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.నిలువు))
		    {
#నిర్వచితమైతే FEAT_SYN_HL
			ఒకవేళ (has_syntax)
			{
			    నిలువు = (పూర్ణాంకం)(p - buf);
			    (శూన్య)syn_get_id(wp, lnum, (colnr_T)నిలువు,
						    FALSE, &can_spell, FALSE);
			    ఒకవేళ (!can_spell)
				attr = HLF_లెక్క;
			}
			లేకపోతే
#అయితే_ముగింపు
			    can_spell = TRUE;

			ఒకవేళ (can_spell)
			{
			    దొరికింది_one = TRUE;
			    దొరికింది_pos.lnum = lnum;
			    దొరికింది_pos.నిలువు = (పూర్ణాంకం)(p - buf);
			    దొరికింది_pos.coladd = 0;
			    ఒకవేళ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = దొరికింది_pos;
				vim_free(buf);
				ఒకవేళ (attrp != ఖాళీ)
				    *attrp = attr;
				తిరిగి len;
			    }
			    లేకపోతే ఒకవేళ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				దొరికింది_pos.నిలువు += len;
			    దొరికింది_len = len;
			}
		    }
		    లేకపోతే
			దొరికింది_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ఒకవేళ (dir == BACKWARD && దొరికింది_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = దొరికింది_pos;
	    vim_free(buf);
	    తిరిగి దొరికింది_len;
	}

	ఒకవేళ (curline)
	    ఆపు;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ఒకవేళ (lnum == wp->w_cursor.lnum && wrapped)
	    ఆపు;

	// Advance to next line.
	ఒకవేళ (dir == BACKWARD)
	{
	    ఒకవేళ (lnum > 1)
		--lnum;
	    లేకపోతే ఒకవేళ (!p_ws)
		ఆపు;	    // at first line and 'nowrapscan'
	    లేకపోతే
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
		wrapped = TRUE;
		ఒకవేళ (!shortmess(SHM_శోధన))
		    give_warning((char_u *)_(శిఖరం_bot_సందేశం), TRUE);
	    }
	    capcol = -1;
	}
	లేకపోతే
	{
	    ఒకవేళ (lnum < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
		++lnum;
	    లేకపోతే ఒకవేళ (!p_ws)
		ఆపు;	    // at first line and 'nowrapscan'
	    లేకపోతే
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ఒకవేళ (!shortmess(SHM_శోధన))
		    give_warning((char_u *)_(bot_శిఖరం_సందేశం), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ఒకవేళ (lnum == wp->w_cursor.lnum && !దొరికింది_one)
		ఆపు;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ఒకవేళ (attr == HLF_లెక్క)
		skip = (పూర్ణాంకం)(p - endp);
	    లేకపోతే
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ఒకవేళ (ఖాళీలు_పంక్తి)
		capcol = 0;
	}

	పంక్తి_breakcheck();
    }

    vim_free(buf);
    తిరిగి 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    శూన్య
spell_cat_పంక్తి(char_u *buf, char_u *పంక్తి, పూర్ణాంకం maxlen)
{
    char_u	*p;
    పూర్ణాంకం		n;

    p = skipwhite(పంక్తి);
    అయితే (vim_strchr((char_u *)"*#/\"\t", *p) != ఖాళీ)
	p = skipwhite(p + 1);

    ఒకవేళ (*p == NUL)
	తిరిగి;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (పూర్ణాంకం)(p - పంక్తి) + 1;
    ఒకవేళ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
రకనిర్వచనం నిర్మాణం spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    పూర్ణాంకం	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    స్థిర శూన్య
spell_ఎక్కించు_lang(char_u *lang)
{
    char_u	fname_enc[85];
    పూర్ణాంకం		r;
    spelload_T	sl;
    పూర్ణాంకం		విడత;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ఖాళీ;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((అక్షరం *)fname_enc, పరిమాణం(fname_enc) - 5,
#నిర్వచితమైతే VMS
					"spell/%s_%s.spl",
#లేకపోతే
					"spell/%s.%s.spl",
#అయితే_ముగింపు
							   lang, spell_enc());
	r = do_లోపల_runtimepath(fname_enc, 0, spell_ఎక్కించు_cb, &sl);

	ఒకవేళ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((అక్షరం *)fname_enc, పరిమాణం(fname_enc) - 5,
#నిర్వచితమైతే VMS
						  "spell/%s_ascii.spl",
#లేకపోతే
						  "spell/%s.ascii.spl",
#అయితే_ముగింపు
									lang);
	    r = do_లోపల_runtimepath(fname_enc, 0, spell_ఎక్కించు_cb, &sl);

	    ఒకవేళ (r == FAIL && *sl.sl_lang != NUL && విడత == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		కొనసాగు;
	    ఆపు;
	}
	ఆపు;
    }

    ఒకవేళ (r == FAIL)
    {
	smsg(
#నిర్వచితమైతే VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#లేకపోతే
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#అయితే_ముగింపు
						     lang, spell_enc(), lang);
    }
    లేకపోతే ఒకవేళ (sl.sl_slang != ఖాళీ)
    {
	// At least one file was loaded, now load ALL the additions.
	STRCPY(fname_enc + STRLEN(fname_enc) - 3, "add.spl");
	do_లోపల_runtimepath(fname_enc, DIP_ALL, spell_ఎక్కించు_cb, &sl);
    }

    --curbuf->b_locked;
}

/*
 * Return the encoding used for spell checking: Use 'encoding', except that we
 * use "latin1" for "latin9".  And limit to 60 characters (just in case).
 */
    char_u *
spell_enc(శూన్య)
{

    ఒకవేళ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	తిరిగి p_enc;
    తిరిగి (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    స్థిర శూన్య
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((అక్షరం *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    ఒకవేళ (lp != ఖాళీ)
    {
	ఒకవేళ (lang != ఖాళీ)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, పరిమాణం(fromto_T), 10);
	ga_init2(&lp->sl_repsal, పరిమాణం(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    తిరిగి lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    శూన్య
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
    శూన్య
free_sal_వస్తువులు(garray_T *gap)
{
    salitem_T	*smp;

    అయితే (gap->ga_len > 0)
    {
	smp = &((salitem_T *)gap->ga_దత్తాంశం)[--gap->ga_len];
	vim_free(smp->sm_lead);
	// Don't free sm_oneof and sm_rules, they point into sm_lead.
	vim_free(smp->sm_గమ్యం);
	vim_free(smp->sm_lead_w);
	vim_free(smp->sm_oneof_w);
	vim_free(smp->sm_గమ్యం_w);
    }
    ga_clear(gap);
}

/*
 * Clear an slang_T so that the file can be reloaded.
 */
    శూన్య
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		విడత;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	gap = విడత == 1 ? &lp->sl_rep : &lp->sl_repsal;
	అయితే (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_దత్తాంశం)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_గమ్యం);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ఒకవేళ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ఒకవేళ (gap->ga_దత్తాంశం != ఖాళీ)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    కొరకు (i = 0; i < gap->ga_len; ++i)
		vim_free(((పూర్ణాంకం **)gap->ga_దత్తాంశం)[i]);
	ga_clear(gap);
    }
    లేకపోతే
	free_sal_వస్తువులు(gap);

    కొరకు (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ఖాళీ;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_వస్తువులు);

    ga_clear_అక్షరశ్రేణులు(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_కీలకం_OFF);
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
    శూన్య
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = ఖాళీ;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    స్థిర శూన్య
spell_ఎక్కించు_cb(char_u *fname, శూన్య *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_ఎక్కించు_file(fname, slp->sl_lang, ఖాళీ, FALSE);
    ఒకవేళ (slang == ఖాళీ)
	తిరిగి;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ఒకవేళ (slp->sl_nobreak && slang->sl_కూడు)
	slang->sl_nobreak = TRUE;
    లేకపోతే ఒకవేళ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    శూన్య
లెక్క_common_మాట(
    slang_T	*lp,
    char_u	*మాట,
    పూర్ణాంకం		len,	    // word length, -1 for up to NUL
    పూర్ణాంకం		లెక్క)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ఒకవేళ (len == -1)
	p = మాట;
    లేకపోతే ఒకవేళ (len >= MAXWLEN)
	తిరిగి;
    లేకపోతే
    {
	vim_strncpy(buf, మాట, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_మాట) + STRLEN(p) + 1);
	ఒకవేళ (wc == ఖాళీ)
	    తిరిగి;
	STRCPY(wc->wc_మాట, p);
	wc->wc_లెక్క = లెక్క;
	hash_కూడు_అంశం(&lp->sl_wordcount, hi, wc->wc_మాట, hash);
    }
    లేకపోతే
    {
	wc = HI2WC(hi);
	ఒకవేళ ((wc->wc_లెక్క += లెక్క) < (గుర్తులేని)లెక్క)	// check for overflow
	    wc->wc_లెక్క = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    పూర్ణాంకం
byte_లోపల_str(char_u *str, పూర్ణాంకం n)
{
    char_u	*p;

    కొరకు (p = str; *p != NUL; ++p)
	ఒకవేళ (*p == n)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

#నిర్వచించు SY_MAXLEN   30
రకనిర్వచనం నిర్మాణం syl_అంశం_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    పూర్ణాంకం		sy_len;
} syl_అంశం_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    పూర్ణాంకం
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    పూర్ణాంకం		l;
    syl_అంశం_T	*syl;

    ga_init2(&slang->sl_syl_వస్తువులు, పరిమాణం(syl_అంశం_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    అయితే (p != ఖాళీ)
    {
	*p++ = NUL;
	ఒకవేళ (*p == NUL)	    // trailing slash
	    ఆపు;
	s = p;
	p = vim_strchr(p, '/');
	ఒకవేళ (p == ఖాళీ)
	    l = (పూర్ణాంకం)STRLEN(s);
	లేకపోతే
	    l = (పూర్ణాంకం)(p - s);
	ఒకవేళ (l >= SY_MAXLEN)
	    తిరిగి SP_FORMERROR;
	ఒకవేళ (ga_grow(&slang->sl_syl_వస్తువులు, 1) == FAIL)
	    తిరిగి SP_OTHERERROR;
	syl = ((syl_అంశం_T *)slang->sl_syl_వస్తువులు.ga_దత్తాంశం)
					       + slang->sl_syl_వస్తువులు.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    తిరిగి OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    స్థిర పూర్ణాంకం
లెక్క_syllables(slang_T *slang, char_u *మాట)
{
    పూర్ణాంకం		cnt = 0;
    పూర్ణాంకం		skip = FALSE;
    char_u	*p;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    syl_అంశం_T	*syl;
    పూర్ణాంకం		c;

    ఒకవేళ (slang->sl_syllable == ఖాళీ)
	తిరిగి 0;

    కొరకు (p = మాట; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ఒకవేళ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    కొనసాగు;
	}

	// Find longest match of syllable items.
	len = 0;
	కొరకు (i = 0; i < slang->sl_syl_వస్తువులు.ga_len; ++i)
	{
	    syl = ((syl_అంశం_T *)slang->sl_syl_వస్తువులు.ga_దత్తాంశం) + i;
	    ఒకవేళ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ఒకవేళ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	లేకపోతే
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ఒకవేళ (vim_strchr(slang->sl_syllable, c) == ఖాళీ)
		skip = FALSE;	    // No, search for next syllable
	    లేకపోతే ఒకవేళ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    తిరిగి cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    అక్షరం *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    పూర్ణాంకం		filename;
    పూర్ణాంకం		region_ముసుగు;
    slang_T	*slang;
    పూర్ణాంకం		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    పూర్ణాంకం		len;
    char_u	*p;
    పూర్ణాంకం		విడత;
    char_u	*spf;
    char_u	*use_region = ఖాళీ;
    పూర్ణాంకం		dont_use_region = FALSE;
    పూర్ణాంకం		nobreak = FALSE;
    పూర్ణాంకం		i, j;
    langp_T	*lp, *lp2;
    స్థిర పూర్ణాంకం	recursive = FALSE;
    అక్షరం	*ret_సందేశం = ఖాళీ;
    char_u	*spl_copy;
    bufref_T	bufref;

    సమితి_bufref(&bufref, wp->w_బఫరు);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ఒకవేళ (recursive)
	తిరిగి ఖాళీ;
    recursive = TRUE;

    ga_init2(&ga, పరిమాణం(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ఒకవేళ (spl_copy == ఖాళీ)
	వెళ్ళు theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    కొరకు (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ఖాళీ;

	ఒకవేళ (!valid_spelllang(lang))
	    కొనసాగు;

	ఒకవేళ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    కొనసాగు;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ఒకవేళ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ఒకవేళ (p != ఖాళీ && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    లేకపోతే
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ఒకవేళ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    ఆపు;
	}
	లేకపోతే
	{
	    filename = FALSE;
	    ఒకవేళ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    లేకపోతే
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ఒకవేళ (STRICMP(lang, slang->sl_name) == 0)
		    ఆపు;
	}

	ఒకవేళ (region != ఖాళీ)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ఒకవేళ (use_region != ఖాళీ && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ఒకవేళ (slang == ఖాళీ)
	{
	    ఒకవేళ (filename)
		(శూన్య)spell_ఎక్కించు_file(lang, lang, ఖాళీ, FALSE);
	    లేకపోతే
	    {
		spell_ఎక్కించు_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ఒకవేళ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_సందేశం = N_(e_spellfilemising_autocommand_deleted_బఫరు);
		    వెళ్ళు theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    ఒకవేళ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_ముసుగు = REGION_ALL;
		ఒకవేళ (!filename && region != ఖాళీ)
		{
		    // find region in sl_regions
		    c = కనుగొను_region(slang->sl_regions, region);
		    ఒకవేళ (c == REGION_ALL)
		    {
			ఒకవేళ (slang->sl_కూడు)
			{
			    ఒకవేళ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_ముసుగు = 0;
			}
			లేకపోతే
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    లేకపోతే
			region_ముసుగు = 1 << c;
		}

		ఒకవేళ (region_ముసుగు != 0)
		{
		    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_సందేశం = e_బహిర్గతం_యొక్క_జ్ఞాపకం;
			వెళ్ళు theend;
		    }
		    LANGP_నమోదు(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_నమోదు(ga, ga.ga_len)->lp_region = region_ముసుగు;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ఒకవేళ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    కొరకు (విడత = 0; విడత == 0 || *spf != NUL; ++విడత)
    {
	ఒకవేళ (విడత == 0)
	{
	    // Internal wordlist, if there is one.
	    ఒకవేళ (int_wordlist == ఖాళీ)
		కొనసాగు;
	    int_wordlist_spl(spf_name);
	}
	లేకపోతే
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    కొరకు (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_నమోదు(ga, c)->lp_slang->sl_fname;
		ఒకవేళ (p != ఖాళీ && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    ఆపు;
	    }
	    ఒకవేళ (c < ga.ga_len)
		కొనసాగు;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ఒకవేళ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		ఆపు;
	ఒకవేళ (slang == ఖాళీ)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ఒకవేళ (విడత == 0)
		STRCPY(lang, "internal wordlist");
	    లేకపోతే
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ఒకవేళ (p != ఖాళీ)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_ఎక్కించు_file(spf_name, lang, ఖాళీ, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ఒకవేళ (slang != ఖాళీ && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ఒకవేళ (slang != ఖాళీ && ga_grow(&ga, 1) == OK)
	{
	    region_ముసుగు = REGION_ALL;
	    ఒకవేళ (use_region != ఖాళీ && !dont_use_region)
	    {
		// find region in sl_regions
		c = కనుగొను_region(slang->sl_regions, use_region);
		ఒకవేళ (c != REGION_ALL)
		    region_ముసుగు = 1 << c;
		లేకపోతే ఒకవేళ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_ముసుగు = 0;
	    }

	    ఒకవేళ (region_ముసుగు != 0)
	    {
		LANGP_నమోదు(ga, ga.ga_len)->lp_slang = slang;
		LANGP_నమోదు(ga, ga.ga_len)->lp_sallang = ఖాళీ;
		LANGP_నమోదు(ga, ga.ga_len)->lp_replang = ఖాళీ;
		LANGP_నమోదు(ga, ga.ga_len)->lp_region = region_ముసుగు;
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
    కొరకు (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_నమోదు(ga, i);

	// sound folding
	ఒకవేళ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	లేకపోతే
	    // find first similar language that does sound folding
	    కొరకు (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_నమోదు(ga, j);
		ఒకవేళ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    ఆపు;
		}
	    }

	// REP items
	ఒకవేళ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	లేకపోతే
	    // find first similar language that has REP items
	    కొరకు (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_నమోదు(ga, j);
		ఒకవేళ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    ఆపు;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    తిరిగి ret_సందేశం;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    స్థిర శూన్య
clear_midword(win_T *wp)
{
    CLEAR_క్షేత్రం(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    స్థిర శూన్య
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    ఒకవేళ (lp->sl_midword == ఖాళీ)	    // there aren't any
	తిరిగి;

    కొరకు (p = lp->sl_midword; *p != NUL; )
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ఒకవేళ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    లేకపోతే ఒకవేళ (wp->w_s->b_spell_ismw_mb == ఖాళీ)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    లేకపోతే
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (పూర్ణాంకం)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ఒకవేళ (bp != ఖాళీ)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	లేకపోతే
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    స్థిర పూర్ణాంకం
కనుగొను_region(char_u *rp, char_u *region)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; ; i += 2)
    {
	ఒకవేళ (rp[i] == NUL)
	    తిరిగి REGION_ALL;
	ఒకవేళ (rp[i] == region[0] && rp[i + 1] == region[1])
	    ఆపు;
    }
    తిరిగి i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    పూర్ణాంకం
captype(
    char_u	*మాట,
    char_u	*చివర)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		firstcap;
    పూర్ణాంకం		allcap;
    పూర్ణాంకం		past_రెండవ = FALSE;	// past second word char

    // find first letter
    కొరకు (p = మాట; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ఒకవేళ (చివర == ఖాళీ ? *p == NUL : p >= చివర)
	    తిరిగి 0;	    // only non-word characters, illegal word
    ఒకవేళ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    లేకపోతే
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    కొరకు ( ; చివర == ఖాళీ ? *p != NUL : p < చివర; MB_PTR_ADV(p))
	ఒకవేళ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ఒకవేళ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ఒకవేళ (past_రెండవ && allcap)
		    తిరిగి WF_KEEPCAP;
		allcap = FALSE;
	    }
	    లేకపోతే ఒకవేళ (!allcap)
		// UlU -> KEEPCAP
		తిరిగి WF_KEEPCAP;
	    past_రెండవ = TRUE;
	}

    ఒకవేళ (allcap)
	తిరిగి WF_ALLCAP;
    ఒకవేళ (firstcap)
	తిరిగి WF_ONECAP;
    తిరిగి 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    శూన్య
spell_delete_wordlist(శూన్య)
{
    char_u	fname[MAXPATHL];

    ఒకవేళ (int_wordlist == ఖాళీ)
	తిరిగి;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    శూన్య
spell_free_all(శూన్య)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    అయితే (మొదటి_lang != ఖాళీ)
    {
	slang = మొదటి_lang;
	మొదటి_lang = slang->sl_తదుపరి;
	slang_free(slang);
    }

    spell_delete_wordlist();

    VIM_CLEAR(repl_గమ్యం);
    VIM_CLEAR(repl_from);
}

/*
 * Clear all spelling tables and reload them.
 * Used after 'encoding' is set and when ":mkspell" was used.
 */
    శూన్య
spell_reload(శూన్య)
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
	ఒకవేళ (*wp->w_s->b_p_spl != NUL)
	{
		ఒకవేళ (wp->w_p_spell)
		{
		    (శూన్య)parse_spelllang(wp);
		    ఆపు;
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
open_spellbuf(శూన్య)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#నిర్వచితమైతే FEAT_CRYPT
    buf->b_p_కీలకం = ఖాళీలు_option;
#అయితే_ముగింపు
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    తిరిగి buf;
}

/*
 * Close the buffer used for spell info.
 */
    శూన్య
close_spellbuf(buf_T *buf)
{
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    శూన్య
clear_spell_chartab(spelltab_T *sp)
{
    పూర్ణాంకం		i;

    // Init everything to FALSE (zero).
    CLEAR_క్షేత్రం(sp->st_isw);
    CLEAR_క్షేత్రం(sp->st_isu);
    కొరకు (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_పై[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    కొరకు (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    కొరకు (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    కొరకు (i = 'a'; i <= 'z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_పై[i] = i - 0x20;
    }
}

/*
 * Init the chartab used for spelling.  Only depends on 'encoding'.
 * Called once while starting up and when 'encoding' changes.
 * The default is to use isalpha(), but the spell file should define the word
 * characters to make it possible that 'encoding' differs from the current
 * locale.  For utf-8 we don't use isalpha() but our own functions.
 */
    శూన్య
init_spell_chartab(శూన్య)
{
    పూర్ణాంకం	    i;

    did_సమితి_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ఒకవేళ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	కొరకు (i = 128; i <= 255; ++i)
	    ఒకవేళ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    లేకపోతే ఒకవేళ (enc_utf8)
    {
	కొరకు (i = 128; i < 256; ++i)
	{
	    పూర్ణాంకం f = utf_fold(i);
	    పూర్ణాంకం u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_పై[i] = (u < 256) ? u : i;
	}
    }
    లేకపోతే
    {
	// Rough guess: use locale-dependent library functions.
	కొరకు (i = 128; i < 256; ++i)
	{
	    ఒకవేళ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    లేకపోతే ఒకవేళ (MB_ISLOWER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_పై[i] = MB_TOUPPER(i);
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
    పూర్ణాంకం
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    పూర్ణాంకం		l;
    పూర్ణాంకం		c;

    ఒకవేళ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ఒకవేళ (l == 1)
	{
	    // be quick for ASCII
	    ఒకవేళ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	లేకపోతే
	{
	    c = mb_ptr2char(p);
	    ఒకవేళ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ఖాళీ
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ఖాళీ))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ఒకవేళ (c > 255)
	    తిరిగి spell_mb_isword_class(mb_get_class(s), wp);
	తిరిగి spelltab.st_isw[c];
    }

    తిరిగి spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    పూర్ణాంకం
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    పూర్ణాంకం		c;

    ఒకవేళ (has_mbyte)
    {
	c = mb_ptr2char(p);
	ఒకవేళ (c > 255)
	    తిరిగి spell_mb_isword_class(mb_get_class(p), wp);
	తిరిగి spelltab.st_isw[c];
    }
    తిరిగి spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    స్థిర పూర్ణాంకం
spell_mb_isword_class(పూర్ణాంకం cl, win_T *wp)
{
    ఒకవేళ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	తిరిగి cl == 2 || cl == 0x2800;
    తిరిగి cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    స్థిర పూర్ణాంకం
spell_iswordp_w(పూర్ణాంకం *p, win_T *wp)
{
    పూర్ణాంకం		*s;

    ఒకవేళ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ఖాళీ
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ఖాళీ))
	s = p + 1;
    లేకపోతే
	s = p;

    ఒకవేళ (*s > 255)
    {
	ఒకవేళ (enc_utf8)
	    తిరిగి spell_mb_isword_class(utf_class(*s), wp);
	ఒకవేళ (enc_dbcs)
	    తిరిగి spell_mb_isword_class(
				dbcs_class((గుర్తులేని)*s >> 8, *s & 0xff), wp);
	తిరిగి 0;
    }
    తిరిగి spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    పూర్ణాంకం
spell_casefold(
    win_T	*wp,
    char_u	*str,
    పూర్ణాంకం		len,
    char_u	*buf,
    పూర్ణాంకం		buflen)
{
    పూర్ణాంకం		i;

    ఒకవేళ (len >= buflen)
    {
	buf[0] = NUL;
	తిరిగి FAIL;		// result will not fit
    }

    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం	outi = 0;
	char_u	*p;
	పూర్ణాంకం	c;

	// Fold one character at a time.
	కొరకు (p = str; p < str + len; )
	{
	    ఒకవేళ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		తిరిగి FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ఒకవేళ (c == 0x03a3 || c == 0x03c2)
	    {
		ఒకవేళ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		లేకపోతే
		    c = 0x03c3;
	    }
	    లేకపోతే
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    లేకపోతే
    {
	// Be quick for non-multibyte encodings.
	కొరకు (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    తిరిగి OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    పూర్ణాంకం
check_need_cap(win_T *wp, linenr_T lnum, colnr_T నిలువు)
{
    ఒకవేళ (wp->w_s->b_cap_prog == ఖాళీ)
	తిరిగి FALSE;

    పూర్ణాంకం		need_cap = FALSE;
    char_u	*పంక్తి = నిలువు ? ml_get_buf(wp->w_బఫరు, lnum, FALSE) : ఖాళీ;
    char_u	*పంక్తి_copy = ఖాళీ;
    colnr_T	endcol = 0;

    ఒకవేళ (నిలువు == 0 || getwhitecols(పంక్తి) >= నిలువు)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ఒకవేళ (lnum == 1)
	    need_cap = TRUE;
	లేకపోతే
	{
	    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum - 1, FALSE);
	    ఒకవేళ (*skipwhite(పంక్తి) == NUL)
		need_cap = TRUE;
	    లేకపోతే
	    {
		// Append a space in place of the line break.
		పంక్తి_copy = concat_str(పంక్తి, (char_u *)" ");
		ఒకవేళ (పంక్తి_copy == ఖాళీ)
		    తిరిగి FALSE;

		పంక్తి = పంక్తి_copy;
		endcol = (colnr_T)STRLEN(పంక్తి);
	    }
	}
    }
    లేకపోతే
	endcol = నిలువు;

    ఒకవేళ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = పంక్తి + endcol;
	కొరకు (;;)
	{
	    MB_PTR_వెనుక(పంక్తి, p);
	    ఒకవేళ (p == పంక్తి || spell_iswordp_nmw(p, wp))
		ఆపు;
	    ఒకవేళ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == పంక్తి + endcol)
	    {
		need_cap = TRUE;
		ఆపు;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(పంక్తి_copy);

    తిరిగి need_cap;
}


/*
 * ":spellrepall"
 */
    శూన్య
ex_spellrepall(exarg_T *eap వాడని)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*పంక్తి;
    char_u	*p;
    పూర్ణాంకం		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ఒకవేళ (repl_from == ఖాళీ || repl_గమ్యం == ఖాళీ)
    {
	emsg(_(e_no_మునుపటి_spell_replacement));
	తిరిగి;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_గమ్యం_len = STRLEN(repl_గమ్యం);
    దీర్ఘ	addlen = (దీర్ఘ)repl_గమ్యం_len - (దీర్ఘ)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ఒకవేళ (frompat == ఖాళీ)
	తిరిగి;
    frompatlen = vim_snprintf((అక్షరం *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    అయితే (!got_int)
    {
	ఒకవేళ (do_శోధన(ఖాళీ, '/', '/', frompat, frompatlen, 1L, శోధన_KEEP, ఖాళీ) == 0
						   || u_save_cursor() == FAIL)
	    ఆపు;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	పంక్తి = ml_get_curline();
	ఒకవేళ (addlen <= 0 || STRNCMP(పంక్తి + curwin->w_cursor.నిలువు,
						   repl_గమ్యం, repl_గమ్యం_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ఒకవేళ (p == ఖాళీ)
		ఆపు;
	    mch_memmove(p, పంక్తి, curwin->w_cursor.నిలువు);
	    STRCPY(p + curwin->w_cursor.నిలువు, repl_గమ్యం);
	    STRCAT(p, పంక్తి + curwin->w_cursor.నిలువు + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    మారింది_బైట్లు(curwin->w_cursor.lnum, curwin->w_cursor.నిలువు);
#ఒకవేళ defined(FEAT_PROP_POPUP)
	    ఒకవేళ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.నిలువు, (పూర్ణాంకం)addlen, APC_SUBSTITUTE);
#అయితే_ముగింపు

	    ఒకవేళ (curwin->w_cursor.lnum != prev_lnum)
	    {
		++sub_nlines;
		prev_lnum = curwin->w_cursor.lnum;
	    }
	    ++sub_nsubs;
	}
	curwin->w_cursor.నిలువు += (colnr_T)repl_గమ్యం_len;
    }

    p_ws = save_ws;
    curwin->w_cursor = pos;
    vim_free(frompat);

    ఒకవేళ (sub_nsubs == 0)
	semsg(_(e_not_దొరికింది_str), repl_from);
    లేకపోతే
	do_sub_సందేశం(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    శూన్య
onecap_copy(
    char_u	*మాట,
    char_u	*wcopy,
    పూర్ణాంకం		పై_2)	    // TRUE: first letter made upper case
{
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		l;

    p = మాట;
    ఒకవేళ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    లేకపోతే
	c = *p++;
    ఒకవేళ (పై_2)
	c = SPELL_TOUPPER(c);
    లేకపోతే
	c = SPELL_TOFOLD(c);
    ఒకవేళ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    లేకపోతే
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
    శూన్య
allcap_copy(char_u *మాట, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    పూర్ణాంకం		c;

    d = wcopy;
    కొరకు (s = మాట; *s != NUL; )
    {
	ఒకవేళ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	లేకపోతే
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ఒకవేళ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ఒకవేళ (d - wcopy >= MAXWLEN - 1)
		ఆపు;
	    *d++ = c;
	}
	లేకపోతే
	    c = SPELL_TOUPPER(c);

	ఒకవేళ (has_mbyte)
	{
	    ఒకవేళ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		ఆపు;
	    d += mb_char2bytes(c, d);
	}
	లేకపోతే
	{
	    ఒకవేళ (d - wcopy >= MAXWLEN - 1)
		ఆపు;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    పూర్ణాంకం
nofold_len(char_u *fword, పూర్ణాంకం flen, char_u *మాట)
{
    char_u	*p;
    పూర్ణాంకం		i = 0;

    కొరకు (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    కొరకు (p = మాట; i > 0; MB_PTR_ADV(p))
	--i;
    తిరిగి (పూర్ణాంకం)(p - మాట);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    శూన్య
make_case_మాట(char_u *fword, char_u *cword, పూర్ణాంకం flags)
{
    ఒకవేళ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    లేకపోతే ఒకవేళ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    లేకపోతే
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *మాట)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    పూర్ణాంకం		lpi;

    ఒకవేళ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	    ఒకవేళ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, మాట, FALSE, sound);
		తిరిగి vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    తిరిగి vim_strsave(మాట);
}
#అయితే_ముగింపు

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
    శూన్య
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    పూర్ణాంకం		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*మాట;

    ఒకవేళ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    లేకపోతే
    {
	// SAL items used.  Requires the word to be case-folded.
	ఒకవేళ (folded)
	    మాట = inword;
	లేకపోతే
	{
	    (శూన్య)spell_casefold(curwin,
				  inword, (పూర్ణాంకం)STRLEN(inword), fword, MAXWLEN);
	    మాట = fword;
	}

	ఒకవేళ (has_mbyte)
	    spell_soundfold_wsal(slang, మాట, res);
	లేకపోతే
	    spell_soundfold_sal(slang, మాట, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    స్థిర శూన్య
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    పూర్ణాంకం		ri = 0;
    పూర్ణాంకం		c;

    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం	prevc = 0;
	పూర్ణాంకం	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	కొరకు (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ఒకవేళ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    లేకపోతే ఒకవేళ (c < 256)
		c = slang->sl_sal_మొదటి[c];
	    లేకపోతే
	    {
		ip = ((పూర్ణాంకం **)slang->sl_sal.ga_దత్తాంశం)[c & 0xff];
		ఒకవేళ (ip == ఖాళీ)		// empty list, can't match
		    c = NUL;
		లేకపోతే
		    కొరకు (;;)		// find "c" in the list
		    {
			ఒకవేళ (*ip == 0)	// not found
			{
			    c = NUL;
			    ఆపు;
			}
			ఒకవేళ (*ip == c)	// match!
			{
			    c = ip[1];
			    ఆపు;
			}
			ip += 2;
		    }
	    }

	    ఒకవేళ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ఒకవేళ (ri + MB_MAXBYTES > MAXWLEN)
		    ఆపు;
		prevc = c;
	    }
	}
    }
    లేకపోతే
    {
	// The sl_sal_first[] table contains the translation.
	కొరకు (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ఒకవేళ (VIM_ISWHITE(c))
		c = ' ';
	    లేకపోతే
		c = slang->sl_sal_మొదటి[c];
	    ఒకవేళ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    స్థిర శూన్య
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	మాట[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    పూర్ణాంకం		i, j, z;
    పూర్ణాంకం		reslen;
    పూర్ణాంకం		n, k = 0;
    పూర్ణాంకం		z0;
    పూర్ణాంకం		k0;
    పూర్ణాంకం		n0;
    పూర్ణాంకం		c;
    పూర్ణాంకం		pri;
    పూర్ణాంకం		p0 = -333;
    పూర్ణాంకం		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ఒకవేళ (slang->sl_rem_accents)
    {
	t = మాట;
	అయితే (*s != NUL)
	{
	    ఒకవేళ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    లేకపోతే
	vim_strncpy(మాట, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_దత్తాంశం;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    అయితే ((c = మాట[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_మొదటి[c];
	z0 = 0;

	ఒకవేళ (n >= 0)
	{
	    // check all rules for the same letter
	    కొరకు (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ఒకవేళ (k > 1)
		{
		    ఒకవేళ (మాట[i + 1] != s[1])
			కొనసాగు;
		    ఒకవేళ (k > 2)
		    {
			కొరకు (j = 2; j < k; ++j)
			    ఒకవేళ (మాట[i + j] != s[j])
				ఆపు;
			ఒకవేళ (j < k)
			    కొనసాగు;
		    }
		}

		ఒకవేళ ((pf = smp[n].sm_oneof) != ఖాళీ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    అయితే (*pf != NUL && *pf != మాట[i + k])
			++pf;
		    ఒకవేళ (*pf == NUL)
			కొనసాగు;
		    ++k;
		}
		s = smp[n].sm_నియమాలు;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		అయితే (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ఒకవేళ (*s == '<')
		    s++;
		ఒకవేళ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ఒకవేళ (*s == '^' && *(s + 1) == '^')
		    s++;

		ఒకవేళ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(మాట[i - 1] == ' '
				      || spell_iswordp(మాట + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp(మాట + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp(మాట + i - 1, curwin)
			    && (!spell_iswordp(మాట + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = మాట[i + k - 1];
		    n0 = slang->sl_sal_మొదటి[c0];

		    ఒకవేళ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && మాట[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			కొరకు ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ఒకవేళ (k0 > 1)
			    {
				ఒకవేళ (మాట[i + k] != s[1])
				    కొనసాగు;
				ఒకవేళ (k0 > 2)
				{
				    pf = మాట + i + k + 1;
				    కొరకు (j = 2; j < k0; ++j)
					ఒకవేళ (*pf++ != s[j])
					    ఆపు;
				    ఒకవేళ (j < k0)
					కొనసాగు;
				}
			    }
			    k0 += k - 1;

			    ఒకవేళ ((pf = smp[n0].sm_oneof) != ఖాళీ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				అయితే (*pf != NUL && *pf != మాట[i + k0])
				    ++pf;
				ఒకవేళ (*pf == NUL)
				    కొనసాగు;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_నియమాలు;
			    అయితే (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ఒకవేళ (*s == '<')
				s++;
			    ఒకవేళ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ఒకవేళ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(మాట + i + k0,
								     curwin)))
			    {
				ఒకవేళ (k0 == k)
				    // this is just a piece of the string
				    కొనసాగు;

				ఒకవేళ (p0 < pri)
				    // priority too low
				    కొనసాగు;
				// rule fits; stop search
				ఆపు;
			    }
			}

			ఒకవేళ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    కొనసాగు;
		    }

		    // replace string
		    s = smp[n].sm_గమ్యం;
		    ఒకవేళ (s == ఖాళీ)
			s = (char_u *)"";
		    pf = smp[n].sm_నియమాలు;
		    p0 = (vim_strchr(pf, '<') != ఖాళీ) ? 1 : 0;
		    ఒకవేళ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ఒకవేళ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			అయితే (*s != NUL && మాట[i + k0] != NUL)
			{
			    మాట[i + k0] = *s;
			    k0++;
			    s++;
			}
			ఒకవేళ (k > k0)
			    STRMOVE(మాట + i + k0, మాట + i + k);

			// new "actual letter"
			c = మాట[i];
		    }
		    లేకపోతే
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			అయితే (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ఒకవేళ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)pf, "^^") != ఖాళీ)
			{
			    ఒకవేళ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(మాట, మాట + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    ఆపు;
		}
	    }
	}
	లేకపోతే ఒకవేళ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ఒకవేళ (z0 == 0)
	{
	    ఒకవేళ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    స్థిర శూన్య
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_దత్తాంశం;
    పూర్ణాంకం		మాట[MAXWLEN];
    పూర్ణాంకం		wres[MAXWLEN];
    పూర్ణాంకం		l;
    char_u	*s;
    పూర్ణాంకం		*ws;
    char_u	*t;
    పూర్ణాంకం		*pf;
    పూర్ణాంకం		i, j, z;
    పూర్ణాంకం		reslen;
    పూర్ణాంకం		n, k = 0;
    పూర్ణాంకం		z0;
    పూర్ణాంకం		k0;
    పూర్ణాంకం		n0;
    పూర్ణాంకం		c;
    పూర్ణాంకం		pri;
    పూర్ణాంకం		p0 = -333;
    పూర్ణాంకం		c0;
    పూర్ణాంకం		did_white = FALSE;
    పూర్ణాంకం		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    కొరకు (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ఒకవేళ (slang->sl_rem_accents)
	{
	    ఒకవేళ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ఒకవేళ (did_white)
		    కొనసాగు;
		c = ' ';
		did_white = TRUE;
	    }
	    లేకపోతే
	    {
		did_white = FALSE;
		ఒకవేళ (!spell_iswordp_nmw(t, curwin))
		    కొనసాగు;
	    }
	}
	మాట[wordlen++] = c;
    }
    మాట[wordlen] = NUL;

    /*
     * This algorithm comes from Aspell phonet.cpp.
     * Converted from C++ to C.  Added support for multi-byte chars.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    అయితే ((c = మాట[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_మొదటి[c & 0xff];
	z0 = 0;

	ఒకవేళ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    కొరకు (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ఒకవేళ (c != ws[0])
		    కొనసాగు;
		k = smp[n].sm_leadlen;
		ఒకవేళ (k > 1)
		{
		    ఒకవేళ (మాట[i + 1] != ws[1])
			కొనసాగు;
		    ఒకవేళ (k > 2)
		    {
			కొరకు (j = 2; j < k; ++j)
			    ఒకవేళ (మాట[i + j] != ws[j])
				ఆపు;
			ఒకవేళ (j < k)
			    కొనసాగు;
		    }
		}

		ఒకవేళ ((pf = smp[n].sm_oneof_w) != ఖాళీ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    అయితే (*pf != NUL && *pf != మాట[i + k])
			++pf;
		    ఒకవేళ (*pf == NUL)
			కొనసాగు;
		    ++k;
		}
		s = smp[n].sm_నియమాలు;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		అయితే (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ఒకవేళ (*s == '<')
		    s++;
		ఒకవేళ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ఒకవేళ (*s == '^' && *(s + 1) == '^')
		    s++;

		ఒకవేళ (*s == NUL
			|| (*s == '^'
			    && (i == 0 || !(మాట[i - 1] == ' '
				    || spell_iswordp_w(మాట + i - 1, curwin)))
			    && (*(s + 1) != '$'
				|| (!spell_iswordp_w(మాట + i + k0, curwin))))
			|| (*s == '$' && i > 0
			    && spell_iswordp_w(మాట + i - 1, curwin)
			    && (!spell_iswordp_w(మాట + i + k0, curwin))))
		{
		    // search for followup rules, if:
		    // followup and k > 1  and  NO '-' in searchstring
		    c0 = మాట[i + k - 1];
		    n0 = slang->sl_sal_మొదటి[c0 & 0xff];

		    ఒకవేళ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && మాట[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			కొరకు ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ఒకవేళ (c0 != ws[0])
				కొనసాగు;
			    k0 = smp[n0].sm_leadlen;
			    ఒకవేళ (k0 > 1)
			    {
				ఒకవేళ (మాట[i + k] != ws[1])
				    కొనసాగు;
				ఒకవేళ (k0 > 2)
				{
				    pf = మాట + i + k + 1;
				    కొరకు (j = 2; j < k0; ++j)
					ఒకవేళ (*pf++ != ws[j])
					    ఆపు;
				    ఒకవేళ (j < k0)
					కొనసాగు;
				}
			    }
			    k0 += k - 1;

			    ఒకవేళ ((pf = smp[n0].sm_oneof_w) != ఖాళీ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				అయితే (*pf != NUL && *pf != మాట[i + k0])
				    ++pf;
				ఒకవేళ (*pf == NUL)
				    కొనసాగు;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_నియమాలు;
			    అయితే (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ఒకవేళ (*s == '<')
				s++;
			    ఒకవేళ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ఒకవేళ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(మాట + i + k0,
								     curwin)))
			    {
				ఒకవేళ (k0 == k)
				    // this is just a piece of the string
				    కొనసాగు;

				ఒకవేళ (p0 < pri)
				    // priority too low
				    కొనసాగు;
				// rule fits; stop search
				ఆపు;
			    }
			}

			ఒకవేళ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    కొనసాగు;
		    }

		    // replace string
		    ws = smp[n].sm_గమ్యం_w;
		    s = smp[n].sm_నియమాలు;
		    p0 = (vim_strchr(s, '<') != ఖాళీ) ? 1 : 0;
		    ఒకవేళ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ఒకవేళ (reslen > 0 && ws != ఖాళీ && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ఒకవేళ (ws != ఖాళీ)
			    అయితే (*ws != NUL && మాట[i + k0] != NUL)
			    {
				మాట[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ఒకవేళ (k > k0)
			    mch_memmove(మాట + i + k0, మాట + i + k,
				    పరిమాణం(పూర్ణాంకం) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = మాట[i];
		    }
		    లేకపోతే
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ఒకవేళ (ws != ఖాళీ)
			    అయితే (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ఒకవేళ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ఒకవేళ (ws == ఖాళీ)
			    c = NUL;
			లేకపోతే
			    c = *ws;
			ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)s, "^^") != ఖాళీ)
			{
			    ఒకవేళ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(మాట, మాట + i + 1,
				       పరిమాణం(పూర్ణాంకం) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    ఆపు;
		}
	    }
	}
	లేకపోతే ఒకవేళ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ఒకవేళ (z0 == 0)
	{
	    ఒకవేళ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    కొరకు (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ఒకవేళ (l + MB_MAXBYTES > MAXWLEN)
	    ఆపు;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    శూన్య
ex_spellinfo(exarg_T *eap వాడని)
{
    పూర్ణాంకం		lpi;
    langp_T	*lp;
    char_u	*p;

    ఒకవేళ (no_spell_checking(curwin))
	తిరిగి;

    సందేశం_ప్రారంభం();
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	సందేశం_puts("file: ");
	సందేశం_puts((అక్షరం *)lp->lp_slang->sl_fname);
	సందేశం_putchar('\n');
	p = lp->lp_slang->sl_info;
	ఒకవేళ (p != ఖాళీ)
	{
	    సందేశం_puts((అక్షరం *)p);
	    సందేశం_putchar('\n');
	}
    }
    సందేశం_చివర();
}

#నిర్వచించు DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#నిర్వచించు DUMPFLAG_లెక్క	    2	// include word count
#నిర్వచించు DUMPFLAG_ICASE	    4	// ignore case when finding matches
#నిర్వచించు DUMPFLAG_ONECAP	    8	// pattern starts with capital
#నిర్వచించు DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    శూన్య
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    దీర్ఘ    dummy;

    ఒకవేళ (no_spell_checking(curwin))
	తిరిగి;
    (శూన్య)get_option_విలువ((char_u*)"spl", &dummy, &spl, ఖాళీ, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    సమితి_option_విలువ_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    సమితి_option_విలువ_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ఒకవేళ (!BUFEMPTY())
	తిరిగి;

    spell_dump_compl(ఖాళీ, 0, ఖాళీ, eap->forceit ? DUMPFLAG_లెక్క : 0);

    // Delete the empty line that we started with.
    ఒకవేళ (curbuf->b_ml.ml_పంక్తి_లెక్క > 1)
	ml_delete(curbuf->b_ml.ml_పంక్తి_లెక్క);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    శూన్య
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    పూర్ణాంకం		ic,	    // ignore case
    పూర్ణాంకం		*dir,	    // direction for adding matches
    పూర్ణాంకం		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    పూర్ణాంకం		curi[MAXWLEN];
    char_u	మాట[MAXWLEN];
    పూర్ణాంకం		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    పూర్ణాంకం		విడత;
    పూర్ణాంకం		లోతు;
    పూర్ణాంకం		n;
    పూర్ణాంకం		flags;
    char_u	*region_పేర్లు = ఖాళీ;	    // region names being used
    పూర్ణాంకం		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    పూర్ణాంకం		lpi;
    పూర్ణాంకం		dumpflags = dumpflags_arg;
    పూర్ణాంకం		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ఒకవేళ (pat != ఖాళీ)
    {
	ఒకవేళ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	లేకపోతే
	{
	    n = captype(pat, ఖాళీ);
	    ఒకవేళ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    లేకపోతే ఒకవేళ (n == WF_ALLCAP && (పూర్ణాంకం)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ఒకవేళ (p[0] != 0)
	{
	    ఒకవేళ (region_పేర్లు == ఖాళీ)	    // first language with regions
		region_పేర్లు = p;
	    లేకపోతే ఒకవేళ (STRCMP(region_పేర్లు, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		ఆపు;
	    }
	}
    }

    ఒకవేళ (do_region && region_పేర్లు != ఖాళీ && pat == ఖాళీ)
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "/regions=%s", region_పేర్లు);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    లేకపోతే
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ఒకవేళ (slang->sl_fbyts == ఖాళీ)	    // reloading failed
	    కొనసాగు;

	ఒకవేళ (pat == ఖాళీ)
	{
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ఒకవేళ (pat != ఖాళీ && slang->sl_pbyts == ఖాళీ)
	    patlen = (పూర్ణాంకం)STRLEN(pat);
	లేకపోతే
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	కొరకు (విడత = 1; విడత <= 2; ++విడత)
	{
	    ఒకవేళ (విడత == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    లేకపోతే
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ఒకవేళ (byts == ఖాళీ)
		కొనసాగు;		// array is empty

	    లోతు = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    అయితే (లోతు >= 0 && !got_int
				  && (pat == ఖాళీ || !ins_compl_interrupted()))
	    {
		ఒకవేళ (curi[లోతు] > byts[arridx[లోతు]])
		{
		    // Done all bytes at this node, go up one level.
		    --లోతు;
		    పంక్తి_breakcheck();
		    ins_compl_check_కీలకాలు(50, FALSE);
		}
		లేకపోతే
		{
		    // Do one more byte at this node.
		    n = arridx[లోతు] + curi[లోతు];
		    ++curi[లోతు];
		    c = byts[n];
		    ఒకవేళ (c == 0 || లోతు >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (పూర్ణాంకం)idxs[n];
			ఒకవేళ ((విడత == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((గుర్తులేని)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    మాట[లోతు] = NUL;
			    ఒకవేళ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (గుర్తులేని)flags >> 24;
			    ఒకవేళ (c == 0 || curi[లోతు] == 2)
			    {
				dump_మాట(slang, మాట, pat, dir,
						      dumpflags, flags, lnum);
				ఒకవేళ (pat == ఖాళీ)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ఒకవేళ (c != 0)
				lnum = dump_prefixes(slang, మాట, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    లేకపోతే
		    {
			// Normal char, go one level deeper.
			మాట[లోతు++] = c;
			arridx[లోతు] = idxs[n];
			curi[లోతు] = 1;

			// Check if this character matches with the pattern.
			// If not skip the whole tree below it.
			// Always ignore case here, dump_word() will check
			// proper case later.  This isn't exactly right when
			// length changes for multi-byte characters with
			// ignore case...
			ఒకవేళ (లోతు <= patlen
					&& MB_STRNICMP(మాట, pat, లోతు) != 0)
			    --లోతు;
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
    స్థిర శూన్య
dump_మాట(
    slang_T	*slang,
    char_u	*మాట,
    char_u	*pat,
    పూర్ణాంకం		*dir,
    పూర్ణాంకం		dumpflags,
    పూర్ణాంకం		wordflags,
    linenr_T	lnum)
{
    పూర్ణాంకం		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    పూర్ణాంకం		i;
    పూర్ణాంకం		flags = wordflags;

    ఒకవేళ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ఒకవేళ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ఒకవేళ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_మాట(మాట, cword, flags);
	p = cword;
    }
    లేకపోతే
    {
	p = మాట;
	ఒకవేళ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(మాట, ఖాళీ) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ఒకవేళ (pat == ఖాళీ)
    {
	// Add flags and regions after a slash.
	ఒకవేళ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ఒకవేళ (keepcap)
		STRCAT(badword, "=");
	    ఒకవేళ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    లేకపోతే ఒకవేళ (flags & WF_RARE)
		STRCAT(badword, "?");
	    ఒకవేళ (flags & WF_REGION)
		కొరకు (i = 0; i < 7; ++i)
		    ఒకవేళ (flags & (0x10000 << i))
			sprintf((అక్షరం *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ఒకవేళ (dumpflags & DUMPFLAG_లెక్క)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_కనుగొను(&slang->sl_wordcount, tw);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_లెక్క);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    లేకపోతే ఒకవేళ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_కూడు_infercase(p, (పూర్ణాంకం)STRLEN(p),
					  p_ic, ఖాళీ, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    స్థిర linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*మాట,	    // case-folded word
    char_u	*pat,
    పూర్ణాంకం		*dir,
    పూర్ణాంకం		dumpflags,
    పూర్ణాంకం		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    పూర్ణాంకం		curi[MAXWLEN];
    char_u	ఉపసర్గ[MAXWLEN];
    char_u	మాట_పైకి[MAXWLEN];
    పూర్ణాంకం		has_మాట_పైకి = FALSE;
    పూర్ణాంకం		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    పూర్ణాంకం		లోతు;
    పూర్ణాంకం		n;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(మాట);
    ఒకవేళ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(మాట, మాట_పైకి, TRUE);
	has_మాట_పైకి = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ఒకవేళ (byts != ఖాళీ)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	లోతు = 0;
	arridx[0] = 0;
	curi[0] = 1;
	అయితే (లోతు >= 0 && !got_int)
	{
	    n = arridx[లోతు];
	    len = byts[n];
	    ఒకవేళ (curi[లోతు] > len)
	    {
		// Done all bytes at this node, go up one level.
		--లోతు;
		పంక్తి_breakcheck();
	    }
	    లేకపోతే
	    {
		// Do one more byte at this node.
		n += curi[లోతు];
		++curi[లోతు];
		c = byts[n];
		ఒకవేళ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    కొరకు (i = 1; i < len; ++i)
			ఒకవేళ (byts[n + i] != 0)
			    ఆపు;
		    curi[లోతు] += i - 1;

		    c = valid_మాట_ఉపసర్గ(i, n, flags, మాట, slang, FALSE);
		    ఒకవేళ (c != 0)
		    {
			vim_strncpy(ఉపసర్గ + లోతు, మాట, MAXWLEN - లోతు - 1);
			dump_మాట(slang, ఉపసర్గ, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ఒకవేళ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ఒకవేళ (has_మాట_పైకి)
		    {
			c = valid_మాట_ఉపసర్గ(i, n, flags, మాట_పైకి, slang,
									TRUE);
			ఒకవేళ (c != 0)
			{
			    vim_strncpy(ఉపసర్గ + లోతు, మాట_పైకి,
							 MAXWLEN - లోతు - 1);
			    dump_మాట(slang, ఉపసర్గ, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ఒకవేళ (lnum != 0)
				++lnum;
			}
		    }
		}
		లేకపోతే ఒకవేళ (లోతు < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    ఉపసర్గ[లోతు++] = c;
		    arridx[లోతు] = idxs[n];
		    curi[లోతు] = 1;
		}
	    }
	}
    }

    తిరిగి lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_గమ్యం_మాట_చివర(char_u *ప్రారంభం, win_T *win)
{
    char_u  *p = ప్రారంభం;

    అయితే (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    తిరిగి p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    పూర్ణాంకం
spell_మాట_ప్రారంభం(పూర్ణాంకం startcol)
{
    char_u	*పంక్తి;
    char_u	*p;
    పూర్ణాంకం		నిలువు = 0;

    ఒకవేళ (no_spell_checking(curwin))
	తిరిగి startcol;

    // Find a word character before "startcol".
    పంక్తి = ml_get_curline();
    కొరకు (p = పంక్తి + startcol; p > పంక్తి; )
    {
	MB_PTR_వెనుక(పంక్తి, p);
	ఒకవేళ (spell_iswordp_nmw(p, curwin))
	    ఆపు;
    }

    // Go back to start of the word.
    అయితే (p > పంక్తి)
    {
	నిలువు = (పూర్ణాంకం)(p - పంక్తి);
	MB_PTR_వెనుక(పంక్తి, p);
	ఒకవేళ (!spell_iswordp(p, curwin))
	    ఆపు;
	నిలువు = 0;
    }

    తిరిగి నిలువు;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
స్థిర పూర్ణాంకం spell_expand_need_cap;

    శూన్య
spell_expand_check_cap(colnr_T నిలువు)
{
    spell_expand_need_cap = check_need_cap(curwin, curwin->w_cursor.lnum, నిలువు);
}

/*
 * Get list of spelling suggestions.
 * Used for Insert mode completion CTRL-X ?.
 * Returns the number of matches.  The matches are in "matchp[]", array of
 * allocated strings.
 */
    పూర్ణాంకం
expand_spelling(
    linenr_T	lnum వాడని,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_దత్తాంశం;
    తిరిగి ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    పూర్ణాంకం
valid_spelllang(char_u *val)
{
    తిరిగి valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    పూర్ణాంకం
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    పూర్ణాంకం		l;

    spf = val;
    అయితే (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ఒకవేళ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    తిరిగి FALSE;
	కొరకు (s = spf_name; *s != NUL; ++s)
	    ఒకవేళ (!vim_is_fname_char(*s))
		తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    అక్షరం *
did_సమితి_spell_option(శూన్య)
{
    అక్షరం    *errmsg = ఖాళీ;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    ఆపు;
	}
    తిరిగి errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    అక్షరం *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    ఒకవేళ (synblock->b_p_spc == ఖాళీ || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ఖాళీ;
    లేకపోతే
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ఒకవేళ (re != ఖాళీ)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ఒకవేళ (synblock->b_cap_prog == ఖాళీ)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		తిరిగి e_invalid_వాదం;
	    }
	}
    }

    vim_regfree(rp);
    తిరిగి ఖాళీ;
}

#అయితే_ముగింపు  // FEAT_SPELL
