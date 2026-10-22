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
#ضمن "vim.h"

#ܐܢ defined(FEAT_SPELL)

#ضمن <time.h>

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
تعريفنوع بنية matchinf_S
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
    صحيح		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    صحيح		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    صحيح		mi_prefcnt;		// number of entries at mi_prefarridx
    صحيح		mi_prefixlen;		// byte length of prefix
    صحيح		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    صحيح		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    صحيح		mi_complen;		// nr of compound words used
    صحيح		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    صحيح		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    صحيح		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    صحيح		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


ساكن صحيح spell_mb_isword_class(صحيح cl, win_T *wp);

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

ساكن فراغ find_word(matchinf_T *mip, صحيح mode);
ساكن فراغ find_prefix(matchinf_T *mip, صحيح mode);
ساكن صحيح fold_more(matchinf_T *mip);
ساكن فراغ spell_load_cb(char_u *fname, فراغ *cookie);
ساكن صحيح count_syllables(slang_T *slang, char_u *word);
ساكن فراغ clear_midword(win_T *buf);
ساكن فراغ use_midword(slang_T *lp, win_T *buf);
ساكن صحيح find_region(char_u *rp, char_u *region);
ساكن فراغ spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
ساكن فراغ spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
ساكن فراغ spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
ساكن فراغ dump_word(slang_T *slang, char_u *word, char_u *pat, صحيح *dir, صحيح round, صحيح flags, linenr_T lnum);
ساكن linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, صحيح *dir, صحيح round, صحيح flags, linenr_T startlnum);
ساكن char_u *advance_camelcase_word(char_u *p, win_T *wp, صحيح *is_camel_case);

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
    صحيح
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    صحيح		*capcol,	// column to check for Capital
    صحيح		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    صحيح		nrlen = 0;	// found a number first
    صحيح		c;
    صحيح		wrongcaplen = 0;
    صحيح		lpi;
    صحيح		count_word = docount;
    صحيح		use_camel_case = *wp->w_s->b_p_spo != NUL;
    صحيح		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    ܐܢ (*ptr <= ' ')
	ܦܢܐ 1;

    // Return here when loading language files failed.
    ܐܢ (wp->w_s->b_langp.ga_len == 0)
	ܦܢܐ 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    ܐܢ (*ptr >= '0' && *ptr <= '9')
    {
	ܐܢ (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	ܐܚܪܢܐ ܐܢ (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	ܐܚܪܢܐ
	    mi.mi_end = skipdigits(ptr);
	nrlen = (صحيح)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    ܐܢ (spell_iswordp(mi.mi_fend, wp))
    {
	ܐܢ (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	ܐܚܪܢܐ
	{
	    ܥܒܕ
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } ܟܡܐ (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	ܐܢ (capcol != عدم && *capcol == 0 && wp->w_s->b_cap_prog != عدم)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    ܐܢ (!SPELL_ISUPPER(c))
		wrongcaplen = (صحيح)(mi.mi_fend - ptr);
	}
    }
    ܐܢ (capcol != عدم)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = عدم;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    ܐܢ (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (فراغ)spell_casefold(wp, ptr, (صحيح)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (صحيح)STRLEN(mi.mi_fword);

    ܐܢ (is_camel_case && mi.mi_fwordlen > 0)
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
    لكل (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	ܐܢ (mi.mi_lp->lp_slang->sl_fidxs == عدم)
	    استمر;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	ܐܢ (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	ܐܢ (count_word && mi.mi_result == SP_OK)
	{
	    count_common_word(mi.mi_lp->lp_slang, ptr,
						   (صحيح)(mi.mi_end - ptr), 1);
	    count_word = FALSE;
	}
    }

    ܐܢ (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	ܐܢ (nrlen > 0)
	{
	    ܐܢ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		ܦܢܐ nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	ܐܚܪܢܐ ܐܢ (!spell_iswordp_nmw(ptr, wp))
	{
	    ܐܢ (capcol != عدم && wp->w_s->b_cap_prog != عدم)
	    {
		regmatch_T	regmatch;
		صحيح		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		ܐܢ (r)
		    *capcol = (صحيح)(regmatch.endp[0] - ptr);
	    }

	    ܐܢ (has_mbyte)
		ܦܢܐ (*mb_ptr2len)(ptr);
	    ܦܢܐ 1;
	}
	ܐܚܪܢܐ ܐܢ (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	ܐܚܪܢܐ ܐܢ (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    صحيح		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    ܐܢ (mi.mi_lp->lp_slang->sl_fidxs != عدم)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		لكل (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    ܐܢ (p >= mi.mi_end)
			اكسر;
		    mi.mi_compoff = (صحيح)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    ܐܢ (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			اكسر;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	ܐܢ (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	ܐܚܪܢܐ ܐܢ (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	ܐܚܪܢܐ
	    *attrp = HLF_SPL;
    }

    ܐܢ (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	ܦܢܐ wrongcaplen;
    }

    ܦܢܐ (صحيح)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    ساكن صحيح
get_char_type(صحيح c)
{
    ܐܢ (VIM_ISDIGIT(c))
	ܦܢܐ CHAR_DIGIT;
    ܐܢ (SPELL_ISUPPER(c))
	ܦܢܐ CHAR_UPPER;
    ܦܢܐ CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    ساكن char_u *
advance_camelcase_word(char_u *str, win_T *wp, صحيح *is_camel_case)
{
    صحيح last_type, last_last_type, this_type;
    صحيح c;
    char_u *end = str;

    *is_camel_case = FALSE;

    ܐܢ (*str == NUL)
	ܦܢܐ str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    ܟܡܐ (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	ܐܢ (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    اكسر;
	}
	ܐܚܪܢܐ ܐܢ ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    اكسر;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    ܦܢܐ end;
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
    ساكن فراغ
find_word(matchinf_T *mip, صحيح mode)
{
    idx_T	arridx = 0;
    صحيح		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    صحيح		endidxcnt = 0;
    صحيح		len;
    صحيح		wlen = 0;
    صحيح		flen;
    صحيح		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    صحيح		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    صحيح		word_ends;
    صحيح		prefix_found;
    صحيح		nobreak_result;

    ܐܢ (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	ܐܢ (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    ܐܚܪܢܐ
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	ܐܢ (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	ܐܚܪܢܐ ܐܢ (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    ܐܢ (byts == عدم)
	ܦܢܐ;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    لكل (;;)
    {
	ܐܢ (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	ܐܢ (byts[arridx] == 0)
	{
	    ܐܢ (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		ܦܢܐ;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    ܟܡܐ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    ܐܢ (len == 0)
		اكسر;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ܐܢ (ptr[wlen] == NUL)
	    اكسر;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	ܐܢ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	ܟܡܐ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ܐܢ (byts[m] > c)
		hi = m - 1;
	    ܐܚܪܢܐ ܐܢ (byts[m] < c)
		lo = m + 1;
	    ܐܚܪܢܐ
	    {
		lo = hi = m;
		اكسر;
	    }
	}

	// Stop if there is no matching byte.
	ܐܢ (hi < lo || byts[lo] != c)
	    اكسر;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ܐܢ (c == ' ')
	{
	    لكل (;;)
	    {
		ܐܢ (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		ܐܢ (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    اكسر;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    ܟܡܐ (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	ܐܢ ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    استمر;	    // not at first byte of character
	ܐܢ (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    ܐܢ (slang->sl_compprog == عدم && !slang->sl_nobreak)
		استمر;	    // next char is a word character
	    word_ends = FALSE;
	}
	ܐܚܪܢܐ
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	ܐܢ (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    ܐܢ (STRNCMP(ptr, p, wlen) != 0)
	    {
		لكل (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (صحيح)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	لكل (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    ܐܢ (mode == FIND_FOLDWORD)
	    {
		ܐܢ (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		ܐܢ (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    استمر;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    ܐܚܪܢܐ ܐܢ (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		ܐܢ (c == 0)
		    استمر;

		// Use the WF_RARE flag for a rare prefix.
		ܐܢ (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    ܐܢ (slang->sl_nobreak)
	    {
		ܐܢ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    اكسر;
		}
	    }

	    ܐܚܪܢܐ ܐܢ ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		ܐܢ (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    استمر;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		ܐܢ (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			استمر;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		ܐܢ (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    استمر;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		ܐܢ (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    استمر;
		ܐܢ (!word_ends && (flags & WF_NOCOMPAFT))
		    استمر;

		// Quickly check if compounding is possible with this flag.
		ܐܢ (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    استمر;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		ܐܢ (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    استمر;

		ܐܢ (mode == FIND_COMPOUND)
		{
		    صحيح	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    ܐܢ (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			لكل (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    ܐܚܪܢܐ
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    ܐܢ (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			استمر;

		    ܐܢ (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			ܐܢ (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    استمر;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		ܐܢ (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    ܐܢ (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			ܐܢ (ptr == mip->mi_word)
			    (فراغ)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			ܐܚܪܢܐ
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    ܐܢ (!can_compound(slang, fword, mip->mi_compflags))
			استمر;
		}
		ܐܚܪܢܐ ܐܢ (slang->sl_comprules != عدم
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    استمر;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    ܐܚܪܢܐ ܐܢ (flags & WF_NEEDCOMP)
		استمر;

	    nobreak_result = SP_OK;

	    ܐܢ (!word_ends)
	    {
		صحيح	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		صحيح	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		ܐܢ (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		ܐܢ (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    ܐܢ (STRNCMP(ptr, p, wlen) != 0)
		    {
			لكل (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (صحيح)(p - mip->mi_fword);
		    }
		}
#ܐܢ 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		ܐܢ (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		لكل (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    ܐܢ (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			ܐܢ (mip->mi_lp->lp_slang->sl_fidxs == عدم
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    استمر;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    ܐܢ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#ܐܢ 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			ܐܢ (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    ܐܢ (!slang->sl_nobreak)
			اكسر;
		}
		--mip->mi_complen;
		ܐܢ (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		ܐܢ (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		ܐܚܪܢܐ
		{
		    ܐܢ (mip->mi_result == SP_OK)
			اكسر;
		    استمر;
		}
	    }

	    ܐܢ (flags & WF_BANNED)
		res = SP_BANNED;
	    ܐܚܪܢܐ ܐܢ (flags & WF_REGION)
	    {
		// Check region.
		ܐܢ ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		ܐܚܪܢܐ
		    res = SP_LOCAL;
	    }
	    ܐܚܪܢܐ ܐܢ (flags & WF_RARE)
		res = SP_RARE;
	    ܐܚܪܢܐ
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    ܐܢ (nobreak_result == SP_BAD)
	    {
		ܐܢ (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		ܐܚܪܢܐ ܐܢ (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    ܐܚܪܢܐ ܐܢ (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    ܐܚܪܢܐ ܐܢ (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    ܐܢ (mip->mi_result == SP_OK)
		اكسر;
	}

	ܐܢ (mip->mi_result == SP_OK)
	    اكسر;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    صحيح
match_checkcompoundpattern(
    char_u	*ptr,
    صحيح		wlen,
    garray_T	*gap)  // &sl_comppat
{
    صحيح		i;
    char_u	*p;
    صحيح		len;

    لكل (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	ܐܢ (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (صحيح)STRLEN(p);
	    ܐܢ (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		ܦܢܐ TRUE;
	}
    }
    ܦܢܐ FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    صحيح
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    صحيح		i;
    char_u	*p;

    ܐܢ (slang->sl_compprog == عدم)
	ܦܢܐ FALSE;
    ܐܢ (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	لكل (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    ܐܚܪܢܐ
	p = flags;
    ܐܢ (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	ܦܢܐ FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    ܐܢ (slang->sl_compsylmax < MAXWLEN
		       && count_syllables(slang, word) > slang->sl_compsylmax)
	ܦܢܐ (صحيح)STRLEN(flags) < slang->sl_compmax;
    ܦܢܐ TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    صحيح
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    صحيح		i;
    صحيح		c;

    // loop over all the COMPOUNDRULE entries
    لكل (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	لكل (i = 0; ; ++i)
	{
	    c = compflags[i];
	    ܐܢ (c == NUL)
		// found a rule that matches for the flags we have so far
		ܦܢܐ TRUE;
	    ܐܢ (*p == '/' || *p == NUL)
		اكسر;  // end of rule, it's too short
	    ܐܢ (*p == '[')
	    {
		صحيح match = FALSE;

		// compare against all the flags in []
		++p;
		ܟܡܐ (*p != ']' && *p != NUL)
		    ܐܢ (*p++ == c)
			match = TRUE;
		ܐܢ (!match)
		    اكسر;  // none matches
	    }
	    ܐܚܪܢܐ ܐܢ (*p != c)
		اكسر;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	ܐܢ (p == عدم)
	    اكسر;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    ܦܢܐ FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    صحيح
valid_word_prefix(
    صحيح		totprefcnt,	// nr of prefix IDs
    صحيح		arridx,		// idx in sl_pidxs[]
    صحيح		flags,
    char_u	*word,
    slang_T	*slang,
    صحيح		cond_req)	// only use prefixes with a condition
{
    صحيح		prefcnt;
    صحيح		pidx;
    regprog_T	**rp;
    صحيح		prefid;

    prefid = (unsigned)flags >> 24;
    لكل (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	ܐܢ (prefid != (pidx & 0xff))
	    استمر;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	ܐܢ ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    استمر;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	ܐܢ (*rp != عدم)
	{
	    ܐܢ (!vim_regexec_prog(rp, FALSE, word, 0))
		استمر;
	}
	ܐܚܪܢܐ ܐܢ (cond_req)
	    استمر;

	// It's a match!  Return the WF_ flags.
	ܦܢܐ pidx;
    }
    ܦܢܐ 0;
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
    ساكن فراغ
find_prefix(matchinf_T *mip, صحيح mode)
{
    idx_T	arridx = 0;
    صحيح		len;
    صحيح		wlen = 0;
    صحيح		flen;
    صحيح		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    ܐܢ (byts == عدم)
	ܦܢܐ;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    ܐܢ (mode == FIND_COMPOUND)
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
    لكل (;;)
    {
	ܐܢ (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	ܐܢ (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    ܟܡܐ (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    ܐܢ (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    ܐܢ (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    ܐܚܪܢܐ
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    ܐܢ (len == 0)
		اكسر;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	ܐܢ (ptr[wlen] == NUL)
	    اكسر;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	ܟܡܐ (lo < hi)
	{
	    m = (lo + hi) / 2;
	    ܐܢ (byts[m] > c)
		hi = m - 1;
	    ܐܚܪܢܐ ܐܢ (byts[m] < c)
		lo = m + 1;
	    ܐܚܪܢܐ
	    {
		lo = hi = m;
		اكسر;
	    }
	}

	// Stop if there is no matching byte.
	ܐܢ (hi < lo || byts[lo] != c)
	    اكسر;

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
    ساكن صحيح
fold_more(matchinf_T *mip)
{
    صحيح		flen;
    char_u	*p;

    p = mip->mi_fend;
    ܥܒܕ
	MB_PTR_ADV(mip->mi_fend);
    ܟܡܐ (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    ܐܢ (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (فراغ)spell_casefold(mip->mi_win, p, (صحيح)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (صحيح)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    ܦܢܐ flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    صحيح
spell_valid_case(
    صحيح	    wordflags,	    // flags for the checked word.
    صحيح	    treeflags)	    // flags for the word in the spell tree
{
    ܦܢܐ ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    صحيح
spell_check_window(win_T *wp)
{
    ܦܢܐ wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(حرف **)(wp->w_s->b_langp.ga_data) != عدم;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    ساكن صحيح
no_spell_checking(win_T *wp)
{
    ܐܢ (spell_check_window(wp))
	ܦܢܐ FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    ܦܢܐ TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    صحيح
spell_move_to(
    win_T	*wp,
    صحيح		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    صحيح		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    صحيح		found_len = 0;
    char_u	*line;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    صحيح		len;
#ifdef FEAT_SYN_HL
    صحيح		has_syntax = syntax_present(wp);
#endif
    صحيح		col;
    صحيح		can_spell;
    char_u	*buf = عدم;
    صحيح		buflen = 0;
    صحيح		skip = 0;
    صحيح		capcol = -1;
    صحيح		found_one = FALSE;
    صحيح		wrapped = FALSE;

    ܐܢ (no_spell_checking(wp))
	ܦܢܐ 0;

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

    ܟܡܐ (!got_int)
    {
	صحيح empty_line;

	line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	ܐܢ (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    ܐܢ (buf == عدم)
		اكسر;
	}

	// In first line check first word for Capital.
	ܐܢ (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	ܐܢ (capcol == 0)
	    capcol = getwhitecols(line);
	ܐܚܪܢܐ ܐܢ (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(line);
	    ܐܢ (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_line = *skipwhite(line) == NUL;
	STRCPY(buf, line);
	ܐܢ (lnum < wp->w_buffer->b_ml.ml_line_count)
	    spell_cat_line(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	ܟܡܐ (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    ܐܢ (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		اكسر;

	    // start of word
	    attr = HLF_COUNT;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    ܐܢ (attr != HLF_COUNT)
	    {
		// We found a bad word.  Check the attribute.
		ܐܢ (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    ܐܢ (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			ܐܢ (has_syntax)
			{
			    col = (صحيح)(p - buf);
			    (فراغ)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    ܐܢ (!can_spell)
				attr = HLF_COUNT;
			}
			ܐܚܪܢܐ
#endif
			    can_spell = TRUE;

			ܐܢ (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (صحيح)(p - buf);
			    found_pos.coladd = 0;
			    ܐܢ (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				ܐܢ (attrp != عدم)
				    *attrp = attr;
				ܦܢܐ len;
			    }
			    ܐܚܪܢܐ ܐܢ (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    ܐܚܪܢܐ
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	ܐܢ (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    ܦܢܐ found_len;
	}

	ܐܢ (curline)
	    اكسر;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	ܐܢ (lnum == wp->w_cursor.lnum && wrapped)
	    اكسر;

	// Advance to next line.
	ܐܢ (dir == BACKWARD)
	{
	    ܐܢ (lnum > 1)
		--lnum;
	    ܐܚܪܢܐ ܐܢ (!p_ws)
		اكسر;	    // at first line and 'nowrapscan'
	    ܐܚܪܢܐ
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_line_count;
		wrapped = TRUE;
		ܐܢ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    capcol = -1;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (lnum < wp->w_buffer->b_ml.ml_line_count)
		++lnum;
	    ܐܚܪܢܐ ܐܢ (!p_ws)
		اكسر;	    // at first line and 'nowrapscan'
	    ܐܚܪܢܐ
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		ܐܢ (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    ܐܢ (lnum == wp->w_cursor.lnum && !found_one)
		اكسر;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    ܐܢ (attr == HLF_COUNT)
		skip = (صحيح)(p - endp);
	    ܐܚܪܢܐ
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    ܐܢ (empty_line)
		capcol = 0;
	}

	line_breakcheck();
    }

    vim_free(buf);
    ܦܢܐ 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    فراغ
spell_cat_line(char_u *buf, char_u *line, صحيح maxlen)
{
    char_u	*p;
    صحيح		n;

    p = skipwhite(line);
    ܟܡܐ (vim_strchr((char_u *)"*#/\"\t", *p) != عدم)
	p = skipwhite(p + 1);

    ܐܢ (*p == NUL)
	ܦܢܐ;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (صحيح)(p - line) + 1;
    ܐܢ (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
تعريفنوع بنية spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    صحيح	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    ساكن فراغ
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    صحيح		r;
    spelload_T	sl;
    صحيح		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = عدم;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    لكل (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((حرف *)fname_enc, حجم(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#ܐܚܪܢܐ
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	ܐܢ (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((حرف *)fname_enc, حجم(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#ܐܚܪܢܐ
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    ܐܢ (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		استمر;
	    اكسر;
	}
	اكسر;
    }

    ܐܢ (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#ܐܚܪܢܐ
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    ܐܚܪܢܐ ܐܢ (sl.sl_slang != عدم)
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
spell_enc(فراغ)
{

    ܐܢ (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	ܦܢܐ p_enc;
    ܦܢܐ (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    ساكن فراغ
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((حرف *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    ܐܢ (lp != عدم)
    {
	ܐܢ (lang != عدم)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, حجم(fromto_T), 10);
	ga_init2(&lp->sl_repsal, حجم(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    ܦܢܐ lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    فراغ
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
    فراغ
free_sal_items(garray_T *gap)
{
    salitem_T	*smp;

    ܟܡܐ (gap->ga_len > 0)
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
    فراغ
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    صحيح		i;
    صحيح		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    لكل (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	ܟܡܐ (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    ܐܢ (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	ܐܢ (gap->ga_data != عدم)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    لكل (i = 0; i < gap->ga_len; ++i)
		vim_free(((صحيح **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    ܐܚܪܢܐ
	free_sal_items(gap);

    لكل (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = عدم;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_items);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_KEY_OFF);
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
    فراغ
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = عدم;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    ساكن فراغ
spell_load_cb(char_u *fname, فراغ *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, عدم, FALSE);
    ܐܢ (slang == عدم)
	ܦܢܐ;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    ܐܢ (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    ܐܚܪܢܐ ܐܢ (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    فراغ
count_common_word(
    slang_T	*lp,
    char_u	*word,
    صحيح		len,	    // word length, -1 for up to NUL
    صحيح		count)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    ܐܢ (len == -1)
	p = word;
    ܐܚܪܢܐ ܐܢ (len >= MAXWLEN)
	ܦܢܐ;
    ܐܚܪܢܐ
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    ܐܢ (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	ܐܢ (wc == عدم)
	    ܦܢܐ;
	STRCPY(wc->wc_word, p);
	wc->wc_count = count;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    ܐܚܪܢܐ
    {
	wc = HI2WC(hi);
	ܐܢ ((wc->wc_count += count) < (unsigned)count)	// check for overflow
	    wc->wc_count = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    صحيح
byte_in_str(char_u *str, صحيح n)
{
    char_u	*p;

    لكل (p = str; *p != NUL; ++p)
	ܐܢ (*p == n)
	    ܦܢܐ TRUE;
    ܦܢܐ FALSE;
}

#define SY_MAXLEN   30
تعريفنوع بنية syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    صحيح		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    صحيح
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    صحيح		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_items, حجم(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    ܟܡܐ (p != عدم)
    {
	*p++ = NUL;
	ܐܢ (*p == NUL)	    // trailing slash
	    اكسر;
	s = p;
	p = vim_strchr(p, '/');
	ܐܢ (p == عدم)
	    l = (صحيح)STRLEN(s);
	ܐܚܪܢܐ
	    l = (صحيح)(p - s);
	ܐܢ (l >= SY_MAXLEN)
	    ܦܢܐ SP_FORMERROR;
	ܐܢ (ga_grow(&slang->sl_syl_items, 1) == FAIL)
	    ܦܢܐ SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_items.ga_data)
					       + slang->sl_syl_items.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    ܦܢܐ OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    ساكن صحيح
count_syllables(slang_T *slang, char_u *word)
{
    صحيح		cnt = 0;
    صحيح		skip = FALSE;
    char_u	*p;
    صحيح		len;
    صحيح		i;
    syl_item_T	*syl;
    صحيح		c;

    ܐܢ (slang->sl_syllable == عدم)
	ܦܢܐ 0;

    لكل (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	ܐܢ (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    استمر;
	}

	// Find longest match of syllable items.
	len = 0;
	لكل (i = 0; i < slang->sl_syl_items.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_items.ga_data) + i;
	    ܐܢ (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	ܐܢ (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	ܐܚܪܢܐ
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    ܐܢ (vim_strchr(slang->sl_syllable, c) == عدم)
		skip = FALSE;	    // No, search for next syllable
	    ܐܚܪܢܐ ܐܢ (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    ܦܢܐ cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    حرف *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    صحيح		filename;
    صحيح		region_mask;
    slang_T	*slang;
    صحيح		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    صحيح		len;
    char_u	*p;
    صحيح		round;
    char_u	*spf;
    char_u	*use_region = عدم;
    صحيح		dont_use_region = FALSE;
    صحيح		nobreak = FALSE;
    صحيح		i, j;
    langp_T	*lp, *lp2;
    ساكن صحيح	recursive = FALSE;
    حرف	*ret_msg = عدم;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    ܐܢ (recursive)
	ܦܢܐ عدم;
    recursive = TRUE;

    ga_init2(&ga, حجم(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    ܐܢ (spl_copy == عدم)
	اذهب theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    لكل (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = عدم;

	ܐܢ (!valid_spelllang(lang))
	    استمر;

	ܐܢ (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    استمر;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	ܐܢ (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    ܐܢ (p != عدم && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    ܐܚܪܢܐ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ܐܢ (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    اكسر;
	}
	ܐܚܪܢܐ
	{
	    filename = FALSE;
	    ܐܢ (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    ܐܚܪܢܐ
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		ܐܢ (STRICMP(lang, slang->sl_name) == 0)
		    اكسر;
	}

	ܐܢ (region != عدم)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    ܐܢ (use_region != عدم && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	ܐܢ (slang == عدم)
	{
	    ܐܢ (filename)
		(فراغ)spell_load_file(lang, lang, عدم, FALSE);
	    ܐܚܪܢܐ
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		ܐܢ (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_msg = N_(e_spellfilemising_autocommand_deleted_buffer);
		    اذهب theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    ܐܢ (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		ܐܢ (!filename && region != عدم)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    ܐܢ (c == REGION_ALL)
		    {
			ܐܢ (slang->sl_add)
			{
			    ܐܢ (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			ܐܚܪܢܐ
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    ܐܚܪܢܐ
			region_mask = 1 << c;
		}

		ܐܢ (region_mask != 0)
		{
		    ܐܢ (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_msg = e_out_of_memory;
			اذهب theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    ܐܢ (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    لكل (round = 0; round == 0 || *spf != NUL; ++round)
    {
	ܐܢ (round == 0)
	{
	    // Internal wordlist, if there is one.
	    ܐܢ (int_wordlist == عدم)
		استمر;
	    int_wordlist_spl(spf_name);
	}
	ܐܚܪܢܐ
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    لكل (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		ܐܢ (p != عدم && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    اكسر;
	    }
	    ܐܢ (c < ga.ga_len)
		استمر;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    ܐܢ (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		اكسر;
	ܐܢ (slang == عدم)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    ܐܢ (round == 0)
		STRCPY(lang, "internal wordlist");
	    ܐܚܪܢܐ
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		ܐܢ (p != عدم)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, عدم, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    ܐܢ (slang != عدم && nobreak)
		slang->sl_nobreak = TRUE;
	}
	ܐܢ (slang != عدم && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    ܐܢ (use_region != عدم && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		ܐܢ (c != REGION_ALL)
		    region_mask = 1 << c;
		ܐܚܪܢܐ ܐܢ (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    ܐܢ (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = عدم;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = عدم;
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
    لكل (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	ܐܢ (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	ܐܚܪܢܐ
	    // find first similar language that does sound folding
	    لكل (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ܐܢ (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    اكسر;
		}
	    }

	// REP items
	ܐܢ (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	ܐܚܪܢܐ
	    // find first similar language that has REP items
	    لكل (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		ܐܢ (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    اكسر;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    ܦܢܐ ret_msg;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    ساكن فراغ
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    ساكن فراغ
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    ܐܢ (lp->sl_midword == عدم)	    // there aren't any
	ܦܢܐ;

    لكل (p = lp->sl_midword; *p != NUL; )
	ܐܢ (has_mbyte)
	{
	    صحيح	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    ܐܢ (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    ܐܚܪܢܐ ܐܢ (wp->w_s->b_spell_ismw_mb == عدم)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    ܐܚܪܢܐ
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (صحيح)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		ܐܢ (bp != عدم)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	ܐܚܪܢܐ
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    ساكن صحيح
find_region(char_u *rp, char_u *region)
{
    صحيح		i;

    لكل (i = 0; ; i += 2)
    {
	ܐܢ (rp[i] == NUL)
	    ܦܢܐ REGION_ALL;
	ܐܢ (rp[i] == region[0] && rp[i + 1] == region[1])
	    اكسر;
    }
    ܦܢܐ i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    صحيح
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    صحيح		c;
    صحيح		firstcap;
    صحيح		allcap;
    صحيح		past_second = FALSE;	// past second word char

    // find first letter
    لكل (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	ܐܢ (end == عدم ? *p == NUL : p >= end)
	    ܦܢܐ 0;	    // only non-word characters, illegal word
    ܐܢ (has_mbyte)
	c = mb_ptr2char_adv(&p);
    ܐܚܪܢܐ
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    لكل ( ; end == عدم ? *p != NUL : p < end; MB_PTR_ADV(p))
	ܐܢ (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    ܐܢ (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		ܐܢ (past_second && allcap)
		    ܦܢܐ WF_KEEPCAP;
		allcap = FALSE;
	    }
	    ܐܚܪܢܐ ܐܢ (!allcap)
		// UlU -> KEEPCAP
		ܦܢܐ WF_KEEPCAP;
	    past_second = TRUE;
	}

    ܐܢ (allcap)
	ܦܢܐ WF_ALLCAP;
    ܐܢ (firstcap)
	ܦܢܐ WF_ONECAP;
    ܦܢܐ 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    فراغ
spell_delete_wordlist(فراغ)
{
    char_u	fname[MAXPATHL];

    ܐܢ (int_wordlist == عدم)
	ܦܢܐ;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    فراغ
spell_free_all(فراغ)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    ܟܡܐ (first_lang != عدم)
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
    فراغ
spell_reload(فراغ)
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
	ܐܢ (*wp->w_s->b_p_spl != NUL)
	{
		ܐܢ (wp->w_p_spell)
		{
		    (فراغ)parse_spelllang(wp);
		    اكسر;
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
open_spellbuf(فراغ)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    ܐܢ (buf == عدم)
	ܦܢܐ عدم;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_key = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    ܦܢܐ buf;
}

/*
 * Close the buffer used for spell info.
 */
    فراغ
close_spellbuf(buf_T *buf)
{
    ܐܢ (buf == عدم)
	ܦܢܐ;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    فراغ
clear_spell_chartab(spelltab_T *sp)
{
    صحيح		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    لكل (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    لكل (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    لكل (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    لكل (i = 'a'; i <= 'z'; ++i)
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
    فراغ
init_spell_chartab(فراغ)
{
    صحيح	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    ܐܢ (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	لكل (i = 128; i <= 255; ++i)
	    ܐܢ (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    ܐܚܪܢܐ ܐܢ (enc_utf8)
    {
	لكل (i = 128; i < 256; ++i)
	{
	    صحيح f = utf_fold(i);
	    صحيح u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    ܐܚܪܢܐ
    {
	// Rough guess: use locale-dependent library functions.
	لكل (i = 128; i < 256; ++i)
	{
	    ܐܢ (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    ܐܚܪܢܐ ܐܢ (MB_ISLOWER(i))
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
    صحيح
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    صحيح		l;
    صحيح		c;

    ܐܢ (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	ܐܢ (l == 1)
	{
	    // be quick for ASCII
	    ܐܢ (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	ܐܚܪܢܐ
	{
	    c = mb_ptr2char(p);
	    ܐܢ (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != عدم
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != عدم))
		s = p + l;
	}

	c = mb_ptr2char(s);
	ܐܢ (c > 255)
	    ܦܢܐ spell_mb_isword_class(mb_get_class(s), wp);
	ܦܢܐ spelltab.st_isw[c];
    }

    ܦܢܐ spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    صحيح
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    صحيح		c;

    ܐܢ (has_mbyte)
    {
	c = mb_ptr2char(p);
	ܐܢ (c > 255)
	    ܦܢܐ spell_mb_isword_class(mb_get_class(p), wp);
	ܦܢܐ spelltab.st_isw[c];
    }
    ܦܢܐ spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    ساكن صحيح
spell_mb_isword_class(صحيح cl, win_T *wp)
{
    ܐܢ (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	ܦܢܐ cl == 2 || cl == 0x2800;
    ܦܢܐ cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    ساكن صحيح
spell_iswordp_w(صحيح *p, win_T *wp)
{
    صحيح		*s;

    ܐܢ (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != عدم
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != عدم))
	s = p + 1;
    ܐܚܪܢܐ
	s = p;

    ܐܢ (*s > 255)
    {
	ܐܢ (enc_utf8)
	    ܦܢܐ spell_mb_isword_class(utf_class(*s), wp);
	ܐܢ (enc_dbcs)
	    ܦܢܐ spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	ܦܢܐ 0;
    }
    ܦܢܐ spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    صحيح
spell_casefold(
    win_T	*wp,
    char_u	*str,
    صحيح		len,
    char_u	*buf,
    صحيح		buflen)
{
    صحيح		i;

    ܐܢ (len >= buflen)
    {
	buf[0] = NUL;
	ܦܢܐ FAIL;		// result will not fit
    }

    ܐܢ (has_mbyte)
    {
	صحيح	outi = 0;
	char_u	*p;
	صحيح	c;

	// Fold one character at a time.
	لكل (p = str; p < str + len; )
	{
	    ܐܢ (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		ܦܢܐ FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    ܐܢ (c == 0x03a3 || c == 0x03c2)
	    {
		ܐܢ (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		ܐܚܪܢܐ
		    c = 0x03c3;
	    }
	    ܐܚܪܢܐ
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    ܐܚܪܢܐ
    {
	// Be quick for non-multibyte encodings.
	لكل (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    ܦܢܐ OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    صحيح
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    ܐܢ (wp->w_s->b_cap_prog == عدم)
	ܦܢܐ FALSE;

    صحيح		need_cap = FALSE;
    char_u	*line = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : عدم;
    char_u	*line_copy = عدم;
    colnr_T	endcol = 0;

    ܐܢ (col == 0 || getwhitecols(line) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	ܐܢ (lnum == 1)
	    need_cap = TRUE;
	ܐܚܪܢܐ
	{
	    line = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    ܐܢ (*skipwhite(line) == NUL)
		need_cap = TRUE;
	    ܐܚܪܢܐ
	    {
		// Append a space in place of the line break.
		line_copy = concat_str(line, (char_u *)" ");
		ܐܢ (line_copy == عدم)
		    ܦܢܐ FALSE;

		line = line_copy;
		endcol = (colnr_T)STRLEN(line);
	    }
	}
    }
    ܐܚܪܢܐ
	endcol = col;

    ܐܢ (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = line + endcol;
	لكل (;;)
	{
	    MB_PTR_BACK(line, p);
	    ܐܢ (p == line || spell_iswordp_nmw(p, wp))
		اكسر;
	    ܐܢ (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == line + endcol)
	    {
		need_cap = TRUE;
		اكسر;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(line_copy);

    ܦܢܐ need_cap;
}


/*
 * ":spellrepall"
 */
    فراغ
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*line;
    char_u	*p;
    صحيح		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    ܐܢ (repl_from == عدم || repl_to == عدم)
    {
	emsg(_(e_no_previous_spell_replacement));
	ܦܢܐ;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    ܐܢ (frompat == عدم)
	ܦܢܐ;
    frompatlen = vim_snprintf((حرف *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    ܟܡܐ (!got_int)
    {
	ܐܢ (do_search(عدم, '/', '/', frompat, frompatlen, 1L, SEARCH_KEEP, عدم) == 0
						   || u_save_cursor() == FAIL)
	    اكسر;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	line = ml_get_curline();
	ܐܢ (addlen <= 0 || STRNCMP(line + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    ܐܢ (p == عدم)
		اكسر;
	    mch_memmove(p, line, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, line + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#ܐܢ defined(FEAT_PROP_POPUP)
	    ܐܢ (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (صحيح)addlen, APC_SUBSTITUTE);
#endif

	    ܐܢ (curwin->w_cursor.lnum != prev_lnum)
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

    ܐܢ (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    ܐܚܪܢܐ
	do_sub_msg(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    فراغ
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    صحيح		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    صحيح		c;
    صحيح		l;

    p = word;
    ܐܢ (has_mbyte)
	c = mb_cptr2char_adv(&p);
    ܐܚܪܢܐ
	c = *p++;
    ܐܢ (upper)
	c = SPELL_TOUPPER(c);
    ܐܚܪܢܐ
	c = SPELL_TOFOLD(c);
    ܐܢ (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    ܐܚܪܢܐ
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
    فراغ
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    صحيح		c;

    d = wcopy;
    لكل (s = word; *s != NUL; )
    {
	ܐܢ (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	ܐܚܪܢܐ
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	ܐܢ (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    ܐܢ (d - wcopy >= MAXWLEN - 1)
		اكسر;
	    *d++ = c;
	}
	ܐܚܪܢܐ
	    c = SPELL_TOUPPER(c);

	ܐܢ (has_mbyte)
	{
	    ܐܢ (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		اكسر;
	    d += mb_char2bytes(c, d);
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (d - wcopy >= MAXWLEN - 1)
		اكسر;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    صحيح
nofold_len(char_u *fword, صحيح flen, char_u *word)
{
    char_u	*p;
    صحيح		i = 0;

    لكل (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    لكل (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    ܦܢܐ (صحيح)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    فراغ
make_case_word(char_u *fword, char_u *cword, صحيح flags)
{
    ܐܢ (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    ܐܚܪܢܐ ܐܢ (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    ܐܚܪܢܐ
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#ܐܢ defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    صحيح		lpi;

    ܐܢ (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	لكل (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    ܐܢ (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		ܦܢܐ vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    ܦܢܐ vim_strsave(word);
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
    فراغ
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    صحيح		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    ܐܢ (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    ܐܚܪܢܐ
    {
	// SAL items used.  Requires the word to be case-folded.
	ܐܢ (folded)
	    word = inword;
	ܐܚܪܢܐ
	{
	    (فراغ)spell_casefold(curwin,
				  inword, (صحيح)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	ܐܢ (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	ܐܚܪܢܐ
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    ساكن فراغ
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    صحيح		ri = 0;
    صحيح		c;

    ܐܢ (has_mbyte)
    {
	صحيح	prevc = 0;
	صحيح	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	لكل (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    ܐܢ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    ܐܚܪܢܐ ܐܢ (c < 256)
		c = slang->sl_sal_first[c];
	    ܐܚܪܢܐ
	    {
		ip = ((صحيح **)slang->sl_sal.ga_data)[c & 0xff];
		ܐܢ (ip == عدم)		// empty list, can't match
		    c = NUL;
		ܐܚܪܢܐ
		    لكل (;;)		// find "c" in the list
		    {
			ܐܢ (*ip == 0)	// not found
			{
			    c = NUL;
			    اكسر;
			}
			ܐܢ (*ip == c)	// match!
			{
			    c = ip[1];
			    اكسر;
			}
			ip += 2;
		    }
	    }

	    ܐܢ (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		ܐܢ (ri + MB_MAXBYTES > MAXWLEN)
		    اكسر;
		prevc = c;
	    }
	}
    }
    ܐܚܪܢܐ
    {
	// The sl_sal_first[] table contains the translation.
	لكل (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    ܐܢ (VIM_ISWHITE(c))
		c = ' ';
	    ܐܚܪܢܐ
		c = slang->sl_sal_first[c];
	    ܐܢ (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    ساكن فراغ
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    صحيح		i, j, z;
    صحيح		reslen;
    صحيح		n, k = 0;
    صحيح		z0;
    صحيح		k0;
    صحيح		n0;
    صحيح		c;
    صحيح		pri;
    صحيح		p0 = -333;
    صحيح		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    ܐܢ (slang->sl_rem_accents)
    {
	t = word;
	ܟܡܐ (*s != NUL)
	{
	    ܐܢ (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    ܐܚܪܢܐ
	    {
		ܐܢ (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    ܐܚܪܢܐ
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    ܟܡܐ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	ܐܢ (n >= 0)
	{
	    // check all rules for the same letter
	    لكل (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		ܐܢ (k > 1)
		{
		    ܐܢ (word[i + 1] != s[1])
			استمر;
		    ܐܢ (k > 2)
		    {
			لكل (j = 2; j < k; ++j)
			    ܐܢ (word[i + j] != s[j])
				اكسر;
			ܐܢ (j < k)
			    استمر;
		    }
		}

		ܐܢ ((pf = smp[n].sm_oneof) != عدم)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ܟܡܐ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ܐܢ (*pf == NUL)
			استمر;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ܟܡܐ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ܐܢ (*s == '<')
		    s++;
		ܐܢ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ܐܢ (*s == '^' && *(s + 1) == '^')
		    s++;

		ܐܢ (*s == NUL
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

		    ܐܢ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			لكل ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    ܐܢ (k0 > 1)
			    {
				ܐܢ (word[i + k] != s[1])
				    استمر;
				ܐܢ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    لكل (j = 2; j < k0; ++j)
					ܐܢ (*pf++ != s[j])
					    اكسر;
				    ܐܢ (j < k0)
					استمر;
				}
			    }
			    k0 += k - 1;

			    ܐܢ ((pf = smp[n0].sm_oneof) != عدم)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ܟܡܐ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ܐܢ (*pf == NUL)
				    استمر;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ܟܡܐ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ܐܢ (*s == '<')
				s++;
			    ܐܢ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ܐܢ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				ܐܢ (k0 == k)
				    // this is just a piece of the string
				    استمر;

				ܐܢ (p0 < pri)
				    // priority too low
				    استمر;
				// rule fits; stop search
				اكسر;
			    }
			}

			ܐܢ (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    استمر;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    ܐܢ (s == عدم)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != عدم) ? 1 : 0;
		    ܐܢ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ܐܢ (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ܟܡܐ (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			ܐܢ (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    ܐܚܪܢܐ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ܟܡܐ (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    ܐܢ (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			ܐܢ (strstr((حرف *)pf, "^^") != عدم)
			{
			    ܐܢ (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    اكسر;
		}
	    }
	}
	ܐܚܪܢܐ ܐܢ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ܐܢ (z0 == 0)
	{
	    ܐܢ (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    ساكن فراغ
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    صحيح		word[MAXWLEN];
    صحيح		wres[MAXWLEN];
    صحيح		l;
    char_u	*s;
    صحيح		*ws;
    char_u	*t;
    صحيح		*pf;
    صحيح		i, j, z;
    صحيح		reslen;
    صحيح		n, k = 0;
    صحيح		z0;
    صحيح		k0;
    صحيح		n0;
    صحيح		c;
    صحيح		pri;
    صحيح		p0 = -333;
    صحيح		c0;
    صحيح		did_white = FALSE;
    صحيح		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    لكل (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	ܐܢ (slang->sl_rem_accents)
	{
	    ܐܢ (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		ܐܢ (did_white)
		    استمر;
		c = ' ';
		did_white = TRUE;
	    }
	    ܐܚܪܢܐ
	    {
		did_white = FALSE;
		ܐܢ (!spell_iswordp_nmw(t, curwin))
		    استمر;
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
    ܟܡܐ ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	ܐܢ (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    لكل (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		ܐܢ (c != ws[0])
		    استمر;
		k = smp[n].sm_leadlen;
		ܐܢ (k > 1)
		{
		    ܐܢ (word[i + 1] != ws[1])
			استمر;
		    ܐܢ (k > 2)
		    {
			لكل (j = 2; j < k; ++j)
			    ܐܢ (word[i + j] != ws[j])
				اكسر;
			ܐܢ (j < k)
			    استمر;
		    }
		}

		ܐܢ ((pf = smp[n].sm_oneof_w) != عدم)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    ܟܡܐ (*pf != NUL && *pf != word[i + k])
			++pf;
		    ܐܢ (*pf == NUL)
			استمر;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		ܟܡܐ (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		ܐܢ (*s == '<')
		    s++;
		ܐܢ (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		ܐܢ (*s == '^' && *(s + 1) == '^')
		    s++;

		ܐܢ (*s == NUL
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

		    ܐܢ (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			لكل ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    ܐܢ (c0 != ws[0])
				استمر;
			    k0 = smp[n0].sm_leadlen;
			    ܐܢ (k0 > 1)
			    {
				ܐܢ (word[i + k] != ws[1])
				    استمر;
				ܐܢ (k0 > 2)
				{
				    pf = word + i + k + 1;
				    لكل (j = 2; j < k0; ++j)
					ܐܢ (*pf++ != ws[j])
					    اكسر;
				    ܐܢ (j < k0)
					استمر;
				}
			    }
			    k0 += k - 1;

			    ܐܢ ((pf = smp[n0].sm_oneof_w) != عدم)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				ܟܡܐ (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				ܐܢ (*pf == NUL)
				    استمر;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    ܟܡܐ (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    ܐܢ (*s == '<')
				s++;
			    ܐܢ (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    ܐܢ (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				ܐܢ (k0 == k)
				    // this is just a piece of the string
				    استمر;

				ܐܢ (p0 < pri)
				    // priority too low
				    استمر;
				// rule fits; stop search
				اكسر;
			    }
			}

			ܐܢ (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    استمر;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != عدم) ? 1 : 0;
		    ܐܢ (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			ܐܢ (reslen > 0 && ws != عدم && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			ܐܢ (ws != عدم)
			    ܟܡܐ (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			ܐܢ (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    حجم(صحيح) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    ܐܚܪܢܐ
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			ܐܢ (ws != عدم)
			    ܟܡܐ (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				ܐܢ (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			ܐܢ (ws == عدم)
			    c = NUL;
			ܐܚܪܢܐ
			    c = *ws;
			ܐܢ (strstr((حرف *)s, "^^") != عدم)
			{
			    ܐܢ (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       حجم(صحيح) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    اكسر;
		}
	    }
	}
	ܐܚܪܢܐ ܐܢ (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	ܐܢ (z0 == 0)
	{
	    ܐܢ (k && !p0 && reslen < MAXWLEN && c != NUL
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
    لكل (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	ܐܢ (l + MB_MAXBYTES > MAXWLEN)
	    اكسر;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    فراغ
ex_spellinfo(exarg_T *eap UNUSED)
{
    صحيح		lpi;
    langp_T	*lp;
    char_u	*p;

    ܐܢ (no_spell_checking(curwin))
	ܦܢܐ;

    msg_start();
    لكل (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	msg_puts("file: ");
	msg_puts((حرف *)lp->lp_slang->sl_fname);
	msg_putchar('\n');
	p = lp->lp_slang->sl_info;
	ܐܢ (p != عدم)
	{
	    msg_puts((حرف *)p);
	    msg_putchar('\n');
	}
    }
    msg_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_COUNT	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    فراغ
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    ܐܢ (no_spell_checking(curwin))
	ܦܢܐ;
    (فراغ)get_option_value((char_u*)"spl", &dummy, &spl, عدم, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_value_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_value_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    ܐܢ (!BUFEMPTY())
	ܦܢܐ;

    spell_dump_compl(عدم, 0, عدم, eap->forceit ? DUMPFLAG_COUNT : 0);

    // Delete the empty line that we started with.
    ܐܢ (curbuf->b_ml.ml_line_count > 1)
	ml_delete(curbuf->b_ml.ml_line_count);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    فراغ
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    صحيح		ic,	    // ignore case
    صحيح		*dir,	    // direction for adding matches
    صحيح		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    صحيح		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    صحيح		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    صحيح		round;
    صحيح		depth;
    صحيح		n;
    صحيح		flags;
    char_u	*region_names = عدم;	    // region names being used
    صحيح		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    صحيح		lpi;
    صحيح		dumpflags = dumpflags_arg;
    صحيح		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    ܐܢ (pat != عدم)
    {
	ܐܢ (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	ܐܚܪܢܐ
	{
	    n = captype(pat, عدم);
	    ܐܢ (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    ܐܚܪܢܐ ܐܢ (n == WF_ALLCAP && (صحيح)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    لكل (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	ܐܢ (p[0] != 0)
	{
	    ܐܢ (region_names == عدم)	    // first language with regions
		region_names = p;
	    ܐܚܪܢܐ ܐܢ (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		اكسر;
	    }
	}
    }

    ܐܢ (do_region && region_names != عدم && pat == عدم)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    ܐܚܪܢܐ
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    لكل (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ܐܢ (slang->sl_fbyts == عدم)	    // reloading failed
	    استمر;

	ܐܢ (pat == عدم)
	{
	    vim_snprintf((حرف *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	ܐܢ (pat != عدم && slang->sl_pbyts == عدم)
	    patlen = (صحيح)STRLEN(pat);
	ܐܚܪܢܐ
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	لكل (round = 1; round <= 2; ++round)
	{
	    ܐܢ (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    ܐܚܪܢܐ
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    ܐܢ (byts == عدم)
		استمر;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    ܟܡܐ (depth >= 0 && !got_int
				  && (pat == عدم || !ins_compl_interrupted()))
	    {
		ܐܢ (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    line_breakcheck();
		    ins_compl_check_keys(50, FALSE);
		}
		ܐܚܪܢܐ
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    ܐܢ (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (صحيح)idxs[n];
			ܐܢ ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    ܐܢ (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    ܐܢ (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				ܐܢ (pat == عدم)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    ܐܢ (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    ܐܚܪܢܐ
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
			ܐܢ (depth <= patlen
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
    ساكن فراغ
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    صحيح		*dir,
    صحيح		dumpflags,
    صحيح		wordflags,
    linenr_T	lnum)
{
    صحيح		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    صحيح		i;
    صحيح		flags = wordflags;

    ܐܢ (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    ܐܢ (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    ܐܢ ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    ܐܚܪܢܐ
    {
	p = word;
	ܐܢ ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, عدم) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    ܐܢ (pat == عدم)
    {
	// Add flags and regions after a slash.
	ܐܢ ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    ܐܢ (keepcap)
		STRCAT(badword, "=");
	    ܐܢ (flags & WF_BANNED)
		STRCAT(badword, "!");
	    ܐܚܪܢܐ ܐܢ (flags & WF_RARE)
		STRCAT(badword, "?");
	    ܐܢ (flags & WF_REGION)
		لكل (i = 0; i < 7; ++i)
		    ܐܢ (flags & (0x10000 << i))
			sprintf((حرف *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	ܐܢ (dumpflags & DUMPFLAG_COUNT)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    ܐܢ (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_count);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    ܐܚܪܢܐ ܐܢ (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (صحيح)STRLEN(p),
					  p_ic, عدم, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    ساكن linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    صحيح		*dir,
    صحيح		dumpflags,
    صحيح		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    صحيح		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    صحيح		has_word_up = FALSE;
    صحيح		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    صحيح		depth;
    صحيح		n;
    صحيح		len;
    صحيح		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    ܐܢ (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    ܐܢ (byts != عدم)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	ܟܡܐ (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    ܐܢ (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		line_breakcheck();
	    }
	    ܐܚܪܢܐ
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		ܐܢ (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    لكل (i = 1; i < len; ++i)
			ܐܢ (byts[n + i] != 0)
			    اكسر;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    ܐܢ (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			ܐܢ (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    ܐܢ (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			ܐܢ (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    ܐܢ (lnum != 0)
				++lnum;
			}
		    }
		}
		ܐܚܪܢܐ ܐܢ (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    ܦܢܐ lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    ܟܡܐ (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    ܦܢܐ p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    صحيح
spell_word_start(صحيح startcol)
{
    char_u	*line;
    char_u	*p;
    صحيح		col = 0;

    ܐܢ (no_spell_checking(curwin))
	ܦܢܐ startcol;

    // Find a word character before "startcol".
    line = ml_get_curline();
    لكل (p = line + startcol; p > line; )
    {
	MB_PTR_BACK(line, p);
	ܐܢ (spell_iswordp_nmw(p, curwin))
	    اكسر;
    }

    // Go back to start of the word.
    ܟܡܐ (p > line)
    {
	col = (صحيح)(p - line);
	MB_PTR_BACK(line, p);
	ܐܢ (!spell_iswordp(p, curwin))
	    اكسر;
	col = 0;
    }

    ܦܢܐ col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
ساكن صحيح spell_expand_need_cap;

    فراغ
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
    صحيح
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    ܦܢܐ ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    صحيح
valid_spelllang(char_u *val)
{
    ܦܢܐ valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    صحيح
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    صحيح		l;

    spf = val;
    ܟܡܐ (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	ܐܢ (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    ܦܢܐ FALSE;
	لكل (s = spf_name; *s != NUL; ++s)
	    ܐܢ (!vim_is_fname_char(*s))
		ܦܢܐ FALSE;
    }
    ܦܢܐ TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    حرف *
did_set_spell_option(فراغ)
{
    حرف    *errmsg = عدم;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ܐܢ (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    اكسر;
	}
    ܦܢܐ errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    حرف *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    ܐܢ (synblock->b_p_spc == عدم || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = عدم;
    ܐܚܪܢܐ
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	ܐܢ (re != عدم)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    ܐܢ (synblock->b_cap_prog == عدم)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		ܦܢܐ e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    ܦܢܐ عدم;
}

#endif  // FEAT_SPELL
