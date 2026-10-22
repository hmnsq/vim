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

#അന്തര്ഭാവ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_വരി	3
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
#define CTRL_X_LOCAL_സന്ദേശം	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_സന്ദേശം(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
സ്ഥിതം അക്ഷരം *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ശൂന്യ, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ശൂന്യ, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ശൂന്യ,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#എങ്കിൽ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
സ്ഥിതം string_T ctrl_x_mode_names[] = {
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
    {ശൂന്യ, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
തരനിർവചനം ഘടന compl_S compl_T;
ഘടന compl_S
{
    compl_T	*cp_next;
    compl_T	*cp_prev;
    compl_T	*cp_match_next;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_എണ്ണം]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_data;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    പൂർണ്ണസംഖ്യ		cp_flags;		// CP_ values
    പൂർണ്ണസംഖ്യ		cp_സംഖ്യ;		// sequence number
    പൂർണ്ണസംഖ്യ		cp_order;		// list position when first scored
    പൂർണ്ണസംഖ്യ		cp_score;		// fuzzy match score or proximity score
    പൂർണ്ണസംഖ്യ		cp_in_match_array;	// collected by compl_match_array
    പൂർണ്ണസംഖ്യ		cp_user_abbr_hl_id;	// highlight group ID for abbr
    പൂർണ്ണസംഖ്യ		cp_user_kind_hl_id;	// highlight group ID for kind
    പൂർണ്ണസംഖ്യ		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
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
സ്ഥിതം compl_T    *compl_first_match = ശൂന്യ;
സ്ഥിതം compl_T    *compl_curr_match = ശൂന്യ;
സ്ഥിതം compl_T    *compl_shown_match = ശൂന്യ;
സ്ഥിതം compl_T    *compl_old_match = ശൂന്യ;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ	    cse_എണ്ണം;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((പൂർണ്ണസംഖ്യ)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_താക്കോൽ - CSE_OFF))

സ്ഥിതം hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    സ്ഥിതം ശൂന്യം
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    എങ്കിൽ (compl_strings_ht.ht_array == ശൂന്യ)
	hash_init(&compl_strings_ht);
    എങ്കിൽ (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	എങ്കിൽ (entry == ശൂന്യ)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    തിരികെ;
	entry->cse_എണ്ണം = 1;
	vim_strncpy(entry->cse_str, str, len);
	എങ്കിൽ (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    അല്ലെങ്കിൽ
	++HI2CSE(hi)->cse_എണ്ണം;
}

// list used to store the compl_T which have the max score
സ്ഥിതം compl_T	  **compl_best_matches = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_അക്കം_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
സ്ഥിതം string_T	  compl_leader = {ശൂന്യ, 0};

സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_used_match;

// didn't finish finding completions.
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_interrupted = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
സ്ഥിതം പൂർണ്ണസംഖ്യ	  ctrl_x_mode = CTRL_X_NORMAL;

സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_matches = 0;	    // number of completion matches
സ്ഥിതം string_T	  compl_pattern = {ശൂന്യ, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
സ്ഥിതം string_T	  cpt_compl_pattern = {ശൂന്യ, 0}; // pattern returned by func in 'cpt'
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_direction = FORWARD;
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_shows_dir = FORWARD;
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_pending = 0;	    // > 1 for postponed CTRL-N
സ്ഥിതം pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_length = 0;
സ്ഥിതം linenr_T	  compl_lnum = 0;           // lnum where the completion start
സ്ഥിതം colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
സ്ഥിതം colnr_T	  compl_ins_end_col = 0;
സ്ഥിതം colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
സ്ഥിതം string_T	  compl_orig_text = {ശൂന്യ, 0};  // text as it was before
					    // completion started
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_cont_mode = 0;
സ്ഥിതം expand_T	  compl_xp;

സ്ഥിതം win_T	  *compl_curr_win = ശൂന്യ;  // win where completion is active
സ്ഥിതം buf_T	  *compl_curr_buf = ശൂന്യ;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_autocomplete = FALSE;	    // whether autocompletion is active
സ്ഥിതം bool	  compl_autostarted = false;	    // Vim started this completion
സ്ഥിതം bool	  compl_autostart_pending = false;  // the trigger armed one
സ്ഥിതം bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
സ്ഥിതം elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    ചെയ്യുക { \
	എങ്കിൽ (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } ആകുമ്പോൾ (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_cont_status = 0;
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

സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_opt_refresh_always = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_opt_suppress_empty = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ	  compl_selected_item = -1;

സ്ഥിതം പൂർണ്ണസംഖ്യ	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
തരനിർവചനം ഘടന cpt_source_T
{
    പൂർണ്ണസംഖ്യ	cs_refresh_always;  // Whether 'refresh:always' is set for func
    പൂർണ്ണസംഖ്യ	cs_startcol;	    // Start column returned by func
    പൂർണ്ണസംഖ്യ	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

സ്ഥിതം cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cpt_sources_എണ്ണം;  // Total number of completion sources specified in the 'cpt' option
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cpt_sources_index = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
സ്ഥിതം pumitem_T *compl_match_array = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ compl_match_arraysize;

സ്ഥിതം പൂർണ്ണസംഖ്യ ins_compl_add(char_u *str, പൂർണ്ണസംഖ്യ len, char_u *fname, char_u **cptext, typval_T *user_data, പൂർണ്ണസംഖ്യ cdir, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ adup, പൂർണ്ണസംഖ്യ *user_hl, പൂർണ്ണസംഖ്യ score);
സ്ഥിതം ശൂന്യം ins_compl_longest_match(compl_T *match);
സ്ഥിതം ശൂന്യം ins_compl_del_pum(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_compl_files(പൂർണ്ണസംഖ്യ എണ്ണം, char_u **files, പൂർണ്ണസംഖ്യ thesaurus, പൂർണ്ണസംഖ്യ flags, regmatch_T *regmatch, char_u *buf, പൂർണ്ണസംഖ്യ *dir);
സ്ഥിതം ശൂന്യം ins_compl_free(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_compl_need_restart(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_compl_new_leader(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  get_compl_len(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_compl_restart(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_compl_set_original_text(char_u *str, size_t len);
സ്ഥിതം ശൂന്യം ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#എങ്കിൽ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
സ്ഥിതം ശൂന്യം ins_compl_add_list(list_T *list);
സ്ഥിതം ശൂന്യം ins_compl_add_dict(dict_T *dict);
സ്ഥിതം പൂർണ്ണസംഖ്യ get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, പൂർണ്ണസംഖ്യ *startcol);
സ്ഥിതം ശൂന്യം get_cpt_func_completion_matches(callback_T *cb);
സ്ഥിതം callback_T *get_callback_if_cpt_func(char_u *p, പൂർണ്ണസംഖ്യ idx);
സ്ഥിതം ശൂന്യം fill_complete_info_dict(dict_T *di, compl_T *match, പൂർണ്ണസംഖ്യ add_match);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ setup_cpt_sources(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ is_cpt_func_refresh_always(ശൂന്യം);
സ്ഥിതം ശൂന്യം cpt_sources_clear(ശൂന്യം);
സ്ഥിതം ശൂന്യം cpt_compl_refresh(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_compl_key2dir(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_compl_pum_താക്കോൽ(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_compl_key2count(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം ശൂന്യം show_pum(പൂർണ്ണസംഖ്യ prev_w_wrow, പൂർണ്ണസംഖ്യ prev_w_leftcol);
സ്ഥിതം unsigned  quote_meta(char_u *dest, char_u *str, പൂർണ്ണസംഖ്യ len);
സ്ഥിതം പൂർണ്ണസംഖ്യ ins_compl_has_multiple(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_compl_expand_multiple(char_u *str);
സ്ഥിതം ശൂന്യം ins_compl_longest_തിരുകുക(char_u *prefix);
സ്ഥിതം ശൂന്യം ins_compl_make_linear(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ ins_compl_make_cyclic(ശൂന്യം);

#ifdef FEAT_SPELL
സ്ഥിതം ശൂന്യം spell_back_to_badword(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    ശൂന്യം
ins_ctrl_x(ശൂന്യം)
{
    എങ്കിൽ (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	എങ്കിൽ (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	അല്ലെങ്കിൽ
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_സന്ദേശം(ctrl_x_mode));
	edit_submode_pre = ശൂന്യ;
	showmode();
    }
    അല്ലെങ്കിൽ
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
പൂർണ്ണസംഖ്യ ctrl_x_mode_none(ശൂന്യം)
    { തിരികെ ctrl_x_mode == 0; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_normal(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_NORMAL; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_scroll(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_SCROLL; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_whole_വരി(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_WHOLE_വരി; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_files(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_FILES; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_tags(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_TAGS; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_path_patterns(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_path_defines(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_PATH_DEFINES; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_dictionary(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_DICTIONARY; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_thesaurus(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_THESAURUS; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_cmdline(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_function(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_FUNCTION; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_omni(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_OMNI; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_spell(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_SPELL; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_eval(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_EVAL; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_വരി_or_eval(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_WHOLE_വരി || ctrl_x_mode == CTRL_X_EVAL; }
പൂർണ്ണസംഖ്യ ctrl_x_mode_register(ശൂന്യം)
    { തിരികെ ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    പൂർണ്ണസംഖ്യ
ctrl_x_mode_not_default(ശൂന്യം)
{
    തിരികെ ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    പൂർണ്ണസംഖ്യ
ctrl_x_mode_not_defined_yet(ശൂന്യം)
{
    തിരികെ ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    പൂർണ്ണസംഖ്യ
compl_status_adding(ശൂന്യം)
{
    തിരികെ compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    പൂർണ്ണസംഖ്യ
compl_status_sol(ശൂന്യം)
{
    തിരികെ compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    പൂർണ്ണസംഖ്യ
compl_status_local(ശൂന്യം)
{
    തിരികെ compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    ശൂന്യം
compl_status_clear(ശൂന്യം)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compl_dir_forward(ശൂന്യം)
{
    തിരികെ compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compl_shows_dir_forward(ശൂന്യം)
{
    തിരികെ compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compl_shows_dir_backward(ശൂന്യം)
{
    തിരികെ compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    പൂർണ്ണസംഖ്യ
has_compl_option(പൂർണ്ണസംഖ്യ dict_opt)
{
    എങ്കിൽ (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
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
	edit_submode = ശൂന്യ;
	സന്ദേശം_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	എങ്കിൽ (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    എങ്കിൽ (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    പൂർണ്ണസംഖ്യ
vim_is_ctrl_x_താക്കോൽ(പൂർണ്ണസംഖ്യ c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    എങ്കിൽ (ins_compl_pum_താക്കോൽ(c))
	തിരികെ TRUE;

    തിരഞ്ഞെടുപ്പ് (ctrl_x_mode)
    {
	അവസ്ഥ 0:		    // Not in any CTRL-X mode
	    തിരികെ (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	അവസ്ഥ CTRL_X_NOT_DEFINED_YET:
	അവസ്ഥ CTRL_X_CMDLINE_CTRL_X:
	    തിരികെ (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	അവസ്ഥ CTRL_X_SCROLL:
	    തിരികെ (c == Ctrl_Y || c == Ctrl_E);
	അവസ്ഥ CTRL_X_WHOLE_വരി:
	    തിരികെ (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_FILES:
	    തിരികെ (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_DICTIONARY:
	    തിരികെ (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_THESAURUS:
	    തിരികെ (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_TAGS:
	    തിരികെ (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	അവസ്ഥ CTRL_X_PATH_PATTERNS:
	    തിരികെ (c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_PATH_DEFINES:
	    തിരികെ (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	അവസ്ഥ CTRL_X_CMDLINE:
	    തിരികെ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	അവസ്ഥ CTRL_X_FUNCTION:
	    തിരികെ (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_OMNI:
	    തിരികെ (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	അവസ്ഥ CTRL_X_SPELL:
	    തിരികെ (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_EVAL:
	    തിരികെ (c == Ctrl_P || c == Ctrl_N);
	അവസ്ഥ CTRL_X_REGISTER:
	    തിരികെ (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    തിരികെ FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_at_original_text(compl_T *match)
{
    തിരികെ match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_first_match(compl_T *match)
{
    തിരികെ match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    സ്ഥിതം compl_T *
find_original_text_match(ശൂന്യം)
{
    എങ്കിൽ (compl_first_match == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (match_at_original_text(compl_first_match))
	തിരികെ compl_first_match;
    എങ്കിൽ (compl_first_match->cp_prev != ശൂന്യ
	    && match_at_original_text(compl_first_match->cp_prev))
	തിരികെ compl_first_match->cp_prev;
    തിരികെ ശൂന്യ;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_accept_char(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (compl_autocomplete && compl_from_nonkeyword)
	തിരികെ FALSE;

    എങ്കിൽ (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	തിരികെ vim_isIDc(c);

    തിരഞ്ഞെടുപ്പ് (ctrl_x_mode)
    {
	അവസ്ഥ CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    തിരികെ vim_isfilec(c) && !vim_ispathsep(c);

	അവസ്ഥ CTRL_X_CMDLINE:
	അവസ്ഥ CTRL_X_CMDLINE_CTRL_X:
	അവസ്ഥ CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    തിരികെ vim_isprintc(c) && !VIM_ISWHITE(c);

	അവസ്ഥ CTRL_X_WHOLE_വരി:
	    // For while line completion a space can be part of the line.
	    തിരികെ vim_isprintc(c);
    }
    തിരികെ vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    സ്ഥിതം char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    പൂർണ്ണസംഖ്യ	    char_len,
    പൂർണ്ണസംഖ്യ	    compl_char_len,
    പൂർണ്ണസംഖ്യ	    min_len,
    char_u  **tofree)
{
    പൂർണ്ണസംഖ്യ		*wca;			// Wide character array.
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i, c;
    പൂർണ്ണസംഖ്യ		has_lower = FALSE;
    പൂർണ്ണസംഖ്യ		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(പൂർണ്ണസംഖ്യ, char_len);
    എങ്കിൽ (wca == ശൂന്യ)
	തിരികെ IObuff;

    p = str;
    വേണ്ടി (i = 0; i < char_len; ++i)
    {
	എങ്കിൽ (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	അല്ലെങ്കിൽ
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    വേണ്ടി (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	എങ്കിൽ (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    എങ്കിൽ (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		വേണ്ടി (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		നിർത്തുക;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    എങ്കിൽ (!has_lower)
    {
	p = compl_orig_text.string;
	വേണ്ടി (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    എങ്കിൽ (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		വേണ്ടി (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		നിർത്തുക;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    വേണ്ടി (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	എങ്കിൽ (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	അല്ലെങ്കിൽ എങ്കിൽ (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    ആകുമ്പോൾ (i < char_len)
    {
	എങ്കിൽ (gap.ga_data != ശൂന്യ)
	{
	    എങ്കിൽ (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		തിരികെ (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    എങ്കിൽ (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    അല്ലെങ്കിൽ
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    എങ്കിൽ (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		തിരികെ (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (പൂർണ്ണസംഖ്യ)(p - IObuff);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	അല്ലെങ്കിൽ
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    എങ്കിൽ (gap.ga_data != ശൂന്യ)
    {
	*tofree = gap.ga_data;
	തിരികെ gap.ga_data;
    }

    *p = NUL;
    തിരികെ IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    പൂർണ്ണസംഖ്യ
ins_compl_add_infercase(
    char_u	*str_arg,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		icase,
    char_u	*fname,
    പൂർണ്ണസംഖ്യ		dir,
    പൂർണ്ണസംഖ്യ		cont_s_ipos,  // next ^X<> will set initial_pos
    പൂർണ്ണസംഖ്യ		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		char_len;		// count multi-byte characters
    പൂർണ്ണസംഖ്യ		compl_char_len;
    പൂർണ്ണസംഖ്യ		min_len;
    പൂർണ്ണസംഖ്യ		flags = 0;
    പൂർണ്ണസംഖ്യ		res;
    char_u	*tofree = ശൂന്യ;

    എങ്കിൽ (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	എങ്കിൽ (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    ആകുമ്പോൾ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    char_len = len;
	}

	// Find actual length of original text.
	എങ്കിൽ (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    ആകുമ്പോൾ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    compl_char_len = compl_length;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    എങ്കിൽ (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    എങ്കിൽ (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, ശൂന്യ, ശൂന്യ, dir, flags, FALSE, ശൂന്യ, score);
    vim_free(tofree);
    തിരികെ res;
}

/*
 * Check if fuzzy matching is enabled
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cot_fuzzy(ശൂന്യം)
{
    തിരികെ (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_nearest_active(ശൂന്യം)
{
    തിരികെ (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    പൂർണ്ണസംഖ്യ
ins_compl_is_match_selected(ശൂന്യം)
{
    തിരികെ compl_shown_match != ശൂന്യ && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_preinsert_longest(ശൂന്യം)
{
    തിരികെ compl_autocomplete
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_add(
    char_u	*str,
    പൂർണ്ണസംഖ്യ		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_data UNUSED,  // "user_data" entry or NULL
    പൂർണ്ണസംഖ്യ		cdir,
    പൂർണ്ണസംഖ്യ		flags_arg,
    പൂർണ്ണസംഖ്യ		adup,		    // accept duplicate match
    പൂർണ്ണസംഖ്യ		*user_hl,           // user abbr/kind hlattr
    പൂർണ്ണസംഖ്യ		score)
{
    compl_T	*match, *current, *prev;
    പൂർണ്ണസംഖ്യ		dir = (cdir == 0 ? compl_direction : cdir);
    പൂർണ്ണസംഖ്യ		flags = flags_arg;
    പൂർണ്ണസംഖ്യ		inserted = FALSE;
    char_u	*new_str = ശൂന്യ;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    എങ്കിൽ (flags & CP_FAST)
	fast_breakcheck();
    അല്ലെങ്കിൽ
	ui_breakcheck();
    എങ്കിൽ (got_int)
	തിരികെ FAIL;
    എങ്കിൽ (len < 0)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(str);

    // If the same match is already present, don't add it.
    എങ്കിൽ (compl_first_match != ശൂന്യ && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *താക്കോൽ;
	hashitem_T  *hi;

	എങ്കിൽ (len < (പൂർണ്ണസംഖ്യ)വലുപ്പം(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    താക്കോൽ = keybuf;
	}
	അല്ലെങ്കിൽ
	{
	    new_str = vim_strnsave(str, len);
	    എങ്കിൽ (new_str == ശൂന്യ)
		തിരികെ FAIL;
	    താക്കോൽ = new_str;
	}
	str_hash = hash_hash(താക്കോൽ);
	hi = hash_lookup(&compl_strings_ht, താക്കോൽ, str_hash);
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    എങ്കിൽ (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		ചെയ്യുക
		{
		    എങ്കിൽ (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((പൂർണ്ണസംഖ്യ)match->cp_str.length <= len
					  || match->cp_str.string[len] == NUL))
		    {
			എങ്കിൽ (score < match->cp_score)
			    match->cp_score = score;
			നിർത്തുക;
		    }
		    match = match->cp_next;
		} ആകുമ്പോൾ (match != ശൂന്യ && !is_first_match(match));
	    }
	    vim_free(new_str);
	    തിരികെ NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    എങ്കിൽ (match == ശൂന്യ)
    {
	vim_free(new_str);
	തിരികെ FAIL;
    }
    match->cp_സംഖ്യ = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    എങ്കിൽ (new_str == ശൂന്യ && (new_str = vim_strnsave(str, len)) == ശൂന്യ)
    {
	vim_free(match);
	തിരികെ FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.length = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    എങ്കിൽ (fname != ശൂന്യ
	    && compl_curr_match != ശൂന്യ
	    && compl_curr_match->cp_fname != ശൂന്യ
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    അല്ലെങ്കിൽ എങ്കിൽ (fname != ശൂന്യ)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    അല്ലെങ്കിൽ
	match->cp_fname = ശൂന്യ;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_index;

    എങ്കിൽ (cptext != ശൂന്യ)
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < CPT_എണ്ണം; ++i)
	{
	    എങ്കിൽ (cptext[i] != ശൂന്യ && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    എങ്കിൽ (user_data != ശൂന്യ)
	match->cp_user_data = *user_data;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    എങ്കിൽ (compl_first_match == ശൂന്യ)
	match->cp_next = match->cp_prev = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_next;
	prev = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	ആകുമ്പോൾ (current != ശൂന്യ && current != compl_first_match)
	{
	    എങ്കിൽ (current->cp_score < score)
	    {
		match->cp_next = current;
		match->cp_prev = current->cp_prev;
		എങ്കിൽ (current->cp_prev)
		    current->cp_prev->cp_next = match;
		current->cp_prev = match;
		inserted = TRUE;
		നിർത്തുക;
	    }
	    prev = current;
	    current = current->cp_next;
	}
	എങ്കിൽ (!inserted)
	{
	    prev->cp_next = match;
	    match->cp_prev = prev;
	    match->cp_next = compl_first_match;
	    compl_first_match->cp_prev = match;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dir == FORWARD)
    {
	match->cp_next = compl_curr_match->cp_next;
	match->cp_prev = compl_curr_match;
    }
    അല്ലെങ്കിൽ	// BACKWARD
    {
	match->cp_next = compl_curr_match;
	match->cp_prev = compl_curr_match->cp_prev;
    }
    എങ്കിൽ (match->cp_next)
	match->cp_next->cp_prev = match;
    എങ്കിൽ (match->cp_prev)
	match->cp_prev->cp_next = match;
    അല്ലെങ്കിൽ	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    എങ്കിൽ (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.length,
								    str_hash);

    // Find the longest common string if still doing that.
    എങ്കിൽ (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    തിരികെ OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_equal(compl_T *match, char_u *str, പൂർണ്ണസംഖ്യ len)
{
    എങ്കിൽ (match->cp_flags & CP_EQUAL)
	തിരികെ TRUE;
    എങ്കിൽ (match->cp_flags & CP_ICASE)
	തിരികെ STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    തിരികെ STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_equal_sc(compl_T *match, char_u *str, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ	typed = compl_length;
    പൂർണ്ണസംഖ്യ	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (പൂർണ്ണസംഖ്യ)(compl_longest_end_col - compl_col) : typed;

    എങ്കിൽ ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	തിരികെ ins_compl_equal(match, str, len);

    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)match->cp_str.length < len)
	തിരികെ FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; ++i)
    {
	എങ്കിൽ (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    സ്ഥിതം ശൂന്യം
ins_compl_തിരുകുക_bytes(char_u *p, പൂർണ്ണസംഖ്യ len)
{
    എങ്കിൽ (len == -1)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(ശൂന്യം)
{
    തിരികെ compl_leader.string != ശൂന്യ ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    സ്ഥിതം size_t
ins_compl_leader_len(ശൂന്യം)
{
    തിരികെ compl_leader.string != ശൂന്യ ? compl_leader.length : compl_orig_text.length;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_col_range_attr(linenr_T lnum, പൂർണ്ണസംഖ്യ col)
{
    പൂർണ്ണസംഖ്യ	    start_col;
    പൂർണ്ണസംഖ്യ	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    പൂർണ്ണസംഖ്യ	    attr;

    എങ്കിൽ (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	തിരികെ -1;

    start_col = compl_col + (പൂർണ്ണസംഖ്യ)ins_compl_leader_len();
    എങ്കിൽ (!ins_compl_has_multiple())
	തിരികെ (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    എങ്കിൽ ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	തിരികെ attr;

    തിരികെ -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_has_multiple(ശൂന്യം)
{
    തിരികെ vim_strchr(compl_shown_match->cp_str.string, '\n') != ശൂന്യ;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_lnum_in_range(linenr_T lnum)
{
    എങ്കിൽ (!ins_compl_has_multiple())
	തിരികെ FALSE;
    തിരികെ lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    സ്ഥിതം ശൂന്യം
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    പൂർണ്ണസംഖ്യ		c1, c2;
    പൂർണ്ണസംഖ്യ		had_match;

    എങ്കിൽ (compl_leader.string == ശൂന്യ)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.length);
	എങ്കിൽ (compl_leader.string == ശൂന്യ)
	    തിരികെ;

	compl_leader.length = match->cp_str.length;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_തിരുകുക(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	എങ്കിൽ (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	തിരികെ;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	അല്ലെങ്കിൽ
	{
	    c1 = *p;
	    c2 = *s;
	}
	എങ്കിൽ ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    നിർത്തുക;
	എങ്കിൽ (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	അല്ലെങ്കിൽ
	{
	    ++p;
	    ++s;
	}
    }

    എങ്കിൽ (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.length = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_തിരുകുക(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	എങ്കിൽ (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    സ്ഥിതം ശൂന്യം
ins_compl_add_matches(
    പൂർണ്ണസംഖ്യ		അക്കം_matches,
    char_u	**matches,
    പൂർണ്ണസംഖ്യ		icase)
{
    പൂർണ്ണസംഖ്യ		add_r = OK;
    പൂർണ്ണസംഖ്യ		dir = compl_direction;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, ശൂന്യ, ശൂന്യ, ശൂന്യ, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ശൂന്യ,
		FUZZY_SCORE_NONE);
	എങ്കിൽ (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(അക്കം_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_make_cyclic(ശൂന്യം)
{
    compl_T *match;
    പൂർണ്ണസംഖ്യ	    എണ്ണം = 0;

    എങ്കിൽ (compl_first_match == ശൂന്യ)
	തിരികെ 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    ആകുമ്പോൾ (match->cp_next != ശൂന്യ && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++എണ്ണം;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    തിരികെ എണ്ണം;
}

/*
 * Return whether there currently is a shown match.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_has_shown_match(ശൂന്യം)
{
    തിരികെ compl_shown_match == ശൂന്യ
	|| compl_shown_match != compl_shown_match->cp_next;
}

/*
 * Return whether the shown match is long enough.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_long_shown_match(ശൂന്യം)
{
    തിരികെ (പൂർണ്ണസംഖ്യ)compl_shown_match->cp_str.length
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned പൂർണ്ണസംഖ്യ
get_cot_flags(ശൂന്യം)
{
    തിരികെ curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_upd_pum(ശൂന്യം)
{
    എങ്കിൽ (compl_match_array == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    എങ്കിൽ (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_del_pum(ശൂന്യം)
{
    എങ്കിൽ (compl_match_array == ശൂന്യ)
	തിരികെ;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    പൂർണ്ണസംഖ്യ
pum_wanted(ശൂന്യം)
{
    // 'completeopt' must contain "menu" or "menuone"
    തിരികെ (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pum_enough_matches(ശൂന്യം)
{
    compl_T     *compl;
    പൂർണ്ണസംഖ്യ		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    ചെയ്യുക
    {
	എങ്കിൽ (compl == ശൂന്യ || (!match_at_original_text(compl) && ++i == 2))
	    നിർത്തുക;
	compl = compl->cp_next;
    } ആകുമ്പോൾ (!is_first_match(compl));

    എങ്കിൽ ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	തിരികെ (i >= 1);
    തിരികെ (i >= 2);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    സ്ഥിതം dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ ശൂന്യ;
    fill_complete_info_dict(dict, match, FALSE);
    തിരികെ dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    സ്ഥിതം ശൂന്യം
trigger_complete_changed_event(പൂർണ്ണസംഖ്യ cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    recursive = FALSE;
    save_v_event_T  save_v_event;

    എങ്കിൽ (recursive)
	തിരികെ;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_ഇനങ്ങൾ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    സ്ഥിതം ശൂന്യം*
cp_get_next(ശൂന്യം *കണ്ണി)
{
    തിരികെ ((compl_T*)കണ്ണി)->cp_next;
}

    സ്ഥിതം ശൂന്യം
cp_set_next(ശൂന്യം *കണ്ണി, ശൂന്യം *next)
{
    ((compl_T*)കണ്ണി)->cp_next = (compl_T*)next;
}

    സ്ഥിതം ശൂന്യം*
cp_get_prev(ശൂന്യം* കണ്ണി)
{
    തിരികെ ((compl_T*)കണ്ണി)->cp_prev;
}

    സ്ഥിതം ശൂന്യം
cp_set_prev(ശൂന്യം* കണ്ണി, ശൂന്യം* prev)
{
    ((compl_T*)കണ്ണി)->cp_prev = (compl_T*)prev;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cp_compare_fuzzy(സ്ഥിരം ശൂന്യം* a, സ്ഥിരം ശൂന്യം* b)
{
    പൂർണ്ണസംഖ്യ score_a = ((compl_T*)a)->cp_score;
    പൂർണ്ണസംഖ്യ score_b = ((compl_T*)b)->cp_score;

    എങ്കിൽ (score_a != score_b)
	തിരികെ (score_b > score_a) ? 1 : -1;

    തിരികെ ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cp_compare_nearest(സ്ഥിരം ശൂന്യം* a, സ്ഥിരം ശൂന്യം* b)
{
    പൂർണ്ണസംഖ്യ score_a = ((compl_T*)a)->cp_score;
    പൂർണ്ണസംഖ്യ score_b = ((compl_T*)b)->cp_score;
    എങ്കിൽ (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	തിരികെ 0;
    തിരികെ (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prepend_startcol_text(string_T *dest, string_T *src, പൂർണ്ണസംഖ്യ startcol)
{
    പൂർണ്ണസംഖ്യ prepend_len = compl_col - startcol;
    പൂർണ്ണസംഖ്യ new_length = prepend_len + (പൂർണ്ണസംഖ്യ)src->length;

    dest->length = (size_t)new_length;
    dest->string = alloc(new_length + 1);  // +1 for NUL
    എങ്കിൽ (dest->string == ശൂന്യ)
    {
	dest->length = 0;
	തിരികെ FAIL;
    }

    char_u	*വരി = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, വരി + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->length);
    dest->string[new_length] = NUL;
    തിരികെ OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    സ്ഥിതം string_T *
get_leader_for_startcol(compl_T *match, പൂർണ്ണസംഖ്യ cached)
{
    സ്ഥിതം string_T adjusted_leader = {ശൂന്യ, 0};

    എങ്കിൽ (match == ശൂന്യ)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (cpt_sources_array == ശൂന്യ)
	പോകുക theend;

    പൂർണ്ണസംഖ്യ	cpt_idx = match->cp_cpt_source_idx;
    എങ്കിൽ (cpt_idx < 0)
	പോകുക theend;
    പൂർണ്ണസംഖ്യ	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    എങ്കിൽ (compl_leader.string == ശൂന്യ)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	എങ്കിൽ (startcol < 0 || startcol >= compl_col)
	    തിരികെ &compl_orig_text;
	തിരികെ &compl_leader;  // pass through (startcol < compl_col)
    }

    എങ്കിൽ (compl_col <= 0)
	പോകുക theend;

    എങ്കിൽ (startcol >= 0 && startcol < compl_col)
    {
	പൂർണ്ണസംഖ്യ prepend_len = compl_col - startcol;
	പൂർണ്ണസംഖ്യ new_length = prepend_len + (പൂർണ്ണസംഖ്യ)compl_leader.length;
	എങ്കിൽ (cached && (size_t)new_length == adjusted_leader.length
		&& adjusted_leader.string != ശൂന്യ)
	    തിരികെ &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	എങ്കിൽ (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    പോകുക theend;

	തിരികെ &adjusted_leader;
    }
theend:
    തിരികെ &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    സ്ഥിതം ശൂന്യം
set_fuzzy_score(ശൂന്യം)
{
    compl_T *compl;
    char_u  *pattern;
    പൂർണ്ണസംഖ്യ	    use_leader;

    എങ്കിൽ (compl_first_match == ശൂന്യ)
	തിരികെ;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ശൂന്യ && compl_leader.length > 0);
    എങ്കിൽ (!use_leader)
    {
	എങ്കിൽ (compl_orig_text.string == ശൂന്യ || compl_orig_text.length == 0)
	    തിരികെ;
	pattern = compl_orig_text.string;
    }
    അല്ലെങ്കിൽ
    {
	/* Clear the leader cache once before the loop */
	(ശൂന്യം)get_leader_for_startcol(ശൂന്യ, TRUE);
	pattern = ശൂന്യ;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    പൂർണ്ണസംഖ്യ next = 0;
    compl = compl_first_match;
    ചെയ്യുക
    {
	എങ്കിൽ (compl->cp_order < 0)
	    compl->cp_order = next;
	next = MAX(next, compl->cp_order + 1);

	എങ്കിൽ (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_next;
    } ആകുമ്പോൾ (compl != ശൂന്യ && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    സ്ഥിതം ശൂന്യം
sort_compl_match_list(പൂർണ്ണസംഖ്യ (*compare)(സ്ഥിരം ശൂന്യം *, സ്ഥിരം ശൂന്യം *))
{
    compl_T	*orig_text;

    എങ്കിൽ (!compl_first_match || is_first_match(compl_first_match->cp_next))
	തിരികെ;

    orig_text = find_original_text_match();
    എങ്കിൽ (orig_text == ശൂന്യ)
	തിരികെ;

    ins_compl_make_linear();
    എങ്കിൽ (orig_text == compl_first_match)
    {
	compl_first_match->cp_next->cp_prev = ശൂന്യ;
	compl_first_match->cp_next = mergesort_list(compl_first_match->cp_next,
		cp_get_next, cp_set_next, cp_get_prev, cp_set_prev, compare);
	compl_first_match->cp_next->cp_prev = compl_first_match;
    }
    അല്ലെങ്കിൽ
    {
	compl_T	*tail;

	orig_text->cp_prev->cp_next = ശൂന്യ;
	compl_first_match = mergesort_list(compl_first_match, cp_get_next,
		cp_set_next, cp_get_prev, cp_set_prev, compare);
	tail = compl_first_match;
	ആകുമ്പോൾ (tail->cp_next != ശൂന്യ)
	    tail = tail->cp_next;
	tail->cp_next = orig_text;
	orig_text->cp_prev = tail;
    }
    (ശൂന്യം)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_user_highlight_attr(പൂർണ്ണസംഖ്യ hl_id)
{
    പൂർണ്ണസംഖ്യ	    attr;

    എങ്കിൽ (hl_id <= 0)
	തിരികെ -1;
    attr = syn_id2attr(hl_id);
    തിരികെ attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_build_pum(ശൂന്യം)
{
    compl_T     *compl;
    compl_T     *shown_compl = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		did_find_shown_match = FALSE;
    പൂർണ്ണസംഖ്യ		shown_match_ok = FALSE;
    പൂർണ്ണസംഖ്യ		i = 0;
    പൂർണ്ണസംഖ്യ		cur = -1;
    പൂർണ്ണസംഖ്യ		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_head = ശൂന്യ;
    compl_T	*match_tail = ശൂന്യ;
    compl_T	*match_next = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		*match_എണ്ണം = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		is_forward = compl_shows_dir_forward();
    പൂർണ്ണസംഖ്യ		is_cpt_completion = (cpt_sources_array != ശൂന്യ);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    എങ്കിൽ (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    എങ്കിൽ (compl_leader.string != ശൂന്യ
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_next;

    എങ്കിൽ (is_cpt_completion)
    {
	match_എണ്ണം = ALLOC_CLEAR_MULT(പൂർണ്ണസംഖ്യ, cpt_sources_എണ്ണം);
	എങ്കിൽ (match_എണ്ണം == ശൂന്യ)
	    തിരികെ -1;
    }

    (ശൂന്യം)get_leader_for_startcol(ശൂന്യ, TRUE); // Clear the cache

    compl = compl_first_match;
    ചെയ്യുക
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	എങ്കിൽ (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	എങ്കിൽ (!match_at_original_text(compl)
		&& (leader->string == ശൂന്യ
		    || ins_compl_equal_sc(compl, leader->string,
			(പൂർണ്ണസംഖ്യ)leader->length)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    പൂർണ്ണസംഖ്യ match_limit_exceeded = FALSE;
	    പൂർണ്ണസംഖ്യ cur_source = compl->cp_cpt_source_idx;
	    എങ്കിൽ (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_എണ്ണം[cur_source]++;
		പൂർണ്ണസംഖ്യ max_matches = cpt_sources_array[cur_source].cs_max_matches;
		എങ്കിൽ (max_matches > 0 && match_എണ്ണം[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    എങ്കിൽ (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		എങ്കിൽ (match_head == ശൂന്യ)
		    match_head = compl;
		അല്ലെങ്കിൽ
		    match_tail->cp_match_next = compl;
		match_tail = compl;

		എങ്കിൽ (!shown_match_ok && !cot_fuzzy())
		{
		    എങ്കിൽ (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    അല്ലെങ്കിൽ
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cot_fuzzy())
		{
		    എങ്കിൽ (i == 0)
			shown_compl = compl;

		    എങ്കിൽ (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	എങ്കിൽ (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    എങ്കിൽ (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    എങ്കിൽ (!shown_match_ok && shown_compl != ശൂന്യ)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } ആകുമ്പോൾ (compl != ശൂന്യ && !is_first_match(compl));

    vim_free(match_എണ്ണം);

    എങ്കിൽ (compl_match_arraysize == 0)
	തിരികെ -1;

    എങ്കിൽ (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    എങ്കിൽ (compl_match_array == ശൂന്യ)
	തിരികെ -1;

    compl = match_head;
    i = 0;
    ആകുമ്പോൾ (compl != ശൂന്യ)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != ശൂന്യ
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != ശൂന്യ
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_next = compl->cp_match_next;
	compl->cp_match_next = ശൂന്യ;
	compl = match_next;
    }

    എങ്കിൽ (!shown_match_ok)    // no displayed match at all
	cur = -1;

    തിരികെ cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    ശൂന്യം
ins_compl_show_pum(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		cur = -1;
    colnr_T	col;

    എങ്കിൽ (!pum_wanted() || !pum_enough_matches())
	തിരികെ;

    // Avoid redrawing the screen under a pum that stays in place.
    എങ്കിൽ (!pum_redraw_in_same_position())
	pum_call_update_screen();

    എങ്കിൽ (compl_match_array == ശൂന്യ)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    അല്ലെങ്കിൽ
    {
	// popup menu already exists, only need to find the current item.
	വേണ്ടി (i = 0; i < compl_match_arraysize; ++i)
	{
	    എങ്കിൽ (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (compl_match_array == ശൂന്യ)
    {
#ifdef FEAT_EVAL
	എങ്കിൽ (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	തിരികെ;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    പൂർണ്ണസംഖ്യ pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    എങ്കിൽ (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    എങ്കിൽ (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		flags,		// DICT_FIRST and/or DICT_EXACT
    പൂർണ്ണസംഖ്യ		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    പൂർണ്ണസംഖ്യ		save_p_scs;
    പൂർണ്ണസംഖ്യ		dir = compl_direction;

    എങ്കിൽ (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	എങ്കിൽ (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	അല്ലെങ്കിൽ
#endif
	    തിരികെ;
    }

    buf = alloc(LSIZE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    regmatch.regprog = ശൂന്യ;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    എങ്കിൽ (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    എങ്കിൽ (ctrl_x_mode_വരി_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	എങ്കിൽ (pat_esc == ശൂന്യ)
	    പോകുക theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	എങ്കിൽ (ptr == ശൂന്യ)
	{
	    vim_free(pat_esc);
	    പോകുക theend;
	}
	vim_snprintf((അക്ഷരം *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    അല്ലെങ്കിൽ
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	    പോകുക theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    ആകുമ്പോൾ (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	എങ്കിൽ (flags == DICT_EXACT)
	{
	    എണ്ണം = 1;
	    files = &dict;
	}
	അല്ലെങ്കിൽ
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    എങ്കിൽ (!thesaurus && STRCMP(buf, "spell") == 0)
		എണ്ണം = -1;
	    അല്ലെങ്കിൽ
#endif
		എങ്കിൽ (vim_strchr(buf, '`') != ശൂന്യ
		    || expand_wildcards(1, &buf, &എണ്ണം, &files,
						     EW_FILE|EW_SILENT) != OK)
		എണ്ണം = 0;
	}

#ifdef FEAT_SPELL
	എങ്കിൽ (എണ്ണം == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    എങ്കിൽ (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    അല്ലെങ്കിൽ
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	അല്ലെങ്കിൽ
#endif
	    എങ്കിൽ (എണ്ണം > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(എണ്ണം, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    എങ്കിൽ (flags != DICT_EXACT)
		FreeWild(എണ്ണം, files);
	}
	എങ്കിൽ (flags != 0)
	    നിർത്തുക;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
thesaurus_add_വാക്കുകൾ_in_വരി(
    char_u	*fname,
    char_u	**buf_arg,
    പൂർണ്ണസംഖ്യ		dir,
    char_u	*skip_word)
{
    പൂർണ്ണസംഖ്യ		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    ആകുമ്പോൾ (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	എങ്കിൽ (*ptr == NUL || *ptr == NL)
	    നിർത്തുക;
	wstart = ptr;

	// Find end of the word.
	എങ്കിൽ (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    ആകുമ്പോൾ (*ptr != NUL)
	    {
		പൂർണ്ണസംഖ്യ l = (*mb_ptr2len)(ptr);

		എങ്കിൽ (l < 2 && !vim_iswordc(*ptr))
		    നിർത്തുക;
		ptr += l;
	    }
	അല്ലെങ്കിൽ
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	എങ്കിൽ (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (പൂർണ്ണസംഖ്യ)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    എങ്കിൽ (status == FAIL)
		നിർത്തുക;
	}
    }

    *buf_arg = ptr;
    തിരികെ status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    സ്ഥിതം ശൂന്യം
ins_compl_files(
    പൂർണ്ണസംഖ്യ		എണ്ണം,
    char_u	**files,
    പൂർണ്ണസംഖ്യ		thesaurus,
    പൂർണ്ണസംഖ്യ		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		*dir)
{
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		i;
    FILE	*fp;
    പൂർണ്ണസംഖ്യ		add_r;
    char_u	*leader = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		leader_len = 0;
    പൂർണ്ണസംഖ്യ		score = 0;
    പൂർണ്ണസംഖ്യ		len = 0;
    char_u	*വരി_end = ശൂന്യ;

    എങ്കിൽ (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (പൂർണ്ണസംഖ്യ)ins_compl_leader_len();
    }

    വേണ്ടി (i = 0; i < എണ്ണം && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((അക്ഷരം *)files[i], "r");  // open dictionary file
	എങ്കിൽ (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    സന്ദേശം_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (അക്ഷരം *)files[i]);
	    (ശൂന്യം)സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	എങ്കിൽ (fp == ശൂന്യ)
	    തുടരുക;

	// Read dictionary file line by line.
	// Check each line for a match.
	ആകുമ്പോൾ (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    എങ്കിൽ (cot_fuzzy() && leader_len > 0)
	    {
		വരി_end = find_വരി_end(ptr);
		ആകുമ്പോൾ (ptr < വരി_end)
		{
		    എങ്കിൽ (fuzzy_match_str_in_വരി(&ptr, leader, &len, ശൂന്യ, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_വരി_or_eval()
					? find_വരി_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (പൂർണ്ണസംഖ്യ)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			എങ്കിൽ (add_r == FAIL)
			    നിർത്തുക;
			ptr = end_ptr;  // start from next word
			എങ്കിൽ (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_next
				&& score == compl_first_match->cp_next->cp_score)
			    compl_അക്കം_bests++;
		    }
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (regmatch != ശൂന്യ)
	    {
		ആകുമ്പോൾ (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_വരി_or_eval() ? find_വരി_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (പൂർണ്ണസംഖ്യ)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    എങ്കിൽ (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_വാക്കുകൾ_in_വരി(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    എങ്കിൽ (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    അല്ലെങ്കിൽ എങ്കിൽ (add_r == FAIL)
			നിർത്തുക;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    എങ്കിൽ (*ptr == '\n' || got_int)
			നിർത്തുക;
		}
	    }
	    വരി_breakcheck();
	    ins_compl_check_താക്കോലുകൾ(50, FALSE);
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
    എങ്കിൽ (has_mbyte)
    {
	ആകുമ്പോൾ (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    അല്ലെങ്കിൽ
    {
	ആകുമ്പോൾ (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    തിരികെ ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    എങ്കിൽ (has_mbyte)
    {
	പൂർണ്ണസംഖ്യ start_class = mb_get_class(ptr);
	എങ്കിൽ (start_class > 1)
	    ആകുമ്പോൾ (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		എങ്കിൽ (mb_get_class(ptr) != start_class)
		    നിർത്തുക;
	    }
    }
    അല്ലെങ്കിൽ
	ആകുമ്പോൾ (vim_iswordc(*ptr))
	    ++ptr;
    തിരികെ ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_വരി_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    ആകുമ്പോൾ (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    തിരികെ s;
}

/*
 * Free a completion item in the list
 */
    സ്ഥിതം ശൂന്യം
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    എങ്കിൽ (compl_strings_ht.ht_used > 0 && match->cp_str.string != ശൂന്യ
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    എങ്കിൽ (--entry->cse_എണ്ണം <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    എങ്കിൽ (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < CPT_എണ്ണം; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_data);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    സ്ഥിതം ശൂന്യം
ins_compl_free(ശൂന്യം)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    എങ്കിൽ (compl_first_match == ശൂന്യ)
	തിരികെ;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    ചെയ്യുക
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	ins_compl_item_free(match);
    } ആകുമ്പോൾ (compl_curr_match != ശൂന്യ && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = ശൂന്യ;
    compl_shown_match = ശൂന്യ;
    compl_old_match = ശൂന്യ;
}

/*
 * Reset/clear the completion state.
 */
    ശൂന്യം
ins_compl_clear(ശൂന്യം)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = ശൂന്യ;
    compl_curr_buf = ശൂന്യ;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ശൂന്യ;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_അക്കം_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_active(ശൂന്യം)
{
    തിരികെ compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    പൂർണ്ണസംഖ്യ
ins_compl_win_active(win_T *wp)
{
    തിരികെ ins_compl_active() && wp == compl_curr_win
	&& wp->w_buffer == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_used_match(ശൂന്യം)
{
    തിരികെ compl_used_match;
}

/*
 * Initialize get longest common string.
 */
    ശൂന്യം
ins_compl_init_get_longest(ശൂന്യം)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_interrupted(ശൂന്യം)
{
    തിരികെ compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_enter_selects(ശൂന്യം)
{
    തിരികെ compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(ശൂന്യം)
{
    തിരികെ compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    പൂർണ്ണസംഖ്യ
ins_compl_len(ശൂന്യം)
{
    തിരികെ compl_length;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_has_preinsert(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ cur_cot_flags = get_cot_flags();
    എങ്കിൽ (compl_autocomplete && p_ic && !p_inf)
	തിരികെ FALSE;
    തിരികെ !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_preinsert_effect(ശൂന്യം)
{
    എങ്കിൽ (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	തിരികെ FALSE;

    തിരികെ curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_bs(ശൂന്യം)
{
    char_u	*വരി;
    char_u	*p;

    എങ്കിൽ (ins_compl_preinsert_effect())
	ins_compl_delete();

    വരി = ml_get_curline();
    p = വരി + curwin->w_cursor.col;
    MB_PTR_BACK(വരി, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(p - വരി) - (പൂർണ്ണസംഖ്യ)compl_col < 0
	    || ((പൂർണ്ണസംഖ്യ)(p - വരി) - (പൂർണ്ണസംഖ്യ)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (പൂർണ്ണസംഖ്യ)(p - വരി) - (പൂർണ്ണസംഖ്യ)compl_col
							- compl_length < 0))
	തിരികെ K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    എങ്കിൽ (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.length = (size_t)((p - വരി) - compl_col);
    compl_leader.string = vim_strnsave(വരി + compl_col, compl_leader.length);
    എങ്കിൽ (compl_leader.string == ശൂന്യ)
    {
	compl_leader.length = 0;
	തിരികെ K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    എങ്കിൽ (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    എങ്കിൽ (compl_shown_match != ശൂന്യ)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    തിരികെ NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_need_restart(ശൂന്യം)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    തിരികെ compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    പൂർണ്ണസംഖ്യ
ins_compl_has_autocomplete(ശൂന്യം)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    തിരികെ curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_fuzzy_sort(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    എങ്കിൽ (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	എങ്കിൽ ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    പൂർണ്ണസംഖ്യ none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_prev);
	    എങ്കിൽ (!none_selected)
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
    സ്ഥിതം ശൂന്യം
ins_compl_new_leader(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    save_w_wrow = curwin->w_wrow;
    പൂർണ്ണസംഖ്യ	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_തിരുകുക_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    എങ്കിൽ (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.length);
	എങ്കിൽ (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	എങ്കിൽ (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	എങ്കിൽ (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	അല്ലെങ്കിൽ
	    compl_autocomplete = FALSE;
	എങ്കിൽ (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    എങ്കിൽ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    എങ്കിൽ (compl_match_array == ശൂന്യ)
	compl_enter_selects = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (ins_compl_has_preinsert() && compl_leader.length > 0)
	ins_compl_തിരുകുക(TRUE, FALSE);
    അല്ലെങ്കിൽ എങ്കിൽ (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.length > 0 && !ins_compl_preinsert_effect())
	ins_compl_തിരുകുക(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_compl_len(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ off = (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col - (പൂർണ്ണസംഖ്യ)compl_col;
    തിരികെ MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ശൂന്യം
ins_compl_addleader(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ	    cc;

    എങ്കിൽ (ins_compl_preinsert_effect())
	ins_compl_delete();

    എങ്കിൽ (stop_arrow() == FAIL)
	തിരികെ;
    എങ്കിൽ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	എങ്കിൽ (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    അല്ലെങ്കിൽ
    {
	ins_char(c);
	എങ്കിൽ (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    എങ്കിൽ (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    എങ്കിൽ (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.length = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.length);
	എങ്കിൽ (compl_leader.string == ശൂന്യ)
	{
	    compl_leader.length = 0;
	    തിരികെ;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_restart(ശൂന്യം)
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
    compl_അക്കം_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    എങ്കിൽ (match == ശൂന്യ)
	തിരികെ;

    p = vim_strnsave(str, len);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.length = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ശൂന്യം
ins_compl_addfrommatch(ശൂന്യം)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len = (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col - (പൂർണ്ണസംഖ്യ)compl_col;
    പൂർണ്ണസംഖ്യ		c;

    p = compl_shown_match->cp_str.string;
    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)compl_shown_match->cp_str.length <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	എങ്കിൽ (!match_at_original_text(compl_shown_match))
	    തിരികെ;

	p = ശൂന്യ;
	plen = 0;
	വേണ്ടി (cp = compl_shown_match->cp_next; cp != ശൂന്യ
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    എങ്കിൽ (compl_leader.string == ശൂന്യ
		    || ins_compl_equal(cp, compl_leader.string,
			(പൂർണ്ണസംഖ്യ)compl_leader.length))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.length;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (p == ശൂന്യ || (പൂർണ്ണസംഖ്യ)plen <= len)
	    തിരികെ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ctrl_x_mode(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ	    retval = FALSE;

    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ Ctrl_E:
	അവസ്ഥ Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    എങ്കിൽ (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    അല്ലെങ്കിൽ
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ശൂന്യ;
	    showmode();
	    നിർത്തുക;
	അവസ്ഥ Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_വരി;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    എങ്കിൽ (vpeekc() == '=')
		നിർത്തുക;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    നിർത്തുക;
#ifdef FEAT_COMPL_FUNC
	അവസ്ഥ Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    നിർത്തുക;
#endif
	അവസ്ഥ 's':
	അവസ്ഥ Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    നിർത്തുക;
	അവസ്ഥ Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    നിർത്തുക;
#ifdef FEAT_FIND_ID
	അവസ്ഥ Ctrl_I:
	അവസ്ഥ K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    നിർത്തുക;
#endif
	അവസ്ഥ Ctrl_V:
	അവസ്ഥ Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ശൂന്യ;
	    showmode();
	    retval = TRUE;
	    നിർത്തുക;
	അവസ്ഥ Ctrl_P:
	അവസ്ഥ Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    എങ്കിൽ (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    അല്ലെങ്കിൽ എങ്കിൽ (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	സ്ഥിരസ്ഥിതി:
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
	    എങ്കിൽ (c == Ctrl_X)
	    {
		എങ്കിൽ (compl_cont_mode != 0)
		    compl_cont_status = 0;
		അല്ലെങ്കിൽ
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ശൂന്യ;
	    showmode();
	    നിർത്തുക;
    }

    തിരികെ retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    സ്ഥിതം ശൂന്യം
trigger_complete_done_event(പൂർണ്ണസംഖ്യ mode UNUSED, string_T *word UNUSED)
{
#എങ്കിൽ defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    എങ്കിൽ (word == ശൂന്യ || word->string == ശൂന്യ)
	(ശൂന്യം)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    അല്ലെങ്കിൽ
	(ശൂന്യം)dict_add_string_len(v_event, "complete_word", word->string, (പൂർണ്ണസംഖ്യ)word->length);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (ശൂന്യം)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == ശൂന്യ) ? (char_u *)"" : mode_str->string, (പൂർണ്ണസംഖ്യ)mode_str->length);

    dict_set_ഇനങ്ങൾ_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#എങ്കിൽ defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_stop(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ prev_mode, പൂർണ്ണസംഖ്യ retval)
{
    പൂർണ്ണസംഖ്യ		want_cindent;
    string_T	word = {ശൂന്യ, 0};

    // Remove pre-inserted text when present.
    എങ്കിൽ (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    എങ്കിൽ (compl_curr_match != ശൂന്യ || compl_leader.string != ശൂന്യ || c == Ctrl_E)
    {
	char_u	*ptr = ശൂന്യ;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	എങ്കിൽ (compl_curr_match != ശൂന്യ && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    എങ്കിൽ (compl_cont_mode == CTRL_X_WHOLE_വരി)
    {
	// re-indent the current line
	എങ്കിൽ (want_cindent)
	{
	    do_c_വ്യഞ്ജകം_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!compl_autocomplete || compl_used_match)
    {
	പൂർണ്ണസംഖ്യ prev_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	എങ്കിൽ (prev_col > 0)
	    dec_cursor();
	// only format when something was inserted
	എങ്കിൽ (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	എങ്കിൽ (prev_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    എങ്കിൽ ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.length);
	എങ്കിൽ (word.string == ശൂന്യ)
	    word.length = 0;
	അല്ലെങ്കിൽ
	    word.length = compl_shown_match->cp_str.length;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    എങ്കിൽ (c == Ctrl_E)
    {
	char_u *p = ശൂന്യ;
	size_t	plen = 0;

	ins_compl_delete();
	എങ്കിൽ (compl_leader.string != ശൂന്യ)
	{
	    p = compl_leader.string;
	    plen = compl_leader.length;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (compl_first_match != ശൂന്യ)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.length;
	}
	എങ്കിൽ (p != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	    compl_len = get_compl_len();

	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)plen > compl_len)
		ins_compl_തിരുകുക_bytes(p + compl_len, (പൂർണ്ണസംഖ്യ)plen - compl_len);
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
    എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU))
	സന്ദേശം_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    എങ്കിൽ (edit_submode != ശൂന്യ)
    {
	edit_submode = ശൂന്യ;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_അക്കം_bests = 0;
    compl_ins_end_col = 0;

    എങ്കിൽ (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    എങ്കിൽ (want_cindent && in_cinkeys(താക്കോൽ_COMPLETE, ' ', inindent(0)))
	do_c_വ്യഞ്ജകം_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(prev_mode, &word);
    vim_free(word.string);

    തിരികെ retval;
}

/*
 * Cancel completion.
 */
    പൂർണ്ണസംഖ്യ
ins_compl_cancel(ശൂന്യം)
{
    തിരികെ ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    പൂർണ്ണസംഖ്യ
ins_compl_prep(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ	    retval = FALSE;
    പൂർണ്ണസംഖ്യ	    prev_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    എങ്കിൽ (c != Ctrl_R && vim_is_ctrl_x_താക്കോൽ(c))
	edit_submode_extra = ശൂന്യ;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    എങ്കിൽ (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	തിരികെ retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    എങ്കിൽ (is_mouse_താക്കോൽ(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	എങ്കിൽ (c == K_LEFTRELEASE
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
	    തിരികെ retval;
	എങ്കിൽ (popup_visible)
	{
	    പൂർണ്ണസംഖ്യ	    row = mouse_row;
	    പൂർണ്ണസംഖ്യ	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    എങ്കിൽ (wp != ശൂന്യ && WIN_IS_POPUP(wp))
		തിരികെ retval;
	}
    }
#endif

    എങ്കിൽ (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	എങ്കിൽ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_താക്കോൽ(c)
		|| !vim_is_ctrl_x_താക്കോൽ(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    എങ്കിൽ (c == Ctrl_Z)
		retval = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    എങ്കിൽ (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    എങ്കിൽ (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	എങ്കിൽ (c != Ctrl_R && !vim_is_ctrl_x_താക്കോൽ(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = ശൂന്യ;
	}
	showmode();
    }

    എങ്കിൽ (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	എങ്കിൽ ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_താക്കോൽ(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, prev_mode, retval);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode == CTRL_X_LOCAL_സന്ദേശം)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, ശൂന്യ);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    എങ്കിൽ (c != Ctrl_R && !vim_is_ctrl_x_താക്കോൽ(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    തിരികെ retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    പൂർണ്ണസംഖ്യ	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    എങ്കിൽ (ptr == ശൂന്യ)
    {
	എങ്കിൽ (compl_leader.string != ശൂന്യ)
	    ptr = compl_leader.string;
	അല്ലെങ്കിൽ
	    തിരികെ;  // nothing to do
    }
    എങ്കിൽ (compl_orig_text.string != ശൂന്യ)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	ആകുമ്പോൾ (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	എങ്കിൽ (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	വേണ്ടി (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    എങ്കിൽ (ptr != ശൂന്യ)
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
    സ്ഥിതം buf_T *
ins_compl_next_buf(buf_T *buf, പൂർണ്ണസംഖ്യ flag)
{
    സ്ഥിതം win_T    *wp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    skip_buffer;

    എങ്കിൽ (flag == 'w')		// just windows
    {
	എങ്കിൽ (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	ആകുമ്പോൾ (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_next != ശൂന്യ) ? wp->w_next : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    എങ്കിൽ (wp == curwin || !wp->w_buffer->b_scanned)
		നിർത്തുക;
	}
	buf = wp->w_buffer;
    }
    അല്ലെങ്കിൽ
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	ആകുമ്പോൾ (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_next != ശൂന്യ) ? buf->b_next : firstbuf;
	    // Break if we're back at start buffer
	    എങ്കിൽ (buf == curbuf)
		നിർത്തുക;

	    // Check buffer conditions based on flag
	    എങ്കിൽ (flag == 'U')
		skip_buffer = buf->b_p_bl;
	    അല്ലെങ്കിൽ
		skip_buffer = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ശൂന്യ) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    എങ്കിൽ (!skip_buffer && !buf->b_scanned)
		നിർത്തുക;
	}
    }
    തിരികെ buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_cpt_sources_എണ്ണം(char_u *cpt)
{
    char_u  dummy[LSIZE];
    പൂർണ്ണസംഖ്യ	    എണ്ണം = 0;
    char_u  *p;

    വേണ്ടി (p = cpt; *p != NUL; )
    {
	ആകുമ്പോൾ (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	എങ്കിൽ (*p != NUL)
	{
	    (ശൂന്യം)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    എണ്ണം++;
	}
    }

    തിരികെ എണ്ണം;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
സ്ഥിതം callback_T cfu_cb;	    // 'completefunc' callback function
സ്ഥിതം callback_T ofu_cb;	    // 'omnifunc' callback function
സ്ഥിതം callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
സ്ഥിതം callback_T *cpt_cb;	    // Callback functions associated with F{func}
സ്ഥിതം പൂർണ്ണസംഖ്യ	  cpt_cb_എണ്ണം;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    സ്ഥിതം ശൂന്യം
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    എങ്കിൽ (globcb->cb_name != ശൂന്യ && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    അക്ഷരം *
did_set_completefunc(optset_T *args)
{
    പൂർണ്ണസംഖ്യ	retval;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    അല്ലെങ്കിൽ
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	എങ്കിൽ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    തിരികെ retval == FAIL ? e_invalid_argument : ശൂന്യ;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    ശൂന്യം
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
    അക്ഷരം *
did_set_omnifunc(optset_T *args)
{
    പൂർണ്ണസംഖ്യ	retval;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    അല്ലെങ്കിൽ
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	എങ്കിൽ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    തിരികെ retval == FAIL ? e_invalid_argument : ശൂന്യ;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    ശൂന്യം
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    ശൂന്യം
clear_cpt_callbacks(callback_T **callbacks, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (callbacks == ശൂന്യ || *callbacks == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_cpt_callbacks(callback_T **dest, പൂർണ്ണസംഖ്യ *dest_cnt, callback_T *src, പൂർണ്ണസംഖ്യ cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    എങ്കിൽ (cnt == 0)
	തിരികെ OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    എങ്കിൽ (*dest == ശൂന്യ)
	തിരികെ FAIL;

    *dest_cnt = cnt;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cnt; i++)
	എങ്കിൽ (src[i].cb_name != ശൂന്യ && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    തിരികെ OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    ശൂന്യം
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    (ശൂന്യം)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_എണ്ണം, cpt_cb,
	    cpt_cb_എണ്ണം);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, പൂർണ്ണസംഖ്യ *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    idx = 0;
    പൂർണ്ണസംഖ്യ	    slen;
    പൂർണ്ണസംഖ്യ	    എണ്ണം;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    എണ്ണം = get_cpt_sources_എണ്ണം(cpt);
    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, എണ്ണം);
    എങ്കിൽ (*cbp == ശൂന്യ)
	തിരികെ FAIL;
    *cnt = എണ്ണം;

    വേണ്ടി (p = cpt; *p != NUL; )
    {
	ആകുമ്പോൾ (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	എങ്കിൽ (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    എങ്കിൽ (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		എങ്കിൽ (caret != ശൂന്യ)
		    *caret = NUL;

		എങ്കിൽ (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ശൂന്യ;
	    }
	    idx++;
	}
    }

    തിരികെ OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    പൂർണ്ണസംഖ്യ
set_cpt_callbacks(optset_T *args)
{
    പൂർണ്ണസംഖ്യ	    opt_flags = args->os_flags;

    എങ്കിൽ (curbuf == ശൂന്യ)
	തിരികെ FAIL;

    // ":setglobal" does not change the buffer-local option value
    എങ്കിൽ (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_എണ്ണം) != OK)
	തിരികെ FAIL;

    // ":setlocal" does not change the global option value
    എങ്കിൽ (opt_flags & OPT_LOCAL)
	തിരികെ OK;

    // Cache the callbacks for the global value
    എങ്കിൽ (opt_flags & OPT_GLOBAL)
	തിരികെ parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_എണ്ണം);

    // set case
    തിരികെ copy_cpt_callbacks(&cpt_cb, &cpt_cb_എണ്ണം, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_എണ്ണം);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    അക്ഷരം *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    പൂർണ്ണസംഖ്യ	retval;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    അല്ലെങ്കിൽ
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	എങ്കിൽ (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    തിരികെ retval == FAIL ? e_invalid_argument : ശൂന്യ;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_cpt_callbacks(callback_T *callbacks, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ	abort = FALSE;

    എങ്കിൽ (callbacks == ശൂന്യ)
	തിരികെ FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    തിരികെ abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_insexpand_funcs(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_എണ്ണം, copyID);

    തിരികെ abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    സ്ഥിതം char_u *
get_complete_funcname(പൂർണ്ണസംഖ്യ type)
{
    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ CTRL_X_FUNCTION:
	    തിരികെ curbuf->b_p_cfu;
	അവസ്ഥ CTRL_X_OMNI:
	    തിരികെ curbuf->b_p_ofu;
	അവസ്ഥ CTRL_X_THESAURUS:
	    തിരികെ *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    സ്ഥിതം callback_T *
get_തിരുകുക_callback(പൂർണ്ണസംഖ്യ type)
{
    എങ്കിൽ (type == CTRL_X_FUNCTION)
	തിരികെ &curbuf->b_cfu_cb;
    എങ്കിൽ (type == CTRL_X_OMNI)
	തിരികെ &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    തിരികെ (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    സ്ഥിതം ശൂന്യം
expand_by_function(പൂർണ്ണസംഖ്യ type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = ശൂന്യ;
    dict_T	*matchdict = ശൂന്യ;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		save_State = State;
    പൂർണ്ണസംഖ്യ		retval;
    പൂർണ്ണസംഖ്യ		is_cpt_function = (cb != ശൂന്യ);
    പൂർണ്ണസംഖ്യ		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    എങ്കിൽ (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	എങ്കിൽ (*funcname == NUL)
	    തിരികെ;
	cb = get_തിരുകുക_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_സംഖ്യ;
    args[0].vval.v_സംഖ്യ = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != ശൂന്യ ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    എങ്കിൽ (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    എങ്കിൽ (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    എങ്കിൽ (retval == OK)
    {
	തിരഞ്ഞെടുപ്പ് (rettv.v_type)
	{
	    അവസ്ഥ VAR_LIST:
		matchlist = rettv.vval.v_list;
		നിർത്തുക;
	    അവസ്ഥ VAR_DICT:
		matchdict = rettv.vval.v_dict;
		നിർത്തുക;
	    അവസ്ഥ VAR_SPECIAL:
		എങ്കിൽ (rettv.vval.v_സംഖ്യ == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    സ്ഥിരസ്ഥിതി:
		// TODO: Give error message?
		clear_tv(&rettv);
		നിർത്തുക;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    എങ്കിൽ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	പോകുക theend;
    }

    എങ്കിൽ (matchlist != ശൂന്യ)
	ins_compl_add_list(matchlist);
    അല്ലെങ്കിൽ എങ്കിൽ (matchdict != ശൂന്യ)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    എങ്കിൽ (matchdict != ശൂന്യ)
	dict_unref(matchdict);
    എങ്കിൽ (matchlist != ശൂന്യ)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#എങ്കിൽ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
get_user_highlight_id(char_u *hlname)
{
    എങ്കിൽ (hlname != ശൂന്യ && *hlname != NUL)
	തിരികെ syn_check_group(hlname, (പൂർണ്ണസംഖ്യ)STRLEN(hlname));
    തിരികെ 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_add_tv(typval_T *tv, പൂർണ്ണസംഖ്യ dir, പൂർണ്ണസംഖ്യ fast)
{
    char_u	*word;
    പൂർണ്ണസംഖ്യ		dup = FALSE;
    പൂർണ്ണസംഖ്യ		empty = FALSE;
    പൂർണ്ണസംഖ്യ		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_എണ്ണം]);
    typval_T	user_data;
    പൂർണ്ണസംഖ്യ		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    പൂർണ്ണസംഖ്യ		user_hl[2] = { 0, 0 };

    user_data.v_type = VAR_UNKNOWN;
    എങ്കിൽ (tv->v_type == VAR_DICT && tv->vval.v_dict != ശൂന്യ)
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
	എങ്കിൽ (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ശൂന്യ
				  && dict_get_സംഖ്യ(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	എങ്കിൽ (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ശൂന്യ)
	    dup = dict_get_സംഖ്യ(tv->vval.v_dict, "dup");
	എങ്കിൽ (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ശൂന്യ)
	    empty = dict_get_സംഖ്യ(tv->vval.v_dict, "empty");
	എങ്കിൽ (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ശൂന്യ
				  && dict_get_സംഖ്യ(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    അല്ലെങ്കിൽ
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    എങ്കിൽ (word == ശൂന്യ || (!empty && *word == NUL))
    {
	clear_tv(&user_data);
	തിരികെ FAIL;
    }
    status = ins_compl_add(word, -1, ശൂന്യ, cptext,
	    &user_data, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    എങ്കിൽ (status != OK)
	clear_tv(&user_data);
    തിരികെ status;
}

/*
 * Add completions from a list.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
    {
	എങ്കിൽ (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	അല്ലെങ്കിൽ എങ്കിൽ (did_emsg)
	    നിർത്തുക;
    }
}

/*
 * Add completions from a dict.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_വാക്കുകൾ;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    എങ്കിൽ (di_refresh != ശൂന്യ && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	എങ്കിൽ (v != ശൂന്യ && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_വാക്കുകൾ = dict_find(dict, (char_u *)"words", 5);
    എങ്കിൽ (di_വാക്കുകൾ != ശൂന്യ && di_വാക്കുകൾ->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_വാക്കുകൾ->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    സ്ഥിതം ശൂന്യം
set_completion(colnr_T startcol, list_T *list)
{
    പൂർണ്ണസംഖ്യ save_w_wrow = curwin->w_wrow;
    പൂർണ്ണസംഖ്യ save_w_leftcol = curwin->w_leftcol;
    പൂർണ്ണസംഖ്യ flags = CP_ORIGINAL_TEXT;
    unsigned പൂർണ്ണസംഖ്യ cur_cot_flags = get_cot_flags();
    പൂർണ്ണസംഖ്യ compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    പൂർണ്ണസംഖ്യ compl_no_തിരുകുക = (cur_cot_flags & COT_NOINSERT) != 0;
    പൂർണ്ണസംഖ്യ compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    എങ്കിൽ (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    എങ്കിൽ (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_length = (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col - (പൂർണ്ണസംഖ്യ)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_length);
    എങ്കിൽ (p_ic)
	flags |= CP_ICASE;
    എങ്കിൽ (compl_orig_text.string == ശൂന്യ)
    {
	compl_orig_text.length = 0;
	തിരികെ;
    }
    compl_orig_text.length = (size_t)compl_length;
    എങ്കിൽ (ins_compl_add(compl_orig_text.string,
			(പൂർണ്ണസംഖ്യ)compl_orig_text.length, ശൂന്യ, ശൂന്യ, ശൂന്യ, 0,
			flags | CP_FAST, FALSE, ശൂന്യ, FUZZY_SCORE_NONE) != OK)
	തിരികെ;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    പൂർണ്ണസംഖ്യ no_select = compl_no_select || compl_longest;
    എങ്കിൽ (compl_no_തിരുകുക || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	എങ്കിൽ (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    അല്ലെങ്കിൽ
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_തിരുകുക;

    // Lazily show the popup menu, unless we got interrupted.
    എങ്കിൽ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    ശൂന്യം
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ ((State & MODE_തിരുകുക) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_തിരുകുക_mode));
	തിരികെ;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    എങ്കിൽ (!undo_allowed())
	തിരികെ;

    എങ്കിൽ (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	പൂർണ്ണസംഖ്യ startcol = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ);
	എങ്കിൽ (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    ശൂന്യം
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    ശൂന്യം
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_താക്കോലുകൾ(0, TRUE);
    rettv->vval.v_സംഖ്യ = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    സ്ഥിതം ശൂന്യം
ins_compl_mode(string_T *ret)
{
    എങ്കിൽ (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == ശൂന്യ) ? (char_u *)"" : mode->string;
	ret->length = mode->length;
    }
    അല്ലെങ്കിൽ
    {
	ret->string = (char_u *)"";
	ret->length = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_update_sequence_numbers(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		സംഖ്യ = 0;
    compl_T	*match;

    എങ്കിൽ (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	വേണ്ടി (match = compl_curr_match->cp_prev; match != ശൂന്യ
		&& !is_first_match(match); match = match->cp_prev)
	    എങ്കിൽ (match->cp_സംഖ്യ != -1)
	    {
		സംഖ്യ = match->cp_സംഖ്യ;
		നിർത്തുക;
	    }
	എങ്കിൽ (match != ശൂന്യ)
	    // go up and assign all numbers which are not assigned yet
	    വേണ്ടി (match = match->cp_next;
		    match != ശൂന്യ && match->cp_സംഖ്യ == -1;
					   match = match->cp_next)
		match->cp_സംഖ്യ = ++സംഖ്യ;
    }
    അല്ലെങ്കിൽ // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	വേണ്ടി (match = compl_curr_match->cp_next; match != ശൂന്യ
		&& !is_first_match(match); match = match->cp_next)
	{
	    എങ്കിൽ (match->cp_സംഖ്യ != -1)
	    {
		സംഖ്യ = match->cp_സംഖ്യ;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (match != ശൂന്യ)
	{
	    // go down and assign all numbers which are not assigned yet
	    വേണ്ടി (match = match->cp_prev; match
		    && match->cp_സംഖ്യ == -1;
					   match = match->cp_prev)
		match->cp_സംഖ്യ = ++സംഖ്യ;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    സ്ഥിതം ശൂന്യം
fill_complete_info_dict(dict_T *di, compl_T *match, പൂർണ്ണസംഖ്യ add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (പൂർണ്ണസംഖ്യ)match->cp_str.length);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    എങ്കിൽ (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    എങ്കിൽ (match->cp_user_data.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    അല്ലെങ്കിൽ
	dict_add_tv(di, "user_data", &match->cp_user_data);
}

/*
 * Get complete information
 */
    സ്ഥിതം ശൂന്യം
get_complete_info(list_T *what_list, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_ഇനങ്ങൾ		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    പൂർണ്ണസംഖ്യ		what_flag;

    എങ്കിൽ (what_list == ശൂന്യ)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    അല്ലെങ്കിൽ
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ഇനങ്ങൾ(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    എങ്കിൽ (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ഇനങ്ങൾ;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    എങ്കിൽ (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (പൂർണ്ണസംഖ്യ)mode.length);
    }

    എങ്കിൽ (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_സംഖ്യ(retdict, "pum_visible", pum_visible());

    എങ്കിൽ (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_സംഖ്യ(retdict, "auto", compl_autostarted);

    എങ്കിൽ (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*വരി = ml_get_curline();
	പൂർണ്ണസംഖ്യ	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? വരി + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    എങ്കിൽ (ret == OK && (what_flag & (CI_WHAT_ഇനങ്ങൾ | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ശൂന്യ;
	dict_T	    *di;
	compl_T     *match;
	പൂർണ്ണസംഖ്യ         selected_idx = -1;
	പൂർണ്ണസംഖ്യ	    has_ഇനങ്ങൾ = what_flag & CI_WHAT_ഇനങ്ങൾ;
	പൂർണ്ണസംഖ്യ	    has_matches = what_flag & CI_WHAT_MATCHES;
	പൂർണ്ണസംഖ്യ	    has_completed = what_flag & CI_WHAT_COMPLETED;

	എങ്കിൽ (has_ഇനങ്ങൾ || has_matches)
	{
	    li = list_alloc();
	    എങ്കിൽ (li == ശൂന്യ)
		തിരികെ;
	    ret = dict_add_list(retdict, (has_matches && !has_ഇനങ്ങൾ)
						? "matches" : "items", li);
	    എങ്കിൽ (ret == FAIL)
		list_unref(li);
	}
	എങ്കിൽ (ret == OK && what_flag & CI_WHAT_SELECTED)
	    എങ്കിൽ (compl_curr_match != ശൂന്യ && compl_curr_match->cp_സംഖ്യ == -1)
		ins_compl_update_sequence_numbers();
	എങ്കിൽ (ret == OK && compl_first_match != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ list_idx = 0;
	    match = compl_first_match;
	    ചെയ്യുക
	    {
		എങ്കിൽ (!match_at_original_text(match))
		{
		    എങ്കിൽ (has_ഇനങ്ങൾ
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			എങ്കിൽ (di == ശൂന്യ)
			    തിരികെ;
			ret = list_append_dict(li, di);
			എങ്കിൽ (ret != OK)
			{
			    dict_unref(di);
			    തിരികെ;
			}
			fill_complete_info_dict(di, match, has_matches && has_ഇനങ്ങൾ);
		    }
		    എങ്കിൽ (compl_curr_match != ശൂന്യ
			    && compl_curr_match->cp_സംഖ്യ == match->cp_സംഖ്യ)
			selected_idx = list_idx;
		    എങ്കിൽ (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_next;
	    }
	    ആകുമ്പോൾ (match != ശൂന്യ && !is_first_match(match));
	}
	എങ്കിൽ (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_സംഖ്യ(retdict, "selected", selected_idx);

	എങ്കിൽ (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    എങ്കിൽ (di == ശൂന്യ)
		തിരികെ;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    എങ്കിൽ (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    ശൂന്യം
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ശൂന്യ;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
	    തിരികെ;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
thesaurus_func_complete(പൂർണ്ണസംഖ്യ type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    തിരികെ type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
may_advance_cpt_index(char_u *cpt)
{
    char_u  *p = cpt;

    എങ്കിൽ (cpt_sources_index == -1)
	തിരികെ FALSE;
    ആകുമ്പോൾ (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    തിരികെ (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
എണ്ണൽ
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
തരനിർവചനം ഘടന
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	prev_match_pos;		// previous match position
    പൂർണ്ണസംഖ്യ		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    പൂർണ്ണസംഖ്യ		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    പൂർണ്ണസംഖ്യ		dict_f;			// "dict" is an exact file name or not
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
process_next_cpt_മൂല്യം(
	ins_compl_next_state_T *st,
	പൂർണ്ണസംഖ്യ		*compl_type_arg,
	pos_T		*start_match_pos,
	പൂർണ്ണസംഖ്യ		fuzzy_collect,
	പൂർണ്ണസംഖ്യ		*advance_cpt_idx)
{
    പൂർണ്ണസംഖ്യ	    compl_type = -1;
    പൂർണ്ണസംഖ്യ	    status = INS_COMPL_CPT_OK;
    പൂർണ്ണസംഖ്യ	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    ആകുമ്പോൾ (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    എങ്കിൽ (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	എങ്കിൽ (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_വരി_എണ്ണം;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ശൂന്യ
	    && (st->ins_buf = ins_compl_next_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	എങ്കിൽ (st->ins_buf->b_ml.ml_mfp != ശൂന്യ)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_വരി_എണ്ണം + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	അല്ലെങ്കിൽ	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    എങ്കിൽ (st->ins_buf->b_fname == ശൂന്യ)
	    {
		status = INS_COMPL_CPT_CONT;
		പോകുക done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	എങ്കിൽ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    സന്ദേശം_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ശൂന്യ
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ശൂന്യ
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (ശൂന്യം)സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (ctrl_x_mode_വരി_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	അല്ലെങ്കിൽ എങ്കിൽ (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_index);
	    എങ്കിൽ (!st->func_cb)
		compl_type = -1;
	}
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (!skip_source)
	{
	    എങ്കിൽ (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		എങ്കിൽ (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		അല്ലെങ്കിൽ
		    compl_type = CTRL_X_THESAURUS;
		എങ്കിൽ (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    അല്ലെങ്കിൽ എങ്കിൽ (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    അല്ലെങ്കിൽ എങ്കിൽ (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    അല്ലെങ്കിൽ എങ്കിൽ (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		എങ്കിൽ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    സന്ദേശം_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, _("Scanning tags."));
		    (ശൂന്യം)സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    അല്ലെങ്കിൽ
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(ശൂന്യം)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_index(st->e_cpt);

	st->found_all = TRUE;
	എങ്കിൽ (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    തിരികെ status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    സ്ഥിതം ശൂന്യം
get_next_include_file_completion(പൂർണ്ണസംഖ്യ compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (പൂർണ്ണസംഖ്യ)compl_pattern.length, FALSE, FALSE,
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
    സ്ഥിതം ശൂന്യം
get_next_dict_tsr_completion(പൂർണ്ണസംഖ്യ compl_type, char_u *dict, പൂർണ്ണസംഖ്യ dict_f)
{
#ifdef FEAT_COMPL_FUNC
    എങ്കിൽ (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, ശൂന്യ);
    അല്ലെങ്കിൽ
#endif
	ins_compl_dictionaries(
		dict != ശൂന്യ ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != ശൂന്യ ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    സ്ഥിതം ശൂന്യം
get_next_tag_completion(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		save_p_ic;
    char_u	**matches;
    പൂർണ്ണസംഖ്യ		അക്കം_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    എങ്കിൽ (find_tags(compl_pattern.string, &അക്കം_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && അക്കം_matches > 0)
	ins_compl_add_matches(അക്കം_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compare_scores(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
{
    പൂർണ്ണസംഖ്യ idx_a = *(സ്ഥിരം പൂർണ്ണസംഖ്യ *)a;
    പൂർണ്ണസംഖ്യ idx_b = *(സ്ഥിരം പൂർണ്ണസംഖ്യ *)b;
    പൂർണ്ണസംഖ്യ score_a = compl_fuzzy_scores[idx_a];
    പൂർണ്ണസംഖ്യ score_b = compl_fuzzy_scores[idx_b];
    തിരികെ score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    സ്ഥിതം ശൂന്യം
ins_compl_longest_തിരുകുക(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_തിരുകുക_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    സ്ഥിതം ശൂന്യം
fuzzy_longest_match(ശൂന്യം)
{
    char_u	*prefix = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		prefix_len = 0;
    പൂർണ്ണസംഖ്യ		i = 0;
    പൂർണ്ണസംഖ്യ		j = 0;
    char_u	*match_str = ശൂന്യ;
    char_u	*prefix_ptr = ശൂന്യ;
    char_u	*match_ptr = ശൂന്യ;
    char_u	*leader = ശൂന്യ;
    size_t	leader_len = 0;
    compl_T	*compl = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		more_candidates = FALSE;
    compl_T	*nn_compl = ശൂന്യ;

    എങ്കിൽ (compl_അക്കം_bests == 0)
	തിരികെ;

    nn_compl = compl_first_match->cp_next->cp_next;
    എങ്കിൽ (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_വരി() ? compl_first_match
				    : compl_first_match->cp_next;
    എങ്കിൽ (compl_അക്കം_bests == 1)
    {
	// no more candidates insert the match str
	എങ്കിൽ (!more_candidates)
	{
	    ins_compl_longest_തിരുകുക(compl->cp_str.string);
	    compl_അക്കം_bests = 0;
	}
	compl_അക്കം_bests = 0;
	തിരികെ;
    }

    എങ്കിൽ ((size_t)compl_അക്കം_bests > SIZE_MAX / വലുപ്പം(compl_T *))
	തിരികെ;
    compl_best_matches = (compl_T **)alloc(compl_അക്കം_bests * വലുപ്പം(compl_T *));
    എങ്കിൽ (compl_best_matches == ശൂന്യ)
	തിരികെ;
    ആകുമ്പോൾ (compl != ശൂന്യ && i < compl_അക്കം_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_next;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (പൂർണ്ണസംഖ്യ)compl_best_matches[0]->cp_str.length;

    വേണ്ടി (i = 1; i < compl_അക്കം_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	ആകുമ്പോൾ (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    എങ്കിൽ (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		നിർത്തുക;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	എങ്കിൽ (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    എങ്കിൽ (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	പോകുക end;

    prefix = vim_strnsave(prefix, prefix_len);
    എങ്കിൽ (prefix != ശൂന്യ)
    {
	ins_compl_longest_തിരുകുക(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = ശൂന്യ;
    compl_അക്കം_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    സ്ഥിതം ശൂന്യം
get_next_filename_completion(ശൂന്യം)
{
    char_u	**matches;
    പൂർണ്ണസംഖ്യ		അക്കം_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    പൂർണ്ണസംഖ്യ		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    പൂർണ്ണസംഖ്യ		*fuzzy_indices_data;
    char_u	*last_sep = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    പൂർണ്ണസംഖ്യ		max_score = 0;
    പൂർണ്ണസംഖ്യ		current_score = 0;
    പൂർണ്ണസംഖ്യ		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    അക്ഷരം pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#അല്ലെങ്കിൽ
    അക്ഷരം pathsep = PATHSEP;
#endif

    എങ്കിൽ (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	എങ്കിൽ (curbuf->b_p_csl[0] == 's')
	{
	    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)leader_len; i++)
	    {
		എങ്കിൽ (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_csl[0] == 'b')
	{
	    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)leader_len; i++)
	    {
		എങ്കിൽ (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	എങ്കിൽ (last_sep == ശൂന്യ)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
		തിരികെ;
	    compl_pattern.length = 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	അല്ലെങ്കിൽ
	{
	    // Split leader into path and file parts
	    പൂർണ്ണസംഖ്യ path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    എങ്കിൽ (path_with_wildcard != ശൂന്യ)
	    {
		vim_snprintf((അക്ഷരം *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.length = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    എങ്കിൽ (expand_wildcards(1, &compl_pattern.string, &അക്കം_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	തിരികെ;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, അക്കം_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    എങ്കിൽ (curbuf->b_p_csl[0] != NUL)
    {
	വേണ്ടി (i = 0; i < അക്കം_matches; ++i)
	{
	    ptr = matches[i];
	    ആകുമ്പോൾ (*ptr != NUL)
	    {
		എങ്കിൽ (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    എങ്കിൽ (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, വലുപ്പം(പൂർണ്ണസംഖ്യ), 10);
	compl_fuzzy_scores = (പൂർണ്ണസംഖ്യ *)alloc(വലുപ്പം(പൂർണ്ണസംഖ്യ) * അക്കം_matches);

	വേണ്ടി (i = 0; i < അക്കം_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    എങ്കിൽ (score != FUZZY_SCORE_NONE)
	    {
		എങ്കിൽ (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((പൂർണ്ണസംഖ്യ *)fuzzy_indices.ga_data)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	എങ്കിൽ (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = ശൂന്യ;
	    fuzzy_indices_data = (പൂർണ്ണസംഖ്യ *)fuzzy_indices.ga_data;
	    qsort(fuzzy_indices_data, fuzzy_indices.ga_len, വലുപ്പം(പൂർണ്ണസംഖ്യ), compare_scores);

	    വേണ്ടി (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_data[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_data[i]];
		എങ്കിൽ (ins_compl_add(match, -1, ശൂന്യ, ശൂന്യ, ശൂന്യ, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ശൂന്യ, current_score) == OK)
		    dir = FORWARD;

		എങ്കിൽ (need_collect_bests)
		{
		    എങ്കിൽ (i == 0 || current_score == max_score)
		    {
			compl_അക്കം_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(അക്കം_matches, matches);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (leader_len > 0)
	{
	    FreeWild(അക്കം_matches, matches);
	    അക്കം_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	എങ്കിൽ (compl_അക്കം_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	തിരികെ;
    }

    എങ്കിൽ (അക്കം_matches > 0)
	ins_compl_add_matches(അക്കം_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    സ്ഥിതം ശൂന്യം
get_next_cmdline_completion(ശൂന്യം)
{
    char_u	**matches;
    പൂർണ്ണസംഖ്യ		അക്കം_matches;

    എങ്കിൽ (expand_cmdline(&compl_xp, compl_pattern.string,
		(പൂർണ്ണസംഖ്യ)compl_pattern.length, &അക്കം_matches, &matches) == EXPAND_OK)
    {
	പൂർണ്ണസംഖ്യ		add_r = OK;
	പൂർണ്ണസംഖ്യ		dir = compl_direction;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_എണ്ണം]) = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};

	    എങ്കിൽ (compl_xp.xp_files_abbr != ശൂന്യ)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    എങ്കിൽ (compl_xp.xp_files_kind != ശൂന്യ)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    എങ്കിൽ (compl_xp.xp_files_menu != ശൂന്യ)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    എങ്കിൽ (compl_xp.xp_files_info != ശൂന്യ)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, ശൂന്യ, cptext, ശൂന്യ, dir,
		    CP_FAST, FALSE, ശൂന്യ, FUZZY_SCORE_NONE);
	    എങ്കിൽ (add_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(അക്കം_matches, matches);
	free_xp_files_extra(&compl_xp, അക്കം_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    സ്ഥിതം ശൂന്യം
get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    പൂർണ്ണസംഖ്യ		അക്കം_matches;

    അക്കം_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    എങ്കിൽ (അക്കം_matches > 0)
	ins_compl_add_matches(അക്കം_matches, matches, p_ic);
    അല്ലെങ്കിൽ
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    സ്ഥിതം char_u *
ins_compl_get_next_word_or_വരി(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	പൂർണ്ണസംഖ്യ	*match_len,
	പൂർണ്ണസംഖ്യ	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (പൂർണ്ണസംഖ്യ)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    എങ്കിൽ (ctrl_x_mode_വരി_or_eval())
    {
	എങ്കിൽ (compl_status_adding())
	{
	    എങ്കിൽ (cur_match_pos->lnum >= ins_buf->b_ml.ml_വരി_എണ്ണം)
		തിരികെ ശൂന്യ;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    എങ്കിൽ (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (പൂർണ്ണസംഖ്യ)(ptr - tmp_ptr);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	char_u	*tmp_ptr = ptr;

	എങ്കിൽ (compl_status_adding() && compl_length <= len)
	{
	    tmp_ptr += compl_length;
	    // Skip if already inside a word.
	    എങ്കിൽ (vim_iswordp(tmp_ptr))
		തിരികെ ശൂന്യ;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (പൂർണ്ണസംഖ്യ)(tmp_ptr - ptr);

	എങ്കിൽ (compl_status_adding() && len == compl_length)
	{
	    എങ്കിൽ (cur_match_pos->lnum < ins_buf->b_ml.ml_വരി_എണ്ണം)
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
		എങ്കിൽ (tmp_ptr > ptr)
		{
		    എങ്കിൽ (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			എങ്കിൽ (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			എങ്കിൽ (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ശൂന്യ
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    എങ്കിൽ (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (പൂർണ്ണസംഖ്യ)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    എങ്കിൽ (len == compl_length)
		തിരികെ ശൂന്യ;
	}
    }

    *match_len = len;
    തിരികെ ptr;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    പൂർണ്ണസംഖ്യ		found_new_match = FAIL;
    പൂർണ്ണസംഖ്യ		save_p_scs;
    പൂർണ്ണസംഖ്യ		save_p_ws;
    പൂർണ്ണസംഖ്യ		looped_around = FALSE;
    char_u	*ptr = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_length > 0;
    char_u	*leader = ins_compl_leader();
    പൂർണ്ണസംഖ്യ		score = FUZZY_SCORE_NONE;
    പൂർണ്ണസംഖ്യ		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    എങ്കിൽ (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    എങ്കിൽ (!in_curbuf)
	p_ws = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	cont_s_ipos = FALSE;

	++സന്ദേശം_silent;  // Don't want messages for wrapscan.

	എങ്കിൽ (in_fuzzy_collect)
	{
	    found_new_match = തിരയൽ_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_whole_വരി() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = തിരയൽ_for_exact_വരി(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	അല്ലെങ്കിൽ
	    found_new_match = searchit(ശൂന്യ, st->ins_buf, st->cur_match_pos,
				ശൂന്യ, compl_direction, compl_pattern.string, (പൂർണ്ണസംഖ്യ)compl_pattern.length,
				1L, തിരയൽ_KEEP + തിരയൽ_NFMSG, RE_LAST, ശൂന്യ);
	--സന്ദേശം_silent;
	എങ്കിൽ (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    എങ്കിൽ (looped_around)
		found_new_match = FAIL;
	    അല്ലെങ്കിൽ
		looped_around = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    എങ്കിൽ (looped_around)
		found_new_match = FAIL;
	    അല്ലെങ്കിൽ
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	എങ്കിൽ (found_new_match == FAIL)
	    നിർത്തുക;

	// when ADDING, the text before the cursor matches, skip it
	എങ്കിൽ (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    തുടരുക;

	എങ്കിൽ (!in_fuzzy_collect)
	    ptr = ins_compl_get_next_word_or_വരി(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	എങ്കിൽ (ptr == ശൂന്യ || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    തുടരുക;

	എങ്കിൽ (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    എങ്കിൽ (score < 0)
		score = -score;
	}

	എങ്കിൽ (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? ശൂന്യ : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    എങ്കിൽ (in_fuzzy_collect && score == compl_first_match->cp_next->cp_score)
		compl_അക്കം_bests++;
	    found_new_match = OK;
	    നിർത്തുക;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    തിരികെ found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    സ്ഥിതം callback_T *
get_callback_if_cpt_func(char_u *p, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (*p == 'o')
	തിരികെ &curbuf->b_ofu_cb;

    എങ്കിൽ (*p == 'F')
    {
	എങ്കിൽ (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    എങ്കിൽ (curbuf->b_p_cpt_cb == ശൂന്യ
		    || idx < 0 || idx >= curbuf->b_p_cpt_എണ്ണം)
		തിരികെ ശൂന്യ;
	    തിരികെ curbuf->b_p_cpt_cb[idx].cb_name != ശൂന്യ
		? &curbuf->b_p_cpt_cb[idx] : ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	    തിരികെ &curbuf->b_cfu_cb; // 'cfu'
    }

    തിരികെ ശൂന്യ;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    സ്ഥിതം ശൂന്യം
get_register_completion(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		dir = compl_direction;
    yankreg_T	*reg = ശൂന്യ;
    ശൂന്യം	*reg_ptr = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		adding_mode = compl_status_adding();

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_REGISTERS; i++)
    {
	പൂർണ്ണസംഖ്യ regname = 0;
	എങ്കിൽ (i == 0)
	    regname = '"';    // unnamed register
	അല്ലെങ്കിൽ എങ്കിൽ (i < 10)
	    regname = '0' + i;
	അല്ലെങ്കിൽ എങ്കിൽ (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	അല്ലെങ്കിൽ എങ്കിൽ (i == STAR_REGISTER)
	    regname = '*';
	അല്ലെങ്കിൽ എങ്കിൽ (i == PLUS_REGISTER)
	    regname = '+';
#endif
	അല്ലെങ്കിൽ
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	എങ്കിൽ (!valid_yank_reg(regname, FALSE) || regname == '_')
	    തുടരുക;

	reg_ptr = get_register(regname, FALSE);
	എങ്കിൽ (reg_ptr == ശൂന്യ)
	    തുടരുക;

	reg = (yankreg_T *)reg_ptr;

	വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < reg->y_size; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    എങ്കിൽ (str == ശൂന്യ)
		തുടരുക;

	    എങ്കിൽ (adding_mode)
	    {
		പൂർണ്ണസംഖ്യ str_len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
		എങ്കിൽ (str_len == 0)
		    തുടരുക;

		എങ്കിൽ (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0))
		{
		    എങ്കിൽ (ins_compl_add_infercase(str, str_len, p_ic, ശൂന്യ,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		ആകുമ്പോൾ (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    എങ്കിൽ (p >= str_end || *p == NUL)
			നിർത്തുക;

		    char_u *word_end = find_word_end(p);

		    എങ്കിൽ (word_end <= p)
		    {
			എങ്കിൽ (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			അല്ലെങ്കിൽ
			    word_end = p + 1;
		    }

		    എങ്കിൽ (word_end > str_end)
			word_end = str_end;

		    പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)(word_end - p);
		    എങ്കിൽ (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0)))
		    {
			എങ്കിൽ (ins_compl_add_infercase(p, len, p_ic, ശൂന്യ,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    എങ്കിൽ (p <= old_p)
		    {
			p = old_p + 1;
			എങ്കിൽ (has_mbyte && p < str_end)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_next_completion_match(പൂർണ്ണസംഖ്യ type, ins_compl_next_state_T *st, pos_T *ini)
{
    പൂർണ്ണസംഖ്യ	found_new_match = FALSE;

    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ -1:
	    നിർത്തുക;
#ifdef FEAT_FIND_ID
	അവസ്ഥ CTRL_X_PATH_PATTERNS:
	അവസ്ഥ CTRL_X_PATH_DEFINES:
	    get_next_include_file_completion(type);
	    നിർത്തുക;
#endif

	അവസ്ഥ CTRL_X_DICTIONARY:
	അവസ്ഥ CTRL_X_THESAURUS:
	    get_next_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ശൂന്യ;
	    നിർത്തുക;

	അവസ്ഥ CTRL_X_TAGS:
	    get_next_tag_completion();
	    നിർത്തുക;

	അവസ്ഥ CTRL_X_FILES:
	    get_next_filename_completion();
	    നിർത്തുക;

	അവസ്ഥ CTRL_X_CMDLINE:
	അവസ്ഥ CTRL_X_CMDLINE_CTRL_X:
	    get_next_cmdline_completion();
	    നിർത്തുക;

#ifdef FEAT_COMPL_FUNC
	അവസ്ഥ CTRL_X_FUNCTION:
	    എങ്കിൽ (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    അല്ലെങ്കിൽ
		expand_by_function(type, compl_pattern.string, ശൂന്യ);
	    നിർത്തുക;
	അവസ്ഥ CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, ശൂന്യ);
	    നിർത്തുക;
#endif

	അവസ്ഥ CTRL_X_SPELL:
	    get_next_spell_completion(st->first_match_pos.lnum);
	    നിർത്തുക;

	അവസ്ഥ CTRL_X_REGISTER:
	    get_register_completion();
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:	// normal ^P/^N and ^X^L
	    found_new_match = get_next_default_completion(st, ini);
	    എങ്കിൽ (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    എങ്കിൽ (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    തിരികെ found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    സ്ഥിതം ശൂന്യം
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *പഠ = str, *ലിഖ = str, *p;

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ;

    ആകുമ്പോൾ (*പഠ)
    {
	എങ്കിൽ (*പഠ == '^')
	{
	    p = പഠ + 1;
	    ആകുമ്പോൾ (vim_isdigit(*p))
		p++;
	    എങ്കിൽ ((*p == ',' || *p == '\0') && p != പഠ + 1)
	    {
		പഠ = p;
		തുടരുക;
	    }
	    അല്ലെങ്കിൽ
		*ലിഖ++ = *പഠ++;
	}
	അല്ലെങ്കിൽ
	    *ലിഖ++ = *പഠ++;
    }
    *ലിഖ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prepare_cpt_compl_funcs(ശൂന്യം)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		idx = 0;
    പൂർണ്ണസംഖ്യ		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    എങ്കിൽ (cpt == ശൂന്യ)
	തിരികെ FAIL;
    strip_caret_numbers_in_place(cpt);

    വേണ്ടി (p = cpt; *p;)
    {
	ആകുമ്പോൾ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	എങ്കിൽ (*p == NUL)
	    നിർത്തുക;

	cb = get_callback_if_cpt_func(p, idx);
	എങ്കിൽ (cb)
	{
	    എങ്കിൽ (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		എങ്കിൽ (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		അല്ലെങ്കിൽ
		    startcol = -2;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	അല്ലെങ്കിൽ
	    cpt_sources_array[idx].cs_startcol = -3;

	(ശൂന്യം)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    തിരികെ OK;
#endif
    തിരികെ FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    സ്ഥിതം ശൂന്യം
compl_source_start_timer(പൂർണ്ണസംഖ്യ source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    എങ്കിൽ (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
advance_cpt_sources_index_safe(ശൂന്യം)
{
    എങ്കിൽ (cpt_sources_index >= 0 && cpt_sources_index < cpt_sources_എണ്ണം - 1)
    {
	cpt_sources_index++;
	തിരികെ OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_index_out_of_range_nr), cpt_sources_index);
#endif
    തിരികെ FAIL;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_get_exp(pos_T *ini)
{
    സ്ഥിതം ins_compl_next_state_T   st;
    സ്ഥിതം പൂർണ്ണസംഖ്യ			    st_cleared = FALSE;
    പൂർണ്ണസംഖ്യ		match_എണ്ണം;
    പൂർണ്ണസംഖ്യ		found_new_match;
    പൂർണ്ണസംഖ്യ		type = ctrl_x_mode;
    പൂർണ്ണസംഖ്യ		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    പൂർണ്ണസംഖ്യ		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    എങ്കിൽ (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	എങ്കിൽ (!st_cleared)
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
	st.e_cpt = st.e_cpt_copy == ശൂന്യ ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_വരി_എണ്ണം	1000
	എങ്കിൽ (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_വരി_എണ്ണം);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_വരി_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != ശൂന്യ;
    എങ്കിൽ (normal_mode_strict)
    {
	cpt_sources_index = 0;
	എങ്കിൽ (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    വേണ്ടി (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	എങ്കിൽ ((ctrl_x_mode_normal() || ctrl_x_mode_വരി_or_eval())
					&& (!compl_started || st.found_all))
	{
	    പൂർണ്ണസംഖ്യ status = process_next_cpt_മൂല്യം(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    എങ്കിൽ (status == INS_COMPL_CPT_END)
		നിർത്തുക;
	    എങ്കിൽ (status == INS_COMPL_CPT_CONT)
	    {
		എങ്കിൽ (may_advance_cpt_idx)
		{
		    എങ്കിൽ (!advance_cpt_sources_index_safe())
			നിർത്തുക;
		    compl_source_start_timer(cpt_sources_index);
		}
		തുടരുക;
	    }
	}

	എങ്കിൽ (normal_mode_strict && type == CTRL_X_FUNCTION
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
	എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	    നിർത്തുക;

	എങ്കിൽ (may_advance_cpt_idx)
	{
	    എങ്കിൽ (!advance_cpt_sources_index_safe())
		നിർത്തുക;
	    compl_source_start_timer(cpt_sources_index);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	എങ്കിൽ ((ctrl_x_mode_not_default() && !ctrl_x_mode_വരി_or_eval())
						|| found_new_match != FAIL)
	{
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	    // Fill the popup menu as soon as possible.
	    എങ്കിൽ (type != -1)
		ins_compl_check_താക്കോലുകൾ(0, FALSE);

	    എങ്കിൽ ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_വരി_or_eval()) || compl_interrupted)
		നിർത്തുക;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    // Mark a buffer scanned when it has been scanned completely
	    എങ്കിൽ (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	എങ്കിൽ (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	എങ്കിൽ (!compl_dir_forward())
	    ആകുമ്പോൾ (compl_curr_match->cp_prev
		    && !match_at_original_text(compl_curr_match->cp_prev))
		compl_curr_match = compl_curr_match->cp_prev;
    }
    cpt_sources_index = -1;
    compl_started = TRUE;

    എങ്കിൽ ((ctrl_x_mode_normal() || ctrl_x_mode_വരി_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_എണ്ണം = -1;		// total of matches, unknown
    എങ്കിൽ (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_വരി_or_eval()))
	match_എണ്ണം = ins_compl_make_cyclic();

    എങ്കിൽ (cot_fuzzy() && compl_get_longest && compl_അക്കം_bests > 0)
	fuzzy_longest_match();

    എങ്കിൽ (compl_old_match != ശൂന്യ)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_next
						    : compl_old_match->cp_prev;
	എങ്കിൽ (compl_curr_match == ശൂന്യ)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    എങ്കിൽ (match_എണ്ണം > 0 && !ctrl_x_mode_spell())
    {
	എങ്കിൽ (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	എങ്കിൽ (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    തിരികെ match_എണ്ണം;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_update_shown_match(ശൂന്യം)
{
    string_T	*leader;

    (ശൂന്യം)get_leader_for_startcol(ശൂന്യ, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    ആകുമ്പോൾ (!ins_compl_equal(compl_shown_match,
		leader->string, (പൂർണ്ണസംഖ്യ)leader->length)
	    && compl_shown_match->cp_next != ശൂന്യ
	    && !is_first_match(compl_shown_match->cp_next))
    {
	compl_shown_match = compl_shown_match->cp_next;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    എങ്കിൽ (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (പൂർണ്ണസംഖ്യ)leader->length)
	    && (compl_shown_match->cp_next == ശൂന്യ
		|| is_first_match(compl_shown_match->cp_next)))
    {
	ആകുമ്പോൾ (!ins_compl_equal(compl_shown_match,
		    leader->string, (പൂർണ്ണസംഖ്യ)leader->length)
		&& compl_shown_match->cp_prev != ശൂന്യ
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
    ശൂന്യം
ins_compl_delete(ശൂന്യം)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    പൂർണ്ണസംഖ്യ col = compl_col + (compl_status_adding() ? compl_length : 0);
    string_T	remaining = {ശൂന്യ, 0};
    പൂർണ്ണസംഖ്യ	    orig_col;

    എങ്കിൽ (ins_compl_preinsert_effect())
    {
	col += (പൂർണ്ണസംഖ്യ)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    എങ്കിൽ (curwin->w_cursor.lnum > compl_lnum)
    {
	എങ്കിൽ (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *വരി = ml_get_cursor();
	    remaining.length = ml_get_cursor_len();
	    remaining.string = vim_strnsave(വരി, remaining.length);
	    എങ്കിൽ (remaining.string == ശൂന്യ)
		തിരികെ;
	}
	ആകുമ്പോൾ (curwin->w_cursor.lnum > compl_lnum)
	{
	    എങ്കിൽ (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		എങ്കിൽ (remaining.string)
		    vim_free(remaining.string);
		തിരികെ;
	    }
	    deleted_വരികൾ_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)curwin->w_cursor.col > col)
    {
	എങ്കിൽ (stop_arrow() == FAIL)
	{
	    എങ്കിൽ (remaining.string)
		vim_free(remaining.string);
	    തിരികെ;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    എങ്കിൽ (remaining.string != ശൂന്യ)
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
    സ്ഥിതം ശൂന്യം
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    പൂർണ്ണസംഖ്യ		base_indent = get_indent();

    ആകുമ്പോൾ (*curr != NUL)
    {
	എങ്കിൽ (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    എങ്കിൽ (curr > start)
		ins_char_bytes(start, (പൂർണ്ണസംഖ്യ)(curr - start));

	    // Handle newline
	    open_വരി(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, ശൂന്യ);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    എങ്കിൽ (curr > start)
	ins_char_bytes(start, (പൂർണ്ണസംഖ്യ)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    സ്ഥിതം char_u *
find_common_prefix(size_t *prefix_len, പൂർണ്ണസംഖ്യ curbuf_only)
{
    compl_T     *compl;
    പൂർണ്ണസംഖ്യ		*match_എണ്ണം;
    char_u	*first = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len = -1;
    പൂർണ്ണസംഖ്യ		is_cpt_completion = (cpt_sources_array != ശൂന്യ);

    എങ്കിൽ (!is_cpt_completion)
	തിരികെ ശൂന്യ;

    match_എണ്ണം = ALLOC_CLEAR_MULT(പൂർണ്ണസംഖ്യ, cpt_sources_എണ്ണം);
    എങ്കിൽ (match_എണ്ണം == ശൂന്യ)
	തിരികെ ശൂന്യ;

    (ശൂന്യം)get_leader_for_startcol(ശൂന്യ, TRUE); // Clear the cache

    compl = compl_first_match;
    ചെയ്യുക
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	എങ്കിൽ (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	എങ്കിൽ (!match_at_original_text(compl)
		&& (leader->string == ശൂന്യ
		    || ins_compl_equal_sc(compl, leader->string,
			(പൂർണ്ണസംഖ്യ)leader->length)))
	{
	    // Limit number of items from each source if max_items is set.
	    പൂർണ്ണസംഖ്യ match_limit_exceeded = FALSE;
	    പൂർണ്ണസംഖ്യ cur_source = compl->cp_cpt_source_idx;

	    എങ്കിൽ (cur_source != -1)
	    {
		match_എണ്ണം[cur_source]++;
		പൂർണ്ണസംഖ്യ max_matches = cpt_sources_array[cur_source].cs_max_matches;
		എങ്കിൽ (max_matches > 0 && match_എണ്ണം[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    എങ്കിൽ (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		എങ്കിൽ (first == ശൂന്യ && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (പൂർണ്ണസംഖ്യ)STRLEN(first);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (first != ശൂന്യ)
		{
		    പൂർണ്ണസംഖ്യ j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    ആകുമ്പോൾ (j < len && *s1 != NUL && *s2 != NUL)
		    {
			എങ്കിൽ (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    നിർത്തുക;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    എങ്കിൽ (len == 0)
			നിർത്തുക;
		}
	    }
	}
	compl = compl->cp_next;
    } ആകുമ്പോൾ (compl != ശൂന്യ && !is_first_match(compl));

    vim_free(match_എണ്ണം);

    എങ്കിൽ (len > (പൂർണ്ണസംഖ്യ)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	എങ്കിൽ (len == (പൂർണ്ണസംഖ്യ)STRLEN(first))
	{
	    char_u *വരി = ml_get_curline();
	    char_u *p = വരി + curwin->w_cursor.col;
	    എങ്കിൽ (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		പൂർണ്ണസംഖ്യ text_len = end - p;
		എങ്കിൽ (text_len > 0
			&& text_len < (len - (പൂർണ്ണസംഖ്യ)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	തിരികെ first;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    ശൂന്യം
ins_compl_തിരുകുക(പൂർണ്ണസംഖ്യ move_cursor, പൂർണ്ണസംഖ്യ തിരുകുക_prefix)
{
    പൂർണ്ണസംഖ്യ		compl_len = get_compl_len();
    പൂർണ്ണസംഖ്യ		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.length;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    എങ്കിൽ (തിരുകുക_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	എങ്കിൽ (cp_str == ശൂന്യ)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    എങ്കിൽ (cp_str == ശൂന്യ)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.length;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    അല്ലെങ്കിൽ എങ്കിൽ (cpt_sources_array != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	എങ്കിൽ (cpt_idx >= 0 && compl_col >= 0)
	{
	    പൂർണ്ണസംഖ്യ	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    എങ്കിൽ (startcol >= 0 && startcol < (പൂർണ്ണസംഖ്യ)compl_col)
	    {
		പൂർണ്ണസംഖ്യ skip = (പൂർണ്ണസംഖ്യ)compl_col - startcol;
		എങ്കിൽ ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    എങ്കിൽ (compl_len < (പൂർണ്ണസംഖ്യ)cp_str_len)
    {
	എങ്കിൽ (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	അല്ലെങ്കിൽ
	{
	    ins_compl_തിരുകുക_bytes(cp_str + compl_len,
		    തിരുകുക_prefix ? (പൂർണ്ണസംഖ്യ)cp_str_len - compl_len : -1);
	    എങ്കിൽ ((preinsert || തിരുകുക_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    എങ്കിൽ (match_at_original_text(compl_shown_match)
	    || (preinsert && !തിരുകുക_prefix))
	compl_used_match = FALSE;
    അല്ലെങ്കിൽ
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = തിരുകുക_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_show_filename(ശൂന്യം)
{
    അക്ഷരം	*lead = _("match in file");
    പൂർണ്ണസംഖ്യ		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    എങ്കിൽ (space <= 0)
	തിരികെ;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    വേണ്ടി (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	ആകുമ്പോൾ (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    എങ്കിൽ (!compl_autocomplete)
    {
	സന്ദേശം_hist_off = TRUE;
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	സന്ദേശം((അക്ഷരം *)IObuff);
	സന്ദേശം_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    സ്ഥിതം compl_T *
find_next_match_in_menu(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    ചെയ്യുക
	match = is_forward ? match->cp_next : match->cp_prev;
    ആകുമ്പോൾ (match->cp_next && !match->cp_in_match_array
	    && !match_at_original_text(match));
    തിരികെ match;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_next_completion_match(
	പൂർണ്ണസംഖ്യ	allow_get_expansion,
	പൂർണ്ണസംഖ്യ	todo,		// repeat completion this many times
	പൂർണ്ണസംഖ്യ	advance,
	പൂർണ്ണസംഖ്യ	*അക്കം_matches)
{
    പൂർണ്ണസംഖ്യ		found_end = FALSE;
    compl_T	*found_compl = ശൂന്യ;
    unsigned പൂർണ്ണസംഖ്യ cur_cot_flags = get_cot_flags();
    പൂർണ്ണസംഖ്യ		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    ആകുമ്പോൾ (--todo >= 0)
    {
	എങ്കിൽ (compl_shows_dir_forward() && compl_shown_match->cp_next != ശൂന്യ)
	{
	    എങ്കിൽ (compl_match_array != ശൂന്യ)
		compl_shown_match = find_next_match_in_menu();
	    അല്ലെങ്കിൽ
		compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != ശൂന്യ
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != ശൂന്യ)
	{
	    found_end = is_first_match(compl_shown_match);
	    എങ്കിൽ (compl_match_array != ശൂന്യ)
		compl_shown_match = find_next_match_in_menu();
	    അല്ലെങ്കിൽ
		compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!allow_get_expansion)
	    {
		എങ്കിൽ (advance)
		{
		    എങ്കിൽ (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    അല്ലെങ്കിൽ
			compl_pending += todo + 1;
		}
		തിരികെ -1;
	    }

	    എങ്കിൽ (!compl_no_select && advance)
	    {
		എങ്കിൽ (compl_shows_dir_backward())
		    --compl_pending;
		അല്ലെങ്കിൽ
		    ++compl_pending;
	    }

	    // Find matches.
	    *അക്കം_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    ആകുമ്പോൾ (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		എങ്കിൽ (compl_pending > 0 && compl_shown_match->cp_next != ശൂന്യ)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (compl_pending < 0 && compl_shown_match->cp_prev != ശൂന്യ)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		അല്ലെങ്കിൽ
		    നിർത്തുക;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	എങ്കിൽ (!match_at_original_text(compl_shown_match)
		&& leader->string != ശൂന്യ
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (പൂർണ്ണസംഖ്യ)leader->length)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	അല്ലെങ്കിൽ
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	എങ്കിൽ (found_end)
	{
	    എങ്കിൽ (found_compl != ശൂന്യ)
	    {
		compl_shown_match = found_compl;
		നിർത്തുക;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_next(
    പൂർണ്ണസംഖ്യ	    allow_get_expansion,
    പൂർണ്ണസംഖ്യ	    എണ്ണം,		// repeat completion this many times; should
				// be at least 1
    പൂർണ്ണസംഖ്യ	    തിരുകുക_match)	// Insert the newly selected match
{
    പൂർണ്ണസംഖ്യ	    അക്കം_matches = -1;
    പൂർണ്ണസംഖ്യ	    todo = എണ്ണം;
    പൂർണ്ണസംഖ്യ	    advance;
    പൂർണ്ണസംഖ്യ	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned പൂർണ്ണസംഖ്യ cur_cot_flags = get_cot_flags();
    പൂർണ്ണസംഖ്യ	    compl_no_തിരുകുക = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    പൂർണ്ണസംഖ്യ	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    എങ്കിൽ (compl_shown_match == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (compl_leader.string != ശൂന്യ
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    എങ്കിൽ (allow_get_expansion && തിരുകുക_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = എണ്ണം != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    എങ്കിൽ (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    എങ്കിൽ (find_next_completion_match(allow_get_expansion, todo, advance,
							&അക്കം_matches) == -1)
	തിരികെ -1;

    എങ്കിൽ (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	തിരികെ -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    എങ്കിൽ (!started && ins_compl_preinsert_longest())
	ins_compl_തിരുകുക(TRUE, TRUE);
    അല്ലെങ്കിൽ എങ്കിൽ (compl_no_തിരുകുക && !started && !compl_preinsert)
    {
	ins_compl_തിരുകുക_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (തിരുകുക_match)
    {
	എങ്കിൽ (!compl_get_longest || compl_used_match)
	{
	    പൂർണ്ണസംഖ്യ preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_തിരുകുക(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	അല്ലെങ്കിൽ
	    ins_compl_തിരുകുക_bytes(compl_leader.string + get_compl_len(), -1);
    }
    അല്ലെങ്കിൽ
	compl_used_match = FALSE;

    എങ്കിൽ (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	എങ്കിൽ (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	അല്ലെങ്കിൽ
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
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
    എങ്കിൽ (compl_no_തിരുകുക && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    അല്ലെങ്കിൽ
	compl_enter_selects = !തിരുകുക_match && compl_match_array != ശൂന്യ;

    // Show the file name for the match (if any)
    എങ്കിൽ (compl_shown_match->cp_fname != ശൂന്യ)
	ins_compl_show_filename();

    തിരികെ അക്കം_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    സ്ഥിതം ശൂന്യം
check_elapsed_time(ശൂന്യം)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_index].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    എങ്കിൽ (elapsed_ms > compl_timeout_ms)
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
    ശൂന്യം
ins_compl_check_താക്കോലുകൾ(പൂർണ്ണസംഖ്യ frequency, പൂർണ്ണസംഖ്യ in_compl_func)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    എങ്കിൽ (!in_compl_func && (using_script() || ex_normal_busy))
	തിരികെ;

    // Only do this at regular intervals
    എങ്കിൽ (++എണ്ണം < frequency)
	തിരികെ;
    എണ്ണം = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    എങ്കിൽ (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	എങ്കിൽ (vim_is_ctrl_x_താക്കോൽ(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (ശൂന്യം)ins_compl_next(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	അല്ലെങ്കിൽ
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    എങ്കിൽ (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		എങ്കിൽ (c != Ctrl_R && (താക്കോൽ_Typed
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_വരി_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != ശൂന്യ && cpt_sources_index >= 0;
	എങ്കിൽ (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    എങ്കിൽ (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	പൂർണ്ണസംഖ്യ todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(ശൂന്യം)ins_compl_next(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_key2dir(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	തിരികെ BACKWARD;
    തിരികെ FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_pum_താക്കോൽ(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_key2count(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (ins_compl_pum_താക്കോൽ(c) && c != K_UP && c != K_DOWN)
    {
	പൂർണ്ണസംഖ്യ h = pum_get_height();
	എങ്കിൽ (h > 3)
	    h -= 2; // keep some context
	തിരികെ h;
    }
    തിരികെ 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_use_match(പൂർണ്ണസംഖ്യ c)
{
    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ K_UP:
	അവസ്ഥ K_DOWN:
	അവസ്ഥ K_PAGEDOWN:
	അവസ്ഥ K_KPAGEDOWN:
	അവസ്ഥ K_S_DOWN:
	അവസ്ഥ K_PAGEUP:
	അവസ്ഥ K_KPAGEUP:
	അവസ്ഥ K_S_UP:
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_normal_compl_info(char_u *വരി, പൂർണ്ണസംഖ്യ startcol, colnr_T curs_col)
{
    എങ്കിൽ ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	എങ്കിൽ (!compl_status_adding())
	{
	    ആകുമ്പോൾ (--startcol >= 0 && vim_isIDc(വരി[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	എങ്കിൽ (p_ic)
	{
	    compl_pattern.string = str_foldcase(വരി + compl_col,
		    compl_length, ശൂന്യ, 0);
	    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	    {
		compl_pattern.length = 0;
		തിരികെ FAIL;
	    }
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	അല്ലെങ്കിൽ
	{
	    compl_pattern.string = vim_strnsave(വരി + compl_col, (size_t)compl_length);
	    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	    {
		compl_pattern.length = 0;
		തിരികെ FAIL;
	    }
	    compl_pattern.length = (size_t)compl_length;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	എങ്കിൽ (!vim_iswordp(വരി + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(വരി, വരി + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ശൂന്യ, വരി + compl_col, compl_length) + prefixlen;
	compl_pattern.string = alloc(n);
	എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	{
	    compl_pattern.length = 0;
	    തിരികെ FAIL;
	}
	STRCPY((അക്ഷരം *)compl_pattern.string, prefix);
	(ശൂന്യം)quote_meta(compl_pattern.string + prefixlen,
		വരി + compl_col, compl_length);
	compl_pattern.length = n - 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (--startcol < 0
	    || !vim_iswordp(mb_prevptr(വരി, വരി + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	{
	    compl_pattern.length = 0;
	    തിരികെ FAIL;
	}
	compl_pattern.length = len;
	compl_col += curs_col;
	compl_length = 0;
	compl_from_nonkeyword = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ base_class;
	    പൂർണ്ണസംഖ്യ head_off;

	    startcol -= (*mb_head_off)(വരി, വരി + startcol);
	    base_class = mb_get_class(വരി + startcol);
	    ആകുമ്പോൾ (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(വരി, വരി + startcol);
		എങ്കിൽ (base_class != mb_get_class(വരി + startcol
			    - head_off))
		    നിർത്തുക;
		startcol -= head_off;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    ആകുമ്പോൾ (--startcol >= 0 && vim_iswordc(വരി[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_length = (പൂർണ്ണസംഖ്യ)curs_col - startcol;
	എങ്കിൽ (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	    {
		compl_pattern.length = 0;
		തിരികെ FAIL;
	    }
	    STRCPY((അക്ഷരം *)compl_pattern.string, "\\<");
	    (ശൂന്യം)quote_meta(compl_pattern.string + 2, വരി + compl_col, 1);
	    STRCAT((അക്ഷരം *)compl_pattern.string, "\\k");
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	അല്ലെങ്കിൽ
	{
	    size_t  n = quote_meta(ശൂന്യ, വരി + compl_col, compl_length) + 2;

	    compl_pattern.string = alloc(n);
	    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	    {
		compl_pattern.length = 0;
		തിരികെ FAIL;
	    }
	    STRCPY((അക്ഷരം *)compl_pattern.string, "\\<");
	    (ശൂന്യം)quote_meta(compl_pattern.string + 2, വരി + compl_col,
		    compl_length);
	    compl_pattern.length = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    എങ്കിൽ (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	എങ്കിൽ (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_wholeline_compl_info(char_u *വരി, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(വരി);
    compl_length = (പൂർണ്ണസംഖ്യ)curs_col - (പൂർണ്ണസംഖ്യ)compl_col;
    എങ്കിൽ (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    എങ്കിൽ (p_ic)
    {
	compl_pattern.string = str_foldcase(വരി + compl_col, compl_length,
		ശൂന്യ, 0);
	എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	{
	    compl_pattern.length = 0;
	    തിരികെ FAIL;
	}
	compl_pattern.length = STRLEN(compl_pattern.string);
    }
    അല്ലെങ്കിൽ
    {
	compl_pattern.string = vim_strnsave(വരി + compl_col, (size_t)compl_length);
	എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	{
	    compl_pattern.length = 0;
	    തിരികെ FAIL;
	}
	compl_pattern.length = (size_t)compl_length;
    }

    തിരികെ OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_filename_compl_info(char_u *വരി, പൂർണ്ണസംഖ്യ startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    എങ്കിൽ (startcol > 0)
    {
	char_u	*p = വരി + startcol;

	MB_PTR_BACK(വരി, p);
	ആകുമ്പോൾ (p > വരി && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(വരി, p);
	എങ്കിൽ (p == വരി && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	അല്ലെങ്കിൽ
	    startcol = (പൂർണ്ണസംഖ്യ)(p - വരി) + 1;
    }

    compl_col += startcol;
    compl_length = (പൂർണ്ണസംഖ്യ)curs_col - startcol;
    compl_pattern.string = addstar(വരി + compl_col, compl_length, EXPAND_FILES);
    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
    {
	compl_pattern.length = 0;
	തിരികെ FAIL;
    }

    compl_pattern.length = STRLEN(compl_pattern.string);

    തിരികെ OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_cmdline_compl_info(char_u *വരി, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(വരി, curs_col);
    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
    {
	compl_pattern.length = 0;
	തിരികെ FAIL;
    }
    compl_pattern.length = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (പൂർണ്ണസംഖ്യ)compl_pattern.length, curs_col, FALSE);
    എങ്കിൽ (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    അല്ലെങ്കിൽ
	compl_col = (പൂർണ്ണസംഖ്യ)(compl_xp.xp_pattern - compl_pattern.string);
    compl_length = curs_col - compl_col;

    തിരികെ OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_compl_globals(
    പൂർണ്ണസംഖ്യ		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    പൂർണ്ണസംഖ്യ		is_cpt_compl UNUSED)
{
    എങ്കിൽ (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	എങ്കിൽ (startcol < compl_col)
	    തിരികെ prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	അല്ലെങ്കിൽ
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.length);
	    cpt_compl_pattern.length = compl_orig_text.length;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*വരി = ml_get(curwin->w_cursor.lnum);
	പൂർണ്ണസംഖ്യ	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(വരി + startcol, (size_t)len);
	എങ്കിൽ (compl_pattern.string == ശൂന്യ)
	{
	    compl_pattern.length = 0;
	    തിരികെ FAIL;
	}
	compl_pattern.length = (size_t)len;
	compl_col = startcol;
	compl_length = len;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    പൂർണ്ണസംഖ്യ		*startcol UNUSED)
{
    പൂർണ്ണസംഖ്യ		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    പൂർണ്ണസംഖ്യ		col;
    char_u	*funcname = ശൂന്യ;
    pos_T	pos;
    പൂർണ്ണസംഖ്യ		save_State = State;
    പൂർണ്ണസംഖ്യ		is_cpt_function = (cb != ശൂന്യ);
    പൂർണ്ണസംഖ്യ		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    എങ്കിൽ (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	എങ്കിൽ (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    തിരികെ FAIL;
	}
	cb = get_തിരുകുക_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_സംഖ്യ;
    args[0].vval.v_സംഖ്യ = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    എങ്കിൽ (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    എങ്കിൽ (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    എങ്കിൽ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	തിരികെ FAIL;
    }

    എങ്കിൽ (startcol != ശൂന്യ)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    എങ്കിൽ (col == -2 || aborting())
	തിരികെ FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    എങ്കിൽ (col == -3)
    {
	എങ്കിൽ (is_cpt_function)
	    തിരികെ FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = ശൂന്യ;
	എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU))
	    സന്ദേശം_clr_cmdline();
	തിരികെ FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    തിരികെ ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_spell_compl_info(പൂർണ്ണസംഖ്യ startcol UNUSED, colnr_T curs_col UNUSED)
{
    പൂർണ്ണസംഖ്യ		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*വരി = ശൂന്യ;

    എങ്കിൽ (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    അല്ലെങ്കിൽ
	compl_col = spell_word_start(startcol);

    എങ്കിൽ (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    അല്ലെങ്കിൽ
    {
	spell_expand_check_cap(compl_col);
	compl_length = (പൂർണ്ണസംഖ്യ)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    വരി = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(വരി + compl_col, (size_t)compl_length);
    എങ്കിൽ (compl_pattern.string == ശൂന്യ)
    {
	compl_pattern.length = 0;
	തിരികെ FAIL;
    }

    compl_pattern.length = (size_t)compl_length;
    ret = OK;
#endif

    തിരികെ ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compl_get_info(char_u *വരി, പൂർണ്ണസംഖ്യ startcol, colnr_T curs_col, പൂർണ്ണസംഖ്യ *വരി_invalid)
{
    എങ്കിൽ (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	എങ്കിൽ (get_normal_compl_info(വരി, startcol, curs_col) != OK)
	    തിരികെ FAIL;
	*വരി_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_വരി_or_eval())
    {
	തിരികെ get_wholeline_compl_info(വരി, curs_col);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_files())
    {
	തിരികെ get_filename_compl_info(വരി, startcol, curs_col);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	തിരികെ get_cmdline_compl_info(വരി, curs_col);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	എങ്കിൽ (get_userdefined_compl_info(curs_col, ശൂന്യ, ശൂന്യ) != OK)
	    തിരികെ FAIL;
	*വരി_invalid = TRUE;	// "line" may have become invalid
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_spell())
    {
	എങ്കിൽ (get_spell_compl_info(startcol, curs_col) == FAIL)
	    തിരികെ FAIL;
	*വരി_invalid = TRUE;	// "line" may have become invalid
    }
    അല്ലെങ്കിൽ
    {
	internal_error("ins_complete()");
	തിരികെ FAIL;
    }

    തിരികെ OK;
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
    സ്ഥിതം ശൂന്യം
ins_compl_continue_തിരയൽ(char_u *വരി)
{
    // it is a continued search
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    എങ്കിൽ (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	എങ്കിൽ (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(വരി);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	അല്ലെങ്കിൽ
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    എങ്കിൽ (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    വരി + compl_length
			    + compl_startpos.col) - വരി);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_length = curwin->w_cursor.col - (പൂർണ്ണസംഖ്യ)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	എങ്കിൽ (compl_length > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_length = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_length;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	എങ്കിൽ (compl_length < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ctrl_x_mode_വരി_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    അല്ലെങ്കിൽ
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_compl_start(ശൂന്യം)
{
    char_u	*വരി = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    പൂർണ്ണസംഖ്യ		വരി_invalid = FALSE;
    പൂർണ്ണസംഖ്യ		save_did_ai = did_ai;
    പൂർണ്ണസംഖ്യ		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    എങ്കിൽ (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	തിരികെ FAIL;
    }

    വരി = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    എങ്കിൽ ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_തിരയൽ(വരി);
    അല്ലെങ്കിൽ
	compl_cont_status &= CONT_LOCAL;

    എങ്കിൽ (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	എങ്കിൽ (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (പൂർണ്ണസംഖ്യ)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    എങ്കിൽ (compl_get_info(വരി, startcol, curs_col, &വരി_invalid) == FAIL)
    {
	എങ്കിൽ (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	തിരികെ FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    എങ്കിൽ (വരി_invalid)
	വരി = ml_get(curwin->w_cursor.lnum);

    എങ്കിൽ (compl_status_adding())
    {
	എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	എങ്കിൽ (ctrl_x_mode_വരി_or_eval())
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
    അല്ലെങ്കിൽ
    {
	edit_submode_pre = ശൂന്യ;
	compl_startpos.col = compl_col;
    }

    എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	എങ്കിൽ (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_സന്ദേശം]);
	അല്ലെങ്കിൽ
	    edit_submode = (char_u *)_(CTRL_X_സന്ദേശം(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ശൂന്യ);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.length = (size_t)compl_length;
    compl_orig_text.string = vim_strnsave(വരി + compl_col, (size_t)compl_length);
    എങ്കിൽ (p_ic)
	flags |= CP_ICASE;
    എങ്കിൽ (compl_orig_text.string == ശൂന്യ
	    || ins_compl_add(compl_orig_text.string,
		(പൂർണ്ണസംഖ്യ)compl_orig_text.length,
		ശൂന്യ, ശൂന്യ, ശൂന്യ, 0, flags, FALSE, ശൂന്യ, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	തിരികെ FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_എണ്ണം;
	showmode();
	edit_submode_extra = ശൂന്യ;
	out_flush();
    }

    did_ai = save_did_ai;
    തിരികെ OK;
}

/*
 * display the completion status message
 */
    സ്ഥിതം ശൂന്യം
ins_compl_show_statusmsg(ശൂന്യം)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    എങ്കിൽ (is_first_match(compl_first_match->cp_next))
    {
	edit_submode_extra = compl_status_adding() && compl_length > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    എങ്കിൽ (edit_submode_extra == ശൂന്യ)
    {
	എങ്കിൽ (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_എണ്ണം;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (compl_curr_match->cp_next == compl_curr_match->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_എണ്ണം;
	    compl_curr_match->cp_സംഖ്യ = 1;
	}
	അല്ലെങ്കിൽ
	{
#എങ്കിൽ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    എങ്കിൽ (compl_curr_match->cp_സംഖ്യ == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    എങ്കിൽ (compl_curr_match->cp_സംഖ്യ != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		സ്ഥിതം char_u match_ref[81];

		എങ്കിൽ (compl_matches > 0)
		    vim_snprintf((അക്ഷരം *)match_ref, വലുപ്പം(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_സംഖ്യ, compl_matches);
		അല്ലെങ്കിൽ
		    vim_snprintf((അക്ഷരം *)match_ref, വലുപ്പം(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_സംഖ്യ);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		എങ്കിൽ (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    എങ്കിൽ (!compl_opt_suppress_empty)
    {
	showmode();
	എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU))
	{
	    എങ്കിൽ (edit_submode_extra != ശൂന്യ)
	    {
		എങ്കിൽ (!p_smd)
		{
		    സന്ദേശം_hist_off = TRUE;
		    സന്ദേശം_attr((അക്ഷരം *)edit_submode_extra,
			    edit_submode_highl < HLF_എണ്ണം
			    ? HL_ATTR(edit_submode_highl) : 0);
		    സന്ദേശം_hist_off = FALSE;
		}
	    }
	    അല്ലെങ്കിൽ
		സന്ദേശം_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    പൂർണ്ണസംഖ്യ
ins_complete(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ enable_pum)
{
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		save_w_wrow;
    പൂർണ്ണസംഖ്യ		save_w_leftcol;
    പൂർണ്ണസംഖ്യ		തിരുകുക_match;
    പൂർണ്ണസംഖ്യ		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    തിരുകുക_match = ins_compl_use_match(c);

    എങ്കിൽ (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	എങ്കിൽ (ins_compl_start() == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (തിരുകുക_match && stop_arrow() == FAIL)
	തിരികെ FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_buffer;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_അക്കം_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), തിരുകുക_match);

    // Reset autocompletion timer expiry flag
    എങ്കിൽ (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    എങ്കിൽ (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    എങ്കിൽ (got_int && !global_busy)
    {
	(ശൂന്യം)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_next);
    എങ്കിൽ (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	എങ്കിൽ (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    എങ്കിൽ (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    അല്ലെങ്കിൽ
	compl_cont_status &= ~CONT_S_IPOS;

    എങ്കിൽ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    എങ്കിൽ (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    തിരികെ OK;
}

/*
 * Enable autocompletion
 */
    ശൂന്യം
ins_compl_enable_autocomplete(ശൂന്യം)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
}

/*
 * Disable autocompletion
 */
    ശൂന്യം
ins_compl_disable_autocomplete(ശൂന്യം)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    ശൂന്യം
ins_compl_arm_autostart(ശൂന്യം)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    ശൂന്യം
ins_compl_disarm_autostart(ശൂന്യം)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(ശൂന്യം)
{
#ifdef ELAPSED_FUNC
    എങ്കിൽ (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	തിരികെ true;
    }
#endif
    തിരികെ false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    ശൂന്യം
ins_compl_clear_autocomplete_delay(ശൂന്യം)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(ശൂന്യം)
{
    തിരികെ compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(ശൂന്യം)
{
#ifdef ELAPSED_FUNC
    തിരികെ ELAPSED_FUNC(compl_autocomplete_start_tv);
#അല്ലെങ്കിൽ
    തിരികെ 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    സ്ഥിതം ശൂന്യം
show_pum(പൂർണ്ണസംഖ്യ prev_w_wrow, പൂർണ്ണസംഖ്യ prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    എങ്കിൽ (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    സ്ഥിതം unsigned
quote_meta(char_u *dest, char_u *src, പൂർണ്ണസംഖ്യ len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    വേണ്ടി ( ; --len >= 0; src++)
    {
	തിരഞ്ഞെടുപ്പ് (*src)
	{
	    അവസ്ഥ '.':
	    അവസ്ഥ '*':
	    അവസ്ഥ '[':
		എങ്കിൽ (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    നിർത്തുക;
		// FALLTHROUGH
	    അവസ്ഥ '~':
		എങ്കിൽ (!magic_isset())	// quote these only if magic is set
		    നിർത്തുക;
		// FALLTHROUGH
	    അവസ്ഥ '\\':
		എങ്കിൽ (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    നിർത്തുക;
		// FALLTHROUGH
	    അവസ്ഥ '^':		// currently it's not needed.
	    അവസ്ഥ '$':
		m++;
		എങ്കിൽ (dest != ശൂന്യ)
		    *dest++ = '\\';
		നിർത്തുക;
	}
	എങ്കിൽ (dest != ശൂന്യ)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ mb_len = (*mb_ptr2len)(src) - 1;
	    എങ്കിൽ (mb_len > 0 && len >= mb_len)
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    എങ്കിൽ (dest != ശൂന്യ)
			*dest++ = *src;
		}
	}
    }
    എങ്കിൽ (dest != ശൂന്യ)
	*dest = NUL;

    തിരികെ m;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_insexpand_stuff(ശൂന്യം)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_എണ്ണം);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    സ്ഥിതം ശൂന്യം
spell_back_to_badword(ശൂന്യം)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, ശൂന്യ);
    എങ്കിൽ (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    സ്ഥിതം ശൂന്യം
cpt_sources_clear(ശൂന്യം)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_index = -1;
    cpt_sources_എണ്ണം = 0;
}

/*
 * Setup completion sources.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
setup_cpt_sources(ശൂന്യം)
{
    char_u  buf[LSIZE];
    പൂർണ്ണസംഖ്യ	    slen;
    പൂർണ്ണസംഖ്യ	    idx = 0;
    പൂർണ്ണസംഖ്യ	    എണ്ണം;
    char_u  *p;

    cpt_sources_clear();

    എണ്ണം = get_cpt_sources_എണ്ണം(curbuf->b_p_cpt);
    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, എണ്ണം);
    എങ്കിൽ (cpt_sources_array == ശൂന്യ)
	തിരികെ FAIL;

    വേണ്ടി (p = curbuf->b_p_cpt; *p;)
    {
	ആകുമ്പോൾ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	എങ്കിൽ (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    എങ്കിൽ (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		എങ്കിൽ (caret != ശൂന്യ)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((അക്ഷരം *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_എണ്ണം = എണ്ണം;
    തിരികെ OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_cpt_func_refresh_always(ശൂന്യം)
{
#ifdef FEAT_COMPL_FUNC
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cpt_sources_എണ്ണം; i++)
	എങ്കിൽ (cpt_sources_array[i].cs_refresh_always)
	    തിരികെ TRUE;
#endif
    തിരികെ FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    സ്ഥിതം ശൂന്യം
ins_compl_make_linear(ശൂന്യം)
{
    compl_T *m;

    എങ്കിൽ (compl_first_match == ശൂന്യ || compl_first_match->cp_prev == ശൂന്യ)
	തിരികെ;
    m = compl_first_match->cp_prev;
    m->cp_next = ശൂന്യ;
    compl_first_match->cp_prev = ശൂന്യ;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    സ്ഥിതം ശൂന്യം
remove_old_matches(ശൂന്യം)
{
    compl_T *current;
    പൂർണ്ണസംഖ്യ	    shown_match_removed = FALSE;
    പൂർണ്ണസംഖ്യ	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    എങ്കിൽ (cpt_sources_index < 0)
	തിരികെ;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    വേണ്ടി (current = compl_first_match; current != ശൂന്യ; )
    {
	എങ്കിൽ (current->cp_cpt_source_idx == cpt_sources_index)
	{
	    compl_T *to_delete = current;

	    എങ്കിൽ (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_next;

	    എങ്കിൽ (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_next;
		compl_first_match->cp_prev = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (to_delete->cp_next == ശൂന്യ) // node to remove is at tail
		to_delete->cp_prev->cp_next = ശൂന്യ;
	    അല്ലെങ്കിൽ // node is in the moddle
	    {
		to_delete->cp_prev->cp_next = to_delete->cp_next;
		to_delete->cp_next->cp_prev = to_delete->cp_prev;
	    }
	    ins_compl_item_free(to_delete);
	}
	അല്ലെങ്കിൽ
	    current = current->cp_next;
    }

    // Re-assign compl_shown_match if necessary
    എങ്കിൽ (shown_match_removed)
    {
	എങ്കിൽ (forward)
	    compl_shown_match = compl_first_match;
	അല്ലെങ്കിൽ
	{  // Last node will have the prefix that is being completed
	    വേണ്ടി (current = compl_first_match; current->cp_next != ശൂന്യ;
		    current = current->cp_next)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    വേണ്ടി (current = compl_first_match; current != ശൂന്യ; )
    {
	എങ്കിൽ ((forward ? current->cp_cpt_source_idx < cpt_sources_index
		    : current->cp_cpt_source_idx > cpt_sources_index))
	{
	    compl_curr_match = forward ? current : current->cp_next;
	    current = current->cp_next;
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    സ്ഥിതം ശൂന്യം
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_index];
    പൂർണ്ണസംഖ്യ		    startcol = cpt_src->cs_startcol;

    എങ്കിൽ (startcol == -2 || startcol == -3)
	തിരികെ;

    എങ്കിൽ (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	എങ്കിൽ (!cpt_src->cs_refresh_always)
	    ins_compl_തിരുകുക_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	എങ്കിൽ (!cpt_src->cs_refresh_always)
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
    സ്ഥിതം ശൂന്യം
cpt_compl_refresh(ശൂന്യം)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_index = 0;
    വേണ്ടി (p = cpt; *p;)
    {
	ആകുമ്പോൾ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	എങ്കിൽ (*p == NUL)
	    നിർത്തുക;

	എങ്കിൽ (cpt_sources_array[cpt_sources_index].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_index);
	    എങ്കിൽ (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		എങ്കിൽ (ret == FAIL)
		{
		    എങ്കിൽ (startcol == -3)
			cpt_sources_array[cpt_sources_index].cs_refresh_always
			    = FALSE;
		    അല്ലെങ്കിൽ
			startcol = -2;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_index].cs_startcol = startcol;
		എങ്കിൽ (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_index);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(ശൂന്യം)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	എങ്കിൽ (may_advance_cpt_index(p))
	    (ശൂന്യം)advance_cpt_sources_index_safe();
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
    ശൂന്യം
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    എങ്കിൽ (ins_compl_preinsert_effect())
	rettv->vval.v_സംഖ്യ = 1;
}
