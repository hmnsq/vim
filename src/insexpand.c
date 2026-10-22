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
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_ಸಾಲು	3
#define CTRL_X_FILES		4
#define CTRL_X_TAGS		(5 + CTRL_X_WANT_IDENT)
#define CTRL_X_PATH_PATTERNS	(6 + CTRL_X_WANT_IDENT)
#define CTRL_X_PATH_DEFINES	(7 + CTRL_X_WANT_IDENT)
#define CTRL_X_FINISHED		8
#define CTRL_X_DICTIONARY	(9 + CTRL_X_WANT_IDENT)
#define CTRL_X_THESAURUS	(10 + CTRL_X_WANT_IDENT)
#define CTRL_X_CMDLINE		11
#define CTRL_X_FUNCTION	12
#define CTRL_X_OMNI		13
#define CTRL_X_SPELL		14
#define CTRL_X_LOCAL_ಸಂದೇಶ	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_ಸಂದೇಶ(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
ಸ್ಥಿರ ವರ್ಣ *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ರಿಕ್ತ, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ರಿಕ್ತ, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ರಿಕ್ತ,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#ಯದಿ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ಸ್ಥಿರ string_T ctrl_x_mode_names[] = {
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
    {ರಿಕ್ತ, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ compl_S compl_T;
ಸಂರಚನಾ compl_S
{
    compl_T	*cp_next;
    compl_T	*cp_prev;
    compl_T	*cp_match_next;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_ಎಣಿಕೆ]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_data;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    ಪೂರ್ಣಾಙ್ಕ		cp_flags;		// CP_ values
    ಪೂರ್ಣಾಙ್ಕ		cp_ಸಂಖ್ಯೆ;		// sequence number
    ಪೂರ್ಣಾಙ್ಕ		cp_order;		// list position when first scored
    ಪೂರ್ಣಾಙ್ಕ		cp_score;		// fuzzy match score or proximity score
    ಪೂರ್ಣಾಙ್ಕ		cp_in_match_array;	// collected by compl_match_array
    ಪೂರ್ಣಾಙ್ಕ		cp_user_abbr_hl_id;	// highlight group ID for abbr
    ಪೂರ್ಣಾಙ್ಕ		cp_user_kind_hl_id;	// highlight group ID for kind
    ಪೂರ್ಣಾಙ್ಕ		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
};

// values for cp_flags
#define CP_ORIGINAL_TEXT   1	// the original text when the expansion begun
#define CP_FREE_FNAME	    2	// cp_fname is allocated
#define CP_CONT_S_IPOS	    4	// use CONT_S_IPOS for compl_cont_status
#define CP_EQUAL	    8	// ins_compl_equal() always returns TRUE
#define CP_ICASE	    16	// ins_compl_equal() ignores case
#define CP_FAST	    32	// use fast_breakcheck instead of ui_breakcheck

/*
 * All the current matches are stored in a list.
 * "compl_first_match" points to the start of the list.
 * "compl_curr_match" points to the currently selected entry.
 * "compl_shown_match" is different from compl_curr_match during
 * ins_compl_get_exp(), when new matches are added to the list.
 * "compl_old_match" points to previous "compl_curr_match".
 */
ಸ್ಥಿರ compl_T    *compl_first_match = ರಿಕ್ತ;
ಸ್ಥಿರ compl_T    *compl_curr_match = ರಿಕ್ತ;
ಸ್ಥಿರ compl_T    *compl_shown_match = ರಿಕ್ತ;
ಸ್ಥಿರ compl_T    *compl_old_match = ರಿಕ್ತ;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ	    cse_ಎಣಿಕೆ;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((ಪೂರ್ಣಾಙ್ಕ)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_ಕೀಲಿ - CSE_OFF))

ಸ್ಥಿರ hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    ಯದಿ (compl_strings_ht.ht_array == ರಿಕ್ತ)
	hash_init(&compl_strings_ht);
    ಯದಿ (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    ಯದಿ (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	ಯದಿ (entry == ರಿಕ್ತ)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    ಪ್ರತ್ಯಾಗಮ;
	entry->cse_ಎಣಿಕೆ = 1;
	vim_strncpy(entry->cse_str, str, len);
	ಯದಿ (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    ಅನ್ಯಥಾ
	++HI2CSE(hi)->cse_ಎಣಿಕೆ;
}

// list used to store the compl_T which have the max score
ಸ್ಥಿರ compl_T	  **compl_best_matches = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_ಅಂಕೆ_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
ಸ್ಥಿರ string_T	  compl_leader = {ರಿಕ್ತ, 0};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_used_match;

// didn't finish finding completions.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_interrupted = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  ctrl_x_mode = CTRL_X_NORMAL;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_matches = 0;	    // number of completion matches
ಸ್ಥಿರ string_T	  compl_pattern = {ರಿಕ್ತ, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
ಸ್ಥಿರ string_T	  cpt_compl_pattern = {ರಿಕ್ತ, 0}; // pattern returned by func in 'cpt'
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_direction = FORWARD;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_shows_dir = FORWARD;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_pending = 0;	    // > 1 for postponed CTRL-N
ಸ್ಥಿರ pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_length = 0;
ಸ್ಥಿರ linenr_T	  compl_lnum = 0;           // lnum where the completion start
ಸ್ಥಿರ colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
ಸ್ಥಿರ colnr_T	  compl_ins_end_col = 0;
ಸ್ಥಿರ colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
ಸ್ಥಿರ string_T	  compl_orig_text = {ರಿಕ್ತ, 0};  // text as it was before
					    // completion started
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_cont_mode = 0;
ಸ್ಥಿರ expand_T	  compl_xp;

ಸ್ಥಿರ win_T	  *compl_curr_win = ರಿಕ್ತ;  // win where completion is active
ಸ್ಥಿರ buf_T	  *compl_curr_buf = ರಿಕ್ತ;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_autocomplete = FALSE;	    // whether autocompletion is active
ಸ್ಥಿರ bool	  compl_autostarted = false;	    // Vim started this completion
ಸ್ಥಿರ bool	  compl_autostart_pending = false;  // the trigger armed one
ಸ್ಥಿರ bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
ಸ್ಥಿರ elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    ಕುರು { \
	ಯದಿ (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } ಯಾವತ್ (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_cont_status = 0;
#define CONT_ADDING	1	// "normal" or "adding" expansion
#define CONT_INTRPT	(2 + 4)	// a ^X interrupted the current expansion
				// it's set only iff N_ADDS is set
#define CONT_N_ADDS	4	// next ^X<> will add-new or expand-current
#define CONT_S_IPOS	8	// next ^X<> will set initial_pos?
				// if so, word-wise-expansion will set SOL
#define CONT_SOL	16	// pattern includes start of line, just for
				// word-wise expansion, not set for ^X^L
#define CONT_LOCAL	32	// for ctrl_x_mode 0, ^X^P/^X^N do a local
				// expansion, (eg use complete=.)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_opt_refresh_always = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_opt_suppress_empty = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  compl_selected_item = -1;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ cpt_source_T
{
    ಪೂರ್ಣಾಙ್ಕ	cs_refresh_always;  // Whether 'refresh:always' is set for func
    ಪೂರ್ಣಾಙ್ಕ	cs_startcol;	    // Start column returned by func
    ಪೂರ್ಣಾಙ್ಕ	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

ಸ್ಥಿರ cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cpt_sources_ಎಣಿಕೆ;  // Total number of completion sources specified in the 'cpt' option
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cpt_sources_index = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ಸ್ಥಿರ pumitem_T *compl_match_array = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compl_match_arraysize;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ins_compl_add(char_u *str, ಪೂರ್ಣಾಙ್ಕ len, char_u *fname, char_u **cptext, typval_T *user_data, ಪೂರ್ಣಾಙ್ಕ cdir, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ adup, ಪೂರ್ಣಾಙ್ಕ *user_hl, ಪೂರ್ಣಾಙ್ಕ score);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_longest_match(compl_T *match);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_del_pum(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_files(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, char_u **files, ಪೂರ್ಣಾಙ್ಕ thesaurus, ಪೂರ್ಣಾಙ್ಕ flags, regmatch_T *regmatch, char_u *buf, ಪೂರ್ಣಾಙ್ಕ *dir);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_free(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_compl_need_restart(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_new_leader(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  get_compl_len(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_restart(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_set_original_text(char_u *str, size_t len);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#ಯದಿ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_add_list(list_T *list);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_add_dict(dict_T *dict);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, ಪೂರ್ಣಾಙ್ಕ *startcol);
ಸ್ಥಿರ ಶೂನ್ಯ get_cpt_func_completion_matches(callback_T *cb);
ಸ್ಥಿರ callback_T *get_callback_if_cpt_func(char_u *p, ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ fill_complete_info_dict(dict_T *di, compl_T *match, ಪೂರ್ಣಾಙ್ಕ add_match);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ setup_cpt_sources(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_cpt_func_refresh_always(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ cpt_sources_clear(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ cpt_compl_refresh(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_compl_key2dir(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_compl_pum_ಕೀಲಿ(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_compl_key2count(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ show_pum(ಪೂರ್ಣಾಙ್ಕ prev_w_wrow, ಪೂರ್ಣಾಙ್ಕ prev_w_leftcol);
ಸ್ಥಿರ unsigned  quote_meta(char_u *dest, char_u *str, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ins_compl_has_multiple(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_expand_multiple(char_u *str);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_longest_ಸೇರಿಸು(char_u *prefix);
ಸ್ಥಿರ ಶೂನ್ಯ ins_compl_make_linear(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ins_compl_make_cyclic(ಶೂನ್ಯ);

#ifdef FEAT_SPELL
ಸ್ಥಿರ ಶೂನ್ಯ spell_back_to_badword(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    ಶೂನ್ಯ
ins_ctrl_x(ಶೂನ್ಯ)
{
    ಯದಿ (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	ಯದಿ (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	ಅನ್ಯಥಾ
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_ಸಂದೇಶ(ctrl_x_mode));
	edit_submode_pre = ರಿಕ್ತ;
	showmode();
    }
    ಅನ್ಯಥಾ
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_none(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == 0; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_normal(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_NORMAL; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_scroll(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_SCROLL; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_whole_ಸಾಲು(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_WHOLE_ಸಾಲು; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_files(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_FILES; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_tags(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_TAGS; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_path_patterns(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_path_defines(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_PATH_DEFINES; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_dictionary(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_DICTIONARY; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_thesaurus(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_THESAURUS; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_cmdline(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_function(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_FUNCTION; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_omni(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_OMNI; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_spell(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_SPELL; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_eval(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_EVAL; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_ಸಾಲು_or_eval(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_WHOLE_ಸಾಲು || ctrl_x_mode == CTRL_X_EVAL; }
ಪೂರ್ಣಾಙ್ಕ ctrl_x_mode_register(ಶೂನ್ಯ)
    { ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    ಪೂರ್ಣಾಙ್ಕ
ctrl_x_mode_not_default(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
ctrl_x_mode_not_defined_yet(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    ಪೂರ್ಣಾಙ್ಕ
compl_status_adding(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    ಪೂರ್ಣಾಙ್ಕ
compl_status_sol(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    ಪೂರ್ಣಾಙ್ಕ
compl_status_local(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    ಶೂನ್ಯ
compl_status_clear(ಶೂನ್ಯ)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compl_dir_forward(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compl_shows_dir_forward(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compl_shows_dir_backward(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_compl_option(ಪೂರ್ಣಾಙ್ಕ dict_opt)
{
    ಯದಿ (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
#ifdef FEAT_SPELL
							&& !curwin->w_p_spell
#endif
							)
		 : (*curbuf->b_p_tsr == NUL && *p_tsr == NUL
#ifdef FEAT_COMPL_FUNC
		     && *curbuf->b_p_tsrfu == NUL && *p_tsrfu == NUL
#endif
		   ))
    {
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = ರಿಕ್ತ;
	ಸಂದೇಶ_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	ಯದಿ (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    ಯದಿ (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_is_ctrl_x_ಕೀಲಿ(ಪೂರ್ಣಾಙ್ಕ c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    ಯದಿ (ins_compl_pum_ಕೀಲಿ(c))
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ವಿಕಲ್ಪ (ctrl_x_mode)
    {
	ಪಕ್ಷ 0:		    // Not in any CTRL-X mode
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	ಪಕ್ಷ CTRL_X_NOT_DEFINED_YET:
	ಪಕ್ಷ CTRL_X_CMDLINE_CTRL_X:
	    ಪ್ರತ್ಯಾಗಮ (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	ಪಕ್ಷ CTRL_X_SCROLL:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_Y || c == Ctrl_E);
	ಪಕ್ಷ CTRL_X_WHOLE_ಸಾಲು:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_FILES:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_DICTIONARY:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_THESAURUS:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_TAGS:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	ಪಕ್ಷ CTRL_X_PATH_PATTERNS:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_PATH_DEFINES:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	ಪಕ್ಷ CTRL_X_CMDLINE:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	ಪಕ್ಷ CTRL_X_FUNCTION:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_OMNI:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	ಪಕ್ಷ CTRL_X_SPELL:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_EVAL:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_P || c == Ctrl_N);
	ಪಕ್ಷ CTRL_X_REGISTER:
	    ಪ್ರತ್ಯಾಗಮ (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_at_original_text(compl_T *match)
{
    ಪ್ರತ್ಯಾಗಮ match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_first_match(compl_T *match)
{
    ಪ್ರತ್ಯಾಗಮ match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    ಸ್ಥಿರ compl_T *
find_original_text_match(ಶೂನ್ಯ)
{
    ಯದಿ (compl_first_match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (match_at_original_text(compl_first_match))
	ಪ್ರತ್ಯಾಗಮ compl_first_match;
    ಯದಿ (compl_first_match->cp_prev != ರಿಕ್ತ
	    && match_at_original_text(compl_first_match->cp_prev))
	ಪ್ರತ್ಯಾಗಮ compl_first_match->cp_prev;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_accept_char(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (compl_autocomplete && compl_from_nonkeyword)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	ಪ್ರತ್ಯಾಗಮ vim_isIDc(c);

    ವಿಕಲ್ಪ (ctrl_x_mode)
    {
	ಪಕ್ಷ CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    ಪ್ರತ್ಯಾಗಮ vim_isfilec(c) && !vim_ispathsep(c);

	ಪಕ್ಷ CTRL_X_CMDLINE:
	ಪಕ್ಷ CTRL_X_CMDLINE_CTRL_X:
	ಪಕ್ಷ CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    ಪ್ರತ್ಯಾಗಮ vim_isprintc(c) && !VIM_ISWHITE(c);

	ಪಕ್ಷ CTRL_X_WHOLE_ಸಾಲು:
	    // For while line completion a space can be part of the line.
	    ಪ್ರತ್ಯಾಗಮ vim_isprintc(c);
    }
    ಪ್ರತ್ಯಾಗಮ vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    ಸ್ಥಿರ char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    ಪೂರ್ಣಾಙ್ಕ	    char_len,
    ಪೂರ್ಣಾಙ್ಕ	    compl_char_len,
    ಪೂರ್ಣಾಙ್ಕ	    min_len,
    char_u  **tofree)
{
    ಪೂರ್ಣಾಙ್ಕ		*wca;			// Wide character array.
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i, c;
    ಪೂರ್ಣಾಙ್ಕ		has_lower = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, char_len);
    ಯದಿ (wca == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ IObuff;

    p = str;
    ಪ್ರತಿ (i = 0; i < char_len; ++i)
    {
	ಯದಿ (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	ಅನ್ಯಥಾ
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    ಪ್ರತಿ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ಯದಿ (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    ಯದಿ (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		ಪ್ರತಿ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		ಭಙ್ಗ;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    ಯದಿ (!has_lower)
    {
	p = compl_orig_text.string;
	ಪ್ರತಿ (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    ಯದಿ (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		ಪ್ರತಿ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		ಭಙ್ಗ;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    ಪ್ರತಿ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ಯದಿ (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	ಅನ್ಯಥಾ ಯದಿ (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    ಯಾವತ್ (i < char_len)
    {
	ಯದಿ (gap.ga_data != ರಿಕ್ತ)
	{
	    ಯದಿ (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		ಪ್ರತ್ಯಾಗಮ (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    ಯದಿ (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    ಅನ್ಯಥಾ
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    ಯದಿ (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		ಪ್ರತ್ಯಾಗಮ (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (ಪೂರ್ಣಾಙ್ಕ)(p - IObuff);
	}
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	ಅನ್ಯಥಾ
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    ಯದಿ (gap.ga_data != ರಿಕ್ತ)
    {
	*tofree = gap.ga_data;
	ಪ್ರತ್ಯಾಗಮ gap.ga_data;
    }

    *p = NUL;
    ಪ್ರತ್ಯಾಗಮ IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_add_infercase(
    char_u	*str_arg,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		icase,
    char_u	*fname,
    ಪೂರ್ಣಾಙ್ಕ		dir,
    ಪೂರ್ಣಾಙ್ಕ		cont_s_ipos,  // next ^X<> will set initial_pos
    ಪೂರ್ಣಾಙ್ಕ		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		char_len;		// count multi-byte characters
    ಪೂರ್ಣಾಙ್ಕ		compl_char_len;
    ಪೂರ್ಣಾಙ್ಕ		min_len;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		res;
    char_u	*tofree = ರಿಕ್ತ;

    ಯದಿ (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	ಯದಿ (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    ಯಾವತ್ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    char_len = len;
	}

	// Find actual length of original text.
	ಯದಿ (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    ಯಾವತ್ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    compl_char_len = compl_length;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    ಯದಿ (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    ಯದಿ (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, ರಿಕ್ತ, ರಿಕ್ತ, dir, flags, FALSE, ರಿಕ್ತ, score);
    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Check if fuzzy matching is enabled
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cot_fuzzy(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_nearest_active(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_is_match_selected(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_shown_match != ರಿಕ್ತ && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_preinsert_longest(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_autocomplete
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_add(
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_data UNUSED,  // "user_data" entry or NULL
    ಪೂರ್ಣಾಙ್ಕ		cdir,
    ಪೂರ್ಣಾಙ್ಕ		flags_arg,
    ಪೂರ್ಣಾಙ್ಕ		adup,		    // accept duplicate match
    ಪೂರ್ಣಾಙ್ಕ		*user_hl,           // user abbr/kind hlattr
    ಪೂರ್ಣಾಙ್ಕ		score)
{
    compl_T	*match, *current, *prev;
    ಪೂರ್ಣಾಙ್ಕ		dir = (cdir == 0 ? compl_direction : cdir);
    ಪೂರ್ಣಾಙ್ಕ		flags = flags_arg;
    ಪೂರ್ಣಾಙ್ಕ		inserted = FALSE;
    char_u	*new_str = ರಿಕ್ತ;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    ಯದಿ (flags & CP_FAST)
	fast_breakcheck();
    ಅನ್ಯಥಾ
	ui_breakcheck();
    ಯದಿ (got_int)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (len < 0)
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);

    // If the same match is already present, don't add it.
    ಯದಿ (compl_first_match != ರಿಕ್ತ && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *ಕೀಲಿ;
	hashitem_T  *hi;

	ಯದಿ (len < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    ಕೀಲಿ = keybuf;
	}
	ಅನ್ಯಥಾ
	{
	    new_str = vim_strnsave(str, len);
	    ಯದಿ (new_str == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಕೀಲಿ = new_str;
	}
	str_hash = hash_hash(ಕೀಲಿ);
	hi = hash_lookup(&compl_strings_ht, ಕೀಲಿ, str_hash);
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    ಯದಿ (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		ಕುರು
		{
		    ಯದಿ (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((ಪೂರ್ಣಾಙ್ಕ)match->cp_str.length <= len
					  || match->cp_str.string[len] == NUL))
		    {
			ಯದಿ (score < match->cp_score)
			    match->cp_score = score;
			ಭಙ್ಗ;
		    }
		    match = match->cp_next;
		} ಯಾವತ್ (match != ರಿಕ್ತ && !is_first_match(match));
	    }
	    vim_free(new_str);
	    ಪ್ರತ್ಯಾಗಮ NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    ಯದಿ (match == ರಿಕ್ತ)
    {
	vim_free(new_str);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    match->cp_ಸಂಖ್ಯೆ = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    ಯದಿ (new_str == ರಿಕ್ತ && (new_str = vim_strnsave(str, len)) == ರಿಕ್ತ)
    {
	vim_free(match);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.length = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    ಯದಿ (fname != ರಿಕ್ತ
	    && compl_curr_match != ರಿಕ್ತ
	    && compl_curr_match->cp_fname != ರಿಕ್ತ
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    ಅನ್ಯಥಾ ಯದಿ (fname != ರಿಕ್ತ)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    ಅನ್ಯಥಾ
	match->cp_fname = ರಿಕ್ತ;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_index;

    ಯದಿ (cptext != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < CPT_ಎಣಿಕೆ; ++i)
	{
	    ಯದಿ (cptext[i] != ರಿಕ್ತ && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    ಯದಿ (user_data != ರಿಕ್ತ)
	match->cp_user_data = *user_data;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    ಯದಿ (compl_first_match == ರಿಕ್ತ)
	match->cp_next = match->cp_prev = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_next;
	prev = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	ಯಾವತ್ (current != ರಿಕ್ತ && current != compl_first_match)
	{
	    ಯದಿ (current->cp_score < score)
	    {
		match->cp_next = current;
		match->cp_prev = current->cp_prev;
		ಯದಿ (current->cp_prev)
		    current->cp_prev->cp_next = match;
		current->cp_prev = match;
		inserted = TRUE;
		ಭಙ್ಗ;
	    }
	    prev = current;
	    current = current->cp_next;
	}
	ಯದಿ (!inserted)
	{
	    prev->cp_next = match;
	    match->cp_prev = prev;
	    match->cp_next = compl_first_match;
	    compl_first_match->cp_prev = match;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (dir == FORWARD)
    {
	match->cp_next = compl_curr_match->cp_next;
	match->cp_prev = compl_curr_match;
    }
    ಅನ್ಯಥಾ	// BACKWARD
    {
	match->cp_next = compl_curr_match;
	match->cp_prev = compl_curr_match->cp_prev;
    }
    ಯದಿ (match->cp_next)
	match->cp_next->cp_prev = match;
    ಯದಿ (match->cp_prev)
	match->cp_prev->cp_next = match;
    ಅನ್ಯಥಾ	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    ಯದಿ (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.length,
								    str_hash);

    // Find the longest common string if still doing that.
    ಯದಿ (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_equal(compl_T *match, char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಯದಿ (match->cp_flags & CP_EQUAL)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (match->cp_flags & CP_ICASE)
	ಪ್ರತ್ಯಾಗಮ STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    ಪ್ರತ್ಯಾಗಮ STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_equal_sc(compl_T *match, char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ	typed = compl_length;
    ಪೂರ್ಣಾಙ್ಕ	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (ಪೂರ್ಣಾಙ್ಕ)(compl_longest_end_col - compl_col) : typed;

    ಯದಿ ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	ಪ್ರತ್ಯಾಗಮ ins_compl_equal(match, str, len);

    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)match->cp_str.length < len)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; ++i)
    {
	ಯದಿ (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_ಸೇರಿಸು_bytes(char_u *p, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಯದಿ (len == -1)
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_leader.string != ರಿಕ್ತ ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    ಸ್ಥಿರ size_t
ins_compl_leader_len(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_leader.string != ರಿಕ್ತ ? compl_leader.length : compl_orig_text.length;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_col_range_attr(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ	    start_col;
    ಪೂರ್ಣಾಙ್ಕ	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    ಪೂರ್ಣಾಙ್ಕ	    attr;

    ಯದಿ (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    start_col = compl_col + (ಪೂರ್ಣಾಙ್ಕ)ins_compl_leader_len();
    ಯದಿ (!ins_compl_has_multiple())
	ಪ್ರತ್ಯಾಗಮ (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    ಯದಿ ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	ಪ್ರತ್ಯಾಗಮ attr;

    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_has_multiple(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ vim_strchr(compl_shown_match->cp_str.string, '\n') != ರಿಕ್ತ;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_lnum_in_range(linenr_T lnum)
{
    ಯದಿ (!ins_compl_has_multiple())
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    ಪೂರ್ಣಾಙ್ಕ		c1, c2;
    ಪೂರ್ಣಾಙ್ಕ		had_match;

    ಯದಿ (compl_leader.string == ರಿಕ್ತ)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.length);
	ಯದಿ (compl_leader.string == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	compl_leader.length = match->cp_str.length;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ಸೇರಿಸು(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ಯದಿ (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	ಪ್ರತ್ಯಾಗಮ;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	ಅನ್ಯಥಾ
	{
	    c1 = *p;
	    c2 = *s;
	}
	ಯದಿ ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    ಭಙ್ಗ;
	ಯದಿ (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	ಅನ್ಯಥಾ
	{
	    ++p;
	    ++s;
	}
    }

    ಯದಿ (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.length = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ಸೇರಿಸು(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ಯದಿ (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_add_matches(
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches,
    char_u	**matches,
    ಪೂರ್ಣಾಙ್ಕ		icase)
{
    ಪೂರ್ಣಾಙ್ಕ		add_r = OK;
    ಪೂರ್ಣಾಙ್ಕ		dir = compl_direction;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ರಿಕ್ತ,
		FUZZY_SCORE_NONE);
	ಯದಿ (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(ಅಂಕೆ_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_make_cyclic(ಶೂನ್ಯ)
{
    compl_T *match;
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ = 0;

    ಯದಿ (compl_first_match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    ಯಾವತ್ (match->cp_next != ರಿಕ್ತ && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++ಎಣಿಕೆ;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

/*
 * Return whether there currently is a shown match.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_has_shown_match(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_shown_match == ರಿಕ್ತ
	|| compl_shown_match != compl_shown_match->cp_next;
}

/*
 * Return whether the shown match is long enough.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_long_shown_match(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)compl_shown_match->cp_str.length
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned ಪೂರ್ಣಾಙ್ಕ
get_cot_flags(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_upd_pum(ಶೂನ್ಯ)
{
    ಯದಿ (compl_match_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    ಯದಿ (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_del_pum(ಶೂನ್ಯ)
{
    ಯದಿ (compl_match_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    ಪೂರ್ಣಾಙ್ಕ
pum_wanted(ಶೂನ್ಯ)
{
    // 'completeopt' must contain "menu" or "menuone"
    ಪ್ರತ್ಯಾಗಮ (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pum_enough_matches(ಶೂನ್ಯ)
{
    compl_T     *compl;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    ಕುರು
    {
	ಯದಿ (compl == ರಿಕ್ತ || (!match_at_original_text(compl) && ++i == 2))
	    ಭಙ್ಗ;
	compl = compl->cp_next;
    } ಯಾವತ್ (!is_first_match(compl));

    ಯದಿ ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	ಪ್ರತ್ಯಾಗಮ (i >= 1);
    ಪ್ರತ್ಯಾಗಮ (i >= 2);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    ಸ್ಥಿರ dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    fill_complete_info_dict(dict, match, FALSE);
    ಪ್ರತ್ಯಾಗಮ dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_complete_changed_event(ಪೂರ್ಣಾಙ್ಕ cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    recursive = FALSE;
    save_v_event_T  save_v_event;

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_ವಸ್ತುಗಳು_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ*
cp_get_next(ಶೂನ್ಯ *ಗಂಟು)
{
    ಪ್ರತ್ಯಾಗಮ ((compl_T*)ಗಂಟು)->cp_next;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cp_set_next(ಶೂನ್ಯ *ಗಂಟು, ಶೂನ್ಯ *next)
{
    ((compl_T*)ಗಂಟು)->cp_next = (compl_T*)next;
}

    ಸ್ಥಿರ ಶೂನ್ಯ*
cp_get_prev(ಶೂನ್ಯ* ಗಂಟು)
{
    ಪ್ರತ್ಯಾಗಮ ((compl_T*)ಗಂಟು)->cp_prev;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cp_set_prev(ಶೂನ್ಯ* ಗಂಟು, ಶೂನ್ಯ* prev)
{
    ((compl_T*)ಗಂಟು)->cp_prev = (compl_T*)prev;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cp_compare_fuzzy(ಧ್ರುವ ಶೂನ್ಯ* a, ಧ್ರುವ ಶೂನ್ಯ* b)
{
    ಪೂರ್ಣಾಙ್ಕ score_a = ((compl_T*)a)->cp_score;
    ಪೂರ್ಣಾಙ್ಕ score_b = ((compl_T*)b)->cp_score;

    ಯದಿ (score_a != score_b)
	ಪ್ರತ್ಯಾಗಮ (score_b > score_a) ? 1 : -1;

    ಪ್ರತ್ಯಾಗಮ ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cp_compare_nearest(ಧ್ರುವ ಶೂನ್ಯ* a, ಧ್ರುವ ಶೂನ್ಯ* b)
{
    ಪೂರ್ಣಾಙ್ಕ score_a = ((compl_T*)a)->cp_score;
    ಪೂರ್ಣಾಙ್ಕ score_b = ((compl_T*)b)->cp_score;
    ಯದಿ (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prepend_startcol_text(string_T *dest, string_T *src, ಪೂರ್ಣಾಙ್ಕ startcol)
{
    ಪೂರ್ಣಾಙ್ಕ prepend_len = compl_col - startcol;
    ಪೂರ್ಣಾಙ್ಕ new_length = prepend_len + (ಪೂರ್ಣಾಙ್ಕ)src->length;

    dest->length = (size_t)new_length;
    dest->string = alloc(new_length + 1);  // +1 for NUL
    ಯದಿ (dest->string == ರಿಕ್ತ)
    {
	dest->length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    char_u	*ಸಾಲು = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, ಸಾಲು + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->length);
    dest->string[new_length] = NUL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    ಸ್ಥಿರ string_T *
get_leader_for_startcol(compl_T *match, ಪೂರ್ಣಾಙ್ಕ cached)
{
    ಸ್ಥಿರ string_T adjusted_leader = {ರಿಕ್ತ, 0};

    ಯದಿ (match == ರಿಕ್ತ)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (cpt_sources_array == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    ಪೂರ್ಣಾಙ್ಕ	cpt_idx = match->cp_cpt_source_idx;
    ಯದಿ (cpt_idx < 0)
	ಗಚ್ಛ theend;
    ಪೂರ್ಣಾಙ್ಕ	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    ಯದಿ (compl_leader.string == ರಿಕ್ತ)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	ಯದಿ (startcol < 0 || startcol >= compl_col)
	    ಪ್ರತ್ಯಾಗಮ &compl_orig_text;
	ಪ್ರತ್ಯಾಗಮ &compl_leader;  // pass through (startcol < compl_col)
    }

    ಯದಿ (compl_col <= 0)
	ಗಚ್ಛ theend;

    ಯದಿ (startcol >= 0 && startcol < compl_col)
    {
	ಪೂರ್ಣಾಙ್ಕ prepend_len = compl_col - startcol;
	ಪೂರ್ಣಾಙ್ಕ new_length = prepend_len + (ಪೂರ್ಣಾಙ್ಕ)compl_leader.length;
	ಯದಿ (cached && (size_t)new_length == adjusted_leader.length
		&& adjusted_leader.string != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	ಯದಿ (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    ಗಚ್ಛ theend;

	ಪ್ರತ್ಯಾಗಮ &adjusted_leader;
    }
theend:
    ಪ್ರತ್ಯಾಗಮ &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_fuzzy_score(ಶೂನ್ಯ)
{
    compl_T *compl;
    char_u  *pattern;
    ಪೂರ್ಣಾಙ್ಕ	    use_leader;

    ಯದಿ (compl_first_match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ರಿಕ್ತ && compl_leader.length > 0);
    ಯದಿ (!use_leader)
    {
	ಯದಿ (compl_orig_text.string == ರಿಕ್ತ || compl_orig_text.length == 0)
	    ಪ್ರತ್ಯಾಗಮ;
	pattern = compl_orig_text.string;
    }
    ಅನ್ಯಥಾ
    {
	/* Clear the leader cache once before the loop */
	(ಶೂನ್ಯ)get_leader_for_startcol(ರಿಕ್ತ, TRUE);
	pattern = ರಿಕ್ತ;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    ಪೂರ್ಣಾಙ್ಕ next = 0;
    compl = compl_first_match;
    ಕುರು
    {
	ಯದಿ (compl->cp_order < 0)
	    compl->cp_order = next;
	next = MAX(next, compl->cp_order + 1);

	ಯದಿ (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_next;
    } ಯಾವತ್ (compl != ರಿಕ್ತ && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sort_compl_match_list(ಪೂರ್ಣಾಙ್ಕ (*compare)(ಧ್ರುವ ಶೂನ್ಯ *, ಧ್ರುವ ಶೂನ್ಯ *))
{
    compl_T	*orig_text;

    ಯದಿ (!compl_first_match || is_first_match(compl_first_match->cp_next))
	ಪ್ರತ್ಯಾಗಮ;

    orig_text = find_original_text_match();
    ಯದಿ (orig_text == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ins_compl_make_linear();
    ಯದಿ (orig_text == compl_first_match)
    {
	compl_first_match->cp_next->cp_prev = ರಿಕ್ತ;
	compl_first_match->cp_next = mergesort_list(compl_first_match->cp_next,
		cp_get_next, cp_set_next, cp_get_prev, cp_set_prev, compare);
	compl_first_match->cp_next->cp_prev = compl_first_match;
    }
    ಅನ್ಯಥಾ
    {
	compl_T	*tail;

	orig_text->cp_prev->cp_next = ರಿಕ್ತ;
	compl_first_match = mergesort_list(compl_first_match, cp_get_next,
		cp_set_next, cp_get_prev, cp_set_prev, compare);
	tail = compl_first_match;
	ಯಾವತ್ (tail->cp_next != ರಿಕ್ತ)
	    tail = tail->cp_next;
	tail->cp_next = orig_text;
	orig_text->cp_prev = tail;
    }
    (ಶೂನ್ಯ)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_user_highlight_attr(ಪೂರ್ಣಾಙ್ಕ hl_id)
{
    ಪೂರ್ಣಾಙ್ಕ	    attr;

    ಯದಿ (hl_id <= 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    attr = syn_id2attr(hl_id);
    ಪ್ರತ್ಯಾಗಮ attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_build_pum(ಶೂನ್ಯ)
{
    compl_T     *compl;
    compl_T     *shown_compl = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		did_find_shown_match = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		shown_match_ok = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ಪೂರ್ಣಾಙ್ಕ		cur = -1;
    ಪೂರ್ಣಾಙ್ಕ		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_head = ರಿಕ್ತ;
    compl_T	*match_tail = ರಿಕ್ತ;
    compl_T	*match_next = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		*match_ಎಣಿಕೆ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		is_forward = compl_shows_dir_forward();
    ಪೂರ್ಣಾಙ್ಕ		is_cpt_completion = (cpt_sources_array != ರಿಕ್ತ);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    ಯದಿ (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    ಯದಿ (compl_leader.string != ರಿಕ್ತ
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_next;

    ಯದಿ (is_cpt_completion)
    {
	match_ಎಣಿಕೆ = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಙ್ಕ, cpt_sources_ಎಣಿಕೆ);
	ಯದಿ (match_ಎಣಿಕೆ == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }

    (ಶೂನ್ಯ)get_leader_for_startcol(ರಿಕ್ತ, TRUE); // Clear the cache

    compl = compl_first_match;
    ಕುರು
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	ಯದಿ (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	ಯದಿ (!match_at_original_text(compl)
		&& (leader->string == ರಿಕ್ತ
		    || ins_compl_equal_sc(compl, leader->string,
			(ಪೂರ್ಣಾಙ್ಕ)leader->length)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    ಪೂರ್ಣಾಙ್ಕ match_limit_exceeded = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ cur_source = compl->cp_cpt_source_idx;
	    ಯದಿ (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_ಎಣಿಕೆ[cur_source]++;
		ಪೂರ್ಣಾಙ್ಕ max_matches = cpt_sources_array[cur_source].cs_max_matches;
		ಯದಿ (max_matches > 0 && match_ಎಣಿಕೆ[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    ಯದಿ (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		ಯದಿ (match_head == ರಿಕ್ತ)
		    match_head = compl;
		ಅನ್ಯಥಾ
		    match_tail->cp_match_next = compl;
		match_tail = compl;

		ಯದಿ (!shown_match_ok && !cot_fuzzy())
		{
		    ಯದಿ (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    ಅನ್ಯಥಾ
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		ಅನ್ಯಥಾ ಯದಿ (cot_fuzzy())
		{
		    ಯದಿ (i == 0)
			shown_compl = compl;

		    ಯದಿ (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	ಯದಿ (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    ಯದಿ (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    ಯದಿ (!shown_match_ok && shown_compl != ರಿಕ್ತ)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } ಯಾವತ್ (compl != ರಿಕ್ತ && !is_first_match(compl));

    vim_free(match_ಎಣಿಕೆ);

    ಯದಿ (compl_match_arraysize == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    ಯದಿ (compl_match_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    compl = match_head;
    i = 0;
    ಯಾವತ್ (compl != ರಿಕ್ತ)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != ರಿಕ್ತ
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != ರಿಕ್ತ
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_next = compl->cp_match_next;
	compl->cp_match_next = ರಿಕ್ತ;
	compl = match_next;
    }

    ಯದಿ (!shown_match_ok)    // no displayed match at all
	cur = -1;

    ಪ್ರತ್ಯಾಗಮ cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    ಶೂನ್ಯ
ins_compl_show_pum(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		cur = -1;
    colnr_T	col;

    ಯದಿ (!pum_wanted() || !pum_enough_matches())
	ಪ್ರತ್ಯಾಗಮ;

    // Avoid redrawing the screen under a pum that stays in place.
    ಯದಿ (!pum_redraw_in_same_position())
	pum_call_update_screen();

    ಯದಿ (compl_match_array == ರಿಕ್ತ)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    ಅನ್ಯಥಾ
    {
	// popup menu already exists, only need to find the current item.
	ಪ್ರತಿ (i = 0; i < compl_match_arraysize; ++i)
	{
	    ಯದಿ (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (compl_match_array == ರಿಕ್ತ)
    {
#ifdef FEAT_EVAL
	ಯದಿ (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	ಪ್ರತ್ಯಾಗಮ;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    ಪೂರ್ಣಾಙ್ಕ pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    ಯದಿ (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    ಯದಿ (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		flags,		// DICT_FIRST and/or DICT_EXACT
    ಪೂರ್ಣಾಙ್ಕ		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		save_p_scs;
    ಪೂರ್ಣಾಙ್ಕ		dir = compl_direction;

    ಯದಿ (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	ಯದಿ (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	ಅನ್ಯಥಾ
#endif
	    ಪ್ರತ್ಯಾಗಮ;
    }

    buf = alloc(LSIZE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    regmatch.regprog = ರಿಕ್ತ;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ಯದಿ (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    ಯದಿ (ctrl_x_mode_ಸಾಲು_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	ಯದಿ (pat_esc == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	ಯದಿ (ptr == ರಿಕ್ತ)
	{
	    vim_free(pat_esc);
	    ಗಚ್ಛ theend;
	}
	vim_snprintf((ವರ್ಣ *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    ಅನ್ಯಥಾ
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    ಯಾವತ್ (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	ಯದಿ (flags == DICT_EXACT)
	{
	    ಎಣಿಕೆ = 1;
	    files = &dict;
	}
	ಅನ್ಯಥಾ
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    ಯದಿ (!thesaurus && STRCMP(buf, "spell") == 0)
		ಎಣಿಕೆ = -1;
	    ಅನ್ಯಥಾ
#endif
		ಯದಿ (vim_strchr(buf, '`') != ರಿಕ್ತ
		    || expand_wildcards(1, &buf, &ಎಣಿಕೆ, &files,
						     EW_FILE|EW_SILENT) != OK)
		ಎಣಿಕೆ = 0;
	}

#ifdef FEAT_SPELL
	ಯದಿ (ಎಣಿಕೆ == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    ಯದಿ (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    ಅನ್ಯಥಾ
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	ಅನ್ಯಥಾ
#endif
	    ಯದಿ (ಎಣಿಕೆ > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(ಎಣಿಕೆ, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    ಯದಿ (flags != DICT_EXACT)
		FreeWild(ಎಣಿಕೆ, files);
	}
	ಯದಿ (flags != 0)
	    ಭಙ್ಗ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
thesaurus_add_ಪದಗಳು_in_ಸಾಲು(
    char_u	*fname,
    char_u	**buf_arg,
    ಪೂರ್ಣಾಙ್ಕ		dir,
    char_u	*skip_word)
{
    ಪೂರ್ಣಾಙ್ಕ		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    ಯಾವತ್ (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	ಯದಿ (*ptr == NUL || *ptr == NL)
	    ಭಙ್ಗ;
	wstart = ptr;

	// Find end of the word.
	ಯದಿ (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    ಯಾವತ್ (*ptr != NUL)
	    {
		ಪೂರ್ಣಾಙ್ಕ l = (*mb_ptr2len)(ptr);

		ಯದಿ (l < 2 && !vim_iswordc(*ptr))
		    ಭಙ್ಗ;
		ptr += l;
	    }
	ಅನ್ಯಥಾ
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	ಯದಿ (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (ಪೂರ್ಣಾಙ್ಕ)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    ಯದಿ (status == FAIL)
		ಭಙ್ಗ;
	}
    }

    *buf_arg = ptr;
    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_files(
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    char_u	**files,
    ಪೂರ್ಣಾಙ್ಕ		thesaurus,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		*dir)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		i;
    FILE	*fp;
    ಪೂರ್ಣಾಙ್ಕ		add_r;
    char_u	*leader = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		leader_len = 0;
    ಪೂರ್ಣಾಙ್ಕ		score = 0;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    char_u	*ಸಾಲು_end = ರಿಕ್ತ;

    ಯದಿ (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (ಪೂರ್ಣಾಙ್ಕ)ins_compl_leader_len();
    }

    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((ವರ್ಣ *)files[i], "r");  // open dictionary file
	ಯದಿ (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (ವರ್ಣ *)files[i]);
	    (ಶೂನ್ಯ)ಸಂದೇಶ_trunc_attr((ವರ್ಣ *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	ಯದಿ (fp == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	// Read dictionary file line by line.
	// Check each line for a match.
	ಯಾವತ್ (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    ಯದಿ (cot_fuzzy() && leader_len > 0)
	    {
		ಸಾಲು_end = find_ಸಾಲು_end(ptr);
		ಯಾವತ್ (ptr < ಸಾಲು_end)
		{
		    ಯದಿ (fuzzy_match_str_in_ಸಾಲು(&ptr, leader, &len, ರಿಕ್ತ, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_ಸಾಲು_or_eval()
					? find_ಸಾಲು_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (ಪೂರ್ಣಾಙ್ಕ)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			ಯದಿ (add_r == FAIL)
			    ಭಙ್ಗ;
			ptr = end_ptr;  // start from next word
			ಯದಿ (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_next
				&& score == compl_first_match->cp_next->cp_score)
			    compl_ಅಂಕೆ_bests++;
		    }
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (regmatch != ರಿಕ್ತ)
	    {
		ಯಾವತ್ (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_ಸಾಲು_or_eval() ? find_ಸಾಲು_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (ಪೂರ್ಣಾಙ್ಕ)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    ಯದಿ (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_ಪದಗಳು_in_ಸಾಲು(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    ಯದಿ (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    ಅನ್ಯಥಾ ಯದಿ (add_r == FAIL)
			ಭಙ್ಗ;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    ಯದಿ (*ptr == '\n' || got_int)
			ಭಙ್ಗ;
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
find_word_start(char_u *ptr)
{
    ಯದಿ (has_mbyte)
    {
	ಯಾವತ್ (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    ಅನ್ಯಥಾ
    {
	ಯಾವತ್ (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    ಪ್ರತ್ಯಾಗಮ ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ start_class = mb_get_class(ptr);
	ಯದಿ (start_class > 1)
	    ಯಾವತ್ (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		ಯದಿ (mb_get_class(ptr) != start_class)
		    ಭಙ್ಗ;
	    }
    }
    ಅನ್ಯಥಾ
	ಯಾವತ್ (vim_iswordc(*ptr))
	    ++ptr;
    ಪ್ರತ್ಯಾಗಮ ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_ಸಾಲು_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    ಯಾವತ್ (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Free a completion item in the list
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    ಯದಿ (compl_strings_ht.ht_used > 0 && match->cp_str.string != ರಿಕ್ತ
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    ಯದಿ (--entry->cse_ಎಣಿಕೆ <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    ಯದಿ (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < CPT_ಎಣಿಕೆ; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_data);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_free(ಶೂನ್ಯ)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    ಯದಿ (compl_first_match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    ಕುರು
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	ins_compl_item_free(match);
    } ಯಾವತ್ (compl_curr_match != ರಿಕ್ತ && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = ರಿಕ್ತ;
    compl_shown_match = ರಿಕ್ತ;
    compl_old_match = ರಿಕ್ತ;
}

/*
 * Reset/clear the completion state.
 */
    ಶೂನ್ಯ
ins_compl_clear(ಶೂನ್ಯ)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = ರಿಕ್ತ;
    compl_curr_buf = ರಿಕ್ತ;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ರಿಕ್ತ;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ಅಂಕೆ_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_active(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_win_active(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ ins_compl_active() && wp == compl_curr_win
	&& wp->w_buffer == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_used_match(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_used_match;
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
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_interrupted(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_enter_selects(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_len(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_length;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_has_preinsert(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ cur_cot_flags = get_cot_flags();
    ಯದಿ (compl_autocomplete && p_ic && !p_inf)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_preinsert_effect(ಶೂನ್ಯ)
{
    ಯದಿ (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_bs(ಶೂನ್ಯ)
{
    char_u	*ಸಾಲು;
    char_u	*p;

    ಯದಿ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ಸಾಲು = ml_get_curline();
    p = ಸಾಲು + curwin->w_cursor.col;
    MB_PTR_BACK(ಸಾಲು, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು) - (ಪೂರ್ಣಾಙ್ಕ)compl_col < 0
	    || ((ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು) - (ಪೂರ್ಣಾಙ್ಕ)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು) - (ಪೂರ್ಣಾಙ್ಕ)compl_col
							- compl_length < 0))
	ಪ್ರತ್ಯಾಗಮ K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    ಯದಿ (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.length = (size_t)((p - ಸಾಲು) - compl_col);
    compl_leader.string = vim_strnsave(ಸಾಲು + compl_col, compl_leader.length);
    ಯದಿ (compl_leader.string == ರಿಕ್ತ)
    {
	compl_leader.length = 0;
	ಪ್ರತ್ಯಾಗಮ K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    ಯದಿ (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    ಯದಿ (compl_shown_match != ರಿಕ್ತ)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    ಪ್ರತ್ಯಾಗಮ NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_need_restart(ಶೂನ್ಯ)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    ಪ್ರತ್ಯಾಗಮ compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_has_autocomplete(ಶೂನ್ಯ)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_fuzzy_sort(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    ಯದಿ (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	ಯದಿ ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    ಪೂರ್ಣಾಙ್ಕ none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_prev);
	    ಯದಿ (!none_selected)
		compl_shown_match
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_first_match->cp_next : compl_first_match;
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
    ಪೂರ್ಣಾಙ್ಕ	    save_w_wrow = curwin->w_wrow;
    ಪೂರ್ಣಾಙ್ಕ	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_ಸೇರಿಸು_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    ಯದಿ (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.length);
	ಯದಿ (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	ಯದಿ (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    ಅನ್ಯಥಾ
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	ಯದಿ (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	ಅನ್ಯಥಾ
	    compl_autocomplete = FALSE;
	ಯದಿ (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    ಯದಿ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    ಯದಿ (compl_match_array == ರಿಕ್ತ)
	compl_enter_selects = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (ins_compl_has_preinsert() && compl_leader.length > 0)
	ins_compl_ಸೇರಿಸು(TRUE, FALSE);
    ಅನ್ಯಥಾ ಯದಿ (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.length > 0 && !ins_compl_preinsert_effect())
	ins_compl_ಸೇರಿಸು(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_compl_len(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ off = (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col - (ಪೂರ್ಣಾಙ್ಕ)compl_col;
    ಪ್ರತ್ಯಾಗಮ MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ಶೂನ್ಯ
ins_compl_addleader(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    cc;

    ಯದಿ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ಯದಿ (stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	ಯದಿ (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    ಅನ್ಯಥಾ
    {
	ins_char(c);
	ಯದಿ (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    ಯದಿ (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    ಯದಿ (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.length = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.length);
	ಯದಿ (compl_leader.string == ರಿಕ್ತ)
	{
	    compl_leader.length = 0;
	    ಪ್ರತ್ಯಾಗಮ;
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
    compl_cont_status = 0;
    compl_cont_mode = 0;
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
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    ಯದಿ (match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    p = vim_strnsave(str, len);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.length = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ಶೂನ್ಯ
ins_compl_addfrommatch(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col - (ಪೂರ್ಣಾಙ್ಕ)compl_col;
    ಪೂರ್ಣಾಙ್ಕ		c;

    p = compl_shown_match->cp_str.string;
    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)compl_shown_match->cp_str.length <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	ಯದಿ (!match_at_original_text(compl_shown_match))
	    ಪ್ರತ್ಯಾಗಮ;

	p = ರಿಕ್ತ;
	plen = 0;
	ಪ್ರತಿ (cp = compl_shown_match->cp_next; cp != ರಿಕ್ತ
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    ಯದಿ (compl_leader.string == ರಿಕ್ತ
		    || ins_compl_equal(cp, compl_leader.string,
			(ಪೂರ್ಣಾಙ್ಕ)compl_leader.length))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.length;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (p == ರಿಕ್ತ || (ಪೂರ್ಣಾಙ್ಕ)plen <= len)
	    ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ctrl_x_mode(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    retval = FALSE;

    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ Ctrl_E:
	ಪಕ್ಷ Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    ಯದಿ (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    ಅನ್ಯಥಾ
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ರಿಕ್ತ;
	    showmode();
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_ಸಾಲು;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    ಯದಿ (vpeekc() == '=')
		ಭಙ್ಗ;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    ಭಙ್ಗ;
#ifdef FEAT_COMPL_FUNC
	ಪಕ್ಷ Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ 's':
	ಪಕ್ಷ Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    ಭಙ್ಗ;
#ifdef FEAT_FIND_ID
	ಪಕ್ಷ Ctrl_I:
	ಪಕ್ಷ K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ Ctrl_V:
	ಪಕ್ಷ Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ರಿಕ್ತ;
	    showmode();
	    retval = TRUE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ Ctrl_P:
	ಪಕ್ಷ Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    ಯದಿ (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    ಅನ್ಯಥಾ ಯದಿ (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	ಸ್ವಭಾವ:
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
	    ಯದಿ (c == Ctrl_X)
	    {
		ಯದಿ (compl_cont_mode != 0)
		    compl_cont_status = 0;
		ಅನ್ಯಥಾ
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ರಿಕ್ತ;
	    showmode();
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_complete_done_event(ಪೂರ್ಣಾಙ್ಕ mode UNUSED, string_T *word UNUSED)
{
#ಯದಿ defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    ಯದಿ (word == ರಿಕ್ತ || word->string == ರಿಕ್ತ)
	(ಶೂನ್ಯ)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)dict_add_string_len(v_event, "complete_word", word->string, (ಪೂರ್ಣಾಙ್ಕ)word->length);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (ಶೂನ್ಯ)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == ರಿಕ್ತ) ? (char_u *)"" : mode_str->string, (ಪೂರ್ಣಾಙ್ಕ)mode_str->length);

    dict_set_ವಸ್ತುಗಳು_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#ಯದಿ defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_stop(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ prev_mode, ಪೂರ್ಣಾಙ್ಕ retval)
{
    ಪೂರ್ಣಾಙ್ಕ		want_cindent;
    string_T	word = {ರಿಕ್ತ, 0};

    // Remove pre-inserted text when present.
    ಯದಿ (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    ಯದಿ (compl_curr_match != ರಿಕ್ತ || compl_leader.string != ರಿಕ್ತ || c == Ctrl_E)
    {
	char_u	*ptr = ರಿಕ್ತ;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	ಯದಿ (compl_curr_match != ರಿಕ್ತ && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    ಯದಿ (compl_cont_mode == CTRL_X_WHOLE_ಸಾಲು)
    {
	// re-indent the current line
	ಯದಿ (want_cindent)
	{
	    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (!compl_autocomplete || compl_used_match)
    {
	ಪೂರ್ಣಾಙ್ಕ prev_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	ಯದಿ (prev_col > 0)
	    dec_cursor();
	// only format when something was inserted
	ಯದಿ (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	ಯದಿ (prev_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    ಯದಿ ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.length);
	ಯದಿ (word.string == ರಿಕ್ತ)
	    word.length = 0;
	ಅನ್ಯಥಾ
	    word.length = compl_shown_match->cp_str.length;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    ಯದಿ (c == Ctrl_E)
    {
	char_u *p = ರಿಕ್ತ;
	size_t	plen = 0;

	ins_compl_delete();
	ಯದಿ (compl_leader.string != ರಿಕ್ತ)
	{
	    p = compl_leader.string;
	    plen = compl_leader.length;
	}
	ಅನ್ಯಥಾ ಯದಿ (compl_first_match != ರಿಕ್ತ)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.length;
	}
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    compl_len = get_compl_len();

	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)plen > compl_len)
		ins_compl_ಸೇರಿಸು_bytes(p + compl_len, (ಪೂರ್ಣಾಙ್ಕ)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_mode = prev_mode;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    ಯದಿ (!shortmess(SHM_COMPLETIONMENU))
	ಸಂದೇಶ_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    ಯದಿ (edit_submode != ರಿಕ್ತ)
    {
	edit_submode = ರಿಕ್ತ;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ಅಂಕೆ_bests = 0;
    compl_ins_end_col = 0;

    ಯದಿ (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    ಯದಿ (want_cindent && in_cinkeys(ಕೀಲಿ_COMPLETE, ' ', inindent(0)))
	do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(prev_mode, &word);
    vim_free(word.string);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Cancel completion.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_cancel(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_compl_prep(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    prev_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    ಯದಿ (c != Ctrl_R && vim_is_ctrl_x_ಕೀಲಿ(c))
	edit_submode_extra = ರಿಕ್ತ;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    ಯದಿ (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	ಪ್ರತ್ಯಾಗಮ retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    ಯದಿ (is_mouse_ಕೀಲಿ(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	ಯದಿ (c == K_LEFTRELEASE
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
	    ಪ್ರತ್ಯಾಗಮ retval;
	ಯದಿ (popup_visible)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    row = mouse_row;
	    ಪೂರ್ಣಾಙ್ಕ	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    ಯದಿ (wp != ರಿಕ್ತ && WIN_IS_POPUP(wp))
		ಪ್ರತ್ಯಾಗಮ retval;
	}
    }
#endif

    ಯದಿ (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	ಯದಿ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_ಕೀಲಿ(c)
		|| !vim_is_ctrl_x_ಕೀಲಿ(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    ಯದಿ (c == Ctrl_Z)
		retval = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    ಯದಿ (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    ಯದಿ (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	ಯದಿ (c != Ctrl_R && !vim_is_ctrl_x_ಕೀಲಿ(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = ರಿಕ್ತ;
	}
	showmode();
    }

    ಯದಿ (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	ಯದಿ ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_ಕೀಲಿ(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, prev_mode, retval);
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode == CTRL_X_LOCAL_ಸಂದೇಶ)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, ರಿಕ್ತ);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    ಯದಿ (c != Ctrl_R && !vim_is_ctrl_x_ಕೀಲಿ(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    ಪೂರ್ಣಾಙ್ಕ	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    ಯದಿ (ptr == ರಿಕ್ತ)
    {
	ಯದಿ (compl_leader.string != ರಿಕ್ತ)
	    ptr = compl_leader.string;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ;  // nothing to do
    }
    ಯದಿ (compl_orig_text.string != ರಿಕ್ತ)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	ಯಾವತ್ (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	ಯದಿ (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	ಪ್ರತಿ (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    ಯದಿ (ptr != ರಿಕ್ತ)
	AppendToRedobuffLit(ptr + len, -1);
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
ins_compl_next_buf(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಸ್ಥಿರ win_T    *wp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    skip_buffer;

    ಯದಿ (flag == 'w')		// just windows
    {
	ಯದಿ (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	ಯಾವತ್ (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_next != ರಿಕ್ತ) ? wp->w_next : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    ಯದಿ (wp == curwin || !wp->w_buffer->b_scanned)
		ಭಙ್ಗ;
	}
	buf = wp->w_buffer;
    }
    ಅನ್ಯಥಾ
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	ಯಾವತ್ (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_next != ರಿಕ್ತ) ? buf->b_next : firstbuf;
	    // Break if we're back at start buffer
	    ಯದಿ (buf == curbuf)
		ಭಙ್ಗ;

	    // Check buffer conditions based on flag
	    ಯದಿ (flag == 'U')
		skip_buffer = buf->b_p_bl;
	    ಅನ್ಯಥಾ
		skip_buffer = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ರಿಕ್ತ) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    ಯದಿ (!skip_buffer && !buf->b_scanned)
		ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_cpt_sources_ಎಣಿಕೆ(char_u *cpt)
{
    char_u  dummy[LSIZE];
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ = 0;
    char_u  *p;

    ಪ್ರತಿ (p = cpt; *p != NUL; )
    {
	ಯಾವತ್ (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	ಯದಿ (*p != NUL)
	{
	    (ಶೂನ್ಯ)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    ಎಣಿಕೆ++;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
ಸ್ಥಿರ callback_T cfu_cb;	    // 'completefunc' callback function
ಸ್ಥಿರ callback_T ofu_cb;	    // 'omnifunc' callback function
ಸ್ಥಿರ callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
ಸ್ಥಿರ callback_T *cpt_cb;	    // Callback functions associated with F{func}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	  cpt_cb_ಎಣಿಕೆ;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    ಯದಿ (globcb->cb_name != ರಿಕ್ತ && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ವರ್ಣ *
did_set_completefunc(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ	retval;

    ಯದಿ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    ಅನ್ಯಥಾ
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	ಯದಿ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    ಪ್ರತ್ಯಾಗಮ retval == FAIL ? e_invalid_argument : ರಿಕ್ತ;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    ಶೂನ್ಯ
set_buflocal_cfu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&cfu_cb, &buf->b_cfu_cb);
# endif
}

/*
 * Parse the 'omnifunc' option value and set the callback function.
 * Invoked when the 'omnifunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ವರ್ಣ *
did_set_omnifunc(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ	retval;

    ಯದಿ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    ಅನ್ಯಥಾ
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	ಯದಿ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    ಪ್ರತ್ಯಾಗಮ retval == FAIL ? e_invalid_argument : ರಿಕ್ತ;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    ಶೂನ್ಯ
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    ಶೂನ್ಯ
clear_cpt_callbacks(callback_T **callbacks, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಯದಿ (callbacks == ರಿಕ್ತ || *callbacks == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_cpt_callbacks(callback_T **dest, ಪೂರ್ಣಾಙ್ಕ *dest_cnt, callback_T *src, ಪೂರ್ಣಾಙ್ಕ cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    ಯದಿ (cnt == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    ಯದಿ (*dest == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    *dest_cnt = cnt;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cnt; i++)
	ಯದಿ (src[i].cb_name != ರಿಕ್ತ && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    ಶೂನ್ಯ
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, ಪೂರ್ಣಾಙ್ಕ *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    idx = 0;
    ಪೂರ್ಣಾಙ್ಕ	    slen;
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    ಎಣಿಕೆ = get_cpt_sources_ಎಣಿಕೆ(cpt);
    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, ಎಣಿಕೆ);
    ಯದಿ (*cbp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *cnt = ಎಣಿಕೆ;

    ಪ್ರತಿ (p = cpt; *p != NUL; )
    {
	ಯಾವತ್ (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	ಯದಿ (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ಯದಿ (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		ಯದಿ (caret != ರಿಕ್ತ)
		    *caret = NUL;

		ಯದಿ (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ರಿಕ್ತ;
	    }
	    idx++;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_cpt_callbacks(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ	    opt_flags = args->os_flags;

    ಯದಿ (curbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // ":setglobal" does not change the buffer-local option value
    ಯದಿ (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_ಎಣಿಕೆ) != OK)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // ":setlocal" does not change the global option value
    ಯದಿ (opt_flags & OPT_LOCAL)
	ಪ್ರತ್ಯಾಗಮ OK;

    // Cache the callbacks for the global value
    ಯದಿ (opt_flags & OPT_GLOBAL)
	ಪ್ರತ್ಯಾಗಮ parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_ಎಣಿಕೆ);

    // set case
    ಪ್ರತ್ಯಾಗಮ copy_cpt_callbacks(&cpt_cb, &cpt_cb_ಎಣಿಕೆ, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_ಎಣಿಕೆ);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ವರ್ಣ *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	retval;

    ಯದಿ (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    ಅನ್ಯಥಾ
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	ಯದಿ (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    ಪ್ರತ್ಯಾಗಮ retval == FAIL ? e_invalid_argument : ರಿಕ್ತ;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_cpt_callbacks(callback_T *callbacks, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ	abort = FALSE;

    ಯದಿ (callbacks == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_insexpand_funcs(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_ಎಣಿಕೆ, copyID);

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    ಸ್ಥಿರ char_u *
get_complete_funcname(ಪೂರ್ಣಾಙ್ಕ type)
{
    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ CTRL_X_FUNCTION:
	    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_cfu;
	ಪಕ್ಷ CTRL_X_OMNI:
	    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ofu;
	ಪಕ್ಷ CTRL_X_THESAURUS:
	    ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    ಸ್ಥಿರ callback_T *
get_ಸೇರಿಸು_callback(ಪೂರ್ಣಾಙ್ಕ type)
{
    ಯದಿ (type == CTRL_X_FUNCTION)
	ಪ್ರತ್ಯಾಗಮ &curbuf->b_cfu_cb;
    ಯದಿ (type == CTRL_X_OMNI)
	ಪ್ರತ್ಯಾಗಮ &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    ಪ್ರತ್ಯಾಗಮ (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
expand_by_function(ಪೂರ್ಣಾಙ್ಕ type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = ರಿಕ್ತ;
    dict_T	*matchdict = ರಿಕ್ತ;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		save_State = State;
    ಪೂರ್ಣಾಙ್ಕ		retval;
    ಪೂರ್ಣಾಙ್ಕ		is_cpt_function = (cb != ರಿಕ್ತ);
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ಯದಿ (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	ಯದಿ (*funcname == NUL)
	    ಪ್ರತ್ಯಾಗಮ;
	cb = get_ಸೇರಿಸು_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_ಸಂಖ್ಯೆ;
    args[0].vval.v_ಸಂಖ್ಯೆ = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != ರಿಕ್ತ ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    ಯದಿ (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    ಯದಿ (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    ಯದಿ (retval == OK)
    {
	ವಿಕಲ್ಪ (rettv.v_type)
	{
	    ಪಕ್ಷ VAR_LIST:
		matchlist = rettv.vval.v_list;
		ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_DICT:
		matchdict = rettv.vval.v_dict;
		ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_SPECIAL:
		ಯದಿ (rettv.vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    ಸ್ವಭಾವ:
		// TODO: Give error message?
		clear_tv(&rettv);
		ಭಙ್ಗ;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ಯದಿ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	ಗಚ್ಛ theend;
    }

    ಯದಿ (matchlist != ರಿಕ್ತ)
	ins_compl_add_list(matchlist);
    ಅನ್ಯಥಾ ಯದಿ (matchdict != ರಿಕ್ತ)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    ಯದಿ (matchdict != ರಿಕ್ತ)
	dict_unref(matchdict);
    ಯದಿ (matchlist != ರಿಕ್ತ)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#ಯದಿ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
get_user_highlight_id(char_u *hlname)
{
    ಯದಿ (hlname != ರಿಕ್ತ && *hlname != NUL)
	ಪ್ರತ್ಯಾಗಮ syn_check_group(hlname, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(hlname));
    ಪ್ರತ್ಯಾಗಮ 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_add_tv(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ dir, ಪೂರ್ಣಾಙ್ಕ fast)
{
    char_u	*word;
    ಪೂರ್ಣಾಙ್ಕ		dup = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		empty = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_ಎಣಿಕೆ]);
    typval_T	user_data;
    ಪೂರ್ಣಾಙ್ಕ		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    ಪೂರ್ಣಾಙ್ಕ		user_hl[2] = { 0, 0 };

    user_data.v_type = VAR_UNKNOWN;
    ಯದಿ (tv->v_type == VAR_DICT && tv->vval.v_dict != ರಿಕ್ತ)
    {
	word = dict_get_string(tv->vval.v_dict, "word", FALSE);
	cptext[CPT_ABBR] = dict_get_string(tv->vval.v_dict, "abbr", FALSE);
	cptext[CPT_MENU] = dict_get_string(tv->vval.v_dict, "menu", FALSE);
	cptext[CPT_KIND] = dict_get_string(tv->vval.v_dict, "kind", FALSE);
	cptext[CPT_INFO] = dict_get_string(tv->vval.v_dict, "info", FALSE);

	user_abbr_hlname = dict_get_string(tv->vval.v_dict, "abbr_hlgroup", FALSE);
	user_hl[0] = get_user_highlight_id(user_abbr_hlname);

	user_kind_hlname = dict_get_string(tv->vval.v_dict, "kind_hlgroup", FALSE);
	user_hl[1] = get_user_highlight_id(user_kind_hlname);

	dict_get_tv(tv->vval.v_dict, "user_data", &user_data);
	ಯದಿ (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ರಿಕ್ತ
				  && dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	ಯದಿ (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ರಿಕ್ತ)
	    dup = dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "dup");
	ಯದಿ (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ರಿಕ್ತ)
	    empty = dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "empty");
	ಯದಿ (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ರಿಕ್ತ
				  && dict_get_ಸಂಖ್ಯೆ(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    ಅನ್ಯಥಾ
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    ಯದಿ (word == ರಿಕ್ತ || (!empty && *word == NUL))
    {
	clear_tv(&user_data);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    status = ins_compl_add(word, -1, ರಿಕ್ತ, cptext,
	    &user_data, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    ಯದಿ (status != OK)
	clear_tv(&user_data);
    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Add completions from a list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
    {
	ಯದಿ (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	ಅನ್ಯಥಾ ಯದಿ (did_emsg)
	    ಭಙ್ಗ;
    }
}

/*
 * Add completions from a dict.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_ಪದಗಳು;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    ಯದಿ (di_refresh != ರಿಕ್ತ && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	ಯದಿ (v != ರಿಕ್ತ && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_ಪದಗಳು = dict_find(dict, (char_u *)"words", 5);
    ಯದಿ (di_ಪದಗಳು != ರಿಕ್ತ && di_ಪದಗಳು->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_ಪದಗಳು->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_completion(colnr_T startcol, list_T *list)
{
    ಪೂರ್ಣಾಙ್ಕ save_w_wrow = curwin->w_wrow;
    ಪೂರ್ಣಾಙ್ಕ save_w_leftcol = curwin->w_leftcol;
    ಪೂರ್ಣಾಙ್ಕ flags = CP_ORIGINAL_TEXT;
    unsigned ಪೂರ್ಣಾಙ್ಕ cur_cot_flags = get_cot_flags();
    ಪೂರ್ಣಾಙ್ಕ compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    ಪೂರ್ಣಾಙ್ಕ compl_no_ಸೇರಿಸು = (cur_cot_flags & COT_NOINSERT) != 0;
    ಪೂರ್ಣಾಙ್ಕ compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    ಯದಿ (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    ಯದಿ (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_length = (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col - (ಪೂರ್ಣಾಙ್ಕ)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_length);
    ಯದಿ (p_ic)
	flags |= CP_ICASE;
    ಯದಿ (compl_orig_text.string == ರಿಕ್ತ)
    {
	compl_orig_text.length = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }
    compl_orig_text.length = (size_t)compl_length;
    ಯದಿ (ins_compl_add(compl_orig_text.string,
			(ಪೂರ್ಣಾಙ್ಕ)compl_orig_text.length, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, 0,
			flags | CP_FAST, FALSE, ರಿಕ್ತ, FUZZY_SCORE_NONE) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    ಪೂರ್ಣಾಙ್ಕ no_select = compl_no_select || compl_longest;
    ಯದಿ (compl_no_ಸೇರಿಸು || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	ಯದಿ (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    ಅನ್ಯಥಾ
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_ಸೇರಿಸು;

    // Lazily show the popup menu, unless we got interrupted.
    ಯದಿ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    ಶೂನ್ಯ
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((State & MODE_ಸೇರಿಸು) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_ಸೇರಿಸು_mode));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    ಯದಿ (!undo_allowed())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	ಪೂರ್ಣಾಙ್ಕ startcol = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ);
	ಯದಿ (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    ಶೂನ್ಯ
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    ಶೂನ್ಯ
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_ಕೀಲಿಗಳು(0, TRUE);
    rettv->vval.v_ಸಂಖ್ಯೆ = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_mode(string_T *ret)
{
    ಯದಿ (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == ರಿಕ್ತ) ? (char_u *)"" : mode->string;
	ret->length = mode->length;
    }
    ಅನ್ಯಥಾ
    {
	ret->string = (char_u *)"";
	ret->length = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_update_sequence_numbers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		ಸಂಖ್ಯೆ = 0;
    compl_T	*match;

    ಯದಿ (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	ಪ್ರತಿ (match = compl_curr_match->cp_prev; match != ರಿಕ್ತ
		&& !is_first_match(match); match = match->cp_prev)
	    ಯದಿ (match->cp_ಸಂಖ್ಯೆ != -1)
	    {
		ಸಂಖ್ಯೆ = match->cp_ಸಂಖ್ಯೆ;
		ಭಙ್ಗ;
	    }
	ಯದಿ (match != ರಿಕ್ತ)
	    // go up and assign all numbers which are not assigned yet
	    ಪ್ರತಿ (match = match->cp_next;
		    match != ರಿಕ್ತ && match->cp_ಸಂಖ್ಯೆ == -1;
					   match = match->cp_next)
		match->cp_ಸಂಖ್ಯೆ = ++ಸಂಖ್ಯೆ;
    }
    ಅನ್ಯಥಾ // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	ಪ್ರತಿ (match = compl_curr_match->cp_next; match != ರಿಕ್ತ
		&& !is_first_match(match); match = match->cp_next)
	{
	    ಯದಿ (match->cp_ಸಂಖ್ಯೆ != -1)
	    {
		ಸಂಖ್ಯೆ = match->cp_ಸಂಖ್ಯೆ;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (match != ರಿಕ್ತ)
	{
	    // go down and assign all numbers which are not assigned yet
	    ಪ್ರತಿ (match = match->cp_prev; match
		    && match->cp_ಸಂಖ್ಯೆ == -1;
					   match = match->cp_prev)
		match->cp_ಸಂಖ್ಯೆ = ++ಸಂಖ್ಯೆ;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_complete_info_dict(dict_T *di, compl_T *match, ಪೂರ್ಣಾಙ್ಕ add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (ಪೂರ್ಣಾಙ್ಕ)match->cp_str.length);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    ಯದಿ (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    ಯದಿ (match->cp_user_data.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    ಅನ್ಯಥಾ
	dict_add_tv(di, "user_data", &match->cp_user_data);
}

/*
 * Get complete information
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_complete_info(list_T *what_list, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_ವಸ್ತುಗಳು		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    ಪೂರ್ಣಾಙ್ಕ		what_flag;

    ಯದಿ (what_list == ರಿಕ್ತ)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    ಅನ್ಯಥಾ
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    ಯದಿ (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ವಸ್ತುಗಳು;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    ಯದಿ (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (ಪೂರ್ಣಾಙ್ಕ)mode.length);
    }

    ಯದಿ (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_ಸಂಖ್ಯೆ(retdict, "pum_visible", pum_visible());

    ಯದಿ (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_ಸಂಖ್ಯೆ(retdict, "auto", compl_autostarted);

    ಯದಿ (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*ಸಾಲು = ml_get_curline();
	ಪೂರ್ಣಾಙ್ಕ	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? ಸಾಲು + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    ಯದಿ (ret == OK && (what_flag & (CI_WHAT_ವಸ್ತುಗಳು | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ರಿಕ್ತ;
	dict_T	    *di;
	compl_T     *match;
	ಪೂರ್ಣಾಙ್ಕ         selected_idx = -1;
	ಪೂರ್ಣಾಙ್ಕ	    has_ವಸ್ತುಗಳು = what_flag & CI_WHAT_ವಸ್ತುಗಳು;
	ಪೂರ್ಣಾಙ್ಕ	    has_matches = what_flag & CI_WHAT_MATCHES;
	ಪೂರ್ಣಾಙ್ಕ	    has_completed = what_flag & CI_WHAT_COMPLETED;

	ಯದಿ (has_ವಸ್ತುಗಳು || has_matches)
	{
	    li = list_alloc();
	    ಯದಿ (li == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    ret = dict_add_list(retdict, (has_matches && !has_ವಸ್ತುಗಳು)
						? "matches" : "items", li);
	    ಯದಿ (ret == FAIL)
		list_unref(li);
	}
	ಯದಿ (ret == OK && what_flag & CI_WHAT_SELECTED)
	    ಯದಿ (compl_curr_match != ರಿಕ್ತ && compl_curr_match->cp_ಸಂಖ್ಯೆ == -1)
		ins_compl_update_sequence_numbers();
	ಯದಿ (ret == OK && compl_first_match != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ list_idx = 0;
	    match = compl_first_match;
	    ಕುರು
	    {
		ಯದಿ (!match_at_original_text(match))
		{
		    ಯದಿ (has_ವಸ್ತುಗಳು
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			ಯದಿ (di == ರಿಕ್ತ)
			    ಪ್ರತ್ಯಾಗಮ;
			ret = list_append_dict(li, di);
			ಯದಿ (ret != OK)
			{
			    dict_unref(di);
			    ಪ್ರತ್ಯಾಗಮ;
			}
			fill_complete_info_dict(di, match, has_matches && has_ವಸ್ತುಗಳು);
		    }
		    ಯದಿ (compl_curr_match != ರಿಕ್ತ
			    && compl_curr_match->cp_ಸಂಖ್ಯೆ == match->cp_ಸಂಖ್ಯೆ)
			selected_idx = list_idx;
		    ಯದಿ (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_next;
	    }
	    ಯಾವತ್ (match != ರಿಕ್ತ && !is_first_match(match));
	}
	ಯದಿ (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_ಸಂಖ್ಯೆ(retdict, "selected", selected_idx);

	ಯದಿ (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    ಯದಿ (di == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    ಯದಿ (ret == FAIL)
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
    list_T	*what_list = ರಿಕ್ತ;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
thesaurus_func_complete(ಪೂರ್ಣಾಙ್ಕ type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    ಪ್ರತ್ಯಾಗಮ type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
may_advance_cpt_index(char_u *cpt)
{
    char_u  *p = cpt;

    ಯದಿ (cpt_sources_index == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯಾವತ್ (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    ಪ್ರತ್ಯಾಗಮ (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
ಗಣನಾ
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	prev_match_pos;		// previous match position
    ಪೂರ್ಣಾಙ್ಕ		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    ಪೂರ್ಣಾಙ್ಕ		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    ಪೂರ್ಣಾಙ್ಕ		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_next_state_T;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
process_next_cpt_ಮೌಲ್ಯ(
	ins_compl_next_state_T *st,
	ಪೂರ್ಣಾಙ್ಕ		*compl_type_arg,
	pos_T		*start_match_pos,
	ಪೂರ್ಣಾಙ್ಕ		fuzzy_collect,
	ಪೂರ್ಣಾಙ್ಕ		*advance_cpt_idx)
{
    ಪೂರ್ಣಾಙ್ಕ	    compl_type = -1;
    ಪೂರ್ಣಾಙ್ಕ	    status = INS_COMPL_CPT_OK;
    ಪೂರ್ಣಾಙ್ಕ	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    ಯಾವತ್ (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    ಯದಿ (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	ಯದಿ (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ರಿಕ್ತ
	    && (st->ins_buf = ins_compl_next_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	ಯದಿ (st->ins_buf->b_ml.ml_mfp != ರಿಕ್ತ)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	ಅನ್ಯಥಾ	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    ಯದಿ (st->ins_buf->b_fname == ರಿಕ್ತ)
	    {
		status = INS_COMPL_CPT_CONT;
		ಗಚ್ಛ done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	ಯದಿ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ರಿಕ್ತ
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ರಿಕ್ತ
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (ಶೂನ್ಯ)ಸಂದೇಶ_trunc_attr((ವರ್ಣ *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (ctrl_x_mode_ಸಾಲು_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	ಅನ್ಯಥಾ ಯದಿ (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_index);
	    ಯದಿ (!st->func_cb)
		compl_type = -1;
	}
#endif
	ಅನ್ಯಥಾ ಯದಿ (!skip_source)
	{
	    ಯದಿ (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		ಯದಿ (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		ಅನ್ಯಥಾ
		    compl_type = CTRL_X_THESAURUS;
		ಯದಿ (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    ಅನ್ಯಥಾ ಯದಿ (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    ಅನ್ಯಥಾ ಯದಿ (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    ಅನ್ಯಥಾ ಯದಿ (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		ಯದಿ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    ಸಂದೇಶ_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Scanning tags."));
		    (ಶೂನ್ಯ)ಸಂದೇಶ_trunc_attr((ವರ್ಣ *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    ಅನ್ಯಥಾ
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(ಶೂನ್ಯ)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_index(st->e_cpt);

	st->found_all = TRUE;
	ಯದಿ (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    ಪ್ರತ್ಯಾಗಮ status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_next_include_file_completion(ಪೂರ್ಣಾಙ್ಕ compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (ಪೂರ್ಣಾಙ್ಕ)compl_pattern.length, FALSE, FALSE,
	    (compl_type == CTRL_X_PATH_DEFINES
	     && !(compl_cont_status & CONT_SOL))
	    ? FIND_DEFINE : FIND_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM, FALSE, compl_autocomplete);
}
#endif

/*
 * Get the next set of words matching "compl_pattern" in dictionary or
 * thesaurus files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_next_dict_tsr_completion(ಪೂರ್ಣಾಙ್ಕ compl_type, char_u *dict, ಪೂರ್ಣಾಙ್ಕ dict_f)
{
#ifdef FEAT_COMPL_FUNC
    ಯದಿ (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
#endif
	ins_compl_dictionaries(
		dict != ರಿಕ್ತ ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != ರಿಕ್ತ ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_next_tag_completion(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		save_p_ic;
    char_u	**matches;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    ಯದಿ (find_tags(compl_pattern.string, &ಅಂಕೆ_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && ಅಂಕೆ_matches > 0)
	ins_compl_add_matches(ಅಂಕೆ_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compare_scores(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    ಪೂರ್ಣಾಙ್ಕ idx_a = *(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *)a;
    ಪೂರ್ಣಾಙ್ಕ idx_b = *(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *)b;
    ಪೂರ್ಣಾಙ್ಕ score_a = compl_fuzzy_scores[idx_a];
    ಪೂರ್ಣಾಙ್ಕ score_b = compl_fuzzy_scores[idx_b];
    ಪ್ರತ್ಯಾಗಮ score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_longest_ಸೇರಿಸು(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_ಸೇರಿಸು_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fuzzy_longest_match(ಶೂನ್ಯ)
{
    char_u	*prefix = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		prefix_len = 0;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ಪೂರ್ಣಾಙ್ಕ		j = 0;
    char_u	*match_str = ರಿಕ್ತ;
    char_u	*prefix_ptr = ರಿಕ್ತ;
    char_u	*match_ptr = ರಿಕ್ತ;
    char_u	*leader = ರಿಕ್ತ;
    size_t	leader_len = 0;
    compl_T	*compl = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		more_candidates = FALSE;
    compl_T	*nn_compl = ರಿಕ್ತ;

    ಯದಿ (compl_ಅಂಕೆ_bests == 0)
	ಪ್ರತ್ಯಾಗಮ;

    nn_compl = compl_first_match->cp_next->cp_next;
    ಯದಿ (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_ಸಾಲು() ? compl_first_match
				    : compl_first_match->cp_next;
    ಯದಿ (compl_ಅಂಕೆ_bests == 1)
    {
	// no more candidates insert the match str
	ಯದಿ (!more_candidates)
	{
	    ins_compl_longest_ಸೇರಿಸು(compl->cp_str.string);
	    compl_ಅಂಕೆ_bests = 0;
	}
	compl_ಅಂಕೆ_bests = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ ((size_t)compl_ಅಂಕೆ_bests > SIZE_MAX / ಪರಿಮಾಣ(compl_T *))
	ಪ್ರತ್ಯಾಗಮ;
    compl_best_matches = (compl_T **)alloc(compl_ಅಂಕೆ_bests * ಪರಿಮಾಣ(compl_T *));
    ಯದಿ (compl_best_matches == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (compl != ರಿಕ್ತ && i < compl_ಅಂಕೆ_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_next;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (ಪೂರ್ಣಾಙ್ಕ)compl_best_matches[0]->cp_str.length;

    ಪ್ರತಿ (i = 1; i < compl_ಅಂಕೆ_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	ಯಾವತ್ (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    ಯದಿ (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		ಭಙ್ಗ;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	ಯದಿ (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    ಯದಿ (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	ಗಚ್ಛ end;

    prefix = vim_strnsave(prefix, prefix_len);
    ಯದಿ (prefix != ರಿಕ್ತ)
    {
	ins_compl_longest_ಸೇರಿಸು(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = ರಿಕ್ತ;
    compl_ಅಂಕೆ_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_next_filename_completion(ಶೂನ್ಯ)
{
    char_u	**matches;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    ಪೂರ್ಣಾಙ್ಕ		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    ಪೂರ್ಣಾಙ್ಕ		*fuzzy_indices_data;
    char_u	*last_sep = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    ಪೂರ್ಣಾಙ್ಕ		max_score = 0;
    ಪೂರ್ಣಾಙ್ಕ		current_score = 0;
    ಪೂರ್ಣಾಙ್ಕ		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    ವರ್ಣ pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#ಅನ್ಯಥಾ
    ವರ್ಣ pathsep = PATHSEP;
#endif

    ಯದಿ (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	ಯದಿ (curbuf->b_p_csl[0] == 's')
	{
	    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)leader_len; i++)
	    {
		ಯದಿ (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (curbuf->b_p_csl[0] == 'b')
	{
	    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)leader_len; i++)
	    {
		ಯದಿ (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	ಯದಿ (last_sep == ರಿಕ್ತ)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    compl_pattern.length = 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	ಅನ್ಯಥಾ
	{
	    // Split leader into path and file parts
	    ಪೂರ್ಣಾಙ್ಕ path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    ಯದಿ (path_with_wildcard != ರಿಕ್ತ)
	    {
		vim_snprintf((ವರ್ಣ *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.length = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    ಯದಿ (expand_wildcards(1, &compl_pattern.string, &ಅಂಕೆ_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, ಅಂಕೆ_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    ಯದಿ (curbuf->b_p_csl[0] != NUL)
    {
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
	{
	    ptr = matches[i];
	    ಯಾವತ್ (*ptr != NUL)
	    {
		ಯದಿ (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ಅನ್ಯಥಾ ಯದಿ (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    ಯದಿ (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), 10);
	compl_fuzzy_scores = (ಪೂರ್ಣಾಙ್ಕ *)alloc(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * ಅಂಕೆ_matches);

	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    ಯದಿ (score != FUZZY_SCORE_NONE)
	    {
		ಯದಿ (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((ಪೂರ್ಣಾಙ್ಕ *)fuzzy_indices.ga_data)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	ಯದಿ (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = ರಿಕ್ತ;
	    fuzzy_indices_data = (ಪೂರ್ಣಾಙ್ಕ *)fuzzy_indices.ga_data;
	    qsort(fuzzy_indices_data, fuzzy_indices.ga_len, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), compare_scores);

	    ಪ್ರತಿ (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_data[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_data[i]];
		ಯದಿ (ins_compl_add(match, -1, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ರಿಕ್ತ, current_score) == OK)
		    dir = FORWARD;

		ಯದಿ (need_collect_bests)
		{
		    ಯದಿ (i == 0 || current_score == max_score)
		    {
			compl_ಅಂಕೆ_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(ಅಂಕೆ_matches, matches);
	}
	ಅನ್ಯಥಾ ಯದಿ (leader_len > 0)
	{
	    FreeWild(ಅಂಕೆ_matches, matches);
	    ಅಂಕೆ_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	ಯದಿ (compl_ಅಂಕೆ_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ಅಂಕೆ_matches > 0)
	ins_compl_add_matches(ಅಂಕೆ_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_next_cmdline_completion(ಶೂನ್ಯ)
{
    char_u	**matches;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches;

    ಯದಿ (expand_cmdline(&compl_xp, compl_pattern.string,
		(ಪೂರ್ಣಾಙ್ಕ)compl_pattern.length, &ಅಂಕೆ_matches, &matches) == EXPAND_OK)
    {
	ಪೂರ್ಣಾಙ್ಕ		add_r = OK;
	ಪೂರ್ಣಾಙ್ಕ		dir = compl_direction;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_ಎಣಿಕೆ]) = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};

	    ಯದಿ (compl_xp.xp_files_abbr != ರಿಕ್ತ)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    ಯದಿ (compl_xp.xp_files_kind != ರಿಕ್ತ)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    ಯದಿ (compl_xp.xp_files_menu != ರಿಕ್ತ)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    ಯದಿ (compl_xp.xp_files_info != ರಿಕ್ತ)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, ರಿಕ್ತ, cptext, ರಿಕ್ತ, dir,
		    CP_FAST, FALSE, ರಿಕ್ತ, FUZZY_SCORE_NONE);
	    ಯದಿ (add_r == OK)
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
get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches;

    ಅಂಕೆ_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    ಯದಿ (ಅಂಕೆ_matches > 0)
	ins_compl_add_matches(ಅಂಕೆ_matches, matches, p_ic);
    ಅನ್ಯಥಾ
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    ಸ್ಥಿರ char_u *
ins_compl_get_next_word_or_ಸಾಲು(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	ಪೂರ್ಣಾಙ್ಕ	*match_len,
	ಪೂರ್ಣಾಙ್ಕ	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (ಪೂರ್ಣಾಙ್ಕ)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    ಯದಿ (ctrl_x_mode_ಸಾಲು_or_eval())
    {
	ಯದಿ (compl_status_adding())
	{
	    ಯದಿ (cur_match_pos->lnum >= ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    ಯದಿ (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (ಪೂರ್ಣಾಙ್ಕ)(ptr - tmp_ptr);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	char_u	*tmp_ptr = ptr;

	ಯದಿ (compl_status_adding() && compl_length <= len)
	{
	    tmp_ptr += compl_length;
	    // Skip if already inside a word.
	    ಯದಿ (vim_iswordp(tmp_ptr))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (ಪೂರ್ಣಾಙ್ಕ)(tmp_ptr - ptr);

	ಯದಿ (compl_status_adding() && len == compl_length)
	{
	    ಯದಿ (cur_match_pos->lnum < ins_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		// Try next line, if any. the new word will be
		// "join" as if the normal command "J" was used.
		// IOSIZE is always greater than
		// compl_length, so the next STRNCPY always
		// works -- Acevedo
		STRNCPY(IObuff, ptr, len);
		ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
		tmp_ptr = ptr = skipwhite(ptr);
		// Find start of next word.
		tmp_ptr = find_word_start(tmp_ptr);
		// Find end of next word.
		tmp_ptr = find_word_end(tmp_ptr);
		ಯದಿ (tmp_ptr > ptr)
		{
		    ಯದಿ (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			ಯದಿ (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			ಯದಿ (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ರಿಕ್ತ
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    ಯದಿ (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (ಪೂರ್ಣಾಙ್ಕ)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    ಯದಿ (len == compl_length)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    *match_len = len;
    ಪ್ರತ್ಯಾಗಮ ptr;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    ಪೂರ್ಣಾಙ್ಕ		found_new_match = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		save_p_scs;
    ಪೂರ್ಣಾಙ್ಕ		save_p_ws;
    ಪೂರ್ಣಾಙ್ಕ		looped_around = FALSE;
    char_u	*ptr = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_length > 0;
    char_u	*leader = ins_compl_leader();
    ಪೂರ್ಣಾಙ್ಕ		score = FUZZY_SCORE_NONE;
    ಪೂರ್ಣಾಙ್ಕ		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ಯದಿ (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    ಯದಿ (!in_curbuf)
	p_ws = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	cont_s_ipos = FALSE;

	++ಸಂದೇಶ_silent;  // Don't want messages for wrapscan.

	ಯದಿ (in_fuzzy_collect)
	{
	    found_new_match = ಹುಡುಕು_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_whole_ಸಾಲು() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = ಹುಡುಕು_for_exact_ಸಾಲು(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	ಅನ್ಯಥಾ
	    found_new_match = searchit(ರಿಕ್ತ, st->ins_buf, st->cur_match_pos,
				ರಿಕ್ತ, compl_direction, compl_pattern.string, (ಪೂರ್ಣಾಙ್ಕ)compl_pattern.length,
				1L, ಹುಡುಕು_KEEP + ಹುಡುಕು_NFMSG, RE_LAST, ರಿಕ್ತ);
	--ಸಂದೇಶ_silent;
	ಯದಿ (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    ಯದಿ (looped_around)
		found_new_match = FAIL;
	    ಅನ್ಯಥಾ
		looped_around = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    ಯದಿ (looped_around)
		found_new_match = FAIL;
	    ಅನ್ಯಥಾ
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	ಯದಿ (found_new_match == FAIL)
	    ಭಙ್ಗ;

	// when ADDING, the text before the cursor matches, skip it
	ಯದಿ (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    ಅನುವರ್ತ;

	ಯದಿ (!in_fuzzy_collect)
	    ptr = ins_compl_get_next_word_or_ಸಾಲು(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	ಯದಿ (ptr == ರಿಕ್ತ || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    ಅನುವರ್ತ;

	ಯದಿ (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    ಯದಿ (score < 0)
		score = -score;
	}

	ಯದಿ (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? ರಿಕ್ತ : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    ಯದಿ (in_fuzzy_collect && score == compl_first_match->cp_next->cp_score)
		compl_ಅಂಕೆ_bests++;
	    found_new_match = OK;
	    ಭಙ್ಗ;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    ಪ್ರತ್ಯಾಗಮ found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    ಸ್ಥಿರ callback_T *
get_callback_if_cpt_func(char_u *p, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (*p == 'o')
	ಪ್ರತ್ಯಾಗಮ &curbuf->b_ofu_cb;

    ಯದಿ (*p == 'F')
    {
	ಯದಿ (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    ಯದಿ (curbuf->b_p_cpt_cb == ರಿಕ್ತ
		    || idx < 0 || idx >= curbuf->b_p_cpt_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_cpt_cb[idx].cb_name != ರಿಕ್ತ
		? &curbuf->b_p_cpt_cb[idx] : ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ &curbuf->b_cfu_cb; // 'cfu'
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_register_completion(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		dir = compl_direction;
    yankreg_T	*reg = ರಿಕ್ತ;
    ಶೂನ್ಯ	*reg_ptr = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		adding_mode = compl_status_adding();

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_REGISTERS; i++)
    {
	ಪೂರ್ಣಾಙ್ಕ regname = 0;
	ಯದಿ (i == 0)
	    regname = '"';    // unnamed register
	ಅನ್ಯಥಾ ಯದಿ (i < 10)
	    regname = '0' + i;
	ಅನ್ಯಥಾ ಯದಿ (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	ಅನ್ಯಥಾ ಯದಿ (i == STAR_REGISTER)
	    regname = '*';
	ಅನ್ಯಥಾ ಯದಿ (i == PLUS_REGISTER)
	    regname = '+';
#endif
	ಅನ್ಯಥಾ
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	ಯದಿ (!valid_yank_reg(regname, FALSE) || regname == '_')
	    ಅನುವರ್ತ;

	reg_ptr = get_register(regname, FALSE);
	ಯದಿ (reg_ptr == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	reg = (yankreg_T *)reg_ptr;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < reg->y_size; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    ಯದಿ (str == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    ಯದಿ (adding_mode)
	    {
		ಪೂರ್ಣಾಙ್ಕ str_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);
		ಯದಿ (str_len == 0)
		    ಅನುವರ್ತ;

		ಯದಿ (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0))
		{
		    ಯದಿ (ins_compl_add_infercase(str, str_len, p_ic, ರಿಕ್ತ,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		ಯಾವತ್ (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    ಯದಿ (p >= str_end || *p == NUL)
			ಭಙ್ಗ;

		    char_u *word_end = find_word_end(p);

		    ಯದಿ (word_end <= p)
		    {
			ಯದಿ (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			ಅನ್ಯಥಾ
			    word_end = p + 1;
		    }

		    ಯದಿ (word_end > str_end)
			word_end = str_end;

		    ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)(word_end - p);
		    ಯದಿ (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0)))
		    {
			ಯದಿ (ins_compl_add_infercase(p, len, p_ic, ರಿಕ್ತ,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    ಯದಿ (p <= old_p)
		    {
			p = old_p + 1;
			ಯದಿ (has_mbyte && p < str_end)
			    p = old_p + (*mb_ptr2len)(old_p);
		    }
		}
	    }
	}

	// Free the register copy
	put_register(regname, reg_ptr);
    }
}

/*
 * get the next set of completion matches for "type".
 * Returns TRUE if a new match is found. Otherwise returns FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_next_completion_match(ಪೂರ್ಣಾಙ್ಕ type, ins_compl_next_state_T *st, pos_T *ini)
{
    ಪೂರ್ಣಾಙ್ಕ	found_new_match = FALSE;

    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ -1:
	    ಭಙ್ಗ;
#ifdef FEAT_FIND_ID
	ಪಕ್ಷ CTRL_X_PATH_PATTERNS:
	ಪಕ್ಷ CTRL_X_PATH_DEFINES:
	    get_next_include_file_completion(type);
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ CTRL_X_DICTIONARY:
	ಪಕ್ಷ CTRL_X_THESAURUS:
	    get_next_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ರಿಕ್ತ;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CTRL_X_TAGS:
	    get_next_tag_completion();
	    ಭಙ್ಗ;

	ಪಕ್ಷ CTRL_X_FILES:
	    get_next_filename_completion();
	    ಭಙ್ಗ;

	ಪಕ್ಷ CTRL_X_CMDLINE:
	ಪಕ್ಷ CTRL_X_CMDLINE_CTRL_X:
	    get_next_cmdline_completion();
	    ಭಙ್ಗ;

#ifdef FEAT_COMPL_FUNC
	ಪಕ್ಷ CTRL_X_FUNCTION:
	    ಯದಿ (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    ಅನ್ಯಥಾ
		expand_by_function(type, compl_pattern.string, ರಿಕ್ತ);
	    ಭಙ್ಗ;
	ಪಕ್ಷ CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, ರಿಕ್ತ);
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ CTRL_X_SPELL:
	    get_next_spell_completion(st->first_match_pos.lnum);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CTRL_X_REGISTER:
	    get_register_completion();
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:	// normal ^P/^N and ^X^L
	    found_new_match = get_next_default_completion(st, ini);
	    ಯದಿ (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    ಯದಿ (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    ಪ್ರತ್ಯಾಗಮ found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *ಪಠ = str, *ಲಿಖ = str, *p;

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (*ಪಠ)
    {
	ಯದಿ (*ಪಠ == '^')
	{
	    p = ಪಠ + 1;
	    ಯಾವತ್ (vim_isdigit(*p))
		p++;
	    ಯದಿ ((*p == ',' || *p == '\0') && p != ಪಠ + 1)
	    {
		ಪಠ = p;
		ಅನುವರ್ತ;
	    }
	    ಅನ್ಯಥಾ
		*ಲಿಖ++ = *ಪಠ++;
	}
	ಅನ್ಯಥಾ
	    *ಲಿಖ++ = *ಪಠ++;
    }
    *ಲಿಖ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prepare_cpt_compl_funcs(ಶೂನ್ಯ)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    ಯದಿ (cpt == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    strip_caret_numbers_in_place(cpt);

    ಪ್ರತಿ (p = cpt; *p;)
    {
	ಯಾವತ್ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ಯದಿ (*p == NUL)
	    ಭಙ್ಗ;

	cb = get_callback_if_cpt_func(p, idx);
	ಯದಿ (cb)
	{
	    ಯದಿ (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		ಯದಿ (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		ಅನ್ಯಥಾ
		    startcol = -2;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	ಅನ್ಯಥಾ
	    cpt_sources_array[idx].cs_startcol = -3;

	(ಶೂನ್ಯ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    ಪ್ರತ್ಯಾಗಮ OK;
#endif
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compl_source_start_timer(ಪೂರ್ಣಾಙ್ಕ source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    ಯದಿ (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
advance_cpt_sources_index_safe(ಶೂನ್ಯ)
{
    ಯದಿ (cpt_sources_index >= 0 && cpt_sources_index < cpt_sources_ಎಣಿಕೆ - 1)
    {
	cpt_sources_index++;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_index_out_of_range_nr), cpt_sources_index);
#endif
    ಪ್ರತ್ಯಾಗಮ FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_get_exp(pos_T *ini)
{
    ಸ್ಥಿರ ins_compl_next_state_T   st;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ			    st_cleared = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		match_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		found_new_match;
    ಪೂರ್ಣಾಙ್ಕ		type = ctrl_x_mode;
    ಪೂರ್ಣಾಙ್ಕ		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    ಪೂರ್ಣಾಙ್ಕ		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    ಯದಿ (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	ಯದಿ (!st_cleared)
	{
	    CLEAR_FIELD(st);
	    st_cleared = TRUE;
	}
	st.found_all = FALSE;
	st.ins_buf = curbuf;
	vim_free(st.e_cpt_copy);
	// Make a copy of 'complete', in case the buffer is wiped out.
	st.e_cpt_copy = vim_strsave((compl_cont_status & CONT_LOCAL)
					    ? (char_u *)"." : curbuf->b_p_cpt);
	strip_caret_numbers_in_place(st.e_cpt_copy);
	st.e_cpt = st.e_cpt_copy == ರಿಕ್ತ ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_ಸಾಲು_ಎಣಿಕೆ	1000
	ಯದಿ (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_ಸಾಲು_ಎಣಿಕೆ);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    ಅನ್ಯಥಾ ಯದಿ (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_ಸಾಲು_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != ರಿಕ್ತ;
    ಯದಿ (normal_mode_strict)
    {
	cpt_sources_index = 0;
	ಯದಿ (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    ಪ್ರತಿ (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	ಯದಿ ((ctrl_x_mode_normal() || ctrl_x_mode_ಸಾಲು_or_eval())
					&& (!compl_started || st.found_all))
	{
	    ಪೂರ್ಣಾಙ್ಕ status = process_next_cpt_ಮೌಲ್ಯ(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    ಯದಿ (status == INS_COMPL_CPT_END)
		ಭಙ್ಗ;
	    ಯದಿ (status == INS_COMPL_CPT_CONT)
	    {
		ಯದಿ (may_advance_cpt_idx)
		{
		    ಯದಿ (!advance_cpt_sources_index_safe())
			ಭಙ್ಗ;
		    compl_source_start_timer(cpt_sources_index);
		}
		ಅನುವರ್ತ;
	    }
	}

	ಯದಿ (normal_mode_strict && type == CTRL_X_FUNCTION
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
	found_new_match = get_next_completion_match(type, &st, &start_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	ಯದಿ (may_advance_cpt_idx)
	{
	    ಯದಿ (!advance_cpt_sources_index_safe())
		ಭಙ್ಗ;
	    compl_source_start_timer(cpt_sources_index);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	ಯದಿ ((ctrl_x_mode_not_default() && !ctrl_x_mode_ಸಾಲು_or_eval())
						|| found_new_match != FAIL)
	{
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	    // Fill the popup menu as soon as possible.
	    ಯದಿ (type != -1)
		ins_compl_check_ಕೀಲಿಗಳು(0, FALSE);

	    ಯದಿ ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_ಸಾಲು_or_eval()) || compl_interrupted)
		ಭಙ್ಗ;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    // Mark a buffer scanned when it has been scanned completely
	    ಯದಿ (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	ಯದಿ (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	ಯದಿ (!compl_dir_forward())
	    ಯಾವತ್ (compl_curr_match->cp_prev
		    && !match_at_original_text(compl_curr_match->cp_prev))
		compl_curr_match = compl_curr_match->cp_prev;
    }
    cpt_sources_index = -1;
    compl_started = TRUE;

    ಯದಿ ((ctrl_x_mode_normal() || ctrl_x_mode_ಸಾಲು_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_ಎಣಿಕೆ = -1;		// total of matches, unknown
    ಯದಿ (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_ಸಾಲು_or_eval()))
	match_ಎಣಿಕೆ = ins_compl_make_cyclic();

    ಯದಿ (cot_fuzzy() && compl_get_longest && compl_ಅಂಕೆ_bests > 0)
	fuzzy_longest_match();

    ಯದಿ (compl_old_match != ರಿಕ್ತ)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_next
						    : compl_old_match->cp_prev;
	ಯದಿ (compl_curr_match == ರಿಕ್ತ)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    ಯದಿ (match_ಎಣಿಕೆ > 0 && !ctrl_x_mode_spell())
    {
	ಯದಿ (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	ಯದಿ (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    ಪ್ರತ್ಯಾಗಮ match_ಎಣಿಕೆ;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_update_shown_match(ಶೂನ್ಯ)
{
    string_T	*leader;

    (ಶೂನ್ಯ)get_leader_for_startcol(ರಿಕ್ತ, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    ಯಾವತ್ (!ins_compl_equal(compl_shown_match,
		leader->string, (ಪೂರ್ಣಾಙ್ಕ)leader->length)
	    && compl_shown_match->cp_next != ರಿಕ್ತ
	    && !is_first_match(compl_shown_match->cp_next))
    {
	compl_shown_match = compl_shown_match->cp_next;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    ಯದಿ (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (ಪೂರ್ಣಾಙ್ಕ)leader->length)
	    && (compl_shown_match->cp_next == ರಿಕ್ತ
		|| is_first_match(compl_shown_match->cp_next)))
    {
	ಯಾವತ್ (!ins_compl_equal(compl_shown_match,
		    leader->string, (ಪೂರ್ಣಾಙ್ಕ)leader->length)
		&& compl_shown_match->cp_prev != ರಿಕ್ತ
		&& !is_first_match(compl_shown_match->cp_prev))
	{
	    compl_shown_match = compl_shown_match->cp_prev;
	    leader = get_leader_for_startcol(compl_shown_match, TRUE);
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
    ಪೂರ್ಣಾಙ್ಕ col = compl_col + (compl_status_adding() ? compl_length : 0);
    string_T	remaining = {ರಿಕ್ತ, 0};
    ಪೂರ್ಣಾಙ್ಕ	    orig_col;

    ಯದಿ (ins_compl_preinsert_effect())
    {
	col += (ಪೂರ್ಣಾಙ್ಕ)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    ಯದಿ (curwin->w_cursor.lnum > compl_lnum)
    {
	ಯದಿ (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *ಸಾಲು = ml_get_cursor();
	    remaining.length = ml_get_cursor_len();
	    remaining.string = vim_strnsave(ಸಾಲು, remaining.length);
	    ಯದಿ (remaining.string == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯಾವತ್ (curwin->w_cursor.lnum > compl_lnum)
	{
	    ಯದಿ (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		ಯದಿ (remaining.string)
		    vim_free(remaining.string);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    deleted_ಸಾಲುಗಳು_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col > col)
    {
	ಯದಿ (stop_arrow() == FAIL)
	{
	    ಯದಿ (remaining.string)
		vim_free(remaining.string);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    ಯದಿ (remaining.string != ರಿಕ್ತ)
    {
	orig_col = curwin->w_cursor.col;
	ins_str(remaining.string, remaining.length);
	curwin->w_cursor.col = orig_col;
	vim_free(remaining.string);
    }
    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    changed_cline_bef_curs();
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Insert a completion string that contains newlines.
 * The string is split and inserted line by line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    ಪೂರ್ಣಾಙ್ಕ		base_indent = get_indent();

    ಯಾವತ್ (*curr != NUL)
    {
	ಯದಿ (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    ಯದಿ (curr > start)
		ins_char_bytes(start, (ಪೂರ್ಣಾಙ್ಕ)(curr - start));

	    // Handle newline
	    open_ಸಾಲು(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, ರಿಕ್ತ);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    ಯದಿ (curr > start)
	ins_char_bytes(start, (ಪೂರ್ಣಾಙ್ಕ)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    ಸ್ಥಿರ char_u *
find_common_prefix(size_t *prefix_len, ಪೂರ್ಣಾಙ್ಕ curbuf_only)
{
    compl_T     *compl;
    ಪೂರ್ಣಾಙ್ಕ		*match_ಎಣಿಕೆ;
    char_u	*first = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len = -1;
    ಪೂರ್ಣಾಙ್ಕ		is_cpt_completion = (cpt_sources_array != ರಿಕ್ತ);

    ಯದಿ (!is_cpt_completion)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    match_ಎಣಿಕೆ = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಙ್ಕ, cpt_sources_ಎಣಿಕೆ);
    ಯದಿ (match_ಎಣಿಕೆ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    (ಶೂನ್ಯ)get_leader_for_startcol(ರಿಕ್ತ, TRUE); // Clear the cache

    compl = compl_first_match;
    ಕುರು
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	ಯದಿ (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	ಯದಿ (!match_at_original_text(compl)
		&& (leader->string == ರಿಕ್ತ
		    || ins_compl_equal_sc(compl, leader->string,
			(ಪೂರ್ಣಾಙ್ಕ)leader->length)))
	{
	    // Limit number of items from each source if max_items is set.
	    ಪೂರ್ಣಾಙ್ಕ match_limit_exceeded = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ cur_source = compl->cp_cpt_source_idx;

	    ಯದಿ (cur_source != -1)
	    {
		match_ಎಣಿಕೆ[cur_source]++;
		ಪೂರ್ಣಾಙ್ಕ max_matches = cpt_sources_array[cur_source].cs_max_matches;
		ಯದಿ (max_matches > 0 && match_ಎಣಿಕೆ[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    ಯದಿ (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		ಯದಿ (first == ರಿಕ್ತ && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(first);
		}
		ಅನ್ಯಥಾ ಯದಿ (first != ರಿಕ್ತ)
		{
		    ಪೂರ್ಣಾಙ್ಕ j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    ಯಾವತ್ (j < len && *s1 != NUL && *s2 != NUL)
		    {
			ಯದಿ (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    ಭಙ್ಗ;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    ಯದಿ (len == 0)
			ಭಙ್ಗ;
		}
	    }
	}
	compl = compl->cp_next;
    } ಯಾವತ್ (compl != ರಿಕ್ತ && !is_first_match(compl));

    vim_free(match_ಎಣಿಕೆ);

    ಯದಿ (len > (ಪೂರ್ಣಾಙ್ಕ)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	ಯದಿ (len == (ಪೂರ್ಣಾಙ್ಕ)STRLEN(first))
	{
	    char_u *ಸಾಲು = ml_get_curline();
	    char_u *p = ಸಾಲು + curwin->w_cursor.col;
	    ಯದಿ (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		ಪೂರ್ಣಾಙ್ಕ text_len = end - p;
		ಯದಿ (text_len > 0
			&& text_len < (len - (ಪೂರ್ಣಾಙ್ಕ)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	ಪ್ರತ್ಯಾಗಮ first;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    ಶೂನ್ಯ
ins_compl_ಸೇರಿಸು(ಪೂರ್ಣಾಙ್ಕ move_cursor, ಪೂರ್ಣಾಙ್ಕ ಸೇರಿಸು_prefix)
{
    ಪೂರ್ಣಾಙ್ಕ		compl_len = get_compl_len();
    ಪೂರ್ಣಾಙ್ಕ		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.length;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    ಯದಿ (ಸೇರಿಸು_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	ಯದಿ (cp_str == ರಿಕ್ತ)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    ಯದಿ (cp_str == ರಿಕ್ತ)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.length;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    ಅನ್ಯಥಾ ಯದಿ (cpt_sources_array != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	ಯದಿ (cpt_idx >= 0 && compl_col >= 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    ಯದಿ (startcol >= 0 && startcol < (ಪೂರ್ಣಾಙ್ಕ)compl_col)
	    {
		ಪೂರ್ಣಾಙ್ಕ skip = (ಪೂರ್ಣಾಙ್ಕ)compl_col - startcol;
		ಯದಿ ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    ಯದಿ (compl_len < (ಪೂರ್ಣಾಙ್ಕ)cp_str_len)
    {
	ಯದಿ (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	ಅನ್ಯಥಾ
	{
	    ins_compl_ಸೇರಿಸು_bytes(cp_str + compl_len,
		    ಸೇರಿಸು_prefix ? (ಪೂರ್ಣಾಙ್ಕ)cp_str_len - compl_len : -1);
	    ಯದಿ ((preinsert || ಸೇರಿಸು_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    ಯದಿ (match_at_original_text(compl_shown_match)
	    || (preinsert && !ಸೇರಿಸು_prefix))
	compl_used_match = FALSE;
    ಅನ್ಯಥಾ
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = ಸೇರಿಸು_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_show_filename(ಶೂನ್ಯ)
{
    ವರ್ಣ	*lead = _("match in file");
    ಪೂರ್ಣಾಙ್ಕ		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    ಯದಿ (space <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    ಪ್ರತಿ (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	ಯಾವತ್ (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    ಯದಿ (!compl_autocomplete)
    {
	ಸಂದೇಶ_hist_off = TRUE;
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	ಸಂದೇಶ((ವರ್ಣ *)IObuff);
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
find_next_match_in_menu(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    ಕುರು
	match = is_forward ? match->cp_next : match->cp_prev;
    ಯಾವತ್ (match->cp_next && !match->cp_in_match_array
	    && !match_at_original_text(match));
    ಪ್ರತ್ಯಾಗಮ match;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_next_completion_match(
	ಪೂರ್ಣಾಙ್ಕ	allow_get_expansion,
	ಪೂರ್ಣಾಙ್ಕ	todo,		// repeat completion this many times
	ಪೂರ್ಣಾಙ್ಕ	advance,
	ಪೂರ್ಣಾಙ್ಕ	*ಅಂಕೆ_matches)
{
    ಪೂರ್ಣಾಙ್ಕ		found_end = FALSE;
    compl_T	*found_compl = ರಿಕ್ತ;
    unsigned ಪೂರ್ಣಾಙ್ಕ cur_cot_flags = get_cot_flags();
    ಪೂರ್ಣಾಙ್ಕ		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    ಯಾವತ್ (--todo >= 0)
    {
	ಯದಿ (compl_shows_dir_forward() && compl_shown_match->cp_next != ರಿಕ್ತ)
	{
	    ಯದಿ (compl_match_array != ರಿಕ್ತ)
		compl_shown_match = find_next_match_in_menu();
	    ಅನ್ಯಥಾ
		compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != ರಿಕ್ತ
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	ಅನ್ಯಥಾ ಯದಿ (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != ರಿಕ್ತ)
	{
	    found_end = is_first_match(compl_shown_match);
	    ಯದಿ (compl_match_array != ರಿಕ್ತ)
		compl_shown_match = find_next_match_in_menu();
	    ಅನ್ಯಥಾ
		compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!allow_get_expansion)
	    {
		ಯದಿ (advance)
		{
		    ಯದಿ (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    ಅನ್ಯಥಾ
			compl_pending += todo + 1;
		}
		ಪ್ರತ್ಯಾಗಮ -1;
	    }

	    ಯದಿ (!compl_no_select && advance)
	    {
		ಯದಿ (compl_shows_dir_backward())
		    --compl_pending;
		ಅನ್ಯಥಾ
		    ++compl_pending;
	    }

	    // Find matches.
	    *ಅಂಕೆ_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    ಯಾವತ್ (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		ಯದಿ (compl_pending > 0 && compl_shown_match->cp_next != ರಿಕ್ತ)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		ಅನ್ಯಥಾ ಯದಿ (compl_pending < 0 && compl_shown_match->cp_prev != ರಿಕ್ತ)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		ಅನ್ಯಥಾ
		    ಭಙ್ಗ;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	ಯದಿ (!match_at_original_text(compl_shown_match)
		&& leader->string != ರಿಕ್ತ
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (ಪೂರ್ಣಾಙ್ಕ)leader->length)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	ಅನ್ಯಥಾ
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	ಯದಿ (found_end)
	{
	    ಯದಿ (found_compl != ರಿಕ್ತ)
	    {
		compl_shown_match = found_compl;
		ಭಙ್ಗ;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_next(
    ಪೂರ್ಣಾಙ್ಕ	    allow_get_expansion,
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ,		// repeat completion this many times; should
				// be at least 1
    ಪೂರ್ಣಾಙ್ಕ	    ಸೇರಿಸು_match)	// Insert the newly selected match
{
    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ_matches = -1;
    ಪೂರ್ಣಾಙ್ಕ	    todo = ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ	    advance;
    ಪೂರ್ಣಾಙ್ಕ	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned ಪೂರ್ಣಾಙ್ಕ cur_cot_flags = get_cot_flags();
    ಪೂರ್ಣಾಙ್ಕ	    compl_no_ಸೇರಿಸು = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    ಪೂರ್ಣಾಙ್ಕ	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    ಯದಿ (compl_shown_match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (compl_leader.string != ರಿಕ್ತ
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    ಯದಿ (allow_get_expansion && ಸೇರಿಸು_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = ಎಣಿಕೆ != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    ಯದಿ (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    ಯದಿ (find_next_completion_match(allow_get_expansion, todo, advance,
							&ಅಂಕೆ_matches) == -1)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    ಯದಿ (!started && ins_compl_preinsert_longest())
	ins_compl_ಸೇರಿಸು(TRUE, TRUE);
    ಅನ್ಯಥಾ ಯದಿ (compl_no_ಸೇರಿಸು && !started && !compl_preinsert)
    {
	ins_compl_ಸೇರಿಸು_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸೇರಿಸು_match)
    {
	ಯದಿ (!compl_get_longest || compl_used_match)
	{
	    ಪೂರ್ಣಾಙ್ಕ preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_ಸೇರಿಸು(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	ಅನ್ಯಥಾ
	    ins_compl_ಸೇರಿಸು_bytes(compl_leader.string + get_compl_len(), -1);
    }
    ಅನ್ಯಥಾ
	compl_used_match = FALSE;

    ಯದಿ (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	ಯದಿ (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	ಅನ್ಯಥಾ
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif

	// Delete old text to be replaced, since we're still searching and
	// don't want to match ourselves!
	ins_compl_delete();
    }

    // Enter will select a match when the match wasn't inserted and the popup
    // menu is visible.
    ಯದಿ (compl_no_ಸೇರಿಸು && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    ಅನ್ಯಥಾ
	compl_enter_selects = !ಸೇರಿಸು_match && compl_match_array != ರಿಕ್ತ;

    // Show the file name for the match (if any)
    ಯದಿ (compl_shown_match->cp_fname != ರಿಕ್ತ)
	ins_compl_show_filename();

    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_elapsed_time(ಶೂನ್ಯ)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_index].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    ಯದಿ (elapsed_ms > compl_timeout_ms)
    {
	compl_time_slice_expired = TRUE;
	DECAY_COMPL_TIMEOUT();
    }
#endif
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
ins_compl_check_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಙ್ಕ frequency, ಪೂರ್ಣಾಙ್ಕ in_compl_func)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    ಯದಿ (!in_compl_func && (using_script() || ex_normal_busy))
	ಪ್ರತ್ಯಾಗಮ;

    // Only do this at regular intervals
    ಯದಿ (++ಎಣಿಕೆ < frequency)
	ಪ್ರತ್ಯಾಗಮ;
    ಎಣಿಕೆ = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    ಯದಿ (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	ಯದಿ (vim_is_ctrl_x_ಕೀಲಿ(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (ಶೂನ್ಯ)ins_compl_next(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	ಅನ್ಯಥಾ
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    ಯದಿ (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		ಯದಿ (c != Ctrl_R && (ಕೀಲಿ_Typed
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_ಸಾಲು_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != ರಿಕ್ತ && cpt_sources_index >= 0;
	ಯದಿ (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    ಯದಿ (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	ಪೂರ್ಣಾಙ್ಕ todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(ಶೂನ್ಯ)ins_compl_next(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_key2dir(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	ಪ್ರತ್ಯಾಗಮ BACKWARD;
    ಪ್ರತ್ಯಾಗಮ FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_pum_ಕೀಲಿ(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_key2count(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (ins_compl_pum_ಕೀಲಿ(c) && c != K_UP && c != K_DOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ h = pum_get_height();
	ಯದಿ (h > 3)
	    h -= 2; // keep some context
	ಪ್ರತ್ಯಾಗಮ h;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_use_match(ಪೂರ್ಣಾಙ್ಕ c)
{
    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ K_UP:
	ಪಕ್ಷ K_DOWN:
	ಪಕ್ಷ K_PAGEDOWN:
	ಪಕ್ಷ K_KPAGEDOWN:
	ಪಕ್ಷ K_S_DOWN:
	ಪಕ್ಷ K_PAGEUP:
	ಪಕ್ಷ K_KPAGEUP:
	ಪಕ್ಷ K_S_UP:
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_normal_compl_info(char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ startcol, colnr_T curs_col)
{
    ಯದಿ ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	ಯದಿ (!compl_status_adding())
	{
	    ಯಾವತ್ (--startcol >= 0 && vim_isIDc(ಸಾಲು[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	ಯದಿ (p_ic)
	{
	    compl_pattern.string = str_foldcase(ಸಾಲು + compl_col,
		    compl_length, ರಿಕ್ತ, 0);
	    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	    {
		compl_pattern.length = 0;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	ಅನ್ಯಥಾ
	{
	    compl_pattern.string = vim_strnsave(ಸಾಲು + compl_col, (size_t)compl_length);
	    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	    {
		compl_pattern.length = 0;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    compl_pattern.length = (size_t)compl_length;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	ಯದಿ (!vim_iswordp(ಸಾಲು + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(ಸಾಲು, ಸಾಲು + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ರಿಕ್ತ, ಸಾಲು + compl_col, compl_length) + prefixlen;
	compl_pattern.string = alloc(n);
	ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	{
	    compl_pattern.length = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	STRCPY((ವರ್ಣ *)compl_pattern.string, prefix);
	(ಶೂನ್ಯ)quote_meta(compl_pattern.string + prefixlen,
		ಸಾಲು + compl_col, compl_length);
	compl_pattern.length = n - 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (--startcol < 0
	    || !vim_iswordp(mb_prevptr(ಸಾಲು, ಸಾಲು + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	{
	    compl_pattern.length = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	compl_pattern.length = len;
	compl_col += curs_col;
	compl_length = 0;
	compl_from_nonkeyword = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ base_class;
	    ಪೂರ್ಣಾಙ್ಕ head_off;

	    startcol -= (*mb_head_off)(ಸಾಲು, ಸಾಲು + startcol);
	    base_class = mb_get_class(ಸಾಲು + startcol);
	    ಯಾವತ್ (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(ಸಾಲು, ಸಾಲು + startcol);
		ಯದಿ (base_class != mb_get_class(ಸಾಲು + startcol
			    - head_off))
		    ಭಙ್ಗ;
		startcol -= head_off;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯಾವತ್ (--startcol >= 0 && vim_iswordc(ಸಾಲು[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_length = (ಪೂರ್ಣಾಙ್ಕ)curs_col - startcol;
	ಯದಿ (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	    {
		compl_pattern.length = 0;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    STRCPY((ವರ್ಣ *)compl_pattern.string, "\\<");
	    (ಶೂನ್ಯ)quote_meta(compl_pattern.string + 2, ಸಾಲು + compl_col, 1);
	    STRCAT((ವರ್ಣ *)compl_pattern.string, "\\k");
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	ಅನ್ಯಥಾ
	{
	    size_t  n = quote_meta(ರಿಕ್ತ, ಸಾಲು + compl_col, compl_length) + 2;

	    compl_pattern.string = alloc(n);
	    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	    {
		compl_pattern.length = 0;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    STRCPY((ವರ್ಣ *)compl_pattern.string, "\\<");
	    (ಶೂನ್ಯ)quote_meta(compl_pattern.string + 2, ಸಾಲು + compl_col,
		    compl_length);
	    compl_pattern.length = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    ಯದಿ (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	ಯದಿ (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_wholeline_compl_info(char_u *ಸಾಲು, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(ಸಾಲು);
    compl_length = (ಪೂರ್ಣಾಙ್ಕ)curs_col - (ಪೂರ್ಣಾಙ್ಕ)compl_col;
    ಯದಿ (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    ಯದಿ (p_ic)
    {
	compl_pattern.string = str_foldcase(ಸಾಲು + compl_col, compl_length,
		ರಿಕ್ತ, 0);
	ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	{
	    compl_pattern.length = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	compl_pattern.length = STRLEN(compl_pattern.string);
    }
    ಅನ್ಯಥಾ
    {
	compl_pattern.string = vim_strnsave(ಸಾಲು + compl_col, (size_t)compl_length);
	ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	{
	    compl_pattern.length = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	compl_pattern.length = (size_t)compl_length;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_filename_compl_info(char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    ಯದಿ (startcol > 0)
    {
	char_u	*p = ಸಾಲು + startcol;

	MB_PTR_BACK(ಸಾಲು, p);
	ಯಾವತ್ (p > ಸಾಲು && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(ಸಾಲು, p);
	ಯದಿ (p == ಸಾಲು && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	ಅನ್ಯಥಾ
	    startcol = (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು) + 1;
    }

    compl_col += startcol;
    compl_length = (ಪೂರ್ಣಾಙ್ಕ)curs_col - startcol;
    compl_pattern.string = addstar(ಸಾಲು + compl_col, compl_length, EXPAND_FILES);
    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
    {
	compl_pattern.length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    compl_pattern.length = STRLEN(compl_pattern.string);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_cmdline_compl_info(char_u *ಸಾಲು, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(ಸಾಲು, curs_col);
    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
    {
	compl_pattern.length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    compl_pattern.length = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (ಪೂರ್ಣಾಙ್ಕ)compl_pattern.length, curs_col, FALSE);
    ಯದಿ (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    ಅನ್ಯಥಾ
	compl_col = (ಪೂರ್ಣಾಙ್ಕ)(compl_xp.xp_pattern - compl_pattern.string);
    compl_length = curs_col - compl_col;

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_compl_globals(
    ಪೂರ್ಣಾಙ್ಕ		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		is_cpt_compl UNUSED)
{
    ಯದಿ (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	ಯದಿ (startcol < compl_col)
	    ಪ್ರತ್ಯಾಗಮ prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	ಅನ್ಯಥಾ
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.length);
	    cpt_compl_pattern.length = compl_orig_text.length;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*ಸಾಲು = ml_get(curwin->w_cursor.lnum);
	ಪೂರ್ಣಾಙ್ಕ	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(ಸಾಲು + startcol, (size_t)len);
	ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
	{
	    compl_pattern.length = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	compl_pattern.length = (size_t)len;
	compl_col = startcol;
	compl_length = len;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Get the pattern, column and length for user defined completion ('omnifunc',
 * 'completefunc' and 'thesaurusfunc')
 * Uses the global variable: spell_bad_len
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 * "startcol", when not NULL, contains the column returned by function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		*startcol UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    ಪೂರ್ಣಾಙ್ಕ		col;
    char_u	*funcname = ರಿಕ್ತ;
    pos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		save_State = State;
    ಪೂರ್ಣಾಙ್ಕ		is_cpt_function = (cb != ರಿಕ್ತ);
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ಯದಿ (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	ಯದಿ (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	cb = get_ಸೇರಿಸು_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_ಸಂಖ್ಯೆ;
    args[0].vval.v_ಸಂಖ್ಯೆ = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    ಯದಿ (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    ಯದಿ (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ಯದಿ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (startcol != ರಿಕ್ತ)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    ಯದಿ (col == -2 || aborting())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    ಯದಿ (col == -3)
    {
	ಯದಿ (is_cpt_function)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = ರಿಕ್ತ;
	ಯದಿ (!shortmess(SHM_COMPLETIONMENU))
	    ಸಂದೇಶ_clr_cmdline();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_spell_compl_info(ಪೂರ್ಣಾಙ್ಕ startcol UNUSED, colnr_T curs_col UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*ಸಾಲು = ರಿಕ್ತ;

    ಯದಿ (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    ಅನ್ಯಥಾ
	compl_col = spell_word_start(startcol);

    ಯದಿ (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    ಅನ್ಯಥಾ
    {
	spell_expand_check_cap(compl_col);
	compl_length = (ಪೂರ್ಣಾಙ್ಕ)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    ಸಾಲು = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(ಸಾಲು + compl_col, (size_t)compl_length);
    ಯದಿ (compl_pattern.string == ರಿಕ್ತ)
    {
	compl_pattern.length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    compl_pattern.length = (size_t)compl_length;
    ret = OK;
#endif

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compl_get_info(char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ startcol, colnr_T curs_col, ಪೂರ್ಣಾಙ್ಕ *ಸಾಲು_invalid)
{
    ಯದಿ (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	ಯದಿ (get_normal_compl_info(ಸಾಲು, startcol, curs_col) != OK)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಸಾಲು_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_ಸಾಲು_or_eval())
    {
	ಪ್ರತ್ಯಾಗಮ get_wholeline_compl_info(ಸಾಲು, curs_col);
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_files())
    {
	ಪ್ರತ್ಯಾಗಮ get_filename_compl_info(ಸಾಲು, startcol, curs_col);
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	ಪ್ರತ್ಯಾಗಮ get_cmdline_compl_info(ಸಾಲು, curs_col);
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	ಯದಿ (get_userdefined_compl_info(curs_col, ರಿಕ್ತ, ರಿಕ್ತ) != OK)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಸಾಲು_invalid = TRUE;	// "line" may have become invalid
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_spell())
    {
	ಯದಿ (get_spell_compl_info(startcol, curs_col) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಸಾಲು_invalid = TRUE;	// "line" may have become invalid
    }
    ಅನ್ಯಥಾ
    {
	internal_error("ins_complete()");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    ಯದಿ (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	ಯದಿ (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(ಸಾಲು);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	ಅನ್ಯಥಾ
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    ಯದಿ (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    ಸಾಲು + compl_length
			    + compl_startpos.col) - ಸಾಲು);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_length = curwin->w_cursor.col - (ಪೂರ್ಣಾಙ್ಕ)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	ಯದಿ (compl_length > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_length = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_length;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	ಯದಿ (compl_length < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_ಸಾಲು_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    ಅನ್ಯಥಾ
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_compl_start(ಶೂನ್ಯ)
{
    char_u	*ಸಾಲು = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_invalid = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		save_did_ai = did_ai;
    ಪೂರ್ಣಾಙ್ಕ		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    ಯದಿ (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಸಾಲು = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    ಯದಿ ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_ಹುಡುಕು(ಸಾಲು);
    ಅನ್ಯಥಾ
	compl_cont_status &= CONT_LOCAL;

    ಯದಿ (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	ಯದಿ (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (ಪೂರ್ಣಾಙ್ಕ)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    ಯದಿ (compl_get_info(ಸಾಲು, startcol, curs_col, &ಸಾಲು_invalid) == FAIL)
    {
	ಯದಿ (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    ಯದಿ (ಸಾಲು_invalid)
	ಸಾಲು = ml_get(curwin->w_cursor.lnum);

    ಯದಿ (compl_status_adding())
    {
	ಯದಿ (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	ಯದಿ (ctrl_x_mode_ಸಾಲು_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.col = compl_col;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_length = 0;
	    compl_col = curwin->w_cursor.col;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    ಅನ್ಯಥಾ
    {
	edit_submode_pre = ರಿಕ್ತ;
	compl_startpos.col = compl_col;
    }

    ಯದಿ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	ಯದಿ (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_ಸಂದೇಶ]);
	ಅನ್ಯಥಾ
	    edit_submode = (char_u *)_(CTRL_X_ಸಂದೇಶ(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ರಿಕ್ತ);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.length = (size_t)compl_length;
    compl_orig_text.string = vim_strnsave(ಸಾಲು + compl_col, (size_t)compl_length);
    ಯದಿ (p_ic)
	flags |= CP_ICASE;
    ಯದಿ (compl_orig_text.string == ರಿಕ್ತ
	    || ins_compl_add(compl_orig_text.string,
		(ಪೂರ್ಣಾಙ್ಕ)compl_orig_text.length,
		ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, 0, flags, FALSE, ರಿಕ್ತ, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    ಯದಿ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_ಎಣಿಕೆ;
	showmode();
	edit_submode_extra = ರಿಕ್ತ;
	out_flush();
    }

    did_ai = save_did_ai;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * display the completion status message
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_show_statusmsg(ಶೂನ್ಯ)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    ಯದಿ (is_first_match(compl_first_match->cp_next))
    {
	edit_submode_extra = compl_status_adding() && compl_length > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    ಯದಿ (edit_submode_extra == ರಿಕ್ತ)
    {
	ಯದಿ (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	ಅನ್ಯಥಾ ಯದಿ (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_ಎಣಿಕೆ;
	}
	ಅನ್ಯಥಾ ಯದಿ (compl_curr_match->cp_next == compl_curr_match->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_ಎಣಿಕೆ;
	    compl_curr_match->cp_ಸಂಖ್ಯೆ = 1;
	}
	ಅನ್ಯಥಾ
	{
#ಯದಿ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    ಯದಿ (compl_curr_match->cp_ಸಂಖ್ಯೆ == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    ಯದಿ (compl_curr_match->cp_ಸಂಖ್ಯೆ != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		ಸ್ಥಿರ char_u match_ref[81];

		ಯದಿ (compl_matches > 0)
		    vim_snprintf((ವರ್ಣ *)match_ref, ಪರಿಮಾಣ(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_ಸಂಖ್ಯೆ, compl_matches);
		ಅನ್ಯಥಾ
		    vim_snprintf((ವರ್ಣ *)match_ref, ಪರಿಮಾಣ(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_ಸಂಖ್ಯೆ);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		ಯದಿ (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    ಯದಿ (!compl_opt_suppress_empty)
    {
	showmode();
	ಯದಿ (!shortmess(SHM_COMPLETIONMENU))
	{
	    ಯದಿ (edit_submode_extra != ರಿಕ್ತ)
	    {
		ಯದಿ (!p_smd)
		{
		    ಸಂದೇಶ_hist_off = TRUE;
		    ಸಂದೇಶ_attr((ವರ್ಣ *)edit_submode_extra,
			    edit_submode_highl < HLF_ಎಣಿಕೆ
			    ? HL_ATTR(edit_submode_highl) : 0);
		    ಸಂದೇಶ_hist_off = FALSE;
		}
	    }
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_complete(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ enable_pum)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		save_w_wrow;
    ಪೂರ್ಣಾಙ್ಕ		save_w_leftcol;
    ಪೂರ್ಣಾಙ್ಕ		ಸೇರಿಸು_match;
    ಪೂರ್ಣಾಙ್ಕ		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    ಸೇರಿಸು_match = ins_compl_use_match(c);

    ಯದಿ (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	ಯದಿ (ins_compl_start() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸೇರಿಸು_match && stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_buffer;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_ಅಂಕೆ_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), ಸೇರಿಸು_match);

    // Reset autocompletion timer expiry flag
    ಯದಿ (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    ಯದಿ (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    ಯದಿ (got_int && !global_busy)
    {
	(ಶೂನ್ಯ)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_next);
    ಯದಿ (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	ಯದಿ (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    ಯದಿ (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    ಅನ್ಯಥಾ
	compl_cont_status &= ~CONT_S_IPOS;

    ಯದಿ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    ಯದಿ (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Enable autocompletion
 */
    ಶೂನ್ಯ
ins_compl_enable_autocomplete(ಶೂನ್ಯ)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
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
#ifdef ELAPSED_FUNC
    ಯದಿ (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	ಪ್ರತ್ಯಾಗಮ true;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ false;
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
    ಪ್ರತ್ಯಾಗಮ compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(ಶೂನ್ಯ)
{
#ifdef ELAPSED_FUNC
    ಪ್ರತ್ಯಾಗಮ ELAPSED_FUNC(compl_autocomplete_start_tv);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
show_pum(ಪೂರ್ಣಾಙ್ಕ prev_w_wrow, ಪೂರ್ಣಾಙ್ಕ prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    ಯದಿ (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    ಸ್ಥಿರ unsigned
quote_meta(char_u *dest, char_u *src, ಪೂರ್ಣಾಙ್ಕ len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    ಪ್ರತಿ ( ; --len >= 0; src++)
    {
	ವಿಕಲ್ಪ (*src)
	{
	    ಪಕ್ಷ '.':
	    ಪಕ್ಷ '*':
	    ಪಕ್ಷ '[':
		ಯದಿ (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    ಭಙ್ಗ;
		// FALLTHROUGH
	    ಪಕ್ಷ '~':
		ಯದಿ (!magic_isset())	// quote these only if magic is set
		    ಭಙ್ಗ;
		// FALLTHROUGH
	    ಪಕ್ಷ '\\':
		ಯದಿ (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    ಭಙ್ಗ;
		// FALLTHROUGH
	    ಪಕ್ಷ '^':		// currently it's not needed.
	    ಪಕ್ಷ '$':
		m++;
		ಯದಿ (dest != ರಿಕ್ತ)
		    *dest++ = '\\';
		ಭಙ್ಗ;
	}
	ಯದಿ (dest != ರಿಕ್ತ)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ mb_len = (*mb_ptr2len)(src) - 1;
	    ಯದಿ (mb_len > 0 && len >= mb_len)
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    ಯದಿ (dest != ರಿಕ್ತ)
			*dest++ = *src;
		}
	}
    }
    ಯದಿ (dest != ರಿಕ್ತ)
	*dest = NUL;

    ಪ್ರತ್ಯಾಗಮ m;
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_insexpand_stuff(ಶೂನ್ಯ)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_ಎಣಿಕೆ);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_back_to_badword(ಶೂನ್ಯ)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, ರಿಕ್ತ);
    ಯದಿ (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cpt_sources_clear(ಶೂನ್ಯ)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_index = -1;
    cpt_sources_ಎಣಿಕೆ = 0;
}

/*
 * Setup completion sources.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
setup_cpt_sources(ಶೂನ್ಯ)
{
    char_u  buf[LSIZE];
    ಪೂರ್ಣಾಙ್ಕ	    slen;
    ಪೂರ್ಣಾಙ್ಕ	    idx = 0;
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ;
    char_u  *p;

    cpt_sources_clear();

    ಎಣಿಕೆ = get_cpt_sources_ಎಣಿಕೆ(curbuf->b_p_cpt);
    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, ಎಣಿಕೆ);
    ಯದಿ (cpt_sources_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (p = curbuf->b_p_cpt; *p;)
    {
	ಯಾವತ್ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ಯದಿ (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ಯದಿ (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		ಯದಿ (caret != ರಿಕ್ತ)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((ವರ್ಣ *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_ಎಣಿಕೆ = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_cpt_func_refresh_always(ಶೂನ್ಯ)
{
#ifdef FEAT_COMPL_FUNC
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cpt_sources_ಎಣಿಕೆ; i++)
	ಯದಿ (cpt_sources_array[i].cs_refresh_always)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_compl_make_linear(ಶೂನ್ಯ)
{
    compl_T *m;

    ಯದಿ (compl_first_match == ರಿಕ್ತ || compl_first_match->cp_prev == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    m = compl_first_match->cp_prev;
    m->cp_next = ರಿಕ್ತ;
    compl_first_match->cp_prev = ರಿಕ್ತ;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_old_matches(ಶೂನ್ಯ)
{
    compl_T *current;
    ಪೂರ್ಣಾಙ್ಕ	    shown_match_removed = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    ಯದಿ (cpt_sources_index < 0)
	ಪ್ರತ್ಯಾಗಮ;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    ಪ್ರತಿ (current = compl_first_match; current != ರಿಕ್ತ; )
    {
	ಯದಿ (current->cp_cpt_source_idx == cpt_sources_index)
	{
	    compl_T *to_delete = current;

	    ಯದಿ (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_next;

	    ಯದಿ (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_next;
		compl_first_match->cp_prev = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (to_delete->cp_next == ರಿಕ್ತ) // node to remove is at tail
		to_delete->cp_prev->cp_next = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ // node is in the moddle
	    {
		to_delete->cp_prev->cp_next = to_delete->cp_next;
		to_delete->cp_next->cp_prev = to_delete->cp_prev;
	    }
	    ins_compl_item_free(to_delete);
	}
	ಅನ್ಯಥಾ
	    current = current->cp_next;
    }

    // Re-assign compl_shown_match if necessary
    ಯದಿ (shown_match_removed)
    {
	ಯದಿ (forward)
	    compl_shown_match = compl_first_match;
	ಅನ್ಯಥಾ
	{  // Last node will have the prefix that is being completed
	    ಪ್ರತಿ (current = compl_first_match; current->cp_next != ರಿಕ್ತ;
		    current = current->cp_next)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    ಪ್ರತಿ (current = compl_first_match; current != ರಿಕ್ತ; )
    {
	ಯದಿ ((forward ? current->cp_cpt_source_idx < cpt_sources_index
		    : current->cp_cpt_source_idx > cpt_sources_index))
	{
	    compl_curr_match = forward ? current : current->cp_next;
	    current = current->cp_next;
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    ಸ್ಥಿರ ಶೂನ್ಯ
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_index];
    ಪೂರ್ಣಾಙ್ಕ		    startcol = cpt_src->cs_startcol;

    ಯದಿ (startcol == -2 || startcol == -3)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	ಯದಿ (!cpt_src->cs_refresh_always)
	    ins_compl_ಸೇರಿಸು_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	ಯದಿ (!cpt_src->cs_refresh_always)
	    ins_compl_delete();

	cpt_src->cs_refresh_always = compl_opt_refresh_always;
	compl_opt_refresh_always = FALSE;
    }
}
#endif

/*
 * Retrieve completion matches from functions in the 'cpt' option where the
 * 'refresh:always' flag is set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cpt_compl_refresh(ಶೂನ್ಯ)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_index = 0;
    ಪ್ರತಿ (p = cpt; *p;)
    {
	ಯಾವತ್ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ಯದಿ (*p == NUL)
	    ಭಙ್ಗ;

	ಯದಿ (cpt_sources_array[cpt_sources_index].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_index);
	    ಯದಿ (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		ಯದಿ (ret == FAIL)
		{
		    ಯದಿ (startcol == -3)
			cpt_sources_array[cpt_sources_index].cs_refresh_always
			    = FALSE;
		    ಅನ್ಯಥಾ
			startcol = -2;
		}
		ಅನ್ಯಥಾ ಯದಿ (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_index].cs_startcol = startcol;
		ಯದಿ (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_index);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(ಶೂನ್ಯ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	ಯದಿ (may_advance_cpt_index(p))
	    (ಶೂನ್ಯ)advance_cpt_sources_index_safe();
    }
    cpt_sources_index = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#endif
}

/*
 * "preinserted()" function
 */
    ಶೂನ್ಯ
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ಯದಿ (ins_compl_preinsert_effect())
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}
