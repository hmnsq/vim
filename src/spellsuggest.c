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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SPELL)

/*
 * Use this to adjust the score after finding suggestions, based on the
 * suggested word sounding like the bad word.  This is much faster than doing
 * it for every possible suggestion.
 * Disadvantage: When "the" is typed as "hte" it sounds quite different ("@"
 * vs "ht") and goes down in the list.
 * Used when 'spellsuggest' is set to "best".
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESCORE(ਸ਼ਬਦ_score, sound_score) ((3 * (ਸ਼ਬਦ_score) + (sound_score)) / 4)

/*
 * Do the opposite: based on a maximum end score and a known sound score,
 * compute the maximum word score that can be used.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXSCORE(ਸ਼ਬਦ_score, sound_score) ((4 * (ਸ਼ਬਦ_score) - (sound_score)) / 3)

// only used for su_badflags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_MIXCAP   0x20	// mix of upper and lower case: macaRONI

/*
 * Information used when looking for suggestions.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    ਪੂਰਨ		su_maxcount;	    // max. number of suggestions displayed
    ਪੂਰਨ		su_maxscore;	    // maximum score for adding to su_ga
    ਪੂਰਨ		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    ਪੂਰਨ		su_badlen;	    // length of detected bad word in line
    ਪੂਰਨ		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ suggest_S
{
    char_u	*st_ਸ਼ਬਦ;	// suggested word, allocated string
    ਪੂਰਨ		st_wordlen;	// STRLEN(st_word)
    ਪੂਰਨ		st_orglen;	// length of replaced text
    ਪੂਰਨ		st_score;	// lower is better
    ਪੂਰਨ		st_altscore;	// used when st_score compares equal
    ਪੂਰਨ		st_salscore;	// st_score is for soundalike
    ਪੂਰਨ		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUG(ga, i) (((suggest_T *)(ga).ga_ਅੰਕੜੇ)[i])

// TRUE if a word appears in the list of banned words.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WAS_BANNED(su, ਸ਼ਬਦ) (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hash_ਲੱਭੋ(&(su)->su_banned, ਸ਼ਬਦ)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUG_CLEAN_ਗਿਣਤੀ(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUG_MAX_ਗਿਣਤੀ(su)	(SUG_CLEAN_ਗਿਣਤੀ(su) + 50)

// score for various changes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਵੰਡੋ	149	// split bad word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਵੰਡੋ_NO	249	// split bad word with NOSPLITSUGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ICASE	52	// slightly different case
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_REGION	200	// word is for different region
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_RARE	180	// rare word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_ਅਦਲਾ_ਬਦਲੀ	75	// swap two characters
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SWAP3	110	// swap two characters in three
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_REP	65	// REP replacement
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SUBST	93	// substitute a character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SIMILAR	33	// substitute a similar character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SUBCOMP	33	// substitute a composing character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_DEL	94	// delete a character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_DELDUP	66	// delete a duplicated character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_DELCOMP	28	// delete a composing character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_INS	96	// insert a character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_INSDUP	67	// insert a duplicate character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_INSCOMP	30	// insert a composing character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_NONWORD	103	// change non-word to word char

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_FILE	30	// suggestion from a file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_MAXINIT	350	// Initial maximum score: higher == slower.
				// 350 allows for about three changes.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_COMMON1	30	// subtracted for words seen before
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_COMMON2	40	// subtracted for words often seen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_COMMON3	50	// subtracted for words very often seen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_THRES2	10	// word count threshold for COMMON2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_THRES3	100	// word count threshold for COMMON3

// When trying changed soundfold words it becomes slow when trying more than
// two changes.  With less than two changes it's slightly faster but we miss a
// few good suggestions.  In rare cases we need to try three of four changes.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SFMAX1	200	// maximum score for first try
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SFMAX2	300	// maximum score for second try
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_SFMAX3	400	// maximum score for third try

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_BIG	(SCORE_INS * 3)	// big difference
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_MAXMAX	999999		// accept any score
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_LIMITMAX	350		// for spell_edit_score_limit()

// for spell_edit_score_limit() we need to know the minimum value of
// SCORE_ICASE, SCORE_SWAP, SCORE_DEL, SCORE_SIMILAR and SCORE_INS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCORE_EDIT_MIN	SCORE_SIMILAR

/*
 * For finding suggestions: At each node in the tree these states are tried:
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    STATE_ਸ਼ੁਰੂ = 0,	// At start of node check for NUL bytes (goodword
			// ends); if badword ends there is a match, otherwise
			// try splitting word.
    STATE_NOPREFIX,	// try without prefix
    STATE_SPLITUNDO,	// Undo splitting.
    STATE_ENDNUL,	// Past NUL bytes at start of the node.
    STATE_PLAIN,	// Use each byte of the node.
    STATE_DEL,		// Delete a byte from the bad word.
    STATE_INS_PREP,	// Prepare for inserting bytes.
    STATE_INS,		// Insert a byte in the bad word.
    STATE_ਅਦਲਾ_ਬਦਲੀ,		// Swap two bytes.
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    ਪੂਰਨ		ts_score;	// score
    idx_T	ts_arridx;	// index in tree array, start of node
    ਛੋਟਾ	ts_curi;	// index in list of child nodes
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIFF_NONE	0	// no different byte (yet)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIFF_YES	1	// different byte found
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIFF_ਪਾਓ	2	// inserting character

// values for ts_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSF_PREFIXOK	1	// already checked that prefix is OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSF_DIDSPLIT	2	// tried split at this point
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFD_NOPREFIX	0xff	// not using prefixes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFD_NOTSPECIAL	0xfd	// highest value that's not special

ਸਥਿਰ ਲੰਮਾ spell_suggest_timeout = 5000;

ਸਥਿਰ ਖਾਲੀ spell_ਲੱਭੋ_suggest(char_u *badptr, ਪੂਰਨ badlen, suginfo_T *su, ਪੂਰਨ maxcount, ਪੂਰਨ banbadword, ਪੂਰਨ need_cap, ਪੂਰਨ interactive);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਖਾਲੀ spell_suggest_ਵਿਅੰਜਕ(suginfo_T *su, char_u *ਵਿਅੰਜਕ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ spell_suggest_file(suginfo_T *su, char_u *fname);
ਸਥਿਰ ਖਾਲੀ spell_suggest_intern(suginfo_T *su, ਪੂਰਨ interactive);
ਸਥਿਰ ਖਾਲੀ spell_ਲੱਭੋ_cleanup(suginfo_T *su);
ਸਥਿਰ ਖਾਲੀ suggest_try_special(suginfo_T *su);
ਸਥਿਰ ਖਾਲੀ suggest_try_change(suginfo_T *su);
ਸਥਿਰ ਖਾਲੀ suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, ਪੂਰਨ soundfold);
ਸਥਿਰ ਖਾਲੀ go_deeper(trystate_T *ਢੇਰ, ਪੂਰਨ ਡੂੰਘਾਈ, ਪੂਰਨ score_ਜੋੜੋ);
ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_keepcap_ਸ਼ਬਦ(slang_T *slang, char_u *fword, char_u *kword);
ਸਥਿਰ ਖਾਲੀ score_comp_sal(suginfo_T *su);
ਸਥਿਰ ਖਾਲੀ score_combine(suginfo_T *su);
ਸਥਿਰ ਪੂਰਨ stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
ਸਥਿਰ ਖਾਲੀ suggest_try_soundalike_prep(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ suggest_try_soundalike(suginfo_T *su);
ਸਥਿਰ ਖਾਲੀ suggest_try_soundalike_ਅੰਤ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_sound_suggest(suginfo_T *su, char_u *goodword, ਪੂਰਨ score, langp_T *lp);
ਸਥਿਰ ਪੂਰਨ soundfold_ਲੱਭੋ(slang_T *slang, char_u *ਸ਼ਬਦ);
ਸਥਿਰ ਪੂਰਨ similar_chars(slang_T *slang, ਪੂਰਨ c1, ਪੂਰਨ c2);
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, ਪੂਰਨ badlen, ਪੂਰਨ score, ਪੂਰਨ altscore, ਪੂਰਨ had_bonus, slang_T *slang, ਪੂਰਨ maxsf);
ਸਥਿਰ ਖਾਲੀ check_suggestions(suginfo_T *su, garray_T *gap);
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_banned(suginfo_T *su, char_u *ਸ਼ਬਦ);
ਸਥਿਰ ਖਾਲੀ rescore_suggestions(suginfo_T *su);
ਸਥਿਰ ਖਾਲੀ rescore_one(suginfo_T *su, suggest_T *stp);
ਸਥਿਰ ਪੂਰਨ cleanup_suggestions(garray_T *gap, ਪੂਰਨ maxscore, ਪੂਰਨ keep);
ਸਥਿਰ ਪੂਰਨ soundalike_score(char_u *goodsound, char_u *badsound);
ਸਥਿਰ ਪੂਰਨ spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
ਸਥਿਰ ਪੂਰਨ spell_edit_score_ਹੱਦਬੰਦੀ(slang_T *slang, char_u *badword, char_u *goodword, ਪੂਰਨ ਹੱਦਬੰਦੀ);
ਸਥਿਰ ਪੂਰਨ spell_edit_score_ਹੱਦਬੰਦੀ_w(slang_T *slang, char_u *badword, char_u *goodword, ਪੂਰਨ ਹੱਦਬੰਦੀ);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    ਸਥਿਰ ਪੂਰਨ
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    ਪੂਰਨ		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    ਜੇ (!byte_ਵਿੱਚ_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	ਵਾਪਸ FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    ਜੇ (slang->sl_comprules != ਨਿਰਾ && sp->ts_complen > sp->ts_compsplit)
    {
	ਪੂਰਨ v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = ਮੇਲ_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	ਵਾਪਸ v;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Adjust the score of common words.
 */
    ਸਥਿਰ ਪੂਰਨ
score_wordcount_adj(
    slang_T	*slang,
    ਪੂਰਨ		score,
    char_u	*ਸ਼ਬਦ,
    ਪੂਰਨ		ਵੰਡੋ)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    ਪੂਰਨ		bonus;
    ਪੂਰਨ		newscore;

    hi = hash_ਲੱਭੋ(&slang->sl_wordcount, ਸ਼ਬਦ);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ score;

    wc = HI2WC(hi);
    ਜੇ (wc->wc_ਗਿਣਤੀ < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    ਨਹੀਂਤਾਂ ਜੇ (wc->wc_ਗਿਣਤੀ < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    ਨਹੀਂਤਾਂ
	bonus = SCORE_COMMON3;
    ਜੇ (ਵੰਡੋ)
	newscore = score - bonus / 2;
    ਨਹੀਂਤਾਂ
	newscore = score - bonus;
    ਜੇ (newscore < 0)
	ਵਾਪਸ 0;
    ਵਾਪਸ newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    ਸਥਿਰ ਪੂਰਨ
badword_captype(char_u *ਸ਼ਬਦ, char_u *ਸਿਰਾ)
{
    ਪੂਰਨ		flags = captype(ਸ਼ਬਦ, ਸਿਰਾ);
    ਪੂਰਨ		c;
    ਪੂਰਨ		l, u;
    ਪੂਰਨ		ਪਹਿਲਾ;
    char_u	*p;

    ਜੇ (!(flags & WF_KEEPCAP))
	ਵਾਪਸ flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    ਪਹਿਲਾ = FALSE;
    ਲਈ (p = ਸ਼ਬਦ; p < ਸਿਰਾ; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	ਜੇ (SPELL_ISUPPER(c))
	{
	    ++u;
	    ਜੇ (p == ਸ਼ਬਦ)
		ਪਹਿਲਾ = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    ਜੇ (u > l && u > 2)
	flags |= WF_ALLCAP;
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ)
	flags |= WF_ONECAP;

    ਜੇ (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    ਵਾਪਸ flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    ਸਥਿਰ ਪੂਰਨ
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    ਪੂਰਨ		nr;
    ਪੂਰਨ		c;

    c = *p++;
    ਜੇ ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    ਨਹੀਂਤਾਂ				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    ਵਾਪਸ nr;
}

// values for sps_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPS_ਉੱਤਮ    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPS_FAST    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPS_DOUBLE  4

ਸਥਿਰ ਪੂਰਨ sps_flags = SPS_ਉੱਤਮ;	// flags from 'spellsuggest'
ਸਥਿਰ ਪੂਰਨ sps_ਹੱਦਬੰਦੀ = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    ਪੂਰਨ
spell_check_sps(ਖਾਲੀ)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    ਪੂਰਨ		f;

    sps_flags = 0;
    sps_ਹੱਦਬੰਦੀ = 9999;

    ਲਈ (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	ਜੇ (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_ਹੱਦਬੰਦੀ = getdigits(&s);
	    ਜੇ (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(buf, "best") == 0)
	    f = SPS_ਉੱਤਮ;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	ਜੇ (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_ਉੱਤਮ;
	    sps_ਹੱਦਬੰਦੀ = 9999;
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (f != 0)
	    sps_flags = f;
    }

    ਜੇ (sps_flags == 0)
	sps_flags = SPS_ਉੱਤਮ;

    ਵਾਪਸ OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    ਖਾਲੀ
spell_suggest(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    char_u	*ਸਤਰ = ਨਿਰਾ;
    pos_T	prev_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;
    suginfo_T	sug;
    suggest_T	*stp;
    ਪੂਰਨ		mouse_ਵਰਤਿਆ;
    ਪੂਰਨ		need_cap;
    ਪੂਰਨ		ਹੱਦਬੰਦੀ;
    ਪੂਰਨ		selected = ਗਿਣਤੀ_2;
    ਪੂਰਨ		badlen = 0;
    ਪੂਰਨ		ਸੁਨੇਹਾ_scroll_save = ਸੁਨੇਹਾ_scroll;
    ਪੂਰਨ		wo_spell_save = curwin->w_p_spell;

    ਜੇ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    prev_cursor = curwin->w_cursor;

    ਜੇ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	ਜਾਓ skip;
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	ਜੇ (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    ਜਾਓ skip;
	}
	badlen = (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ - (ਪੂਰਨ)VIsual.ਖਾਨਾ;
	ਜੇ (badlen < 0)
	    badlen = -badlen;
	ਨਹੀਂਤਾਂ
	    curwin->w_cursor.ਖਾਨਾ = VIsual.ਖਾਨਾ;
	++badlen;
	ਸਿਰਾ_visual_ਬਹੁਲਕ();
	// make sure we don't include the NUL at the end of the line
	ਜੇ (badlen > ml_get_curline_len() - (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ)
	    badlen = ml_get_curline_len() - (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ;
    }
    // Find the start of the badly spelled word.
    ਨਹੀਂਤਾਂ ਜੇ (spell_move_ਮੰਜ਼ਿਲ(curwin, FORWARD, SMT_ALL, TRUE, ਨਿਰਾ) == 0
	    || curwin->w_cursor.ਖਾਨਾ > prev_cursor.ਖਾਨਾ)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = prev_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.ਖਾਨਾ;
	// Backup to before start of word.
	ਜਦੋਂ (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_ਪਿੱਠ(curline, p);
	// Forward to start of word.
	ਜਦੋਂ (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	ਜੇ (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    ਜਾਓ skip;
	}
	curwin->w_cursor.ਖਾਨਾ = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.ਖਾਨਾ);

    // Make a copy of current line since autocommands may free the line.
    ਸਤਰ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ਜੇ (ਸਤਰ == ਨਿਰਾ)
	ਜਾਓ skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    ਜੇ (sps_ਹੱਦਬੰਦੀ > (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 2)
	ਹੱਦਬੰਦੀ = (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 2;
    ਨਹੀਂਤਾਂ
	ਹੱਦਬੰਦੀ = sps_ਹੱਦਬੰਦੀ;
    spell_ਲੱਭੋ_suggest(ਸਤਰ + curwin->w_cursor.ਖਾਨਾ, badlen, &sug, ਹੱਦਬੰਦੀ,
							TRUE, need_cap, TRUE);

    ਜੇ (sug.su_ga.ga_len == 0)
	ਸੁਨੇਹਾ(_("Sorry, no suggestions"));
    ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_2 > 0)
    {
	ਜੇ (ਗਿਣਤੀ_2 > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (ਲੰਮਾ)sug.su_ga.ga_len);
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	ਜੇ (cmdmsg_rl)
	    ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
#ਜੇ_ਅੰਤ

	// List the suggestions.
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;	// for when 'cmdheight' > 1
	ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ;	// avoid more prompt
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
	ਸੁਨੇਹਾ_clr_eos();
	ਸੁਨੇਹਾ_putchar('\n');

	ਸੁਨੇਹਾ_scroll = TRUE;
	ਲਈ (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    ਪੂਰਨ el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_ਸ਼ਬਦ, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    ਜੇ (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%2d", i + 1);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (cmdmsg_rl)
		rl_mirror(IObuff);
#ਜੇ_ਅੰਤ
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);

	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);

	    // The word may replace more than "su_badlen".
	    ਜੇ (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
	    }

	    ਜੇ (p_verbose > 0)
	    {
		// Add the score.
		ਜੇ (sps_flags & (SPS_DOUBLE | SPS_ਉੱਤਮ))
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		ਨਹੀਂਤਾਂ
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		ਜੇ (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_advance(30);
		ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
	    }
	    ਸੁਨੇਹਾ_putchar('\n');
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
#ਜੇ_ਅੰਤ
	// Ask for choice.
	selected = prompt_for_ਸੰਖਿਆ(&mouse_ਵਰਤਿਆ);
	ਜੇ (mouse_ਵਰਤਿਆ)
	    selected -= ਸਤਰਾਂ_ਖੱਬਾ;
	ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll_save;
    }

    ਜੇ (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_ਮੰਜ਼ਿਲ);

	stp = &SUG(sug.su_ga, selected - 1);
	ਜੇ (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%s%.*s", stp->st_ਸ਼ਬਦ,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_ਮੰਜ਼ਿਲ = vim_strsave(IObuff);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_ਮੰਜ਼ਿਲ = vim_strsave(stp->st_ਸ਼ਬਦ);
	}

	// Replace the word.
	p = alloc(STRLEN(ਸਤਰ) - stp->st_orglen + stp->st_wordlen + 1);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਪੂਰਨ len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (ਪੂਰਨ)(sug.su_badptr - ਸਤਰ);
	    mch_memmove(p, ਸਤਰ, c);
	    STRCPY(p + c, stp->st_ਸ਼ਬਦ);
	    STRCAT(p, sug.su_badptr + stp->st_orglen);

	    // For redo we use a change-word command.
	    ResetRedobuff();
	    Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)"ciw");
	    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(p + c,
			    stp->st_wordlen + sug.su_badlen - stp->st_orglen);
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(ESC);

	    // "p" may be freed here
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    curwin->w_cursor.ਖਾਨਾ = c;

	    ਬਦਲਿਆ_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, c);
#ਜੇ defined(FEAT_PROP_POPUP)
	    ਜੇ (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#ਜੇ_ਅੰਤ
	}
    }
    ਨਹੀਂਤਾਂ
	curwin->w_cursor = prev_cursor;

    spell_ਲੱਭੋ_cleanup(&sug);
skip:
    vim_free(ਸਤਰ);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    ਖਾਲੀ
spell_suggest_list(
    garray_T	*gap,
    char_u	*ਸ਼ਬਦ,
    ਪੂਰਨ		maxcount,	// maximum nr of suggestions
    ਪੂਰਨ		need_cap,	// 'spellcapcheck' matched
    ਪੂਰਨ		interactive)
{
    suginfo_T	sug;
    ਪੂਰਨ		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_ਲੱਭੋ_suggest(ਸ਼ਬਦ, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, ਆਕਾਰ(char_u *), sug.su_ga.ga_len + 1);
    ਜੇ (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	ਲਈ (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    ਜੇ (wcopy == ਨਿਰਾ)
		ਰੋਕੋ;
	    STRCPY(wcopy, stp->st_ਸ਼ਬਦ);
	    STRCPY(wcopy + stp->st_wordlen, sug.su_badptr + stp->st_orglen);
	    ((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = wcopy;
	}
    }

    spell_ਲੱਭੋ_cleanup(&sug);
}

/*
 * Find spell suggestions for the word at the start of "badptr".
 * Return the suggestions in "su->su_ga".
 * The maximum number of suggestions is "maxcount".
 * Note: does use info for the current window.
 * This is based on the mechanisms of Aspell, but completely reimplemented.
 */
    ਸਥਿਰ ਖਾਲੀ
spell_ਲੱਭੋ_suggest(
    char_u	*badptr,
    ਪੂਰਨ		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    ਪੂਰਨ		maxcount,
    ਪੂਰਨ		banbadword,	// don't include badword in suggestions
    ਪੂਰਨ		need_cap,	// word should start with capital
    ਪੂਰਨ		interactive)
{
    hlf_T	attr = HLF_ਗਿਣਤੀ;
    char_u	buf[MAXPATHL];
    char_u	*p;
    ਪੂਰਨ		do_combine = FALSE;
    char_u	*sps_copy;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸਥਿਰ ਪੂਰਨ	ਵਿਅੰਜਕ_busy = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;
    langp_T	*lp;
    ਪੂਰਨ		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_ਸੂਚਕ(su);
    ga_init2(&su->su_ga, ਆਕਾਰ(suggest_T), 10);
    ga_init2(&su->su_sga, ਆਕਾਰ(suggest_T), 10);
    ਜੇ (*badptr == NUL)
	ਵਾਪਸ;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    ਜੇ (badlen != 0)
	su->su_badlen = badlen;
    ਨਹੀਂਤਾਂ
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, ਨਿਰਾ, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    ਜੇ (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (ਖਾਲੀ)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    ਜੇ (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    ਲਈ (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curbuf->b_s.b_langp, i);
	ਜੇ (lp->lp_sallang != ਨਿਰਾ)
	{
	    su->su_sallang = lp->lp_sallang;
	    ਰੋਕੋ;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    ਜੇ (su->su_sallang != ਨਿਰਾ)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    ਜੇ (!SPELL_ISUPPER(c) && attr == HLF_ਗਿਣਤੀ)
    {
	make_case_ਸ਼ਬਦ(su->su_badword, buf, WF_ONECAP);
	ਜੋੜੋ_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    ਜੇ (banbadword)
	ਜੋੜੋ_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    ਜੇ (sps_copy == ਨਿਰਾ)
	ਵਾਪਸ;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    ਲਈ (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	ਜੇ (STRNCMP(buf, "expr:", 5) == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    ਜੇ (!ਵਿਅੰਜਕ_busy)
	    {
		ਵਿਅੰਜਕ_busy = TRUE;
		spell_suggest_ਵਿਅੰਜਕ(su, buf + 5);
		ਵਿਅੰਜਕ_busy = FALSE;
	    }
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((ਅੱਖਰ *)buf + 8);
	ਨਹੀਂਤਾਂ ਜੇ (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    ਜੇ (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    ਜੇ (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    ਸਥਿਰ ਖਾਲੀ
spell_suggest_ਵਿਅੰਜਕ(suginfo_T *su, char_u *ਵਿਅੰਜਕ)
{
    list_T	*list;
    listitem_T	*li;
    ਪੂਰਨ		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_ਵਿਅੰਜਕ(su->su_badword, ਵਿਅੰਜਕ);
    ਜੇ (list != ਨਿਰਾ)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, li)
	    ਜੇ (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		ਜੇ (score >= 0 && score <= su->su_maxscore)
		    ਜੋੜੋ_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (ਖਾਲੀ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#ਜੇ_ਅੰਤ

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    ਸਥਿਰ ਖਾਲੀ
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	ਸਤਰ[MAXWLEN * 2];
    char_u	*p;
    ਪੂਰਨ		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((ਅੱਖਰ *)fname, "r");
    ਜੇ (fd == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ਵਾਪਸ;
    }

    // Read it line by line.
    ਜਦੋਂ (!vim_fgets(ਸਤਰ, MAXWLEN * 2, fd) && !got_int)
    {
	ਸਤਰ_breakcheck();

	p = vim_strchr(ਸਤਰ, '/');
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਰੀ;	    // No Tab found, just skip the line.
	*p++ = NUL;
	ਜੇ (STRICMP(su->su_badword, ਸਤਰ) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    ਲਈ (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    ਜੇ (captype(p, ਨਿਰਾ) == 0)
	    {
		make_case_ਸ਼ਬਦ(p, cword, su->su_badflags);
		p = cword;
	    }

	    ਜੋੜੋ_suggestion(su, &su->su_ga, p, su->su_badlen,
				  SCORE_FILE, 0, TRUE, su->su_sallang, FALSE);
	}
    }

    fclose(fd);

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (ਖਾਲੀ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    ਸਥਿਰ ਖਾਲੀ
spell_suggest_intern(suginfo_T *su, ਪੂਰਨ interactive)
{
    // Load the .sug file(s) that are available and not done yet.
    suggest_ਲੱਦੋ_files();

    // 1. Try special cases, such as repeating a word: "the the" -> "the".
    //
    // Set a maximum score to limit the combination of operations that is
    // tried.
    suggest_try_special(su);

    // 2. Try inserting/deleting/swapping/changing a letter, use REP entries
    //    from the .aff file and inserting a space (split the word).
    suggest_try_change(su);

    // For the resulting top-scorers compute the sound-a-like score.
    ਜੇ (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    ਜੇ ((sps_flags & SPS_FAST) == 0)
    {
	ਜੇ (sps_flags & SPS_ਉੱਤਮ)
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
	ਜੇ (su->su_ga.ga_len < SUG_CLEAN_ਗਿਣਤੀ(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    ਜੇ (su->su_ga.ga_len < SUG_CLEAN_ਗਿਣਤੀ(su))
	    {
		// Still didn't find enough matches, try again, allowing even
		// more changes to the soundfold word.
		su->su_maxscore = SCORE_SFMAX3;
		suggest_try_soundalike(su);
	    }
	}
	su->su_maxscore = su->su_sfmaxscore;
	suggest_try_soundalike_ਅੰਤ();
    }

    // When CTRL-C was hit while searching do show the results.  Only clear
    // got_int when using a command, not for spellsuggest().
    ui_breakcheck();
    ਜੇ (interactive && got_int)
    {
	(ਖਾਲੀ)vgetc();
	got_int = FALSE;
    }

    ਜੇ ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	ਜੇ (sps_flags & SPS_ਉੱਤਮ)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(ਖਾਲੀ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    ਸਥਿਰ ਖਾਲੀ
spell_ਲੱਭੋ_cleanup(suginfo_T *su)
{
    ਪੂਰਨ		i;

    // Free the suggestions.
    ਲਈ (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_ਸ਼ਬਦ);
    ga_clear(&su->su_ga);
    ਲਈ (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_ਸ਼ਬਦ);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    ਸਥਿਰ ਖਾਲੀ
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    ਪੂਰਨ		c;
    char_u	ਸ਼ਬਦ[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    ਜੇ (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
    {
	// Include badflags: if the badword is onecap or allcap
	// use that for the goodword too: "The the" -> "The".
	c = su->su_fbadword[len];
	su->su_fbadword[len] = NUL;
	make_case_ਸ਼ਬਦ(su->su_fbadword, ਸ਼ਬਦ, su->su_badflags);
	su->su_fbadword[len] = c;

	// Give a soundalike score of 0, compute the score as if deleting one
	// character.
	ਜੋੜੋ_suggestion(su, &su->su_ga, ਸ਼ਬਦ, su->su_badlen,
		       RESCORE(SCORE_REP, 0), 0, TRUE, su->su_sallang, FALSE);
    }
}

/*
 * Change the 0 to 1 to measure how much time is spent in each state.
 * Output is dumped in "suggestprof".
 */
#ਜੇ 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T ਕੁੱਲ;
proftime_T times[STATE_FINAL + 1];
ਲੰਮਾ counts[STATE_FINAL + 1];

    ਸਥਿਰ ਖਾਲੀ
prof_init(ਖਾਲੀ)
{
    ਲਈ (ਪੂਰਨ i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_ਸ਼ੁਰੂ(&current);
    profile_ਸ਼ੁਰੂ(&ਕੁੱਲ);
}

// call before changing state
    ਸਥਿਰ ਖਾਲੀ
prof_store(state_T state)
{
    profile_ਸਿਰਾ(&current);
    profile_ਜੋੜੋ(&times[state], &current);
    ++counts[state];
    profile_ਸ਼ੁਰੂ(&current);
}
# define PROF_STORE(state) prof_store(state);

    ਸਥਿਰ ਖਾਲੀ
prof_report(ਅੱਖਰ *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_ਸਿਰਾ(&ਕੁੱਲ);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_ਸੁਨੇਹਾ(&ਕੁੱਲ));
    ਲਈ (ਪੂਰਨ i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_ਸੁਨੇਹਾ(&times[i]), counts[i]);
    fclose(fd);
}
#ਨਹੀਂਤਾਂ
# define PROF_STORE(state)
#ਜੇ_ਅੰਤ

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    ਸਥਿਰ ਖਾਲੀ
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    ਪੂਰਨ		n;
    char_u	*p;
    ਪੂਰਨ		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (ਪੂਰਨ)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (ਖਾਲੀ)spell_casefold(curwin, p, (ਪੂਰਨ)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (ਪੂਰਨ)STRLEN(su->su_badptr);
    ਜੇ (n < MAXWLEN)
	fword[n] = NUL;

    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	ਜੇ (lp->lp_slang->sl_fbyts == ਨਿਰਾ)
	    ਜਾਰੀ;

	// Try it for this language.  Will add possible suggestions.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SUGGEST_PROFILE
	prof_init();
#ਜੇ_ਅੰਤ
	suggest_trie_walk(su, lp, fword, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SUGGEST_PROFILE
	prof_report("try_change");
#ਜੇ_ਅੰਤ
    }
}

// Check the maximum score, if we go over it we won't try this change.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, ਜੋੜੋ) \
       ((ਡੂੰਘਾਈ) < MAXWLEN - 1 && (ਢੇਰ)[ਡੂੰਘਾਈ].ts_score + (ਜੋੜੋ) < (su)->su_maxscore)

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
    ਸਥਿਰ ਖਾਲੀ
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    ਪੂਰਨ		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	ਢੇਰ[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    ਪੂਰਨ		newscore;
    ਪੂਰਨ		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    ਪੂਰਨ		c, c2, c3;
    ਪੂਰਨ		n = 0;
    ਪੂਰਨ		flags;
    garray_T	*gap;
    idx_T	arridx;
    ਪੂਰਨ		len;
    char_u	*p;
    fromto_T	*ftp;
    ਪੂਰਨ		fl = 0, tl;
    ਪੂਰਨ		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    ਪੂਰਨ		fword_ਸਿਰੇ;
    ਪੂਰਨ		goodword_ਸਿਰੇ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		breakcheckcount = 1000;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    proftime_T	ਸਮਾਂ_ਹੱਦਬੰਦੀ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    ਡੂੰਘਾਈ = 0;
    sp = &ਢੇਰ[0];
    CLEAR_ਸੂਚਕ(sp);
    sp->ts_curi = 1;

    ਜੇ (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = ਨਿਰਾ;
	pidxs = ਨਿਰਾ;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_ਸ਼ੁਰੂ;
    }
    ਨਹੀਂਤਾਂ
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	ਜੇ (pbyts != ਨਿਰਾ)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	ਨਹੀਂਤਾਂ
	{
	    byts = fbyts;
	    idxs = fidxs;
	    sp->ts_prefixdepth = PFD_NOPREFIX;
	    sp->ts_state = STATE_ਸ਼ੁਰੂ;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    // The loop may take an indefinite amount of time. Break out after some
    // time.
    ਜੇ (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &ਸਮਾਂ_ਹੱਦਬੰਦੀ);
#ਜੇ_ਅੰਤ

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    ਜਦੋਂ (ਡੂੰਘਾਈ >= 0 && !got_int)
    {
	sp = &ਢੇਰ[ਡੂੰਘਾਈ];
	ਚੋਣ (sp->ts_state)
	{
	ਹਾਲਤ STATE_ਸ਼ੁਰੂ:
	ਹਾਲਤ STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    ਜੇ (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		ਲਈ (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (ਪੂਰਨ)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		ਜੇ (ਡੂੰਘਾਈ < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (ਪੂਰਨ)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    ਜੇ (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    ਨਹੀਂਤਾਂ
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		    sprintf(changename[ਡੂੰਘਾਈ], "prefix");
#ਜੇ_ਅੰਤ
		    go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, 0);
		    ++ਡੂੰਘਾਈ;
		    sp = &ਢੇਰ[ਡੂੰਘਾਈ];
		    sp->ts_prefixdepth = ਡੂੰਘਾਈ - 1;
		    byts = fbyts;
		    idxs = fidxs;
		    sp->ts_arridx = 0;

		    // Move the prefix to preword[] with the right case
		    // and make find_keepcap_word() works.
		    tword[sp->ts_twordlen] = NUL;
		    make_case_ਸ਼ਬਦ(tword + sp->ts_splitoff,
					  preword + sp->ts_prewordlen, flags);
		    sp->ts_prewordlen = (char_u)STRLEN(preword);
		    sp->ts_splitoff = sp->ts_twordlen;
		}
		ਰੋਕੋ;
	    }

	    ਜੇ (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		ਰੋਕੋ;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (ਪੂਰਨ)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    ਜੇ (flags & WF_NOSUGGEST)
		ਰੋਕੋ;

	    fword_ਸਿਰੇ = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    ਜੇ (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != ਨਿਰਾ)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = ਢੇਰ[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		ਲਈ (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		ਜੇ (c > 0)
		{
		    c = valid_ਸ਼ਬਦ_ਅਗੇਤਰ(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    ਜੇ (c == 0)
			ਰੋਕੋ;

		    // Use the WF_RARE flag for a rare prefix.
		    ਜੇ (c & WF_RAREPFX)
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
	    ਜੇ (sp->ts_complen == sp->ts_compsplit && fword_ਸਿਰੇ
						     && (flags & WF_NEEDCOMP))
		goodword_ਸਿਰੇ = FALSE;
	    ਨਹੀਂਤਾਂ
		goodword_ਸਿਰੇ = TRUE;

	    p = ਨਿਰਾ;
	    compound_ok = TRUE;
	    ਜੇ (sp->ts_complen > sp->ts_compsplit)
	    {
		ਜੇ (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    ਜੇ (sp->ts_fidx - sp->ts_splitfidx
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
			ਜੇ (newscore <= su->su_maxscore)
			    ਜੋੜੋ_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    ਜੇ (((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			ਰੋਕੋ;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    ਜੇ (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			ਰੋਕੋ;

		    compflags[sp->ts_complen] = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    ਜੇ (ਮੇਲ_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    ਜੇ (compound_ok)
		    {
			p = preword;
			ਜਦੋਂ (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			ਜੇ (fword_ਸਿਰੇ && !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    // Compound is not allowed.  But it may still be
			    // possible if we add another (short) word.
			    compound_ok = FALSE;
		    }

		    // Get pointer to last char of previous word.
		    p = preword + sp->ts_prewordlen;
		    MB_PTR_ਪਿੱਠ(preword, p);
		}
	    }

	    // Form the word with proper case in preword.
	    // If there is a word from a previous split, append.
	    // For the soundfold tree don't change the case, simply append.
	    ਜੇ (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    ਨਹੀਂਤਾਂ ਜੇ (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		ਲੱਭੋ_keepcap_ਸ਼ਬਦ(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    ਨਹੀਂਤਾਂ
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		ਜੇ ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		ਜੇ (p != ਨਿਰਾ && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_ਸ਼ਬਦ(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    ਜੇ (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		ਜੇ (flags & WF_BANNED)
		{
		    ਜੋੜੋ_banned(su, preword + sp->ts_prewordlen);
		    ਰੋਕੋ;
		}
		ਜੇ ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    ਜੇ (slang->sl_compprog == ਨਿਰਾ)
			ਰੋਕੋ;
		    // the word so far was banned but we may try compounding
		    goodword_ਸਿਰੇ = FALSE;
		}
	    }

	    newscore = 0;
	    ਜੇ (!soundfold)	// soundfold words don't have flags
	    {
		ਜੇ ((flags & WF_REGION)
			    && (((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		ਜੇ (flags & WF_RARE)
		    newscore += SCORE_RARE;

		ਜੇ (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, ਨਿਰਾ)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    ਜੇ (fword_ਸਿਰੇ
		    && goodword_ਸਿਰੇ
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		ਜੇ (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    ਪੂਰਨ	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    ਲਈ (j = 0; j < ਡੂੰਘਾਈ; ++j)
			smsg("%s", changename[j]);
		}
#ਜੇ_ਅੰਤ
		ਜੇ (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    ਜੋੜੋ_sound_suggest(su, preword, sp->ts_score, lp);
		}
		ਨਹੀਂਤਾਂ ਜੇ (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_ਪਿੱਠ(fword, p);
		    ਜੇ (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_ਪਿੱਠ(preword, p);
			ਜੇ (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    ਜੇ (score <= su->su_maxscore)
		    {
			ਜੋੜੋ_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			ਜੇ (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, ਨਿਰਾ);
			    ਜੇ (c == 0 || c == WF_ALLCAP)
			    {
				make_case_ਸ਼ਬਦ(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen,
						      c == 0 ? WF_ALLCAP : 0);

				ਜੋੜੋ_suggestion(su, &su->su_ga, preword,
					sp->ts_fidx - repextra,
					score + SCORE_ICASE, 0, FALSE,
					lp->lp_sallang, FALSE);
			    }
			}
		    }
		}
	    }

	    // Try word split and/or compounding.
	    ਜੇ ((sp->ts_fidx >= sp->ts_fidxtry || fword_ਸਿਰੇ)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		ਪੂਰਨ	try_compound;
		ਪੂਰਨ	try_ਵੰਡੋ;

		// If past the end of the bad word don't try a split.
		// Otherwise try changing the next word.  E.g., find
		// suggestions for "the the" where the second "the" is
		// different.  It's done like a split.
		// TODO: word split for soundfold words
		try_ਵੰਡੋ = (sp->ts_fidx - repextra < su->su_badlen)
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
		ਜੇ (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != ਨਿਰਾ
			&& ((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24) != 0
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
					 compflags, ((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24))))

		{
		    try_compound = TRUE;
		    compflags[sp->ts_complen] = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		}

		// For NOBREAK we never try splitting, it won't make any word
		// valid.
		ਜੇ (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		ਨਹੀਂਤਾਂ ਜੇ (!fword_ਸਿਰੇ
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		ਨਹੀਂਤਾਂ
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		ਜੇ (try_ਵੰਡੋ || try_compound)
		{
		    ਜੇ (!try_compound && (!fword_ਸਿਰੇ || !goodword_ਸਿਰੇ))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			ਜੇ (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    ਰੋਕੋ;
			p = preword;
			ਜਦੋਂ (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			ਜੇ (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    ਰੋਕੋ;

			ਜੇ (slang->sl_nosplitsugs)
			    newscore += SCORE_ਵੰਡੋ_NO;
			ਨਹੀਂਤਾਂ
			    newscore += SCORE_ਵੰਡੋ;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    ਜੇ (TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, newscore))
		    {
			go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, newscore);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
			ਜੇ (!try_compound && !fword_ਸਿਰੇ)
			    sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			ਨਹੀਂਤਾਂ
			    sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#ਜੇ_ਅੰਤ
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++ਡੂੰਘਾਈ;
			sp = &ਢੇਰ[ਡੂੰਘਾਈ];

			// Append a space to preword when splitting.
			ਜੇ (!try_compound && !fword_ਸਿਰੇ)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			ਜੇ (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_ਸਿਰੇ)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_ਸਿਰੇ)
			{
			    ਪੂਰਨ	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    ਜੇ (fword_ਸਿਰੇ)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    ਨਹੀਂਤਾਂ
				sp->ts_score -= SCORE_ਵੰਡੋ - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			ਜੇ (try_compound)
			    ++sp->ts_complen;
			ਨਹੀਂਤਾਂ
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			ਜੇ (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			ਨਹੀਂਤਾਂ
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			ਜੇ (pbyts != ਨਿਰਾ)
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
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_ਸ਼ੁਰੂ;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    ਜੇ (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		ਰੋਕੋ;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	ਹਾਲਤ STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    ਜੇ (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		ਜੇ (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		ਨਹੀਂਤਾਂ
		    sp->ts_state = STATE_FINAL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		ਜੇ (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		ਨਹੀਂਤਾਂ
		    newscore = SCORE_SUBST;
		ਜੇ ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, newscore))
		{
		    go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, newscore);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		    ਜੇ (newscore > 0)
			sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    ਨਹੀਂਤਾਂ
			sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#ਜੇ_ਅੰਤ
		    ++ਡੂੰਘਾਈ;
		    sp = &ਢੇਰ[ਡੂੰਘਾਈ];
		    ਜੇ (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    ਜੇ (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    ਜੇ (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			ਜੇ (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			ਨਹੀਂਤਾਂ ਜੇ (sp->ts_isdiff == DIFF_ਪਾਓ
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			ਜੇ (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    ਜੇ (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				ਜੇ (enc_utf8
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
				ਨਹੀਂਤਾਂ ਜੇ (!soundfold
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
			    ਨਹੀਂਤਾਂ ਜੇ (sp->ts_isdiff == DIFF_ਪਾਓ
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				ਜੇ (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				ਨਹੀਂਤਾਂ
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_ਪਿੱਠ(tword, p);
				    ਜੇ (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			ਜੇ (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    ਜੇ (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ਰੋਕੋ;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    ਜੇ (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    ਨਹੀਂਤਾਂ
		newscore = SCORE_DEL;
	    ਜੇ (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, newscore))
	    {
		go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, newscore);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#ਜੇ_ਅੰਤ
		++ਡੂੰਘਾਈ;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		ਢੇਰ[ਡੂੰਘਾਈ].ts_flags |= TSF_DIDDEL;
		ਢੇਰ[ਡੂੰਘਾਈ].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		ਜੇ (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    ਜੇ (enc_utf8 && utf_iscomposing(c))
			ਢੇਰ[ਡੂੰਘਾਈ].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    ਨਹੀਂਤਾਂ ਜੇ (c == mb_ptr2char(fword + ਢੇਰ[ਡੂੰਘਾਈ].ts_fidx))
			ਢੇਰ[ਡੂੰਘਾਈ].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ਨਹੀਂਤਾਂ
		{
		    ++ਢੇਰ[ਡੂੰਘਾਈ].ts_fidx;
		    ਜੇ (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			ਢੇਰ[ਡੂੰਘਾਈ].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ਰੋਕੋ;
	    }
	    // FALLTHROUGH

	ਹਾਲਤ STATE_INS_PREP:
	    ਜੇ (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ਅਦਲਾ_ਬਦਲੀ;
		ਰੋਕੋ;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    ਲਈ (;;)
	    {
		ਜੇ (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_ਅਦਲਾ_ਬਦਲੀ;
		    ਰੋਕੋ;
		}
		ਜੇ (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    ਰੋਕੋ;
		}
		++sp->ts_curi;
	    }
	    ਰੋਕੋ;

	    // FALLTHROUGH

	ਹਾਲਤ STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    ਜੇ (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ਅਦਲਾ_ਬਦਲੀ;
		ਰੋਕੋ;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    ਜੇ (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		ਰੋਕੋ;
	    }
	    c = byts[n];
	    ਜੇ (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    ਨਹੀਂਤਾਂ
		newscore = SCORE_INS;
	    ਜੇ (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, newscore))
	    {
		go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, newscore);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#ਜੇ_ਅੰਤ
		++ਡੂੰਘਾਈ;
		sp = &ਢੇਰ[ਡੂੰਘਾਈ];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		ਜੇ (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    ਜੇ (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_ਪਾਓ;
		    }
		}
		ਨਹੀਂਤਾਂ
		    fl = 1;
		ਜੇ (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    ਜੇ (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_ਅਦਲਾ_ਬਦਲੀ:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    ਜੇ (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ਰੋਕੋ;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    ਜੇ (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ਰੋਕੋ;
	    }

	    ਜੇ (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		ਜੇ (p[n] == NUL)
		    c2 = NUL;
		ਨਹੀਂਤਾਂ ਜੇ (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		ਨਹੀਂਤਾਂ
		    c2 = mb_ptr2char(p + n);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (p[1] == NUL)
		    c2 = NUL;
		ਨਹੀਂਤਾਂ ਜੇ (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		ਨਹੀਂਤਾਂ
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    ਜੇ (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ਰੋਕੋ;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    ਜੇ (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		ਰੋਕੋ;
	    }
	    ਜੇ (c2 != NUL && TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, SCORE_ਅਦਲਾ_ਬਦਲੀ))
	    {
		go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, SCORE_ਅਦਲਾ_ਬਦਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#ਜੇ_ਅੰਤ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++ਡੂੰਘਾਈ;
		ਜੇ (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		ਨਹੀਂਤਾਂ
		{
		    p[0] = c2;
		    p[1] = c;
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    ਜੇ (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	ਹਾਲਤ STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    ਜੇ (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		ਜੇ (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		ਨਹੀਂਤਾਂ
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		c = *p;
		c2 = p[1];
		ਜੇ (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		ਨਹੀਂਤਾਂ
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    ਜੇ (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ਰੋਕੋ;
	    }
	    ਜੇ (TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, SCORE_SWAP3))
	    {
		go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, SCORE_SWAP3);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#ਜੇ_ਅੰਤ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++ਡੂੰਘਾਈ;
		ਜੇ (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		ਨਹੀਂਤਾਂ
		{
		    p[0] = p[2];
		    p[2] = c;
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    ਜੇ (has_mbyte)
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
	    ਨਹੀਂਤਾਂ
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    ਜੇ (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ਰੋਕੋ;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    ਜੇ (TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, SCORE_SWAP3))
	    {
		go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, SCORE_SWAP3);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: rotate left %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#ਜੇ_ਅੰਤ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3L;
		++ਡੂੰਘਾਈ;
		p = fword + sp->ts_fidx;
		ਜੇ (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		ਨਹੀਂਤਾਂ
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    ਜੇ (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    ਜੇ (TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, SCORE_SWAP3))
	    {
		go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, SCORE_SWAP3);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: rotate right %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#ਜੇ_ਅੰਤ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3R;
		++ਡੂੰਘਾਈ;
		p = fword + sp->ts_fidx;
		ਜੇ (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		ਨਹੀਂਤਾਂ
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    ਜੇ (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	ਹਾਲਤ STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    ਜੇ ((lp->lp_replang == ਨਿਰਾ && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ਰੋਕੋ;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    ਜੇ (soundfold)
		sp->ts_curi = slang->sl_repsal_ਪਹਿਲਾ[fword[sp->ts_fidx]];
	    ਨਹੀਂਤਾਂ
		sp->ts_curi = lp->lp_replang->sl_rep_ਪਹਿਲਾ[fword[sp->ts_fidx]];

	    ਜੇ (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ਰੋਕੋ;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	ਹਾਲਤ STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    ਜੇ (soundfold)
		gap = &slang->sl_repsal;
	    ਨਹੀਂਤਾਂ
		gap = &lp->lp_replang->sl_rep;
	    ਜਦੋਂ (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_ਅੰਕੜੇ + sp->ts_curi++;
		ਜੇ (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    ਰੋਕੋ;
		}
		ਜੇ (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, ਢੇਰ, ਡੂੰਘਾਈ, SCORE_REP))
		{
		    go_deeper(ਢੇਰ, ਡੂੰਘਾਈ, SCORE_REP);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TRIEWALK
		    sprintf(changename[ਡੂੰਘਾਈ], "%.*s-%s: replace %s with %s",
			    sp->ts_twordlen, tword, fword + sp->ts_fidx,
			    ftp->ft_from, ftp->ft_ਮੰਜ਼ਿਲ);
#ਜੇ_ਅੰਤ
		    // Need to undo this afterwards.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_REP_UNDO;

		    // Change the "from" to the "to" string.
		    ++ਡੂੰਘਾਈ;
		    fl = (ਪੂਰਨ)STRLEN(ftp->ft_from);
		    tl = (ਪੂਰਨ)STRLEN(ftp->ft_ਮੰਜ਼ਿਲ);
		    ਜੇ (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_ਮੰਜ਼ਿਲ, tl);
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_fidxtry = sp->ts_fidx + tl;
		    ਢੇਰ[ਡੂੰਘਾਈ].ts_tcharlen = 0;
		    ਰੋਕੋ;
		}
	    }

	    ਜੇ (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    ਰੋਕੋ;

	ਹਾਲਤ STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    ਜੇ (soundfold)
		gap = &slang->sl_repsal;
	    ਨਹੀਂਤਾਂ
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_ਅੰਕੜੇ + sp->ts_curi - 1;
	    fl = (ਪੂਰਨ)STRLEN(ftp->ft_from);
	    tl = (ਪੂਰਨ)STRLEN(ftp->ft_ਮੰਜ਼ਿਲ);
	    p = fword + sp->ts_fidx;
	    ਜੇ (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    ਰੋਕੋ;

	ਮੂਲ:
	    // Did all possible states at this level, go up one level.
	    --ਡੂੰਘਾਈ;

	    ਜੇ (ਡੂੰਘਾਈ >= 0 && ਢੇਰ[ਡੂੰਘਾਈ].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    ਜੇ (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
		ਜੇ (spell_suggest_timeout > 0
					  && profile_passed_ਹੱਦਬੰਦੀ(&ਸਮਾਂ_ਹੱਦਬੰਦੀ))
		    got_int = TRUE;
#ਜੇ_ਅੰਤ
	    }
	}
    }
}


/*
 * Go one level deeper in the tree.
 */
    ਸਥਿਰ ਖਾਲੀ
go_deeper(trystate_T *ਢੇਰ, ਪੂਰਨ ਡੂੰਘਾਈ, ਪੂਰਨ score_ਜੋੜੋ)
{
    ਢੇਰ[ਡੂੰਘਾਈ + 1] = ਢੇਰ[ਡੂੰਘਾਈ];
    ਢੇਰ[ਡੂੰਘਾਈ + 1].ts_state = STATE_ਸ਼ੁਰੂ;
    ਢੇਰ[ਡੂੰਘਾਈ + 1].ts_score = ਢੇਰ[ਡੂੰਘਾਈ].ts_score + score_ਜੋੜੋ;
    ਢੇਰ[ਡੂੰਘਾਈ + 1].ts_curi = 1;	// start just after length byte
    ਢੇਰ[ਡੂੰਘਾਈ + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_keepcap_ਸ਼ਬਦ(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    ਪੂਰਨ		ਗੇੜਾ[MAXWLEN];
    ਪੂਰਨ		fwordidx[MAXWLEN];
    ਪੂਰਨ		uwordidx[MAXWLEN];
    ਪੂਰਨ		kwordlen[MAXWLEN];

    ਪੂਰਨ		flen, ulen;
    ਪੂਰਨ		l;
    ਪੂਰਨ		len;
    ਪੂਰਨ		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    ਜੇ (byts == ਨਿਰਾ)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	ਵਾਪਸ;
    }

    // Make an all-cap version of "fword".
    allcap_copy(fword, uword);

    // Each character needs to be tried both case-folded and upper-case.
    // All this gets very complicated if we keep in mind that changing case
    // may change the byte length of a multi-byte character...
    ਡੂੰਘਾਈ = 0;
    arridx[0] = 0;
    ਗੇੜਾ[0] = 0;
    fwordidx[0] = 0;
    uwordidx[0] = 0;
    kwordlen[0] = 0;
    ਜਦੋਂ (ਡੂੰਘਾਈ >= 0)
    {
	ਜੇ (fword[fwordidx[ਡੂੰਘਾਈ]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    ਜੇ (byts[arridx[ਡੂੰਘਾਈ] + 1] == 0)
	    {
		kword[kwordlen[ਡੂੰਘਾਈ]] = NUL;
		ਵਾਪਸ;
	    }

	    // kword is getting too long, continue one level up
	    --ਡੂੰਘਾਈ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (++ਗੇੜਾ[ਡੂੰਘਾਈ] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --ਡੂੰਘਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    ਜੇ (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[ਡੂੰਘਾਈ]);
		ulen = MB_CPTR2LEN(uword + uwordidx[ਡੂੰਘਾਈ]);
	    }
	    ਨਹੀਂਤਾਂ
		ulen = flen = 1;
	    ਜੇ (ਗੇੜਾ[ਡੂੰਘਾਈ] == 1)
	    {
		p = fword + fwordidx[ਡੂੰਘਾਈ];
		l = flen;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		p = uword + uwordidx[ਡੂੰਘਾਈ];
		l = ulen;
	    }

	    ਲਈ (tryidx = arridx[ਡੂੰਘਾਈ]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
		ਜਦੋਂ (lo < hi)
		{
		    m = (lo + hi) / 2;
		    ਜੇ (byts[m] > c)
			hi = m - 1;
		    ਨਹੀਂਤਾਂ ਜੇ (byts[m] < c)
			lo = m + 1;
		    ਨਹੀਂਤਾਂ
		    {
			lo = hi = m;
			ਰੋਕੋ;
		    }
		}

		// Stop if there is no matching byte.
		ਜੇ (hi < lo || byts[lo] != c)
		    ਰੋਕੋ;

		// Continue at the child (if there is one).
		tryidx = idxs[lo];
	    }

	    ਜੇ (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		ਜੇ (ਗੇੜਾ[ਡੂੰਘਾਈ] == 1)
		{
		    STRNCPY(kword + kwordlen[ਡੂੰਘਾਈ], fword + fwordidx[ਡੂੰਘਾਈ],
									flen);
		    kwordlen[ਡੂੰਘਾਈ + 1] = kwordlen[ਡੂੰਘਾਈ] + flen;
		}
		ਨਹੀਂਤਾਂ
		{
		    STRNCPY(kword + kwordlen[ਡੂੰਘਾਈ], uword + uwordidx[ਡੂੰਘਾਈ],
									ulen);
		    kwordlen[ਡੂੰਘਾਈ + 1] = kwordlen[ਡੂੰਘਾਈ] + ulen;
		}
		fwordidx[ਡੂੰਘਾਈ + 1] = fwordidx[ਡੂੰਘਾਈ] + flen;
		uwordidx[ਡੂੰਘਾਈ + 1] = uwordidx[ਡੂੰਘਾਈ] + ulen;

		++ਡੂੰਘਾਈ;
		arridx[ਡੂੰਘਾਈ] = tryidx;
		ਗੇੜਾ[ਡੂੰਘਾਈ] = 0;
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
    ਸਥਿਰ ਖਾਲੀ
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    ਪੂਰਨ		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    ਪੂਰਨ		score;
    ਪੂਰਨ		lpi;

    ਜੇ (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	ਵਾਪਸ;

    // Use the sound-folding of the first language that supports it.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	ਜੇ (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    ਲਈ (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		ਜੇ (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_ਸ਼ਬਦ = vim_strsave(stp->st_ਸ਼ਬਦ);
		    ਜੇ (sstp->st_ਸ਼ਬਦ != ਨਿਰਾ)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    ਰੋਕੋ;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    ਸਥਿਰ ਖਾਲੀ
score_combine(suginfo_T *su)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		lpi;
    slang_T	*slang = ਨਿਰਾ;

    // Add the alternate score to su_ga.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	ਜੇ (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    ਲਈ (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		ਜੇ (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		ਨਹੀਂਤਾਂ
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    ਰੋਕੋ;
	}
    }

    ਜੇ (slang == ਨਿਰਾ)	// Using "double" without sound folding.
    {
	(ਖਾਲੀ)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	ਵਾਪਸ;
    }

    // Add the alternate score to su_sga.
    ਲਈ (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_ਸ਼ਬਦ);
	ਜੇ (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	ਨਹੀਂਤਾਂ
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (ਖਾਲੀ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (ਖਾਲੀ)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, ਆਕਾਰ(suginfo_T), 1);
    ਜੇ (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	ਵਾਪਸ;

    stp = &SUG(ga, 0);
    ਲਈ (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	{
	    gap = ਗੇੜਾ == 1 ? &su->su_ga : &su->su_sga;
	    ਜੇ (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_ਸ਼ਬਦ;
		ਲਈ (j = 0; j < ga.ga_len; ++j)
		    ਜੇ (STRCMP(stp[j].st_ਸ਼ਬਦ, p) == 0)
			ਰੋਕੋ;
		ਜੇ (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		ਨਹੀਂਤਾਂ
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    ਜੇ (ga.ga_len > su->su_maxcount)
    {
	ਲਈ (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_ਸ਼ਬਦ);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    ਸਥਿਰ ਪੂਰਨ
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
    ਪੂਰਨ		lendiff;

    lendiff = (ਪੂਰਨ)(su->su_badlen - stp->st_orglen);
    ਜੇ (lendiff >= 0)
	pbad = badsound;
    ਨਹੀਂਤਾਂ
    {
	// soundfold the bad word with more characters following
	(ਖਾਲੀ)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	ਜੇ (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_ਸ਼ਬਦ) == NUL)
	    ਲਈ (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    ਜੇ (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_ਸ਼ਬਦ);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    ਨਹੀਂਤਾਂ
	pgood = stp->st_ਸ਼ਬਦ;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    ਵਾਪਸ soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਛੋਟਾ	sft_score;	// lowest score used
    char_u	sft_ਸ਼ਬਦ[1];    // soundfolded word, actually longer
} sftword_T;

ਸਥਿਰ sftword_T dumsft;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_ਸ਼ਬਦ - (char_u *)&dumsft)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2SFT(hi)     HIKEY2SFT((hi)->hi_ਕੁੰਜੀ)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    ਸਥਿਰ ਖਾਲੀ
suggest_try_soundalike_prep(ਖਾਲੀ)
{
    langp_T	*lp;
    ਪੂਰਨ		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ਜੇ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ਨਿਰਾ)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    ਸਥਿਰ ਖਾਲੀ
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    ਪੂਰਨ		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ਜੇ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ਨਿਰਾ)
	{
	    // soundfold the bad word
	    spell_soundfold(slang, su->su_fbadword, TRUE, salword);

	    // try all kinds of inserts/deletes/swaps/etc.
	    // TODO: also soundfold the next words, so that we can try joining
	    // and splitting
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SUGGEST_PROFILE
	    prof_init();
#ਜੇ_ਅੰਤ
	    suggest_trie_walk(su, lp, salword, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SUGGEST_PROFILE
	    prof_report("soundalike");
#ਜੇ_ਅੰਤ
	}
    }
}

/*
 * Finish up after calling suggest_try_soundalike().
 */
    ਸਥਿਰ ਖਾਲੀ
suggest_try_soundalike_ਅੰਤ(ਖਾਲੀ)
{
    langp_T	*lp;
    ਪੂਰਨ		lpi;
    slang_T	*slang;
    ਪੂਰਨ		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ਜੇ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ਨਿਰਾ)
	{
	    // Free the info about handled words.
	    todo = (ਪੂਰਨ)slang->sl_sounddone.ht_ਵਰਤਿਆ;
	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&slang->sl_sounddone, hi, todo)
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
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
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    ਪੂਰਨ		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    ਪੂਰਨ		sfwordnr;
    char_u	*nrline;
    ਪੂਰਨ		orgnr;
    char_u	theword[MAXWLEN];
    ਪੂਰਨ		i;
    ਪੂਰਨ		wlen;
    char_u	*byts;
    idx_T	*idxs;
    ਪੂਰਨ		n;
    ਪੂਰਨ		wordcount;
    ਪੂਰਨ		wc;
    ਪੂਰਨ		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    ਪੂਰਨ		bc, gc;
    ਪੂਰਨ		ਹੱਦਬੰਦੀ;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_ਸ਼ਬਦ) + STRLEN(goodword) + 1);
	ਜੇ (sft != ਨਿਰਾ)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_ਸ਼ਬਦ, goodword);
	    hash_ਜੋੜੋ_ਵਸਤੂ(&slang->sl_sounddone, hi, sft->sft_ਸ਼ਬਦ, hash);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	sft = HI2SFT(hi);
	ਜੇ (score >= sft->sft_score)
	    ਵਾਪਸ;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_ਲੱਭੋ(slang, goodword);
    ਜੇ (sfwordnr < 0)
    {
	ਅੰਦਰੂਨੀ_error("add_sound_suggest()");
	ਵਾਪਸ;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    ਜਦੋਂ (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	ਲਈ (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    ਜੇ (wordcount == orgnr && byts[n + 1] == NUL)
		ਰੋਕੋ;	// found end of word

	    ਜੇ (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    ਲਈ ( ; byts[n + i] == NUL; ++i)
		ਜੇ (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    ਜਾਓ badword;
		}

	    // One of the siblings must have the word.
	    ਲਈ ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		ਜੇ (wordcount + wc > orgnr)
		    ਰੋਕੋ;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	ਲਈ (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    ਪੂਰਨ		flags = (ਪੂਰਨ)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    ਜੇ (flags & WF_NOSUGGEST)
		ਜਾਰੀ;

	    ਜੇ (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		ਲੱਭੋ_keepcap_ਸ਼ਬਦ(slang, theword, cword);
		p = cword;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		flags |= su->su_badflags;
		ਜੇ ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_ਸ਼ਬਦ(theword, cword, flags);
		    p = cword;
		}
		ਨਹੀਂਤਾਂ
		    p = theword;
	    }

	    // Add the suggestion.
	    ਜੇ (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		ਜੇ (score <= su->su_maxscore)
		    ਜੋੜੋ_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Add a penalty for words in another region.
		ਜੇ ((flags & WF_REGION)
			    && (((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		ਨਹੀਂਤਾਂ
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		ਜੇ (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    ਜੇ (!SPELL_ISUPPER(bc)
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
		ਹੱਦਬੰਦੀ = MAXSCORE(su->su_sfmaxscore - goodscore, score);
		ਜੇ (ਹੱਦਬੰਦੀ > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		ਨਹੀਂਤਾਂ
		    goodscore += spell_edit_score_ਹੱਦਬੰਦੀ(slang, su->su_badword,
								    p, ਹੱਦਬੰਦੀ);

		// When going over the limit don't bother to do the rest.
		ਜੇ (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    ਜੇ (goodscore <= su->su_sfmaxscore)
			ਜੋੜੋ_suggestion(su, &su->su_ga, p, su->su_badlen,
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
    ਸਥਿਰ ਪੂਰਨ
soundfold_ਲੱਭੋ(slang_T *slang, char_u *ਸ਼ਬਦ)
{
    idx_T	arridx = 0;
    ਪੂਰਨ		len;
    ਪੂਰਨ		wlen = 0;
    ਪੂਰਨ		c;
    char_u	*ptr = ਸ਼ਬਦ;
    char_u	*byts;
    idx_T	*idxs;
    ਪੂਰਨ		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    ਲਈ (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	ਜੇ (byts[arridx] == NUL)
	{
	    ਜੇ (c == NUL)
		ਰੋਕੋ;

	    // Skip over the zeros, there can be several.
	    ਜਦੋਂ (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    ਜੇ (len == 0)
		ਵਾਪਸ -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	ਜੇ (c == NUL)
	    ਵਾਪਸ -1;

	// Perform a binary search in the list of accepted bytes.
	ਜੇ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	ਜਦੋਂ (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    ਜੇ (--len == 0)	// end of the bytes, didn't find it
		ਵਾਪਸ -1;
	}
	ਜੇ (byts[arridx] != c)	// didn't find the byte
	    ਵਾਪਸ -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ਜੇ (c == ' ')
	    ਜਦੋਂ (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    ਵਾਪਸ wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    ਸਥਿਰ ਪੂਰਨ
similar_chars(slang_T *slang, ਪੂਰਨ c1, ਪੂਰਨ c2)
{
    ਪੂਰਨ		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    ਜੇ (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_ਲੱਭੋ(&slang->sl_map_hash, buf);
	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    m1 = 0;
	ਨਹੀਂਤਾਂ
	    m1 = mb_ptr2char(hi->hi_ਕੁੰਜੀ + STRLEN(hi->hi_ਕੁੰਜੀ) + 1);
    }
    ਨਹੀਂਤਾਂ
	m1 = slang->sl_map_array[c1];
    ਜੇ (m1 == 0)
	ਵਾਪਸ FALSE;


    ਜੇ (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_ਲੱਭੋ(&slang->sl_map_hash, buf);
	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    m2 = 0;
	ਨਹੀਂਤਾਂ
	    m2 = mb_ptr2char(hi->hi_ਕੁੰਜੀ + STRLEN(hi->hi_ਕੁੰਜੀ) + 1);
    }
    ਨਹੀਂਤਾਂ
	m2 = slang->sl_map_array[c2];

    ਵਾਪਸ m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    ਪੂਰਨ		badlenarg,	// len of bad word replaced with "goodword"
    ਪੂਰਨ		score,
    ਪੂਰਨ		altscore,
    ਪੂਰਨ		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    ਪੂਰਨ		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    ਪੂਰਨ		goodlen;	// len of goodword changed
    ਪੂਰਨ		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    ਪੂਰਨ		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    ਲਈ (;;)
    {
	goodlen = (ਪੂਰਨ)(pgood - goodword);
	badlen = (ਪੂਰਨ)(pbad - su->su_badptr);
	ਜੇ (goodlen <= 0 || badlen <= 0)
	    ਰੋਕੋ;
	MB_PTR_ਪਿੱਠ(goodword, pgood);
	MB_PTR_ਪਿੱਠ(su->su_badptr, pbad);
	ਜੇ (has_mbyte)
	{
	    ਜੇ (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*pgood != *pbad)
		ਰੋਕੋ;
    }

    ਜੇ (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	ਵਾਪਸ;

    ਜੇ (gap->ga_len == 0)
	i = -1;
    ਨਹੀਂਤਾਂ
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	ਲਈ (i = gap->ga_len; --i >= 0; ++stp)
	    ਜੇ (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_ਸ਼ਬਦ, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		ਜੇ (stp->st_slang == ਨਿਰਾ)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		ਜੇ (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    ਜੇ (had_bonus)
			rescore_one(su, stp);
		    ਨਹੀਂਤਾਂ
		    {
			new_sug.st_ਸ਼ਬਦ = stp->st_ਸ਼ਬਦ;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		ਜੇ (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		ਰੋਕੋ;
	    }
    }

    ਜੇ (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_ਸ਼ਬਦ = vim_strnsave(goodword, goodlen);
	ਜੇ (stp->st_ਸ਼ਬਦ != ਨਿਰਾ)
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
	    ਜੇ (gap->ga_len > SUG_MAX_ਗਿਣਤੀ(su))
	    {
		ਜੇ (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_ਗਿਣਤੀ(su));
		ਨਹੀਂਤਾਂ
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_ਗਿਣਤੀ(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    ਸਥਿਰ ਖਾਲੀ
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    ਪੂਰਨ		i;
    char_u	longword[MAXWLEN + 1];
    ਪੂਰਨ		len;
    hlf_T	attr;

    ਜੇ (gap->ga_len == 0)
	ਵਾਪਸ;
    stp = &SUG(*gap, 0);
    ਲਈ (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_ਸ਼ਬਦ, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_ਗਿਣਤੀ;
	(ਖਾਲੀ)spell_check(curwin, longword, &attr, ਨਿਰਾ, FALSE);
	ਜੇ (attr != HLF_ਗਿਣਤੀ)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_ਸ਼ਬਦ);
	    --gap->ga_len;
	    ਜੇ (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       ਆਕਾਰ(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_banned(
    suginfo_T	*su,
    char_u	*ਸ਼ਬਦ)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(ਸ਼ਬਦ);
    hi = hash_lookup(&su->su_banned, ਸ਼ਬਦ, hash);
    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))		// already present
	ਵਾਪਸ;
    s = vim_strsave(ਸ਼ਬਦ);
    ਜੇ (s != ਨਿਰਾ)
	hash_ਜੋੜੋ_ਵਸਤੂ(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    ਸਥਿਰ ਖਾਲੀ
rescore_suggestions(suginfo_T *su)
{
    ਪੂਰਨ		i;

    ਜੇ (su->su_sallang != ਨਿਰਾ)
	ਲਈ (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    ਸਥਿਰ ਖਾਲੀ
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    ਜੇ (slang != ਨਿਰਾ && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	ਜੇ (slang == su->su_sallang)
	    p = su->su_sal_badword;
	ਨਹੀਂਤਾਂ
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	ਜੇ (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

ਸਥਿਰ ਪੂਰਨ sug_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    ਸਥਿਰ ਪੂਰਨ
sug_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    ਪੂਰਨ		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    ਜੇ (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	ਜੇ (n == 0)
	    n = STRICMP(p1->st_ਸ਼ਬਦ, p2->st_ਸ਼ਬਦ);
    }
    ਵਾਪਸ n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    ਸਥਿਰ ਪੂਰਨ
cleanup_suggestions(
    garray_T	*gap,
    ਪੂਰਨ		maxscore,
    ਪੂਰਨ		keep)		// nr of suggestions to keep
{
    ਜੇ (gap->ga_len <= 0)
	ਵਾਪਸ maxscore;

    // Sort the list.
    qsort(gap->ga_ਅੰਕੜੇ, (size_t)gap->ga_len, ਆਕਾਰ(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    ਜੇ (gap->ga_len > keep)
    {
	ਪੂਰਨ		i;
	suggest_T   *stp = &SUG(*gap, 0);

	ਲਈ (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_ਸ਼ਬਦ);
	gap->ga_len = keep;
	ਜੇ (keep >= 1)
	    ਵਾਪਸ stp[keep - 1].st_score;
    }
    ਵਾਪਸ maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    ਸਥਿਰ ਪੂਰਨ
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    ਪੂਰਨ		goodlen;
    ਪੂਰਨ		badlen;
    ਪੂਰਨ		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    ਪੂਰਨ		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    ਜੇ ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	ਜੇ ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    ਵਾਪਸ SCORE_DEL;
	ਜੇ (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    ਵਾਪਸ SCORE_MAXMAX;

	ਜੇ (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	ਨਹੀਂਤਾਂ
	{
	    score = 2 * SCORE_DEL / 3;
	    ਜੇ (*badsound == '*')
		++badsound;
	    ਨਹੀਂਤਾਂ
		++goodsound;
	}
    }

    goodlen = (ਪੂਰਨ)STRLEN(goodsound);
    badlen = (ਪੂਰਨ)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    ਜੇ (n < -2 || n > 2)
	ਵਾਪਸ SCORE_MAXMAX;

    ਜੇ (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    ਨਹੀਂਤਾਂ
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    ਜਦੋਂ (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    ਚੋਣ (n)
    {
	ਹਾਲਤ -2:
	ਹਾਲਤ 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    ਜਦੋਂ (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    ਜੇ (STRCMP(pl + 1, ps) == 0)
		ਵਾਪਸ score + SCORE_DEL * 2;

	    // Failed to compare.
	    ਰੋਕੋ;

	ਹਾਲਤ -1:
	ਹਾਲਤ 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    ਜਦੋਂ (*pl2 == *ps2)
	    {
		ਜੇ (*pl2 == NUL)	// reached the end
		    ਵਾਪਸ score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    ਜੇ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		ਵਾਪਸ score + SCORE_DEL + SCORE_ਅਦਲਾ_ਬਦਲੀ;

	    // 3: delete then substitute, then the rest must be equal
	    ਜੇ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		ਵਾਪਸ score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    ਜੇ (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		ਜਦੋਂ (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		ਜੇ (STRCMP(pl2 + 1, ps2) == 0)
		    ਵਾਪਸ score + SCORE_ਅਦਲਾ_ਬਦਲੀ + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    ਜਦੋਂ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    ਜੇ (STRCMP(pl2 + 1, ps2) == 0)
		ਵਾਪਸ score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    ਰੋਕੋ;

	ਹਾਲਤ 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    ਜੇ (*pl == NUL)
		ਵਾਪਸ score;

	    // 2: swap
	    ਜੇ (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		ਜਦੋਂ (*pl2 == *ps2)
		{
		    ਜੇ (*pl2 == NUL)	// reached the end
			ਵਾਪਸ score + SCORE_ਅਦਲਾ_ਬਦਲੀ;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		ਜੇ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    ਵਾਪਸ score + SCORE_ਅਦਲਾ_ਬਦਲੀ + SCORE_ਅਦਲਾ_ਬਦਲੀ;

		// 4: swap and substitute
		ਜੇ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    ਵਾਪਸ score + SCORE_ਅਦਲਾ_ਬਦਲੀ + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    ਜਦੋਂ (*pl2 == *ps2)
	    {
		ਜੇ (*pl2 == NUL)	// reached the end
		    ਵਾਪਸ score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    ਜੇ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		ਵਾਪਸ score + SCORE_SUBST + SCORE_ਅਦਲਾ_ਬਦਲੀ;

	    // 7: substitute and substitute
	    ਜੇ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		ਵਾਪਸ score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    ਜਦੋਂ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    ਜੇ (STRCMP(pl2 + 1, ps2) == 0)
		ਵਾਪਸ score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    ਜਦੋਂ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    ਜੇ (STRCMP(pl2, ps2 + 1) == 0)
		ਵਾਪਸ score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    ਰੋਕੋ;
    }

    ਵਾਪਸ SCORE_MAXMAX;
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
    ਸਥਿਰ ਪੂਰਨ
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    ਪੂਰਨ		*cnt;
    ਪੂਰਨ		badlen, goodlen;	// lengths including NUL
    ਪੂਰਨ		j, i;
    ਪੂਰਨ		t;
    ਪੂਰਨ		bc, gc;
    ਪੂਰਨ		pbc, pgc;
    char_u	*p;
    ਪੂਰਨ		wbadword[MAXWLEN];
    ਪੂਰਨ		wgoodword[MAXWLEN];

    ਜੇ (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	ਲਈ (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	ਲਈ (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	badlen = (ਪੂਰਨ)STRLEN(badword) + 1;
	goodlen = (ਪੂਰਨ)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(ਪੂਰਨ, (badlen + 1) * (goodlen + 1));
    ਜੇ (cnt == ਨਿਰਾ)
	ਵਾਪਸ 0;	// out of memory

    CNT(0, 0) = 0;
    ਲਈ (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    ਲਈ (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	ਲਈ (j = 1; j <= goodlen; ++j)
	{
	    ਜੇ (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    ਨਹੀਂਤਾਂ
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    ਜੇ (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    ਨਹੀਂਤਾਂ
	    {
		// Use a better score when there is only a case difference.
		ਜੇ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		ਨਹੀਂਤਾਂ
		{
		    // For a similar character use SCORE_SIMILAR.
		    ਜੇ (slang != ਨਿਰਾ
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    ਨਹੀਂਤਾਂ
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		ਜੇ (i > 1 && j > 1)
		{
		    ਜੇ (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    ਨਹੀਂਤਾਂ
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    ਜੇ (bc == pgc && pbc == gc)
		    {
			t = SCORE_ਅਦਲਾ_ਬਦਲੀ + CNT(i - 2, j - 2);
			ਜੇ (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		ਜੇ (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		ਜੇ (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    ਵਾਪਸ i;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		badi;
    ਪੂਰਨ		goodi;
    ਪੂਰਨ		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    ਸਥਿਰ ਪੂਰਨ
spell_edit_score_ਹੱਦਬੰਦੀ(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    ਪੂਰਨ		ਹੱਦਬੰਦੀ)
{
    limitscore_T    ਢੇਰ[10];		// allow for over 3 * 2 edits
    ਪੂਰਨ		    stackidx;
    ਪੂਰਨ		    bi, gi;
    ਪੂਰਨ		    bi2, gi2;
    ਪੂਰਨ		    bc, gc;
    ਪੂਰਨ		    score;
    ਪੂਰਨ		    score_off;
    ਪੂਰਨ		    minscore;
    ਪੂਰਨ		    ਗੇੜਾ;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    ਜੇ (has_mbyte)
	ਵਾਪਸ spell_edit_score_ਹੱਦਬੰਦੀ_w(slang, badword, goodword, ਹੱਦਬੰਦੀ);

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
    minscore = ਹੱਦਬੰਦੀ + 1;

    ਲਈ (;;)
    {
	// Skip over an equal part, score remains the same.
	ਲਈ (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    ਜੇ (bc != gc)	// stop at a char that's different
		ਰੋਕੋ;
	    ਜੇ (bc == NUL)	// both words end
	    {
		ਜੇ (score < minscore)
		    minscore = score;
		ਜਾਓ ਕੱਢੋ;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	ਜੇ (gc == NUL)    // goodword ends, delete badword chars
	{
	    ਕਰੋ
	    {
		ਜੇ ((score += SCORE_DEL) >= minscore)
		    ਜਾਓ ਕੱਢੋ;	    // do next alternative
	    } ਜਦੋਂ (badword[++bi] != NUL);
	    minscore = score;
	}
	ਨਹੀਂਤਾਂ ਜੇ (bc == NUL) // badword ends, insert badword chars
	{
	    ਕਰੋ
	    {
		ਜੇ ((score += SCORE_INS) >= minscore)
		    ਜਾਓ ਕੱਢੋ;	    // do next alternative
	    } ਜਦੋਂ (goodword[++gi] != NUL);
	    minscore = score;
	}
	ਨਹੀਂਤਾਂ			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    ਲਈ (ਗੇੜਾ = 0; ਗੇੜਾ <= 1; ++ਗੇੜਾ)
	    {
		score_off = score + (ਗੇੜਾ == 0 ? SCORE_DEL : SCORE_INS);
		ਜੇ (score_off < minscore)
		{
		    ਜੇ (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - ਗੇੜਾ;
			gi2 = gi + ਗੇੜਾ;
			ਜਦੋਂ (goodword[gi2] == badword[bi2])
			{
			    ਜੇ (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				ਰੋਕੋ;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// try deleting/inserting a character later
			ਢੇਰ[stackidx].badi = bi + 1 - ਗੇੜਾ;
			ਢੇਰ[stackidx].goodi = gi + ਗੇੜਾ;
			ਢੇਰ[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    ਜੇ (score + SCORE_ਅਦਲਾ_ਬਦਲੀ < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		ਜੇ (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_ਅਦਲਾ_ਬਦਲੀ;
		    ਜਾਰੀ;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    ਜੇ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    ਨਹੀਂਤਾਂ
	    {
		// For a similar character use SCORE_SIMILAR.
		ਜੇ (slang != ਨਿਰਾ
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		ਨਹੀਂਤਾਂ
		    score += SCORE_SUBST;
	    }

	    ਜੇ (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		ਜਾਰੀ;
	    }
	}
ਕੱਢੋ:
	// Get here to try the next alternative, pop it from the stack.
	ਜੇ (stackidx == 0)		// stack is empty, finished
	    ਰੋਕੋ;

	// pop an item from the stack
	--stackidx;
	gi = ਢੇਰ[stackidx].goodi;
	bi = ਢੇਰ[stackidx].badi;
	score = ਢੇਰ[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    ਜੇ (minscore > ਹੱਦਬੰਦੀ)
	ਵਾਪਸ SCORE_MAXMAX;
    ਵਾਪਸ minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    ਸਥਿਰ ਪੂਰਨ
spell_edit_score_ਹੱਦਬੰਦੀ_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    ਪੂਰਨ		ਹੱਦਬੰਦੀ)
{
    limitscore_T    ਢੇਰ[10];		// allow for over 3 * 2 edits
    ਪੂਰਨ		    stackidx;
    ਪੂਰਨ		    bi, gi;
    ਪੂਰਨ		    bi2, gi2;
    ਪੂਰਨ		    bc, gc;
    ਪੂਰਨ		    score;
    ਪੂਰਨ		    score_off;
    ਪੂਰਨ		    minscore;
    ਪੂਰਨ		    ਗੇੜਾ;
    char_u	    *p;
    ਪੂਰਨ		    wbadword[MAXWLEN];
    ਪੂਰਨ		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    ਲਈ (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    ਲਈ (p = goodword; *p != NUL; )
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
    minscore = ਹੱਦਬੰਦੀ + 1;

    ਲਈ (;;)
    {
	// Skip over an equal part, score remains the same.
	ਲਈ (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    ਜੇ (bc != gc)	// stop at a char that's different
		ਰੋਕੋ;
	    ਜੇ (bc == NUL)	// both words end
	    {
		ਜੇ (score < minscore)
		    minscore = score;
		ਜਾਓ ਕੱਢੋ;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	ਜੇ (gc == NUL)    // goodword ends, delete badword chars
	{
	    ਕਰੋ
	    {
		ਜੇ ((score += SCORE_DEL) >= minscore)
		    ਜਾਓ ਕੱਢੋ;	    // do next alternative
	    } ਜਦੋਂ (wbadword[++bi] != NUL);
	    minscore = score;
	}
	ਨਹੀਂਤਾਂ ਜੇ (bc == NUL) // badword ends, insert badword chars
	{
	    ਕਰੋ
	    {
		ਜੇ ((score += SCORE_INS) >= minscore)
		    ਜਾਓ ਕੱਢੋ;	    // do next alternative
	    } ਜਦੋਂ (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	ਨਹੀਂਤਾਂ			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    ਲਈ (ਗੇੜਾ = 0; ਗੇੜਾ <= 1; ++ਗੇੜਾ)
	    {
		score_off = score + (ਗੇੜਾ == 0 ? SCORE_DEL : SCORE_INS);
		ਜੇ (score_off < minscore)
		{
		    ਜੇ (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - ਗੇੜਾ;
			gi2 = gi + ਗੇੜਾ;
			ਜਦੋਂ (wgoodword[gi2] == wbadword[bi2])
			{
			    ਜੇ (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				ਰੋਕੋ;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// try deleting a character from badword later
			ਢੇਰ[stackidx].badi = bi + 1 - ਗੇੜਾ;
			ਢੇਰ[stackidx].goodi = gi + ਗੇੜਾ;
			ਢੇਰ[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    ਜੇ (score + SCORE_ਅਦਲਾ_ਬਦਲੀ < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		ਜੇ (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_ਅਦਲਾ_ਬਦਲੀ;
		    ਜਾਰੀ;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    ਜੇ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    ਨਹੀਂਤਾਂ
	    {
		// For a similar character use SCORE_SIMILAR.
		ਜੇ (slang != ਨਿਰਾ
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		ਨਹੀਂਤਾਂ
		    score += SCORE_SUBST;
	    }

	    ਜੇ (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		ਜਾਰੀ;
	    }
	}
ਕੱਢੋ:
	// Get here to try the next alternative, pop it from the stack.
	ਜੇ (stackidx == 0)		// stack is empty, finished
	    ਰੋਕੋ;

	// pop an item from the stack
	--stackidx;
	gi = ਢੇਰ[stackidx].goodi;
	bi = ਢੇਰ[stackidx].badi;
	score = ਢੇਰ[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    ਜੇ (minscore > ਹੱਦਬੰਦੀ)
	ਵਾਪਸ SCORE_MAXMAX;
    ਵਾਪਸ minscore;
}
#ਜੇ_ਅੰਤ  // FEAT_SPELL
