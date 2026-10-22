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
#شامل "vim.h"

#اگر defined(FEAT_SPELL)

#شامل <time.h>

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
تعریفنوع ساختار matchinf_S
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
    صحیح		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    صحیح		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    صحیح		mi_prefcnt;		// number of entries at mi_prefarridx
    صحیح		mi_prefixlen;		// byte length of prefix
    صحیح		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    صحیح		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    صحیح		mi_complen;		// nr of compound words used
    صحیح		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    صحیح		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    صحیح		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    صحیح		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


ایستا صحیح spell_mb_isword_class(صحیح cl, win_T *wp);

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

ایستا تهی find_word(matchinf_T *mip, صحیح mode);
ایستا تهی find_prefix(matchinf_T *mip, صحیح mode);
ایستا صحیح fold_more(matchinf_T *mip);
ایستا تهی spell_load_cb(char_u *fname, تهی *cookie);
ایستا صحیح شمار_syllables(slang_T *slang, char_u *word);
ایستا تهی clear_midword(win_T *buf);
ایستا تهی use_midword(slang_T *lp, win_T *buf);
ایستا صحیح find_region(char_u *rp, char_u *region);
ایستا تهی spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
ایستا تهی spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
ایستا تهی spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
ایستا تهی dump_word(slang_T *slang, char_u *word, char_u *pat, صحیح *dir, صحیح round, صحیح flags, linenr_T lnum);
ایستا linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, صحیح *dir, صحیح round, صحیح flags, linenr_T startlnum);
ایستا char_u *advance_camelcase_word(char_u *p, win_T *wp, صحیح *is_camel_case);

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
    صحیح
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    صحیح		*capcol,	// column to check for Capital
    صحیح		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    صحیح		nrlen = 0;	// found a number first
    صحیح		c;
    صحیح		wrongcaplen = 0;
    صحیح		lpi;
    صحیح		شمار_word = docount;
    صحیح		use_camel_case = *wp->w_s->b_p_spo != NUL;
    صحیح		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    اگر (*ptr <= ' ')
	بازگشت 1;

    // Return here when loading language files failed.
    اگر (wp->w_s->b_langp.ga_len == 0)
	بازگشت 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    اگر (*ptr >= '0' && *ptr <= '9')
    {
	اگر (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	وگرنه اگر (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	وگرنه
	    mi.mi_end = skipdigits(ptr);
	nrlen = (صحیح)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    اگر (spell_iswordp(mi.mi_fend, wp))
    {
	اگر (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	وگرنه
	{
	    انجامبده
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } وقتی (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	اگر (capcol != پوچ && *capcol == 0 && wp->w_s->b_cap_prog != پوچ)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    اگر (!SPELL_ISUPPER(c))
		wrongcaplen = (صحیح)(mi.mi_fend - ptr);
	}
    }
    اگر (capcol != پوچ)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = پوچ;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    اگر (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (تهی)spell_casefold(wp, ptr, (صحیح)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (صحیح)STRLEN(mi.mi_fword);

    اگر (is_camel_case && mi.mi_fwordlen > 0)
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
    برای (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	اگر (mi.mi_lp->lp_slang->sl_fidxs == پوچ)
	    ادامه;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	اگر (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	اگر (شمار_word && mi.mi_result == SP_OK)
	{
	    شمار_common_word(mi.mi_lp->lp_slang, ptr,
						   (صحیح)(mi.mi_end - ptr), 1);
	    شمار_word = FALSE;
	}
    }

    اگر (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	اگر (nrlen > 0)
	{
	    اگر (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		بازگشت nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	وگرنه اگر (!spell_iswordp_nmw(ptr, wp))
	{
	    اگر (capcol != پوچ && wp->w_s->b_cap_prog != پوچ)
	    {
		regmatch_T	regmatch;
		صحیح		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		اگر (r)
		    *capcol = (صحیح)(regmatch.endp[0] - ptr);
	    }

	    اگر (has_mbyte)
		بازگشت (*mb_ptr2len)(ptr);
	    بازگشت 1;
	}
	وگرنه اگر (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	وگرنه اگر (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    صحیح		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    اگر (mi.mi_lp->lp_slang->sl_fidxs != پوچ)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		برای (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    اگر (p >= mi.mi_end)
			بشکن;
		    mi.mi_compoff = (صحیح)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    اگر (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			بشکن;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	اگر (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	وگرنه اگر (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	وگرنه
	    *attrp = HLF_SPL;
    }

    اگر (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	بازگشت wrongcaplen;
    }

    بازگشت (صحیح)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    ایستا صحیح
get_char_type(صحیح c)
{
    اگر (VIM_ISDIGIT(c))
	بازگشت CHAR_DIGIT;
    اگر (SPELL_ISUPPER(c))
	بازگشت CHAR_UPPER;
    بازگشت CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    ایستا char_u *
advance_camelcase_word(char_u *str, win_T *wp, صحیح *is_camel_case)
{
    صحیح last_type, last_last_type, this_type;
    صحیح c;
    char_u *end = str;

    *is_camel_case = FALSE;

    اگر (*str == NUL)
	بازگشت str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    وقتی (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	اگر (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    بشکن;
	}
	وگرنه اگر ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    بشکن;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    بازگشت end;
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
    ایستا تهی
find_word(matchinf_T *mip, صحیح mode)
{
    idx_T	arridx = 0;
    صحیح		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    صحیح		endidxcnt = 0;
    صحیح		len;
    صحیح		wlen = 0;
    صحیح		flen;
    صحیح		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    صحیح		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    صحیح		word_ends;
    صحیح		prefix_found;
    صحیح		nobreak_result;

    اگر (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	اگر (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    وگرنه
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	اگر (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	وگرنه اگر (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    اگر (byts == پوچ)
	بازگشت;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    برای (;;)
    {
	اگر (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	اگر (byts[arridx] == 0)
	{
	    اگر (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		بازگشت;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    وقتی (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    اگر (len == 0)
		بشکن;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	اگر (ptr[wlen] == NUL)
	    بشکن;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	اگر (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	وقتی (lo < hi)
	{
	    m = (lo + hi) / 2;
	    اگر (byts[m] > c)
		hi = m - 1;
	    وگرنه اگر (byts[m] < c)
		lo = m + 1;
	    وگرنه
	    {
		lo = hi = m;
		بشکن;
	    }
	}

	// Stop if there is no matching byte.
	اگر (hi < lo || byts[lo] != c)
	    بشکن;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	اگر (c == ' ')
	{
	    برای (;;)
	    {
		اگر (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		اگر (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    بشکن;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    وقتی (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	اگر ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    ادامه;	    // not at first byte of character
	اگر (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    اگر (slang->sl_compprog == پوچ && !slang->sl_nobreak)
		ادامه;	    // next char is a word character
	    word_ends = FALSE;
	}
	وگرنه
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	اگر (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    اگر (STRNCMP(ptr, p, wlen) != 0)
	    {
		برای (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (صحیح)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	برای (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    اگر (mode == FIND_FOLDWORD)
	    {
		اگر (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		اگر (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    ادامه;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    وگرنه اگر (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		اگر (c == 0)
		    ادامه;

		// Use the WF_RARE flag for a rare prefix.
		اگر (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    اگر (slang->sl_nobreak)
	    {
		اگر ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    بشکن;
		}
	    }

	    وگرنه اگر ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		اگر (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    ادامه;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		اگر (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			ادامه;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		اگر (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    ادامه;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		اگر (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    ادامه;
		اگر (!word_ends && (flags & WF_NOCOMPAFT))
		    ادامه;

		// Quickly check if compounding is possible with this flag.
		اگر (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    ادامه;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		اگر (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    ادامه;

		اگر (mode == FIND_COMPOUND)
		{
		    صحیح	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    اگر (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			برای (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    وگرنه
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    اگر (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			ادامه;

		    اگر (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			اگر (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    ادامه;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		اگر (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    اگر (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			اگر (ptr == mip->mi_word)
			    (تهی)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			وگرنه
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    اگر (!can_compound(slang, fword, mip->mi_compflags))
			ادامه;
		}
		وگرنه اگر (slang->sl_comprules != پوچ
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    ادامه;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    وگرنه اگر (flags & WF_NEEDCOMP)
		ادامه;

	    nobreak_result = SP_OK;

	    اگر (!word_ends)
	    {
		صحیح	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		صحیح	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		اگر (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		اگر (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    اگر (STRNCMP(ptr, p, wlen) != 0)
		    {
			برای (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (صحیح)(p - mip->mi_fword);
		    }
		}
#اگر 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		اگر (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		برای (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    اگر (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			اگر (mip->mi_lp->lp_slang->sl_fidxs == پوچ
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    ادامه;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    اگر (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#اگر 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			اگر (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    اگر (!slang->sl_nobreak)
			بشکن;
		}
		--mip->mi_complen;
		اگر (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		اگر (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		وگرنه
		{
		    اگر (mip->mi_result == SP_OK)
			بشکن;
		    ادامه;
		}
	    }

	    اگر (flags & WF_BANNED)
		res = SP_BANNED;
	    وگرنه اگر (flags & WF_REGION)
	    {
		// Check region.
		اگر ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		وگرنه
		    res = SP_LOCAL;
	    }
	    وگرنه اگر (flags & WF_RARE)
		res = SP_RARE;
	    وگرنه
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    اگر (nobreak_result == SP_BAD)
	    {
		اگر (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		وگرنه اگر (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    وگرنه اگر (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    وگرنه اگر (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    اگر (mip->mi_result == SP_OK)
		بشکن;
	}

	اگر (mip->mi_result == SP_OK)
	    بشکن;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    صحیح
match_checkcompoundpattern(
    char_u	*ptr,
    صحیح		wlen,
    garray_T	*gap)  // &sl_comppat
{
    صحیح		i;
    char_u	*p;
    صحیح		len;

    برای (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	اگر (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (صحیح)STRLEN(p);
	    اگر (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    صحیح
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    صحیح		i;
    char_u	*p;

    اگر (slang->sl_compprog == پوچ)
	بازگشت FALSE;
    اگر (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	برای (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    وگرنه
	p = flags;
    اگر (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	بازگشت FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    اگر (slang->sl_compsylmax < MAXWLEN
		       && شمار_syllables(slang, word) > slang->sl_compsylmax)
	بازگشت (صحیح)STRLEN(flags) < slang->sl_compmax;
    بازگشت TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    صحیح
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    صحیح		i;
    صحیح		c;

    // loop over all the COMPOUNDRULE entries
    برای (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	برای (i = 0; ; ++i)
	{
	    c = compflags[i];
	    اگر (c == NUL)
		// found a rule that matches for the flags we have so far
		بازگشت TRUE;
	    اگر (*p == '/' || *p == NUL)
		بشکن;  // end of rule, it's too short
	    اگر (*p == '[')
	    {
		صحیح match = FALSE;

		// compare against all the flags in []
		++p;
		وقتی (*p != ']' && *p != NUL)
		    اگر (*p++ == c)
			match = TRUE;
		اگر (!match)
		    بشکن;  // none matches
	    }
	    وگرنه اگر (*p != c)
		بشکن;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	اگر (p == پوچ)
	    بشکن;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    بازگشت FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    صحیح
valid_word_prefix(
    صحیح		totprefcnt,	// nr of prefix IDs
    صحیح		arridx,		// idx in sl_pidxs[]
    صحیح		flags,
    char_u	*word,
    slang_T	*slang,
    صحیح		cond_req)	// only use prefixes with a condition
{
    صحیح		prefcnt;
    صحیح		pidx;
    regprog_T	**rp;
    صحیح		prefid;

    prefid = (unsigned)flags >> 24;
    برای (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	اگر (prefid != (pidx & 0xff))
	    ادامه;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	اگر ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    ادامه;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	اگر (*rp != پوچ)
	{
	    اگر (!vim_regexec_prog(rp, FALSE, word, 0))
		ادامه;
	}
	وگرنه اگر (cond_req)
	    ادامه;

	// It's a match!  Return the WF_ flags.
	بازگشت pidx;
    }
    بازگشت 0;
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
    ایستا تهی
find_prefix(matchinf_T *mip, صحیح mode)
{
    idx_T	arridx = 0;
    صحیح		len;
    صحیح		wlen = 0;
    صحیح		flen;
    صحیح		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    اگر (byts == پوچ)
	بازگشت;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    اگر (mode == FIND_COMPOUND)
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
    برای (;;)
    {
	اگر (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	اگر (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    وقتی (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    اگر (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    اگر (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    وگرنه
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    اگر (len == 0)
		بشکن;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	اگر (ptr[wlen] == NUL)
	    بشکن;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	وقتی (lo < hi)
	{
	    m = (lo + hi) / 2;
	    اگر (byts[m] > c)
		hi = m - 1;
	    وگرنه اگر (byts[m] < c)
		lo = m + 1;
	    وگرنه
	    {
		lo = hi = m;
		بشکن;
	    }
	}

	// Stop if there is no matching byte.
	اگر (hi < lo || byts[lo] != c)
	    بشکن;

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
    ایستا صحیح
fold_more(matchinf_T *mip)
{
    صحیح		flen;
    char_u	*p;

    p = mip->mi_fend;
    انجامبده
	MB_PTR_ADV(mip->mi_fend);
    وقتی (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    اگر (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (تهی)spell_casefold(mip->mi_win, p, (صحیح)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (صحیح)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    بازگشت flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    صحیح
spell_valid_case(
    صحیح	    wordflags,	    // flags for the checked word.
    صحیح	    treeflags)	    // flags for the word in the spell tree
{
    بازگشت ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    صحیح
spell_check_window(win_T *wp)
{
    بازگشت wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(نویسه **)(wp->w_s->b_langp.ga_data) != پوچ;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    ایستا صحیح
no_spell_checking(win_T *wp)
{
    اگر (spell_check_window(wp))
	بازگشت FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    بازگشت TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    صحیح
spell_move_to(
    win_T	*wp,
    صحیح		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    صحیح		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    صحیح		found_len = 0;
    char_u	*سطر;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    صحیح		len;
#ifdef FEAT_SYN_HL
    صحیح		has_syntax = syntax_present(wp);
#endif
    صحیح		col;
    صحیح		can_spell;
    char_u	*buf = پوچ;
    صحیح		buflen = 0;
    صحیح		skip = 0;
    صحیح		capcol = -1;
    صحیح		found_one = FALSE;
    صحیح		wrapped = FALSE;

    اگر (no_spell_checking(wp))
	بازگشت 0;

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

    وقتی (!got_int)
    {
	صحیح empty_سطر;

	سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	اگر (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    اگر (buf == پوچ)
		بشکن;
	}

	// In first line check first word for Capital.
	اگر (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	اگر (capcol == 0)
	    capcol = getwhitecols(سطر);
	وگرنه اگر (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(سطر);
	    اگر (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_سطر = *skipwhite(سطر) == NUL;
	STRCPY(buf, سطر);
	اگر (lnum < wp->w_buffer->b_ml.ml_سطر_شمار)
	    spell_cat_سطر(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	وقتی (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    اگر (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		بشکن;

	    // start of word
	    attr = HLF_شمار;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    اگر (attr != HLF_شمار)
	    {
		// We found a bad word.  Check the attribute.
		اگر (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    اگر (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			اگر (has_syntax)
			{
			    col = (صحیح)(p - buf);
			    (تهی)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    اگر (!can_spell)
				attr = HLF_شمار;
			}
			وگرنه
#endif
			    can_spell = TRUE;

			اگر (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (صحیح)(p - buf);
			    found_pos.coladd = 0;
			    اگر (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				اگر (attrp != پوچ)
				    *attrp = attr;
				بازگشت len;
			    }
			    وگرنه اگر (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    وگرنه
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	اگر (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    بازگشت found_len;
	}

	اگر (curline)
	    بشکن;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	اگر (lnum == wp->w_cursor.lnum && wrapped)
	    بشکن;

	// Advance to next line.
	اگر (dir == BACKWARD)
	{
	    اگر (lnum > 1)
		--lnum;
	    وگرنه اگر (!p_ws)
		بشکن;	    // at first line and 'nowrapscan'
	    وگرنه
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_سطر_شمار;
		wrapped = TRUE;
		اگر (!shortmess(SHM_جستجو))
		    give_warning((char_u *)_(بالاترین_bot_پیام), TRUE);
	    }
	    capcol = -1;
	}
	وگرنه
	{
	    اگر (lnum < wp->w_buffer->b_ml.ml_سطر_شمار)
		++lnum;
	    وگرنه اگر (!p_ws)
		بشکن;	    // at first line and 'nowrapscan'
	    وگرنه
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		اگر (!shortmess(SHM_جستجو))
		    give_warning((char_u *)_(bot_بالاترین_پیام), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    اگر (lnum == wp->w_cursor.lnum && !found_one)
		بشکن;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    اگر (attr == HLF_شمار)
		skip = (صحیح)(p - endp);
	    وگرنه
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    اگر (empty_سطر)
		capcol = 0;
	}

	سطر_breakcheck();
    }

    vim_free(buf);
    بازگشت 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    تهی
spell_cat_سطر(char_u *buf, char_u *سطر, صحیح maxlen)
{
    char_u	*p;
    صحیح		n;

    p = skipwhite(سطر);
    وقتی (vim_strchr((char_u *)"*#/\"\t", *p) != پوچ)
	p = skipwhite(p + 1);

    اگر (*p == NUL)
	بازگشت;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (صحیح)(p - سطر) + 1;
    اگر (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
تعریفنوع ساختار spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    صحیح	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    ایستا تهی
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    صحیح		r;
    spelload_T	sl;
    صحیح		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = پوچ;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    برای (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((نویسه *)fname_enc, اندازه(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#وگرنه
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	اگر (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((نویسه *)fname_enc, اندازه(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#وگرنه
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    اگر (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		ادامه;
	    بشکن;
	}
	بشکن;
    }

    اگر (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#وگرنه
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    وگرنه اگر (sl.sl_slang != پوچ)
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
spell_enc(تهی)
{

    اگر (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	بازگشت p_enc;
    بازگشت (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    ایستا تهی
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((نویسه *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    اگر (lp != پوچ)
    {
	اگر (lang != پوچ)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, اندازه(fromto_T), 10);
	ga_init2(&lp->sl_repsal, اندازه(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    بازگشت lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    تهی
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
    تهی
free_sal_اقلام(garray_T *gap)
{
    salitem_T	*smp;

    وقتی (gap->ga_len > 0)
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
    تهی
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    صحیح		i;
    صحیح		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    برای (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	وقتی (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    اگر (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	اگر (gap->ga_data != پوچ)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    برای (i = 0; i < gap->ga_len; ++i)
		vim_free(((صحیح **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    وگرنه
	free_sal_اقلام(gap);

    برای (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = پوچ;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_اقلام);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_کلید_OFF);
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
    تهی
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = پوچ;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    ایستا تهی
spell_load_cb(char_u *fname, تهی *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, پوچ, FALSE);
    اگر (slang == پوچ)
	بازگشت;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    اگر (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    وگرنه اگر (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    تهی
شمار_common_word(
    slang_T	*lp,
    char_u	*word,
    صحیح		len,	    // word length, -1 for up to NUL
    صحیح		شمار)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    اگر (len == -1)
	p = word;
    وگرنه اگر (len >= MAXWLEN)
	بازگشت;
    وگرنه
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    اگر (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	اگر (wc == پوچ)
	    بازگشت;
	STRCPY(wc->wc_word, p);
	wc->wc_شمار = شمار;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    وگرنه
    {
	wc = HI2WC(hi);
	اگر ((wc->wc_شمار += شمار) < (unsigned)شمار)	// check for overflow
	    wc->wc_شمار = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    صحیح
byte_in_str(char_u *str, صحیح n)
{
    char_u	*p;

    برای (p = str; *p != NUL; ++p)
	اگر (*p == n)
	    بازگشت TRUE;
    بازگشت FALSE;
}

#define SY_MAXLEN   30
تعریفنوع ساختار syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    صحیح		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    صحیح
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    صحیح		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_اقلام, اندازه(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    وقتی (p != پوچ)
    {
	*p++ = NUL;
	اگر (*p == NUL)	    // trailing slash
	    بشکن;
	s = p;
	p = vim_strchr(p, '/');
	اگر (p == پوچ)
	    l = (صحیح)STRLEN(s);
	وگرنه
	    l = (صحیح)(p - s);
	اگر (l >= SY_MAXLEN)
	    بازگشت SP_FORMERROR;
	اگر (ga_grow(&slang->sl_syl_اقلام, 1) == FAIL)
	    بازگشت SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_اقلام.ga_data)
					       + slang->sl_syl_اقلام.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    بازگشت OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    ایستا صحیح
شمار_syllables(slang_T *slang, char_u *word)
{
    صحیح		cnt = 0;
    صحیح		skip = FALSE;
    char_u	*p;
    صحیح		len;
    صحیح		i;
    syl_item_T	*syl;
    صحیح		c;

    اگر (slang->sl_syllable == پوچ)
	بازگشت 0;

    برای (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	اگر (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    ادامه;
	}

	// Find longest match of syllable items.
	len = 0;
	برای (i = 0; i < slang->sl_syl_اقلام.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_اقلام.ga_data) + i;
	    اگر (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	اگر (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	وگرنه
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    اگر (vim_strchr(slang->sl_syllable, c) == پوچ)
		skip = FALSE;	    // No, search for next syllable
	    وگرنه اگر (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    بازگشت cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    نویسه *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    صحیح		filename;
    صحیح		region_mask;
    slang_T	*slang;
    صحیح		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    صحیح		len;
    char_u	*p;
    صحیح		round;
    char_u	*spf;
    char_u	*use_region = پوچ;
    صحیح		dont_use_region = FALSE;
    صحیح		nobreak = FALSE;
    صحیح		i, j;
    langp_T	*lp, *lp2;
    ایستا صحیح	recursive = FALSE;
    نویسه	*ret_پیام = پوچ;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    اگر (recursive)
	بازگشت پوچ;
    recursive = TRUE;

    ga_init2(&ga, اندازه(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    اگر (spl_copy == پوچ)
	برو theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    برای (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = پوچ;

	اگر (!valid_spelllang(lang))
	    ادامه;

	اگر (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    ادامه;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	اگر (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    اگر (p != پوچ && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    وگرنه
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		اگر (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    بشکن;
	}
	وگرنه
	{
	    filename = FALSE;
	    اگر (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    وگرنه
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		اگر (STRICMP(lang, slang->sl_name) == 0)
		    بشکن;
	}

	اگر (region != پوچ)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    اگر (use_region != پوچ && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	اگر (slang == پوچ)
	{
	    اگر (filename)
		(تهی)spell_load_file(lang, lang, پوچ, FALSE);
	    وگرنه
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		اگر (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_پیام = N_(e_spellfilemising_autocommand_deleted_buffer);
		    برو theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    اگر (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		اگر (!filename && region != پوچ)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    اگر (c == REGION_ALL)
		    {
			اگر (slang->sl_add)
			{
			    اگر (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			وگرنه
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    وگرنه
			region_mask = 1 << c;
		}

		اگر (region_mask != 0)
		{
		    اگر (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_پیام = e_out_of_memory;
			برو theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    اگر (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    برای (round = 0; round == 0 || *spf != NUL; ++round)
    {
	اگر (round == 0)
	{
	    // Internal wordlist, if there is one.
	    اگر (int_wordlist == پوچ)
		ادامه;
	    int_wordlist_spl(spf_name);
	}
	وگرنه
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    برای (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		اگر (p != پوچ && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    بشکن;
	    }
	    اگر (c < ga.ga_len)
		ادامه;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    اگر (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		بشکن;
	اگر (slang == پوچ)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    اگر (round == 0)
		STRCPY(lang, "internal wordlist");
	    وگرنه
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		اگر (p != پوچ)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, پوچ, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    اگر (slang != پوچ && nobreak)
		slang->sl_nobreak = TRUE;
	}
	اگر (slang != پوچ && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    اگر (use_region != پوچ && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		اگر (c != REGION_ALL)
		    region_mask = 1 << c;
		وگرنه اگر (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    اگر (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = پوچ;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = پوچ;
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
    برای (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	اگر (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	وگرنه
	    // find first similar language that does sound folding
	    برای (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		اگر (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    بشکن;
		}
	    }

	// REP items
	اگر (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	وگرنه
	    // find first similar language that has REP items
	    برای (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		اگر (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    بشکن;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    بازگشت ret_پیام;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    ایستا تهی
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    ایستا تهی
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    اگر (lp->sl_midword == پوچ)	    // there aren't any
	بازگشت;

    برای (p = lp->sl_midword; *p != NUL; )
	اگر (has_mbyte)
	{
	    صحیح	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    اگر (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    وگرنه اگر (wp->w_s->b_spell_ismw_mb == پوچ)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    وگرنه
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (صحیح)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		اگر (bp != پوچ)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	وگرنه
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    ایستا صحیح
find_region(char_u *rp, char_u *region)
{
    صحیح		i;

    برای (i = 0; ; i += 2)
    {
	اگر (rp[i] == NUL)
	    بازگشت REGION_ALL;
	اگر (rp[i] == region[0] && rp[i + 1] == region[1])
	    بشکن;
    }
    بازگشت i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    صحیح
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    صحیح		c;
    صحیح		firstcap;
    صحیح		allcap;
    صحیح		past_second = FALSE;	// past second word char

    // find first letter
    برای (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	اگر (end == پوچ ? *p == NUL : p >= end)
	    بازگشت 0;	    // only non-word characters, illegal word
    اگر (has_mbyte)
	c = mb_ptr2char_adv(&p);
    وگرنه
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    برای ( ; end == پوچ ? *p != NUL : p < end; MB_PTR_ADV(p))
	اگر (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    اگر (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		اگر (past_second && allcap)
		    بازگشت WF_KEEPCAP;
		allcap = FALSE;
	    }
	    وگرنه اگر (!allcap)
		// UlU -> KEEPCAP
		بازگشت WF_KEEPCAP;
	    past_second = TRUE;
	}

    اگر (allcap)
	بازگشت WF_ALLCAP;
    اگر (firstcap)
	بازگشت WF_ONECAP;
    بازگشت 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    تهی
spell_delete_wordlist(تهی)
{
    char_u	fname[MAXPATHL];

    اگر (int_wordlist == پوچ)
	بازگشت;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    تهی
spell_free_all(تهی)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    وقتی (first_lang != پوچ)
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
    تهی
spell_reload(تهی)
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
	اگر (*wp->w_s->b_p_spl != NUL)
	{
		اگر (wp->w_p_spell)
		{
		    (تهی)parse_spelllang(wp);
		    بشکن;
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
open_spellbuf(تهی)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    اگر (buf == پوچ)
	بازگشت پوچ;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_کلید = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    بازگشت buf;
}

/*
 * Close the buffer used for spell info.
 */
    تهی
close_spellbuf(buf_T *buf)
{
    اگر (buf == پوچ)
	بازگشت;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    تهی
clear_spell_chartab(spelltab_T *sp)
{
    صحیح		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    برای (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    برای (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    برای (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    برای (i = 'a'; i <= 'z'; ++i)
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
    تهی
init_spell_chartab(تهی)
{
    صحیح	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    اگر (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	برای (i = 128; i <= 255; ++i)
	    اگر (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    وگرنه اگر (enc_utf8)
    {
	برای (i = 128; i < 256; ++i)
	{
	    صحیح f = utf_fold(i);
	    صحیح u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    وگرنه
    {
	// Rough guess: use locale-dependent library functions.
	برای (i = 128; i < 256; ++i)
	{
	    اگر (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    وگرنه اگر (MB_ISLOWER(i))
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
    صحیح
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    صحیح		l;
    صحیح		c;

    اگر (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	اگر (l == 1)
	{
	    // be quick for ASCII
	    اگر (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	وگرنه
	{
	    c = mb_ptr2char(p);
	    اگر (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != پوچ
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != پوچ))
		s = p + l;
	}

	c = mb_ptr2char(s);
	اگر (c > 255)
	    بازگشت spell_mb_isword_class(mb_get_class(s), wp);
	بازگشت spelltab.st_isw[c];
    }

    بازگشت spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    صحیح
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    صحیح		c;

    اگر (has_mbyte)
    {
	c = mb_ptr2char(p);
	اگر (c > 255)
	    بازگشت spell_mb_isword_class(mb_get_class(p), wp);
	بازگشت spelltab.st_isw[c];
    }
    بازگشت spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    ایستا صحیح
spell_mb_isword_class(صحیح cl, win_T *wp)
{
    اگر (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	بازگشت cl == 2 || cl == 0x2800;
    بازگشت cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    ایستا صحیح
spell_iswordp_w(صحیح *p, win_T *wp)
{
    صحیح		*s;

    اگر (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != پوچ
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != پوچ))
	s = p + 1;
    وگرنه
	s = p;

    اگر (*s > 255)
    {
	اگر (enc_utf8)
	    بازگشت spell_mb_isword_class(utf_class(*s), wp);
	اگر (enc_dbcs)
	    بازگشت spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	بازگشت 0;
    }
    بازگشت spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    صحیح
spell_casefold(
    win_T	*wp,
    char_u	*str,
    صحیح		len,
    char_u	*buf,
    صحیح		buflen)
{
    صحیح		i;

    اگر (len >= buflen)
    {
	buf[0] = NUL;
	بازگشت FAIL;		// result will not fit
    }

    اگر (has_mbyte)
    {
	صحیح	outi = 0;
	char_u	*p;
	صحیح	c;

	// Fold one character at a time.
	برای (p = str; p < str + len; )
	{
	    اگر (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		بازگشت FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    اگر (c == 0x03a3 || c == 0x03c2)
	    {
		اگر (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		وگرنه
		    c = 0x03c3;
	    }
	    وگرنه
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    وگرنه
    {
	// Be quick for non-multibyte encodings.
	برای (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    بازگشت OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    صحیح
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    اگر (wp->w_s->b_cap_prog == پوچ)
	بازگشت FALSE;

    صحیح		need_cap = FALSE;
    char_u	*سطر = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : پوچ;
    char_u	*سطر_copy = پوچ;
    colnr_T	endcol = 0;

    اگر (col == 0 || getwhitecols(سطر) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	اگر (lnum == 1)
	    need_cap = TRUE;
	وگرنه
	{
	    سطر = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    اگر (*skipwhite(سطر) == NUL)
		need_cap = TRUE;
	    وگرنه
	    {
		// Append a space in place of the line break.
		سطر_copy = concat_str(سطر, (char_u *)" ");
		اگر (سطر_copy == پوچ)
		    بازگشت FALSE;

		سطر = سطر_copy;
		endcol = (colnr_T)STRLEN(سطر);
	    }
	}
    }
    وگرنه
	endcol = col;

    اگر (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = سطر + endcol;
	برای (;;)
	{
	    MB_PTR_BACK(سطر, p);
	    اگر (p == سطر || spell_iswordp_nmw(p, wp))
		بشکن;
	    اگر (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == سطر + endcol)
	    {
		need_cap = TRUE;
		بشکن;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(سطر_copy);

    بازگشت need_cap;
}


/*
 * ":spellrepall"
 */
    تهی
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*سطر;
    char_u	*p;
    صحیح		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    اگر (repl_from == پوچ || repl_to == پوچ)
    {
	emsg(_(e_no_previous_spell_replacement));
	بازگشت;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    اگر (frompat == پوچ)
	بازگشت;
    frompatlen = vim_snprintf((نویسه *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    وقتی (!got_int)
    {
	اگر (do_جستجو(پوچ, '/', '/', frompat, frompatlen, 1L, جستجو_KEEP, پوچ) == 0
						   || u_save_cursor() == FAIL)
	    بشکن;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	سطر = ml_get_curline();
	اگر (addlen <= 0 || STRNCMP(سطر + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    اگر (p == پوچ)
		بشکن;
	    mch_memmove(p, سطر, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, سطر + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#اگر defined(FEAT_PROP_POPUP)
	    اگر (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (صحیح)addlen, APC_SUBSTITUTE);
#endif

	    اگر (curwin->w_cursor.lnum != prev_lnum)
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

    اگر (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    وگرنه
	do_sub_پیام(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    تهی
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    صحیح		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    صحیح		c;
    صحیح		l;

    p = word;
    اگر (has_mbyte)
	c = mb_cptr2char_adv(&p);
    وگرنه
	c = *p++;
    اگر (upper)
	c = SPELL_TOUPPER(c);
    وگرنه
	c = SPELL_TOFOLD(c);
    اگر (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    وگرنه
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
    تهی
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    صحیح		c;

    d = wcopy;
    برای (s = word; *s != NUL; )
    {
	اگر (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	وگرنه
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	اگر (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    اگر (d - wcopy >= MAXWLEN - 1)
		بشکن;
	    *d++ = c;
	}
	وگرنه
	    c = SPELL_TOUPPER(c);

	اگر (has_mbyte)
	{
	    اگر (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		بشکن;
	    d += mb_char2bytes(c, d);
	}
	وگرنه
	{
	    اگر (d - wcopy >= MAXWLEN - 1)
		بشکن;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    صحیح
nofold_len(char_u *fword, صحیح flen, char_u *word)
{
    char_u	*p;
    صحیح		i = 0;

    برای (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    برای (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    بازگشت (صحیح)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    تهی
make_case_word(char_u *fword, char_u *cword, صحیح flags)
{
    اگر (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    وگرنه اگر (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    وگرنه
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#اگر defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    صحیح		lpi;

    اگر (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	برای (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    اگر (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		بازگشت vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    بازگشت vim_strsave(word);
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
    تهی
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    صحیح		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    اگر (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    وگرنه
    {
	// SAL items used.  Requires the word to be case-folded.
	اگر (folded)
	    word = inword;
	وگرنه
	{
	    (تهی)spell_casefold(curwin,
				  inword, (صحیح)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	اگر (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	وگرنه
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    ایستا تهی
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    صحیح		ri = 0;
    صحیح		c;

    اگر (has_mbyte)
    {
	صحیح	prevc = 0;
	صحیح	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	برای (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    اگر (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    وگرنه اگر (c < 256)
		c = slang->sl_sal_first[c];
	    وگرنه
	    {
		ip = ((صحیح **)slang->sl_sal.ga_data)[c & 0xff];
		اگر (ip == پوچ)		// empty list, can't match
		    c = NUL;
		وگرنه
		    برای (;;)		// find "c" in the list
		    {
			اگر (*ip == 0)	// not found
			{
			    c = NUL;
			    بشکن;
			}
			اگر (*ip == c)	// match!
			{
			    c = ip[1];
			    بشکن;
			}
			ip += 2;
		    }
	    }

	    اگر (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		اگر (ri + MB_MAXBYTES > MAXWLEN)
		    بشکن;
		prevc = c;
	    }
	}
    }
    وگرنه
    {
	// The sl_sal_first[] table contains the translation.
	برای (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    اگر (VIM_ISWHITE(c))
		c = ' ';
	    وگرنه
		c = slang->sl_sal_first[c];
	    اگر (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    ایستا تهی
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    صحیح		i, j, z;
    صحیح		reslen;
    صحیح		n, k = 0;
    صحیح		z0;
    صحیح		k0;
    صحیح		n0;
    صحیح		c;
    صحیح		pri;
    صحیح		p0 = -333;
    صحیح		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    اگر (slang->sl_rem_accents)
    {
	t = word;
	وقتی (*s != NUL)
	{
	    اگر (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    وگرنه
	    {
		اگر (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    وگرنه
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    وقتی ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	اگر (n >= 0)
	{
	    // check all rules for the same letter
	    برای (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		اگر (k > 1)
		{
		    اگر (word[i + 1] != s[1])
			ادامه;
		    اگر (k > 2)
		    {
			برای (j = 2; j < k; ++j)
			    اگر (word[i + j] != s[j])
				بشکن;
			اگر (j < k)
			    ادامه;
		    }
		}

		اگر ((pf = smp[n].sm_oneof) != پوچ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    وقتی (*pf != NUL && *pf != word[i + k])
			++pf;
		    اگر (*pf == NUL)
			ادامه;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		وقتی (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		اگر (*s == '<')
		    s++;
		اگر (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		اگر (*s == '^' && *(s + 1) == '^')
		    s++;

		اگر (*s == NUL
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

		    اگر (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			برای ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    اگر (k0 > 1)
			    {
				اگر (word[i + k] != s[1])
				    ادامه;
				اگر (k0 > 2)
				{
				    pf = word + i + k + 1;
				    برای (j = 2; j < k0; ++j)
					اگر (*pf++ != s[j])
					    بشکن;
				    اگر (j < k0)
					ادامه;
				}
			    }
			    k0 += k - 1;

			    اگر ((pf = smp[n0].sm_oneof) != پوچ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				وقتی (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				اگر (*pf == NUL)
				    ادامه;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    وقتی (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    اگر (*s == '<')
				s++;
			    اگر (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    اگر (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				اگر (k0 == k)
				    // this is just a piece of the string
				    ادامه;

				اگر (p0 < pri)
				    // priority too low
				    ادامه;
				// rule fits; stop search
				بشکن;
			    }
			}

			اگر (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    ادامه;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    اگر (s == پوچ)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != پوچ) ? 1 : 0;
		    اگر (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			اگر (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			وقتی (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			اگر (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    وگرنه
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			وقتی (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    اگر (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			اگر (strstr((نویسه *)pf, "^^") != پوچ)
			{
			    اگر (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    بشکن;
		}
	    }
	}
	وگرنه اگر (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	اگر (z0 == 0)
	{
	    اگر (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    ایستا تهی
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    صحیح		word[MAXWLEN];
    صحیح		wres[MAXWLEN];
    صحیح		l;
    char_u	*s;
    صحیح		*ws;
    char_u	*t;
    صحیح		*pf;
    صحیح		i, j, z;
    صحیح		reslen;
    صحیح		n, k = 0;
    صحیح		z0;
    صحیح		k0;
    صحیح		n0;
    صحیح		c;
    صحیح		pri;
    صحیح		p0 = -333;
    صحیح		c0;
    صحیح		did_white = FALSE;
    صحیح		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    برای (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	اگر (slang->sl_rem_accents)
	{
	    اگر (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		اگر (did_white)
		    ادامه;
		c = ' ';
		did_white = TRUE;
	    }
	    وگرنه
	    {
		did_white = FALSE;
		اگر (!spell_iswordp_nmw(t, curwin))
		    ادامه;
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
    وقتی ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	اگر (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    برای (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		اگر (c != ws[0])
		    ادامه;
		k = smp[n].sm_leadlen;
		اگر (k > 1)
		{
		    اگر (word[i + 1] != ws[1])
			ادامه;
		    اگر (k > 2)
		    {
			برای (j = 2; j < k; ++j)
			    اگر (word[i + j] != ws[j])
				بشکن;
			اگر (j < k)
			    ادامه;
		    }
		}

		اگر ((pf = smp[n].sm_oneof_w) != پوچ)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    وقتی (*pf != NUL && *pf != word[i + k])
			++pf;
		    اگر (*pf == NUL)
			ادامه;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		وقتی (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		اگر (*s == '<')
		    s++;
		اگر (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		اگر (*s == '^' && *(s + 1) == '^')
		    s++;

		اگر (*s == NUL
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

		    اگر (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			برای ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    اگر (c0 != ws[0])
				ادامه;
			    k0 = smp[n0].sm_leadlen;
			    اگر (k0 > 1)
			    {
				اگر (word[i + k] != ws[1])
				    ادامه;
				اگر (k0 > 2)
				{
				    pf = word + i + k + 1;
				    برای (j = 2; j < k0; ++j)
					اگر (*pf++ != ws[j])
					    بشکن;
				    اگر (j < k0)
					ادامه;
				}
			    }
			    k0 += k - 1;

			    اگر ((pf = smp[n0].sm_oneof_w) != پوچ)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				وقتی (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				اگر (*pf == NUL)
				    ادامه;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    وقتی (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    اگر (*s == '<')
				s++;
			    اگر (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    اگر (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				اگر (k0 == k)
				    // this is just a piece of the string
				    ادامه;

				اگر (p0 < pri)
				    // priority too low
				    ادامه;
				// rule fits; stop search
				بشکن;
			    }
			}

			اگر (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    ادامه;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != پوچ) ? 1 : 0;
		    اگر (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			اگر (reslen > 0 && ws != پوچ && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			اگر (ws != پوچ)
			    وقتی (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			اگر (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    اندازه(صحیح) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    وگرنه
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			اگر (ws != پوچ)
			    وقتی (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				اگر (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			اگر (ws == پوچ)
			    c = NUL;
			وگرنه
			    c = *ws;
			اگر (strstr((نویسه *)s, "^^") != پوچ)
			{
			    اگر (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       اندازه(صحیح) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    بشکن;
		}
	    }
	}
	وگرنه اگر (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	اگر (z0 == 0)
	{
	    اگر (k && !p0 && reslen < MAXWLEN && c != NUL
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
    برای (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	اگر (l + MB_MAXBYTES > MAXWLEN)
	    بشکن;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    تهی
ex_spellinfo(exarg_T *eap UNUSED)
{
    صحیح		lpi;
    langp_T	*lp;
    char_u	*p;

    اگر (no_spell_checking(curwin))
	بازگشت;

    پیام_start();
    برای (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	پیام_puts("file: ");
	پیام_puts((نویسه *)lp->lp_slang->sl_fname);
	پیام_putchar('\n');
	p = lp->lp_slang->sl_info;
	اگر (p != پوچ)
	{
	    پیام_puts((نویسه *)p);
	    پیام_putchar('\n');
	}
    }
    پیام_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_شمار	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    تهی
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    اگر (no_spell_checking(curwin))
	بازگشت;
    (تهی)get_option_مقدار((char_u*)"spl", &dummy, &spl, پوچ, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_مقدار_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_مقدار_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    اگر (!BUFEMPTY())
	بازگشت;

    spell_dump_compl(پوچ, 0, پوچ, eap->forceit ? DUMPFLAG_شمار : 0);

    // Delete the empty line that we started with.
    اگر (curbuf->b_ml.ml_سطر_شمار > 1)
	ml_delete(curbuf->b_ml.ml_سطر_شمار);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    تهی
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    صحیح		ic,	    // ignore case
    صحیح		*dir,	    // direction for adding matches
    صحیح		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    صحیح		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    صحیح		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    صحیح		round;
    صحیح		depth;
    صحیح		n;
    صحیح		flags;
    char_u	*region_names = پوچ;	    // region names being used
    صحیح		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    صحیح		lpi;
    صحیح		dumpflags = dumpflags_arg;
    صحیح		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    اگر (pat != پوچ)
    {
	اگر (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	وگرنه
	{
	    n = captype(pat, پوچ);
	    اگر (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    وگرنه اگر (n == WF_ALLCAP && (صحیح)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    برای (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	اگر (p[0] != 0)
	{
	    اگر (region_names == پوچ)	    // first language with regions
		region_names = p;
	    وگرنه اگر (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		بشکن;
	    }
	}
    }

    اگر (do_region && region_names != پوچ && pat == پوچ)
    {
	vim_snprintf((نویسه *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    وگرنه
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    برای (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	اگر (slang->sl_fbyts == پوچ)	    // reloading failed
	    ادامه;

	اگر (pat == پوچ)
	{
	    vim_snprintf((نویسه *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	اگر (pat != پوچ && slang->sl_pbyts == پوچ)
	    patlen = (صحیح)STRLEN(pat);
	وگرنه
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	برای (round = 1; round <= 2; ++round)
	{
	    اگر (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    وگرنه
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    اگر (byts == پوچ)
		ادامه;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    وقتی (depth >= 0 && !got_int
				  && (pat == پوچ || !ins_compl_interrupted()))
	    {
		اگر (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    سطر_breakcheck();
		    ins_compl_check_کلیدها(50, FALSE);
		}
		وگرنه
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    اگر (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (صحیح)idxs[n];
			اگر ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    اگر (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    اگر (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				اگر (pat == پوچ)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    اگر (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    وگرنه
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
			اگر (depth <= patlen
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
    ایستا تهی
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    صحیح		*dir,
    صحیح		dumpflags,
    صحیح		wordflags,
    linenr_T	lnum)
{
    صحیح		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    صحیح		i;
    صحیح		flags = wordflags;

    اگر (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    اگر (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    اگر ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    وگرنه
    {
	p = word;
	اگر ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, پوچ) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    اگر (pat == پوچ)
    {
	// Add flags and regions after a slash.
	اگر ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    اگر (keepcap)
		STRCAT(badword, "=");
	    اگر (flags & WF_BANNED)
		STRCAT(badword, "!");
	    وگرنه اگر (flags & WF_RARE)
		STRCAT(badword, "?");
	    اگر (flags & WF_REGION)
		برای (i = 0; i < 7; ++i)
		    اگر (flags & (0x10000 << i))
			sprintf((نویسه *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	اگر (dumpflags & DUMPFLAG_شمار)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((نویسه *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_شمار);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    وگرنه اگر (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (صحیح)STRLEN(p),
					  p_ic, پوچ, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    ایستا linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    صحیح		*dir,
    صحیح		dumpflags,
    صحیح		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    صحیح		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    صحیح		has_word_up = FALSE;
    صحیح		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    صحیح		depth;
    صحیح		n;
    صحیح		len;
    صحیح		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    اگر (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    اگر (byts != پوچ)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	وقتی (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    اگر (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		سطر_breakcheck();
	    }
	    وگرنه
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		اگر (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    برای (i = 1; i < len; ++i)
			اگر (byts[n + i] != 0)
			    بشکن;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    اگر (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			اگر (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    اگر (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			اگر (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    اگر (lnum != 0)
				++lnum;
			}
		    }
		}
		وگرنه اگر (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    بازگشت lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    وقتی (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    بازگشت p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    صحیح
spell_word_start(صحیح startcol)
{
    char_u	*سطر;
    char_u	*p;
    صحیح		col = 0;

    اگر (no_spell_checking(curwin))
	بازگشت startcol;

    // Find a word character before "startcol".
    سطر = ml_get_curline();
    برای (p = سطر + startcol; p > سطر; )
    {
	MB_PTR_BACK(سطر, p);
	اگر (spell_iswordp_nmw(p, curwin))
	    بشکن;
    }

    // Go back to start of the word.
    وقتی (p > سطر)
    {
	col = (صحیح)(p - سطر);
	MB_PTR_BACK(سطر, p);
	اگر (!spell_iswordp(p, curwin))
	    بشکن;
	col = 0;
    }

    بازگشت col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
ایستا صحیح spell_expand_need_cap;

    تهی
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
    صحیح
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    بازگشت ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    صحیح
valid_spelllang(char_u *val)
{
    بازگشت valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    صحیح
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    صحیح		l;

    spf = val;
    وقتی (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	اگر (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    بازگشت FALSE;
	برای (s = spf_name; *s != NUL; ++s)
	    اگر (!vim_is_fname_char(*s))
		بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    نویسه *
did_set_spell_option(تهی)
{
    نویسه    *errmsg = پوچ;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    بشکن;
	}
    بازگشت errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    نویسه *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    اگر (synblock->b_p_spc == پوچ || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = پوچ;
    وگرنه
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	اگر (re != پوچ)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    اگر (synblock->b_cap_prog == پوچ)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		بازگشت e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    بازگشت پوچ;
}

#endif  // FEAT_SPELL
