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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SPELL)

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
પ્રકારવ્યાખ્યા સંરચના suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    પૂર્ણાંક		su_maxcount;	    // max. number of suggestions displayed
    પૂર્ણાંક		su_maxscore;	    // maximum score for adding to su_ga
    પૂર્ણાંક		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    પૂર્ણાંક		su_badlen;	    // length of detected bad word in line
    પૂર્ણાંક		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
પ્રકારવ્યાખ્યા સંરચના suggest_S
{
    char_u	*st_word;	// suggested word, allocated string
    પૂર્ણાંક		st_wordlen;	// STRLEN(st_word)
    પૂર્ણાંક		st_orglen;	// length of replaced text
    પૂર્ણાંક		st_score;	// lower is better
    પૂર્ણાંક		st_altscore;	// used when st_score compares equal
    પૂર્ણાંક		st_salscore;	// st_score is for soundalike
    પૂર્ણાંક		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#define SUG(ga, i) (((suggest_T *)(ga).ga_data)[i])

// TRUE if a word appears in the list of banned words.
#define WAS_BANNED(su, word) (!HASHITEM_EMPTY(hash_find(&(su)->su_banned, word)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#define SUG_CLEAN_ગણતરી(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#define SUG_MAX_ગણતરી(su)	(SUG_CLEAN_ગણતરી(su) + 50)

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
પ્રકારવ્યાખ્યા ગણના
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
પ્રકારવ્યાખ્યા સંરચના trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    પૂર્ણાંક		ts_score;	// score
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
#define DIFF_ઉમેરો	2	// inserting character

// values for ts_flags
#define TSF_PREFIXOK	1	// already checked that prefix is OK
#define TSF_DIDSPLIT	2	// tried split at this point
#define TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#define PFD_NOPREFIX	0xff	// not using prefixes
#define PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#define PFD_NOTSPECIAL	0xfd	// highest value that's not special

સ્થિર long spell_suggest_timeout = 5000;

સ્થિર શૂન્ય spell_find_suggest(char_u *badptr, પૂર્ણાંક badlen, suginfo_T *su, પૂર્ણાંક maxcount, પૂર્ણાંક banbadword, પૂર્ણાંક need_cap, પૂર્ણાંક interactive);
#ifdef FEAT_EVAL
સ્થિર શૂન્ય spell_suggest_પદાવલિ(suginfo_T *su, char_u *પદાવલિ);
#endif
સ્થિર શૂન્ય spell_suggest_file(suginfo_T *su, char_u *fname);
સ્થિર શૂન્ય spell_suggest_intern(suginfo_T *su, પૂર્ણાંક interactive);
સ્થિર શૂન્ય spell_find_cleanup(suginfo_T *su);
સ્થિર શૂન્ય suggest_try_special(suginfo_T *su);
સ્થિર શૂન્ય suggest_try_change(suginfo_T *su);
સ્થિર શૂન્ય suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, પૂર્ણાંક soundfold);
સ્થિર શૂન્ય go_deeper(trystate_T *ઢગલો, પૂર્ણાંક depth, પૂર્ણાંક score_add);
સ્થિર શૂન્ય find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword);
સ્થિર શૂન્ય score_comp_sal(suginfo_T *su);
સ્થિર શૂન્ય score_combine(suginfo_T *su);
સ્થિર પૂર્ણાંક stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
સ્થિર શૂન્ય suggest_try_soundalike_prep(શૂન્ય);
સ્થિર શૂન્ય suggest_try_soundalike(suginfo_T *su);
સ્થિર શૂન્ય suggest_try_soundalike_finish(શૂન્ય);
સ્થિર શૂન્ય add_sound_suggest(suginfo_T *su, char_u *goodword, પૂર્ણાંક score, langp_T *lp);
સ્થિર પૂર્ણાંક soundfold_find(slang_T *slang, char_u *word);
સ્થિર પૂર્ણાંક similar_chars(slang_T *slang, પૂર્ણાંક c1, પૂર્ણાંક c2);
સ્થિર શૂન્ય add_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, પૂર્ણાંક badlen, પૂર્ણાંક score, પૂર્ણાંક altscore, પૂર્ણાંક had_bonus, slang_T *slang, પૂર્ણાંક maxsf);
સ્થિર શૂન્ય check_suggestions(suginfo_T *su, garray_T *gap);
સ્થિર શૂન્ય add_banned(suginfo_T *su, char_u *word);
સ્થિર શૂન્ય rescore_suggestions(suginfo_T *su);
સ્થિર શૂન્ય rescore_one(suginfo_T *su, suggest_T *stp);
સ્થિર પૂર્ણાંક cleanup_suggestions(garray_T *gap, પૂર્ણાંક maxscore, પૂર્ણાંક keep);
સ્થિર પૂર્ણાંક soundalike_score(char_u *goodsound, char_u *badsound);
સ્થિર પૂર્ણાંક spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
સ્થિર પૂર્ણાંક spell_edit_score_limit(slang_T *slang, char_u *badword, char_u *goodword, પૂર્ણાંક limit);
સ્થિર પૂર્ણાંક spell_edit_score_limit_w(slang_T *slang, char_u *badword, char_u *goodword, પૂર્ણાંક limit);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    સ્થિર પૂર્ણાંક
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    પૂર્ણાંક		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    જો (!byte_in_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	પાછા FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    જો (slang->sl_comprules != ખાલી && sp->ts_complen > sp->ts_compsplit)
    {
	પૂર્ણાંક v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = match_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	પાછા v;
    }

    પાછા TRUE;
}

/*
 * Adjust the score of common words.
 */
    સ્થિર પૂર્ણાંક
score_wordcount_adj(
    slang_T	*slang,
    પૂર્ણાંક		score,
    char_u	*word,
    પૂર્ણાંક		split)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    પૂર્ણાંક		bonus;
    પૂર્ણાંક		newscore;

    hi = hash_find(&slang->sl_wordcount, word);
    જો (HASHITEM_EMPTY(hi))
	પાછા score;

    wc = HI2WC(hi);
    જો (wc->wc_ગણતરી < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    નહીંતર જો (wc->wc_ગણતરી < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    નહીંતર
	bonus = SCORE_COMMON3;
    જો (split)
	newscore = score - bonus / 2;
    નહીંતર
	newscore = score - bonus;
    જો (newscore < 0)
	પાછા 0;
    પાછા newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    સ્થિર પૂર્ણાંક
badword_captype(char_u *word, char_u *end)
{
    પૂર્ણાંક		flags = captype(word, end);
    પૂર્ણાંક		c;
    પૂર્ણાંક		l, u;
    પૂર્ણાંક		first;
    char_u	*p;

    જો (!(flags & WF_KEEPCAP))
	પાછા flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    first = FALSE;
    માટે (p = word; p < end; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	જો (SPELL_ISUPPER(c))
	{
	    ++u;
	    જો (p == word)
		first = TRUE;
	}
	નહીંતર
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    જો (u > l && u > 2)
	flags |= WF_ALLCAP;
    નહીંતર જો (first)
	flags |= WF_ONECAP;

    જો (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    પાછા flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    સ્થિર પૂર્ણાંક
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    પૂર્ણાંક		nr;
    પૂર્ણાંક		c;

    c = *p++;
    જો ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    નહીંતર જો ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    નહીંતર જો ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    નહીંતર				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    પાછા nr;
}

// values for sps_flags
#define SPS_BEST    1
#define SPS_FAST    2
#define SPS_DOUBLE  4

સ્થિર પૂર્ણાંક sps_flags = SPS_BEST;	// flags from 'spellsuggest'
સ્થિર પૂર્ણાંક sps_limit = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    પૂર્ણાંક
spell_check_sps(શૂન્ય)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    પૂર્ણાંક		f;

    sps_flags = 0;
    sps_limit = 9999;

    માટે (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	જો (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_limit = getdigits(&s);
	    જો (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	નહીંતર જો (STRCMP(buf, "best") == 0)
	    f = SPS_BEST;
	નહીંતર જો (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	નહીંતર જો (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	નહીંતર જો (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	જો (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_BEST;
	    sps_limit = 9999;
	    પાછા FAIL;
	}
	જો (f != 0)
	    sps_flags = f;
    }

    જો (sps_flags == 0)
	sps_flags = SPS_BEST;

    પાછા OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    શૂન્ય
spell_suggest(પૂર્ણાંક ગણતરી)
{
    char_u	*લીટી = ખાલી;
    pos_T	prev_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;
    suginfo_T	sug;
    suggest_T	*stp;
    પૂર્ણાંક		mouse_used;
    પૂર્ણાંક		need_cap;
    પૂર્ણાંક		limit;
    પૂર્ણાંક		selected = ગણતરી;
    પૂર્ણાંક		badlen = 0;
    પૂર્ણાંક		સંદેશ_scroll_save = સંદેશ_scroll;
    પૂર્ણાંક		wo_spell_save = curwin->w_p_spell;

    જો (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    prev_cursor = curwin->w_cursor;

    જો (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	જાઓ skip;
    }

    જો (VIsual_active)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	જો (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    જાઓ skip;
	}
	badlen = (પૂર્ણાંક)curwin->w_cursor.col - (પૂર્ણાંક)VIsual.col;
	જો (badlen < 0)
	    badlen = -badlen;
	નહીંતર
	    curwin->w_cursor.col = VIsual.col;
	++badlen;
	end_visual_mode();
	// make sure we don't include the NUL at the end of the line
	જો (badlen > ml_get_curline_len() - (પૂર્ણાંક)curwin->w_cursor.col)
	    badlen = ml_get_curline_len() - (પૂર્ણાંક)curwin->w_cursor.col;
    }
    // Find the start of the badly spelled word.
    નહીંતર જો (spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, ખાલી) == 0
	    || curwin->w_cursor.col > prev_cursor.col)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = prev_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.col;
	// Backup to before start of word.
	જ્યારે (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_BACK(curline, p);
	// Forward to start of word.
	જ્યારે (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	જો (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    જાઓ skip;
	}
	curwin->w_cursor.col = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.col);

    // Make a copy of current line since autocommands may free the line.
    લીટી = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    જો (લીટી == ખાલી)
	જાઓ skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    જો (sps_limit > (પૂર્ણાંક)Rows - 2)
	limit = (પૂર્ણાંક)Rows - 2;
    નહીંતર
	limit = sps_limit;
    spell_find_suggest(લીટી + curwin->w_cursor.col, badlen, &sug, limit,
							TRUE, need_cap, TRUE);

    જો (sug.su_ga.ga_len == 0)
	સંદેશ(_("Sorry, no suggestions"));
    નહીંતર જો (ગણતરી > 0)
    {
	જો (ગણતરી > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (long)sug.su_ga.ga_len);
    }
    નહીંતર
    {
#ifdef FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	જો (cmdmsg_rl)
	    સંદેશ_col = cmdline_width - 1;
#endif

	// List the suggestions.
	સંદેશ_start();
	સંદેશ_row = Rows - 1;	// for when 'cmdheight' > 1
	લીટીઓ_ડાબું = Rows;	// avoid more prompt
	vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#ifdef FEAT_RIGHTLEFT
	જો (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#endif
	સંદેશ_puts((અક્ષર *)IObuff);
	સંદેશ_clr_eos();
	સંદેશ_putchar('\n');

	સંદેશ_scroll = TRUE;
	માટે (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    પૂર્ણાંક el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_word, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    જો (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "%2d", i + 1);
#ifdef FEAT_RIGHTLEFT
	    જો (cmdmsg_rl)
		rl_mirror(IObuff);
#endif
	    સંદેશ_puts((અક્ષર *)IObuff);

	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    સંદેશ_puts((અક્ષર *)IObuff);

	    // The word may replace more than "su_badlen".
	    જો (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((અક્ષર *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		સંદેશ_puts((અક્ષર *)IObuff);
	    }

	    જો (p_verbose > 0)
	    {
		// Add the score.
		જો (sps_flags & (SPS_DOUBLE | SPS_BEST))
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		નહીંતર
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#ifdef FEAT_RIGHTLEFT
		જો (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#endif
		સંદેશ_advance(30);
		સંદેશ_puts((અક્ષર *)IObuff);
	    }
	    સંદેશ_putchar('\n');
	}

#ifdef FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	સંદેશ_col = 0;
#endif
	// Ask for choice.
	selected = prompt_for_સંખ્યા(&mouse_used);
	જો (mouse_used)
	    selected -= લીટીઓ_ડાબું;
	લીટીઓ_ડાબું = Rows;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	સંદેશ_scroll = સંદેશ_scroll_save;
    }

    જો (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_to);

	stp = &SUG(sug.su_ga, selected - 1);
	જો (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "%s%.*s", stp->st_word,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_to = vim_strsave(IObuff);
	}
	નહીંતર
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_to = vim_strsave(stp->st_word);
	}

	// Replace the word.
	p = alloc(STRLEN(લીટી) - stp->st_orglen + stp->st_wordlen + 1);
	જો (p != ખાલી)
	{
	    પૂર્ણાંક len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (પૂર્ણાંક)(sug.su_badptr - લીટી);
	    mch_memmove(p, લીટી, c);
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
#જો defined(FEAT_PROP_POPUP)
	    જો (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#endif
	}
    }
    નહીંતર
	curwin->w_cursor = prev_cursor;

    spell_find_cleanup(&sug);
skip:
    vim_free(લીટી);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    શૂન્ય
spell_suggest_list(
    garray_T	*gap,
    char_u	*word,
    પૂર્ણાંક		maxcount,	// maximum nr of suggestions
    પૂર્ણાંક		need_cap,	// 'spellcapcheck' matched
    પૂર્ણાંક		interactive)
{
    suginfo_T	sug;
    પૂર્ણાંક		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_find_suggest(word, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, કદ(char_u *), sug.su_ga.ga_len + 1);
    જો (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	માટે (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (unsigned)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    જો (wcopy == ખાલી)
		તોડો;
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
    સ્થિર શૂન્ય
spell_find_suggest(
    char_u	*badptr,
    પૂર્ણાંક		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    પૂર્ણાંક		maxcount,
    પૂર્ણાંક		banbadword,	// don't include badword in suggestions
    પૂર્ણાંક		need_cap,	// word should start with capital
    પૂર્ણાંક		interactive)
{
    hlf_T	attr = HLF_ગણતરી;
    char_u	buf[MAXPATHL];
    char_u	*p;
    પૂર્ણાંક		do_combine = FALSE;
    char_u	*sps_copy;
#ifdef FEAT_EVAL
    સ્થિર પૂર્ણાંક	પદાવલિ_busy = FALSE;
#endif
    પૂર્ણાંક		c;
    પૂર્ણાંક		i;
    langp_T	*lp;
    પૂર્ણાંક		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_POINTER(su);
    ga_init2(&su->su_ga, કદ(suggest_T), 10);
    ga_init2(&su->su_sga, કદ(suggest_T), 10);
    જો (*badptr == NUL)
	પાછા;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    જો (badlen != 0)
	su->su_badlen = badlen;
    નહીંતર
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, ખાલી, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    જો (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (શૂન્ય)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    જો (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    માટે (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_ENTRY(curbuf->b_s.b_langp, i);
	જો (lp->lp_sallang != ખાલી)
	{
	    su->su_sallang = lp->lp_sallang;
	    તોડો;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    જો (su->su_sallang != ખાલી)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    જો (!SPELL_ISUPPER(c) && attr == HLF_ગણતરી)
    {
	make_case_word(su->su_badword, buf, WF_ONECAP);
	add_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    જો (banbadword)
	add_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    જો (sps_copy == ખાલી)
	પાછા;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    માટે (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	જો (STRNCMP(buf, "expr:", 5) == 0)
	{
#ifdef FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    જો (!પદાવલિ_busy)
	    {
		પદાવલિ_busy = TRUE;
		spell_suggest_પદાવલિ(su, buf + 5);
		પદાવલિ_busy = FALSE;
	    }
#endif
	}
	નહીંતર જો (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	નહીંતર જો (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((અક્ષર *)buf + 8);
	નહીંતર જો (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    જો (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    જો (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#ifdef FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    સ્થિર શૂન્ય
spell_suggest_પદાવલિ(suginfo_T *su, char_u *પદાવલિ)
{
    list_T	*list;
    listitem_T	*li;
    પૂર્ણાંક		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_પદાવલિ(su->su_badword, પદાવલિ);
    જો (list != ખાલી)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_વસ્તુઓ(list, li)
	    જો (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		જો (score >= 0 && score <= su->su_maxscore)
		    add_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (શૂન્ય)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#endif

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    સ્થિર શૂન્ય
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	લીટી[MAXWLEN * 2];
    char_u	*p;
    પૂર્ણાંક		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((અક્ષર *)fname, "r");
    જો (fd == ખાલી)
    {
	semsg(_(e_cant_open_file_str), fname);
	પાછા;
    }

    // Read it line by line.
    જ્યારે (!vim_fgets(લીટી, MAXWLEN * 2, fd) && !got_int)
    {
	લીટી_breakcheck();

	p = vim_strchr(લીટી, '/');
	જો (p == ખાલી)
	    ચાલુ;	    // No Tab found, just skip the line.
	*p++ = NUL;
	જો (STRICMP(su->su_badword, લીટી) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    માટે (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    જો (captype(p, ખાલી) == 0)
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
    (શૂન્ય)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    સ્થિર શૂન્ય
spell_suggest_intern(suginfo_T *su, પૂર્ણાંક interactive)
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
    જો (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    જો ((sps_flags & SPS_FAST) == 0)
    {
	જો (sps_flags & SPS_BEST)
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
	જો (su->su_ga.ga_len < SUG_CLEAN_ગણતરી(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    જો (su->su_ga.ga_len < SUG_CLEAN_ગણતરી(su))
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
    જો (interactive && got_int)
    {
	(શૂન્ય)vgetc();
	got_int = FALSE;
    }

    જો ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	જો (sps_flags & SPS_BEST)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(શૂન્ય)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    સ્થિર શૂન્ય
spell_find_cleanup(suginfo_T *su)
{
    પૂર્ણાંક		i;

    // Free the suggestions.
    માટે (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_word);
    ga_clear(&su->su_ga);
    માટે (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_word);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    સ્થિર શૂન્ય
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    પૂર્ણાંક		c;
    char_u	word[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    જો (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
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
#જો 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T total;
proftime_T times[STATE_FINAL + 1];
long counts[STATE_FINAL + 1];

    સ્થિર શૂન્ય
prof_init(શૂન્ય)
{
    માટે (પૂર્ણાંક i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_start(&current);
    profile_start(&total);
}

// call before changing state
    સ્થિર શૂન્ય
prof_store(state_T state)
{
    profile_end(&current);
    profile_add(&times[state], &current);
    ++counts[state];
    profile_start(&current);
}
# define PROF_STORE(state) prof_store(state);

    સ્થિર શૂન્ય
prof_report(અક્ષર *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_end(&total);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_સંદેશ(&total));
    માટે (પૂર્ણાંક i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_સંદેશ(&times[i]), counts[i]);
    fclose(fd);
}
#નહીંતર
# define PROF_STORE(state)
#endif

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    સ્થિર શૂન્ય
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    પૂર્ણાંક		n;
    char_u	*p;
    પૂર્ણાંક		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (પૂર્ણાંક)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (શૂન્ય)spell_casefold(curwin, p, (પૂર્ણાંક)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (પૂર્ણાંક)STRLEN(su->su_badptr);
    જો (n < MAXWLEN)
	fword[n] = NUL;

    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	જો (lp->lp_slang->sl_fbyts == ખાલી)
	    ચાલુ;

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
#define TRY_DEEPER(su, ઢગલો, depth, add) \
       ((depth) < MAXWLEN - 1 && (ઢગલો)[depth].ts_score + (add) < (su)->su_maxscore)

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
    સ્થિર શૂન્ય
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    પૂર્ણાંક		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	ઢગલો[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    પૂર્ણાંક		newscore;
    પૂર્ણાંક		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    પૂર્ણાંક		depth;
    પૂર્ણાંક		c, c2, c3;
    પૂર્ણાંક		n = 0;
    પૂર્ણાંક		flags;
    garray_T	*gap;
    idx_T	arridx;
    પૂર્ણાંક		len;
    char_u	*p;
    fromto_T	*ftp;
    પૂર્ણાંક		fl = 0, tl;
    પૂર્ણાંક		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    પૂર્ણાંક		fword_ends;
    પૂર્ણાંક		goodword_ends;
#ifdef DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#endif
    પૂર્ણાંક		breakcheckcount = 1000;
#ifdef FEAT_RELTIME
    proftime_T	time_limit;
#endif
    પૂર્ણાંક		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    depth = 0;
    sp = &ઢગલો[0];
    CLEAR_POINTER(sp);
    sp->ts_curi = 1;

    જો (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = ખાલી;
	pidxs = ખાલી;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_START;
    }
    નહીંતર
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	જો (pbyts != ખાલી)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	નહીંતર
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
    જો (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &time_limit);
#endif

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    જ્યારે (depth >= 0 && !got_int)
    {
	sp = &ઢગલો[depth];
	પસંદગી (sp->ts_state)
	{
	સ્થિતિ STATE_START:
	સ્થિતિ STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    જો (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		માટે (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (પૂર્ણાંક)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		જો (depth < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (પૂર્ણાંક)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    જો (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    નહીંતર
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#ifdef DEBUG_TRIEWALK
		    sprintf(changename[depth], "prefix");
#endif
		    go_deeper(ઢગલો, depth, 0);
		    ++depth;
		    sp = &ઢગલો[depth];
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
		તોડો;
	    }

	    જો (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		તોડો;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (પૂર્ણાંક)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    જો (flags & WF_NOSUGGEST)
		તોડો;

	    fword_ends = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    જો (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != ખાલી)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = ઢગલો[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		માટે (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		જો (c > 0)
		{
		    c = valid_word_prefix(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    જો (c == 0)
			તોડો;

		    // Use the WF_RARE flag for a rare prefix.
		    જો (c & WF_RAREPFX)
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
	    જો (sp->ts_complen == sp->ts_compsplit && fword_ends
						     && (flags & WF_NEEDCOMP))
		goodword_ends = FALSE;
	    નહીંતર
		goodword_ends = TRUE;

	    p = ખાલી;
	    compound_ok = TRUE;
	    જો (sp->ts_complen > sp->ts_compsplit)
	    {
		જો (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    જો (sp->ts_fidx - sp->ts_splitfidx
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
			જો (newscore <= su->su_maxscore)
			    add_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			તોડો;
		    }
		}
		નહીંતર
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    જો (((unsigned)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			તોડો;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    જો (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			તોડો;

		    compflags[sp->ts_complen] = ((unsigned)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    જો (match_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    જો (compound_ok)
		    {
			p = preword;
			જ્યારે (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			જો (fword_ends && !can_compound(slang, p,
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
	    જો (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    નહીંતર જો (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    નહીંતર
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		જો ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		જો (p != ખાલી && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_word(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    જો (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		જો (flags & WF_BANNED)
		{
		    add_banned(su, preword + sp->ts_prewordlen);
		    તોડો;
		}
		જો ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    જો (slang->sl_compprog == ખાલી)
			તોડો;
		    // the word so far was banned but we may try compounding
		    goodword_ends = FALSE;
		}
	    }

	    newscore = 0;
	    જો (!soundfold)	// soundfold words don't have flags
	    {
		જો ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		જો (flags & WF_RARE)
		    newscore += SCORE_RARE;

		જો (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, ખાલી)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    જો (fword_ends
		    && goodword_ends
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#ifdef DEBUG_TRIEWALK
		જો (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    પૂર્ણાંક	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    માટે (j = 0; j < depth; ++j)
			smsg("%s", changename[j]);
		}
#endif
		જો (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    add_sound_suggest(su, preword, sp->ts_score, lp);
		}
		નહીંતર જો (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_BACK(fword, p);
		    જો (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_BACK(preword, p);
			જો (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    જો (score <= su->su_maxscore)
		    {
			add_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			જો (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, ખાલી);
			    જો (c == 0 || c == WF_ALLCAP)
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
	    જો ((sp->ts_fidx >= sp->ts_fidxtry || fword_ends)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		પૂર્ણાંક	try_compound;
		પૂર્ણાંક	try_split;

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
		જો (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != ખાલી
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
		જો (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		નહીંતર જો (!fword_ends
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		નહીંતર
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		જો (try_split || try_compound)
		{
		    જો (!try_compound && (!fword_ends || !goodword_ends))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			જો (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    તોડો;
			p = preword;
			જ્યારે (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			જો (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    તોડો;

			જો (slang->sl_nosplitsugs)
			    newscore += SCORE_SPLIT_NO;
			નહીંતર
			    newscore += SCORE_SPLIT;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    જો (TRY_DEEPER(su, ઢગલો, depth, newscore))
		    {
			go_deeper(ઢગલો, depth, newscore);
#ifdef DEBUG_TRIEWALK
			જો (!try_compound && !fword_ends)
			    sprintf(changename[depth], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			નહીંતર
			    sprintf(changename[depth], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#endif
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++depth;
			sp = &ઢગલો[depth];

			// Append a space to preword when splitting.
			જો (!try_compound && !fword_ends)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			જો (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_ends)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_ends)
			{
			    પૂર્ણાંક	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    જો (fword_ends)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    નહીંતર
				sp->ts_score -= SCORE_SPLIT - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			જો (try_compound)
			    ++sp->ts_complen;
			નહીંતર
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			જો (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			નહીંતર
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			જો (pbyts != ખાલી)
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
	    તોડો;

	સ્થિતિ STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_START;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    તોડો;

	સ્થિતિ STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    જો (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		તોડો;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	સ્થિતિ STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    જો (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		જો (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		નહીંતર
		    sp->ts_state = STATE_FINAL;
	    }
	    નહીંતર
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		જો (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		નહીંતર
		    newscore = SCORE_SUBST;
		જો ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, ઢગલો, depth, newscore))
		{
		    go_deeper(ઢગલો, depth, newscore);
#ifdef DEBUG_TRIEWALK
		    જો (newscore > 0)
			sprintf(changename[depth], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    નહીંતર
			sprintf(changename[depth], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#endif
		    ++depth;
		    sp = &ઢગલો[depth];
		    જો (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    જો (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    જો (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			જો (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			નહીંતર જો (sp->ts_isdiff == DIFF_ઉમેરો
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			જો (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    જો (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				જો (enc_utf8
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
				નહીંતર જો (!soundfold
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
			    નહીંતર જો (sp->ts_isdiff == DIFF_ઉમેરો
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				જો (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				નહીંતર
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_BACK(tword, p);
				    જો (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    નહીંતર
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			જો (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    તોડો;

	સ્થિતિ STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    જો (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		તોડો;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    જો (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    નહીંતર
		newscore = SCORE_DEL;
	    જો (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, ઢગલો, depth, newscore))
	    {
		go_deeper(ઢગલો, depth, newscore);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#endif
		++depth;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		ઢગલો[depth].ts_flags |= TSF_DIDDEL;
		ઢગલો[depth].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		જો (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    ઢગલો[depth].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    જો (enc_utf8 && utf_iscomposing(c))
			ઢગલો[depth].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    નહીંતર જો (c == mb_ptr2char(fword + ઢગલો[depth].ts_fidx))
			ઢગલો[depth].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		નહીંતર
		{
		    ++ઢગલો[depth].ts_fidx;
		    જો (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			ઢગલો[depth].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		તોડો;
	    }
	    // FALLTHROUGH

	સ્થિતિ STATE_INS_PREP:
	    જો (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP;
		તોડો;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    માટે (;;)
	    {
		જો (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_SWAP;
		    તોડો;
		}
		જો (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    તોડો;
		}
		++sp->ts_curi;
	    }
	    તોડો;

	    // FALLTHROUGH

	સ્થિતિ STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    જો (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP;
		તોડો;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    જો (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		તોડો;
	    }
	    c = byts[n];
	    જો (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    નહીંતર
		newscore = SCORE_INS;
	    જો (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, ઢગલો, depth, newscore))
	    {
		go_deeper(ઢગલો, depth, newscore);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#endif
		++depth;
		sp = &ઢગલો[depth];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		જો (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    જો (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_ઉમેરો;
		    }
		}
		નહીંતર
		    fl = 1;
		જો (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    જો (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    તોડો;

	સ્થિતિ STATE_SWAP:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    જો (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		તોડો;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    જો (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		તોડો;
	    }

	    જો (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		જો (p[n] == NUL)
		    c2 = NUL;
		નહીંતર જો (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		નહીંતર
		    c2 = mb_ptr2char(p + n);
	    }
	    નહીંતર
	    {
		જો (p[1] == NUL)
		    c2 = NUL;
		નહીંતર જો (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		નહીંતર
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    જો (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		તોડો;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    જો (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		તોડો;
	    }
	    જો (c2 != NUL && TRY_DEEPER(su, ઢગલો, depth, SCORE_SWAP))
	    {
		go_deeper(ઢગલો, depth, SCORE_SWAP);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++depth;
		જો (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		નહીંતર
		{
		    p[0] = c2;
		    p[1] = c;
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    નહીંતર
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    તોડો;

	સ્થિતિ STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    જો (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    નહીંતર
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	સ્થિતિ STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    જો (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		જો (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		નહીંતર
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    નહીંતર
	    {
		c = *p;
		c2 = p[1];
		જો (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		નહીંતર
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    જો (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		તોડો;
	    }
	    જો (TRY_DEEPER(su, ઢગલો, depth, SCORE_SWAP3))
	    {
		go_deeper(ઢગલો, depth, SCORE_SWAP3);
#ifdef DEBUG_TRIEWALK
		sprintf(changename[depth], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#endif
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++depth;
		જો (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		નહીંતર
		{
		    p[0] = p[2];
		    p[2] = c;
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    નહીંતર
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    તોડો;

	સ્થિતિ STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    જો (has_mbyte)
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
	    નહીંતર
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    જો (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		તોડો;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    જો (TRY_DEEPER(su, ઢગલો, depth, SCORE_SWAP3))
	    {
		go_deeper(ઢગલો, depth, SCORE_SWAP3);
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
		જો (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		નહીંતર
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    નહીંતર
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    તોડો;

	સ્થિતિ STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    જો (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    નહીંતર
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    જો (TRY_DEEPER(su, ઢગલો, depth, SCORE_SWAP3))
	    {
		go_deeper(ઢગલો, depth, SCORE_SWAP3);
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
		જો (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		નહીંતર
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    નહીંતર
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    તોડો;

	સ્થિતિ STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    જો (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    નહીંતર
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	સ્થિતિ STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    જો ((lp->lp_replang == ખાલી && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		તોડો;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    જો (soundfold)
		sp->ts_curi = slang->sl_repsal_first[fword[sp->ts_fidx]];
	    નહીંતર
		sp->ts_curi = lp->lp_replang->sl_rep_first[fword[sp->ts_fidx]];

	    જો (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		તોડો;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	સ્થિતિ STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    જો (soundfold)
		gap = &slang->sl_repsal;
	    નહીંતર
		gap = &lp->lp_replang->sl_rep;
	    જ્યારે (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_data + sp->ts_curi++;
		જો (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    તોડો;
		}
		જો (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, ઢગલો, depth, SCORE_REP))
		{
		    go_deeper(ઢગલો, depth, SCORE_REP);
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
		    fl = (પૂર્ણાંક)STRLEN(ftp->ft_from);
		    tl = (પૂર્ણાંક)STRLEN(ftp->ft_to);
		    જો (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_to, tl);
		    ઢગલો[depth].ts_fidxtry = sp->ts_fidx + tl;
		    ઢગલો[depth].ts_tcharlen = 0;
		    તોડો;
		}
	    }

	    જો (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    તોડો;

	સ્થિતિ STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    જો (soundfold)
		gap = &slang->sl_repsal;
	    નહીંતર
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_data + sp->ts_curi - 1;
	    fl = (પૂર્ણાંક)STRLEN(ftp->ft_from);
	    tl = (પૂર્ણાંક)STRLEN(ftp->ft_to);
	    p = fword + sp->ts_fidx;
	    જો (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    તોડો;

	મૂળભૂત:
	    // Did all possible states at this level, go up one level.
	    --depth;

	    જો (depth >= 0 && ઢગલો[depth].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    જો (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#ifdef FEAT_RELTIME
		જો (spell_suggest_timeout > 0
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
    સ્થિર શૂન્ય
go_deeper(trystate_T *ઢગલો, પૂર્ણાંક depth, પૂર્ણાંક score_add)
{
    ઢગલો[depth + 1] = ઢગલો[depth];
    ઢગલો[depth + 1].ts_state = STATE_START;
    ઢગલો[depth + 1].ts_score = ઢગલો[depth].ts_score + score_add;
    ઢગલો[depth + 1].ts_curi = 1;	// start just after length byte
    ઢગલો[depth + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    સ્થિર શૂન્ય
find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    પૂર્ણાંક		depth;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    પૂર્ણાંક		round[MAXWLEN];
    પૂર્ણાંક		fwordidx[MAXWLEN];
    પૂર્ણાંક		uwordidx[MAXWLEN];
    પૂર્ણાંક		kwordlen[MAXWLEN];

    પૂર્ણાંક		flen, ulen;
    પૂર્ણાંક		l;
    પૂર્ણાંક		len;
    પૂર્ણાંક		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    જો (byts == ખાલી)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	પાછા;
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
    જ્યારે (depth >= 0)
    {
	જો (fword[fwordidx[depth]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    જો (byts[arridx[depth] + 1] == 0)
	    {
		kword[kwordlen[depth]] = NUL;
		પાછા;
	    }

	    // kword is getting too long, continue one level up
	    --depth;
	}
	નહીંતર જો (++round[depth] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --depth;
	}
	નહીંતર
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    જો (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[depth]);
		ulen = MB_CPTR2LEN(uword + uwordidx[depth]);
	    }
	    નહીંતર
		ulen = flen = 1;
	    જો (round[depth] == 1)
	    {
		p = fword + fwordidx[depth];
		l = flen;
	    }
	    નહીંતર
	    {
		p = uword + uwordidx[depth];
		l = ulen;
	    }

	    માટે (tryidx = arridx[depth]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
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
		tryidx = idxs[lo];
	    }

	    જો (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		જો (round[depth] == 1)
		{
		    STRNCPY(kword + kwordlen[depth], fword + fwordidx[depth],
									flen);
		    kwordlen[depth + 1] = kwordlen[depth] + flen;
		}
		નહીંતર
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
    સ્થિર શૂન્ય
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    પૂર્ણાંક		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    પૂર્ણાંક		score;
    પૂર્ણાંક		lpi;

    જો (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	પાછા;

    // Use the sound-folding of the first language that supports it.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	જો (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    માટે (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		જો (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_word = vim_strsave(stp->st_word);
		    જો (sstp->st_word != ખાલી)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    તોડો;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    સ્થિર શૂન્ય
score_combine(suginfo_T *su)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    પૂર્ણાંક		round;
    પૂર્ણાંક		lpi;
    slang_T	*slang = ખાલી;

    // Add the alternate score to su_ga.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	જો (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    માટે (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		જો (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		નહીંતર
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    તોડો;
	}
    }

    જો (slang == ખાલી)	// Using "double" without sound folding.
    {
	(શૂન્ય)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	પાછા;
    }

    // Add the alternate score to su_sga.
    માટે (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_word);
	જો (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	નહીંતર
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (શૂન્ય)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (શૂન્ય)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, કદ(suginfo_T), 1);
    જો (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	પાછા;

    stp = &SUG(ga, 0);
    માટે (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	માટે (round = 1; round <= 2; ++round)
	{
	    gap = round == 1 ? &su->su_ga : &su->su_sga;
	    જો (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_word;
		માટે (j = 0; j < ga.ga_len; ++j)
		    જો (STRCMP(stp[j].st_word, p) == 0)
			તોડો;
		જો (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		નહીંતર
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    જો (ga.ga_len > su->su_maxcount)
    {
	માટે (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_word);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    સ્થિર પૂર્ણાંક
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
    પૂર્ણાંક		lendiff;

    lendiff = (પૂર્ણાંક)(su->su_badlen - stp->st_orglen);
    જો (lendiff >= 0)
	pbad = badsound;
    નહીંતર
    {
	// soundfold the bad word with more characters following
	(શૂન્ય)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	જો (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_word) == NUL)
	    માટે (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    જો (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_word);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    નહીંતર
	pgood = stp->st_word;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    પાછા soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
પ્રકારવ્યાખ્યા સંરચના
{
    short	sft_score;	// lowest score used
    char_u	sft_word[1];    // soundfolded word, actually longer
} sftword_T;

સ્થિર sftword_T dumsft;
#define HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_word - (char_u *)&dumsft)))
#define HI2SFT(hi)     HIKEY2SFT((hi)->hi_ચાવી)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    સ્થિર શૂન્ય
suggest_try_soundalike_prep(શૂન્ય)
{
    langp_T	*lp;
    પૂર્ણાંક		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	જો (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ખાલી)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    સ્થિર શૂન્ય
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    પૂર્ણાંક		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	જો (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ખાલી)
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
    સ્થિર શૂન્ય
suggest_try_soundalike_finish(શૂન્ય)
{
    langp_T	*lp;
    પૂર્ણાંક		lpi;
    slang_T	*slang;
    પૂર્ણાંક		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	જો (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ખાલી)
	{
	    // Free the info about handled words.
	    todo = (પૂર્ણાંક)slang->sl_sounddone.ht_used;
	    FOR_ALL_HASHTAB_વસ્તુઓ(&slang->sl_sounddone, hi, todo)
		જો (!HASHITEM_EMPTY(hi))
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
    સ્થિર શૂન્ય
add_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    પૂર્ણાંક		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    પૂર્ણાંક		sfwordnr;
    char_u	*nrline;
    પૂર્ણાંક		orgnr;
    char_u	theword[MAXWLEN];
    પૂર્ણાંક		i;
    પૂર્ણાંક		wlen;
    char_u	*byts;
    idx_T	*idxs;
    પૂર્ણાંક		n;
    પૂર્ણાંક		wordcount;
    પૂર્ણાંક		wc;
    પૂર્ણાંક		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    પૂર્ણાંક		bc, gc;
    પૂર્ણાંક		limit;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    જો (HASHITEM_EMPTY(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_word) + STRLEN(goodword) + 1);
	જો (sft != ખાલી)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_word, goodword);
	    hash_add_item(&slang->sl_sounddone, hi, sft->sft_word, hash);
	}
    }
    નહીંતર
    {
	sft = HI2SFT(hi);
	જો (score >= sft->sft_score)
	    પાછા;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_find(slang, goodword);
    જો (sfwordnr < 0)
    {
	internal_error("add_sound_suggest()");
	પાછા;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    જ્યારે (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	માટે (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    જો (wordcount == orgnr && byts[n + 1] == NUL)
		તોડો;	// found end of word

	    જો (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    માટે ( ; byts[n + i] == NUL; ++i)
		જો (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    જાઓ badword;
		}

	    // One of the siblings must have the word.
	    માટે ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		જો (wordcount + wc > orgnr)
		    તોડો;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	માટે (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    પૂર્ણાંક		flags = (પૂર્ણાંક)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    જો (flags & WF_NOSUGGEST)
		ચાલુ;

	    જો (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, theword, cword);
		p = cword;
	    }
	    નહીંતર
	    {
		flags |= su->su_badflags;
		જો ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_word(theword, cword, flags);
		    p = cword;
		}
		નહીંતર
		    p = theword;
	    }

	    // Add the suggestion.
	    જો (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		જો (score <= su->su_maxscore)
		    add_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    નહીંતર
	    {
		// Add a penalty for words in another region.
		જો ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		નહીંતર
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		જો (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    જો (!SPELL_ISUPPER(bc)
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
		જો (limit > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		નહીંતર
		    goodscore += spell_edit_score_limit(slang, su->su_badword,
								    p, limit);

		// When going over the limit don't bother to do the rest.
		જો (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    જો (goodscore <= su->su_sfmaxscore)
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
    સ્થિર પૂર્ણાંક
soundfold_find(slang_T *slang, char_u *word)
{
    idx_T	arridx = 0;
    પૂર્ણાંક		len;
    પૂર્ણાંક		wlen = 0;
    પૂર્ણાંક		c;
    char_u	*ptr = word;
    char_u	*byts;
    idx_T	*idxs;
    પૂર્ણાંક		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    માટે (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	જો (byts[arridx] == NUL)
	{
	    જો (c == NUL)
		તોડો;

	    // Skip over the zeros, there can be several.
	    જ્યારે (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    જો (len == 0)
		પાછા -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	જો (c == NUL)
	    પાછા -1;

	// Perform a binary search in the list of accepted bytes.
	જો (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	જ્યારે (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    જો (--len == 0)	// end of the bytes, didn't find it
		પાછા -1;
	}
	જો (byts[arridx] != c)	// didn't find the byte
	    પાછા -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	જો (c == ' ')
	    જ્યારે (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    પાછા wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    સ્થિર પૂર્ણાંક
similar_chars(slang_T *slang, પૂર્ણાંક c1, પૂર્ણાંક c2)
{
    પૂર્ણાંક		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    જો (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	જો (HASHITEM_EMPTY(hi))
	    m1 = 0;
	નહીંતર
	    m1 = mb_ptr2char(hi->hi_ચાવી + STRLEN(hi->hi_ચાવી) + 1);
    }
    નહીંતર
	m1 = slang->sl_map_array[c1];
    જો (m1 == 0)
	પાછા FALSE;


    જો (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	જો (HASHITEM_EMPTY(hi))
	    m2 = 0;
	નહીંતર
	    m2 = mb_ptr2char(hi->hi_ચાવી + STRLEN(hi->hi_ચાવી) + 1);
    }
    નહીંતર
	m2 = slang->sl_map_array[c2];

    પાછા m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    સ્થિર શૂન્ય
add_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    પૂર્ણાંક		badlenarg,	// len of bad word replaced with "goodword"
    પૂર્ણાંક		score,
    પૂર્ણાંક		altscore,
    પૂર્ણાંક		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    પૂર્ણાંક		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    પૂર્ણાંક		goodlen;	// len of goodword changed
    પૂર્ણાંક		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    પૂર્ણાંક		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    માટે (;;)
    {
	goodlen = (પૂર્ણાંક)(pgood - goodword);
	badlen = (પૂર્ણાંક)(pbad - su->su_badptr);
	જો (goodlen <= 0 || badlen <= 0)
	    તોડો;
	MB_PTR_BACK(goodword, pgood);
	MB_PTR_BACK(su->su_badptr, pbad);
	જો (has_mbyte)
	{
	    જો (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		તોડો;
	}
	નહીંતર જો (*pgood != *pbad)
		તોડો;
    }

    જો (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	પાછા;

    જો (gap->ga_len == 0)
	i = -1;
    નહીંતર
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	માટે (i = gap->ga_len; --i >= 0; ++stp)
	    જો (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_word, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		જો (stp->st_slang == ખાલી)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		જો (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    જો (had_bonus)
			rescore_one(su, stp);
		    નહીંતર
		    {
			new_sug.st_word = stp->st_word;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		જો (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		તોડો;
	    }
    }

    જો (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_word = vim_strnsave(goodword, goodlen);
	જો (stp->st_word != ખાલી)
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
	    જો (gap->ga_len > SUG_MAX_ગણતરી(su))
	    {
		જો (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_ગણતરી(su));
		નહીંતર
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_ગણતરી(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    સ્થિર શૂન્ય
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    પૂર્ણાંક		i;
    char_u	longword[MAXWLEN + 1];
    પૂર્ણાંક		len;
    hlf_T	attr;

    જો (gap->ga_len == 0)
	પાછા;
    stp = &SUG(*gap, 0);
    માટે (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_word, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_ગણતરી;
	(શૂન્ય)spell_check(curwin, longword, &attr, ખાલી, FALSE);
	જો (attr != HLF_ગણતરી)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_word);
	    --gap->ga_len;
	    જો (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       કદ(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    સ્થિર શૂન્ય
add_banned(
    suginfo_T	*su,
    char_u	*word)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(word);
    hi = hash_lookup(&su->su_banned, word, hash);
    જો (!HASHITEM_EMPTY(hi))		// already present
	પાછા;
    s = vim_strsave(word);
    જો (s != ખાલી)
	hash_add_item(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    સ્થિર શૂન્ય
rescore_suggestions(suginfo_T *su)
{
    પૂર્ણાંક		i;

    જો (su->su_sallang != ખાલી)
	માટે (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    સ્થિર શૂન્ય
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    જો (slang != ખાલી && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	જો (slang == su->su_sallang)
	    p = su->su_sal_badword;
	નહીંતર
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	જો (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

સ્થિર પૂર્ણાંક sug_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    સ્થિર પૂર્ણાંક
sug_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    પૂર્ણાંક		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    જો (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	જો (n == 0)
	    n = STRICMP(p1->st_word, p2->st_word);
    }
    પાછા n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    સ્થિર પૂર્ણાંક
cleanup_suggestions(
    garray_T	*gap,
    પૂર્ણાંક		maxscore,
    પૂર્ણાંક		keep)		// nr of suggestions to keep
{
    જો (gap->ga_len <= 0)
	પાછા maxscore;

    // Sort the list.
    qsort(gap->ga_data, (size_t)gap->ga_len, કદ(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    જો (gap->ga_len > keep)
    {
	પૂર્ણાંક		i;
	suggest_T   *stp = &SUG(*gap, 0);

	માટે (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_word);
	gap->ga_len = keep;
	જો (keep >= 1)
	    પાછા stp[keep - 1].st_score;
    }
    પાછા maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    સ્થિર પૂર્ણાંક
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    પૂર્ણાંક		goodlen;
    પૂર્ણાંક		badlen;
    પૂર્ણાંક		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    પૂર્ણાંક		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    જો ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	જો ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    પાછા SCORE_DEL;
	જો (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    પાછા SCORE_MAXMAX;

	જો (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	નહીંતર
	{
	    score = 2 * SCORE_DEL / 3;
	    જો (*badsound == '*')
		++badsound;
	    નહીંતર
		++goodsound;
	}
    }

    goodlen = (પૂર્ણાંક)STRLEN(goodsound);
    badlen = (પૂર્ણાંક)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    જો (n < -2 || n > 2)
	પાછા SCORE_MAXMAX;

    જો (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    નહીંતર
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    જ્યારે (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    પસંદગી (n)
    {
	સ્થિતિ -2:
	સ્થિતિ 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    જ્યારે (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    જો (STRCMP(pl + 1, ps) == 0)
		પાછા score + SCORE_DEL * 2;

	    // Failed to compare.
	    તોડો;

	સ્થિતિ -1:
	સ્થિતિ 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    જ્યારે (*pl2 == *ps2)
	    {
		જો (*pl2 == NUL)	// reached the end
		    પાછા score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    જો (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		પાછા score + SCORE_DEL + SCORE_SWAP;

	    // 3: delete then substitute, then the rest must be equal
	    જો (STRCMP(pl2 + 1, ps2 + 1) == 0)
		પાછા score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    જો (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		જ્યારે (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		જો (STRCMP(pl2 + 1, ps2) == 0)
		    પાછા score + SCORE_SWAP + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    જ્યારે (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    જો (STRCMP(pl2 + 1, ps2) == 0)
		પાછા score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    તોડો;

	સ્થિતિ 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    જો (*pl == NUL)
		પાછા score;

	    // 2: swap
	    જો (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		જ્યારે (*pl2 == *ps2)
		{
		    જો (*pl2 == NUL)	// reached the end
			પાછા score + SCORE_SWAP;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		જો (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    પાછા score + SCORE_SWAP + SCORE_SWAP;

		// 4: swap and substitute
		જો (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    પાછા score + SCORE_SWAP + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    જ્યારે (*pl2 == *ps2)
	    {
		જો (*pl2 == NUL)	// reached the end
		    પાછા score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    જો (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		પાછા score + SCORE_SUBST + SCORE_SWAP;

	    // 7: substitute and substitute
	    જો (STRCMP(pl2 + 1, ps2 + 1) == 0)
		પાછા score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    જ્યારે (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    જો (STRCMP(pl2 + 1, ps2) == 0)
		પાછા score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    જ્યારે (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    જો (STRCMP(pl2, ps2 + 1) == 0)
		પાછા score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    તોડો;
    }

    પાછા SCORE_MAXMAX;
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
    સ્થિર પૂર્ણાંક
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    પૂર્ણાંક		*cnt;
    પૂર્ણાંક		badlen, goodlen;	// lengths including NUL
    પૂર્ણાંક		j, i;
    પૂર્ણાંક		t;
    પૂર્ણાંક		bc, gc;
    પૂર્ણાંક		pbc, pgc;
    char_u	*p;
    પૂર્ણાંક		wbadword[MAXWLEN];
    પૂર્ણાંક		wgoodword[MAXWLEN];

    જો (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	માટે (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	માટે (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    નહીંતર
    {
	badlen = (પૂર્ણાંક)STRLEN(badword) + 1;
	goodlen = (પૂર્ણાંક)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#define CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(પૂર્ણાંક, (badlen + 1) * (goodlen + 1));
    જો (cnt == ખાલી)
	પાછા 0;	// out of memory

    CNT(0, 0) = 0;
    માટે (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    માટે (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	માટે (j = 1; j <= goodlen; ++j)
	{
	    જો (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    નહીંતર
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    જો (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    નહીંતર
	    {
		// Use a better score when there is only a case difference.
		જો (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		નહીંતર
		{
		    // For a similar character use SCORE_SIMILAR.
		    જો (slang != ખાલી
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    નહીંતર
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		જો (i > 1 && j > 1)
		{
		    જો (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    નહીંતર
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    જો (bc == pgc && pbc == gc)
		    {
			t = SCORE_SWAP + CNT(i - 2, j - 2);
			જો (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		જો (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		જો (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    પાછા i;
}

પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		badi;
    પૂર્ણાંક		goodi;
    પૂર્ણાંક		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    સ્થિર પૂર્ણાંક
spell_edit_score_limit(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    પૂર્ણાંક		limit)
{
    limitscore_T    ઢગલો[10];		// allow for over 3 * 2 edits
    પૂર્ણાંક		    stackidx;
    પૂર્ણાંક		    bi, gi;
    પૂર્ણાંક		    bi2, gi2;
    પૂર્ણાંક		    bc, gc;
    પૂર્ણાંક		    score;
    પૂર્ણાંક		    score_off;
    પૂર્ણાંક		    minscore;
    પૂર્ણાંક		    round;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    જો (has_mbyte)
	પાછા spell_edit_score_limit_w(slang, badword, goodword, limit);

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

    માટે (;;)
    {
	// Skip over an equal part, score remains the same.
	માટે (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    જો (bc != gc)	// stop at a char that's different
		તોડો;
	    જો (bc == NUL)	// both words end
	    {
		જો (score < minscore)
		    minscore = score;
		જાઓ કાઢો;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	જો (gc == NUL)    // goodword ends, delete badword chars
	{
	    કરો
	    {
		જો ((score += SCORE_DEL) >= minscore)
		    જાઓ કાઢો;	    // do next alternative
	    } જ્યારે (badword[++bi] != NUL);
	    minscore = score;
	}
	નહીંતર જો (bc == NUL) // badword ends, insert badword chars
	{
	    કરો
	    {
		જો ((score += SCORE_INS) >= minscore)
		    જાઓ કાઢો;	    // do next alternative
	    } જ્યારે (goodword[++gi] != NUL);
	    minscore = score;
	}
	નહીંતર			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    માટે (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		જો (score_off < minscore)
		{
		    જો (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			જ્યારે (goodword[gi2] == badword[bi2])
			{
			    જો (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				તોડો;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    નહીંતર
		    {
			// try deleting/inserting a character later
			ઢગલો[stackidx].badi = bi + 1 - round;
			ઢગલો[stackidx].goodi = gi + round;
			ઢગલો[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    જો (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		જો (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    ચાલુ;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    જો (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    નહીંતર
	    {
		// For a similar character use SCORE_SIMILAR.
		જો (slang != ખાલી
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		નહીંતર
		    score += SCORE_SUBST;
	    }

	    જો (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		ચાલુ;
	    }
	}
કાઢો:
	// Get here to try the next alternative, pop it from the stack.
	જો (stackidx == 0)		// stack is empty, finished
	    તોડો;

	// pop an item from the stack
	--stackidx;
	gi = ઢગલો[stackidx].goodi;
	bi = ઢગલો[stackidx].badi;
	score = ઢગલો[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    જો (minscore > limit)
	પાછા SCORE_MAXMAX;
    પાછા minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    સ્થિર પૂર્ણાંક
spell_edit_score_limit_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    પૂર્ણાંક		limit)
{
    limitscore_T    ઢગલો[10];		// allow for over 3 * 2 edits
    પૂર્ણાંક		    stackidx;
    પૂર્ણાંક		    bi, gi;
    પૂર્ણાંક		    bi2, gi2;
    પૂર્ણાંક		    bc, gc;
    પૂર્ણાંક		    score;
    પૂર્ણાંક		    score_off;
    પૂર્ણાંક		    minscore;
    પૂર્ણાંક		    round;
    char_u	    *p;
    પૂર્ણાંક		    wbadword[MAXWLEN];
    પૂર્ણાંક		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    માટે (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    માટે (p = goodword; *p != NUL; )
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

    માટે (;;)
    {
	// Skip over an equal part, score remains the same.
	માટે (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    જો (bc != gc)	// stop at a char that's different
		તોડો;
	    જો (bc == NUL)	// both words end
	    {
		જો (score < minscore)
		    minscore = score;
		જાઓ કાઢો;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	જો (gc == NUL)    // goodword ends, delete badword chars
	{
	    કરો
	    {
		જો ((score += SCORE_DEL) >= minscore)
		    જાઓ કાઢો;	    // do next alternative
	    } જ્યારે (wbadword[++bi] != NUL);
	    minscore = score;
	}
	નહીંતર જો (bc == NUL) // badword ends, insert badword chars
	{
	    કરો
	    {
		જો ((score += SCORE_INS) >= minscore)
		    જાઓ કાઢો;	    // do next alternative
	    } જ્યારે (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	નહીંતર			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    માટે (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		જો (score_off < minscore)
		{
		    જો (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			જ્યારે (wgoodword[gi2] == wbadword[bi2])
			{
			    જો (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				તોડો;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    નહીંતર
		    {
			// try deleting a character from badword later
			ઢગલો[stackidx].badi = bi + 1 - round;
			ઢગલો[stackidx].goodi = gi + round;
			ઢગલો[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    જો (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		જો (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    ચાલુ;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    જો (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    નહીંતર
	    {
		// For a similar character use SCORE_SIMILAR.
		જો (slang != ખાલી
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		નહીંતર
		    score += SCORE_SUBST;
	    }

	    જો (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		ચાલુ;
	    }
	}
કાઢો:
	// Get here to try the next alternative, pop it from the stack.
	જો (stackidx == 0)		// stack is empty, finished
	    તોડો;

	// pop an item from the stack
	--stackidx;
	gi = ઢગલો[stackidx].goodi;
	bi = ઢગલો[stackidx].badi;
	score = ઢગલો[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    જો (minscore > limit)
	પાછા SCORE_MAXMAX;
    પાછા minscore;
}
#endif  // FEAT_SPELL
