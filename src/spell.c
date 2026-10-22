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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SPELL)

#અન્તર્ભાવ <time.h>

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
પ્રકારવ્યાખ્યા સંરચના matchinf_S
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
    પૂર્ણાંક		mi_fwordlen;		// nr of valid bytes in mi_fword

    // for when checking word after a prefix
    પૂર્ણાંક		mi_prefarridx;		// index in sl_pidxs with list of
					// affixID/condition
    પૂર્ણાંક		mi_prefcnt;		// number of entries at mi_prefarridx
    પૂર્ણાંક		mi_prefixlen;		// byte length of prefix
    પૂર્ણાંક		mi_cprefixlen;		// byte length of prefix in original
					// case

    // for when checking a compound word
    પૂર્ણાંક		mi_compoff;		// start of following word offset
    char_u	mi_compflags[MAXWLEN];	// flags for compound words used
    પૂર્ણાંક		mi_complen;		// nr of compound words used
    પૂર્ણાંક		mi_compextra;		// nr of COMPOUNDROOT words

    // others
    પૂર્ણાંક		mi_result;		// result so far: SP_BAD, SP_OK, etc.
    પૂર્ણાંક		mi_capflags;		// WF_ONECAP WF_ALLCAP WF_KEEPCAP
    win_T	*mi_win;		// buffer being checked

    // for NOBREAK
    પૂર્ણાંક		mi_result2;		// "mi_result" without following word
    char_u	*mi_end2;		// "mi_end" without following word
} matchinf_T;


સ્થિર પૂર્ણાંક spell_mb_isword_class(પૂર્ણાંક cl, win_T *wp);

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

સ્થિર શૂન્ય find_word(matchinf_T *mip, પૂર્ણાંક mode);
સ્થિર શૂન્ય find_prefix(matchinf_T *mip, પૂર્ણાંક mode);
સ્થિર પૂર્ણાંક fold_more(matchinf_T *mip);
સ્થિર શૂન્ય spell_load_cb(char_u *fname, શૂન્ય *cookie);
સ્થિર પૂર્ણાંક ગણતરી_syllables(slang_T *slang, char_u *word);
સ્થિર શૂન્ય clear_midword(win_T *buf);
સ્થિર શૂન્ય use_midword(slang_T *lp, win_T *buf);
સ્થિર પૂર્ણાંક find_region(char_u *rp, char_u *region);
સ્થિર શૂન્ય spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res);
સ્થિર શૂન્ય spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res);
સ્થિર શૂન્ય spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res);
સ્થિર શૂન્ય dump_word(slang_T *slang, char_u *word, char_u *pat, પૂર્ણાંક *dir, પૂર્ણાંક round, પૂર્ણાંક flags, linenr_T lnum);
સ્થિર linenr_T dump_prefixes(slang_T *slang, char_u *word, char_u *pat, પૂર્ણાંક *dir, પૂર્ણાંક round, પૂર્ણાંક flags, linenr_T startlnum);
સ્થિર char_u *advance_camelcase_word(char_u *p, win_T *wp, પૂર્ણાંક *is_camel_case);

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
    પૂર્ણાંક
spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    પૂર્ણાંક		*capcol,	// column to check for Capital
    પૂર્ણાંક		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    પૂર્ણાંક		nrlen = 0;	// found a number first
    પૂર્ણાંક		c;
    પૂર્ણાંક		wrongcaplen = 0;
    પૂર્ણાંક		lpi;
    પૂર્ણાંક		ગણતરી_word = docount;
    પૂર્ણાંક		use_camel_case = *wp->w_s->b_p_spo != NUL;
    પૂર્ણાંક		is_camel_case = FALSE;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    જો (*ptr <= ' ')
	પાછા 1;

    // Return here when loading language files failed.
    જો (wp->w_s->b_langp.ga_len == 0)
	પાછા 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    જો (*ptr >= '0' && *ptr <= '9')
    {
	જો (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	નહીંતર જો (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	નહીંતર
	    mi.mi_end = skipdigits(ptr);
	nrlen = (પૂર્ણાંક)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    જો (spell_iswordp(mi.mi_fend, wp))
    {
	જો (use_camel_case)
	    mi.mi_fend = advance_camelcase_word(ptr, wp, &is_camel_case);
	નહીંતર
	{
	    કરો
	    {
		MB_PTR_ADV(mi.mi_fend);
	    } જ્યારે (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp));
	}

	જો (capcol != ખાલી && *capcol == 0 && wp->w_s->b_cap_prog != ખાલી)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    જો (!SPELL_ISUPPER(c))
		wrongcaplen = (પૂર્ણાંક)(mi.mi_fend - ptr);
	}
    }
    જો (capcol != ખાલી)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = ખાલી;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    જો (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (શૂન્ય)spell_casefold(wp, ptr, (પૂર્ણાંક)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (પૂર્ણાંક)STRLEN(mi.mi_fword);

    જો (is_camel_case && mi.mi_fwordlen > 0)
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
    માટે (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	જો (mi.mi_lp->lp_slang->sl_fidxs == ખાલી)
	    ચાલુ;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	જો (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	જો (ગણતરી_word && mi.mi_result == SP_OK)
	{
	    ગણતરી_common_word(mi.mi_lp->lp_slang, ptr,
						   (પૂર્ણાંક)(mi.mi_end - ptr), 1);
	    ગણતરી_word = FALSE;
	}
    }

    જો (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	જો (nrlen > 0)
	{
	    જો (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		પાછા nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	નહીંતર જો (!spell_iswordp_nmw(ptr, wp))
	{
	    જો (capcol != ખાલી && wp->w_s->b_cap_prog != ખાલી)
	    {
		regmatch_T	regmatch;
		પૂર્ણાંક		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		જો (r)
		    *capcol = (પૂર્ણાંક)(regmatch.endp[0] - ptr);
	    }

	    જો (has_mbyte)
		પાછા (*mb_ptr2len)(ptr);
	    પાછા 1;
	}
	નહીંતર જો (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	નહીંતર જો (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    પૂર્ણાંક		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    જો (mi.mi_lp->lp_slang->sl_fidxs != ખાલી)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		માટે (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    જો (p >= mi.mi_end)
			તોડો;
		    mi.mi_compoff = (પૂર્ણાંક)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    જો (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			તોડો;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	જો (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	નહીંતર જો (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	નહીંતર
	    *attrp = HLF_SPL;
    }

    જો (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	પાછા wrongcaplen;
    }

    પાછા (પૂર્ણાંક)(mi.mi_end - ptr);
}

/*
 * Determine the type of character 'c'.
 */
    સ્થિર પૂર્ણાંક
get_char_type(પૂર્ણાંક c)
{
    જો (VIM_ISDIGIT(c))
	પાછા CHAR_DIGIT;
    જો (SPELL_ISUPPER(c))
	પાછા CHAR_UPPER;
    પાછા CHAR_OTHER;
}

/*
 * Returns a pointer to the end of the word starting at "str".
 * Supports camelCase words.
 */
    સ્થિર char_u *
advance_camelcase_word(char_u *str, win_T *wp, પૂર્ણાંક *is_camel_case)
{
    પૂર્ણાંક last_type, last_last_type, this_type;
    પૂર્ણાંક c;
    char_u *end = str;

    *is_camel_case = FALSE;

    જો (*str == NUL)
	પાછા str;

    c = PTR2CHAR(end);
    MB_PTR_ADV(end);
    // We need at most the types of the type of the last two chars.
    last_last_type = -1;
    last_type = get_char_type(c);

    જ્યારે (*end != NUL && spell_iswordp(end, wp))
    {
	c = PTR2CHAR(end);
	this_type = get_char_type(c);

	જો (last_last_type == CHAR_UPPER && last_type == CHAR_UPPER
		&& this_type == CHAR_OTHER)
	{
	    // Handle the following cases:
	    // UpperUpperLower
	    *is_camel_case = TRUE;
	    // Back up by one char.
	    MB_PTR_BACK(str, end);
	    તોડો;
	}
	નહીંતર જો ((this_type == CHAR_UPPER && last_type == CHAR_OTHER)
		|| (this_type != last_type
		    && (this_type == CHAR_DIGIT || last_type == CHAR_DIGIT)))
	{
	    // Handle the following cases:
	    // LowerUpper LowerDigit UpperDigit DigitUpper DigitLower
	    *is_camel_case = TRUE;
	    તોડો;
	}

	last_last_type = last_type;
	last_type = this_type;

	MB_PTR_ADV(end);
    }

    પાછા end;
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
    સ્થિર શૂન્ય
find_word(matchinf_T *mip, પૂર્ણાંક mode)
{
    idx_T	arridx = 0;
    પૂર્ણાંક		endlen[MAXWLEN];    // length at possible word endings
    idx_T	endidx[MAXWLEN];    // possible word endings
    પૂર્ણાંક		endidxcnt = 0;
    પૂર્ણાંક		len;
    પૂર્ણાંક		wlen = 0;
    પૂર્ણાંક		flen;
    પૂર્ણાંક		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    char_u	*s;
    char_u	*p;
    પૂર્ણાંક		res = SP_BAD;
    slang_T	*slang = mip->mi_lp->lp_slang;
    unsigned	flags;
    char_u	*byts;
    idx_T	*idxs;
    પૂર્ણાંક		word_ends;
    પૂર્ણાંક		prefix_found;
    પૂર્ણાંક		nobreak_result;

    જો (mode == FIND_KEEPWORD || mode == FIND_KEEPCOMPOUND)
    {
	// Check for word with matching case in keep-case tree.
	ptr = mip->mi_word;
	flen = 9999;		    // no case folding, always enough bytes
	byts = slang->sl_kbyts;
	idxs = slang->sl_kidxs;

	જો (mode == FIND_KEEPCOMPOUND)
	    // Skip over the previously found word(s).
	    wlen += mip->mi_compoff;
    }
    નહીંતર
    {
	// Check for case-folded in case-folded tree.
	ptr = mip->mi_fword;
	flen = mip->mi_fwordlen;    // available case-folded bytes
	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	જો (mode == FIND_PREFIX)
	{
	    // Skip over the prefix.
	    wlen = mip->mi_prefixlen;
	    flen -= mip->mi_prefixlen;
	}
	નહીંતર જો (mode == FIND_COMPOUND)
	{
	    // Skip over the previously found word(s).
	    wlen = mip->mi_compoff;
	    flen -= mip->mi_compoff;
	}

    }

    જો (byts == ખાલી)
	પાછા;			// array is empty

    /*
     * Repeat advancing in the tree until:
     * - there is a byte that doesn't match,
     * - we reach the end of the tree,
     * - or we reach the end of the line.
     */
    માટે (;;)
    {
	જો (flen <= 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// Remember this index, we first check for the longest word.
	જો (byts[arridx] == 0)
	{
	    જો (endidxcnt == MAXWLEN)
	    {
		// Must be a corrupted spell file.
		emsg(_(e_format_error_in_spell_file));
		પાછા;
	    }
	    endlen[endidxcnt] = wlen;
	    endidx[endidxcnt++] = arridx++;
	    --len;

	    // Skip over the zeros, there can be several flag/region
	    // combinations.
	    જ્યારે (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    જો (len == 0)
		તોડો;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	જો (ptr[wlen] == NUL)
	    તોડો;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	જો (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	lo = arridx;
	hi = arridx + len - 1;
	જ્યારે (lo < hi)
	{
	    m = (lo + hi) / 2;
	    જો (byts[m] > c)
		hi = m - 1;
	    નહીંતર જો (byts[m] < c)
		lo = m + 1;
	    નહીંતર
	    {
		lo = hi = m;
		તોડો;
	    }
	}

	// Stop if there is no matching byte.
	જો (hi < lo || byts[lo] != c)
	    તોડો;

	// Continue at the child (if there is one).
	arridx = idxs[lo];
	++wlen;
	--flen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	જો (c == ' ')
	{
	    માટે (;;)
	    {
		જો (flen <= 0 && *mip->mi_fend != NUL)
		    flen = fold_more(mip);
		જો (ptr[wlen] != ' ' && ptr[wlen] != TAB)
		    તોડો;
		++wlen;
		--flen;
	    }
	}
    }

    /*
     * Verify that one of the possible endings is valid.  Try the longest
     * first.
     */
    જ્યારે (endidxcnt > 0)
    {
	--endidxcnt;
	arridx = endidx[endidxcnt];
	wlen = endlen[endidxcnt];

	જો ((*mb_head_off)(ptr, ptr + wlen) > 0)
	    ચાલુ;	    // not at first byte of character
	જો (spell_iswordp(ptr + wlen, mip->mi_win))
	{
	    જો (slang->sl_compprog == ખાલી && !slang->sl_nobreak)
		ચાલુ;	    // next char is a word character
	    word_ends = FALSE;
	}
	નહીંતર
	    word_ends = TRUE;
	// The prefix flag is before compound flags.  Once a valid prefix flag
	// has been found we try compound flags.
	prefix_found = FALSE;

	જો (mode != FIND_KEEPWORD && has_mbyte)
	{
	    // Compute byte length in original word, length may change
	    // when folding case.  This can be slow, take a shortcut when the
	    // case-folded word is equal to the keep-case word.
	    p = mip->mi_word;
	    જો (STRNCMP(ptr, p, wlen) != 0)
	    {
		માટે (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
		    MB_PTR_ADV(p);
		wlen = (પૂર્ણાંક)(p - mip->mi_word);
	    }
	}

	// Check flags and region.  For FIND_PREFIX check the condition and
	// prefix ID.
	// Repeat this if there are more flags/region alternatives until there
	// is a match.
	res = SP_BAD;
	માટે (len = byts[arridx - 1]; len > 0 && byts[arridx] == 0;
							      --len, ++arridx)
	{
	    flags = idxs[arridx];

	    // For the fold-case tree check that the case of the checked word
	    // matches with what the word in the tree requires.
	    // For keep-case tree the case is always right.  For prefixes we
	    // don't bother to check.
	    જો (mode == FIND_FOLDWORD)
	    {
		જો (mip->mi_cend != mip->mi_word + wlen)
		{
		    // mi_capflags was set for a different word length, need
		    // to do it again.
		    mip->mi_cend = mip->mi_word + wlen;
		    mip->mi_capflags = captype(mip->mi_word, mip->mi_cend);
		}

		જો (mip->mi_capflags == WF_KEEPCAP
				|| !spell_valid_case(mip->mi_capflags, flags))
		    ચાલુ;
	    }

	    // When mode is FIND_PREFIX the word must support the prefix:
	    // check the prefix ID and the condition.  Do that for the list at
	    // mip->mi_prefarridx that find_prefix() filled.
	    નહીંતર જો (mode == FIND_PREFIX && !prefix_found)
	    {
		c = valid_word_prefix(mip->mi_prefcnt, mip->mi_prefarridx,
				    flags,
				    mip->mi_word + mip->mi_cprefixlen, slang,
				    FALSE);
		જો (c == 0)
		    ચાલુ;

		// Use the WF_RARE flag for a rare prefix.
		જો (c & WF_RAREPFX)
		    flags |= WF_RARE;
		prefix_found = TRUE;
	    }

	    જો (slang->sl_nobreak)
	    {
		જો ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND)
			&& (flags & WF_BANNED) == 0)
		{
		    // NOBREAK: found a valid following word.  That's all we
		    // need to know, so return.
		    mip->mi_result = SP_OK;
		    તોડો;
		}
	    }

	    નહીંતર જો ((mode == FIND_COMPOUND || mode == FIND_KEEPCOMPOUND
								|| !word_ends))
	    {
		// If there is no compound flag or the word is shorter than
		// COMPOUNDMIN reject it quickly.
		// Makes you wonder why someone puts a compound flag on a word
		// that's too short...  Myspell compatibility requires this
		// anyway.
		જો (((unsigned)flags >> 24) == 0
			     || wlen - mip->mi_compoff < slang->sl_compminlen)
		    ચાલુ;
		// For multi-byte chars check character length against
		// COMPOUNDMIN.
		જો (has_mbyte
			&& slang->sl_compminlen > 0
			&& mb_charlen_len(mip->mi_word + mip->mi_compoff,
				wlen - mip->mi_compoff) < slang->sl_compminlen)
			ચાલુ;

		// Limit the number of compound words to COMPOUNDWORDMAX if no
		// maximum for syllables is specified.
		જો (!word_ends && mip->mi_complen + mip->mi_compextra + 2
							   > slang->sl_compmax
					   && slang->sl_compsylmax == MAXWLEN)
		    ચાલુ;

		// Don't allow compounding on a side where an affix was added,
		// unless COMPOUNDPERMITFLAG was used.
		જો (mip->mi_complen > 0 && (flags & WF_NOCOMPBEF))
		    ચાલુ;
		જો (!word_ends && (flags & WF_NOCOMPAFT))
		    ચાલુ;

		// Quickly check if compounding is possible with this flag.
		જો (!byte_in_str(mip->mi_complen == 0
					? slang->sl_compstartflags
					: slang->sl_compallflags,
					    ((unsigned)flags >> 24)))
		    ચાલુ;

		// If there is a match with a CHECKCOMPOUNDPATTERN rule
		// discard the compound word.
		જો (match_checkcompoundpattern(ptr, wlen, &slang->sl_comppat))
		    ચાલુ;

		જો (mode == FIND_COMPOUND)
		{
		    પૂર્ણાંક	    capflags;

		    // Need to check the caps type of the appended compound
		    // word.
		    જો (has_mbyte && STRNCMP(ptr, mip->mi_word,
							mip->mi_compoff) != 0)
		    {
			// case folding may have changed the length
			p = mip->mi_word;
			માટે (s = ptr; s < ptr + mip->mi_compoff; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
		    }
		    નહીંતર
			p = mip->mi_word + mip->mi_compoff;
		    capflags = captype(p, mip->mi_word + wlen);
		    જો (capflags == WF_KEEPCAP || (capflags == WF_ALLCAP
						 && (flags & WF_FIXCAP) != 0))
			ચાલુ;

		    જો (capflags != WF_ALLCAP)
		    {
			// When the character before the word is a word
			// character we do not accept a Onecap word.  We do
			// accept a no-caps word, even when the dictionary
			// word specifies ONECAP.
			MB_PTR_BACK(mip->mi_word, p);
			જો (spell_iswordp_nmw(p, mip->mi_win)
				? capflags == WF_ONECAP
				: (flags & WF_ONECAP) != 0
						     && capflags != WF_ONECAP)
			    ચાલુ;
		    }
		}

		// If the word ends the sequence of compound flags of the
		// words must match with one of the COMPOUNDRULE items and
		// the number of syllables must not be too large.
		mip->mi_compflags[mip->mi_complen] = ((unsigned)flags >> 24);
		mip->mi_compflags[mip->mi_complen + 1] = NUL;
		જો (word_ends)
		{
		    char_u	fword[MAXWLEN];

		    જો (slang->sl_compsylmax < MAXWLEN)
		    {
			// "fword" is only needed for checking syllables.
			જો (ptr == mip->mi_word)
			    (શૂન્ય)spell_casefold(mip->mi_win,
						    ptr, wlen, fword, MAXWLEN);
			નહીંતર
			    vim_strncpy(fword, ptr, endlen[endidxcnt]);
		    }
		    જો (!can_compound(slang, fword, mip->mi_compflags))
			ચાલુ;
		}
		નહીંતર જો (slang->sl_comprules != ખાલી
			     && !match_compoundrule(slang, mip->mi_compflags))
		    // The compound flags collected so far do not match any
		    // COMPOUNDRULE, discard the compounded word.
		    ચાલુ;
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.
	    નહીંતર જો (flags & WF_NEEDCOMP)
		ચાલુ;

	    nobreak_result = SP_OK;

	    જો (!word_ends)
	    {
		પૂર્ણાંક	save_result = mip->mi_result;
		char_u	*save_end = mip->mi_end;
		langp_T	*save_lp = mip->mi_lp;
		પૂર્ણાંક	lpi;

		// Check that a valid word follows.  If there is one and we
		// are compounding, it will set "mi_result", thus we are
		// always finished here.  For NOBREAK we only check that a
		// valid word follows.
		// Recursive!
		જો (slang->sl_nobreak)
		    mip->mi_result = SP_BAD;

		// Find following word in case-folded tree.
		mip->mi_compoff = endlen[endidxcnt];
		જો (has_mbyte && mode == FIND_KEEPWORD)
		{
		    // Compute byte length in case-folded word from "wlen":
		    // byte length in keep-case word.  Length may change when
		    // folding case.  This can be slow, take a shortcut when
		    // the case-folded word is equal to the keep-case word.
		    p = mip->mi_fword;
		    જો (STRNCMP(ptr, p, wlen) != 0)
		    {
			માટે (s = ptr; s < ptr + wlen; MB_PTR_ADV(s))
			    MB_PTR_ADV(p);
			mip->mi_compoff = (પૂર્ણાંક)(p - mip->mi_fword);
		    }
		}
#જો 0 // Disabled, see below
		c = mip->mi_compoff;
#endif
		++mip->mi_complen;
		જો (flags & WF_COMPROOT)
		    ++mip->mi_compextra;

		// For NOBREAK we need to try all NOBREAK languages, at least
		// to find the ".add" file(s).
		માટે (lpi = 0; lpi < mip->mi_win->w_s->b_langp.ga_len; ++lpi)
		{
		    જો (slang->sl_nobreak)
		    {
			mip->mi_lp = LANGP_ENTRY(mip->mi_win->w_s->b_langp, lpi);
			જો (mip->mi_lp->lp_slang->sl_fidxs == ખાલી
					 || !mip->mi_lp->lp_slang->sl_nobreak)
			    ચાલુ;
		    }

		    find_word(mip, FIND_COMPOUND);

		    // When NOBREAK any word that matches is OK.  Otherwise we
		    // need to find the longest match, thus try with keep-case
		    // and prefix too.
		    જો (!slang->sl_nobreak || mip->mi_result == SP_BAD)
		    {
			// Find following word in keep-case tree.
			mip->mi_compoff = wlen;
			find_word(mip, FIND_KEEPCOMPOUND);

#જો 0	    // Disabled, a prefix must not appear halfway a compound word,
	    // unless the COMPOUNDPERMITFLAG is used and then it can't be a
	    // postponed prefix.
			જો (!slang->sl_nobreak || mip->mi_result == SP_BAD)
			{
			    // Check for following word with prefix.
			    mip->mi_compoff = c;
			    find_prefix(mip, FIND_COMPOUND);
			}
#endif
		    }

		    જો (!slang->sl_nobreak)
			તોડો;
		}
		--mip->mi_complen;
		જો (flags & WF_COMPROOT)
		    --mip->mi_compextra;
		mip->mi_lp = save_lp;

		જો (slang->sl_nobreak)
		{
		    nobreak_result = mip->mi_result;
		    mip->mi_result = save_result;
		    mip->mi_end = save_end;
		}
		નહીંતર
		{
		    જો (mip->mi_result == SP_OK)
			તોડો;
		    ચાલુ;
		}
	    }

	    જો (flags & WF_BANNED)
		res = SP_BANNED;
	    નહીંતર જો (flags & WF_REGION)
	    {
		// Check region.
		જો ((mip->mi_lp->lp_region & (flags >> 16)) != 0)
		    res = SP_OK;
		નહીંતર
		    res = SP_LOCAL;
	    }
	    નહીંતર જો (flags & WF_RARE)
		res = SP_RARE;
	    નહીંતર
		res = SP_OK;

	    // Always use the longest match and the best result.  For NOBREAK
	    // we separately keep the longest match without a following good
	    // word as a fall-back.
	    જો (nobreak_result == SP_BAD)
	    {
		જો (mip->mi_result2 > res)
		{
		    mip->mi_result2 = res;
		    mip->mi_end2 = mip->mi_word + wlen;
		}
		નહીંતર જો (mip->mi_result2 == res
					&& mip->mi_end2 < mip->mi_word + wlen)
		    mip->mi_end2 = mip->mi_word + wlen;
	    }
	    નહીંતર જો (mip->mi_result > res)
	    {
		mip->mi_result = res;
		mip->mi_end = mip->mi_word + wlen;
	    }
	    નહીંતર જો (mip->mi_result == res && mip->mi_end < mip->mi_word + wlen)
		mip->mi_end = mip->mi_word + wlen;

	    જો (mip->mi_result == SP_OK)
		તોડો;
	}

	જો (mip->mi_result == SP_OK)
	    તોડો;
    }
}

/*
 * Return TRUE if there is a match between the word ptr[wlen] and
 * CHECKCOMPOUNDPATTERN rules, assuming that we will concatenate with another
 * word.
 * A match means that the first part of CHECKCOMPOUNDPATTERN matches at the
 * end of ptr[wlen] and the second part matches after it.
 */
    પૂર્ણાંક
match_checkcompoundpattern(
    char_u	*ptr,
    પૂર્ણાંક		wlen,
    garray_T	*gap)  // &sl_comppat
{
    પૂર્ણાંક		i;
    char_u	*p;
    પૂર્ણાંક		len;

    માટે (i = 0; i + 1 < gap->ga_len; i += 2)
    {
	p = ((char_u **)gap->ga_data)[i + 1];
	જો (STRNCMP(ptr + wlen, p, STRLEN(p)) == 0)
	{
	    // Second part matches at start of following compound word, now
	    // check if first part matches at end of previous word.
	    p = ((char_u **)gap->ga_data)[i];
	    len = (પૂર્ણાંક)STRLEN(p);
	    જો (len <= wlen && STRNCMP(ptr + wlen - len, p, len) == 0)
		પાછા TRUE;
	}
    }
    પાછા FALSE;
}

/*
 * Return TRUE if "flags" is a valid sequence of compound flags and "word"
 * does not have too many syllables.
 */
    પૂર્ણાંક
can_compound(slang_T *slang, char_u *word, char_u *flags)
{
    char_u	uflags[MAXWLEN * 2];
    પૂર્ણાંક		i;
    char_u	*p;

    જો (slang->sl_compprog == ખાલી)
	પાછા FALSE;
    જો (enc_utf8)
    {
	// Need to convert the single byte flags to utf8 characters.
	p = uflags;
	માટે (i = 0; flags[i] != NUL; ++i)
	    p += utf_char2bytes(flags[i], p);
	*p = NUL;
	p = uflags;
    }
    નહીંતર
	p = flags;
    જો (!vim_regexec_prog(&slang->sl_compprog, FALSE, p, 0))
	પાછા FALSE;

    // Count the number of syllables.  This may be slow, do it last.  If there
    // are too many syllables AND the number of compound words is above
    // COMPOUNDWORDMAX then compounding is not allowed.
    જો (slang->sl_compsylmax < MAXWLEN
		       && ગણતરી_syllables(slang, word) > slang->sl_compsylmax)
	પાછા (પૂર્ણાંક)STRLEN(flags) < slang->sl_compmax;
    પાછા TRUE;
}

/*
 * Return TRUE if the compound flags in compflags[] match the start of any
 * compound rule.  This is used to stop trying a compound if the flags
 * collected so far can't possibly match any compound rule.
 * Caller must check that slang->sl_comprules is not NULL.
 */
    પૂર્ણાંક
match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;

    // loop over all the COMPOUNDRULE entries
    માટે (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	માટે (i = 0; ; ++i)
	{
	    c = compflags[i];
	    જો (c == NUL)
		// found a rule that matches for the flags we have so far
		પાછા TRUE;
	    જો (*p == '/' || *p == NUL)
		તોડો;  // end of rule, it's too short
	    જો (*p == '[')
	    {
		પૂર્ણાંક match = FALSE;

		// compare against all the flags in []
		++p;
		જ્યારે (*p != ']' && *p != NUL)
		    જો (*p++ == c)
			match = TRUE;
		જો (!match)
		    તોડો;  // none matches
	    }
	    નહીંતર જો (*p != c)
		તોડો;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	જો (p == ખાલી)
	    તોડો;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    પાછા FALSE;
}

/*
 * Return non-zero if the prefix indicated by "arridx" matches with the prefix
 * ID in "flags" for the word "word".
 * The WF_RAREPFX flag is included in the return value for a rare prefix.
 */
    પૂર્ણાંક
valid_word_prefix(
    પૂર્ણાંક		totprefcnt,	// nr of prefix IDs
    પૂર્ણાંક		arridx,		// idx in sl_pidxs[]
    પૂર્ણાંક		flags,
    char_u	*word,
    slang_T	*slang,
    પૂર્ણાંક		cond_req)	// only use prefixes with a condition
{
    પૂર્ણાંક		prefcnt;
    પૂર્ણાંક		pidx;
    regprog_T	**rp;
    પૂર્ણાંક		prefid;

    prefid = (unsigned)flags >> 24;
    માટે (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	જો (prefid != (pidx & 0xff))
	    ચાલુ;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	જો ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    ચાલુ;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	જો (*rp != ખાલી)
	{
	    જો (!vim_regexec_prog(rp, FALSE, word, 0))
		ચાલુ;
	}
	નહીંતર જો (cond_req)
	    ચાલુ;

	// It's a match!  Return the WF_ flags.
	પાછા pidx;
    }
    પાછા 0;
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
    સ્થિર શૂન્ય
find_prefix(matchinf_T *mip, પૂર્ણાંક mode)
{
    idx_T	arridx = 0;
    પૂર્ણાંક		len;
    પૂર્ણાંક		wlen = 0;
    પૂર્ણાંક		flen;
    પૂર્ણાંક		c;
    char_u	*ptr;
    idx_T	lo, hi, m;
    slang_T	*slang = mip->mi_lp->lp_slang;
    char_u	*byts;
    idx_T	*idxs;

    byts = slang->sl_pbyts;
    જો (byts == ખાલી)
	પાછા;			// array is empty

    // We use the case-folded word here, since prefixes are always
    // case-folded.
    ptr = mip->mi_fword;
    flen = mip->mi_fwordlen;    // available case-folded bytes
    જો (mode == FIND_COMPOUND)
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
    માટે (;;)
    {
	જો (flen == 0 && *mip->mi_fend != NUL)
	    flen = fold_more(mip);

	len = byts[arridx++];

	// If the first possible byte is a zero the prefix could end here.
	// Check if the following word matches and supports the prefix.
	જો (byts[arridx] == 0)
	{
	    // There can be several prefixes with different conditions.  We
	    // try them all, since we don't know which one will give the
	    // longest match.  The word is the same each time, pass the list
	    // of possible prefixes to find_word().
	    mip->mi_prefarridx = arridx;
	    mip->mi_prefcnt = len;
	    જ્યારે (len > 0 && byts[arridx] == 0)
	    {
		++arridx;
		--len;
	    }
	    mip->mi_prefcnt -= len;

	    // Find the word that comes after the prefix.
	    mip->mi_prefixlen = wlen;
	    જો (mode == FIND_COMPOUND)
		// Skip over the previously found word(s).
		mip->mi_prefixlen += mip->mi_compoff;

	    જો (has_mbyte)
	    {
		// Case-folded length may differ from original length.
		mip->mi_cprefixlen = nofold_len(mip->mi_fword,
					     mip->mi_prefixlen, mip->mi_word);
	    }
	    નહીંતર
		mip->mi_cprefixlen = mip->mi_prefixlen;
	    find_word(mip, FIND_PREFIX);


	    જો (len == 0)
		તોડો;	    // no children, word must end here
	}

	// Stop looking at end of the line.
	જો (ptr[wlen] == NUL)
	    તોડો;

	// Perform a binary search in the list of accepted bytes.
	c = ptr[wlen];
	lo = arridx;
	hi = arridx + len - 1;
	જ્યારે (lo < hi)
	{
	    m = (lo + hi) / 2;
	    જો (byts[m] > c)
		hi = m - 1;
	    નહીંતર જો (byts[m] < c)
		lo = m + 1;
	    નહીંતર
	    {
		lo = hi = m;
		તોડો;
	    }
	}

	// Stop if there is no matching byte.
	જો (hi < lo || byts[lo] != c)
	    તોડો;

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
    સ્થિર પૂર્ણાંક
fold_more(matchinf_T *mip)
{
    પૂર્ણાંક		flen;
    char_u	*p;

    p = mip->mi_fend;
    કરો
	MB_PTR_ADV(mip->mi_fend);
    જ્યારે (*mip->mi_fend != NUL && spell_iswordp(mip->mi_fend, mip->mi_win));

    // Include the non-word character so that we can check for the word end.
    જો (*mip->mi_fend != NUL)
	MB_PTR_ADV(mip->mi_fend);

    (શૂન્ય)spell_casefold(mip->mi_win, p, (પૂર્ણાંક)(mip->mi_fend - p),
			     mip->mi_fword + mip->mi_fwordlen,
			     MAXWLEN - mip->mi_fwordlen);
    flen = (પૂર્ણાંક)STRLEN(mip->mi_fword + mip->mi_fwordlen);
    mip->mi_fwordlen += flen;
    પાછા flen;
}

/*
 * Check case flags for a word.  Return TRUE if the word has the requested
 * case.
 */
    પૂર્ણાંક
spell_valid_case(
    પૂર્ણાંક	    wordflags,	    // flags for the checked word.
    પૂર્ણાંક	    treeflags)	    // flags for the word in the spell tree
{
    પાછા ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}

/*
 * Return TRUE if spell checking is enabled for "wp".
 */
    પૂર્ણાંક
spell_check_window(win_T *wp)
{
    પાછા wp->w_p_spell
		&& *wp->w_s->b_p_spl != NUL
		&& wp->w_s->b_langp.ga_len > 0
		&& *(અક્ષર **)(wp->w_s->b_langp.ga_data) != ખાલી;
}

/*
 * Return TRUE and give an error if spell checking is not enabled.
 */
    સ્થિર પૂર્ણાંક
no_spell_checking(win_T *wp)
{
    જો (spell_check_window(wp))
	પાછા FALSE;
    emsg(_(e_spell_checking_is_not_possible));
    પાછા TRUE;
}

/*
 * Move to next spell error.
 * "curline" is FALSE for "[s", "]s", "[S" and "]S".
 * "curline" is TRUE to find word under/after cursor in the same line.
 * For Insert mode completion "dir" is BACKWARD and "curline" is TRUE: move
 * to after badly spelled word before the cursor.
 * Return 0 if not found, length of the badly spelled word otherwise.
 */
    પૂર્ણાંક
spell_move_to(
    win_T	*wp,
    પૂર્ણાંક		dir,		// FORWARD or BACKWARD
    smt_T	behaviour,	// Behaviour of the function
    પૂર્ણાંક		curline,
    hlf_T	*attrp)		// return: attributes of bad word or NULL
				// (only when "dir" is FORWARD)
{
    linenr_T	lnum;
    pos_T	found_pos;
    પૂર્ણાંક		found_len = 0;
    char_u	*લીટી;
    char_u	*p;
    char_u	*endp;
    hlf_T	attr = 0;
    પૂર્ણાંક		len;
#ifdef FEAT_SYN_HL
    પૂર્ણાંક		has_syntax = syntax_present(wp);
#endif
    પૂર્ણાંક		col;
    પૂર્ણાંક		can_spell;
    char_u	*buf = ખાલી;
    પૂર્ણાંક		buflen = 0;
    પૂર્ણાંક		skip = 0;
    પૂર્ણાંક		capcol = -1;
    પૂર્ણાંક		found_one = FALSE;
    પૂર્ણાંક		wrapped = FALSE;

    જો (no_spell_checking(wp))
	પાછા 0;

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

    જ્યારે (!got_int)
    {
	પૂર્ણાંક empty_લીટી;

	લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);

	len = ml_get_buf_len(wp->w_buffer, lnum);
	જો (buflen < len + MAXWLEN + 2)
	{
	    vim_free(buf);
	    buflen = len + MAXWLEN + 2;
	    buf = alloc(buflen);
	    જો (buf == ખાલી)
		તોડો;
	}

	// In first line check first word for Capital.
	જો (lnum == 1)
	    capcol = 0;

	// For checking first word with a capital skip white space.
	જો (capcol == 0)
	    capcol = getwhitecols(લીટી);
	નહીંતર જો (curline && wp == curwin)
	{
	    // For spellbadword(): check if first word needs a capital.
	    col = getwhitecols(લીટી);
	    જો (check_need_cap(curwin, lnum, col))
		capcol = col;

	    // Need to get the line again, may have looked at the previous
	    // one.
	    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);
	}

	// Copy the line into "buf" and append the start of the next line if
	// possible.  Note: this ml_get_buf() may make "line" invalid, check
	// for empty line first.
	empty_લીટી = *skipwhite(લીટી) == NUL;
	STRCPY(buf, લીટી);
	જો (lnum < wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    spell_cat_લીટી(buf + STRLEN(buf),
			  ml_get_buf(wp->w_buffer, lnum + 1, FALSE), MAXWLEN);

	p = buf + skip;
	endp = buf + len;
	જ્યારે (p < endp)
	{
	    // When searching backward don't search after the cursor.  Unless
	    // we wrapped around the end of the buffer.
	    જો (dir == BACKWARD
		    && lnum == wp->w_cursor.lnum
		    && !wrapped
		    && (colnr_T)(p - buf) >= wp->w_cursor.col)
		તોડો;

	    // start of word
	    attr = HLF_ગણતરી;
	    len = spell_check(wp, p, &attr, &capcol, FALSE);

	    જો (attr != HLF_ગણતરી)
	    {
		// We found a bad word.  Check the attribute.
		જો (behaviour == SMT_ALL
				|| (behaviour == SMT_BAD && attr == HLF_SPB)
				|| (behaviour == SMT_RARE && attr == HLF_SPR))
		{
		    // When searching forward only accept a bad word after
		    // the cursor.
		    જો (dir == BACKWARD
			    || lnum != wp->w_cursor.lnum
			    || (wrapped
				|| (colnr_T)(curline ? p - buf + len
						     : p - buf)
						  > wp->w_cursor.col))
		    {
#ifdef FEAT_SYN_HL
			જો (has_syntax)
			{
			    col = (પૂર્ણાંક)(p - buf);
			    (શૂન્ય)syn_get_id(wp, lnum, (colnr_T)col,
						    FALSE, &can_spell, FALSE);
			    જો (!can_spell)
				attr = HLF_ગણતરી;
			}
			નહીંતર
#endif
			    can_spell = TRUE;

			જો (can_spell)
			{
			    found_one = TRUE;
			    found_pos.lnum = lnum;
			    found_pos.col = (પૂર્ણાંક)(p - buf);
			    found_pos.coladd = 0;
			    જો (dir == FORWARD)
			    {
				// No need to search further.
				wp->w_cursor = found_pos;
				vim_free(buf);
				જો (attrp != ખાલી)
				    *attrp = attr;
				પાછા len;
			    }
			    નહીંતર જો (curline)
				// Insert mode completion: put cursor after
				// the bad word.
				found_pos.col += len;
			    found_len = len;
			}
		    }
		    નહીંતર
			found_one = TRUE;
		}
	    }

	    // advance to character after the word
	    p += len;
	    capcol -= len;
	}

	જો (dir == BACKWARD && found_pos.lnum != 0)
	{
	    // Use the last match in the line (before the cursor).
	    wp->w_cursor = found_pos;
	    vim_free(buf);
	    પાછા found_len;
	}

	જો (curline)
	    તોડો;	// only check cursor line

	// If we are back at the starting line and searched it again there
	// is no match, give up.
	જો (lnum == wp->w_cursor.lnum && wrapped)
	    તોડો;

	// Advance to next line.
	જો (dir == BACKWARD)
	{
	    જો (lnum > 1)
		--lnum;
	    નહીંતર જો (!p_ws)
		તોડો;	    // at first line and 'nowrapscan'
	    નહીંતર
	    {
		// Wrap around to the end of the buffer.  May search the
		// starting line again and accept the last match.
		lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
		wrapped = TRUE;
		જો (!shortmess(SHM_શોધ))
		    give_warning((char_u *)_(ટોચ_bot_સંદેશ), TRUE);
	    }
	    capcol = -1;
	}
	નહીંતર
	{
	    જો (lnum < wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
		++lnum;
	    નહીંતર જો (!p_ws)
		તોડો;	    // at first line and 'nowrapscan'
	    નહીંતર
	    {
		// Wrap around to the start of the buffer.  May search the
		// starting line again and accept the first match.
		lnum = 1;
		wrapped = TRUE;
		જો (!shortmess(SHM_શોધ))
		    give_warning((char_u *)_(bot_ટોચ_સંદેશ), TRUE);
	    }

	    // If we are back at the starting line and there is no match then
	    // give up.
	    જો (lnum == wp->w_cursor.lnum && !found_one)
		તોડો;

	    // Skip the characters at the start of the next line that were
	    // included in a match crossing line boundaries.
	    જો (attr == HLF_ગણતરી)
		skip = (પૂર્ણાંક)(p - endp);
	    નહીંતર
		skip = 0;

	    // Capcol skips over the inserted space.
	    --capcol;

	    // But after empty line check first word in next line
	    જો (empty_લીટી)
		capcol = 0;
	}

	લીટી_breakcheck();
    }

    vim_free(buf);
    પાછા 0;
}

/*
 * For spell checking: concatenate the start of the following line "line" into
 * "buf", blanking-out special characters.  Copy less than "maxlen" bytes.
 * Keep the blanks at the start of the next line, this is used in win_line()
 * to skip those bytes if the word was OK.
 */
    શૂન્ય
spell_cat_લીટી(char_u *buf, char_u *લીટી, પૂર્ણાંક maxlen)
{
    char_u	*p;
    પૂર્ણાંક		n;

    p = skipwhite(લીટી);
    જ્યારે (vim_strchr((char_u *)"*#/\"\t", *p) != ખાલી)
	p = skipwhite(p + 1);

    જો (*p == NUL)
	પાછા;

    // Only worth concatenating if there is something else than spaces to
    // concatenate.
    n = (પૂર્ણાંક)(p - લીટી) + 1;
    જો (n < maxlen - 1)
    {
	vim_memset(buf, ' ', n);
	vim_strncpy(buf +  n, p, maxlen - 1 - n);
    }
}

/*
 * Structure used for the cookie argument of do_in_runtimepath().
 */
પ્રકારવ્યાખ્યા સંરચના spelload_S
{
    char_u  sl_lang[MAXWLEN + 1];	// language name
    slang_T *sl_slang;			// resulting slang_T struct
    પૂર્ણાંક	    sl_nobreak;			// NOBREAK language found
} spelload_T;

/*
 * Load word list(s) for "lang" from Vim spell file(s).
 * "lang" must be the language without the region: e.g., "en".
 */
    સ્થિર શૂન્ય
spell_load_lang(char_u *lang)
{
    char_u	fname_enc[85];
    પૂર્ણાંક		r;
    spelload_T	sl;
    પૂર્ણાંક		round;

    // Copy the language name to pass it to spell_load_cb() as a cookie.
    // It's truncated when an error is detected.
    STRCPY(sl.sl_lang, lang);
    sl.sl_slang = ખાલી;
    sl.sl_nobreak = FALSE;

    // Disallow deleting the current buffer.  Autocommands can do weird things
    // and cause "lang" to be freed.
    ++curbuf->b_locked;

    // We may retry when no spell file is found for the language, an
    // autocommand may load it then.
    માટે (round = 1; round <= 2; ++round)
    {
	/*
	 * Find the first spell file for "lang" in 'runtimepath' and load it.
	 */
	vim_snprintf((અક્ષર *)fname_enc, કદ(fname_enc) - 5,
#ifdef VMS
					"spell/%s_%s.spl",
#નહીંતર
					"spell/%s.%s.spl",
#endif
							   lang, spell_enc());
	r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	જો (r == FAIL && *sl.sl_lang != NUL)
	{
	    // Try loading the ASCII version.
	    vim_snprintf((અક્ષર *)fname_enc, કદ(fname_enc) - 5,
#ifdef VMS
						  "spell/%s_ascii.spl",
#નહીંતર
						  "spell/%s.ascii.spl",
#endif
									lang);
	    r = do_in_runtimepath(fname_enc, 0, spell_load_cb, &sl);

	    જો (r == FAIL && *sl.sl_lang != NUL && round == 1
		    && apply_autocmds(EVENT_SPELLFILEMISSING, lang,
					      curbuf->b_fname, FALSE, curbuf))
		ચાલુ;
	    તોડો;
	}
	તોડો;
    }

    જો (r == FAIL)
    {
	smsg(
#ifdef VMS
	_("Warning: Cannot find word list \"%s_%s.spl\" or \"%s_ascii.spl\""),
#નહીંતર
	_("Warning: Cannot find word list \"%s.%s.spl\" or \"%s.ascii.spl\""),
#endif
						     lang, spell_enc(), lang);
    }
    નહીંતર જો (sl.sl_slang != ખાલી)
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
spell_enc(શૂન્ય)
{

    જો (STRLEN(p_enc) < 60 && STRCMP(p_enc, "iso-8859-15") != 0)
	પાછા p_enc;
    પાછા (char_u *)"latin1";
}

/*
 * Get the name of the .spl file for the internal wordlist into
 * "fname[MAXPATHL]".
 */
    સ્થિર શૂન્ય
int_wordlist_spl(char_u *fname)
{
    vim_snprintf((અક્ષર *)fname, MAXPATHL, SPL_FNAME_TMPL,
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
    જો (lp != ખાલી)
    {
	જો (lang != ખાલી)
	    lp->sl_name = vim_strsave(lang);
	ga_init2(&lp->sl_rep, કદ(fromto_T), 10);
	ga_init2(&lp->sl_repsal, કદ(fromto_T), 10);
	lp->sl_compmax = MAXWLEN;
	lp->sl_compsylmax = MAXWLEN;
	hash_init(&lp->sl_wordcount);
    }

    પાછા lp;
}

/*
 * Free the contents of an slang_T and the structure itself.
 */
    શૂન્ય
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
    શૂન્ય
free_sal_વસ્તુઓ(garray_T *gap)
{
    salitem_T	*smp;

    જ્યારે (gap->ga_len > 0)
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
    શૂન્ય
slang_clear(slang_T *lp)
{
    garray_T	*gap;
    fromto_T	*ftp;
    પૂર્ણાંક		i;
    પૂર્ણાંક		round;

    VIM_CLEAR(lp->sl_fbyts);
    VIM_CLEAR(lp->sl_kbyts);
    VIM_CLEAR(lp->sl_pbyts);

    VIM_CLEAR(lp->sl_fidxs);
    VIM_CLEAR(lp->sl_kidxs);
    VIM_CLEAR(lp->sl_pidxs);

    માટે (round = 1; round <= 2; ++round)
    {
	gap = round == 1 ? &lp->sl_rep : &lp->sl_repsal;
	જ્યારે (gap->ga_len > 0)
	{
	    ftp = &((fromto_T *)gap->ga_data)[--gap->ga_len];
	    vim_free(ftp->ft_from);
	    vim_free(ftp->ft_to);
	}
	ga_clear(gap);
    }

    gap = &lp->sl_sal;
    જો (lp->sl_sofo)
    {
	// "ga_len" is set to 1 without adding an item for latin1
	જો (gap->ga_data != ખાલી)
	    // SOFOFROM and SOFOTO items: free lists of wide characters.
	    માટે (i = 0; i < gap->ga_len; ++i)
		vim_free(((પૂર્ણાંક **)gap->ga_data)[i]);
	ga_clear(gap);
    }
    નહીંતર
	free_sal_વસ્તુઓ(gap);

    માટે (i = 0; i < lp->sl_prefixcnt; ++i)
	vim_regfree(lp->sl_prefprog[i]);
    lp->sl_prefixcnt = 0;
    VIM_CLEAR(lp->sl_prefprog);

    VIM_CLEAR(lp->sl_info);

    VIM_CLEAR(lp->sl_midword);

    vim_regfree(lp->sl_compprog);
    lp->sl_compprog = ખાલી;
    VIM_CLEAR(lp->sl_comprules);
    VIM_CLEAR(lp->sl_compstartflags);
    VIM_CLEAR(lp->sl_compallflags);

    VIM_CLEAR(lp->sl_syllable);
    ga_clear(&lp->sl_syl_વસ્તુઓ);

    ga_clear_strings(&lp->sl_comppat);

    hash_clear_all(&lp->sl_wordcount, WC_ચાવી_OFF);
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
    શૂન્ય
slang_clear_sug(slang_T *lp)
{
    VIM_CLEAR(lp->sl_sbyts);
    VIM_CLEAR(lp->sl_sidxs);
    close_spellbuf(lp->sl_sugbuf);
    lp->sl_sugbuf = ખાલી;
    lp->sl_sugloaded = FALSE;
    lp->sl_sugtime = 0;
}

/*
 * Load one spell file and store the info into a slang_T.
 * Invoked through do_in_runtimepath().
 */
    સ્થિર શૂન્ય
spell_load_cb(char_u *fname, શૂન્ય *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, ખાલી, FALSE);
    જો (slang == ખાલી)
	પાછા;

    // When a previously loaded file has NOBREAK also use it for the
    // ".add" files.
    જો (slp->sl_nobreak && slang->sl_add)
	slang->sl_nobreak = TRUE;
    નહીંતર જો (slang->sl_nobreak)
	slp->sl_nobreak = TRUE;

    slp->sl_slang = slang;
}


/*
 * Add a word to the hashtable of common words.
 * If it's already there then the counter is increased.
 */
    શૂન્ય
ગણતરી_common_word(
    slang_T	*lp,
    char_u	*word,
    પૂર્ણાંક		len,	    // word length, -1 for up to NUL
    પૂર્ણાંક		ગણતરી)	    // 1 to count once, 10 to init
{
    hash_T	hash;
    hashitem_T	*hi;
    wordcount_T	*wc;
    char_u	buf[MAXWLEN];
    char_u	*p;

    જો (len == -1)
	p = word;
    નહીંતર જો (len >= MAXWLEN)
	પાછા;
    નહીંતર
    {
	vim_strncpy(buf, word, len);
	p = buf;
    }

    hash = hash_hash(p);
    hi = hash_lookup(&lp->sl_wordcount, p, hash);
    જો (HASHITEM_EMPTY(hi))
    {
	wc = alloc(offsetof(wordcount_T, wc_word) + STRLEN(p) + 1);
	જો (wc == ખાલી)
	    પાછા;
	STRCPY(wc->wc_word, p);
	wc->wc_ગણતરી = ગણતરી;
	hash_add_item(&lp->sl_wordcount, hi, wc->wc_word, hash);
    }
    નહીંતર
    {
	wc = HI2WC(hi);
	જો ((wc->wc_ગણતરી += ગણતરી) < (unsigned)ગણતરી)	// check for overflow
	    wc->wc_ગણતરી = MAXWORDCOUNT;
    }
}

/*
 * Return TRUE if byte "n" appears in "str".
 * Like strchr() but independent of locale.
 */
    પૂર્ણાંક
byte_in_str(char_u *str, પૂર્ણાંક n)
{
    char_u	*p;

    માટે (p = str; *p != NUL; ++p)
	જો (*p == n)
	    પાછા TRUE;
    પાછા FALSE;
}

#define SY_MAXLEN   30
પ્રકારવ્યાખ્યા સંરચના syl_item_S
{
    char_u	sy_chars[SY_MAXLEN];	    // the sequence of chars
    પૂર્ણાંક		sy_len;
} syl_item_T;

/*
 * Truncate "slang->sl_syllable" at the first slash and put the following items
 * in "slang->sl_syl_items".
 */
    પૂર્ણાંક
init_syl_tab(slang_T *slang)
{
    char_u	*p;
    char_u	*s;
    પૂર્ણાંક		l;
    syl_item_T	*syl;

    ga_init2(&slang->sl_syl_વસ્તુઓ, કદ(syl_item_T), 4);
    p = vim_strchr(slang->sl_syllable, '/');
    જ્યારે (p != ખાલી)
    {
	*p++ = NUL;
	જો (*p == NUL)	    // trailing slash
	    તોડો;
	s = p;
	p = vim_strchr(p, '/');
	જો (p == ખાલી)
	    l = (પૂર્ણાંક)STRLEN(s);
	નહીંતર
	    l = (પૂર્ણાંક)(p - s);
	જો (l >= SY_MAXLEN)
	    પાછા SP_FORMERROR;
	જો (ga_grow(&slang->sl_syl_વસ્તુઓ, 1) == FAIL)
	    પાછા SP_OTHERERROR;
	syl = ((syl_item_T *)slang->sl_syl_વસ્તુઓ.ga_data)
					       + slang->sl_syl_વસ્તુઓ.ga_len++;
	vim_strncpy(syl->sy_chars, s, l);
	syl->sy_len = l;
    }
    પાછા OK;
}

/*
 * Count the number of syllables in "word".
 * When "word" contains spaces the syllables after the last space are counted.
 * Returns zero if syllables are not defines.
 */
    સ્થિર પૂર્ણાંક
ગણતરી_syllables(slang_T *slang, char_u *word)
{
    પૂર્ણાંક		cnt = 0;
    પૂર્ણાંક		skip = FALSE;
    char_u	*p;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    syl_item_T	*syl;
    પૂર્ણાંક		c;

    જો (slang->sl_syllable == ખાલી)
	પાછા 0;

    માટે (p = word; *p != NUL; p += len)
    {
	// When running into a space reset counter.
	જો (*p == ' ')
	{
	    len = 1;
	    cnt = 0;
	    ચાલુ;
	}

	// Find longest match of syllable items.
	len = 0;
	માટે (i = 0; i < slang->sl_syl_વસ્તુઓ.ga_len; ++i)
	{
	    syl = ((syl_item_T *)slang->sl_syl_વસ્તુઓ.ga_data) + i;
	    જો (syl->sy_len > len
			       && STRNCMP(p, syl->sy_chars, syl->sy_len) == 0)
		len = syl->sy_len;
	}
	જો (len != 0)	// found a match, count syllable
	{
	    ++cnt;
	    skip = FALSE;
	}
	નહીંતર
	{
	    // No recognized syllable item, at least a syllable char then?
	    c = mb_ptr2char(p);
	    len = (*mb_ptr2len)(p);
	    જો (vim_strchr(slang->sl_syllable, c) == ખાલી)
		skip = FALSE;	    // No, search for next syllable
	    નહીંતર જો (!skip)
	    {
		++cnt;		    // Yes, count it
		skip = TRUE;	    // don't count following syllable chars
	    }
	}
    }
    પાછા cnt;
}

/*
 * Parse 'spelllang' and set w_s->b_langp accordingly.
 * Returns NULL if it's OK, an untranslated error message otherwise.
 */
    અક્ષર *
parse_spelllang(win_T *wp)
{
    garray_T	ga;
    char_u	*splp;
    char_u	*region;
    char_u	region_cp[3];
    પૂર્ણાંક		filename;
    પૂર્ણાંક		region_mask;
    slang_T	*slang;
    પૂર્ણાંક		c;
    char_u	lang[MAXWLEN + 1];
    char_u	spf_name[MAXPATHL];
    પૂર્ણાંક		len;
    char_u	*p;
    પૂર્ણાંક		round;
    char_u	*spf;
    char_u	*use_region = ખાલી;
    પૂર્ણાંક		dont_use_region = FALSE;
    પૂર્ણાંક		nobreak = FALSE;
    પૂર્ણાંક		i, j;
    langp_T	*lp, *lp2;
    સ્થિર પૂર્ણાંક	recursive = FALSE;
    અક્ષર	*ret_સંદેશ = ખાલી;
    char_u	*spl_copy;
    bufref_T	bufref;

    set_bufref(&bufref, wp->w_buffer);

    // We don't want to do this recursively.  May happen when a language is
    // not available and the SpellFileMissing autocommand opens a new buffer
    // in which 'spell' is set.
    જો (recursive)
	પાછા ખાલી;
    recursive = TRUE;

    ga_init2(&ga, કદ(langp_T), 2);
    clear_midword(wp);

    // Make a copy of 'spelllang', the SpellFileMissing autocommands may change
    // it under our fingers.
    spl_copy = vim_strsave(wp->w_s->b_p_spl);
    જો (spl_copy == ખાલી)
	જાઓ theend;

    wp->w_s->b_cjk = 0;

    // Loop over comma separated language names.
    માટે (splp = spl_copy; *splp != NUL; )
    {
	// Get one language name.
	len = copy_option_part(&splp, lang, MAXWLEN, ",");
	region = ખાલી;

	જો (!valid_spelllang(lang))
	    ચાલુ;

	જો (STRCMP(lang, "cjk") == 0)
	{
	    wp->w_s->b_cjk = 1;
	    ચાલુ;
	}

	// If the name ends in ".spl" use it as the name of the spell file.
	// If there is a region name let "region" point to it and remove it
	// from the name.
	જો (len > 4 && fnamecmp(lang + len - 4, ".spl") == 0)
	{
	    filename = TRUE;

	    // Locate a region and remove it from the file name.
	    p = vim_strchr(gettail(lang), '_');
	    જો (p != ખાલી && ASCII_ISALPHA(p[1]) && ASCII_ISALPHA(p[2])
						      && !ASCII_ISALPHA(p[3]))
	    {
		vim_strncpy(region_cp, p + 1, 2);
		mch_memmove(p, p + 3, len - (p - lang) - 2);
		region = region_cp;
	    }
	    નહીંતર
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		જો (fullpathcmp(lang, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
		    તોડો;
	}
	નહીંતર
	{
	    filename = FALSE;
	    જો (len > 3 && lang[len - 3] == '_')
	    {
		region = lang + len - 2;
		len -= 3;
		lang[len] = NUL;
	    }
	    નહીંતર
		dont_use_region = TRUE;

	    // Check if we loaded this language before.
	    FOR_ALL_SPELL_LANGS(slang)
		જો (STRICMP(lang, slang->sl_name) == 0)
		    તોડો;
	}

	જો (region != ખાલી)
	{
	    // If the region differs from what was used before then don't
	    // use it for 'spellfile'.
	    જો (use_region != ખાલી && STRCMP(region, use_region) != 0)
		dont_use_region = TRUE;
	    use_region = region;
	}

	// If not found try loading the language now.
	જો (slang == ખાલી)
	{
	    જો (filename)
		(શૂન્ય)spell_load_file(lang, lang, ખાલી, FALSE);
	    નહીંતર
	    {
		spell_load_lang(lang);
		// SpellFileMissing autocommands may do anything, including
		// destroying the buffer we are using or closing the window.
		જો (!bufref_valid(&bufref) || !win_valid_any_tab(wp))
		{
		    ret_સંદેશ = N_(e_spellfilemising_autocommand_deleted_buffer);
		    જાઓ theend;
		}
	    }
	}

	/*
	 * Loop over the languages, there can be several files for "lang".
	 */
	FOR_ALL_SPELL_LANGS(slang)
	    જો (filename ? fullpathcmp(lang, slang->sl_fname, FALSE, TRUE)
								    == FPC_SAME
			 : STRICMP(lang, slang->sl_name) == 0)
	    {
		region_mask = REGION_ALL;
		જો (!filename && region != ખાલી)
		{
		    // find region in sl_regions
		    c = find_region(slang->sl_regions, region);
		    જો (c == REGION_ALL)
		    {
			જો (slang->sl_add)
			{
			    જો (*slang->sl_regions != NUL)
				// This addition file is for other regions.
				region_mask = 0;
			}
			નહીંતર
			    // This is probably an error.  Give a warning and
			    // accept the words anyway.
			    smsg(_("Warning: region %s not supported"),
								      region);
		    }
		    નહીંતર
			region_mask = 1 << c;
		}

		જો (region_mask != 0)
		{
		    જો (ga_grow(&ga, 1) == FAIL)
		    {
			ga_clear(&ga);
			ret_સંદેશ = e_out_of_memory;
			જાઓ theend;
		    }
		    LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		    LANGP_ENTRY(ga, ga.ga_len)->lp_region = region_mask;
		    ++ga.ga_len;
		    use_midword(slang, wp);
		    જો (slang->sl_nobreak)
			nobreak = TRUE;
		}
	    }
    }

    // round 0: load int_wordlist, if possible.
    // round 1: load first name in 'spellfile'.
    // round 2: load second name in 'spellfile.
    // etc.
    spf = curwin->w_s->b_p_spf;
    માટે (round = 0; round == 0 || *spf != NUL; ++round)
    {
	જો (round == 0)
	{
	    // Internal wordlist, if there is one.
	    જો (int_wordlist == ખાલી)
		ચાલુ;
	    int_wordlist_spl(spf_name);
	}
	નહીંતર
	{
	    // One entry in 'spellfile'.
	    len = copy_option_part(&spf, spf_name, MAXPATHL - 4, ",");
	    STRCPY(spf_name + len, ".spl");

	    // If it was already found above then skip it.
	    માટે (c = 0; c < ga.ga_len; ++c)
	    {
		p = LANGP_ENTRY(ga, c)->lp_slang->sl_fname;
		જો (p != ખાલી && fullpathcmp(spf_name, p, FALSE, TRUE)
								== FPC_SAME)
		    તોડો;
	    }
	    જો (c < ga.ga_len)
		ચાલુ;
	}

	// Check if it was loaded already.
	FOR_ALL_SPELL_LANGS(slang)
	    જો (fullpathcmp(spf_name, slang->sl_fname, FALSE, TRUE)
								== FPC_SAME)
		તોડો;
	જો (slang == ખાલી)
	{
	    // Not loaded, try loading it now.  The language name includes the
	    // region name, the region is ignored otherwise.  for int_wordlist
	    // use an arbitrary name.
	    જો (round == 0)
		STRCPY(lang, "internal wordlist");
	    નહીંતર
	    {
		vim_strncpy(lang, gettail(spf_name), MAXWLEN);
		p = vim_strchr(lang, '.');
		જો (p != ખાલી)
		    *p = NUL;	// truncate at ".encoding.add"
	    }
	    slang = spell_load_file(spf_name, lang, ખાલી, TRUE);

	    // If one of the languages has NOBREAK we assume the addition
	    // files also have this.
	    જો (slang != ખાલી && nobreak)
		slang->sl_nobreak = TRUE;
	}
	જો (slang != ખાલી && ga_grow(&ga, 1) == OK)
	{
	    region_mask = REGION_ALL;
	    જો (use_region != ખાલી && !dont_use_region)
	    {
		// find region in sl_regions
		c = find_region(slang->sl_regions, use_region);
		જો (c != REGION_ALL)
		    region_mask = 1 << c;
		નહીંતર જો (*slang->sl_regions != NUL)
		    // This spell file is for other regions.
		    region_mask = 0;
	    }

	    જો (region_mask != 0)
	    {
		LANGP_ENTRY(ga, ga.ga_len)->lp_slang = slang;
		LANGP_ENTRY(ga, ga.ga_len)->lp_sallang = ખાલી;
		LANGP_ENTRY(ga, ga.ga_len)->lp_replang = ખાલી;
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
    માટે (i = 0; i < ga.ga_len; ++i)
    {
	lp = LANGP_ENTRY(ga, i);

	// sound folding
	જો (lp->lp_slang->sl_sal.ga_len > 0)
	    // language does sound folding itself
	    lp->lp_sallang = lp->lp_slang;
	નહીંતર
	    // find first similar language that does sound folding
	    માટે (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		જો (lp2->lp_slang->sl_sal.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_sallang = lp2->lp_slang;
		    તોડો;
		}
	    }

	// REP items
	જો (lp->lp_slang->sl_rep.ga_len > 0)
	    // language has REP items itself
	    lp->lp_replang = lp->lp_slang;
	નહીંતર
	    // find first similar language that has REP items
	    માટે (j = 0; j < ga.ga_len; ++j)
	    {
		lp2 = LANGP_ENTRY(ga, j);
		જો (lp2->lp_slang->sl_rep.ga_len > 0
			&& STRNCMP(lp->lp_slang->sl_name,
					      lp2->lp_slang->sl_name, 2) == 0)
		{
		    lp->lp_replang = lp2->lp_slang;
		    તોડો;
		}
	    }
    }
    redraw_win_later(wp, UPD_NOT_VALID);

theend:
    vim_free(spl_copy);
    recursive = FALSE;
    પાછા ret_સંદેશ;
}

/*
 * Clear the midword characters for buffer "buf".
 */
    સ્થિર શૂન્ય
clear_midword(win_T *wp)
{
    CLEAR_FIELD(wp->w_s->b_spell_ismw);
    VIM_CLEAR(wp->w_s->b_spell_ismw_mb);
}

/*
 * Use the "sl_midword" field of language "lp" for buffer "buf".
 * They add up to any currently used midword characters.
 */
    સ્થિર શૂન્ય
use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    જો (lp->sl_midword == ખાલી)	    // there aren't any
	પાછા;

    માટે (p = lp->sl_midword; *p != NUL; )
	જો (has_mbyte)
	{
	    પૂર્ણાંક	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    જો (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    નહીંતર જો (wp->w_s->b_spell_ismw_mb == ખાલી)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    નહીંતર
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (પૂર્ણાંક)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		જો (bp != ખાલી)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	નહીંતર
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}

/*
 * Find the region "region[2]" in "rp" (points to "sl_regions").
 * Each region is simply stored as the two characters of its name.
 * Returns the index if found (first is 0), REGION_ALL if not found.
 */
    સ્થિર પૂર્ણાંક
find_region(char_u *rp, char_u *region)
{
    પૂર્ણાંક		i;

    માટે (i = 0; ; i += 2)
    {
	જો (rp[i] == NUL)
	    પાછા REGION_ALL;
	જો (rp[i] == region[0] && rp[i + 1] == region[1])
	    તોડો;
    }
    પાછા i / 2;
}

/*
 * Return case type of word:
 * w word	0
 * Word		WF_ONECAP
 * W WORD	WF_ALLCAP
 * WoRd	wOrd	WF_KEEPCAP
 */
    પૂર્ણાંક
captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		firstcap;
    પૂર્ણાંક		allcap;
    પૂર્ણાંક		past_second = FALSE;	// past second word char

    // find first letter
    માટે (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	જો (end == ખાલી ? *p == NUL : p >= end)
	    પાછા 0;	    // only non-word characters, illegal word
    જો (has_mbyte)
	c = mb_ptr2char_adv(&p);
    નહીંતર
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    માટે ( ; end == ખાલી ? *p != NUL : p < end; MB_PTR_ADV(p))
	જો (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    જો (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		જો (past_second && allcap)
		    પાછા WF_KEEPCAP;
		allcap = FALSE;
	    }
	    નહીંતર જો (!allcap)
		// UlU -> KEEPCAP
		પાછા WF_KEEPCAP;
	    past_second = TRUE;
	}

    જો (allcap)
	પાછા WF_ALLCAP;
    જો (firstcap)
	પાછા WF_ONECAP;
    પાછા 0;
}

/*
 * Delete the internal wordlist and its .spl file.
 */
    શૂન્ય
spell_delete_wordlist(શૂન્ય)
{
    char_u	fname[MAXPATHL];

    જો (int_wordlist == ખાલી)
	પાછા;

    mch_remove(int_wordlist);
    int_wordlist_spl(fname);
    mch_remove(fname);
    VIM_CLEAR(int_wordlist);
}

/*
 * Free all languages.
 */
    શૂન્ય
spell_free_all(શૂન્ય)
{
    slang_T	*slang;
    buf_T	*buf;

    // Go through all buffers and handle 'spelllang'. <VN>
    FOR_ALL_BUFFERS(buf)
	ga_clear(&buf->b_s.b_langp);

    જ્યારે (first_lang != ખાલી)
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
    શૂન્ય
spell_reload(શૂન્ય)
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
	જો (*wp->w_s->b_p_spl != NUL)
	{
		જો (wp->w_p_spell)
		{
		    (શૂન્ય)parse_spelllang(wp);
		    તોડો;
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
open_spellbuf(શૂન્ય)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    જો (buf == ખાલી)
	પાછા ખાલી;

    buf->b_spell = true;
    buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
    buf->b_p_ચાવી = empty_option;
#endif
    ml_open(buf);
    ml_open_file(buf);	// create swap file now
    પાછા buf;
}

/*
 * Close the buffer used for spell info.
 */
    શૂન્ય
close_spellbuf(buf_T *buf)
{
    જો (buf == ખાલી)
	પાછા;

    ml_close(buf, TRUE);
    vim_free(buf);
}

/*
 * Init the chartab used for spelling for ASCII.
 */
    શૂન્ય
clear_spell_chartab(spelltab_T *sp)
{
    પૂર્ણાંક		i;

    // Init everything to FALSE (zero).
    CLEAR_FIELD(sp->st_isw);
    CLEAR_FIELD(sp->st_isu);
    માટે (i = 0; i < 256; ++i)
    {
	sp->st_fold[i] = i;
	sp->st_upper[i] = i;
    }

    // We include digits.  A word shouldn't start with a digit, but handling
    // that is done separately.
    માટે (i = '0'; i <= '9'; ++i)
	sp->st_isw[i] = TRUE;
    માટે (i = 'A'; i <= 'Z'; ++i)
    {
	sp->st_isw[i] = TRUE;
	sp->st_isu[i] = TRUE;
	sp->st_fold[i] = i + 0x20;
    }
    માટે (i = 'a'; i <= 'z'; ++i)
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
    શૂન્ય
init_spell_chartab(શૂન્ય)
{
    પૂર્ણાંક	    i;

    did_set_spelltab = FALSE;
    clear_spell_chartab(&spelltab);
    જો (enc_dbcs)
    {
	// DBCS: assume double-wide characters are word characters.
	માટે (i = 128; i <= 255; ++i)
	    જો (MB_BYTE2LEN(i) == 2)
		spelltab.st_isw[i] = TRUE;
    }
    નહીંતર જો (enc_utf8)
    {
	માટે (i = 128; i < 256; ++i)
	{
	    પૂર્ણાંક f = utf_fold(i);
	    પૂર્ણાંક u = utf_toupper(i);

	    spelltab.st_isu[i] = utf_isupper(i);
	    spelltab.st_isw[i] = spelltab.st_isu[i] || utf_islower(i);
	    // The folded/upper-cased value is different between latin1 and
	    // utf8 for 0xb5, causing E763 for no good reason.  Use the latin1
	    // value for utf-8 to avoid this.
	    spelltab.st_fold[i] = (f < 256) ? f : i;
	    spelltab.st_upper[i] = (u < 256) ? u : i;
	}
    }
    નહીંતર
    {
	// Rough guess: use locale-dependent library functions.
	માટે (i = 128; i < 256; ++i)
	{
	    જો (MB_ISUPPER(i))
	    {
		spelltab.st_isw[i] = TRUE;
		spelltab.st_isu[i] = TRUE;
		spelltab.st_fold[i] = MB_TOLOWER(i);
	    }
	    નહીંતર જો (MB_ISLOWER(i))
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
    પૂર્ણાંક
spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    પૂર્ણાંક		l;
    પૂર્ણાંક		c;

    જો (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	જો (l == 1)
	{
	    // be quick for ASCII
	    જો (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	નહીંતર
	{
	    c = mb_ptr2char(p);
	    જો (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != ખાલી
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != ખાલી))
		s = p + l;
	}

	c = mb_ptr2char(s);
	જો (c > 255)
	    પાછા spell_mb_isword_class(mb_get_class(s), wp);
	પાછા spelltab.st_isw[c];
    }

    પાછા spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}

/*
 * Return TRUE if "p" points to a word character.
 * Unlike spell_iswordp() this doesn't check for "midword" characters.
 */
    પૂર્ણાંક
spell_iswordp_nmw(char_u *p, win_T *wp)
{
    પૂર્ણાંક		c;

    જો (has_mbyte)
    {
	c = mb_ptr2char(p);
	જો (c > 255)
	    પાછા spell_mb_isword_class(mb_get_class(p), wp);
	પાછા spelltab.st_isw[c];
    }
    પાછા spelltab.st_isw[*p];
}

/*
 * Return TRUE if word class indicates a word character.
 * Only for characters above 255.
 * Unicode subscript and superscript are not considered word characters.
 * See also dbcs_class() and utf_class() in mbyte.c.
 */
    સ્થિર પૂર્ણાંક
spell_mb_isword_class(પૂર્ણાંક cl, win_T *wp)
{
    જો (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	પાછા cl == 2 || cl == 0x2800;
    પાછા cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}

/*
 * Return TRUE if "p" points to a word character.
 * Wide version of spell_iswordp().
 */
    સ્થિર પૂર્ણાંક
spell_iswordp_w(પૂર્ણાંક *p, win_T *wp)
{
    પૂર્ણાંક		*s;

    જો (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != ખાલી
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != ખાલી))
	s = p + 1;
    નહીંતર
	s = p;

    જો (*s > 255)
    {
	જો (enc_utf8)
	    પાછા spell_mb_isword_class(utf_class(*s), wp);
	જો (enc_dbcs)
	    પાછા spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	પાછા 0;
    }
    પાછા spelltab.st_isw[*s];
}

/*
 * Case-fold "str[len]" into "buf[buflen]".  The result is NUL terminated.
 * Uses the character definitions from the .spl file.
 * When using a multi-byte 'encoding' the length may change!
 * Returns FAIL when something wrong.
 */
    પૂર્ણાંક
spell_casefold(
    win_T	*wp,
    char_u	*str,
    પૂર્ણાંક		len,
    char_u	*buf,
    પૂર્ણાંક		buflen)
{
    પૂર્ણાંક		i;

    જો (len >= buflen)
    {
	buf[0] = NUL;
	પાછા FAIL;		// result will not fit
    }

    જો (has_mbyte)
    {
	પૂર્ણાંક	outi = 0;
	char_u	*p;
	પૂર્ણાંક	c;

	// Fold one character at a time.
	માટે (p = str; p < str + len; )
	{
	    જો (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		પાછા FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    જો (c == 0x03a3 || c == 0x03c2)
	    {
		જો (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		નહીંતર
		    c = 0x03c3;
	    }
	    નહીંતર
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    નહીંતર
    {
	// Be quick for non-multibyte encodings.
	માટે (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    પાછા OK;
}

/*
 * Check if the word at line "lnum" column "col" is required to start with a
 * capital.  This uses 'spellcapcheck' of the buffer in window "wp".
 */
    પૂર્ણાંક
check_need_cap(win_T *wp, linenr_T lnum, colnr_T col)
{
    જો (wp->w_s->b_cap_prog == ખાલી)
	પાછા FALSE;

    પૂર્ણાંક		need_cap = FALSE;
    char_u	*લીટી = col ? ml_get_buf(wp->w_buffer, lnum, FALSE) : ખાલી;
    char_u	*લીટી_copy = ખાલી;
    colnr_T	endcol = 0;

    જો (col == 0 || getwhitecols(લીટી) >= col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	જો (lnum == 1)
	    need_cap = TRUE;
	નહીંતર
	{
	    લીટી = ml_get_buf(wp->w_buffer, lnum - 1, FALSE);
	    જો (*skipwhite(લીટી) == NUL)
		need_cap = TRUE;
	    નહીંતર
	    {
		// Append a space in place of the line break.
		લીટી_copy = concat_str(લીટી, (char_u *)" ");
		જો (લીટી_copy == ખાલી)
		    પાછા FALSE;

		લીટી = લીટી_copy;
		endcol = (colnr_T)STRLEN(લીટી);
	    }
	}
    }
    નહીંતર
	endcol = col;

    જો (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch_T	regmatch;
	regmatch.regprog = wp->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	char_u *p = લીટી + endcol;
	માટે (;;)
	{
	    MB_PTR_BACK(લીટી, p);
	    જો (p == લીટી || spell_iswordp_nmw(p, wp))
		તોડો;
	    જો (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == લીટી + endcol)
	    {
		need_cap = TRUE;
		તોડો;
	    }
	}
	wp->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(લીટી_copy);

    પાછા need_cap;
}


/*
 * ":spellrepall"
 */
    શૂન્ય
ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    size_t	frompatlen;
    char_u	*લીટી;
    char_u	*p;
    પૂર્ણાંક		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    જો (repl_from == ખાલી || repl_to == ખાલી)
    {
	emsg(_(e_no_previous_spell_replacement));
	પાછા;
    }
    size_t	repl_from_len = STRLEN(repl_from);
    size_t	repl_to_len = STRLEN(repl_to);
    long	addlen = (long)repl_to_len - (long)repl_from_len;

    frompat = alloc(repl_from_len + 7);
    જો (frompat == ખાલી)
	પાછા;
    frompatlen = vim_snprintf((અક્ષર *)frompat, repl_from_len + 7, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    જ્યારે (!got_int)
    {
	જો (do_શોધ(ખાલી, '/', '/', frompat, frompatlen, 1L, શોધ_KEEP, ખાલી) == 0
						   || u_save_cursor() == FAIL)
	    તોડો;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	લીટી = ml_get_curline();
	જો (addlen <= 0 || STRNCMP(લીટી + curwin->w_cursor.col,
						   repl_to, repl_to_len) != 0)
	{
	    p = alloc(ml_get_curline_len() + addlen + 1);
	    જો (p == ખાલી)
		તોડો;
	    mch_memmove(p, લીટી, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, લીટી + curwin->w_cursor.col + repl_from_len);
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
#જો defined(FEAT_PROP_POPUP)
	    જો (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, (પૂર્ણાંક)addlen, APC_SUBSTITUTE);
#endif

	    જો (curwin->w_cursor.lnum != prev_lnum)
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

    જો (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    નહીંતર
	do_sub_સંદેશ(FALSE);
}

/*
 * Make a copy of "word", with the first letter upper or lower cased, to
 * "wcopy[MAXWLEN]".  "word" must not be empty.
 * The result is NUL terminated.
 */
    શૂન્ય
onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    પૂર્ણાંક		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		l;

    p = word;
    જો (has_mbyte)
	c = mb_cptr2char_adv(&p);
    નહીંતર
	c = *p++;
    જો (upper)
	c = SPELL_TOUPPER(c);
    નહીંતર
	c = SPELL_TOFOLD(c);
    જો (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    નહીંતર
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
    શૂન્ય
allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    પૂર્ણાંક		c;

    d = wcopy;
    માટે (s = word; *s != NUL; )
    {
	જો (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	નહીંતર
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	જો (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    જો (d - wcopy >= MAXWLEN - 1)
		તોડો;
	    *d++ = c;
	}
	નહીંતર
	    c = SPELL_TOUPPER(c);

	જો (has_mbyte)
	{
	    જો (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		તોડો;
	    d += mb_char2bytes(c, d);
	}
	નહીંતર
	{
	    જો (d - wcopy >= MAXWLEN - 1)
		તોડો;
	    *d++ = c;
	}
    }
    *d = NUL;
}

/*
 * Case-folding may change the number of bytes: Count nr of chars in
 * fword[flen] and return the byte length of that many chars in "word".
 */
    પૂર્ણાંક
nofold_len(char_u *fword, પૂર્ણાંક flen, char_u *word)
{
    char_u	*p;
    પૂર્ણાંક		i = 0;

    માટે (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    માટે (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    પાછા (પૂર્ણાંક)(p - word);
}

/*
 * Copy "fword" to "cword", fixing case according to "flags".
 */
    શૂન્ય
make_case_word(char_u *fword, char_u *cword, પૂર્ણાંક flags)
{
    જો (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    નહીંતર જો (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    નહીંતર
	// Use goodword as-is.
	STRCPY(cword, fword);
}

#જો defined(FEAT_EVAL)
/*
 * Soundfold a string, for soundfold().
 * Result is in allocated memory, NULL for an error.
 */
    char_u *
eval_soundfold(char_u *word)
{
    langp_T	*lp;
    char_u	sound[MAXWLEN];
    પૂર્ણાંક		lpi;

    જો (curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	// Use the sound-folding of the first language that supports it.
	માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
	{
	    lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	    જો (lp->lp_slang->sl_sal.ga_len > 0)
	    {
		// soundfold the word
		spell_soundfold(lp->lp_slang, word, FALSE, sound);
		પાછા vim_strsave(sound);
	    }
	}

    // No language with sound folding, return word as-is.
    પાછા vim_strsave(word);
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
    શૂન્ય
spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    પૂર્ણાંક		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    જો (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    નહીંતર
    {
	// SAL items used.  Requires the word to be case-folded.
	જો (folded)
	    word = inword;
	નહીંતર
	{
	    (શૂન્ય)spell_casefold(curwin,
				  inword, (પૂર્ણાંક)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	જો (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	નહીંતર
	    spell_soundfold_sal(slang, word, res);
    }
}

/*
 * Perform sound folding of "inword" into "res" according to SOFOFROM and
 * SOFOTO lines.
 */
    સ્થિર શૂન્ય
spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    પૂર્ણાંક		ri = 0;
    પૂર્ણાંક		c;

    જો (has_mbyte)
    {
	પૂર્ણાંક	prevc = 0;
	પૂર્ણાંક	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	માટે (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    જો (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    નહીંતર જો (c < 256)
		c = slang->sl_sal_first[c];
	    નહીંતર
	    {
		ip = ((પૂર્ણાંક **)slang->sl_sal.ga_data)[c & 0xff];
		જો (ip == ખાલી)		// empty list, can't match
		    c = NUL;
		નહીંતર
		    માટે (;;)		// find "c" in the list
		    {
			જો (*ip == 0)	// not found
			{
			    c = NUL;
			    તોડો;
			}
			જો (*ip == c)	// match!
			{
			    c = ip[1];
			    તોડો;
			}
			ip += 2;
		    }
	    }

	    જો (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		જો (ri + MB_MAXBYTES > MAXWLEN)
		    તોડો;
		prevc = c;
	    }
	}
    }
    નહીંતર
    {
	// The sl_sal_first[] table contains the translation.
	માટે (s = inword; (c = *s) != NUL && ri < MAXWLEN - 1; ++s)
	{
	    જો (VIM_ISWHITE(c))
		c = ' ';
	    નહીંતર
		c = slang->sl_sal_first[c];
	    જો (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}

    સ્થિર શૂન્ય
spell_soundfold_sal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp;
    char_u	word[MAXWLEN];
    char_u	*s = inword;
    char_u	*t;
    char_u	*pf;
    પૂર્ણાંક		i, j, z;
    પૂર્ણાંક		reslen;
    પૂર્ણાંક		n, k = 0;
    પૂર્ણાંક		z0;
    પૂર્ણાંક		k0;
    પૂર્ણાંક		n0;
    પૂર્ણાંક		c;
    પૂર્ણાંક		pri;
    પૂર્ણાંક		p0 = -333;
    પૂર્ણાંક		c0;

    // Remove accents, if wanted.  We actually remove all non-word characters.
    // But keep white space.  We need a copy, the word may be changed here.
    જો (slang->sl_rem_accents)
    {
	t = word;
	જ્યારે (*s != NUL)
	{
	    જો (VIM_ISWHITE(*s))
	    {
		*t++ = ' ';
		s = skipwhite(s);
	    }
	    નહીંતર
	    {
		જો (spell_iswordp_nmw(s, curwin))
		    *t++ = *s;
		++s;
	    }
	}
	*t = NUL;
    }
    નહીંતર
	vim_strncpy(word, s, MAXWLEN - 1);

    smp = (salitem_T *)slang->sl_sal.ga_data;

    /*
     * This comes from Aspell phonet.cpp.  Converted from C++ to C.
     * Changed to keep spaces.
     */
    i = reslen = z = 0;
    જ્યારે ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c];
	z0 = 0;

	જો (n >= 0)
	{
	    // check all rules for the same letter
	    માટે (; n < slang->sl_sal.ga_len && (s = smp[n].sm_lead)[0] == c; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		k = smp[n].sm_leadlen;
		જો (k > 1)
		{
		    જો (word[i + 1] != s[1])
			ચાલુ;
		    જો (k > 2)
		    {
			માટે (j = 2; j < k; ++j)
			    જો (word[i + j] != s[j])
				તોડો;
			જો (j < k)
			    ચાલુ;
		    }
		}

		જો ((pf = smp[n].sm_oneof) != ખાલી)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    જ્યારે (*pf != NUL && *pf != word[i + k])
			++pf;
		    જો (*pf == NUL)
			ચાલુ;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		જ્યારે (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		જો (*s == '<')
		    s++;
		જો (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		જો (*s == '^' && *(s + 1) == '^')
		    s++;

		જો (*s == NUL
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

		    જો (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// test follow-up rule for "word[i + k]"
			માટે ( ; (s = smp[n0].sm_lead)[0] == c0; ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    //
			    k0 = smp[n0].sm_leadlen;
			    જો (k0 > 1)
			    {
				જો (word[i + k] != s[1])
				    ચાલુ;
				જો (k0 > 2)
				{
				    pf = word + i + k + 1;
				    માટે (j = 2; j < k0; ++j)
					જો (*pf++ != s[j])
					    તોડો;
				    જો (j < k0)
					ચાલુ;
				}
			    }
			    k0 += k - 1;

			    જો ((pf = smp[n0].sm_oneof) != ખાલી)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				જ્યારે (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				જો (*pf == NUL)
				    ચાલુ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    જ્યારે (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    જો (*s == '<')
				s++;
			    જો (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    જો (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					    && !spell_iswordp(word + i + k0,
								     curwin)))
			    {
				જો (k0 == k)
				    // this is just a piece of the string
				    ચાલુ;

				જો (p0 < pri)
				    // priority too low
				    ચાલુ;
				// rule fits; stop search
				તોડો;
			    }
			}

			જો (p0 >= pri && smp[n0].sm_lead[0] == c0)
			    ચાલુ;
		    }

		    // replace string
		    s = smp[n].sm_to;
		    જો (s == ખાલી)
			s = (char_u *)"";
		    pf = smp[n].sm_rules;
		    p0 = (vim_strchr(pf, '<') != ખાલી) ? 1 : 0;
		    જો (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			જો (reslen > 0 && *s != NUL && (res[reslen - 1] == c
						    || res[reslen - 1] == *s))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			જ્યારે (*s != NUL && word[i + k0] != NUL)
			{
			    word[i + k0] = *s;
			    k0++;
			    s++;
			}
			જો (k > k0)
			    STRMOVE(word + i + k0, word + i + k);

			// new "actual letter"
			c = word[i];
		    }
		    નહીંતર
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			જ્યારે (*s != NUL && s[1] != NUL && reslen < MAXWLEN - 1)
			{
			    જો (reslen == 0 || res[reslen - 1] != *s)
				res[reslen++] = *s;
			    s++;
			}
			// new "actual letter"
			c = *s;
			જો (strstr((અક્ષર *)pf, "^^") != ખાલી)
			{
			    જો (c != NUL && reslen < MAXWLEN - 1)
				res[reslen++] = c;
			    STRMOVE(word, word + i + 1);
			    i = 0;
			    z0 = 1;
			}
		    }
		    તોડો;
		}
	    }
	}
	નહીંતર જો (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	જો (z0 == 0)
	{
	    જો (k && !p0 && reslen < MAXWLEN - 1 && c != NUL
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
    સ્થિર શૂન્ય
spell_soundfold_wsal(slang_T *slang, char_u *inword, char_u *res)
{
    salitem_T	*smp = (salitem_T *)slang->sl_sal.ga_data;
    પૂર્ણાંક		word[MAXWLEN];
    પૂર્ણાંક		wres[MAXWLEN];
    પૂર્ણાંક		l;
    char_u	*s;
    પૂર્ણાંક		*ws;
    char_u	*t;
    પૂર્ણાંક		*pf;
    પૂર્ણાંક		i, j, z;
    પૂર્ણાંક		reslen;
    પૂર્ણાંક		n, k = 0;
    પૂર્ણાંક		z0;
    પૂર્ણાંક		k0;
    પૂર્ણાંક		n0;
    પૂર્ણાંક		c;
    પૂર્ણાંક		pri;
    પૂર્ણાંક		p0 = -333;
    પૂર્ણાંક		c0;
    પૂર્ણાંક		did_white = FALSE;
    પૂર્ણાંક		wordlen;


    /*
     * Convert the multi-byte string to a wide-character string.
     * Remove accents, if wanted.  We actually remove all non-word characters.
     * But keep white space.
     */
    wordlen = 0;
    માટે (s = inword; *s != NUL; )
    {
	t = s;
	c = mb_cptr2char_adv(&s);
	જો (slang->sl_rem_accents)
	{
	    જો (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
	    {
		જો (did_white)
		    ચાલુ;
		c = ' ';
		did_white = TRUE;
	    }
	    નહીંતર
	    {
		did_white = FALSE;
		જો (!spell_iswordp_nmw(t, curwin))
		    ચાલુ;
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
    જ્યારે ((c = word[i]) != NUL)
    {
	// Start with the first rule that has the character in the word.
	n = slang->sl_sal_first[c & 0xff];
	z0 = 0;

	જો (n >= 0)
	{
	    // Check all rules for the same index byte.
	    // If c is 0x300 need extra check for the end of the array, as
	    // (c & 0xff) is NUL.
	    માટે (; n < slang->sl_sal.ga_len
			&& ((ws = smp[n].sm_lead_w)[0] & 0xff) == (c & 0xff)
							 && ws[0] != NUL; ++n)
	    {
		// Quickly skip entries that don't match the word.  Most
		// entries are less than three chars, optimize for that.
		જો (c != ws[0])
		    ચાલુ;
		k = smp[n].sm_leadlen;
		જો (k > 1)
		{
		    જો (word[i + 1] != ws[1])
			ચાલુ;
		    જો (k > 2)
		    {
			માટે (j = 2; j < k; ++j)
			    જો (word[i + j] != ws[j])
				તોડો;
			જો (j < k)
			    ચાલુ;
		    }
		}

		જો ((pf = smp[n].sm_oneof_w) != ખાલી)
		{
		    // Check for match with one of the chars in "sm_oneof".
		    જ્યારે (*pf != NUL && *pf != word[i + k])
			++pf;
		    જો (*pf == NUL)
			ચાલુ;
		    ++k;
		}
		s = smp[n].sm_rules;
		pri = 5;    // default priority

		p0 = *s;
		k0 = k;
		જ્યારે (*s == '-' && k > 1)
		{
		    k--;
		    s++;
		}
		જો (*s == '<')
		    s++;
		જો (VIM_ISDIGIT(*s))
		{
		    // determine priority
		    pri = *s - '0';
		    s++;
		}
		જો (*s == '^' && *(s + 1) == '^')
		    s++;

		જો (*s == NUL
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

		    જો (slang->sl_followup && k > 1 && n0 >= 0
					   && p0 != '-' && word[i + k] != NUL)
		    {
			// Test follow-up rule for "word[i + k]"; loop over
			// all entries with the same index byte.
			માટે ( ; ((ws = smp[n0].sm_lead_w)[0] & 0xff)
							 == (c0 & 0xff); ++n0)
			{
			    // Quickly skip entries that don't match the word.
			    જો (c0 != ws[0])
				ચાલુ;
			    k0 = smp[n0].sm_leadlen;
			    જો (k0 > 1)
			    {
				જો (word[i + k] != ws[1])
				    ચાલુ;
				જો (k0 > 2)
				{
				    pf = word + i + k + 1;
				    માટે (j = 2; j < k0; ++j)
					જો (*pf++ != ws[j])
					    તોડો;
				    જો (j < k0)
					ચાલુ;
				}
			    }
			    k0 += k - 1;

			    જો ((pf = smp[n0].sm_oneof_w) != ખાલી)
			    {
				// Check for match with one of the chars in
				// "sm_oneof".
				જ્યારે (*pf != NUL && *pf != word[i + k0])
				    ++pf;
				જો (*pf == NUL)
				    ચાલુ;
				++k0;
			    }

			    p0 = 5;
			    s = smp[n0].sm_rules;
			    જ્યારે (*s == '-')
			    {
				// "k0" gets NOT reduced because
				// "if (k0 == k)"
				s++;
			    }
			    જો (*s == '<')
				s++;
			    જો (VIM_ISDIGIT(*s))
			    {
				p0 = *s - '0';
				s++;
			    }

			    જો (*s == NUL
				    // *s == '^' cuts
				    || (*s == '$'
					 && !spell_iswordp_w(word + i + k0,
								     curwin)))
			    {
				જો (k0 == k)
				    // this is just a piece of the string
				    ચાલુ;

				જો (p0 < pri)
				    // priority too low
				    ચાલુ;
				// rule fits; stop search
				તોડો;
			    }
			}

			જો (p0 >= pri && (smp[n0].sm_lead_w[0] & 0xff)
							       == (c0 & 0xff))
			    ચાલુ;
		    }

		    // replace string
		    ws = smp[n].sm_to_w;
		    s = smp[n].sm_rules;
		    p0 = (vim_strchr(s, '<') != ખાલી) ? 1 : 0;
		    જો (p0 == 1 && z == 0)
		    {
			// rule with '<' is used
			જો (reslen > 0 && ws != ખાલી && *ws != NUL
				&& (wres[reslen - 1] == c
						    || wres[reslen - 1] == *ws))
			    reslen--;
			z0 = 1;
			z = 1;
			k0 = 0;
			જો (ws != ખાલી)
			    જ્યારે (*ws != NUL && word[i + k0] != NUL)
			    {
				word[i + k0] = *ws;
				k0++;
				ws++;
			    }
			જો (k > k0)
			    mch_memmove(word + i + k0, word + i + k,
				    કદ(પૂર્ણાંક) * (wordlen - (i + k) + 1));

			// new "actual letter"
			c = word[i];
		    }
		    નહીંતર
		    {
			// no '<' rule used
			i += k - 1;
			z = 0;
			જો (ws != ખાલી)
			    જ્યારે (*ws != NUL && ws[1] != NUL
							  && reslen < MAXWLEN)
			    {
				જો (reslen == 0 || wres[reslen - 1] != *ws)
				    wres[reslen++] = *ws;
				ws++;
			    }
			// new "actual letter"
			જો (ws == ખાલી)
			    c = NUL;
			નહીંતર
			    c = *ws;
			જો (strstr((અક્ષર *)s, "^^") != ખાલી)
			{
			    જો (c != NUL && reslen < MAXWLEN)
				wres[reslen++] = c;
			    mch_memmove(word, word + i + 1,
				       કદ(પૂર્ણાંક) * (wordlen - (i + 1) + 1));
			    i = 0;
			    z0 = 1;
			}
		    }
		    તોડો;
		}
	    }
	}
	નહીંતર જો (VIM_ISWHITE(c))
	{
	    c = ' ';
	    k = 1;
	}

	જો (z0 == 0)
	{
	    જો (k && !p0 && reslen < MAXWLEN && c != NUL
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
    માટે (n = 0; n < reslen; ++n)
    {
	l += mb_char2bytes(wres[n], res + l);
	જો (l + MB_MAXBYTES > MAXWLEN)
	    તોડો;
    }
    res[l] = NUL;
}

/*
 * ":spellinfo"
 */
    શૂન્ય
ex_spellinfo(exarg_T *eap UNUSED)
{
    પૂર્ણાંક		lpi;
    langp_T	*lp;
    char_u	*p;

    જો (no_spell_checking(curwin))
	પાછા;

    સંદેશ_start();
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len && !got_int; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	સંદેશ_puts("file: ");
	સંદેશ_puts((અક્ષર *)lp->lp_slang->sl_fname);
	સંદેશ_putchar('\n');
	p = lp->lp_slang->sl_info;
	જો (p != ખાલી)
	{
	    સંદેશ_puts((અક્ષર *)p);
	    સંદેશ_putchar('\n');
	}
    }
    સંદેશ_end();
}

#define DUMPFLAG_KEEPCASE   1	// round 2: keep-case tree
#define DUMPFLAG_ગણતરી	    2	// include word count
#define DUMPFLAG_ICASE	    4	// ignore case when finding matches
#define DUMPFLAG_ONECAP	    8	// pattern starts with capital
#define DUMPFLAG_ALLCAP	    16	// pattern is all capitals

/*
 * ":spelldump"
 */
    શૂન્ય
ex_spelldump(exarg_T *eap)
{
    char_u  *spl;
    long    dummy;

    જો (no_spell_checking(curwin))
	પાછા;
    (શૂન્ય)get_option_કિંમત((char_u*)"spl", &dummy, &spl, ખાલી, OPT_LOCAL);

    // Create a new empty buffer in a new window.
    do_cmdline_cmd((char_u *)"new");

    // enable spelling locally in the new window
    set_option_કિંમત_give_err((char_u*)"spell", TRUE, (char_u*)"", OPT_LOCAL);
    set_option_કિંમત_give_err((char_u*)"spl",  dummy, spl, OPT_LOCAL);
    vim_free(spl);

    જો (!BUFEMPTY())
	પાછા;

    spell_dump_compl(ખાલી, 0, ખાલી, eap->forceit ? DUMPFLAG_ગણતરી : 0);

    // Delete the empty line that we started with.
    જો (curbuf->b_ml.ml_લીટી_ગણતરી > 1)
	ml_delete(curbuf->b_ml.ml_લીટી_ગણતરી);

    redraw_later(UPD_NOT_VALID);
}

/*
 * Go through all possible words and:
 * 1. When "pat" is NULL: dump a list of all words in the current buffer.
 *	"ic" and "dir" are not used.
 * 2. When "pat" is not NULL: add matching words to insert mode completion.
 */
    શૂન્ય
spell_dump_compl(
    char_u	*pat,	    // leading part of the word
    પૂર્ણાંક		ic,	    // ignore case
    પૂર્ણાંક		*dir,	    // direction for adding matches
    પૂર્ણાંક		dumpflags_arg)	// DUMPFLAG_*
{
    langp_T	*lp;
    slang_T	*slang;
    idx_T	arridx[MAXWLEN];
    પૂર્ણાંક		curi[MAXWLEN];
    char_u	word[MAXWLEN];
    પૂર્ણાંક		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = 0;
    પૂર્ણાંક		round;
    પૂર્ણાંક		depth;
    પૂર્ણાંક		n;
    પૂર્ણાંક		flags;
    char_u	*region_names = ખાલી;	    // region names being used
    પૂર્ણાંક		do_region = TRUE;	    // dump region names and numbers
    char_u	*p;
    પૂર્ણાંક		lpi;
    પૂર્ણાંક		dumpflags = dumpflags_arg;
    પૂર્ણાંક		patlen;

    // When ignoring case or when the pattern starts with capital pass this on
    // to dump_word().
    જો (pat != ખાલી)
    {
	જો (ic)
	    dumpflags |= DUMPFLAG_ICASE;
	નહીંતર
	{
	    n = captype(pat, ખાલી);
	    જો (n == WF_ONECAP)
		dumpflags |= DUMPFLAG_ONECAP;
	    નહીંતર જો (n == WF_ALLCAP && (પૂર્ણાંક)STRLEN(pat) > mb_ptr2len(pat))
		dumpflags |= DUMPFLAG_ALLCAP;
	}
    }

    // Find out if we can support regions: All languages must support the same
    // regions or none at all.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	p = lp->lp_slang->sl_regions;
	જો (p[0] != 0)
	{
	    જો (region_names == ખાલી)	    // first language with regions
		region_names = p;
	    નહીંતર જો (STRCMP(region_names, p) != 0)
	    {
		do_region = FALSE;	    // region names are different
		તોડો;
	    }
	}
    }

    જો (do_region && region_names != ખાલી && pat == ખાલી)
    {
	vim_snprintf((અક્ષર *)IObuff, IOSIZE, "/regions=%s", region_names);
	ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
    }
    નહીંતર
	do_region = FALSE;

    /*
     * Loop over all files loaded for the entries in 'spelllang'.
     */
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	જો (slang->sl_fbyts == ખાલી)	    // reloading failed
	    ચાલુ;

	જો (pat == ખાલી)
	{
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "# file: %s", slang->sl_fname);
	    ml_append(lnum++, IObuff, (colnr_T)0, FALSE);
	}

	// When matching with a pattern and there are no prefixes only use
	// parts of the tree that match "pat".
	જો (pat != ખાલી && slang->sl_pbyts == ખાલી)
	    patlen = (પૂર્ણાંક)STRLEN(pat);
	નહીંતર
	    patlen = -1;

	// round 1: case-folded tree
	// round 2: keep-case tree
	માટે (round = 1; round <= 2; ++round)
	{
	    જો (round == 1)
	    {
		dumpflags &= ~DUMPFLAG_KEEPCASE;
		byts = slang->sl_fbyts;
		idxs = slang->sl_fidxs;
	    }
	    નહીંતર
	    {
		dumpflags |= DUMPFLAG_KEEPCASE;
		byts = slang->sl_kbyts;
		idxs = slang->sl_kidxs;
	    }
	    જો (byts == ખાલી)
		ચાલુ;		// array is empty

	    depth = 0;
	    arridx[0] = 0;
	    curi[0] = 1;
	    જ્યારે (depth >= 0 && !got_int
				  && (pat == ખાલી || !ins_compl_interrupted()))
	    {
		જો (curi[depth] > byts[arridx[depth]])
		{
		    // Done all bytes at this node, go up one level.
		    --depth;
		    લીટી_breakcheck();
		    ins_compl_check_ચાવીઓ(50, FALSE);
		}
		નહીંતર
		{
		    // Do one more byte at this node.
		    n = arridx[depth] + curi[depth];
		    ++curi[depth];
		    c = byts[n];
		    જો (c == 0 || depth >= MAXWLEN - 1)
		    {
			// End of word or reached maximum length, deal with the
			// word.
			// Don't use keep-case words in the fold-case tree,
			// they will appear in the keep-case tree.
			// Only use the word when the region matches.
			flags = (પૂર્ણાંક)idxs[n];
			જો ((round == 2 || (flags & WF_KEEPCAP) == 0)
				&& (flags & WF_NEEDCOMP) == 0
				&& (do_region
				    || (flags & WF_REGION) == 0
				    || (((unsigned)flags >> 16)
						       & lp->lp_region) != 0))
			{
			    word[depth] = NUL;
			    જો (!do_region)
				flags &= ~WF_REGION;

			    // Dump the basic word if there is no prefix or
			    // when it's the first one.
			    c = (unsigned)flags >> 24;
			    જો (c == 0 || curi[depth] == 2)
			    {
				dump_word(slang, word, pat, dir,
						      dumpflags, flags, lnum);
				જો (pat == ખાલી)
				    ++lnum;
			    }

			    // Apply the prefix, if there is one.
			    જો (c != 0)
				lnum = dump_prefixes(slang, word, pat, dir,
						      dumpflags, flags, lnum);
			}
		    }
		    નહીંતર
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
			જો (depth <= patlen
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
    સ્થિર શૂન્ય
dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    પૂર્ણાંક		*dir,
    પૂર્ણાંક		dumpflags,
    પૂર્ણાંક		wordflags,
    linenr_T	lnum)
{
    પૂર્ણાંક		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    પૂર્ણાંક		i;
    પૂર્ણાંક		flags = wordflags;

    જો (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    જો (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    જો ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    નહીંતર
    {
	p = word;
	જો ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, ખાલી) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    જો (pat == ખાલી)
    {
	// Add flags and regions after a slash.
	જો ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    જો (keepcap)
		STRCAT(badword, "=");
	    જો (flags & WF_BANNED)
		STRCAT(badword, "!");
	    નહીંતર જો (flags & WF_RARE)
		STRCAT(badword, "?");
	    જો (flags & WF_REGION)
		માટે (i = 0; i < 7; ++i)
		    જો (flags & (0x10000 << i))
			sprintf((અક્ષર *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	જો (dumpflags & DUMPFLAG_ગણતરી)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    જો (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((અક્ષર *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_ગણતરી);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    નહીંતર જો (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (પૂર્ણાંક)STRLEN(p),
					  p_ic, ખાલી, *dir, FALSE, 0) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}

/*
 * For ":spelldump": Find matching prefixes for "word".  Prepend each to
 * "word" and append a line to the buffer.
 * When "lnum" is zero add insert mode completion.
 * Return the updated line number.
 */
    સ્થિર linenr_T
dump_prefixes(
    slang_T	*slang,
    char_u	*word,	    // case-folded word
    char_u	*pat,
    પૂર્ણાંક		*dir,
    પૂર્ણાંક		dumpflags,
    પૂર્ણાંક		flags,	    // flags with prefix ID
    linenr_T	startlnum)
{
    idx_T	arridx[MAXWLEN];
    પૂર્ણાંક		curi[MAXWLEN];
    char_u	prefix[MAXWLEN];
    char_u	word_up[MAXWLEN];
    પૂર્ણાંક		has_word_up = FALSE;
    પૂર્ણાંક		c;
    char_u	*byts;
    idx_T	*idxs;
    linenr_T	lnum = startlnum;
    પૂર્ણાંક		depth;
    પૂર્ણાંક		n;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;

    // If the word starts with a lower-case letter make the word with an
    // upper-case letter in word_up[].
    c = PTR2CHAR(word);
    જો (SPELL_TOUPPER(c) != c)
    {
	onecap_copy(word, word_up, TRUE);
	has_word_up = TRUE;
    }

    byts = slang->sl_pbyts;
    idxs = slang->sl_pidxs;
    જો (byts != ખાલી)		// array not is empty
    {
	/*
	 * Loop over all prefixes, building them byte-by-byte in prefix[].
	 * When at the end of a prefix check that it supports "flags".
	 */
	depth = 0;
	arridx[0] = 0;
	curi[0] = 1;
	જ્યારે (depth >= 0 && !got_int)
	{
	    n = arridx[depth];
	    len = byts[n];
	    જો (curi[depth] > len)
	    {
		// Done all bytes at this node, go up one level.
		--depth;
		લીટી_breakcheck();
	    }
	    નહીંતર
	    {
		// Do one more byte at this node.
		n += curi[depth];
		++curi[depth];
		c = byts[n];
		જો (c == 0)
		{
		    // End of prefix, find out how many IDs there are.
		    માટે (i = 1; i < len; ++i)
			જો (byts[n + i] != 0)
			    તોડો;
		    curi[depth] += i - 1;

		    c = valid_word_prefix(i, n, flags, word, slang, FALSE);
		    જો (c != 0)
		    {
			vim_strncpy(prefix + depth, word, MAXWLEN - depth - 1);
			dump_word(slang, prefix, pat, dir, dumpflags,
				(c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			જો (lnum != 0)
			    ++lnum;
		    }

		    // Check for prefix that matches the word when the
		    // first letter is upper-case, but only if the prefix has
		    // a condition.
		    જો (has_word_up)
		    {
			c = valid_word_prefix(i, n, flags, word_up, slang,
									TRUE);
			જો (c != 0)
			{
			    vim_strncpy(prefix + depth, word_up,
							 MAXWLEN - depth - 1);
			    dump_word(slang, prefix, pat, dir, dumpflags,
				    (c & WF_RAREPFX) ? (flags | WF_RARE)
							       : flags, lnum);
			    જો (lnum != 0)
				++lnum;
			}
		    }
		}
		નહીંતર જો (depth < MAXWLEN - 1)
		{
		    // Normal char, go one level deeper.
		    prefix[depth++] = c;
		    arridx[depth] = idxs[n];
		    curi[depth] = 1;
		}
	    }
	}
    }

    પાછા lnum;
}

/*
 * Move "p" to the end of word "start".
 * Uses the spell-checking word characters.
 */
    char_u *
spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    જ્યારે (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    પાછા p;
}

/*
 * For Insert mode completion CTRL-X s:
 * Find start of the word in front of column "startcol".
 * We don't check if it is badly spelled, with completion we can only change
 * the word in front of the cursor.
 * Returns the column number of the word.
 */
    પૂર્ણાંક
spell_word_start(પૂર્ણાંક startcol)
{
    char_u	*લીટી;
    char_u	*p;
    પૂર્ણાંક		col = 0;

    જો (no_spell_checking(curwin))
	પાછા startcol;

    // Find a word character before "startcol".
    લીટી = ml_get_curline();
    માટે (p = લીટી + startcol; p > લીટી; )
    {
	MB_PTR_BACK(લીટી, p);
	જો (spell_iswordp_nmw(p, curwin))
	    તોડો;
    }

    // Go back to start of the word.
    જ્યારે (p > લીટી)
    {
	col = (પૂર્ણાંક)(p - લીટી);
	MB_PTR_BACK(લીટી, p);
	જો (!spell_iswordp(p, curwin))
	    તોડો;
	col = 0;
    }

    પાછા col;
}

/*
 * Need to check for 'spellcapcheck' now, the word is removed before
 * expand_spelling() is called.  Therefore the ugly global variable.
 */
સ્થિર પૂર્ણાંક spell_expand_need_cap;

    શૂન્ય
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
    પૂર્ણાંક
expand_spelling(
    linenr_T	lnum UNUSED,
    char_u	*pat,
    char_u	***matchp)
{
    garray_T	ga;

    spell_suggest_list(&ga, pat, 100, spell_expand_need_cap, TRUE);
    *matchp = ga.ga_data;
    પાછા ga.ga_len;
}

/*
 * Return TRUE if "val" is a valid 'spelllang' value.
 */
    પૂર્ણાંક
valid_spelllang(char_u *val)
{
    પાછા valid_name(val, ".-_,@");
}

/*
 * Return TRUE if "val" is a valid 'spellfile' value.
 */
    પૂર્ણાંક
valid_spellfile(char_u *val)
{
    char_u	spf_name[MAXPATHL];
    char_u	*spf;
    char_u	*s;
    પૂર્ણાંક		l;

    spf = val;
    જ્યારે (*spf != NUL)
    {
	l = copy_option_part(&spf, spf_name, MAXPATHL, ",");
	જો (l >= MAXPATHL - 4 || l < 4
				  || STRCMP(spf_name + l - 4, ".add") != 0)
	    પાછા FALSE;
	માટે (s = spf_name; *s != NUL; ++s)
	    જો (!vim_is_fname_char(*s))
		પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Handle side effects of setting 'spell' or 'spellfile'
 * Return an error message or NULL for success.
 */
    અક્ષર *
did_set_spell_option(શૂન્ય)
{
    અક્ષર    *errmsg = ખાલી;
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_buffer == curbuf && wp->w_p_spell)
	{
	    errmsg = parse_spelllang(wp);
	    તોડો;
	}
    પાછા errmsg;
}

/*
 * Set curbuf->b_cap_prog to the regexp program for 'spellcapcheck'.
 * Return error message when failed, NULL when OK.
 */
    અક્ષર *
compile_cap_prog(synblock_T *synblock)
{
    regprog_T   *rp = synblock->b_cap_prog;
    char_u	*re;

    જો (synblock->b_p_spc == ખાલી || *synblock->b_p_spc == NUL)
	synblock->b_cap_prog = ખાલી;
    નહીંતર
    {
	// Prepend a ^ so that we only match at one column
	re = concat_str((char_u *)"^", synblock->b_p_spc);
	જો (re != ખાલી)
	{
	    synblock->b_cap_prog = vim_regcomp(re, RE_MAGIC);
	    vim_free(re);
	    જો (synblock->b_cap_prog == ખાલી)
	    {
		synblock->b_cap_prog = rp; // restore the previous program
		પાછા e_invalid_argument;
	    }
	}
    }

    vim_regfree(rp);
    પાછા ખાલી;
}

#endif  // FEAT_SPELL
