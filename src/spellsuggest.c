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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_SPELL)

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
வகைவரை கட்டமைப்பு suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    முழு		su_maxcount;	    // max. number of suggestions displayed
    முழு		su_maxscore;	    // maximum score for adding to su_ga
    முழு		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    முழு		su_badlen;	    // length of detected bad word in line
    முழு		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
வகைவரை கட்டமைப்பு suggest_S
{
    char_u	*st_word;	// suggested word, allocated string
    முழு		st_wordlen;	// STRLEN(st_word)
    முழு		st_orglen;	// length of replaced text
    முழு		st_score;	// lower is better
    முழு		st_altscore;	// used when st_score compares equal
    முழு		st_salscore;	// st_score is for soundalike
    முழு		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#define SUG(ga, i) (((suggest_T *)(ga).ga_data)[i])

// TRUE if a word appears in the list of banned words.
#define WAS_BANNED(su, word) (!HASHITEM_EMPTY(hash_find(&(su)->su_banned, word)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#define SUG_CLEAN_மொத்தம்(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#define SUG_MAX_மொத்தம்(su)	(SUG_CLEAN_மொத்தம்(su) + 50)

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
வகைவரை எண்ணிக்கை
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
வகைவரை கட்டமைப்பு trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    முழு		ts_score;	// score
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
#define DIFF_செருகு	2	// inserting character

// values for ts_flags
#define TSF_PREFIXOK	1	// already checked that prefix is OK
#define TSF_DIDSPLIT	2	// tried split at this point
#define TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#define PFD_NOPREFIX	0xff	// not using prefixes
#define PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#define PFD_NOTSPECIAL	0xfd	// highest value that's not special

நிலையான long spell_suggest_timeout = 5000;

நிலையான வெறுமை spell_find_suggest(char_u *badptr, முழு badlen, suginfo_T *su, முழு maxcount, முழு banbadword, முழு need_cap, முழு interactive);
#ifdef FEAT_EVAL
நிலையான வெறுமை spell_suggest_கோவை(suginfo_T *su, char_u *கோவை);
#endif
நிலையான வெறுமை spell_suggest_file(suginfo_T *su, char_u *fname);
நிலையான வெறுமை spell_suggest_intern(suginfo_T *su, முழு interactive);
நிலையான வெறுமை spell_find_cleanup(suginfo_T *su);
நிலையான வெறுமை suggest_try_special(suginfo_T *su);
நிலையான வெறுமை suggest_try_change(suginfo_T *su);
நிலையான வெறுமை suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, முழு soundfold);
நிலையான வெறுமை go_deeper(trystate_T *அடுக்கு, முழு depth, முழு score_add);
நிலையான வெறுமை find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword);
நிலையான வெறுமை score_comp_sal(suginfo_T *su);
நிலையான வெறுமை score_combine(suginfo_T *su);
நிலையான முழு stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
நிலையான வெறுமை suggest_try_soundalike_prep(வெறுமை);
நிலையான வெறுமை suggest_try_soundalike(suginfo_T *su);
நிலையான வெறுமை suggest_try_soundalike_finish(வெறுமை);
நிலையான வெறுமை add_sound_suggest(suginfo_T *su, char_u *goodword, முழு score, langp_T *lp);
நிலையான முழு soundfold_find(slang_T *slang, char_u *word);
நிலையான முழு similar_chars(slang_T *slang, முழு c1, முழு c2);
நிலையான வெறுமை add_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, முழு badlen, முழு score, முழு altscore, முழு had_bonus, slang_T *slang, முழு maxsf);
நிலையான வெறுமை check_suggestions(suginfo_T *su, garray_T *gap);
நிலையான வெறுமை add_banned(suginfo_T *su, char_u *word);
நிலையான வெறுமை rescore_suggestions(suginfo_T *su);
நிலையான வெறுமை rescore_one(suginfo_T *su, suggest_T *stp);
நிலையான முழு cleanup_suggestions(garray_T *gap, முழு maxscore, முழு keep);
நிலையான முழு soundalike_score(char_u *goodsound, char_u *badsound);
நிலையான முழு spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
நிலையான முழு spell_edit_score_limit(slang_T *slang, char_u *badword, char_u *goodword, முழு limit);
நிலையான முழு spell_edit_score_limit_w(slang_T *slang, char_u *badword, char_u *goodword, முழு limit);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    நிலையான முழு
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    முழு		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    எனில் (!byte_in_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	திரும்பு FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    எனில் (slang->sl_comprules != வெற்று && sp->ts_complen > sp->ts_compsplit)
    {
	முழு v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = match_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	திரும்பு v;
    }

    திரும்பு TRUE;
}

/*
 * Adjust the score of common words.
 */
    நிலையான முழு
score_wordcount_adj(
    slang_T	*slang,
    முழு		score,
    char_u	*word,
    முழு		split)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    முழு		bonus;
    முழு		newscore;

    hi = hash_find(&slang->sl_wordcount, word);
    எனில் (HASHITEM_EMPTY(hi))
	திரும்பு score;

    wc = HI2WC(hi);
    எனில் (wc->wc_மொத்தம் < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    இல்லையெனில் எனில் (wc->wc_மொத்தம் < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    இல்லையெனில்
	bonus = SCORE_COMMON3;
    எனில் (split)
	newscore = score - bonus / 2;
    இல்லையெனில்
	newscore = score - bonus;
    எனில் (newscore < 0)
	திரும்பு 0;
    திரும்பு newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    நிலையான முழு
badword_captype(char_u *word, char_u *end)
{
    முழு		flags = captype(word, end);
    முழு		c;
    முழு		l, u;
    முழு		first;
    char_u	*p;

    எனில் (!(flags & WF_KEEPCAP))
	திரும்பு flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    first = FALSE;
    க்கு (p = word; p < end; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	எனில் (SPELL_ISUPPER(c))
	{
	    ++u;
	    எனில் (p == word)
		first = TRUE;
	}
	இல்லையெனில்
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    எனில் (u > l && u > 2)
	flags |= WF_ALLCAP;
    இல்லையெனில் எனில் (first)
	flags |= WF_ONECAP;

    எனில் (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    திரும்பு flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    நிலையான முழு
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    முழு		nr;
    முழு		c;

    c = *p++;
    எனில் ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    இல்லையெனில் எனில் ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    இல்லையெனில் எனில் ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    இல்லையெனில்				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    திரும்பு nr;
}

// values for sps_flags
#define SPS_BEST    1
#define SPS_FAST    2
#define SPS_DOUBLE  4

நிலையான முழு sps_flags = SPS_BEST;	// flags from 'spellsuggest'
நிலையான முழு sps_limit = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    முழு
spell_check_sps(வெறுமை)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    முழு		f;

    sps_flags = 0;
    sps_limit = 9999;

    க்கு (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	எனில் (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_limit = getdigits(&s);
	    எனில் (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	இல்லையெனில் எனில் (STRCMP(buf, "best") == 0)
	    f = SPS_BEST;
	இல்லையெனில் எனில் (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	இல்லையெனில் எனில் (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	இல்லையெனில் எனில் (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	எனில் (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_BEST;
	    sps_limit = 9999;
	    திரும்பு FAIL;
	}
	எனில் (f != 0)
	    sps_flags = f;
    }

    எனில் (sps_flags == 0)
	sps_flags = SPS_BEST;

    திரும்பு OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    வெறுமை
spell_suggest(முழு மொத்தம்)
{
    char_u	*வரி = வெற்று;
    pos_T	prev_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    முழு		i;
    முழு		c;
    suginfo_T	sug;
    suggest_T	*stp;
    முழு		mouse_used;
    முழு		need_cap;
    முழு		limit;
    முழு		selected = மொத்தம்;
    முழு		badlen = 0;
    முழு		செய்தி_scroll_save = செய்தி_scroll;
    முழு		wo_spell_save = curwin->w_p_spell;

    எனில் (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    prev_cursor = curwin->w_cursor;

    எனில் (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	செல் skip;
    }

    எனில் (VIsual_active)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	எனில் (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    செல் skip;
	}
	badlen = (முழு)curwin->w_cursor.col - (முழு)VIsual.col;
	எனில் (badlen < 0)
	    badlen = -badlen;
	இல்லையெனில்
	    curwin->w_cursor.col = VIsual.col;
	++badlen;
	end_visual_mode();
	// make sure we don't include the NUL at the end of the line
	எனில் (badlen > ml_get_curline_len() - (முழு)curwin->w_cursor.col)
	    badlen = ml_get_curline_len() - (முழு)curwin->w_cursor.col;
    }
    // Find the start of the badly spelled word.
    இல்லையெனில் எனில் (spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, வெற்று) == 0
	    || curwin->w_cursor.col > prev_cursor.col)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = prev_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.col;
	// Backup to before start of word.
	போது (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_BACK(curline, p);
	// Forward to start of word.
	போது (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	எனில் (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    செல் skip;
	}
	curwin->w_cursor.col = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.col);

    // Make a copy of current line since autocommands may free the line.
    வரி = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    எனில் (வரி == வெற்று)
	செல் skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    எனில் (sps_limit > (முழு)Rows - 2)
	limit = (முழு)Rows - 2;
    இல்லையெனில்
	limit = sps_limit;
    spell_find_suggest(வரி + curwin->w_cursor.col, badlen, &sug, limit,
							TRUE, need_cap, TRUE);

    எனில் (sug.su_ga.ga_len == 0)
	செய்தி(_("Sorry, no suggestions"));
    இல்லையெனில் எனில் (மொத்தம் > 0)
    {
	எனில் (மொத்தம் > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (long)sug.su_ga.ga_len);
    }
    இல்லையெனில்
    {
#ifdef FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	எனில் (cmdmsg_rl)
	    செய்தி_col = cmdline_width - 1;
#endif

	// List the suggestions.
	செய்தி_start();
	செய்தி_row = Rows - 1;	// for when 'cmdheight' > 1
	வரிகள்_இடது = Rows;	// avoid more prompt
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#ifdef FEAT_RIGHTLEFT
	எனில் (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#endif
	செய்தி_puts((எழுத்து *)IObuff);
	செய்தி_clr_eos();
	செய்தி_putchar('\n');

	செய்தி_scroll = TRUE;
	க்கு (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    முழு el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_word, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    எனில் (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%2d", i + 1);
#ifdef FEAT_RIGHTLEFT
	    எனில் (cmdmsg_rl)
		rl_mirror(IObuff);
#endif
	    செய்தி_puts((எழுத்து *)IObuff);

	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    செய்தி_puts((எழுத்து *)IObuff);

	    // The word may replace more than "su_badlen".
	    எனில் (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((எழுத்து *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		செய்தி_puts((எழுத்து *)IObuff);
	    }

	    எனில் (p_verbose > 0)
	    {
		// Add the score.
		எனில் (sps_flags & (SPS_DOUBLE | SPS_BEST))
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		இல்லையெனில்
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#ifdef FEAT_RIGHTLEFT
		எனில் (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#endif
		செய்தி_advance(30);
		செய்தி_puts((எழுத்து *)IObuff);
	    }
	    செய்தி_putchar('\n');
	}

#ifdef FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	செய்தி_col = 0;
#endif
	// Ask for choice.
	selected = prompt_for_எண்(&mouse_used);
	எனில் (mouse_used)
	    selected -= வரிகள்_இடது;
	வரிகள்_இடது = Rows;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	செய்தி_scroll = செய்தி_scroll_save;
    }

    எனில் (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_to);

	stp = &SUG(sug.su_ga, selected - 1);
	எனில் (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%s%.*s", stp->st_word,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_to = vim_strsave(IObuff);
	}
	இல்லையெனில்
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_to = vim_strsave(stp->st_word);
	}

	// Replace the word.
	p = alloc(STRLEN(வரி) - stp->st_orglen + stp->st_wordlen + 1);
	எனில் (p != வெற்று)
	{
	    முழு len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (முழு)(sug.su_badptr - வரி);
	    mch_memmove(p, வரி, c);
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
#எனில் defined(FEAT_PROP_POPUP)
	    எனில் (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#endif
	}
    }
    இல்லையெனில்
	curwin->w_cursor = prev_cursor;

    spell_find_cleanup(&sug);
skip:
    vim_free(வரி);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    வெறுமை
spell_suggest_list(
    garray_T	*gap,
    char_u	*word,
    முழு		maxcount,	// maximum nr of suggestions
    முழு		need_cap,	// 'spellcapcheck' matched
    முழு		interactive)
{
    suginfo_T	sug;
    முழு		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_find_suggest(word, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, அளவு(char_u *), sug.su_ga.ga_len + 1);
    எனில் (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	க்கு (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (unsigned)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    எனில் (wcopy == வெற்று)
		நிறுத்து;
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
    நிலையான வெறுமை
spell_find_suggest(
    char_u	*badptr,
    முழு		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    முழு		maxcount,
    முழு		banbadword,	// don't include badword in suggestions
    முழு		need_cap,	// word should start with capital
    முழு		interactive)
{
    hlf_T	attr = HLF_மொத்தம்;
    char_u	buf[MAXPATHL];
    char_u	*p;
    முழு		do_combine = FALSE;
    char_u	*sps_copy;
#ifdef FEAT_EVAL
    நிலையான முழு	கோவை_busy = FALSE;
#endif
    முழு		c;
    முழு		i;
    langp_T	*lp;
    முழு		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_POINTER(su);
    ga_init2(&su->su_ga, அளவு(suggest_T), 10);
    ga_init2(&su->su_sga, அளவு(suggest_T), 10);
    எனில் (*badptr == NUL)
	திரும்பு;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    எனில் (badlen != 0)
	su->su_badlen = badlen;
    இல்லையெனில்
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, வெற்று, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    எனில் (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (வெறுமை)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    எனில் (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    க்கு (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_ENTRY(curbuf->b_s.b_langp, i);
	எனில் (lp->lp_sallang != வெற்று)
	{
	    su->su_sallang = lp->lp_sallang;
	    நிறுத்து;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    எனில் (su->su_sallang != வெற்று)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    எனில் (!SPELL_ISUPPER(c) && attr == HLF_மொத்தம்)
    {
	make_case_word(su->su_badword, buf, WF_ONECAP);
	add_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    எனில் (banbadword)
	add_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    எனில் (sps_copy == வெற்று)
	திரும்பு;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    க்கு (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	எனில் (STRNCMP(buf, "expr:", 5) == 0)
	{
#ifdef FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    எனில் (!கோவை_busy)
	    {
		கோவை_busy = TRUE;
		spell_suggest_கோவை(su, buf + 5);
		கோவை_busy = FALSE;
	    }
#endif
	}
	இல்லையெனில் எனில் (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	இல்லையெனில் எனில் (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((எழுத்து *)buf + 8);
	இல்லையெனில் எனில் (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    எனில் (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    எனில் (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#ifdef FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    நிலையான வெறுமை
spell_suggest_கோவை(suginfo_T *su, char_u *கோவை)
{
    list_T	*list;
    listitem_T	*li;
    முழு		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_கோவை(su->su_badword, கோவை);
    எனில் (list != வெற்று)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_பொருட்கள்(list, li)
	    எனில் (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		எனில் (score >= 0 && score <= su->su_maxscore)
		    add_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (வெறுமை)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#endif

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    நிலையான வெறுமை
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	வரி[MAXWLEN * 2];
    char_u	*p;
    முழு		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((எழுத்து *)fname, "r");
    எனில் (fd == வெற்று)
    {
	semsg(_(e_cant_open_file_str), fname);
	திரும்பு;
    }

    // Read it line by line.
    போது (!vim_fgets(வரி, MAXWLEN * 2, fd) && !got_int)
    {
	வரி_breakcheck();

	p = vim_strchr(வரி, '/');
	எனில் (p == வெற்று)
	    தொடர்;	    // No Tab found, just skip the line.
	*p++ = NUL;
	எனில் (STRICMP(su->su_badword, வரி) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    க்கு (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    எனில் (captype(p, வெற்று) == 0)
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
    (வெறுமை)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    நிலையான வெறுமை
spell_suggest_intern(suginfo_T *su, முழு interactive)
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
    எனில் (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    எனில் ((sps_flags & SPS_FAST) == 0)
    {
	எனில் (sps_flags & SPS_BEST)
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
	எனில் (su->su_ga.ga_len < SUG_CLEAN_மொத்தம்(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    எனில் (su->su_ga.ga_len < SUG_CLEAN_மொத்தம்(su))
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
    எனில் (interactive && got_int)
    {
	(வெறுமை)vgetc();
	got_int = FALSE;
    }

    எனில் ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	எனில் (sps_flags & SPS_BEST)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(வெறுமை)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    நிலையான வெறுமை
spell_find_cleanup(suginfo_T *su)
{
    முழு		i;

    // Free the suggestions.
    க்கு (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_word);
    ga_clear(&su->su_ga);
    க்கு (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_word);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    நிலையான வெறுமை
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    முழு		c;
    char_u	word[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    எனில் (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
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
#எனில் 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T total;
proftime_T times[STATE_FINAL + 1];
long counts[STATE_FINAL + 1];

    நிலையான வெறுமை
prof_init(வெறுமை)
{
    க்கு (முழு i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_start(&current);
    profile_start(&total);
}

// call before changing state
    நிலையான வெறுமை
prof_store(state_T state)
{
    profile_end(&current);
    profile_add(&times[state], &current);
    ++counts[state];
    profile_start(&current);
}
# define PROF_STORE(state) prof_store(state);

    நிலையான வெறுமை
prof_report(எழுத்து *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_end(&total);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_செய்தி(&total));
    க்கு (முழு i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_செய்தி(&times[i]), counts[i]);
    fclose(fd);
}
#இல்லையெனில்
# define PROF_STORE(state)
#endif

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    நிலையான வெறுமை
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    முழு		n;
    char_u	*p;
    முழு		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (முழு)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (வெறுமை)spell_casefold(curwin, p, (முழு)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (முழு)STRLEN(su->su_badptr);
    எனில் (n < MAXWLEN)
	fword[n] = NUL;

    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	எனில் (lp->lp_slang->sl_fbyts == வெற்று)
	    தொடர்;

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
#define TRY_DEEPER(su, அடுக்கு, depth, add) \
       ((depth) < MAXWLEN - 1 && (அடுக்கு)[depth].ts_score + (add) < (su)->su_maxscore)

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
    நிலையான வெறுமை
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    முழு		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	அடுக்கு[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    முழு		newscore;
    முழு		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    முழு		depth;
    முழு		c, c2, c3;
    முழு		n = 0;
    முழு		flags;
    garray_T	*gap;
    idx_T	arridx;
    முழு		len;
    char_u	*p;
    fromto_T	*ftp;
    முழு		fl = 0, tl;
    முழு		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    முழு		fword_ends;
    முழு		goodword_ends;
#ifdef DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#endif
    முழு		breakcheckcount = 1000;
#ifdef FEAT_RELTIME
    proftime_T	time_limit;
#endif
    முழு		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    depth = 0;
    sp = &அடுக்கு[0];
    CLEAR_POINTER(sp);
    sp->ts_curi = 1;

    எனில் (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = வெற்று;
	pidxs = வெற்று;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_START;
    }
    இல்லையெனில்
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	எனில் (pbyts != வெற்று)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	இல்லையெனில்
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
    எனில் (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &time_limit);
#endif

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    போது (depth >= 0 && !got_int)
    {
	sp = &அடுக்கு[depth];
	தேர்வு (sp->ts_state)
	{
	நிலை STATE_START:
	நிலை STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    எனில் (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		க்கு (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (முழு)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		எனில் (depth < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (முழு)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    எனில் (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    இல்லையெனில்
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#ifdef DEBUG_TRIEWALK
		    sprintf(changename[depth], "prefix");
#endif
		    go_deeper(அடுக்கு, depth, 0);
		    ++depth;
		    sp = &அடுக்கு[depth];
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
		நிறுத்து;
	    }

	    எனில் (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		நிறுத்து;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (முழு)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    எனில் (flags & WF_NOSUGGEST)
		நிறுத்து;

	    fword_ends = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    எனில் (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != வெற்று)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = அடுக்கு[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		க்கு (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		எனில் (c > 0)
		{
		    c = valid_word_prefix(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    எனில் (c == 0)
			நிறுத்து;

		    // Use the WF_RARE flag for a rare prefix.
		    எனில் (c & WF_RAREPFX)
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
	    எனில் (sp->ts_complen == sp->ts_compsplit && fword_ends
						     && (flags & WF_NEEDCOMP))
		goodword_ends = FALSE;
	    இல்லையெனில்
		goodword_ends = TRUE;

	    p = வெற்று;
	    compound_ok = TRUE;
	    எனில் (sp->ts_complen > sp->ts_compsplit)
	    {
		எனில் (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    எனில் (sp->ts_fidx - sp->ts_splitfidx
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
			எனில் (newscore <= su->su_maxscore)
			    add_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			நிறுத்து;
		    }
		}
		இல்லையெனில்
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    எனில் (((unsigned)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			நிறுத்து;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    எனில் (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			நிறுத்து;

		    compflags[sp->ts_complen] = ((unsigned)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    எனில் (match_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    எனில் (compound_ok)
		    {
			p = preword;
			போது (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			எனில் (fword_ends && !can_compound(slang, p,
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
	    எனில் (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    இல்லையெனில் எனில் (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    இல்லையெனில்
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		எனில் ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		எனில் (p != வெற்று && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_word(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    எனில் (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		எனில் (flags & WF_BANNED)
		{
		    add_banned(su, preword + sp->ts_prewordlen);
		    நிறுத்து;
		}
		எனில் ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    எனில் (slang->sl_compprog == வெற்று)
			நிறுத்து;
		    // the word so far was banned but we may try compounding
		    goodword_ends = FALSE;
		}
	    }

	    newscore = 0;
	    எனில் (!soundfold)	// soundfold words don't have flags
	    {
		எனில் ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		எனில் (flags & WF_RARE)
		    newscore += SCORE_RARE;

		எனில் (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, வெற்று)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    எனில் (fword_ends
		    && goodword_ends
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#ifdef DEBUG_TRIEWALK
		எனில் (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    முழு	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    க்கு (j = 0; j < depth; ++j)
			smsg("%s", changename[j]);
		}
#endif
		எனில் (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    add_sound_suggest(su, preword, sp->ts_score, lp);
		}
		இல்லையெனில் எனில் (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_BACK(fword, p);
		    எனில் (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_BACK(preword, p);
			எனில் (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    எனில் (score <= su->su_maxscore)
		    {
			add_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			எனில் (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, வெற்று);
			    எனில் (c == 0 || c == WF_ALLCAP)
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
	    எனில் ((sp->ts_fidx >= sp->ts_fidxtry || fword_ends)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		முழு	try_compound;
		முழு	try_split;

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
		எனில் (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != வெற்று
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
		எனில் (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		இல்லையெனில் எனில் (!fword_ends
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		இல்லையெனில்
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		எனில் (try_split || try_compound)
		{
		    எனில் (!try_compound && (!fword_ends || !goodword_ends))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			எனில் (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    நிறுத்து;
			p = preword;
			போது (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			எனில் (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    நிறுத்து;

			எனில் (slang->sl_nosplitsugs)
			    newscore += SCORE_SPLIT_NO;
			இல்லையெனில்
			    newscore += SCORE_SPLIT;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    எனில் (TRY_DEEPER(su, அடுக்கு, depth, newscore))
		    {
			go_deeper(அடுக்கு, depth, newscore);
#ifdef DEBUG_TRIEWALK
			எனில் (!try_compound && !fword_ends)
			    sprintf(changename[depth], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			இல்லையெனில்
			    sprintf(changename[depth], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#endif
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++depth;
			sp = &அடுக்கு[depth];

			// Append a space to preword when splitting.
			எனில் (!try_compound && !fword_ends)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			எனில் (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_ends)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_ends)
			{
			    முழு	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    எனில் (fword_ends)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    இல்லையெனில்
				sp->ts_score -= SCORE_SPLIT - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			எனில் (try_compound)
			    ++sp->ts_complen;
			இல்லையெனில்
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			எனில் (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			இல்லையெனில்
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			எனில் (pbyts != வெற்று)
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
	    நிறுத்து;

	நிலை STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_START;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    நிறுத்து;

	நிலை STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    எனில் (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		நிறுத்து;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	நிலை STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    எனில் (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		எனில் (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		இல்லையெனில்
		    sp->ts_state = STATE_FINAL;
	    }
	    இல்லையெனில்
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		எனில் (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		இல்லையெனில்
		    newscore = SCORE_SUBST;
		எனில் ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, அடுக்கு, depth, newscore))
		{
		    go_deeper(அடுக்கு, depth, newscore);
#ifdef DEBUG_TRIEWALK
		    எனில் (newscore > 0)
			sprintf(changename[depth], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    இல்லையெனில்
			sprintf(changename[depth], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#endif
		    ++depth;
		    sp = &அடுக்கு[depth];
		    எனில் (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    எனில் (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    எனில் (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			எனில் (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			இல்லையெனில் எனில் (sp->ts_isdiff == DIFF_செருகு
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			எனில் (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    எனில் (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				எனில் (enc_utf8
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
				இல்லையெனில் எனில் (!soundfold
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
			    இல்லையெனில் எனில் (sp->ts_isdiff == DIFF_செருகு
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				எனில் (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				இல்லையெனில்
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_BACK(tword, p);
				    எனில் (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    இல்லையெனில்
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			எனில் (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    நிறுத்து;

	நிலை STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    எனில் (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		நிறுத்து;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    எனில் (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    இல்லையெனில்
		newscore = SCORE_DEL;
	    எனில் (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, அடுக்கு, depth, newscore))
	    {
		go_deeper(அடுக்கு, depth, newscore);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#endif
		++depth;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		அடுக்கு[depth].ts_flags |= TSF_DIDDEL;
		அடுக்கு[depth].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		எனில் (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    அடுக்கு[depth].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    எனில் (enc_utf8 && utf_iscomposing(c))
			அடுக்கு[depth].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    இல்லையெனில் எனில் (c == mb_ptr2char(fword + அடுக்கு[depth].ts_fidx))
			அடுக்கு[depth].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		இல்லையெனில்
		{
		    ++அடுக்கு[depth].ts_fidx;
		    எனில் (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			அடுக்கு[depth].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		நிறுத்து;
	    }
	    // FALLTHROUGH

	நிலை STATE_INS_PREP:
	    எனில் (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP;
		நிறுத்து;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    க்கு (;;)
	    {
		எனில் (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_SWAP;
		    நிறுத்து;
		}
		எனில் (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    நிறுத்து;
		}
		++sp->ts_curi;
	    }
	    நிறுத்து;

	    // FALLTHROUGH

	நிலை STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    எனில் (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP;
		நிறுத்து;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    எனில் (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		நிறுத்து;
	    }
	    c = byts[n];
	    எனில் (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    இல்லையெனில்
		newscore = SCORE_INS;
	    எனில் (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, அடுக்கு, depth, newscore))
	    {
		go_deeper(அடுக்கு, depth, newscore);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#endif
		++depth;
		sp = &அடுக்கு[depth];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		எனில் (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    எனில் (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_செருகு;
		    }
		}
		இல்லையெனில்
		    fl = 1;
		எனில் (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    எனில் (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    நிறுத்து;

	நிலை STATE_SWAP:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    எனில் (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		நிறுத்து;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    எனில் (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		நிறுத்து;
	    }

	    எனில் (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		எனில் (p[n] == NUL)
		    c2 = NUL;
		இல்லையெனில் எனில் (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		இல்லையெனில்
		    c2 = mb_ptr2char(p + n);
	    }
	    இல்லையெனில்
	    {
		எனில் (p[1] == NUL)
		    c2 = NUL;
		இல்லையெனில் எனில் (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		இல்லையெனில்
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    எனில் (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		நிறுத்து;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    எனில் (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		நிறுத்து;
	    }
	    எனில் (c2 != NUL && TRY_DEEPER(su, அடுக்கு, depth, SCORE_SWAP))
	    {
		go_deeper(அடுக்கு, depth, SCORE_SWAP);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++depth;
		எனில் (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		இல்லையெனில்
		{
		    p[0] = c2;
		    p[1] = c;
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    இல்லையெனில்
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    நிறுத்து;

	நிலை STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    எனில் (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    இல்லையெனில்
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	நிலை STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    எனில் (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		எனில் (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		இல்லையெனில்
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    இல்லையெனில்
	    {
		c = *p;
		c2 = p[1];
		எனில் (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		இல்லையெனில்
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    எனில் (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		நிறுத்து;
	    }
	    எனில் (TRY_DEEPER(su, அடுக்கு, depth, SCORE_SWAP3))
	    {
		go_deeper(அடுக்கு, depth, SCORE_SWAP3);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++depth;
		எனில் (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		இல்லையெனில்
		{
		    p[0] = p[2];
		    p[2] = c;
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    இல்லையெனில்
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    நிறுத்து;

	நிலை STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    எனில் (has_mbyte)
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
	    இல்லையெனில்
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    எனில் (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		நிறுத்து;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    எனில் (TRY_DEEPER(su, அடுக்கு, depth, SCORE_SWAP3))
	    {
		go_deeper(அடுக்கு, depth, SCORE_SWAP3);
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
		எனில் (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		இல்லையெனில்
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    இல்லையெனில்
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    நிறுத்து;

	நிலை STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    எனில் (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    இல்லையெனில்
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    எனில் (TRY_DEEPER(su, அடுக்கு, depth, SCORE_SWAP3))
	    {
		go_deeper(அடுக்கு, depth, SCORE_SWAP3);
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
		எனில் (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		இல்லையெனில்
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    இல்லையெனில்
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    நிறுத்து;

	நிலை STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    எனில் (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    இல்லையெனில்
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	நிலை STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    எனில் ((lp->lp_replang == வெற்று && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		நிறுத்து;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    எனில் (soundfold)
		sp->ts_curi = slang->sl_repsal_first[fword[sp->ts_fidx]];
	    இல்லையெனில்
		sp->ts_curi = lp->lp_replang->sl_rep_first[fword[sp->ts_fidx]];

	    எனில் (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		நிறுத்து;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	நிலை STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    எனில் (soundfold)
		gap = &slang->sl_repsal;
	    இல்லையெனில்
		gap = &lp->lp_replang->sl_rep;
	    போது (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_data + sp->ts_curi++;
		எனில் (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    நிறுத்து;
		}
		எனில் (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, அடுக்கு, depth, SCORE_REP))
		{
		    go_deeper(அடுக்கு, depth, SCORE_REP);
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
		    fl = (முழு)STRLEN(ftp->ft_from);
		    tl = (முழு)STRLEN(ftp->ft_to);
		    எனில் (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_to, tl);
		    அடுக்கு[depth].ts_fidxtry = sp->ts_fidx + tl;
		    அடுக்கு[depth].ts_tcharlen = 0;
		    நிறுத்து;
		}
	    }

	    எனில் (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    நிறுத்து;

	நிலை STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    எனில் (soundfold)
		gap = &slang->sl_repsal;
	    இல்லையெனில்
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_data + sp->ts_curi - 1;
	    fl = (முழு)STRLEN(ftp->ft_from);
	    tl = (முழு)STRLEN(ftp->ft_to);
	    p = fword + sp->ts_fidx;
	    எனில் (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    நிறுத்து;

	இயல்பு:
	    // Did all possible states at this level, go up one level.
	    --depth;

	    எனில் (depth >= 0 && அடுக்கு[depth].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    எனில் (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#ifdef FEAT_RELTIME
		எனில் (spell_suggest_timeout > 0
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
    நிலையான வெறுமை
go_deeper(trystate_T *அடுக்கு, முழு depth, முழு score_add)
{
    அடுக்கு[depth + 1] = அடுக்கு[depth];
    அடுக்கு[depth + 1].ts_state = STATE_START;
    அடுக்கு[depth + 1].ts_score = அடுக்கு[depth].ts_score + score_add;
    அடுக்கு[depth + 1].ts_curi = 1;	// start just after length byte
    அடுக்கு[depth + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    நிலையான வெறுமை
find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    முழு		depth;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    முழு		round[MAXWLEN];
    முழு		fwordidx[MAXWLEN];
    முழு		uwordidx[MAXWLEN];
    முழு		kwordlen[MAXWLEN];

    முழு		flen, ulen;
    முழு		l;
    முழு		len;
    முழு		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    எனில் (byts == வெற்று)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	திரும்பு;
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
    போது (depth >= 0)
    {
	எனில் (fword[fwordidx[depth]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    எனில் (byts[arridx[depth] + 1] == 0)
	    {
		kword[kwordlen[depth]] = NUL;
		திரும்பு;
	    }

	    // kword is getting too long, continue one level up
	    --depth;
	}
	இல்லையெனில் எனில் (++round[depth] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --depth;
	}
	இல்லையெனில்
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    எனில் (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[depth]);
		ulen = MB_CPTR2LEN(uword + uwordidx[depth]);
	    }
	    இல்லையெனில்
		ulen = flen = 1;
	    எனில் (round[depth] == 1)
	    {
		p = fword + fwordidx[depth];
		l = flen;
	    }
	    இல்லையெனில்
	    {
		p = uword + uwordidx[depth];
		l = ulen;
	    }

	    க்கு (tryidx = arridx[depth]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
		போது (lo < hi)
		{
		    m = (lo + hi) / 2;
		    எனில் (byts[m] > c)
			hi = m - 1;
		    இல்லையெனில் எனில் (byts[m] < c)
			lo = m + 1;
		    இல்லையெனில்
		    {
			lo = hi = m;
			நிறுத்து;
		    }
		}

		// Stop if there is no matching byte.
		எனில் (hi < lo || byts[lo] != c)
		    நிறுத்து;

		// Continue at the child (if there is one).
		tryidx = idxs[lo];
	    }

	    எனில் (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		எனில் (round[depth] == 1)
		{
		    STRNCPY(kword + kwordlen[depth], fword + fwordidx[depth],
									flen);
		    kwordlen[depth + 1] = kwordlen[depth] + flen;
		}
		இல்லையெனில்
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
    நிலையான வெறுமை
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    முழு		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    முழு		score;
    முழு		lpi;

    எனில் (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	திரும்பு;

    // Use the sound-folding of the first language that supports it.
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	எனில் (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    க்கு (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		எனில் (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_word = vim_strsave(stp->st_word);
		    எனில் (sstp->st_word != வெற்று)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    நிறுத்து;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    நிலையான வெறுமை
score_combine(suginfo_T *su)
{
    முழு		i;
    முழு		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    முழு		round;
    முழு		lpi;
    slang_T	*slang = வெற்று;

    // Add the alternate score to su_ga.
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	எனில் (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    க்கு (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		எனில் (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		இல்லையெனில்
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    நிறுத்து;
	}
    }

    எனில் (slang == வெற்று)	// Using "double" without sound folding.
    {
	(வெறுமை)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	திரும்பு;
    }

    // Add the alternate score to su_sga.
    க்கு (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_word);
	எனில் (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	இல்லையெனில்
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (வெறுமை)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (வெறுமை)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, அளவு(suginfo_T), 1);
    எனில் (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	திரும்பு;

    stp = &SUG(ga, 0);
    க்கு (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	க்கு (round = 1; round <= 2; ++round)
	{
	    gap = round == 1 ? &su->su_ga : &su->su_sga;
	    எனில் (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_word;
		க்கு (j = 0; j < ga.ga_len; ++j)
		    எனில் (STRCMP(stp[j].st_word, p) == 0)
			நிறுத்து;
		எனில் (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		இல்லையெனில்
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    எனில் (ga.ga_len > su->su_maxcount)
    {
	க்கு (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_word);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    நிலையான முழு
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
    முழு		lendiff;

    lendiff = (முழு)(su->su_badlen - stp->st_orglen);
    எனில் (lendiff >= 0)
	pbad = badsound;
    இல்லையெனில்
    {
	// soundfold the bad word with more characters following
	(வெறுமை)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	எனில் (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_word) == NUL)
	    க்கு (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    எனில் (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_word);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    இல்லையெனில்
	pgood = stp->st_word;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    திரும்பு soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
வகைவரை கட்டமைப்பு
{
    short	sft_score;	// lowest score used
    char_u	sft_word[1];    // soundfolded word, actually longer
} sftword_T;

நிலையான sftword_T dumsft;
#define HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_word - (char_u *)&dumsft)))
#define HI2SFT(hi)     HIKEY2SFT((hi)->hi_சாவி)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    நிலையான வெறுமை
suggest_try_soundalike_prep(வெறுமை)
{
    langp_T	*lp;
    முழு		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	எனில் (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != வெற்று)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    நிலையான வெறுமை
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    முழு		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	எனில் (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != வெற்று)
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
    நிலையான வெறுமை
suggest_try_soundalike_finish(வெறுமை)
{
    langp_T	*lp;
    முழு		lpi;
    slang_T	*slang;
    முழு		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    க்கு (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	எனில் (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != வெற்று)
	{
	    // Free the info about handled words.
	    todo = (முழு)slang->sl_sounddone.ht_used;
	    FOR_ALL_HASHTAB_பொருட்கள்(&slang->sl_sounddone, hi, todo)
		எனில் (!HASHITEM_EMPTY(hi))
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
    நிலையான வெறுமை
add_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    முழு		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    முழு		sfwordnr;
    char_u	*nrline;
    முழு		orgnr;
    char_u	theword[MAXWLEN];
    முழு		i;
    முழு		wlen;
    char_u	*byts;
    idx_T	*idxs;
    முழு		n;
    முழு		wordcount;
    முழு		wc;
    முழு		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    முழு		bc, gc;
    முழு		limit;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    எனில் (HASHITEM_EMPTY(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_word) + STRLEN(goodword) + 1);
	எனில் (sft != வெற்று)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_word, goodword);
	    hash_add_item(&slang->sl_sounddone, hi, sft->sft_word, hash);
	}
    }
    இல்லையெனில்
    {
	sft = HI2SFT(hi);
	எனில் (score >= sft->sft_score)
	    திரும்பு;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_find(slang, goodword);
    எனில் (sfwordnr < 0)
    {
	internal_error("add_sound_suggest()");
	திரும்பு;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    போது (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	க்கு (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    எனில் (wordcount == orgnr && byts[n + 1] == NUL)
		நிறுத்து;	// found end of word

	    எனில் (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    க்கு ( ; byts[n + i] == NUL; ++i)
		எனில் (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    செல் badword;
		}

	    // One of the siblings must have the word.
	    க்கு ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		எனில் (wordcount + wc > orgnr)
		    நிறுத்து;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	க்கு (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    முழு		flags = (முழு)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    எனில் (flags & WF_NOSUGGEST)
		தொடர்;

	    எனில் (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, theword, cword);
		p = cword;
	    }
	    இல்லையெனில்
	    {
		flags |= su->su_badflags;
		எனில் ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_word(theword, cword, flags);
		    p = cword;
		}
		இல்லையெனில்
		    p = theword;
	    }

	    // Add the suggestion.
	    எனில் (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		எனில் (score <= su->su_maxscore)
		    add_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    இல்லையெனில்
	    {
		// Add a penalty for words in another region.
		எனில் ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		இல்லையெனில்
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		எனில் (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    எனில் (!SPELL_ISUPPER(bc)
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
		எனில் (limit > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		இல்லையெனில்
		    goodscore += spell_edit_score_limit(slang, su->su_badword,
								    p, limit);

		// When going over the limit don't bother to do the rest.
		எனில் (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    எனில் (goodscore <= su->su_sfmaxscore)
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
    நிலையான முழு
soundfold_find(slang_T *slang, char_u *word)
{
    idx_T	arridx = 0;
    முழு		len;
    முழு		wlen = 0;
    முழு		c;
    char_u	*ptr = word;
    char_u	*byts;
    idx_T	*idxs;
    முழு		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    க்கு (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	எனில் (byts[arridx] == NUL)
	{
	    எனில் (c == NUL)
		நிறுத்து;

	    // Skip over the zeros, there can be several.
	    போது (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    எனில் (len == 0)
		திரும்பு -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	எனில் (c == NUL)
	    திரும்பு -1;

	// Perform a binary search in the list of accepted bytes.
	எனில் (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	போது (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    எனில் (--len == 0)	// end of the bytes, didn't find it
		திரும்பு -1;
	}
	எனில் (byts[arridx] != c)	// didn't find the byte
	    திரும்பு -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	எனில் (c == ' ')
	    போது (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    திரும்பு wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    நிலையான முழு
similar_chars(slang_T *slang, முழு c1, முழு c2)
{
    முழு		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    எனில் (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	எனில் (HASHITEM_EMPTY(hi))
	    m1 = 0;
	இல்லையெனில்
	    m1 = mb_ptr2char(hi->hi_சாவி + STRLEN(hi->hi_சாவி) + 1);
    }
    இல்லையெனில்
	m1 = slang->sl_map_array[c1];
    எனில் (m1 == 0)
	திரும்பு FALSE;


    எனில் (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	எனில் (HASHITEM_EMPTY(hi))
	    m2 = 0;
	இல்லையெனில்
	    m2 = mb_ptr2char(hi->hi_சாவி + STRLEN(hi->hi_சாவி) + 1);
    }
    இல்லையெனில்
	m2 = slang->sl_map_array[c2];

    திரும்பு m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    நிலையான வெறுமை
add_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    முழு		badlenarg,	// len of bad word replaced with "goodword"
    முழு		score,
    முழு		altscore,
    முழு		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    முழு		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    முழு		goodlen;	// len of goodword changed
    முழு		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    முழு		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    க்கு (;;)
    {
	goodlen = (முழு)(pgood - goodword);
	badlen = (முழு)(pbad - su->su_badptr);
	எனில் (goodlen <= 0 || badlen <= 0)
	    நிறுத்து;
	MB_PTR_BACK(goodword, pgood);
	MB_PTR_BACK(su->su_badptr, pbad);
	எனில் (has_mbyte)
	{
	    எனில் (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		நிறுத்து;
	}
	இல்லையெனில் எனில் (*pgood != *pbad)
		நிறுத்து;
    }

    எனில் (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	திரும்பு;

    எனில் (gap->ga_len == 0)
	i = -1;
    இல்லையெனில்
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	க்கு (i = gap->ga_len; --i >= 0; ++stp)
	    எனில் (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_word, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		எனில் (stp->st_slang == வெற்று)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		எனில் (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    எனில் (had_bonus)
			rescore_one(su, stp);
		    இல்லையெனில்
		    {
			new_sug.st_word = stp->st_word;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		எனில் (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		நிறுத்து;
	    }
    }

    எனில் (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_word = vim_strnsave(goodword, goodlen);
	எனில் (stp->st_word != வெற்று)
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
	    எனில் (gap->ga_len > SUG_MAX_மொத்தம்(su))
	    {
		எனில் (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_மொத்தம்(su));
		இல்லையெனில்
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_மொத்தம்(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    நிலையான வெறுமை
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    முழு		i;
    char_u	longword[MAXWLEN + 1];
    முழு		len;
    hlf_T	attr;

    எனில் (gap->ga_len == 0)
	திரும்பு;
    stp = &SUG(*gap, 0);
    க்கு (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_word, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_மொத்தம்;
	(வெறுமை)spell_check(curwin, longword, &attr, வெற்று, FALSE);
	எனில் (attr != HLF_மொத்தம்)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_word);
	    --gap->ga_len;
	    எனில் (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       அளவு(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    நிலையான வெறுமை
add_banned(
    suginfo_T	*su,
    char_u	*word)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(word);
    hi = hash_lookup(&su->su_banned, word, hash);
    எனில் (!HASHITEM_EMPTY(hi))		// already present
	திரும்பு;
    s = vim_strsave(word);
    எனில் (s != வெற்று)
	hash_add_item(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    நிலையான வெறுமை
rescore_suggestions(suginfo_T *su)
{
    முழு		i;

    எனில் (su->su_sallang != வெற்று)
	க்கு (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    நிலையான வெறுமை
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    எனில் (slang != வெற்று && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	எனில் (slang == su->su_sallang)
	    p = su->su_sal_badword;
	இல்லையெனில்
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	எனில் (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

நிலையான முழு sug_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    நிலையான முழு
sug_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    முழு		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    எனில் (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	எனில் (n == 0)
	    n = STRICMP(p1->st_word, p2->st_word);
    }
    திரும்பு n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    நிலையான முழு
cleanup_suggestions(
    garray_T	*gap,
    முழு		maxscore,
    முழு		keep)		// nr of suggestions to keep
{
    எனில் (gap->ga_len <= 0)
	திரும்பு maxscore;

    // Sort the list.
    qsort(gap->ga_data, (size_t)gap->ga_len, அளவு(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    எனில் (gap->ga_len > keep)
    {
	முழு		i;
	suggest_T   *stp = &SUG(*gap, 0);

	க்கு (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_word);
	gap->ga_len = keep;
	எனில் (keep >= 1)
	    திரும்பு stp[keep - 1].st_score;
    }
    திரும்பு maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    நிலையான முழு
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    முழு		goodlen;
    முழு		badlen;
    முழு		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    முழு		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    எனில் ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	எனில் ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    திரும்பு SCORE_DEL;
	எனில் (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    திரும்பு SCORE_MAXMAX;

	எனில் (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	இல்லையெனில்
	{
	    score = 2 * SCORE_DEL / 3;
	    எனில் (*badsound == '*')
		++badsound;
	    இல்லையெனில்
		++goodsound;
	}
    }

    goodlen = (முழு)STRLEN(goodsound);
    badlen = (முழு)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    எனில் (n < -2 || n > 2)
	திரும்பு SCORE_MAXMAX;

    எனில் (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    இல்லையெனில்
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    போது (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    தேர்வு (n)
    {
	நிலை -2:
	நிலை 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    போது (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    எனில் (STRCMP(pl + 1, ps) == 0)
		திரும்பு score + SCORE_DEL * 2;

	    // Failed to compare.
	    நிறுத்து;

	நிலை -1:
	நிலை 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    போது (*pl2 == *ps2)
	    {
		எனில் (*pl2 == NUL)	// reached the end
		    திரும்பு score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    எனில் (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		திரும்பு score + SCORE_DEL + SCORE_SWAP;

	    // 3: delete then substitute, then the rest must be equal
	    எனில் (STRCMP(pl2 + 1, ps2 + 1) == 0)
		திரும்பு score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    எனில் (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		போது (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		எனில் (STRCMP(pl2 + 1, ps2) == 0)
		    திரும்பு score + SCORE_SWAP + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    போது (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    எனில் (STRCMP(pl2 + 1, ps2) == 0)
		திரும்பு score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    நிறுத்து;

	நிலை 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    எனில் (*pl == NUL)
		திரும்பு score;

	    // 2: swap
	    எனில் (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		போது (*pl2 == *ps2)
		{
		    எனில் (*pl2 == NUL)	// reached the end
			திரும்பு score + SCORE_SWAP;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		எனில் (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    திரும்பு score + SCORE_SWAP + SCORE_SWAP;

		// 4: swap and substitute
		எனில் (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    திரும்பு score + SCORE_SWAP + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    போது (*pl2 == *ps2)
	    {
		எனில் (*pl2 == NUL)	// reached the end
		    திரும்பு score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    எனில் (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		திரும்பு score + SCORE_SUBST + SCORE_SWAP;

	    // 7: substitute and substitute
	    எனில் (STRCMP(pl2 + 1, ps2 + 1) == 0)
		திரும்பு score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    போது (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    எனில் (STRCMP(pl2 + 1, ps2) == 0)
		திரும்பு score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    போது (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    எனில் (STRCMP(pl2, ps2 + 1) == 0)
		திரும்பு score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    நிறுத்து;
    }

    திரும்பு SCORE_MAXMAX;
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
    நிலையான முழு
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    முழு		*cnt;
    முழு		badlen, goodlen;	// lengths including NUL
    முழு		j, i;
    முழு		t;
    முழு		bc, gc;
    முழு		pbc, pgc;
    char_u	*p;
    முழு		wbadword[MAXWLEN];
    முழு		wgoodword[MAXWLEN];

    எனில் (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	க்கு (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	க்கு (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    இல்லையெனில்
    {
	badlen = (முழு)STRLEN(badword) + 1;
	goodlen = (முழு)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#define CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(முழு, (badlen + 1) * (goodlen + 1));
    எனில் (cnt == வெற்று)
	திரும்பு 0;	// out of memory

    CNT(0, 0) = 0;
    க்கு (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    க்கு (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	க்கு (j = 1; j <= goodlen; ++j)
	{
	    எனில் (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    இல்லையெனில்
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    எனில் (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    இல்லையெனில்
	    {
		// Use a better score when there is only a case difference.
		எனில் (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		இல்லையெனில்
		{
		    // For a similar character use SCORE_SIMILAR.
		    எனில் (slang != வெற்று
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    இல்லையெனில்
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		எனில் (i > 1 && j > 1)
		{
		    எனில் (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    இல்லையெனில்
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    எனில் (bc == pgc && pbc == gc)
		    {
			t = SCORE_SWAP + CNT(i - 2, j - 2);
			எனில் (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		எனில் (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		எனில் (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    திரும்பு i;
}

வகைவரை கட்டமைப்பு
{
    முழு		badi;
    முழு		goodi;
    முழு		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    நிலையான முழு
spell_edit_score_limit(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    முழு		limit)
{
    limitscore_T    அடுக்கு[10];		// allow for over 3 * 2 edits
    முழு		    stackidx;
    முழு		    bi, gi;
    முழு		    bi2, gi2;
    முழு		    bc, gc;
    முழு		    score;
    முழு		    score_off;
    முழு		    minscore;
    முழு		    round;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    எனில் (has_mbyte)
	திரும்பு spell_edit_score_limit_w(slang, badword, goodword, limit);

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

    க்கு (;;)
    {
	// Skip over an equal part, score remains the same.
	க்கு (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    எனில் (bc != gc)	// stop at a char that's different
		நிறுத்து;
	    எனில் (bc == NUL)	// both words end
	    {
		எனில் (score < minscore)
		    minscore = score;
		செல் எடு;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	எனில் (gc == NUL)    // goodword ends, delete badword chars
	{
	    செய்
	    {
		எனில் ((score += SCORE_DEL) >= minscore)
		    செல் எடு;	    // do next alternative
	    } போது (badword[++bi] != NUL);
	    minscore = score;
	}
	இல்லையெனில் எனில் (bc == NUL) // badword ends, insert badword chars
	{
	    செய்
	    {
		எனில் ((score += SCORE_INS) >= minscore)
		    செல் எடு;	    // do next alternative
	    } போது (goodword[++gi] != NUL);
	    minscore = score;
	}
	இல்லையெனில்			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    க்கு (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		எனில் (score_off < minscore)
		{
		    எனில் (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			போது (goodword[gi2] == badword[bi2])
			{
			    எனில் (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				நிறுத்து;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    இல்லையெனில்
		    {
			// try deleting/inserting a character later
			அடுக்கு[stackidx].badi = bi + 1 - round;
			அடுக்கு[stackidx].goodi = gi + round;
			அடுக்கு[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    எனில் (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		எனில் (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    தொடர்;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    எனில் (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    இல்லையெனில்
	    {
		// For a similar character use SCORE_SIMILAR.
		எனில் (slang != வெற்று
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		இல்லையெனில்
		    score += SCORE_SUBST;
	    }

	    எனில் (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		தொடர்;
	    }
	}
எடு:
	// Get here to try the next alternative, pop it from the stack.
	எனில் (stackidx == 0)		// stack is empty, finished
	    நிறுத்து;

	// pop an item from the stack
	--stackidx;
	gi = அடுக்கு[stackidx].goodi;
	bi = அடுக்கு[stackidx].badi;
	score = அடுக்கு[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    எனில் (minscore > limit)
	திரும்பு SCORE_MAXMAX;
    திரும்பு minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    நிலையான முழு
spell_edit_score_limit_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    முழு		limit)
{
    limitscore_T    அடுக்கு[10];		// allow for over 3 * 2 edits
    முழு		    stackidx;
    முழு		    bi, gi;
    முழு		    bi2, gi2;
    முழு		    bc, gc;
    முழு		    score;
    முழு		    score_off;
    முழு		    minscore;
    முழு		    round;
    char_u	    *p;
    முழு		    wbadword[MAXWLEN];
    முழு		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    க்கு (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    க்கு (p = goodword; *p != NUL; )
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

    க்கு (;;)
    {
	// Skip over an equal part, score remains the same.
	க்கு (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    எனில் (bc != gc)	// stop at a char that's different
		நிறுத்து;
	    எனில் (bc == NUL)	// both words end
	    {
		எனில் (score < minscore)
		    minscore = score;
		செல் எடு;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	எனில் (gc == NUL)    // goodword ends, delete badword chars
	{
	    செய்
	    {
		எனில் ((score += SCORE_DEL) >= minscore)
		    செல் எடு;	    // do next alternative
	    } போது (wbadword[++bi] != NUL);
	    minscore = score;
	}
	இல்லையெனில் எனில் (bc == NUL) // badword ends, insert badword chars
	{
	    செய்
	    {
		எனில் ((score += SCORE_INS) >= minscore)
		    செல் எடு;	    // do next alternative
	    } போது (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	இல்லையெனில்			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    க்கு (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		எனில் (score_off < minscore)
		{
		    எனில் (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			போது (wgoodword[gi2] == wbadword[bi2])
			{
			    எனில் (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				நிறுத்து;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    இல்லையெனில்
		    {
			// try deleting a character from badword later
			அடுக்கு[stackidx].badi = bi + 1 - round;
			அடுக்கு[stackidx].goodi = gi + round;
			அடுக்கு[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    எனில் (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		எனில் (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    தொடர்;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    எனில் (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    இல்லையெனில்
	    {
		// For a similar character use SCORE_SIMILAR.
		எனில் (slang != வெற்று
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		இல்லையெனில்
		    score += SCORE_SUBST;
	    }

	    எனில் (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		தொடர்;
	    }
	}
எடு:
	// Get here to try the next alternative, pop it from the stack.
	எனில் (stackidx == 0)		// stack is empty, finished
	    நிறுத்து;

	// pop an item from the stack
	--stackidx;
	gi = அடுக்கு[stackidx].goodi;
	bi = அடுக்கு[stackidx].badi;
	score = அடுக்கு[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    எனில் (minscore > limit)
	திரும்பு SCORE_MAXMAX;
    திரும்பு minscore;
}
#endif  // FEAT_SPELL
