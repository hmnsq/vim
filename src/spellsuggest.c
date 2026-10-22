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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SPELL)

/*
 * Use this to adjust the score after finding suggestions, based on the
 * suggested word sounding like the bad word.  This is much faster than doing
 * it for every possible suggestion.
 * Disadvantage: When "the" is typed as "hte" it sounds quite different ("@"
 * vs "ht") and goes down in the list.
 * Used when 'spellsuggest' is set to "best".
 */
#నిర్వచించు RESCORE(మాట_score, sound_score) ((3 * (మాట_score) + (sound_score)) / 4)

/*
 * Do the opposite: based on a maximum end score and a known sound score,
 * compute the maximum word score that can be used.
 */
#నిర్వచించు MAXSCORE(మాట_score, sound_score) ((4 * (మాట_score) - (sound_score)) / 3)

// only used for su_badflags
#నిర్వచించు WF_MIXCAP   0x20	// mix of upper and lower case: macaRONI

/*
 * Information used when looking for suggestions.
 */
రకనిర్వచనం నిర్మాణం suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    పూర్ణాంకం		su_maxcount;	    // max. number of suggestions displayed
    పూర్ణాంకం		su_maxscore;	    // maximum score for adding to su_ga
    పూర్ణాంకం		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    పూర్ణాంకం		su_badlen;	    // length of detected bad word in line
    పూర్ణాంకం		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
రకనిర్వచనం నిర్మాణం suggest_S
{
    char_u	*st_మాట;	// suggested word, allocated string
    పూర్ణాంకం		st_wordlen;	// STRLEN(st_word)
    పూర్ణాంకం		st_orglen;	// length of replaced text
    పూర్ణాంకం		st_score;	// lower is better
    పూర్ణాంకం		st_altscore;	// used when st_score compares equal
    పూర్ణాంకం		st_salscore;	// st_score is for soundalike
    పూర్ణాంకం		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#నిర్వచించు SUG(ga, i) (((suggest_T *)(ga).ga_దత్తాంశం)[i])

// TRUE if a word appears in the list of banned words.
#నిర్వచించు WAS_BANNED(su, మాట) (!HASHITEM_ఖాళీలు(hash_కనుగొను(&(su)->su_banned, మాట)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#నిర్వచించు SUG_CLEAN_లెక్క(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#నిర్వచించు SUG_MAX_లెక్క(su)	(SUG_CLEAN_లెక్క(su) + 50)

// score for various changes
#నిర్వచించు SCORE_విభజించు	149	// split bad word
#నిర్వచించు SCORE_విభజించు_NO	249	// split bad word with NOSPLITSUGS
#నిర్వచించు SCORE_ICASE	52	// slightly different case
#నిర్వచించు SCORE_REGION	200	// word is for different region
#నిర్వచించు SCORE_RARE	180	// rare word
#నిర్వచించు SCORE_మార్పిడి	75	// swap two characters
#నిర్వచించు SCORE_SWAP3	110	// swap two characters in three
#నిర్వచించు SCORE_REP	65	// REP replacement
#నిర్వచించు SCORE_SUBST	93	// substitute a character
#నిర్వచించు SCORE_SIMILAR	33	// substitute a similar character
#నిర్వచించు SCORE_SUBCOMP	33	// substitute a composing character
#నిర్వచించు SCORE_DEL	94	// delete a character
#నిర్వచించు SCORE_DELDUP	66	// delete a duplicated character
#నిర్వచించు SCORE_DELCOMP	28	// delete a composing character
#నిర్వచించు SCORE_INS	96	// insert a character
#నిర్వచించు SCORE_INSDUP	67	// insert a duplicate character
#నిర్వచించు SCORE_INSCOMP	30	// insert a composing character
#నిర్వచించు SCORE_NONWORD	103	// change non-word to word char

#నిర్వచించు SCORE_FILE	30	// suggestion from a file
#నిర్వచించు SCORE_MAXINIT	350	// Initial maximum score: higher == slower.
				// 350 allows for about three changes.

#నిర్వచించు SCORE_COMMON1	30	// subtracted for words seen before
#నిర్వచించు SCORE_COMMON2	40	// subtracted for words often seen
#నిర్వచించు SCORE_COMMON3	50	// subtracted for words very often seen
#నిర్వచించు SCORE_THRES2	10	// word count threshold for COMMON2
#నిర్వచించు SCORE_THRES3	100	// word count threshold for COMMON3

// When trying changed soundfold words it becomes slow when trying more than
// two changes.  With less than two changes it's slightly faster but we miss a
// few good suggestions.  In rare cases we need to try three of four changes.
#నిర్వచించు SCORE_SFMAX1	200	// maximum score for first try
#నిర్వచించు SCORE_SFMAX2	300	// maximum score for second try
#నిర్వచించు SCORE_SFMAX3	400	// maximum score for third try

#నిర్వచించు SCORE_BIG	(SCORE_INS * 3)	// big difference
#నిర్వచించు SCORE_MAXMAX	999999		// accept any score
#నిర్వచించు SCORE_LIMITMAX	350		// for spell_edit_score_limit()

// for spell_edit_score_limit() we need to know the minimum value of
// SCORE_ICASE, SCORE_SWAP, SCORE_DEL, SCORE_SIMILAR and SCORE_INS
#నిర్వచించు SCORE_EDIT_MIN	SCORE_SIMILAR

/*
 * For finding suggestions: At each node in the tree these states are tried:
 */
రకనిర్వచనం గణన
{
    STATE_ప్రారంభం = 0,	// At start of node check for NUL bytes (goodword
			// ends); if badword ends there is a match, otherwise
			// try splitting word.
    STATE_NOPREFIX,	// try without prefix
    STATE_SPLITUNDO,	// Undo splitting.
    STATE_ENDNUL,	// Past NUL bytes at start of the node.
    STATE_PLAIN,	// Use each byte of the node.
    STATE_DEL,		// Delete a byte from the bad word.
    STATE_INS_PREP,	// Prepare for inserting bytes.
    STATE_INS,		// Insert a byte in the bad word.
    STATE_మార్పిడి,		// Swap two bytes.
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
రకనిర్వచనం నిర్మాణం trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    పూర్ణాంకం		ts_score;	// score
    idx_T	ts_arridx;	// index in tree array, start of node
    పొట్టి	ts_curi;	// index in list of child nodes
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
#నిర్వచించు DIFF_NONE	0	// no different byte (yet)
#నిర్వచించు DIFF_YES	1	// different byte found
#నిర్వచించు DIFF_చొప్పించు	2	// inserting character

// values for ts_flags
#నిర్వచించు TSF_PREFIXOK	1	// already checked that prefix is OK
#నిర్వచించు TSF_DIDSPLIT	2	// tried split at this point
#నిర్వచించు TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#నిర్వచించు PFD_NOPREFIX	0xff	// not using prefixes
#నిర్వచించు PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#నిర్వచించు PFD_NOTSPECIAL	0xfd	// highest value that's not special

స్థిర దీర్ఘ spell_suggest_timeout = 5000;

స్థిర శూన్య spell_కనుగొను_suggest(char_u *badptr, పూర్ణాంకం badlen, suginfo_T *su, పూర్ణాంకం maxcount, పూర్ణాంకం banbadword, పూర్ణాంకం need_cap, పూర్ణాంకం interactive);
#నిర్వచితమైతే FEAT_EVAL
స్థిర శూన్య spell_suggest_సమాసం(suginfo_T *su, char_u *సమాసం);
#అయితే_ముగింపు
స్థిర శూన్య spell_suggest_file(suginfo_T *su, char_u *fname);
స్థిర శూన్య spell_suggest_intern(suginfo_T *su, పూర్ణాంకం interactive);
స్థిర శూన్య spell_కనుగొను_cleanup(suginfo_T *su);
స్థిర శూన్య suggest_try_special(suginfo_T *su);
స్థిర శూన్య suggest_try_change(suginfo_T *su);
స్థిర శూన్య suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, పూర్ణాంకం soundfold);
స్థిర శూన్య go_deeper(trystate_T *దొంతర, పూర్ణాంకం లోతు, పూర్ణాంకం score_కూడు);
స్థిర శూన్య కనుగొను_keepcap_మాట(slang_T *slang, char_u *fword, char_u *kword);
స్థిర శూన్య score_comp_sal(suginfo_T *su);
స్థిర శూన్య score_combine(suginfo_T *su);
స్థిర పూర్ణాంకం stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
స్థిర శూన్య suggest_try_soundalike_prep(శూన్య);
స్థిర శూన్య suggest_try_soundalike(suginfo_T *su);
స్థిర శూన్య suggest_try_soundalike_ముగింపు(శూన్య);
స్థిర శూన్య కూడు_sound_suggest(suginfo_T *su, char_u *goodword, పూర్ణాంకం score, langp_T *lp);
స్థిర పూర్ణాంకం soundfold_కనుగొను(slang_T *slang, char_u *మాట);
స్థిర పూర్ణాంకం similar_chars(slang_T *slang, పూర్ణాంకం c1, పూర్ణాంకం c2);
స్థిర శూన్య కూడు_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, పూర్ణాంకం badlen, పూర్ణాంకం score, పూర్ణాంకం altscore, పూర్ణాంకం had_bonus, slang_T *slang, పూర్ణాంకం maxsf);
స్థిర శూన్య check_suggestions(suginfo_T *su, garray_T *gap);
స్థిర శూన్య కూడు_banned(suginfo_T *su, char_u *మాట);
స్థిర శూన్య rescore_suggestions(suginfo_T *su);
స్థిర శూన్య rescore_one(suginfo_T *su, suggest_T *stp);
స్థిర పూర్ణాంకం cleanup_suggestions(garray_T *gap, పూర్ణాంకం maxscore, పూర్ణాంకం keep);
స్థిర పూర్ణాంకం soundalike_score(char_u *goodsound, char_u *badsound);
స్థిర పూర్ణాంకం spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
స్థిర పూర్ణాంకం spell_edit_score_హద్దు(slang_T *slang, char_u *badword, char_u *goodword, పూర్ణాంకం హద్దు);
స్థిర పూర్ణాంకం spell_edit_score_హద్దు_w(slang_T *slang, char_u *badword, char_u *goodword, పూర్ణాంకం హద్దు);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    స్థిర పూర్ణాంకం
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    పూర్ణాంకం		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    ఒకవేళ (!byte_లోపల_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	తిరిగి FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    ఒకవేళ (slang->sl_comprules != ఖాళీ && sp->ts_complen > sp->ts_compsplit)
    {
	పూర్ణాంకం v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = సరిపోలిక_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	తిరిగి v;
    }

    తిరిగి TRUE;
}

/*
 * Adjust the score of common words.
 */
    స్థిర పూర్ణాంకం
score_wordcount_adj(
    slang_T	*slang,
    పూర్ణాంకం		score,
    char_u	*మాట,
    పూర్ణాంకం		విభజించు)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    పూర్ణాంకం		bonus;
    పూర్ణాంకం		newscore;

    hi = hash_కనుగొను(&slang->sl_wordcount, మాట);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	తిరిగి score;

    wc = HI2WC(hi);
    ఒకవేళ (wc->wc_లెక్క < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    లేకపోతే ఒకవేళ (wc->wc_లెక్క < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    లేకపోతే
	bonus = SCORE_COMMON3;
    ఒకవేళ (విభజించు)
	newscore = score - bonus / 2;
    లేకపోతే
	newscore = score - bonus;
    ఒకవేళ (newscore < 0)
	తిరిగి 0;
    తిరిగి newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    స్థిర పూర్ణాంకం
badword_captype(char_u *మాట, char_u *చివర)
{
    పూర్ణాంకం		flags = captype(మాట, చివర);
    పూర్ణాంకం		c;
    పూర్ణాంకం		l, u;
    పూర్ణాంకం		మొదటి;
    char_u	*p;

    ఒకవేళ (!(flags & WF_KEEPCAP))
	తిరిగి flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    మొదటి = FALSE;
    కొరకు (p = మాట; p < చివర; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	ఒకవేళ (SPELL_ISUPPER(c))
	{
	    ++u;
	    ఒకవేళ (p == మాట)
		మొదటి = TRUE;
	}
	లేకపోతే
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    ఒకవేళ (u > l && u > 2)
	flags |= WF_ALLCAP;
    లేకపోతే ఒకవేళ (మొదటి)
	flags |= WF_ONECAP;

    ఒకవేళ (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    తిరిగి flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    స్థిర పూర్ణాంకం
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    పూర్ణాంకం		nr;
    పూర్ణాంకం		c;

    c = *p++;
    ఒకవేళ ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    లేకపోతే ఒకవేళ ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    లేకపోతే ఒకవేళ ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    లేకపోతే				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    తిరిగి nr;
}

// values for sps_flags
#నిర్వచించు SPS_ఉత్తమ    1
#నిర్వచించు SPS_FAST    2
#నిర్వచించు SPS_DOUBLE  4

స్థిర పూర్ణాంకం sps_flags = SPS_ఉత్తమ;	// flags from 'spellsuggest'
స్థిర పూర్ణాంకం sps_హద్దు = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    పూర్ణాంకం
spell_check_sps(శూన్య)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    పూర్ణాంకం		f;

    sps_flags = 0;
    sps_హద్దు = 9999;

    కొరకు (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	ఒకవేళ (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_హద్దు = getdigits(&s);
	    ఒకవేళ (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	లేకపోతే ఒకవేళ (STRCMP(buf, "best") == 0)
	    f = SPS_ఉత్తమ;
	లేకపోతే ఒకవేళ (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	లేకపోతే ఒకవేళ (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	లేకపోతే ఒకవేళ (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	ఒకవేళ (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_ఉత్తమ;
	    sps_హద్దు = 9999;
	    తిరిగి FAIL;
	}
	ఒకవేళ (f != 0)
	    sps_flags = f;
    }

    ఒకవేళ (sps_flags == 0)
	sps_flags = SPS_ఉత్తమ;

    తిరిగి OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    శూన్య
spell_suggest(పూర్ణాంకం లెక్క)
{
    char_u	*పంక్తి = ఖాళీ;
    pos_T	prev_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;
    suginfo_T	sug;
    suggest_T	*stp;
    పూర్ణాంకం		mouse_వాడిన;
    పూర్ణాంకం		need_cap;
    పూర్ణాంకం		హద్దు;
    పూర్ణాంకం		selected = లెక్క;
    పూర్ణాంకం		badlen = 0;
    పూర్ణాంకం		సందేశం_scroll_save = సందేశం_scroll;
    పూర్ణాంకం		wo_spell_save = curwin->w_p_spell;

    ఒకవేళ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    prev_cursor = curwin->w_cursor;

    ఒకవేళ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	వెళ్ళు skip;
    }

    ఒకవేళ (VIsual_చురుకైన)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	ఒకవేళ (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    వెళ్ళు skip;
	}
	badlen = (పూర్ణాంకం)curwin->w_cursor.నిలువు - (పూర్ణాంకం)VIsual.నిలువు;
	ఒకవేళ (badlen < 0)
	    badlen = -badlen;
	లేకపోతే
	    curwin->w_cursor.నిలువు = VIsual.నిలువు;
	++badlen;
	చివర_visual_బహుళకం();
	// make sure we don't include the NUL at the end of the line
	ఒకవేళ (badlen > ml_get_curline_len() - (పూర్ణాంకం)curwin->w_cursor.నిలువు)
	    badlen = ml_get_curline_len() - (పూర్ణాంకం)curwin->w_cursor.నిలువు;
    }
    // Find the start of the badly spelled word.
    లేకపోతే ఒకవేళ (spell_move_గమ్యం(curwin, FORWARD, SMT_ALL, TRUE, ఖాళీ) == 0
	    || curwin->w_cursor.నిలువు > prev_cursor.నిలువు)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = prev_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.నిలువు;
	// Backup to before start of word.
	అయితే (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_వెనుక(curline, p);
	// Forward to start of word.
	అయితే (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	ఒకవేళ (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    వెళ్ళు skip;
	}
	curwin->w_cursor.నిలువు = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.నిలువు);

    // Make a copy of current line since autocommands may free the line.
    పంక్తి = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ఒకవేళ (పంక్తి == ఖాళీ)
	వెళ్ళు skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    ఒకవేళ (sps_హద్దు > (పూర్ణాంకం)అడ్డువరుసలు - 2)
	హద్దు = (పూర్ణాంకం)అడ్డువరుసలు - 2;
    లేకపోతే
	హద్దు = sps_హద్దు;
    spell_కనుగొను_suggest(పంక్తి + curwin->w_cursor.నిలువు, badlen, &sug, హద్దు,
							TRUE, need_cap, TRUE);

    ఒకవేళ (sug.su_ga.ga_len == 0)
	సందేశం(_("Sorry, no suggestions"));
    లేకపోతే ఒకవేళ (లెక్క > 0)
    {
	ఒకవేళ (లెక్క > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (దీర్ఘ)sug.su_ga.ga_len);
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	ఒకవేళ (cmdmsg_rl)
	    సందేశం_నిలువు = cmdline_వెడల్పు - 1;
#అయితే_ముగింపు

	// List the suggestions.
	సందేశం_ప్రారంభం();
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;	// for when 'cmdheight' > 1
	పంక్తులు_ఎడమ = అడ్డువరుసలు;	// avoid more prompt
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#అయితే_ముగింపు
	సందేశం_puts((అక్షరం *)IObuff);
	సందేశం_clr_eos();
	సందేశం_putchar('\n');

	సందేశం_scroll = TRUE;
	కొరకు (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    పూర్ణాంకం el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_మాట, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    ఒకవేళ (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%2d", i + 1);
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (cmdmsg_rl)
		rl_mirror(IObuff);
#అయితే_ముగింపు
	    సందేశం_puts((అక్షరం *)IObuff);

	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    సందేశం_puts((అక్షరం *)IObuff);

	    // The word may replace more than "su_badlen".
	    ఒకవేళ (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((అక్షరం *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		సందేశం_puts((అక్షరం *)IObuff);
	    }

	    ఒకవేళ (p_verbose > 0)
	    {
		// Add the score.
		ఒకవేళ (sps_flags & (SPS_DOUBLE | SPS_ఉత్తమ))
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		లేకపోతే
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#నిర్వచితమైతే FEAT_RIGHTLEFT
		ఒకవేళ (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#అయితే_ముగింపు
		సందేశం_advance(30);
		సందేశం_puts((అక్షరం *)IObuff);
	    }
	    సందేశం_putchar('\n');
	}

#నిర్వచితమైతే FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	సందేశం_నిలువు = 0;
#అయితే_ముగింపు
	// Ask for choice.
	selected = prompt_for_సంఖ్య(&mouse_వాడిన);
	ఒకవేళ (mouse_వాడిన)
	    selected -= పంక్తులు_ఎడమ;
	పంక్తులు_ఎడమ = అడ్డువరుసలు;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	సందేశం_scroll = సందేశం_scroll_save;
    }

    ఒకవేళ (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_గమ్యం);

	stp = &SUG(sug.su_ga, selected - 1);
	ఒకవేళ (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%s%.*s", stp->st_మాట,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_గమ్యం = vim_strsave(IObuff);
	}
	లేకపోతే
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_గమ్యం = vim_strsave(stp->st_మాట);
	}

	// Replace the word.
	p = alloc(STRLEN(పంక్తి) - stp->st_orglen + stp->st_wordlen + 1);
	ఒకవేళ (p != ఖాళీ)
	{
	    పూర్ణాంకం len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (పూర్ణాంకం)(sug.su_badptr - పంక్తి);
	    mch_memmove(p, పంక్తి, c);
	    STRCPY(p + c, stp->st_మాట);
	    STRCAT(p, sug.su_badptr + stp->st_orglen);

	    // For redo we use a change-word command.
	    ResetRedobuff();
	    Append_గమ్యం_Redobuff((char_u *)"ciw");
	    Append_గమ్యం_Redobuff_Lit(p + c,
			    stp->st_wordlen + sug.su_badlen - stp->st_orglen);
	    Append_Char_గమ్యం_Redobuff(ESC);

	    // "p" may be freed here
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    curwin->w_cursor.నిలువు = c;

	    మారింది_బైట్లు(curwin->w_cursor.lnum, c);
#ఒకవేళ defined(FEAT_PROP_POPUP)
	    ఒకవేళ (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#అయితే_ముగింపు
	}
    }
    లేకపోతే
	curwin->w_cursor = prev_cursor;

    spell_కనుగొను_cleanup(&sug);
skip:
    vim_free(పంక్తి);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    శూన్య
spell_suggest_list(
    garray_T	*gap,
    char_u	*మాట,
    పూర్ణాంకం		maxcount,	// maximum nr of suggestions
    పూర్ణాంకం		need_cap,	// 'spellcapcheck' matched
    పూర్ణాంకం		interactive)
{
    suginfo_T	sug;
    పూర్ణాంకం		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_కనుగొను_suggest(మాట, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, పరిమాణం(char_u *), sug.su_ga.ga_len + 1);
    ఒకవేళ (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	కొరకు (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (గుర్తులేని)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    ఒకవేళ (wcopy == ఖాళీ)
		ఆపు;
	    STRCPY(wcopy, stp->st_మాట);
	    STRCPY(wcopy + stp->st_wordlen, sug.su_badptr + stp->st_orglen);
	    ((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = wcopy;
	}
    }

    spell_కనుగొను_cleanup(&sug);
}

/*
 * Find spell suggestions for the word at the start of "badptr".
 * Return the suggestions in "su->su_ga".
 * The maximum number of suggestions is "maxcount".
 * Note: does use info for the current window.
 * This is based on the mechanisms of Aspell, but completely reimplemented.
 */
    స్థిర శూన్య
spell_కనుగొను_suggest(
    char_u	*badptr,
    పూర్ణాంకం		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    పూర్ణాంకం		maxcount,
    పూర్ణాంకం		banbadword,	// don't include badword in suggestions
    పూర్ణాంకం		need_cap,	// word should start with capital
    పూర్ణాంకం		interactive)
{
    hlf_T	attr = HLF_లెక్క;
    char_u	buf[MAXPATHL];
    char_u	*p;
    పూర్ణాంకం		do_combine = FALSE;
    char_u	*sps_copy;
#నిర్వచితమైతే FEAT_EVAL
    స్థిర పూర్ణాంకం	సమాసం_busy = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;
    langp_T	*lp;
    పూర్ణాంకం		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_సూచిక(su);
    ga_init2(&su->su_ga, పరిమాణం(suggest_T), 10);
    ga_init2(&su->su_sga, పరిమాణం(suggest_T), 10);
    ఒకవేళ (*badptr == NUL)
	తిరిగి;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    ఒకవేళ (badlen != 0)
	su->su_badlen = badlen;
    లేకపోతే
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, ఖాళీ, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    ఒకవేళ (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (శూన్య)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    ఒకవేళ (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    కొరకు (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_నమోదు(curbuf->b_s.b_langp, i);
	ఒకవేళ (lp->lp_sallang != ఖాళీ)
	{
	    su->su_sallang = lp->lp_sallang;
	    ఆపు;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    ఒకవేళ (su->su_sallang != ఖాళీ)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    ఒకవేళ (!SPELL_ISUPPER(c) && attr == HLF_లెక్క)
    {
	make_case_మాట(su->su_badword, buf, WF_ONECAP);
	కూడు_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    ఒకవేళ (banbadword)
	కూడు_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    ఒకవేళ (sps_copy == ఖాళీ)
	తిరిగి;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    కొరకు (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	ఒకవేళ (STRNCMP(buf, "expr:", 5) == 0)
	{
#నిర్వచితమైతే FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    ఒకవేళ (!సమాసం_busy)
	    {
		సమాసం_busy = TRUE;
		spell_suggest_సమాసం(su, buf + 5);
		సమాసం_busy = FALSE;
	    }
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	లేకపోతే ఒకవేళ (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((అక్షరం *)buf + 8);
	లేకపోతే ఒకవేళ (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    ఒకవేళ (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    ఒకవేళ (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    స్థిర శూన్య
spell_suggest_సమాసం(suginfo_T *su, char_u *సమాసం)
{
    list_T	*list;
    listitem_T	*li;
    పూర్ణాంకం		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_సమాసం(su->su_badword, సమాసం);
    ఒకవేళ (list != ఖాళీ)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_వస్తువులు(list, li)
	    ఒకవేళ (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		ఒకవేళ (score >= 0 && score <= su->su_maxscore)
		    కూడు_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (శూన్య)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#అయితే_ముగింపు

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    స్థిర శూన్య
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	పంక్తి[MAXWLEN * 2];
    char_u	*p;
    పూర్ణాంకం		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((అక్షరం *)fname, "r");
    ఒకవేళ (fd == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), fname);
	తిరిగి;
    }

    // Read it line by line.
    అయితే (!vim_fgets(పంక్తి, MAXWLEN * 2, fd) && !got_int)
    {
	పంక్తి_breakcheck();

	p = vim_strchr(పంక్తి, '/');
	ఒకవేళ (p == ఖాళీ)
	    కొనసాగు;	    // No Tab found, just skip the line.
	*p++ = NUL;
	ఒకవేళ (STRICMP(su->su_badword, పంక్తి) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    కొరకు (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    ఒకవేళ (captype(p, ఖాళీ) == 0)
	    {
		make_case_మాట(p, cword, su->su_badflags);
		p = cword;
	    }

	    కూడు_suggestion(su, &su->su_ga, p, su->su_badlen,
				  SCORE_FILE, 0, TRUE, su->su_sallang, FALSE);
	}
    }

    fclose(fd);

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (శూన్య)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    స్థిర శూన్య
spell_suggest_intern(suginfo_T *su, పూర్ణాంకం interactive)
{
    // Load the .sug file(s) that are available and not done yet.
    suggest_ఎక్కించు_files();

    // 1. Try special cases, such as repeating a word: "the the" -> "the".
    //
    // Set a maximum score to limit the combination of operations that is
    // tried.
    suggest_try_special(su);

    // 2. Try inserting/deleting/swapping/changing a letter, use REP entries
    //    from the .aff file and inserting a space (split the word).
    suggest_try_change(su);

    // For the resulting top-scorers compute the sound-a-like score.
    ఒకవేళ (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    ఒకవేళ ((sps_flags & SPS_FAST) == 0)
    {
	ఒకవేళ (sps_flags & SPS_ఉత్తమ)
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
	ఒకవేళ (su->su_ga.ga_len < SUG_CLEAN_లెక్క(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    ఒకవేళ (su->su_ga.ga_len < SUG_CLEAN_లెక్క(su))
	    {
		// Still didn't find enough matches, try again, allowing even
		// more changes to the soundfold word.
		su->su_maxscore = SCORE_SFMAX3;
		suggest_try_soundalike(su);
	    }
	}
	su->su_maxscore = su->su_sfmaxscore;
	suggest_try_soundalike_ముగింపు();
    }

    // When CTRL-C was hit while searching do show the results.  Only clear
    // got_int when using a command, not for spellsuggest().
    ui_breakcheck();
    ఒకవేళ (interactive && got_int)
    {
	(శూన్య)vgetc();
	got_int = FALSE;
    }

    ఒకవేళ ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	ఒకవేళ (sps_flags & SPS_ఉత్తమ)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(శూన్య)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    స్థిర శూన్య
spell_కనుగొను_cleanup(suginfo_T *su)
{
    పూర్ణాంకం		i;

    // Free the suggestions.
    కొరకు (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_మాట);
    ga_clear(&su->su_ga);
    కొరకు (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_మాట);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    స్థిర శూన్య
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    పూర్ణాంకం		c;
    char_u	మాట[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    ఒకవేళ (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
    {
	// Include badflags: if the badword is onecap or allcap
	// use that for the goodword too: "The the" -> "The".
	c = su->su_fbadword[len];
	su->su_fbadword[len] = NUL;
	make_case_మాట(su->su_fbadword, మాట, su->su_badflags);
	su->su_fbadword[len] = c;

	// Give a soundalike score of 0, compute the score as if deleting one
	// character.
	కూడు_suggestion(su, &su->su_ga, మాట, su->su_badlen,
		       RESCORE(SCORE_REP, 0), 0, TRUE, su->su_sallang, FALSE);
    }
}

/*
 * Change the 0 to 1 to measure how much time is spent in each state.
 * Output is dumped in "suggestprof".
 */
#ఒకవేళ 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T మొత్తం;
proftime_T times[STATE_FINAL + 1];
దీర్ఘ counts[STATE_FINAL + 1];

    స్థిర శూన్య
prof_init(శూన్య)
{
    కొరకు (పూర్ణాంకం i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_ప్రారంభం(&current);
    profile_ప్రారంభం(&మొత్తం);
}

// call before changing state
    స్థిర శూన్య
prof_store(state_T state)
{
    profile_చివర(&current);
    profile_కూడు(&times[state], &current);
    ++counts[state];
    profile_ప్రారంభం(&current);
}
# define PROF_STORE(state) prof_store(state);

    స్థిర శూన్య
prof_report(అక్షరం *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_చివర(&మొత్తం);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_సందేశం(&మొత్తం));
    కొరకు (పూర్ణాంకం i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_సందేశం(&times[i]), counts[i]);
    fclose(fd);
}
#లేకపోతే
# define PROF_STORE(state)
#అయితే_ముగింపు

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    స్థిర శూన్య
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    పూర్ణాంకం		n;
    char_u	*p;
    పూర్ణాంకం		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (పూర్ణాంకం)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (శూన్య)spell_casefold(curwin, p, (పూర్ణాంకం)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (పూర్ణాంకం)STRLEN(su->su_badptr);
    ఒకవేళ (n < MAXWLEN)
	fword[n] = NUL;

    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	ఒకవేళ (lp->lp_slang->sl_fbyts == ఖాళీ)
	    కొనసాగు;

	// Try it for this language.  Will add possible suggestions.
#నిర్వచితమైతే SUGGEST_PROFILE
	prof_init();
#అయితే_ముగింపు
	suggest_trie_walk(su, lp, fword, FALSE);
#నిర్వచితమైతే SUGGEST_PROFILE
	prof_report("try_change");
#అయితే_ముగింపు
    }
}

// Check the maximum score, if we go over it we won't try this change.
#నిర్వచించు TRY_DEEPER(su, దొంతర, లోతు, కూడు) \
       ((లోతు) < MAXWLEN - 1 && (దొంతర)[లోతు].ts_score + (కూడు) < (su)->su_maxscore)

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
    స్థిర శూన్య
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    పూర్ణాంకం		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	దొంతర[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    పూర్ణాంకం		newscore;
    పూర్ణాంకం		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    పూర్ణాంకం		లోతు;
    పూర్ణాంకం		c, c2, c3;
    పూర్ణాంకం		n = 0;
    పూర్ణాంకం		flags;
    garray_T	*gap;
    idx_T	arridx;
    పూర్ణాంకం		len;
    char_u	*p;
    fromto_T	*ftp;
    పూర్ణాంకం		fl = 0, tl;
    పూర్ణాంకం		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    పూర్ణాంకం		fword_చివర్లు;
    పూర్ణాంకం		goodword_చివర్లు;
#నిర్వచితమైతే DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#అయితే_ముగింపు
    పూర్ణాంకం		breakcheckcount = 1000;
#నిర్వచితమైతే FEAT_RELTIME
    proftime_T	సమయం_హద్దు;
#అయితే_ముగింపు
    పూర్ణాంకం		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    లోతు = 0;
    sp = &దొంతర[0];
    CLEAR_సూచిక(sp);
    sp->ts_curi = 1;

    ఒకవేళ (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = ఖాళీ;
	pidxs = ఖాళీ;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_ప్రారంభం;
    }
    లేకపోతే
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	ఒకవేళ (pbyts != ఖాళీ)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	లేకపోతే
	{
	    byts = fbyts;
	    idxs = fidxs;
	    sp->ts_prefixdepth = PFD_NOPREFIX;
	    sp->ts_state = STATE_ప్రారంభం;
	}
    }
#నిర్వచితమైతే FEAT_RELTIME
    // The loop may take an indefinite amount of time. Break out after some
    // time.
    ఒకవేళ (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &సమయం_హద్దు);
#అయితే_ముగింపు

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    అయితే (లోతు >= 0 && !got_int)
    {
	sp = &దొంతర[లోతు];
	ఎంపిక (sp->ts_state)
	{
	స్థితి STATE_ప్రారంభం:
	స్థితి STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    ఒకవేళ (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		కొరకు (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (పూర్ణాంకం)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		ఒకవేళ (లోతు < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (పూర్ణాంకం)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    ఒకవేళ (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    లేకపోతే
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#నిర్వచితమైతే DEBUG_TRIEWALK
		    sprintf(changename[లోతు], "prefix");
#అయితే_ముగింపు
		    go_deeper(దొంతర, లోతు, 0);
		    ++లోతు;
		    sp = &దొంతర[లోతు];
		    sp->ts_prefixdepth = లోతు - 1;
		    byts = fbyts;
		    idxs = fidxs;
		    sp->ts_arridx = 0;

		    // Move the prefix to preword[] with the right case
		    // and make find_keepcap_word() works.
		    tword[sp->ts_twordlen] = NUL;
		    make_case_మాట(tword + sp->ts_splitoff,
					  preword + sp->ts_prewordlen, flags);
		    sp->ts_prewordlen = (char_u)STRLEN(preword);
		    sp->ts_splitoff = sp->ts_twordlen;
		}
		ఆపు;
	    }

	    ఒకవేళ (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		ఆపు;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (పూర్ణాంకం)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    ఒకవేళ (flags & WF_NOSUGGEST)
		ఆపు;

	    fword_చివర్లు = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    ఒకవేళ (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != ఖాళీ)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = దొంతర[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		కొరకు (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		ఒకవేళ (c > 0)
		{
		    c = valid_మాట_ఉపసర్గ(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    ఒకవేళ (c == 0)
			ఆపు;

		    // Use the WF_RARE flag for a rare prefix.
		    ఒకవేళ (c & WF_RAREPFX)
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
	    ఒకవేళ (sp->ts_complen == sp->ts_compsplit && fword_చివర్లు
						     && (flags & WF_NEEDCOMP))
		goodword_చివర్లు = FALSE;
	    లేకపోతే
		goodword_చివర్లు = TRUE;

	    p = ఖాళీ;
	    compound_ok = TRUE;
	    ఒకవేళ (sp->ts_complen > sp->ts_compsplit)
	    {
		ఒకవేళ (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    ఒకవేళ (sp->ts_fidx - sp->ts_splitfidx
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
			ఒకవేళ (newscore <= su->su_maxscore)
			    కూడు_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			ఆపు;
		    }
		}
		లేకపోతే
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    ఒకవేళ (((గుర్తులేని)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			ఆపు;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    ఒకవేళ (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			ఆపు;

		    compflags[sp->ts_complen] = ((గుర్తులేని)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    ఒకవేళ (సరిపోలిక_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    ఒకవేళ (compound_ok)
		    {
			p = preword;
			అయితే (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			ఒకవేళ (fword_చివర్లు && !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    // Compound is not allowed.  But it may still be
			    // possible if we add another (short) word.
			    compound_ok = FALSE;
		    }

		    // Get pointer to last char of previous word.
		    p = preword + sp->ts_prewordlen;
		    MB_PTR_వెనుక(preword, p);
		}
	    }

	    // Form the word with proper case in preword.
	    // If there is a word from a previous split, append.
	    // For the soundfold tree don't change the case, simply append.
	    ఒకవేళ (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    లేకపోతే ఒకవేళ (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		కనుగొను_keepcap_మాట(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    లేకపోతే
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		ఒకవేళ ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		ఒకవేళ (p != ఖాళీ && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_మాట(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    ఒకవేళ (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		ఒకవేళ (flags & WF_BANNED)
		{
		    కూడు_banned(su, preword + sp->ts_prewordlen);
		    ఆపు;
		}
		ఒకవేళ ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    ఒకవేళ (slang->sl_compprog == ఖాళీ)
			ఆపు;
		    // the word so far was banned but we may try compounding
		    goodword_చివర్లు = FALSE;
		}
	    }

	    newscore = 0;
	    ఒకవేళ (!soundfold)	// soundfold words don't have flags
	    {
		ఒకవేళ ((flags & WF_REGION)
			    && (((గుర్తులేని)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		ఒకవేళ (flags & WF_RARE)
		    newscore += SCORE_RARE;

		ఒకవేళ (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, ఖాళీ)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    ఒకవేళ (fword_చివర్లు
		    && goodword_చివర్లు
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#నిర్వచితమైతే DEBUG_TRIEWALK
		ఒకవేళ (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    పూర్ణాంకం	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    కొరకు (j = 0; j < లోతు; ++j)
			smsg("%s", changename[j]);
		}
#అయితే_ముగింపు
		ఒకవేళ (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    కూడు_sound_suggest(su, preword, sp->ts_score, lp);
		}
		లేకపోతే ఒకవేళ (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_వెనుక(fword, p);
		    ఒకవేళ (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_వెనుక(preword, p);
			ఒకవేళ (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    ఒకవేళ (score <= su->su_maxscore)
		    {
			కూడు_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			ఒకవేళ (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, ఖాళీ);
			    ఒకవేళ (c == 0 || c == WF_ALLCAP)
			    {
				make_case_మాట(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen,
						      c == 0 ? WF_ALLCAP : 0);

				కూడు_suggestion(su, &su->su_ga, preword,
					sp->ts_fidx - repextra,
					score + SCORE_ICASE, 0, FALSE,
					lp->lp_sallang, FALSE);
			    }
			}
		    }
		}
	    }

	    // Try word split and/or compounding.
	    ఒకవేళ ((sp->ts_fidx >= sp->ts_fidxtry || fword_చివర్లు)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		పూర్ణాంకం	try_compound;
		పూర్ణాంకం	try_విభజించు;

		// If past the end of the bad word don't try a split.
		// Otherwise try changing the next word.  E.g., find
		// suggestions for "the the" where the second "the" is
		// different.  It's done like a split.
		// TODO: word split for soundfold words
		try_విభజించు = (sp->ts_fidx - repextra < su->su_badlen)
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
		ఒకవేళ (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != ఖాళీ
			&& ((గుర్తులేని)flags >> 24) != 0
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
					 compflags, ((గుర్తులేని)flags >> 24))))

		{
		    try_compound = TRUE;
		    compflags[sp->ts_complen] = ((గుర్తులేని)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		}

		// For NOBREAK we never try splitting, it won't make any word
		// valid.
		ఒకవేళ (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		లేకపోతే ఒకవేళ (!fword_చివర్లు
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		లేకపోతే
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		ఒకవేళ (try_విభజించు || try_compound)
		{
		    ఒకవేళ (!try_compound && (!fword_చివర్లు || !goodword_చివర్లు))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			ఒకవేళ (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    ఆపు;
			p = preword;
			అయితే (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			ఒకవేళ (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    ఆపు;

			ఒకవేళ (slang->sl_nosplitsugs)
			    newscore += SCORE_విభజించు_NO;
			లేకపోతే
			    newscore += SCORE_విభజించు;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    ఒకవేళ (TRY_DEEPER(su, దొంతర, లోతు, newscore))
		    {
			go_deeper(దొంతర, లోతు, newscore);
#నిర్వచితమైతే DEBUG_TRIEWALK
			ఒకవేళ (!try_compound && !fword_చివర్లు)
			    sprintf(changename[లోతు], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			లేకపోతే
			    sprintf(changename[లోతు], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#అయితే_ముగింపు
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++లోతు;
			sp = &దొంతర[లోతు];

			// Append a space to preword when splitting.
			ఒకవేళ (!try_compound && !fword_చివర్లు)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			ఒకవేళ (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_చివర్లు)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_చివర్లు)
			{
			    పూర్ణాంకం	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    ఒకవేళ (fword_చివర్లు)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    లేకపోతే
				sp->ts_score -= SCORE_విభజించు - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			ఒకవేళ (try_compound)
			    ++sp->ts_complen;
			లేకపోతే
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			ఒకవేళ (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			లేకపోతే
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			ఒకవేళ (pbyts != ఖాళీ)
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
	    ఆపు;

	స్థితి STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_ప్రారంభం;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    ఆపు;

	స్థితి STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    ఒకవేళ (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		ఆపు;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	స్థితి STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    ఒకవేళ (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		ఒకవేళ (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		లేకపోతే
		    sp->ts_state = STATE_FINAL;
	    }
	    లేకపోతే
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		ఒకవేళ (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		లేకపోతే
		    newscore = SCORE_SUBST;
		ఒకవేళ ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, దొంతర, లోతు, newscore))
		{
		    go_deeper(దొంతర, లోతు, newscore);
#నిర్వచితమైతే DEBUG_TRIEWALK
		    ఒకవేళ (newscore > 0)
			sprintf(changename[లోతు], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    లేకపోతే
			sprintf(changename[లోతు], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#అయితే_ముగింపు
		    ++లోతు;
		    sp = &దొంతర[లోతు];
		    ఒకవేళ (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    ఒకవేళ (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    ఒకవేళ (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			ఒకవేళ (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			లేకపోతే ఒకవేళ (sp->ts_isdiff == DIFF_చొప్పించు
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			ఒకవేళ (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    ఒకవేళ (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				ఒకవేళ (enc_utf8
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
				లేకపోతే ఒకవేళ (!soundfold
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
			    లేకపోతే ఒకవేళ (sp->ts_isdiff == DIFF_చొప్పించు
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				ఒకవేళ (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				లేకపోతే
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_వెనుక(tword, p);
				    ఒకవేళ (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    లేకపోతే
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			ఒకవేళ (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    ఆపు;

	స్థితి STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    ఒకవేళ (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ఆపు;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    ఒకవేళ (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    లేకపోతే
		newscore = SCORE_DEL;
	    ఒకవేళ (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, దొంతర, లోతు, newscore))
	    {
		go_deeper(దొంతర, లోతు, newscore);
#నిర్వచితమైతే DEBUG_TRIEWALK
		sprintf(changename[లోతు], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#అయితే_ముగింపు
		++లోతు;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		దొంతర[లోతు].ts_flags |= TSF_DIDDEL;
		దొంతర[లోతు].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		ఒకవేళ (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    దొంతర[లోతు].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    ఒకవేళ (enc_utf8 && utf_iscomposing(c))
			దొంతర[లోతు].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    లేకపోతే ఒకవేళ (c == mb_ptr2char(fword + దొంతర[లోతు].ts_fidx))
			దొంతర[లోతు].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		లేకపోతే
		{
		    ++దొంతర[లోతు].ts_fidx;
		    ఒకవేళ (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			దొంతర[లోతు].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ఆపు;
	    }
	    // FALLTHROUGH

	స్థితి STATE_INS_PREP:
	    ఒకవేళ (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_మార్పిడి;
		ఆపు;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    కొరకు (;;)
	    {
		ఒకవేళ (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_మార్పిడి;
		    ఆపు;
		}
		ఒకవేళ (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    ఆపు;
		}
		++sp->ts_curi;
	    }
	    ఆపు;

	    // FALLTHROUGH

	స్థితి STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    ఒకవేళ (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_మార్పిడి;
		ఆపు;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    ఒకవేళ (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		ఆపు;
	    }
	    c = byts[n];
	    ఒకవేళ (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    లేకపోతే
		newscore = SCORE_INS;
	    ఒకవేళ (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, దొంతర, లోతు, newscore))
	    {
		go_deeper(దొంతర, లోతు, newscore);
#నిర్వచితమైతే DEBUG_TRIEWALK
		sprintf(changename[లోతు], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#అయితే_ముగింపు
		++లోతు;
		sp = &దొంతర[లోతు];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		ఒకవేళ (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    ఒకవేళ (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_చొప్పించు;
		    }
		}
		లేకపోతే
		    fl = 1;
		ఒకవేళ (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    ఒకవేళ (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    ఆపు;

	స్థితి STATE_మార్పిడి:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    ఒకవేళ (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ఆపు;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    ఒకవేళ (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ఆపు;
	    }

	    ఒకవేళ (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		ఒకవేళ (p[n] == NUL)
		    c2 = NUL;
		లేకపోతే ఒకవేళ (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		లేకపోతే
		    c2 = mb_ptr2char(p + n);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (p[1] == NUL)
		    c2 = NUL;
		లేకపోతే ఒకవేళ (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		లేకపోతే
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    ఒకవేళ (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ఆపు;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    ఒకవేళ (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		ఆపు;
	    }
	    ఒకవేళ (c2 != NUL && TRY_DEEPER(su, దొంతర, లోతు, SCORE_మార్పిడి))
	    {
		go_deeper(దొంతర, లోతు, SCORE_మార్పిడి);
#నిర్వచితమైతే DEBUG_TRIEWALK
		sprintf(changename[లోతు], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#అయితే_ముగింపు
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++లోతు;
		ఒకవేళ (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		లేకపోతే
		{
		    p[0] = c2;
		    p[1] = c;
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    లేకపోతే
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ఆపు;

	స్థితి STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    ఒకవేళ (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    లేకపోతే
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	స్థితి STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    ఒకవేళ (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		ఒకవేళ (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		లేకపోతే
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    లేకపోతే
	    {
		c = *p;
		c2 = p[1];
		ఒకవేళ (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		లేకపోతే
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    ఒకవేళ (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ఆపు;
	    }
	    ఒకవేళ (TRY_DEEPER(su, దొంతర, లోతు, SCORE_SWAP3))
	    {
		go_deeper(దొంతర, లోతు, SCORE_SWAP3);
#నిర్వచితమైతే DEBUG_TRIEWALK
		sprintf(changename[లోతు], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#అయితే_ముగింపు
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++లోతు;
		ఒకవేళ (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		లేకపోతే
		{
		    p[0] = p[2];
		    p[2] = c;
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    లేకపోతే
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ఆపు;

	స్థితి STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    ఒకవేళ (has_mbyte)
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
	    లేకపోతే
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    ఒకవేళ (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ఆపు;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    ఒకవేళ (TRY_DEEPER(su, దొంతర, లోతు, SCORE_SWAP3))
	    {
		go_deeper(దొంతర, లోతు, SCORE_SWAP3);
#నిర్వచితమైతే DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[లోతు], "%.*s-%s: rotate left %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#అయితే_ముగింపు
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3L;
		++లోతు;
		p = fword + sp->ts_fidx;
		ఒకవేళ (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		లేకపోతే
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    లేకపోతే
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ఆపు;

	స్థితి STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    ఒకవేళ (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    లేకపోతే
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    ఒకవేళ (TRY_DEEPER(su, దొంతర, లోతు, SCORE_SWAP3))
	    {
		go_deeper(దొంతర, లోతు, SCORE_SWAP3);
#నిర్వచితమైతే DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[లోతు], "%.*s-%s: rotate right %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#అయితే_ముగింపు
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3R;
		++లోతు;
		p = fword + sp->ts_fidx;
		ఒకవేళ (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		లేకపోతే
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    లేకపోతే
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ఆపు;

	స్థితి STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    ఒకవేళ (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    లేకపోతే
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	స్థితి STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    ఒకవేళ ((lp->lp_replang == ఖాళీ && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ఆపు;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    ఒకవేళ (soundfold)
		sp->ts_curi = slang->sl_repsal_మొదటి[fword[sp->ts_fidx]];
	    లేకపోతే
		sp->ts_curi = lp->lp_replang->sl_rep_మొదటి[fword[sp->ts_fidx]];

	    ఒకవేళ (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ఆపు;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	స్థితి STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    ఒకవేళ (soundfold)
		gap = &slang->sl_repsal;
	    లేకపోతే
		gap = &lp->lp_replang->sl_rep;
	    అయితే (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_దత్తాంశం + sp->ts_curi++;
		ఒకవేళ (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    ఆపు;
		}
		ఒకవేళ (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, దొంతర, లోతు, SCORE_REP))
		{
		    go_deeper(దొంతర, లోతు, SCORE_REP);
#నిర్వచితమైతే DEBUG_TRIEWALK
		    sprintf(changename[లోతు], "%.*s-%s: replace %s with %s",
			    sp->ts_twordlen, tword, fword + sp->ts_fidx,
			    ftp->ft_from, ftp->ft_గమ్యం);
#అయితే_ముగింపు
		    // Need to undo this afterwards.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_REP_UNDO;

		    // Change the "from" to the "to" string.
		    ++లోతు;
		    fl = (పూర్ణాంకం)STRLEN(ftp->ft_from);
		    tl = (పూర్ణాంకం)STRLEN(ftp->ft_గమ్యం);
		    ఒకవేళ (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_గమ్యం, tl);
		    దొంతర[లోతు].ts_fidxtry = sp->ts_fidx + tl;
		    దొంతర[లోతు].ts_tcharlen = 0;
		    ఆపు;
		}
	    }

	    ఒకవేళ (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    ఆపు;

	స్థితి STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    ఒకవేళ (soundfold)
		gap = &slang->sl_repsal;
	    లేకపోతే
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_దత్తాంశం + sp->ts_curi - 1;
	    fl = (పూర్ణాంకం)STRLEN(ftp->ft_from);
	    tl = (పూర్ణాంకం)STRLEN(ftp->ft_గమ్యం);
	    p = fword + sp->ts_fidx;
	    ఒకవేళ (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    ఆపు;

	అప్రమేయ:
	    // Did all possible states at this level, go up one level.
	    --లోతు;

	    ఒకవేళ (లోతు >= 0 && దొంతర[లోతు].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    ఒకవేళ (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#నిర్వచితమైతే FEAT_RELTIME
		ఒకవేళ (spell_suggest_timeout > 0
					  && profile_passed_హద్దు(&సమయం_హద్దు))
		    got_int = TRUE;
#అయితే_ముగింపు
	    }
	}
    }
}


/*
 * Go one level deeper in the tree.
 */
    స్థిర శూన్య
go_deeper(trystate_T *దొంతర, పూర్ణాంకం లోతు, పూర్ణాంకం score_కూడు)
{
    దొంతర[లోతు + 1] = దొంతర[లోతు];
    దొంతర[లోతు + 1].ts_state = STATE_ప్రారంభం;
    దొంతర[లోతు + 1].ts_score = దొంతర[లోతు].ts_score + score_కూడు;
    దొంతర[లోతు + 1].ts_curi = 1;	// start just after length byte
    దొంతర[లోతు + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    స్థిర శూన్య
కనుగొను_keepcap_మాట(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    పూర్ణాంకం		లోతు;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    పూర్ణాంకం		విడత[MAXWLEN];
    పూర్ణాంకం		fwordidx[MAXWLEN];
    పూర్ణాంకం		uwordidx[MAXWLEN];
    పూర్ణాంకం		kwordlen[MAXWLEN];

    పూర్ణాంకం		flen, ulen;
    పూర్ణాంకం		l;
    పూర్ణాంకం		len;
    పూర్ణాంకం		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    ఒకవేళ (byts == ఖాళీ)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	తిరిగి;
    }

    // Make an all-cap version of "fword".
    allcap_copy(fword, uword);

    // Each character needs to be tried both case-folded and upper-case.
    // All this gets very complicated if we keep in mind that changing case
    // may change the byte length of a multi-byte character...
    లోతు = 0;
    arridx[0] = 0;
    విడత[0] = 0;
    fwordidx[0] = 0;
    uwordidx[0] = 0;
    kwordlen[0] = 0;
    అయితే (లోతు >= 0)
    {
	ఒకవేళ (fword[fwordidx[లోతు]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    ఒకవేళ (byts[arridx[లోతు] + 1] == 0)
	    {
		kword[kwordlen[లోతు]] = NUL;
		తిరిగి;
	    }

	    // kword is getting too long, continue one level up
	    --లోతు;
	}
	లేకపోతే ఒకవేళ (++విడత[లోతు] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --లోతు;
	}
	లేకపోతే
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    ఒకవేళ (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[లోతు]);
		ulen = MB_CPTR2LEN(uword + uwordidx[లోతు]);
	    }
	    లేకపోతే
		ulen = flen = 1;
	    ఒకవేళ (విడత[లోతు] == 1)
	    {
		p = fword + fwordidx[లోతు];
		l = flen;
	    }
	    లేకపోతే
	    {
		p = uword + uwordidx[లోతు];
		l = ulen;
	    }

	    కొరకు (tryidx = arridx[లోతు]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
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
		tryidx = idxs[lo];
	    }

	    ఒకవేళ (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		ఒకవేళ (విడత[లోతు] == 1)
		{
		    STRNCPY(kword + kwordlen[లోతు], fword + fwordidx[లోతు],
									flen);
		    kwordlen[లోతు + 1] = kwordlen[లోతు] + flen;
		}
		లేకపోతే
		{
		    STRNCPY(kword + kwordlen[లోతు], uword + uwordidx[లోతు],
									ulen);
		    kwordlen[లోతు + 1] = kwordlen[లోతు] + ulen;
		}
		fwordidx[లోతు + 1] = fwordidx[లోతు] + flen;
		uwordidx[లోతు + 1] = uwordidx[లోతు] + ulen;

		++లోతు;
		arridx[లోతు] = tryidx;
		విడత[లోతు] = 0;
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
    స్థిర శూన్య
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    పూర్ణాంకం		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    పూర్ణాంకం		score;
    పూర్ణాంకం		lpi;

    ఒకవేళ (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	తిరిగి;

    // Use the sound-folding of the first language that supports it.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	ఒకవేళ (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    కొరకు (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		ఒకవేళ (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_మాట = vim_strsave(stp->st_మాట);
		    ఒకవేళ (sstp->st_మాట != ఖాళీ)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    ఆపు;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    స్థిర శూన్య
score_combine(suginfo_T *su)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    పూర్ణాంకం		విడత;
    పూర్ణాంకం		lpi;
    slang_T	*slang = ఖాళీ;

    // Add the alternate score to su_ga.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	ఒకవేళ (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    కొరకు (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		ఒకవేళ (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		లేకపోతే
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    ఆపు;
	}
    }

    ఒకవేళ (slang == ఖాళీ)	// Using "double" without sound folding.
    {
	(శూన్య)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	తిరిగి;
    }

    // Add the alternate score to su_sga.
    కొరకు (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_మాట);
	ఒకవేళ (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	లేకపోతే
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (శూన్య)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (శూన్య)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, పరిమాణం(suginfo_T), 1);
    ఒకవేళ (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	తిరిగి;

    stp = &SUG(ga, 0);
    కొరకు (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	కొరకు (విడత = 1; విడత <= 2; ++విడత)
	{
	    gap = విడత == 1 ? &su->su_ga : &su->su_sga;
	    ఒకవేళ (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_మాట;
		కొరకు (j = 0; j < ga.ga_len; ++j)
		    ఒకవేళ (STRCMP(stp[j].st_మాట, p) == 0)
			ఆపు;
		ఒకవేళ (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		లేకపోతే
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    ఒకవేళ (ga.ga_len > su->su_maxcount)
    {
	కొరకు (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_మాట);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    స్థిర పూర్ణాంకం
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
    పూర్ణాంకం		lendiff;

    lendiff = (పూర్ణాంకం)(su->su_badlen - stp->st_orglen);
    ఒకవేళ (lendiff >= 0)
	pbad = badsound;
    లేకపోతే
    {
	// soundfold the bad word with more characters following
	(శూన్య)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	ఒకవేళ (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_మాట) == NUL)
	    కొరకు (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    ఒకవేళ (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_మాట);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    లేకపోతే
	pgood = stp->st_మాట;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    తిరిగి soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
రకనిర్వచనం నిర్మాణం
{
    పొట్టి	sft_score;	// lowest score used
    char_u	sft_మాట[1];    // soundfolded word, actually longer
} sftword_T;

స్థిర sftword_T dumsft;
#నిర్వచించు HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_మాట - (char_u *)&dumsft)))
#నిర్వచించు HI2SFT(hi)     HIKEY2SFT((hi)->hi_కీలకం)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    స్థిర శూన్య
suggest_try_soundalike_prep(శూన్య)
{
    langp_T	*lp;
    పూర్ణాంకం		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ఒకవేళ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ఖాళీ)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    స్థిర శూన్య
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    పూర్ణాంకం		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ఒకవేళ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ఖాళీ)
	{
	    // soundfold the bad word
	    spell_soundfold(slang, su->su_fbadword, TRUE, salword);

	    // try all kinds of inserts/deletes/swaps/etc.
	    // TODO: also soundfold the next words, so that we can try joining
	    // and splitting
#నిర్వచితమైతే SUGGEST_PROFILE
	    prof_init();
#అయితే_ముగింపు
	    suggest_trie_walk(su, lp, salword, TRUE);
#నిర్వచితమైతే SUGGEST_PROFILE
	    prof_report("soundalike");
#అయితే_ముగింపు
	}
    }
}

/*
 * Finish up after calling suggest_try_soundalike().
 */
    స్థిర శూన్య
suggest_try_soundalike_ముగింపు(శూన్య)
{
    langp_T	*lp;
    పూర్ణాంకం		lpi;
    slang_T	*slang;
    పూర్ణాంకం		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ఒకవేళ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ఖాళీ)
	{
	    // Free the info about handled words.
	    todo = (పూర్ణాంకం)slang->sl_sounddone.ht_వాడిన;
	    FOR_ALL_HASHTAB_వస్తువులు(&slang->sl_sounddone, hi, todo)
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
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
    స్థిర శూన్య
కూడు_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    పూర్ణాంకం		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    పూర్ణాంకం		sfwordnr;
    char_u	*nrline;
    పూర్ణాంకం		orgnr;
    char_u	theword[MAXWLEN];
    పూర్ణాంకం		i;
    పూర్ణాంకం		wlen;
    char_u	*byts;
    idx_T	*idxs;
    పూర్ణాంకం		n;
    పూర్ణాంకం		wordcount;
    పూర్ణాంకం		wc;
    పూర్ణాంకం		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    పూర్ణాంకం		bc, gc;
    పూర్ణాంకం		హద్దు;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_మాట) + STRLEN(goodword) + 1);
	ఒకవేళ (sft != ఖాళీ)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_మాట, goodword);
	    hash_కూడు_అంశం(&slang->sl_sounddone, hi, sft->sft_మాట, hash);
	}
    }
    లేకపోతే
    {
	sft = HI2SFT(hi);
	ఒకవేళ (score >= sft->sft_score)
	    తిరిగి;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_కనుగొను(slang, goodword);
    ఒకవేళ (sfwordnr < 0)
    {
	అంతర్గత_error("add_sound_suggest()");
	తిరిగి;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    అయితే (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	కొరకు (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    ఒకవేళ (wordcount == orgnr && byts[n + 1] == NUL)
		ఆపు;	// found end of word

	    ఒకవేళ (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    కొరకు ( ; byts[n + i] == NUL; ++i)
		ఒకవేళ (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    వెళ్ళు badword;
		}

	    // One of the siblings must have the word.
	    కొరకు ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		ఒకవేళ (wordcount + wc > orgnr)
		    ఆపు;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	కొరకు (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    పూర్ణాంకం		flags = (పూర్ణాంకం)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    ఒకవేళ (flags & WF_NOSUGGEST)
		కొనసాగు;

	    ఒకవేళ (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		కనుగొను_keepcap_మాట(slang, theword, cword);
		p = cword;
	    }
	    లేకపోతే
	    {
		flags |= su->su_badflags;
		ఒకవేళ ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_మాట(theword, cword, flags);
		    p = cword;
		}
		లేకపోతే
		    p = theword;
	    }

	    // Add the suggestion.
	    ఒకవేళ (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		ఒకవేళ (score <= su->su_maxscore)
		    కూడు_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    లేకపోతే
	    {
		// Add a penalty for words in another region.
		ఒకవేళ ((flags & WF_REGION)
			    && (((గుర్తులేని)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		లేకపోతే
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		ఒకవేళ (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    ఒకవేళ (!SPELL_ISUPPER(bc)
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
		హద్దు = MAXSCORE(su->su_sfmaxscore - goodscore, score);
		ఒకవేళ (హద్దు > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		లేకపోతే
		    goodscore += spell_edit_score_హద్దు(slang, su->su_badword,
								    p, హద్దు);

		// When going over the limit don't bother to do the rest.
		ఒకవేళ (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    ఒకవేళ (goodscore <= su->su_sfmaxscore)
			కూడు_suggestion(su, &su->su_ga, p, su->su_badlen,
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
    స్థిర పూర్ణాంకం
soundfold_కనుగొను(slang_T *slang, char_u *మాట)
{
    idx_T	arridx = 0;
    పూర్ణాంకం		len;
    పూర్ణాంకం		wlen = 0;
    పూర్ణాంకం		c;
    char_u	*ptr = మాట;
    char_u	*byts;
    idx_T	*idxs;
    పూర్ణాంకం		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    కొరకు (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	ఒకవేళ (byts[arridx] == NUL)
	{
	    ఒకవేళ (c == NUL)
		ఆపు;

	    // Skip over the zeros, there can be several.
	    అయితే (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    ఒకవేళ (len == 0)
		తిరిగి -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	ఒకవేళ (c == NUL)
	    తిరిగి -1;

	// Perform a binary search in the list of accepted bytes.
	ఒకవేళ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	అయితే (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    ఒకవేళ (--len == 0)	// end of the bytes, didn't find it
		తిరిగి -1;
	}
	ఒకవేళ (byts[arridx] != c)	// didn't find the byte
	    తిరిగి -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ఒకవేళ (c == ' ')
	    అయితే (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    తిరిగి wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    స్థిర పూర్ణాంకం
similar_chars(slang_T *slang, పూర్ణాంకం c1, పూర్ణాంకం c2)
{
    పూర్ణాంకం		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    ఒకవేళ (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_కనుగొను(&slang->sl_map_hash, buf);
	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    m1 = 0;
	లేకపోతే
	    m1 = mb_ptr2char(hi->hi_కీలకం + STRLEN(hi->hi_కీలకం) + 1);
    }
    లేకపోతే
	m1 = slang->sl_map_array[c1];
    ఒకవేళ (m1 == 0)
	తిరిగి FALSE;


    ఒకవేళ (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_కనుగొను(&slang->sl_map_hash, buf);
	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    m2 = 0;
	లేకపోతే
	    m2 = mb_ptr2char(hi->hi_కీలకం + STRLEN(hi->hi_కీలకం) + 1);
    }
    లేకపోతే
	m2 = slang->sl_map_array[c2];

    తిరిగి m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    స్థిర శూన్య
కూడు_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    పూర్ణాంకం		badlenarg,	// len of bad word replaced with "goodword"
    పూర్ణాంకం		score,
    పూర్ణాంకం		altscore,
    పూర్ణాంకం		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    పూర్ణాంకం		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    పూర్ణాంకం		goodlen;	// len of goodword changed
    పూర్ణాంకం		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    పూర్ణాంకం		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    కొరకు (;;)
    {
	goodlen = (పూర్ణాంకం)(pgood - goodword);
	badlen = (పూర్ణాంకం)(pbad - su->su_badptr);
	ఒకవేళ (goodlen <= 0 || badlen <= 0)
	    ఆపు;
	MB_PTR_వెనుక(goodword, pgood);
	MB_PTR_వెనుక(su->su_badptr, pbad);
	ఒకవేళ (has_mbyte)
	{
	    ఒకవేళ (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		ఆపు;
	}
	లేకపోతే ఒకవేళ (*pgood != *pbad)
		ఆపు;
    }

    ఒకవేళ (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	తిరిగి;

    ఒకవేళ (gap->ga_len == 0)
	i = -1;
    లేకపోతే
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	కొరకు (i = gap->ga_len; --i >= 0; ++stp)
	    ఒకవేళ (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_మాట, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		ఒకవేళ (stp->st_slang == ఖాళీ)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		ఒకవేళ (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    ఒకవేళ (had_bonus)
			rescore_one(su, stp);
		    లేకపోతే
		    {
			new_sug.st_మాట = stp->st_మాట;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		ఒకవేళ (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		ఆపు;
	    }
    }

    ఒకవేళ (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_మాట = vim_strnsave(goodword, goodlen);
	ఒకవేళ (stp->st_మాట != ఖాళీ)
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
	    ఒకవేళ (gap->ga_len > SUG_MAX_లెక్క(su))
	    {
		ఒకవేళ (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_లెక్క(su));
		లేకపోతే
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_లెక్క(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    స్థిర శూన్య
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    పూర్ణాంకం		i;
    char_u	longword[MAXWLEN + 1];
    పూర్ణాంకం		len;
    hlf_T	attr;

    ఒకవేళ (gap->ga_len == 0)
	తిరిగి;
    stp = &SUG(*gap, 0);
    కొరకు (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_మాట, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_లెక్క;
	(శూన్య)spell_check(curwin, longword, &attr, ఖాళీ, FALSE);
	ఒకవేళ (attr != HLF_లెక్క)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_మాట);
	    --gap->ga_len;
	    ఒకవేళ (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       పరిమాణం(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    స్థిర శూన్య
కూడు_banned(
    suginfo_T	*su,
    char_u	*మాట)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(మాట);
    hi = hash_lookup(&su->su_banned, మాట, hash);
    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))		// already present
	తిరిగి;
    s = vim_strsave(మాట);
    ఒకవేళ (s != ఖాళీ)
	hash_కూడు_అంశం(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    స్థిర శూన్య
rescore_suggestions(suginfo_T *su)
{
    పూర్ణాంకం		i;

    ఒకవేళ (su->su_sallang != ఖాళీ)
	కొరకు (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    స్థిర శూన్య
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    ఒకవేళ (slang != ఖాళీ && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	ఒకవేళ (slang == su->su_sallang)
	    p = su->su_sal_badword;
	లేకపోతే
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	ఒకవేళ (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

స్థిర పూర్ణాంకం sug_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    స్థిర పూర్ణాంకం
sug_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    పూర్ణాంకం		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    ఒకవేళ (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	ఒకవేళ (n == 0)
	    n = STRICMP(p1->st_మాట, p2->st_మాట);
    }
    తిరిగి n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    స్థిర పూర్ణాంకం
cleanup_suggestions(
    garray_T	*gap,
    పూర్ణాంకం		maxscore,
    పూర్ణాంకం		keep)		// nr of suggestions to keep
{
    ఒకవేళ (gap->ga_len <= 0)
	తిరిగి maxscore;

    // Sort the list.
    qsort(gap->ga_దత్తాంశం, (size_t)gap->ga_len, పరిమాణం(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    ఒకవేళ (gap->ga_len > keep)
    {
	పూర్ణాంకం		i;
	suggest_T   *stp = &SUG(*gap, 0);

	కొరకు (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_మాట);
	gap->ga_len = keep;
	ఒకవేళ (keep >= 1)
	    తిరిగి stp[keep - 1].st_score;
    }
    తిరిగి maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    స్థిర పూర్ణాంకం
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    పూర్ణాంకం		goodlen;
    పూర్ణాంకం		badlen;
    పూర్ణాంకం		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    పూర్ణాంకం		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    ఒకవేళ ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	ఒకవేళ ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    తిరిగి SCORE_DEL;
	ఒకవేళ (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    తిరిగి SCORE_MAXMAX;

	ఒకవేళ (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	లేకపోతే
	{
	    score = 2 * SCORE_DEL / 3;
	    ఒకవేళ (*badsound == '*')
		++badsound;
	    లేకపోతే
		++goodsound;
	}
    }

    goodlen = (పూర్ణాంకం)STRLEN(goodsound);
    badlen = (పూర్ణాంకం)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    ఒకవేళ (n < -2 || n > 2)
	తిరిగి SCORE_MAXMAX;

    ఒకవేళ (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    లేకపోతే
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    అయితే (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    ఎంపిక (n)
    {
	స్థితి -2:
	స్థితి 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    అయితే (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    ఒకవేళ (STRCMP(pl + 1, ps) == 0)
		తిరిగి score + SCORE_DEL * 2;

	    // Failed to compare.
	    ఆపు;

	స్థితి -1:
	స్థితి 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    అయితే (*pl2 == *ps2)
	    {
		ఒకవేళ (*pl2 == NUL)	// reached the end
		    తిరిగి score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    ఒకవేళ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		తిరిగి score + SCORE_DEL + SCORE_మార్పిడి;

	    // 3: delete then substitute, then the rest must be equal
	    ఒకవేళ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		తిరిగి score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    ఒకవేళ (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		అయితే (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		ఒకవేళ (STRCMP(pl2 + 1, ps2) == 0)
		    తిరిగి score + SCORE_మార్పిడి + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    అయితే (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    ఒకవేళ (STRCMP(pl2 + 1, ps2) == 0)
		తిరిగి score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    ఆపు;

	స్థితి 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    ఒకవేళ (*pl == NUL)
		తిరిగి score;

	    // 2: swap
	    ఒకవేళ (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		అయితే (*pl2 == *ps2)
		{
		    ఒకవేళ (*pl2 == NUL)	// reached the end
			తిరిగి score + SCORE_మార్పిడి;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		ఒకవేళ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    తిరిగి score + SCORE_మార్పిడి + SCORE_మార్పిడి;

		// 4: swap and substitute
		ఒకవేళ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    తిరిగి score + SCORE_మార్పిడి + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    అయితే (*pl2 == *ps2)
	    {
		ఒకవేళ (*pl2 == NUL)	// reached the end
		    తిరిగి score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    ఒకవేళ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		తిరిగి score + SCORE_SUBST + SCORE_మార్పిడి;

	    // 7: substitute and substitute
	    ఒకవేళ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		తిరిగి score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    అయితే (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    ఒకవేళ (STRCMP(pl2 + 1, ps2) == 0)
		తిరిగి score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    అయితే (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    ఒకవేళ (STRCMP(pl2, ps2 + 1) == 0)
		తిరిగి score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    ఆపు;
    }

    తిరిగి SCORE_MAXMAX;
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
    స్థిర పూర్ణాంకం
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    పూర్ణాంకం		*cnt;
    పూర్ణాంకం		badlen, goodlen;	// lengths including NUL
    పూర్ణాంకం		j, i;
    పూర్ణాంకం		t;
    పూర్ణాంకం		bc, gc;
    పూర్ణాంకం		pbc, pgc;
    char_u	*p;
    పూర్ణాంకం		wbadword[MAXWLEN];
    పూర్ణాంకం		wgoodword[MAXWLEN];

    ఒకవేళ (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	కొరకు (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	కొరకు (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    లేకపోతే
    {
	badlen = (పూర్ణాంకం)STRLEN(badword) + 1;
	goodlen = (పూర్ణాంకం)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#నిర్వచించు CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(పూర్ణాంకం, (badlen + 1) * (goodlen + 1));
    ఒకవేళ (cnt == ఖాళీ)
	తిరిగి 0;	// out of memory

    CNT(0, 0) = 0;
    కొరకు (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    కొరకు (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	కొరకు (j = 1; j <= goodlen; ++j)
	{
	    ఒకవేళ (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    లేకపోతే
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    ఒకవేళ (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    లేకపోతే
	    {
		// Use a better score when there is only a case difference.
		ఒకవేళ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		లేకపోతే
		{
		    // For a similar character use SCORE_SIMILAR.
		    ఒకవేళ (slang != ఖాళీ
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    లేకపోతే
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		ఒకవేళ (i > 1 && j > 1)
		{
		    ఒకవేళ (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    లేకపోతే
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    ఒకవేళ (bc == pgc && pbc == gc)
		    {
			t = SCORE_మార్పిడి + CNT(i - 2, j - 2);
			ఒకవేళ (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		ఒకవేళ (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		ఒకవేళ (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    తిరిగి i;
}

రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		badi;
    పూర్ణాంకం		goodi;
    పూర్ణాంకం		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    స్థిర పూర్ణాంకం
spell_edit_score_హద్దు(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    పూర్ణాంకం		హద్దు)
{
    limitscore_T    దొంతర[10];		// allow for over 3 * 2 edits
    పూర్ణాంకం		    stackidx;
    పూర్ణాంకం		    bi, gi;
    పూర్ణాంకం		    bi2, gi2;
    పూర్ణాంకం		    bc, gc;
    పూర్ణాంకం		    score;
    పూర్ణాంకం		    score_off;
    పూర్ణాంకం		    minscore;
    పూర్ణాంకం		    విడత;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    ఒకవేళ (has_mbyte)
	తిరిగి spell_edit_score_హద్దు_w(slang, badword, goodword, హద్దు);

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
    minscore = హద్దు + 1;

    కొరకు (;;)
    {
	// Skip over an equal part, score remains the same.
	కొరకు (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    ఒకవేళ (bc != gc)	// stop at a char that's different
		ఆపు;
	    ఒకవేళ (bc == NUL)	// both words end
	    {
		ఒకవేళ (score < minscore)
		    minscore = score;
		వెళ్ళు తీయి;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	ఒకవేళ (gc == NUL)    // goodword ends, delete badword chars
	{
	    చేయి
	    {
		ఒకవేళ ((score += SCORE_DEL) >= minscore)
		    వెళ్ళు తీయి;	    // do next alternative
	    } అయితే (badword[++bi] != NUL);
	    minscore = score;
	}
	లేకపోతే ఒకవేళ (bc == NUL) // badword ends, insert badword chars
	{
	    చేయి
	    {
		ఒకవేళ ((score += SCORE_INS) >= minscore)
		    వెళ్ళు తీయి;	    // do next alternative
	    } అయితే (goodword[++gi] != NUL);
	    minscore = score;
	}
	లేకపోతే			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    కొరకు (విడత = 0; విడత <= 1; ++విడత)
	    {
		score_off = score + (విడత == 0 ? SCORE_DEL : SCORE_INS);
		ఒకవేళ (score_off < minscore)
		{
		    ఒకవేళ (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - విడత;
			gi2 = gi + విడత;
			అయితే (goodword[gi2] == badword[bi2])
			{
			    ఒకవేళ (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				ఆపు;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    లేకపోతే
		    {
			// try deleting/inserting a character later
			దొంతర[stackidx].badi = bi + 1 - విడత;
			దొంతర[stackidx].goodi = gi + విడత;
			దొంతర[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    ఒకవేళ (score + SCORE_మార్పిడి < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		ఒకవేళ (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_మార్పిడి;
		    కొనసాగు;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    ఒకవేళ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    లేకపోతే
	    {
		// For a similar character use SCORE_SIMILAR.
		ఒకవేళ (slang != ఖాళీ
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		లేకపోతే
		    score += SCORE_SUBST;
	    }

	    ఒకవేళ (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		కొనసాగు;
	    }
	}
తీయి:
	// Get here to try the next alternative, pop it from the stack.
	ఒకవేళ (stackidx == 0)		// stack is empty, finished
	    ఆపు;

	// pop an item from the stack
	--stackidx;
	gi = దొంతర[stackidx].goodi;
	bi = దొంతర[stackidx].badi;
	score = దొంతర[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    ఒకవేళ (minscore > హద్దు)
	తిరిగి SCORE_MAXMAX;
    తిరిగి minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    స్థిర పూర్ణాంకం
spell_edit_score_హద్దు_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    పూర్ణాంకం		హద్దు)
{
    limitscore_T    దొంతర[10];		// allow for over 3 * 2 edits
    పూర్ణాంకం		    stackidx;
    పూర్ణాంకం		    bi, gi;
    పూర్ణాంకం		    bi2, gi2;
    పూర్ణాంకం		    bc, gc;
    పూర్ణాంకం		    score;
    పూర్ణాంకం		    score_off;
    పూర్ణాంకం		    minscore;
    పూర్ణాంకం		    విడత;
    char_u	    *p;
    పూర్ణాంకం		    wbadword[MAXWLEN];
    పూర్ణాంకం		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    కొరకు (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    కొరకు (p = goodword; *p != NUL; )
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
    minscore = హద్దు + 1;

    కొరకు (;;)
    {
	// Skip over an equal part, score remains the same.
	కొరకు (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    ఒకవేళ (bc != gc)	// stop at a char that's different
		ఆపు;
	    ఒకవేళ (bc == NUL)	// both words end
	    {
		ఒకవేళ (score < minscore)
		    minscore = score;
		వెళ్ళు తీయి;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	ఒకవేళ (gc == NUL)    // goodword ends, delete badword chars
	{
	    చేయి
	    {
		ఒకవేళ ((score += SCORE_DEL) >= minscore)
		    వెళ్ళు తీయి;	    // do next alternative
	    } అయితే (wbadword[++bi] != NUL);
	    minscore = score;
	}
	లేకపోతే ఒకవేళ (bc == NUL) // badword ends, insert badword chars
	{
	    చేయి
	    {
		ఒకవేళ ((score += SCORE_INS) >= minscore)
		    వెళ్ళు తీయి;	    // do next alternative
	    } అయితే (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	లేకపోతే			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    కొరకు (విడత = 0; విడత <= 1; ++విడత)
	    {
		score_off = score + (విడత == 0 ? SCORE_DEL : SCORE_INS);
		ఒకవేళ (score_off < minscore)
		{
		    ఒకవేళ (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - విడత;
			gi2 = gi + విడత;
			అయితే (wgoodword[gi2] == wbadword[bi2])
			{
			    ఒకవేళ (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				ఆపు;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    లేకపోతే
		    {
			// try deleting a character from badword later
			దొంతర[stackidx].badi = bi + 1 - విడత;
			దొంతర[stackidx].goodi = gi + విడత;
			దొంతర[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    ఒకవేళ (score + SCORE_మార్పిడి < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		ఒకవేళ (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_మార్పిడి;
		    కొనసాగు;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    ఒకవేళ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    లేకపోతే
	    {
		// For a similar character use SCORE_SIMILAR.
		ఒకవేళ (slang != ఖాళీ
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		లేకపోతే
		    score += SCORE_SUBST;
	    }

	    ఒకవేళ (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		కొనసాగు;
	    }
	}
తీయి:
	// Get here to try the next alternative, pop it from the stack.
	ఒకవేళ (stackidx == 0)		// stack is empty, finished
	    ఆపు;

	// pop an item from the stack
	--stackidx;
	gi = దొంతర[stackidx].goodi;
	bi = దొంతర[stackidx].badi;
	score = దొంతర[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    ఒకవేళ (minscore > హద్దు)
	తిరిగి SCORE_MAXMAX;
    తిరిగి minscore;
}
#అయితే_ముగింపు  // FEAT_SPELL
