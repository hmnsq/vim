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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_SPELL)

#ଅନ୍ତର୍ଭାଵ <time.h>

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା matchinf_S
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_prefcnt;		// number of entries at mi_prefarridx
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_prefixlen;		// byte length of prefix
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_complen;		// nr of compound words used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spell_mb_isword_class(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl, win_T *wp);

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

ସ୍ଥିର ଶୂନ୍ୟ find_word(matchinf_T *mip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);
ସ୍ଥିର ଶୂନ୍ୟ find_prefix(matchinf_T *mip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fold_more(matchinf_T *mip);
ସ୍ଥିର ଶୂନ୍ୟ spell_load_cb(char_u *fname, ଶୂନ୍ୟ *cookie);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_syllables(slang_T *slang, char_u *word);
ସ୍ଥିର ଶୂନ୍ୟ clear_midword(win_T *buf);
ସ୍ଥିର ଶୂନ୍ୟ use_midword(slang_T *lp, win_T *buf);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_region(char_u *rp, char_u *region);
ସ୍ଥିର ଶୂନ୍ୟ spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
ସ୍ଥିର ଶୂନ୍ୟ spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
ସ୍ଥିର ଶୂନ୍ୟ spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
ସ୍ଥିର ଶୂନ୍ୟ dump_word(slang_T *slang, char_u *word, char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା round, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, linenr_T lnum);
ସ୍ଥିର linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା round, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, linenr_T startlnum);
ସ୍ଥିର char_u *advance_camelcase_word(char_u *p, win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *is_camel_case);

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*capcol,	// column to check for Capital
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nrlen = 0;	// found a number first
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrongcaplen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lpi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_word = docount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_camel_case = *wp->w_s->b_p_spo != NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ଯଦି (*ptr <= ' ')
	ଫେରନ୍ତୁ 1;

    // Return here when loading language files failed.
    ଯଦି (wp->w_s->b_langp.ga_len == 0)
	ଫେରନ୍ତୁ 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ଯଦି (*ptr >= '0' && *ptr <= '9')
    {
	ଯଦି (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	ଅନ୍ୟଥା ଯଦି (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	ଅନ୍ୟଥା
	    mi.mi_end = skipdigits(ptr);
	nrlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    ଯଦି (spell_iswordp(mi.mi_fend, wp))
    {
	ଯଦି (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	ଅନ୍ୟଥା
	{
	    କର
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ଯେତେବେଳେ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ଯଦି (capcol != ଖାଲି && *capcol == 0 && wp->w_s->b_cap_prog != ଖାଲି)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ଯଦି (!SPELL_ISUPPER(c))
		wrongcaplen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mi.mi_fend - ptr);
	}
    }
    ଯଦି (capcol != ଖାଲି)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ଖାଲି;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    ଯଦି (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (ଶୂନ୍ୟ)spell_casefold(wp, ptr, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(mi.mi_fword);

    ଯଦି (is_camel_case && mi.mi_fwordlen > 0)
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
    ପାଇଁ (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ଯଦି (mi.mi_lp->lp_slang->sl_fidxs == ଖାଲି)
	    ଜାରି;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ଯଦି (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ଯଦି (ଗଣନା_word && mi.mi_result == SP_OK)
	{
	    ଗଣନା_common_word(mi.mi_lp->lp_slang, ptr,
						   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mi.mi_end - ptr), 1);
	    ଗଣନା_word = FALSE;
	}
    }

    ଯଦି (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ଯଦି (nrlen > 0)
	{
	    ଯଦି (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		ଫେରନ୍ତୁ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	ଅନ୍ୟଥା ଯଦି (!spell_iswordp_nmw(ptr, wp))
	{
	    ଯଦି (capcol != ଖାଲି && wp->w_s->b_cap_prog != ଖାଲି)
	    {
		regmatch_T	regmatch;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ଯଦି (r)
		    *capcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(regmatch.endp[0] - ptr);
	    }

	    ଯଦି (has_mbyte)
		ଫେରନ୍ତୁ (*mb_ptr2len)(ptr);
	    ଫେରନ୍ତୁ 1;
	}
	ଅନ୍ୟଥା ଯଦି (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	ଅନ୍ୟଥା ଯଦି (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    ଯଦି (mi.mi_lp->lp_slang->sl_fidxs != ଖାଲି)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		ପାଇଁ (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ଯଦି (p >= mi.mi_end)
			ଭାଙ୍ଗ;
		    mi.mi_compoff = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    ଯଦି (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			ଭାଙ୍ଗ;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	ଯଦି (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	ଅନ୍ୟଥା ଯଦି (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	ଅନ୍ୟଥା
	    *attrp = HLF_SPL;
    }

    ଯଦି (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ଫେରନ୍ତୁ wrongcaplen;
    }

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_char_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (VIM_ISDIGIT(c))
	ଫେରନ୍ତୁ CHAR_DIGIT;
    ଯଦି (SPELL_ISUPPER(c))
	ଫେରନ୍ତୁ CHAR_UPPER;
    ଫେରନ୍ତୁ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    ସ୍ଥିର char_u *
advance_camelcase_word(char_u *str, win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *is_camel_case)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_type, last_last_type, this_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
    char_u *end = str;

    *is_camel_case = FALSE;

    ଯଦି (*str == NUL)
	ଫେରନ୍ତୁ str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    ଯେତେବେଳେ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	ଯଦି (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    ଭାଙ୍ଗ;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    ଫେରନ୍ତୁ end;
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
    ସ୍ଥିର ଶୂନ୍ୟ
find_word(matchinf_T *mip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
{
    idx_T	arridx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		endidxcnt = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		word_ends;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefix_found;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nobreak_result;

    ଯଦି (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ଯଦି (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    ଅନ୍ୟଥା
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ଯଦି (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	ଅନ୍ୟଥା ଯଦି (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ଯଦି (byts == ଖାଲି)
	ଫେରନ୍ତୁ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    ପାଇଁ (;;)
    {
	ଯଦି (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ଯଦି (byts[arridx] == 0)
	{
	    ଯଦି (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		ଫେରନ୍ତୁ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ଯେତେବେଳେ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ଯଦି (len == 0)
		ଭାଙ୍ଗ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ଯଦି (ptr[wlen] == NUL)
	    ଭାଙ୍ଗ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ଯଦି (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ଯେତେବେଳେ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ଯଦି (byts[m] > c)
		hi = m - 1;
	    ଅନ୍ୟଥା ଯଦି (byts[m] < c)
		lo = m + 1;
	    ଅନ୍ୟଥା
	    {
		lo = hi = m;
		ଭାଙ୍ଗ;
	    }
	}

	// Stop if there is no matching byte.
	ଯଦି (hi < lo || byts[lo] != c)
	    ଭାଙ୍ଗ;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ଯଦି (c == ' ')
	{
	    ପାଇଁ (;;)
	    {
		ଯଦି (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		ଯଦି (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    ଭାଙ୍ଗ;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    ଯେତେବେଳେ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ଯଦି ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    ଜାରି;	    // not at first byte of character
	ଯଦି (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ଯଦି (slang->sl_compprog == ଖାଲି && !slang->sl_nobreak)
		ଜାରି;	    // next char is a word character
	    word_ends = FALSE;
	}
	ଅନ୍ୟଥା
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	ଯଦି (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    ଯଦି (STRNCMP(ptr, p, wlen) != 0)
	    {
		ପାଇଁ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	ପାଇଁ (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ଯଦି (mode == FIND_FOLDWORD)
	    {
		ଯଦି (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		ଯଦି (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    ଜାରି;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    ଅନ୍ୟଥା ଯଦି (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		ଯଦି (c == 0)
		    ଜାରି;

		// Use the WF_RARE flag for a rare prefix.
		ଯଦି (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    ଯଦି (slang->sl_nobreak)
	    {
		ଯଦି ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    ଭାଙ୍ଗ;
		}
	    }

	    ଅନ୍ୟଥା ଯଦି ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ଯଦି (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    ଜାରି;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ଯଦି (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			ଜାରି;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ଯଦି (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    ଜାରି;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ଯଦି (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    ଜାରି;
		ଯଦି (!word_ends && (flags & WF_NOCOMPAFT))
		    ଜାରି;

		// Quickly check if compounding is possible with this flag.
		ଯଦି (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    ଜାରି;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ଯଦି (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    ଜାରି;

		ଯଦି (mode == FIND_COMPOUND)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ଯଦି (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			ପାଇଁ (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    ଅନ୍ୟଥା
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    ଯଦି (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			ଜାରି;

		    ଯଦି (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			ଯଦି (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    ଜାରି;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		ଯଦି (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    ଯଦି (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ଯଦି (ptr == mip->mi_word)
			    (ଶୂନ୍ୟ)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			ଅନ୍ୟଥା
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ଯଦି (!can_compound(slang, fword, mip->mi_compflags))
			ଜାରି;
		}
		ଅନ୍ୟଥା ଯଦି (slang->sl_comprules != ଖାଲି
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    ଜାରି;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    ଅନ୍ୟଥା ଯଦି (flags & WF_NEEDCOMP)
		ଜାରି;

	    nobreak_result = SP_OK;

	    ଯଦି (!word_ends)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ଯଦି (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ଯଦି (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ଯଦି (STRNCMP(ptr, p, wlen) != 0)
		    {
			ପାଇଁ (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - mip->mi_fword);
		    }
		}
#ଯଦି 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		ଯଦି (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		ପାଇଁ (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ଯଦି (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			ଯଦି (mip->mi_lp->lp_slang->sl_fidxs == ଖାଲି
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    ଜାରି;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ଯଦି (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#ଯଦି 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ଯଦି (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    ଯଦି (!slang->sl_nobreak)
			ଭାଙ୍ଗ;
		}
		--mip->mi_complen;
		ଯଦି (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ଯଦି (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (mip->mi_result == SP_OK)
			ଭାଙ୍ଗ;
		    ଜାରି;
		}
	    }

	    ଯଦି (flags & WF_BANNED)
		res = SP_BANNED;
	    ଅନ୍ୟଥା ଯଦି (flags & WF_REGION)
	    {
		// Check region.
		ଯଦି ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		ଅନ୍ୟଥା
		    res = SP_LOCAL;
	    }
	    ଅନ୍ୟଥା ଯଦି (flags & WF_RARE)
		res = SP_RARE;
	    ଅନ୍ୟଥା
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ଯଦି (nobreak_result == SP_BAD)
	    {
		ଯଦି (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		ଅନ୍ୟଥା ଯଦି (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    ଅନ୍ୟଥା ଯଦି (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    ଅନ୍ୟଥା ଯଦି (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    ଯଦି (mip->mi_result == SP_OK)
		ଭାଙ୍ଗ;
	}

	ଯଦି (mip->mi_result == SP_OK)
	    ଭାଙ୍ଗ;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_checkcompoundpattern(
    char_u	*ptr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen,
    garray_T	*gap)  // &sl_comppat
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ପାଇଁ (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	ଯଦି (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
	    ଯଦି (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;

    ଯଦି (slang->sl_compprog == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	ପାଇଁ (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    ଅନ୍ୟଥା
	p = flags;
    ଯଦି (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ଫେରନ୍ତୁ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ଯଦି (slang->sl_compsylmax < MAXWLEN
		       && ଗଣନା_syllables(slang, word) > slang->sl_compsylmax)
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(flags) < slang->sl_compmax;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    // loop over all the COMPOUNDRULE entries
    ପାଇଁ (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	ପାଇଁ (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ଯଦି (c == NUL)
		// found a rule that matches for the flags we have so far
		ଫେରନ୍ତୁ TRUE;
	    ଯଦି (*p == '/' || *p == NUL)
		ଭାଙ୍ଗ;  // end of rule, it's too short
	    ଯଦି (*p == '[')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା match = FALSE;

		// compare against all the flags in []
		++p;
		ଯେତେବେଳେ (*p != ']' && *p != NUL)
		    ଯଦି (*p++ == c)
			match = TRUE;
		ଯଦି (!match)
		    ଭାଙ୍ଗ;  // none matches
	    }
	    ଅନ୍ୟଥା ଯଦି (*p != c)
		ଭାଙ୍ଗ;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ଯଦି (p == ଖାଲି)
	    ଭାଙ୍ଗ;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_word_prefix(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		totprefcnt,	// nr of prefix IDs
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arridx,		// idx in sl_pidxs[]
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    char_u	*word,
    slang_T	*slang,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cond_req)	// only use prefixes with a condition
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefcnt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pidx;
    regprog_T	**rp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefid;

    prefid = (unsigned)flags >> 24;
    ପାଇଁ (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ଯଦି (prefid != (pidx & 0xff))
	    ଜାରି;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ଯଦି ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    ଜାରି;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	ଯଦି (*rp != ଖାଲି)
	{
	    ଯଦି (!vim_regexec_prog(rp, FALSE, word, 0))
		ଜାରି;
	}
	ଅନ୍ୟଥା ଯଦି (cond_req)
	    ଜାରି;

	// It's a match!  Return the WF_ flags.
	ଫେରନ୍ତୁ pidx;
    }
    ଫେରନ୍ତୁ 0;
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
    ସ୍ଥିର ଶୂନ୍ୟ
find_prefix(matchinf_T *mip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
{
    idx_T	arridx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ଯଦି (byts == ଖାଲି)
	ଫେରନ୍ତୁ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ଯଦି (mode == FIND_COMPOUND)
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
    ପାଇଁ (;;)
    {
	ଯଦି (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ଯଦି (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ଯେତେବେଳେ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ଯଦି (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ଯଦି (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    ଅନ୍ୟଥା
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    ଯଦି (len == 0)
		ଭାଙ୍ଗ;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ଯଦି (ptr[wlen] == NUL)
	    ଭାଙ୍ଗ;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ଯେତେବେଳେ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ଯଦି (byts[m] > c)
		hi = m - 1;
	    ଅନ୍ୟଥା ଯଦି (byts[m] < c)
		lo = m + 1;
	    ଅନ୍ୟଥା
	    {
		lo = hi = m;
		ଭାଙ୍ଗ;
	    }
	}

	// Stop if there is no matching byte.
	ଯଦି (hi < lo || byts[lo] != c)
	    ଭାଙ୍ଗ;

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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fold_more(matchinf_T *mip)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flen;
    char_u	*p;

    p = mip->mi_fend;
    କର
	MB_PTR_ADV(mip->mi_fend);
    ଯେତେବେଳେ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ଯଦି (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (ଶୂନ୍ୟ)spell_casefold(mip->mi_win, p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ଫେରନ୍ତୁ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_valid_case(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    wordflags,	    // flags for the checked word.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    treeflags)	    // flags for the word in the spell tree
{
    ଫେରନ୍ତୁ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_check_window(win_T *wp)
{
    ଫେରନ୍ତୁ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(ଅକ୍ଷର **)(wp->w_s->b_langp.ga_data) != ଖାଲି;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
no_spell_checking(win_T *wp)
{
    ଯଦି (spell_check_window(wp))
	ଫେରନ୍ତୁ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_move_to(
    win_T	*wp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_len = 0;
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_syntax = syntax_present(wp);
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		can_spell;
    char_u	*buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		capcol = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_one = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrapped = FALSE;

    ଯଦି (no_spell_checking(wp))
	ଫେରନ୍ତୁ 0;

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

    ଯେତେବେଳେ (!got_int)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା empty_ଧାଡ଼ି;

	ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	ଯଦି (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ଯଦି (buf == ଖାଲି)
		ଭାଙ୍ଗ;
	}

	// In first line check first word for Capital.
	ଯଦି (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ଯଦି (capcol == 0)
	    capcol = getwhitecols(ଧାଡ଼ି);
	ଅନ୍ୟଥା ଯଦି (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(ଧାଡ଼ି);
	    ଯଦି (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_ଧାଡ଼ି = *skipwhite(ଧାଡ଼ି) == NUL;
	STRCPY(buf, ଧାଡ଼ି);
	ଯଦି (lnum < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    spell_cat_ଧାଡ଼ି(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ଯେତେବେଳେ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ଯଦି (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		ଭାଙ୍ଗ;

	    // start of word
	    attr = HLF_ଗଣନା;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ଯଦି (attr != HLF_ଗଣନା)
	    {
		// We found a bad word.  Check the attribute.
		ଯଦି (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ଯଦି (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			ଯଦି (has_syntax)
			{
			    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - buf);
			    (ଶୂନ୍ୟ)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    ଯଦି (!can_spell)
				attr = HLF_ଗଣନା;
			}
			ଅନ୍ୟଥା
#endif
			    can_spell = TRUE;

			ଯଦି (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - buf);
			    found_pos.coladd = 0;
			    ଯଦି (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				ଯଦି (attrp != ଖାଲି)
				    *attrp = attr;
				ଫେରନ୍ତୁ len;
			    }
			    ଅନ୍ୟଥା ଯଦି (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    ଅନ୍ୟଥା
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ଯଦି (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    ଫେରନ୍ତୁ found_len;
	}

	ଯଦି (curline)
	    ଭାଙ୍ଗ;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ଯଦି (lnum == wp->w_cursor.lnum && wrapped)
	    ଭାଙ୍ଗ;

	// Advance to next line.
	ଯଦି (dir == BACKWARD)
	{
	    ଯଦି (lnum > 1)
		--lnum;
	    ଅନ୍ୟଥା ଯଦି (!p_ws)
		ଭାଙ୍ଗ;	    // at first line and 'nowrapscan'
	    ଅନ୍ୟଥା
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		wrapped = TRUE;
		ଯଦି (!shortmess(SHM_ସନ୍ଧାନ))
		    give_warning((char_u *)_(ଶୀର୍ଷ_bot_ସନ୍ଦେଶ), TRUE);
	    }
	    capcol = -1;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (lnum < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		++lnum;
	    ଅନ୍ୟଥା ଯଦି (!p_ws)
		ଭାଙ୍ଗ;	    // at first line and 'nowrapscan'
	    ଅନ୍ୟଥା
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ଯଦି (!shortmess(SHM_ସନ୍ଧାନ))
		    give_warning((char_u *)_(bot_ଶୀର୍ଷ_ସନ୍ଦେଶ), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ଯଦି (lnum == wp->w_cursor.lnum && !found_one)
		ଭାଙ୍ଗ;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ଯଦି (attr == HLF_ଗଣନା)
		skip = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - endp);
	    ଅନ୍ୟଥା
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ଯଦି (empty_ଧାଡ଼ି)
		capcol = 0;
	}

	ଧାଡ଼ି_breakcheck();
    }

    vim_free(buf);
    ଫେରନ୍ତୁ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    ଶୂନ୍ୟ
spell_cat_ଧାଡ଼ି(char_u *buf, char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    p = skipwhite(ଧାଡ଼ି);
    ଯେତେବେଳେ (vim_strchr((char_u *)"*#/\"\t", *p) != ଖାଲି)
	p = skipwhite(p + 1);

    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି) + 1;
    ଯଦି (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    spelload_T	sl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ଖାଲି;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((ଅକ୍ଷର *)fname_enc, ଆକାର(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#ଅନ୍ୟଥା
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	ଯଦି (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((ଅକ୍ଷର *)fname_enc, ଆକାର(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#ଅନ୍ୟଥା
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    ଯଦି (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		ଜାରି;
	    ଭାଙ୍ଗ;
	}
	ଭାଙ୍ଗ;
    }

    ଯଦି (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#ଅନ୍ୟଥା
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    ଅନ୍ୟଥା ଯଦି (sl.sl_slang != ଖାଲି)
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
spell_enc(ଶୂନ୍ୟ)
{

    ଯଦି (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ଫେରନ୍ତୁ p_enc;
    ଫେରନ୍ତୁ (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((ଅକ୍ଷର *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    ଯଦି (lp != ଖାଲି)
    {
	ଯଦି (lang != ଖାଲି)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, ଆକାର(fromto_T), 10);
	ga_init2(&lp->sl_repsal, ଆକାର(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ଫେରନ୍ତୁ lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
free_sal_ବସ୍ତୁଗୁଡ଼ିକ(garray_T *gap)
{
    salitem_T	*smp;

    ଯେତେବେଳେ (gap->ga_len > 0)
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
    ଶୂନ୍ୟ
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ଯେତେବେଳେ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ଯଦି (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ଯଦି (gap->ga_data != ଖାଲି)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
		vim_free(((ପୂର୍ଣ୍ଣସଂଖ୍ୟା **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    ଅନ୍ୟଥା
	free_sal_ବସ୍ତୁଗୁଡ଼ିକ(gap);

    ପାଇଁ (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ଖାଲି;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_ଚାବି_OFF);
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
    ଶୂନ୍ୟ
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = ଖାଲି;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_load_cb(char_u *fname, ଶୂନ୍ୟ *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, ଖାଲି, FALSE);
    ଯଦି (slang == ଖାଲି)
	ଫେରନ୍ତୁ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ଯଦି (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    ଅନ୍ୟଥା ଯଦି (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    ଶୂନ୍ୟ
ଗଣନା_common_word(
    slang_T	*lp,
    char_u	*word,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,	    // word length, -1 for up to NUL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ଯଦି (len == -1)
	p = word;
    ଅନ୍ୟଥା ଯଦି (len >= MAXWLEN)
	ଫେରନ୍ତୁ;
    ଅନ୍ୟଥା
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ଯଦି (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	ଯଦି (wc == ଖାଲି)
	    ଫେରନ୍ତୁ;
	STRCPY(wc->wc_word, p);
	wc->wc_ଗଣନା = ଗଣନା_2;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    ଅନ୍ୟଥା
    {
	wc = HI2WC(hi);
	ଯଦି ((wc->wc_ଗଣନା += ଗଣନା_2) < (unsigned)ଗଣନା_2)	// check for overflow
	    wc->wc_ଗଣନା = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
byte_in_str(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    char_u	*p;

    ପାଇଁ (p = str; *p != NUL; ++p)
	ଯଦି (*p == n)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

#define SY_MAXLEN   30
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ, ଆକାର(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ଯେତେବେଳେ (p != ଖାଲି)
    {
	*p++ = NUL;
	ଯଦି (*p == NUL)	    // trailing slash
	    ଭାଙ୍ଗ;
	s = p;
	p = vim_strchr(p, '/');
	ଯଦି (p == ଖାଲି)
	    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
	ଅନ୍ୟଥା
	    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - s);
	ଯଦି (l >= SY_MAXLEN)
	    ଫେରନ୍ତୁ SP_FORMERROR;
	ଯଦି (ga_grow(&slang->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ, 1) == FAIL)
	    ଫେରନ୍ତୁ SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ.ga_data)
					       + slang->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଗଣନା_syllables(slang_T *slang, char_u *word)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip = FALSE;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    syl_item_T	*syl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (slang->sl_syllable == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ପାଇଁ (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ଯଦି (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    ଜାରି;
	}

	// Find longest match of syllable items.
	len = 0;
	ପାଇଁ (i = 0; i < slang->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_ବସ୍ତୁଗୁଡ଼ିକ.ga_data) + i;
	    ଯଦି (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ଯଦି (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ଯଦି (vim_strchr(slang->sl_syllable, c) == ଖାଲି)
		skip = FALSE;	    // No, search for next syllable
	    ଅନ୍ୟଥା ଯଦି (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ଫେରନ୍ତୁ cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    ଅକ୍ଷର *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filename;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		region_mask;
    slang_T	*slang;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    char_u	*spf;
    char_u	*use_region = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dont_use_region = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nobreak = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    langp_T	*lp, *lp2;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;
    ଅକ୍ଷର	*ret_ସନ୍ଦେଶ = ଖାଲି;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ଯଦି (recursive)
	ଫେରନ୍ତୁ ଖାଲି;
    recursive = TRUE;

    ga_init2(&ga, ଆକାର(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ଯଦି (spl_copy == ଖାଲି)
	ଯାଅ theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    ପାଇଁ (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ଖାଲି;

	ଯଦି (!valid_spelllang(lang))
	    ଜାରି;

	ଯଦି (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    ଜାରି;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ଯଦି (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ଯଦି (p != ଖାଲି && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    ଅନ୍ୟଥା
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ଯଦି (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	{
	    filename = FALSE;
	    ଯଦି (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    ଅନ୍ୟଥା
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ଯଦି (STRICMP(lang, slang->sl_name) == 0)
		    ଭାଙ୍ଗ;
	}

	ଯଦି (region != ଖାଲି)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ଯଦି (use_region != ଖାଲି && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ଯଦି (slang == ଖାଲି)
	{
	    ଯଦି (filename)
		(ଶୂନ୍ୟ)spell_load_file(lang, lang, ଖାଲି, FALSE);
	    ଅନ୍ୟଥା
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ଯଦି (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_ସନ୍ଦେଶ = N_(e_spellfilemising_autocommand_deleted_buffer);
		    ଯାଅ theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    ଯଦି (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		ଯଦି (!filename && region != ଖାଲି)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    ଯଦି (c == REGION_ALL)
		    {
			ଯଦି (slang->sl_add)
			{
			    ଯଦି (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			ଅନ୍ୟଥା
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    ଅନ୍ୟଥା
			region_mask = 1 << c;
		}

		ଯଦି (region_mask != 0)
		{
		    ଯଦି (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_ସନ୍ଦେଶ = e_out_of_memory;
			ଯାଅ theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ଯଦି (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    ପାଇଁ (round = 0; round == 0 || *spf != NUL; ++round)
    {
	ଯଦି (round == 0)
	{
	    // Internal wordlist, if there is one.
	    ଯଦି (int_wordlist == ଖାଲି)
		ଜାରି;
	    int_wordlist_spl(spf_name);
	}
	ଅନ୍ୟଥା
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    ପାଇଁ (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		ଯଦି (p != ଖାଲି && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (c < ga.ga_len)
		ଜାରି;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ଯଦି (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		ଭାଙ୍ଗ;
	ଯଦି (slang == ଖାଲି)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ଯଦି (round == 0)
		STRCPY(lang, "internal wordlist");
	    ଅନ୍ୟଥା
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ଯଦି (p != ଖାଲି)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, ଖାଲି, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ଯଦି (slang != ଖାଲି && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ଯଦି (slang != ଖାଲି && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    ଯଦି (use_region != ଖାଲି && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		ଯଦି (c != REGION_ALL)
		    region_mask = 1 << c;
		ଅନ୍ୟଥା ଯଦି (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    ଯଦି (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = ଖାଲି;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = ଖାଲି;
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
    ପାଇଁ (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	ଯଦି (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	ଅନ୍ୟଥା
	    // find first similar language that does sound folding
	    ପାଇଁ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ଯଦି (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    ଭାଙ୍ଗ;
		}
	    }

	// REP items
	ଯଦି (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	ଅନ୍ୟଥା
	    // find first similar language that has REP items
	    ପାଇଁ (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ଯଦି (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    ଭାଙ୍ଗ;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ଫେରନ୍ତୁ ret_ସନ୍ଦେଶ;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    ଯଦି (lp->sl_midword == ଖାଲି)	    // there aren't any
	ଫେରନ୍ତୁ;

    ପାଇଁ (p = lp->sl_midword; *p != NUL; )
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ଯଦି (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    ଅନ୍ୟଥା ଯଦି (wp->w_s->b_spell_ismw_mb == ଖାଲି)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    ଅନ୍ୟଥା
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ଯଦି (bp != ଖାଲି)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	ଅନ୍ୟଥା
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_region(char_u *rp, char_u *region)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; ; i += 2)
    {
	ଯଦି (rp[i] == NUL)
	    ଫେରନ୍ତୁ REGION_ALL;
	ଯଦି (rp[i] == region[0] && rp[i + 1] == region[1])
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		firstcap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allcap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		past_second = FALSE;	// past second word char

    // find first letter
    ପାଇଁ (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ଯଦି (end == ଖାଲି ? *p == NUL : p >= end)
	    ଫେରନ୍ତୁ 0;	    // only non-word characters, illegal word
    ଯଦି (has_mbyte)
	c = mb_ptr2char_adv(&p);
    ଅନ୍ୟଥା
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    ପାଇଁ ( ; end == ଖାଲି ? *p != NUL : p < end; MB_PTR_ADV(p))
	ଯଦି (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ଯଦି (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ଯଦି (past_second && allcap)
		    ଫେରନ୍ତୁ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (!allcap)
		// UlU -> KEEPCAP
		ଫେରନ୍ତୁ WF_KEEPCAP;
	    past_second = TRUE;
	}

    ଯଦି (allcap)
	ଫେରନ୍ତୁ WF_ALLCAP;
    ଯଦି (firstcap)
	ଫେରନ୍ତୁ WF_ONECAP;
    ଫେରନ୍ତୁ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    ଶୂନ୍ୟ
spell_delete_wordlist(ଶୂନ୍ୟ)
{
    char_u	fname[MAXPATHL];

    ଯଦି (int_wordlist == ଖାଲି)
	ଫେରନ୍ତୁ;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    ଶୂନ୍ୟ
spell_free_all(ଶୂନ୍ୟ)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    ଯେତେବେଳେ (first_lang != ଖାଲି)
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
    ଶୂନ୍ୟ
spell_reload(ଶୂନ୍ୟ)
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
	ଯଦି (*wp->w_s->b_p_spl != NUL)
	{
		ଯଦି (wp->w_p_spell)
		{
		    (ଶୂନ୍ୟ)parse_spelllang(wp);
		    ଭାଙ୍ଗ;
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
open_spellbuf(ଶୂନ୍ୟ)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_ଚାବି = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ଫେରନ୍ତୁ buf;
}

/*
 * Close the buffer used for spell info.
 */
    ଶୂନ୍ୟ
close_spellbuf(buf_T *buf)
{
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    ଶୂନ୍ୟ
clear_spell_chartab(spelltab_T *sp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    ପାଇଁ (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    ପାଇଁ (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    ପାଇଁ (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    ପାଇଁ (i = 'a'; i <= 'z'; ++i)
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
    ଶୂନ୍ୟ
init_spell_chartab(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ଯଦି (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	ପାଇଁ (i = 128; i <= 255; ++i)
	    ଯଦି (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (enc_utf8)
    {
	ପାଇଁ (i = 128; i < 256; ++i)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା f = utf_fold(i);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    ଅନ୍ୟଥା
    {
	// Rough guess: use locale-dependent library functions.
	ପାଇଁ (i = 128; i < 256; ++i)
	{
	    ଯଦି (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    ଅନ୍ୟଥା ଯଦି (MB_ISLOWER(i))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ଯଦି (l == 1)
	{
	    // be quick for ASCII
	    ଯଦି (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	ଅନ୍ୟଥା
	{
	    c = mb_ptr2char(p);
	    ଯଦି (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ଖାଲି
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ଖାଲି))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ଯଦି (c > 255)
	    ଫେରନ୍ତୁ spell_mb_isword_class(mb_get_class(s), wp);
	ଫେରନ୍ତୁ spelltab.st_isw[c];
    }

    ଫେରନ୍ତୁ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (has_mbyte)
    {
	c = mb_ptr2char(p);
	ଯଦି (c > 255)
	    ଫେରନ୍ତୁ spell_mb_isword_class(mb_get_class(p), wp);
	ଫେରନ୍ତୁ spelltab.st_isw[c];
    }
    ଫେରନ୍ତୁ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_mb_isword_class(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl, win_T *wp)
{
    ଯଦି (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ଫେରନ୍ତୁ cl == 2 || cl == 0x2800;
    ଫେରନ୍ତୁ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_iswordp_w(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p, win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*s;

    ଯଦି (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ଖାଲି
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ଖାଲି))
	s = p + 1;
    ଅନ୍ୟଥା
	s = p;

    ଯଦି (*s > 255)
    {
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ spell_mb_isword_class(utf_class(*s), wp);
	ଯଦି (enc_dbcs)
	    ଫେରନ୍ତୁ spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_casefold(
    win_T	*wp,
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (len >= buflen)
    {
	buf[0] = NUL;
	ଫେରନ୍ତୁ FAIL;		// result will not fit
    }

    ଯଦି (has_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	outi = 0;
	char_u	*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c;

	// Fold one character at a time.
	ପାଇଁ (p = str; p < str + len; )
	{
	    ଯଦି (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ଫେରନ୍ତୁ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ଯଦି (c == 0x03a3 || c == 0x03c2)
	    {
		ଯଦି (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		ଅନ୍ୟଥା
		    c = 0x03c3;
	    }
	    ଅନ୍ୟଥା
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    ଅନ୍ୟଥା
    {
	// Be quick for non-multibyte encodings.
	ପାଇଁ (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    ଯଦି (wp->w_s->b_cap_prog == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_cap = FALSE;
    char_u	*ଧାଡ଼ି = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : ଖାଲି;
    char_u	*ଧାଡ଼ି_copy = ଖାଲି;
    colnr_T	endcol = 0;

    ଯଦି (col == 0 || getwhitecols(ଧାଡ଼ି) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ଯଦି (lnum == 1)
	    need_cap = TRUE;
	ଅନ୍ୟଥା
	{
	    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    ଯଦି (*skipwhite(ଧାଡ଼ି) == NUL)
		need_cap = TRUE;
	    ଅନ୍ୟଥା
	    {
		// Append a space in place of the line break.
		ଧାଡ଼ି_copy = concat_str(ଧାଡ଼ି, (char_u *)" ");
		ଯଦି (ଧାଡ଼ି_copy == ଖାଲି)
		    ଫେରନ୍ତୁ FALSE;

		ଧାଡ଼ି = ଧାଡ଼ି_copy;
		endcol = (colnr_T)STRLEN(ଧାଡ଼ି);
	    }
	}
    }
    ଅନ୍ୟଥା
	endcol = col;

    ଯଦି (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = ଧାଡ଼ି + endcol;
	ପାଇଁ (;;)
	{
	    MB_PTR_BACK(ଧାଡ଼ି, p);
	    ଯଦି (p == ଧାଡ଼ି || spell_iswordp_nmw(p, wp))
		ଭାଙ୍ଗ;
	    ଯଦି (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == ଧାଡ଼ି + endcol)
	    {
		need_cap = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(ଧାଡ଼ି_copy);

    ଫେରନ୍ତୁ need_cap;
}


/*
 * ":spellrepall"
 */
    ଶୂନ୍ୟ
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ଯଦି (repl_from == ଖାଲି || repl_to == ଖାଲି)
    {
	emsg(_(e_no_previous_spell_replacement));
	ଫେରନ୍ତୁ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ଯଦି (frompat == ଖାଲି)
	ଫେରନ୍ତୁ;
    frompatlen = vim_snprintf((ଅକ୍ଷର *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ଯେତେବେଳେ (!got_int)
    {
	ଯଦି (do_ସନ୍ଧାନ(ଖାଲି, '/', '/', frompat, frompatlen, 1L, ସନ୍ଧାନ_KEEP, ଖାଲି) == 0
						   || u_save_cursor() == FAIL)
	    ଭାଙ୍ଗ;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	ଧାଡ଼ି = ml_get_curline();
	ଯଦି (addlen <= 0 || STRNCMP(ଧାଡ଼ି + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ଯଦି (p == ଖାଲି)
		ଭାଙ୍ଗ;
	    mch_memmove(p, ଧାଡ଼ି, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, ଧାଡ଼ି + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#ଯଦି defined(FEAT_PROP_POPUP)
	    ଯଦି (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)addlen, APC_SUBSTITUTE);
#endif

	    ଯଦି (curwin->w_cursor.lnum != prev_lnum)
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

    ଯଦି (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    ଅନ୍ୟଥା
	do_sub_ସନ୍ଦେଶ(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    ଶୂନ୍ୟ
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    p = word;
    ଯଦି (has_mbyte)
	c = mb_cptr2char_adv(&p);
    ଅନ୍ୟଥା
	c = *p++;
    ଯଦି (upper)
	c = SPELL_TOUPPER(c);
    ଅନ୍ୟଥା
	c = SPELL_TOFOLD(c);
    ଯଦି (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    d = wcopy;
    ପାଇଁ (s = word; *s != NUL; )
    {
	ଯଦି (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	ଅନ୍ୟଥା
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ଯଦି (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ଯଦି (d - wcopy >= MAXWLEN - 1)
		ଭାଙ୍ଗ;
	    *d++ = c;
	}
	ଅନ୍ୟଥା
	    c = SPELL_TOUPPER(c);

	ଯଦି (has_mbyte)
	{
	    ଯଦି (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		ଭାଙ୍ଗ;
	    d += mb_char2bytes(c, d);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (d - wcopy >= MAXWLEN - 1)
		ଭାଙ୍ଗ;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nofold_len(char_u *fword, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flen, char_u *word)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    ପାଇଁ (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    ପାଇଁ (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    ଶୂନ୍ୟ
make_case_word(char_u *fword, char_u *cword, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ଯଦି (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    ଅନ୍ୟଥା ଯଦି (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    ଅନ୍ୟଥା
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lpi;

    ଯଦି (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	ପାଇଁ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    ଯଦି (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		ଫେରନ୍ତୁ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ଫେରନ୍ତୁ vim_strsave(word);
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
    ଶୂନ୍ୟ
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    ଯଦି (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    ଅନ୍ୟଥା
    {
	// SAL items used.  Requires the word to be case-folded.
	ଯଦି (folded)
	    word = inword;
	ଅନ୍ୟଥା
	{
	    (ଶୂନ୍ୟ)spell_casefold(curwin,
				  inword, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	ଯଦି (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	ଅନ୍ୟଥା
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ri = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (has_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prevc = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	ପାଇଁ (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ଯଦି (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    ଅନ୍ୟଥା ଯଦି (c < 256)
		c = slang->sl_sal_first[c];
	    ଅନ୍ୟଥା
	    {
		ip = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା **)slang->sl_sal.ga_data)[c & 0xff];
		ଯଦି (ip == ଖାଲି)		// empty list, can't match
		    c = NUL;
		ଅନ୍ୟଥା
		    ପାଇଁ (;;)		// find "c" in the list
		    {
			ଯଦି (*ip == 0)	// not found
			{
			    c = NUL;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (*ip == c)	// match!
			{
			    c = ip[1];
			    ଭାଙ୍ଗ;
			}
			ip += 2;
		    }
	    }

	    ଯଦି (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ଯଦି (ri + MB_MAXBYTES > MAXWLEN)
		    ଭାଙ୍ଗ;
		prevc = c;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	// The sl_sal_first[] table contains the translation.
	ପାଇଁ (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ଯଦି (VIM_ISWHITE(c))
		c = ' ';
	    ଅନ୍ୟଥା
		c = slang->sl_sal_first[c];
	    ଯଦି (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    ସ୍ଥିର ଶୂନ୍ୟ
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j, z;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reslen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n, k = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		z0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		k0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pri;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		p0 = -333;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ଯଦି (slang->sl_rem_accents)
    {
	t = word;
	ଯେତେବେଳେ (*s != NUL)
	{
	    ଯଦି (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    ଅନ୍ୟଥା
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ଯେତେବେଳେ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	ଯଦି (n >= 0)
	{
	    // check all rules for the same letter
	    ପାଇଁ (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ଯଦି (k > 1)
		{
		    ଯଦି (word[i + 1] != s[1])
			ଜାରି;
		    ଯଦି (k > 2)
		    {
			ପାଇଁ (j = 2; j < k; ++j)
			    ଯଦି (word[i + j] != s[j])
				ଭାଙ୍ଗ;
			ଯଦି (j < k)
			    ଜାରି;
		    }
		}

		ଯଦି ((pf = smp[n].sm_oneof) != ଖାଲି)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ଯେତେବେଳେ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ଯଦି (*pf == NUL)
			ଜାରି;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ଯେତେବେଳେ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ଯଦି (*s == '<')
		    s++;
		ଯଦି (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ଯଦି (*s == '^' && *(s + 1) == '^')
		    s++;

		ଯଦି (*s == NUL
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

		    ଯଦି (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			ପାଇଁ ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ଯଦି (k0 > 1)
			    {
				ଯଦି (word[i + k] != s[1])
				    ଜାରି;
				ଯଦି (k0 > 2)
				{
				    pf = word + i + k + 1;
				    ପାଇଁ (j = 2; j < k0; ++j)
					ଯଦି (*pf++ != s[j])
					    ଭାଙ୍ଗ;
				    ଯଦି (j < k0)
					ଜାରି;
				}
			    }
			    k0 += k - 1;

			    ଯଦି ((pf = smp[n0].sm_oneof) != ଖାଲି)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ଯେତେବେଳେ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ଯଦି (*pf == NUL)
				    ଜାରି;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ଯେତେବେଳେ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ଯଦି (*s == '<')
				s++;
			    ଯଦି (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ଯଦି (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				ଯଦି (k0 == k)
				    // this is just a piece of the string
				    ଜାରି;

				ଯଦି (p0 < pri)
				    // priority too low
				    ଜାରି;
				// rule fits; stop search
				ଭାଙ୍ଗ;
			    }
			}

			ଯଦି (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    ଜାରି;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    ଯଦି (s == ଖାଲି)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != ଖାଲି) ? 1 : 0;
		    ଯଦି (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ଯଦି (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ଯେତେବେଳେ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			ଯଦି (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    ଅନ୍ୟଥା
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ଯେତେବେଳେ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ଯଦି (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ଯଦି (strstr((ଅକ୍ଷର *)pf, "^^") != ଖାଲି)
			{
			    ଯଦି (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ଯଦି (z0 == 0)
	{
	    ଯଦି (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    ସ୍ଥିର ଶୂନ୍ୟ
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		word[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wres[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ws;
    char_u	*t;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*pf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j, z;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reslen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n, k = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		z0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		k0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pri;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		p0 = -333;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_white = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    ପାଇଁ (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ଯଦି (slang->sl_rem_accents)
	{
	    ଯଦି (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ଯଦି (did_white)
		    ଜାରି;
		c = ' ';
		did_white = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		did_white = FALSE;
		ଯଦି (!spell_iswordp_nmw(t, curwin))
		    ଜାରି;
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
    ଯେତେବେଳେ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	ଯଦି (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    ପାଇଁ (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ଯଦି (c != ws[0])
		    ଜାରି;
		k = smp[n].sm_leadlen;
		ଯଦି (k > 1)
		{
		    ଯଦି (word[i + 1] != ws[1])
			ଜାରି;
		    ଯଦି (k > 2)
		    {
			ପାଇଁ (j = 2; j < k; ++j)
			    ଯଦି (word[i + j] != ws[j])
				ଭାଙ୍ଗ;
			ଯଦି (j < k)
			    ଜାରି;
		    }
		}

		ଯଦି ((pf = smp[n].sm_oneof_w) != ଖାଲି)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ଯେତେବେଳେ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ଯଦି (*pf == NUL)
			ଜାରି;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ଯେତେବେଳେ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ଯଦି (*s == '<')
		    s++;
		ଯଦି (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ଯଦି (*s == '^' && *(s + 1) == '^')
		    s++;

		ଯଦି (*s == NUL
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

		    ଯଦି (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			ପାଇଁ ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ଯଦି (c0 != ws[0])
				ଜାରି;
			    k0 = smp[n0].sm_leadlen;
			    ଯଦି (k0 > 1)
			    {
				ଯଦି (word[i + k] != ws[1])
				    ଜାରି;
				ଯଦି (k0 > 2)
				{
				    pf = word + i + k + 1;
				    ପାଇଁ (j = 2; j < k0; ++j)
					ଯଦି (*pf++ != ws[j])
					    ଭାଙ୍ଗ;
				    ଯଦି (j < k0)
					ଜାରି;
				}
			    }
			    k0 += k - 1;

			    ଯଦି ((pf = smp[n0].sm_oneof_w) != ଖାଲି)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ଯେତେବେଳେ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ଯଦି (*pf == NUL)
				    ଜାରି;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ଯେତେବେଳେ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ଯଦି (*s == '<')
				s++;
			    ଯଦି (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ଯଦି (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				ଯଦି (k0 == k)
				    // this is just a piece of the string
				    ଜାରି;

				ଯଦି (p0 < pri)
				    // priority too low
				    ଜାରି;
				// rule fits; stop search
				ଭାଙ୍ଗ;
			    }
			}

			ଯଦି (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    ଜାରି;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != ଖାଲି) ? 1 : 0;
		    ଯଦି (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ଯଦି (reslen > 0 && ws != ଖାଲି && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ଯଦି (ws != ଖାଲି)
			    ଯେତେବେଳେ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ଯଦି (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    ଅନ୍ୟଥା
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ଯଦି (ws != ଖାଲି)
			    ଯେତେବେଳେ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ଯଦି (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ଯଦି (ws == ଖାଲି)
			    c = NUL;
			ଅନ୍ୟଥା
			    c = *ws;
			ଯଦି (strstr((ଅକ୍ଷର *)s, "^^") != ଖାଲି)
			{
			    ଯଦି (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ଯଦି (z0 == 0)
	{
	    ଯଦି (k && !p0 && reslen < MAXWLEN && c != NUL
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
    ପାଇଁ (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ଯଦି (l + MB_MAXBYTES > MAXWLEN)
	    ଭାଙ୍ଗ;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    ଶୂନ୍ୟ
ex_spellinfo(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lpi;
    langp_T	*lp;
    char_u	*p;

    ଯଦି (no_spell_checking(curwin))
	ଫେରନ୍ତୁ;

    ସନ୍ଦେଶ_start();
    ପାଇଁ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	ସନ୍ଦେଶ_puts("file: ");
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)lp->lp_slang->sl_fname);
	ସନ୍ଦେଶ_putchar('\n');
	p = lp->lp_slang->sl_info;
	ଯଦି (p != ଖାଲି)
	{
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
	    ସନ୍ଦେଶ_putchar('\n');
	}
    }
    ସନ୍ଦେଶ_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_ଗଣନା	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    ଶୂନ୍ୟ
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    ଯଦି (no_spell_checking(curwin))
	ଫେରନ୍ତୁ;
    (ଶୂନ୍ୟ)get_option_ମୂଲ୍ୟ((char_u*)"spl", &dummy, &spl, ଖାଲି, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_ମୂଲ୍ୟ_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_ମୂଲ୍ୟ_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ଯଦି (!BUFEMPTY())
	ଫେରନ୍ତୁ;

    spell_dump_compl(ଖାଲି, 0, ଖାଲି, eap->forceit ? DUMPFLAG_ଗଣନା : 0);

    // Delete the empty line that we started with.
    ଯଦି (curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା > 1)
	ml_delete(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    ଶୂନ୍ୟ
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic,	    // ignore case
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dir,	    // direction for adding matches
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    char_u	*region_names = ଖାଲି;	    // region names being used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lpi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dumpflags = dumpflags_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ଯଦି (pat != ଖାଲି)
    {
	ଯଦି (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	ଅନ୍ୟଥା
	{
	    n = captype(pat, ଖାଲି);
	    ଯଦି (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    ଅନ୍ୟଥା ଯଦି (n == WF_ALLCAP && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    ପାଇଁ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ଯଦି (p[0] != 0)
	{
	    ଯଦି (region_names == ଖାଲି)	    // first language with regions
		region_names = p;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (do_region && region_names != ଖାଲି && pat == ଖାଲି)
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    ଅନ୍ୟଥା
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    ପାଇଁ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ଯଦି (slang->sl_fbyts == ଖାଲି)	    // reloading failed
	    ଜାରି;

	ଯଦି (pat == ଖାଲି)
	{
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ଯଦି (pat != ଖାଲି && slang->sl_pbyts == ଖାଲି)
	    patlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
	ଅନ୍ୟଥା
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	ପାଇଁ (round = 1; round <= 2; ++round)
	{
	    ଯଦି (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    ଅନ୍ୟଥା
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ଯଦି (byts == ଖାଲି)
		ଜାରି;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ଯେତେବେଳେ (depth >= 0 && !got_int
				  && (pat == ଖାଲି || !ins_compl_interrupted()))
	    {
		ଯଦି (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    ଧାଡ଼ି_breakcheck();
		    ins_compl_check_ଚାବିଗୁଡ଼ିକ(50, FALSE);
		}
		ଅନ୍ୟଥା
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    ଯଦି (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idxs[n];
			ଯଦି ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    ଯଦି (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    ଯଦି (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				ଯଦି (pat == ଖାଲି)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ଯଦି (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    ଅନ୍ୟଥା
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
			ଯଦି (depth <= patlen
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
    ସ୍ଥିର ଶୂନ୍ୟ
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dir,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dumpflags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wordflags,
    linenr_T	lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = wordflags;

    ଯଦି (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ଯଦି (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ଯଦି ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    ଅନ୍ୟଥା
    {
	p = word;
	ଯଦି ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, ଖାଲି) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ଯଦି (pat == ଖାଲି)
    {
	// Add flags and regions after a slash.
	ଯଦି ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ଯଦି (keepcap)
		STRCAT(badword, "=");
	    ଯଦି (flags & WF_BANNED)
		STRCAT(badword, "!");
	    ଅନ୍ୟଥା ଯଦି (flags & WF_RARE)
		STRCAT(badword, "?");
	    ଯଦି (flags & WF_REGION)
		ପାଇଁ (i = 0; i < 7; ++i)
		    ଯଦି (flags & (0x10000 << i))
			sprintf((ଅକ୍ଷର *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ଯଦି (dumpflags & DUMPFLAG_ଗଣନା)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_ଗଣନା);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    ଅନ୍ୟଥା ଯଦି (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p),
					  p_ic, ଖାଲି, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    ସ୍ଥିର linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dir,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dumpflags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_word_up = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    ଯଦି (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ଯଦି (byts != ଖାଲି)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ଯେତେବେଳେ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    ଯଦି (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		ଧାଡ଼ି_breakcheck();
	    }
	    ଅନ୍ୟଥା
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		ଯଦି (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    ପାଇଁ (i = 1; i < len; ++i)
			ଯଦି (byts[n + i] != 0)
			    ଭାଙ୍ଗ;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    ଯଦି (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ଯଦି (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ଯଦି (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			ଯଦି (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ଯଦି (lnum != 0)
				++lnum;
			}
		    }
		}
		ଅନ୍ୟଥା ଯଦି (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    ଫେରନ୍ତୁ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    ଯେତେବେଳେ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ଫେରନ୍ତୁ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_word_start(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol)
{
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = 0;

    ଯଦି (no_spell_checking(curwin))
	ଫେରନ୍ତୁ startcol;

    // Find a word character before "startcol".
    ଧାଡ଼ି = ml_get_curline();
    ପାଇଁ (p = ଧାଡ଼ି + startcol; p > ଧାଡ଼ି; )
    {
	MB_PTR_BACK(ଧାଡ଼ି, p);
	ଯଦି (spell_iswordp_nmw(p, curwin))
	    ଭାଙ୍ଗ;
    }

    // Go back to start of the word.
    ଯେତେବେଳେ (p > ଧାଡ଼ି)
    {
	col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି);
	MB_PTR_BACK(ଧାଡ଼ି, p);
	ଯଦି (!spell_iswordp(p, curwin))
	    ଭାଙ୍ଗ;
	col = 0;
    }

    ଫେରନ୍ତୁ col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spell_expand_need_cap;

    ଶୂନ୍ୟ
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    ଫେରନ୍ତୁ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_spelllang(char_u *val)
{
    ଫେରନ୍ତୁ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    spf = val;
    ଯେତେବେଳେ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ଯଦି (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ଫେରନ୍ତୁ FALSE;
	ପାଇଁ (s = spf_name; *s != NUL; ++s)
	    ଯଦି (!vim_is_fname_char(*s))
		ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    ଅକ୍ଷର *
did_set_spell_option(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର    *errmsg = ଖାଲି;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    ଭାଙ୍ଗ;
	}
    ଫେରନ୍ତୁ errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    ଅକ୍ଷର *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    ଯଦି (synblock->b_p_spc == ଖାଲି || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ଖାଲି;
    ଅନ୍ୟଥା
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ଯଦି (re != ଖାଲି)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ଯଦି (synblock->b_cap_prog == ଖାଲି)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ଫେରନ୍ତୁ e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    ଫେରନ୍ତୁ ଖାଲି;
}

#endif  // FEAT_SPELL
