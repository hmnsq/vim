/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * insexpand.c: functions for Insert mode completion
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_WANT_IDENT	0x100

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಸಾಮಾನ್ಯ		0  // CTRL-N CTRL-P completion, default
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_NOT_DEFINED_YET	1
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_SCROLL		2
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು	3
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_FILES		4
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಟ್ಯಾಗುಗಳು		(5 + CTRL_X_WANT_IDENT)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಮಾರ್ಗ_PATTERNS	(6 + CTRL_X_WANT_IDENT)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ	(7 + CTRL_X_WANT_IDENT)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಪೂರ್ಣಗೊಂಡ		8
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_DICTIONARY	(9 + CTRL_X_WANT_IDENT)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_THESAURUS	(10 + CTRL_X_WANT_IDENT)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_CMDLINE		11
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_FUNCTION	12
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_OMNI		13
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_SPELL		14
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_LOCAL_ಸಂದೇಶ	15	// only used in "ctrl_x_msgs"
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_EVAL		16	// for builtin function complete()
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ರಿಜಿಸ್ಟರ್	18	// complete words from registers

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_X_ಸಂದೇಶ(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
ಸ್ಥಿರ ಅಕ್ಷರ *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ಖಾಲಿ, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ಖಾಲಿ, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ಖಾಲಿ,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#ಆದರೆ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ಸ್ಥಿರ string_T ctrl_x_ಬಹುಲಕ_ಹೆಸರುಗಳು[] = {
    STR_LITERAL_INIT("keyword"),
    STR_LITERAL_INIT("ctrl_x"),
    STR_LITERAL_INIT("scroll"),
    STR_LITERAL_INIT("whole_line"),
    STR_LITERAL_INIT("files"),
    STR_LITERAL_INIT("tags"),
    STR_LITERAL_INIT("path_patterns"),
    STR_LITERAL_INIT("path_defines"),
    STR_LITERAL_INIT("unknown"),		    // CTRL_X_FINISHED
    STR_LITERAL_INIT("dictionary"),
    STR_LITERAL_INIT("thesaurus"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("function"),
    STR_LITERAL_INIT("omni"),
    STR_LITERAL_INIT("spell"),
    {ಖಾಲಿ, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Structure used to store one match for insert completion.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ compl_S compl_T;
ರಚನೆ compl_S
{
    compl_T	*cp_ಮುಂದಿನ;
    compl_T	*cp_prev;
    compl_T	*cp_ಹೊಂದಿಕೆ_ಮುಂದಿನ;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_ಪಠ್ಯ[CPT_ಎಣಿಕೆ]);	// text for the menu
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    typval_T	cp_user_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    ಪೂರ್ಣಾಂಕ		cp_flags;		// CP_ values
    ಪೂರ್ಣಾಂಕ		cp_ಸಂಖ್ಯೆ;		// sequence number
    ಪೂರ್ಣಾಂಕ		cp_ಕ್ರಮ;		// list position when first scored
    ಪೂರ್ಣಾಂಕ		cp_score;		// fuzzy match score or proximity score
    ಪೂರ್ಣಾಂಕ		cp_ಒಳ_ಹೊಂದಿಕೆ_array;	// collected by compl_match_array
    ಪೂರ್ಣಾಂಕ		cp_user_abbr_hl_id;	// highlight group ID for abbr
    ಪೂರ್ಣಾಂಕ		cp_user_ಬಗೆ_hl_id;	// highlight group ID for kind
    ಪೂರ್ಣಾಂಕ		cp_cpt_ಮೂಲ_idx;	// index of this match's source in 'cpt' option
};

// values for cp_flags
#ವ್ಯಾಖ್ಯಾನಿಸು CP_ORIGINAL_ಪಠ್ಯ   1	// the original text when the expansion begun
#ವ್ಯಾಖ್ಯಾನಿಸು CP_FREE_FNAME	    2	// cp_fname is allocated
#ವ್ಯಾಖ್ಯಾನಿಸು CP_CONT_S_IPOS	    4	// use CONT_S_IPOS for compl_cont_status
#ವ್ಯಾಖ್ಯಾನಿಸು CP_EQUAL	    8	// ins_compl_equal() always returns TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು CP_ICASE	    16	// ins_compl_equal() ignores case
#ವ್ಯಾಖ್ಯಾನಿಸು CP_FAST	    32	// use fast_breakcheck instead of ui_breakcheck

/*
 * All the current matches are stored in a list.
 * "compl_first_match" points to the start of the list.
 * "compl_curr_match" points to the currently selected entry.
 * "compl_shown_match" is different from compl_curr_match during
 * ins_compl_get_exp(), when new matches are added to the list.
 * "compl_old_match" points to previous "compl_curr_match".
 */
ಸ್ಥಿರ compl_T    *compl_ಮೊದಲ_ಹೊಂದಿಕೆ = ಖಾಲಿ;
ಸ್ಥಿರ compl_T    *compl_curr_ಹೊಂದಿಕೆ = ಖಾಲಿ;
ಸ್ಥಿರ compl_T    *compl_shown_ಹೊಂದಿಕೆ = ಖಾಲಿ;
ಸ್ಥಿರ compl_T    *compl_old_ಹೊಂದಿಕೆ = ಖಾಲಿ;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    cse_ಎಣಿಕೆ;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#ವ್ಯಾಖ್ಯಾನಿಸು CSE_OFF		((ಪೂರ್ಣಾಂಕ)offsetof(complstr_T, cse_str))
#ವ್ಯಾಖ್ಯಾನಿಸು HI2CSE(hi)	((complstr_T *)((hi)->hi_ಕೀಲಿ - CSE_OFF))

ಸ್ಥಿರ hashtab_T  compl_ಅಕ್ಷರಸರಣಿಗಳು_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compl_ಅಕ್ಷರಸರಣಿಗಳು_ಕೂಡಿಸು(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*ನಮೂದು;

    ಆದರೆ (compl_ಅಕ್ಷರಸರಣಿಗಳು_ht.ht_array == ಖಾಲಿ)
	hash_init(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht);
    ಆದರೆ (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht, str, hash);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	ನಮೂದು = alloc(CSE_OFF + len + 1);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    ಹಿಂತಿರುಗಿ;
	ನಮೂದು->cse_ಎಣಿಕೆ = 1;
	vim_strncpy(ನಮೂದು->cse_str, str, len);
	ಆದರೆ (hash_ಕೂಡಿಸು_ಅಂಶ(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht, hi, ನಮೂದು->cse_str, hash)
								      == FAIL)
	    vim_free(ನಮೂದು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++HI2CSE(hi)->cse_ಎಣಿಕೆ;
}

// list used to store the compl_T which have the max score
ಸ್ಥಿರ compl_T	  **compl_ಉತ್ತಮ_matches = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_ಅಂಕೆ_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
ಸ್ಥಿರ string_T	  compl_leader = {ಖಾಲಿ, 0};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ;

// didn't finish finding completions.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_interrupted = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಾಮಾನ್ಯ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_matches = 0;	    // number of completion matches
ಸ್ಥಿರ string_T	  compl_ನಮೂನೆ = {ಖಾಲಿ, 0};	 // search pattern for matching items
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
ಸ್ಥಿರ string_T	  cpt_compl_ನಮೂನೆ = {ಖಾಲಿ, 0}; // pattern returned by func in 'cpt'
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_direction = FORWARD;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_shows_dir = FORWARD;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_pending = 0;	    // > 1 for postponed CTRL-N
ಸ್ಥಿರ pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_ಉದ್ದಳತೆ = 0;
ಸ್ಥಿರ linenr_T	  compl_lnum = 0;           // lnum where the completion start
ಸ್ಥಿರ colnr_T	  compl_ಲಂಬಸಾಲು = 0;	    // column where the text starts
					    // that is being completed
ಸ್ಥಿರ colnr_T	  compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು = 0;
ಸ್ಥಿರ colnr_T	  compl_longest_ಕೊನೆ_ಲಂಬಸಾಲು = 0;  // end of 'longest' inserted text
ಸ್ಥಿರ string_T	  compl_orig_ಪಠ್ಯ = {ಖಾಲಿ, 0};  // text as it was before
					    // completion started
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_cont_ಬಹುಲಕ = 0;
ಸ್ಥಿರ expand_T	  compl_xp;

ಸ್ಥಿರ win_T	  *compl_curr_win = ಖಾಲಿ;  // win where completion is active
ಸ್ಥಿರ buf_T	  *compl_curr_buf = ಖಾಲಿ;  // buf where completion is active

#ವ್ಯಾಖ್ಯಾನಿಸು COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_autocomplete = FALSE;	    // whether autocompletion is active
ಸ್ಥಿರ bool	  compl_autostarted = false;	    // Vim started this completion
ಸ್ಥಿರ bool	  compl_autostart_pending = false;  // the trigger armed one
ಸ್ಥಿರ bool	  compl_autocomplete_pending = false;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
ಸ್ಥಿರ elapsed_T  compl_autocomplete_ಆರಂಭ_tv;	    // when the delay was armed
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_ಸಮಯ_ತುಂಡು_expired = FALSE; // time budget exceeded for current source
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#ವ್ಯಾಖ್ಯಾನಿಸು COMPL_MIN_TIMEOUT_MS	5
#ವ್ಯಾಖ್ಯಾನಿಸು DECAY_COMPL_TIMEOUT() \
    ಮಾಡು { \
	ಆದರೆ (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } ಆಗ (0)

// Timeout values for F{func}, F and o values in 'complete'
#ವ್ಯಾಖ್ಯಾನಿಸು COMPL_FUNC_TIMEOUT_MS		300
#ವ್ಯಾಖ್ಯಾನಿಸು COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_cont_ಸ್ಥಿತಿ = 0;
#ವ್ಯಾಖ್ಯಾನಿಸು CONT_ADDING	1	// "normal" or "adding" expansion
#ವ್ಯಾಖ್ಯಾನಿಸು CONT_INTRPT	(2 + 4)	// a ^X interrupted the current expansion
				// it's set only iff N_ADDS is set
#ವ್ಯಾಖ್ಯಾನಿಸು CONT_N_ADDS	4	// next ^X<> will add-new or expand-current
#ವ್ಯಾಖ್ಯಾನಿಸು CONT_S_IPOS	8	// next ^X<> will set initial_pos?
				// if so, word-wise-expansion will set SOL
#ವ್ಯಾಖ್ಯಾನಿಸು CONT_SOL	16	// pattern includes start of line, just for
				// word-wise expansion, not set for ^X^L
#ವ್ಯಾಖ್ಯಾನಿಸು CONT_LOCAL	32	// for ctrl_x_mode 0, ^X^P/^X^N do a local
				// expansion, (eg use complete=.)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_opt_refresh_always = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_opt_suppress_ಖಾಲಿಗಳು = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  compl_selected_ಅಂಶ = -1;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cpt_ಮೂಲ_T
{
    ಪೂರ್ಣಾಂಕ	cs_refresh_always;  // Whether 'refresh:always' is set for func
    ಪೂರ್ಣಾಂಕ	cs_startcol;	    // Start column returned by func
    ಪೂರ್ಣಾಂಕ	cs_max_matches;	    // Max items to display from this source
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T	compl_ಆರಂಭ_tv;	    // Timestamp when match collection starts
#ಆದರೆ_ಅಂತ್ಯ
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_ಮೂಲ_T;

ಸ್ಥಿರ cpt_ಮೂಲ_T *cpt_sources_array; // Pointer to the array of completion sources
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cpt_sources_ಎಣಿಕೆ;  // Total number of completion sources specified in the 'cpt' option
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cpt_sources_ಸೂಚ್ಯಂಕ = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ಸ್ಥಿರ pumitem_T *compl_ಹೊಂದಿಕೆ_array = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compl_ಹೊಂದಿಕೆ_arraysize;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ins_compl_ಕೂಡಿಸು(char_u *str, ಪೂರ್ಣಾಂಕ len, char_u *fname, char_u **cptext, typval_T *user_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ cdir, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ adup, ಪೂರ್ಣಾಂಕ *user_hl, ಪೂರ್ಣಾಂಕ score);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_longest_ಹೊಂದಿಕೆ(compl_T *ಹೊಂದಿಕೆ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_del_pum(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_files(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, char_u **files, ಪೂರ್ಣಾಂಕ thesaurus, ಪೂರ್ಣಾಂಕ flags, regmatch_T *regmatch, char_u *buf, ಪೂರ್ಣಾಂಕ *dir);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_free(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_compl_need_restart(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_new_leader(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  get_compl_len(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_restart(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_ಗಣ_original_ಪಠ್ಯ(char_u *str, size_t len);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#ಆದರೆ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_ಕೂಡಿಸು_list(list_T *list);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_ಕೂಡಿಸು_dict(dict_T *dict);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_userdefined_compl_info(colnr_T curs_ಲಂಬಸಾಲು, callback_T *cb, ಪೂರ್ಣಾಂಕ *startcol);
ಸ್ಥಿರ ಶೂನ್ಯ get_cpt_func_completion_matches(callback_T *cb);
ಸ್ಥಿರ callback_T *get_callback_if_cpt_func(char_u *p, ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ fill_complete_info_dict(dict_T *di, compl_T *ಹೊಂದಿಕೆ, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_ಹೊಂದಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಿದ್ಧತೆ_cpt_sources(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_cpt_func_refresh_always(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ cpt_sources_clear(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ cpt_compl_refresh(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_compl_key2dir(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_compl_pum_ಕೀಲಿ(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_compl_key2count(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ show_pum(ಪೂರ್ಣಾಂಕ prev_w_wrow, ಪೂರ್ಣಾಂಕ prev_w_leftcol);
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ  quote_meta(char_u *dest, char_u *str, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ins_compl_has_ಅಪವರ್ತ್ಯ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_expand_ಅಪವರ್ತ್ಯ(char_u *str);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_longest_ಸೇರಿಸು(char_u *ಪೂರ್ವಪ್ರತ್ಯಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_make_ರೇಖೀಯ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ins_compl_make_cyclic(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
ಸ್ಥಿರ ಶೂನ್ಯ spell_ಹಿಂಭಾಗ_ಗಮ್ಯ_badword(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  spell_bad_len = 0;	// length of located bad word
#ಆದರೆ_ಅಂತ್ಯ

/*
 * CTRL-X pressed in Insert mode.
 */
    ಶೂನ್ಯ
ins_ctrl_x(ಶೂನ್ಯ)
{
    ಆದರೆ (!ctrl_x_ಬಹುಲಕ_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	ಆದರೆ (compl_cont_ಸ್ಥಿತಿ & CONT_N_ADDS)
	    compl_cont_ಸ್ಥಿತಿ |= CONT_INTRPT;
	ಇಲ್ಲದಿದ್ದರೆ
	    compl_cont_ಸ್ಥಿತಿ = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_ಬಹುಲಕ = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_ಸಂದೇಶ(ctrl_x_ಬಹುಲಕ));
	edit_submode_pre = ಖಾಲಿ;
	showmode();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_ಬಹುಲಕ = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_none(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == 0; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ಸಾಮಾನ್ಯ; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_scroll(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_SCROLL; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ಸಂಪೂರ್ಣ_ಸಾಲು(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_files(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_FILES; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ಟ್ಯಾಗುಗಳು(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ಟ್ಯಾಗುಗಳು; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_patterns(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ಮಾರ್ಗ_PATTERNS; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_dictionary(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_DICTIONARY; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_thesaurus(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_THESAURUS; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_cmdline(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_CMDLINE
		|| ctrl_x_ಬಹುಲಕ == CTRL_X_CMDLINE_CTRL_X; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_function(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_FUNCTION; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_omni(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_OMNI; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_spell(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_SPELL; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_eval(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_EVAL; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು || ctrl_x_ಬಹುಲಕ == CTRL_X_EVAL; }
ಪೂರ್ಣಾಂಕ ctrl_x_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ)
    { ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_ರಿಜಿಸ್ಟರ್; }

/*
 * Whether other than default completion has been selected.
 */
    ಪೂರ್ಣಾಂಕ
ctrl_x_ಬಹುಲಕ_not_default(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ != CTRL_X_ಸಾಮಾನ್ಯ;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    ಪೂರ್ಣಾಂಕ
ctrl_x_ಬಹುಲಕ_not_defined_yet(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ctrl_x_ಬಹುಲಕ == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    ಪೂರ್ಣಾಂಕ
compl_ಸ್ಥಿತಿ_adding(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_cont_ಸ್ಥಿತಿ & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    ಪೂರ್ಣಾಂಕ
compl_ಸ್ಥಿತಿ_sol(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_cont_ಸ್ಥಿತಿ & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    ಪೂರ್ಣಾಂಕ
compl_ಸ್ಥಿತಿ_local(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_cont_ಸ್ಥಿತಿ & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    ಶೂನ್ಯ
compl_ಸ್ಥಿತಿ_clear(ಶೂನ್ಯ)
{
    compl_cont_ಸ್ಥಿತಿ = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compl_dir_forward(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compl_shows_dir_forward(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compl_shows_dir_backward(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    ಪೂರ್ಣಾಂಕ
has_compl_option(ಪೂರ್ಣಾಂಕ dict_opt)
{
    ಆದರೆ (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
							&& !curwin->w_p_spell
#ಆದರೆ_ಅಂತ್ಯ
							)
		 : (*curbuf->b_p_tsr == NUL && *p_tsr == NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
		     && *curbuf->b_p_tsrfu == NUL && *p_tsrfu == NUL
#ಆದರೆ_ಅಂತ್ಯ
		   ))
    {
	ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಾಮಾನ್ಯ;
	edit_submode = ಖಾಲಿ;
	ಸಂದೇಶ_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	ಆದರೆ (emsg_silent == 0 && !ಒಳ_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    ಹೊರಹೋಗು_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (!get_vim_var_nr(VV_TESTING))
#ಆದರೆ_ಅಂತ್ಯ
		ui_delay(2004L, FALSE);
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    ಪೂರ್ಣಾಂಕ
vim_is_ctrl_x_ಕೀಲಿ(ಪೂರ್ಣಾಂಕ c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    ಆದರೆ (ins_compl_pum_ಕೀಲಿ(c))
	ಹಿಂತಿರುಗಿ TRUE;

    ಆಯ್ಕೆ (ctrl_x_ಬಹುಲಕ)
    {
	ಸ್ಥಿತಿ 0:		    // Not in any CTRL-X mode
	    ಹಿಂತಿರುಗಿ (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	ಸ್ಥಿತಿ CTRL_X_NOT_DEFINED_YET:
	ಸ್ಥಿತಿ CTRL_X_CMDLINE_CTRL_X:
	    ಹಿಂತಿರುಗಿ (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	ಸ್ಥಿತಿ CTRL_X_SCROLL:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_Y || c == Ctrl_E);
	ಸ್ಥಿತಿ CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_FILES:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_DICTIONARY:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_THESAURUS:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_ಟ್ಯಾಗುಗಳು:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	ಸ್ಥಿತಿ CTRL_X_ಮಾರ್ಗ_PATTERNS:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CTRL_X_CMDLINE:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	ಸ್ಥಿತಿ CTRL_X_FUNCTION:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_OMNI:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CTRL_X_SPELL:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_EVAL:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_P || c == Ctrl_N);
	ಸ್ಥಿತಿ CTRL_X_ರಿಜಿಸ್ಟರ್:
	    ಹಿಂತಿರುಗಿ (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    ಆಂತರಿಕ_error("vim_is_ctrl_x_key()");
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_T *ಹೊಂದಿಕೆ)
{
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ->cp_flags & CP_ORIGINAL_ಪಠ್ಯ;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_T *ಹೊಂದಿಕೆ)
{
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ == compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    ಸ್ಥಿರ compl_T *
ಕಂಡುಹಿಡಿ_original_ಪಠ್ಯ_ಹೊಂದಿಕೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_ಮೊದಲ_ಹೊಂದಿಕೆ))
	ಹಿಂತಿರುಗಿ compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev != ಖಾಲಿ
	    && ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev))
	ಹಿಂತಿರುಗಿ compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_accept_char(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (compl_autocomplete && compl_from_nonkeyword)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (ctrl_x_ಬಹುಲಕ & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	ಹಿಂತಿರುಗಿ vim_isIDc(c);

    ಆಯ್ಕೆ (ctrl_x_ಬಹುಲಕ)
    {
	ಸ್ಥಿತಿ CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    ಹಿಂತಿರುಗಿ vim_isfilec(c) && !vim_ispathsep(c);

	ಸ್ಥಿತಿ CTRL_X_CMDLINE:
	ಸ್ಥಿತಿ CTRL_X_CMDLINE_CTRL_X:
	ಸ್ಥಿತಿ CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    ಹಿಂತಿರುಗಿ vim_isprintc(c) && !VIM_ISWHITE(c);

	ಸ್ಥಿತಿ CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು:
	    // For while line completion a space can be part of the line.
	    ಹಿಂತಿರುಗಿ vim_isprintc(c);
    }
    ಹಿಂತಿರುಗಿ vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    ಸ್ಥಿರ char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    ಪೂರ್ಣಾಂಕ	    char_len,
    ಪೂರ್ಣಾಂಕ	    compl_char_len,
    ಪೂರ್ಣಾಂಕ	    min_len,
    char_u  **tofree)
{
    ಪೂರ್ಣಾಂಕ		*wca;			// Wide character array.
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i, c;
    ಪೂರ್ಣಾಂಕ		has_ಅಧೋ = FALSE;
    ಪೂರ್ಣಾಂಕ		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, char_len);
    ಆದರೆ (wca == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ IObuff;

    p = str;
    ಗಾಗಿ (i = 0; i < char_len; ++i)
    {
	ಆದರೆ (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	ಇಲ್ಲದಿದ್ದರೆ
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_ಪಠ್ಯ.string;
    ಗಾಗಿ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ಆದರೆ (MB_ISLOWER(c))
	{
	    has_ಅಧೋ = TRUE;
	    ಆದರೆ (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		ಗಾಗಿ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    ಆದರೆ (!has_ಅಧೋ)
    {
	p = compl_orig_ಪಠ್ಯ.string;
	ಗಾಗಿ (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    ಆದರೆ (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		ಗಾಗಿ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		ನಿಲ್ಲಿಸು;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_ಪಠ್ಯ.string;
    ಗಾಗಿ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ಆದರೆ (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    ಆಗ (i < char_len)
    {
	ಆದರೆ (gap.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	{
	    ಆದರೆ (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		ಹಿಂತಿರುಗಿ (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_ದತ್ತಾಂಶ + gap.ga_len;
	    ಆದರೆ (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    ಆದರೆ (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		ಹಿಂತಿರುಗಿ (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_ದತ್ತಾಂಶ, IObuff);
	    gap.ga_len = (ಪೂರ್ಣಾಂಕ)(p - IObuff);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	ಇಲ್ಲದಿದ್ದರೆ
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    ಆದರೆ (gap.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	*tofree = gap.ga_ದತ್ತಾಂಶ;
	ಹಿಂತಿರುಗಿ gap.ga_ದತ್ತಾಂಶ;
    }

    *p = NUL;
    ಹಿಂತಿರುಗಿ IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_ಕೂಡಿಸು_infercase(
    char_u	*str_arg,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		icase,
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		dir,
    ಪೂರ್ಣಾಂಕ		cont_s_ipos,  // next ^X<> will set initial_pos
    ಪೂರ್ಣಾಂಕ		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		char_len;		// count multi-byte characters
    ಪೂರ್ಣಾಂಕ		compl_char_len;
    ಪೂರ್ಣಾಂಕ		min_len;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    ಪೂರ್ಣಾಂಕ		res;
    char_u	*tofree = ಖಾಲಿ;

    ಆದರೆ (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	ಆದರೆ (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    ಆಗ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_len = len;
	}

	// Find actual length of original text.
	ಆದರೆ (has_mbyte)
	{
	    p = compl_orig_ಪಠ್ಯ.string;
	    compl_char_len = 0;
	    ಆಗ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    compl_char_len = compl_ಉದ್ದಳತೆ;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    ಆದರೆ (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    ಆದರೆ (icase)
	flags |= CP_ICASE;

    res = ins_compl_ಕೂಡಿಸು(str, len, fname, ಖಾಲಿ, ಖಾಲಿ, dir, flags, FALSE, ಖಾಲಿ, score);
    vim_free(tofree);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Check if fuzzy matching is enabled
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cot_fuzzy(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_ಬಹುಲಕ_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_ಹತ್ತಿರದ_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (compl_autocomplete || (get_cot_flags() & COT_ಹತ್ತಿರದ))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_is_ಹೊಂದಿಕೆ_selected(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_shown_ಹೊಂದಿಕೆ != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_preinsert_longest(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_autocomplete
	&& (get_cot_flags() & (COT_LONGEST | COT_PREINSERT | COT_FUZZY))
		== COT_LONGEST;
}

/*
 * Add a match to the list of matches. The arguments are:
 *     str       - text of the match to add
 *     len       - length of "str". If -1, then the length of "str" is
 *		   computed.
 *     fname     - file name to associate with this match.
 *     cptext    - list of strings to use with this match (for abbr, menu, info
 *		   and kind)
 *     user_data - user supplied data (any vim type) for this match
 *     cdir	 - match direction. If 0, use "compl_direction".
 *     flags_arg - match flags (cp_flags)
 *     adup	 - accept this match even if it is already present.
 *     *user_hl  - list of extra highlight attributes for abbr kind.
 * If "cdir" is FORWARD, then the match is added after the current match.
 * Otherwise, it is added before the current match.
 *
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_ಕೂಡಿಸು(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_ದತ್ತಾಂಶ ಬಳಸದ,  // "user_data" entry or NULL
    ಪೂರ್ಣಾಂಕ		cdir,
    ಪೂರ್ಣಾಂಕ		flags_arg,
    ಪೂರ್ಣಾಂಕ		adup,		    // accept duplicate match
    ಪೂರ್ಣಾಂಕ		*user_hl,           // user abbr/kind hlattr
    ಪೂರ್ಣಾಂಕ		score)
{
    compl_T	*ಹೊಂದಿಕೆ, *current, *prev;
    ಪೂರ್ಣಾಂಕ		dir = (cdir == 0 ? compl_direction : cdir);
    ಪೂರ್ಣಾಂಕ		flags = flags_arg;
    ಪೂರ್ಣಾಂಕ		inserted = FALSE;
    char_u	*new_str = ಖಾಲಿ;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    ಆದರೆ (flags & CP_FAST)
	fast_breakcheck();
    ಇಲ್ಲದಿದ್ದರೆ
	ui_breakcheck();
    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (len < 0)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);

    // If the same match is already present, don't add it.
    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ != ಖಾಲಿ && !adup && compl_ಅಕ್ಷರಸರಣಿಗಳು_ht.ht_ಬಳಸಿದ > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *ಕೀಲಿ;
	hashitem_T  *hi;

	ಆದರೆ (len < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    ಕೀಲಿ = keybuf;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    new_str = vim_strnsave(str, len);
	    ಆದರೆ (new_str == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಕೀಲಿ = new_str;
	}
	str_hash = hash_hash(ಕೀಲಿ);
	hi = hash_lookup(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht, ಕೀಲಿ, str_hash);
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    ಆದರೆ (is_ಹತ್ತಿರದ_ಸಕ್ರಿಯ() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
		ಮಾಡು
		{
		    ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(ಹೊಂದಿಕೆ)
			    && STRNCMP(ಹೊಂದಿಕೆ->cp_str.string, str, len) == 0
			    && ((ಪೂರ್ಣಾಂಕ)ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ <= len
					  || ಹೊಂದಿಕೆ->cp_str.string[len] == NUL))
		    {
			ಆದರೆ (score < ಹೊಂದಿಕೆ->cp_score)
			    ಹೊಂದಿಕೆ->cp_score = score;
			ನಿಲ್ಲಿಸು;
		    }
		    ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
		} ಆಗ (ಹೊಂದಿಕೆ != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ));
	    }
	    vim_free(new_str);
	    ಹಿಂತಿರುಗಿ NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    ಹೊಂದಿಕೆ = ALLOC_CLEAR_ONE(compl_T);
    ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ)
    {
	vim_free(new_str);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ = flags & CP_ORIGINAL_ಪಠ್ಯ ? 0 : -1;
    ಹೊಂದಿಕೆ->cp_ಕ್ರಮ = -1;
    ಆದರೆ (new_str == ಖಾಲಿ && (new_str = vim_strnsave(str, len)) == ಖಾಲಿ)
    {
	vim_free(ಹೊಂದಿಕೆ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹೊಂದಿಕೆ->cp_str.string = new_str;
    ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    ಆದರೆ (fname != ಖಾಲಿ
	    && compl_curr_ಹೊಂದಿಕೆ != ಖಾಲಿ
	    && compl_curr_ಹೊಂದಿಕೆ->cp_fname != ಖಾಲಿ
	    && STRCMP(fname, compl_curr_ಹೊಂದಿಕೆ->cp_fname) == 0)
	ಹೊಂದಿಕೆ->cp_fname = compl_curr_ಹೊಂದಿಕೆ->cp_fname;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fname != ಖಾಲಿ)
    {
	ಹೊಂದಿಕೆ->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹೊಂದಿಕೆ->cp_fname = ಖಾಲಿ;
    ಹೊಂದಿಕೆ->cp_flags = flags;
    ಹೊಂದಿಕೆ->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    ಹೊಂದಿಕೆ->cp_user_ಬಗೆ_hl_id = user_hl ? user_hl[1] : 0;
    ಹೊಂದಿಕೆ->cp_score = score;
    ಹೊಂದಿಕೆ->cp_cpt_ಮೂಲ_idx = cpt_sources_ಸೂಚ್ಯಂಕ;

    ಆದರೆ (cptext != ಖಾಲಿ)
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CPT_ಎಣಿಕೆ; ++i)
	{
	    ಆದರೆ (cptext[i] != ಖಾಲಿ && *cptext[i] != NUL)
		ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[i] = vim_strsave(cptext[i]);
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (user_ದತ್ತಾಂಶ != ಖಾಲಿ)
	ಹೊಂದಿಕೆ->cp_user_ದತ್ತಾಂಶ = *user_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = ಹೊಂದಿಕೆ->cp_prev = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	prev = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	ಆಗ (current != ಖಾಲಿ && current != compl_ಮೊದಲ_ಹೊಂದಿಕೆ)
	{
	    ಆದರೆ (current->cp_score < score)
	    {
		ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = current;
		ಹೊಂದಿಕೆ->cp_prev = current->cp_prev;
		ಆದರೆ (current->cp_prev)
		    current->cp_prev->cp_ಮುಂದಿನ = ಹೊಂದಿಕೆ;
		current->cp_prev = ಹೊಂದಿಕೆ;
		inserted = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    prev = current;
	    current = current->cp_ಮುಂದಿನ;
	}
	ಆದರೆ (!inserted)
	{
	    prev->cp_ಮುಂದಿನ = ಹೊಂದಿಕೆ;
	    ಹೊಂದಿಕೆ->cp_prev = prev;
	    ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
	    compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev = ಹೊಂದಿಕೆ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == FORWARD)
    {
	ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = compl_curr_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	ಹೊಂದಿಕೆ->cp_prev = compl_curr_ಹೊಂದಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ	// BACKWARD
    {
	ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = compl_curr_ಹೊಂದಿಕೆ;
	ಹೊಂದಿಕೆ->cp_prev = compl_curr_ಹೊಂದಿಕೆ->cp_prev;
    }
    ಆದರೆ (ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ)
	ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ->cp_prev = ಹೊಂದಿಕೆ;
    ಆದರೆ (ಹೊಂದಿಕೆ->cp_prev)
	ಹೊಂದಿಕೆ->cp_prev->cp_ಮುಂದಿನ = ಹೊಂದಿಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ	// if there's nothing before, it is the first match
	compl_ಮೊದಲ_ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ;
    compl_curr_ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ;

    ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(ಹೊಂದಿಕೆ))
	compl_ಅಕ್ಷರಸರಣಿಗಳು_ಕೂಡಿಸು(ಹೊಂದಿಕೆ->cp_str.string, ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ,
								    str_hash);

    // Find the longest common string if still doing that.
    ಆದರೆ (compl_get_longest && (flags & CP_ORIGINAL_ಪಠ್ಯ) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_ಬಹುಲಕ_thesaurus())
	ins_compl_longest_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_equal(compl_T *ಹೊಂದಿಕೆ, char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    ಆದರೆ (ಹೊಂದಿಕೆ->cp_flags & CP_EQUAL)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (ಹೊಂದಿಕೆ->cp_flags & CP_ICASE)
	ಹಿಂತಿರುಗಿ STRNICMP(ಹೊಂದಿಕೆ->cp_str.string, str, (size_t)len) == 0;
    ಹಿಂತಿರುಗಿ STRNCMP(ಹೊಂದಿಕೆ->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_equal_sc(compl_T *ಹೊಂದಿಕೆ, char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ	typed = compl_ಉದ್ದಳತೆ;
    ಪೂರ್ಣಾಂಕ	longest_ಕೊನೆ = (compl_get_longest && compl_longest_ಕೊನೆ_ಲಂಬಸಾಲು > compl_ಲಂಬಸಾಲು)
			    ? (ಪೂರ್ಣಾಂಕ)(compl_longest_ಕೊನೆ_ಲಂಬಸಾಲು - compl_ಲಂಬಸಾಲು) : typed;

    ಆದರೆ ((ಹೊಂದಿಕೆ->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_ಕೊನೆ <= typed)
	ಹಿಂತಿರುಗಿ ins_compl_equal(ಹೊಂದಿಕೆ, str, len);

    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ < len)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; ++i)
    {
	ಆದರೆ (i >= typed && i < longest_ಕೊನೆ
		? TOLOWER_LOC(ಹೊಂದಿಕೆ->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: ಹೊಂದಿಕೆ->cp_str.string[i] != str[i])
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(char_u *p, ಪೂರ್ಣಾಂಕ len)
{
    ಆದರೆ (len == -1)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
    ins_ಬೈಟುಗಳು_len(p, len);
    compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_leader.string != ಖಾಲಿ ? compl_leader.string : compl_orig_ಪಠ್ಯ.string;
}

/*
 * Get current completion leader length
 */
    ಸ್ಥಿರ size_t
ins_compl_leader_len(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_leader.string != ಖಾಲಿ ? compl_leader.ಉದ್ದಳತೆ : compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_ಲಂಬಸಾಲು_range_attr(linenr_T lnum, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ	    ಆರಂಭ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    ಪೂರ್ಣಾಂಕ	    attr;

    ಆದರೆ (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	ಹಿಂತಿರುಗಿ -1;

    ಆರಂಭ_ಲಂಬಸಾಲು = compl_ಲಂಬಸಾಲು + (ಪೂರ್ಣಾಂಕ)ins_compl_leader_len();
    ಆದರೆ (!ins_compl_has_ಅಪವರ್ತ್ಯ())
	ಹಿಂತಿರುಗಿ (ಲಂಬಸಾಲು >= ಆರಂಭ_ಲಂಬಸಾಲು && ಲಂಬಸಾಲು < compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು) ? attr : -1;

    // Multiple lines
    ಆದರೆ ((lnum == compl_lnum && ಲಂಬಸಾಲು >= ಆರಂಭ_ಲಂಬಸಾಲು && ಲಂಬಸಾಲು < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && ಲಂಬಸಾಲು <= compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು))
	ಹಿಂತಿರುಗಿ attr;

    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_has_ಅಪವರ್ತ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ vim_strchr(compl_shown_ಹೊಂದಿಕೆ->cp_str.string, '\n') != ಖಾಲಿ;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_lnum_ಒಳ_range(linenr_T lnum)
{
    ಆದರೆ (!ins_compl_has_ಅಪವರ್ತ್ಯ())
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_longest_ಹೊಂದಿಕೆ(compl_T *ಹೊಂದಿಕೆ)
{
    char_u	*p, *s;
    ಪೂರ್ಣಾಂಕ		c1, c2;
    ಪೂರ್ಣಾಂಕ		had_ಹೊಂದಿಕೆ;

    ಆದರೆ (compl_leader.string == ಖಾಲಿ)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(ಹೊಂದಿಕೆ->cp_str.string, ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ);
	ಆದರೆ (compl_leader.string == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	compl_leader.ಉದ್ದಳತೆ = ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ;
	had_ಹೊಂದಿಕೆ = (curwin->w_cursor.ಲಂಬಸಾಲು > compl_ಲಂಬಸಾಲು);
	ins_compl_longest_ಸೇರಿಸು(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ಆದರೆ (!had_ಹೊಂದಿಕೆ)
	    ins_compl_delete();
	compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = FALSE;

	ಹಿಂತಿರುಗಿ;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = ಹೊಂದಿಕೆ->cp_str.string;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    c1 = *p;
	    c2 = *s;
	}
	ಆದರೆ ((ಹೊಂದಿಕೆ->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ++p;
	    ++s;
	}
    }

    ಆದರೆ (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.ಉದ್ದಳತೆ = (size_t)(p - compl_leader.string);

	had_ಹೊಂದಿಕೆ = (curwin->w_cursor.ಲಂಬಸಾಲು > compl_ಲಂಬಸಾಲು);
	ins_compl_longest_ಸೇರಿಸು(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ಆದರೆ (!had_ಹೊಂದಿಕೆ)
	    ins_compl_delete();
    }

    compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಕೂಡಿಸು_matches(
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches,
    char_u	**matches,
    ಪೂರ್ಣಾಂಕ		icase)
{
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_r = OK;
    ಪೂರ್ಣಾಂಕ		dir = compl_direction;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_matches && ಕೂಡಿಸು_r != FAIL; i++)
    {
	ಕೂಡಿಸು_r = ins_compl_ಕೂಡಿಸು(matches[i], -1, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ಖಾಲಿ,
		FUZZY_SCORE_NONE);
	ಆದರೆ (ಕೂಡಿಸು_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(ಅಂಕೆ_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_make_cyclic(ಶೂನ್ಯ)
{
    compl_T *ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    // Find the end of the list.
    ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    // there's always an entry for the compl_orig_text, it doesn't count.
    ಆಗ (ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ))
    {
	ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	++ಎಣಿಕೆ_2;
    }
    ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev = ಹೊಂದಿಕೆ;

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

/*
 * Return whether there currently is a shown match.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_has_shown_ಹೊಂದಿಕೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_shown_ಹೊಂದಿಕೆ == ಖಾಲಿ
	|| compl_shown_ಹೊಂದಿಕೆ != compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
}

/*
 * Return whether the shown match is long enough.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_long_shown_ಹೊಂದಿಕೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)compl_shown_ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ
					    > curwin->w_cursor.ಲಂಬಸಾಲು - compl_ಲಂಬಸಾಲು;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
get_cot_flags(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_upd_pum(ಶೂನ್ಯ)
{
    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ h = curwin->w_cline_ಎತ್ತರ;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    ಆದರೆ (h != curwin->w_cline_ಎತ್ತರ)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_del_pum(ಶೂನ್ಯ)
{
    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pum_undisplay();
    VIM_CLEAR(compl_ಹೊಂದಿಕೆ_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    ಪೂರ್ಣಾಂಕ
pum_wanted(ಶೂನ್ಯ)
{
    // 'completeopt' must contain "menu" or "menuone"
    ಹಿಂತಿರುಗಿ (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pum_enough_matches(ಶೂನ್ಯ)
{
    compl_T     *compl;
    ಪೂರ್ಣಾಂಕ		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಮಾಡು
    {
	ಆದರೆ (compl == ಖಾಲಿ || (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl) && ++i == 2))
	    ನಿಲ್ಲಿಸು;
	compl = compl->cp_ಮುಂದಿನ;
    } ಆಗ (!is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl));

    ಆದರೆ ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	ಹಿಂತಿರುಗಿ (i >= 1);
    ಹಿಂತಿರುಗಿ (i >= 2);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    ಸ್ಥಿರ dict_T *
ins_compl_dict_alloc(compl_T *ಹೊಂದಿಕೆ)
{
    dict_T *dict = dict_alloc_ಬೀಗ(VAR_FIXED);

    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    fill_complete_info_dict(dict, ಹೊಂದಿಕೆ, FALSE);
    ಹಿಂತಿರುಗಿ dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_complete_ಬದಲಾಯಿತು_event(ಪೂರ್ಣಾಂಕ cur)
{
    dict_T	    *v_event;
    dict_T	    *ಅಂಶ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    recursive = FALSE;
    save_v_event_T  save_v_event;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;

    ಅಂಶ = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_ಹೊಂದಿಕೆ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    v_event = get_v_event(&save_v_event);
    dict_ಕೂಡಿಸು_dict(v_event, "completed_item", ಅಂಶ);
    pum_ಗಣ_event_info(v_event);
    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Helper functions for mergesort_list().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ*
cp_get_ಮುಂದಿನ(ಶೂನ್ಯ *ಗಂಟು)
{
    ಹಿಂತಿರುಗಿ ((compl_T*)ಗಂಟು)->cp_ಮುಂದಿನ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cp_ಗಣ_ಮುಂದಿನ(ಶೂನ್ಯ *ಗಂಟು, ಶೂನ್ಯ *ಮುಂದಿನ)
{
    ((compl_T*)ಗಂಟು)->cp_ಮುಂದಿನ = (compl_T*)ಮುಂದಿನ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ*
cp_get_prev(ಶೂನ್ಯ* ಗಂಟು)
{
    ಹಿಂತಿರುಗಿ ((compl_T*)ಗಂಟು)->cp_prev;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cp_ಗಣ_prev(ಶೂನ್ಯ* ಗಂಟು, ಶೂನ್ಯ* prev)
{
    ((compl_T*)ಗಂಟು)->cp_prev = (compl_T*)prev;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cp_compare_fuzzy(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* b)
{
    ಪೂರ್ಣಾಂಕ score_a = ((compl_T*)a)->cp_score;
    ಪೂರ್ಣಾಂಕ score_b = ((compl_T*)b)->cp_score;

    ಆದರೆ (score_a != score_b)
	ಹಿಂತಿರುಗಿ (score_b > score_a) ? 1 : -1;

    ಹಿಂತಿರುಗಿ ((compl_T *)a)->cp_ಕ್ರಮ - ((compl_T *)b)->cp_ಕ್ರಮ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cp_compare_ಹತ್ತಿರದ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* b)
{
    ಪೂರ್ಣಾಂಕ score_a = ((compl_T*)a)->cp_score;
    ಪೂರ್ಣಾಂಕ score_b = ((compl_T*)b)->cp_score;
    ಆದರೆ (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prepend_startcol_ಪಠ್ಯ(string_T *dest, string_T *src, ಪೂರ್ಣಾಂಕ startcol)
{
    ಪೂರ್ಣಾಂಕ prepend_len = compl_ಲಂಬಸಾಲು - startcol;
    ಪೂರ್ಣಾಂಕ new_ಉದ್ದಳತೆ = prepend_len + (ಪೂರ್ಣಾಂಕ)src->ಉದ್ದಳತೆ;

    dest->ಉದ್ದಳತೆ = (size_t)new_ಉದ್ದಳತೆ;
    dest->string = alloc(new_ಉದ್ದಳತೆ + 1);  // +1 for NUL
    ಆದರೆ (dest->string == ಖಾಲಿ)
    {
	dest->ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    char_u	*ಸಾಲು = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, ಸಾಲು + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->ಉದ್ದಳತೆ);
    dest->string[new_ಉದ್ದಳತೆ] = NUL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    ಸ್ಥಿರ string_T *
get_leader_for_startcol(compl_T *ಹೊಂದಿಕೆ, ಪೂರ್ಣಾಂಕ cached)
{
    ಸ್ಥಿರ string_T adjusted_leader = {ಖಾಲಿ, 0};

    ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (cpt_sources_array == ಖಾಲಿ)
	ಹೋಗು theend;

    ಪೂರ್ಣಾಂಕ	cpt_idx = ಹೊಂದಿಕೆ->cp_cpt_ಮೂಲ_idx;
    ಆದರೆ (cpt_idx < 0)
	ಹೋಗು theend;
    ಪೂರ್ಣಾಂಕ	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    ಆದರೆ (compl_leader.string == ಖಾಲಿ)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	ಆದರೆ (startcol < 0 || startcol >= compl_ಲಂಬಸಾಲು)
	    ಹಿಂತಿರುಗಿ &compl_orig_ಪಠ್ಯ;
	ಹಿಂತಿರುಗಿ &compl_leader;  // pass through (startcol < compl_col)
    }

    ಆದರೆ (compl_ಲಂಬಸಾಲು <= 0)
	ಹೋಗು theend;

    ಆದರೆ (startcol >= 0 && startcol < compl_ಲಂಬಸಾಲು)
    {
	ಪೂರ್ಣಾಂಕ prepend_len = compl_ಲಂಬಸಾಲು - startcol;
	ಪೂರ್ಣಾಂಕ new_ಉದ್ದಳತೆ = prepend_len + (ಪೂರ್ಣಾಂಕ)compl_leader.ಉದ್ದಳತೆ;
	ಆದರೆ (cached && (size_t)new_ಉದ್ದಳತೆ == adjusted_leader.ಉದ್ದಳತೆ
		&& adjusted_leader.string != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	ಆದರೆ (prepend_startcol_ಪಠ್ಯ(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    ಹೋಗು theend;

	ಹಿಂತಿರುಗಿ &adjusted_leader;
    }
theend:
    ಹಿಂತಿರುಗಿ &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_fuzzy_score(ಶೂನ್ಯ)
{
    compl_T *compl;
    char_u  *ನಮೂನೆ;
    ಪೂರ್ಣಾಂಕ	    use_leader;

    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ಖಾಲಿ && compl_leader.ಉದ್ದಳತೆ > 0);
    ಆದರೆ (!use_leader)
    {
	ಆದರೆ (compl_orig_ಪಠ್ಯ.string == ಖಾಲಿ || compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ == 0)
	    ಹಿಂತಿರುಗಿ;
	ನಮೂನೆ = compl_orig_ಪಠ್ಯ.string;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/* Clear the leader cache once before the loop */
	(ಶೂನ್ಯ)get_leader_for_startcol(ಖಾಲಿ, TRUE);
	ನಮೂನೆ = ಖಾಲಿ;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    ಪೂರ್ಣಾಂಕ ಮುಂದಿನ = 0;
    compl = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಮಾಡು
    {
	ಆದರೆ (compl->cp_ಕ್ರಮ < 0)
	    compl->cp_ಕ್ರಮ = ಮುಂದಿನ;
	ಮುಂದಿನ = MAX(ಮುಂದಿನ, compl->cp_ಕ್ರಮ + 1);

	ಆದರೆ (use_leader)
	    ನಮೂನೆ = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_ಹೊಂದಿಕೆ_str(compl->cp_str.string, ನಮೂನೆ);
	compl = compl->cp_ಮುಂದಿನ;
    } ಆಗ (compl != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ವಿಂಗಡಿಸು_compl_ಹೊಂದಿಕೆ_list(ಪೂರ್ಣಾಂಕ (*compare)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *))
{
    compl_T	*orig_ಪಠ್ಯ;

    ಆದರೆ (!compl_ಮೊದಲ_ಹೊಂದಿಕೆ || is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ))
	ಹಿಂತಿರುಗಿ;

    orig_ಪಠ್ಯ = ಕಂಡುಹಿಡಿ_original_ಪಠ್ಯ_ಹೊಂದಿಕೆ();
    ಆದರೆ (orig_ಪಠ್ಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ins_compl_make_ರೇಖೀಯ();
    ಆದರೆ (orig_ಪಠ್ಯ == compl_ಮೊದಲ_ಹೊಂದಿಕೆ)
    {
	compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ->cp_prev = ಖಾಲಿ;
	compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ = mergesort_list(compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ,
		cp_get_ಮುಂದಿನ, cp_ಗಣ_ಮುಂದಿನ, cp_get_prev, cp_ಗಣ_prev, compare);
	compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ->cp_prev = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	compl_T	*ಬಾಲ;

	orig_ಪಠ್ಯ->cp_prev->cp_ಮುಂದಿನ = ಖಾಲಿ;
	compl_ಮೊದಲ_ಹೊಂದಿಕೆ = mergesort_list(compl_ಮೊದಲ_ಹೊಂದಿಕೆ, cp_get_ಮುಂದಿನ,
		cp_ಗಣ_ಮುಂದಿನ, cp_get_prev, cp_ಗಣ_prev, compare);
	ಬಾಲ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
	ಆಗ (ಬಾಲ->cp_ಮುಂದಿನ != ಖಾಲಿ)
	    ಬಾಲ = ಬಾಲ->cp_ಮುಂದಿನ;
	ಬಾಲ->cp_ಮುಂದಿನ = orig_ಪಠ್ಯ;
	orig_ಪಠ್ಯ->cp_prev = ಬಾಲ;
    }
    (ಶೂನ್ಯ)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_user_highlight_attr(ಪೂರ್ಣಾಂಕ hl_id)
{
    ಪೂರ್ಣಾಂಕ	    attr;

    ಆದರೆ (hl_id <= 0)
	ಹಿಂತಿರುಗಿ -1;
    attr = syn_id2attr(hl_id);
    ಹಿಂತಿರುಗಿ attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_build_pum(ಶೂನ್ಯ)
{
    compl_T     *compl;
    compl_T     *shown_compl = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		did_ಕಂಡುಹಿಡಿ_shown_ಹೊಂದಿಕೆ = FALSE;
    ಪೂರ್ಣಾಂಕ		shown_ಹೊಂದಿಕೆ_ok = FALSE;
    ಪೂರ್ಣಾಂಕ		i = 0;
    ಪೂರ್ಣಾಂಕ		cur = -1;
    ಪೂರ್ಣಾಂಕ		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*ಹೊಂದಿಕೆ_ತಲೆ = ಖಾಲಿ;
    compl_T	*ಹೊಂದಿಕೆ_ಬಾಲ = ಖಾಲಿ;
    compl_T	*ಹೊಂದಿಕೆ_ಮುಂದಿನ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		*ಹೊಂದಿಕೆ_ಎಣಿಕೆ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		is_forward = compl_shows_dir_forward();
    ಪೂರ್ಣಾಂಕ		is_cpt_completion = (cpt_sources_array != ಖಾಲಿ);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_ಹೊಂದಿಕೆ_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    ಆದರೆ (ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ))
	shown_ಹೊಂದಿಕೆ_ok = TRUE;

    ಆದರೆ (compl_leader.string != ಖಾಲಿ
	    && STRCMP(compl_leader.string, compl_orig_ಪಠ್ಯ.string) == 0
	    && shown_ಹೊಂದಿಕೆ_ok == FALSE)
	compl_shown_ಹೊಂದಿಕೆ = compl_no_select ? compl_ಮೊದಲ_ಹೊಂದಿಕೆ
					    : compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;

    ಆದರೆ (is_cpt_completion)
    {
	ಹೊಂದಿಕೆ_ಎಣಿಕೆ = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಂಕ, cpt_sources_ಎಣಿಕೆ);
	ಆದರೆ (ಹೊಂದಿಕೆ_ಎಣಿಕೆ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
    }

    (ಶೂನ್ಯ)get_leader_for_startcol(ಖಾಲಿ, TRUE); // Clear the cache

    compl = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಮಾಡು
    {
	compl->cp_ಒಳ_ಹೊಂದಿಕೆ_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && !p_inf && compl_orig_ಪಠ್ಯ.string
		&& !ignorecase(compl_orig_ಪಠ್ಯ.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl)
		&& (leader->string == ಖಾಲಿ
		    || ins_compl_equal_sc(compl, leader->string,
			(ಪೂರ್ಣಾಂಕ)leader->ಉದ್ದಳತೆ)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ_ಎಲ್ಲೆ_exceeded = FALSE;
	    ಪೂರ್ಣಾಂಕ cur_ಮೂಲ = compl->cp_cpt_ಮೂಲ_idx;
	    ಆದರೆ (is_forward && cur_ಮೂಲ != -1 && is_cpt_completion)
	    {
		ಹೊಂದಿಕೆ_ಎಣಿಕೆ[cur_ಮೂಲ]++;
		ಪೂರ್ಣಾಂಕ max_matches = cpt_sources_array[cur_ಮೂಲ].cs_max_matches;
		ಆದರೆ (max_matches > 0 && ಹೊಂದಿಕೆ_ಎಣಿಕೆ[cur_ಮೂಲ] > max_matches)
		    ಹೊಂದಿಕೆ_ಎಲ್ಲೆ_exceeded = TRUE;
	    }

	    ಆದರೆ (!ಹೊಂದಿಕೆ_ಎಲ್ಲೆ_exceeded)
	    {
		++compl_ಹೊಂದಿಕೆ_arraysize;
		compl->cp_ಒಳ_ಹೊಂದಿಕೆ_array = TRUE;
		ಆದರೆ (ಹೊಂದಿಕೆ_ತಲೆ == ಖಾಲಿ)
		    ಹೊಂದಿಕೆ_ತಲೆ = compl;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೊಂದಿಕೆ_ಬಾಲ->cp_ಹೊಂದಿಕೆ_ಮುಂದಿನ = compl;
		ಹೊಂದಿಕೆ_ಬಾಲ = compl;

		ಆದರೆ (!shown_ಹೊಂದಿಕೆ_ok && !cot_fuzzy())
		{
		    ಆದರೆ (compl == compl_shown_ಹೊಂದಿಕೆ || did_ಕಂಡುಹಿಡಿ_shown_ಹೊಂದಿಕೆ)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_ಹೊಂದಿಕೆ = compl;
			did_ಕಂಡುಹಿಡಿ_shown_ಹೊಂದಿಕೆ = TRUE;
			shown_ಹೊಂದಿಕೆ_ok = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cot_fuzzy())
		{
		    ಆದರೆ (i == 0)
			shown_compl = compl;

		    ಆದರೆ (!shown_ಹೊಂದಿಕೆ_ok && compl == compl_shown_ಹೊಂದಿಕೆ)
		    {
			cur = i;
			shown_ಹೊಂದಿಕೆ_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	ಆದರೆ (compl == compl_shown_ಹೊಂದಿಕೆ && !cot_fuzzy())
	{
	    did_ಕಂಡುಹಿಡಿ_shown_ಹೊಂದಿಕೆ = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    ಆದರೆ (ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl))
		shown_ಹೊಂದಿಕೆ_ok = TRUE;

	    ಆದರೆ (!shown_ಹೊಂದಿಕೆ_ok && shown_compl != ಖಾಲಿ)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_ಹೊಂದಿಕೆ = shown_compl;
		shown_ಹೊಂದಿಕೆ_ok = TRUE;
	    }
	}
	compl = compl->cp_ಮುಂದಿನ;
    } ಆಗ (compl != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl));

    vim_free(ಹೊಂದಿಕೆ_ಎಣಿಕೆ);

    ಆದರೆ (compl_ಹೊಂದಿಕೆ_arraysize == 0)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (cot_fuzzy() && !compl_no_select && !shown_ಹೊಂದಿಕೆ_ok)
    {
	compl_shown_ಹೊಂದಿಕೆ = shown_compl;
	shown_ಹೊಂದಿಕೆ_ok = TRUE;
	cur = 0;
    }

    compl_ಹೊಂದಿಕೆ_array = ALLOC_CLEAR_MULT(pumitem_T, compl_ಹೊಂದಿಕೆ_arraysize);
    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    compl = ಹೊಂದಿಕೆ_ತಲೆ;
    i = 0;
    ಆಗ (compl != ಖಾಲಿ)
    {
	compl_ಹೊಂದಿಕೆ_array[i].pum_ಪಠ್ಯ = compl->cp_ಪಠ್ಯ[CPT_ABBR] != ಖಾಲಿ
			    ? compl->cp_ಪಠ್ಯ[CPT_ABBR] : compl->cp_str.string;
	compl_ಹೊಂದಿಕೆ_array[i].pum_ಬಗೆ = compl->cp_ಪಠ್ಯ[CPT_ಬಗೆ];
	compl_ಹೊಂದಿಕೆ_array[i].pum_info = compl->cp_ಪಠ್ಯ[CPT_INFO];
	compl_ಹೊಂದಿಕೆ_array[i].pum_cpt_ಮೂಲ_idx = compl->cp_cpt_ಮೂಲ_idx;
	compl_ಹೊಂದಿಕೆ_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_ಹೊಂದಿಕೆ_array[i].pum_user_ಬಗೆ_hlattr =
			get_user_highlight_attr(compl->cp_user_ಬಗೆ_hl_id);
	compl_ಹೊಂದಿಕೆ_array[i++].pum_extra = compl->cp_ಪಠ್ಯ[CPT_MENU] != ಖಾಲಿ
			    ? compl->cp_ಪಠ್ಯ[CPT_MENU] : compl->cp_fname;
	ಹೊಂದಿಕೆ_ಮುಂದಿನ = compl->cp_ಹೊಂದಿಕೆ_ಮುಂದಿನ;
	compl->cp_ಹೊಂದಿಕೆ_ಮುಂದಿನ = ಖಾಲಿ;
	compl = ಹೊಂದಿಕೆ_ಮುಂದಿನ;
    }

    ಆದರೆ (!shown_ಹೊಂದಿಕೆ_ok)    // no displayed match at all
	cur = -1;

    ಹಿಂತಿರುಗಿ cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    ಶೂನ್ಯ
ins_compl_show_pum(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		cur = -1;
    colnr_T	ಲಂಬಸಾಲು;

    ಆದರೆ (!pum_wanted() || !pum_enough_matches())
	ಹಿಂತಿರುಗಿ;

    // Avoid redrawing the screen under a pum that stays in place.
    ಆದರೆ (!pum_redraw_ಒಳ_same_position())
	pum_call_update_screen();

    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// popup menu already exists, only need to find the current item.
	ಗಾಗಿ (i = 0; i < compl_ಹೊಂದಿಕೆ_arraysize; ++i)
	{
	    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array[i].pum_ಪಠ್ಯ == compl_shown_ಹೊಂದಿಕೆ->cp_str.string
		    || compl_ಹೊಂದಿಕೆ_array[i].pum_ಪಠ್ಯ
				      == compl_shown_ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[CPT_ABBR])
	    {
		cur = i;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (compl_started && has_completechanged())
	    trigger_complete_ಬದಲಾಯಿತು_event(cur);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    curwin->w_cursor.ಲಂಬಸಾಲು = compl_ಲಂಬಸಾಲು;
    validate_cursor_ಲಂಬಸಾಲು();
    ಪೂರ್ಣಾಂಕ pum_wcol = curwin->w_wcol;
    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    validate_cursor_ಲಂಬಸಾಲು();
    compl_selected_ಅಂಶ = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_ಸ್ಥಿತಿ = true;
    pum_display(compl_ಹೊಂದಿಕೆ_array, compl_ಹೊಂದಿಕೆ_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    ಆದರೆ (compl_started && compl_curr_ಹೊಂದಿಕೆ != compl_shown_ಹೊಂದಿಕೆ)
	compl_curr_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (has_completechanged())
	trigger_complete_ಬದಲಾಯಿತು_event(cur);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿಸು DICT_ಮೊದಲ	(1)	// use just first element in "dict"
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_dictionaries(
    char_u	*dict_ಆರಂಭ,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		flags,		// DICT_FIRST and/or DICT_EXACT
    ಪೂರ್ಣಾಂಕ		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_ಆರಂಭ;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		save_p_scs;
    ಪೂರ್ಣಾಂಕ		dir = compl_direction;

    ಆದರೆ (*dict == NUL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	ಆದರೆ (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ;
    }

    buf = alloc(LSIZE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    regmatch.regprog = ಖಾಲಿ;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ಆದರೆ (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	ಆದರೆ (pat_esc == ಖಾಲಿ)
	    ಹೋಗು theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	ಆದರೆ (ptr == ಖಾಲಿ)
	{
	    vim_free(pat_esc);
	    ಹೋಗು theend;
	}
	vim_snprintf((ಅಕ್ಷರ *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	    ಹೋಗು theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    ಆಗ (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	ಆದರೆ (flags == DICT_EXACT)
	{
	    ಎಣಿಕೆ_2 = 1;
	    files = &dict;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	    ಆದರೆ (!thesaurus && STRCMP(buf, "spell") == 0)
		ಎಣಿಕೆ_2 = -1;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (vim_strchr(buf, '`') != ಖಾಲಿ
		    || expand_wildcards(1, &buf, &ಎಣಿಕೆ_2, &files,
						     EW_FILE|EW_SILENT) != OK)
		ಎಣಿಕೆ_2 = 0;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	ಆದರೆ (ಎಣಿಕೆ_2 == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    ಆದರೆ (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (ಎಣಿಕೆ_2 > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(ಎಣಿಕೆ_2, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    ಆದರೆ (flags != DICT_EXACT)
		FreeWild(ಎಣಿಕೆ_2, files);
	}
	ಆದರೆ (flags != 0)
	    ನಿಲ್ಲಿಸು;
    }

theend:
    p_scs = save_p_scs;
    vim_regfree(regmatch.regprog);
    vim_free(buf);
}

/*
 * Add all the words in the line "*buf_arg" from the thesaurus file "fname"
 * skipping the word at 'skip_word'.  Returns OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
thesaurus_ಕೂಡಿಸು_ಪದಗಳು_ಒಳ_ಸಾಲು(
    char_u	*fname,
    char_u	**buf_arg,
    ಪೂರ್ಣಾಂಕ		dir,
    char_u	*skip_ಶಬ್ದ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    ಆಗ (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(ptr);
	ಆದರೆ (*ptr == NUL || *ptr == NL)
	    ನಿಲ್ಲಿಸು;
	wstart = ptr;

	// Find end of the word.
	ಆದರೆ (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    ಆಗ (*ptr != NUL)
	    {
		ಪೂರ್ಣಾಂಕ l = (*mb_ptr2len)(ptr);

		ಆದರೆ (l < 2 && !vim_iswordc(*ptr))
		    ನಿಲ್ಲಿಸು;
		ptr += l;
	    }
	ಇಲ್ಲದಿದ್ದರೆ
	    ptr = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(ptr);

	// Add the word. Skip the regexp match.
	ಆದರೆ (wstart != skip_ಶಬ್ದ)
	{
	    ಸ್ಥಿತಿ_2 = ins_compl_ಕೂಡಿಸು_infercase(wstart, (ಪೂರ್ಣಾಂಕ)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
		ನಿಲ್ಲಿಸು;
	}
    }

    *buf_arg = ptr;
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_files(
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    char_u	**files,
    ಪೂರ್ಣಾಂಕ		thesaurus,
    ಪೂರ್ಣಾಂಕ		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		*dir)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		i;
    FILE	*fp;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_r;
    char_u	*leader = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		leader_len = 0;
    ಪೂರ್ಣಾಂಕ		score = 0;
    ಪೂರ್ಣಾಂಕ		len = 0;
    char_u	*ಸಾಲು_ಕೊನೆ = ಖಾಲಿ;

    ಆದರೆ (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (ಪೂರ್ಣಾಂಕ)ins_compl_leader_len();
    }

    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2 && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((ಅಕ್ಷರ *)files[i], "r");  // open dictionary file
	ಆದರೆ (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (ಅಕ್ಷರ *)files[i]);
	    (ಶೂನ್ಯ)ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	ಆದರೆ (fp == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// Read dictionary file line by line.
	// Check each line for a match.
	ಆಗ (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    ಆದರೆ (cot_fuzzy() && leader_len > 0)
	    {
		ಸಾಲು_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಸಾಲು_ಕೊನೆ(ptr);
		ಆಗ (ptr < ಸಾಲು_ಕೊನೆ)
		{
		    ಆದರೆ (fuzzy_ಹೊಂದಿಕೆ_str_ಒಳ_ಸಾಲು(&ptr, leader, &len, ಖಾಲಿ, &score))
		    {
			char_u *end_ptr = ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval()
					? ಕಂಡುಹಿಡಿ_ಸಾಲು_ಕೊನೆ(ptr) : ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(ptr);
			ಕೂಡಿಸು_r = ins_compl_ಕೂಡಿಸು_infercase(ptr, (ಪೂರ್ಣಾಂಕ)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			ಆದರೆ (ಕೂಡಿಸು_r == FAIL)
			    ನಿಲ್ಲಿಸು;
			ptr = end_ptr;  // start from next word
			ಆದರೆ (compl_get_longest && ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ()
				&& compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ
				&& score == compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ->cp_score)
			    compl_ಅಂಕೆ_bests++;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regmatch != ಖಾಲಿ)
	    {
		ಆಗ (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval() ? ಕಂಡುಹಿಡಿ_ಸಾಲು_ಕೊನೆ(ptr)
						    : ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(ptr);
		    ಕೂಡಿಸು_r = ins_compl_ಕೂಡಿಸು_infercase(regmatch->startp[0],
			    (ಪೂರ್ಣಾಂಕ)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    ಆದರೆ (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			ಕೂಡಿಸು_r = thesaurus_ಕೂಡಿಸು_ಪದಗಳು_ಒಳ_ಸಾಲು(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    ಆದರೆ (ಕೂಡಿಸು_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೂಡಿಸು_r == FAIL)
			ನಿಲ್ಲಿಸು;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    ಆದರೆ (*ptr == '\n' || got_int)
			ನಿಲ್ಲಿಸು;
		}
	    }
	    ಸಾಲು_breakcheck();
	    ins_compl_check_ಕೀಲಿಗಳು(50, FALSE);
	}
	fclose(fp);
    }
}

/*
 * Find the start of the next word.
 * Returns a pointer to the first char of the word.  Also stops at a NUL.
 */
    char_u *
ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(char_u *ptr)
{
    ಆದರೆ (has_mbyte)
    {
	ಆಗ (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    ಹಿಂತಿರುಗಿ ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(char_u *ptr)
{
    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ ಆರಂಭ_class = mb_get_class(ptr);
	ಆದರೆ (ಆರಂಭ_class > 1)
	    ಆಗ (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		ಆದರೆ (mb_get_class(ptr) != ಆರಂಭ_class)
		    ನಿಲ್ಲಿಸು;
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (vim_iswordc(*ptr))
	    ++ptr;
    ಹಿಂತಿರುಗಿ ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
ಕಂಡುಹಿಡಿ_ಸಾಲು_ಕೊನೆ(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    ಆಗ (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Free a completion item in the list
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಅಂಶ_free(compl_T *ಹೊಂದಿಕೆ)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    ಆದರೆ (compl_ಅಕ್ಷರಸರಣಿಗಳು_ht.ht_ಬಳಸಿದ > 0 && ಹೊಂದಿಕೆ->cp_str.string != ಖಾಲಿ
					   && !ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(ಹೊಂದಿಕೆ))
    {
	hashitem_T *hi = hash_ಕಂಡುಹಿಡಿ(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht, ಹೊಂದಿಕೆ->cp_str.string);

	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    complstr_T *ನಮೂದು = HI2CSE(hi);

	    ಆದರೆ (--ನಮೂದು->cse_ಎಣಿಕೆ <= 0)
	    {
		hash_remove(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht, hi, "completion match");
		vim_free(ನಮೂದು);
	    }
	}
    }
    VIM_CLEAR_STRING(ಹೊಂದಿಕೆ->cp_str);
    // several entries may use the same fname, free it just once.
    ಆದರೆ (ಹೊಂದಿಕೆ->cp_flags & CP_FREE_FNAME)
	vim_free(ಹೊಂದಿಕೆ->cp_fname);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CPT_ಎಣಿಕೆ; ++i)
	vim_free(ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[i]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    clear_tv(&ಹೊಂದಿಕೆ->cp_user_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(ಹೊಂದಿಕೆ);
}

/*
 * Free the list of completions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_free(ಶೂನ್ಯ)
{
    compl_T *ಹೊಂದಿಕೆ;

    VIM_CLEAR_STRING(compl_ನಮೂನೆ);
    VIM_CLEAR_STRING(compl_leader);

    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht, CSE_OFF);
    hash_init(&compl_ಅಕ್ಷರಸರಣಿಗಳು_ht);

    compl_curr_ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಮಾಡು
    {
	ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ;
	compl_curr_ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	ins_compl_ಅಂಶ_free(ಹೊಂದಿಕೆ);
    } ಆಗ (compl_curr_ಹೊಂದಿಕೆ != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_curr_ಹೊಂದಿಕೆ));
    compl_ಮೊದಲ_ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ = ಖಾಲಿ;
    compl_shown_ಹೊಂದಿಕೆ = ಖಾಲಿ;
    compl_old_ಹೊಂದಿಕೆ = ಖಾಲಿ;
}

/*
 * Reset/clear the completion state.
 */
    ಶೂನ್ಯ
ins_compl_clear(ಶೂನ್ಯ)
{
    compl_cont_ಸ್ಥಿತಿ = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_ಅಂಶ = -1;
    compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು = 0;
    compl_longest_ಕೊನೆ_ಲಂಬಸಾಲು = 0;
    compl_curr_win = ಖಾಲಿ;
    compl_curr_buf = ಖಾಲಿ;
    VIM_CLEAR_STRING(compl_ನಮೂನೆ);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ಖಾಲಿ;
    VIM_CLEAR_STRING(compl_orig_ಪಠ್ಯ);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ಅಂಕೆ_bests = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // clear v:completed_item
    ಗಣ_vim_var_dict(VV_COMPLETED_ಅಂಶ, dict_alloc_ಬೀಗ(VAR_FIXED));
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE when Insert completion is active.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_win_ಸಕ್ರಿಯ(win_T *wp)
{
    ಹಿಂತಿರುಗಿ ins_compl_ಸಕ್ರಿಯ() && wp == compl_curr_win
	&& wp->w_ಬಫರ್ == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ;
}

/*
 * Initialize get longest common string.
 */
    ಶೂನ್ಯ
ins_compl_init_get_longest(ಶೂನ್ಯ)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_interrupted(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_interrupted || compl_ಸಮಯ_ತುಂಡು_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_enter_selects(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_ಲಂಬಸಾಲು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_ಲಂಬಸಾಲು;
}

/*
 * Return the length in bytes of the text being completed
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_len(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_ಉದ್ದಳತೆ;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_has_preinsert(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ cur_cot_flags = get_cot_flags();
    ಆದರೆ (compl_autocomplete && p_ic && !p_inf)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_preinsert_effect(ಶೂನ್ಯ)
{
    ಆದರೆ (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ curwin->w_cursor.ಲಂಬಸಾಲು < compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_bs(ಶೂನ್ಯ)
{
    char_u	*ಸಾಲು;
    char_u	*p;

    ಆದರೆ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ಸಾಲು = ml_get_curline();
    p = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು;
    MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು) - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು < 0
	    || ((ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು) - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು == 0 && !ctrl_x_ಬಹುಲಕ_omni())
	    || ctrl_x_ಬಹುಲಕ_eval()
	    || (!can_bs(BS_ಆರಂಭ) && (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು) - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು
							- compl_ಉದ್ದಳತೆ < 0))
	ಹಿಂತಿರುಗಿ K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <= compl_ಲಂಬಸಾಲು + compl_ಉದ್ದಳತೆ
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.ಉದ್ದಳತೆ = (size_t)((p - ಸಾಲು) - compl_ಲಂಬಸಾಲು);
    compl_leader.string = vim_strnsave(ಸಾಲು + compl_ಲಂಬಸಾಲು, compl_leader.ಉದ್ದಳತೆ);
    ಆದರೆ (compl_leader.string == ಖಾಲಿ)
    {
	compl_leader.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    ಆದರೆ (compl_autocomplete && compl_ಮೊದಲ_ಹೊಂದಿಕೆ && !ins_compl_has_preinsert())
	compl_shown_ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;

    ins_compl_new_leader();
    ಆದರೆ (compl_shown_ಹೊಂದಿಕೆ != ಖಾಲಿ)
	// Make sure current match is not a hidden item.
	compl_curr_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ;
    ಹಿಂತಿರುಗಿ NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_need_restart(ಶೂನ್ಯ)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    ಹಿಂತಿರುಗಿ compl_was_interrupted
	|| ((ctrl_x_ಬಹುಲಕ_function() || ctrl_x_ಬಹುಲಕ_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_has_autocomplete(ಶೂನ್ಯ)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    ಹಿಂತಿರುಗಿ curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_fuzzy_ವಿಂಗಡಿಸು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    ಗಣ_fuzzy_score();
    ಆದರೆ (!(cur_cot_flags & COT_NOSORT))
    {
	ವಿಂಗಡಿಸು_compl_ಹೊಂದಿಕೆ_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	ಆದರೆ ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    ಪೂರ್ಣಾಂಕ none_selected = compl_shown_ಹೊಂದಿಕೆ == (compl_shows_dir_forward()
		    ? compl_ಮೊದಲ_ಹೊಂದಿಕೆ : compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev);
	    ಆದರೆ (!none_selected)
		compl_shown_ಹೊಂದಿಕೆ
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ : compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
	}
    }
}

/*
 * Called after changing "compl_leader".
 * Show the popup menu with a different set of matches.
 * May also search for matches again if the previous search was interrupted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_new_leader(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    save_w_wrow = curwin->w_wrow;
    ಪೂರ್ಣಾಂಕ	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(compl_leader.string + get_compl_len(), -1);
    compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = FALSE;

    ಆದರೆ (compl_started)
    {
	ins_compl_ಗಣ_original_ಪಠ್ಯ(compl_leader.string, compl_leader.ಉದ್ದಳತೆ);
	ಆದರೆ (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	ಆದರೆ (cot_fuzzy())
	    ins_compl_fuzzy_ವಿಂಗಡಿಸು();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#ಆದರೆ_ಅಂತ್ಯ
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
	}
#ಆದರೆ_ಅಂತ್ಯ
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	ಆದರೆ (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	ಇಲ್ಲದಿದ್ದರೆ
	    compl_autocomplete = FALSE;
	ಆದರೆ (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_ಸ್ಥಿತಿ = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ && compl_selected_ಅಂಶ != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    ಆದರೆ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
	compl_enter_selects = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ins_compl_has_preinsert() && compl_leader.ಉದ್ದಳತೆ > 0)
	ins_compl_ಸೇರಿಸು(TRUE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.ಉದ್ದಳತೆ > 0 && !ins_compl_preinsert_effect())
	ins_compl_ಸೇರಿಸು(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_compl_len(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ off = (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ಶೂನ್ಯ
ins_compl_addleader(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    cc;

    ಆದರೆ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ಆದರೆ (stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_ಬೈಟುಗಳು(buf, cc);
	ಆದರೆ (compl_opt_refresh_always)
	    Append_ಗಮ್ಯ_Redobuff(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ins_char(c);
	ಆದರೆ (compl_opt_refresh_always)
	    Append_Char_ಗಮ್ಯ_Redobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    ಆದರೆ (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    ಆದರೆ (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.ಉದ್ದಳತೆ = (size_t)(curwin->w_cursor.ಲಂಬಸಾಲು - compl_ಲಂಬಸಾಲು);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_ಲಂಬಸಾಲು,
		compl_leader.ಉದ್ದಳತೆ);
	ಆದರೆ (compl_leader.string == ಖಾಲಿ)
	{
	    compl_leader.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_restart(ಶೂನ್ಯ)
{
    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    compl_cont_ಸ್ಥಿತಿ = 0;
    compl_cont_ಬಹುಲಕ = 0;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ಅಂಕೆ_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಗಣ_original_ಪಠ್ಯ(char_u *str, size_t len)
{
    compl_T	*ಹೊಂದಿಕೆ = ಕಂಡುಹಿಡಿ_original_ಪಠ್ಯ_ಹೊಂದಿಕೆ();
    char_u	*p;

    // Replace the original text entry.
    ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p = vim_strnsave(str, len);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    VIM_CLEAR_STRING(ಹೊಂದಿಕೆ->cp_str);
    ಹೊಂದಿಕೆ->cp_str.string = p;
    ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ಶೂನ್ಯ
ins_compl_addfrommatch(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		c;

    p = compl_shown_ಹೊಂದಿಕೆ->cp_str.string;
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)compl_shown_ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ))
	    ಹಿಂತಿರುಗಿ;

	p = ಖಾಲಿ;
	plen = 0;
	ಗಾಗಿ (cp = compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ; cp != ಖಾಲಿ
		&& !is_ಮೊದಲ_ಹೊಂದಿಕೆ(cp); cp = cp->cp_ಮುಂದಿನ)
	{
	    ಆದರೆ (compl_leader.string == ಖಾಲಿ
		    || ins_compl_equal(cp, compl_leader.string,
			(ಪೂರ್ಣಾಂಕ)compl_leader.ಉದ್ದಳತೆ))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.ಉದ್ದಳತೆ;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (p == ಖಾಲಿ || (ಪೂರ್ಣಾಂಕ)plen <= len)
	    ಹಿಂತಿರುಗಿ;
    }
    p += len;
    c = PTR2CHAR(p);
    ins_compl_addleader(c);
}

/*
 * Set the CTRL-X completion mode based on the key "c" typed after a CTRL-X.
 * Uses the global variables: ctrl_x_mode, edit_submode, edit_submode_pre,
 * compl_cont_mode and compl_cont_status.
 * Returns TRUE when the character is not to be inserted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ctrl_x_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    retval = FALSE;

    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ Ctrl_E:
	ಸ್ಥಿತಿ Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_ಬಹುಲಕ = CTRL_X_SCROLL;
	    ಆದರೆ (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    ಇಲ್ಲದಿದ್ದರೆ
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ಖಾಲಿ;
	    showmode();
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_L:
	    // complete whole line
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_F:
	    // complete filenames
	    ctrl_x_ಬಹುಲಕ = CTRL_X_FILES;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_ಬಹುಲಕ = CTRL_X_DICTIONARY;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    ಆದರೆ (vpeekc() == '=')
		ನಿಲ್ಲಿಸು;
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ರಿಜಿಸ್ಟರ್;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_ಬಹುಲಕ = CTRL_X_THESAURUS;
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	ಸ್ಥಿತಿ Ctrl_U:
	    // user defined completion
	    ctrl_x_ಬಹುಲಕ = CTRL_X_FUNCTION;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_O:
	    // omni completion
	    ctrl_x_ಬಹುಲಕ = CTRL_X_OMNI;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ 's':
	ಸ್ಥಿತಿ Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_ಬಹುಲಕ = CTRL_X_SPELL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_ಹಿಂಭಾಗ_ಗಮ್ಯ_badword();
	    --emsg_off;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_RSB:
	    // complete tag names
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ಟ್ಯಾಗುಗಳು;
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	ಸ್ಥಿತಿ Ctrl_I:
	ಸ್ಥಿತಿ K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ಮಾರ್ಗ_PATTERNS;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ Ctrl_V:
	ಸ್ಥಿತಿ Ctrl_Q:
	    // complete vim commands
	    ctrl_x_ಬಹುಲಕ = CTRL_X_CMDLINE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_Z:
	    // stop completion
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಾಮಾನ್ಯ;
	    edit_submode = ಖಾಲಿ;
	    showmode();
	    retval = TRUE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ Ctrl_P:
	ಸ್ಥಿತಿ Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    ಆದರೆ (!(compl_cont_ಸ್ಥಿತಿ & CONT_INTRPT))
		compl_cont_ಸ್ಥಿತಿ |= CONT_LOCAL;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_cont_ಬಹುಲಕ != 0)
		compl_cont_ಸ್ಥಿತಿ &= ~CONT_LOCAL;
	    // FALLTHROUGH
	ಪೂರ್ವನಿಯೋಜಿತ:
	    // If we have typed at least 2 ^X's... for modes != 0, we set
	    // compl_cont_status = 0 (eg, as if we had just started ^X
	    // mode).
	    // For mode 0, we set "compl_cont_mode" to an impossible
	    // value, in both cases ^X^X can be used to restart the same
	    // mode (avoiding ADDING mode).
	    // Undocumented feature: In a mode != 0 ^X^P and ^X^X^P start
	    // 'complete' and local ^P expansions respectively.
	    // In mode 0 an extra ^X is needed since ^X^P goes to ADDING
	    // mode  -- Acevedo
	    ಆದರೆ (c == Ctrl_X)
	    {
		ಆದರೆ (compl_cont_ಬಹುಲಕ != 0)
		    compl_cont_ಸ್ಥಿತಿ = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    compl_cont_ಬಹುಲಕ = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಾಮಾನ್ಯ;
	    edit_submode = ಖಾಲಿ;
	    showmode();
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_complete_ಮುಗಿದ_event(ಪೂರ್ಣಾಂಕ ಬಹುಲಕ ಬಳಸದ, string_T *ಶಬ್ದ ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*ಬಹುಲಕ_str;

    ಆದರೆ (ಶಬ್ದ == ಖಾಲಿ || ಶಬ್ದ->string == ಖಾಲಿ)
	(ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "complete_word", (char_u *)"", 0);
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "complete_word", ಶಬ್ದ->string, (ಪೂರ್ಣಾಂಕ)ಶಬ್ದ->ಉದ್ದಳತೆ);

    ಬಹುಲಕ_str = &ctrl_x_ಬಹುಲಕ_ಹೆಸರುಗಳು[ಬಹುಲಕ & ~CTRL_X_WANT_IDENT];
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(v_event, "complete_type",
	(ಬಹುಲಕ_str->string == ಖಾಲಿ) ? (char_u *)"" : ಬಹುಲಕ_str->string, (ಪೂರ್ಣಾಂಕ)ಬಹುಲಕ_str->ಉದ್ದಳತೆ);

    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);
#ಆದರೆ_ಅಂತ್ಯ
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#ಆದರೆ defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Stop insert completion mode
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_stop(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ prev_ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ retval)
{
    ಪೂರ್ಣಾಂಕ		want_cindent;
    string_T	ಶಬ್ದ = {ಖಾಲಿ, 0};

    // Remove pre-inserted text when present.
    ಆದರೆ (ins_compl_preinsert_effect() && ins_compl_win_ಸಕ್ರಿಯ(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ != ಖಾಲಿ || compl_leader.string != ಖಾಲಿ || c == Ctrl_E)
    {
	char_u	*ptr = ಖಾಲಿ;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ != ಖಾಲಿ && compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ && c != Ctrl_E)
	    ptr = compl_curr_ಹೊಂದಿಕೆ->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    ಆದರೆ (compl_cont_ಬಹುಲಕ == CTRL_X_ಸಂಪೂರ್ಣ_ಸಾಲು)
    {
	// re-indent the current line
	ಆದರೆ (want_cindent)
	{
	    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!compl_autocomplete || compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ)
    {
	ಪೂರ್ಣಾಂಕ prev_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

	// put the cursor on the last char, for 'tw' formatting
	ಆದರೆ (prev_ಲಂಬಸಾಲು > 0)
	    dec_cursor();
	// only format when something was inserted
	ಆದರೆ (!arrow_ಬಳಸಿದ && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	ಆದರೆ (prev_ಲಂಬಸಾಲು > 0
		&& ml_get_curline()[curwin->w_cursor.ಲಂಬಸಾಲು] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    ಆದರೆ ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	ಶಬ್ದ.string = vim_strnsave(compl_shown_ಹೊಂದಿಕೆ->cp_str.string,
	    compl_shown_ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ);
	ಆದರೆ (ಶಬ್ದ.string == ಖಾಲಿ)
	    ಶಬ್ದ.ಉದ್ದಳತೆ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಶಬ್ದ.ಉದ್ದಳತೆ = compl_shown_ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    ಆದರೆ (c == Ctrl_E)
    {
	char_u *p = ಖಾಲಿ;
	size_t	plen = 0;

	ins_compl_delete();
	ಆದರೆ (compl_leader.string != ಖಾಲಿ)
	{
	    p = compl_leader.string;
	    plen = compl_leader.ಉದ್ದಳತೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ != ಖಾಲಿ)
	{
	    p = compl_orig_ಪಠ್ಯ.string;
	    plen = compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ;
	}
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	    compl_len = get_compl_len();

	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)plen > compl_len)
		ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(p + compl_len, (ಪೂರ್ಣಾಂಕ)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_ಬಹುಲಕ = prev_ಬಹುಲಕ;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    ಆದರೆ (!shortmess(SHM_COMPLETIONMENU))
	ಸಂದೇಶ_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಾಮಾನ್ಯ;
    compl_enter_selects = FALSE;
    ಆದರೆ (edit_submode != ಖಾಲಿ)
    {
	edit_submode = ಖಾಲಿ;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ಅಂಕೆ_bests = 0;
    compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು = 0;

    ಆದರೆ (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    ಆದರೆ (want_cindent && ಒಳ_cinkeys(ಕೀಲಿ_COMPLETE, ' ', inindent(0)))
	do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_ಮುಗಿದ_event(prev_ಬಹುಲಕ, &ಶಬ್ದ);
    vim_free(ಶಬ್ದ.string);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Cancel completion.
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_cancel(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ins_compl_stop(' ', ctrl_x_ಬಹುಲಕ, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    ಪೂರ್ಣಾಂಕ
ins_compl_prep(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    retval = FALSE;
    ಪೂರ್ಣಾಂಕ	    prev_ಬಹುಲಕ = ctrl_x_ಬಹುಲಕ;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    ಆದರೆ (c != Ctrl_R && vim_is_ctrl_x_ಕೀಲಿ(c))
	edit_submode_extra = ಖಾಲಿ;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    ಆದರೆ (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	ಹಿಂತಿರುಗಿ retval;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    ಆದರೆ (is_mouse_ಕೀಲಿ(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	ಆದರೆ (c == K_LEFTRELEASE
		|| c == K_LEFTRELEASE_NM
		|| c == K_MIDDLERELEASE
		|| c == K_RIGHTRELEASE
		|| c == K_X1RELEASE
		|| c == K_X2RELEASE
		|| c == K_LEFTDRAG
		|| c == K_MIDDLEDRAG
		|| c == K_RIGHTDRAG
		|| c == K_X1DRAG
		|| c == K_X2DRAG)
	    ಹಿಂತಿರುಗಿ retval;
	ಆದರೆ (popup_visible)
	{
	    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
	    win_T   *wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);

	    ಆದರೆ (wp != ಖಾಲಿ && WIN_IS_POPUP(wp))
		ಹಿಂತಿರುಗಿ retval;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ctrl_x_ಬಹುಲಕ == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	ಆದರೆ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_ಕೀಲಿ(c)
		|| !vim_is_ctrl_x_ಕೀಲಿ(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_ಬಹುಲಕ = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    ಆದರೆ (c == Ctrl_Z)
		retval = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ctrl_x_ಬಹುಲಕ = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_ಬಹುಲಕ = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_not_defined_yet()
			   || (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = TRUE;
    }

    ಆದರೆ (ctrl_x_ಬಹುಲಕ_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = ಗಣ_ctrl_x_ಬಹುಲಕ(c);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	ಆದರೆ (c != Ctrl_R && !vim_is_ctrl_x_ಕೀಲಿ(c))
	{
	    ctrl_x_ಬಹುಲಕ = ctrl_x_ಬಹುಲಕ_scroll() ? CTRL_X_ಸಾಮಾನ್ಯ : CTRL_X_ಪೂರ್ಣಗೊಂಡ;
	    edit_submode = ಖಾಲಿ;
	}
	showmode();
    }

    ಆದರೆ (compl_started || ctrl_x_ಬಹುಲಕ == CTRL_X_ಪೂರ್ಣಗೊಂಡ)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	ಆದರೆ ((ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_ಕೀಲಿ(c))
		|| ctrl_x_ಬಹುಲಕ == CTRL_X_ಪೂರ್ಣಗೊಂಡ)
	    retval = ins_compl_stop(c, prev_ಬಹುಲಕ, retval);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ == CTRL_X_LOCAL_ಸಂದೇಶ)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_ಮುಗಿದ_event(ctrl_x_ಬಹುಲಕ, ಖಾಲಿ);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    ಆದರೆ (c != Ctrl_R && !vim_is_ctrl_x_ಕೀಲಿ(c))
    {
	compl_cont_ಸ್ಥಿತಿ = 0;
	compl_cont_ಬಹುಲಕ = 0;
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    ಪೂರ್ಣಾಂಕ	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    ಆದರೆ (ptr == ಖಾಲಿ)
    {
	ಆದರೆ (compl_leader.string != ಖಾಲಿ)
	    ptr = compl_leader.string;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ;  // nothing to do
    }
    ಆದರೆ (compl_orig_ಪಠ್ಯ.string != ಖಾಲಿ)
    {
	p = compl_orig_ಪಠ್ಯ.string;
	// Find length of common prefix between original text and new completion
	ಆಗ (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	ಆದರೆ (len > 0)
	    len -= (*mb_ತಲೆ_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	ಗಾಗಿ (p += len; *p != NUL; MB_PTR_ADV(p))
	    Append_Char_ಗಮ್ಯ_Redobuff(K_BS);
    }
    ಆದರೆ (ptr != ಖಾಲಿ)
	Append_ಗಮ್ಯ_Redobuff_Lit(ptr + len, -1);
}

/*
 * Loops through the list of windows, loaded-buffers or non-loaded-buffers
 * (depending on flag) starting from buf and looking for a non-scanned
 * buffer (other than curbuf).	curbuf is special, if it is called with
 * buf=curbuf then it has to be the first call for a given flag/expansion.
 *
 * Returns the buffer to scan, if any, otherwise returns curbuf -- Acevedo
 */
    ಸ್ಥಿರ buf_T *
ins_compl_ಮುಂದಿನ_buf(buf_T *buf, ಪೂರ್ಣಾಂಕ flag)
{
    ಸ್ಥಿರ win_T    *wp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    skip_ಬಫರ್;

    ಆದರೆ (flag == 'w')		// just windows
    {
	ಆದರೆ (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	ಆಗ (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_ಮುಂದಿನ != ಖಾಲಿ) ? wp->w_ಮುಂದಿನ : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    ಆದರೆ (wp == curwin || !wp->w_ಬಫರ್->b_scanned)
		ನಿಲ್ಲಿಸು;
	}
	buf = wp->w_ಬಫರ್;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	ಆಗ (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_ಮುಂದಿನ != ಖಾಲಿ) ? buf->b_ಮುಂದಿನ : firstbuf;
	    // Break if we're back at start buffer
	    ಆದರೆ (buf == curbuf)
		ನಿಲ್ಲಿಸು;

	    // Check buffer conditions based on flag
	    ಆದರೆ (flag == 'U')
		skip_ಬಫರ್ = buf->b_p_bl;
	    ಇಲ್ಲದಿದ್ದರೆ
		skip_ಬಫರ್ = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ಖಾಲಿ) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    ಆದರೆ (!skip_ಬಫರ್ && !buf->b_scanned)
		ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_cpt_sources_ಎಣಿಕೆ(char_u *cpt)
{
    char_u  dummy[LSIZE];
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = 0;
    char_u  *p;

    ಗಾಗಿ (p = cpt; *p != NUL; )
    {
	ಆಗ (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	ಆದರೆ (*p != NUL)
	{
	    (ಶೂನ್ಯ)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    ಎಣಿಕೆ_2++;
	}
    }

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
ಸ್ಥಿರ callback_T cfu_cb;	    // 'completefunc' callback function
ಸ್ಥಿರ callback_T ofu_cb;	    // 'omnifunc' callback function
ಸ್ಥಿರ callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
ಸ್ಥಿರ callback_T *cpt_cb;	    // Callback functions associated with F{func}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	  cpt_cb_ಎಣಿಕೆ;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_global_ಗಮ್ಯ_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    ಆದರೆ (globcb->cb_name != ಖಾಲಿ && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ಅಕ್ಷರ *
did_ಗಣ_completefunc(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ	retval;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
	retval = option_ಗಣ_callback_func(ವಾದಗಳು->os_newval.string,
							    &curbuf->b_cfu_cb);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = option_ಗಣ_callback_func(ವಾದಗಳು->os_newval.string, &cfu_cb);
	ಆದರೆ (retval == OK && !(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	    ಗಣ_buflocal_cfu_callback(curbuf);
    }

    ಹಿಂತಿರುಗಿ retval == FAIL ? e_invalid_ವಾದ : ಖಾಲಿ;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    ಶೂನ್ಯ
ಗಣ_buflocal_cfu_callback(buf_T *buf ಬಳಸದ)
{
# ifdef FEAT_EVAL
    copy_global_ಗಮ್ಯ_buflocal_cb(&cfu_cb, &buf->b_cfu_cb);
# endif
}

/*
 * Parse the 'omnifunc' option value and set the callback function.
 * Invoked when the 'omnifunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ಅಕ್ಷರ *
did_ಗಣ_omnifunc(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ	retval;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
	retval = option_ಗಣ_callback_func(ವಾದಗಳು->os_newval.string,
							    &curbuf->b_ofu_cb);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = option_ಗಣ_callback_func(ವಾದಗಳು->os_newval.string, &ofu_cb);
	ಆದರೆ (retval == OK && !(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	    ಗಣ_buflocal_ofu_callback(curbuf);
    }

    ಹಿಂತಿರುಗಿ retval == FAIL ? e_invalid_ವಾದ : ಖಾಲಿ;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    ಶೂನ್ಯ
ಗಣ_buflocal_ofu_callback(buf_T *buf ಬಳಸದ)
{
# ifdef FEAT_EVAL
    copy_global_ಗಮ್ಯ_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    ಶೂನ್ಯ
clear_cpt_callbacks(callback_T **callbacks, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (callbacks == ಖಾಲಿ || *callbacks == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_cpt_callbacks(callback_T **dest, ಪೂರ್ಣಾಂಕ *dest_cnt, callback_T *src, ಪೂರ್ಣಾಂಕ cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    ಆದರೆ (cnt == 0)
	ಹಿಂತಿರುಗಿ OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    ಆದರೆ (*dest == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    *dest_cnt = cnt;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cnt; i++)
	ಆದರೆ (src[i].cb_name != ಖಾಲಿ && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    ಶೂನ್ಯ
ಗಣ_buflocal_cpt_callbacks(buf_T *buf ಬಳಸದ)
{
# ifdef FEAT_EVAL
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    (ಶೂನ್ಯ)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_ಎಣಿಕೆ, cpt_cb,
	    cpt_cb_ಎಣಿಕೆ);
# endif
}

/*
 * Parse the 'complete' option value "cpt" and store the callbacks for the
 * F{func} entries in a newly allocated array in "*cbp", setting "*cnt" to the
 * number of entries.  The array has one entry for every item in "cpt", also
 * for items that are not a function, so that it can be indexed with the item
 * index.  Any previous array in "*cbp" is cleared.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, ಪೂರ್ಣಾಂಕ *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    idx = 0;
    ಪೂರ್ಣಾಂಕ	    slen;
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    ಎಣಿಕೆ_2 = get_cpt_sources_ಎಣಿಕೆ(cpt);
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, ಎಣಿಕೆ_2);
    ಆದರೆ (*cbp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    *cnt = ಎಣಿಕೆ_2;

    ಗಾಗಿ (p = cpt; *p != NUL; )
    {
	ಆಗ (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	ಆದರೆ (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ಆದರೆ (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		ಆದರೆ (caret != ಖಾಲಿ)
		    *caret = NUL;

		ಆದರೆ (option_ಗಣ_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ಖಾಲಿ;
	    }
	    idx++;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_cpt_callbacks(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ	    opt_flags = ವಾದಗಳು->os_flags;

    ಆದರೆ (curbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // ":setglobal" does not change the buffer-local option value
    ಆದರೆ (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_ಎಣಿಕೆ) != OK)
	ಹಿಂತಿರುಗಿ FAIL;

    // ":setlocal" does not change the global option value
    ಆದರೆ (opt_flags & OPT_LOCAL)
	ಹಿಂತಿರುಗಿ OK;

    // Cache the callbacks for the global value
    ಆದರೆ (opt_flags & OPT_GLOBAL)
	ಹಿಂತಿರುಗಿ parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_ಎಣಿಕೆ);

    // set case
    ಹಿಂತಿರುಗಿ copy_cpt_callbacks(&cpt_cb, &cpt_cb_ಎಣಿಕೆ, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_ಎಣಿಕೆ);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ಅಕ್ಷರ *
did_ಗಣ_thesaurusfunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	retval;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_ಗಣ_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// global option set
	retval = option_ಗಣ_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	ಆದರೆ (!(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    ಹಿಂತಿರುಗಿ retval == FAIL ? e_invalid_ವಾದ : ಖಾಲಿ;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_cpt_callbacks(callback_T *callbacks, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ	abort = FALSE;

    ಆದರೆ (callbacks == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; i++)
	abort = abort || ಗಣ_ref_ಒಳ_callback(&callbacks[i], copyID);
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_insexpand_funcs(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ abort = ಗಣ_ref_ಒಳ_callback(&cfu_cb, copyID);
    abort = abort || ಗಣ_ref_ಒಳ_callback(&ofu_cb, copyID);
    abort = abort || ಗಣ_ref_ಒಳ_callback(&tsrfu_cb, copyID);
    abort = abort || ಗಣ_ref_ಒಳ_cpt_callbacks(cpt_cb, cpt_cb_ಎಣಿಕೆ, copyID);

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    ಸ್ಥಿರ char_u *
get_complete_funcname(ಪೂರ್ಣಾಂಕ type)
{
    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ CTRL_X_FUNCTION:
	    ಹಿಂತಿರುಗಿ curbuf->b_p_cfu;
	ಸ್ಥಿತಿ CTRL_X_OMNI:
	    ಹಿಂತಿರುಗಿ curbuf->b_p_ofu;
	ಸ್ಥಿತಿ CTRL_X_THESAURUS:
	    ಹಿಂತಿರುಗಿ *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    ಸ್ಥಿರ callback_T *
get_ಸೇರಿಸು_callback(ಪೂರ್ಣಾಂಕ type)
{
    ಆದರೆ (type == CTRL_X_FUNCTION)
	ಹಿಂತಿರುಗಿ &curbuf->b_cfu_cb;
    ಆದರೆ (type == CTRL_X_OMNI)
	ಹಿಂತಿರುಗಿ &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    ಹಿಂತಿರುಗಿ (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
expand_by_function(ಪೂರ್ಣಾಂಕ type, char_u *ತಳ, callback_T *cb)
{
    list_T      *matchlist = ಖಾಲಿ;
    dict_T	*matchdict = ಖಾಲಿ;
    typval_T	ವಾದಗಳು[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    ಪೂರ್ಣಾಂಕ		save_State = State;
    ಪೂರ್ಣಾಂಕ		retval;
    ಪೂರ್ಣಾಂಕ		is_cpt_function = (cb != ಖಾಲಿ);
    ಪೂರ್ಣಾಂಕ		use_sandbox = is_cpt_function
			    && was_ಗಣ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ಆದರೆ (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	ಆದರೆ (*funcname == NUL)
	    ಹಿಂತಿರುಗಿ;
	cb = get_ಸೇರಿಸು_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    ವಾದಗಳು[0].v_type = VAR_ಸಂಖ್ಯೆ;
    ವಾದಗಳು[0].vval.v_ಸಂಖ್ಯೆ = 0;
    ವಾದಗಳು[1].v_type = VAR_STRING;
    ವಾದಗಳು[1].vval.v_string = ತಳ != ಖಾಲಿ ? ತಳ : (char_u *)"";
    ವಾದಗಳು[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    ಆದರೆ (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, ವಾದಗಳು);
    ಆದರೆ (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    ಆದರೆ (retval == OK)
    {
	ಆಯ್ಕೆ (rettv.v_type)
	{
	    ಸ್ಥಿತಿ VAR_LIST:
		matchlist = rettv.vval.v_list;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_DICT:
		matchdict = rettv.vval.v_dict;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_SPECIAL:
		ಆದರೆ (rettv.vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
		    compl_opt_suppress_ಖಾಲಿಗಳು = TRUE;
		// FALLTHROUGH
	    ಪೂರ್ವನಿಯೋಜಿತ:
		// TODO: Give error message?
		clear_tv(&rettv);
		ನಿಲ್ಲಿಸು;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ಆದರೆ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_ಪಠ್ಯ));
	ಹೋಗು theend;
    }

    ಆದರೆ (matchlist != ಖಾಲಿ)
	ins_compl_ಕೂಡಿಸು_list(matchlist);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (matchdict != ಖಾಲಿ)
	ins_compl_ಕೂಡಿಸು_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    ಆದರೆ (matchdict != ಖಾಲಿ)
	dict_unref(matchdict);
    ಆದರೆ (matchlist != ಖಾಲಿ)
	list_unref(matchlist);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_COMPL_FUNC

#ಆದರೆ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
get_user_highlight_id(char_u *hlname)
{
    ಆದರೆ (hlname != ಖಾಲಿ && *hlname != NUL)
	ಹಿಂತಿರುಗಿ syn_check_ಗುಂಪು(hlname, (ಪೂರ್ಣಾಂಕ)STRLEN(hlname));
    ಹಿಂತಿರುಗಿ 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_ಕೂಡಿಸು_tv(typval_T *tv, ಪೂರ್ಣಾಂಕ dir, ಪೂರ್ಣಾಂಕ fast)
{
    char_u	*ಶಬ್ದ;
    ಪೂರ್ಣಾಂಕ		dup = FALSE;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2 = FALSE;
    ಪೂರ್ಣಾಂಕ		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_ಎಣಿಕೆ]);
    typval_T	user_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    char_u	*user_abbr_hlname;
    char_u	*user_ಬಗೆ_hlname;
    ಪೂರ್ಣಾಂಕ		user_hl[2] = { 0, 0 };

    user_ದತ್ತಾಂಶ.v_type = VAR_UNKNOWN;
    ಆದರೆ (tv->v_type == VAR_DICT && tv->vval.v_dict != ಖಾಲಿ)
    {
	ಶಬ್ದ = dict_get_string(tv->vval.v_dict, "word", FALSE);
	cptext[CPT_ABBR] = dict_get_string(tv->vval.v_dict, "abbr", FALSE);
	cptext[CPT_MENU] = dict_get_string(tv->vval.v_dict, "menu", FALSE);
	cptext[CPT_ಬಗೆ] = dict_get_string(tv->vval.v_dict, "kind", FALSE);
	cptext[CPT_INFO] = dict_get_string(tv->vval.v_dict, "info", FALSE);

	user_abbr_hlname = dict_get_string(tv->vval.v_dict, "abbr_hlgroup", FALSE);
	user_hl[0] = get_user_highlight_id(user_abbr_hlname);

	user_ಬಗೆ_hlname = dict_get_string(tv->vval.v_dict, "kind_hlgroup", FALSE);
	user_hl[1] = get_user_highlight_id(user_ಬಗೆ_hlname);

	dict_get_tv(tv->vval.v_dict, "user_data", &user_ದತ್ತಾಂಶ);
	ಆದರೆ (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ಖಾಲಿ
				  && dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	ಆದರೆ (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ಖಾಲಿ)
	    dup = dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "dup");
	ಆದರೆ (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ಖಾಲಿ)
	    ಖಾಲಿಗಳು_2 = dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "empty");
	ಆದರೆ (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ಖಾಲಿ
				  && dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಶಬ್ದ = tv_get_string_chk(tv);
	CLEAR_ಕ್ಷೇತ್ರ(cptext);
    }
    ಆದರೆ (ಶಬ್ದ == ಖಾಲಿ || (!ಖಾಲಿಗಳು_2 && *ಶಬ್ದ == NUL))
    {
	clear_tv(&user_ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಸ್ಥಿತಿ_2 = ins_compl_ಕೂಡಿಸು(ಶಬ್ದ, -1, ಖಾಲಿ, cptext,
	    &user_ದತ್ತಾಂಶ, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != OK)
	clear_tv(&user_ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Add completions from a list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಕೂಡಿಸು_list(list_T *list)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
    {
	ಆದರೆ (ins_compl_ಕೂಡಿಸು_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_emsg)
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Add completions from a dict.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಕೂಡಿಸು_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_ಪದಗಳು;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"refresh", 7);
    ಆದರೆ (di_refresh != ಖಾಲಿ && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	ಆದರೆ (v != ಖಾಲಿ && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_ಪದಗಳು = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"words", 5);
    ಆದರೆ (di_ಪದಗಳು != ಖಾಲಿ && di_ಪದಗಳು->di_tv.v_type == VAR_LIST)
	ins_compl_ಕೂಡಿಸು_list(di_ಪದಗಳು->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_completion(colnr_T startcol, list_T *list)
{
    ಪೂರ್ಣಾಂಕ save_w_wrow = curwin->w_wrow;
    ಪೂರ್ಣಾಂಕ save_w_leftcol = curwin->w_leftcol;
    ಪೂರ್ಣಾಂಕ flags = CP_ORIGINAL_ಪಠ್ಯ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_cot_flags = get_cot_flags();
    ಪೂರ್ಣಾಂಕ compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    ಪೂರ್ಣಾಂಕ compl_no_ಸೇರಿಸು = (cur_cot_flags & COT_NOINSERT) != 0;
    ಪೂರ್ಣಾಂಕ compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    ಆದರೆ (compl_started || ctrl_x_ಬಹುಲಕ_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    ಆದರೆ (startcol > curwin->w_cursor.ಲಂಬಸಾಲು)
	startcol = curwin->w_cursor.ಲಂಬಸಾಲು;
    compl_ಲಂಬಸಾಲು = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು - (ಪೂರ್ಣಾಂಕ)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_ಪಠ್ಯ.string = vim_strnsave(ml_get_curline() + compl_ಲಂಬಸಾಲು,
							(size_t)compl_ಉದ್ದಳತೆ);
    ಆದರೆ (p_ic)
	flags |= CP_ICASE;
    ಆದರೆ (compl_orig_ಪಠ್ಯ.string == ಖಾಲಿ)
    {
	compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ;
    }
    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ = (size_t)compl_ಉದ್ದಳತೆ;
    ಆದರೆ (ins_compl_ಕೂಡಿಸು(compl_orig_ಪಠ್ಯ.string,
			(ಪೂರ್ಣಾಂಕ)compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, 0,
			flags | CP_FAST, FALSE, ಖಾಲಿ, FUZZY_SCORE_NONE) != OK)
	ಹಿಂತಿರುಗಿ;

    ctrl_x_ಬಹುಲಕ = CTRL_X_EVAL;

    ins_compl_ಕೂಡಿಸು_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = TRUE;
    compl_cont_ಸ್ಥಿತಿ = 0;

    compl_curr_ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ no_select = compl_no_select || compl_longest;
    ಆದರೆ (compl_no_ಸೇರಿಸು || no_select)
    {
	ins_complete(K_ಕೆಳಕ್ಕೆ, FALSE);
	ಆದರೆ (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_ಮೇಲೆ, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_ಸೇರಿಸು;

    // Lazily show the popup menu, unless we got interrupted.
    ಆದರೆ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    ಹೊರಹೋಗು_flush();
}

/*
 * "complete()" function
 */
    ಶೂನ್ಯ
f_complete(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0)
    {
	emsg(_(e_complete_can_only_be_ಬಳಸಿದ_ಒಳ_ಸೇರಿಸು_ಬಹುಲಕ));
	ಹಿಂತಿರುಗಿ;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    ಆದರೆ (!undo_allowed())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	ಪೂರ್ಣಾಂಕ startcol = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ);
	ಆದರೆ (startcol > 0)
	    ಗಣ_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    ಶೂನ್ಯ
f_complete_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = ins_compl_ಕೂಡಿಸು_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    ಶೂನ್ಯ
f_complete_check(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_ಕೀಲಿಗಳು(0, TRUE);
    rettv->vval.v_ಸಂಖ್ಯೆ = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಬಹುಲಕ(string_T *ret)
{
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_not_defined_yet() || ctrl_x_ಬಹುಲಕ_scroll() || compl_started)
    {
	string_T    *ಬಹುಲಕ;

	ಬಹುಲಕ = &ctrl_x_ಬಹುಲಕ_ಹೆಸರುಗಳು[ctrl_x_ಬಹುಲಕ & ~CTRL_X_WANT_IDENT];
	ret->string = (ಬಹುಲಕ->string == ಖಾಲಿ) ? (char_u *)"" : ಬಹುಲಕ->string;
	ret->ಉದ್ದಳತೆ = ಬಹುಲಕ->ಉದ್ದಳತೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ret->string = (char_u *)"";
	ret->ಉದ್ದಳತೆ = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_update_sequence_numbers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಸಂಖ್ಯೆ = 0;
    compl_T	*ಹೊಂದಿಕೆ;

    ಆದರೆ (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	ಗಾಗಿ (ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ->cp_prev; ಹೊಂದಿಕೆ != ಖಾಲಿ
		&& !is_ಮೊದಲ_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ); ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_prev)
	    ಆದರೆ (ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ != -1)
	    {
		ಸಂಖ್ಯೆ = ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ;
		ನಿಲ್ಲಿಸು;
	    }
	ಆದರೆ (ಹೊಂದಿಕೆ != ಖಾಲಿ)
	    // go up and assign all numbers which are not assigned yet
	    ಗಾಗಿ (ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
		    ಹೊಂದಿಕೆ != ಖಾಲಿ && ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ == -1;
					   ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ)
		ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ = ++ಸಂಖ್ಯೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	ಗಾಗಿ (ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ; ಹೊಂದಿಕೆ != ಖಾಲಿ
		&& !is_ಮೊದಲ_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ); ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ)
	{
	    ಆದರೆ (ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ != -1)
	    {
		ಸಂಖ್ಯೆ = ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ಹೊಂದಿಕೆ != ಖಾಲಿ)
	{
	    // go down and assign all numbers which are not assigned yet
	    ಗಾಗಿ (ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_prev; ಹೊಂದಿಕೆ
		    && ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ == -1;
					   ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_prev)
		ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ = ++ಸಂಖ್ಯೆ;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_complete_info_dict(dict_T *di, compl_T *ಹೊಂದಿಕೆ, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_ಹೊಂದಿಕೆ)
{
    dict_ಕೂಡಿಸು_string_len(di, "word", ಹೊಂದಿಕೆ->cp_str.string, (ಪೂರ್ಣಾಂಕ)ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ);
    dict_ಕೂಡಿಸು_string(di, "abbr", ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[CPT_ABBR]);
    dict_ಕೂಡಿಸು_string(di, "menu", ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[CPT_MENU]);
    dict_ಕೂಡಿಸು_string(di, "kind", ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[CPT_ಬಗೆ]);
    dict_ಕೂಡಿಸು_string(di, "info", ಹೊಂದಿಕೆ->cp_ಪಠ್ಯ[CPT_INFO]);
    dict_ಕೂಡಿಸು_string(di, "abbr_hlgroup", syn_id2name(ಹೊಂದಿಕೆ->cp_user_abbr_hl_id));
    dict_ಕೂಡಿಸು_string(di, "kind_hlgroup", syn_id2name(ಹೊಂದಿಕೆ->cp_user_ಬಗೆ_hl_id));
    ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ)
	dict_ಕೂಡಿಸು_bool(di, "match", ಹೊಂದಿಕೆ->cp_ಒಳ_ಹೊಂದಿಕೆ_array);
    ಆದರೆ (ಹೊಂದಿಕೆ->cp_user_ದತ್ತಾಂಶ.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_ಕೂಡಿಸು_string_len(di, "user_data", (char_u *)"", 0);
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_tv(di, "user_data", &ಹೊಂದಿಕೆ->cp_user_ದತ್ತಾಂಶ);
}

/*
 * Get complete information
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_complete_info(list_T *what_list, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ		ret = OK;
    listitem_T	*ಅಂಶ;
# define CI_WHAT_ಬಹುಲಕ		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_ವಸ್ತುಗಳು		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_ಪಠ್ಯ   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    ಪೂರ್ಣಾಂಕ		what_flag;

    ಆದರೆ (what_list == ಖಾಲಿ)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(what_list, ಅಂಶ)
	{
	    char_u *what = tv_get_string(&ಅಂಶ->li_tv);

	    ಆದರೆ (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_ಬಹುಲಕ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ವಸ್ತುಗಳು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_ಪಠ್ಯ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    ಆದರೆ (ret == OK && (what_flag & CI_WHAT_ಬಹುಲಕ))
    {
	string_T    ಬಹುಲಕ;

	ins_compl_ಬಹುಲಕ(&ಬಹುಲಕ);
	ret = dict_ಕೂಡಿಸು_string_len(retdict, "mode", ಬಹುಲಕ.string, (ಪೂರ್ಣಾಂಕ)ಬಹುಲಕ.ಉದ್ದಳತೆ);
    }

    ಆದರೆ (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "pum_visible", pum_visible());

    ಆದರೆ (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "auto", compl_autostarted);

    ಆದರೆ (ret == OK && (what_flag & CI_WHAT_PREINSERTED_ಪಠ್ಯ))
    {
	char_u	*ಸಾಲು = ml_get_curline();
	ಪೂರ್ಣಾಂಕ	len = compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು - curwin->w_cursor.ಲಂಬಸಾಲು;

	ret = dict_ಕೂಡಿಸು_string_len(retdict, "preinserted_text",
		(len > 0) ? ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು : (char_u *)"", (len > 0) ? len : 0);
    }

    ಆದರೆ (ret == OK && (what_flag & (CI_WHAT_ವಸ್ತುಗಳು | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ಖಾಲಿ;
	dict_T	    *di;
	compl_T     *ಹೊಂದಿಕೆ;
	ಪೂರ್ಣಾಂಕ         selected_idx = -1;
	ಪೂರ್ಣಾಂಕ	    has_ವಸ್ತುಗಳು = what_flag & CI_WHAT_ವಸ್ತುಗಳು;
	ಪೂರ್ಣಾಂಕ	    has_matches = what_flag & CI_WHAT_MATCHES;
	ಪೂರ್ಣಾಂಕ	    has_completed = what_flag & CI_WHAT_COMPLETED;

	ಆದರೆ (has_ವಸ್ತುಗಳು || has_matches)
	{
	    li = list_alloc();
	    ಆದರೆ (li == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    ret = dict_ಕೂಡಿಸು_list(retdict, (has_matches && !has_ವಸ್ತುಗಳು)
						? "matches" : "items", li);
	    ಆದರೆ (ret == FAIL)
		list_unref(li);
	}
	ಆದರೆ (ret == OK && what_flag & CI_WHAT_SELECTED)
	    ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ != ಖಾಲಿ && compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ == -1)
		ins_compl_update_sequence_numbers();
	ಆದರೆ (ret == OK && compl_ಮೊದಲ_ಹೊಂದಿಕೆ != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ list_idx = 0;
	    ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
	    ಮಾಡು
	    {
		ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(ಹೊಂದಿಕೆ))
		{
		    ಆದರೆ (has_ವಸ್ತುಗಳು
			    || (has_matches && ಹೊಂದಿಕೆ->cp_ಒಳ_ಹೊಂದಿಕೆ_array))
		    {
			di = dict_alloc();
			ಆದರೆ (di == ಖಾಲಿ)
			    ಹಿಂತಿರುಗಿ;
			ret = list_append_dict(li, di);
			ಆದರೆ (ret != OK)
			{
			    dict_unref(di);
			    ಹಿಂತಿರುಗಿ;
			}
			fill_complete_info_dict(di, ಹೊಂದಿಕೆ, has_matches && has_ವಸ್ತುಗಳು);
		    }
		    ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ != ಖಾಲಿ
			    && compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ == ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ)
			selected_idx = list_idx;
		    ಆದರೆ (!has_matches || ಹೊಂದಿಕೆ->cp_ಒಳ_ಹೊಂದಿಕೆ_array)
			list_idx++;
		}
		ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	    }
	    ಆಗ (ಹೊಂದಿಕೆ != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ));
	}
	ಆದರೆ (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "selected", selected_idx);

	ಆದರೆ (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    ಆದರೆ (di == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    fill_complete_info_dict(di, compl_curr_ಹೊಂದಿಕೆ, FALSE);
	    ret = dict_ಕೂಡಿಸು_dict(retdict, "completed", di);
	    ಆದರೆ (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    ಶೂನ್ಯ
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ಖಾಲಿ;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
thesaurus_func_complete(ಪೂರ್ಣಾಂಕ type ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    ಹಿಂತಿರುಗಿ type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
may_advance_cpt_ಸೂಚ್ಯಂಕ(char_u *cpt)
{
    char_u  *p = cpt;

    ಆದರೆ (cpt_sources_ಸೂಚ್ಯಂಕ == -1)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆಗ (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    ಹಿಂತಿರುಗಿ (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
ಎಣಿಕೆ
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_ಕೊನೆ
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_ಹೊಂದಿಕೆ_pos;		// current match position
    pos_T	prev_ಹೊಂದಿಕೆ_pos;		// previous match position
    ಪೂರ್ಣಾಂಕ		ಗಣ_ಹೊಂದಿಕೆ_pos;		// save first_match_pos/last_match_pos
    pos_T	ಮೊದಲ_ಹೊಂದಿಕೆ_pos;	// first match position
    pos_T	ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos;		// last match position
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    ಪೂರ್ಣಾಂಕ		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_ಮುಂದಿನ_state_T;

/*
 * Process the next 'complete' option value in st->e_cpt.
 *
 * If successful, the arguments are set as below:
 *   st->cpt - pointer to the next option value in "st->cpt"
 *   compl_type_arg - type of insert mode completion to use
 *   st->found_all - all matches of this type are found
 *   st->ins_buf - search for completions in this buffer
 *   st->first_match_pos - position of the first completion match
 *   st->last_match_pos - position of the last completion match
 *   st->set_match_pos - TRUE if the first match position should be saved to
 *			    avoid loops after the search wraps around.
 *   st->dict - name of the dictionary or thesaurus file to search
 *   st->dict_f - flag specifying whether "dict" is an exact file name or not
 *
 * Returns INS_COMPL_CPT_OK if the next value is processed successfully.
 * Returns INS_COMPL_CPT_CONT to skip the current completion source matching
 * the "st->e_cpt" option value and process the next matching source.
 * Returns INS_COMPL_CPT_END if all the values in "st->e_cpt" are processed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
process_ಮುಂದಿನ_cpt_ಮೌಲ್ಯ(
	ins_compl_ಮುಂದಿನ_state_T *st,
	ಪೂರ್ಣಾಂಕ		*compl_type_arg,
	pos_T		*ಆರಂಭ_ಹೊಂದಿಕೆ_pos,
	ಪೂರ್ಣಾಂಕ		fuzzy_collect,
	ಪೂರ್ಣಾಂಕ		*advance_cpt_idx)
{
    ಪೂರ್ಣಾಂಕ	    compl_type = -1;
    ಪೂರ್ಣಾಂಕ	    ಸ್ಥಿತಿ_2 = INS_COMPL_CPT_OK;
    ಪೂರ್ಣಾಂಕ	    skip_ಮೂಲ = compl_autocomplete && compl_from_nonkeyword;

    st->ಸಿಕ್ಕಿತು_all = FALSE;
    *advance_cpt_idx = FALSE;

    ಆಗ (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    ಆದರೆ (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_ಮೂಲ
	    && !compl_ಸಮಯ_ತುಂಡು_expired)
    {
	st->ins_buf = curbuf;
	st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos = *ಆರಂಭ_ಹೊಂದಿಕೆ_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && (!fuzzy_collect && dec(&st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.lnum = st->ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು = ml_get_len(st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.lnum);
	}
	st->ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos = st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->ಗಣ_ಹೊಂದಿಕೆ_pos = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!skip_ಮೂಲ && !compl_ಸಮಯ_ತುಂಡು_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ಖಾಲಿ
	    && (st->ins_buf = ins_compl_ಮುಂದಿನ_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	ಆದರೆ (st->ins_buf->b_ml.ml_mfp != ಖಾಲಿ)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು = st->ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು = 0;
	    st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.lnum = st->ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	    st->ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos.lnum = 0;
	    compl_type = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ	// unloaded buffer, scan like dictionary
	{
	    st->ಸಿಕ್ಕಿತು_all = TRUE;
	    ಆದರೆ (st->ins_buf->b_fname == ಖಾಲಿ)
	    {
		ಸ್ಥಿತಿ_2 = INS_COMPL_CPT_CONT;
		ಹೋಗು ಮುಗಿದ;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	ಆದರೆ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ಖಾಲಿ
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ಖಾಲಿ
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (ಶೂನ್ಯ)ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*st->e_cpt == NUL)
	ಸ್ಥಿತಿ_2 = INS_COMPL_CPT_ಕೊನೆ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
	    compl_type = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_ಸೂಚ್ಯಂಕ);
	    ಆದರೆ (!st->func_cb)
		compl_type = -1;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!skip_ಮೂಲ)
	{
	    ಆದರೆ (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		ಆದರೆ (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		ಇಲ್ಲದಿದ್ದರೆ
		    compl_type = CTRL_X_THESAURUS;
		ಆದರೆ (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_ಮೊದಲ;
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*st->e_cpt == 'i')
		compl_type = CTRL_X_ಮಾರ್ಗ_PATTERNS;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*st->e_cpt == 'd')
		compl_type = CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ;
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_ಟ್ಯಾಗುಗಳು;
		ಆದರೆ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("Scanning tags."));
		    (ಶೂನ್ಯ)ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(ಶೂನ್ಯ)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_ಸೂಚ್ಯಂಕ(st->e_cpt);

	st->ಸಿಕ್ಕಿತು_all = TRUE;
	ಆದರೆ (compl_type == -1)
	    ಸ್ಥಿತಿ_2 = INS_COMPL_CPT_CONT;
    }

ಮುಗಿದ:
    *compl_type_arg = compl_type;
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಮುಂದಿನ_include_file_completion(ಪೂರ್ಣಾಂಕ compl_type)
{
    ಕಂಡುಹಿಡಿ_ನಮೂನೆ_ಒಳ_ಮಾರ್ಗ(compl_ನಮೂನೆ.string, compl_direction,
	    (ಪೂರ್ಣಾಂಕ)compl_ನಮೂನೆ.ಉದ್ದಳತೆ, FALSE, FALSE,
	    (compl_type == CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ
	     && !(compl_cont_ಸ್ಥಿತಿ & CONT_SOL))
	    ? ಕಂಡುಹಿಡಿ_DEFINE : ಕಂಡುಹಿಡಿ_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM, FALSE, compl_autocomplete);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the next set of words matching "compl_pattern" in dictionary or
 * thesaurus files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಮುಂದಿನ_dict_tsr_completion(ಪೂರ್ಣಾಂಕ compl_type, char_u *dict, ಪೂರ್ಣಾಂಕ dict_f)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    ಆದರೆ (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_ನಮೂನೆ.string, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ins_compl_dictionaries(
		dict != ಖಾಲಿ ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_ನಮೂನೆ.string,
		dict != ಖಾಲಿ ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಮುಂದಿನ_ಟ್ಯಾಗ್_completion(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		save_p_ic;
    char_u	**matches;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_ನಮೂನೆ.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_ಟ್ಯಾಗ್_at_cursor = TRUE;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(compl_ನಮೂನೆ.string, &ಅಂಕೆ_matches, &matches,
		ಟ್ಯಾಗ್_REGEXP | ಟ್ಯಾಗ್_ಹೆಸರುಗಳು | ಟ್ಯಾಗ್_NOIC | ಟ್ಯಾಗ್_INS_COMP
		| (ctrl_x_ಬಹುಲಕ_not_default() ? ಟ್ಯಾಗ್_VERBOSE : 0),
		ಟ್ಯಾಗ್_MANY, curbuf->b_ffname) == OK && ಅಂಕೆ_matches > 0)
	ins_compl_ಕೂಡಿಸು_matches(ಅಂಕೆ_matches, matches, p_ic);
    g_ಟ್ಯಾಗ್_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compare_scores(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    ಪೂರ್ಣಾಂಕ idx_a = *(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *)a;
    ಪೂರ್ಣಾಂಕ idx_b = *(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *)b;
    ಪೂರ್ಣಾಂಕ score_a = compl_fuzzy_scores[idx_a];
    ಪೂರ್ಣಾಂಕ score_b = compl_fuzzy_scores[idx_b];
    ಹಿಂತಿರುಗಿ score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_longest_ಸೇರಿಸು(char_u *ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    ins_compl_delete();
    ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(ಪೂರ್ವಪ್ರತ್ಯಯ + get_compl_len(), -1);
    compl_longest_ಕೊನೆ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_longest_ಹೊಂದಿಕೆ(ಶೂನ್ಯ)
{
    char_u	*ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಪೂರ್ವಪ್ರತ್ಯಯ_len = 0;
    ಪೂರ್ಣಾಂಕ		i = 0;
    ಪೂರ್ಣಾಂಕ		j = 0;
    char_u	*ಹೊಂದಿಕೆ_str = ಖಾಲಿ;
    char_u	*ಪೂರ್ವಪ್ರತ್ಯಯ_ptr = ಖಾಲಿ;
    char_u	*ಹೊಂದಿಕೆ_ptr = ಖಾಲಿ;
    char_u	*leader = ಖಾಲಿ;
    size_t	leader_len = 0;
    compl_T	*compl = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಇನ್ನಷ್ಟು_candidates = FALSE;
    compl_T	*nn_compl = ಖಾಲಿ;

    ಆದರೆ (compl_ಅಂಕೆ_bests == 0)
	ಹಿಂತಿರುಗಿ;

    nn_compl = compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ->cp_ಮುಂದಿನ;
    ಆದರೆ (nn_compl && nn_compl != compl_ಮೊದಲ_ಹೊಂದಿಕೆ)
	ಇನ್ನಷ್ಟು_candidates = TRUE;

    compl = ctrl_x_ಬಹುಲಕ_ಸಂಪೂರ್ಣ_ಸಾಲು() ? compl_ಮೊದಲ_ಹೊಂದಿಕೆ
				    : compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
    ಆದರೆ (compl_ಅಂಕೆ_bests == 1)
    {
	// no more candidates insert the match str
	ಆದರೆ (!ಇನ್ನಷ್ಟು_candidates)
	{
	    ins_compl_longest_ಸೇರಿಸು(compl->cp_str.string);
	    compl_ಅಂಕೆ_bests = 0;
	}
	compl_ಅಂಕೆ_bests = 0;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ ((size_t)compl_ಅಂಕೆ_bests > ಗಾತ್ರ_MAX / ಗಾತ್ರ(compl_T *))
	ಹಿಂತಿರುಗಿ;
    compl_ಉತ್ತಮ_matches = (compl_T **)alloc(compl_ಅಂಕೆ_bests * ಗಾತ್ರ(compl_T *));
    ಆದರೆ (compl_ಉತ್ತಮ_matches == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆಗ (compl != ಖಾಲಿ && i < compl_ಅಂಕೆ_bests)
    {
	compl_ಉತ್ತಮ_matches[i] = compl;
	compl = compl->cp_ಮುಂದಿನ;
	i++;
    }

    ಪೂರ್ವಪ್ರತ್ಯಯ = compl_ಉತ್ತಮ_matches[0]->cp_str.string;
    ಪೂರ್ವಪ್ರತ್ಯಯ_len = (ಪೂರ್ಣಾಂಕ)compl_ಉತ್ತಮ_matches[0]->cp_str.ಉದ್ದಳತೆ;

    ಗಾಗಿ (i = 1; i < compl_ಅಂಕೆ_bests; i++)
    {
	ಹೊಂದಿಕೆ_str = compl_ಉತ್ತಮ_matches[i]->cp_str.string;
	ಪೂರ್ವಪ್ರತ್ಯಯ_ptr = ಪೂರ್ವಪ್ರತ್ಯಯ;
	ಹೊಂದಿಕೆ_ptr = ಹೊಂದಿಕೆ_str;
	j = 0;

	ಆಗ (j < ಪೂರ್ವಪ್ರತ್ಯಯ_len && *ಹೊಂದಿಕೆ_ptr != NUL && *ಪೂರ್ವಪ್ರತ್ಯಯ_ptr != NUL)
	{
	    ಆದರೆ (STRNCMP(ಪೂರ್ವಪ್ರತ್ಯಯ_ptr, ಹೊಂದಿಕೆ_ptr, mb_ptr2len(ಪೂರ್ವಪ್ರತ್ಯಯ_ptr)) != 0)
		ನಿಲ್ಲಿಸು;

	    MB_PTR_ADV(ಪೂರ್ವಪ್ರತ್ಯಯ_ptr);
	    MB_PTR_ADV(ಹೊಂದಿಕೆ_ptr);
	    j++;
	}

	ಆದರೆ (j > 0)
	    ಪೂರ್ವಪ್ರತ್ಯಯ_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    ಆದರೆ (leader_len > 0 && STRNCMP(ಪೂರ್ವಪ್ರತ್ಯಯ, leader, leader_len) != 0)
	ಹೋಗು ಕೊನೆ;

    ಪೂರ್ವಪ್ರತ್ಯಯ = vim_strnsave(ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len);
    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
    {
	ins_compl_longest_ಸೇರಿಸು(ಪೂರ್ವಪ್ರತ್ಯಯ);
	vim_free(ಪೂರ್ವಪ್ರತ್ಯಯ);
    }

ಕೊನೆ:
    vim_free(compl_ಉತ್ತಮ_matches);
    compl_ಉತ್ತಮ_matches = ಖಾಲಿ;
    compl_ಅಂಕೆ_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಮುಂದಿನ_filename_completion(ಶೂನ್ಯ)
{
    char_u	**matches;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    ಪೂರ್ಣಾಂಕ		ಒಳ_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    ಪೂರ್ಣಾಂಕ		*fuzzy_indices_ದತ್ತಾಂಶ;
    char_u	*ಕೊನೆಯ_sep = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		need_collect_bests = ಒಳ_fuzzy_collect && compl_get_longest;
    ಪೂರ್ಣಾಂಕ		max_score = 0;
    ಪೂರ್ಣಾಂಕ		current_score = 0;
    ಪೂರ್ಣಾಂಕ		dir = compl_direction;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    ಅಕ್ಷರ pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#ಇಲ್ಲದಿದ್ದರೆ
    ಅಕ್ಷರ pathsep = PATHSEP;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಒಳ_fuzzy_collect)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	ಆದರೆ (curbuf->b_p_csl[0] == 's')
	{
	    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)leader_len; i++)
	    {
		ಆದರೆ (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_csl[0] == 'b')
	{
	    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)leader_len; i++)
	    {
		ಆದರೆ (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಕೊನೆಯ_sep = vim_strrchr(leader, pathsep);
	ಆದರೆ (ಕೊನೆಯ_sep == ಖಾಲಿ)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_ನಮೂನೆ);
	    compl_ನಮೂನೆ.string = vim_strnsave((char_u *)"*", 1);
	    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*(ಕೊನೆಯ_sep + 1) == '\0')
	    ಒಳ_fuzzy_collect = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Split leader into path and file parts
	    ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_len = ಕೊನೆಯ_sep - leader + 1;
	    char_u  *ಮಾರ್ಗ_with_wildcard;

	    ಮಾರ್ಗ_with_wildcard = alloc(ಮಾರ್ಗ_len + 2);
	    ಆದರೆ (ಮಾರ್ಗ_with_wildcard != ಖಾಲಿ)
	    {
		vim_snprintf((ಅಕ್ಷರ *)ಮಾರ್ಗ_with_wildcard, ಮಾರ್ಗ_len + 2, "%*.*s*", ಮಾರ್ಗ_len, ಮಾರ್ಗ_len, leader);
		VIM_CLEAR_STRING(compl_ನಮೂನೆ);
		compl_ನಮೂನೆ.string = ಮಾರ್ಗ_with_wildcard;
		compl_ನಮೂನೆ.ಉದ್ದಳತೆ = ಮಾರ್ಗ_len + 1;

		// Move leader to the file part
		leader = ಕೊನೆಯ_sep + 1;
		leader_len -= ಮಾರ್ಗ_len;
	    }
	}
    }

    ಆದರೆ (expand_wildcards(1, &compl_ನಮೂನೆ.string, &ಅಂಕೆ_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	ಹಿಂತಿರುಗಿ;

    // May change home directory back to "~".
    tilde_replace(compl_ನಮೂನೆ.string, ಅಂಕೆ_matches, matches);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    ಆದರೆ (curbuf->b_p_csl[0] != NUL)
    {
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
	{
	    ptr = matches[i];
	    ಆಗ (*ptr != NUL)
	    {
		ಆದರೆ (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಒಳ_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), 10);
	compl_fuzzy_scores = (ಪೂರ್ಣಾಂಕ *)alloc(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ಅಂಕೆ_matches);

	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_ಹೊಂದಿಕೆ_str(ptr, leader);
	    ಆದರೆ (score != FUZZY_SCORE_NONE)
	    {
		ಆದರೆ (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((ಪೂರ್ಣಾಂಕ *)fuzzy_indices.ga_ದತ್ತಾಂಶ)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	ಆದರೆ (fuzzy_indices.ga_len > 0)
	{
	    char_u	*ಹೊಂದಿಕೆ = ಖಾಲಿ;
	    fuzzy_indices_ದತ್ತಾಂಶ = (ಪೂರ್ಣಾಂಕ *)fuzzy_indices.ga_ದತ್ತಾಂಶ;
	    qsort(fuzzy_indices_ದತ್ತಾಂಶ, fuzzy_indices.ga_len, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), compare_scores);

	    ಗಾಗಿ (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		ಹೊಂದಿಕೆ = matches[fuzzy_indices_ದತ್ತಾಂಶ[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_ದತ್ತಾಂಶ[i]];
		ಆದರೆ (ins_compl_ಕೂಡಿಸು(ಹೊಂದಿಕೆ, -1, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ಖಾಲಿ, current_score) == OK)
		    dir = FORWARD;

		ಆದರೆ (need_collect_bests)
		{
		    ಆದರೆ (i == 0 || current_score == max_score)
		    {
			compl_ಅಂಕೆ_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(ಅಂಕೆ_matches, matches);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (leader_len > 0)
	{
	    FreeWild(ಅಂಕೆ_matches, matches);
	    ಅಂಕೆ_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	ಆದರೆ (compl_ಅಂಕೆ_bests > 0 && compl_get_longest)
	    fuzzy_longest_ಹೊಂದಿಕೆ();
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಅಂಕೆ_matches > 0)
	ins_compl_ಕೂಡಿಸು_matches(ಅಂಕೆ_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಮುಂದಿನ_cmdline_completion(ಶೂನ್ಯ)
{
    char_u	**matches;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches;

    ಆದರೆ (expand_cmdline(&compl_xp, compl_ನಮೂನೆ.string,
		(ಪೂರ್ಣಾಂಕ)compl_ನಮೂನೆ.ಉದ್ದಳತೆ, &ಅಂಕೆ_matches, &matches) == EXPAND_OK)
    {
	ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_r = OK;
	ಪೂರ್ಣಾಂಕ		dir = compl_direction;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_matches && ಕೂಡಿಸು_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_ಎಣಿಕೆ]) = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};

	    ಆದರೆ (compl_xp.xp_files_abbr != ಖಾಲಿ)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    ಆದರೆ (compl_xp.xp_files_ಬಗೆ != ಖಾಲಿ)
		cptext[CPT_ಬಗೆ] = compl_xp.xp_files_ಬಗೆ[i];
	    ಆದರೆ (compl_xp.xp_files_menu != ಖಾಲಿ)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    ಆದರೆ (compl_xp.xp_files_info != ಖಾಲಿ)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    ಕೂಡಿಸು_r = ins_compl_ಕೂಡಿಸು(matches[i], -1, ಖಾಲಿ, cptext, ಖಾಲಿ, dir,
		    CP_FAST, FALSE, ಖಾಲಿ, FUZZY_SCORE_NONE);
	    ಆದರೆ (ಕೂಡಿಸು_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(ಅಂಕೆ_matches, matches);
	free_xp_files_extra(&compl_xp, ಅಂಕೆ_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಮುಂದಿನ_spell_completion(linenr_T lnum ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    char_u	**matches;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches;

    ಅಂಕೆ_matches = expand_spelling(lnum, compl_ನಮೂನೆ.string, &matches);
    ಆದರೆ (ಅಂಕೆ_matches > 0)
	ins_compl_ಕೂಡಿಸು_matches(ಅಂಕೆ_matches, matches, p_ic);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(matches);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    ಸ್ಥಿರ char_u *
ins_compl_get_ಮುಂದಿನ_ಶಬ್ದ_or_ಸಾಲು(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_ಹೊಂದಿಕೆ_pos,		// current match position
	ಪೂರ್ಣಾಂಕ	*ಹೊಂದಿಕೆ_len,
	ಪೂರ್ಣಾಂಕ	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		len;

    *ಹೊಂದಿಕೆ_len = 0;
    ptr = ml_get_buf(ins_buf, cur_ಹೊಂದಿಕೆ_pos->lnum, FALSE) +
	cur_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು;
    len = (ಪೂರ್ಣಾಂಕ)ml_get_buf_len(ins_buf, cur_ಹೊಂದಿಕೆ_pos->lnum) - cur_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು;
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
    {
	ಆದರೆ (compl_ಸ್ಥಿತಿ_adding())
	{
	    ಆದರೆ (cur_ಹೊಂದಿಕೆ_pos->lnum >= ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ptr = ml_get_buf(ins_buf, cur_ಹೊಂದಿಕೆ_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_ಹೊಂದಿಕೆ_pos->lnum + 1);
	    ಆದರೆ (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (ಪೂರ್ಣಾಂಕ)(ptr - tmp_ptr);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*tmp_ptr = ptr;

	ಆದರೆ (compl_ಸ್ಥಿತಿ_adding() && compl_ಉದ್ದಳತೆ <= len)
	{
	    tmp_ptr += compl_ಉದ್ದಳತೆ;
	    // Skip if already inside a word.
	    ಆದರೆ (vim_iswordp(tmp_ptr))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    // Find start of next word.
	    tmp_ptr = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(tmp_ptr);
	len = (ಪೂರ್ಣಾಂಕ)(tmp_ptr - ptr);

	ಆದರೆ (compl_ಸ್ಥಿತಿ_adding() && len == compl_ಉದ್ದಳತೆ)
	{
	    ಆದರೆ (cur_ಹೊಂದಿಕೆ_pos->lnum < ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		// Try next line, if any. the new word will be
		// "join" as if the normal command "J" was used.
		// IOSIZE is always greater than
		// compl_length, so the next STRNCPY always
		// works -- Acevedo
		STRNCPY(IObuff, ptr, len);
		ptr = ml_get_buf(ins_buf, cur_ಹೊಂದಿಕೆ_pos->lnum + 1, FALSE);
		tmp_ptr = ptr = skipwhite(ptr);
		// Find start of next word.
		tmp_ptr = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(tmp_ptr);
		// Find end of next word.
		tmp_ptr = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(tmp_ptr);
		ಆದರೆ (tmp_ptr > ptr)
		{
		    ಆದರೆ (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			ಆದರೆ (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			ಆದರೆ (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ಖಾಲಿ
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    ಆದರೆ (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (ಪೂರ್ಣಾಂಕ)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    ಆದರೆ (len == compl_ಉದ್ದಳತೆ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    *ಹೊಂದಿಕೆ_len = len;
    ಹಿಂತಿರುಗಿ ptr;
}

/*
 * Get the next set of words matching "compl_pattern" for default completion(s)
 * (normal ^P/^N and ^X^L).
 * Search for "compl_pattern" in the buffer "st->ins_buf" starting from the
 * position "st->start_pos" in the "compl_direction" direction. If
 * "st->set_match_pos" is TRUE, then set the "st->first_match_pos" and
 * "st->last_match_pos".
 * Returns OK if a new next match is found, otherwise returns FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಮುಂದಿನ_default_completion(ins_compl_ಮುಂದಿನ_state_T *st, pos_T *ಆರಂಭ_pos)
{
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FAIL;
    ಪೂರ್ಣಾಂಕ		save_p_scs;
    ಪೂರ್ಣಾಂಕ		save_p_ws;
    ಪೂರ್ಣಾಂಕ		looped_around = FALSE;
    char_u	*ptr = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		ಒಳ_fuzzy_collect = !compl_ಸ್ಥಿತಿ_adding()
			&& cot_fuzzy() && compl_ಉದ್ದಳತೆ > 0;
    char_u	*leader = ins_compl_leader();
    ಪೂರ್ಣಾಂಕ		score = FUZZY_SCORE_NONE;
    ಪೂರ್ಣಾಂಕ		ಒಳ_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ಆದರೆ (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    ಆದರೆ (!ಒಳ_curbuf)
	p_ws = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	cont_s_ipos = FALSE;

	++ಸಂದೇಶ_silent;  // Don't want messages for wrapscan.

	ಆದರೆ (ಒಳ_fuzzy_collect)
	{
	    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = ಹುಡುಕು_for_fuzzy_ಹೊಂದಿಕೆ(st->ins_buf,
			    st->cur_ಹೊಂದಿಕೆ_pos, leader, compl_direction,
			    ಆರಂಭ_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಂಪೂರ್ಣ_ಸಾಲು() || ctrl_x_ಬಹುಲಕ_eval() || (compl_cont_ಸ್ಥಿತಿ & CONT_SOL))
	    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = ಹುಡುಕು_for_exact_ಸಾಲು(st->ins_buf,
			    st->cur_ಹೊಂದಿಕೆ_pos, compl_direction, compl_ನಮೂನೆ.string);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = searchit(ಖಾಲಿ, st->ins_buf, st->cur_ಹೊಂದಿಕೆ_pos,
				ಖಾಲಿ, compl_direction, compl_ನಮೂನೆ.string, (ಪೂರ್ಣಾಂಕ)compl_ನಮೂನೆ.ಉದ್ದಳತೆ,
				1L, ಹುಡುಕು_KEEP + ಹುಡುಕು_NFMSG, RE_ಕೊನೆಯ, ಖಾಲಿ);
	--ಸಂದೇಶ_silent;
	ಆದರೆ (!compl_started || st->ಗಣ_ಹೊಂದಿಕೆ_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos = *st->cur_ಹೊಂದಿಕೆ_pos;
	    st->ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos = *st->cur_ಹೊಂದಿಕೆ_pos;
	    st->ಗಣ_ಹೊಂದಿಕೆ_pos = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.lnum == st->ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos.lnum
		&& st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು == st->ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು)
	{
	    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_dir_forward()
		&& (st->prev_ಹೊಂದಿಕೆ_pos.lnum > st->cur_ಹೊಂದಿಕೆ_pos->lnum
		    || (st->prev_ಹೊಂದಿಕೆ_pos.lnum == st->cur_ಹೊಂದಿಕೆ_pos->lnum
			&& st->prev_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು >= st->cur_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು)))
	{
	    ಆದರೆ (looped_around)
		ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
		looped_around = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!compl_dir_forward()
		&& (st->prev_ಹೊಂದಿಕೆ_pos.lnum < st->cur_ಹೊಂದಿಕೆ_pos->lnum
		    || (st->prev_ಹೊಂದಿಕೆ_pos.lnum == st->cur_ಹೊಂದಿಕೆ_pos->lnum
			&& st->prev_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು <= st->cur_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು)))
	{
	    ಆದರೆ (looped_around)
		ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
		looped_around = TRUE;
	}
	st->prev_ಹೊಂದಿಕೆ_pos = *st->cur_ಹೊಂದಿಕೆ_pos;
	ಆದರೆ (ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ == FAIL)
	    ನಿಲ್ಲಿಸು;

	// when ADDING, the text before the cursor matches, skip it
	ಆದರೆ (compl_ಸ್ಥಿತಿ_adding() && ಒಳ_curbuf
		&& ಆರಂಭ_pos->lnum == st->cur_ಹೊಂದಿಕೆ_pos->lnum
		&& ಆರಂಭ_pos->ಲಂಬಸಾಲು  == st->cur_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು)
	    ಮುಂದುವರಿ;

	ಆದರೆ (!ಒಳ_fuzzy_collect)
	    ptr = ins_compl_get_ಮುಂದಿನ_ಶಬ್ದ_or_ಸಾಲು(st->ins_buf,
		    st->cur_ಹೊಂದಿಕೆ_pos, &len, &cont_s_ipos);
	ಆದರೆ (ptr == ಖಾಲಿ || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    ಮುಂದುವರಿ;

	ಆದರೆ (is_ಹತ್ತಿರದ_ಸಕ್ರಿಯ() && ಒಳ_curbuf)
	{
	    score = st->cur_ಹೊಂದಿಕೆ_pos->lnum - curwin->w_cursor.lnum;
	    ಆದರೆ (score < 0)
		score = -score;
	}

	ಆದರೆ (ins_compl_ಕೂಡಿಸು_infercase(ptr, len, p_ic,
			ಒಳ_curbuf ? ಖಾಲಿ : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    ಆದರೆ (ಒಳ_fuzzy_collect && score == compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ->cp_score)
		compl_ಅಂಕೆ_bests++;
	    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = OK;
	    ನಿಲ್ಲಿಸು;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    ಸ್ಥಿರ callback_T *
get_callback_if_cpt_func(char_u *p, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (*p == 'o')
	ಹಿಂತಿರುಗಿ &curbuf->b_ofu_cb;

    ಆದರೆ (*p == 'F')
    {
	ಆದರೆ (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    ಆದರೆ (curbuf->b_p_cpt_cb == ಖಾಲಿ
		    || idx < 0 || idx >= curbuf->b_p_cpt_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ curbuf->b_p_cpt_cb[idx].cb_name != ಖಾಲಿ
		? &curbuf->b_p_cpt_cb[idx] : ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ &curbuf->b_cfu_cb; // 'cfu'
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ರಿಜಿಸ್ಟರ್_completion(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		dir = compl_direction;
    yankreg_T	*reg = ಖಾಲಿ;
    ಶೂನ್ಯ	*reg_ptr = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		adding_ಬಹುಲಕ = compl_ಸ್ಥಿತಿ_adding();

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_REGISTERS; i++)
    {
	ಪೂರ್ಣಾಂಕ regname = 0;
	ಆದರೆ (i == 0)
	    regname = '"';    // unnamed register
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i < 10)
	    regname = '0' + i;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == DELETION_ರಿಜಿಸ್ಟರ್)
	    regname = '-';
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್)
	    regname = '*';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == PLUS_ರಿಜಿಸ್ಟರ್)
	    regname = '+';
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	ಆದರೆ (!valid_yank_reg(regname, FALSE) || regname == '_')
	    ಮುಂದುವರಿ;

	reg_ptr = get_ರಿಜಿಸ್ಟರ್(regname, FALSE);
	ಆದರೆ (reg_ptr == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	reg = (yankreg_T *)reg_ptr;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < reg->y_ಗಾತ್ರ; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    ಆದರೆ (str == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    ಆದರೆ (adding_ಬಹುಲಕ)
	    {
		ಪೂರ್ಣಾಂಕ str_len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
		ಆದರೆ (str_len == 0)
		    ಮುಂದುವರಿ;

		ಆದರೆ (!compl_orig_ಪಠ್ಯ.string
			|| (p_ic ? STRNICMP(str, compl_orig_ಪಠ್ಯ.string,
					    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ) == 0
				: STRNCMP(str, compl_orig_ಪಠ್ಯ.string,
					    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ) == 0))
		{
		    ಆದರೆ (ins_compl_ಕೂಡಿಸು_infercase(str, str_len, p_ic, ಖಾಲಿ,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_ಕೊನೆ = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		ಆಗ (p < str_ಕೊನೆ && *p != NUL)
		{
		    char_u *old_p = p;
		    p = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಆರಂಭ(p);
		    ಆದರೆ (p >= str_ಕೊನೆ || *p == NUL)
			ನಿಲ್ಲಿಸು;

		    char_u *ಶಬ್ದ_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(p);

		    ಆದರೆ (ಶಬ್ದ_ಕೊನೆ <= p)
		    {
			ಆದರೆ (has_mbyte)
			    ಶಬ್ದ_ಕೊನೆ = p + (*mb_ptr2len)(p);
			ಇಲ್ಲದಿದ್ದರೆ
			    ಶಬ್ದ_ಕೊನೆ = p + 1;
		    }

		    ಆದರೆ (ಶಬ್ದ_ಕೊನೆ > str_ಕೊನೆ)
			ಶಬ್ದ_ಕೊನೆ = str_ಕೊನೆ;

		    ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)(ಶಬ್ದ_ಕೊನೆ - p);
		    ಆದರೆ (len > 0 && (!compl_orig_ಪಠ್ಯ.string
				|| (p_ic ? STRNICMP(p, compl_orig_ಪಠ್ಯ.string,
						    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ) == 0
					: STRNCMP(p, compl_orig_ಪಠ್ಯ.string,
						    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ) == 0)))
		    {
			ಆದರೆ (ins_compl_ಕೂಡಿಸು_infercase(p, len, p_ic, ಖಾಲಿ,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = ಶಬ್ದ_ಕೊನೆ;

		    ಆದರೆ (p <= old_p)
		    {
			p = old_p + 1;
			ಆದರೆ (has_mbyte && p < str_ಕೊನೆ)
			    p = old_p + (*mb_ptr2len)(old_p);
		    }
		}
	    }
	}

	// Free the register copy
	put_ರಿಜಿಸ್ಟರ್(regname, reg_ptr);
    }
}

/*
 * get the next set of completion matches for "type".
 * Returns TRUE if a new match is found. Otherwise returns FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಮುಂದಿನ_completion_ಹೊಂದಿಕೆ(ಪೂರ್ಣಾಂಕ type, ins_compl_ಮುಂದಿನ_state_T *st, pos_T *ini)
{
    ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FALSE;

    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ -1:
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	ಸ್ಥಿತಿ CTRL_X_ಮಾರ್ಗ_PATTERNS:
	ಸ್ಥಿತಿ CTRL_X_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ:
	    get_ಮುಂದಿನ_include_file_completion(type);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ CTRL_X_DICTIONARY:
	ಸ್ಥಿತಿ CTRL_X_THESAURUS:
	    get_ಮುಂದಿನ_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CTRL_X_ಟ್ಯಾಗುಗಳು:
	    get_ಮುಂದಿನ_ಟ್ಯಾಗ್_completion();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CTRL_X_FILES:
	    get_ಮುಂದಿನ_filename_completion();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CTRL_X_CMDLINE:
	ಸ್ಥಿತಿ CTRL_X_CMDLINE_CTRL_X:
	    get_ಮುಂದಿನ_cmdline_completion();
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	ಸ್ಥಿತಿ CTRL_X_FUNCTION:
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    ಇಲ್ಲದಿದ್ದರೆ
		expand_by_function(type, compl_ನಮೂನೆ.string, ಖಾಲಿ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CTRL_X_OMNI:
	    expand_by_function(type, compl_ನಮೂನೆ.string, ಖಾಲಿ);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ CTRL_X_SPELL:
	    get_ಮುಂದಿನ_spell_completion(st->ಮೊದಲ_ಹೊಂದಿಕೆ_pos.lnum);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CTRL_X_ರಿಜಿಸ್ಟರ್:
	    get_ರಿಜಿಸ್ಟರ್_completion();
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:	// normal ^P/^N and ^X^L
	    ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = get_ಮುಂದಿನ_default_completion(st, ini);
	    ಆದರೆ (ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ == FAIL && st->ins_buf == curbuf)
		st->ಸಿಕ್ಕಿತು_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    ಆದರೆ (type != 0 && compl_curr_ಹೊಂದಿಕೆ != compl_old_ಹೊಂದಿಕೆ)
	ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = OK;

    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
strip_caret_numbers_ಒಳ_ಇರಿಸು(char_u *str)
{
    char_u  *ಪಠ = str, *ಲಿಖ = str, *p;

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (*ಪಠ)
    {
	ಆದರೆ (*ಪಠ == '^')
	{
	    p = ಪಠ + 1;
	    ಆಗ (vim_isdigit(*p))
		p++;
	    ಆದರೆ ((*p == ',' || *p == '\0') && p != ಪಠ + 1)
	    {
		ಪಠ = p;
		ಮುಂದುವರಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*ಲಿಖ++ = *ಪಠ++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *ಲಿಖ++ = *ಪಠ++;
    }
    *ಲಿಖ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prepare_cpt_compl_funcs(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    ಪೂರ್ಣಾಂಕ		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    ಆದರೆ (cpt == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    strip_caret_numbers_ಒಳ_ಇರಿಸು(cpt);

    ಗಾಗಿ (p = cpt; *p;)
    {
	ಆಗ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ಆದರೆ (*p == NUL)
	    ನಿಲ್ಲಿಸು;

	cb = get_callback_if_cpt_func(p, idx);
	ಆದರೆ (cb)
	{
	    ಆದರೆ (get_userdefined_compl_info(curwin->w_cursor.ಲಂಬಸಾಲು, cb, &startcol)
		    == FAIL)
	    {
		ಆದರೆ (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		ಇಲ್ಲದಿದ್ದರೆ
		    startcol = -2;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (startcol < 0 || startcol > curwin->w_cursor.ಲಂಬಸಾಲು)
		startcol = curwin->w_cursor.ಲಂಬಸಾಲು;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    cpt_sources_array[idx].cs_startcol = -3;

	(ಶೂನ್ಯ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    ಹಿಂತಿರುಗಿ OK;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compl_ಮೂಲ_ಆರಂಭ_timer(ಪೂರ್ಣಾಂಕ ಮೂಲ_idx ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    ಆದರೆ (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[ಮೂಲ_idx].compl_ಆರಂಭ_tv);
	compl_ಸಮಯ_ತುಂಡು_expired = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
advance_cpt_sources_ಸೂಚ್ಯಂಕ_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ)
{
    ಆದರೆ (cpt_sources_ಸೂಚ್ಯಂಕ >= 0 && cpt_sources_ಸೂಚ್ಯಂಕ < cpt_sources_ಎಣಿಕೆ - 1)
    {
	cpt_sources_ಸೂಚ್ಯಂಕ++;
	ಹಿಂತಿರುಗಿ OK;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), cpt_sources_ಸೂಚ್ಯಂಕ);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Get the next expansion(s), using "compl_pattern".
 * The search starts at position "ini" in curbuf and in the direction
 * compl_direction.
 * When "compl_started" is FALSE start at that position, otherwise continue
 * where we stopped searching before.
 * This may return before finding all the matches.
 * Return the total number of matches or -1 if still unknown -- Acevedo
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_get_exp(pos_T *ini)
{
    ಸ್ಥಿರ ins_compl_ಮುಂದಿನ_state_T   st;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ			    st_cleared = FALSE;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		type = ctrl_x_ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		may_advance_cpt_idx = FALSE;
    pos_T	ಆರಂಭ_pos = *ini;
    ಪೂರ್ಣಾಂಕ		ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict = FALSE;
    ದೀರ್ಘ	compl_timeout_save = 0;

    ಆದರೆ (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	ಆದರೆ (!st_cleared)
	{
	    CLEAR_ಕ್ಷೇತ್ರ(st);
	    st_cleared = TRUE;
	}
	st.ಸಿಕ್ಕಿತು_all = FALSE;
	st.ins_buf = curbuf;
	vim_free(st.e_cpt_copy);
	// Make a copy of 'complete', in case the buffer is wiped out.
	st.e_cpt_copy = vim_strsave((compl_cont_ಸ್ಥಿತಿ & CONT_LOCAL)
					    ? (char_u *)"." : curbuf->b_p_cpt);
	strip_caret_numbers_ಒಳ_ಇರಿಸು(st.e_cpt_copy);
	st.e_cpt = st.e_cpt_copy == ಖಾಲಿ ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKBACK_ಸಾಲು_ಎಣಿಕೆ	1000
	ಆದರೆ (compl_autocomplete && is_ಹತ್ತಿರದ_ಸಕ್ರಿಯ())
	{
	    ಆರಂಭ_pos.lnum = MAX(1, ಆರಂಭ_pos.lnum - LOOKBACK_ಸಾಲು_ಎಣಿಕೆ);
	    ಆರಂಭ_pos.ಲಂಬಸಾಲು = 0;
	}
	st.ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos = st.ಮೊದಲ_ಹೊಂದಿಕೆ_pos = ಆರಂಭ_pos;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ;	// remember the last current match
    st.cur_ಹೊಂದಿಕೆ_pos = (compl_dir_forward())
				    ? &st.ಕೊನೆಯ_ಹೊಂದಿಕೆ_pos : &st.ಮೊದಲ_ಹೊಂದಿಕೆ_pos;

    ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict = ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && !ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval()
	&& !(compl_cont_ಸ್ಥಿತಿ & CONT_LOCAL) && cpt_sources_array != ಖಾಲಿ;
    ಆದರೆ (ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict)
    {
	cpt_sources_ಸೂಚ್ಯಂಕ = 0;
	ಆದರೆ (compl_autocomplete || p_cto > 0)
	{
	    compl_ಮೂಲ_ಆರಂಭ_timer(0);
	    compl_ಸಮಯ_ತುಂಡು_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    ಗಾಗಿ (;;)
    {
	ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FAIL;
	st.ಗಣ_ಹೊಂದಿಕೆ_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	ಆದರೆ ((ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() || ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
					&& (!compl_started || st.ಸಿಕ್ಕಿತು_all))
	{
	    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = process_ಮುಂದಿನ_cpt_ಮೌಲ್ಯ(&st, &type, &ಆರಂಭ_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    ಆದರೆ (ಸ್ಥಿತಿ_2 == INS_COMPL_CPT_ಕೊನೆ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == INS_COMPL_CPT_CONT)
	    {
		ಆದರೆ (may_advance_cpt_idx)
		{
		    ಆದರೆ (!advance_cpt_sources_ಸೂಚ್ಯಂಕ_ಸುರಕ್ಷಿತ())
			ನಿಲ್ಲಿಸು;
		    compl_ಮೂಲ_ಆರಂಭ_timer(cpt_sources_ಸೂಚ್ಯಂಕ);
		}
		ಮುಂದುವರಿ;
	    }
	}

	ಆದರೆ (ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	{
	    // LSP servers may sporadically take >1s to respond (e.g., while
	    // loading modules), but other sources might already have matches.
	    // To show results quickly use a short timeout for keyword
	    // completion. Allow longer timeout for non-keyword completion
	    // where only function based sources (e.g. LSP) are active.
	    compl_timeout_save = compl_timeout_ms;
	    compl_timeout_ms = compl_from_nonkeyword
		? COMPL_FUNC_TIMEOUT_NON_KW_MS : COMPL_FUNC_TIMEOUT_MS;
	}

	// get the next set of completion matches
	ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = get_ಮುಂದಿನ_completion_ಹೊಂದಿಕೆ(type, &st, &ಆರಂಭ_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (may_advance_cpt_idx)
	{
	    ಆದರೆ (!advance_cpt_sources_ಸೂಚ್ಯಂಕ_ಸುರಕ್ಷಿತ())
		ನಿಲ್ಲಿಸು;
	    compl_ಮೂಲ_ಆರಂಭ_timer(cpt_sources_ಸೂಚ್ಯಂಕ);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	//ಹೊಂದಿಕೆ
	ಆದರೆ ((ctrl_x_ಬಹುಲಕ_not_default() && !ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
						|| ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ != FAIL)
	{
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    // Fill the popup menu as soon as possible.
	    ಆದರೆ (type != -1)
		ins_compl_check_ಕೀಲಿಗಳು(0, FALSE);

	    ಆದರೆ ((ctrl_x_ಬಹುಲಕ_not_default()
			&& !ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval()) || compl_interrupted)
		ನಿಲ್ಲಿಸು;
	    compl_started = compl_ಸಮಯ_ತುಂಡು_expired ? FALSE : TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Mark a buffer scanned when it has been scanned completely
	    ಆದರೆ (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_ಮಾರ್ಗ_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	ಆದರೆ (ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	ಆದರೆ (!compl_dir_forward())
	    ಆಗ (compl_curr_ಹೊಂದಿಕೆ->cp_prev
		    && !ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_curr_ಹೊಂದಿಕೆ->cp_prev))
		compl_curr_ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ->cp_prev;
    }
    cpt_sources_ಸೂಚ್ಯಂಕ = -1;
    compl_started = TRUE;

    ಆದರೆ ((ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() || ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = FAIL;

    ಹೊಂದಿಕೆ_ಎಣಿಕೆ = -1;		// total of matches, unknown
    ಆದರೆ (ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ == FAIL || (ctrl_x_ಬಹುಲಕ_not_default()
		&& !ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval()))
	ಹೊಂದಿಕೆ_ಎಣಿಕೆ = ins_compl_make_cyclic();

    ಆದರೆ (cot_fuzzy() && compl_get_longest && compl_ಅಂಕೆ_bests > 0)
	fuzzy_longest_ಹೊಂದಿಕೆ();

    ಆದರೆ (compl_old_ಹೊಂದಿಕೆ != ಖಾಲಿ)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_ಹೊಂದಿಕೆ = compl_dir_forward() ? compl_old_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ
						    : compl_old_ಹೊಂದಿಕೆ->cp_prev;
	ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	    compl_curr_ಹೊಂದಿಕೆ = compl_old_ಹೊಂದಿಕೆ;
    }
    may_trigger_modechanged();

    ಆದರೆ (ಹೊಂದಿಕೆ_ಎಣಿಕೆ > 0 && !ctrl_x_ಬಹುಲಕ_spell())
    {
	ಆದರೆ (is_ಹತ್ತಿರದ_ಸಕ್ರಿಯ() && !ins_compl_has_preinsert())
	    ವಿಂಗಡಿಸು_compl_ಹೊಂದಿಕೆ_list(cp_compare_ಹತ್ತಿರದ);

	ಆದರೆ (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_ವಿಂಗಡಿಸು();
    }

    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_ಎಣಿಕೆ;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_update_shown_ಹೊಂದಿಕೆ(ಶೂನ್ಯ)
{
    string_T	*leader;

    (ಶೂನ್ಯ)get_leader_for_startcol(ಖಾಲಿ, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_ಹೊಂದಿಕೆ, TRUE);

    ಆಗ (!ins_compl_equal(compl_shown_ಹೊಂದಿಕೆ,
		leader->string, (ಪೂರ್ಣಾಂಕ)leader->ಉದ್ದಳತೆ)
	    && compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ != ಖಾಲಿ
	    && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ))
    {
	compl_shown_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	leader = get_leader_for_startcol(compl_shown_ಹೊಂದಿಕೆ, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    ಆದರೆ (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_ಹೊಂದಿಕೆ,
		leader->string, (ಪೂರ್ಣಾಂಕ)leader->ಉದ್ದಳತೆ)
	    && (compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ == ಖಾಲಿ
		|| is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ)))
    {
	ಆಗ (!ins_compl_equal(compl_shown_ಹೊಂದಿಕೆ,
		    leader->string, (ಪೂರ್ಣಾಂಕ)leader->ಉದ್ದಳತೆ)
		&& compl_shown_ಹೊಂದಿಕೆ->cp_prev != ಖಾಲಿ
		&& !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ->cp_prev))
	{
	    compl_shown_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ->cp_prev;
	    leader = get_leader_for_startcol(compl_shown_ಹೊಂದಿಕೆ, TRUE);
	}
    }
}

/*
 * Delete the old text being completed.
 */
    ಶೂನ್ಯ
ins_compl_delete(ಶೂನ್ಯ)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = compl_ಲಂಬಸಾಲು + (compl_ಸ್ಥಿತಿ_adding() ? compl_ಉದ್ದಳತೆ : 0);
    string_T	ಉಳಿದ = {ಖಾಲಿ, 0};
    ಪೂರ್ಣಾಂಕ	    orig_ಲಂಬಸಾಲು;

    ಆದರೆ (ins_compl_preinsert_effect())
    {
	ಲಂಬಸಾಲು += (ಪೂರ್ಣಾಂಕ)ins_compl_leader_len();
	curwin->w_cursor.ಲಂಬಸಾಲು = compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು;
    }

    ಆದರೆ (curwin->w_cursor.lnum > compl_lnum)
    {
	ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < ml_get_curline_len())
	{
	    char_u *ಸಾಲು = ml_get_cursor();
	    ಉಳಿದ.ಉದ್ದಳತೆ = ml_get_cursor_len();
	    ಉಳಿದ.string = vim_strnsave(ಸಾಲು, ಉಳಿದ.ಉದ್ದಳತೆ);
	    ಆದರೆ (ಉಳಿದ.string == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}
	ಆಗ (curwin->w_cursor.lnum > compl_lnum)
	{
	    ಆದರೆ (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		ಆದರೆ (ಉಳಿದ.string)
		    vim_free(ಉಳಿದ.string);
		ಹಿಂತಿರುಗಿ;
	    }
	    deleted_ಸಾಲುಗಳು_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.ಲಂಬಸಾಲು = ml_get_curline_len();
    }

    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು > ಲಂಬಸಾಲು)
    {
	ಆದರೆ (stop_arrow() == FAIL)
	{
	    ಆದರೆ (ಉಳಿದ.string)
		vim_free(ಉಳಿದ.string);
	    ಹಿಂತಿರುಗಿ;
	}
	backspace_until_ಸ್ತಂಭ(ಲಂಬಸಾಲು);
	compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    }

    ಆದರೆ (ಉಳಿದ.string != ಖಾಲಿ)
    {
	orig_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
	ins_str(ಉಳಿದ.string, ಉಳಿದ.ಉದ್ದಳತೆ);
	curwin->w_cursor.ಲಂಬಸಾಲು = orig_ಲಂಬಸಾಲು;
	vim_free(ಉಳಿದ.string);
    }
    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    ಬದಲಾಯಿತು_cline_bef_curs();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // clear v:completed_item
    ಗಣ_vim_var_dict(VV_COMPLETED_ಅಂಶ, dict_alloc_ಬೀಗ(VAR_FIXED));
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Insert a completion string that contains newlines.
 * The string is split and inserted line by line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_expand_ಅಪವರ್ತ್ಯ(char_u *str)
{
    char_u	*ಆರಂಭ = str;
    char_u	*curr = str;
    ಪೂರ್ಣಾಂಕ		ತಳ_indent = get_indent();

    ಆಗ (*curr != NUL)
    {
	ಆದರೆ (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    ಆದರೆ (curr > ಆರಂಭ)
		ins_char_ಬೈಟುಗಳು(ಆರಂಭ, (ಪೂರ್ಣಾಂಕ)(curr - ಆರಂಭ));

	    // Handle newline
	    open_ಸಾಲು(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, ತಳ_indent, ಖಾಲಿ);
	    ಆರಂಭ = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    ಆದರೆ (curr > ಆರಂಭ)
	ins_char_ಬೈಟುಗಳು(ಆರಂಭ, (ಪೂರ್ಣಾಂಕ)(curr - ಆರಂಭ));

    compl_ins_ಕೊನೆ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_common_ಪೂರ್ವಪ್ರತ್ಯಯ(size_t *ಪೂರ್ವಪ್ರತ್ಯಯ_len, ಪೂರ್ಣಾಂಕ curbuf_only)
{
    compl_T     *compl;
    ಪೂರ್ಣಾಂಕ		*ಹೊಂದಿಕೆ_ಎಣಿಕೆ;
    char_u	*ಮೊದಲ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len = -1;
    ಪೂರ್ಣಾಂಕ		is_cpt_completion = (cpt_sources_array != ಖಾಲಿ);

    ಆದರೆ (!is_cpt_completion)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹೊಂದಿಕೆ_ಎಣಿಕೆ = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಂಕ, cpt_sources_ಎಣಿಕೆ);
    ಆದರೆ (ಹೊಂದಿಕೆ_ಎಣಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    (ಶೂನ್ಯ)get_leader_for_startcol(ಖಾಲಿ, TRUE); // Clear the cache

    compl = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಮಾಡು
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && !p_inf && compl_orig_ಪಠ್ಯ.string
		&& !ignorecase(compl_orig_ಪಠ್ಯ.string))
	    compl->cp_flags &= ~CP_ICASE;

	ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl)
		&& (leader->string == ಖಾಲಿ
		    || ins_compl_equal_sc(compl, leader->string,
			(ಪೂರ್ಣಾಂಕ)leader->ಉದ್ದಳತೆ)))
	{
	    // Limit number of items from each source if max_items is set.
	    ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ_ಎಲ್ಲೆ_exceeded = FALSE;
	    ಪೂರ್ಣಾಂಕ cur_ಮೂಲ = compl->cp_cpt_ಮೂಲ_idx;

	    ಆದರೆ (cur_ಮೂಲ != -1)
	    {
		ಹೊಂದಿಕೆ_ಎಣಿಕೆ[cur_ಮೂಲ]++;
		ಪೂರ್ಣಾಂಕ max_matches = cpt_sources_array[cur_ಮೂಲ].cs_max_matches;
		ಆದರೆ (max_matches > 0 && ಹೊಂದಿಕೆ_ಎಣಿಕೆ[cur_ಮೂಲ] > max_matches)
		    ಹೊಂದಿಕೆ_ಎಲ್ಲೆ_exceeded = TRUE;
	    }

	    ಆದರೆ (!ಹೊಂದಿಕೆ_ಎಲ್ಲೆ_exceeded && (!curbuf_only
			|| (cur_ಮೂಲ != -1
			    && cpt_sources_array[cur_ಮೂಲ].cs_flag == '.')))
	    {
		ಆದರೆ (ಮೊದಲ == ಖಾಲಿ && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    ಮೊದಲ = compl->cp_str.string;
		    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಮೊದಲ);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ != ಖಾಲಿ)
		{
		    ಪೂರ್ಣಾಂಕ j = 0;  // count in bytes
		    char_u *s1 = ಮೊದಲ;
		    char_u *s2 = compl->cp_str.string;

		    ಆಗ (j < len && *s1 != NUL && *s2 != NUL)
		    {
			ಆದರೆ (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    ನಿಲ್ಲಿಸು;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    ಆದರೆ (len == 0)
			ನಿಲ್ಲಿಸು;
		}
	    }
	}
	compl = compl->cp_ಮುಂದಿನ;
    } ಆಗ (compl != ಖಾಲಿ && !is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl));

    vim_free(ಹೊಂದಿಕೆ_ಎಣಿಕೆ);

    ಆದರೆ (len > (ಪೂರ್ಣಾಂಕ)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	ಆದರೆ (len == (ಪೂರ್ಣಾಂಕ)STRLEN(ಮೊದಲ))
	{
	    char_u *ಸಾಲು = ml_get_curline();
	    char_u *p = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಆದರೆ (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(p);
		ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_len = ಕೊನೆ - p;
		ಆದರೆ (ಪಠ್ಯ_len > 0
			&& ಪಠ್ಯ_len < (len - (ಪೂರ್ಣಾಂಕ)ins_compl_leader_len())
			&& STRNCMP(ಮೊದಲ + len - ಪಠ್ಯ_len, p, ಪಠ್ಯ_len) == 0)
		    len -= ಪಠ್ಯ_len;
	    }
	}
	*ಪೂರ್ವಪ್ರತ್ಯಯ_len = (size_t)len;
	ಹಿಂತಿರುಗಿ ಮೊದಲ;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    ಶೂನ್ಯ
ins_compl_ಸೇರಿಸು(ಪೂರ್ಣಾಂಕ move_cursor, ಪೂರ್ಣಾಂಕ ಸೇರಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    ಪೂರ್ಣಾಂಕ		compl_len = get_compl_len();
    ಪೂರ್ಣಾಂಕ		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_ಹೊಂದಿಕೆ->cp_str.string;
    size_t	cp_str_len = compl_shown_ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_ಅಪವರ್ತ್ಯ = vim_strchr(cp_str, '\n');

    ಆದರೆ (ಸೇರಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ)
    {
	cp_str = ಕಂಡುಹಿಡಿ_common_ಪೂರ್ವಪ್ರತ್ಯಯ(&cp_str_len, FALSE);
	ಆದರೆ (cp_str == ಖಾಲಿ)
	{
	    cp_str = ಕಂಡುಹಿಡಿ_common_ಪೂರ್ವಪ್ರತ್ಯಯ(&cp_str_len, TRUE);
	    ಆದರೆ (cp_str == ಖಾಲಿ)
	    {
		cp_str = compl_shown_ಹೊಂದಿಕೆ->cp_str.string;
		cp_str_len = compl_shown_ಹೊಂದಿಕೆ->cp_str.ಉದ್ದಳತೆ;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cpt_sources_array != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	cpt_idx = compl_shown_ಹೊಂದಿಕೆ->cp_cpt_ಮೂಲ_idx;
	ಆದರೆ (cpt_idx >= 0 && compl_ಲಂಬಸಾಲು >= 0)
	{
	    ಪೂರ್ಣಾಂಕ	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    ಆದರೆ (startcol >= 0 && startcol < (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು)
	    {
		ಪೂರ್ಣಾಂಕ skip = (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು - startcol;
		ಆದರೆ ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    ಆದರೆ (compl_len < (ಪೂರ್ಣಾಂಕ)cp_str_len)
    {
	ಆದರೆ (has_ಅಪವರ್ತ್ಯ)
	    ins_compl_expand_ಅಪವರ್ತ್ಯ(cp_str + compl_len);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(cp_str + compl_len,
		    ಸೇರಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ ? (ಪೂರ್ಣಾಂಕ)cp_str_len - compl_len : -1);
	    ಆದರೆ ((preinsert || ಸೇರಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ) && move_cursor)
		curwin->w_cursor.ಲಂಬಸಾಲು -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    ಆದರೆ (ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ)
	    || (preinsert && !ಸೇರಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ))
	compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_ಹೊಂದಿಕೆ);

	ಗಣ_vim_var_dict(VV_COMPLETED_ಅಂಶ, dict);
    }
#ಆದರೆ_ಅಂತ್ಯ
    compl_hi_on_autocompl_longest = ಸೇರಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_show_filename(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	*lead = _("match in file");
    ಪೂರ್ಣಾಂಕ		space = sc_ಲಂಬಸಾಲು - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    ಆದರೆ (space <= 0)
	ಹಿಂತಿರುಗಿ;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    ಗಾಗಿ (s = e = compl_shown_ಹೊಂದಿಕೆ->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	ಆಗ (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    ಆದರೆ (!compl_autocomplete)
    {
	ಸಂದೇಶ_hist_off = TRUE;
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_ಹೊಂದಿಕೆ->cp_fname ? "<" : "", s);
	ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
	ಸಂದೇಶ_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    ಸ್ಥಿರ compl_T *
ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಒಳ_menu(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    is_forward = compl_shows_dir_forward();
    compl_T *ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ;

    ಮಾಡು
	ಹೊಂದಿಕೆ = is_forward ? ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ : ಹೊಂದಿಕೆ->cp_prev;
    ಆಗ (ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ && !ಹೊಂದಿಕೆ->cp_ಒಳ_ಹೊಂದಿಕೆ_array
	    && !ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(ಹೊಂದಿಕೆ));
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;
}

/*
 * Find the next set of matches for completion. Repeat the completion "todo"
 * times.  The number of matches found is returned in 'num_matches'.
 *
 * If "allow_get_expansion" is TRUE, then ins_compl_get_exp() may be called to
 * get more completions. If it is FALSE, then do nothing when there are no more
 * completions in the given direction.
 *
 * If "advance" is TRUE, then completion will move to the first match.
 * Otherwise, the original text will be shown.
 *
 * Returns OK on success and -1 if the number of matches are unknown.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_completion_ಹೊಂದಿಕೆ(
	ಪೂರ್ಣಾಂಕ	allow_get_expansion,
	ಪೂರ್ಣಾಂಕ	todo,		// repeat completion this many times
	ಪೂರ್ಣಾಂಕ	advance,
	ಪೂರ್ಣಾಂಕ	*ಅಂಕೆ_matches)
{
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_ಕೊನೆ = FALSE;
    compl_T	*ಸಿಕ್ಕಿತು_compl = ಖಾಲಿ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_cot_flags = get_cot_flags();
    ಪೂರ್ಣಾಂಕ		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    ಆಗ (--todo >= 0)
    {
	ಆದರೆ (compl_shows_dir_forward() && compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array != ಖಾಲಿ)
		compl_shown_ಹೊಂದಿಕೆ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಒಳ_menu();
	    ಇಲ್ಲದಿದ್ದರೆ
		compl_shown_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
	    ಸಿಕ್ಕಿತು_ಕೊನೆ = (compl_ಮೊದಲ_ಹೊಂದಿಕೆ != ಖಾಲಿ
		    && (is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ)
			|| is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ)));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_shows_dir_backward()
		&& compl_shown_ಹೊಂದಿಕೆ->cp_prev != ಖಾಲಿ)
	{
	    ಸಿಕ್ಕಿತು_ಕೊನೆ = is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ);
	    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array != ಖಾಲಿ)
		compl_shown_ಹೊಂದಿಕೆ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಒಳ_menu();
	    ಇಲ್ಲದಿದ್ದರೆ
		compl_shown_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ->cp_prev;
	    ಸಿಕ್ಕಿತು_ಕೊನೆ |= is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_shown_ಹೊಂದಿಕೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!allow_get_expansion)
	    {
		ಆದರೆ (advance)
		{
		    ಆದರೆ (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			compl_pending += todo + 1;
		}
		ಹಿಂತಿರುಗಿ -1;
	    }

	    ಆದರೆ (!compl_no_select && advance)
	    {
		ಆದರೆ (compl_shows_dir_backward())
		    --compl_pending;
		ಇಲ್ಲದಿದ್ದರೆ
		    ++compl_pending;
	    }

	    // Find matches.
	    *ಅಂಕೆ_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    ಆಗ (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		ಆದರೆ (compl_pending > 0 && compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ != ಖಾಲಿ)
		{
		    compl_shown_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ;
		    --compl_pending;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_pending < 0 && compl_shown_ಹೊಂದಿಕೆ->cp_prev != ಖಾಲಿ)
		{
		    compl_shown_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ->cp_prev;
		    ++compl_pending;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ನಿಲ್ಲಿಸು;
	    }
	    ಸಿಕ್ಕಿತು_ಕೊನೆ = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_ಹೊಂದಿಕೆ, FALSE);

	ಆದರೆ (!ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ)
		&& leader->string != ಖಾಲಿ
		&& !ins_compl_equal(compl_shown_ಹೊಂದಿಕೆ,
		    leader->string, (ಪೂರ್ಣಾಂಕ)leader->ಉದ್ದಳತೆ)
		&& !(cot_fuzzy()
		    && compl_shown_ಹೊಂದಿಕೆ->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Remember a matching item.
	    ಸಿಕ್ಕಿತು_compl = compl_shown_ಹೊಂದಿಕೆ;

	// Stop at the end of the list when we found a usable match.
	ಆದರೆ (ಸಿಕ್ಕಿತು_ಕೊನೆ)
	{
	    ಆದರೆ (ಸಿಕ್ಕಿತು_compl != ಖಾಲಿ)
	    {
		compl_shown_ಹೊಂದಿಕೆ = ಸಿಕ್ಕಿತು_compl;
		ನಿಲ್ಲಿಸು;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Fill in the next completion in the current direction.
 * If "allow_get_expansion" is TRUE, then we may call ins_compl_get_exp() to
 * get more completions.  If it is FALSE, then we just do nothing when there
 * are no more completions in a given direction.  The latter case is used when
 * we are still in the middle of finding completions, to allow browsing
 * through the ones found so far.
 * Return the total number of matches, or -1 if still unknown -- webb.
 *
 * compl_curr_match is currently being used by ins_compl_get_exp(), so we use
 * compl_shown_match here.
 *
 * Note that this function may be called recursively once only.  First with
 * "allow_get_expansion" TRUE, which calls ins_compl_get_exp(), which in turn
 * calls this function with "allow_get_expansion" FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_ಮುಂದಿನ(
    ಪೂರ್ಣಾಂಕ	    allow_get_expansion,
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2,		// repeat completion this many times; should
				// be at least 1
    ಪೂರ್ಣಾಂಕ	    ಸೇರಿಸು_ಹೊಂದಿಕೆ)	// Insert the newly selected match
{
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_matches = -1;
    ಪೂರ್ಣಾಂಕ	    todo = ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ	    advance;
    ಪೂರ್ಣಾಂಕ	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_cot_flags = get_cot_flags();
    ಪೂರ್ಣಾಂಕ	    compl_no_ಸೇರಿಸು = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    ಪೂರ್ಣಾಂಕ	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    ಆದರೆ (compl_shown_ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (compl_leader.string != ಖಾಲಿ
	    && !ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_ಹೊಂದಿಕೆ();

    ಆದರೆ (allow_get_expansion && ಸೇರಿಸು_ಹೊಂದಿಕೆ
	    && (!compl_get_longest || compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = ಎಣಿಕೆ_2 != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    ಆದರೆ (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_completion_ಹೊಂದಿಕೆ(allow_get_expansion, todo, advance,
							&ಅಂಕೆ_matches) == -1)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	ಹಿಂತಿರುಗಿ -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    ಆದರೆ (!started && ins_compl_preinsert_longest())
	ins_compl_ಸೇರಿಸು(TRUE, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_no_ಸೇರಿಸು && !started && !compl_preinsert)
    {
	ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(compl_orig_ಪಠ್ಯ.string + get_compl_len(), -1);
	compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೇರಿಸು_ಹೊಂದಿಕೆ)
    {
	ಆದರೆ (!compl_get_longest || compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ)
	{
	    ಪೂರ್ಣಾಂಕ preinsert_longest = ins_compl_preinsert_longest()
		&& ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ); // none selected
	    ins_compl_ಸೇರಿಸು(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(compl_leader.string + get_compl_len(), -1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ = FALSE;

    ಆದರೆ (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	ಆದರೆ (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	ಇಲ್ಲದಿದ್ದರೆ
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Delete old text to be replaced, since we're still searching and
	// don't want to match ourselves!
	ins_compl_delete();
    }

    // Enter will select a match when the match wasn't inserted and the popup
    // menu is visible.
    ಆದರೆ (compl_no_ಸೇರಿಸು && !started
				 && !ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_shown_ಹೊಂದಿಕೆ))
	compl_enter_selects = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	compl_enter_selects = !ಸೇರಿಸು_ಹೊಂದಿಕೆ && compl_ಹೊಂದಿಕೆ_array != ಖಾಲಿ;

    // Show the file name for the match (if any)
    ಆದರೆ (compl_shown_ಹೊಂದಿಕೆ->cp_fname != ಖಾಲಿ)
	ins_compl_show_filename();

    ಹಿಂತಿರುಗಿ ಅಂಕೆ_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_elapsed_ಸಮಯ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T	*ಆರಂಭ_tv
	= &cpt_sources_array[cpt_sources_ಸೂಚ್ಯಂಕ].compl_ಆರಂಭ_tv;
    ದೀರ್ಘ	elapsed_ms = ELAPSED_FUNC(*ಆರಂಭ_tv);

    ಆದರೆ (elapsed_ms > compl_timeout_ms)
    {
	compl_ಸಮಯ_ತುಂಡು_expired = TRUE;
	DECAY_COMPL_TIMEOUT();
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Call this while finding completions, to check whether the user has hit a key
 * that should change the currently displayed completion, or exit completion
 * mode.  Also, when compl_pending is not zero, show a completion as soon as
 * possible. -- webb
 * "frequency" specifies out of how many calls we actually check.
 * "in_compl_func" is TRUE when called from complete_check(), don't set
 * compl_curr_match.
 */
    ಶೂನ್ಯ
ins_compl_check_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಂಕ frequency, ಪೂರ್ಣಾಂಕ ಒಳ_compl_func)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    ಆದರೆ (!ಒಳ_compl_func && (using_script() || ex_ಸಾಮಾನ್ಯ_busy))
	ಹಿಂತಿರುಗಿ;

    // Only do this at regular intervals
    ಆದರೆ (++ಎಣಿಕೆ_2 < frequency)
	ಹಿಂತಿರುಗಿ;
    ಎಣಿಕೆ_2 = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    ಆದರೆ (c != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ಆದರೆ (vim_is_ctrl_x_ಕೀಲಿ(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = ಸುರಕ್ಷಿತ_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (ಶೂನ್ಯ)ins_compl_ಮುಂದಿನ(FALSE, ins_compl_key2count(c),
				      c != K_ಮೇಲೆ && c != K_ಕೆಳಕ್ಕೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = ಸುರಕ್ಷಿತ_vgetc();
	    ಆದರೆ (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		ಆದರೆ (c != Ctrl_R && (ಕೀಲಿ_Typed
			    || (ಒಳ_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict = ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ()
	    && !ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval() && !(compl_cont_ಸ್ಥಿತಿ & CONT_LOCAL)
	    && cpt_sources_array != ಖಾಲಿ && cpt_sources_ಸೂಚ್ಯಂಕ >= 0;
	ಆದರೆ (ಸಾಮಾನ್ಯ_ಬಹುಲಕ_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_ಸಮಯ();
    }

    ಆದರೆ (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	ಪೂರ್ಣಾಂಕ todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(ಶೂನ್ಯ)ins_compl_ಮುಂದಿನ(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_key2dir(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_ಮೇಲೆ || c == K_ಮೇಲೆ)
	ಹಿಂತಿರುಗಿ BACKWARD;
    ಹಿಂತಿರುಗಿ FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_pum_ಕೀಲಿ(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_ಮೇಲೆ
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_ಕೆಳಕ್ಕೆ
		     || c == K_ಮೇಲೆ || c == K_ಕೆಳಕ್ಕೆ);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_key2count(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (ins_compl_pum_ಕೀಲಿ(c) && c != K_ಮೇಲೆ && c != K_ಕೆಳಕ್ಕೆ)
    {
	ಪೂರ್ಣಾಂಕ h = pum_get_ಎತ್ತರ();
	ಆದರೆ (h > 3)
	    h -= 2; // keep some context
	ಹಿಂತಿರುಗಿ h;
    }
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_use_ಹೊಂದಿಕೆ(ಪೂರ್ಣಾಂಕ c)
{
    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ K_ಮೇಲೆ:
	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ K_PAGEDOWN:
	ಸ್ಥಿತಿ K_KPAGEDOWN:
	ಸ್ಥಿತಿ K_S_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ K_PAGEUP:
	ಸ್ಥಿತಿ K_KPAGEUP:
	ಸ್ಥಿತಿ K_S_ಮೇಲೆ:
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಸಾಮಾನ್ಯ_compl_info(char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ startcol, colnr_T curs_ಲಂಬಸಾಲು)
{
    ಆದರೆ ((compl_cont_ಸ್ಥಿತಿ & CONT_SOL) || ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ())
    {
	ಆದರೆ (!compl_ಸ್ಥಿತಿ_adding())
	{
	    ಆಗ (--startcol >= 0 && vim_isIDc(ಸಾಲು[startcol]))
		;
	    compl_ಲಂಬಸಾಲು += ++startcol;
	    compl_ಉದ್ದಳತೆ = curs_ಲಂಬಸಾಲು - startcol;
	}
	ಆದರೆ (p_ic)
	{
	    compl_ನಮೂನೆ.string = str_foldcase(ಸಾಲು + compl_ಲಂಬಸಾಲು,
		    compl_ಉದ್ದಳತೆ, ಖಾಲಿ, 0);
	    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	    {
		compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = STRLEN(compl_ನಮೂನೆ.string);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    compl_ನಮೂನೆ.string = vim_strnsave(ಸಾಲು + compl_ಲಂಬಸಾಲು, (size_t)compl_ಉದ್ದಳತೆ);
	    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	    {
		compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = (size_t)compl_ಉದ್ದಳತೆ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_ಸ್ಥಿತಿ_adding())
    {
	char_u	    *ಪೂರ್ವಪ್ರತ್ಯಯ = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	ಆದರೆ (!vim_iswordp(ಸಾಲು + compl_ಲಂಬಸಾಲು)
		|| (compl_ಲಂಬಸಾಲು > 0
		    && (vim_iswordp(mb_prevptr(ಸಾಲು, ಸಾಲು + compl_ಲಂಬಸಾಲು)))))
	{
	    ಪೂರ್ವಪ್ರತ್ಯಯ = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ಖಾಲಿ, ಸಾಲು + compl_ಲಂಬಸಾಲು, compl_ಉದ್ದಳತೆ) + prefixlen;
	compl_ನಮೂನೆ.string = alloc(n);
	ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	{
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	STRCPY((ಅಕ್ಷರ *)compl_ನಮೂನೆ.string, ಪೂರ್ವಪ್ರತ್ಯಯ);
	(ಶೂನ್ಯ)quote_meta(compl_ನಮೂನೆ.string + prefixlen,
		ಸಾಲು + compl_ಲಂಬಸಾಲು, compl_ಉದ್ದಳತೆ);
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = n - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (--startcol < 0
	    || !vim_iswordp(mb_prevptr(ಸಾಲು, ಸಾಲು + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_ನಮೂನೆ.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	{
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = len;
	compl_ಲಂಬಸಾಲು += curs_ಲಂಬಸಾಲು;
	compl_ಉದ್ದಳತೆ = 0;
	compl_from_nonkeyword = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ ತಳ_class;
	    ಪೂರ್ಣಾಂಕ ತಲೆ_off;

	    startcol -= (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + startcol);
	    ತಳ_class = mb_get_class(ಸಾಲು + startcol);
	    ಆಗ (--startcol >= 0)
	    {
		ತಲೆ_off = (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + startcol);
		ಆದರೆ (ತಳ_class != mb_get_class(ಸಾಲು + startcol
			    - ತಲೆ_off))
		    ನಿಲ್ಲಿಸು;
		startcol -= ತಲೆ_off;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಗ (--startcol >= 0 && vim_iswordc(ಸಾಲು[startcol]))
		;
	}

	compl_ಲಂಬಸಾಲು += ++startcol;
	compl_ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)curs_ಲಂಬಸಾಲು - startcol;
	ಆದರೆ (compl_ಉದ್ದಳತೆ == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_ನಮೂನೆ.string = alloc(7);
	    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	    {
		compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    STRCPY((ಅಕ್ಷರ *)compl_ನಮೂನೆ.string, "\\<");
	    (ಶೂನ್ಯ)quote_meta(compl_ನಮೂನೆ.string + 2, ಸಾಲು + compl_ಲಂಬಸಾಲು, 1);
	    STRCAT((ಅಕ್ಷರ *)compl_ನಮೂನೆ.string, "\\k");
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = STRLEN(compl_ನಮೂನೆ.string);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    size_t  n = quote_meta(ಖಾಲಿ, ಸಾಲು + compl_ಲಂಬಸಾಲು, compl_ಉದ್ದಳತೆ) + 2;

	    compl_ನಮೂನೆ.string = alloc(n);
	    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	    {
		compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    STRCPY((ಅಕ್ಷರ *)compl_ನಮೂನೆ.string, "\\<");
	    (ಶೂನ್ಯ)quote_meta(compl_ನಮೂನೆ.string + 2, ಸಾಲು + compl_ಲಂಬಸಾಲು,
		    compl_ಉದ್ದಳತೆ);
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() && !(compl_cont_ಸ್ಥಿತಿ & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	ಆದರೆ (ಸಿದ್ಧತೆ_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_wholeline_compl_info(char_u *ಸಾಲು, colnr_T curs_ಲಂಬಸಾಲು)
{
    compl_ಲಂಬಸಾಲು = (colnr_T)getwhitecols(ಸಾಲು);
    compl_ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)curs_ಲಂಬಸಾಲು - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು;
    ಆದರೆ (compl_ಉದ್ದಳತೆ < 0)	// cursor in indent: empty pattern
	compl_ಉದ್ದಳತೆ = 0;
    ಆದರೆ (p_ic)
    {
	compl_ನಮೂನೆ.string = str_foldcase(ಸಾಲು + compl_ಲಂಬಸಾಲು, compl_ಉದ್ದಳತೆ,
		ಖಾಲಿ, 0);
	ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	{
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = STRLEN(compl_ನಮೂನೆ.string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	compl_ನಮೂನೆ.string = vim_strnsave(ಸಾಲು + compl_ಲಂಬಸಾಲು, (size_t)compl_ಉದ್ದಳತೆ);
	ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	{
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = (size_t)compl_ಉದ್ದಳತೆ;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_filename_compl_info(char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ startcol, colnr_T curs_ಲಂಬಸಾಲು)
{
    // Go back to just before the first filename character.
    ಆದರೆ (startcol > 0)
    {
	char_u	*p = ಸಾಲು + startcol;

	MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
	ಆಗ (p > ಸಾಲು && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
	ಆದರೆ (p == ಸಾಲು && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    startcol = (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು) + 1;
    }

    compl_ಲಂಬಸಾಲು += startcol;
    compl_ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)curs_ಲಂಬಸಾಲು - startcol;
    compl_ನಮೂನೆ.string = addstar(ಸಾಲು + compl_ಲಂಬಸಾಲು, compl_ಉದ್ದಳತೆ, EXPAND_FILES);
    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
    {
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = STRLEN(compl_ನಮೂನೆ.string);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_cmdline_compl_info(char_u *ಸಾಲು, colnr_T curs_ಲಂಬಸಾಲು)
{
    compl_ನಮೂನೆ.string = vim_strnsave(ಸಾಲು, curs_ಲಂಬಸಾಲು);
    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
    {
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = curs_ಲಂಬಸಾಲು;
    ಗಣ_cmd_context(&compl_xp, compl_ನಮೂನೆ.string,
	    (ಪೂರ್ಣಾಂಕ)compl_ನಮೂನೆ.ಉದ್ದಳತೆ, curs_ಲಂಬಸಾಲು, FALSE);
    ಆದರೆ (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_ಲಂಬಸಾಲು = curs_ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	compl_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(compl_xp.xp_ನಮೂನೆ - compl_ನಮೂನೆ.string);
    compl_ಉದ್ದಳತೆ = curs_ಲಂಬಸಾಲು - compl_ಲಂಬಸಾಲು;

    ಹಿಂತಿರುಗಿ OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_compl_globals(
    ಪೂರ್ಣಾಂಕ		startcol ಬಳಸದ,
    colnr_T	curs_ಲಂಬಸಾಲು ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		is_cpt_compl ಬಳಸದ)
{
    ಆದರೆ (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_ನಮೂನೆ);
	ಆದರೆ (startcol < compl_ಲಂಬಸಾಲು)
	    ಹಿಂತಿರುಗಿ prepend_startcol_ಪಠ್ಯ(&cpt_compl_ನಮೂನೆ, &compl_orig_ಪಠ್ಯ,
		    startcol);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cpt_compl_ನಮೂನೆ.string = vim_strnsave(compl_orig_ಪಠ್ಯ.string,
		    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ);
	    cpt_compl_ನಮೂನೆ.ಉದ್ದಳತೆ = compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (startcol < 0 || startcol > curs_ಲಂಬಸಾಲು)
	    startcol = curs_ಲಂಬಸಾಲು;

	// Re-obtain line in case it has changed
	char_u	*ಸಾಲು = ml_get(curwin->w_cursor.lnum);
	ಪೂರ್ಣಾಂಕ	len = curs_ಲಂಬಸಾಲು - startcol;

	compl_ನಮೂನೆ.string = vim_strnsave(ಸಾಲು + startcol, (size_t)len);
	ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
	{
	    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = (size_t)len;
	compl_ಲಂಬಸಾಲು = startcol;
	compl_ಉದ್ದಳತೆ = len;
    }

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the pattern, column and length for user defined completion ('omnifunc',
 * 'completefunc' and 'thesaurusfunc')
 * Uses the global variable: spell_bad_len
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 * "startcol", when not NULL, contains the column returned by function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_userdefined_compl_info(
    colnr_T	curs_ಲಂಬಸಾಲು ಬಳಸದ,
    callback_T	*cb ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		*startcol ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	ವಾದಗಳು[3];
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    char_u	*funcname = ಖಾಲಿ;
    pos_T	pos;
    ಪೂರ್ಣಾಂಕ		save_State = State;
    ಪೂರ್ಣಾಂಕ		is_cpt_function = (cb != ಖಾಲಿ);
    ಪೂರ್ಣಾಂಕ		use_sandbox = is_cpt_function
			    && was_ಗಣ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ಆದರೆ (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_ಬಹುಲಕ);
	ಆದರೆ (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_ಗಣ), ctrl_x_ಬಹುಲಕ_function()
		    ? "completefunc" : "omnifunc");
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	cb = get_ಸೇರಿಸು_callback(ctrl_x_ಬಹುಲಕ);
    }

    ವಾದಗಳು[0].v_type = VAR_ಸಂಖ್ಯೆ;
    ವಾದಗಳು[0].vval.v_ಸಂಖ್ಯೆ = 1;
    ವಾದಗಳು[1].v_type = VAR_STRING;
    ವಾದಗಳು[1].vval.v_string = (char_u *)"";
    ವಾದಗಳು[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    ಆದರೆ (use_sandbox)
	++sandbox;
    ಲಂಬಸಾಲು = call_callback_retnr(cb, 2, ವಾದಗಳು);
    ಆದರೆ (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ಆದರೆ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_ಪಠ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (startcol != ಖಾಲಿ)
	*startcol = ಲಂಬಸಾಲು;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    ಆದರೆ (ಲಂಬಸಾಲು == -2 || aborting())
	ಹಿಂತಿರುಗಿ FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    ಆದರೆ (ಲಂಬಸಾಲು == -3)
    {
	ಆದರೆ (is_cpt_function)
	    ಹಿಂತಿರುಗಿ FAIL;
	ctrl_x_ಬಹುಲಕ = CTRL_X_ಸಾಮಾನ್ಯ;
	edit_submode = ಖಾಲಿ;
	ಆದರೆ (!shortmess(SHM_COMPLETIONMENU))
	    ಸಂದೇಶ_clr_cmdline();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_ಖಾಲಿಗಳು = FALSE;

    ret = !is_cpt_function ? ಗಣ_compl_globals(ಲಂಬಸಾಲು, curs_ಲಂಬಸಾಲು, FALSE) : OK;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_spell_compl_info(ಪೂರ್ಣಾಂಕ startcol ಬಳಸದ, colnr_T curs_ಲಂಬಸಾಲು ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    char_u	*ಸಾಲು = ಖಾಲಿ;

    ಆದರೆ (spell_bad_len > 0)
	compl_ಲಂಬಸಾಲು = curs_ಲಂಬಸಾಲು - spell_bad_len;
    ಇಲ್ಲದಿದ್ದರೆ
	compl_ಲಂಬಸಾಲು = spell_ಶಬ್ದ_ಆರಂಭ(startcol);

    ಆದರೆ (compl_ಲಂಬಸಾಲು >= (colnr_T)startcol)
    {
	compl_ಉದ್ದಳತೆ = 0;
	compl_ಲಂಬಸಾಲು = curs_ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	spell_expand_check_cap(compl_ಲಂಬಸಾಲು);
	compl_ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)curs_ಲಂಬಸಾಲು - compl_ಲಂಬಸಾಲು;
    }
    // Need to obtain "line" again, it may have become invalid.
    ಸಾಲು = ml_get(curwin->w_cursor.lnum);
    compl_ನಮೂನೆ.string = vim_strnsave(ಸಾಲು + compl_ಲಂಬಸಾಲು, (size_t)compl_ಉದ್ದಳತೆ);
    ಆದರೆ (compl_ನಮೂನೆ.string == ಖಾಲಿ)
    {
	compl_ನಮೂನೆ.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    compl_ನಮೂನೆ.ಉದ್ದಳತೆ = (size_t)compl_ಉದ್ದಳತೆ;
    ret = OK;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compl_get_info(char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ startcol, colnr_T curs_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ *ಸಾಲು_invalid)
{
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() || ctrl_x_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್()
	    || (ctrl_x_ಬಹುಲಕ & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_ಬಹುಲಕ)))
    {
	ಆದರೆ (get_ಸಾಮಾನ್ಯ_compl_info(ಸಾಲು, startcol, curs_ಲಂಬಸಾಲು) != OK)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಸಾಲು_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
    {
	ಹಿಂತಿರುಗಿ get_wholeline_compl_info(ಸಾಲು, curs_ಲಂಬಸಾಲು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_files())
    {
	ಹಿಂತಿರುಗಿ get_filename_compl_info(ಸಾಲು, startcol, curs_ಲಂಬಸಾಲು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ == CTRL_X_CMDLINE)
    {
	ಹಿಂತಿರುಗಿ get_cmdline_compl_info(ಸಾಲು, curs_ಲಂಬಸಾಲು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_function() || ctrl_x_ಬಹುಲಕ_omni()
	    || thesaurus_func_complete(ctrl_x_ಬಹುಲಕ))
    {
	ಆದರೆ (get_userdefined_compl_info(curs_ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ) != OK)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಸಾಲು_invalid = TRUE;	// "line" may have become invalid
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_spell())
    {
	ಆದರೆ (get_spell_compl_info(startcol, curs_ಲಂಬಸಾಲು) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಸಾಲು_invalid = TRUE;	// "line" may have become invalid
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಂತರಿಕ_error("ins_complete()");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Continue an interrupted completion mode search in "line".
 *
 * If this same ctrl_x_mode has been interrupted use the text from
 * "compl_startpos" to the cursor as a pattern to add a new word instead of
 * expand the one before the cursor, in word-wise if "compl_startpos" is not in
 * the same line as the cursor then fix it (the line has been split because it
 * was longer than 'tw').  if SOL is set then skip the previous pattern, a word
 * at the beginning of the line has been inserted, we'll look for that.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_continue_ಹುಡುಕು(char_u *ಸಾಲು)
{
    // it is a continued search
    compl_cont_ಸ್ಥಿತಿ &= ~CONT_INTRPT;	// remove INTRPT
    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() || ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_patterns()
						|| ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ())
    {
	ಆದರೆ (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_ಲಂಬಸಾಲು = (colnr_T)getwhitecols(ಸಾಲು);
	    compl_startpos.ಲಂಬಸಾಲು = compl_ಲಂಬಸಾಲು;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_ಸ್ಥಿತಿ &= ~CONT_SOL;   // clear SOL if present
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    ಆದರೆ (compl_cont_ಸ್ಥಿತಿ & CONT_S_IPOS)
	    {
		compl_cont_ಸ್ಥಿತಿ |= CONT_SOL;
		compl_startpos.ಲಂಬಸಾಲು = (colnr_T)(skipwhite(
			    ಸಾಲು + compl_ಉದ್ದಳತೆ
			    + compl_startpos.ಲಂಬಸಾಲು) - ಸಾಲು);
	    }
	    compl_ಲಂಬಸಾಲು = compl_startpos.ಲಂಬಸಾಲು;
	}
	compl_ಉದ್ದಳತೆ = curwin->w_cursor.ಲಂಬಸಾಲು - (ಪೂರ್ಣಾಂಕ)compl_ಲಂಬಸಾಲು;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#ವ್ಯಾಖ್ಯಾನಿಸು	MIN_SPACE 75
	ಆದರೆ (compl_ಉದ್ದಳತೆ > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_ಸ್ಥಿತಿ &= ~CONT_SOL;
	    compl_ಉದ್ದಳತೆ = (IOSIZE - MIN_SPACE);
	    compl_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು - compl_ಉದ್ದಳತೆ;
	}
	compl_cont_ಸ್ಥಿತಿ |= CONT_ADDING | CONT_N_ADDS;
	ಆದರೆ (compl_ಉದ್ದಳತೆ < 1)
	    compl_cont_ಸ್ಥಿತಿ &= CONT_LOCAL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval() || ctrl_x_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್())
	compl_cont_ಸ್ಥಿತಿ = CONT_ADDING | CONT_N_ADDS;
    ಇಲ್ಲದಿದ್ದರೆ
	compl_cont_ಸ್ಥಿತಿ = 0;
}

/*
 * start insert mode completion
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_compl_ಆರಂಭ(ಶೂನ್ಯ)
{
    char_u	*ಸಾಲು = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		startcol = 0;	    // column where searched text starts
    colnr_T	curs_ಲಂಬಸಾಲು;	    // cursor column
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_invalid = FALSE;
    ಪೂರ್ಣಾಂಕ		save_did_ai = did_ai;
    ಪೂರ್ಣಾಂಕ		flags = CP_ORIGINAL_ಪಠ್ಯ;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;
    ಆದರೆ (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಸಾಲು = ml_get(curwin->w_cursor.lnum);
    curs_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    ಆದರೆ ((compl_cont_ಸ್ಥಿತಿ & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_ಬಹುಲಕ == ctrl_x_ಬಹುಲಕ)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_ಹುಡುಕು(ಸಾಲು);
    ಇಲ್ಲದಿದ್ದರೆ
	compl_cont_ಸ್ಥಿತಿ &= CONT_LOCAL;

    ಆದರೆ (!compl_ಸ್ಥಿತಿ_adding())	// normal expansion
    {
	compl_cont_ಬಹುಲಕ = ctrl_x_ಬಹುಲಕ;
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_ಸ್ಥಿತಿ = 0;
	compl_cont_ಸ್ಥಿತಿ |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (ಪೂರ್ಣಾಂಕ)curs_ಲಂಬಸಾಲು;
	compl_ಲಂಬಸಾಲು = 0;
    }

    // Work out completion pattern and original text -- webb
    ಆದರೆ (compl_get_info(ಸಾಲು, startcol, curs_ಲಂಬಸಾಲು, &ಸಾಲು_invalid) == FAIL)
    {
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_function() || ctrl_x_ಬಹುಲಕ_omni()
				|| thesaurus_func_complete(ctrl_x_ಬಹುಲಕ))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    ಆದರೆ (ಸಾಲು_invalid)
	ಸಾಲು = ml_get(curwin->w_cursor.lnum);

    ಆದರೆ (compl_ಸ್ಥಿತಿ_adding())
    {
	ಆದರೆ (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.ಲಂಬಸಾಲು = compl_ಲಂಬಸಾಲು;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_ಉದ್ದಳತೆ = 0;
	    compl_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	edit_submode_pre = ಖಾಲಿ;
	compl_startpos.ಲಂಬಸಾಲು = compl_ಲಂಬಸಾಲು;
    }

    ಆದರೆ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	ಆದರೆ (compl_cont_ಸ್ಥಿತಿ & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_ಸಂದೇಶ]);
	ಇಲ್ಲದಿದ್ದರೆ
	    edit_submode = (char_u *)_(CTRL_X_ಸಂದೇಶ(ctrl_x_ಬಹುಲಕ));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ಖಾಲಿ);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_ಪಠ್ಯ);
    compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ = (size_t)compl_ಉದ್ದಳತೆ;
    compl_orig_ಪಠ್ಯ.string = vim_strnsave(ಸಾಲು + compl_ಲಂಬಸಾಲು, (size_t)compl_ಉದ್ದಳತೆ);
    ಆದರೆ (p_ic)
	flags |= CP_ICASE;
    ಆದರೆ (compl_orig_ಪಠ್ಯ.string == ಖಾಲಿ
	    || ins_compl_ಕೂಡಿಸು(compl_orig_ಪಠ್ಯ.string,
		(ಪೂರ್ಣಾಂಕ)compl_orig_ಪಠ್ಯ.ಉದ್ದಳತೆ,
		ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, 0, flags, FALSE, ಖಾಲಿ, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_ನಮೂನೆ);
	VIM_CLEAR_STRING(compl_orig_ಪಠ್ಯ);
	did_ai = save_did_ai;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    ಆದರೆ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_ಎಣಿಕೆ;
	showmode();
	edit_submode_extra = ಖಾಲಿ;
	ಹೊರಹೋಗು_flush();
    }

    did_ai = save_did_ai;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * display the completion status message
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_show_statusmsg(ಶೂನ್ಯ)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    ಆದರೆ (is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ))
    {
	edit_submode_extra = compl_ಸ್ಥಿತಿ_adding() && compl_ಉದ್ದಳತೆ > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    ಆದರೆ (edit_submode_extra == ಖಾಲಿ)
    {
	ಆದರೆ (ಹೊಂದಿಕೆ_at_original_ಪಠ್ಯ(compl_curr_ಹೊಂದಿಕೆ))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_cont_ಸ್ಥಿತಿ & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_ಎಣಿಕೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ == compl_curr_ಹೊಂದಿಕೆ->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_ಎಣಿಕೆ;
	    compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ಆದರೆ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ == -1)
		ins_compl_update_sequence_numbers();
#ಆದರೆ_ಅಂತ್ಯ
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		ಸ್ಥಿರ char_u ಹೊಂದಿಕೆ_ref[81];

		ಆದರೆ (compl_matches > 0)
		    vim_snprintf((ಅಕ್ಷರ *)ಹೊಂದಿಕೆ_ref, ಗಾತ್ರ(ಹೊಂದಿಕೆ_ref),
			    _("match %d of %d"),
			    compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ, compl_matches);
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_snprintf((ಅಕ್ಷರ *)ಹೊಂದಿಕೆ_ref, ಗಾತ್ರ(ಹೊಂದಿಕೆ_ref),
			    _("match %d"),
			    compl_curr_ಹೊಂದಿಕೆ->cp_ಸಂಖ್ಯೆ);
		edit_submode_extra = ಹೊಂದಿಕೆ_ref;
		edit_submode_highl = HLF_R;
		ಆದರೆ (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    ಆದರೆ (!compl_opt_suppress_ಖಾಲಿಗಳು)
    {
	showmode();
	ಆದರೆ (!shortmess(SHM_COMPLETIONMENU))
	{
	    ಆದರೆ (edit_submode_extra != ಖಾಲಿ)
	    {
		ಆದರೆ (!p_smd)
		{
		    ಸಂದೇಶ_hist_off = TRUE;
		    ಸಂದೇಶ_attr((ಅಕ್ಷರ *)edit_submode_extra,
			    edit_submode_highl < HLF_ಎಣಿಕೆ
			    ? HL_ATTR(edit_submode_highl) : 0);
		    ಸಂದೇಶ_hist_off = FALSE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    ಪೂರ್ಣಾಂಕ
ins_complete(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ enable_pum)
{
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		save_w_wrow;
    ಪೂರ್ಣಾಂಕ		save_w_leftcol;
    ಪೂರ್ಣಾಂಕ		ಸೇರಿಸು_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		no_matches_ಸಿಕ್ಕಿತು;

    compl_direction = ins_compl_key2dir(c);
    ಸೇರಿಸು_ಹೊಂದಿಕೆ = ins_compl_use_ಹೊಂದಿಕೆ(c);

    ಆದರೆ (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	ಆದರೆ (ins_compl_ಆರಂಭ() == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೇರಿಸು_ಹೊಂದಿಕೆ && stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_ಬಫರ್;
    compl_shown_ಹೊಂದಿಕೆ = compl_curr_ಹೊಂದಿಕೆ;
    compl_shows_dir = compl_direction;
    compl_ಅಂಕೆ_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_ಮುಂದಿನ(TRUE, ins_compl_key2count(c), ಸೇರಿಸು_ಹೊಂದಿಕೆ);

    // Reset autocompletion timer expiry flag
    ಆದರೆ (compl_autocomplete)
	compl_ಸಮಯ_ತುಂಡು_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    ಆದರೆ (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_ಹೊಂದಿಕೆ = compl_shown_ಹೊಂದಿಕೆ;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    ಆದರೆ (got_int && !global_busy)
    {
	(ಶೂನ್ಯ)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_ಸಿಕ್ಕಿತು = is_ಮೊದಲ_ಹೊಂದಿಕೆ(compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_ಮುಂದಿನ);
    ಆದರೆ (no_matches_ಸಿಕ್ಕಿತು)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	ಆದರೆ (compl_ಉದ್ದಳತೆ > 1
		|| compl_ಸ್ಥಿತಿ_adding()
		|| (ctrl_x_ಬಹುಲಕ_not_default()
		    && !ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_patterns()
		    && !ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ()))
	    compl_cont_ಸ್ಥಿತಿ &= ~CONT_N_ADDS;
    }

    ಆದರೆ (compl_curr_ಹೊಂದಿಕೆ->cp_flags & CP_CONT_S_IPOS)
	compl_cont_ಸ್ಥಿತಿ |= CONT_S_IPOS;
    ಇಲ್ಲದಿದ್ದರೆ
	compl_cont_ಸ್ಥಿತಿ &= ~CONT_S_IPOS;

    ಆದರೆ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    ಆದರೆ (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Enable autocompletion
 */
    ಶೂನ್ಯ
ins_compl_enable_autocomplete(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Disable autocompletion
 */
    ಶೂನ್ಯ
ins_compl_disable_autocomplete(ಶೂನ್ಯ)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    ಶೂನ್ಯ
ins_compl_arm_autostart(ಶೂನ್ಯ)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    ಶೂನ್ಯ
ins_compl_disarm_autostart(ಶೂನ್ಯ)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    ಆದರೆ (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_ಆರಂಭ_tv);
	compl_autocomplete_pending = true;
	ಹಿಂತಿರುಗಿ true;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    ಶೂನ್ಯ
ins_compl_clear_autocomplete_delay(ಶೂನ್ಯ)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    ದೀರ್ಘ
ins_compl_autocomplete_elapsed(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    ಹಿಂತಿರುಗಿ ELAPSED_FUNC(compl_autocomplete_ಆರಂಭ_tv);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Remove (if needed) and show the popup menu
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
show_pum(ಪೂರ್ಣಾಂಕ prev_w_wrow, ಪೂರ್ಣಾಂಕ prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    ಆದರೆ (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
	ins_compl_del_pum();

    ins_compl_show_pum();
    setcursor();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Looks in the first "len" chars. of "src" for search-metachars.
 * If dest is not NULL the chars. are copied there quoting (with
 * a backslash) the metachars, and dest would be NUL terminated.
 * Returns the length (needed) of dest
 */
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ
quote_meta(char_u *dest, char_u *src, ಪೂರ್ಣಾಂಕ len)
{
    ಚಿಹ್ನೆರಹಿತ	m = (ಚಿಹ್ನೆರಹಿತ)len + 1;  // one extra for the NUL

    ಗಾಗಿ ( ; --len >= 0; src++)
    {
	ಆಯ್ಕೆ (*src)
	{
	    ಸ್ಥಿತಿ '.':
	    ಸ್ಥಿತಿ '*':
	    ಸ್ಥಿತಿ '[':
		ಆದರೆ (ctrl_x_ಬಹುಲಕ_dictionary() || ctrl_x_ಬಹುಲಕ_thesaurus())
		    ನಿಲ್ಲಿಸು;
		// FALLTHROUGH
	    ಸ್ಥಿತಿ '~':
		ಆದರೆ (!magic_isset())	// quote these only if magic is set
		    ನಿಲ್ಲಿಸು;
		// FALLTHROUGH
	    ಸ್ಥಿತಿ '\\':
		ಆದರೆ (ctrl_x_ಬಹುಲಕ_dictionary() || ctrl_x_ಬಹುಲಕ_thesaurus())
		    ನಿಲ್ಲಿಸು;
		// FALLTHROUGH
	    ಸ್ಥಿತಿ '^':		// currently it's not needed.
	    ಸ್ಥಿತಿ '$':
		m++;
		ಆದರೆ (dest != ಖಾಲಿ)
		    *dest++ = '\\';
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (dest != ಖಾಲಿ)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ mb_len = (*mb_ptr2len)(src) - 1;
	    ಆದರೆ (mb_len > 0 && len >= mb_len)
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    ಆದರೆ (dest != ಖಾಲಿ)
			*dest++ = *src;
		}
	}
    }
    ಆದರೆ (dest != ಖಾಲಿ)
	*dest = NUL;

    ಹಿಂತಿರುಗಿ m;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_insexpand_stuff(ಶೂನ್ಯ)
{
    VIM_CLEAR_STRING(compl_orig_ಪಠ್ಯ);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_ಎಣಿಕೆ);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_ಹಿಂಭಾಗ_ಗಮ್ಯ_badword(ಶೂನ್ಯ)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_ಗಮ್ಯ(curwin, BACKWARD, SMT_ALL, TRUE, ಖಾಲಿ);
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು != tpos.ಲಂಬಸಾಲು)
	ಆರಂಭ_arrow(&tpos);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Reset the info associated with completion sources.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cpt_sources_clear(ಶೂನ್ಯ)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_ಸೂಚ್ಯಂಕ = -1;
    cpt_sources_ಎಣಿಕೆ = 0;
}

/*
 * Setup completion sources.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸಿದ್ಧತೆ_cpt_sources(ಶೂನ್ಯ)
{
    char_u  buf[LSIZE];
    ಪೂರ್ಣಾಂಕ	    slen;
    ಪೂರ್ಣಾಂಕ	    idx = 0;
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2;
    char_u  *p;

    cpt_sources_clear();

    ಎಣಿಕೆ_2 = get_cpt_sources_ಎಣಿಕೆ(curbuf->b_p_cpt);
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_ಮೂಲ_T, ಎಣಿಕೆ_2);
    ಆದರೆ (cpt_sources_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (p = curbuf->b_p_cpt; *p;)
    {
	ಆಗ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ಆದರೆ (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ಆದರೆ (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		ಆದರೆ (caret != ಖಾಲಿ)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((ಅಕ್ಷರ *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_cpt_func_refresh_always(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cpt_sources_ಎಣಿಕೆ; i++)
	ಆದರೆ (cpt_sources_array[i].cs_refresh_always)
	    ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_make_ರೇಖೀಯ(ಶೂನ್ಯ)
{
    compl_T *m;

    ಆದರೆ (compl_ಮೊದಲ_ಹೊಂದಿಕೆ == ಖಾಲಿ || compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    m = compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev;
    m->cp_ಮುಂದಿನ = ಖಾಲಿ;
    compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev = ಖಾಲಿ;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_old_matches(ಶೂನ್ಯ)
{
    compl_T *current;
    ಪೂರ್ಣಾಂಕ	    shown_ಹೊಂದಿಕೆ_removed = FALSE;
    ಪೂರ್ಣಾಂಕ	    forward = (compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_cpt_ಮೂಲ_idx < 0);

    ಆದರೆ (cpt_sources_ಸೂಚ್ಯಂಕ < 0)
	ಹಿಂತಿರುಗಿ;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    ಗಾಗಿ (current = compl_ಮೊದಲ_ಹೊಂದಿಕೆ; current != ಖಾಲಿ; )
    {
	ಆದರೆ (current->cp_cpt_ಮೂಲ_idx == cpt_sources_ಸೂಚ್ಯಂಕ)
	{
	    compl_T *ಗಮ್ಯ_delete = current;

	    ಆದರೆ (!shown_ಹೊಂದಿಕೆ_removed && compl_shown_ಹೊಂದಿಕೆ == current)
		shown_ಹೊಂದಿಕೆ_removed = TRUE;

	    current = current->cp_ಮುಂದಿನ;

	    ಆದರೆ (ಗಮ್ಯ_delete == compl_ಮೊದಲ_ಹೊಂದಿಕೆ)  // node to remove is at head
	    {
		compl_ಮೊದಲ_ಹೊಂದಿಕೆ = ಗಮ್ಯ_delete->cp_ಮುಂದಿನ;
		compl_ಮೊದಲ_ಹೊಂದಿಕೆ->cp_prev = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಮ್ಯ_delete->cp_ಮುಂದಿನ == ಖಾಲಿ) // node to remove is at tail
		ಗಮ್ಯ_delete->cp_prev->cp_ಮುಂದಿನ = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ // node is in the moddle
	    {
		ಗಮ್ಯ_delete->cp_prev->cp_ಮುಂದಿನ = ಗಮ್ಯ_delete->cp_ಮುಂದಿನ;
		ಗಮ್ಯ_delete->cp_ಮುಂದಿನ->cp_prev = ಗಮ್ಯ_delete->cp_prev;
	    }
	    ins_compl_ಅಂಶ_free(ಗಮ್ಯ_delete);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    current = current->cp_ಮುಂದಿನ;
    }

    // Re-assign compl_shown_match if necessary
    ಆದರೆ (shown_ಹೊಂದಿಕೆ_removed)
    {
	ಆದರೆ (forward)
	    compl_shown_ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	{  // Last node will have the prefix that is being completed
	    ಗಾಗಿ (current = compl_ಮೊದಲ_ಹೊಂದಿಕೆ; current->cp_ಮುಂದಿನ != ಖಾಲಿ;
		    current = current->cp_ಮುಂದಿನ)
		;
	    compl_shown_ಹೊಂದಿಕೆ = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_ಹೊಂದಿಕೆ = compl_ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಗಾಗಿ (current = compl_ಮೊದಲ_ಹೊಂದಿಕೆ; current != ಖಾಲಿ; )
    {
	ಆದರೆ ((forward ? current->cp_cpt_ಮೂಲ_idx < cpt_sources_ಸೂಚ್ಯಂಕ
		    : current->cp_cpt_ಮೂಲ_idx > cpt_sources_ಸೂಚ್ಯಂಕ))
	{
	    compl_curr_ಹೊಂದಿಕೆ = forward ? current : current->cp_ಮುಂದಿನ;
	    current = current->cp_ಮುಂದಿನ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    ಸ್ಥಿರ ಶೂನ್ಯ
get_cpt_func_completion_matches(callback_T *cb ಬಳಸದ)
{
    cpt_ಮೂಲ_T    *cpt_src = &cpt_sources_array[cpt_sources_ಸೂಚ್ಯಂಕ];
    ಪೂರ್ಣಾಂಕ		    startcol = cpt_src->cs_startcol;

    ಆದರೆ (startcol == -2 || startcol == -3)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಗಣ_compl_globals(startcol, curwin->w_cursor.ಲಂಬಸಾಲು, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	ಆದರೆ (!cpt_src->cs_refresh_always)
	    ins_compl_ಸೇರಿಸು_ಬೈಟುಗಳು(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_ನಮೂನೆ.string, cb);

	ಆದರೆ (!cpt_src->cs_refresh_always)
	    ins_compl_delete();

	cpt_src->cs_refresh_always = compl_opt_refresh_always;
	compl_opt_refresh_always = FALSE;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Retrieve completion matches from functions in the 'cpt' option where the
 * 'refresh:always' flag is set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cpt_compl_refresh(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_ರೇಖೀಯ();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_ಒಳ_ಇರಿಸು(cpt);

    cpt_sources_ಸೂಚ್ಯಂಕ = 0;
    ಗಾಗಿ (p = cpt; *p;)
    {
	ಆಗ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ಆದರೆ (*p == NUL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (cpt_sources_array[cpt_sources_ಸೂಚ್ಯಂಕ].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_ಸೂಚ್ಯಂಕ);
	    ಆದರೆ (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.ಲಂಬಸಾಲು, cb,
			&startcol);
		ಆದರೆ (ret == FAIL)
		{
		    ಆದರೆ (startcol == -3)
			cpt_sources_array[cpt_sources_ಸೂಚ್ಯಂಕ].cs_refresh_always
			    = FALSE;
		    ಇಲ್ಲದಿದ್ದರೆ
			startcol = -2;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (startcol < 0 || startcol > curwin->w_cursor.ಲಂಬಸಾಲು)
		    startcol = curwin->w_cursor.ಲಂಬಸಾಲು;
		cpt_sources_array[cpt_sources_ಸೂಚ್ಯಂಕ].cs_startcol = startcol;
		ಆದರೆ (ret == OK)
		{
		    compl_ಮೂಲ_ಆರಂಭ_timer(cpt_sources_ಸೂಚ್ಯಂಕ);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(ಶೂನ್ಯ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	ಆದರೆ (may_advance_cpt_ಸೂಚ್ಯಂಕ(p))
	    (ಶೂನ್ಯ)advance_cpt_sources_ಸೂಚ್ಯಂಕ_ಸುರಕ್ಷಿತ();
    }
    cpt_sources_ಸೂಚ್ಯಂಕ = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "preinserted()" function
 */
    ಶೂನ್ಯ
f_preinserted(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ins_compl_preinsert_effect())
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}
