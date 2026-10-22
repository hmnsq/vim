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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_SPELL)

/*
 * Use this to adjust the score after finding suggestions, based on the
 * suggested word sounding like the bad word.  This is much faster than doing
 * it for every possible suggestion.
 * Disadvantage: When "the" is typed as "hte" it sounds quite different ("@"
 * vs "ht") and goes down in the list.
 * Used when 'spellsuggest' is set to "best".
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RESCORE(ಶಬ್ದ_score, sound_score) ((3 * (ಶಬ್ದ_score) + (sound_score)) / 4)

/*
 * Do the opposite: based on a maximum end score and a known sound score,
 * compute the maximum word score that can be used.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAXSCORE(ಶಬ್ದ_score, sound_score) ((4 * (ಶಬ್ದ_score) - (sound_score)) / 3)

// only used for su_badflags
#ವ್ಯಾಖ್ಯಾನಿಸು WF_MIXCAP   0x20	// mix of upper and lower case: macaRONI

/*
 * Information used when looking for suggestions.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ suginfo_S
{
    garray_T	su_ga;		    // suggestions, contains "suggest_T"
    ಪೂರ್ಣಾಂಕ		su_maxcount;	    // max. number of suggestions displayed
    ಪೂರ್ಣಾಂಕ		su_maxscore;	    // maximum score for adding to su_ga
    ಪೂರ್ಣಾಂಕ		su_sfmaxscore;	    // idem, for when doing soundfold words
    garray_T	su_sga;		    // like su_ga, sound-folded scoring
    char_u	*su_badptr;	    // start of bad word in line
    ಪೂರ್ಣಾಂಕ		su_badlen;	    // length of detected bad word in line
    ಪೂರ್ಣಾಂಕ		su_badflags;	    // caps flags for bad word
    char_u	su_badword[MAXWLEN]; // bad word truncated at su_badlen
    char_u	su_fbadword[MAXWLEN]; // su_badword case-folded
    char_u	su_sal_badword[MAXWLEN]; // su_badword soundfolded
    hashtab_T	su_banned;	    // table with banned words
    slang_T	*su_sallang;	    // default language for sound folding
} suginfo_T;

// One word suggestion.  Used in "si_ga".
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ suggest_S
{
    char_u	*st_ಶಬ್ದ;	// suggested word, allocated string
    ಪೂರ್ಣಾಂಕ		st_wordlen;	// STRLEN(st_word)
    ಪೂರ್ಣಾಂಕ		st_orglen;	// length of replaced text
    ಪೂರ್ಣಾಂಕ		st_score;	// lower is better
    ಪೂರ್ಣಾಂಕ		st_altscore;	// used when st_score compares equal
    ಪೂರ್ಣಾಂಕ		st_salscore;	// st_score is for soundalike
    ಪೂರ್ಣಾಂಕ		st_had_bonus;	// bonus already included in score
    slang_T	*st_slang;	// language used for sound folding
} suggest_T;

#ವ್ಯಾಖ್ಯಾನಿಸು SUG(ga, i) (((suggest_T *)(ga).ga_ದತ್ತಾಂಶ)[i])

// TRUE if a word appears in the list of banned words.
#ವ್ಯಾಖ್ಯಾನಿಸು WAS_BANNED(su, ಶಬ್ದ) (!HASHITEM_ಖಾಲಿಗಳು(hash_ಕಂಡುಹಿಡಿ(&(su)->su_banned, ಶಬ್ದ)))

// Number of suggestions kept when cleaning up.  We need to keep more than
// what is displayed, because when rescore_suggestions() is called the score
// may change and wrong suggestions may be removed later.
#ವ್ಯಾಖ್ಯಾನಿಸು SUG_CLEAN_ಎಣಿಕೆ(su)    ((su)->su_maxcount < 130 ? 150 : (su)->su_maxcount + 20)

// Threshold for sorting and cleaning up suggestions.  Don't want to keep lots
// of suggestions that are not going to be displayed.
#ವ್ಯಾಖ್ಯಾನಿಸು SUG_MAX_ಎಣಿಕೆ(su)	(SUG_CLEAN_ಎಣಿಕೆ(su) + 50)

// score for various changes
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ವಿಭಜಿಸು	149	// split bad word
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ವಿಭಜಿಸು_NO	249	// split bad word with NOSPLITSUGS
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ICASE	52	// slightly different case
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_REGION	200	// word is for different region
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_RARE	180	// rare word
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_ಅದಲುಬದಲು	75	// swap two characters
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SWAP3	110	// swap two characters in three
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_REP	65	// REP replacement
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SUBST	93	// substitute a character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SIMILAR	33	// substitute a similar character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SUBCOMP	33	// substitute a composing character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_DEL	94	// delete a character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_DELDUP	66	// delete a duplicated character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_DELCOMP	28	// delete a composing character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_INS	96	// insert a character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_INSDUP	67	// insert a duplicate character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_INSCOMP	30	// insert a composing character
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_NONWORD	103	// change non-word to word char

#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_FILE	30	// suggestion from a file
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_MAXINIT	350	// Initial maximum score: higher == slower.
				// 350 allows for about three changes.

#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_COMMON1	30	// subtracted for words seen before
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_COMMON2	40	// subtracted for words often seen
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_COMMON3	50	// subtracted for words very often seen
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_THRES2	10	// word count threshold for COMMON2
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_THRES3	100	// word count threshold for COMMON3

// When trying changed soundfold words it becomes slow when trying more than
// two changes.  With less than two changes it's slightly faster but we miss a
// few good suggestions.  In rare cases we need to try three of four changes.
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SFMAX1	200	// maximum score for first try
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SFMAX2	300	// maximum score for second try
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_SFMAX3	400	// maximum score for third try

#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_BIG	(SCORE_INS * 3)	// big difference
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_MAXMAX	999999		// accept any score
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_LIMITMAX	350		// for spell_edit_score_limit()

// for spell_edit_score_limit() we need to know the minimum value of
// SCORE_ICASE, SCORE_SWAP, SCORE_DEL, SCORE_SIMILAR and SCORE_INS
#ವ್ಯಾಖ್ಯಾನಿಸು SCORE_EDIT_MIN	SCORE_SIMILAR

/*
 * For finding suggestions: At each node in the tree these states are tried:
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    STATE_ಆರಂಭ = 0,	// At start of node check for NUL bytes (goodword
			// ends); if badword ends there is a match, otherwise
			// try splitting word.
    STATE_NOPREFIX,	// try without prefix
    STATE_SPLITUNDO,	// Undo splitting.
    STATE_ENDNUL,	// Past NUL bytes at start of the node.
    STATE_PLAIN,	// Use each byte of the node.
    STATE_DEL,		// Delete a byte from the bad word.
    STATE_INS_PREP,	// Prepare for inserting bytes.
    STATE_INS,		// Insert a byte in the bad word.
    STATE_ಅದಲುಬದಲು,		// Swap two bytes.
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ trystate_S
{
    state_T	ts_state;	// state at this level, STATE_
    ಪೂರ್ಣಾಂಕ		ts_score;	// score
    idx_T	ts_arridx;	// index in tree array, start of node
    ಗಿಡ್ಡ	ts_curi;	// index in list of child nodes
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
#ವ್ಯಾಖ್ಯಾನಿಸು DIFF_NONE	0	// no different byte (yet)
#ವ್ಯಾಖ್ಯಾನಿಸು DIFF_YES	1	// different byte found
#ವ್ಯಾಖ್ಯಾನಿಸು DIFF_ಸೇರಿಸು	2	// inserting character

// values for ts_flags
#ವ್ಯಾಖ್ಯಾನಿಸು TSF_PREFIXOK	1	// already checked that prefix is OK
#ವ್ಯಾಖ್ಯಾನಿಸು TSF_DIDSPLIT	2	// tried split at this point
#ವ್ಯಾಖ್ಯಾನಿಸು TSF_DIDDEL	4	// did a delete, "ts_delidx" has index

// special values ts_prefixdepth
#ವ್ಯಾಖ್ಯಾನಿಸು PFD_NOPREFIX	0xff	// not using prefixes
#ವ್ಯಾಖ್ಯಾನಿಸು PFD_PREFIXTREE	0xfe	// walking through the prefix tree
#ವ್ಯಾಖ್ಯಾನಿಸು PFD_NOTSPECIAL	0xfd	// highest value that's not special

ಸ್ಥಿರ ದೀರ್ಘ spell_suggest_timeout = 5000;

ಸ್ಥಿರ ಶೂನ್ಯ spell_ಕಂಡುಹಿಡಿ_suggest(char_u *badptr, ಪೂರ್ಣಾಂಕ badlen, suginfo_T *su, ಪೂರ್ಣಾಂಕ maxcount, ಪೂರ್ಣಾಂಕ banbadword, ಪೂರ್ಣಾಂಕ need_cap, ಪೂರ್ಣಾಂಕ interactive);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ spell_suggest_ಅಭಿವ್ಯಕ್ತಿ(suginfo_T *su, char_u *ಅಭಿವ್ಯಕ್ತಿ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ spell_suggest_file(suginfo_T *su, char_u *fname);
ಸ್ಥಿರ ಶೂನ್ಯ spell_suggest_intern(suginfo_T *su, ಪೂರ್ಣಾಂಕ interactive);
ಸ್ಥಿರ ಶೂನ್ಯ spell_ಕಂಡುಹಿಡಿ_cleanup(suginfo_T *su);
ಸ್ಥಿರ ಶೂನ್ಯ suggest_try_special(suginfo_T *su);
ಸ್ಥಿರ ಶೂನ್ಯ suggest_try_change(suginfo_T *su);
ಸ್ಥಿರ ಶೂನ್ಯ suggest_trie_walk(suginfo_T *su, langp_T *lp, char_u *fword, ಪೂರ್ಣಾಂಕ soundfold);
ಸ್ಥಿರ ಶೂನ್ಯ go_deeper(trystate_T *ರಾಶಿ, ಪೂರ್ಣಾಂಕ ಆಳ, ಪೂರ್ಣಾಂಕ score_ಕೂಡಿಸು);
ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_keepcap_ಶಬ್ದ(slang_T *slang, char_u *fword, char_u *kword);
ಸ್ಥಿರ ಶೂನ್ಯ score_comp_sal(suginfo_T *su);
ಸ್ಥಿರ ಶೂನ್ಯ score_combine(suginfo_T *su);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stp_sal_score(suggest_T *stp, suginfo_T *su, slang_T *slang, char_u *badsound);
ಸ್ಥಿರ ಶೂನ್ಯ suggest_try_soundalike_prep(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ suggest_try_soundalike(suginfo_T *su);
ಸ್ಥಿರ ಶೂನ್ಯ suggest_try_soundalike_ಮುಕ್ತಾಯ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_sound_suggest(suginfo_T *su, char_u *goodword, ಪೂರ್ಣಾಂಕ score, langp_T *lp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ soundfold_ಕಂಡುಹಿಡಿ(slang_T *slang, char_u *ಶಬ್ದ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ similar_chars(slang_T *slang, ಪೂರ್ಣಾಂಕ c1, ಪೂರ್ಣಾಂಕ c2);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_suggestion(suginfo_T *su, garray_T *gap, char_u *goodword, ಪೂರ್ಣಾಂಕ badlen, ಪೂರ್ಣಾಂಕ score, ಪೂರ್ಣಾಂಕ altscore, ಪೂರ್ಣಾಂಕ had_bonus, slang_T *slang, ಪೂರ್ಣಾಂಕ maxsf);
ಸ್ಥಿರ ಶೂನ್ಯ check_suggestions(suginfo_T *su, garray_T *gap);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_banned(suginfo_T *su, char_u *ಶಬ್ದ);
ಸ್ಥಿರ ಶೂನ್ಯ rescore_suggestions(suginfo_T *su);
ಸ್ಥಿರ ಶೂನ್ಯ rescore_one(suginfo_T *su, suggest_T *stp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cleanup_suggestions(garray_T *gap, ಪೂರ್ಣಾಂಕ maxscore, ಪೂರ್ಣಾಂಕ keep);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ soundalike_score(char_u *goodsound, char_u *badsound);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spell_edit_score(slang_T *slang, char_u *badword, char_u *goodword);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spell_edit_score_ಎಲ್ಲೆ(slang_T *slang, char_u *badword, char_u *goodword, ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spell_edit_score_ಎಲ್ಲೆ_w(slang_T *slang, char_u *badword, char_u *goodword, ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ);

/*
 * Return TRUE when the sequence of flags in "compflags" plus "flag" can
 * possibly form a valid compounded word.  This also checks the COMPOUNDRULE
 * lines if they don't contain wildcards.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_be_compound(
    trystate_T	*sp,
    slang_T	*slang,
    char_u	*compflags,
    ಪೂರ್ಣಾಂಕ		flag)
{
    // If the flag doesn't appear in sl_compstartflags or sl_compallflags
    // then it can't possibly compound.
    ಆದರೆ (!byte_ಒಳ_str(sp->ts_complen == sp->ts_compsplit
		? slang->sl_compstartflags : slang->sl_compallflags, flag))
	ಹಿಂತಿರುಗಿ FALSE;

    // If there are no wildcards, we can check if the flags collected so far
    // possibly can form a match with COMPOUNDRULE patterns.  This only
    // makes sense when we have two or more words.
    ಆದರೆ (slang->sl_comprules != ಖಾಲಿ && sp->ts_complen > sp->ts_compsplit)
    {
	ಪೂರ್ಣಾಂಕ v;

	compflags[sp->ts_complen] = flag;
	compflags[sp->ts_complen + 1] = NUL;
	v = ಹೊಂದಿಕೆ_compoundrule(slang, compflags + sp->ts_compsplit);
	compflags[sp->ts_complen] = NUL;
	ಹಿಂತಿರುಗಿ v;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Adjust the score of common words.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
score_wordcount_adj(
    slang_T	*slang,
    ಪೂರ್ಣಾಂಕ		score,
    char_u	*ಶಬ್ದ,
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    ಪೂರ್ಣಾಂಕ		bonus;
    ಪೂರ್ಣಾಂಕ		newscore;

    hi = hash_ಕಂಡುಹಿಡಿ(&slang->sl_wordcount, ಶಬ್ದ);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ score;

    wc = HI2WC(hi);
    ಆದರೆ (wc->wc_ಎಣಿಕೆ < SCORE_THRES2)
	bonus = SCORE_COMMON1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wc->wc_ಎಣಿಕೆ < SCORE_THRES3)
	bonus = SCORE_COMMON2;
    ಇಲ್ಲದಿದ್ದರೆ
	bonus = SCORE_COMMON3;
    ಆದರೆ (ವಿಭಜಿಸು)
	newscore = score - bonus / 2;
    ಇಲ್ಲದಿದ್ದರೆ
	newscore = score - bonus;
    ಆದರೆ (newscore < 0)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ newscore;
}

/*
 * Like captype() but for a KEEPCAP word add ONECAP if the word starts with a
 * capital.  So that make_case_word() can turn WOrd into Word.
 * Add ALLCAP for "WOrD".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
badword_captype(char_u *ಶಬ್ದ, char_u *ಕೊನೆ)
{
    ಪೂರ್ಣಾಂಕ		flags = captype(ಶಬ್ದ, ಕೊನೆ);
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		l, u;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ;
    char_u	*p;

    ಆದರೆ (!(flags & WF_KEEPCAP))
	ಹಿಂತಿರುಗಿ flags;

    // Count the number of UPPER and lower case letters.
    l = u = 0;
    ಮೊದಲ = FALSE;
    ಗಾಗಿ (p = ಶಬ್ದ; p < ಕೊನೆ; MB_PTR_ADV(p))
    {
	c = PTR2CHAR(p);
	ಆದರೆ (SPELL_ISUPPER(c))
	{
	    ++u;
	    ಆದರೆ (p == ಶಬ್ದ)
		ಮೊದಲ = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ++l;
    }

    // If there are more UPPER than lower case letters suggest an
    // ALLCAP word.  Otherwise, if the first letter is UPPER then
    // suggest ONECAP.  Exception: "ALl" most likely should be "All",
    // require three upper case letters.
    ಆದರೆ (u > l && u > 2)
	flags |= WF_ALLCAP;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ)
	flags |= WF_ONECAP;

    ಆದರೆ (u >= 2 && l >= 2)	// maCARONI maCAroni
	flags |= WF_MIXCAP;

    ಹಿಂತಿರುಗಿ flags;
}

/*
 * Opposite of offset2bytes().
 * "pp" points to the bytes and is advanced over it.
 * Returns the offset.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bytes2offset(char_u **pp)
{
    char_u	*p = *pp;
    ಪೂರ್ಣಾಂಕ		nr;
    ಪೂರ್ಣಾಂಕ		c;

    c = *p++;
    ಆದರೆ ((c & 0x80) == 0x00)		// 1 byte
    {
	nr = c - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c & 0xc0) == 0x80)	// 2 bytes
    {
	nr = (c & 0x3f) - 1;
	nr = nr * 255 + (*p++ - 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c & 0xe0) == 0xc0)	// 3 bytes
    {
	nr = (c & 0x1f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ				// 4 bytes
    {
	nr = (c & 0x0f) - 1;
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
	nr = nr * 255 + (*p++ - 1);
    }

    *pp = p;
    ಹಿಂತಿರುಗಿ nr;
}

// values for sps_flags
#ವ್ಯಾಖ್ಯಾನಿಸು SPS_ಉತ್ತಮ    1
#ವ್ಯಾಖ್ಯಾನಿಸು SPS_FAST    2
#ವ್ಯಾಖ್ಯಾನಿಸು SPS_DOUBLE  4

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sps_flags = SPS_ಉತ್ತಮ;	// flags from 'spellsuggest'
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sps_ಎಲ್ಲೆ = 9999;		// max nr of suggestions given

/*
 * Check the 'spellsuggest' option.  Return FAIL if it's wrong.
 * Sets "sps_flags" and "sps_limit".
 */
    ಪೂರ್ಣಾಂಕ
spell_check_sps(ಶೂನ್ಯ)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    ಪೂರ್ಣಾಂಕ		f;

    sps_flags = 0;
    sps_ಎಲ್ಲೆ = 9999;

    ಗಾಗಿ (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	ಆದರೆ (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_ಎಲ್ಲೆ = getdigits(&s);
	    ಆದರೆ (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	// Note: Keep this in sync with p_sps_values.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(buf, "best") == 0)
	    f = SPS_ಉತ್ತಮ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	ಆದರೆ (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_ಉತ್ತಮ;
	    sps_ಎಲ್ಲೆ = 9999;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (f != 0)
	    sps_flags = f;
    }

    ಆದರೆ (sps_flags == 0)
	sps_flags = SPS_ಉತ್ತಮ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "z=": Find badly spelled word under or after the cursor.
 * Give suggestions for the properly spelled word.
 * In Visual mode use the highlighted word as the bad word.
 * When "count" is non-zero use that suggestion.
 */
    ಶೂನ್ಯ
spell_suggest(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    char_u	*ಸಾಲು = ಖಾಲಿ;
    pos_T	prev_cursor = curwin->w_cursor;
    char_u	wcopy[MAXWLEN + 2];
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;
    suginfo_T	sug;
    suggest_T	*stp;
    ಪೂರ್ಣಾಂಕ		mouse_ಬಳಸಿದ;
    ಪೂರ್ಣಾಂಕ		need_cap;
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ;
    ಪೂರ್ಣಾಂಕ		selected = ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		badlen = 0;
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_scroll_save = ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಂಕ		wo_spell_save = curwin->w_p_spell;

    ಆದರೆ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }
    // Autocommands may have changed the buffer and made the cursor invalid
    check_cursor();
    prev_cursor = curwin->w_cursor;

    ಆದರೆ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	ಹೋಗು skip;
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	// Use the Visually selected text as the bad word.  But reject
	// a multi-line selection.
	ಆದರೆ (curwin->w_cursor.lnum != VIsual.lnum)
	{
	    vim_beep(BO_SPELL);
	    ಹೋಗು skip;
	}
	badlen = (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು - (ಪೂರ್ಣಾಂಕ)VIsual.ಲಂಬಸಾಲು;
	ಆದರೆ (badlen < 0)
	    badlen = -badlen;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_cursor.ಲಂಬಸಾಲು = VIsual.ಲಂಬಸಾಲು;
	++badlen;
	ಕೊನೆ_visual_ಬಹುಲಕ();
	// make sure we don't include the NUL at the end of the line
	ಆದರೆ (badlen > ml_get_curline_len() - (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು)
	    badlen = ml_get_curline_len() - (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು;
    }
    // Find the start of the badly spelled word.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (spell_move_ಗಮ್ಯ(curwin, FORWARD, SMT_ALL, TRUE, ಖಾಲಿ) == 0
	    || curwin->w_cursor.ಲಂಬಸಾಲು > prev_cursor.ಲಂಬಸಾಲು)
    {
	// No bad word or it starts after the cursor: use the word under the
	// cursor.
	curwin->w_cursor = prev_cursor;
	char_u *curline = ml_get_curline();
	p = curline + curwin->w_cursor.ಲಂಬಸಾಲು;
	// Backup to before start of word.
	ಆಗ (p > curline && spell_iswordp_nmw(p, curwin))
	    MB_PTR_ಹಿಂಭಾಗ(curline, p);
	// Forward to start of word.
	ಆಗ (*p != NUL && !spell_iswordp_nmw(p, curwin))
	    MB_PTR_ADV(p);

	ಆದರೆ (!spell_iswordp_nmw(p, curwin))		// No word found.
	{
	    beep_flush();
	    ಹೋಗು skip;
	}
	curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)(p - curline);
    }

    // Get the word and its length.

    // Figure out if the word should be capitalised.
    need_cap = check_need_cap(curwin, curwin->w_cursor.lnum,
							curwin->w_cursor.ಲಂಬಸಾಲು);

    // Make a copy of current line since autocommands may free the line.
    ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());
    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	ಹೋಗು skip;

    // Get the list of suggestions.  Limit to 'lines' - 2 or the number in
    // 'spellsuggest', whatever is smaller.
    ಆದರೆ (sps_ಎಲ್ಲೆ > (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 2)
	ಎಲ್ಲೆ = (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 2;
    ಇಲ್ಲದಿದ್ದರೆ
	ಎಲ್ಲೆ = sps_ಎಲ್ಲೆ;
    spell_ಕಂಡುಹಿಡಿ_suggest(ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು, badlen, &sug, ಎಲ್ಲೆ,
							TRUE, need_cap, TRUE);

    ಆದರೆ (sug.su_ga.ga_len == 0)
	ಸಂದೇಶ(_("Sorry, no suggestions"));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_2 > 0)
    {
	ಆದರೆ (ಎಣಿಕೆ_2 > sug.su_ga.ga_len)
	    smsg(_("Sorry, only %ld suggestions"), (ದೀರ್ಘ)sug.su_ga.ga_len);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	// When 'rightleft' is set the list is drawn right-left.
	cmdmsg_rl = curwin->w_p_rl;
	ಆದರೆ (cmdmsg_rl)
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
#ಆದರೆ_ಅಂತ್ಯ

	// List the suggestions.
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;	// for when 'cmdheight' > 1
	ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು;	// avoid more prompt
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("Change \"%.*s\" to:"),
						sug.su_badlen, sug.su_badptr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (cmdmsg_rl && STRNCMP(IObuff, "Change", 6) == 0)
	{
	    // And now the rabbit from the high hat: Avoid showing the
	    // untranslated message rightleft.
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, ":ot \"%.*s\" egnahC",
						sug.su_badlen, sug.su_badptr);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
	ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_putchar('\n');

	ಸಂದೇಶ_scroll = TRUE;
	ಗಾಗಿ (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    ಪೂರ್ಣಾಂಕ el;

	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of the bad word, add
	    // the not replaced part.  But only when it's not getting too long.
	    vim_strncpy(wcopy, stp->st_ಶಬ್ದ, MAXWLEN);
	    el = sug.su_badlen - stp->st_orglen;
	    ಆದರೆ (el > 0 && stp->st_wordlen + el <= MAXWLEN)
		vim_strncpy(wcopy + stp->st_wordlen,
					   sug.su_badptr + stp->st_orglen, el);
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%2d", i + 1);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (cmdmsg_rl)
		rl_mirror(IObuff);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);

	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, " \"%s\"", wcopy);
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);

	    // The word may replace more than "su_badlen".
	    ಆದರೆ (sug.su_badlen < stp->st_orglen)
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _(" < \"%.*s\""),
					       stp->st_orglen, sug.su_badptr);
		ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
	    }

	    ಆದರೆ (p_verbose > 0)
	    {
		// Add the score.
		ಆದರೆ (sps_flags & (SPS_DOUBLE | SPS_ಉತ್ತಮ))
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, " (%s%d - %d)",
			stp->st_salscore ? "s " : "",
			stp->st_score, stp->st_altscore);
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, " (%d)",
			    stp->st_score);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		ಆದರೆ (cmdmsg_rl)
		    // Mirror the numbers, but keep the leading space.
		    rl_mirror(IObuff + 1);
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_advance(30);
		ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
	    }
	    ಸಂದೇಶ_putchar('\n');
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	cmdmsg_rl = FALSE;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
#ಆದರೆ_ಅಂತ್ಯ
	// Ask for choice.
	selected = prompt_for_ಸಂಖ್ಯೆ(&mouse_ಬಳಸಿದ);
	ಆದರೆ (mouse_ಬಳಸಿದ)
	    selected -= ಸಾಲುಗಳು_ಎಡ;
	ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು;		// avoid more prompt
	// don't delay for 'smd' in normal_cmd()
	ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll_save;
    }

    ಆದರೆ (selected > 0 && selected <= sug.su_ga.ga_len && u_save_cursor() == OK)
    {
	// Save the from and to text for :spellrepall.
	VIM_CLEAR(repl_from);
	VIM_CLEAR(repl_ಗಮ್ಯ);

	stp = &SUG(sug.su_ga, selected - 1);
	ಆದರೆ (sug.su_badlen > stp->st_orglen)
	{
	    // Replacing less than "su_badlen", append the remainder to
	    // repl_to.
	    repl_from = vim_strnsave(sug.su_badptr, sug.su_badlen);
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%s%.*s", stp->st_ಶಬ್ದ,
		    sug.su_badlen - stp->st_orglen,
					      sug.su_badptr + stp->st_orglen);
	    repl_ಗಮ್ಯ = vim_strsave(IObuff);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Replacing su_badlen or more, use the whole word.
	    repl_from = vim_strnsave(sug.su_badptr, stp->st_orglen);
	    repl_ಗಮ್ಯ = vim_strsave(stp->st_ಶಬ್ದ);
	}

	// Replace the word.
	p = alloc(STRLEN(ಸಾಲು) - stp->st_orglen + stp->st_wordlen + 1);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ len_diff = stp->st_wordlen - stp->st_orglen;

	    c = (ಪೂರ್ಣಾಂಕ)(sug.su_badptr - ಸಾಲು);
	    mch_memmove(p, ಸಾಲು, c);
	    STRCPY(p + c, stp->st_ಶಬ್ದ);
	    STRCAT(p, sug.su_badptr + stp->st_orglen);

	    // For redo we use a change-word command.
	    ResetRedobuff();
	    Append_ಗಮ್ಯ_Redobuff((char_u *)"ciw");
	    Append_ಗಮ್ಯ_Redobuff_Lit(p + c,
			    stp->st_wordlen + sug.su_badlen - stp->st_orglen);
	    Append_Char_ಗಮ್ಯ_Redobuff(ESC);

	    // "p" may be freed here
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    curwin->w_cursor.ಲಂಬಸಾಲು = c;

	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curwin->w_cursor.lnum, c);
#ಆದರೆ defined(FEAT_PROP_POPUP)
	    ಆದರೆ (curbuf->b_has_textprop && len_diff != 0)
		adjust_prop_columns(curwin->w_cursor.lnum, c, len_diff,
							       APC_SUBSTITUTE);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_cursor = prev_cursor;

    spell_ಕಂಡುಹಿಡಿ_cleanup(&sug);
skip:
    vim_free(ಸಾಲು);
    curwin->w_p_spell = wo_spell_save;
}

/*
 * Find spell suggestions for "word".  Return them in the growarray "*gap" as
 * a list of allocated strings.
 */
    ಶೂನ್ಯ
spell_suggest_list(
    garray_T	*gap,
    char_u	*ಶಬ್ದ,
    ಪೂರ್ಣಾಂಕ		maxcount,	// maximum nr of suggestions
    ಪೂರ್ಣಾಂಕ		need_cap,	// 'spellcapcheck' matched
    ಪೂರ್ಣಾಂಕ		interactive)
{
    suginfo_T	sug;
    ಪೂರ್ಣಾಂಕ		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_ಕಂಡುಹಿಡಿ_suggest(ಶಬ್ದ, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, ಗಾತ್ರ(char_u *), sug.su_ga.ga_len + 1);
    ಆದರೆ (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	ಗಾಗಿ (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (ಚಿಹ್ನೆರಹಿತ)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    ಆದರೆ (wcopy == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    STRCPY(wcopy, stp->st_ಶಬ್ದ);
	    STRCPY(wcopy + stp->st_wordlen, sug.su_badptr + stp->st_orglen);
	    ((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = wcopy;
	}
    }

    spell_ಕಂಡುಹಿಡಿ_cleanup(&sug);
}

/*
 * Find spell suggestions for the word at the start of "badptr".
 * Return the suggestions in "su->su_ga".
 * The maximum number of suggestions is "maxcount".
 * Note: does use info for the current window.
 * This is based on the mechanisms of Aspell, but completely reimplemented.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_ಕಂಡುಹಿಡಿ_suggest(
    char_u	*badptr,
    ಪೂರ್ಣಾಂಕ		badlen,		// length of bad word or 0 if unknown
    suginfo_T	*su,
    ಪೂರ್ಣಾಂಕ		maxcount,
    ಪೂರ್ಣಾಂಕ		banbadword,	// don't include badword in suggestions
    ಪೂರ್ಣಾಂಕ		need_cap,	// word should start with capital
    ಪೂರ್ಣಾಂಕ		interactive)
{
    hlf_T	attr = HLF_ಎಣಿಕೆ;
    char_u	buf[MAXPATHL];
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		do_combine = FALSE;
    char_u	*sps_copy;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಅಭಿವ್ಯಕ್ತಿ_busy = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;
    langp_T	*lp;
    ಪೂರ್ಣಾಂಕ		did_intern = FALSE;

    // Set the info in "*su".
    CLEAR_ಸೂಚಕ(su);
    ga_init2(&su->su_ga, ಗಾತ್ರ(suggest_T), 10);
    ga_init2(&su->su_sga, ಗಾತ್ರ(suggest_T), 10);
    ಆದರೆ (*badptr == NUL)
	ಹಿಂತಿರುಗಿ;
    hash_init(&su->su_banned);

    su->su_badptr = badptr;
    ಆದರೆ (badlen != 0)
	su->su_badlen = badlen;
    ಇಲ್ಲದಿದ್ದರೆ
	su->su_badlen = spell_check(curwin, su->su_badptr, &attr, ಖಾಲಿ, FALSE);
    su->su_maxcount = maxcount;
    su->su_maxscore = SCORE_MAXINIT;

    ಆದರೆ (su->su_badlen >= MAXWLEN)
	su->su_badlen = MAXWLEN - 1;	// just in case
    vim_strncpy(su->su_badword, su->su_badptr, su->su_badlen);
    (ಶೂನ್ಯ)spell_casefold(curwin, su->su_badptr, su->su_badlen,
						    su->su_fbadword, MAXWLEN);
    // TODO: make this work if the case-folded text is longer than the original
    // text. Currently an illegal byte causes wrong pointer computations.
    su->su_fbadword[su->su_badlen] = NUL;

    // get caps flags for bad word
    su->su_badflags = badword_captype(su->su_badptr,
					       su->su_badptr + su->su_badlen);
    ಆದರೆ (need_cap)
	su->su_badflags |= WF_ONECAP;

    // Find the default language for sound folding.  We simply use the first
    // one in 'spelllang' that supports sound folding.  That's good for when
    // using multiple files for one language, it's not that bad when mixing
    // languages (e.g., "pl,en").
    ಗಾಗಿ (i = 0; i < curbuf->b_s.b_langp.ga_len; ++i)
    {
	lp = LANGP_ನಮೂದು(curbuf->b_s.b_langp, i);
	ಆದರೆ (lp->lp_sallang != ಖಾಲಿ)
	{
	    su->su_sallang = lp->lp_sallang;
	    ನಿಲ್ಲಿಸು;
	}
    }

    // Soundfold the bad word with the default sound folding, so that we don't
    // have to do this many times.
    ಆದರೆ (su->su_sallang != ಖಾಲಿ)
	spell_soundfold(su->su_sallang, su->su_fbadword, TRUE,
							  su->su_sal_badword);

    // If the word is not capitalised and spell_check() doesn't consider the
    // word to be bad then it might need to be capitalised.  Add a suggestion
    // for that.
    c = PTR2CHAR(su->su_badptr);
    ಆದರೆ (!SPELL_ISUPPER(c) && attr == HLF_ಎಣಿಕೆ)
    {
	make_case_ಶಬ್ದ(su->su_badword, buf, WF_ONECAP);
	ಕೂಡಿಸು_suggestion(su, &su->su_ga, buf, su->su_badlen, SCORE_ICASE,
					      0, TRUE, su->su_sallang, FALSE);
    }

    // Ban the bad word itself.  It may appear in another region.
    ಆದರೆ (banbadword)
	ಕೂಡಿಸು_banned(su, su->su_badword);

    // Make a copy of 'spellsuggest', because the expression may change it.
    sps_copy = vim_strsave(p_sps);
    ಆದರೆ (sps_copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    spell_suggest_timeout = 5000;

    // Loop over the items in 'spellsuggest'.
    ಗಾಗಿ (p = sps_copy; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	ಆದರೆ (STRNCMP(buf, "expr:", 5) == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // Evaluate an expression.  Skip this when called recursively,
	    // when using spellsuggest() in the expression.
	    ಆದರೆ (!ಅಭಿವ್ಯಕ್ತಿ_busy)
	    {
		ಅಭಿವ್ಯಕ್ತಿ_busy = TRUE;
		spell_suggest_ಅಭಿವ್ಯಕ್ತಿ(su, buf + 5);
		ಅಭಿವ್ಯಕ್ತಿ_busy = FALSE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(buf, "file:", 5) == 0)
	    // Use list of suggestions in a file.
	    spell_suggest_file(su, buf + 5);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(buf, "timeout:", 8) == 0)
	    // Limit the time searching for suggestions.
	    spell_suggest_timeout = atol((ಅಕ್ಷರ *)buf + 8);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!did_intern)
	{
	    // Use internal method once.
	    spell_suggest_intern(su, interactive);
	    ಆದರೆ (sps_flags & SPS_DOUBLE)
		do_combine = TRUE;
	    did_intern = TRUE;
	}
    }

    vim_free(sps_copy);

    ಆದರೆ (do_combine)
	// Combine the two list of suggestions.  This must be done last,
	// because sorting changes the order again.
	score_combine(su);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Find suggestions by evaluating expression "expr".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_suggest_ಅಭಿವ್ಯಕ್ತಿ(suginfo_T *su, char_u *ಅಭಿವ್ಯಕ್ತಿ)
{
    list_T	*list;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		score;
    char_u	*p;

    // The work is split up in a few parts to avoid having to export
    // suginfo_T.
    // First evaluate the expression and get the resulting list.
    list = eval_spell_ಅಭಿವ್ಯಕ್ತಿ(su->su_badword, ಅಭಿವ್ಯಕ್ತಿ);
    ಆದರೆ (list != ಖಾಲಿ)
    {
	// Loop over the items in the list.
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	    ಆದರೆ (li->li_tv.v_type == VAR_LIST)
	    {
		// Get the word and the score from the items.
		score = get_spellword(li->li_tv.vval.v_list, &p);
		ಆದರೆ (score >= 0 && score <= su->su_maxscore)
		    ಕೂಡಿಸು_suggestion(su, &su->su_ga, p, su->su_badlen,
				       score, 0, TRUE, su->su_sallang, FALSE);
	    }
	list_unref(list);
    }

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (ಶೂನ್ಯ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Find suggestions in file "fname".  Used for "file:" in 'spellsuggest'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	ಸಾಲು[MAXWLEN * 2];
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((ಅಕ್ಷರ *)fname, "r");
    ಆದರೆ (fd == ಖಾಲಿ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ಹಿಂತಿರುಗಿ;
    }

    // Read it line by line.
    ಆಗ (!vim_fgets(ಸಾಲು, MAXWLEN * 2, fd) && !got_int)
    {
	ಸಾಲು_breakcheck();

	p = vim_strchr(ಸಾಲು, '/');
	ಆದರೆ (p == ಖಾಲಿ)
	    ಮುಂದುವರಿ;	    // No Tab found, just skip the line.
	*p++ = NUL;
	ಆದರೆ (STRICMP(su->su_badword, ಸಾಲು) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    ಗಾಗಿ (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    ಆದರೆ (captype(p, ಖಾಲಿ) == 0)
	    {
		make_case_ಶಬ್ದ(p, cword, su->su_badflags);
		p = cword;
	    }

	    ಕೂಡಿಸು_suggestion(su, &su->su_ga, p, su->su_badlen,
				  SCORE_FILE, 0, TRUE, su->su_sallang, FALSE);
	}
    }

    fclose(fd);

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (ಶೂನ್ಯ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}

/*
 * Find suggestions for the internal method indicated by "sps_flags".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_suggest_intern(suginfo_T *su, ಪೂರ್ಣಾಂಕ interactive)
{
    // Load the .sug file(s) that are available and not done yet.
    suggest_ತುಂಬು_files();

    // 1. Try special cases, such as repeating a word: "the the" -> "the".
    //
    // Set a maximum score to limit the combination of operations that is
    // tried.
    suggest_try_special(su);

    // 2. Try inserting/deleting/swapping/changing a letter, use REP entries
    //    from the .aff file and inserting a space (split the word).
    suggest_try_change(su);

    // For the resulting top-scorers compute the sound-a-like score.
    ಆದರೆ (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    ಆದರೆ ((sps_flags & SPS_FAST) == 0)
    {
	ಆದರೆ (sps_flags & SPS_ಉತ್ತಮ)
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
	ಆದರೆ (su->su_ga.ga_len < SUG_CLEAN_ಎಣಿಕೆ(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    ಆದರೆ (su->su_ga.ga_len < SUG_CLEAN_ಎಣಿಕೆ(su))
	    {
		// Still didn't find enough matches, try again, allowing even
		// more changes to the soundfold word.
		su->su_maxscore = SCORE_SFMAX3;
		suggest_try_soundalike(su);
	    }
	}
	su->su_maxscore = su->su_sfmaxscore;
	suggest_try_soundalike_ಮುಕ್ತಾಯ();
    }

    // When CTRL-C was hit while searching do show the results.  Only clear
    // got_int when using a command, not for spellsuggest().
    ui_breakcheck();
    ಆದರೆ (interactive && got_int)
    {
	(ಶೂನ್ಯ)vgetc();
	got_int = FALSE;
    }

    ಆದರೆ ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	ಆದರೆ (sps_flags & SPS_ಉತ್ತಮ)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(ಶೂನ್ಯ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}

/*
 * Free the info put in "*su" by spell_find_suggest().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_ಕಂಡುಹಿಡಿ_cleanup(suginfo_T *su)
{
    ಪೂರ್ಣಾಂಕ		i;

    // Free the suggestions.
    ಗಾಗಿ (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_ಶಬ್ದ);
    ga_clear(&su->su_ga);
    ಗಾಗಿ (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_ಶಬ್ದ);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}

/*
 * Try finding suggestions by recognizing specific situations.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suggest_try_special(suginfo_T *su)
{
    char_u	*p;
    size_t	len;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	ಶಬ್ದ[MAXWLEN];

    // Recognize a word that is repeated: "the the".
    p = skiptowhite(su->su_fbadword);
    len = p - su->su_fbadword;
    p = skipwhite(p);
    ಆದರೆ (STRLEN(p) == len && STRNCMP(su->su_fbadword, p, len) == 0)
    {
	// Include badflags: if the badword is onecap or allcap
	// use that for the goodword too: "The the" -> "The".
	c = su->su_fbadword[len];
	su->su_fbadword[len] = NUL;
	make_case_ಶಬ್ದ(su->su_fbadword, ಶಬ್ದ, su->su_badflags);
	su->su_fbadword[len] = c;

	// Give a soundalike score of 0, compute the score as if deleting one
	// character.
	ಕೂಡಿಸು_suggestion(su, &su->su_ga, ಶಬ್ದ, su->su_badlen,
		       RESCORE(SCORE_REP, 0), 0, TRUE, su->su_sallang, FALSE);
    }
}

/*
 * Change the 0 to 1 to measure how much time is spent in each state.
 * Output is dumped in "suggestprof".
 */
#ಆದರೆ 0
# define SUGGEST_PROFILE
proftime_T current;
proftime_T ಒಟ್ಟು;
proftime_T times[STATE_FINAL + 1];
ದೀರ್ಘ counts[STATE_FINAL + 1];

    ಸ್ಥಿರ ಶೂನ್ಯ
prof_init(ಶೂನ್ಯ)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= STATE_FINAL; ++i)
    {
	profile_zero(&times[i]);
	counts[i] = 0;
    }
    profile_ಆರಂಭ(&current);
    profile_ಆರಂಭ(&ಒಟ್ಟು);
}

// call before changing state
    ಸ್ಥಿರ ಶೂನ್ಯ
prof_store(state_T state)
{
    profile_ಕೊನೆ(&current);
    profile_ಕೂಡಿಸು(&times[state], &current);
    ++counts[state];
    profile_ಆರಂಭ(&current);
}
# define PROF_STORE(state) prof_store(state);

    ಸ್ಥಿರ ಶೂನ್ಯ
prof_report(ಅಕ್ಷರ *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_ಕೊನೆ(&ಒಟ್ಟು);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_ಸಂದೇಶ(&ಒಟ್ಟು));
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_ಸಂದೇಶ(&times[i]), counts[i]);
    fclose(fd);
}
#ಇಲ್ಲದಿದ್ದರೆ
# define PROF_STORE(state)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Try finding suggestions by adding/removing/swapping letters.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (ಪೂರ್ಣಾಂಕ)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (ಶೂನ್ಯ)spell_casefold(curwin, p, (ಪೂರ್ಣಾಂಕ)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (ಪೂರ್ಣಾಂಕ)STRLEN(su->su_badptr);
    ಆದರೆ (n < MAXWLEN)
	fword[n] = NUL;

    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	ಆದರೆ (lp->lp_slang->sl_fbyts == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// Try it for this language.  Will add possible suggestions.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SUGGEST_PROFILE
	prof_init();
#ಆದರೆ_ಅಂತ್ಯ
	suggest_trie_walk(su, lp, fword, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SUGGEST_PROFILE
	prof_report("try_change");
#ಆದರೆ_ಅಂತ್ಯ
    }
}

// Check the maximum score, if we go over it we won't try this change.
#ವ್ಯಾಖ್ಯಾನಿಸು TRY_DEEPER(su, ರಾಶಿ, ಆಳ, ಕೂಡಿಸು) \
       ((ಆಳ) < MAXWLEN - 1 && (ರಾಶಿ)[ಆಳ].ts_score + (ಕೂಡಿಸು) < (su)->su_maxscore)

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
    ಸ್ಥಿರ ಶೂನ್ಯ
suggest_trie_walk(
    suginfo_T	*su,
    langp_T	*lp,
    char_u	*fword,
    ಪೂರ್ಣಾಂಕ		soundfold)
{
    char_u	tword[MAXWLEN];	    // good word collected so far
    trystate_T	ರಾಶಿ[MAXWLEN];
    char_u	preword[MAXWLEN * 3]; // word found with proper case;
				      // concatenation of prefix compound
				      // words and split word.  NUL terminated
				      // when going deeper but not when coming
				      // back.
    char_u	compflags[MAXWLEN];	// compound flags, one for each word
    trystate_T	*sp;
    ಪೂರ್ಣಾಂಕ		newscore;
    ಪೂರ್ಣಾಂಕ		score;
    char_u	*byts, *fbyts, *pbyts;
    idx_T	*idxs, *fidxs, *pidxs;
    ಪೂರ್ಣಾಂಕ		ಆಳ;
    ಪೂರ್ಣಾಂಕ		c, c2, c3;
    ಪೂರ್ಣಾಂಕ		n = 0;
    ಪೂರ್ಣಾಂಕ		flags;
    garray_T	*gap;
    idx_T	arridx;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;
    fromto_T	*ftp;
    ಪೂರ್ಣಾಂಕ		fl = 0, tl;
    ಪೂರ್ಣಾಂಕ		repextra = 0;	    // extra bytes in fword[] from REP item
    slang_T	*slang = lp->lp_slang;
    ಪೂರ್ಣಾಂಕ		fword_ತುದಿಗಳು;
    ಪೂರ್ಣಾಂಕ		goodword_ತುದಿಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
    // Stores the name of the change made at each level.
    char_u	changename[MAXWLEN][80];
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		breakcheckcount = 1000;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    proftime_T	ಸಮಯ_ಎಲ್ಲೆ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		compound_ok;

    // Go through the whole case-fold tree, try changes at each node.
    // "tword[]" contains the word collected from nodes in the tree.
    // "fword[]" the word we are trying to match with (initially the bad
    // word).
    ಆಳ = 0;
    sp = &ರಾಶಿ[0];
    CLEAR_ಸೂಚಕ(sp);
    sp->ts_curi = 1;

    ಆದರೆ (soundfold)
    {
	// Going through the soundfold tree.
	byts = fbyts = slang->sl_sbyts;
	idxs = fidxs = slang->sl_sidxs;
	pbyts = ಖಾಲಿ;
	pidxs = ಖಾಲಿ;
	sp->ts_prefixdepth = PFD_NOPREFIX;
	sp->ts_state = STATE_ಆರಂಭ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When there are postponed prefixes we need to use these first.  At
	// the end of the prefix we continue in the case-fold tree.
	fbyts = slang->sl_fbyts;
	fidxs = slang->sl_fidxs;
	pbyts = slang->sl_pbyts;
	pidxs = slang->sl_pidxs;
	ಆದರೆ (pbyts != ಖಾಲಿ)
	{
	    byts = pbyts;
	    idxs = pidxs;
	    sp->ts_prefixdepth = PFD_PREFIXTREE;
	    sp->ts_state = STATE_NOPREFIX;	// try without prefix first
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    byts = fbyts;
	    idxs = fidxs;
	    sp->ts_prefixdepth = PFD_NOPREFIX;
	    sp->ts_state = STATE_ಆರಂಭ;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    // The loop may take an indefinite amount of time. Break out after some
    // time.
    ಆದರೆ (spell_suggest_timeout > 0)
	profile_setlimit(spell_suggest_timeout, &ಸಮಯ_ಎಲ್ಲೆ);
#ಆದರೆ_ಅಂತ್ಯ

    // Loop to find all suggestions.  At each round we either:
    // - For the current state try one operation, advance "ts_curi",
    //   increase "depth".
    // - When a state is done go to the next, set "ts_state".
    // - When all states are tried decrease "depth".
    ಆಗ (ಆಳ >= 0 && !got_int)
    {
	sp = &ರಾಶಿ[ಆಳ];
	ಆಯ್ಕೆ (sp->ts_state)
	{
	ಸ್ಥಿತಿ STATE_ಆರಂಭ:
	ಸ್ಥಿತಿ STATE_NOPREFIX:
	    // Start of node: Deal with NUL bytes, which means
	    // tword[] may end here.
	    arridx = sp->ts_arridx;	    // current node in the tree
	    len = byts[arridx];		    // bytes in this node
	    arridx += sp->ts_curi;	    // index of current byte

	    ಆದರೆ (sp->ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Skip over the NUL bytes, we use them later.
		ಗಾಗಿ (n = 0; n < len && byts[arridx + n] == 0; ++n)
		    ;
		sp->ts_curi += n;

		// Always past NUL bytes now.
		n = (ಪೂರ್ಣಾಂಕ)sp->ts_state;
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;

		// At end of a prefix or at start of prefixtree: check for
		// following word.
		ಆದರೆ (ಆಳ < MAXWLEN - 1
			    && (byts[arridx] == 0 || n == (ಪೂರ್ಣಾಂಕ)STATE_NOPREFIX))
		{
		    // Set su->su_badflags to the caps type at this position.
		    // Use the caps type until here for the prefix itself.
		    ಆದರೆ (has_mbyte)
			n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
		    ಇಲ್ಲದಿದ್ದರೆ
			n = sp->ts_fidx;
		    flags = badword_captype(su->su_badptr, su->su_badptr + n);
		    su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		    sprintf(changename[ಆಳ], "prefix");
#ಆದರೆ_ಅಂತ್ಯ
		    go_deeper(ರಾಶಿ, ಆಳ, 0);
		    ++ಆಳ;
		    sp = &ರಾಶಿ[ಆಳ];
		    sp->ts_prefixdepth = ಆಳ - 1;
		    byts = fbyts;
		    idxs = fidxs;
		    sp->ts_arridx = 0;

		    // Move the prefix to preword[] with the right case
		    // and make find_keepcap_word() works.
		    tword[sp->ts_twordlen] = NUL;
		    make_case_ಶಬ್ದ(tword + sp->ts_splitoff,
					  preword + sp->ts_prewordlen, flags);
		    sp->ts_prewordlen = (char_u)STRLEN(preword);
		    sp->ts_splitoff = sp->ts_twordlen;
		}
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (sp->ts_curi > len || byts[arridx] != 0)
	    {
		// Past bytes in node and/or past NUL bytes.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ENDNUL;
		sp->ts_save_badflags = su->su_badflags;
		ನಿಲ್ಲಿಸು;
	    }

	    // End of word in tree.
	    ++sp->ts_curi;		// eat one NUL byte

	    flags = (ಪೂರ್ಣಾಂಕ)idxs[arridx];

	    // Skip words with the NOSUGGEST flag.
	    ಆದರೆ (flags & WF_NOSUGGEST)
		ನಿಲ್ಲಿಸು;

	    fword_ತುದಿಗಳು = (fword[sp->ts_fidx] == NUL
			   || (soundfold
			       ? VIM_ISWHITE(fword[sp->ts_fidx])
			       : !spell_iswordp(fword + sp->ts_fidx, curwin)));
	    tword[sp->ts_twordlen] = NUL;

	    ಆದರೆ (sp->ts_prefixdepth <= PFD_NOTSPECIAL
					&& (sp->ts_flags & TSF_PREFIXOK) == 0
					&& pbyts != ಖಾಲಿ)
	    {
		// There was a prefix before the word.  Check that the prefix
		// can be used with this word.
		// Count the length of the NULs in the prefix.  If there are
		// none this must be the first try without a prefix.
		n = ರಾಶಿ[sp->ts_prefixdepth].ts_arridx;
		len = pbyts[n++];
		ಗಾಗಿ (c = 0; c < len && pbyts[n + c] == 0; ++c)
		    ;
		ಆದರೆ (c > 0)
		{
		    c = valid_ಶಬ್ದ_ಪೂರ್ವಪ್ರತ್ಯಯ(c, n, flags,
				       tword + sp->ts_splitoff, slang, FALSE);
		    ಆದರೆ (c == 0)
			ನಿಲ್ಲಿಸು;

		    // Use the WF_RARE flag for a rare prefix.
		    ಆದರೆ (c & WF_RAREPFX)
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
	    ಆದರೆ (sp->ts_complen == sp->ts_compsplit && fword_ತುದಿಗಳು
						     && (flags & WF_NEEDCOMP))
		goodword_ತುದಿಗಳು = FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ
		goodword_ತುದಿಗಳು = TRUE;

	    p = ಖಾಲಿ;
	    compound_ok = TRUE;
	    ಆದರೆ (sp->ts_complen > sp->ts_compsplit)
	    {
		ಆದರೆ (slang->sl_nobreak)
		{
		    // There was a word before this word.  When there was no
		    // change in this word (it was correct) add the first word
		    // as a suggestion.  If this word was corrected too, we
		    // need to check if a correct word follows.
		    ಆದರೆ (sp->ts_fidx - sp->ts_splitfidx
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
			ಆದರೆ (newscore <= su->su_maxscore)
			    ಕೂಡಿಸು_suggestion(su, &su->su_ga, preword,
				    sp->ts_splitfidx - repextra,
				    newscore, 0, FALSE,
				    lp->lp_sallang, FALSE);
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // There was a compound word before this word.  If this
		    // word does not support compounding then give up
		    // (splitting is tried for the word without compound
		    // flag).
		    ಆದರೆ (((ಚಿಹ್ನೆರಹಿತ)flags >> 24) == 0
			    || sp->ts_twordlen - sp->ts_splitoff
						       < slang->sl_compminlen)
			ನಿಲ್ಲಿಸು;
		    // For multi-byte chars check character length against
		    // COMPOUNDMIN.
		    ಆದರೆ (has_mbyte
			    && slang->sl_compminlen > 0
			    && mb_charlen(tword + sp->ts_splitoff)
						       < slang->sl_compminlen)
			ನಿಲ್ಲಿಸು;

		    compflags[sp->ts_complen] = ((ಚಿಹ್ನೆರಹಿತ)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		    vim_strncpy(preword + sp->ts_prewordlen,
			    tword + sp->ts_splitoff,
			    sp->ts_twordlen - sp->ts_splitoff);

		    // Verify CHECKCOMPOUNDPATTERN  rules.
		    ಆದರೆ (ಹೊಂದಿಕೆ_checkcompoundpattern(preword,  sp->ts_prewordlen,
							  &slang->sl_comppat))
			compound_ok = FALSE;

		    ಆದರೆ (compound_ok)
		    {
			p = preword;
			ಆಗ (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			ಆದರೆ (fword_ತುದಿಗಳು && !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    // Compound is not allowed.  But it may still be
			    // possible if we add another (short) word.
			    compound_ok = FALSE;
		    }

		    // Get pointer to last char of previous word.
		    p = preword + sp->ts_prewordlen;
		    MB_PTR_ಹಿಂಭಾಗ(preword, p);
		}
	    }

	    // Form the word with proper case in preword.
	    // If there is a word from a previous split, append.
	    // For the soundfold tree don't change the case, simply append.
	    ಆದರೆ (soundfold)
		STRCPY(preword + sp->ts_prewordlen, tword + sp->ts_splitoff);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WF_KEEPCAP)
		// Must find the word in the keep-case tree.
		ಕಂಡುಹಿಡಿ_keepcap_ಶಬ್ದ(slang, tword + sp->ts_splitoff,
						 preword + sp->ts_prewordlen);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Include badflags: If the badword is onecap or allcap
		// use that for the goodword too.  But if the badword is
		// allcap and it's only one char long use onecap.
		c = su->su_badflags;
		ಆದರೆ ((c & WF_ALLCAP)
			&& su->su_badlen == (*mb_ptr2len)(su->su_badptr))
		    c = WF_ONECAP;
		c |= flags;

		// When appending a compound word after a word character don't
		// use Onecap.
		ಆದರೆ (p != ಖಾಲಿ && spell_iswordp_nmw(p, curwin))
		    c &= ~WF_ONECAP;
		make_case_ಶಬ್ದ(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen, c);
	    }

	    ಆದರೆ (!soundfold)
	    {
		// Don't use a banned word.  It may appear again as a good
		// word, thus remember it.
		ಆದರೆ (flags & WF_BANNED)
		{
		    ಕೂಡಿಸು_banned(su, preword + sp->ts_prewordlen);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ ((sp->ts_complen == sp->ts_compsplit
			    && WAS_BANNED(su, preword + sp->ts_prewordlen))
						   || WAS_BANNED(su, preword))
		{
		    ಆದರೆ (slang->sl_compprog == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		    // the word so far was banned but we may try compounding
		    goodword_ತುದಿಗಳು = FALSE;
		}
	    }

	    newscore = 0;
	    ಆದರೆ (!soundfold)	// soundfold words don't have flags
	    {
		ಆದರೆ ((flags & WF_REGION)
			    && (((ಚಿಹ್ನೆರಹಿತ)flags >> 16) & lp->lp_region) == 0)
		    newscore += SCORE_REGION;
		ಆದರೆ (flags & WF_RARE)
		    newscore += SCORE_RARE;

		ಆದರೆ (!spell_valid_case(su->su_badflags,
				  captype(preword + sp->ts_prewordlen, ಖಾಲಿ)))
		    newscore += SCORE_ICASE;
	    }

	    // TODO: how about splitting in the soundfold tree?
	    ಆದರೆ (fword_ತುದಿಗಳು
		    && goodword_ತುದಿಗಳು
		    && sp->ts_fidx >= sp->ts_fidxtry
		    && compound_ok)
	    {
		// The badword also ends: add suggestions.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		ಆದರೆ (soundfold && STRCMP(preword, "smwrd") == 0)
		{
		    ಪೂರ್ಣಾಂಕ	    j;

		    // print the stack of changes that brought us here
		    smsg("------ %s -------", fword);
		    ಗಾಗಿ (j = 0; j < ಆಳ; ++j)
			smsg("%s", changename[j]);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (soundfold)
		{
		    // For soundfolded words we need to find the original
		    // words, the edit distance and then add them.
		    ಕೂಡಿಸು_sound_suggest(su, preword, sp->ts_score, lp);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp->ts_fidx > 0)
		{
		    // Give a penalty when changing non-word char to word
		    // char, e.g., "thes," -> "these".
		    p = fword + sp->ts_fidx;
		    MB_PTR_ಹಿಂಭಾಗ(fword, p);
		    ಆದರೆ (!spell_iswordp(p, curwin) && *preword != NUL)
		    {
			p = preword + STRLEN(preword);
			MB_PTR_ಹಿಂಭಾಗ(preword, p);
			ಆದರೆ (spell_iswordp(p, curwin))
			    newscore += SCORE_NONWORD;
		    }

		    // Give a bonus to words seen before.
		    score = score_wordcount_adj(slang,
						sp->ts_score + newscore,
						preword + sp->ts_prewordlen,
						sp->ts_prewordlen > 0);

		    // Add the suggestion if the score isn't too bad.
		    ಆದರೆ (score <= su->su_maxscore)
		    {
			ಕೂಡಿಸು_suggestion(su, &su->su_ga, preword,
				    sp->ts_fidx - repextra,
				    score, 0, FALSE, lp->lp_sallang, FALSE);

			ಆದರೆ (su->su_badflags & WF_MIXCAP)
			{
			    // We really don't know if the word should be
			    // upper or lower case, add both.
			    c = captype(preword, ಖಾಲಿ);
			    ಆದರೆ (c == 0 || c == WF_ALLCAP)
			    {
				make_case_ಶಬ್ದ(tword + sp->ts_splitoff,
					      preword + sp->ts_prewordlen,
						      c == 0 ? WF_ALLCAP : 0);

				ಕೂಡಿಸು_suggestion(su, &su->su_ga, preword,
					sp->ts_fidx - repextra,
					score + SCORE_ICASE, 0, FALSE,
					lp->lp_sallang, FALSE);
			    }
			}
		    }
		}
	    }

	    // Try word split and/or compounding.
	    ಆದರೆ ((sp->ts_fidx >= sp->ts_fidxtry || fword_ತುದಿಗಳು)
		    // Don't split halfway a character.
		    && (!has_mbyte || sp->ts_tcharlen == 0))
	    {
		ಪೂರ್ಣಾಂಕ	try_compound;
		ಪೂರ್ಣಾಂಕ	try_ವಿಭಜಿಸು;

		// If past the end of the bad word don't try a split.
		// Otherwise try changing the next word.  E.g., find
		// suggestions for "the the" where the second "the" is
		// different.  It's done like a split.
		// TODO: word split for soundfold words
		try_ವಿಭಜಿಸು = (sp->ts_fidx - repextra < su->su_badlen)
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
		ಆದರೆ (!soundfold
			&& !slang->sl_nocompoundsugs
			&& slang->sl_compprog != ಖಾಲಿ
			&& ((ಚಿಹ್ನೆರಹಿತ)flags >> 24) != 0
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
					 compflags, ((ಚಿಹ್ನೆರಹಿತ)flags >> 24))))

		{
		    try_compound = TRUE;
		    compflags[sp->ts_complen] = ((ಚಿಹ್ನೆರಹಿತ)flags >> 24);
		    compflags[sp->ts_complen + 1] = NUL;
		}

		// For NOBREAK we never try splitting, it won't make any word
		// valid.
		ಆದರೆ (slang->sl_nobreak && !slang->sl_nocompoundsugs)
		    try_compound = TRUE;

		// If we could add a compound word, and it's also possible to
		// split at this point, do the split first and set
		// TSF_DIDSPLIT to avoid doing it again.
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!fword_ತುದಿಗಳು
			&& try_compound
			&& (sp->ts_flags & TSF_DIDSPLIT) == 0)
		{
		    try_compound = FALSE;
		    sp->ts_flags |= TSF_DIDSPLIT;
		    --sp->ts_curi;	    // do the same NUL again
		    compflags[sp->ts_complen] = NUL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    sp->ts_flags &= ~TSF_DIDSPLIT;

		ಆದರೆ (try_ವಿಭಜಿಸು || try_compound)
		{
		    ಆದರೆ (!try_compound && (!fword_ತುದಿಗಳು || !goodword_ತುದಿಗಳು))
		    {
			// If we're going to split need to check that the
			// words so far are valid for compounding.  If there
			// is only one word it must not have the NEEDCOMPOUND
			// flag.
			ಆದರೆ (sp->ts_complen == sp->ts_compsplit
						     && (flags & WF_NEEDCOMP))
			    ನಿಲ್ಲಿಸು;
			p = preword;
			ಆಗ (*skiptowhite(p) != NUL)
			    p = skipwhite(skiptowhite(p));
			ಆದರೆ (sp->ts_complen > sp->ts_compsplit
				&& !can_compound(slang, p,
						compflags + sp->ts_compsplit))
			    ನಿಲ್ಲಿಸು;

			ಆದರೆ (slang->sl_nosplitsugs)
			    newscore += SCORE_ವಿಭಜಿಸು_NO;
			ಇಲ್ಲದಿದ್ದರೆ
			    newscore += SCORE_ವಿಭಜಿಸು;

			// Give a bonus to words seen before.
			newscore = score_wordcount_adj(slang, newscore,
					   preword + sp->ts_prewordlen, TRUE);
		    }

		    ಆದರೆ (TRY_DEEPER(su, ರಾಶಿ, ಆಳ, newscore))
		    {
			go_deeper(ರಾಶಿ, ಆಳ, newscore);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
			ಆದರೆ (!try_compound && !fword_ತುದಿಗಳು)
			    sprintf(changename[ಆಳ], "%.*s-%s: split",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
			ಇಲ್ಲದಿದ್ದರೆ
			    sprintf(changename[ಆಳ], "%.*s-%s: compound",
				 sp->ts_twordlen, tword, fword + sp->ts_fidx);
#ಆದರೆ_ಅಂತ್ಯ
			// Save things to be restored at STATE_SPLITUNDO.
			sp->ts_save_badflags = su->su_badflags;
			PROF_STORE(sp->ts_state)
			sp->ts_state = STATE_SPLITUNDO;

			++ಆಳ;
			sp = &ರಾಶಿ[ಆಳ];

			// Append a space to preword when splitting.
			ಆದರೆ (!try_compound && !fword_ತುದಿಗಳು)
			    STRCAT(preword, " ");
			sp->ts_prewordlen = (char_u)STRLEN(preword);
			sp->ts_splitoff = sp->ts_twordlen;
			sp->ts_splitfidx = sp->ts_fidx;

			// If the badword has a non-word character at this
			// position skip it.  That means replacing the
			// non-word character with a space.  Always skip a
			// character when the word ends.  But only when the
			// good word can end.
			ಆದರೆ (((!try_compound && !spell_iswordp_nmw(fword
							       + sp->ts_fidx,
							       curwin))
				    || fword_ತುದಿಗಳು)
				&& fword[sp->ts_fidx] != NUL
				&& goodword_ತುದಿಗಳು)
			{
			    ಪೂರ್ಣಾಂಕ	    l;

			    l = mb_ptr2len(fword + sp->ts_fidx);
			    ಆದರೆ (fword_ತುದಿಗಳು)
			    {
				// Copy the skipped character to preword.
				mch_memmove(preword + sp->ts_prewordlen,
						      fword + sp->ts_fidx, l);
				sp->ts_prewordlen += l;
				preword[sp->ts_prewordlen] = NUL;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				sp->ts_score -= SCORE_ವಿಭಜಿಸು - SCORE_SUBST;
			    sp->ts_fidx += l;
			}

			// When compounding include compound flag in
			// compflags[] (already set above).  When splitting we
			// may start compounding over again.
			ಆದರೆ (try_compound)
			    ++sp->ts_complen;
			ಇಲ್ಲದಿದ್ದರೆ
			    sp->ts_compsplit = sp->ts_complen;
			sp->ts_prefixdepth = PFD_NOPREFIX;

			// set su->su_badflags to the caps type at this
			// position
			ಆದರೆ (has_mbyte)
			    n = nofold_len(fword, sp->ts_fidx, su->su_badptr);
			ಇಲ್ಲದಿದ್ದರೆ
			    n = sp->ts_fidx;
			su->su_badflags = badword_captype(su->su_badptr + n,
					       su->su_badptr + su->su_badlen);

			// Restart at top of the tree.
			sp->ts_arridx = 0;

			// If there are postponed prefixes, try these too.
			ಆದರೆ (pbyts != ಖಾಲಿ)
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
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_SPLITUNDO:
	    // Undo the changes done for word split or compound word.
	    su->su_badflags = sp->ts_save_badflags;

	    // Continue looking for NUL bytes.
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_ಆರಂಭ;

	    // In case we went into the prefix tree.
	    byts = fbyts;
	    idxs = fidxs;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_ENDNUL:
	    // Past the NUL bytes in the node.
	    su->su_badflags = sp->ts_save_badflags;
	    ಆದರೆ (fword[sp->ts_fidx] == NUL && sp->ts_tcharlen == 0)
	    {
		// The badword ends, can't use STATE_PLAIN.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_DEL;
		ನಿಲ್ಲಿಸು;
	    }
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_PLAIN;
	    // FALLTHROUGH

	ಸ್ಥಿತಿ STATE_PLAIN:
	    // Go over all possible bytes at this node, add each to tword[]
	    // and use child node.  "ts_curi" is the index.
	    arridx = sp->ts_arridx;
	    ಆದರೆ (sp->ts_curi > byts[arridx])
	    {
		// Done all bytes at this node, do next state.  When still at
		// already changed bytes skip the other tricks.
		PROF_STORE(sp->ts_state)
		ಆದರೆ (sp->ts_fidx >= sp->ts_fidxtry)
		    sp->ts_state = STATE_DEL;
		ಇಲ್ಲದಿದ್ದರೆ
		    sp->ts_state = STATE_FINAL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		arridx += sp->ts_curi++;
		c = byts[arridx];

		// Normal byte, go one level deeper.  If it's not equal to the
		// byte in the bad word adjust the score.  But don't even try
		// when the byte was already changed.  And don't try when we
		// just deleted this byte, accepting it is always cheaper than
		// delete + substitute.
		ಆದರೆ (c == fword[sp->ts_fidx]
			|| (sp->ts_tcharlen > 0 && sp->ts_isdiff != DIFF_NONE))
		    newscore = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    newscore = SCORE_SUBST;
		ಆದರೆ ((newscore == 0
			    || (sp->ts_fidx >= sp->ts_fidxtry
				&& ((sp->ts_flags & TSF_DIDDEL) == 0
				    || c != fword[sp->ts_delidx])))
			&& TRY_DEEPER(su, ರಾಶಿ, ಆಳ, newscore))
		{
		    go_deeper(ರಾಶಿ, ಆಳ, newscore);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		    ಆದರೆ (newscore > 0)
			sprintf(changename[ಆಳ], "%.*s-%s: subst %c to %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx], c);
		    ಇಲ್ಲದಿದ್ದರೆ
			sprintf(changename[ಆಳ], "%.*s-%s: accept %c",
				sp->ts_twordlen, tword, fword + sp->ts_fidx,
				fword[sp->ts_fidx]);
#ಆದರೆ_ಅಂತ್ಯ
		    ++ಆಳ;
		    sp = &ರಾಶಿ[ಆಳ];
		    ಆದರೆ (fword[sp->ts_fidx] != NUL)
			++sp->ts_fidx;
		    tword[sp->ts_twordlen++] = c;
		    sp->ts_arridx = idxs[arridx];
		    ಆದರೆ (newscore == SCORE_SUBST)
			sp->ts_isdiff = DIFF_YES;
		    ಆದರೆ (has_mbyte)
		    {
			// Multi-byte characters are a bit complicated to
			// handle: They differ when any of the bytes differ
			// and then their length may also differ.
			ಆದರೆ (sp->ts_tcharlen == 0)
			{
			    // First byte.
			    sp->ts_tcharidx = 0;
			    sp->ts_tcharlen = MB_BYTE2LEN(c);
			    sp->ts_fcharstart = sp->ts_fidx - 1;
			    sp->ts_isdiff = (newscore != 0)
						       ? DIFF_YES : DIFF_NONE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp->ts_isdiff == DIFF_ಸೇರಿಸು
							    && sp->ts_fidx > 0)
			    // When inserting trail bytes don't advance in the
			    // bad word.
			    --sp->ts_fidx;
			ಆದರೆ (++sp->ts_tcharidx == sp->ts_tcharlen)
			{
			    // Last byte of character.
			    ಆದರೆ (sp->ts_isdiff == DIFF_YES)
			    {
				// Correct ts_fidx for the byte length of the
				// character (we didn't check that before).
				sp->ts_fidx = sp->ts_fcharstart
					    + mb_ptr2len(
						    fword + sp->ts_fcharstart);
				// For changing a composing character adjust
				// the score from SCORE_SUBST to
				// SCORE_SUBCOMP.
				ಆದರೆ (enc_utf8
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
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!soundfold
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
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp->ts_isdiff == DIFF_ಸೇರಿಸು
					 && sp->ts_twordlen > sp->ts_tcharlen)
			    {
				p = tword + sp->ts_twordlen - sp->ts_tcharlen;
				c = mb_ptr2char(p);
				ಆದರೆ (enc_utf8 && utf_iscomposing(c))
				{
				    // Inserting a composing char doesn't
				    // count that much.
				    sp->ts_score -= SCORE_INS - SCORE_INSCOMP;
				}
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    // If the previous character was the same,
				    // thus doubling a character, give a bonus
				    // to the score.  Also for the soundfold
				    // tree (might seem illogical but does
				    // give better scores).
				    MB_PTR_ಹಿಂಭಾಗ(tword, p);
				    ಆದರೆ (c == mb_ptr2char(p))
					sp->ts_score -= SCORE_INS
							       - SCORE_INSDUP;
				}
			    }

			    // Starting a new char, reset the length.
			    sp->ts_tcharlen = 0;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// If we found a similar char adjust the score.
			// We do this after calling go_deeper() because
			// it's slow.
			ಆದರೆ (newscore != 0
				&& !soundfold
				&& slang->sl_has_map
				&& similar_chars(slang,
						   c, fword[sp->ts_fidx - 1]))
			    sp->ts_score -= SCORE_SUBST - SCORE_SIMILAR;
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_DEL:
	    // When past the first byte of a multi-byte char don't try
	    // delete/insert/swap a character.
	    ಆದರೆ (has_mbyte && sp->ts_tcharlen > 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ನಿಲ್ಲಿಸು;
	    }
	    // Try skipping one character in the bad word (delete it).
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_INS_PREP;
	    sp->ts_curi = 1;
	    ಆದರೆ (soundfold && sp->ts_fidx == 0 && fword[sp->ts_fidx] == '*')
		// Deleting a vowel at the start of a word counts less, see
		// soundalike_score().
		newscore = 2 * SCORE_DEL / 3;
	    ಇಲ್ಲದಿದ್ದರೆ
		newscore = SCORE_DEL;
	    ಆದರೆ (fword[sp->ts_fidx] != NUL
				    && TRY_DEEPER(su, ರಾಶಿ, ಆಳ, newscore))
	    {
		go_deeper(ರಾಶಿ, ಆಳ, newscore);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		sprintf(changename[ಆಳ], "%.*s-%s: delete %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			fword[sp->ts_fidx]);
#ಆದರೆ_ಅಂತ್ಯ
		++ಆಳ;

		// Remember what character we deleted, so that we can avoid
		// inserting it again.
		ರಾಶಿ[ಆಳ].ts_flags |= TSF_DIDDEL;
		ರಾಶಿ[ಆಳ].ts_delidx = sp->ts_fidx;

		// Advance over the character in fword[].  Give a bonus to the
		// score if the same character is following "nn" -> "n".  It's
		// a bit illogical for soundfold tree but it does give better
		// results.
		ಆದರೆ (has_mbyte)
		{
		    c = mb_ptr2char(fword + sp->ts_fidx);
		    ರಾಶಿ[ಆಳ].ts_fidx += mb_ptr2len(fword + sp->ts_fidx);
		    ಆದರೆ (enc_utf8 && utf_iscomposing(c))
			ರಾಶಿ[ಆಳ].ts_score -= SCORE_DEL - SCORE_DELCOMP;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == mb_ptr2char(fword + ರಾಶಿ[ಆಳ].ts_fidx))
			ರಾಶಿ[ಆಳ].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ++ರಾಶಿ[ಆಳ].ts_fidx;
		    ಆದರೆ (fword[sp->ts_fidx] == fword[sp->ts_fidx + 1])
			ರಾಶಿ[ಆಳ].ts_score -= SCORE_DEL - SCORE_DELDUP;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    // FALLTHROUGH

	ಸ್ಥಿತಿ STATE_INS_PREP:
	    ಆದರೆ (sp->ts_flags & TSF_DIDDEL)
	    {
		// If we just deleted a byte then inserting won't make sense,
		// a substitute is always cheaper.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ಅದಲುಬದಲು;
		ನಿಲ್ಲಿಸು;
	    }

	    // skip over NUL bytes
	    n = sp->ts_arridx;
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (sp->ts_curi > byts[n])
		{
		    // Only NUL bytes at this node, go to next state.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_ಅದಲುಬದಲು;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (byts[n + sp->ts_curi] != NUL)
		{
		    // Found a byte to insert.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_INS;
		    ನಿಲ್ಲಿಸು;
		}
		++sp->ts_curi;
	    }
	    ನಿಲ್ಲಿಸು;

	    // FALLTHROUGH

	ಸ್ಥಿತಿ STATE_INS:
	    // Insert one byte.  Repeat this for each possible byte at this
	    // node.
	    n = sp->ts_arridx;
	    ಆದರೆ (sp->ts_curi > byts[n])
	    {
		// Done all bytes at this node, go to next state.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_ಅದಲುಬದಲು;
		ನಿಲ್ಲಿಸು;
	    }

	    // Do one more byte at this node, but:
	    // - Skip NUL bytes.
	    // - Skip the byte if it's equal to the byte in the word,
	    //   accepting that byte is always better.
	    n += sp->ts_curi++;

	    // break out, if we would be accessing byts buffer out of bounds
	    ಆದರೆ (byts == slang->sl_fbyts && n >= slang->sl_fbyts_len)
	    {
		got_int = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    c = byts[n];
	    ಆದರೆ (soundfold && sp->ts_twordlen == 0 && c == '*')
		// Inserting a vowel at the start of a word counts less,
		// see soundalike_score().
		newscore = 2 * SCORE_INS / 3;
	    ಇಲ್ಲದಿದ್ದರೆ
		newscore = SCORE_INS;
	    ಆದರೆ (c != fword[sp->ts_fidx]
				    && TRY_DEEPER(su, ರಾಶಿ, ಆಳ, newscore))
	    {
		go_deeper(ರಾಶಿ, ಆಳ, newscore);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		sprintf(changename[ಆಳ], "%.*s-%s: insert %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c);
#ಆದರೆ_ಅಂತ್ಯ
		++ಆಳ;
		sp = &ರಾಶಿ[ಆಳ];
		tword[sp->ts_twordlen++] = c;
		sp->ts_arridx = idxs[n];
		ಆದರೆ (has_mbyte)
		{
		    fl = MB_BYTE2LEN(c);
		    ಆದರೆ (fl > 1)
		    {
			// There are following bytes for the same character.
			// We must find all bytes before trying
			// delete/insert/swap/etc.
			sp->ts_tcharlen = fl;
			sp->ts_tcharidx = 1;
			sp->ts_isdiff = DIFF_ಸೇರಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    fl = 1;
		ಆದರೆ (fl == 1)
		{
		    // If the previous character was the same, thus doubling a
		    // character, give a bonus to the score.  Also for
		    // soundfold words (illogical but does give a better
		    // score).
		    ಆದರೆ (sp->ts_twordlen >= 2
					   && tword[sp->ts_twordlen - 2] == c)
			sp->ts_score -= SCORE_INS - SCORE_INSDUP;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_ಅದಲುಬದಲು:
	    // Swap two bytes in the bad word: "12" -> "21".
	    // We change "fword" here, it's changed back afterwards at
	    // STATE_UNSWAP.
	    p = fword + sp->ts_fidx;
	    c = *p;
	    ಆದರೆ (c == NUL)
	    {
		// End of word, can't swap or replace.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ನಿಲ್ಲಿಸು;
	    }

	    // Don't swap if the first character is not a word character.
	    // SWAP3 etc. also don't make sense then.
	    ಆದರೆ (!soundfold && !spell_iswordp(p, curwin))
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		ಆದರೆ (p[n] == NUL)
		    c2 = NUL;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!soundfold && !spell_iswordp(p + n, curwin))
		    c2 = c; // don't swap non-word char
		ಇಲ್ಲದಿದ್ದರೆ
		    c2 = mb_ptr2char(p + n);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (p[1] == NUL)
		    c2 = NUL;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!soundfold && !spell_iswordp(p + 1, curwin))
		    c2 = c; // don't swap non-word char
		ಇಲ್ಲದಿದ್ದರೆ
		    c2 = p[1];
	    }

	    // When the second character is NUL we can't swap.
	    ಆದರೆ (c2 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ನಿಲ್ಲಿಸು;
	    }

	    // When characters are identical, swap won't do anything.
	    // Also get here if the second char is not a word character.
	    ಆದರೆ (c == c2)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_SWAP3;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (c2 != NUL && TRY_DEEPER(su, ರಾಶಿ, ಆಳ, SCORE_ಅದಲುಬದಲು))
	    {
		go_deeper(ರಾಶಿ, ಆಳ, SCORE_ಅದಲುಬದಲು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		sprintf(changename[ಆಳ], "%.*s-%s: swap %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c2);
#ಆದರೆ_ಅಂತ್ಯ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP;
		++ಆಳ;
		ಆದರೆ (has_mbyte)
		{
		    fl = mb_char2len(c2);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    p[0] = c2;
		    p[1] = c;
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + 2;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// If this swap doesn't work then SWAP3 won't either.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_UNSWAP:
	    // Undo the STATE_SWAP swap: "21" -> "12".
	    p = fword + sp->ts_fidx;
	    ಆದರೆ (has_mbyte)
	    {
		n = mb_ptr2len(p);
		c = mb_ptr2char(p + n);
		mch_memmove(p + mb_ptr2len(p + n), p, n);
		mb_char2bytes(c, p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		c = *p;
		*p = p[1];
		p[1] = c;
	    }
	    // FALLTHROUGH

	ಸ್ಥಿತಿ STATE_SWAP3:
	    // Swap two bytes, skipping one: "123" -> "321".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNSWAP3.
	    p = fword + sp->ts_fidx;
	    ಆದರೆ (has_mbyte)
	    {
		n = MB_CPTR2LEN(p);
		c = mb_ptr2char(p);
		fl = MB_CPTR2LEN(p + n);
		c2 = mb_ptr2char(p + n);
		ಆದರೆ (!soundfold && !spell_iswordp(p + n + fl, curwin))
		    c3 = c;	// don't swap non-word char
		ಇಲ್ಲದಿದ್ದರೆ
		    c3 = mb_ptr2char(p + n + fl);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		c = *p;
		c2 = p[1];
		ಆದರೆ (!soundfold && !spell_iswordp(p + 2, curwin))
		    c3 = c;	// don't swap non-word char
		ಇಲ್ಲದಿದ್ದರೆ
		    c3 = p[2];
	    }

	    // When characters are identical: "121" then SWAP3 result is
	    // identical, ROT3L result is same as SWAP: "211", ROT3L result is
	    // same as SWAP on next char: "112".  Thus skip all swapping.
	    // Also skip when c3 is NUL.
	    // Also get here when the third character is not a word character.
	    // Second character may any char: "a.b" -> "b.a"
	    ಆದರೆ (c == c3 || c3 == NUL)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (TRY_DEEPER(su, ರಾಶಿ, ಆಳ, SCORE_SWAP3))
	    {
		go_deeper(ರಾಶಿ, ಆಳ, SCORE_SWAP3);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		sprintf(changename[ಆಳ], "%.*s-%s: swap3 %c and %c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			c, c3);
#ಆದರೆ_ಅಂತ್ಯ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNSWAP3;
		++ಆಳ;
		ಆದರೆ (has_mbyte)
		{
		    tl = mb_char2len(c3);
		    mch_memmove(p, p + n + fl, tl);
		    mb_char2bytes(c2, p + tl);
		    mb_char2bytes(c, p + fl + tl);
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + n + fl + tl;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    p[0] = p[2];
		    p[2] = c;
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_UNSWAP3:
	    // Undo STATE_SWAP3: "321" -> "123"
	    p = fword + sp->ts_fidx;
	    ಆದರೆ (has_mbyte)
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
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		c = *p;
		*p = p[2];
		p[2] = c;
		++p;
	    }

	    ಆದರೆ (!soundfold && !spell_iswordp(p, curwin))
	    {
		// Middle char is not a word char, skip the rotate.  First and
		// third char were already checked at swap and swap3.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
		ನಿಲ್ಲಿಸು;
	    }

	    // Rotate three characters left: "123" -> "231".  We change
	    // "fword" here, it's changed back afterwards at STATE_UNROT3L.
	    ಆದರೆ (TRY_DEEPER(su, ರಾಶಿ, ಆಳ, SCORE_SWAP3))
	    {
		go_deeper(ರಾಶಿ, ಆಳ, SCORE_SWAP3);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[ಆಳ], "%.*s-%s: rotate left %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#ಆದರೆ_ಅಂತ್ಯ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3L;
		++ಆಳ;
		p = fword + sp->ts_fidx;
		ಆದರೆ (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    c = mb_ptr2char(p);
		    fl = MB_CPTR2LEN(p + n);
		    fl += MB_CPTR2LEN(p + n + fl);
		    mch_memmove(p, p + n, fl);
		    mb_char2bytes(c, p + fl);
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + n + fl;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    c = *p;
		    *p = p[1];
		    p[1] = p[2];
		    p[2] = c;
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_UNROT3L:
	    // Undo ROT3L: "231" -> "123"
	    p = fword + sp->ts_fidx;
	    ಆದರೆ (has_mbyte)
	    {
		n = mb_ptr2len(p);
		n += mb_ptr2len(p + n);
		c = mb_ptr2char(p + n);
		tl = mb_ptr2len(p + n);
		mch_memmove(p + tl, p, n);
		mb_char2bytes(c, p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		c = p[2];
		p[2] = p[1];
		p[1] = *p;
		*p = c;
	    }

	    // Rotate three bytes right: "123" -> "312".  We change "fword"
	    // here, it's changed back afterwards at STATE_UNROT3R.
	    ಆದರೆ (TRY_DEEPER(su, ರಾಶಿ, ಆಳ, SCORE_SWAP3))
	    {
		go_deeper(ರಾಶಿ, ಆಳ, SCORE_SWAP3);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		p = fword + sp->ts_fidx;
		sprintf(changename[ಆಳ], "%.*s-%s: rotate right %c%c%c",
			sp->ts_twordlen, tword, fword + sp->ts_fidx,
			p[0], p[1], p[2]);
#ಆದರೆ_ಅಂತ್ಯ
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_UNROT3R;
		++ಆಳ;
		p = fword + sp->ts_fidx;
		ಆದರೆ (has_mbyte)
		{
		    n = MB_CPTR2LEN(p);
		    n += MB_CPTR2LEN(p + n);
		    c = mb_ptr2char(p + n);
		    tl = MB_CPTR2LEN(p + n);
		    mch_memmove(p + tl, p, n);
		    mb_char2bytes(c, p);
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + n + tl;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    c = p[2];
		    p[2] = p[1];
		    p[1] = *p;
		    *p = c;
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + 3;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_REP_INI;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_UNROT3R:
	    // Undo ROT3R: "312" -> "123"
	    p = fword + sp->ts_fidx;
	    ಆದರೆ (has_mbyte)
	    {
		c = mb_ptr2char(p);
		tl = mb_ptr2len(p);
		n = mb_ptr2len(p + tl);
		n += mb_ptr2len(p + tl + n);
		mch_memmove(p, p + tl, n);
		mb_char2bytes(c, p + n);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		c = *p;
		*p = p[1];
		p[1] = p[2];
		p[2] = c;
	    }
	    // FALLTHROUGH

	ಸ್ಥಿತಿ STATE_REP_INI:
	    // Check if matching with REP items from the .aff file would work.
	    // Quickly skip if:
	    // - there are no REP items and we are not in the soundfold trie
	    // - the score is going to be too high anyway
	    // - already applied a REP item or swapped here
	    ಆದರೆ ((lp->lp_replang == ಖಾಲಿ && !soundfold)
		    || sp->ts_score + SCORE_REP >= su->su_maxscore
		    || sp->ts_fidx < sp->ts_fidxtry)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ನಿಲ್ಲಿಸು;
	    }

	    // Use the first byte to quickly find the first entry that may
	    // match.  If the index is -1 there is none.
	    ಆದರೆ (soundfold)
		sp->ts_curi = slang->sl_repsal_ಮೊದಲ[fword[sp->ts_fidx]];
	    ಇಲ್ಲದಿದ್ದರೆ
		sp->ts_curi = lp->lp_replang->sl_rep_ಮೊದಲ[fword[sp->ts_fidx]];

	    ಆದರೆ (sp->ts_curi < 0)
	    {
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
		ನಿಲ್ಲಿಸು;
	    }

	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    // FALLTHROUGH

	ಸ್ಥಿತಿ STATE_REP:
	    // Try matching with REP items from the .aff file.  For each match
	    // replace the characters and check if the resulting word is
	    // valid.
	    p = fword + sp->ts_fidx;

	    ಆದರೆ (soundfold)
		gap = &slang->sl_repsal;
	    ಇಲ್ಲದಿದ್ದರೆ
		gap = &lp->lp_replang->sl_rep;
	    ಆಗ (sp->ts_curi < gap->ga_len)
	    {
		ftp = (fromto_T *)gap->ga_ದತ್ತಾಂಶ + sp->ts_curi++;
		ಆದರೆ (*ftp->ft_from != *p)
		{
		    // past possible matching entries
		    sp->ts_curi = gap->ga_len;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (STRNCMP(ftp->ft_from, p, STRLEN(ftp->ft_from)) == 0
			&& TRY_DEEPER(su, ರಾಶಿ, ಆಳ, SCORE_REP))
		{
		    go_deeper(ರಾಶಿ, ಆಳ, SCORE_REP);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TRIEWALK
		    sprintf(changename[ಆಳ], "%.*s-%s: replace %s with %s",
			    sp->ts_twordlen, tword, fword + sp->ts_fidx,
			    ftp->ft_from, ftp->ft_ಗಮ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
		    // Need to undo this afterwards.
		    PROF_STORE(sp->ts_state)
		    sp->ts_state = STATE_REP_UNDO;

		    // Change the "from" to the "to" string.
		    ++ಆಳ;
		    fl = (ಪೂರ್ಣಾಂಕ)STRLEN(ftp->ft_from);
		    tl = (ಪೂರ್ಣಾಂಕ)STRLEN(ftp->ft_ಗಮ್ಯ);
		    ಆದರೆ (fl != tl)
		    {
			STRMOVE(p + tl, p + fl);
			repextra += tl - fl;
		    }
		    mch_memmove(p, ftp->ft_ಗಮ್ಯ, tl);
		    ರಾಶಿ[ಆಳ].ts_fidxtry = sp->ts_fidx + tl;
		    ರಾಶಿ[ಆಳ].ts_tcharlen = 0;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	    ಆದರೆ (sp->ts_curi >= gap->ga_len && sp->ts_state == STATE_REP)
	    {
		// No (more) matches.
		PROF_STORE(sp->ts_state)
		sp->ts_state = STATE_FINAL;
	    }

	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE_REP_UNDO:
	    // Undo a REP replacement and continue with the next one.
	    ಆದರೆ (soundfold)
		gap = &slang->sl_repsal;
	    ಇಲ್ಲದಿದ್ದರೆ
		gap = &lp->lp_replang->sl_rep;
	    ftp = (fromto_T *)gap->ga_ದತ್ತಾಂಶ + sp->ts_curi - 1;
	    fl = (ಪೂರ್ಣಾಂಕ)STRLEN(ftp->ft_from);
	    tl = (ಪೂರ್ಣಾಂಕ)STRLEN(ftp->ft_ಗಮ್ಯ);
	    p = fword + sp->ts_fidx;
	    ಆದರೆ (fl != tl)
	    {
		STRMOVE(p + fl, p + tl);
		repextra -= tl - fl;
	    }
	    mch_memmove(p, ftp->ft_from, fl);
	    PROF_STORE(sp->ts_state)
	    sp->ts_state = STATE_REP;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    // Did all possible states at this level, go up one level.
	    --ಆಳ;

	    ಆದರೆ (ಆಳ >= 0 && ರಾಶಿ[ಆಳ].ts_prefixdepth == PFD_PREFIXTREE)
	    {
		// Continue in or go back to the prefix tree.
		byts = pbyts;
		idxs = pidxs;
	    }

	    // Don't check for CTRL-C too often, it takes time.
	    ಆದರೆ (--breakcheckcount == 0)
	    {
		ui_breakcheck();
		breakcheckcount = 1000;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
		ಆದರೆ (spell_suggest_timeout > 0
					  && profile_passed_ಎಲ್ಲೆ(&ಸಮಯ_ಎಲ್ಲೆ))
		    got_int = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
    }
}


/*
 * Go one level deeper in the tree.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
go_deeper(trystate_T *ರಾಶಿ, ಪೂರ್ಣಾಂಕ ಆಳ, ಪೂರ್ಣಾಂಕ score_ಕೂಡಿಸು)
{
    ರಾಶಿ[ಆಳ + 1] = ರಾಶಿ[ಆಳ];
    ರಾಶಿ[ಆಳ + 1].ts_state = STATE_ಆರಂಭ;
    ರಾಶಿ[ಆಳ + 1].ts_score = ರಾಶಿ[ಆಳ].ts_score + score_ಕೂಡಿಸು;
    ರಾಶಿ[ಆಳ + 1].ts_curi = 1;	// start just after length byte
    ರಾಶಿ[ಆಳ + 1].ts_flags = 0;
}

/*
 * "fword" is a good word with case folded.  Find the matching keep-case
 * words and put it in "kword".
 * Theoretically there could be several keep-case words that result in the
 * same case-folded word, but we only find one...
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_keepcap_ಶಬ್ದ(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    ಪೂರ್ಣಾಂಕ		ಆಳ;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		fwordidx[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		uwordidx[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		kwordlen[MAXWLEN];

    ಪೂರ್ಣಾಂಕ		flen, ulen;
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    ಆದರೆ (byts == ಖಾಲಿ)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	ಹಿಂತಿರುಗಿ;
    }

    // Make an all-cap version of "fword".
    allcap_copy(fword, uword);

    // Each character needs to be tried both case-folded and upper-case.
    // All this gets very complicated if we keep in mind that changing case
    // may change the byte length of a multi-byte character...
    ಆಳ = 0;
    arridx[0] = 0;
    ಸುತ್ತು[0] = 0;
    fwordidx[0] = 0;
    uwordidx[0] = 0;
    kwordlen[0] = 0;
    ಆಗ (ಆಳ >= 0)
    {
	ಆದರೆ (fword[fwordidx[ಆಳ]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    ಆದರೆ (byts[arridx[ಆಳ] + 1] == 0)
	    {
		kword[kwordlen[ಆಳ]] = NUL;
		ಹಿಂತಿರುಗಿ;
	    }

	    // kword is getting too long, continue one level up
	    --ಆಳ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (++ಸುತ್ತು[ಆಳ] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --ಆಳ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    ಆದರೆ (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[ಆಳ]);
		ulen = MB_CPTR2LEN(uword + uwordidx[ಆಳ]);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ulen = flen = 1;
	    ಆದರೆ (ಸುತ್ತು[ಆಳ] == 1)
	    {
		p = fword + fwordidx[ಆಳ];
		l = flen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		p = uword + uwordidx[ಆಳ];
		l = ulen;
	    }

	    ಗಾಗಿ (tryidx = arridx[ಆಳ]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
		ಆಗ (lo < hi)
		{
		    m = (lo + hi) / 2;
		    ಆದರೆ (byts[m] > c)
			hi = m - 1;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (byts[m] < c)
			lo = m + 1;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			lo = hi = m;
			ನಿಲ್ಲಿಸು;
		    }
		}

		// Stop if there is no matching byte.
		ಆದರೆ (hi < lo || byts[lo] != c)
		    ನಿಲ್ಲಿಸು;

		// Continue at the child (if there is one).
		tryidx = idxs[lo];
	    }

	    ಆದರೆ (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		ಆದರೆ (ಸುತ್ತು[ಆಳ] == 1)
		{
		    STRNCPY(kword + kwordlen[ಆಳ], fword + fwordidx[ಆಳ],
									flen);
		    kwordlen[ಆಳ + 1] = kwordlen[ಆಳ] + flen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    STRNCPY(kword + kwordlen[ಆಳ], uword + uwordidx[ಆಳ],
									ulen);
		    kwordlen[ಆಳ + 1] = kwordlen[ಆಳ] + ulen;
		}
		fwordidx[ಆಳ + 1] = fwordidx[ಆಳ] + flen;
		uwordidx[ಆಳ + 1] = uwordidx[ಆಳ] + ulen;

		++ಆಳ;
		arridx[ಆಳ] = tryidx;
		ಸುತ್ತು[ಆಳ] = 0;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    ಪೂರ್ಣಾಂಕ		score;
    ಪೂರ್ಣಾಂಕ		lpi;

    ಆದರೆ (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Use the sound-folding of the first language that supports it.
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	ಆದರೆ (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    ಗಾಗಿ (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		ಆದರೆ (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_ಶಬ್ದ = vim_strsave(stp->st_ಶಬ್ದ);
		    ಆದರೆ (sstp->st_ಶಬ್ದ != ಖಾಲಿ)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;
	}
    }
}

/*
 * Combine the list of suggestions in su->su_ga and su->su_sga.
 * They are entwined.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
score_combine(suginfo_T *su)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;
    garray_T	ga;
    garray_T	*gap;
    langp_T	*lp;
    suggest_T	*stp;
    char_u	*p;
    char_u	badsound[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		lpi;
    slang_T	*slang = ಖಾಲಿ;

    // Add the alternate score to su_ga.
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	ಆದರೆ (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    slang = lp->lp_slang;
	    spell_soundfold(slang, su->su_fbadword, TRUE, badsound);

	    ಗಾಗಿ (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);
		stp->st_altscore = stp_sal_score(stp, su, slang, badsound);
		ಆದರೆ (stp->st_altscore == SCORE_MAXMAX)
		    stp->st_score = (stp->st_score * 3 + SCORE_BIG) / 4;
		ಇಲ್ಲದಿದ್ದರೆ
		    stp->st_score = (stp->st_score * 3
						  + stp->st_altscore) / 4;
		stp->st_salscore = FALSE;
	    }
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (slang == ಖಾಲಿ)	// Using "double" without sound folding.
    {
	(ಶೂನ್ಯ)cleanup_suggestions(&su->su_ga, su->su_maxscore,
							     su->su_maxcount);
	ಹಿಂತಿರುಗಿ;
    }

    // Add the alternate score to su_sga.
    ಗಾಗಿ (i = 0; i < su->su_sga.ga_len; ++i)
    {
	stp = &SUG(su->su_sga, i);
	stp->st_altscore = spell_edit_score(slang,
						su->su_badword, stp->st_ಶಬ್ದ);
	ಆದರೆ (stp->st_score == SCORE_MAXMAX)
	    stp->st_score = (SCORE_BIG * 7 + stp->st_altscore) / 8;
	ಇಲ್ಲದಿದ್ದರೆ
	    stp->st_score = (stp->st_score * 7 + stp->st_altscore) / 8;
	stp->st_salscore = TRUE;
    }

    // Remove bad suggestions, sort the suggestions and truncate at "maxcount"
    // for both lists.
    check_suggestions(su, &su->su_ga);
    (ಶೂನ್ಯ)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    check_suggestions(su, &su->su_sga);
    (ಶೂನ್ಯ)cleanup_suggestions(&su->su_sga, su->su_maxscore, su->su_maxcount);

    ga_init2(&ga, ಗಾತ್ರ(suginfo_T), 1);
    ಆದರೆ (ga_grow(&ga, su->su_ga.ga_len + su->su_sga.ga_len) == FAIL)
	ಹಿಂತಿರುಗಿ;

    stp = &SUG(ga, 0);
    ಗಾಗಿ (i = 0; i < su->su_ga.ga_len || i < su->su_sga.ga_len; ++i)
    {
	// round 1: get a suggestion from su_ga
	// round 2: get a suggestion from su_sga
	ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	{
	    gap = ಸುತ್ತು == 1 ? &su->su_ga : &su->su_sga;
	    ಆದರೆ (i < gap->ga_len)
	    {
		// Don't add a word if it's already there.
		p = SUG(*gap, i).st_ಶಬ್ದ;
		ಗಾಗಿ (j = 0; j < ga.ga_len; ++j)
		    ಆದರೆ (STRCMP(stp[j].st_ಶಬ್ದ, p) == 0)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (j == ga.ga_len)
		    stp[ga.ga_len++] = SUG(*gap, i);
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_free(p);
	    }
	}
    }

    ga_clear(&su->su_ga);
    ga_clear(&su->su_sga);

    // Truncate the list to the number of suggestions that will be displayed.
    ಆದರೆ (ga.ga_len > su->su_maxcount)
    {
	ಗಾಗಿ (i = su->su_maxcount; i < ga.ga_len; ++i)
	    vim_free(stp[i].st_ಶಬ್ದ);
	ga.ga_len = su->su_maxcount;
    }

    su->su_ga = ga;
}

/*
 * For the goodword in "stp" compute the soundalike score compared to the
 * badword.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
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
    ಪೂರ್ಣಾಂಕ		lendiff;

    lendiff = (ಪೂರ್ಣಾಂಕ)(su->su_badlen - stp->st_orglen);
    ಆದರೆ (lendiff >= 0)
	pbad = badsound;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// soundfold the bad word with more characters following
	(ಶೂನ್ಯ)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	ಆದರೆ (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_ಶಬ್ದ) == NUL)
	    ಗಾಗಿ (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    ಆದರೆ (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_ಶಬ್ದ);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	pgood = stp->st_ಶಬ್ದ;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    ಹಿಂತಿರುಗಿ soundalike_score(goodsound, pbad);
}

// structure used to store soundfolded words that add_sound_suggest() has
// handled already.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಗಿಡ್ಡ	sft_score;	// lowest score used
    char_u	sft_ಶಬ್ದ[1];    // soundfolded word, actually longer
} sftword_T;

ಸ್ಥಿರ sftword_T dumsft;
#ವ್ಯಾಖ್ಯಾನಿಸು HIKEY2SFT(p)  ((sftword_T *)((p) - (dumsft.sft_ಶಬ್ದ - (char_u *)&dumsft)))
#ವ್ಯಾಖ್ಯಾನಿಸು HI2SFT(hi)     HIKEY2SFT((hi)->hi_ಕೀಲಿ)

/*
 * Prepare for calling suggest_try_soundalike().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suggest_try_soundalike_prep(ಶೂನ್ಯ)
{
    langp_T	*lp;
    ಪೂರ್ಣಾಂಕ		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ಆದರೆ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ಖಾಲಿ)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}

/*
 * Find suggestions by comparing the word in a sound-a-like form.
 * Note: This doesn't support postponed prefixes.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suggest_try_soundalike(suginfo_T *su)
{
    char_u	salword[MAXWLEN];
    langp_T	*lp;
    ಪೂರ್ಣಾಂಕ		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ಆದರೆ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ಖಾಲಿ)
	{
	    // soundfold the bad word
	    spell_soundfold(slang, su->su_fbadword, TRUE, salword);

	    // try all kinds of inserts/deletes/swaps/etc.
	    // TODO: also soundfold the next words, so that we can try joining
	    // and splitting
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SUGGEST_PROFILE
	    prof_init();
#ಆದರೆ_ಅಂತ್ಯ
	    suggest_trie_walk(su, lp, salword, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SUGGEST_PROFILE
	    prof_report("soundalike");
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
}

/*
 * Finish up after calling suggest_try_soundalike().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suggest_try_soundalike_ಮುಕ್ತಾಯ(ಶೂನ್ಯ)
{
    langp_T	*lp;
    ಪೂರ್ಣಾಂಕ		lpi;
    slang_T	*slang;
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    ಗಾಗಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ನಮೂದು(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ಆದರೆ (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != ಖಾಲಿ)
	{
	    // Free the info about handled words.
	    todo = (ಪೂರ್ಣಾಂಕ)slang->sl_sounddone.ht_ಬಳಸಿದ;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&slang->sl_sounddone, hi, todo)
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    ಪೂರ್ಣಾಂಕ		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    ಪೂರ್ಣಾಂಕ		sfwordnr;
    char_u	*nrline;
    ಪೂರ್ಣಾಂಕ		orgnr;
    char_u	theword[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		wlen;
    char_u	*byts;
    idx_T	*idxs;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		wordcount;
    ಪೂರ್ಣಾಂಕ		wc;
    ಪೂರ್ಣಾಂಕ		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    ಪೂರ್ಣಾಂಕ		bc, gc;
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	sft = alloc(offsetof(sftword_T, sft_ಶಬ್ದ) + STRLEN(goodword) + 1);
	ಆದರೆ (sft != ಖಾಲಿ)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_ಶಬ್ದ, goodword);
	    hash_ಕೂಡಿಸು_ಅಂಶ(&slang->sl_sounddone, hi, sft->sft_ಶಬ್ದ, hash);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	sft = HI2SFT(hi);
	ಆದರೆ (score >= sft->sft_score)
	    ಹಿಂತಿರುಗಿ;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_ಕಂಡುಹಿಡಿ(slang, goodword);
    ಆದರೆ (sfwordnr < 0)
    {
	ಆಂತರಿಕ_error("add_sound_suggest()");
	ಹಿಂತಿರುಗಿ;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    ಆಗ (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	ಗಾಗಿ (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    ಆದರೆ (wordcount == orgnr && byts[n + 1] == NUL)
		ನಿಲ್ಲಿಸು;	// found end of word

	    ಆದರೆ (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    ಗಾಗಿ ( ; byts[n + i] == NUL; ++i)
		ಆದರೆ (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    ಹೋಗು badword;
		}

	    // One of the siblings must have the word.
	    ಗಾಗಿ ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		ಆದರೆ (wordcount + wc > orgnr)
		    ನಿಲ್ಲಿಸು;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	ಗಾಗಿ (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    ಪೂರ್ಣಾಂಕ		flags = (ಪೂರ್ಣಾಂಕ)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    ಆದರೆ (flags & WF_NOSUGGEST)
		ಮುಂದುವರಿ;

	    ಆದರೆ (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		ಕಂಡುಹಿಡಿ_keepcap_ಶಬ್ದ(slang, theword, cword);
		p = cword;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		flags |= su->su_badflags;
		ಆದರೆ ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_ಶಬ್ದ(theword, cword, flags);
		    p = cword;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    p = theword;
	    }

	    // Add the suggestion.
	    ಆದರೆ (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		ಆದರೆ (score <= su->su_maxscore)
		    ಕೂಡಿಸು_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Add a penalty for words in another region.
		ಆದರೆ ((flags & WF_REGION)
			    && (((ಚಿಹ್ನೆರಹಿತ)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		ಇಲ್ಲದಿದ್ದರೆ
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		ಆದರೆ (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    ಆದರೆ (!SPELL_ISUPPER(bc)
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
		ಎಲ್ಲೆ = MAXSCORE(su->su_sfmaxscore - goodscore, score);
		ಆದರೆ (ಎಲ್ಲೆ > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		ಇಲ್ಲದಿದ್ದರೆ
		    goodscore += spell_edit_score_ಎಲ್ಲೆ(slang, su->su_badword,
								    p, ಎಲ್ಲೆ);

		// When going over the limit don't bother to do the rest.
		ಆದರೆ (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    ಆದರೆ (goodscore <= su->su_sfmaxscore)
			ಕೂಡಿಸು_suggestion(su, &su->su_ga, p, su->su_badlen,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
soundfold_ಕಂಡುಹಿಡಿ(slang_T *slang, char_u *ಶಬ್ದ)
{
    idx_T	arridx = 0;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		wlen = 0;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*ptr = ಶಬ್ದ;
    char_u	*byts;
    idx_T	*idxs;
    ಪೂರ್ಣಾಂಕ		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    ಗಾಗಿ (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	ಆದರೆ (byts[arridx] == NUL)
	{
	    ಆದರೆ (c == NUL)
		ನಿಲ್ಲಿಸು;

	    // Skip over the zeros, there can be several.
	    ಆಗ (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    ಆದರೆ (len == 0)
		ಹಿಂತಿರುಗಿ -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	ಆದರೆ (c == NUL)
	    ಹಿಂತಿರುಗಿ -1;

	// Perform a binary search in the list of accepted bytes.
	ಆದರೆ (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	ಆಗ (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    ಆದರೆ (--len == 0)	// end of the bytes, didn't find it
		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (byts[arridx] != c)	// didn't find the byte
	    ಹಿಂತಿರುಗಿ -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	ಆದರೆ (c == ' ')
	    ಆಗ (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    ಹಿಂತಿರುಗಿ wordnr;
}

/*
 * Return TRUE if "c1" and "c2" are similar characters according to the MAP
 * lines in the .aff file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
similar_chars(slang_T *slang, ಪೂರ್ಣಾಂಕ c1, ಪೂರ್ಣಾಂಕ c2)
{
    ಪೂರ್ಣಾಂಕ		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    ಆದರೆ (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_ಕಂಡುಹಿಡಿ(&slang->sl_map_hash, buf);
	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    m1 = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    m1 = mb_ptr2char(hi->hi_ಕೀಲಿ + STRLEN(hi->hi_ಕೀಲಿ) + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	m1 = slang->sl_map_array[c1];
    ಆದರೆ (m1 == 0)
	ಹಿಂತಿರುಗಿ FALSE;


    ಆದರೆ (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_ಕಂಡುಹಿಡಿ(&slang->sl_map_hash, buf);
	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    m2 = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    m2 = mb_ptr2char(hi->hi_ಕೀಲಿ + STRLEN(hi->hi_ಕೀಲಿ) + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	m2 = slang->sl_map_array[c2];

    ಹಿಂತಿರುಗಿ m1 == m2;
}

/*
 * Add a suggestion to the list of suggestions.
 * For a suggestion that is already in the list the lowest score is remembered.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    ಪೂರ್ಣಾಂಕ		badlenarg,	// len of bad word replaced with "goodword"
    ಪೂರ್ಣಾಂಕ		score,
    ಪೂರ್ಣಾಂಕ		altscore,
    ಪೂರ್ಣಾಂಕ		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    ಪೂರ್ಣಾಂಕ		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    ಪೂರ್ಣಾಂಕ		goodlen;	// len of goodword changed
    ಪೂರ್ಣಾಂಕ		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    ಗಾಗಿ (;;)
    {
	goodlen = (ಪೂರ್ಣಾಂಕ)(pgood - goodword);
	badlen = (ಪೂರ್ಣಾಂಕ)(pbad - su->su_badptr);
	ಆದರೆ (goodlen <= 0 || badlen <= 0)
	    ನಿಲ್ಲಿಸು;
	MB_PTR_ಹಿಂಭಾಗ(goodword, pgood);
	MB_PTR_ಹಿಂಭಾಗ(su->su_badptr, pbad);
	ಆದರೆ (has_mbyte)
	{
	    ಆದರೆ (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*pgood != *pbad)
		ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (gap->ga_len == 0)
	i = -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	ಗಾಗಿ (i = gap->ga_len; --i >= 0; ++stp)
	    ಆದರೆ (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_ಶಬ್ದ, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		ಆದರೆ (stp->st_slang == ಖಾಲಿ)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		ಆದರೆ (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    ಆದರೆ (had_bonus)
			rescore_one(su, stp);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			new_sug.st_ಶಬ್ದ = stp->st_ಶಬ್ದ;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		ಆದರೆ (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		ನಿಲ್ಲಿಸು;
	    }
    }

    ಆದರೆ (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_ಶಬ್ದ = vim_strnsave(goodword, goodlen);
	ಆದರೆ (stp->st_ಶಬ್ದ != ಖಾಲಿ)
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
	    ಆದರೆ (gap->ga_len > SUG_MAX_ಎಣಿಕೆ(su))
	    {
		ಆದರೆ (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_ಎಣಿಕೆ(su));
		ಇಲ್ಲದಿದ್ದರೆ
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_ಎಣಿಕೆ(su));
	    }
	}
    }
}

/*
 * Suggestions may in fact be flagged as errors.  Esp. for banned words and
 * for split words, such as "the the".  Remove these from the list here.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	longword[MAXWLEN + 1];
    ಪೂರ್ಣಾಂಕ		len;
    hlf_T	attr;

    ಆದರೆ (gap->ga_len == 0)
	ಹಿಂತಿರುಗಿ;
    stp = &SUG(*gap, 0);
    ಗಾಗಿ (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_ಶಬ್ದ, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_ಎಣಿಕೆ;
	(ಶೂನ್ಯ)spell_check(curwin, longword, &attr, ಖಾಲಿ, FALSE);
	ಆದರೆ (attr != HLF_ಎಣಿಕೆ)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_ಶಬ್ದ);
	    --gap->ga_len;
	    ಆದರೆ (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       ಗಾತ್ರ(suggest_T) * (gap->ga_len - i));
	}
    }
}


/*
 * Add a word to be banned.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_banned(
    suginfo_T	*su,
    char_u	*ಶಬ್ದ)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(ಶಬ್ದ);
    hi = hash_lookup(&su->su_banned, ಶಬ್ದ, hash);
    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))		// already present
	ಹಿಂತಿರುಗಿ;
    s = vim_strsave(ಶಬ್ದ);
    ಆದರೆ (s != ಖಾಲಿ)
	hash_ಕೂಡಿಸು_ಅಂಶ(&su->su_banned, hi, s, hash);
}

/*
 * Recompute the score for all suggestions if sound-folding is possible.  This
 * is slow, thus only done for the final results.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
rescore_suggestions(suginfo_T *su)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (su->su_sallang != ಖಾಲಿ)
	ಗಾಗಿ (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}

/*
 * Recompute the score for one suggestion if sound-folding is possible.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
rescore_one(suginfo_T *su, suggest_T *stp)
{
    slang_T	*slang = stp->st_slang;
    char_u	sal_badword[MAXWLEN];
    char_u	*p;

    // Only rescore suggestions that have no sal score yet and do have a
    // language.
    ಆದರೆ (slang != ಖಾಲಿ && slang->sl_sal.ga_len > 0 && !stp->st_had_bonus)
    {
	ಆದರೆ (slang == su->su_sallang)
	    p = su->su_sal_badword;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    spell_soundfold(slang, su->su_fbadword, TRUE, sal_badword);
	    p = sal_badword;
	}

	stp->st_altscore = stp_sal_score(stp, su, slang, p);
	ಆದರೆ (stp->st_altscore == SCORE_MAXMAX)
	    stp->st_altscore = SCORE_BIG;
	stp->st_score = RESCORE(stp->st_score, stp->st_altscore);
	stp->st_had_bonus = TRUE;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sug_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);

/*
 * Function given to qsort() to sort the suggestions on st_score.
 * First on "st_score", then "st_altscore" then alphabetically.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sug_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    ಪೂರ್ಣಾಂಕ		n;

    n = p1->st_score == p2->st_score ? 0 :
	p1->st_score > p2->st_score ? 1 : -1;

    ಆದರೆ (n == 0)
    {
	n = p1->st_altscore == p2->st_altscore ? 0 :
	    p1->st_altscore > p2->st_altscore ? 1 : -1;

	ಆದರೆ (n == 0)
	    n = STRICMP(p1->st_ಶಬ್ದ, p2->st_ಶಬ್ದ);
    }
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Cleanup the suggestions:
 * - Sort on score.
 * - Remove words that won't be displayed.
 * Returns the maximum score in the list or "maxscore" unmodified.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cleanup_suggestions(
    garray_T	*gap,
    ಪೂರ್ಣಾಂಕ		maxscore,
    ಪೂರ್ಣಾಂಕ		keep)		// nr of suggestions to keep
{
    ಆದರೆ (gap->ga_len <= 0)
	ಹಿಂತಿರುಗಿ maxscore;

    // Sort the list.
    qsort(gap->ga_ದತ್ತಾಂಶ, (size_t)gap->ga_len, ಗಾತ್ರ(suggest_T),
	    sug_compare);

    // Truncate the list to the number of suggestions that will be
    // displayed.
    ಆದರೆ (gap->ga_len > keep)
    {
	ಪೂರ್ಣಾಂಕ		i;
	suggest_T   *stp = &SUG(*gap, 0);

	ಗಾಗಿ (i = keep; i < gap->ga_len; ++i)
	    vim_free(stp[i].st_ಶಬ್ದ);
	gap->ga_len = keep;
	ಆದರೆ (keep >= 1)
	    ಹಿಂತಿರುಗಿ stp[keep - 1].st_score;
    }
    ಹಿಂತಿರುಗಿ maxscore;
}

/*
 * Compute a score for two sound-a-like words.
 * This permits up to two inserts/deletes/swaps/etc. to keep things fast.
 * Instead of a generic loop we write out the code.  That keeps it fast by
 * avoiding checks that will not be possible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    ಪೂರ್ಣಾಂಕ		goodlen;
    ಪೂರ್ಣಾಂಕ		badlen;
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    ಪೂರ್ಣಾಂಕ		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    ಆದರೆ ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	ಆದರೆ ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    ಹಿಂತಿರುಗಿ SCORE_DEL;
	ಆದರೆ (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    ಹಿಂತಿರುಗಿ SCORE_MAXMAX;

	ಆದರೆ (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    score = 2 * SCORE_DEL / 3;
	    ಆದರೆ (*badsound == '*')
		++badsound;
	    ಇಲ್ಲದಿದ್ದರೆ
		++goodsound;
	}
    }

    goodlen = (ಪೂರ್ಣಾಂಕ)STRLEN(goodsound);
    badlen = (ಪೂರ್ಣಾಂಕ)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    ಆದರೆ (n < -2 || n > 2)
	ಹಿಂತಿರುಗಿ SCORE_MAXMAX;

    ಆದರೆ (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    ಆಗ (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    ಆಯ್ಕೆ (n)
    {
	ಸ್ಥಿತಿ -2:
	ಸ್ಥಿತಿ 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    ಆಗ (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    ಆದರೆ (STRCMP(pl + 1, ps) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_DEL * 2;

	    // Failed to compare.
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ -1:
	ಸ್ಥಿತಿ 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    ಆಗ (*pl2 == *ps2)
	    {
		ಆದರೆ (*pl2 == NUL)	// reached the end
		    ಹಿಂತಿರುಗಿ score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    ಆದರೆ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_DEL + SCORE_ಅದಲುಬದಲು;

	    // 3: delete then substitute, then the rest must be equal
	    ಆದರೆ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    ಆದರೆ (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		ಆಗ (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		ಆದರೆ (STRCMP(pl2 + 1, ps2) == 0)
		    ಹಿಂತಿರುಗಿ score + SCORE_ಅದಲುಬದಲು + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    ಆಗ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    ಆದರೆ (STRCMP(pl2 + 1, ps2) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    ಆದರೆ (*pl == NUL)
		ಹಿಂತಿರುಗಿ score;

	    // 2: swap
	    ಆದರೆ (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		ಆಗ (*pl2 == *ps2)
		{
		    ಆದರೆ (*pl2 == NUL)	// reached the end
			ಹಿಂತಿರುಗಿ score + SCORE_ಅದಲುಬದಲು;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		ಆದರೆ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    ಹಿಂತಿರುಗಿ score + SCORE_ಅದಲುಬದಲು + SCORE_ಅದಲುಬದಲು;

		// 4: swap and substitute
		ಆದರೆ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    ಹಿಂತಿರುಗಿ score + SCORE_ಅದಲುಬದಲು + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    ಆಗ (*pl2 == *ps2)
	    {
		ಆದರೆ (*pl2 == NUL)	// reached the end
		    ಹಿಂತಿರುಗಿ score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    ಆದರೆ (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_SUBST + SCORE_ಅದಲುಬದಲು;

	    // 7: substitute and substitute
	    ಆದರೆ (STRCMP(pl2 + 1, ps2 + 1) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    ಆಗ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    ಆದರೆ (STRCMP(pl2 + 1, ps2) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    ಆಗ (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    ಆದರೆ (STRCMP(pl2, ps2 + 1) == 0)
		ಹಿಂತಿರುಗಿ score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ SCORE_MAXMAX;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    ಪೂರ್ಣಾಂಕ		*cnt;
    ಪೂರ್ಣಾಂಕ		badlen, goodlen;	// lengths including NUL
    ಪೂರ್ಣಾಂಕ		j, i;
    ಪೂರ್ಣಾಂಕ		t;
    ಪೂರ್ಣಾಂಕ		bc, gc;
    ಪೂರ್ಣಾಂಕ		pbc, pgc;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		wbadword[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		wgoodword[MAXWLEN];

    ಆದರೆ (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	ಗಾಗಿ (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	ಗಾಗಿ (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	badlen = (ಪೂರ್ಣಾಂಕ)STRLEN(badword) + 1;
	goodlen = (ಪೂರ್ಣಾಂಕ)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#ವ್ಯಾಖ್ಯಾನಿಸು CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, (badlen + 1) * (goodlen + 1));
    ಆದರೆ (cnt == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;	// out of memory

    CNT(0, 0) = 0;
    ಗಾಗಿ (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    ಗಾಗಿ (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	ಗಾಗಿ (j = 1; j <= goodlen; ++j)
	{
	    ಆದರೆ (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    ಆದರೆ (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Use a better score when there is only a case difference.
		ಆದರೆ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // For a similar character use SCORE_SIMILAR.
		    ಆದರೆ (slang != ಖಾಲಿ
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    ಇಲ್ಲದಿದ್ದರೆ
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		ಆದರೆ (i > 1 && j > 1)
		{
		    ಆದರೆ (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    ಆದರೆ (bc == pgc && pbc == gc)
		    {
			t = SCORE_ಅದಲುಬದಲು + CNT(i - 2, j - 2);
			ಆದರೆ (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		ಆದರೆ (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		ಆದರೆ (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    ಹಿಂತಿರುಗಿ i;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		badi;
    ಪೂರ್ಣಾಂಕ		goodi;
    ಪೂರ್ಣಾಂಕ		score;
} limitscore_T;

/*
 * Like spell_edit_score(), but with a limit on the score to make it faster.
 * May return SCORE_MAXMAX when the score is higher than "limit".
 *
 * This uses a stack for the edits still to be tried.
 * The idea comes from Aspell leditdist.cpp.  Rewritten in C and added support
 * for multi-byte characters.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
spell_edit_score_ಎಲ್ಲೆ(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ)
{
    limitscore_T    ರಾಶಿ[10];		// allow for over 3 * 2 edits
    ಪೂರ್ಣಾಂಕ		    stackidx;
    ಪೂರ್ಣಾಂಕ		    bi, gi;
    ಪೂರ್ಣಾಂಕ		    bi2, gi2;
    ಪೂರ್ಣಾಂಕ		    bc, gc;
    ಪೂರ್ಣಾಂಕ		    score;
    ಪೂರ್ಣಾಂಕ		    score_off;
    ಪೂರ್ಣಾಂಕ		    minscore;
    ಪೂರ್ಣಾಂಕ		    ಸುತ್ತು;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    ಆದರೆ (has_mbyte)
	ಹಿಂತಿರುಗಿ spell_edit_score_ಎಲ್ಲೆ_w(slang, badword, goodword, ಎಲ್ಲೆ);

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
    minscore = ಎಲ್ಲೆ + 1;

    ಗಾಗಿ (;;)
    {
	// Skip over an equal part, score remains the same.
	ಗಾಗಿ (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    ಆದರೆ (bc != gc)	// stop at a char that's different
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (bc == NUL)	// both words end
	    {
		ಆದರೆ (score < minscore)
		    minscore = score;
		ಹೋಗು ತೆಗೆ;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	ಆದರೆ (gc == NUL)    // goodword ends, delete badword chars
	{
	    ಮಾಡು
	    {
		ಆದರೆ ((score += SCORE_DEL) >= minscore)
		    ಹೋಗು ತೆಗೆ;	    // do next alternative
	    } ಆಗ (badword[++bi] != NUL);
	    minscore = score;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bc == NUL) // badword ends, insert badword chars
	{
	    ಮಾಡು
	    {
		ಆದರೆ ((score += SCORE_INS) >= minscore)
		    ಹೋಗು ತೆಗೆ;	    // do next alternative
	    } ಆಗ (goodword[++gi] != NUL);
	    minscore = score;
	}
	ಇಲ್ಲದಿದ್ದರೆ			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    ಗಾಗಿ (ಸುತ್ತು = 0; ಸುತ್ತು <= 1; ++ಸುತ್ತು)
	    {
		score_off = score + (ಸುತ್ತು == 0 ? SCORE_DEL : SCORE_INS);
		ಆದರೆ (score_off < minscore)
		{
		    ಆದರೆ (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - ಸುತ್ತು;
			gi2 = gi + ಸುತ್ತು;
			ಆಗ (goodword[gi2] == badword[bi2])
			{
			    ಆದರೆ (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				ನಿಲ್ಲಿಸು;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// try deleting/inserting a character later
			ರಾಶಿ[stackidx].badi = bi + 1 - ಸುತ್ತು;
			ರಾಶಿ[stackidx].goodi = gi + ಸುತ್ತು;
			ರಾಶಿ[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    ಆದರೆ (score + SCORE_ಅದಲುಬದಲು < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		ಆದರೆ (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_ಅದಲುಬದಲು;
		    ಮುಂದುವರಿ;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    ಆದರೆ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// For a similar character use SCORE_SIMILAR.
		ಆದರೆ (slang != ಖಾಲಿ
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		ಇಲ್ಲದಿದ್ದರೆ
		    score += SCORE_SUBST;
	    }

	    ಆದರೆ (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		ಮುಂದುವರಿ;
	    }
	}
ತೆಗೆ:
	// Get here to try the next alternative, pop it from the stack.
	ಆದರೆ (stackidx == 0)		// stack is empty, finished
	    ನಿಲ್ಲಿಸು;

	// pop an item from the stack
	--stackidx;
	gi = ರಾಶಿ[stackidx].goodi;
	bi = ರಾಶಿ[stackidx].badi;
	score = ರಾಶಿ[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    ಆದರೆ (minscore > ಎಲ್ಲೆ)
	ಹಿಂತಿರುಗಿ SCORE_MAXMAX;
    ಹಿಂತಿರುಗಿ minscore;
}

/*
 * Multi-byte version of spell_edit_score_limit().
 * Keep it in sync with the above!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
spell_edit_score_ಎಲ್ಲೆ_w(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ)
{
    limitscore_T    ರಾಶಿ[10];		// allow for over 3 * 2 edits
    ಪೂರ್ಣಾಂಕ		    stackidx;
    ಪೂರ್ಣಾಂಕ		    bi, gi;
    ಪೂರ್ಣಾಂಕ		    bi2, gi2;
    ಪೂರ್ಣಾಂಕ		    bc, gc;
    ಪೂರ್ಣಾಂಕ		    score;
    ಪೂರ್ಣಾಂಕ		    score_off;
    ಪೂರ್ಣಾಂಕ		    minscore;
    ಪೂರ್ಣಾಂಕ		    ಸುತ್ತು;
    char_u	    *p;
    ಪೂರ್ಣಾಂಕ		    wbadword[MAXWLEN];
    ಪೂರ್ಣಾಂಕ		    wgoodword[MAXWLEN];

    // Get the characters from the multi-byte strings and put them in an
    // int array for easy access.
    bi = 0;
    ಗಾಗಿ (p = badword; *p != NUL; )
	wbadword[bi++] = mb_cptr2char_adv(&p);
    wbadword[bi++] = 0;
    gi = 0;
    ಗಾಗಿ (p = goodword; *p != NUL; )
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
    minscore = ಎಲ್ಲೆ + 1;

    ಗಾಗಿ (;;)
    {
	// Skip over an equal part, score remains the same.
	ಗಾಗಿ (;;)
	{
	    bc = wbadword[bi];
	    gc = wgoodword[gi];

	    ಆದರೆ (bc != gc)	// stop at a char that's different
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (bc == NUL)	// both words end
	    {
		ಆದರೆ (score < minscore)
		    minscore = score;
		ಹೋಗು ತೆಗೆ;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	ಆದರೆ (gc == NUL)    // goodword ends, delete badword chars
	{
	    ಮಾಡು
	    {
		ಆದರೆ ((score += SCORE_DEL) >= minscore)
		    ಹೋಗು ತೆಗೆ;	    // do next alternative
	    } ಆಗ (wbadword[++bi] != NUL);
	    minscore = score;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bc == NUL) // badword ends, insert badword chars
	{
	    ಮಾಡು
	    {
		ಆದರೆ ((score += SCORE_INS) >= minscore)
		    ಹೋಗು ತೆಗೆ;	    // do next alternative
	    } ಆಗ (wgoodword[++gi] != NUL);
	    minscore = score;
	}
	ಇಲ್ಲದಿದ್ದರೆ			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    ಗಾಗಿ (ಸುತ್ತು = 0; ಸುತ್ತು <= 1; ++ಸುತ್ತು)
	    {
		score_off = score + (ಸುತ್ತು == 0 ? SCORE_DEL : SCORE_INS);
		ಆದರೆ (score_off < minscore)
		{
		    ಆದರೆ (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - ಸುತ್ತು;
			gi2 = gi + ಸುತ್ತು;
			ಆಗ (wgoodword[gi2] == wbadword[bi2])
			{
			    ಆದರೆ (wgoodword[gi2] == NUL)
			    {
				minscore = score_off;
				ನಿಲ್ಲಿಸು;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// try deleting a character from badword later
			ರಾಶಿ[stackidx].badi = bi + 1 - ಸುತ್ತು;
			ರಾಶಿ[stackidx].goodi = gi + ಸುತ್ತು;
			ರಾಶಿ[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    ಆದರೆ (score + SCORE_ಅದಲುಬದಲು < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		ಆದರೆ (gc == wbadword[bi + 1] && bc == wgoodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_ಅದಲುಬದಲು;
		    ಮುಂದುವರಿ;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    ಆದರೆ (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// For a similar character use SCORE_SIMILAR.
		ಆದರೆ (slang != ಖಾಲಿ
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		ಇಲ್ಲದಿದ್ದರೆ
		    score += SCORE_SUBST;
	    }

	    ಆದರೆ (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		ಮುಂದುವರಿ;
	    }
	}
ತೆಗೆ:
	// Get here to try the next alternative, pop it from the stack.
	ಆದರೆ (stackidx == 0)		// stack is empty, finished
	    ನಿಲ್ಲಿಸು;

	// pop an item from the stack
	--stackidx;
	gi = ರಾಶಿ[stackidx].goodi;
	bi = ರಾಶಿ[stackidx].badi;
	score = ರಾಶಿ[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    ಆದರೆ (minscore > ಎಲ್ಲೆ)
	ಹಿಂತಿರುಗಿ SCORE_MAXMAX;
    ಹಿಂತಿರುಗಿ minscore;
}
#ಆದರೆ_ಅಂತ್ಯ  // FEAT_SPELL
