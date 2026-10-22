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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_ଧାଡ଼ି	3
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
#define CTRL_X_LOCAL_ସନ୍ଦେଶ	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_ସନ୍ଦେଶ(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
ସ୍ଥିର ଅକ୍ଷର *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ଖାଲି, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ଖାଲି, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ଖାଲି,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#ଯଦି defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ସ୍ଥିର string_T ctrl_x_mode_names[] = {
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
    {ଖାଲି, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା compl_S compl_T;
ସଂରଚନା compl_S
{
    compl_T	*cp_next;
    compl_T	*cp_prev;
    compl_T	*cp_match_next;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_ଗଣନା]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_data;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_flags;		// CP_ values
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_ସଂଖ୍ୟା;		// sequence number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_order;		// list position when first scored
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_score;		// fuzzy match score or proximity score
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_in_match_array;	// collected by compl_match_array
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_user_abbr_hl_id;	// highlight group ID for abbr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_user_kind_hl_id;	// highlight group ID for kind
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
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
ସ୍ଥିର compl_T    *compl_first_match = ଖାଲି;
ସ୍ଥିର compl_T    *compl_curr_match = ଖାଲି;
ସ୍ଥିର compl_T    *compl_shown_match = ଖାଲି;
ସ୍ଥିର compl_T    *compl_old_match = ଖାଲି;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cse_ଗଣନା;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_ଚାବି - CSE_OFF))

ସ୍ଥିର hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    ଯଦି (compl_strings_ht.ht_array == ଖାଲି)
	hash_init(&compl_strings_ht);
    ଯଦି (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    ଯଦି (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	ଯଦି (entry == ଖାଲି)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    ଫେରନ୍ତୁ;
	entry->cse_ଗଣନା = 1;
	vim_strncpy(entry->cse_str, str, len);
	ଯଦି (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    ଅନ୍ୟଥା
	++HI2CSE(hi)->cse_ଗଣନା;
}

// list used to store the compl_T which have the max score
ସ୍ଥିର compl_T	  **compl_best_matches = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_ଅଙ୍କ_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
ସ୍ଥିର string_T	  compl_leader = {ଖାଲି, 0};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_used_match;

// didn't finish finding completions.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_interrupted = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  ctrl_x_mode = CTRL_X_NORMAL;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_matches = 0;	    // number of completion matches
ସ୍ଥିର string_T	  compl_pattern = {ଖାଲି, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
ସ୍ଥିର string_T	  cpt_compl_pattern = {ଖାଲି, 0}; // pattern returned by func in 'cpt'
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_direction = FORWARD;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_shows_dir = FORWARD;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_pending = 0;	    // > 1 for postponed CTRL-N
ସ୍ଥିର pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_length = 0;
ସ୍ଥିର linenr_T	  compl_lnum = 0;           // lnum where the completion start
ସ୍ଥିର colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
ସ୍ଥିର colnr_T	  compl_ins_end_col = 0;
ସ୍ଥିର colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
ସ୍ଥିର string_T	  compl_orig_text = {ଖାଲି, 0};  // text as it was before
					    // completion started
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_cont_mode = 0;
ସ୍ଥିର expand_T	  compl_xp;

ସ୍ଥିର win_T	  *compl_curr_win = ଖାଲି;  // win where completion is active
ସ୍ଥିର buf_T	  *compl_curr_buf = ଖାଲି;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_autocomplete = FALSE;	    // whether autocompletion is active
ସ୍ଥିର bool	  compl_autostarted = false;	    // Vim started this completion
ସ୍ଥିର bool	  compl_autostart_pending = false;  // the trigger armed one
ସ୍ଥିର bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
ସ୍ଥିର elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    କର { \
	ଯଦି (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } ଯେତେବେଳେ (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_cont_status = 0;
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_opt_refresh_always = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_opt_suppress_empty = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  compl_selected_item = -1;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା cpt_source_T
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cs_refresh_always;  // Whether 'refresh:always' is set for func
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cs_startcol;	    // Start column returned by func
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

ସ୍ଥିର cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cpt_sources_ଗଣନା;  // Total number of completion sources specified in the 'cpt' option
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cpt_sources_index = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ସ୍ଥିର pumitem_T *compl_match_array = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_match_arraysize;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ins_compl_add(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, char_u *fname, char_u **cptext, typval_T *user_data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cdir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା adup, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *user_hl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା score);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_longest_match(compl_T *match);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_del_pum(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_files(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, char_u **files, ପୂର୍ଣ୍ଣସଂଖ୍ୟା thesaurus, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, regmatch_T *regmatch, char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dir);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_free(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_compl_need_restart(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_new_leader(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  get_compl_len(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_restart(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_set_original_text(char_u *str, size_t len);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#ଯଦି defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_add_list(list_T *list);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_add_dict(dict_T *dict);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *startcol);
ସ୍ଥିର ଶୂନ୍ୟ get_cpt_func_completion_matches(callback_T *cb);
ସ୍ଥିର callback_T *get_callback_if_cpt_func(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ fill_complete_info_dict(dict_T *di, compl_T *match, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_match);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା setup_cpt_sources(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_cpt_func_refresh_always(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ cpt_sources_clear(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ cpt_compl_refresh(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_compl_key2dir(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_compl_pum_ଚାବି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_compl_key2count(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ଶୂନ୍ୟ show_pum(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_w_wrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_w_leftcol);
ସ୍ଥିର unsigned  quote_meta(char_u *dest, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ins_compl_has_multiple(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_expand_multiple(char_u *str);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_longest_ଭର୍ତ୍ତି(char_u *prefix);
ସ୍ଥିର ଶୂନ୍ୟ ins_compl_make_linear(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ins_compl_make_cyclic(ଶୂନ୍ୟ);

#ifdef FEAT_SPELL
ସ୍ଥିର ଶୂନ୍ୟ spell_back_to_badword(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    ଶୂନ୍ୟ
ins_ctrl_x(ଶୂନ୍ୟ)
{
    ଯଦି (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	ଯଦି (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	ଅନ୍ୟଥା
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_ସନ୍ଦେଶ(ctrl_x_mode));
	edit_submode_pre = ଖାଲି;
	showmode();
    }
    ଅନ୍ୟଥା
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_none(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == 0; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_normal(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_NORMAL; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_scroll(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_SCROLL; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_whole_ଧାଡ଼ି(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_WHOLE_ଧାଡ଼ି; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_files(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_FILES; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_tags(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_TAGS; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_path_patterns(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_path_defines(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_PATH_DEFINES; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_dictionary(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_DICTIONARY; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_thesaurus(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_THESAURUS; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_cmdline(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_function(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_FUNCTION; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_omni(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_OMNI; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_spell(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_SPELL; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_eval(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_EVAL; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_ଧାଡ଼ି_or_eval(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_WHOLE_ଧାଡ଼ି || ctrl_x_mode == CTRL_X_EVAL; }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl_x_mode_register(ଶୂନ୍ୟ)
    { ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ctrl_x_mode_not_default(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ctrl_x_mode_not_defined_yet(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_status_adding(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_status_sol(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_status_local(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    ଶୂନ୍ୟ
compl_status_clear(ଶୂନ୍ୟ)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_dir_forward(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_shows_dir_forward(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_shows_dir_backward(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_compl_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dict_opt)
{
    ଯଦି (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
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
	edit_submode = ଖାଲି;
	ସନ୍ଦେଶ_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	ଯଦି (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    ଯଦି (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_ctrl_x_ଚାବି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    ଯଦି (ins_compl_pum_ଚାବି(c))
	ଫେରନ୍ତୁ TRUE;

    ବାଛ (ctrl_x_mode)
    {
	ସ୍ଥିତି 0:		    // Not in any CTRL-X mode
	    ଫେରନ୍ତୁ (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	ସ୍ଥିତି CTRL_X_NOT_DEFINED_YET:
	ସ୍ଥିତି CTRL_X_CMDLINE_CTRL_X:
	    ଫେରନ୍ତୁ (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	ସ୍ଥିତି CTRL_X_SCROLL:
	    ଫେରନ୍ତୁ (c == Ctrl_Y || c == Ctrl_E);
	ସ୍ଥିତି CTRL_X_WHOLE_ଧାଡ଼ି:
	    ଫେରନ୍ତୁ (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_FILES:
	    ଫେରନ୍ତୁ (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_DICTIONARY:
	    ଫେରନ୍ତୁ (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_THESAURUS:
	    ଫେରନ୍ତୁ (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_TAGS:
	    ଫେରନ୍ତୁ (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	ସ୍ଥିତି CTRL_X_PATH_PATTERNS:
	    ଫେରନ୍ତୁ (c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_PATH_DEFINES:
	    ଫେରନ୍ତୁ (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	ସ୍ଥିତି CTRL_X_CMDLINE:
	    ଫେରନ୍ତୁ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	ସ୍ଥିତି CTRL_X_FUNCTION:
	    ଫେରନ୍ତୁ (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_OMNI:
	    ଫେରନ୍ତୁ (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	ସ୍ଥିତି CTRL_X_SPELL:
	    ଫେରନ୍ତୁ (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_EVAL:
	    ଫେରନ୍ତୁ (c == Ctrl_P || c == Ctrl_N);
	ସ୍ଥିତି CTRL_X_REGISTER:
	    ଫେରନ୍ତୁ (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_at_original_text(compl_T *match)
{
    ଫେରନ୍ତୁ match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_first_match(compl_T *match)
{
    ଫେରନ୍ତୁ match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    ସ୍ଥିର compl_T *
find_original_text_match(ଶୂନ୍ୟ)
{
    ଯଦି (compl_first_match == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (match_at_original_text(compl_first_match))
	ଫେରନ୍ତୁ compl_first_match;
    ଯଦି (compl_first_match->cp_prev != ଖାଲି
	    && match_at_original_text(compl_first_match->cp_prev))
	ଫେରନ୍ତୁ compl_first_match->cp_prev;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_accept_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (compl_autocomplete && compl_from_nonkeyword)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	ଫେରନ୍ତୁ vim_isIDc(c);

    ବାଛ (ctrl_x_mode)
    {
	ସ୍ଥିତି CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    ଫେରନ୍ତୁ vim_isfilec(c) && !vim_ispathsep(c);

	ସ୍ଥିତି CTRL_X_CMDLINE:
	ସ୍ଥିତି CTRL_X_CMDLINE_CTRL_X:
	ସ୍ଥିତି CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    ଫେରନ୍ତୁ vim_isprintc(c) && !VIM_ISWHITE(c);

	ସ୍ଥିତି CTRL_X_WHOLE_ଧାଡ଼ି:
	    // For while line completion a space can be part of the line.
	    ଫେରନ୍ତୁ vim_isprintc(c);
    }
    ଫେରନ୍ତୁ vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    ସ୍ଥିର char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    char_len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    compl_char_len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    min_len,
    char_u  **tofree)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*wca;			// Wide character array.
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_lower = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_len);
    ଯଦି (wca == ଖାଲି)
	ଫେରନ୍ତୁ IObuff;

    p = str;
    ପାଇଁ (i = 0; i < char_len; ++i)
    {
	ଯଦି (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	ଅନ୍ୟଥା
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    ପାଇଁ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ଯଦି (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    ଯଦି (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		ପାଇଁ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		ଭାଙ୍ଗ;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    ଯଦି (!has_lower)
    {
	p = compl_orig_text.string;
	ପାଇଁ (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    ଯଦି (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		ପାଇଁ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		ଭାଙ୍ଗ;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    ପାଇଁ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ଯଦି (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	ଅନ୍ୟଥା ଯଦି (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    ଯେତେବେଳେ (i < char_len)
    {
	ଯଦି (gap.ga_data != ଖାଲି)
	{
	    ଯଦି (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		ଫେରନ୍ତୁ (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    ଯଦି (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    ଅନ୍ୟଥା
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	ଅନ୍ୟଥା ଯଦି ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    ଯଦି (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		ଫେରନ୍ତୁ (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - IObuff);
	}
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	ଅନ୍ୟଥା
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    ଯଦି (gap.ga_data != ଖାଲି)
    {
	*tofree = gap.ga_data;
	ଫେରନ୍ତୁ gap.ga_data;
    }

    *p = NUL;
    ଫେରନ୍ତୁ IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_add_infercase(
    char_u	*str_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		icase,
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cont_s_ipos,  // next ^X<> will set initial_pos
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		char_len;		// count multi-byte characters
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_char_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    char_u	*tofree = ଖାଲି;

    ଯଦି (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	ଯଦି (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    ଯେତେବେଳେ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	ଅନ୍ୟଥା
	{
	    char_len = len;
	}

	// Find actual length of original text.
	ଯଦି (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    ଯେତେବେଳେ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	ଅନ୍ୟଥା
	{
	    compl_char_len = compl_length;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    ଯଦି (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    ଯଦି (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, ଖାଲି, ଖାଲି, dir, flags, FALSE, ଖାଲି, score);
    vim_free(tofree);
    ଫେରନ୍ତୁ res;
}

/*
 * Check if fuzzy matching is enabled
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cot_fuzzy(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_nearest_active(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_is_match_selected(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_shown_match != ଖାଲି && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_preinsert_longest(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_autocomplete
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_add(
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_data UNUSED,  // "user_data" entry or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cdir,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		adup,		    // accept duplicate match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*user_hl,           // user abbr/kind hlattr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score)
{
    compl_T	*match, *current, *prev;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = (cdir == 0 ? compl_direction : cdir);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = flags_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inserted = FALSE;
    char_u	*new_str = ଖାଲି;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    ଯଦି (flags & CP_FAST)
	fast_breakcheck();
    ଅନ୍ୟଥା
	ui_breakcheck();
    ଯଦି (got_int)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (len < 0)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);

    // If the same match is already present, don't add it.
    ଯଦି (compl_first_match != ଖାଲି && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *ଚାବି;
	hashitem_T  *hi;

	ଯଦି (len < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    ଚାବି = keybuf;
	}
	ଅନ୍ୟଥା
	{
	    new_str = vim_strnsave(str, len);
	    ଯଦି (new_str == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    ଚାବି = new_str;
	}
	str_hash = hash_hash(ଚାବି);
	hi = hash_lookup(&compl_strings_ht, ଚାବି, str_hash);
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    ଯଦି (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		କର
		{
		    ଯଦି (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)match->cp_str.length <= len
					  || match->cp_str.string[len] == NUL))
		    {
			ଯଦି (score < match->cp_score)
			    match->cp_score = score;
			ଭାଙ୍ଗ;
		    }
		    match = match->cp_next;
		} ଯେତେବେଳେ (match != ଖାଲି && !is_first_match(match));
	    }
	    vim_free(new_str);
	    ଫେରନ୍ତୁ NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    ଯଦି (match == ଖାଲି)
    {
	vim_free(new_str);
	ଫେରନ୍ତୁ FAIL;
    }
    match->cp_ସଂଖ୍ୟା = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    ଯଦି (new_str == ଖାଲି && (new_str = vim_strnsave(str, len)) == ଖାଲି)
    {
	vim_free(match);
	ଫେରନ୍ତୁ FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.length = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    ଯଦି (fname != ଖାଲି
	    && compl_curr_match != ଖାଲି
	    && compl_curr_match->cp_fname != ଖାଲି
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    ଅନ୍ୟଥା ଯଦି (fname != ଖାଲି)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    ଅନ୍ୟଥା
	match->cp_fname = ଖାଲି;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_index;

    ଯଦି (cptext != ଖାଲି)
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < CPT_ଗଣନା; ++i)
	{
	    ଯଦି (cptext[i] != ଖାଲି && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    ଯଦି (user_data != ଖାଲି)
	match->cp_user_data = *user_data;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    ଯଦି (compl_first_match == ଖାଲି)
	match->cp_next = match->cp_prev = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_next;
	prev = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	ଯେତେବେଳେ (current != ଖାଲି && current != compl_first_match)
	{
	    ଯଦି (current->cp_score < score)
	    {
		match->cp_next = current;
		match->cp_prev = current->cp_prev;
		ଯଦି (current->cp_prev)
		    current->cp_prev->cp_next = match;
		current->cp_prev = match;
		inserted = TRUE;
		ଭାଙ୍ଗ;
	    }
	    prev = current;
	    current = current->cp_next;
	}
	ଯଦି (!inserted)
	{
	    prev->cp_next = match;
	    match->cp_prev = prev;
	    match->cp_next = compl_first_match;
	    compl_first_match->cp_prev = match;
	}
    }
    ଅନ୍ୟଥା ଯଦି (dir == FORWARD)
    {
	match->cp_next = compl_curr_match->cp_next;
	match->cp_prev = compl_curr_match;
    }
    ଅନ୍ୟଥା	// BACKWARD
    {
	match->cp_next = compl_curr_match;
	match->cp_prev = compl_curr_match->cp_prev;
    }
    ଯଦି (match->cp_next)
	match->cp_next->cp_prev = match;
    ଯଦି (match->cp_prev)
	match->cp_prev->cp_next = match;
    ଅନ୍ୟଥା	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    ଯଦି (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.length,
								    str_hash);

    // Find the longest common string if still doing that.
    ଯଦି (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_equal(compl_T *match, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଯଦି (match->cp_flags & CP_EQUAL)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (match->cp_flags & CP_ICASE)
	ଫେରନ୍ତୁ STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    ଫେରନ୍ତୁ STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_equal_sc(compl_T *match, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	typed = compl_length;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(compl_longest_end_col - compl_col) : typed;

    ଯଦି ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	ଫେରନ୍ତୁ ins_compl_equal(match, str, len);

    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)match->cp_str.length < len)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; ++i)
    {
	ଯଦି (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_ଭର୍ତ୍ତି_bytes(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଯଦି (len == -1)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_leader.string != ଖାଲି ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    ସ୍ଥିର size_t
ins_compl_leader_len(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_leader.string != ଖାଲି ? compl_leader.length : compl_orig_text.length;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_col_range_attr(linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    start_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr;

    ଯଦି (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	ଫେରନ୍ତୁ -1;

    start_col = compl_col + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ins_compl_leader_len();
    ଯଦି (!ins_compl_has_multiple())
	ଫେରନ୍ତୁ (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    ଯଦି ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	ଫେରନ୍ତୁ attr;

    ଫେରନ୍ତୁ -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_has_multiple(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ vim_strchr(compl_shown_match->cp_str.string, '\n') != ଖାଲି;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_lnum_in_range(linenr_T lnum)
{
    ଯଦି (!ins_compl_has_multiple())
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1, c2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_match;

    ଯଦି (compl_leader.string == ଖାଲି)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.length);
	ଯଦି (compl_leader.string == ଖାଲି)
	    ଫେରନ୍ତୁ;

	compl_leader.length = match->cp_str.length;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ଭର୍ତ୍ତି(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ଯଦି (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	ଫେରନ୍ତୁ;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	ଅନ୍ୟଥା
	{
	    c1 = *p;
	    c2 = *s;
	}
	ଯଦି ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    ଭାଙ୍ଗ;
	ଯଦି (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	ଅନ୍ୟଥା
	{
	    ++p;
	    ++s;
	}
    }

    ଯଦି (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.length = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ଭର୍ତ୍ତି(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ଯଦି (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_add_matches(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches,
    char_u	**matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		icase)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_r = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = compl_direction;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, ଖାଲି, ଖାଲି, ଖାଲି, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ଖାଲି,
		FUZZY_SCORE_NONE);
	ଯଦି (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(ଅଙ୍କ_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_make_cyclic(ଶୂନ୍ୟ)
{
    compl_T *match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2 = 0;

    ଯଦି (compl_first_match == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    ଯେତେବେଳେ (match->cp_next != ଖାଲି && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++ଗଣନା_2;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    ଫେରନ୍ତୁ ଗଣନା_2;
}

/*
 * Return whether there currently is a shown match.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_has_shown_match(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_shown_match == ଖାଲି
	|| compl_shown_match != compl_shown_match->cp_next;
}

/*
 * Return whether the shown match is long enough.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_long_shown_match(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_shown_match->cp_str.length
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cot_flags(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_upd_pum(ଶୂନ୍ୟ)
{
    ଯଦି (compl_match_array == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    ଯଦି (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_del_pum(ଶୂନ୍ୟ)
{
    ଯଦି (compl_match_array == ଖାଲି)
	ଫେରନ୍ତୁ;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_wanted(ଶୂନ୍ୟ)
{
    // 'completeopt' must contain "menu" or "menuone"
    ଫେରନ୍ତୁ (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_enough_matches(ଶୂନ୍ୟ)
{
    compl_T     *compl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    କର
    {
	ଯଦି (compl == ଖାଲି || (!match_at_original_text(compl) && ++i == 2))
	    ଭାଙ୍ଗ;
	compl = compl->cp_next;
    } ଯେତେବେଳେ (!is_first_match(compl));

    ଯଦି ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	ଫେରନ୍ତୁ (i >= 1);
    ଫେରନ୍ତୁ (i >= 2);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    ସ୍ଥିର dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    fill_complete_info_dict(dict, match, FALSE);
    ଫେରନ୍ତୁ dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
trigger_complete_changed_event(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    recursive = FALSE;
    save_v_event_T  save_v_event;

    ଯଦି (recursive)
	ଫେରନ୍ତୁ;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ଖାଲି, ଖାଲି, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    ସ୍ଥିର ଶୂନ୍ୟ*
cp_get_next(ଶୂନ୍ୟ *ଗଣ୍ଠି)
{
    ଫେରନ୍ତୁ ((compl_T*)ଗଣ୍ଠି)->cp_next;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cp_set_next(ଶୂନ୍ୟ *ଗଣ୍ଠି, ଶୂନ୍ୟ *next)
{
    ((compl_T*)ଗଣ୍ଠି)->cp_next = (compl_T*)next;
}

    ସ୍ଥିର ଶୂନ୍ୟ*
cp_get_prev(ଶୂନ୍ୟ* ଗଣ୍ଠି)
{
    ଫେରନ୍ତୁ ((compl_T*)ଗଣ୍ଠି)->cp_prev;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cp_set_prev(ଶୂନ୍ୟ* ଗଣ୍ଠି, ଶୂନ୍ୟ* prev)
{
    ((compl_T*)ଗଣ୍ଠି)->cp_prev = (compl_T*)prev;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cp_compare_fuzzy(ଅଚଳ ଶୂନ୍ୟ* a, ଅଚଳ ଶୂନ୍ୟ* b)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା score_a = ((compl_T*)a)->cp_score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା score_b = ((compl_T*)b)->cp_score;

    ଯଦି (score_a != score_b)
	ଫେରନ୍ତୁ (score_b > score_a) ? 1 : -1;

    ଫେରନ୍ତୁ ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cp_compare_nearest(ଅଚଳ ଶୂନ୍ୟ* a, ଅଚଳ ଶୂନ୍ୟ* b)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା score_a = ((compl_T*)a)->cp_score;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା score_b = ((compl_T*)b)->cp_score;
    ଯଦି (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prepend_startcol_text(string_T *dest, string_T *src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prepend_len = compl_col - startcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_length = prepend_len + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)src->length;

    dest->length = (size_t)new_length;
    dest->string = alloc(new_length + 1);  // +1 for NUL
    ଯଦି (dest->string == ଖାଲି)
    {
	dest->length = 0;
	ଫେରନ୍ତୁ FAIL;
    }

    char_u	*ଧାଡ଼ି = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, ଧାଡ଼ି + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->length);
    dest->string[new_length] = NUL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    ସ୍ଥିର string_T *
get_leader_for_startcol(compl_T *match, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached)
{
    ସ୍ଥିର string_T adjusted_leader = {ଖାଲି, 0};

    ଯଦି (match == ଖାଲି)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (cpt_sources_array == ଖାଲି)
	ଯାଅ theend;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cpt_idx = match->cp_cpt_source_idx;
    ଯଦି (cpt_idx < 0)
	ଯାଅ theend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    ଯଦି (compl_leader.string == ଖାଲି)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	ଯଦି (startcol < 0 || startcol >= compl_col)
	    ଫେରନ୍ତୁ &compl_orig_text;
	ଫେରନ୍ତୁ &compl_leader;  // pass through (startcol < compl_col)
    }

    ଯଦି (compl_col <= 0)
	ଯାଅ theend;

    ଯଦି (startcol >= 0 && startcol < compl_col)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prepend_len = compl_col - startcol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_length = prepend_len + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_leader.length;
	ଯଦି (cached && (size_t)new_length == adjusted_leader.length
		&& adjusted_leader.string != ଖାଲି)
	    ଫେରନ୍ତୁ &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	ଯଦି (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    ଯାଅ theend;

	ଫେରନ୍ତୁ &adjusted_leader;
    }
theend:
    ଫେରନ୍ତୁ &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_fuzzy_score(ଶୂନ୍ୟ)
{
    compl_T *compl;
    char_u  *pattern;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    use_leader;

    ଯଦି (compl_first_match == ଖାଲି)
	ଫେରନ୍ତୁ;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ଖାଲି && compl_leader.length > 0);
    ଯଦି (!use_leader)
    {
	ଯଦି (compl_orig_text.string == ଖାଲି || compl_orig_text.length == 0)
	    ଫେରନ୍ତୁ;
	pattern = compl_orig_text.string;
    }
    ଅନ୍ୟଥା
    {
	/* Clear the leader cache once before the loop */
	(ଶୂନ୍ୟ)get_leader_for_startcol(ଖାଲି, TRUE);
	pattern = ଖାଲି;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା next = 0;
    compl = compl_first_match;
    କର
    {
	ଯଦି (compl->cp_order < 0)
	    compl->cp_order = next;
	next = MAX(next, compl->cp_order + 1);

	ଯଦି (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_next;
    } ଯେତେବେଳେ (compl != ଖାଲି && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sort_compl_match_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*compare)(ଅଚଳ ଶୂନ୍ୟ *, ଅଚଳ ଶୂନ୍ୟ *))
{
    compl_T	*orig_text;

    ଯଦି (!compl_first_match || is_first_match(compl_first_match->cp_next))
	ଫେରନ୍ତୁ;

    orig_text = find_original_text_match();
    ଯଦି (orig_text == ଖାଲି)
	ଫେରନ୍ତୁ;

    ins_compl_make_linear();
    ଯଦି (orig_text == compl_first_match)
    {
	compl_first_match->cp_next->cp_prev = ଖାଲି;
	compl_first_match->cp_next = mergesort_list(compl_first_match->cp_next,
		cp_get_next, cp_set_next, cp_get_prev, cp_set_prev, compare);
	compl_first_match->cp_next->cp_prev = compl_first_match;
    }
    ଅନ୍ୟଥା
    {
	compl_T	*tail;

	orig_text->cp_prev->cp_next = ଖାଲି;
	compl_first_match = mergesort_list(compl_first_match, cp_get_next,
		cp_set_next, cp_get_prev, cp_set_prev, compare);
	tail = compl_first_match;
	ଯେତେବେଳେ (tail->cp_next != ଖାଲି)
	    tail = tail->cp_next;
	tail->cp_next = orig_text;
	orig_text->cp_prev = tail;
    }
    (ଶୂନ୍ୟ)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_user_highlight_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr;

    ଯଦି (hl_id <= 0)
	ଫେରନ୍ତୁ -1;
    attr = syn_id2attr(hl_id);
    ଫେରନ୍ତୁ attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_build_pum(ଶୂନ୍ୟ)
{
    compl_T     *compl;
    compl_T     *shown_compl = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_find_shown_match = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shown_match_ok = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_head = ଖାଲି;
    compl_T	*match_tail = ଖାଲି;
    compl_T	*match_next = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*match_ଗଣନା = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_forward = compl_shows_dir_forward();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cpt_completion = (cpt_sources_array != ଖାଲି);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    ଯଦି (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    ଯଦି (compl_leader.string != ଖାଲି
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_next;

    ଯଦି (is_cpt_completion)
    {
	match_ଗଣନା = ALLOC_CLEAR_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, cpt_sources_ଗଣନା);
	ଯଦି (match_ଗଣନା == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
    }

    (ଶୂନ୍ୟ)get_leader_for_startcol(ଖାଲି, TRUE); // Clear the cache

    compl = compl_first_match;
    କର
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	ଯଦି (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	ଯଦି (!match_at_original_text(compl)
		&& (leader->string == ଖାଲି
		    || ins_compl_equal_sc(compl, leader->string,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader->length)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା match_limit_exceeded = FALSE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_source = compl->cp_cpt_source_idx;
	    ଯଦି (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_ଗଣନା[cur_source]++;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_matches = cpt_sources_array[cur_source].cs_max_matches;
		ଯଦି (max_matches > 0 && match_ଗଣନା[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    ଯଦି (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		ଯଦି (match_head == ଖାଲି)
		    match_head = compl;
		ଅନ୍ୟଥା
		    match_tail->cp_match_next = compl;
		match_tail = compl;

		ଯଦି (!shown_match_ok && !cot_fuzzy())
		{
		    ଯଦି (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    ଅନ୍ୟଥା
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		ଅନ୍ୟଥା ଯଦି (cot_fuzzy())
		{
		    ଯଦି (i == 0)
			shown_compl = compl;

		    ଯଦି (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	ଯଦି (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    ଯଦି (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    ଯଦି (!shown_match_ok && shown_compl != ଖାଲି)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } ଯେତେବେଳେ (compl != ଖାଲି && !is_first_match(compl));

    vim_free(match_ଗଣନା);

    ଯଦି (compl_match_arraysize == 0)
	ଫେରନ୍ତୁ -1;

    ଯଦି (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    ଯଦି (compl_match_array == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    compl = match_head;
    i = 0;
    ଯେତେବେଳେ (compl != ଖାଲି)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != ଖାଲି
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != ଖାଲି
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_next = compl->cp_match_next;
	compl->cp_match_next = ଖାଲି;
	compl = match_next;
    }

    ଯଦି (!shown_match_ok)    // no displayed match at all
	cur = -1;

    ଫେରନ୍ତୁ cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    ଶୂନ୍ୟ
ins_compl_show_pum(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur = -1;
    colnr_T	col;

    ଯଦି (!pum_wanted() || !pum_enough_matches())
	ଫେରନ୍ତୁ;

    // Avoid redrawing the screen under a pum that stays in place.
    ଯଦି (!pum_redraw_in_same_position())
	pum_call_update_screen();

    ଯଦି (compl_match_array == ଖାଲି)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    ଅନ୍ୟଥା
    {
	// popup menu already exists, only need to find the current item.
	ପାଇଁ (i = 0; i < compl_match_arraysize; ++i)
	{
	    ଯଦି (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (compl_match_array == ଖାଲି)
    {
#ifdef FEAT_EVAL
	ଯଦି (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	ଫେରନ୍ତୁ;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    ଯଦି (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    ଯଦି (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// DICT_FIRST and/or DICT_EXACT
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_scs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = compl_direction;

    ଯଦି (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	ଯଦି (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	ଅନ୍ୟଥା
#endif
	    ଫେରନ୍ତୁ;
    }

    buf = alloc(LSIZE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    regmatch.regprog = ଖାଲି;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ଯଦି (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    ଯଦି (ctrl_x_mode_ଧାଡ଼ି_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	ଯଦି (pat_esc == ଖାଲି)
	    ଯାଅ theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	ଯଦି (ptr == ଖାଲି)
	{
	    vim_free(pat_esc);
	    ଯାଅ theend;
	}
	vim_snprintf((ଅକ୍ଷର *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    ଅନ୍ୟଥା
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ଯଦି (regmatch.regprog == ଖାଲି)
	    ଯାଅ theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    ଯେତେବେଳେ (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	ଯଦି (flags == DICT_EXACT)
	{
	    ଗଣନା_2 = 1;
	    files = &dict;
	}
	ଅନ୍ୟଥା
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    ଯଦି (!thesaurus && STRCMP(buf, "spell") == 0)
		ଗଣନା_2 = -1;
	    ଅନ୍ୟଥା
#endif
		ଯଦି (vim_strchr(buf, '`') != ଖାଲି
		    || expand_wildcards(1, &buf, &ଗଣନା_2, &files,
						     EW_FILE|EW_SILENT) != OK)
		ଗଣନା_2 = 0;
	}

#ifdef FEAT_SPELL
	ଯଦି (ଗଣନା_2 == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    ଯଦି (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    ଅନ୍ୟଥା
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	ଅନ୍ୟଥା
#endif
	    ଯଦି (ଗଣନା_2 > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(ଗଣନା_2, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    ଯଦି (flags != DICT_EXACT)
		FreeWild(ଗଣନା_2, files);
	}
	ଯଦି (flags != 0)
	    ଭାଙ୍ଗ;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
thesaurus_add_ଶବ୍ଦଗୁଡ଼ିକ_in_ଧାଡ଼ି(
    char_u	*fname,
    char_u	**buf_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    char_u	*skip_word)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    ଯେତେବେଳେ (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	ଯଦି (*ptr == NUL || *ptr == NL)
	    ଭାଙ୍ଗ;
	wstart = ptr;

	// Find end of the word.
	ଯଦି (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    ଯେତେବେଳେ (*ptr != NUL)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = (*mb_ptr2len)(ptr);

		ଯଦି (l < 2 && !vim_iswordc(*ptr))
		    ଭାଙ୍ଗ;
		ptr += l;
	    }
	ଅନ୍ୟଥା
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	ଯଦି (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    ଯଦି (status == FAIL)
		ଭାଙ୍ଗ;
	}
    }

    *buf_arg = ptr;
    ଫେରନ୍ତୁ status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_files(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,
    char_u	**files,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		thesaurus,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dir)
{
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    FILE	*fp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_r;
    char_u	*leader = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leader_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    char_u	*ଧାଡ଼ି_end = ଖାଲି;

    ଯଦି (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ins_compl_leader_len();
    }

    ପାଇଁ (i = 0; i < ଗଣନା_2 && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((ଅକ୍ଷର *)files[i], "r");  // open dictionary file
	ଯଦି (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    ସନ୍ଦେଶ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (ଅକ୍ଷର *)files[i]);
	    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_trunc_attr((ଅକ୍ଷର *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	ଯଦି (fp == ଖାଲି)
	    ଜାରି;

	// Read dictionary file line by line.
	// Check each line for a match.
	ଯେତେବେଳେ (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    ଯଦି (cot_fuzzy() && leader_len > 0)
	    {
		ଧାଡ଼ି_end = find_ଧାଡ଼ି_end(ptr);
		ଯେତେବେଳେ (ptr < ଧାଡ଼ି_end)
		{
		    ଯଦି (fuzzy_match_str_in_ଧାଡ଼ି(&ptr, leader, &len, ଖାଲି, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_ଧାଡ଼ି_or_eval()
					? find_ଧାଡ଼ି_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			ଯଦି (add_r == FAIL)
			    ଭାଙ୍ଗ;
			ptr = end_ptr;  // start from next word
			ଯଦି (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_next
				&& score == compl_first_match->cp_next->cp_score)
			    compl_ଅଙ୍କ_bests++;
		    }
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (regmatch != ଖାଲି)
	    {
		ଯେତେବେଳେ (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_ଧାଡ଼ି_or_eval() ? find_ଧାଡ଼ି_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    ଯଦି (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_ଶବ୍ଦଗୁଡ଼ିକ_in_ଧାଡ଼ି(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    ଯଦି (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    ଅନ୍ୟଥା ଯଦି (add_r == FAIL)
			ଭାଙ୍ଗ;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    ଯଦି (*ptr == '\n' || got_int)
			ଭାଙ୍ଗ;
		}
	    }
	    ଧାଡ଼ି_breakcheck();
	    ins_compl_check_ଚାବିଗୁଡ଼ିକ(50, FALSE);
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
    ଯଦି (has_mbyte)
    {
	ଯେତେବେଳେ (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    ଅନ୍ୟଥା
    {
	ଯେତେବେଳେ (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    ଫେରନ୍ତୁ ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    ଯଦି (has_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_class = mb_get_class(ptr);
	ଯଦି (start_class > 1)
	    ଯେତେବେଳେ (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		ଯଦି (mb_get_class(ptr) != start_class)
		    ଭାଙ୍ଗ;
	    }
    }
    ଅନ୍ୟଥା
	ଯେତେବେଳେ (vim_iswordc(*ptr))
	    ++ptr;
    ଫେରନ୍ତୁ ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_ଧାଡ଼ି_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    ଯେତେବେଳେ (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    ଫେରନ୍ତୁ s;
}

/*
 * Free a completion item in the list
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    ଯଦି (compl_strings_ht.ht_used > 0 && match->cp_str.string != ଖାଲି
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    ଯଦି (--entry->cse_ଗଣନା <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    ଯଦି (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < CPT_ଗଣନା; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_data);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_free(ଶୂନ୍ୟ)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    ଯଦି (compl_first_match == ଖାଲି)
	ଫେରନ୍ତୁ;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    କର
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	ins_compl_item_free(match);
    } ଯେତେବେଳେ (compl_curr_match != ଖାଲି && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = ଖାଲି;
    compl_shown_match = ଖାଲି;
    compl_old_match = ଖାଲି;
}

/*
 * Reset/clear the completion state.
 */
    ଶୂନ୍ୟ
ins_compl_clear(ଶୂନ୍ୟ)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = ଖାଲି;
    compl_curr_buf = ଖାଲି;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ଖାଲି;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ଅଙ୍କ_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_active(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_win_active(win_T *wp)
{
    ଫେରନ୍ତୁ ins_compl_active() && wp == compl_curr_win
	&& wp->w_buffer == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_used_match(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_used_match;
}

/*
 * Initialize get longest common string.
 */
    ଶୂନ୍ୟ
ins_compl_init_get_longest(ଶୂନ୍ୟ)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_interrupted(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_enter_selects(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_len(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_length;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_has_preinsert(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_cot_flags = get_cot_flags();
    ଯଦି (compl_autocomplete && p_ic && !p_inf)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_preinsert_effect(ଶୂନ୍ୟ)
{
    ଯଦି (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_bs(ଶୂନ୍ୟ)
{
    char_u	*ଧାଡ଼ି;
    char_u	*p;

    ଯଦି (ins_compl_preinsert_effect())
	ins_compl_delete();

    ଧାଡ଼ି = ml_get_curline();
    p = ଧାଡ଼ି + curwin->w_cursor.col;
    MB_PTR_BACK(ଧାଡ଼ି, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col < 0
	    || ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col
							- compl_length < 0))
	ଫେରନ୍ତୁ K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    ଯଦି (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.length = (size_t)((p - ଧାଡ଼ି) - compl_col);
    compl_leader.string = vim_strnsave(ଧାଡ଼ି + compl_col, compl_leader.length);
    ଯଦି (compl_leader.string == ଖାଲି)
    {
	compl_leader.length = 0;
	ଫେରନ୍ତୁ K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    ଯଦି (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    ଯଦି (compl_shown_match != ଖାଲି)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    ଫେରନ୍ତୁ NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_need_restart(ଶୂନ୍ୟ)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    ଫେରନ୍ତୁ compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_has_autocomplete(ଶୂନ୍ୟ)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    ଫେରନ୍ତୁ curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_fuzzy_sort(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    ଯଦି (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	ଯଦି ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_prev);
	    ଯଦି (!none_selected)
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
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_new_leader(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_w_wrow = curwin->w_wrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_ଭର୍ତ୍ତି_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    ଯଦି (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.length);
	ଯଦି (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	ଯଦି (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    ଅନ୍ୟଥା
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	ଯଦି (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	ଅନ୍ୟଥା
	    compl_autocomplete = FALSE;
	ଯଦି (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    ଯଦି (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    ଯଦି (compl_match_array == ଖାଲି)
	compl_enter_selects = FALSE;
    ଅନ୍ୟଥା ଯଦି (ins_compl_has_preinsert() && compl_leader.length > 0)
	ins_compl_ଭର୍ତ୍ତି(TRUE, FALSE);
    ଅନ୍ୟଥା ଯଦି (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.length > 0 && !ins_compl_preinsert_effect())
	ins_compl_ଭର୍ତ୍ତି(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_compl_len(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col;
    ଫେରନ୍ତୁ MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ଶୂନ୍ୟ
ins_compl_addleader(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cc;

    ଯଦି (ins_compl_preinsert_effect())
	ins_compl_delete();

    ଯଦି (stop_arrow() == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	ଯଦି (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    ଅନ୍ୟଥା
    {
	ins_char(c);
	ଯଦି (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    ଯଦି (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    ଯଦି (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.length = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.length);
	ଯଦି (compl_leader.string == ଖାଲି)
	{
	    compl_leader.length = 0;
	    ଫେରନ୍ତୁ;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_restart(ଶୂନ୍ୟ)
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
    compl_ଅଙ୍କ_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    ଯଦି (match == ଖାଲି)
	ଫେରନ୍ତୁ;

    p = vim_strnsave(str, len);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.length = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ଶୂନ୍ୟ
ins_compl_addfrommatch(ଶୂନ୍ୟ)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    p = compl_shown_match->cp_str.string;
    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_shown_match->cp_str.length <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	ଯଦି (!match_at_original_text(compl_shown_match))
	    ଫେରନ୍ତୁ;

	p = ଖାଲି;
	plen = 0;
	ପାଇଁ (cp = compl_shown_match->cp_next; cp != ଖାଲି
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    ଯଦି (compl_leader.string == ଖାଲି
		    || ins_compl_equal(cp, compl_leader.string,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_leader.length))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.length;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (p == ଖାଲି || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen <= len)
	    ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ctrl_x_mode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FALSE;

    ବାଛ (c)
    {
	ସ୍ଥିତି Ctrl_E:
	ସ୍ଥିତି Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    ଯଦି (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    ଅନ୍ୟଥା
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ଖାଲି;
	    showmode();
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_ଧାଡ଼ି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    ଯଦି (vpeekc() == '=')
		ଭାଙ୍ଗ;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    ଭାଙ୍ଗ;
#ifdef FEAT_COMPL_FUNC
	ସ୍ଥିତି Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି 's':
	ସ୍ଥିତି Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    ଭାଙ୍ଗ;
#ifdef FEAT_FIND_ID
	ସ୍ଥିତି Ctrl_I:
	ସ୍ଥିତି K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି Ctrl_V:
	ସ୍ଥିତି Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ଖାଲି;
	    showmode();
	    retval = TRUE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି Ctrl_P:
	ସ୍ଥିତି Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    ଯଦି (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    ଅନ୍ୟଥା ଯଦି (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
	    ଯଦି (c == Ctrl_X)
	    {
		ଯଦି (compl_cont_mode != 0)
		    compl_cont_status = 0;
		ଅନ୍ୟଥା
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ଖାଲି;
	    showmode();
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    ସ୍ଥିର ଶୂନ୍ୟ
trigger_complete_done_event(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode UNUSED, string_T *word UNUSED)
{
#ଯଦି defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    ଯଦି (word == ଖାଲି || word->string == ଖାଲି)
	(ଶୂନ୍ୟ)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)dict_add_string_len(v_event, "complete_word", word->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)word->length);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (ଶୂନ୍ୟ)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == ଖାଲି) ? (char_u *)"" : mode_str->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mode_str->length);

    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#ଯଦି defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_stop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		want_cindent;
    string_T	word = {ଖାଲି, 0};

    // Remove pre-inserted text when present.
    ଯଦି (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    ଯଦି (compl_curr_match != ଖାଲି || compl_leader.string != ଖାଲି || c == Ctrl_E)
    {
	char_u	*ptr = ଖାଲି;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	ଯଦି (compl_curr_match != ଖାଲି && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    ଯଦି (compl_cont_mode == CTRL_X_WHOLE_ଧାଡ଼ି)
    {
	// re-indent the current line
	ଯଦି (want_cindent)
	{
	    do_c_ପଦସମଷ୍ଟି_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    ଅନ୍ୟଥା ଯଦି (!compl_autocomplete || compl_used_match)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	ଯଦି (prev_col > 0)
	    dec_cursor();
	// only format when something was inserted
	ଯଦି (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	ଯଦି (prev_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    ଯଦି ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.length);
	ଯଦି (word.string == ଖାଲି)
	    word.length = 0;
	ଅନ୍ୟଥା
	    word.length = compl_shown_match->cp_str.length;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    ଯଦି (c == Ctrl_E)
    {
	char_u *p = ଖାଲି;
	size_t	plen = 0;

	ins_compl_delete();
	ଯଦି (compl_leader.string != ଖାଲି)
	{
	    p = compl_leader.string;
	    plen = compl_leader.length;
	}
	ଅନ୍ୟଥା ଯଦି (compl_first_match != ଖାଲି)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.length;
	}
	ଯଦି (p != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    compl_len = get_compl_len();

	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen > compl_len)
		ins_compl_ଭର୍ତ୍ତି_bytes(p + compl_len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen - compl_len);
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
    ଯଦି (!shortmess(SHM_COMPLETIONMENU))
	ସନ୍ଦେଶ_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    ଯଦି (edit_submode != ଖାଲି)
    {
	edit_submode = ଖାଲି;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ଅଙ୍କ_bests = 0;
    compl_ins_end_col = 0;

    ଯଦି (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    ଯଦି (want_cindent && in_cinkeys(ଚାବି_COMPLETE, ' ', inindent(0)))
	do_c_ପଦସମଷ୍ଟି_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(prev_mode, &word);
    vim_free(word.string);

    ଫେରନ୍ତୁ retval;
}

/*
 * Cancel completion.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_cancel(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_prep(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    ଯଦି (c != Ctrl_R && vim_is_ctrl_x_ଚାବି(c))
	edit_submode_extra = ଖାଲି;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    ଯଦି (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	ଫେରନ୍ତୁ retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    ଯଦି (is_mouse_ଚାବି(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	ଯଦି (c == K_LEFTRELEASE
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
	    ଫେରନ୍ତୁ retval;
	ଯଦି (popup_visible)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row = mouse_row;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    ଯଦି (wp != ଖାଲି && WIN_IS_POPUP(wp))
		ଫେରନ୍ତୁ retval;
	}
    }
#endif

    ଯଦି (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	ଯଦି (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_ଚାବି(c)
		|| !vim_is_ctrl_x_ଚାବି(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    ଯଦି (c == Ctrl_Z)
		retval = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    ଯଦି (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    ଯଦି (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	ଯଦି (c != Ctrl_R && !vim_is_ctrl_x_ଚାବି(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = ଖାଲି;
	}
	showmode();
    }

    ଯଦି (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	ଯଦି ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_ଚାବି(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, prev_mode, retval);
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode == CTRL_X_LOCAL_ସନ୍ଦେଶ)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, ଖାଲି);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    ଯଦି (c != Ctrl_R && !vim_is_ctrl_x_ଚାବି(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    ଯଦି (ptr == ଖାଲି)
    {
	ଯଦି (compl_leader.string != ଖାଲି)
	    ptr = compl_leader.string;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ;  // nothing to do
    }
    ଯଦି (compl_orig_text.string != ଖାଲି)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	ଯେତେବେଳେ (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	ଯଦି (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	ପାଇଁ (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    ଯଦି (ptr != ଖାଲି)
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
    ସ୍ଥିର buf_T *
ins_compl_next_buf(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ସ୍ଥିର win_T    *wp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    skip_buffer;

    ଯଦି (flag == 'w')		// just windows
    {
	ଯଦି (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	ଯେତେବେଳେ (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_next != ଖାଲି) ? wp->w_next : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    ଯଦି (wp == curwin || !wp->w_buffer->b_scanned)
		ଭାଙ୍ଗ;
	}
	buf = wp->w_buffer;
    }
    ଅନ୍ୟଥା
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	ଯେତେବେଳେ (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_next != ଖାଲି) ? buf->b_next : firstbuf;
	    // Break if we're back at start buffer
	    ଯଦି (buf == curbuf)
		ଭାଙ୍ଗ;

	    // Check buffer conditions based on flag
	    ଯଦି (flag == 'U')
		skip_buffer = buf->b_p_bl;
	    ଅନ୍ୟଥା
		skip_buffer = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ଖାଲି) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    ଯଦି (!skip_buffer && !buf->b_scanned)
		ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cpt_sources_ଗଣନା(char_u *cpt)
{
    char_u  dummy[LSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2 = 0;
    char_u  *p;

    ପାଇଁ (p = cpt; *p != NUL; )
    {
	ଯେତେବେଳେ (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	ଯଦି (*p != NUL)
	{
	    (ଶୂନ୍ୟ)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    ଗଣନା_2++;
	}
    }

    ଫେରନ୍ତୁ ଗଣନା_2;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
ସ୍ଥିର callback_T cfu_cb;	    // 'completefunc' callback function
ସ୍ଥିର callback_T ofu_cb;	    // 'omnifunc' callback function
ସ୍ଥିର callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
ସ୍ଥିର callback_T *cpt_cb;	    // Callback functions associated with F{func}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  cpt_cb_ଗଣନା;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    ଯଦି (globcb->cb_name != ଖାଲି && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ଅକ୍ଷର *
did_set_completefunc(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval;

    ଯଦି (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    ଅନ୍ୟଥା
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	ଯଦି (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    ଫେରନ୍ତୁ retval == FAIL ? e_invalid_argument : ଖାଲି;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    ଶୂନ୍ୟ
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
    ଅକ୍ଷର *
did_set_omnifunc(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval;

    ଯଦି (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    ଅନ୍ୟଥା
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	ଯଦି (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    ଫେରନ୍ତୁ retval == FAIL ? e_invalid_argument : ଖାଲି;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    ଶୂନ୍ୟ
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    ଶୂନ୍ୟ
clear_cpt_callbacks(callback_T **callbacks, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ଯଦି (callbacks == ଖାଲି || *callbacks == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_cpt_callbacks(callback_T **dest, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dest_cnt, callback_T *src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    ଯଦି (cnt == 0)
	ଫେରନ୍ତୁ OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    ଯଦି (*dest == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    *dest_cnt = cnt;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cnt; i++)
	ଯଦି (src[i].cb_name != ଖାଲି && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    ଫେରନ୍ତୁ OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    ଶୂନ୍ୟ
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    (ଶୂନ୍ୟ)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_ଗଣନା, cpt_cb,
	    cpt_cb_ଗଣନା);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    ଗଣନା_2 = get_cpt_sources_ଗଣନା(cpt);
    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, ଗଣନା_2);
    ଯଦି (*cbp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    *cnt = ଗଣନା_2;

    ପାଇଁ (p = cpt; *p != NUL; )
    {
	ଯେତେବେଳେ (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	ଯଦି (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ଯଦି (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		ଯଦି (caret != ଖାଲି)
		    *caret = NUL;

		ଯଦି (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ଖାଲି;
	    }
	    idx++;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_cpt_callbacks(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_flags = args->os_flags;

    ଯଦି (curbuf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // ":setglobal" does not change the buffer-local option value
    ଯଦି (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_ଗଣନା) != OK)
	ଫେରନ୍ତୁ FAIL;

    // ":setlocal" does not change the global option value
    ଯଦି (opt_flags & OPT_LOCAL)
	ଫେରନ୍ତୁ OK;

    // Cache the callbacks for the global value
    ଯଦି (opt_flags & OPT_GLOBAL)
	ଫେରନ୍ତୁ parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_ଗଣନା);

    // set case
    ଫେରନ୍ତୁ copy_cpt_callbacks(&cpt_cb, &cpt_cb_ଗଣନା, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_ଗଣନା);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ଅକ୍ଷର *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval;

    ଯଦି (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    ଅନ୍ୟଥା
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	ଯଦି (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    ଫେରନ୍ତୁ retval == FAIL ? e_invalid_argument : ଖାଲି;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_cpt_callbacks(callback_T *callbacks, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	abort = FALSE;

    ଯଦି (callbacks == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    ଫେରନ୍ତୁ abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_insexpand_funcs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_ଗଣନା, copyID);

    ଫେରନ୍ତୁ abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    ସ୍ଥିର char_u *
get_complete_funcname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ବାଛ (type)
    {
	ସ୍ଥିତି CTRL_X_FUNCTION:
	    ଫେରନ୍ତୁ curbuf->b_p_cfu;
	ସ୍ଥିତି CTRL_X_OMNI:
	    ଫେରନ୍ତୁ curbuf->b_p_ofu;
	ସ୍ଥିତି CTRL_X_THESAURUS:
	    ଫେରନ୍ତୁ *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    ସ୍ଥିର callback_T *
get_ଭର୍ତ୍ତି_callback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ଯଦି (type == CTRL_X_FUNCTION)
	ଫେରନ୍ତୁ &curbuf->b_cfu_cb;
    ଯଦି (type == CTRL_X_OMNI)
	ଫେରନ୍ତୁ &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    ଫେରନ୍ତୁ (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
expand_by_function(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = ଖାଲି;
    dict_T	*matchdict = ଖାଲି;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State = State;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cpt_function = (cb != ଖାଲି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ଯଦି (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	ଯଦି (*funcname == NUL)
	    ଫେରନ୍ତୁ;
	cb = get_ଭର୍ତ୍ତି_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_ସଂଖ୍ୟା;
    args[0].vval.v_ସଂଖ୍ୟା = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != ଖାଲି ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    ଯଦି (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    ଯଦି (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    ଯଦି (retval == OK)
    {
	ବାଛ (rettv.v_type)
	{
	    ସ୍ଥିତି VAR_LIST:
		matchlist = rettv.vval.v_list;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_DICT:
		matchdict = rettv.vval.v_dict;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_SPECIAL:
		ଯଦି (rettv.vval.v_ସଂଖ୍ୟା == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// TODO: Give error message?
		clear_tv(&rettv);
		ଭାଙ୍ଗ;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ଯଦି (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	ଯାଅ theend;
    }

    ଯଦି (matchlist != ଖାଲି)
	ins_compl_add_list(matchlist);
    ଅନ୍ୟଥା ଯଦି (matchdict != ଖାଲି)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    ଯଦି (matchdict != ଖାଲି)
	dict_unref(matchdict);
    ଯଦି (matchlist != ଖାଲି)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#ଯଦି defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_user_highlight_id(char_u *hlname)
{
    ଯଦି (hlname != ଖାଲି && *hlname != NUL)
	ଫେରନ୍ତୁ syn_check_group(hlname, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(hlname));
    ଫେରନ୍ତୁ 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_add_tv(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fast)
{
    char_u	*word;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dup = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_ଗଣନା]);
    typval_T	user_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		user_hl[2] = { 0, 0 };

    user_data.v_type = VAR_UNKNOWN;
    ଯଦି (tv->v_type == VAR_DICT && tv->vval.v_dict != ଖାଲି)
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
	ଯଦି (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ଖାଲି
				  && dict_get_ସଂଖ୍ୟା(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	ଯଦି (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ଖାଲି)
	    dup = dict_get_ସଂଖ୍ୟା(tv->vval.v_dict, "dup");
	ଯଦି (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ଖାଲି)
	    empty = dict_get_ସଂଖ୍ୟା(tv->vval.v_dict, "empty");
	ଯଦି (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ଖାଲି
				  && dict_get_ସଂଖ୍ୟା(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    ଅନ୍ୟଥା
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    ଯଦି (word == ଖାଲି || (!empty && *word == NUL))
    {
	clear_tv(&user_data);
	ଫେରନ୍ତୁ FAIL;
    }
    status = ins_compl_add(word, -1, ଖାଲି, cptext,
	    &user_data, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    ଯଦି (status != OK)
	clear_tv(&user_data);
    ଫେରନ୍ତୁ status;
}

/*
 * Add completions from a list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
    {
	ଯଦି (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	ଅନ୍ୟଥା ଯଦି (did_emsg)
	    ଭାଙ୍ଗ;
    }
}

/*
 * Add completions from a dict.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_ଶବ୍ଦଗୁଡ଼ିକ;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    ଯଦି (di_refresh != ଖାଲି && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	ଯଦି (v != ଖାଲି && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_ଶବ୍ଦଗୁଡ଼ିକ = dict_find(dict, (char_u *)"words", 5);
    ଯଦି (di_ଶବ୍ଦଗୁଡ଼ିକ != ଖାଲି && di_ଶବ୍ଦଗୁଡ଼ିକ->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_ଶବ୍ଦଗୁଡ଼ିକ->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_completion(colnr_T startcol, list_T *list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_w_wrow = curwin->w_wrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_w_leftcol = curwin->w_leftcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = CP_ORIGINAL_TEXT;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_cot_flags = get_cot_flags();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_no_ଭର୍ତ୍ତି = (cur_cot_flags & COT_NOINSERT) != 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    ଯଦି (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    ଯଦି (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_length);
    ଯଦି (p_ic)
	flags |= CP_ICASE;
    ଯଦି (compl_orig_text.string == ଖାଲି)
    {
	compl_orig_text.length = 0;
	ଫେରନ୍ତୁ;
    }
    compl_orig_text.length = (size_t)compl_length;
    ଯଦି (ins_compl_add(compl_orig_text.string,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_orig_text.length, ଖାଲି, ଖାଲି, ଖାଲି, 0,
			flags | CP_FAST, FALSE, ଖାଲି, FUZZY_SCORE_NONE) != OK)
	ଫେରନ୍ତୁ;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_select = compl_no_select || compl_longest;
    ଯଦି (compl_no_ଭର୍ତ୍ତି || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	ଯଦି (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    ଅନ୍ୟଥା
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_ଭର୍ତ୍ତି;

    // Lazily show the popup menu, unless we got interrupted.
    ଯଦି (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    ଶୂନ୍ୟ
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି ((State & MODE_ଭର୍ତ୍ତି) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_ଭର୍ତ୍ତି_mode));
	ଫେରନ୍ତୁ;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    ଯଦି (!undo_allowed())
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି);
	ଯଦି (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    ଶୂନ୍ୟ
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    ଶୂନ୍ୟ
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_ଚାବିଗୁଡ଼ିକ(0, TRUE);
    rettv->vval.v_ସଂଖ୍ୟା = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_mode(string_T *ret)
{
    ଯଦି (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == ଖାଲି) ? (char_u *)"" : mode->string;
	ret->length = mode->length;
    }
    ଅନ୍ୟଥା
    {
	ret->string = (char_u *)"";
	ret->length = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_update_sequence_numbers(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସଂଖ୍ୟା = 0;
    compl_T	*match;

    ଯଦି (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	ପାଇଁ (match = compl_curr_match->cp_prev; match != ଖାଲି
		&& !is_first_match(match); match = match->cp_prev)
	    ଯଦି (match->cp_ସଂଖ୍ୟା != -1)
	    {
		ସଂଖ୍ୟା = match->cp_ସଂଖ୍ୟା;
		ଭାଙ୍ଗ;
	    }
	ଯଦି (match != ଖାଲି)
	    // go up and assign all numbers which are not assigned yet
	    ପାଇଁ (match = match->cp_next;
		    match != ଖାଲି && match->cp_ସଂଖ୍ୟା == -1;
					   match = match->cp_next)
		match->cp_ସଂଖ୍ୟା = ++ସଂଖ୍ୟା;
    }
    ଅନ୍ୟଥା // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	ପାଇଁ (match = compl_curr_match->cp_next; match != ଖାଲି
		&& !is_first_match(match); match = match->cp_next)
	{
	    ଯଦି (match->cp_ସଂଖ୍ୟା != -1)
	    {
		ସଂଖ୍ୟା = match->cp_ସଂଖ୍ୟା;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (match != ଖାଲି)
	{
	    // go down and assign all numbers which are not assigned yet
	    ପାଇଁ (match = match->cp_prev; match
		    && match->cp_ସଂଖ୍ୟା == -1;
					   match = match->cp_prev)
		match->cp_ସଂଖ୍ୟା = ++ସଂଖ୍ୟା;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fill_complete_info_dict(dict_T *di, compl_T *match, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)match->cp_str.length);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    ଯଦି (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    ଯଦି (match->cp_user_data.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    ଅନ୍ୟଥା
	dict_add_tv(di, "user_data", &match->cp_user_data);
}

/*
 * Get complete information
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_complete_info(list_T *what_list, dict_T *retdict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_ବସ୍ତୁଗୁଡ଼ିକ		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what_flag;

    ଯଦି (what_list == ଖାଲି)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    ଅନ୍ୟଥା
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    ଯଦି (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ବସ୍ତୁଗୁଡ଼ିକ;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    ଯଦି (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mode.length);
    }

    ଯଦି (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_ସଂଖ୍ୟା(retdict, "pum_visible", pum_visible());

    ଯଦି (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_ସଂଖ୍ୟା(retdict, "auto", compl_autostarted);

    ଯଦି (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*ଧାଡ଼ି = ml_get_curline();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? ଧାଡ଼ି + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    ଯଦି (ret == OK && (what_flag & (CI_WHAT_ବସ୍ତୁଗୁଡ଼ିକ | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ଖାଲି;
	dict_T	    *di;
	compl_T     *match;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା         selected_idx = -1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_ବସ୍ତୁଗୁଡ଼ିକ = what_flag & CI_WHAT_ବସ୍ତୁଗୁଡ଼ିକ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_matches = what_flag & CI_WHAT_MATCHES;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_completed = what_flag & CI_WHAT_COMPLETED;

	ଯଦି (has_ବସ୍ତୁଗୁଡ଼ିକ || has_matches)
	{
	    li = list_alloc();
	    ଯଦି (li == ଖାଲି)
		ଫେରନ୍ତୁ;
	    ret = dict_add_list(retdict, (has_matches && !has_ବସ୍ତୁଗୁଡ଼ିକ)
						? "matches" : "items", li);
	    ଯଦି (ret == FAIL)
		list_unref(li);
	}
	ଯଦି (ret == OK && what_flag & CI_WHAT_SELECTED)
	    ଯଦି (compl_curr_match != ଖାଲି && compl_curr_match->cp_ସଂଖ୍ୟା == -1)
		ins_compl_update_sequence_numbers();
	ଯଦି (ret == OK && compl_first_match != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା list_idx = 0;
	    match = compl_first_match;
	    କର
	    {
		ଯଦି (!match_at_original_text(match))
		{
		    ଯଦି (has_ବସ୍ତୁଗୁଡ଼ିକ
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			ଯଦି (di == ଖାଲି)
			    ଫେରନ୍ତୁ;
			ret = list_append_dict(li, di);
			ଯଦି (ret != OK)
			{
			    dict_unref(di);
			    ଫେରନ୍ତୁ;
			}
			fill_complete_info_dict(di, match, has_matches && has_ବସ୍ତୁଗୁଡ଼ିକ);
		    }
		    ଯଦି (compl_curr_match != ଖାଲି
			    && compl_curr_match->cp_ସଂଖ୍ୟା == match->cp_ସଂଖ୍ୟା)
			selected_idx = list_idx;
		    ଯଦି (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_next;
	    }
	    ଯେତେବେଳେ (match != ଖାଲି && !is_first_match(match));
	}
	ଯଦି (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_ସଂଖ୍ୟା(retdict, "selected", selected_idx);

	ଯଦି (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    ଯଦି (di == ଖାଲି)
		ଫେରନ୍ତୁ;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    ଯଦି (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    ଶୂନ୍ୟ
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ଖାଲି;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	    ଫେରନ୍ତୁ;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
thesaurus_func_complete(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    ଫେରନ୍ତୁ type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_advance_cpt_index(char_u *cpt)
{
    char_u  *p = cpt;

    ଯଦି (cpt_sources_index == -1)
	ଫେରନ୍ତୁ FALSE;
    ଯେତେବେଳେ (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    ଫେରନ୍ତୁ (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
ଗଣନା
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	prev_match_pos;		// previous match position
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dict_f;			// "dict" is an exact file name or not
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
process_next_cpt_ମୂଲ୍ୟ(
	ins_compl_next_state_T *st,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*compl_type_arg,
	pos_T		*start_match_pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy_collect,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*advance_cpt_idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    compl_type = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    status = INS_COMPL_CPT_OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    ଯେତେବେଳେ (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    ଯଦି (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	ଯଦି (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ଖାଲି
	    && (st->ins_buf = ins_compl_next_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	ଯଦି (st->ins_buf->b_ml.ml_mfp != ଖାଲି)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	ଅନ୍ୟଥା	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    ଯଦି (st->ins_buf->b_fname == ଖାଲି)
	    {
		status = INS_COMPL_CPT_CONT;
		ଯାଅ done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	ଯଦି (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    ସନ୍ଦେଶ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ଖାଲି
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ଖାଲି
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_trunc_attr((ଅକ୍ଷର *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    ଅନ୍ୟଥା ଯଦି (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    ଅନ୍ୟଥା
    {
	ଯଦି (ctrl_x_mode_ଧାଡ଼ି_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	ଅନ୍ୟଥା ଯଦି (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_index);
	    ଯଦି (!st->func_cb)
		compl_type = -1;
	}
#endif
	ଅନ୍ୟଥା ଯଦି (!skip_source)
	{
	    ଯଦି (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		ଯଦି (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		ଅନ୍ୟଥା
		    compl_type = CTRL_X_THESAURUS;
		ଯଦି (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    ଅନ୍ୟଥା ଯଦି (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    ଅନ୍ୟଥା ଯଦି (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    ଅନ୍ୟଥା ଯଦି (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		ଯଦି (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    ସନ୍ଦେଶ_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Scanning tags."));
		    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_trunc_attr((ଅକ୍ଷର *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    ଅନ୍ୟଥା
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(ଶୂନ୍ୟ)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_index(st->e_cpt);

	st->found_all = TRUE;
	ଯଦି (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    ଫେରନ୍ତୁ status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_next_include_file_completion(ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_pattern.length, FALSE, FALSE,
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
    ସ୍ଥିର ଶୂନ୍ୟ
get_next_dict_tsr_completion(ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_type, char_u *dict, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dict_f)
{
#ifdef FEAT_COMPL_FUNC
    ଯଦି (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, ଖାଲି);
    ଅନ୍ୟଥା
#endif
	ins_compl_dictionaries(
		dict != ଖାଲି ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != ଖାଲି ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_next_tag_completion(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ic;
    char_u	**matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    ଯଦି (find_tags(compl_pattern.string, &ଅଙ୍କ_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && ଅଙ୍କ_matches > 0)
	ins_compl_add_matches(ଅଙ୍କ_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compare_scores(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_a = *(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)a;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_b = *(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)b;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା score_a = compl_fuzzy_scores[idx_a];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା score_b = compl_fuzzy_scores[idx_b];
    ଫେରନ୍ତୁ score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_longest_ଭର୍ତ୍ତି(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_ଭର୍ତ୍ତି_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fuzzy_longest_match(ଶୂନ୍ୟ)
{
    char_u	*prefix = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefix_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j = 0;
    char_u	*match_str = ଖାଲି;
    char_u	*prefix_ptr = ଖାଲି;
    char_u	*match_ptr = ଖାଲି;
    char_u	*leader = ଖାଲି;
    size_t	leader_len = 0;
    compl_T	*compl = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		more_candidates = FALSE;
    compl_T	*nn_compl = ଖାଲି;

    ଯଦି (compl_ଅଙ୍କ_bests == 0)
	ଫେରନ୍ତୁ;

    nn_compl = compl_first_match->cp_next->cp_next;
    ଯଦି (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_ଧାଡ଼ି() ? compl_first_match
				    : compl_first_match->cp_next;
    ଯଦି (compl_ଅଙ୍କ_bests == 1)
    {
	// no more candidates insert the match str
	ଯଦି (!more_candidates)
	{
	    ins_compl_longest_ଭର୍ତ୍ତି(compl->cp_str.string);
	    compl_ଅଙ୍କ_bests = 0;
	}
	compl_ଅଙ୍କ_bests = 0;
	ଫେରନ୍ତୁ;
    }

    ଯଦି ((size_t)compl_ଅଙ୍କ_bests > SIZE_MAX / ଆକାର(compl_T *))
	ଫେରନ୍ତୁ;
    compl_best_matches = (compl_T **)alloc(compl_ଅଙ୍କ_bests * ଆକାର(compl_T *));
    ଯଦି (compl_best_matches == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯେତେବେଳେ (compl != ଖାଲି && i < compl_ଅଙ୍କ_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_next;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_best_matches[0]->cp_str.length;

    ପାଇଁ (i = 1; i < compl_ଅଙ୍କ_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	ଯେତେବେଳେ (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    ଯଦି (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		ଭାଙ୍ଗ;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	ଯଦି (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    ଯଦି (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	ଯାଅ end;

    prefix = vim_strnsave(prefix, prefix_len);
    ଯଦି (prefix != ଖାଲି)
    {
	ins_compl_longest_ଭର୍ତ୍ତି(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = ଖାଲି;
    compl_ଅଙ୍କ_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_next_filename_completion(ଶୂନ୍ୟ)
{
    char_u	**matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fuzzy_indices_data;
    char_u	*last_sep = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_score = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_score = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    ଅକ୍ଷର pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#ଅନ୍ୟଥା
    ଅକ୍ଷର pathsep = PATHSEP;
#endif

    ଯଦି (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	ଯଦି (curbuf->b_p_csl[0] == 's')
	{
	    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader_len; i++)
	    {
		ଯଦି (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	ଅନ୍ୟଥା ଯଦି (curbuf->b_p_csl[0] == 'b')
	{
	    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader_len; i++)
	    {
		ଯଦି (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	ଯଦି (last_sep == ଖାଲି)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    ଯଦି (compl_pattern.string == ଖାଲି)
		ଫେରନ୍ତୁ;
	    compl_pattern.length = 1;
	}
	ଅନ୍ୟଥା ଯଦି (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	ଅନ୍ୟଥା
	{
	    // Split leader into path and file parts
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    ଯଦି (path_with_wildcard != ଖାଲି)
	    {
		vim_snprintf((ଅକ୍ଷର *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.length = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    ଯଦି (expand_wildcards(1, &compl_pattern.string, &ଅଙ୍କ_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	ଫେରନ୍ତୁ;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, ଅଙ୍କ_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    ଯଦି (curbuf->b_p_csl[0] != NUL)
    {
	ପାଇଁ (i = 0; i < ଅଙ୍କ_matches; ++i)
	{
	    ptr = matches[i];
	    ଯେତେବେଳେ (*ptr != NUL)
	    {
		ଯଦି (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ଅନ୍ୟଥା ଯଦି (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    ଯଦି (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 10);
	compl_fuzzy_scores = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)alloc(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * ଅଙ୍କ_matches);

	ପାଇଁ (i = 0; i < ଅଙ୍କ_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    ଯଦି (score != FUZZY_SCORE_NONE)
	    {
		ଯଦି (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)fuzzy_indices.ga_data)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	ଯଦି (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = ଖାଲି;
	    fuzzy_indices_data = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)fuzzy_indices.ga_data;
	    qsort(fuzzy_indices_data, fuzzy_indices.ga_len, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), compare_scores);

	    ପାଇଁ (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_data[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_data[i]];
		ଯଦି (ins_compl_add(match, -1, ଖାଲି, ଖାଲି, ଖାଲି, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ଖାଲି, current_score) == OK)
		    dir = FORWARD;

		ଯଦି (need_collect_bests)
		{
		    ଯଦି (i == 0 || current_score == max_score)
		    {
			compl_ଅଙ୍କ_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(ଅଙ୍କ_matches, matches);
	}
	ଅନ୍ୟଥା ଯଦି (leader_len > 0)
	{
	    FreeWild(ଅଙ୍କ_matches, matches);
	    ଅଙ୍କ_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	ଯଦି (compl_ଅଙ୍କ_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ଅଙ୍କ_matches > 0)
	ins_compl_add_matches(ଅଙ୍କ_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_next_cmdline_completion(ଶୂନ୍ୟ)
{
    char_u	**matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches;

    ଯଦି (expand_cmdline(&compl_xp, compl_pattern.string,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_pattern.length, &ଅଙ୍କ_matches, &matches) == EXPAND_OK)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_r = OK;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = compl_direction;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_ଗଣନା]) = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};

	    ଯଦି (compl_xp.xp_files_abbr != ଖାଲି)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    ଯଦି (compl_xp.xp_files_kind != ଖାଲି)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    ଯଦି (compl_xp.xp_files_menu != ଖାଲି)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    ଯଦି (compl_xp.xp_files_info != ଖାଲି)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, ଖାଲି, cptext, ଖାଲି, dir,
		    CP_FAST, FALSE, ଖାଲି, FUZZY_SCORE_NONE);
	    ଯଦି (add_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(ଅଙ୍କ_matches, matches);
	free_xp_files_extra(&compl_xp, ଅଙ୍କ_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches;

    ଅଙ୍କ_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    ଯଦି (ଅଙ୍କ_matches > 0)
	ins_compl_add_matches(ଅଙ୍କ_matches, matches, p_ic);
    ଅନ୍ୟଥା
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    ସ୍ଥିର char_u *
ins_compl_get_next_word_or_ଧାଡ଼ି(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*match_len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    ଯଦି (ctrl_x_mode_ଧାଡ଼ି_or_eval())
    {
	ଯଦି (compl_status_adding())
	{
	    ଯଦି (cur_match_pos->lnum >= ins_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଫେରନ୍ତୁ ଖାଲି;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    ଯଦି (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - tmp_ptr);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	char_u	*tmp_ptr = ptr;

	ଯଦି (compl_status_adding() && compl_length <= len)
	{
	    tmp_ptr += compl_length;
	    // Skip if already inside a word.
	    ଯଦି (vim_iswordp(tmp_ptr))
		ଫେରନ୍ତୁ ଖାଲି;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tmp_ptr - ptr);

	ଯଦି (compl_status_adding() && len == compl_length)
	{
	    ଯଦି (cur_match_pos->lnum < ins_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
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
		ଯଦି (tmp_ptr > ptr)
		{
		    ଯଦି (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			ଯଦି (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			ଯଦି (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ଖାଲି
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    ଯଦି (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    ଯଦି (len == compl_length)
		ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    *match_len = len;
    ଫେରନ୍ତୁ ptr;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_new_match = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_scs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ws;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		looped_around = FALSE;
    char_u	*ptr = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_length > 0;
    char_u	*leader = ins_compl_leader();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = FUZZY_SCORE_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ଯଦି (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    ଯଦି (!in_curbuf)
	p_ws = FALSE;
    ଅନ୍ୟଥା ଯଦି (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cont_s_ipos = FALSE;

	++ସନ୍ଦେଶ_silent;  // Don't want messages for wrapscan.

	ଯଦି (in_fuzzy_collect)
	{
	    found_new_match = ସନ୍ଧାନ_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_whole_ଧାଡ଼ି() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = ସନ୍ଧାନ_for_exact_ଧାଡ଼ି(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	ଅନ୍ୟଥା
	    found_new_match = searchit(ଖାଲି, st->ins_buf, st->cur_match_pos,
				ଖାଲି, compl_direction, compl_pattern.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_pattern.length,
				1L, ସନ୍ଧାନ_KEEP + ସନ୍ଧାନ_NFMSG, RE_LAST, ଖାଲି);
	--ସନ୍ଦେଶ_silent;
	ଯଦି (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    ଯଦି (looped_around)
		found_new_match = FAIL;
	    ଅନ୍ୟଥା
		looped_around = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    ଯଦି (looped_around)
		found_new_match = FAIL;
	    ଅନ୍ୟଥା
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	ଯଦି (found_new_match == FAIL)
	    ଭାଙ୍ଗ;

	// when ADDING, the text before the cursor matches, skip it
	ଯଦି (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    ଜାରି;

	ଯଦି (!in_fuzzy_collect)
	    ptr = ins_compl_get_next_word_or_ଧାଡ଼ି(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	ଯଦି (ptr == ଖାଲି || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    ଜାରି;

	ଯଦି (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    ଯଦି (score < 0)
		score = -score;
	}

	ଯଦି (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? ଖାଲି : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    ଯଦି (in_fuzzy_collect && score == compl_first_match->cp_next->cp_score)
		compl_ଅଙ୍କ_bests++;
	    found_new_match = OK;
	    ଭାଙ୍ଗ;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    ଫେରନ୍ତୁ found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    ସ୍ଥିର callback_T *
get_callback_if_cpt_func(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (*p == 'o')
	ଫେରନ୍ତୁ &curbuf->b_ofu_cb;

    ଯଦି (*p == 'F')
    {
	ଯଦି (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    ଯଦି (curbuf->b_p_cpt_cb == ଖାଲି
		    || idx < 0 || idx >= curbuf->b_p_cpt_ଗଣନା)
		ଫେରନ୍ତୁ ଖାଲି;
	    ଫେରନ୍ତୁ curbuf->b_p_cpt_cb[idx].cb_name != ଖାଲି
		? &curbuf->b_p_cpt_cb[idx] : ଖାଲି;
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ &curbuf->b_cfu_cb; // 'cfu'
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_register_completion(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = compl_direction;
    yankreg_T	*reg = ଖାଲି;
    ଶୂନ୍ୟ	*reg_ptr = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		adding_mode = compl_status_adding();

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_REGISTERS; i++)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname = 0;
	ଯଦି (i == 0)
	    regname = '"';    // unnamed register
	ଅନ୍ୟଥା ଯଦି (i < 10)
	    regname = '0' + i;
	ଅନ୍ୟଥା ଯଦି (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	ଅନ୍ୟଥା ଯଦି (i == STAR_REGISTER)
	    regname = '*';
	ଅନ୍ୟଥା ଯଦି (i == PLUS_REGISTER)
	    regname = '+';
#endif
	ଅନ୍ୟଥା
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	ଯଦି (!valid_yank_reg(regname, FALSE) || regname == '_')
	    ଜାରି;

	reg_ptr = get_register(regname, FALSE);
	ଯଦି (reg_ptr == ଖାଲି)
	    ଜାରି;

	reg = (yankreg_T *)reg_ptr;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < reg->y_size; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    ଯଦି (str == ଖାଲି)
		ଜାରି;

	    ଯଦି (adding_mode)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
		ଯଦି (str_len == 0)
		    ଜାରି;

		ଯଦି (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0))
		{
		    ଯଦି (ins_compl_add_infercase(str, str_len, p_ic, ଖାଲି,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		ଯେତେବେଳେ (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    ଯଦି (p >= str_end || *p == NUL)
			ଭାଙ୍ଗ;

		    char_u *word_end = find_word_end(p);

		    ଯଦି (word_end <= p)
		    {
			ଯଦି (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			ଅନ୍ୟଥା
			    word_end = p + 1;
		    }

		    ଯଦି (word_end > str_end)
			word_end = str_end;

		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(word_end - p);
		    ଯଦି (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0)))
		    {
			ଯଦି (ins_compl_add_infercase(p, len, p_ic, ଖାଲି,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    ଯଦି (p <= old_p)
		    {
			p = old_p + 1;
			ଯଦି (has_mbyte && p < str_end)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_next_completion_match(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ins_compl_next_state_T *st, pos_T *ini)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	found_new_match = FALSE;

    ବାଛ (type)
    {
	ସ୍ଥିତି -1:
	    ଭାଙ୍ଗ;
#ifdef FEAT_FIND_ID
	ସ୍ଥିତି CTRL_X_PATH_PATTERNS:
	ସ୍ଥିତି CTRL_X_PATH_DEFINES:
	    get_next_include_file_completion(type);
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି CTRL_X_DICTIONARY:
	ସ୍ଥିତି CTRL_X_THESAURUS:
	    get_next_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ଖାଲି;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CTRL_X_TAGS:
	    get_next_tag_completion();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CTRL_X_FILES:
	    get_next_filename_completion();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CTRL_X_CMDLINE:
	ସ୍ଥିତି CTRL_X_CMDLINE_CTRL_X:
	    get_next_cmdline_completion();
	    ଭାଙ୍ଗ;

#ifdef FEAT_COMPL_FUNC
	ସ୍ଥିତି CTRL_X_FUNCTION:
	    ଯଦି (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    ଅନ୍ୟଥା
		expand_by_function(type, compl_pattern.string, ଖାଲି);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, ଖାଲି);
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି CTRL_X_SPELL:
	    get_next_spell_completion(st->first_match_pos.lnum);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CTRL_X_REGISTER:
	    get_register_completion();
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	// normal ^P/^N and ^X^L
	    found_new_match = get_next_default_completion(st, ini);
	    ଯଦି (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    ଯଦି (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    ଫେରନ୍ତୁ found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *ପଠ = str, *ଲିଖ = str, *p;

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (*ପଠ)
    {
	ଯଦି (*ପଠ == '^')
	{
	    p = ପଠ + 1;
	    ଯେତେବେଳେ (vim_isdigit(*p))
		p++;
	    ଯଦି ((*p == ',' || *p == '\0') && p != ପଠ + 1)
	    {
		ପଠ = p;
		ଜାରି;
	    }
	    ଅନ୍ୟଥା
		*ଲିଖ++ = *ପଠ++;
	}
	ଅନ୍ୟଥା
	    *ଲିଖ++ = *ପଠ++;
    }
    *ଲିଖ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prepare_cpt_compl_funcs(ଶୂନ୍ୟ)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    ଯଦି (cpt == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    strip_caret_numbers_in_place(cpt);

    ପାଇଁ (p = cpt; *p;)
    {
	ଯେତେବେଳେ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ଯଦି (*p == NUL)
	    ଭାଙ୍ଗ;

	cb = get_callback_if_cpt_func(p, idx);
	ଯଦି (cb)
	{
	    ଯଦି (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		ଯଦି (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		ଅନ୍ୟଥା
		    startcol = -2;
	    }
	    ଅନ୍ୟଥା ଯଦି (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	ଅନ୍ୟଥା
	    cpt_sources_array[idx].cs_startcol = -3;

	(ଶୂନ୍ୟ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    ଫେରନ୍ତୁ OK;
#endif
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compl_source_start_timer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    ଯଦି (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
advance_cpt_sources_index_safe(ଶୂନ୍ୟ)
{
    ଯଦି (cpt_sources_index >= 0 && cpt_sources_index < cpt_sources_ଗଣନା - 1)
    {
	cpt_sources_index++;
	ଫେରନ୍ତୁ OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_index_out_of_range_nr), cpt_sources_index);
#endif
    ଫେରନ୍ତୁ FAIL;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_get_exp(pos_T *ini)
{
    ସ୍ଥିର ins_compl_next_state_T   st;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    st_cleared = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_new_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type = ctrl_x_mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    ଯଦି (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	ଯଦି (!st_cleared)
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
	st.e_cpt = st.e_cpt_copy == ଖାଲି ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_ଧାଡ଼ି_ଗଣନା	1000
	ଯଦି (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_ଧାଡ଼ି_ଗଣନା);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    ଅନ୍ୟଥା ଯଦି (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_ଧାଡ଼ି_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != ଖାଲି;
    ଯଦି (normal_mode_strict)
    {
	cpt_sources_index = 0;
	ଯଦି (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    ପାଇଁ (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	ଯଦି ((ctrl_x_mode_normal() || ctrl_x_mode_ଧାଡ଼ି_or_eval())
					&& (!compl_started || st.found_all))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = process_next_cpt_ମୂଲ୍ୟ(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    ଯଦି (status == INS_COMPL_CPT_END)
		ଭାଙ୍ଗ;
	    ଯଦି (status == INS_COMPL_CPT_CONT)
	    {
		ଯଦି (may_advance_cpt_idx)
		{
		    ଯଦି (!advance_cpt_sources_index_safe())
			ଭାଙ୍ଗ;
		    compl_source_start_timer(cpt_sources_index);
		}
		ଜାରି;
	    }
	}

	ଯଦି (normal_mode_strict && type == CTRL_X_FUNCTION
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
	ଯଦି (compl_pattern.string == ଖାଲି)
	    ଭାଙ୍ଗ;

	ଯଦି (may_advance_cpt_idx)
	{
	    ଯଦି (!advance_cpt_sources_index_safe())
		ଭାଙ୍ଗ;
	    compl_source_start_timer(cpt_sources_index);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	ଯଦି ((ctrl_x_mode_not_default() && !ctrl_x_mode_ଧାଡ଼ି_or_eval())
						|| found_new_match != FAIL)
	{
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	    // Fill the popup menu as soon as possible.
	    ଯଦି (type != -1)
		ins_compl_check_ଚାବିଗୁଡ଼ିକ(0, FALSE);

	    ଯଦି ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_ଧାଡ଼ି_or_eval()) || compl_interrupted)
		ଭାଙ୍ଗ;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	ଅନ୍ୟଥା
	{
	    // Mark a buffer scanned when it has been scanned completely
	    ଯଦି (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	ଯଦି (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	ଯଦି (!compl_dir_forward())
	    ଯେତେବେଳେ (compl_curr_match->cp_prev
		    && !match_at_original_text(compl_curr_match->cp_prev))
		compl_curr_match = compl_curr_match->cp_prev;
    }
    cpt_sources_index = -1;
    compl_started = TRUE;

    ଯଦି ((ctrl_x_mode_normal() || ctrl_x_mode_ଧାଡ଼ି_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_ଗଣନା = -1;		// total of matches, unknown
    ଯଦି (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_ଧାଡ଼ି_or_eval()))
	match_ଗଣନା = ins_compl_make_cyclic();

    ଯଦି (cot_fuzzy() && compl_get_longest && compl_ଅଙ୍କ_bests > 0)
	fuzzy_longest_match();

    ଯଦି (compl_old_match != ଖାଲି)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_next
						    : compl_old_match->cp_prev;
	ଯଦି (compl_curr_match == ଖାଲି)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    ଯଦି (match_ଗଣନା > 0 && !ctrl_x_mode_spell())
    {
	ଯଦି (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	ଯଦି (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    ଫେରନ୍ତୁ match_ଗଣନା;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_update_shown_match(ଶୂନ୍ୟ)
{
    string_T	*leader;

    (ଶୂନ୍ୟ)get_leader_for_startcol(ଖାଲି, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    ଯେତେବେଳେ (!ins_compl_equal(compl_shown_match,
		leader->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader->length)
	    && compl_shown_match->cp_next != ଖାଲି
	    && !is_first_match(compl_shown_match->cp_next))
    {
	compl_shown_match = compl_shown_match->cp_next;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    ଯଦି (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader->length)
	    && (compl_shown_match->cp_next == ଖାଲି
		|| is_first_match(compl_shown_match->cp_next)))
    {
	ଯେତେବେଳେ (!ins_compl_equal(compl_shown_match,
		    leader->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader->length)
		&& compl_shown_match->cp_prev != ଖାଲି
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
    ଶୂନ୍ୟ
ins_compl_delete(ଶୂନ୍ୟ)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = compl_col + (compl_status_adding() ? compl_length : 0);
    string_T	remaining = {ଖାଲି, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    orig_col;

    ଯଦି (ins_compl_preinsert_effect())
    {
	col += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    ଯଦି (curwin->w_cursor.lnum > compl_lnum)
    {
	ଯଦି (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *ଧାଡ଼ି = ml_get_cursor();
	    remaining.length = ml_get_cursor_len();
	    remaining.string = vim_strnsave(ଧାଡ଼ି, remaining.length);
	    ଯଦି (remaining.string == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
	ଯେତେବେଳେ (curwin->w_cursor.lnum > compl_lnum)
	{
	    ଯଦି (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		ଯଦି (remaining.string)
		    vim_free(remaining.string);
		ଫେରନ୍ତୁ;
	    }
	    deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col > col)
    {
	ଯଦି (stop_arrow() == FAIL)
	{
	    ଯଦି (remaining.string)
		vim_free(remaining.string);
	    ଫେରନ୍ତୁ;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    ଯଦି (remaining.string != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		base_indent = get_indent();

    ଯେତେବେଳେ (*curr != NUL)
    {
	ଯଦି (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    ଯଦି (curr > start)
		ins_char_bytes(start, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(curr - start));

	    // Handle newline
	    open_ଧାଡ଼ି(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, ଖାଲି);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    ଯଦି (curr > start)
	ins_char_bytes(start, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    ସ୍ଥିର char_u *
find_common_prefix(size_t *prefix_len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା curbuf_only)
{
    compl_T     *compl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*match_ଗଣନା;
    char_u	*first = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cpt_completion = (cpt_sources_array != ଖାଲି);

    ଯଦି (!is_cpt_completion)
	ଫେରନ୍ତୁ ଖାଲି;

    match_ଗଣନା = ALLOC_CLEAR_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, cpt_sources_ଗଣନା);
    ଯଦି (match_ଗଣନା == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    (ଶୂନ୍ୟ)get_leader_for_startcol(ଖାଲି, TRUE); // Clear the cache

    compl = compl_first_match;
    କର
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	ଯଦି (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	ଯଦି (!match_at_original_text(compl)
		&& (leader->string == ଖାଲି
		    || ins_compl_equal_sc(compl, leader->string,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader->length)))
	{
	    // Limit number of items from each source if max_items is set.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା match_limit_exceeded = FALSE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_source = compl->cp_cpt_source_idx;

	    ଯଦି (cur_source != -1)
	    {
		match_ଗଣନା[cur_source]++;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_matches = cpt_sources_array[cur_source].cs_max_matches;
		ଯଦି (max_matches > 0 && match_ଗଣନା[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    ଯଦି (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		ଯଦି (first == ଖାଲି && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(first);
		}
		ଅନ୍ୟଥା ଯଦି (first != ଖାଲି)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    ଯେତେବେଳେ (j < len && *s1 != NUL && *s2 != NUL)
		    {
			ଯଦି (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    ଭାଙ୍ଗ;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    ଯଦି (len == 0)
			ଭାଙ୍ଗ;
		}
	    }
	}
	compl = compl->cp_next;
    } ଯେତେବେଳେ (compl != ଖାଲି && !is_first_match(compl));

    vim_free(match_ଗଣନା);

    ଯଦି (len > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	ଯଦି (len == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(first))
	{
	    char_u *ଧାଡ଼ି = ml_get_curline();
	    char_u *p = ଧାଡ଼ି + curwin->w_cursor.col;
	    ଯଦି (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା text_len = end - p;
		ଯଦି (text_len > 0
			&& text_len < (len - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	ଫେରନ୍ତୁ first;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    ଶୂନ୍ୟ
ins_compl_ଭର୍ତ୍ତି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା move_cursor, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଭର୍ତ୍ତି_prefix)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_len = get_compl_len();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.length;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    ଯଦି (ଭର୍ତ୍ତି_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	ଯଦି (cp_str == ଖାଲି)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    ଯଦି (cp_str == ଖାଲି)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.length;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    ଅନ୍ୟଥା ଯଦି (cpt_sources_array != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	ଯଦି (cpt_idx >= 0 && compl_col >= 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    ଯଦି (startcol >= 0 && startcol < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col - startcol;
		ଯଦି ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    ଯଦି (compl_len < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cp_str_len)
    {
	ଯଦି (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	ଅନ୍ୟଥା
	{
	    ins_compl_ଭର୍ତ୍ତି_bytes(cp_str + compl_len,
		    ଭର୍ତ୍ତି_prefix ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cp_str_len - compl_len : -1);
	    ଯଦି ((preinsert || ଭର୍ତ୍ତି_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    ଯଦି (match_at_original_text(compl_shown_match)
	    || (preinsert && !ଭର୍ତ୍ତି_prefix))
	compl_used_match = FALSE;
    ଅନ୍ୟଥା
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = ଭର୍ତ୍ତି_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_show_filename(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	*lead = _("match in file");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    ଯଦି (space <= 0)
	ଫେରନ୍ତୁ;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    ପାଇଁ (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	ଯେତେବେଳେ (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    ଯଦି (!compl_autocomplete)
    {
	ସନ୍ଦେଶ_hist_off = TRUE;
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
	ସନ୍ଦେଶ_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    ସ୍ଥିର compl_T *
find_next_match_in_menu(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    କର
	match = is_forward ? match->cp_next : match->cp_prev;
    ଯେତେବେଳେ (match->cp_next && !match->cp_in_match_array
	    && !match_at_original_text(match));
    ଫେରନ୍ତୁ match;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_next_completion_match(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	allow_get_expansion,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	todo,		// repeat completion this many times
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	advance,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ଅଙ୍କ_matches)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_end = FALSE;
    compl_T	*found_compl = ଖାଲି;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_cot_flags = get_cot_flags();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    ଯେତେବେଳେ (--todo >= 0)
    {
	ଯଦି (compl_shows_dir_forward() && compl_shown_match->cp_next != ଖାଲି)
	{
	    ଯଦି (compl_match_array != ଖାଲି)
		compl_shown_match = find_next_match_in_menu();
	    ଅନ୍ୟଥା
		compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != ଖାଲି
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	ଅନ୍ୟଥା ଯଦି (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != ଖାଲି)
	{
	    found_end = is_first_match(compl_shown_match);
	    ଯଦି (compl_match_array != ଖାଲି)
		compl_shown_match = find_next_match_in_menu();
	    ଅନ୍ୟଥା
		compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (!allow_get_expansion)
	    {
		ଯଦି (advance)
		{
		    ଯଦି (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    ଅନ୍ୟଥା
			compl_pending += todo + 1;
		}
		ଫେରନ୍ତୁ -1;
	    }

	    ଯଦି (!compl_no_select && advance)
	    {
		ଯଦି (compl_shows_dir_backward())
		    --compl_pending;
		ଅନ୍ୟଥା
		    ++compl_pending;
	    }

	    // Find matches.
	    *ଅଙ୍କ_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    ଯେତେବେଳେ (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		ଯଦି (compl_pending > 0 && compl_shown_match->cp_next != ଖାଲି)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		ଅନ୍ୟଥା ଯଦି (compl_pending < 0 && compl_shown_match->cp_prev != ଖାଲି)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		ଅନ୍ୟଥା
		    ଭାଙ୍ଗ;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	ଯଦି (!match_at_original_text(compl_shown_match)
		&& leader->string != ଖାଲି
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader->length)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	ଅନ୍ୟଥା
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	ଯଦି (found_end)
	{
	    ଯଦି (found_compl != ଖାଲି)
	    {
		compl_shown_match = found_compl;
		ଭାଙ୍ଗ;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_next(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    allow_get_expansion,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2,		// repeat completion this many times; should
				// be at least 1
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଭର୍ତ୍ତି_match)	// Insert the newly selected match
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_matches = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    todo = ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    advance;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_cot_flags = get_cot_flags();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    compl_no_ଭର୍ତ୍ତି = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    ଯଦି (compl_shown_match == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (compl_leader.string != ଖାଲି
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    ଯଦି (allow_get_expansion && ଭର୍ତ୍ତି_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = ଗଣନା_2 != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    ଯଦି (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    ଯଦି (find_next_completion_match(allow_get_expansion, todo, advance,
							&ଅଙ୍କ_matches) == -1)
	ଫେରନ୍ତୁ -1;

    ଯଦି (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	ଫେରନ୍ତୁ -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    ଯଦି (!started && ins_compl_preinsert_longest())
	ins_compl_ଭର୍ତ୍ତି(TRUE, TRUE);
    ଅନ୍ୟଥା ଯଦି (compl_no_ଭର୍ତ୍ତି && !started && !compl_preinsert)
    {
	ins_compl_ଭର୍ତ୍ତି_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (ଭର୍ତ୍ତି_match)
    {
	ଯଦି (!compl_get_longest || compl_used_match)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_ଭର୍ତ୍ତି(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	ଅନ୍ୟଥା
	    ins_compl_ଭର୍ତ୍ତି_bytes(compl_leader.string + get_compl_len(), -1);
    }
    ଅନ୍ୟଥା
	compl_used_match = FALSE;

    ଯଦି (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	ଯଦି (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	ଅନ୍ୟଥା
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	ଯଦି (gui.in_use)
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
    ଯଦି (compl_no_ଭର୍ତ୍ତି && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    ଅନ୍ୟଥା
	compl_enter_selects = !ଭର୍ତ୍ତି_match && compl_match_array != ଖାଲି;

    // Show the file name for the match (if any)
    ଯଦି (compl_shown_match->cp_fname != ଖାଲି)
	ins_compl_show_filename();

    ଫେରନ୍ତୁ ଅଙ୍କ_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_elapsed_time(ଶୂନ୍ୟ)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_index].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    ଯଦି (elapsed_ms > compl_timeout_ms)
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
    ଶୂନ୍ୟ
ins_compl_check_ଚାବିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା frequency, ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_compl_func)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    ଯଦି (!in_compl_func && (using_script() || ex_normal_busy))
	ଫେରନ୍ତୁ;

    // Only do this at regular intervals
    ଯଦି (++ଗଣନା_2 < frequency)
	ଫେରନ୍ତୁ;
    ଗଣନା_2 = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    ଯଦି (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	ଯଦି (vim_is_ctrl_x_ଚାବି(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (ଶୂନ୍ୟ)ins_compl_next(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	ଅନ୍ୟଥା
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    ଯଦି (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		ଯଦି (c != Ctrl_R && (ଚାବି_Typed
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_ଧାଡ଼ି_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != ଖାଲି && cpt_sources_index >= 0;
	ଯଦି (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    ଯଦି (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(ଶୂନ୍ୟ)ins_compl_next(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_key2dir(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	ଫେରନ୍ତୁ BACKWARD;
    ଫେରନ୍ତୁ FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_pum_ଚାବି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_key2count(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (ins_compl_pum_ଚାବି(c) && c != K_UP && c != K_DOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = pum_get_height();
	ଯଦି (h > 3)
	    h -= 2; // keep some context
	ଫେରନ୍ତୁ h;
    }
    ଫେରନ୍ତୁ 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_use_match(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ବାଛ (c)
    {
	ସ୍ଥିତି K_UP:
	ସ୍ଥିତି K_DOWN:
	ସ୍ଥିତି K_PAGEDOWN:
	ସ୍ଥିତି K_KPAGEDOWN:
	ସ୍ଥିତି K_S_DOWN:
	ସ୍ଥିତି K_PAGEUP:
	ସ୍ଥିତି K_KPAGEUP:
	ସ୍ଥିତି K_S_UP:
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_normal_compl_info(char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol, colnr_T curs_col)
{
    ଯଦି ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	ଯଦି (!compl_status_adding())
	{
	    ଯେତେବେଳେ (--startcol >= 0 && vim_isIDc(ଧାଡ଼ି[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	ଯଦି (p_ic)
	{
	    compl_pattern.string = str_foldcase(ଧାଡ଼ି + compl_col,
		    compl_length, ଖାଲି, 0);
	    ଯଦି (compl_pattern.string == ଖାଲି)
	    {
		compl_pattern.length = 0;
		ଫେରନ୍ତୁ FAIL;
	    }
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	ଅନ୍ୟଥା
	{
	    compl_pattern.string = vim_strnsave(ଧାଡ଼ି + compl_col, (size_t)compl_length);
	    ଯଦି (compl_pattern.string == ଖାଲି)
	    {
		compl_pattern.length = 0;
		ଫେରନ୍ତୁ FAIL;
	    }
	    compl_pattern.length = (size_t)compl_length;
	}
    }
    ଅନ୍ୟଥା ଯଦି (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	ଯଦି (!vim_iswordp(ଧାଡ଼ି + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(ଧାଡ଼ି, ଧାଡ଼ି + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ଖାଲି, ଧାଡ଼ି + compl_col, compl_length) + prefixlen;
	compl_pattern.string = alloc(n);
	ଯଦି (compl_pattern.string == ଖାଲି)
	{
	    compl_pattern.length = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	STRCPY((ଅକ୍ଷର *)compl_pattern.string, prefix);
	(ଶୂନ୍ୟ)quote_meta(compl_pattern.string + prefixlen,
		ଧାଡ଼ି + compl_col, compl_length);
	compl_pattern.length = n - 1;
    }
    ଅନ୍ୟଥା ଯଦି (--startcol < 0
	    || !vim_iswordp(mb_prevptr(ଧାଡ଼ି, ଧାଡ଼ି + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	ଯଦି (compl_pattern.string == ଖାଲି)
	{
	    compl_pattern.length = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	compl_pattern.length = len;
	compl_col += curs_col;
	compl_length = 0;
	compl_from_nonkeyword = TRUE;
    }
    ଅନ୍ୟଥା
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_class;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା head_off;

	    startcol -= (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + startcol);
	    base_class = mb_get_class(ଧାଡ଼ି + startcol);
	    ଯେତେବେଳେ (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + startcol);
		ଯଦି (base_class != mb_get_class(ଧାଡ଼ି + startcol
			    - head_off))
		    ଭାଙ୍ଗ;
		startcol -= head_off;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯେତେବେଳେ (--startcol >= 0 && vim_iswordc(ଧାଡ଼ି[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curs_col - startcol;
	ଯଦି (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    ଯଦି (compl_pattern.string == ଖାଲି)
	    {
		compl_pattern.length = 0;
		ଫେରନ୍ତୁ FAIL;
	    }
	    STRCPY((ଅକ୍ଷର *)compl_pattern.string, "\\<");
	    (ଶୂନ୍ୟ)quote_meta(compl_pattern.string + 2, ଧାଡ଼ି + compl_col, 1);
	    STRCAT((ଅକ୍ଷର *)compl_pattern.string, "\\k");
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	ଅନ୍ୟଥା
	{
	    size_t  n = quote_meta(ଖାଲି, ଧାଡ଼ି + compl_col, compl_length) + 2;

	    compl_pattern.string = alloc(n);
	    ଯଦି (compl_pattern.string == ଖାଲି)
	    {
		compl_pattern.length = 0;
		ଫେରନ୍ତୁ FAIL;
	    }
	    STRCPY((ଅକ୍ଷର *)compl_pattern.string, "\\<");
	    (ଶୂନ୍ୟ)quote_meta(compl_pattern.string + 2, ଧାଡ଼ି + compl_col,
		    compl_length);
	    compl_pattern.length = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    ଯଦି (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	ଯଦି (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_wholeline_compl_info(char_u *ଧାଡ଼ି, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(ଧାଡ଼ି);
    compl_length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curs_col - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col;
    ଯଦି (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    ଯଦି (p_ic)
    {
	compl_pattern.string = str_foldcase(ଧାଡ଼ି + compl_col, compl_length,
		ଖାଲି, 0);
	ଯଦି (compl_pattern.string == ଖାଲି)
	{
	    compl_pattern.length = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	compl_pattern.length = STRLEN(compl_pattern.string);
    }
    ଅନ୍ୟଥା
    {
	compl_pattern.string = vim_strnsave(ଧାଡ଼ି + compl_col, (size_t)compl_length);
	ଯଦି (compl_pattern.string == ଖାଲି)
	{
	    compl_pattern.length = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	compl_pattern.length = (size_t)compl_length;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_filename_compl_info(char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    ଯଦି (startcol > 0)
    {
	char_u	*p = ଧାଡ଼ି + startcol;

	MB_PTR_BACK(ଧାଡ଼ି, p);
	ଯେତେବେଳେ (p > ଧାଡ଼ି && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(ଧାଡ଼ି, p);
	ଯଦି (p == ଧାଡ଼ି && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	ଅନ୍ୟଥା
	    startcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି) + 1;
    }

    compl_col += startcol;
    compl_length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curs_col - startcol;
    compl_pattern.string = addstar(ଧାଡ଼ି + compl_col, compl_length, EXPAND_FILES);
    ଯଦି (compl_pattern.string == ଖାଲି)
    {
	compl_pattern.length = 0;
	ଫେରନ୍ତୁ FAIL;
    }

    compl_pattern.length = STRLEN(compl_pattern.string);

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cmdline_compl_info(char_u *ଧାଡ଼ି, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(ଧାଡ଼ି, curs_col);
    ଯଦି (compl_pattern.string == ଖାଲି)
    {
	compl_pattern.length = 0;
	ଫେରନ୍ତୁ FAIL;
    }
    compl_pattern.length = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_pattern.length, curs_col, FALSE);
    ଯଦି (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    ଅନ୍ୟଥା
	compl_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(compl_xp.xp_pattern - compl_pattern.string);
    compl_length = curs_col - compl_col;

    ଫେରନ୍ତୁ OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_compl_globals(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cpt_compl UNUSED)
{
    ଯଦି (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	ଯଦି (startcol < compl_col)
	    ଫେରନ୍ତୁ prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	ଅନ୍ୟଥା
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.length);
	    cpt_compl_pattern.length = compl_orig_text.length;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*ଧାଡ଼ି = ml_get(curwin->w_cursor.lnum);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(ଧାଡ଼ି + startcol, (size_t)len);
	ଯଦି (compl_pattern.string == ଖାଲି)
	{
	    compl_pattern.length = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	compl_pattern.length = (size_t)len;
	compl_col = startcol;
	compl_length = len;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*startcol UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    char_u	*funcname = ଖାଲି;
    pos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State = State;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cpt_function = (cb != ଖାଲି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ଯଦି (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	ଯଦି (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    ଫେରନ୍ତୁ FAIL;
	}
	cb = get_ଭର୍ତ୍ତି_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_ସଂଖ୍ୟା;
    args[0].vval.v_ସଂଖ୍ୟା = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    ଯଦି (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    ଯଦି (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ଯଦି (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (startcol != ଖାଲି)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    ଯଦି (col == -2 || aborting())
	ଫେରନ୍ତୁ FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    ଯଦି (col == -3)
    {
	ଯଦି (is_cpt_function)
	    ଫେରନ୍ତୁ FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = ଖାଲି;
	ଯଦି (!shortmess(SHM_COMPLETIONMENU))
	    ସନ୍ଦେଶ_clr_cmdline();
	ଫେରନ୍ତୁ FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    ଫେରନ୍ତୁ ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_spell_compl_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol UNUSED, colnr_T curs_col UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*ଧାଡ଼ି = ଖାଲି;

    ଯଦି (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    ଅନ୍ୟଥା
	compl_col = spell_word_start(startcol);

    ଯଦି (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    ଅନ୍ୟଥା
    {
	spell_expand_check_cap(compl_col);
	compl_length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    ଧାଡ଼ି = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(ଧାଡ଼ି + compl_col, (size_t)compl_length);
    ଯଦି (compl_pattern.string == ଖାଲି)
    {
	compl_pattern.length = 0;
	ଫେରନ୍ତୁ FAIL;
    }

    compl_pattern.length = (size_t)compl_length;
    ret = OK;
#endif

    ଫେରନ୍ତୁ ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compl_get_info(char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol, colnr_T curs_col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଧାଡ଼ି_invalid)
{
    ଯଦି (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	ଯଦି (get_normal_compl_info(ଧାଡ଼ି, startcol, curs_col) != OK)
	    ଫେରନ୍ତୁ FAIL;
	*ଧାଡ଼ି_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_ଧାଡ଼ି_or_eval())
    {
	ଫେରନ୍ତୁ get_wholeline_compl_info(ଧାଡ଼ି, curs_col);
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_files())
    {
	ଫେରନ୍ତୁ get_filename_compl_info(ଧାଡ଼ି, startcol, curs_col);
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	ଫେରନ୍ତୁ get_cmdline_compl_info(ଧାଡ଼ି, curs_col);
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	ଯଦି (get_userdefined_compl_info(curs_col, ଖାଲି, ଖାଲି) != OK)
	    ଫେରନ୍ତୁ FAIL;
	*ଧାଡ଼ି_invalid = TRUE;	// "line" may have become invalid
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_spell())
    {
	ଯଦି (get_spell_compl_info(startcol, curs_col) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	*ଧାଡ଼ି_invalid = TRUE;	// "line" may have become invalid
    }
    ଅନ୍ୟଥା
    {
	internal_error("ins_complete()");
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_continue_ସନ୍ଧାନ(char_u *ଧାଡ଼ି)
{
    // it is a continued search
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    ଯଦି (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	ଯଦି (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(ଧାଡ଼ି);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	ଅନ୍ୟଥା
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    ଯଦି (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    ଧାଡ଼ି + compl_length
			    + compl_startpos.col) - ଧାଡ଼ି);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_length = curwin->w_cursor.col - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	ଯଦି (compl_length > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_length = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_length;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	ଯଦି (compl_length < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_ଧାଡ଼ି_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    ଅନ୍ୟଥା
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_compl_start(ଶୂନ୍ୟ)
{
    char_u	*ଧାଡ଼ି = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_invalid = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_ai = did_ai;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    ଯଦି (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	ଫେରନ୍ତୁ FAIL;
    }

    ଧାଡ଼ି = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    ଯଦି ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_ସନ୍ଧାନ(ଧାଡ଼ି);
    ଅନ୍ୟଥା
	compl_cont_status &= CONT_LOCAL;

    ଯଦି (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	ଯଦି (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    ଯଦି (compl_get_info(ଧାଡ଼ି, startcol, curs_col, &ଧାଡ଼ି_invalid) == FAIL)
    {
	ଯଦି (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	ଫେରନ୍ତୁ FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    ଯଦି (ଧାଡ଼ି_invalid)
	ଧାଡ଼ି = ml_get(curwin->w_cursor.lnum);

    ଯଦି (compl_status_adding())
    {
	ଯଦି (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	ଯଦି (ctrl_x_mode_ଧାଡ଼ି_or_eval())
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
    ଅନ୍ୟଥା
    {
	edit_submode_pre = ଖାଲି;
	compl_startpos.col = compl_col;
    }

    ଯଦି (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	ଯଦି (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_ସନ୍ଦେଶ]);
	ଅନ୍ୟଥା
	    edit_submode = (char_u *)_(CTRL_X_ସନ୍ଦେଶ(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ଖାଲି);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.length = (size_t)compl_length;
    compl_orig_text.string = vim_strnsave(ଧାଡ଼ି + compl_col, (size_t)compl_length);
    ଯଦି (p_ic)
	flags |= CP_ICASE;
    ଯଦି (compl_orig_text.string == ଖାଲି
	    || ins_compl_add(compl_orig_text.string,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compl_orig_text.length,
		ଖାଲି, ଖାଲି, ଖାଲି, 0, flags, FALSE, ଖାଲି, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	ଫେରନ୍ତୁ FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    ଯଦି (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_ଗଣନା;
	showmode();
	edit_submode_extra = ଖାଲି;
	out_flush();
    }

    did_ai = save_did_ai;
    ଫେରନ୍ତୁ OK;
}

/*
 * display the completion status message
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_show_statusmsg(ଶୂନ୍ୟ)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    ଯଦି (is_first_match(compl_first_match->cp_next))
    {
	edit_submode_extra = compl_status_adding() && compl_length > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    ଯଦି (edit_submode_extra == ଖାଲି)
    {
	ଯଦି (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	ଅନ୍ୟଥା ଯଦି (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_ଗଣନା;
	}
	ଅନ୍ୟଥା ଯଦି (compl_curr_match->cp_next == compl_curr_match->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_ଗଣନା;
	    compl_curr_match->cp_ସଂଖ୍ୟା = 1;
	}
	ଅନ୍ୟଥା
	{
#ଯଦି defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    ଯଦି (compl_curr_match->cp_ସଂଖ୍ୟା == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    ଯଦି (compl_curr_match->cp_ସଂଖ୍ୟା != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		ସ୍ଥିର char_u match_ref[81];

		ଯଦି (compl_matches > 0)
		    vim_snprintf((ଅକ୍ଷର *)match_ref, ଆକାର(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_ସଂଖ୍ୟା, compl_matches);
		ଅନ୍ୟଥା
		    vim_snprintf((ଅକ୍ଷର *)match_ref, ଆକାର(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_ସଂଖ୍ୟା);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		ଯଦି (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    ଯଦି (!compl_opt_suppress_empty)
    {
	showmode();
	ଯଦି (!shortmess(SHM_COMPLETIONMENU))
	{
	    ଯଦି (edit_submode_extra != ଖାଲି)
	    {
		ଯଦି (!p_smd)
		{
		    ସନ୍ଦେଶ_hist_off = TRUE;
		    ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)edit_submode_extra,
			    edit_submode_highl < HLF_ଗଣନା
			    ? HL_ATTR(edit_submode_highl) : 0);
		    ସନ୍ଦେଶ_hist_off = FALSE;
		}
	    }
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_complete(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable_pum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_w_wrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_w_leftcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଭର୍ତ୍ତି_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    ଭର୍ତ୍ତି_match = ins_compl_use_match(c);

    ଯଦି (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	ଯଦି (ins_compl_start() == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (ଭର୍ତ୍ତି_match && stop_arrow() == FAIL)
	ଫେରନ୍ତୁ FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_buffer;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_ଅଙ୍କ_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), ଭର୍ତ୍ତି_match);

    // Reset autocompletion timer expiry flag
    ଯଦି (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    ଯଦି (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    ଯଦି (got_int && !global_busy)
    {
	(ଶୂନ୍ୟ)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_next);
    ଯଦି (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	ଯଦି (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    ଯଦି (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    ଅନ୍ୟଥା
	compl_cont_status &= ~CONT_S_IPOS;

    ଯଦି (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    ଯଦି (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    ଫେରନ୍ତୁ OK;
}

/*
 * Enable autocompletion
 */
    ଶୂନ୍ୟ
ins_compl_enable_autocomplete(ଶୂନ୍ୟ)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
}

/*
 * Disable autocompletion
 */
    ଶୂନ୍ୟ
ins_compl_disable_autocomplete(ଶୂନ୍ୟ)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    ଶୂନ୍ୟ
ins_compl_arm_autostart(ଶୂନ୍ୟ)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    ଶୂନ୍ୟ
ins_compl_disarm_autostart(ଶୂନ୍ୟ)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(ଶୂନ୍ୟ)
{
#ifdef ELAPSED_FUNC
    ଯଦି (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	ଫେରନ୍ତୁ true;
    }
#endif
    ଫେରନ୍ତୁ false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    ଶୂନ୍ୟ
ins_compl_clear_autocomplete_delay(ଶୂନ୍ୟ)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(ଶୂନ୍ୟ)
{
#ifdef ELAPSED_FUNC
    ଫେରନ୍ତୁ ELAPSED_FUNC(compl_autocomplete_start_tv);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    ସ୍ଥିର ଶୂନ୍ୟ
show_pum(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_w_wrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    ଯଦି (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    ସ୍ଥିର unsigned
quote_meta(char_u *dest, char_u *src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    ପାଇଁ ( ; --len >= 0; src++)
    {
	ବାଛ (*src)
	{
	    ସ୍ଥିତି '.':
	    ସ୍ଥିତି '*':
	    ସ୍ଥିତି '[':
		ଯଦି (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    ଭାଙ୍ଗ;
		// FALLTHROUGH
	    ସ୍ଥିତି '~':
		ଯଦି (!magic_isset())	// quote these only if magic is set
		    ଭାଙ୍ଗ;
		// FALLTHROUGH
	    ସ୍ଥିତି '\\':
		ଯଦି (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    ଭାଙ୍ଗ;
		// FALLTHROUGH
	    ସ୍ଥିତି '^':		// currently it's not needed.
	    ସ୍ଥିତି '$':
		m++;
		ଯଦି (dest != ଖାଲି)
		    *dest++ = '\\';
		ଭାଙ୍ଗ;
	}
	ଯଦି (dest != ଖାଲି)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mb_len = (*mb_ptr2len)(src) - 1;
	    ଯଦି (mb_len > 0 && len >= mb_len)
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    ଯଦି (dest != ଖାଲି)
			*dest++ = *src;
		}
	}
    }
    ଯଦି (dest != ଖାଲି)
	*dest = NUL;

    ଫେରନ୍ତୁ m;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_insexpand_stuff(ଶୂନ୍ୟ)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_ଗଣନା);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_back_to_badword(ଶୂନ୍ୟ)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, ଖାଲି);
    ଯଦି (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cpt_sources_clear(ଶୂନ୍ୟ)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_index = -1;
    cpt_sources_ଗଣନା = 0;
}

/*
 * Setup completion sources.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
setup_cpt_sources(ଶୂନ୍ୟ)
{
    char_u  buf[LSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2;
    char_u  *p;

    cpt_sources_clear();

    ଗଣନା_2 = get_cpt_sources_ଗଣନା(curbuf->b_p_cpt);
    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, ଗଣନା_2);
    ଯଦି (cpt_sources_array == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (p = curbuf->b_p_cpt; *p;)
    {
	ଯେତେବେଳେ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ଯଦି (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ଯଦି (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		ଯଦି (caret != ଖାଲି)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((ଅକ୍ଷର *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_ଗଣନା = ଗଣନା_2;
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_cpt_func_refresh_always(ଶୂନ୍ୟ)
{
#ifdef FEAT_COMPL_FUNC
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cpt_sources_ଗଣନା; i++)
	ଯଦି (cpt_sources_array[i].cs_refresh_always)
	    ଫେରନ୍ତୁ TRUE;
#endif
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_compl_make_linear(ଶୂନ୍ୟ)
{
    compl_T *m;

    ଯଦି (compl_first_match == ଖାଲି || compl_first_match->cp_prev == ଖାଲି)
	ଫେରନ୍ତୁ;
    m = compl_first_match->cp_prev;
    m->cp_next = ଖାଲି;
    compl_first_match->cp_prev = ଖାଲି;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    ସ୍ଥିର ଶୂନ୍ୟ
remove_old_matches(ଶୂନ୍ୟ)
{
    compl_T *current;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    shown_match_removed = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    ଯଦି (cpt_sources_index < 0)
	ଫେରନ୍ତୁ;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    ପାଇଁ (current = compl_first_match; current != ଖାଲି; )
    {
	ଯଦି (current->cp_cpt_source_idx == cpt_sources_index)
	{
	    compl_T *to_delete = current;

	    ଯଦି (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_next;

	    ଯଦି (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_next;
		compl_first_match->cp_prev = ଖାଲି;
	    }
	    ଅନ୍ୟଥା ଯଦି (to_delete->cp_next == ଖାଲି) // node to remove is at tail
		to_delete->cp_prev->cp_next = ଖାଲି;
	    ଅନ୍ୟଥା // node is in the moddle
	    {
		to_delete->cp_prev->cp_next = to_delete->cp_next;
		to_delete->cp_next->cp_prev = to_delete->cp_prev;
	    }
	    ins_compl_item_free(to_delete);
	}
	ଅନ୍ୟଥା
	    current = current->cp_next;
    }

    // Re-assign compl_shown_match if necessary
    ଯଦି (shown_match_removed)
    {
	ଯଦି (forward)
	    compl_shown_match = compl_first_match;
	ଅନ୍ୟଥା
	{  // Last node will have the prefix that is being completed
	    ପାଇଁ (current = compl_first_match; current->cp_next != ଖାଲି;
		    current = current->cp_next)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    ପାଇଁ (current = compl_first_match; current != ଖାଲି; )
    {
	ଯଦି ((forward ? current->cp_cpt_source_idx < cpt_sources_index
		    : current->cp_cpt_source_idx > cpt_sources_index))
	{
	    compl_curr_match = forward ? current : current->cp_next;
	    current = current->cp_next;
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    ସ୍ଥିର ଶୂନ୍ୟ
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_index];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    startcol = cpt_src->cs_startcol;

    ଯଦି (startcol == -2 || startcol == -3)
	ଫେରନ୍ତୁ;

    ଯଦି (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	ଯଦି (!cpt_src->cs_refresh_always)
	    ins_compl_ଭର୍ତ୍ତି_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	ଯଦି (!cpt_src->cs_refresh_always)
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
    ସ୍ଥିର ଶୂନ୍ୟ
cpt_compl_refresh(ଶୂନ୍ୟ)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_index = 0;
    ପାଇଁ (p = cpt; *p;)
    {
	ଯେତେବେଳେ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ଯଦି (*p == NUL)
	    ଭାଙ୍ଗ;

	ଯଦି (cpt_sources_array[cpt_sources_index].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_index);
	    ଯଦି (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		ଯଦି (ret == FAIL)
		{
		    ଯଦି (startcol == -3)
			cpt_sources_array[cpt_sources_index].cs_refresh_always
			    = FALSE;
		    ଅନ୍ୟଥା
			startcol = -2;
		}
		ଅନ୍ୟଥା ଯଦି (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_index].cs_startcol = startcol;
		ଯଦି (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_index);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(ଶୂନ୍ୟ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	ଯଦି (may_advance_cpt_index(p))
	    (ଶୂନ୍ୟ)advance_cpt_sources_index_safe();
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
    ଶୂନ୍ୟ
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ଯଦି (ins_compl_preinsert_effect())
	rettv->vval.v_ସଂଖ୍ୟା = 1;
}
