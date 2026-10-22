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

#شامل "vim.h"

#اگر defined(FEAT_SPELL)

/*
 * Use this to adjust the score after finding suggestions, based on the
 * suggested word sounding like the bad word.  This is much faster than doing
 * it for every possible suggestion.
 * Disadvantage: When "the" is typed as "hte" it sounds quite different ("@"
 * vs "ht") and goes down in the list.
 * Used when 'spellsuggest' is set to "best".
 */
#تعریف_کرو RESCORE(لفظ_score, sound_score) ((3 * (لفظ_score) + (sound_score)) / 4)

/*
 * Do the opposite: based on a maximum end score and a known sound score,
 * compute the maximum word score that can be used.
 */
#تعریف_کرو MAXSCORE(لفظ_score, sound_score) ((4 * (لفظ_score) - (sound_score)) / 3)

// only used for su_badflags
#تعریف_کرو WF_MIXCAP   0x20	// mix of upper and lower case: macaRONI

/*
 * Information used when looking for suggestions.
 */
قسمتعریف ساخت suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    صحیح		su_maxcount;	    // max. number of suggestions displayed
    صحیح		su_maxscore;	    // maximum score for adding to su_ga
    صحیح		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    صحیح		su_badlen;	    // length of detected bad word in line
    صحیح		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
قسمتعریف ساخت suggest_S
{
    char_u	*st_لفظ;	// suggested word, allocated string
    صحیح		st_wordlen;	// STRLEN(st_word)
    صحیح		st_orglen;	// length of replaced text
    صحیح		st_score;	// lower is better
    صحیح		st_altscore;	// used when st_score compares equal
    صحیح		st_salscore;	// st_score is for soundalike
    صحیح		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#تعریف_کرو SUG(ga, i) (((suggest_T *)(ga).ga_ڈیٹا)[i])

// TRUE if a word appears in the list of banned words.
#تعریف_کرو WAS_BANNED(su, لفظ) (!HASHITEM_خالی_خانے(hash_ڈھونڈو(&(su)->su_banned, لفظ)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#تعریف_کرو SUG_CLEAN_گنتی(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#تعریف_کرو SUG_MAX_گنتی(su)	(SUG_CLEAN_گنتی(su) + 50)

// score for various changes
#تعریف_کرو SCORE_بانٹو	149	// split bad word
#تعریف_کرو SCORE_بانٹو_NO	249	// split bad word with NOSPLITSUGS
#تعریف_کرو SCORE_ICASE	52	// slightly different case
#تعریف_کرو SCORE_REGION	200	// word is for different region
#تعریف_کرو SCORE_RARE	180	// rare word
#تعریف_کرو SCORE_تبادلہ	75	// swap two characters
#تعریف_کرو SCORE_SWAP3	110	// swap two characters in three
#تعریف_کرو SCORE_REP	65	// REP replacement
#تعریف_کرو SCORE_SUBST	93	// substitute a character
#تعریف_کرو SCORE_SIMILAR	33	// substitute a similar character
#تعریف_کرو SCORE_SUBCOMP	33	// substitute a composing character
#تعریف_کرو SCORE_DEL	94	// delete a character
#تعریف_کرو SCORE_DELDUP	66	// delete a duplicated character
#تعریف_کرو SCORE_DELCOMP	28	// delete a composing character
#تعریف_کرو SCORE_INS	96	// insert a character
#تعریف_کرو SCORE_INSDUP	67	// insert a duplicate character
#تعریف_کرو SCORE_INSCOMP	30	// insert a composing character
#تعریف_کرو SCORE_NONWORD	103	// change non-word to word char

#تعریف_کرو SCORE_FILE	30	// suggestion from a file
#تعریف_کرو SCORE_MAXINIT	350	// Initial maximum score: higher == slower.
				// 350 allows for about three changes.

#تعریف_کرو SCORE_COMMON1	30	// subtracted for words seen before
#تعریف_کرو SCORE_COMMON2	40	// subtracted for words often seen
#تعریف_کرو SCORE_COMMON3	50	// subtracted for words very often seen
#تعریف_کرو SCORE_THRES2	10	// word count threshold for COMMON2
#تعریف_کرو SCORE_THRES3	100	// word count threshold for COMMON3

// When trying changed soundfold words it becomes slow when trying more than
// two changes.  With less than two changes it's slightly faster but we miss a
// few good suggestions.  In rare cases we need to try three of four changes.
#تعریف_کرو SCORE_SFMAX1	200	// maximum score for first try
#تعریف_کرو SCORE_SFMAX2	300	// maximum score for second try
#تعریف_کرو SCORE_SFMAX3	400	// maximum score for third try

#تعریف_کرو SCORE_BIG	(SCORE_INS * 3)	// big difference
#تعریف_کرو SCORE_MAXMAX	999999		// accept any score
#تعریف_کرو SCORE_LIMITMAX	350		// for spell_edit_score_limit()

// for spell_edit_score_limit() we need to know the minimum value of
// SCORE_ICASE, SCORE_SWAP, SCORE_DEL, SCORE_SIMILAR and SCORE_INS
#تعریف_کرو SCORE_EDIT_MIN	SCORE_SIMILAR

/*
 * For finding suggestions: At each node in the tree these states are tried:
 */
قسمتعریف شمار
{
    STATE_آغاز = 0,	// At start of node check for NUL bytes (goodword
			// ends); if badword ends there is a match, otherwise
			// try splitting word.
    STATE_NOPREFIX,	// try without prefix
    STATE_SPLITUNDO,	// Undo splitting.
    STATE_ENDNUL,	// Past NUL bytes at start of the node.
    STATE_PLAIN,	// Use each byte of the node.
    STATE_DEL,		// Delete a byte from the bad word.
    STATE_INS_PREP,	// Prepare for inserting bytes.
    STATE_INS,		// Insert a byte in the bad word.
    STATE_تبادلہ,		// Swap two bytes.
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
قسمتعریف ساخت trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    صحیح		ts_score;	// score
    idx_T	ts_arridx;	// index in tree array, start of node
    مختصر	ts_curi;	// index in list of child nodes
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
#تعریف_کرو DIFF_NONE	0	// no different byte (yet)
#تعریف_کرو DIFF_YES	1	// different byte found
#تعریف_کرو DIFF_داخل_کرو	2	// inserting character

// values for ts_flags
#تعریف_کرو TSF_PREFIXOK	1	// already checked that prefix is OK
#تعریف_کرو TSF_DIDSPLIT	2	// tried split at this point
#تعریف_کرو TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#تعریف_کرو PFD_NOPREFIX	0xff	// not using prefixes
#تعریف_کرو PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#تعریف_کرو PFD_NOTSPECIAL	0xfd	// highest value that's not special

ساکن طویل spell_suggest_timeout = 5000;

ساکن خالی spell_ڈھونڈو_suggest(char_u *badptr, صحیح badlen, suginfo_T *su, صحیح maxcount, صحیح banbadword, صحیح need_cap, صحیح interactive);
#اگر_تعریف_شدہ FEAT_EVAL
ساکن خالی spell_suggest_اظہاریہ(suginfo_T *su, char_u *اظہاریہ);
#اگر_ختم
ساکن خالی spell_suggest_file(suginfo_T *su, char_u *fname);
ساکن خالی spell_suggest_intern(suginfo_T *su, صحیح interactive);
ساکن خالی spell_ڈھونڈو_cleanup(suginfo_T *su);
ساکن خالی suggest_try_special(suginfo_T *su);
ساکن خالی suggest_try_change(suginfo_T *su);
ساکن خالی suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, صحیح soundfold);
ساکن خالی go_deeper(trystate_T *ڈھیر, صحیح گہرائی, صحیح score_جوڑو);
ساکن خالی ڈھونڈو_keepcap_لفظ(slang_T *slang, char_u *fword, char_u *kword);
ساکن خالی score_comp_sal(suginfo_T *su);
ساکن خالی score_combine(suginfo_T *su);
ساکن صحیح stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
ساکن خالی suggest_try_soundalike_prep(خالی);
ساکن خالی suggest_try_soundalike(suginfo_T *su);
ساکن خالی suggest_try_soundalike_اختتام(خالی);
ساکن خالی جوڑو_sound_suggest(suginfo_T *su, char_u *goodword, صحیح score, langp_T *lp);
ساکن صحیح soundfold_ڈھونڈو(slang_T *slang, char_u *لفظ);
ساکن صحیح similar_chars(slang_T *slang, صحیح c1, صحیح c2);
ساکن خالی جوڑو_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, صحیح badlen, صحیح score, صحیح altscore, صحیح had_bonus, slang_T *slang, صحیح maxsf);
ساکن خالی check_suggestions(suginfo_T *su, garray_T *gap);
ساکن خالی جوڑو_banned(suginfo_T *su, char_u *لفظ);
ساکن خالی rescore_suggestions(suginfo_T *su);
ساکن خالی rescore_one(suginfo_T *su, suggest_T *stp);
ساکن صحیح cleanup_suggestions(garray_T *gap, صحیح maxscore, صحیح keep);
ساکن صحیح soundalike_score(char_u *goodsound, char_u *badsound);
ساکن صحیح spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
ساکن صحیح spell_edit_score_حد_بندی(slang_T *slang, char_u *badword, char_u *goodword, صحیح حد_بندی);
ساکن صحیح spell_edit_score_حد_بندی_w(slang_T *slang, char_u *badword, char_u *goodword, صحیح حد_بندی);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    ساکن صحیح
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    صحیح		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    اگر (!byte_اندر_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	واپس FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    اگر (slang->sl_comprules != تہی && sp->ts_complen > sp->ts_compsplit)
    {
	صحیح v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = مطابقت_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	واپس v;
    }

    واپس TRUE;
}

/*
 * Adjust the score of common words.
 */
    ساکن صحیح
score_wordcount_adj(
    slang_T	*slang,
    صحیح		score,
    char_u	*لفظ,
    صحیح		بانٹو)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    صحیح		bonus;
    صحیح		newscore;

    hi = hash_ڈھونڈو(&slang->sl_wordcount, لفظ);
    اگر (HASHITEM_خالی_خانے(hi))
	واپس score;

    wc = HI2WC(hi);
    اگر (wc->wc_گنتی < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    ورنہ اگر (wc->wc_گنتی < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    ورنہ
	bonus = SCORE_COMMON3;
    اگر (بانٹو)
	newscore = score - bonus / 2;
    ورنہ
	newscore = score - bonus;
    اگر (newscore < 0)
	واپس 0;
    واپس newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    ساکن صحیح
badword_captype(char_u *لفظ, char_u *انتہا)
{
    صحیح		flags = captype(لفظ, انتہا);
    صحیح		c;
    صحیح		l, u;
    صحیح		پہلا;
    char_u	*p;

    اگر (!(flags & WF_KEEPCAP))
	واپس flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    پہلا = FALSE;
    برائے (p = لفظ; p < انتہا; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	اگر (SPELL_ISUPPER(c))
	{
	    ++u;
	    اگر (p == لفظ)
		پہلا = TRUE;
	}
	ورنہ
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    اگر (u > l && u > 2)
	flags |= WF_ALLCAP;
    ورنہ اگر (پہلا)
	flags |= WF_ONECAP;

    اگر (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    واپس flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    ساکن صحیح
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    صحیح		nr;
    صحیح		c;

    c = *p++;
    اگر ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    ورنہ اگر ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    ورنہ اگر ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    ورنہ				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    واپس nr;
}

// values for sps_flags
#تعریف_کرو SPS_بہترین    1
#تعریف_کرو SPS_FAST    2
#تعریف_کرو SPS_DOUBLE  4

ساکن صحیح sps_flags = SPS_بہترین;	// flags from 'spellsuggest'
ساکن صحیح sps_حد_بندی = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    صحیح
spell_check_sps(خالی)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    صحیح		f;

    sps_flags = 0;
    sps_حد_بندی = 9999;

    برائے (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	اگر (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_حد_بندی = getdigits(&s);
	    اگر (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	ورنہ اگر (STRCMP(buf, "best") == 0)
	    f = SPS_بہترین;
	ورنہ اگر (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	ورنہ اگر (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	ورنہ اگر (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	اگر (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_بہترین;
	    sps_حد_بندی = 9999;
	    واپس FAIL;
	}
	اگر (f != 0)
	    sps_flags = f;
    }

    اگر (sps_flags == 0)
	sps_flags = SPS_بہترین;

    واپس OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    خالی
spell_suggest(صحیح گنتی)
{
    char_u	*سطر = تہی;
    pos_T	پچھلا_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    صحیح		i;
    صحیح		c;
    suginfo_T	sug;
    suggest_T	*stp;
    صحیح		mouse_مستعمل;
    صحیح		need_cap;
    صحیح		حد_بندی;
    صحیح		selected = گنتی;
    صحیح		badlen = 0;
    صحیح		پیغام_scroll_save = پیغام_scroll;
    صحیح		wo_spell_save = curwin->w_p_spell;

    اگر (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    پچھلا_cursor = curwin->w_cursor;

    اگر (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	جاؤ skip;
    }

    اگر (VIsual_فعال)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	اگر (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    جاؤ skip;
	}
	badlen = (صحیح)curwin->w_cursor.ستون - (صحیح)VIsual.ستون;
	اگر (badlen < 0)
	    badlen = -badlen;
	ورنہ
	    curwin->w_cursor.ستون = VIsual.ستون;
	++badlen;
	انتہا_visual_منوال();
	// make sure we don't include the NUL at the end of the line
	اگر (badlen > ml_get_curline_len() - (صحیح)curwin->w_cursor.ستون)
	    badlen = ml_get_curline_len() - (صحیح)curwin->w_cursor.ستون;
    }
    // Find the start of the badly spelled word.
    ورنہ اگر (spell_move_مقصد(curwin, FORWARD, SMT_ALL, TRUE, تہی) == 0
	    || curwin->w_cursor.ستون > پچھلا_cursor.ستون)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = پچھلا_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.ستون;
	// Backup to before start of word.
	جب (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_عقب(curline, p);
	// Forward to start of word.
	جب (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	اگر (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    جاؤ skip;
	}
	curwin->w_cursor.ستون = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.ستون);

    // Make a copy of current line since autocommands may free the line.
    سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    اگر (سطر == تہی)
	جاؤ skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    اگر (sps_حد_بندی > (صحیح)صفیں - 2)
	حد_بندی = (صحیح)صفیں - 2;
    ورنہ
	حد_بندی = sps_حد_بندی;
    spell_ڈھونڈو_suggest(سطر + curwin->w_cursor.ستون, badlen, &sug, حد_بندی,
							TRUE, need_cap, TRUE);

    اگر (sug.su_ga.ga_len == 0)
	پیغام(_("Sorry, no suggestions"));
    ورنہ اگر (گنتی > 0)
    {
	اگر (گنتی > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (طویل)sug.su_ga.ga_len);
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	اگر (cmdmsg_rl)
	    پیغام_ستون = cmdline_چوڑائی - 1;
#اگر_ختم

	// List the suggestions.
	پیغام_آغاز();
	پیغام_صف = صفیں - 1;	// for when 'cmdheight' > 1
	سطریں_بایاں = صفیں;	// avoid more prompt
	vim_snprintf((حرف *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((حرف *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#اگر_ختم
	پیغام_puts((حرف *)IObuff);
	پیغام_clr_eos();
	پیغام_putchar('\n');

	پیغام_scroll = TRUE;
	برائے (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    صحیح el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_لفظ, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    اگر (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%2d", i + 1);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (cmdmsg_rl)
		rl_mirror(IObuff);
#اگر_ختم
	    پیغام_puts((حرف *)IObuff);

	    vim_snprintf((حرف *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    پیغام_puts((حرف *)IObuff);

	    // The word may replace more than "su_badlen".
	    اگر (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		پیغام_puts((حرف *)IObuff);
	    }

	    اگر (p_verbose > 0)
	    {
		// Add the score.
		اگر (sps_flags & (SPS_DOUBLE | SPS_بہترین))
		    vim_snprintf((حرف *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		ورنہ
		    vim_snprintf((حرف *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		اگر (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#اگر_ختم
		پیغام_advance(30);
		پیغام_puts((حرف *)IObuff);
	    }
	    پیغام_putchar('\n');
	}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	پیغام_ستون = 0;
#اگر_ختم
	// Ask for choice.
	selected = prompt_for_نمبر(&mouse_مستعمل);
	اگر (mouse_مستعمل)
	    selected -= سطریں_بایاں;
	سطریں_بایاں = صفیں;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	پیغام_scroll = پیغام_scroll_save;
    }

    اگر (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_مقصد);

	stp = &SUG(sug.su_ga, selected - 1);
	اگر (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%s%.*s", stp->st_لفظ,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_مقصد = vim_strsave(IObuff);
	}
	ورنہ
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_مقصد = vim_strsave(stp->st_لفظ);
	}

	// Replace the word.
	p = alloc(STRLEN(سطر) - stp->st_orglen + stp->st_wordlen + 1);
	اگر (p != تہی)
	{
	    صحیح len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (صحیح)(sug.su_badptr - سطر);
	    mch_memmove(p, سطر, c);
	    STRCPY(p + c, stp->st_لفظ);
	    STRCAT(p, sug.su_badptr + stp->st_orglen);

	    // For redo we use a change-word command.
	    ResetRedobuff();
	    Append_مقصد_Redobuff((char_u *)"ciw");
	    Append_مقصد_Redobuff_Lit(p + c,
			    stp->st_wordlen + sug.su_badlen - stp->st_orglen);
	    Append_Char_مقصد_Redobuff(ESC);

	    // "p" may be freed here
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    curwin->w_cursor.ستون = c;

	    بدلا_بائٹیں(curwin->w_cursor.lnum, c);
#اگر defined(FEAT_PROP_POPUP)
	    اگر (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#اگر_ختم
	}
    }
    ورنہ
	curwin->w_cursor = پچھلا_cursor;

    spell_ڈھونڈو_cleanup(&sug);
skip:
    vim_free(سطر);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    خالی
spell_suggest_list(
    garray_T	*gap,
    char_u	*لفظ,
    صحیح		maxcount,	// maximum nr of suggestions
    صحیح		need_cap,	// 'spellcapcheck' matched
    صحیح		interactive)
{
    suginfo_T	sug;
    صحیح		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_ڈھونڈو_suggest(لفظ, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, حجم(char_u *), sug.su_ga.ga_len + 1);
    اگر (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	برائے (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (غیرعلامتی)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    اگر (wcopy == تہی)
		توڑو;
	    STRCPY(wcopy, stp->st_لفظ);
	    STRCPY(wcopy + stp->st_wordlen, sug.su_badptr + stp->st_orglen);
	    ((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = wcopy;
	}
    }

    spell_ڈھونڈو_cleanup(&sug);
}

/*
 * Find spell suggestions for the word at the start of "badptr".
 * Return the suggestions in "su->su_ga".
 * The maximum number of suggestions is "maxcount".
 * Note: does use info for the current window.
 * This is based on the mechanisms of Aspell, but completely reimplemented.
 */
    ساکن خالی
spell_ڈھونڈو_suggest(
    char_u	*badptr,
    صحیح		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    صحیح		maxcount,
    صحیح		banbadword,	// don't include badword in suggestions
    صحیح		need_cap,	// word should start with capital
    صحیح		interactive)
{
    hlf_T	attr = HLF_گنتی;
    char_u	buf[MAXPATHL];
    char_u	*p;
    صحیح		do_combine = FALSE;
    char_u	*sps_copy;
#اگر_تعریف_شدہ FEAT_EVAL
    ساکن صحیح	اظہاریہ_busy = FALSE;
#اگر_ختم
    صحیح		c;
    صحیح		i;
    langp_T	*lp;
    صحیح		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_اشارہ_گر(su);
    ga_init2(&su->su_ga, حجم(suggest_T), 10);
    ga_init2(&su->su_sga, حجم(suggest_T), 10);
    اگر (*badptr == NUL)
	واپس;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    اگر (badlen != 0)
	su->su_badlen = badlen;
    ورنہ
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, تہی, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    اگر (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (خالی)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    اگر (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    برائے (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_اندراج(curbuf->b_s.b_langp, i);
	اگر (lp->lp_sallang != تہی)
	{
	    su->su_sallang = lp->lp_sallang;
	    توڑو;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    اگر (su->su_sallang != تہی)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    اگر (!SPELL_ISUPPER(c) && attr == HLF_گنتی)
    {
	make_case_لفظ(su->su_badword, buf, WF_ONECAP);
	جوڑو_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    اگر (banbadword)
	جوڑو_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    اگر (sps_copy == تہی)
	واپس;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    برائے (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	اگر (STRNCMP(buf, "expr:", 5) == 0)
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    اگر (!اظہاریہ_busy)
	    {
		اظہاریہ_busy = TRUE;
		spell_suggest_اظہاریہ(su, buf + 5);
		اظہاریہ_busy = FALSE;
	    }
#اگر_ختم
	}
	ورنہ اگر (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	ورنہ اگر (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((حرف *)buf + 8);
	ورنہ اگر (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    اگر (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    اگر (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    ساکن خالی
spell_suggest_اظہاریہ(suginfo_T *su, char_u *اظہاریہ)
{
    list_T	*list;
    listitem_T	*li;
    صحیح		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_اظہاریہ(su->su_badword, اظہاریہ);
    اگر (list != تہی)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_اشیاء(list, li)
	    اگر (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		اگر (score >= 0 && score <= su->su_maxscore)
		    جوڑو_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (خالی)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#اگر_ختم

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    ساکن خالی
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	سطر[MAXWLEN * 2];
    char_u	*p;
    صحیح		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((حرف *)fname, "r");
    اگر (fd == تہی)
    {
	semsg(_(e_cant_open_file_str), fname);
	واپس;
    }

    // Read it line by line.
    جب (!vim_fgets(سطر, MAXWLEN * 2, fd) && !got_int)
    {
	سطر_breakcheck();

	p = vim_strchr(سطر, '/');
	اگر (p == تہی)
	    جاری;	    // No Tab found, just skip the line.
	*p++ = NUL;
	اگر (STRICMP(su->su_badword, سطر) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    برائے (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    اگر (captype(p, تہی) == 0)
	    {
		make_case_لفظ(p, cword, su->su_badflags);
		p = cword;
	    }

	    جوڑو_suggestion(su, &su->su_ga, p, su->su_badlen,
				  SCORE_FILE, 0, TRUE, su->su_sallang, FALSE);
	}
    }

    fclose(fd);

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (خالی)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    ساکن خالی
spell_suggest_intern(suginfo_T *su, صحیح interactive)
{
    // Load the .sug file(s) that are available and not done yet.
    suggest_بھرو_files();

    // 1. Try special cases, such as repeating a word: "the the" -> "the".
    //
    // Set a maximum score to limit the combination of operations that is
    // tried.
    suggest_try_special(su);

    // 2. Try inserting/deleting/swapping/changing a letter, use REP entries
    //    from the .aff file and inserting a space (split the word).
    suggest_try_change(su);

    // For the resulting top-scorers compute the sound-a-like score.
    اگر (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    اگر ((sps_flags & SPS_FAST) == 0)
    {
	اگر (sps_flags & SPS_بہترین)
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
	اگر (su->su_ga.ga_len < SUG_CLEAN_گنتی(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    اگر (su->su_ga.ga_len < SUG_CLEAN_گنتی(su))
	    {
		// Still didn't find enough matches, try again, allowing even
		// more changes to the soundfold word.
		su->su_maxscore = SCORE_SFMAX3;
		suggest_try_soundalike(su);
	    }
	}
	su->su_maxscore = su->su_sfmaxscore;
	suggest_try_soundalike_اختتام();
    }

    // When CTRL-C was hit while searching do show the results.  Only clear
    // got_int when using a command, not for spellsuggest().
    ui_breakcheck();
    اگر (interactive && got_int)
    {
	(خالی)vgetc();
	got_int = FALSE;
    }

    اگر ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	اگر (sps_flags & SPS_بہترین)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(خالی)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    ساکن خالی
spell_ڈھونڈو_cleanup(suginfo_T *su)
{
    صحیح		i;

    // Free the suggestions.
    برائے (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_لفظ);
    ga_clear(&su->su_ga);
    برائے (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_لفظ);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    ساکن خالی
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    صحیح		c;
    char_u	لفظ[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    اگر (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
    {
	// Include badflags: if the badword is onecap or allcap
	// use that for the goodword too: "The the" -> "The".
	c = su->su_fbadword[len];
	su->su_fbadword[len] = NUL;
	make_case_لفظ(su->su_fbadword, لفظ, su->su_badflags);
	su->su_fbadword[len] = c;

	// Give a soundalike score of 0, compute the score as if deleting one
	// character.
	جوڑو_suggestion(su, &su->su_ga, لفظ, su->su_badlen,
		       RESCORE(SCORE_REP, 0), 0, TRUE, su->su_sallang, FALSE);
    }
}

/*
 * Change the 0 to 1 to measure how much time is spent in each state.
 * Output is dumped in "suggestprof".
 */
#اگر 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T کل;
proftime_T times[STATE_FINAL + 1];
طویل counts[STATE_FINAL + 1];

    ساکن خالی
prof_init(خالی)
{
    برائے (صحیح i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_آغاز(&current);
    profile_آغاز(&کل);
}

// call before changing state
    ساکن خالی
prof_store(state_T state)
{
    profile_انتہا(&current);
    profile_جوڑو(&times[state], &current);
    ++counts[state];
    profile_آغاز(&current);
}
# define PROF_STORE(state) prof_store(state);

    ساکن خالی
prof_report(حرف *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_انتہا(&کل);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_پیغام(&کل));
    برائے (صحیح i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_پیغام(&times[i]), counts[i]);
    fclose(fd);
}
#ورنہ
# define PROF_STORE(state)
#اگر_ختم

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    ساکن خالی
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    صحیح		n;
    char_u	*p;
    صحیح		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (صحیح)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (خالی)spell_casefold(curwin, p, (صحیح)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (صحیح)STRLEN(su->su_badptr);
    اگر (n < MAXWLEN)
	fword[n] = NUL;

    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	اگر (lp->lp_slang->sl_fbyts == تہی)
	    جاری;

	// Try it for this language.  Will add possible suggestions.
#اگر_تعریف_شدہ SUGGEST_PROFILE
	prof_init();
#اگر_ختم
	suggest_trie_walk(su, lp, fword, FALSE);
#اگر_تعریف_شدہ SUGGEST_PROFILE
	prof_report("try_change");
#اگر_ختم
    }
}

// Check the maximum score, if we go over it we won't try this change.
#تعریف_کرو TRY_DEEPER(su, ڈھیر, گہرائی, جوڑو) \
       ((گہرائی) < MAXWLEN - 1 && (ڈھیر)[گہرائی].ts_score + (جوڑو) < (su)->su_maxscore)

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
    ساکن خالی
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    صحیح		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	ڈھیر[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    صحیح		newscore;
    صحیح		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    صحیح		گہرائی;
    صحیح		c, c2, c3;
    صحیح		n = 0;
    صحیح		flags;
    garray_T	*gap;
    idx_T	arridx;
    صحیح		len;
    char_u	*p;
    fromto_T	*ftp;
    صحیح		fl = 0, tl;
    صحیح		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    صحیح		fword_سرے;
    صحیح		goodword_سرے;
#اگر_تعریف_شدہ DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#اگر_ختم
    صحیح		breakcheckcount = 1000;
#اگر_تعریف_شدہ FEAT_RELTIME
    proftime_T	وقت_حد_بندی;
#اگر_ختم
    صحیح		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    گہرائی = 0;
    sp = &ڈھیر[0];
    CLEAR_اشارہ_گر(sp);
    sp->ts_curi = 1;

    اگر (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = تہی;
	pidxs = تہی;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_آغاز;
    }
    ورنہ
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	اگر (pbyts != تہی)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	ورنہ
	{
	    byts = fbyts;
	    idxs = fidxs;
	    sp->ts_prefixdepth = PFD_NOPREFIX;
	    sp->ts_state = STATE_آغاز;
	}
    }
#اگر_تعریف_شدہ FEAT_RELTIME
    // The loop may take an indefinite amount of time. Break out after some
    // time.
    اگر (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &وقت_حد_بندی);
#اگر_ختم

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    جب (گہرائی >= 0 && !got_int)
    {
	sp = &ڈھیر[گہرائی];
	انتخاب (sp->ts_state)
	{
	حالت STATE_آغاز:
	حالت STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    اگر (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		برائے (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (صحیح)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		اگر (گہرائی < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (صحیح)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    اگر (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    ورنہ
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		    sprintf(changename[گہرائی], "prefix");
#اگر_ختم
		    go_deeper(ڈھیر, گہرائی, 0);
		    ++گہرائی;
		    sp = &ڈھیر[گہرائی];
		    sp->ts_prefixdepth = گہرائی - 1;
		    byts = fbyts;
		    idxs = fidxs;
		    sp->ts_arridx = 0;

		    // Move the prefix to preword[] with the right case
		    // and make find_keepcap_word() works.
		    tword[sp->ts_twordlen] = NUL;
		    make_case_لفظ(tword + sp->ts_splitoff,
					  preword + sp->ts_prewordlen, flags);
		    sp->ts_prewordlen = (char_u)STRLEN(preword);
		    sp->ts_splitoff = sp->ts_twordlen;
		}
		توڑو;
	    }

	    اگر (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		توڑو;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (صحیح)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    اگر (flags & WF_NOSUGGEST)
		توڑو;

	    fword_سرے = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    اگر (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != تہی)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = ڈھیر[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		برائے (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		اگر (c > 0)
		{
		    c = valid_لفظ_سابقہ(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    اگر (c == 0)
			توڑو;

		    // Use the WF_RARE flag for a rare prefix.
		    اگر (c & WF_RAREPFX)
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
	    اگر (sp->ts_complen == sp->ts_compsplit && fword_سرے
						     && (flags & WF_NEEDCOMP))
		goodword_سرے = FALSE;
	    ورنہ
		goodword_سرے = TRUE;

	    p = تہی;
	    compound_ok = TRUE;
	    اگر (sp->ts_complen > sp->ts_compsplit)
	    {
		اگر (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    اگر (sp->ts_fidx - sp->ts_splitfidx
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
			اگر (newscore <= su->su_maxscore)
			    جوڑو_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			توڑو;
		    }
		}
		ورنہ
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    اگر (((غیرعلامتی)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			توڑو;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    اگر (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			توڑو;

		    compflags[sp->ts_complen] = ((غیرعلامتی)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    اگر (مطابقت_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    اگر (compound_ok)
		    {
			p = preword;
			جب (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			اگر (fword_سرے && !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    // Compound is not allowed.  But it may still be
			    // possible if we add another (short) word.
			    compound_ok = FALSE;
		    }

		    // Get pointer to last char of previous word.
		    p = preword + sp->ts_prewordlen;
		    MB_PTR_عقب(preword, p);
		}
	    }

	    // Form the word with proper case in preword.
	    // If there is a word from a previous split, append.
	    // For the soundfold tree don't change the case, simply append.
	    اگر (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    ورنہ اگر (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		ڈھونڈو_keepcap_لفظ(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    ورنہ
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		اگر ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		اگر (p != تہی && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_لفظ(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    اگر (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		اگر (flags & WF_BANNED)
		{
		    جوڑو_banned(su, preword + sp->ts_prewordlen);
		    توڑو;
		}
		اگر ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    اگر (slang->sl_compprog == تہی)
			توڑو;
		    // the word so far was banned but we may try compounding
		    goodword_سرے = FALSE;
		}
	    }

	    newscore = 0;
	    اگر (!soundfold)	// soundfold words don't have flags
	    {
		اگر ((flags & WF_REGION)
			    && (((غیرعلامتی)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		اگر (flags & WF_RARE)
		    newscore += SCORE_RARE;

		اگر (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, تہی)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    اگر (fword_سرے
		    && goodword_سرے
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		اگر (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    صحیح	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    برائے (j = 0; j < گہرائی; ++j)
			smsg("%s", changename[j]);
		}
#اگر_ختم
		اگر (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    جوڑو_sound_suggest(su, preword, sp->ts_score, lp);
		}
		ورنہ اگر (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_عقب(fword, p);
		    اگر (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_عقب(preword, p);
			اگر (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    اگر (score <= su->su_maxscore)
		    {
			جوڑو_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			اگر (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, تہی);
			    اگر (c == 0 || c == WF_ALLCAP)
			    {
				make_case_لفظ(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen,
						      c == 0 ? WF_ALLCAP : 0);

				جوڑو_suggestion(su, &su->su_ga, preword,
					sp->ts_fidx - repextra,
					score + SCORE_ICASE, 0, FALSE,
					lp->lp_sallang, FALSE);
			    }
			}
		    }
		}
	    }

	    // Try word split and/or compounding.
	    اگر ((sp->ts_fidx >= sp->ts_fidxtry || fword_سرے)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		صحیح	try_compound;
		صحیح	try_بانٹو;

		// If past the end of the bad word don't try a split.
		// Otherwise try changing the next word.  E.g., find
		// suggestions for "the the" where the second "the" is
		// different.  It's done like a split.
		// TODO: word split for soundfold words
		try_بانٹو = (sp->ts_fidx - repextra < su->su_badlen)
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
		اگر (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != تہی
			&& ((غیرعلامتی)flags >> 24) != 0
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
					 compflags, ((غیرعلامتی)flags >> 24))))

		{
		    try_compound = TRUE;
		    compflags[sp->ts_complen] = ((غیرعلامتی)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		}

		// For NOBREAK we never try splitting, it won't make any word
		// valid.
		اگر (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		ورنہ اگر (!fword_سرے
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		ورنہ
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		اگر (try_بانٹو || try_compound)
		{
		    اگر (!try_compound && (!fword_سرے || !goodword_سرے))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			اگر (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    توڑو;
			p = preword;
			جب (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			اگر (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    توڑو;

			اگر (slang->sl_nosplitsugs)
			    newscore += SCORE_بانٹو_NO;
			ورنہ
			    newscore += SCORE_بانٹو;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    اگر (TRY_DEEPER(su, ڈھیر, گہرائی, newscore))
		    {
			go_deeper(ڈھیر, گہرائی, newscore);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
			اگر (!try_compound && !fword_سرے)
			    sprintf(changename[گہرائی], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			ورنہ
			    sprintf(changename[گہرائی], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#اگر_ختم
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++گہرائی;
			sp = &ڈھیر[گہرائی];

			// Append a space to preword when splitting.
			اگر (!try_compound && !fword_سرے)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			اگر (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_سرے)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_سرے)
			{
			    صحیح	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    اگر (fword_سرے)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    ورنہ
				sp->ts_score -= SCORE_بانٹو - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			اگر (try_compound)
			    ++sp->ts_complen;
			ورنہ
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			اگر (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			ورنہ
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			اگر (pbyts != تہی)
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
	    توڑو;

	حالت STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_آغاز;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    توڑو;

	حالت STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    اگر (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		توڑو;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	حالت STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    اگر (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		اگر (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		ورنہ
		    sp->ts_state = STATE_FINAL;
	    }
	    ورنہ
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		اگر (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		ورنہ
		    newscore = SCORE_SUBST;
		اگر ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, ڈھیر, گہرائی, newscore))
		{
		    go_deeper(ڈھیر, گہرائی, newscore);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		    اگر (newscore > 0)
			sprintf(changename[گہرائی], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    ورنہ
			sprintf(changename[گہرائی], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#اگر_ختم
		    ++گہرائی;
		    sp = &ڈھیر[گہرائی];
		    اگر (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    اگر (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    اگر (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			اگر (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			ورنہ اگر (sp->ts_isdiff == DIFF_داخل_کرو
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			اگر (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    اگر (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				اگر (enc_utf8
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
				ورنہ اگر (!soundfold
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
			    ورنہ اگر (sp->ts_isdiff == DIFF_داخل_کرو
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				اگر (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				ورنہ
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_عقب(tword, p);
				    اگر (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    ورنہ
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			اگر (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    توڑو;

	حالت STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    اگر (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		توڑو;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    اگر (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    ورنہ
		newscore = SCORE_DEL;
	    اگر (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, ڈھیر, گہرائی, newscore))
	    {
		go_deeper(ڈھیر, گہرائی, newscore);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		sprintf(changename[گہرائی], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#اگر_ختم
		++گہرائی;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		ڈھیر[گہرائی].ts_flags |= TSF_DIDDEL;
		ڈھیر[گہرائی].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		اگر (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    ڈھیر[گہرائی].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    اگر (enc_utf8 && utf_iscomposing(c))
			ڈھیر[گہرائی].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    ورنہ اگر (c == mb_ptr2char(fword + ڈھیر[گہرائی].ts_fidx))
			ڈھیر[گہرائی].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ورنہ
		{
		    ++ڈھیر[گہرائی].ts_fidx;
		    اگر (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			ڈھیر[گہرائی].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		توڑو;
	    }
	    // FALLTHROUGH

	حالت STATE_INS_PREP:
	    اگر (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_تبادلہ;
		توڑو;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    برائے (;;)
	    {
		اگر (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_تبادلہ;
		    توڑو;
		}
		اگر (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    توڑو;
		}
		++sp->ts_curi;
	    }
	    توڑو;

	    // FALLTHROUGH

	حالت STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    اگر (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_تبادلہ;
		توڑو;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    اگر (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		توڑو;
	    }
	    c = byts[n];
	    اگر (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    ورنہ
		newscore = SCORE_INS;
	    اگر (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, ڈھیر, گہرائی, newscore))
	    {
		go_deeper(ڈھیر, گہرائی, newscore);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		sprintf(changename[گہرائی], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#اگر_ختم
		++گہرائی;
		sp = &ڈھیر[گہرائی];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		اگر (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    اگر (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_داخل_کرو;
		    }
		}
		ورنہ
		    fl = 1;
		اگر (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    اگر (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    توڑو;

	حالت STATE_تبادلہ:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    اگر (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		توڑو;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    اگر (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		توڑو;
	    }

	    اگر (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		اگر (p[n] == NUL)
		    c2 = NUL;
		ورنہ اگر (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		ورنہ
		    c2 = mb_ptr2char(p + n);
	    }
	    ورنہ
	    {
		اگر (p[1] == NUL)
		    c2 = NUL;
		ورنہ اگر (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		ورنہ
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    اگر (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		توڑو;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    اگر (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		توڑو;
	    }
	    اگر (c2 != NUL && TRY_DEEPER(su, ڈھیر, گہرائی, SCORE_تبادلہ))
	    {
		go_deeper(ڈھیر, گہرائی, SCORE_تبادلہ);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		sprintf(changename[گہرائی], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#اگر_ختم
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++گہرائی;
		اگر (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		ورنہ
		{
		    p[0] = c2;
		    p[1] = c;
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    ورنہ
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    توڑو;

	حالت STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    اگر (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    ورنہ
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	حالت STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    اگر (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		اگر (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		ورنہ
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    ورنہ
	    {
		c = *p;
		c2 = p[1];
		اگر (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		ورنہ
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    اگر (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		توڑو;
	    }
	    اگر (TRY_DEEPER(su, ڈھیر, گہرائی, SCORE_SWAP3))
	    {
		go_deeper(ڈھیر, گہرائی, SCORE_SWAP3);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		sprintf(changename[گہرائی], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#اگر_ختم
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++گہرائی;
		اگر (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		ورنہ
		{
		    p[0] = p[2];
		    p[2] = c;
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ورنہ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    توڑو;

	حالت STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    اگر (has_mbyte)
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
	    ورنہ
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    اگر (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		توڑو;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    اگر (TRY_DEEPER(su, ڈھیر, گہرائی, SCORE_SWAP3))
	    {
		go_deeper(ڈھیر, گہرائی, SCORE_SWAP3);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[گہرائی], "%.*s-%s: rotate left %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#اگر_ختم
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3L;
		++گہرائی;
		p = fword + sp->ts_fidx;
		اگر (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		ورنہ
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ورنہ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    توڑو;

	حالت STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    اگر (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    ورنہ
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    اگر (TRY_DEEPER(su, ڈھیر, گہرائی, SCORE_SWAP3))
	    {
		go_deeper(ڈھیر, گہرائی, SCORE_SWAP3);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[گہرائی], "%.*s-%s: rotate right %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#اگر_ختم
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3R;
		++گہرائی;
		p = fword + sp->ts_fidx;
		اگر (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		ورنہ
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ورنہ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    توڑو;

	حالت STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    اگر (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    ورنہ
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	حالت STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    اگر ((lp->lp_replang == تہی && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		توڑو;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    اگر (soundfold)
		sp->ts_curi = slang->sl_repsal_پہلا[fword[sp->ts_fidx]];
	    ورنہ
		sp->ts_curi = lp->lp_replang->sl_rep_پہلا[fword[sp->ts_fidx]];

	    اگر (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		توڑو;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	حالت STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    اگر (soundfold)
		gap = &slang->sl_repsal;
	    ورنہ
		gap = &lp->lp_replang->sl_rep;
	    جب (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_ڈیٹا + sp->ts_curi++;
		اگر (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    توڑو;
		}
		اگر (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, ڈھیر, گہرائی, SCORE_REP))
		{
		    go_deeper(ڈھیر, گہرائی, SCORE_REP);
#اگر_تعریف_شدہ DEBUG_TRIEWALK
		    sprintf(changename[گہرائی], "%.*s-%s: replace %s with %s",
			    sp->ts_twordlen, tword, fword + sp->ts_fidx,
			    ftp->ft_from, ftp->ft_مقصد);
#اگر_ختم
		    // Need to undo this afterwards.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_REP_UNDO;

		    // Change the "from" to the "to" string.
		    ++گہرائی;
		    fl = (صحیح)STRLEN(ftp->ft_from);
		    tl = (صحیح)STRLEN(ftp->ft_مقصد);
		    اگر (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_مقصد, tl);
		    ڈھیر[گہرائی].ts_fidxtry = sp->ts_fidx + tl;
		    ڈھیر[گہرائی].ts_tcharlen = 0;
		    توڑو;
		}
	    }

	    اگر (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    توڑو;

	حالت STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    اگر (soundfold)
		gap = &slang->sl_repsal;
	    ورنہ
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_ڈیٹا + sp->ts_curi - 1;
	    fl = (صحیح)STRLEN(ftp->ft_from);
	    tl = (صحیح)STRLEN(ftp->ft_مقصد);
	    p = fword + sp->ts_fidx;
	    اگر (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    توڑو;

	طےشدہ:
	    // Did all possible states at this level, go up one level.
	    --گہرائی;

	    اگر (گہرائی >= 0 && ڈھیر[گہرائی].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    اگر (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#اگر_تعریف_شدہ FEAT_RELTIME
		اگر (spell_suggest_timeout > 0
					  && profile_passed_حد_بندی(&وقت_حد_بندی))
		    got_int = TRUE;
#اگر_ختم
	    }
	}
    }
}


/*
 * Go one level deeper in the tree.
 */
    ساکن خالی
go_deeper(trystate_T *ڈھیر, صحیح گہرائی, صحیح score_جوڑو)
{
    ڈھیر[گہرائی + 1] = ڈھیر[گہرائی];
    ڈھیر[گہرائی + 1].ts_state = STATE_آغاز;
    ڈھیر[گہرائی + 1].ts_score = ڈھیر[گہرائی].ts_score + score_جوڑو;
    ڈھیر[گہرائی + 1].ts_curi = 1;	// start just after length byte
    ڈھیر[گہرائی + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    ساکن خالی
ڈھونڈو_keepcap_لفظ(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    صحیح		گہرائی;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    صحیح		باری[MAXWLEN];
    صحیح		fwordidx[MAXWLEN];
    صحیح		uwordidx[MAXWLEN];
    صحیح		kwordlen[MAXWLEN];

    صحیح		flen, ulen;
    صحیح		l;
    صحیح		len;
    صحیح		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    اگر (byts == تہی)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	واپس;
    }

    // Make an all-cap version of "fword".
    allcap_copy(fword, uword);

    // Each character needs to be tried both case-folded and upper-case.
    // All this gets very complicated if we keep in mind that changing case
    // may change the byte length of a multi-byte character...
    گہرائی = 0;
    arridx[0] = 0;
    باری[0] = 0;
    fwordidx[0] = 0;
    uwordidx[0] = 0;
    kwordlen[0] = 0;
    جب (گہرائی >= 0)
    {
	اگر (fword[fwordidx[گہرائی]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    اگر (byts[arridx[گہرائی] + 1] == 0)
	    {
		kword[kwordlen[گہرائی]] = NUL;
		واپس;
	    }

	    // kword is getting too long, continue one level up
	    --گہرائی;
	}
	ورنہ اگر (++باری[گہرائی] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --گہرائی;
	}
	ورنہ
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    اگر (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[گہرائی]);
		ulen = MB_CPTR2LEN(uword + uwordidx[گہرائی]);
	    }
	    ورنہ
		ulen = flen = 1;
	    اگر (باری[گہرائی] == 1)
	    {
		p = fword + fwordidx[گہرائی];
		l = flen;
	    }
	    ورنہ
	    {
		p = uword + uwordidx[گہرائی];
		l = ulen;
	    }

	    برائے (tryidx = arridx[گہرائی]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
		جب (lo < hi)
		{
		    m = (lo + hi) / 2;
		    اگر (byts[m] > c)
			hi = m - 1;
		    ورنہ اگر (byts[m] < c)
			lo = m + 1;
		    ورنہ
		    {
			lo = hi = m;
			توڑو;
		    }
		}

		// Stop if there is no matching byte.
		اگر (hi < lo || byts[lo] != c)
		    توڑو;

		// Continue at the child (if there is one).
		tryidx = idxs[lo];
	    }

	    اگر (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		اگر (باری[گہرائی] == 1)
		{
		    STRNCPY(kword + kwordlen[گہرائی], fword + fwordidx[گہرائی],
									flen);
		    kwordlen[گہرائی + 1] = kwordlen[گہرائی] + flen;
		}
		ورنہ
		{
		    STRNCPY(kword + kwordlen[گہرائی], uword + uwordidx[گہرائی],
									ulen);
		    kwordlen[گہرائی + 1] = kwordlen[گہرائی] + ulen;
		}
		fwordidx[گہرائی + 1] = fwordidx[گہرائی] + flen;
		uwordidx[گہرائی + 1] = uwordidx[گہرائی] + ulen;

		++گہرائی;
		arridx[گہرائی] = tryidx;
		باری[گہرائی] = 0;
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
    ساکن خالی
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    صحیح		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    صحیح		score;
    صحیح		lpi;

    اگر (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	واپس;

    // Use the sound-folding of the first language that supports it.
    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);
	اگر (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    برائے (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		اگر (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_لفظ = vim_strsave(stp->st_لفظ);
		    اگر (sstp->st_لفظ != تہی)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    توڑو;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    ساکن خالی
score_combine(suginfo_T *su)
{
    صحیح		i;
    صحیح		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    صحیح		باری;
    صحیح		lpi;
    slang_T	*slang = تہی;

    // Add the alternate score to su_ga.
    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);
	اگر (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    برائے (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		اگر (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		ورنہ
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    توڑو;
	}
    }

    اگر (slang == تہی)	// Using "double" without sound folding.
    {
	(خالی)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	واپس;
    }

    // Add the alternate score to su_sga.
    برائے (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_لفظ);
	اگر (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	ورنہ
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (خالی)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (خالی)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, حجم(suginfo_T), 1);
    اگر (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	واپس;

    stp = &SUG(ga, 0);
    برائے (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	برائے (باری = 1; باری <= 2; ++باری)
	{
	    gap = باری == 1 ? &su->su_ga : &su->su_sga;
	    اگر (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_لفظ;
		برائے (j = 0; j < ga.ga_len; ++j)
		    اگر (STRCMP(stp[j].st_لفظ, p) == 0)
			توڑو;
		اگر (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		ورنہ
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    اگر (ga.ga_len > su->su_maxcount)
    {
	برائے (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_لفظ);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    ساکن صحیح
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
    صحیح		lendiff;

    lendiff = (صحیح)(su->su_badlen - stp->st_orglen);
    اگر (lendiff >= 0)
	pbad = badsound;
    ورنہ
    {
	// soundfold the bad word with more characters following
	(خالی)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	اگر (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_لفظ) == NUL)
	    برائے (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    اگر (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_لفظ);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    ورنہ
	pgood = stp->st_لفظ;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    واپس soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
قسمتعریف ساخت
{
    مختصر	sft_score;	// lowest score used
    char_u	sft_لفظ[1];    // soundfolded word, actually longer
} sftword_T;

ساکن sftword_T dumsft;
#تعریف_کرو HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_لفظ - (char_u *)&dumsft)))
#تعریف_کرو HI2SFT(hi)     HIKEY2SFT((hi)->hi_کلید)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    ساکن خالی
suggest_try_soundalike_prep(خالی)
{
    langp_T	*lp;
    صحیح		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	اگر (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != تہی)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    ساکن خالی
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    صحیح		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	اگر (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != تہی)
	{
	    // soundfold the bad word
	    spell_soundfold(slang, su->su_fbadword, TRUE, salword);

	    // try all kinds of inserts/deletes/swaps/etc.
	    // TODO: also soundfold the next words, so that we can try joining
	    // and splitting
#اگر_تعریف_شدہ SUGGEST_PROFILE
	    prof_init();
#اگر_ختم
	    suggest_trie_walk(su, lp, salword, TRUE);
#اگر_تعریف_شدہ SUGGEST_PROFILE
	    prof_report("soundalike");
#اگر_ختم
	}
    }
}

/*
 * Finish up after calling suggest_try_soundalike().
 */
    ساکن خالی
suggest_try_soundalike_اختتام(خالی)
{
    langp_T	*lp;
    صحیح		lpi;
    slang_T	*slang;
    صحیح		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	اگر (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != تہی)
	{
	    // Free the info about handled words.
	    todo = (صحیح)slang->sl_sounddone.ht_مستعمل;
	    FOR_ALL_HASHTAB_اشیاء(&slang->sl_sounddone, hi, todo)
		اگر (!HASHITEM_خالی_خانے(hi))
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
    ساکن خالی
جوڑو_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    صحیح		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    صحیح		sfwordnr;
    char_u	*nrline;
    صحیح		orgnr;
    char_u	theword[MAXWLEN];
    صحیح		i;
    صحیح		wlen;
    char_u	*byts;
    idx_T	*idxs;
    صحیح		n;
    صحیح		wordcount;
    صحیح		wc;
    صحیح		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    صحیح		bc, gc;
    صحیح		حد_بندی;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    اگر (HASHITEM_خالی_خانے(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_لفظ) + STRLEN(goodword) + 1);
	اگر (sft != تہی)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_لفظ, goodword);
	    hash_جوڑو_شے(&slang->sl_sounddone, hi, sft->sft_لفظ, hash);
	}
    }
    ورنہ
    {
	sft = HI2SFT(hi);
	اگر (score >= sft->sft_score)
	    واپس;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_ڈھونڈو(slang, goodword);
    اگر (sfwordnr < 0)
    {
	اندرونی_error("add_sound_suggest()");
	واپس;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    جب (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	برائے (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    اگر (wordcount == orgnr && byts[n + 1] == NUL)
		توڑو;	// found end of word

	    اگر (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    برائے ( ; byts[n + i] == NUL; ++i)
		اگر (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    جاؤ badword;
		}

	    // One of the siblings must have the word.
	    برائے ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		اگر (wordcount + wc > orgnr)
		    توڑو;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	برائے (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    صحیح		flags = (صحیح)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    اگر (flags & WF_NOSUGGEST)
		جاری;

	    اگر (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		ڈھونڈو_keepcap_لفظ(slang, theword, cword);
		p = cword;
	    }
	    ورنہ
	    {
		flags |= su->su_badflags;
		اگر ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_لفظ(theword, cword, flags);
		    p = cword;
		}
		ورنہ
		    p = theword;
	    }

	    // Add the suggestion.
	    اگر (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		اگر (score <= su->su_maxscore)
		    جوڑو_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    ورنہ
	    {
		// Add a penalty for words in another region.
		اگر ((flags & WF_REGION)
			    && (((غیرعلامتی)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		ورنہ
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		اگر (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    اگر (!SPELL_ISUPPER(bc)
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
		حد_بندی = MAXSCORE(su->su_sfmaxscore - goodscore, score);
		اگر (حد_بندی > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		ورنہ
		    goodscore += spell_edit_score_حد_بندی(slang, su->su_badword,
								    p, حد_بندی);

		// When going over the limit don't bother to do the rest.
		اگر (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    اگر (goodscore <= su->su_sfmaxscore)
			جوڑو_suggestion(su, &su->su_ga, p, su->su_badlen,
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
    ساکن صحیح
soundfold_ڈھونڈو(slang_T *slang, char_u *لفظ)
{
    idx_T	arridx = 0;
    صحیح		len;
    صحیح		wlen = 0;
    صحیح		c;
    char_u	*ptr = لفظ;
    char_u	*byts;
    idx_T	*idxs;
    صحیح		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    برائے (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	اگر (byts[arridx] == NUL)
	{
	    اگر (c == NUL)
		توڑو;

	    // Skip over the zeros, there can be several.
	    جب (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    اگر (len == 0)
		واپس -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	اگر (c == NUL)
	    واپس -1;

	// Perform a binary search in the list of accepted bytes.
	اگر (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	جب (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    اگر (--len == 0)	// end of the bytes, didn't find it
		واپس -1;
	}
	اگر (byts[arridx] != c)	// didn't find the byte
	    واپس -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	اگر (c == ' ')
	    جب (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    واپس wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    ساکن صحیح
similar_chars(slang_T *slang, صحیح c1, صحیح c2)
{
    صحیح		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    اگر (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_ڈھونڈو(&slang->sl_map_hash, buf);
	اگر (HASHITEM_خالی_خانے(hi))
	    m1 = 0;
	ورنہ
	    m1 = mb_ptr2char(hi->hi_کلید + STRLEN(hi->hi_کلید) + 1);
    }
    ورنہ
	m1 = slang->sl_map_array[c1];
    اگر (m1 == 0)
	واپس FALSE;


    اگر (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_ڈھونڈو(&slang->sl_map_hash, buf);
	اگر (HASHITEM_خالی_خانے(hi))
	    m2 = 0;
	ورنہ
	    m2 = mb_ptr2char(hi->hi_کلید + STRLEN(hi->hi_کلید) + 1);
    }
    ورنہ
	m2 = slang->sl_map_array[c2];

    واپس m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    ساکن خالی
جوڑو_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    صحیح		badlenarg,	// len of bad word replaced with "goodword"
    صحیح		score,
    صحیح		altscore,
    صحیح		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    صحیح		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    صحیح		goodlen;	// len of goodword changed
    صحیح		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    صحیح		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    برائے (;;)
    {
	goodlen = (صحیح)(pgood - goodword);
	badlen = (صحیح)(pbad - su->su_badptr);
	اگر (goodlen <= 0 || badlen <= 0)
	    توڑو;
	MB_PTR_عقب(goodword, pgood);
	MB_PTR_عقب(su->su_badptr, pbad);
	اگر (has_mbyte)
	{
	    اگر (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		توڑو;
	}
	ورنہ اگر (*pgood != *pbad)
		توڑو;
    }

    اگر (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	واپس;

    اگر (gap->ga_len == 0)
	i = -1;
    ورنہ
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	برائے (i = gap->ga_len; --i >= 0; ++stp)
	    اگر (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_لفظ, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		اگر (stp->st_slang == تہی)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		اگر (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    اگر (had_bonus)
			rescore_one(su, stp);
		    ورنہ
		    {
			new_sug.st_لفظ = stp->st_لفظ;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		اگر (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		توڑو;
	    }
    }

    اگر (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_لفظ = vim_strnsave(goodword, goodlen);
	اگر (stp->st_لفظ != تہی)
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
	    اگر (gap->ga_len > SUG_MAX_گنتی(su))
	    {
		اگر (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_گنتی(su));
		ورنہ
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_گنتی(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    ساکن خالی
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    صحیح		i;
    char_u	longword[MAXWLEN + 1];
    صحیح		len;
    hlf_T	attr;

    اگر (gap->ga_len == 0)
	واپس;
    stp = &SUG(*gap, 0);
    برائے (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_لفظ, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_گنتی;
	(خالی)spell_check(curwin, longword, &attr, تہی, FALSE);
	اگر (attr != HLF_گنتی)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_لفظ);
	    --gap->ga_len;
	    اگر (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       حجم(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    ساکن خالی
جوڑو_banned(
    suginfo_T	*su,
    char_u	*لفظ)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(لفظ);
    hi = hash_lookup(&su->su_banned, لفظ, hash);
    اگر (!HASHITEM_خالی_خانے(hi))		// already present
	واپس;
    s = vim_strsave(لفظ);
    اگر (s != تہی)
	hash_جوڑو_شے(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    ساکن خالی
rescore_suggestions(suginfo_T *su)
{
    صحیح		i;

    اگر (su->su_sallang != تہی)
	برائے (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    ساکن خالی
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    اگر (slang != تہی && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	اگر (slang == su->su_sallang)
	    p = su->su_sal_badword;
	ورنہ
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	اگر (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

ساکن صحیح sug_compare(ثابت خالی *s1, ثابت خالی *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    ساکن صحیح
sug_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    صحیح		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    اگر (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	اگر (n == 0)
	    n = STRICMP(p1->st_لفظ, p2->st_لفظ);
    }
    واپس n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    ساکن صحیح
cleanup_suggestions(
    garray_T	*gap,
    صحیح		maxscore,
    صحیح		keep)		// nr of suggestions to keep
{
    اگر (gap->ga_len <= 0)
	واپس maxscore;

    // Sort the list.
    qsort(gap->ga_ڈیٹا, (size_t)gap->ga_len, حجم(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    اگر (gap->ga_len > keep)
    {
	صحیح		i;
	suggest_T   *stp = &SUG(*gap, 0);

	برائے (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_لفظ);
	gap->ga_len = keep;
	اگر (keep >= 1)
	    واپس stp[keep - 1].st_score;
    }
    واپس maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    ساکن صحیح
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    صحیح		goodlen;
    صحیح		badlen;
    صحیح		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    صحیح		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    اگر ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	اگر ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    واپس SCORE_DEL;
	اگر (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    واپس SCORE_MAXMAX;

	اگر (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	ورنہ
	{
	    score = 2 * SCORE_DEL / 3;
	    اگر (*badsound == '*')
		++badsound;
	    ورنہ
		++goodsound;
	}
    }

    goodlen = (صحیح)STRLEN(goodsound);
    badlen = (صحیح)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    اگر (n < -2 || n > 2)
	واپس SCORE_MAXMAX;

    اگر (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    ورنہ
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    جب (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    انتخاب (n)
    {
	حالت -2:
	حالت 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    جب (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    اگر (STRCMP(pl + 1, ps) == 0)
		واپس score + SCORE_DEL * 2;

	    // Failed to compare.
	    توڑو;

	حالت -1:
	حالت 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    جب (*pl2 == *ps2)
	    {
		اگر (*pl2 == NUL)	// reached the end
		    واپس score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    اگر (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		واپس score + SCORE_DEL + SCORE_تبادلہ;

	    // 3: delete then substitute, then the rest must be equal
	    اگر (STRCMP(pl2 + 1, ps2 + 1) == 0)
		واپس score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    اگر (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		جب (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		اگر (STRCMP(pl2 + 1, ps2) == 0)
		    واپس score + SCORE_تبادلہ + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    جب (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    اگر (STRCMP(pl2 + 1, ps2) == 0)
		واپس score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    توڑو;

	حالت 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    اگر (*pl == NUL)
		واپس score;

	    // 2: swap
	    اگر (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		جب (*pl2 == *ps2)
		{
		    اگر (*pl2 == NUL)	// reached the end
			واپس score + SCORE_تبادلہ;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		اگر (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    واپس score + SCORE_تبادلہ + SCORE_تبادلہ;

		// 4: swap and substitute
		اگر (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    واپس score + SCORE_تبادلہ + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    جب (*pl2 == *ps2)
	    {
		اگر (*pl2 == NUL)	// reached the end
		    واپس score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    اگر (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		واپس score + SCORE_SUBST + SCORE_تبادلہ;

	    // 7: substitute and substitute
	    اگر (STRCMP(pl2 + 1, ps2 + 1) == 0)
		واپس score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    جب (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    اگر (STRCMP(pl2 + 1, ps2) == 0)
		واپس score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    جب (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    اگر (STRCMP(pl2, ps2 + 1) == 0)
		واپس score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    توڑو;
    }

    واپس SCORE_MAXMAX;
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
    ساکن صحیح
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    صحیح		*cnt;
    صحیح		badlen, goodlen;	// lengths including NUL
    صحیح		j, i;
    صحیح		t;
    صحیح		bc, gc;
    صحیح		pbc, pgc;
    char_u	*p;
    صحیح		wbadword[MAXWLEN];
    صحیح		wgoodword[MAXWLEN];

    اگر (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	برائے (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	برائے (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    ورنہ
    {
	badlen = (صحیح)STRLEN(badword) + 1;
	goodlen = (صحیح)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#تعریف_کرو CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(صحیح, (badlen + 1) * (goodlen + 1));
    اگر (cnt == تہی)
	واپس 0;	// out of memory

    CNT(0, 0) = 0;
    برائے (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    برائے (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	برائے (j = 1; j <= goodlen; ++j)
	{
	    اگر (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    ورنہ
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    اگر (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    ورنہ
	    {
		// Use a better score when there is only a case difference.
		اگر (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		ورنہ
		{
		    // For a similar character use SCORE_SIMILAR.
		    اگر (slang != تہی
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    ورنہ
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		اگر (i > 1 && j > 1)
		{
		    اگر (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    ورنہ
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    اگر (bc == pgc && pbc == gc)
		    {
			t = SCORE_تبادلہ + CNT(i - 2, j - 2);
			اگر (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		اگر (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		اگر (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    واپس i;
}

قسمتعریف ساخت
{
    صحیح		badi;
    صحیح		goodi;
    صحیح		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    ساکن صحیح
spell_edit_score_حد_بندی(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    صحیح		حد_بندی)
{
    limitscore_T    ڈھیر[10];		// allow for over 3 * 2 edits
    صحیح		    stackidx;
    صحیح		    bi, gi;
    صحیح		    bi2, gi2;
    صحیح		    bc, gc;
    صحیح		    score;
    صحیح		    score_off;
    صحیح		    minscore;
    صحیح		    باری;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    اگر (has_mbyte)
	واپس spell_edit_score_حد_بندی_w(slang, badword, goodword, حد_بندی);

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
    minscore = حد_بندی + 1;

    برائے (;;)
    {
	// Skip over an equal part, score remains the same.
	برائے (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    اگر (bc != gc)	// stop at a char that's different
		توڑو;
	    اگر (bc == NUL)	// both words end
	    {
		اگر (score < minscore)
		    minscore = score;
		جاؤ نکالو;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	اگر (gc == NUL)    // goodword ends, delete badword chars
	{
	    کرو
	    {
		اگر ((score += SCORE_DEL) >= minscore)
		    جاؤ نکالو;	    // do next alternative
	    } جب (badword[++bi] != NUL);
	    minscore = score;
	}
	ورنہ اگر (bc == NUL) // badword ends, insert badword chars
	{
	    کرو
	    {
		اگر ((score += SCORE_INS) >= minscore)
		    جاؤ نکالو;	    // do next alternative
	    } جب (goodword[++gi] != NUL);
	    minscore = score;
	}
	ورنہ			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    برائے (باری = 0; باری <= 1; ++باری)
	    {
		score_off = score + (باری == 0 ? SCORE_DEL : SCORE_INS);
		اگر (score_off < minscore)
		{
		    اگر (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - باری;
			gi2 = gi + باری;
			جب (goodword[gi2] == badword[bi2])
			{
			    اگر (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				توڑو;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    ورنہ
		    {
			// try deleting/inserting a character later
			ڈھیر[stackidx].badi = bi + 1 - باری;
			ڈھیر[stackidx].goodi = gi + باری;
			ڈھیر[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    اگر (score + SCORE_تبادلہ < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		اگر (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_تبادلہ;
		    جاری;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    اگر (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    ورنہ
	    {
		// For a similar character use SCORE_SIMILAR.
		اگر (slang != تہی
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		ورنہ
		    score += SCORE_SUBST;
	    }

	    اگر (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		جاری;
	    }
	}
نکالو:
	// Get here to try the next alternative, pop it from the stack.
	اگر (stackidx == 0)		// stack is empty, finished
	    توڑو;

	// pop an item from the stack
	--stackidx;
	gi = ڈھیر[stackidx].goodi;
	bi = ڈھیر[stackidx].badi;
	score = ڈھیر[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    اگر (minscore > حد_بندی)
	واپس SCORE_MAXMAX;
    واپس minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    ساکن صحیح
spell_edit_score_حد_بندی_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    صحیح		حد_بندی)
{
    limitscore_T    ڈھیر[10];		// allow for over 3 * 2 edits
    صحیح		    stackidx;
    صحیح		    bi, gi;
    صحیح		    bi2, gi2;
    صحیح		    bc, gc;
    صحیح		    score;
    صحیح		    score_off;
    صحیح		    minscore;
    صحیح		    باری;
    char_u	    *p;
    صحیح		    wbadword[MAXWLEN];
    صحیح		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    برائے (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    برائے (p = goodword; *p != NUL; )
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
    minscore = حد_بندی + 1;

    برائے (;;)
    {
	// Skip over an equal part, score remains the same.
	برائے (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    اگر (bc != gc)	// stop at a char that's different
		توڑو;
	    اگر (bc == NUL)	// both words end
	    {
		اگر (score < minscore)
		    minscore = score;
		جاؤ نکالو;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	اگر (gc == NUL)    // goodword ends, delete badword chars
	{
	    کرو
	    {
		اگر ((score += SCORE_DEL) >= minscore)
		    جاؤ نکالو;	    // do next alternative
	    } جب (wbadword[++bi] != NUL);
	    minscore = score;
	}
	ورنہ اگر (bc == NUL) // badword ends, insert badword chars
	{
	    کرو
	    {
		اگر ((score += SCORE_INS) >= minscore)
		    جاؤ نکالو;	    // do next alternative
	    } جب (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	ورنہ			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    برائے (باری = 0; باری <= 1; ++باری)
	    {
		score_off = score + (باری == 0 ? SCORE_DEL : SCORE_INS);
		اگر (score_off < minscore)
		{
		    اگر (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - باری;
			gi2 = gi + باری;
			جب (wgoodword[gi2] == wbadword[bi2])
			{
			    اگر (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				توڑو;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    ورنہ
		    {
			// try deleting a character from badword later
			ڈھیر[stackidx].badi = bi + 1 - باری;
			ڈھیر[stackidx].goodi = gi + باری;
			ڈھیر[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    اگر (score + SCORE_تبادلہ < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		اگر (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_تبادلہ;
		    جاری;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    اگر (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    ورنہ
	    {
		// For a similar character use SCORE_SIMILAR.
		اگر (slang != تہی
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		ورنہ
		    score += SCORE_SUBST;
	    }

	    اگر (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		جاری;
	    }
	}
نکالو:
	// Get here to try the next alternative, pop it from the stack.
	اگر (stackidx == 0)		// stack is empty, finished
	    توڑو;

	// pop an item from the stack
	--stackidx;
	gi = ڈھیر[stackidx].goodi;
	bi = ڈھیر[stackidx].badi;
	score = ڈھیر[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    اگر (minscore > حد_بندی)
	واپس SCORE_MAXMAX;
    واپس minscore;
}
#اگر_ختم  // FEAT_SPELL
