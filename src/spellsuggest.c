/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * spellsuggest.c: functions for spelling suggestions
 */

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_SPELL)

/*
 * Use this to adjust the score after finding suggestions, based on the
 * suggested word sounding like the bad word.  This is much faster than doing
 * it for every possible suggestion.
 * Disadvantage: When "the" is typed as "hte" it sounds quite different ("@"
 * vs "ht") and goes down in the list.
 * Used when 'spellsuggest' is set to "best".
 */
#define RESCORE(word_score, sound_score) ((3 * (word_score) + (sound_score)) / 4)

/*
 * Do the opposite: based on a maximum end score and a known sound score,
 * compute the maximum word score that can be used.
 */
#define MAXSCORE(word_score, sound_score) ((4 * (word_score) - (sound_score)) / 3)

// only used for su_badflags
#define WF_MIXCAP   0x20	// mix of upper and lower case: macaRONI

/*
 * Information used when looking for suggestions.
 */
প্রকারনির্দেশ সংরচনা suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    পূর্ণাঙ্ক		su_maxcount;	    // max. number of suggestions displayed
    পূর্ণাঙ্ক		su_maxscore;	    // maximum score for adding to su_ga
    পূর্ণাঙ্ক		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    পূর্ণাঙ্ক		su_badlen;	    // length of detected bad word in line
    পূর্ণাঙ্ক		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
প্রকারনির্দেশ সংরচনা suggest_S
{
    char_u	*st_word;	// suggested word, allocated string
    পূর্ণাঙ্ক		st_wordlen;	// STRLEN(st_word)
    পূর্ণাঙ্ক		st_orglen;	// length of replaced text
    পূর্ণাঙ্ক		st_score;	// lower is better
    পূর্ণাঙ্ক		st_altscore;	// used when st_score compares equal
    পূর্ণাঙ্ক		st_salscore;	// st_score is for soundalike
    পূর্ণাঙ্ক		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#define SUG(ga, i) (((suggest_T *)(ga).ga_data)[i])

// TRUE if a word appears in the list of banned words.
#define WAS_BANNED(su, word) (!HASHITEM_EMPTY(hash_find(&(su)->su_banned, word)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#define SUG_CLEAN_গুনতি(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#define SUG_MAX_গুনতি(su)	(SUG_CLEAN_গুনতি(su) + 50)

// score for various changes
#define SCORE_SPLIT	149	// split bad word
#define SCORE_SPLIT_NO	249	// split bad word with NOSPLITSUGS
#define SCORE_ICASE	52	// slightly different case
#define SCORE_REGION	200	// word is for different region
#define SCORE_RARE	180	// rare word
#define SCORE_SWAP	75	// swap two characters
#define SCORE_SWAP3	110	// swap two characters in three
#define SCORE_REP	65	// REP replacement
#define SCORE_SUBST	93	// substitute a character
#define SCORE_SIMILAR	33	// substitute a similar character
#define SCORE_SUBCOMP	33	// substitute a composing character
#define SCORE_DEL	94	// delete a character
#define SCORE_DELDUP	66	// delete a duplicated character
#define SCORE_DELCOMP	28	// delete a composing character
#define SCORE_INS	96	// insert a character
#define SCORE_INSDUP	67	// insert a duplicate character
#define SCORE_INSCOMP	30	// insert a composing character
#define SCORE_NONWORD	103	// change non-word to word char

#define SCORE_FILE	30	// suggestion from a file
#define SCORE_MAXINIT	350	// Initial maximum score: higher == slower.
				// 350 allows for about three changes.

#define SCORE_COMMON1	30	// subtracted for words seen before
#define SCORE_COMMON2	40	// subtracted for words often seen
#define SCORE_COMMON3	50	// subtracted for words very often seen
#define SCORE_THRES2	10	// word count threshold for COMMON2
#define SCORE_THRES3	100	// word count threshold for COMMON3

// When trying changed soundfold words it becomes slow when trying more than
// two changes.  With less than two changes it's slightly faster but we miss a
// few good suggestions.  In rare cases we need to try three of four changes.
#define SCORE_SFMAX1	200	// maximum score for first try
#define SCORE_SFMAX2	300	// maximum score for second try
#define SCORE_SFMAX3	400	// maximum score for third try

#define SCORE_BIG	(SCORE_INS * 3)	// big difference
#define SCORE_MAXMAX	999999		// accept any score
#define SCORE_LIMITMAX	350		// for spell_edit_score_limit()

// for spell_edit_score_limit() we need to know the minimum value of
// SCORE_ICASE, SCORE_SWAP, SCORE_DEL, SCORE_SIMILAR and SCORE_INS
#define SCORE_EDIT_MIN	SCORE_SIMILAR

/*
 * For finding suggestions: At each node in the tree these states are tried:
 */
প্রকারনির্দেশ গণনা
{
    STATE_START = 0,	// At start of node check for NUL bytes (goodword
			// ends); if badword ends there is a match, otherwise
			// try splitting word.
    STATE_NOPREFIX,	// try without prefix
    STATE_SPLITUNDO,	// Undo splitting.
    STATE_ENDNUL,	// Past NUL bytes at start of the node.
    STATE_PLAIN,	// Use each byte of the node.
    STATE_DEL,		// Delete a byte from the bad word.
    STATE_INS_PREP,	// Prepare for inserting bytes.
    STATE_INS,		// Insert a byte in the bad word.
    STATE_SWAP,		// Swap two bytes.
    STATE_UNSWAP,	// Undo swap two characters.
    STATE_SWAP3,	// Swap two characters over three.
    STATE_UNSWAP3,	// Undo Swap two characters over three.
    STATE_UNROT3L,	// Undo rotate three characters left
    STATE_UNROT3R,	// Undo rotate three characters right
    STATE_REP_INI,	// Prepare for using REP items.
    STATE_REP,		// Use matching REP items from the .aff file.
    STATE_REP_UNDO,	// Undo a REP item replacement.
    STATE_FINAL		// End of this node.
} state_T;

/*
 * Struct to keep the state at each level in suggest_try_change().
 */
প্রকারনির্দেশ সংরচনা trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    পূর্ণাঙ্ক		ts_score;	// score
    idx_T	ts_arridx;	// index in tree array, start of node
    short	ts_curi;	// index in list of child nodes
    char_u	ts_fidx;	// index in fword[], case-folded bad word
    char_u	ts_fidxtry;	// ts_fidx at which bytes may be changed
    char_u	ts_twordlen;	// valid length of tword[]
    char_u	ts_prefixdepth;	// stack depth for end of prefix or
				// PFD_PREFIXTREE or PFD_NOPREFIX
    char_u	ts_flags;	// TSF_ flags
    char_u	ts_tcharlen;	// number of bytes in tword character
    char_u	ts_tcharidx;	// current byte index in tword character
    char_u	ts_isdiff;	// DIFF_ values
    char_u	ts_fcharstart;	// index in fword where badword char started
    char_u	ts_prewordlen;	// length of word in "preword[]"
    char_u	ts_splitoff;	// index in "tword" after last split
    char_u	ts_splitfidx;	// "ts_fidx" at word split
    char_u	ts_complen;	// nr of compound words used
    char_u	ts_compsplit;	// index for "compflags" where word was spit
    char_u	ts_save_badflags;   // su_badflags saved here
    char_u	ts_delidx;	// index in fword for char that was deleted,
				// valid when "ts_flags" has TSF_DIDDEL
} trystate_T;

// values for ts_isdiff
#define DIFF_NONE	0	// no different byte (yet)
#define DIFF_YES	1	// different byte found
#define DIFF_ঢোকাও	2	// inserting character

// values for ts_flags
#define TSF_PREFIXOK	1	// already checked that prefix is OK
#define TSF_DIDSPLIT	2	// tried split at this point
#define TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#define PFD_NOPREFIX	0xff	// not using prefixes
#define PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#define PFD_NOTSPECIAL	0xfd	// highest value that's not special

স্থির long spell_suggest_timeout = 5000;

স্থির শূন্য spell_find_suggest(char_u *badptr, পূর্ণাঙ্ক badlen, suginfo_T *su, পূর্ণাঙ্ক maxcount, পূর্ণাঙ্ক banbadword, পূর্ণাঙ্ক need_cap, পূর্ণাঙ্ক interactive);
#ifdef FEAT_EVAL
স্থির শূন্য spell_suggest_রাশিমালা(suginfo_T *su, char_u *রাশিমালা);
#endif
স্থির শূন্য spell_suggest_file(suginfo_T *su, char_u *fname);
স্থির শূন্য spell_suggest_intern(suginfo_T *su, পূর্ণাঙ্ক interactive);
স্থির শূন্য spell_find_cleanup(suginfo_T *su);
স্থির শূন্য suggest_try_special(suginfo_T *su);
স্থির শূন্য suggest_try_change(suginfo_T *su);
স্থির শূন্য suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, পূর্ণাঙ্ক soundfold);
স্থির শূন্য go_deeper(trystate_T *স্তূপ, পূর্ণাঙ্ক depth, পূর্ণাঙ্ক score_add);
স্থির শূন্য find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword);
স্থির শূন্য score_comp_sal(suginfo_T *su);
স্থির শূন্য score_combine(suginfo_T *su);
স্থির পূর্ণাঙ্ক stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
স্থির শূন্য suggest_try_soundalike_prep(শূন্য);
স্থির শূন্য suggest_try_soundalike(suginfo_T *su);
স্থির শূন্য suggest_try_soundalike_finish(শূন্য);
স্থির শূন্য add_sound_suggest(suginfo_T *su, char_u *goodword, পূর্ণাঙ্ক score, langp_T *lp);
স্থির পূর্ণাঙ্ক soundfold_find(slang_T *slang, char_u *word);
স্থির পূর্ণাঙ্ক similar_chars(slang_T *slang, পূর্ণাঙ্ক c1, পূর্ণাঙ্ক c2);
স্থির শূন্য add_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, পূর্ণাঙ্ক badlen, পূর্ণাঙ্ক score, পূর্ণাঙ্ক altscore, পূর্ণাঙ্ক had_bonus, slang_T *slang, পূর্ণাঙ্ক maxsf);
স্থির শূন্য check_suggestions(suginfo_T *su, garray_T *gap);
স্থির শূন্য add_banned(suginfo_T *su, char_u *word);
স্থির শূন্য rescore_suggestions(suginfo_T *su);
স্থির শূন্য rescore_one(suginfo_T *su, suggest_T *stp);
স্থির পূর্ণাঙ্ক cleanup_suggestions(garray_T *gap, পূর্ণাঙ্ক maxscore, পূর্ণাঙ্ক keep);
স্থির পূর্ণাঙ্ক soundalike_score(char_u *goodsound, char_u *badsound);
স্থির পূর্ণাঙ্ক spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
স্থির পূর্ণাঙ্ক spell_edit_score_limit(slang_T *slang, char_u *badword, char_u *goodword, পূর্ণাঙ্ক limit);
স্থির পূর্ণাঙ্ক spell_edit_score_limit_w(slang_T *slang, char_u *badword, char_u *goodword, পূর্ণাঙ্ক limit);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    স্থির পূর্ণাঙ্ক
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    পূর্ণাঙ্ক		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    যদি (!byte_in_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	প্রত্যাগম FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    যদি (slang->sl_comprules != রিক্ত && sp->ts_complen > sp->ts_compsplit)
    {
	পূর্ণাঙ্ক v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = match_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	প্রত্যাগম v;
    }

    প্রত্যাগম TRUE;
}

/*
 * Adjust the score of common words.
 */
    স্থির পূর্ণাঙ্ক
score_wordcount_adj(
    slang_T	*slang,
    পূর্ণাঙ্ক		score,
    char_u	*word,
    পূর্ণাঙ্ক		split)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    পূর্ণাঙ্ক		bonus;
    পূর্ণাঙ্ক		newscore;

    hi = hash_find(&slang->sl_wordcount, word);
    যদি (HASHITEM_EMPTY(hi))
	প্রত্যাগম score;

    wc = HI2WC(hi);
    যদি (wc->wc_গুনতি < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    অন্যথা যদি (wc->wc_গুনতি < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    অন্যথা
	bonus = SCORE_COMMON3;
    যদি (split)
	newscore = score - bonus / 2;
    অন্যথা
	newscore = score - bonus;
    যদি (newscore < 0)
	প্রত্যাগম 0;
    প্রত্যাগম newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    স্থির পূর্ণাঙ্ক
badword_captype(char_u *word, char_u *end)
{
    পূর্ণাঙ্ক		flags = captype(word, end);
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		l, u;
    পূর্ণাঙ্ক		first;
    char_u	*p;

    যদি (!(flags & WF_KEEPCAP))
	প্রত্যাগম flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    first = FALSE;
    প্রতি (p = word; p < end; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	যদি (SPELL_ISUPPER(c))
	{
	    ++u;
	    যদি (p == word)
		first = TRUE;
	}
	অন্যথা
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    যদি (u > l && u > 2)
	flags |= WF_ALLCAP;
    অন্যথা যদি (first)
	flags |= WF_ONECAP;

    যদি (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    প্রত্যাগম flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    স্থির পূর্ণাঙ্ক
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    পূর্ণাঙ্ক		nr;
    পূর্ণাঙ্ক		c;

    c = *p++;
    যদি ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    অন্যথা যদি ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    অন্যথা যদি ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    অন্যথা				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    প্রত্যাগম nr;
}

// values for sps_flags
#define SPS_BEST    1
#define SPS_FAST    2
#define SPS_DOUBLE  4

স্থির পূর্ণাঙ্ক sps_flags = SPS_BEST;	// flags from 'spellsuggest'
স্থির পূর্ণাঙ্ক sps_limit = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    পূর্ণাঙ্ক
spell_check_sps(শূন্য)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    পূর্ণাঙ্ক		f;

    sps_flags = 0;
    sps_limit = 9999;

    প্রতি (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	যদি (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_limit = getdigits(&s);
	    যদি (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	অন্যথা যদি (STRCMP(buf, "best") == 0)
	    f = SPS_BEST;
	অন্যথা যদি (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	অন্যথা যদি (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	অন্যথা যদি (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	যদি (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_BEST;
	    sps_limit = 9999;
	    প্রত্যাগম FAIL;
	}
	যদি (f != 0)
	    sps_flags = f;
    }

    যদি (sps_flags == 0)
	sps_flags = SPS_BEST;

    প্রত্যাগম OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    শূন্য
spell_suggest(পূর্ণাঙ্ক গুনতি)
{
    char_u	*সারি = রিক্ত;
    pos_T	prev_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;
    suginfo_T	sug;
    suggest_T	*stp;
    পূর্ণাঙ্ক		mouse_used;
    পূর্ণাঙ্ক		need_cap;
    পূর্ণাঙ্ক		limit;
    পূর্ণাঙ্ক		selected = গুনতি;
    পূর্ণাঙ্ক		badlen = 0;
    পূর্ণাঙ্ক		বার্তা_scroll_save = বার্তা_scroll;
    পূর্ণাঙ্ক		wo_spell_save = curwin->w_p_spell;

    যদি (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    prev_cursor = curwin->w_cursor;

    যদি (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	গচ্ছ skip;
    }

    যদি (VIsual_active)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	যদি (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    গচ্ছ skip;
	}
	badlen = (পূর্ণাঙ্ক)curwin->w_cursor.col - (পূর্ণাঙ্ক)VIsual.col;
	যদি (badlen < 0)
	    badlen = -badlen;
	অন্যথা
	    curwin->w_cursor.col = VIsual.col;
	++badlen;
	end_visual_mode();
	// make sure we don't include the NUL at the end of the line
	যদি (badlen > ml_get_curline_len() - (পূর্ণাঙ্ক)curwin->w_cursor.col)
	    badlen = ml_get_curline_len() - (পূর্ণাঙ্ক)curwin->w_cursor.col;
    }
    // Find the start of the badly spelled word.
    অন্যথা যদি (spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, রিক্ত) == 0
	    || curwin->w_cursor.col > prev_cursor.col)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = prev_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.col;
	// Backup to before start of word.
	যাবত্ (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_BACK(curline, p);
	// Forward to start of word.
	যাবত্ (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	যদি (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    গচ্ছ skip;
	}
	curwin->w_cursor.col = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.col);

    // Make a copy of current line since autocommands may free the line.
    সারি = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    যদি (সারি == রিক্ত)
	গচ্ছ skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    যদি (sps_limit > (পূর্ণাঙ্ক)Rows - 2)
	limit = (পূর্ণাঙ্ক)Rows - 2;
    অন্যথা
	limit = sps_limit;
    spell_find_suggest(সারি + curwin->w_cursor.col, badlen, &sug, limit,
							TRUE, need_cap, TRUE);

    যদি (sug.su_ga.ga_len == 0)
	বার্তা(_("Sorry, no suggestions"));
    অন্যথা যদি (গুনতি > 0)
    {
	যদি (গুনতি > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (long)sug.su_ga.ga_len);
    }
    অন্যথা
    {
#ifdef FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	যদি (cmdmsg_rl)
	    বার্তা_col = cmdline_width - 1;
#endif

	// List the suggestions.
	বার্তা_start();
	বার্তা_row = Rows - 1;	// for when 'cmdheight' > 1
	সারিগুলি_বাম = Rows;	// avoid more prompt
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#ifdef FEAT_RIGHTLEFT
	যদি (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#endif
	বার্তা_puts((বর্ণ *)IObuff);
	বার্তা_clr_eos();
	বার্তা_putchar('\n');

	বার্তা_scroll = TRUE;
	প্রতি (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    পূর্ণাঙ্ক el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_word, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    যদি (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%2d", i + 1);
#ifdef FEAT_RIGHTLEFT
	    যদি (cmdmsg_rl)
		rl_mirror(IObuff);
#endif
	    বার্তা_puts((বর্ণ *)IObuff);

	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    বার্তা_puts((বর্ণ *)IObuff);

	    // The word may replace more than "su_badlen".
	    যদি (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((বর্ণ *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		বার্তা_puts((বর্ণ *)IObuff);
	    }

	    যদি (p_verbose > 0)
	    {
		// Add the score.
		যদি (sps_flags & (SPS_DOUBLE | SPS_BEST))
		    vim_snprintf((বর্ণ *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		অন্যথা
		    vim_snprintf((বর্ণ *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#ifdef FEAT_RIGHTLEFT
		যদি (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#endif
		বার্তা_advance(30);
		বার্তা_puts((বর্ণ *)IObuff);
	    }
	    বার্তা_putchar('\n');
	}

#ifdef FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	বার্তা_col = 0;
#endif
	// Ask for choice.
	selected = prompt_for_সংখ্যা(&mouse_used);
	যদি (mouse_used)
	    selected -= সারিগুলি_বাম;
	সারিগুলি_বাম = Rows;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	বার্তা_scroll = বার্তা_scroll_save;
    }

    যদি (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_to);

	stp = &SUG(sug.su_ga, selected - 1);
	যদি (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%s%.*s", stp->st_word,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_to = vim_strsave(IObuff);
	}
	অন্যথা
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_to = vim_strsave(stp->st_word);
	}

	// Replace the word.
	p = alloc(STRLEN(সারি) - stp->st_orglen + stp->st_wordlen + 1);
	যদি (p != রিক্ত)
	{
	    পূর্ণাঙ্ক len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (পূর্ণাঙ্ক)(sug.su_badptr - সারি);
	    mch_memmove(p, সারি, c);
	    STRCPY(p + c, stp->st_word);
	    STRCAT(p, sug.su_badptr + stp->st_orglen);

	    // For redo we use a change-word command.
	    ResetRedobuff();
	    AppendToRedobuff((char_u *)"ciw");
	    AppendToRedobuffLit(p + c,
			    stp->st_wordlen + sug.su_badlen - stp->st_orglen);
	    AppendCharToRedobuff(ESC);

	    // "p" may be freed here
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    curwin->w_cursor.col = c;

	    changed_bytes(curwin->w_cursor.lnum, c);
#যদি defined(FEAT_PROP_POPUP)
	    যদি (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#endif
	}
    }
    অন্যথা
	curwin->w_cursor = prev_cursor;

    spell_find_cleanup(&sug);
skip:
    vim_free(সারি);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    শূন্য
spell_suggest_list(
    garray_T	*gap,
    char_u	*word,
    পূর্ণাঙ্ক		maxcount,	// maximum nr of suggestions
    পূর্ণাঙ্ক		need_cap,	// 'spellcapcheck' matched
    পূর্ণাঙ্ক		interactive)
{
    suginfo_T	sug;
    পূর্ণাঙ্ক		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_find_suggest(word, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, পরিমাণ(char_u *), sug.su_ga.ga_len + 1);
    যদি (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	প্রতি (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (unsigned)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    যদি (wcopy == রিক্ত)
		ভঙ্গ;
	    STRCPY(wcopy, stp->st_word);
	    STRCPY(wcopy + stp->st_wordlen, sug.su_badptr + stp->st_orglen);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = wcopy;
	}
    }

    spell_find_cleanup(&sug);
}

/*
 * Find spell suggestions for the word at the start of "badptr".
 * Return the suggestions in "su->su_ga".
 * The maximum number of suggestions is "maxcount".
 * Note: does use info for the current window.
 * This is based on the mechanisms of Aspell, but completely reimplemented.
 */
    স্থির শূন্য
spell_find_suggest(
    char_u	*badptr,
    পূর্ণাঙ্ক		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    পূর্ণাঙ্ক		maxcount,
    পূর্ণাঙ্ক		banbadword,	// don't include badword in suggestions
    পূর্ণাঙ্ক		need_cap,	// word should start with capital
    পূর্ণাঙ্ক		interactive)
{
    hlf_T	attr = HLF_গুনতি;
    char_u	buf[MAXPATHL];
    char_u	*p;
    পূর্ণাঙ্ক		do_combine = FALSE;
    char_u	*sps_copy;
#ifdef FEAT_EVAL
    স্থির পূর্ণাঙ্ক	রাশিমালা_busy = FALSE;
#endif
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		i;
    langp_T	*lp;
    পূর্ণাঙ্ক		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_POINTER(su);
    ga_init2(&su->su_ga, পরিমাণ(suggest_T), 10);
    ga_init2(&su->su_sga, পরিমাণ(suggest_T), 10);
    যদি (*badptr == NUL)
	প্রত্যাগম;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    যদি (badlen != 0)
	su->su_badlen = badlen;
    অন্যথা
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, রিক্ত, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    যদি (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (শূন্য)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    যদি (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    প্রতি (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_ENTRY(curbuf->b_s.b_langp, i);
	যদি (lp->lp_sallang != রিক্ত)
	{
	    su->su_sallang = lp->lp_sallang;
	    ভঙ্গ;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    যদি (su->su_sallang != রিক্ত)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    যদি (!SPELL_ISUPPER(c) && attr == HLF_গুনতি)
    {
	make_case_word(su->su_badword, buf, WF_ONECAP);
	add_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    যদি (banbadword)
	add_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    যদি (sps_copy == রিক্ত)
	প্রত্যাগম;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    প্রতি (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	যদি (STRNCMP(buf, "expr:", 5) == 0)
	{
#ifdef FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    যদি (!রাশিমালা_busy)
	    {
		রাশিমালা_busy = TRUE;
		spell_suggest_রাশিমালা(su, buf + 5);
		রাশিমালা_busy = FALSE;
	    }
#endif
	}
	অন্যথা যদি (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	অন্যথা যদি (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((বর্ণ *)buf + 8);
	অন্যথা যদি (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    যদি (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    যদি (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#ifdef FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    স্থির শূন্য
spell_suggest_রাশিমালা(suginfo_T *su, char_u *রাশিমালা)
{
    list_T	*list;
    listitem_T	*li;
    পূর্ণাঙ্ক		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_রাশিমালা(su->su_badword, রাশিমালা);
    যদি (list != রিক্ত)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_বস্তুসমূহ(list, li)
	    যদি (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		যদি (score >= 0 && score <= su->su_maxscore)
		    add_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (শূন্য)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#endif

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    স্থির শূন্য
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	সারি[MAXWLEN * 2];
    char_u	*p;
    পূর্ণাঙ্ক		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((বর্ণ *)fname, "r");
    যদি (fd == রিক্ত)
    {
	semsg(_(e_cant_open_file_str), fname);
	প্রত্যাগম;
    }

    // Read it line by line.
    যাবত্ (!vim_fgets(সারি, MAXWLEN * 2, fd) && !got_int)
    {
	সারি_breakcheck();

	p = vim_strchr(সারি, '/');
	যদি (p == রিক্ত)
	    অনুবর্ত;	    // No Tab found, just skip the line.
	*p++ = NUL;
	যদি (STRICMP(su->su_badword, সারি) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    প্রতি (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    যদি (captype(p, রিক্ত) == 0)
	    {
		make_case_word(p, cword, su->su_badflags);
		p = cword;
	    }

	    add_suggestion(su, &su->su_ga, p, su->su_badlen,
				  SCORE_FILE, 0, TRUE, su->su_sallang, FALSE);
	}
    }

    fclose(fd);

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (শূন্য)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    স্থির শূন্য
spell_suggest_intern(suginfo_T *su, পূর্ণাঙ্ক interactive)
{
    // Load the .sug file(s) that are available and not done yet.
    suggest_load_files();

    // 1. Try special cases, such as repeating a word: "the the" -> "the".
    //
    // Set a maximum score to limit the combination of operations that is
    // tried.
    suggest_try_special(su);

    // 2. Try inserting/deleting/swapping/changing a letter, use REP entries
    //    from the .aff file and inserting a space (split the word).
    suggest_try_change(su);

    // For the resulting top-scorers compute the sound-a-like score.
    যদি (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    যদি ((sps_flags & SPS_FAST) == 0)
    {
	যদি (sps_flags & SPS_BEST)
	    // Adjust the word score for the suggestions found so far for how
	    // they sound like.
	    rescore_suggestions(su);

	// While going through the soundfold tree "su_maxscore" is the score
	// for the soundfold word, limits the changes that are being tried,
	// and "su_sfmaxscore" the rescored score, which is set by
	// cleanup_suggestions().
	// First find words with a small edit distance, because this is much
	// faster and often already finds the top-N suggestions.  If we didn't
	// find many suggestions try again with a higher edit distance.
	// "sl_sounddone" is used to avoid doing the same word twice.
	suggest_try_soundalike_prep();
	su->su_maxscore = SCORE_SFMAX1;
	su->su_sfmaxscore = SCORE_MAXINIT * 3;
	suggest_try_soundalike(su);
	যদি (su->su_ga.ga_len < SUG_CLEAN_গুনতি(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    যদি (su->su_ga.ga_len < SUG_CLEAN_গুনতি(su))
	    {
		// Still didn't find enough matches, try again, allowing even
		// more changes to the soundfold word.
		su->su_maxscore = SCORE_SFMAX3;
		suggest_try_soundalike(su);
	    }
	}
	su->su_maxscore = su->su_sfmaxscore;
	suggest_try_soundalike_finish();
    }

    // When CTRL-C was hit while searching do show the results.  Only clear
    // got_int when using a command, not for spellsuggest().
    ui_breakcheck();
    যদি (interactive && got_int)
    {
	(শূন্য)vgetc();
	got_int = FALSE;
    }

    যদি ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	যদি (sps_flags & SPS_BEST)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(শূন্য)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    স্থির শূন্য
spell_find_cleanup(suginfo_T *su)
{
    পূর্ণাঙ্ক		i;

    // Free the suggestions.
    প্রতি (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_word);
    ga_clear(&su->su_ga);
    প্রতি (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_word);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    স্থির শূন্য
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    পূর্ণাঙ্ক		c;
    char_u	word[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    যদি (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
    {
	// Include badflags: if the badword is onecap or allcap
	// use that for the goodword too: "The the" -> "The".
	c = su->su_fbadword[len];
	su->su_fbadword[len] = NUL;
	make_case_word(su->su_fbadword, word, su->su_badflags);
	su->su_fbadword[len] = c;

	// Give a soundalike score of 0, compute the score as if deleting one
	// character.
	add_suggestion(su, &su->su_ga, word, su->su_badlen,
		       RESCORE(SCORE_REP, 0), 0, TRUE, su->su_sallang, FALSE);
    }
}

/*
 * Change the 0 to 1 to measure how much time is spent in each state.
 * Output is dumped in "suggestprof".
 */
#যদি 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T total;
proftime_T times[STATE_FINAL + 1];
long counts[STATE_FINAL + 1];

    স্থির শূন্য
prof_init(শূন্য)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_start(&current);
    profile_start(&total);
}

// call before changing state
    স্থির শূন্য
prof_store(state_T state)
{
    profile_end(&current);
    profile_add(&times[state], &current);
    ++counts[state];
    profile_start(&current);
}
# define PROF_STORE(state) prof_store(state);

    স্থির শূন্য
prof_report(বর্ণ *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_end(&total);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_বার্তা(&total));
    প্রতি (পূর্ণাঙ্ক i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_বার্তা(&times[i]), counts[i]);
    fclose(fd);
}
#অন্যথা
# define PROF_STORE(state)
#endif

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    স্থির শূন্য
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    পূর্ণাঙ্ক		n;
    char_u	*p;
    পূর্ণাঙ্ক		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (পূর্ণাঙ্ক)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (শূন্য)spell_casefold(curwin, p, (পূর্ণাঙ্ক)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (পূর্ণাঙ্ক)STRLEN(su->su_badptr);
    যদি (n < MAXWLEN)
	fword[n] = NUL;

    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	যদি (lp->lp_slang->sl_fbyts == রিক্ত)
	    অনুবর্ত;

	// Try it for this language.  Will add possible suggestions.
#ifdef SUGGEST_PROFILE
	prof_init();
#endif
	suggest_trie_walk(su, lp, fword, FALSE);
#ifdef SUGGEST_PROFILE
	prof_report("try_change");
#endif
    }
}

// Check the maximum score, if we go over it we won't try this change.
#define TRY_DEEPER(su, স্তূপ, depth, add) \
       ((depth) < MAXWLEN - 1 && (স্তূপ)[depth].ts_score + (add) < (su)->su_maxscore)

/*
 * Try finding suggestions by adding/removing/swapping letters.
 *
 * This uses a state machine.  At each node in the tree we try various
 * operations.  When trying if an operation works "depth" is increased and the
 * stack[] is used to store info.  This allows combinations, thus insert one
 * character, replace one and delete another.  The number of changes is
 * limited by su->su_maxscore.
 *
 * After implementing this I noticed an article by Kemal Oflazer that
 * describes something similar: "Error-tolerant Finite State Recognition with
 * Applications to Morphological Analysis and Spelling Correction" (1996).
 * The implementation in the article is simplified and requires a stack of
 * unknown depth.  The implementation here only needs a stack depth equal to
 * the length of the word.
 *
 * This is also used for the sound-folded word, "soundfold" is TRUE then.
 * The mechanism is the same, but we find a match with a sound-folded word
 * that comes from one or more original words.  Each of these words may be
 * added, this is done by add_sound_suggest().
 * Don't use:
 *	the prefix tree or the keep-case tree
 *	"su->su_badlen"
 *	anything to do with upper and lower case
 *	anything to do with word or non-word characters ("spell_iswordp()")
 *	banned words
 *	word flags (rare, region, compounding)
 *	word splitting for now
 *	"similar_chars()"
 *	use "slang->sl_repsal" instead of "lp->lp_replang->sl_rep"
 */
    স্থির শূন্য
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    পূর্ণাঙ্ক		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	স্তূপ[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    পূর্ণাঙ্ক		newscore;
    পূর্ণাঙ্ক		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    পূর্ণাঙ্ক		depth;
    পূর্ণাঙ্ক		c, c2, c3;
    পূর্ণাঙ্ক		n = 0;
    পূর্ণাঙ্ক		flags;
    garray_T	*gap;
    idx_T	arridx;
    পূর্ণাঙ্ক		len;
    char_u	*p;
    fromto_T	*ftp;
    পূর্ণাঙ্ক		fl = 0, tl;
    পূর্ণাঙ্ক		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    পূর্ণাঙ্ক		fword_ends;
    পূর্ণাঙ্ক		goodword_ends;
#ifdef DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#endif
    পূর্ণাঙ্ক		breakcheckcount = 1000;
#ifdef FEAT_RELTIME
    proftime_T	time_limit;
#endif
    পূর্ণাঙ্ক		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    depth = 0;
    sp = &স্তূপ[0];
    CLEAR_POINTER(sp);
    sp->ts_curi = 1;

    যদি (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = রিক্ত;
	pidxs = রিক্ত;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_START;
    }
    অন্যথা
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	যদি (pbyts != রিক্ত)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	অন্যথা
	{
	    byts = fbyts;
	    idxs = fidxs;
	    sp->ts_prefixdepth = PFD_NOPREFIX;
	    sp->ts_state = STATE_START;
	}
    }
#ifdef FEAT_RELTIME
    // The loop may take an indefinite amount of time. Break out after some
    // time.
    যদি (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &time_limit);
#endif

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    যাবত্ (depth >= 0 && !got_int)
    {
	sp = &স্তূপ[depth];
	বিকল্প (sp->ts_state)
	{
	পক্ষ STATE_START:
	পক্ষ STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    যদি (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		প্রতি (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (পূর্ণাঙ্ক)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		যদি (depth < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (পূর্ণাঙ্ক)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    যদি (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    অন্যথা
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#ifdef DEBUG_TRIEWALK
		    sprintf(changename[depth], "prefix");
#endif
		    go_deeper(স্তূপ, depth, 0);
		    ++depth;
		    sp = &স্তূপ[depth];
		    sp->ts_prefixdepth = depth - 1;
		    byts = fbyts;
		    idxs = fidxs;
		    sp->ts_arridx = 0;

		    // Move the prefix to preword[] with the right case
		    // and make find_keepcap_word() works.
		    tword[sp->ts_twordlen] = NUL;
		    make_case_word(tword + sp->ts_splitoff,
					  preword + sp->ts_prewordlen, flags);
		    sp->ts_prewordlen = (char_u)STRLEN(preword);
		    sp->ts_splitoff = sp->ts_twordlen;
		}
		ভঙ্গ;
	    }

	    যদি (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		ভঙ্গ;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (পূর্ণাঙ্ক)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    যদি (flags & WF_NOSUGGEST)
		ভঙ্গ;

	    fword_ends = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    যদি (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != রিক্ত)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = স্তূপ[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		প্রতি (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		যদি (c > 0)
		{
		    c = valid_word_prefix(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    যদি (c == 0)
			ভঙ্গ;

		    // Use the WF_RARE flag for a rare prefix.
		    যদি (c & WF_RAREPFX)
			flags |= WF_RARE;

		    // Tricky: when checking for both prefix and compounding
		    // we run into the prefix flag first.
		    // Remember that it's OK, so that we accept the prefix
		    // when arriving at a compound flag.
		    sp->ts_flags |= TSF_PREFIXOK;
		}
	    }

	    // Check NEEDCOMPOUND: can't use word without compounding.  Do try
	    // appending another compound word below.
	    যদি (sp->ts_complen == sp->ts_compsplit && fword_ends
						     && (flags & WF_NEEDCOMP))
		goodword_ends = FALSE;
	    অন্যথা
		goodword_ends = TRUE;

	    p = রিক্ত;
	    compound_ok = TRUE;
	    যদি (sp->ts_complen > sp->ts_compsplit)
	    {
		যদি (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    যদি (sp->ts_fidx - sp->ts_splitfidx
					  == sp->ts_twordlen - sp->ts_splitoff
			    && STRNCMP(fword + sp->ts_splitfidx,
					tword + sp->ts_splitoff,
					 sp->ts_fidx - sp->ts_splitfidx) == 0)
		    {
			preword[sp->ts_prewordlen] = NUL;
			newscore = score_wordcount_adj(slang, sp->ts_score,
						 preword + sp->ts_prewordlen,
						 sp->ts_prewordlen > 0);
			// Add the suggestion if the score isn't too bad.
			যদি (newscore <= su->su_maxscore)
			    add_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			ভঙ্গ;
		    }
		}
		অন্যথা
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    যদি (((unsigned)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			ভঙ্গ;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    যদি (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			ভঙ্গ;

		    compflags[sp->ts_complen] = ((unsigned)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    যদি (match_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    যদি (compound_ok)
		    {
			p = preword;
			যাবত্ (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			যদি (fword_ends && !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    // Compound is not allowed.  But it may still be
			    // possible if we add another (short) word.
			    compound_ok = FALSE;
		    }

		    // Get pointer to last char of previous word.
		    p = preword + sp->ts_prewordlen;
		    MB_PTR_BACK(preword, p);
		}
	    }

	    // Form the word with proper case in preword.
	    // If there is a word from a previous split, append.
	    // For the soundfold tree don't change the case, simply append.
	    যদি (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    অন্যথা যদি (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    অন্যথা
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		যদি ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		যদি (p != রিক্ত && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_word(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    যদি (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		যদি (flags & WF_BANNED)
		{
		    add_banned(su, preword + sp->ts_prewordlen);
		    ভঙ্গ;
		}
		যদি ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    যদি (slang->sl_compprog == রিক্ত)
			ভঙ্গ;
		    // the word so far was banned but we may try compounding
		    goodword_ends = FALSE;
		}
	    }

	    newscore = 0;
	    যদি (!soundfold)	// soundfold words don't have flags
	    {
		যদি ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		যদি (flags & WF_RARE)
		    newscore += SCORE_RARE;

		যদি (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, রিক্ত)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    যদি (fword_ends
		    && goodword_ends
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#ifdef DEBUG_TRIEWALK
		যদি (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    পূর্ণাঙ্ক	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    প্রতি (j = 0; j < depth; ++j)
			smsg("%s", changename[j]);
		}
#endif
		যদি (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    add_sound_suggest(su, preword, sp->ts_score, lp);
		}
		অন্যথা যদি (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_BACK(fword, p);
		    যদি (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_BACK(preword, p);
			যদি (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    যদি (score <= su->su_maxscore)
		    {
			add_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			যদি (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, রিক্ত);
			    যদি (c == 0 || c == WF_ALLCAP)
			    {
				make_case_word(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen,
						      c == 0 ? WF_ALLCAP : 0);

				add_suggestion(su, &su->su_ga, preword,
					sp->ts_fidx - repextra,
					score + SCORE_ICASE, 0, FALSE,
					lp->lp_sallang, FALSE);
			    }
			}
		    }
		}
	    }

	    // Try word split and/or compounding.
	    যদি ((sp->ts_fidx >= sp->ts_fidxtry || fword_ends)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		পূর্ণাঙ্ক	try_compound;
		পূর্ণাঙ্ক	try_split;

		// If past the end of the bad word don't try a split.
		// Otherwise try changing the next word.  E.g., find
		// suggestions for "the the" where the second "the" is
		// different.  It's done like a split.
		// TODO: word split for soundfold words
		try_split = (sp->ts_fidx - repextra < su->su_badlen)
								&& !soundfold;

		// Get here in several situations:
		// 1. The word in the tree ends:
		//    If the word allows compounding try that.  Otherwise try
		//    a split by inserting a space.  For both check that a
		//    valid words starts at fword[sp->ts_fidx].
		//    For NOBREAK do like compounding to be able to check if
		//    the next word is valid.
		// 2. The badword does end, but it was due to a change (e.g.,
		//    a swap).  No need to split, but do check that the
		//    following word is valid.
		// 3. The badword and the word in the tree end.  It may still
		//    be possible to compound another (short) word.
		try_compound = FALSE;
		যদি (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != রিক্ত
			&& ((unsigned)flags >> 24) != 0
			&& sp->ts_twordlen - sp->ts_splitoff
						       >= slang->sl_compminlen
			&& (!has_mbyte
			    || slang->sl_compminlen == 0
			    || mb_charlen(tword + sp->ts_splitoff)
						      >= slang->sl_compminlen)
			&& (slang->sl_compsylmax < MAXWLEN
			    || sp->ts_complen + 1 - sp->ts_compsplit
							  < slang->sl_compmax)
			&& (can_be_compound(sp, slang,
					 compflags, ((unsigned)flags >> 24))))

		{
		    try_compound = TRUE;
		    compflags[sp->ts_complen] = ((unsigned)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		}

		// For NOBREAK we never try splitting, it won't make any word
		// valid.
		যদি (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		অন্যথা যদি (!fword_ends
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		অন্যথা
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		যদি (try_split || try_compound)
		{
		    যদি (!try_compound && (!fword_ends || !goodword_ends))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			যদি (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    ভঙ্গ;
			p = preword;
			যাবত্ (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			যদি (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    ভঙ্গ;

			যদি (slang->sl_nosplitsugs)
			    newscore += SCORE_SPLIT_NO;
			অন্যথা
			    newscore += SCORE_SPLIT;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    যদি (TRY_DEEPER(su, স্তূপ, depth, newscore))
		    {
			go_deeper(স্তূপ, depth, newscore);
#ifdef DEBUG_TRIEWALK
			যদি (!try_compound && !fword_ends)
			    sprintf(changename[depth], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			অন্যথা
			    sprintf(changename[depth], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#endif
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++depth;
			sp = &স্তূপ[depth];

			// Append a space to preword when splitting.
			যদি (!try_compound && !fword_ends)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			যদি (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_ends)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_ends)
			{
			    পূর্ণাঙ্ক	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    যদি (fword_ends)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    অন্যথা
				sp->ts_score -= SCORE_SPLIT - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			যদি (try_compound)
			    ++sp->ts_complen;
			অন্যথা
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			যদি (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			অন্যথা
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			যদি (pbyts != রিক্ত)
			{
			    byts = pbyts;
			    idxs = pidxs;
			    sp->ts_prefixdepth = PFD_PREFIXTREE;
			    PROF_STORE(sp->ts_state)
			    sp->ts_state = STATE_NOPREFIX;
			}
		    }
		}
	    }
	    ভঙ্গ;

	পক্ষ STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_START;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    ভঙ্গ;

	পক্ষ STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    যদি (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		ভঙ্গ;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	পক্ষ STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    যদি (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		যদি (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		অন্যথা
		    sp->ts_state = STATE_FINAL;
	    }
	    অন্যথা
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		যদি (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		অন্যথা
		    newscore = SCORE_SUBST;
		যদি ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, স্তূপ, depth, newscore))
		{
		    go_deeper(স্তূপ, depth, newscore);
#ifdef DEBUG_TRIEWALK
		    যদি (newscore > 0)
			sprintf(changename[depth], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    অন্যথা
			sprintf(changename[depth], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#endif
		    ++depth;
		    sp = &স্তূপ[depth];
		    যদি (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    যদি (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    যদি (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			যদি (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			অন্যথা যদি (sp->ts_isdiff == DIFF_ঢোকাও
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			যদি (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    যদি (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				যদি (enc_utf8
					&& utf_iscomposing(
					    utf_ptr2char(tword
						+ sp->ts_twordlen
							   - sp->ts_tcharlen))
					&& utf_iscomposing(
					    utf_ptr2char(fword
							+ sp->ts_fcharstart)))
				    sp->ts_score -=
						  SCORE_SUBST - SCORE_SUBCOMP;

				// For a similar character adjust score from
				// SCORE_SUBST to SCORE_SIMILAR.
				অন্যথা যদি (!soundfold
					&& slang->sl_has_map
					&& similar_chars(slang,
					    mb_ptr2char(tword
						+ sp->ts_twordlen
							   - sp->ts_tcharlen),
					    mb_ptr2char(fword
							+ sp->ts_fcharstart)))
				    sp->ts_score -=
						  SCORE_SUBST - SCORE_SIMILAR;
			    }
			    অন্যথা যদি (sp->ts_isdiff == DIFF_ঢোকাও
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				যদি (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				অন্যথা
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_BACK(tword, p);
				    যদি (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    অন্যথা
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			যদি (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    ভঙ্গ;

	পক্ষ STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    যদি (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ভঙ্গ;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    যদি (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    অন্যথা
		newscore = SCORE_DEL;
	    যদি (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, স্তূপ, depth, newscore))
	    {
		go_deeper(স্তূপ, depth, newscore);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#endif
		++depth;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		স্তূপ[depth].ts_flags |= TSF_DIDDEL;
		স্তূপ[depth].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		যদি (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    স্তূপ[depth].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    যদি (enc_utf8 && utf_iscomposing(c))
			স্তূপ[depth].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    অন্যথা যদি (c == mb_ptr2char(fword + স্তূপ[depth].ts_fidx))
			স্তূপ[depth].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		অন্যথা
		{
		    ++স্তূপ[depth].ts_fidx;
		    যদি (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			স্তূপ[depth].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ভঙ্গ;
	    }
	    // FALLTHROUGH

	পক্ষ STATE_INS_PREP:
	    যদি (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP;
		ভঙ্গ;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    প্রতি (;;)
	    {
		যদি (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_SWAP;
		    ভঙ্গ;
		}
		যদি (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    ভঙ্গ;
		}
		++sp->ts_curi;
	    }
	    ভঙ্গ;

	    // FALLTHROUGH

	পক্ষ STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    যদি (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP;
		ভঙ্গ;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    যদি (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		ভঙ্গ;
	    }
	    c = byts[n];
	    যদি (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    অন্যথা
		newscore = SCORE_INS;
	    যদি (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, স্তূপ, depth, newscore))
	    {
		go_deeper(স্তূপ, depth, newscore);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#endif
		++depth;
		sp = &স্তূপ[depth];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		যদি (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    যদি (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_ঢোকাও;
		    }
		}
		অন্যথা
		    fl = 1;
		যদি (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    যদি (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    ভঙ্গ;

	পক্ষ STATE_SWAP:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    যদি (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ভঙ্গ;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    যদি (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ভঙ্গ;
	    }

	    যদি (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		যদি (p[n] == NUL)
		    c2 = NUL;
		অন্যথা যদি (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		অন্যথা
		    c2 = mb_ptr2char(p + n);
	    }
	    অন্যথা
	    {
		যদি (p[1] == NUL)
		    c2 = NUL;
		অন্যথা যদি (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		অন্যথা
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    যদি (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ভঙ্গ;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    যদি (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		ভঙ্গ;
	    }
	    যদি (c2 != NUL && TRY_DEEPER(su, স্তূপ, depth, SCORE_SWAP))
	    {
		go_deeper(স্তূপ, depth, SCORE_SWAP);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++depth;
		যদি (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		অন্যথা
		{
		    p[0] = c2;
		    p[1] = c;
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    অন্যথা
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ভঙ্গ;

	পক্ষ STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    যদি (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    অন্যথা
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	পক্ষ STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    যদি (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		যদি (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		অন্যথা
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    অন্যথা
	    {
		c = *p;
		c2 = p[1];
		যদি (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		অন্যথা
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    যদি (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ভঙ্গ;
	    }
	    যদি (TRY_DEEPER(su, স্তূপ, depth, SCORE_SWAP3))
	    {
		go_deeper(স্তূপ, depth, SCORE_SWAP3);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++depth;
		যদি (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		অন্যথা
		{
		    p[0] = p[2];
		    p[2] = c;
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    অন্যথা
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ভঙ্গ;

	পক্ষ STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    যদি (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c2 = mb_ptr2char(p + n);
		fl = mb_ptr2len(p + n);
		c = mb_ptr2char(p + n + fl);
		tl = mb_ptr2len(p + n + fl);
		mch_memmove(p + fl + tl, p, n);
		mb_char2bytes(c, p);
		mb_char2bytes(c2, p + tl);
		p = p + tl;
	    }
	    অন্যথা
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    যদি (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ভঙ্গ;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    যদি (TRY_DEEPER(su, স্তূপ, depth, SCORE_SWAP3))
	    {
		go_deeper(স্তূপ, depth, SCORE_SWAP3);
#ifdef DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[depth], "%.*s-%s: rotate left %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3L;
		++depth;
		p = fword + sp->ts_fidx;
		যদি (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		অন্যথা
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    অন্যথা
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ভঙ্গ;

	পক্ষ STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    যদি (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    অন্যথা
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    যদি (TRY_DEEPER(su, স্তূপ, depth, SCORE_SWAP3))
	    {
		go_deeper(স্তূপ, depth, SCORE_SWAP3);
#ifdef DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[depth], "%.*s-%s: rotate right %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3R;
		++depth;
		p = fword + sp->ts_fidx;
		যদি (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		অন্যথা
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    অন্যথা
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ভঙ্গ;

	পক্ষ STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    যদি (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    অন্যথা
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	পক্ষ STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    যদি ((lp->lp_replang == রিক্ত && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ভঙ্গ;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    যদি (soundfold)
		sp->ts_curi = slang->sl_repsal_first[fword[sp->ts_fidx]];
	    অন্যথা
		sp->ts_curi = lp->lp_replang->sl_rep_first[fword[sp->ts_fidx]];

	    যদি (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ভঙ্গ;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	পক্ষ STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    যদি (soundfold)
		gap = &slang->sl_repsal;
	    অন্যথা
		gap = &lp->lp_replang->sl_rep;
	    যাবত্ (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_data + sp->ts_curi++;
		যদি (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    ভঙ্গ;
		}
		যদি (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, স্তূপ, depth, SCORE_REP))
		{
		    go_deeper(স্তূপ, depth, SCORE_REP);
#ifdef DEBUG_TRIEWALK
		    sprintf(changename[depth], "%.*s-%s: replace %s with %s",
			    sp->ts_twordlen, tword, fword + sp->ts_fidx,
			    ftp->ft_from, ftp->ft_to);
#endif
		    // Need to undo this afterwards.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_REP_UNDO;

		    // Change the "from" to the "to" string.
		    ++depth;
		    fl = (পূর্ণাঙ্ক)STRLEN(ftp->ft_from);
		    tl = (পূর্ণাঙ্ক)STRLEN(ftp->ft_to);
		    যদি (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_to, tl);
		    স্তূপ[depth].ts_fidxtry = sp->ts_fidx + tl;
		    স্তূপ[depth].ts_tcharlen = 0;
		    ভঙ্গ;
		}
	    }

	    যদি (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    ভঙ্গ;

	পক্ষ STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    যদি (soundfold)
		gap = &slang->sl_repsal;
	    অন্যথা
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_data + sp->ts_curi - 1;
	    fl = (পূর্ণাঙ্ক)STRLEN(ftp->ft_from);
	    tl = (পূর্ণাঙ্ক)STRLEN(ftp->ft_to);
	    p = fword + sp->ts_fidx;
	    যদি (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    ভঙ্গ;

	স্বভাব:
	    // Did all possible states at this level, go up one level.
	    --depth;

	    যদি (depth >= 0 && স্তূপ[depth].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    যদি (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#ifdef FEAT_RELTIME
		যদি (spell_suggest_timeout > 0
					  && profile_passed_limit(&time_limit))
		    got_int = TRUE;
#endif
	    }
	}
    }
}


/*
 * Go one level deeper in the tree.
 */
    স্থির শূন্য
go_deeper(trystate_T *স্তূপ, পূর্ণাঙ্ক depth, পূর্ণাঙ্ক score_add)
{
    স্তূপ[depth + 1] = স্তূপ[depth];
    স্তূপ[depth + 1].ts_state = STATE_START;
    স্তূপ[depth + 1].ts_score = স্তূপ[depth].ts_score + score_add;
    স্তূপ[depth + 1].ts_curi = 1;	// start just after length byte
    স্তূপ[depth + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    স্থির শূন্য
find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    পূর্ণাঙ্ক		depth;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    পূর্ণাঙ্ক		round[MAXWLEN];
    পূর্ণাঙ্ক		fwordidx[MAXWLEN];
    পূর্ণাঙ্ক		uwordidx[MAXWLEN];
    পূর্ণাঙ্ক		kwordlen[MAXWLEN];

    পূর্ণাঙ্ক		flen, ulen;
    পূর্ণাঙ্ক		l;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    যদি (byts == রিক্ত)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	প্রত্যাগম;
    }

    // Make an all-cap version of "fword".
    allcap_copy(fword, uword);

    // Each character needs to be tried both case-folded and upper-case.
    // All this gets very complicated if we keep in mind that changing case
    // may change the byte length of a multi-byte character...
    depth = 0;
    arridx[0] = 0;
    round[0] = 0;
    fwordidx[0] = 0;
    uwordidx[0] = 0;
    kwordlen[0] = 0;
    যাবত্ (depth >= 0)
    {
	যদি (fword[fwordidx[depth]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    যদি (byts[arridx[depth] + 1] == 0)
	    {
		kword[kwordlen[depth]] = NUL;
		প্রত্যাগম;
	    }

	    // kword is getting too long, continue one level up
	    --depth;
	}
	অন্যথা যদি (++round[depth] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --depth;
	}
	অন্যথা
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    যদি (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[depth]);
		ulen = MB_CPTR2LEN(uword + uwordidx[depth]);
	    }
	    অন্যথা
		ulen = flen = 1;
	    যদি (round[depth] == 1)
	    {
		p = fword + fwordidx[depth];
		l = flen;
	    }
	    অন্যথা
	    {
		p = uword + uwordidx[depth];
		l = ulen;
	    }

	    প্রতি (tryidx = arridx[depth]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
		যাবত্ (lo < hi)
		{
		    m = (lo + hi) / 2;
		    যদি (byts[m] > c)
			hi = m - 1;
		    অন্যথা যদি (byts[m] < c)
			lo = m + 1;
		    অন্যথা
		    {
			lo = hi = m;
			ভঙ্গ;
		    }
		}

		// Stop if there is no matching byte.
		যদি (hi < lo || byts[lo] != c)
		    ভঙ্গ;

		// Continue at the child (if there is one).
		tryidx = idxs[lo];
	    }

	    যদি (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		যদি (round[depth] == 1)
		{
		    STRNCPY(kword + kwordlen[depth], fword + fwordidx[depth],
									flen);
		    kwordlen[depth + 1] = kwordlen[depth] + flen;
		}
		অন্যথা
		{
		    STRNCPY(kword + kwordlen[depth], uword + uwordidx[depth],
									ulen);
		    kwordlen[depth + 1] = kwordlen[depth] + ulen;
		}
		fwordidx[depth + 1] = fwordidx[depth] + flen;
		uwordidx[depth + 1] = uwordidx[depth] + ulen;

		++depth;
		arridx[depth] = tryidx;
		round[depth] = 0;
	    }
	}
    }

    // Didn't find it: "cannot happen".
    *kword = NUL;
}

/*
 * Compute the sound-a-like score for suggestions in su->su_ga and add them to
 * su->su_sga.
 */
    স্থির শূন্য
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    পূর্ণাঙ্ক		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    পূর্ণাঙ্ক		score;
    পূর্ণাঙ্ক		lpi;

    যদি (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	প্রত্যাগম;

    // Use the sound-folding of the first language that supports it.
    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	যদি (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    প্রতি (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		যদি (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_word = vim_strsave(stp->st_word);
		    যদি (sstp->st_word != রিক্ত)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    ভঙ্গ;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    স্থির শূন্য
score_combine(suginfo_T *su)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		lpi;
    slang_T	*slang = রিক্ত;

    // Add the alternate score to su_ga.
    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	যদি (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    প্রতি (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		যদি (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		অন্যথা
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    ভঙ্গ;
	}
    }

    যদি (slang == রিক্ত)	// Using "double" without sound folding.
    {
	(শূন্য)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	প্রত্যাগম;
    }

    // Add the alternate score to su_sga.
    প্রতি (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_word);
	যদি (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	অন্যথা
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (শূন্য)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (শূন্য)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, পরিমাণ(suginfo_T), 1);
    যদি (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	প্রত্যাগম;

    stp = &SUG(ga, 0);
    প্রতি (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	প্রতি (round = 1; round <= 2; ++round)
	{
	    gap = round == 1 ? &su->su_ga : &su->su_sga;
	    যদি (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_word;
		প্রতি (j = 0; j < ga.ga_len; ++j)
		    যদি (STRCMP(stp[j].st_word, p) == 0)
			ভঙ্গ;
		যদি (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		অন্যথা
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    যদি (ga.ga_len > su->su_maxcount)
    {
	প্রতি (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_word);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    স্থির পূর্ণাঙ্ক
stp_sal_score(
    suggest_T	*stp,
    suginfo_T	*su,
    slang_T	*slang,
    char_u	*badsound)	// sound-folded badword
{
    char_u	*p;
    char_u	*pbad;
    char_u	*pgood;
    char_u	badsound2[MAXWLEN];
    char_u	fword[MAXWLEN];
    char_u	goodsound[MAXWLEN];
    char_u	goodword[MAXWLEN];
    পূর্ণাঙ্ক		lendiff;

    lendiff = (পূর্ণাঙ্ক)(su->su_badlen - stp->st_orglen);
    যদি (lendiff >= 0)
	pbad = badsound;
    অন্যথা
    {
	// soundfold the bad word with more characters following
	(শূন্য)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	যদি (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_word) == NUL)
	    প্রতি (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    যদি (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_word);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    অন্যথা
	pgood = stp->st_word;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    প্রত্যাগম soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
প্রকারনির্দেশ সংরচনা
{
    short	sft_score;	// lowest score used
    char_u	sft_word[1];    // soundfolded word, actually longer
} sftword_T;

স্থির sftword_T dumsft;
#define HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_word - (char_u *)&dumsft)))
#define HI2SFT(hi)     HIKEY2SFT((hi)->hi_চাবি)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    স্থির শূন্য
suggest_try_soundalike_prep(শূন্য)
{
    langp_T	*lp;
    পূর্ণাঙ্ক		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	যদি (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != রিক্ত)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    স্থির শূন্য
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    পূর্ণাঙ্ক		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	যদি (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != রিক্ত)
	{
	    // soundfold the bad word
	    spell_soundfold(slang, su->su_fbadword, TRUE, salword);

	    // try all kinds of inserts/deletes/swaps/etc.
	    // TODO: also soundfold the next words, so that we can try joining
	    // and splitting
#ifdef SUGGEST_PROFILE
	    prof_init();
#endif
	    suggest_trie_walk(su, lp, salword, TRUE);
#ifdef SUGGEST_PROFILE
	    prof_report("soundalike");
#endif
	}
    }
}

/*
 * Finish up after calling suggest_try_soundalike().
 */
    স্থির শূন্য
suggest_try_soundalike_finish(শূন্য)
{
    langp_T	*lp;
    পূর্ণাঙ্ক		lpi;
    slang_T	*slang;
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	যদি (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != রিক্ত)
	{
	    // Free the info about handled words.
	    todo = (পূর্ণাঙ্ক)slang->sl_sounddone.ht_used;
	    FOR_ALL_HASHTAB_বস্তুসমূহ(&slang->sl_sounddone, hi, todo)
		যদি (!HASHITEM_EMPTY(hi))
		{
		    vim_free(HI2SFT(hi));
		    --todo;
		}

	    // Clear the hashtable, it may also be used by another region.
	    hash_clear(&slang->sl_sounddone);
	    hash_init(&slang->sl_sounddone);
	}
    }
}

/*
 * A match with a soundfolded word is found.  Add the good word(s) that
 * produce this soundfolded word.
 */
    স্থির শূন্য
add_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    পূর্ণাঙ্ক		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    পূর্ণাঙ্ক		sfwordnr;
    char_u	*nrline;
    পূর্ণাঙ্ক		orgnr;
    char_u	theword[MAXWLEN];
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		wlen;
    char_u	*byts;
    idx_T	*idxs;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		wordcount;
    পূর্ণাঙ্ক		wc;
    পূর্ণাঙ্ক		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    পূর্ণাঙ্ক		bc, gc;
    পূর্ণাঙ্ক		limit;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    যদি (HASHITEM_EMPTY(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_word) + STRLEN(goodword) + 1);
	যদি (sft != রিক্ত)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_word, goodword);
	    hash_add_item(&slang->sl_sounddone, hi, sft->sft_word, hash);
	}
    }
    অন্যথা
    {
	sft = HI2SFT(hi);
	যদি (score >= sft->sft_score)
	    প্রত্যাগম;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_find(slang, goodword);
    যদি (sfwordnr < 0)
    {
	internal_error("add_sound_suggest()");
	প্রত্যাগম;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    যাবত্ (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	প্রতি (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    যদি (wordcount == orgnr && byts[n + 1] == NUL)
		ভঙ্গ;	// found end of word

	    যদি (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    প্রতি ( ; byts[n + i] == NUL; ++i)
		যদি (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    গচ্ছ badword;
		}

	    // One of the siblings must have the word.
	    প্রতি ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		যদি (wordcount + wc > orgnr)
		    ভঙ্গ;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	প্রতি (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    পূর্ণাঙ্ক		flags = (পূর্ণাঙ্ক)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    যদি (flags & WF_NOSUGGEST)
		অনুবর্ত;

	    যদি (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, theword, cword);
		p = cword;
	    }
	    অন্যথা
	    {
		flags |= su->su_badflags;
		যদি ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_word(theword, cword, flags);
		    p = cword;
		}
		অন্যথা
		    p = theword;
	    }

	    // Add the suggestion.
	    যদি (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		যদি (score <= su->su_maxscore)
		    add_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    অন্যথা
	    {
		// Add a penalty for words in another region.
		যদি ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		অন্যথা
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		যদি (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    যদি (!SPELL_ISUPPER(bc)
				      && SPELL_TOFOLD(bc) != SPELL_TOFOLD(gc))
			goodscore += SCORE_ICASE / 2;
		}

		// Compute the score for the good word.  This only does letter
		// insert/delete/swap/replace.  REP items are not considered,
		// which may make the score a bit higher.
		// Use a limit for the score to make it work faster.  Use
		// MAXSCORE(), because RESCORE() will change the score.
		// If the limit is very high then the iterative method is
		// inefficient, using an array is quicker.
		limit = MAXSCORE(su->su_sfmaxscore - goodscore, score);
		যদি (limit > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		অন্যথা
		    goodscore += spell_edit_score_limit(slang, su->su_badword,
								    p, limit);

		// When going over the limit don't bother to do the rest.
		যদি (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    যদি (goodscore <= su->su_sfmaxscore)
			add_suggestion(su, &su->su_ga, p, su->su_badlen,
					 goodscore, score, TRUE, slang, TRUE);
		}
	    }
	}
	// smsg("word %s (%d): %s (%d)", sftword, sftnr, theword, orgnr);
    }
}

/*
 * Find word "word" in fold-case tree for "slang" and return the word number.
 */
    স্থির পূর্ণাঙ্ক
soundfold_find(slang_T *slang, char_u *word)
{
    idx_T	arridx = 0;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		wlen = 0;
    পূর্ণাঙ্ক		c;
    char_u	*ptr = word;
    char_u	*byts;
    idx_T	*idxs;
    পূর্ণাঙ্ক		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    প্রতি (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	যদি (byts[arridx] == NUL)
	{
	    যদি (c == NUL)
		ভঙ্গ;

	    // Skip over the zeros, there can be several.
	    যাবত্ (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    যদি (len == 0)
		প্রত্যাগম -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	যদি (c == NUL)
	    প্রত্যাগম -1;

	// Perform a binary search in the list of accepted bytes.
	যদি (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	যাবত্ (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    যদি (--len == 0)	// end of the bytes, didn't find it
		প্রত্যাগম -1;
	}
	যদি (byts[arridx] != c)	// didn't find the byte
	    প্রত্যাগম -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	যদি (c == ' ')
	    যাবত্ (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    প্রত্যাগম wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    স্থির পূর্ণাঙ্ক
similar_chars(slang_T *slang, পূর্ণাঙ্ক c1, পূর্ণাঙ্ক c2)
{
    পূর্ণাঙ্ক		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    যদি (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	যদি (HASHITEM_EMPTY(hi))
	    m1 = 0;
	অন্যথা
	    m1 = mb_ptr2char(hi->hi_চাবি + STRLEN(hi->hi_চাবি) + 1);
    }
    অন্যথা
	m1 = slang->sl_map_array[c1];
    যদি (m1 == 0)
	প্রত্যাগম FALSE;


    যদি (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	যদি (HASHITEM_EMPTY(hi))
	    m2 = 0;
	অন্যথা
	    m2 = mb_ptr2char(hi->hi_চাবি + STRLEN(hi->hi_চাবি) + 1);
    }
    অন্যথা
	m2 = slang->sl_map_array[c2];

    প্রত্যাগম m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    স্থির শূন্য
add_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    পূর্ণাঙ্ক		badlenarg,	// len of bad word replaced with "goodword"
    পূর্ণাঙ্ক		score,
    পূর্ণাঙ্ক		altscore,
    পূর্ণাঙ্ক		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    পূর্ণাঙ্ক		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    পূর্ণাঙ্ক		goodlen;	// len of goodword changed
    পূর্ণাঙ্ক		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    পূর্ণাঙ্ক		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    প্রতি (;;)
    {
	goodlen = (পূর্ণাঙ্ক)(pgood - goodword);
	badlen = (পূর্ণাঙ্ক)(pbad - su->su_badptr);
	যদি (goodlen <= 0 || badlen <= 0)
	    ভঙ্গ;
	MB_PTR_BACK(goodword, pgood);
	MB_PTR_BACK(su->su_badptr, pbad);
	যদি (has_mbyte)
	{
	    যদি (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		ভঙ্গ;
	}
	অন্যথা যদি (*pgood != *pbad)
		ভঙ্গ;
    }

    যদি (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	প্রত্যাগম;

    যদি (gap->ga_len == 0)
	i = -1;
    অন্যথা
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	প্রতি (i = gap->ga_len; --i >= 0; ++stp)
	    যদি (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_word, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		যদি (stp->st_slang == রিক্ত)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		যদি (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    যদি (had_bonus)
			rescore_one(su, stp);
		    অন্যথা
		    {
			new_sug.st_word = stp->st_word;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		যদি (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		ভঙ্গ;
	    }
    }

    যদি (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_word = vim_strnsave(goodword, goodlen);
	যদি (stp->st_word != রিক্ত)
	{
	    stp->st_wordlen = goodlen;
	    stp->st_score = score;
	    stp->st_altscore = altscore;
	    stp->st_had_bonus = had_bonus;
	    stp->st_orglen = badlen;
	    stp->st_slang = slang;
	    ++gap->ga_len;

	    // If we have too many suggestions now, sort the list and keep
	    // the best suggestions.
	    যদি (gap->ga_len > SUG_MAX_গুনতি(su))
	    {
		যদি (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_গুনতি(su));
		অন্যথা
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_গুনতি(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    স্থির শূন্য
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    পূর্ণাঙ্ক		i;
    char_u	longword[MAXWLEN + 1];
    পূর্ণাঙ্ক		len;
    hlf_T	attr;

    যদি (gap->ga_len == 0)
	প্রত্যাগম;
    stp = &SUG(*gap, 0);
    প্রতি (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_word, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_গুনতি;
	(শূন্য)spell_check(curwin, longword, &attr, রিক্ত, FALSE);
	যদি (attr != HLF_গুনতি)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_word);
	    --gap->ga_len;
	    যদি (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       পরিমাণ(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    স্থির শূন্য
add_banned(
    suginfo_T	*su,
    char_u	*word)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(word);
    hi = hash_lookup(&su->su_banned, word, hash);
    যদি (!HASHITEM_EMPTY(hi))		// already present
	প্রত্যাগম;
    s = vim_strsave(word);
    যদি (s != রিক্ত)
	hash_add_item(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    স্থির শূন্য
rescore_suggestions(suginfo_T *su)
{
    পূর্ণাঙ্ক		i;

    যদি (su->su_sallang != রিক্ত)
	প্রতি (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    স্থির শূন্য
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    যদি (slang != রিক্ত && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	যদি (slang == su->su_sallang)
	    p = su->su_sal_badword;
	অন্যথা
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	যদি (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

স্থির পূর্ণাঙ্ক sug_compare(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    স্থির পূর্ণাঙ্ক
sug_compare(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    পূর্ণাঙ্ক		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    যদি (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	যদি (n == 0)
	    n = STRICMP(p1->st_word, p2->st_word);
    }
    প্রত্যাগম n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    স্থির পূর্ণাঙ্ক
cleanup_suggestions(
    garray_T	*gap,
    পূর্ণাঙ্ক		maxscore,
    পূর্ণাঙ্ক		keep)		// nr of suggestions to keep
{
    যদি (gap->ga_len <= 0)
	প্রত্যাগম maxscore;

    // Sort the list.
    qsort(gap->ga_data, (size_t)gap->ga_len, পরিমাণ(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    যদি (gap->ga_len > keep)
    {
	পূর্ণাঙ্ক		i;
	suggest_T   *stp = &SUG(*gap, 0);

	প্রতি (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_word);
	gap->ga_len = keep;
	যদি (keep >= 1)
	    প্রত্যাগম stp[keep - 1].st_score;
    }
    প্রত্যাগম maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    স্থির পূর্ণাঙ্ক
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    পূর্ণাঙ্ক		goodlen;
    পূর্ণাঙ্ক		badlen;
    পূর্ণাঙ্ক		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    পূর্ণাঙ্ক		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    যদি ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	যদি ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    প্রত্যাগম SCORE_DEL;
	যদি (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    প্রত্যাগম SCORE_MAXMAX;

	যদি (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	অন্যথা
	{
	    score = 2 * SCORE_DEL / 3;
	    যদি (*badsound == '*')
		++badsound;
	    অন্যথা
		++goodsound;
	}
    }

    goodlen = (পূর্ণাঙ্ক)STRLEN(goodsound);
    badlen = (পূর্ণাঙ্ক)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    যদি (n < -2 || n > 2)
	প্রত্যাগম SCORE_MAXMAX;

    যদি (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    অন্যথা
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    যাবত্ (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    বিকল্প (n)
    {
	পক্ষ -2:
	পক্ষ 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    যাবত্ (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    যদি (STRCMP(pl + 1, ps) == 0)
		প্রত্যাগম score + SCORE_DEL * 2;

	    // Failed to compare.
	    ভঙ্গ;

	পক্ষ -1:
	পক্ষ 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    যাবত্ (*pl2 == *ps2)
	    {
		যদি (*pl2 == NUL)	// reached the end
		    প্রত্যাগম score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    যদি (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		প্রত্যাগম score + SCORE_DEL + SCORE_SWAP;

	    // 3: delete then substitute, then the rest must be equal
	    যদি (STRCMP(pl2 + 1, ps2 + 1) == 0)
		প্রত্যাগম score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    যদি (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		যাবত্ (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		যদি (STRCMP(pl2 + 1, ps2) == 0)
		    প্রত্যাগম score + SCORE_SWAP + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    যাবত্ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    যদি (STRCMP(pl2 + 1, ps2) == 0)
		প্রত্যাগম score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    ভঙ্গ;

	পক্ষ 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    যদি (*pl == NUL)
		প্রত্যাগম score;

	    // 2: swap
	    যদি (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		যাবত্ (*pl2 == *ps2)
		{
		    যদি (*pl2 == NUL)	// reached the end
			প্রত্যাগম score + SCORE_SWAP;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		যদি (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    প্রত্যাগম score + SCORE_SWAP + SCORE_SWAP;

		// 4: swap and substitute
		যদি (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    প্রত্যাগম score + SCORE_SWAP + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    যাবত্ (*pl2 == *ps2)
	    {
		যদি (*pl2 == NUL)	// reached the end
		    প্রত্যাগম score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    যদি (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		প্রত্যাগম score + SCORE_SUBST + SCORE_SWAP;

	    // 7: substitute and substitute
	    যদি (STRCMP(pl2 + 1, ps2 + 1) == 0)
		প্রত্যাগম score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    যাবত্ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    যদি (STRCMP(pl2 + 1, ps2) == 0)
		প্রত্যাগম score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    যাবত্ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    যদি (STRCMP(pl2, ps2 + 1) == 0)
		প্রত্যাগম score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    ভঙ্গ;
    }

    প্রত্যাগম SCORE_MAXMAX;
}

/*
 * Compute the "edit distance" to turn "badword" into "goodword".  The less
 * deletes/inserts/substitutes/swaps are required the lower the score.
 *
 * The algorithm is described by Du and Chang, 1992.
 * The implementation of the algorithm comes from Aspell editdist.cpp,
 * edit_distance().  It has been converted from C++ to C and modified to
 * support multi-byte characters.
 */
    স্থির পূর্ণাঙ্ক
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    পূর্ণাঙ্ক		*cnt;
    পূর্ণাঙ্ক		badlen, goodlen;	// lengths including NUL
    পূর্ণাঙ্ক		j, i;
    পূর্ণাঙ্ক		t;
    পূর্ণাঙ্ক		bc, gc;
    পূর্ণাঙ্ক		pbc, pgc;
    char_u	*p;
    পূর্ণাঙ্ক		wbadword[MAXWLEN];
    পূর্ণাঙ্ক		wgoodword[MAXWLEN];

    যদি (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	প্রতি (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	প্রতি (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    অন্যথা
    {
	badlen = (পূর্ণাঙ্ক)STRLEN(badword) + 1;
	goodlen = (পূর্ণাঙ্ক)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#define CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(পূর্ণাঙ্ক, (badlen + 1) * (goodlen + 1));
    যদি (cnt == রিক্ত)
	প্রত্যাগম 0;	// out of memory

    CNT(0, 0) = 0;
    প্রতি (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    প্রতি (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	প্রতি (j = 1; j <= goodlen; ++j)
	{
	    যদি (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    অন্যথা
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    যদি (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    অন্যথা
	    {
		// Use a better score when there is only a case difference.
		যদি (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		অন্যথা
		{
		    // For a similar character use SCORE_SIMILAR.
		    যদি (slang != রিক্ত
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    অন্যথা
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		যদি (i > 1 && j > 1)
		{
		    যদি (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    অন্যথা
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    যদি (bc == pgc && pbc == gc)
		    {
			t = SCORE_SWAP + CNT(i - 2, j - 2);
			যদি (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		যদি (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		যদি (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    প্রত্যাগম i;
}

প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		badi;
    পূর্ণাঙ্ক		goodi;
    পূর্ণাঙ্ক		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    স্থির পূর্ণাঙ্ক
spell_edit_score_limit(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    পূর্ণাঙ্ক		limit)
{
    limitscore_T    স্তূপ[10];		// allow for over 3 * 2 edits
    পূর্ণাঙ্ক		    stackidx;
    পূর্ণাঙ্ক		    bi, gi;
    পূর্ণাঙ্ক		    bi2, gi2;
    পূর্ণাঙ্ক		    bc, gc;
    পূর্ণাঙ্ক		    score;
    পূর্ণাঙ্ক		    score_off;
    পূর্ণাঙ্ক		    minscore;
    পূর্ণাঙ্ক		    round;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    যদি (has_mbyte)
	প্রত্যাগম spell_edit_score_limit_w(slang, badword, goodword, limit);

    // The idea is to go from start to end over the words.  So long as
    // characters are equal just continue, this always gives the lowest score.
    // When there is a difference try several alternatives.  Each alternative
    // increases "score" for the edit distance.  Some of the alternatives are
    // pushed unto a stack and tried later, some are tried right away.  At the
    // end of the word the score for one alternative is known.  The lowest
    // possible score is stored in "minscore".
    stackidx = 0;
    bi = 0;
    gi = 0;
    score = 0;
    minscore = limit + 1;

    প্রতি (;;)
    {
	// Skip over an equal part, score remains the same.
	প্রতি (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    যদি (bc != gc)	// stop at a char that's different
		ভঙ্গ;
	    যদি (bc == NUL)	// both words end
	    {
		যদি (score < minscore)
		    minscore = score;
		গচ্ছ বের_করো;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	যদি (gc == NUL)    // goodword ends, delete badword chars
	{
	    কুরু
	    {
		যদি ((score += SCORE_DEL) >= minscore)
		    গচ্ছ বের_করো;	    // do next alternative
	    } যাবত্ (badword[++bi] != NUL);
	    minscore = score;
	}
	অন্যথা যদি (bc == NUL) // badword ends, insert badword chars
	{
	    কুরু
	    {
		যদি ((score += SCORE_INS) >= minscore)
		    গচ্ছ বের_করো;	    // do next alternative
	    } যাবত্ (goodword[++gi] != NUL);
	    minscore = score;
	}
	অন্যথা			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    প্রতি (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		যদি (score_off < minscore)
		{
		    যদি (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			যাবত্ (goodword[gi2] == badword[bi2])
			{
			    যদি (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				ভঙ্গ;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    অন্যথা
		    {
			// try deleting/inserting a character later
			স্তূপ[stackidx].badi = bi + 1 - round;
			স্তূপ[stackidx].goodi = gi + round;
			স্তূপ[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    যদি (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		যদি (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    অনুবর্ত;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    যদি (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    অন্যথা
	    {
		// For a similar character use SCORE_SIMILAR.
		যদি (slang != রিক্ত
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		অন্যথা
		    score += SCORE_SUBST;
	    }

	    যদি (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		অনুবর্ত;
	    }
	}
বের_করো:
	// Get here to try the next alternative, pop it from the stack.
	যদি (stackidx == 0)		// stack is empty, finished
	    ভঙ্গ;

	// pop an item from the stack
	--stackidx;
	gi = স্তূপ[stackidx].goodi;
	bi = স্তূপ[stackidx].badi;
	score = স্তূপ[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    যদি (minscore > limit)
	প্রত্যাগম SCORE_MAXMAX;
    প্রত্যাগম minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    স্থির পূর্ণাঙ্ক
spell_edit_score_limit_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    পূর্ণাঙ্ক		limit)
{
    limitscore_T    স্তূপ[10];		// allow for over 3 * 2 edits
    পূর্ণাঙ্ক		    stackidx;
    পূর্ণাঙ্ক		    bi, gi;
    পূর্ণাঙ্ক		    bi2, gi2;
    পূর্ণাঙ্ক		    bc, gc;
    পূর্ণাঙ্ক		    score;
    পূর্ণাঙ্ক		    score_off;
    পূর্ণাঙ্ক		    minscore;
    পূর্ণাঙ্ক		    round;
    char_u	    *p;
    পূর্ণাঙ্ক		    wbadword[MAXWLEN];
    পূর্ণাঙ্ক		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    প্রতি (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    প্রতি (p = goodword; *p != NUL; )
	wgoodword[gi++] = mb_cptr2char_adv(&p);
    wgoodword[gi++] = 0;

    // The idea is to go from start to end over the words.  So long as
    // characters are equal just continue, this always gives the lowest score.
    // When there is a difference try several alternatives.  Each alternative
    // increases "score" for the edit distance.  Some of the alternatives are
    // pushed unto a stack and tried later, some are tried right away.  At the
    // end of the word the score for one alternative is known.  The lowest
    // possible score is stored in "minscore".
    stackidx = 0;
    bi = 0;
    gi = 0;
    score = 0;
    minscore = limit + 1;

    প্রতি (;;)
    {
	// Skip over an equal part, score remains the same.
	প্রতি (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    যদি (bc != gc)	// stop at a char that's different
		ভঙ্গ;
	    যদি (bc == NUL)	// both words end
	    {
		যদি (score < minscore)
		    minscore = score;
		গচ্ছ বের_করো;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	যদি (gc == NUL)    // goodword ends, delete badword chars
	{
	    কুরু
	    {
		যদি ((score += SCORE_DEL) >= minscore)
		    গচ্ছ বের_করো;	    // do next alternative
	    } যাবত্ (wbadword[++bi] != NUL);
	    minscore = score;
	}
	অন্যথা যদি (bc == NUL) // badword ends, insert badword chars
	{
	    কুরু
	    {
		যদি ((score += SCORE_INS) >= minscore)
		    গচ্ছ বের_করো;	    // do next alternative
	    } যাবত্ (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	অন্যথা			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    প্রতি (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		যদি (score_off < minscore)
		{
		    যদি (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			যাবত্ (wgoodword[gi2] == wbadword[bi2])
			{
			    যদি (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				ভঙ্গ;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    অন্যথা
		    {
			// try deleting a character from badword later
			স্তূপ[stackidx].badi = bi + 1 - round;
			স্তূপ[stackidx].goodi = gi + round;
			স্তূপ[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    যদি (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		যদি (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    অনুবর্ত;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    যদি (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    অন্যথা
	    {
		// For a similar character use SCORE_SIMILAR.
		যদি (slang != রিক্ত
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		অন্যথা
		    score += SCORE_SUBST;
	    }

	    যদি (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		অনুবর্ত;
	    }
	}
বের_করো:
	// Get here to try the next alternative, pop it from the stack.
	যদি (stackidx == 0)		// stack is empty, finished
	    ভঙ্গ;

	// pop an item from the stack
	--stackidx;
	gi = স্তূপ[stackidx].goodi;
	bi = স্তূপ[stackidx].badi;
	score = স্তূপ[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    যদি (minscore > limit)
	প্রত্যাগম SCORE_MAXMAX;
    প্রত্যাগম minscore;
}
#endif  // FEAT_SPELL
