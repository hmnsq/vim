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

#અન્તર્ભાવ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_લીટી	3
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
#define CTRL_X_LOCAL_સંદેશ	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_સંદેશ(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
સ્થિર અક્ષર *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ખાલી, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ખાલી, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ખાલી,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#જો defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
સ્થિર string_T ctrl_x_mode_names[] = {
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
    {ખાલી, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
પ્રકારવ્યાખ્યા સંરચના compl_S compl_T;
સંરચના compl_S
{
    compl_T	*cp_next;
    compl_T	*cp_prev;
    compl_T	*cp_match_next;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_ગણતરી]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_data;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    પૂર્ણાંક		cp_flags;		// CP_ values
    પૂર્ણાંક		cp_સંખ્યા;		// sequence number
    પૂર્ણાંક		cp_order;		// list position when first scored
    પૂર્ણાંક		cp_score;		// fuzzy match score or proximity score
    પૂર્ણાંક		cp_in_match_array;	// collected by compl_match_array
    પૂર્ણાંક		cp_user_abbr_hl_id;	// highlight group ID for abbr
    પૂર્ણાંક		cp_user_kind_hl_id;	// highlight group ID for kind
    પૂર્ણાંક		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
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
સ્થિર compl_T    *compl_first_match = ખાલી;
સ્થિર compl_T    *compl_curr_match = ખાલી;
સ્થિર compl_T    *compl_shown_match = ખાલી;
સ્થિર compl_T    *compl_old_match = ખાલી;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક	    cse_ગણતરી;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((પૂર્ણાંક)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_ચાવી - CSE_OFF))

સ્થિર hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    સ્થિર શૂન્ય
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    જો (compl_strings_ht.ht_array == ખાલી)
	hash_init(&compl_strings_ht);
    જો (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    જો (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	જો (entry == ખાલી)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    પાછા;
	entry->cse_ગણતરી = 1;
	vim_strncpy(entry->cse_str, str, len);
	જો (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    નહીંતર
	++HI2CSE(hi)->cse_ગણતરી;
}

// list used to store the compl_T which have the max score
સ્થિર compl_T	  **compl_best_matches = ખાલી;
સ્થિર પૂર્ણાંક	  compl_અંક_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
સ્થિર પૂર્ણાંક	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
સ્થિર string_T	  compl_leader = {ખાલી, 0};

સ્થિર પૂર્ણાંક	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
સ્થિર પૂર્ણાંક	  compl_used_match;

// didn't finish finding completions.
સ્થિર પૂર્ણાંક	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
સ્થિર પૂર્ણાંક	  compl_interrupted = FALSE;

સ્થિર પૂર્ણાંક	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
સ્થિર પૂર્ણાંક	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
સ્થિર પૂર્ણાંક	  ctrl_x_mode = CTRL_X_NORMAL;

સ્થિર પૂર્ણાંક	  compl_matches = 0;	    // number of completion matches
સ્થિર string_T	  compl_pattern = {ખાલી, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
સ્થિર string_T	  cpt_compl_pattern = {ખાલી, 0}; // pattern returned by func in 'cpt'
#endif
સ્થિર પૂર્ણાંક	  compl_direction = FORWARD;
સ્થિર પૂર્ણાંક	  compl_shows_dir = FORWARD;
સ્થિર પૂર્ણાંક	  compl_pending = 0;	    // > 1 for postponed CTRL-N
સ્થિર pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
સ્થિર પૂર્ણાંક	  compl_length = 0;
સ્થિર linenr_T	  compl_lnum = 0;           // lnum where the completion start
સ્થિર colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
સ્થિર colnr_T	  compl_ins_end_col = 0;
સ્થિર colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
સ્થિર string_T	  compl_orig_text = {ખાલી, 0};  // text as it was before
					    // completion started
સ્થિર પૂર્ણાંક	  compl_cont_mode = 0;
સ્થિર expand_T	  compl_xp;

સ્થિર win_T	  *compl_curr_win = ખાલી;  // win where completion is active
સ્થિર buf_T	  *compl_curr_buf = ખાલી;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
સ્થિર પૂર્ણાંક	  compl_autocomplete = FALSE;	    // whether autocompletion is active
સ્થિર bool	  compl_autostarted = false;	    // Vim started this completion
સ્થિર bool	  compl_autostart_pending = false;  // the trigger armed one
સ્થિર bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
સ્થિર elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
સ્થિર પૂર્ણાંક	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
સ્થિર પૂર્ણાંક	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
સ્થિર પૂર્ણાંક	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
સ્થિર પૂર્ણાંક	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    કરો { \
	જો (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } જ્યારે (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
સ્થિર પૂર્ણાંક	  compl_cont_status = 0;
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

સ્થિર પૂર્ણાંક	  compl_opt_refresh_always = FALSE;
સ્થિર પૂર્ણાંક	  compl_opt_suppress_empty = FALSE;

સ્થિર પૂર્ણાંક	  compl_selected_item = -1;

સ્થિર પૂર્ણાંક	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
પ્રકારવ્યાખ્યા સંરચના cpt_source_T
{
    પૂર્ણાંક	cs_refresh_always;  // Whether 'refresh:always' is set for func
    પૂર્ણાંક	cs_startcol;	    // Start column returned by func
    પૂર્ણાંક	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

સ્થિર cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
સ્થિર પૂર્ણાંક	    cpt_sources_ગણતરી;  // Total number of completion sources specified in the 'cpt' option
સ્થિર પૂર્ણાંક	    cpt_sources_index = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
સ્થિર pumitem_T *compl_match_array = ખાલી;
સ્થિર પૂર્ણાંક compl_match_arraysize;

સ્થિર પૂર્ણાંક ins_compl_add(char_u *str, પૂર્ણાંક len, char_u *fname, char_u **cptext, typval_T *user_data, પૂર્ણાંક cdir, પૂર્ણાંક flags, પૂર્ણાંક adup, પૂર્ણાંક *user_hl, પૂર્ણાંક score);
સ્થિર શૂન્ય ins_compl_longest_match(compl_T *match);
સ્થિર શૂન્ય ins_compl_del_pum(શૂન્ય);
સ્થિર શૂન્ય ins_compl_files(પૂર્ણાંક ગણતરી, char_u **files, પૂર્ણાંક thesaurus, પૂર્ણાંક flags, regmatch_T *regmatch, char_u *buf, પૂર્ણાંક *dir);
સ્થિર શૂન્ય ins_compl_free(શૂન્ય);
સ્થિર પૂર્ણાંક  ins_compl_need_restart(શૂન્ય);
સ્થિર શૂન્ય ins_compl_new_leader(શૂન્ય);
સ્થિર પૂર્ણાંક  get_compl_len(શૂન્ય);
સ્થિર શૂન્ય ins_compl_restart(શૂન્ય);
સ્થિર શૂન્ય ins_compl_set_original_text(char_u *str, size_t len);
સ્થિર શૂન્ય ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#જો defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
સ્થિર શૂન્ય ins_compl_add_list(list_T *list);
સ્થિર શૂન્ય ins_compl_add_dict(dict_T *dict);
સ્થિર પૂર્ણાંક get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, પૂર્ણાંક *startcol);
સ્થિર શૂન્ય get_cpt_func_completion_matches(callback_T *cb);
સ્થિર callback_T *get_callback_if_cpt_func(char_u *p, પૂર્ણાંક idx);
સ્થિર શૂન્ય fill_complete_info_dict(dict_T *di, compl_T *match, પૂર્ણાંક add_match);
#endif
સ્થિર પૂર્ણાંક setup_cpt_sources(શૂન્ય);
સ્થિર પૂર્ણાંક is_cpt_func_refresh_always(શૂન્ય);
સ્થિર શૂન્ય cpt_sources_clear(શૂન્ય);
સ્થિર શૂન્ય cpt_compl_refresh(શૂન્ય);
સ્થિર પૂર્ણાંક  ins_compl_key2dir(પૂર્ણાંક c);
સ્થિર પૂર્ણાંક  ins_compl_pum_ચાવી(પૂર્ણાંક c);
સ્થિર પૂર્ણાંક  ins_compl_key2count(પૂર્ણાંક c);
સ્થિર શૂન્ય show_pum(પૂર્ણાંક prev_w_wrow, પૂર્ણાંક prev_w_leftcol);
સ્થિર unsigned  quote_meta(char_u *dest, char_u *str, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક ins_compl_has_multiple(શૂન્ય);
સ્થિર શૂન્ય ins_compl_expand_multiple(char_u *str);
સ્થિર શૂન્ય ins_compl_longest_ઉમેરો(char_u *prefix);
સ્થિર શૂન્ય ins_compl_make_linear(શૂન્ય);
સ્થિર પૂર્ણાંક ins_compl_make_cyclic(શૂન્ય);

#ifdef FEAT_SPELL
સ્થિર શૂન્ય spell_back_to_badword(શૂન્ય);
સ્થિર પૂર્ણાંક  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    શૂન્ય
ins_ctrl_x(શૂન્ય)
{
    જો (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	જો (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	નહીંતર
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_સંદેશ(ctrl_x_mode));
	edit_submode_pre = ખાલી;
	showmode();
    }
    નહીંતર
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
પૂર્ણાંક ctrl_x_mode_none(શૂન્ય)
    { પાછા ctrl_x_mode == 0; }
પૂર્ણાંક ctrl_x_mode_normal(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_NORMAL; }
પૂર્ણાંક ctrl_x_mode_scroll(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_SCROLL; }
પૂર્ણાંક ctrl_x_mode_whole_લીટી(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_WHOLE_લીટી; }
પૂર્ણાંક ctrl_x_mode_files(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_FILES; }
પૂર્ણાંક ctrl_x_mode_tags(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_TAGS; }
પૂર્ણાંક ctrl_x_mode_path_patterns(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
પૂર્ણાંક ctrl_x_mode_path_defines(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_PATH_DEFINES; }
પૂર્ણાંક ctrl_x_mode_dictionary(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_DICTIONARY; }
પૂર્ણાંક ctrl_x_mode_thesaurus(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_THESAURUS; }
પૂર્ણાંક ctrl_x_mode_cmdline(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
પૂર્ણાંક ctrl_x_mode_function(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_FUNCTION; }
પૂર્ણાંક ctrl_x_mode_omni(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_OMNI; }
પૂર્ણાંક ctrl_x_mode_spell(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_SPELL; }
પૂર્ણાંક ctrl_x_mode_eval(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_EVAL; }
પૂર્ણાંક ctrl_x_mode_લીટી_or_eval(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_WHOLE_લીટી || ctrl_x_mode == CTRL_X_EVAL; }
પૂર્ણાંક ctrl_x_mode_register(શૂન્ય)
    { પાછા ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    પૂર્ણાંક
ctrl_x_mode_not_default(શૂન્ય)
{
    પાછા ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    પૂર્ણાંક
ctrl_x_mode_not_defined_yet(શૂન્ય)
{
    પાછા ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    પૂર્ણાંક
compl_status_adding(શૂન્ય)
{
    પાછા compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    પૂર્ણાંક
compl_status_sol(શૂન્ય)
{
    પાછા compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    પૂર્ણાંક
compl_status_local(શૂન્ય)
{
    પાછા compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    શૂન્ય
compl_status_clear(શૂન્ય)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    સ્થિર પૂર્ણાંક
compl_dir_forward(શૂન્ય)
{
    પાછા compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    સ્થિર પૂર્ણાંક
compl_shows_dir_forward(શૂન્ય)
{
    પાછા compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    સ્થિર પૂર્ણાંક
compl_shows_dir_backward(શૂન્ય)
{
    પાછા compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    પૂર્ણાંક
has_compl_option(પૂર્ણાંક dict_opt)
{
    જો (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
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
	edit_submode = ખાલી;
	સંદેશ_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	જો (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    જો (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    પૂર્ણાંક
vim_is_ctrl_x_ચાવી(પૂર્ણાંક c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    જો (ins_compl_pum_ચાવી(c))
	પાછા TRUE;

    પસંદગી (ctrl_x_mode)
    {
	સ્થિતિ 0:		    // Not in any CTRL-X mode
	    પાછા (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	સ્થિતિ CTRL_X_NOT_DEFINED_YET:
	સ્થિતિ CTRL_X_CMDLINE_CTRL_X:
	    પાછા (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	સ્થિતિ CTRL_X_SCROLL:
	    પાછા (c == Ctrl_Y || c == Ctrl_E);
	સ્થિતિ CTRL_X_WHOLE_લીટી:
	    પાછા (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_FILES:
	    પાછા (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_DICTIONARY:
	    પાછા (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_THESAURUS:
	    પાછા (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_TAGS:
	    પાછા (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	સ્થિતિ CTRL_X_PATH_PATTERNS:
	    પાછા (c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_PATH_DEFINES:
	    પાછા (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	સ્થિતિ CTRL_X_CMDLINE:
	    પાછા (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	સ્થિતિ CTRL_X_FUNCTION:
	    પાછા (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_OMNI:
	    પાછા (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	સ્થિતિ CTRL_X_SPELL:
	    પાછા (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_EVAL:
	    પાછા (c == Ctrl_P || c == Ctrl_N);
	સ્થિતિ CTRL_X_REGISTER:
	    પાછા (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    પાછા FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    સ્થિર પૂર્ણાંક
match_at_original_text(compl_T *match)
{
    પાછા match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    સ્થિર પૂર્ણાંક
is_first_match(compl_T *match)
{
    પાછા match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    સ્થિર compl_T *
find_original_text_match(શૂન્ય)
{
    જો (compl_first_match == ખાલી)
	પાછા ખાલી;
    જો (match_at_original_text(compl_first_match))
	પાછા compl_first_match;
    જો (compl_first_match->cp_prev != ખાલી
	    && match_at_original_text(compl_first_match->cp_prev))
	પાછા compl_first_match->cp_prev;
    પાછા ખાલી;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    પૂર્ણાંક
ins_compl_accept_char(પૂર્ણાંક c)
{
    જો (compl_autocomplete && compl_from_nonkeyword)
	પાછા FALSE;

    જો (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	પાછા vim_isIDc(c);

    પસંદગી (ctrl_x_mode)
    {
	સ્થિતિ CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    પાછા vim_isfilec(c) && !vim_ispathsep(c);

	સ્થિતિ CTRL_X_CMDLINE:
	સ્થિતિ CTRL_X_CMDLINE_CTRL_X:
	સ્થિતિ CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    પાછા vim_isprintc(c) && !VIM_ISWHITE(c);

	સ્થિતિ CTRL_X_WHOLE_લીટી:
	    // For while line completion a space can be part of the line.
	    પાછા vim_isprintc(c);
    }
    પાછા vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    સ્થિર char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    પૂર્ણાંક	    char_len,
    પૂર્ણાંક	    compl_char_len,
    પૂર્ણાંક	    min_len,
    char_u  **tofree)
{
    પૂર્ણાંક		*wca;			// Wide character array.
    char_u	*p;
    પૂર્ણાંક		i, c;
    પૂર્ણાંક		has_lower = FALSE;
    પૂર્ણાંક		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(પૂર્ણાંક, char_len);
    જો (wca == ખાલી)
	પાછા IObuff;

    p = str;
    માટે (i = 0; i < char_len; ++i)
    {
	જો (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	નહીંતર
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    માટે (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	જો (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    જો (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		માટે (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		તોડો;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    જો (!has_lower)
    {
	p = compl_orig_text.string;
	માટે (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    જો (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		માટે (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		તોડો;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    માટે (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	જો (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	નહીંતર જો (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    જ્યારે (i < char_len)
    {
	જો (gap.ga_data != ખાલી)
	{
	    જો (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		પાછા (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    જો (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    નહીંતર
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	નહીંતર જો ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    જો (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		પાછા (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (પૂર્ણાંક)(p - IObuff);
	}
	નહીંતર જો (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	નહીંતર
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    જો (gap.ga_data != ખાલી)
    {
	*tofree = gap.ga_data;
	પાછા gap.ga_data;
    }

    *p = NUL;
    પાછા IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    પૂર્ણાંક
ins_compl_add_infercase(
    char_u	*str_arg,
    પૂર્ણાંક		len,
    પૂર્ણાંક		icase,
    char_u	*fname,
    પૂર્ણાંક		dir,
    પૂર્ણાંક		cont_s_ipos,  // next ^X<> will set initial_pos
    પૂર્ણાંક		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    પૂર્ણાંક		char_len;		// count multi-byte characters
    પૂર્ણાંક		compl_char_len;
    પૂર્ણાંક		min_len;
    પૂર્ણાંક		flags = 0;
    પૂર્ણાંક		res;
    char_u	*tofree = ખાલી;

    જો (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	જો (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    જ્યારે (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	નહીંતર
	{
	    char_len = len;
	}

	// Find actual length of original text.
	જો (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    જ્યારે (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	નહીંતર
	{
	    compl_char_len = compl_length;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    જો (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    જો (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, ખાલી, ખાલી, dir, flags, FALSE, ખાલી, score);
    vim_free(tofree);
    પાછા res;
}

/*
 * Check if fuzzy matching is enabled
 */
    સ્થિર પૂર્ણાંક
cot_fuzzy(શૂન્ય)
{
    પાછા (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    સ્થિર પૂર્ણાંક
is_nearest_active(શૂન્ય)
{
    પાછા (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    પૂર્ણાંક
ins_compl_is_match_selected(શૂન્ય)
{
    પાછા compl_shown_match != ખાલી && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    પૂર્ણાંક
ins_compl_preinsert_longest(શૂન્ય)
{
    પાછા compl_autocomplete
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
    સ્થિર પૂર્ણાંક
ins_compl_add(
    char_u	*str,
    પૂર્ણાંક		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_data UNUSED,  // "user_data" entry or NULL
    પૂર્ણાંક		cdir,
    પૂર્ણાંક		flags_arg,
    પૂર્ણાંક		adup,		    // accept duplicate match
    પૂર્ણાંક		*user_hl,           // user abbr/kind hlattr
    પૂર્ણાંક		score)
{
    compl_T	*match, *current, *prev;
    પૂર્ણાંક		dir = (cdir == 0 ? compl_direction : cdir);
    પૂર્ણાંક		flags = flags_arg;
    પૂર્ણાંક		inserted = FALSE;
    char_u	*new_str = ખાલી;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    જો (flags & CP_FAST)
	fast_breakcheck();
    નહીંતર
	ui_breakcheck();
    જો (got_int)
	પાછા FAIL;
    જો (len < 0)
	len = (પૂર્ણાંક)STRLEN(str);

    // If the same match is already present, don't add it.
    જો (compl_first_match != ખાલી && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *ચાવી;
	hashitem_T  *hi;

	જો (len < (પૂર્ણાંક)કદ(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    ચાવી = keybuf;
	}
	નહીંતર
	{
	    new_str = vim_strnsave(str, len);
	    જો (new_str == ખાલી)
		પાછા FAIL;
	    ચાવી = new_str;
	}
	str_hash = hash_hash(ચાવી);
	hi = hash_lookup(&compl_strings_ht, ચાવી, str_hash);
	જો (!HASHITEM_EMPTY(hi))
	{
	    જો (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		કરો
		{
		    જો (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((પૂર્ણાંક)match->cp_str.length <= len
					  || match->cp_str.string[len] == NUL))
		    {
			જો (score < match->cp_score)
			    match->cp_score = score;
			તોડો;
		    }
		    match = match->cp_next;
		} જ્યારે (match != ખાલી && !is_first_match(match));
	    }
	    vim_free(new_str);
	    પાછા NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    જો (match == ખાલી)
    {
	vim_free(new_str);
	પાછા FAIL;
    }
    match->cp_સંખ્યા = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    જો (new_str == ખાલી && (new_str = vim_strnsave(str, len)) == ખાલી)
    {
	vim_free(match);
	પાછા FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.length = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    જો (fname != ખાલી
	    && compl_curr_match != ખાલી
	    && compl_curr_match->cp_fname != ખાલી
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    નહીંતર જો (fname != ખાલી)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    નહીંતર
	match->cp_fname = ખાલી;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_index;

    જો (cptext != ખાલી)
    {
	માટે (પૂર્ણાંક i = 0; i < CPT_ગણતરી; ++i)
	{
	    જો (cptext[i] != ખાલી && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    જો (user_data != ખાલી)
	match->cp_user_data = *user_data;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    જો (compl_first_match == ખાલી)
	match->cp_next = match->cp_prev = ખાલી;
    નહીંતર જો (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_next;
	prev = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	જ્યારે (current != ખાલી && current != compl_first_match)
	{
	    જો (current->cp_score < score)
	    {
		match->cp_next = current;
		match->cp_prev = current->cp_prev;
		જો (current->cp_prev)
		    current->cp_prev->cp_next = match;
		current->cp_prev = match;
		inserted = TRUE;
		તોડો;
	    }
	    prev = current;
	    current = current->cp_next;
	}
	જો (!inserted)
	{
	    prev->cp_next = match;
	    match->cp_prev = prev;
	    match->cp_next = compl_first_match;
	    compl_first_match->cp_prev = match;
	}
    }
    નહીંતર જો (dir == FORWARD)
    {
	match->cp_next = compl_curr_match->cp_next;
	match->cp_prev = compl_curr_match;
    }
    નહીંતર	// BACKWARD
    {
	match->cp_next = compl_curr_match;
	match->cp_prev = compl_curr_match->cp_prev;
    }
    જો (match->cp_next)
	match->cp_next->cp_prev = match;
    જો (match->cp_prev)
	match->cp_prev->cp_next = match;
    નહીંતર	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    જો (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.length,
								    str_hash);

    // Find the longest common string if still doing that.
    જો (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    પાછા OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    સ્થિર પૂર્ણાંક
ins_compl_equal(compl_T *match, char_u *str, પૂર્ણાંક len)
{
    જો (match->cp_flags & CP_EQUAL)
	પાછા TRUE;
    જો (match->cp_flags & CP_ICASE)
	પાછા STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    પાછા STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    સ્થિર પૂર્ણાંક
ins_compl_equal_sc(compl_T *match, char_u *str, પૂર્ણાંક len)
{
    પૂર્ણાંક	typed = compl_length;
    પૂર્ણાંક	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (પૂર્ણાંક)(compl_longest_end_col - compl_col) : typed;

    જો ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	પાછા ins_compl_equal(match, str, len);

    જો ((પૂર્ણાંક)match->cp_str.length < len)
	પાછા FALSE;

    માટે (પૂર્ણાંક i = 0; i < len; ++i)
    {
	જો (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    સ્થિર શૂન્ય
ins_compl_ઉમેરો_bytes(char_u *p, પૂર્ણાંક len)
{
    જો (len == -1)
	len = (પૂર્ણાંક)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(શૂન્ય)
{
    પાછા compl_leader.string != ખાલી ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    સ્થિર size_t
ins_compl_leader_len(શૂન્ય)
{
    પાછા compl_leader.string != ખાલી ? compl_leader.length : compl_orig_text.length;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    પૂર્ણાંક
ins_compl_col_range_attr(linenr_T lnum, પૂર્ણાંક col)
{
    પૂર્ણાંક	    start_col;
    પૂર્ણાંક	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    પૂર્ણાંક	    attr;

    જો (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	પાછા -1;

    start_col = compl_col + (પૂર્ણાંક)ins_compl_leader_len();
    જો (!ins_compl_has_multiple())
	પાછા (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    જો ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	પાછા attr;

    પાછા -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    સ્થિર પૂર્ણાંક
ins_compl_has_multiple(શૂન્ય)
{
    પાછા vim_strchr(compl_shown_match->cp_str.string, '\n') != ખાલી;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    પૂર્ણાંક
ins_compl_lnum_in_range(linenr_T lnum)
{
    જો (!ins_compl_has_multiple())
	પાછા FALSE;
    પાછા lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    સ્થિર શૂન્ય
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    પૂર્ણાંક		c1, c2;
    પૂર્ણાંક		had_match;

    જો (compl_leader.string == ખાલી)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.length);
	જો (compl_leader.string == ખાલી)
	    પાછા;

	compl_leader.length = match->cp_str.length;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ઉમેરો(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	જો (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	પાછા;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    જ્યારે (*p != NUL)
    {
	જો (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	નહીંતર
	{
	    c1 = *p;
	    c2 = *s;
	}
	જો ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    તોડો;
	જો (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	નહીંતર
	{
	    ++p;
	    ++s;
	}
    }

    જો (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.length = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ઉમેરો(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	જો (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    સ્થિર શૂન્ય
ins_compl_add_matches(
    પૂર્ણાંક		અંક_matches,
    char_u	**matches,
    પૂર્ણાંક		icase)
{
    પૂર્ણાંક		add_r = OK;
    પૂર્ણાંક		dir = compl_direction;

    માટે (પૂર્ણાંક i = 0; i < અંક_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, ખાલી, ખાલી, ખાલી, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ખાલી,
		FUZZY_SCORE_NONE);
	જો (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(અંક_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    સ્થિર પૂર્ણાંક
ins_compl_make_cyclic(શૂન્ય)
{
    compl_T *match;
    પૂર્ણાંક	    ગણતરી = 0;

    જો (compl_first_match == ખાલી)
	પાછા 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    જ્યારે (match->cp_next != ખાલી && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++ગણતરી;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    પાછા ગણતરી;
}

/*
 * Return whether there currently is a shown match.
 */
    પૂર્ણાંક
ins_compl_has_shown_match(શૂન્ય)
{
    પાછા compl_shown_match == ખાલી
	|| compl_shown_match != compl_shown_match->cp_next;
}

/*
 * Return whether the shown match is long enough.
 */
    પૂર્ણાંક
ins_compl_long_shown_match(શૂન્ય)
{
    પાછા (પૂર્ણાંક)compl_shown_match->cp_str.length
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned પૂર્ણાંક
get_cot_flags(શૂન્ય)
{
    પાછા curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    સ્થિર શૂન્ય
ins_compl_upd_pum(શૂન્ય)
{
    જો (compl_match_array == ખાલી)
	પાછા;

    પૂર્ણાંક h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    જો (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    સ્થિર શૂન્ય
ins_compl_del_pum(શૂન્ય)
{
    જો (compl_match_array == ખાલી)
	પાછા;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    પૂર્ણાંક
pum_wanted(શૂન્ય)
{
    // 'completeopt' must contain "menu" or "menuone"
    પાછા (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    સ્થિર પૂર્ણાંક
pum_enough_matches(શૂન્ય)
{
    compl_T     *compl;
    પૂર્ણાંક		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    કરો
    {
	જો (compl == ખાલી || (!match_at_original_text(compl) && ++i == 2))
	    તોડો;
	compl = compl->cp_next;
    } જ્યારે (!is_first_match(compl));

    જો ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	પાછા (i >= 1);
    પાછા (i >= 2);
}

#જો defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    સ્થિર dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    જો (dict == ખાલી)
	પાછા ખાલી;
    fill_complete_info_dict(dict, match, FALSE);
    પાછા dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    સ્થિર શૂન્ય
trigger_complete_changed_event(પૂર્ણાંક cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    સ્થિર પૂર્ણાંક	    recursive = FALSE;
    save_v_event_T  save_v_event;

    જો (recursive)
	પાછા;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    જો (item == ખાલી)
	પાછા;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_વસ્તુઓ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ખાલી, ખાલી, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    સ્થિર શૂન્ય*
cp_get_next(શૂન્ય *ગાંઠ)
{
    પાછા ((compl_T*)ગાંઠ)->cp_next;
}

    સ્થિર શૂન્ય
cp_set_next(શૂન્ય *ગાંઠ, શૂન્ય *next)
{
    ((compl_T*)ગાંઠ)->cp_next = (compl_T*)next;
}

    સ્થિર શૂન્ય*
cp_get_prev(શૂન્ય* ગાંઠ)
{
    પાછા ((compl_T*)ગાંઠ)->cp_prev;
}

    સ્થિર શૂન્ય
cp_set_prev(શૂન્ય* ગાંઠ, શૂન્ય* prev)
{
    ((compl_T*)ગાંઠ)->cp_prev = (compl_T*)prev;
}

    સ્થિર પૂર્ણાંક
cp_compare_fuzzy(અચલ શૂન્ય* a, અચલ શૂન્ય* b)
{
    પૂર્ણાંક score_a = ((compl_T*)a)->cp_score;
    પૂર્ણાંક score_b = ((compl_T*)b)->cp_score;

    જો (score_a != score_b)
	પાછા (score_b > score_a) ? 1 : -1;

    પાછા ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    સ્થિર પૂર્ણાંક
cp_compare_nearest(અચલ શૂન્ય* a, અચલ શૂન્ય* b)
{
    પૂર્ણાંક score_a = ((compl_T*)a)->cp_score;
    પૂર્ણાંક score_b = ((compl_T*)b)->cp_score;
    જો (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	પાછા 0;
    પાછા (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
prepend_startcol_text(string_T *dest, string_T *src, પૂર્ણાંક startcol)
{
    પૂર્ણાંક prepend_len = compl_col - startcol;
    પૂર્ણાંક new_length = prepend_len + (પૂર્ણાંક)src->length;

    dest->length = (size_t)new_length;
    dest->string = alloc(new_length + 1);  // +1 for NUL
    જો (dest->string == ખાલી)
    {
	dest->length = 0;
	પાછા FAIL;
    }

    char_u	*લીટી = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, લીટી + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->length);
    dest->string[new_length] = NUL;
    પાછા OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    સ્થિર string_T *
get_leader_for_startcol(compl_T *match, પૂર્ણાંક cached)
{
    સ્થિર string_T adjusted_leader = {ખાલી, 0};

    જો (match == ખાલી)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	પાછા ખાલી;
    }

    જો (cpt_sources_array == ખાલી)
	જાઓ theend;

    પૂર્ણાંક	cpt_idx = match->cp_cpt_source_idx;
    જો (cpt_idx < 0)
	જાઓ theend;
    પૂર્ણાંક	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    જો (compl_leader.string == ખાલી)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	જો (startcol < 0 || startcol >= compl_col)
	    પાછા &compl_orig_text;
	પાછા &compl_leader;  // pass through (startcol < compl_col)
    }

    જો (compl_col <= 0)
	જાઓ theend;

    જો (startcol >= 0 && startcol < compl_col)
    {
	પૂર્ણાંક prepend_len = compl_col - startcol;
	પૂર્ણાંક new_length = prepend_len + (પૂર્ણાંક)compl_leader.length;
	જો (cached && (size_t)new_length == adjusted_leader.length
		&& adjusted_leader.string != ખાલી)
	    પાછા &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	જો (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    જાઓ theend;

	પાછા &adjusted_leader;
    }
theend:
    પાછા &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    સ્થિર શૂન્ય
set_fuzzy_score(શૂન્ય)
{
    compl_T *compl;
    char_u  *pattern;
    પૂર્ણાંક	    use_leader;

    જો (compl_first_match == ખાલી)
	પાછા;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ખાલી && compl_leader.length > 0);
    જો (!use_leader)
    {
	જો (compl_orig_text.string == ખાલી || compl_orig_text.length == 0)
	    પાછા;
	pattern = compl_orig_text.string;
    }
    નહીંતર
    {
	/* Clear the leader cache once before the loop */
	(શૂન્ય)get_leader_for_startcol(ખાલી, TRUE);
	pattern = ખાલી;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    પૂર્ણાંક next = 0;
    compl = compl_first_match;
    કરો
    {
	જો (compl->cp_order < 0)
	    compl->cp_order = next;
	next = MAX(next, compl->cp_order + 1);

	જો (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_next;
    } જ્યારે (compl != ખાલી && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    સ્થિર શૂન્ય
sort_compl_match_list(પૂર્ણાંક (*compare)(અચલ શૂન્ય *, અચલ શૂન્ય *))
{
    compl_T	*orig_text;

    જો (!compl_first_match || is_first_match(compl_first_match->cp_next))
	પાછા;

    orig_text = find_original_text_match();
    જો (orig_text == ખાલી)
	પાછા;

    ins_compl_make_linear();
    જો (orig_text == compl_first_match)
    {
	compl_first_match->cp_next->cp_prev = ખાલી;
	compl_first_match->cp_next = mergesort_list(compl_first_match->cp_next,
		cp_get_next, cp_set_next, cp_get_prev, cp_set_prev, compare);
	compl_first_match->cp_next->cp_prev = compl_first_match;
    }
    નહીંતર
    {
	compl_T	*tail;

	orig_text->cp_prev->cp_next = ખાલી;
	compl_first_match = mergesort_list(compl_first_match, cp_get_next,
		cp_set_next, cp_get_prev, cp_set_prev, compare);
	tail = compl_first_match;
	જ્યારે (tail->cp_next != ખાલી)
	    tail = tail->cp_next;
	tail->cp_next = orig_text;
	orig_text->cp_prev = tail;
    }
    (શૂન્ય)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    સ્થિર પૂર્ણાંક
get_user_highlight_attr(પૂર્ણાંક hl_id)
{
    પૂર્ણાંક	    attr;

    જો (hl_id <= 0)
	પાછા -1;
    attr = syn_id2attr(hl_id);
    પાછા attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    સ્થિર પૂર્ણાંક
ins_compl_build_pum(શૂન્ય)
{
    compl_T     *compl;
    compl_T     *shown_compl = ખાલી;
    પૂર્ણાંક		did_find_shown_match = FALSE;
    પૂર્ણાંક		shown_match_ok = FALSE;
    પૂર્ણાંક		i = 0;
    પૂર્ણાંક		cur = -1;
    પૂર્ણાંક		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_head = ખાલી;
    compl_T	*match_tail = ખાલી;
    compl_T	*match_next = ખાલી;
    પૂર્ણાંક		*match_ગણતરી = ખાલી;
    પૂર્ણાંક		is_forward = compl_shows_dir_forward();
    પૂર્ણાંક		is_cpt_completion = (cpt_sources_array != ખાલી);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    જો (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    જો (compl_leader.string != ખાલી
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_next;

    જો (is_cpt_completion)
    {
	match_ગણતરી = ALLOC_CLEAR_MULT(પૂર્ણાંક, cpt_sources_ગણતરી);
	જો (match_ગણતરી == ખાલી)
	    પાછા -1;
    }

    (શૂન્ય)get_leader_for_startcol(ખાલી, TRUE); // Clear the cache

    compl = compl_first_match;
    કરો
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	જો (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	જો (!match_at_original_text(compl)
		&& (leader->string == ખાલી
		    || ins_compl_equal_sc(compl, leader->string,
			(પૂર્ણાંક)leader->length)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    પૂર્ણાંક match_limit_exceeded = FALSE;
	    પૂર્ણાંક cur_source = compl->cp_cpt_source_idx;
	    જો (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_ગણતરી[cur_source]++;
		પૂર્ણાંક max_matches = cpt_sources_array[cur_source].cs_max_matches;
		જો (max_matches > 0 && match_ગણતરી[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    જો (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		જો (match_head == ખાલી)
		    match_head = compl;
		નહીંતર
		    match_tail->cp_match_next = compl;
		match_tail = compl;

		જો (!shown_match_ok && !cot_fuzzy())
		{
		    જો (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    નહીંતર
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		નહીંતર જો (cot_fuzzy())
		{
		    જો (i == 0)
			shown_compl = compl;

		    જો (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	જો (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    જો (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    જો (!shown_match_ok && shown_compl != ખાલી)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } જ્યારે (compl != ખાલી && !is_first_match(compl));

    vim_free(match_ગણતરી);

    જો (compl_match_arraysize == 0)
	પાછા -1;

    જો (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    જો (compl_match_array == ખાલી)
	પાછા -1;

    compl = match_head;
    i = 0;
    જ્યારે (compl != ખાલી)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != ખાલી
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != ખાલી
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_next = compl->cp_match_next;
	compl->cp_match_next = ખાલી;
	compl = match_next;
    }

    જો (!shown_match_ok)    // no displayed match at all
	cur = -1;

    પાછા cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    શૂન્ય
ins_compl_show_pum(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		cur = -1;
    colnr_T	col;

    જો (!pum_wanted() || !pum_enough_matches())
	પાછા;

    // Avoid redrawing the screen under a pum that stays in place.
    જો (!pum_redraw_in_same_position())
	pum_call_update_screen();

    જો (compl_match_array == ખાલી)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    નહીંતર
    {
	// popup menu already exists, only need to find the current item.
	માટે (i = 0; i < compl_match_arraysize; ++i)
	{
	    જો (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		તોડો;
	    }
	}
    }

    જો (compl_match_array == ખાલી)
    {
#ifdef FEAT_EVAL
	જો (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	પાછા;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    પૂર્ણાંક pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    જો (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    જો (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    સ્થિર શૂન્ય
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    પૂર્ણાંક		flags,		// DICT_FIRST and/or DICT_EXACT
    પૂર્ણાંક		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		save_p_scs;
    પૂર્ણાંક		dir = compl_direction;

    જો (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	જો (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	નહીંતર
#endif
	    પાછા;
    }

    buf = alloc(LSIZE);
    જો (buf == ખાલી)
	પાછા;
    regmatch.regprog = ખાલી;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    જો (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    જો (ctrl_x_mode_લીટી_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	જો (pat_esc == ખાલી)
	    જાઓ theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	જો (ptr == ખાલી)
	{
	    vim_free(pat_esc);
	    જાઓ theend;
	}
	vim_snprintf((અક્ષર *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    નહીંતર
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	જો (regmatch.regprog == ખાલી)
	    જાઓ theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    જ્યારે (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	જો (flags == DICT_EXACT)
	{
	    ગણતરી = 1;
	    files = &dict;
	}
	નહીંતર
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    જો (!thesaurus && STRCMP(buf, "spell") == 0)
		ગણતરી = -1;
	    નહીંતર
#endif
		જો (vim_strchr(buf, '`') != ખાલી
		    || expand_wildcards(1, &buf, &ગણતરી, &files,
						     EW_FILE|EW_SILENT) != OK)
		ગણતરી = 0;
	}

#ifdef FEAT_SPELL
	જો (ગણતરી == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    જો (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    નહીંતર
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	નહીંતર
#endif
	    જો (ગણતરી > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(ગણતરી, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    જો (flags != DICT_EXACT)
		FreeWild(ગણતરી, files);
	}
	જો (flags != 0)
	    તોડો;
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
    સ્થિર પૂર્ણાંક
thesaurus_add_શબ્દો_in_લીટી(
    char_u	*fname,
    char_u	**buf_arg,
    પૂર્ણાંક		dir,
    char_u	*skip_word)
{
    પૂર્ણાંક		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    જ્યારે (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	જો (*ptr == NUL || *ptr == NL)
	    તોડો;
	wstart = ptr;

	// Find end of the word.
	જો (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    જ્યારે (*ptr != NUL)
	    {
		પૂર્ણાંક l = (*mb_ptr2len)(ptr);

		જો (l < 2 && !vim_iswordc(*ptr))
		    તોડો;
		ptr += l;
	    }
	નહીંતર
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	જો (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (પૂર્ણાંક)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    જો (status == FAIL)
		તોડો;
	}
    }

    *buf_arg = ptr;
    પાછા status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    સ્થિર શૂન્ય
ins_compl_files(
    પૂર્ણાંક		ગણતરી,
    char_u	**files,
    પૂર્ણાંક		thesaurus,
    પૂર્ણાંક		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    પૂર્ણાંક		*dir)
{
    char_u	*ptr;
    પૂર્ણાંક		i;
    FILE	*fp;
    પૂર્ણાંક		add_r;
    char_u	*leader = ખાલી;
    પૂર્ણાંક		leader_len = 0;
    પૂર્ણાંક		score = 0;
    પૂર્ણાંક		len = 0;
    char_u	*લીટી_end = ખાલી;

    જો (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (પૂર્ણાંક)ins_compl_leader_len();
    }

    માટે (i = 0; i < ગણતરી && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((અક્ષર *)files[i], "r");  // open dictionary file
	જો (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    સંદેશ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (અક્ષર *)files[i]);
	    (શૂન્ય)સંદેશ_trunc_attr((અક્ષર *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	જો (fp == ખાલી)
	    ચાલુ;

	// Read dictionary file line by line.
	// Check each line for a match.
	જ્યારે (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    જો (cot_fuzzy() && leader_len > 0)
	    {
		લીટી_end = find_લીટી_end(ptr);
		જ્યારે (ptr < લીટી_end)
		{
		    જો (fuzzy_match_str_in_લીટી(&ptr, leader, &len, ખાલી, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_લીટી_or_eval()
					? find_લીટી_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (પૂર્ણાંક)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			જો (add_r == FAIL)
			    તોડો;
			ptr = end_ptr;  // start from next word
			જો (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_next
				&& score == compl_first_match->cp_next->cp_score)
			    compl_અંક_bests++;
		    }
		}
	    }
	    નહીંતર જો (regmatch != ખાલી)
	    {
		જ્યારે (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_લીટી_or_eval() ? find_લીટી_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (પૂર્ણાંક)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    જો (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_શબ્દો_in_લીટી(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    જો (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    નહીંતર જો (add_r == FAIL)
			તોડો;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    જો (*ptr == '\n' || got_int)
			તોડો;
		}
	    }
	    લીટી_breakcheck();
	    ins_compl_check_ચાવીઓ(50, FALSE);
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
    જો (has_mbyte)
    {
	જ્યારે (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    નહીંતર
    {
	જ્યારે (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    પાછા ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    જો (has_mbyte)
    {
	પૂર્ણાંક start_class = mb_get_class(ptr);
	જો (start_class > 1)
	    જ્યારે (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		જો (mb_get_class(ptr) != start_class)
		    તોડો;
	    }
    }
    નહીંતર
	જ્યારે (vim_iswordc(*ptr))
	    ++ptr;
    પાછા ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_લીટી_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    જ્યારે (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    પાછા s;
}

/*
 * Free a completion item in the list
 */
    સ્થિર શૂન્ય
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    જો (compl_strings_ht.ht_used > 0 && match->cp_str.string != ખાલી
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	જો (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    જો (--entry->cse_ગણતરી <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    જો (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    માટે (પૂર્ણાંક i = 0; i < CPT_ગણતરી; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_data);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    સ્થિર શૂન્ય
ins_compl_free(શૂન્ય)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    જો (compl_first_match == ખાલી)
	પાછા;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    કરો
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	ins_compl_item_free(match);
    } જ્યારે (compl_curr_match != ખાલી && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = ખાલી;
    compl_shown_match = ખાલી;
    compl_old_match = ખાલી;
}

/*
 * Reset/clear the completion state.
 */
    શૂન્ય
ins_compl_clear(શૂન્ય)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = ખાલી;
    compl_curr_buf = ખાલી;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ખાલી;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_અંક_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    પૂર્ણાંક
ins_compl_active(શૂન્ય)
{
    પાછા compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    પૂર્ણાંક
ins_compl_win_active(win_T *wp)
{
    પાછા ins_compl_active() && wp == compl_curr_win
	&& wp->w_buffer == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    પૂર્ણાંક
ins_compl_used_match(શૂન્ય)
{
    પાછા compl_used_match;
}

/*
 * Initialize get longest common string.
 */
    શૂન્ય
ins_compl_init_get_longest(શૂન્ય)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    પૂર્ણાંક
ins_compl_interrupted(શૂન્ય)
{
    પાછા compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    પૂર્ણાંક
ins_compl_enter_selects(શૂન્ય)
{
    પાછા compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(શૂન્ય)
{
    પાછા compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    પૂર્ણાંક
ins_compl_len(શૂન્ય)
{
    પાછા compl_length;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    પૂર્ણાંક
ins_compl_has_preinsert(શૂન્ય)
{
    પૂર્ણાંક cur_cot_flags = get_cot_flags();
    જો (compl_autocomplete && p_ic && !p_inf)
	પાછા FALSE;
    પાછા !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    પૂર્ણાંક
ins_compl_preinsert_effect(શૂન્ય)
{
    જો (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	પાછા FALSE;

    પાછા curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    પૂર્ણાંક
ins_compl_bs(શૂન્ય)
{
    char_u	*લીટી;
    char_u	*p;

    જો (ins_compl_preinsert_effect())
	ins_compl_delete();

    લીટી = ml_get_curline();
    p = લીટી + curwin->w_cursor.col;
    MB_PTR_BACK(લીટી, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    જો ((પૂર્ણાંક)(p - લીટી) - (પૂર્ણાંક)compl_col < 0
	    || ((પૂર્ણાંક)(p - લીટી) - (પૂર્ણાંક)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (પૂર્ણાંક)(p - લીટી) - (પૂર્ણાંક)compl_col
							- compl_length < 0))
	પાછા K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    જો (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.length = (size_t)((p - લીટી) - compl_col);
    compl_leader.string = vim_strnsave(લીટી + compl_col, compl_leader.length);
    જો (compl_leader.string == ખાલી)
    {
	compl_leader.length = 0;
	પાછા K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    જો (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    જો (compl_shown_match != ખાલી)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    પાછા NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    સ્થિર પૂર્ણાંક
ins_compl_need_restart(શૂન્ય)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    પાછા compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    પૂર્ણાંક
ins_compl_has_autocomplete(શૂન્ય)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    પાછા curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    સ્થિર શૂન્ય
ins_compl_fuzzy_sort(શૂન્ય)
{
    પૂર્ણાંક	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    જો (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	જો ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    પૂર્ણાંક none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_prev);
	    જો (!none_selected)
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
    સ્થિર શૂન્ય
ins_compl_new_leader(શૂન્ય)
{
    પૂર્ણાંક	    save_w_wrow = curwin->w_wrow;
    પૂર્ણાંક	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_ઉમેરો_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    જો (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.length);
	જો (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	જો (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    નહીંતર
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	જો (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	જો (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	નહીંતર
	    compl_autocomplete = FALSE;
	જો (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    જો (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    જો (compl_match_array == ખાલી)
	compl_enter_selects = FALSE;
    નહીંતર જો (ins_compl_has_preinsert() && compl_leader.length > 0)
	ins_compl_ઉમેરો(TRUE, FALSE);
    નહીંતર જો (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.length > 0 && !ins_compl_preinsert_effect())
	ins_compl_ઉમેરો(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    સ્થિર પૂર્ણાંક
get_compl_len(શૂન્ય)
{
    પૂર્ણાંક off = (પૂર્ણાંક)curwin->w_cursor.col - (પૂર્ણાંક)compl_col;
    પાછા MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    શૂન્ય
ins_compl_addleader(પૂર્ણાંક c)
{
    પૂર્ણાંક	    cc;

    જો (ins_compl_preinsert_effect())
	ins_compl_delete();

    જો (stop_arrow() == FAIL)
	પાછા;
    જો (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	જો (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    નહીંતર
    {
	ins_char(c);
	જો (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    જો (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    જો (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.length = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.length);
	જો (compl_leader.string == ખાલી)
	{
	    compl_leader.length = 0;
	    પાછા;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    સ્થિર શૂન્ય
ins_compl_restart(શૂન્ય)
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
    compl_અંક_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    સ્થિર શૂન્ય
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    જો (match == ખાલી)
	પાછા;

    p = vim_strnsave(str, len);
    જો (p == ખાલી)
	પાછા;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.length = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    શૂન્ય
ins_compl_addfrommatch(શૂન્ય)
{
    char_u	*p;
    પૂર્ણાંક		len = (પૂર્ણાંક)curwin->w_cursor.col - (પૂર્ણાંક)compl_col;
    પૂર્ણાંક		c;

    p = compl_shown_match->cp_str.string;
    જો ((પૂર્ણાંક)compl_shown_match->cp_str.length <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	જો (!match_at_original_text(compl_shown_match))
	    પાછા;

	p = ખાલી;
	plen = 0;
	માટે (cp = compl_shown_match->cp_next; cp != ખાલી
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    જો (compl_leader.string == ખાલી
		    || ins_compl_equal(cp, compl_leader.string,
			(પૂર્ણાંક)compl_leader.length))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.length;
		તોડો;
	    }
	}
	જો (p == ખાલી || (પૂર્ણાંક)plen <= len)
	    પાછા;
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
    સ્થિર પૂર્ણાંક
set_ctrl_x_mode(પૂર્ણાંક c)
{
    પૂર્ણાંક	    retval = FALSE;

    પસંદગી (c)
    {
	સ્થિતિ Ctrl_E:
	સ્થિતિ Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    જો (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    નહીંતર
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ખાલી;
	    showmode();
	    તોડો;
	સ્થિતિ Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_લીટી;
	    તોડો;
	સ્થિતિ Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    તોડો;
	સ્થિતિ Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    તોડો;
	સ્થિતિ Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    જો (vpeekc() == '=')
		તોડો;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    તોડો;
	સ્થિતિ Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    તોડો;
#ifdef FEAT_COMPL_FUNC
	સ્થિતિ Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    તોડો;
	સ્થિતિ Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    તોડો;
#endif
	સ્થિતિ 's':
	સ્થિતિ Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    તોડો;
	સ્થિતિ Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    તોડો;
#ifdef FEAT_FIND_ID
	સ્થિતિ Ctrl_I:
	સ્થિતિ K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    તોડો;
	સ્થિતિ Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    તોડો;
#endif
	સ્થિતિ Ctrl_V:
	સ્થિતિ Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    તોડો;
	સ્થિતિ Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ખાલી;
	    showmode();
	    retval = TRUE;
	    તોડો;
	સ્થિતિ Ctrl_P:
	સ્થિતિ Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    જો (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    નહીંતર જો (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	મૂળભૂત:
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
	    જો (c == Ctrl_X)
	    {
		જો (compl_cont_mode != 0)
		    compl_cont_status = 0;
		નહીંતર
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ખાલી;
	    showmode();
	    તોડો;
    }

    પાછા retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    સ્થિર શૂન્ય
trigger_complete_done_event(પૂર્ણાંક mode UNUSED, string_T *word UNUSED)
{
#જો defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    જો (word == ખાલી || word->string == ખાલી)
	(શૂન્ય)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    નહીંતર
	(શૂન્ય)dict_add_string_len(v_event, "complete_word", word->string, (પૂર્ણાંક)word->length);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (શૂન્ય)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == ખાલી) ? (char_u *)"" : mode_str->string, (પૂર્ણાંક)mode_str->length);

    dict_set_વસ્તુઓ_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#જો defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    સ્થિર પૂર્ણાંક
ins_compl_stop(પૂર્ણાંક c, પૂર્ણાંક prev_mode, પૂર્ણાંક retval)
{
    પૂર્ણાંક		want_cindent;
    string_T	word = {ખાલી, 0};

    // Remove pre-inserted text when present.
    જો (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    જો (compl_curr_match != ખાલી || compl_leader.string != ખાલી || c == Ctrl_E)
    {
	char_u	*ptr = ખાલી;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	જો (compl_curr_match != ખાલી && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    જો (compl_cont_mode == CTRL_X_WHOLE_લીટી)
    {
	// re-indent the current line
	જો (want_cindent)
	{
	    do_c_પદાવલિ_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    નહીંતર જો (!compl_autocomplete || compl_used_match)
    {
	પૂર્ણાંક prev_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	જો (prev_col > 0)
	    dec_cursor();
	// only format when something was inserted
	જો (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	જો (prev_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    જો ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.length);
	જો (word.string == ખાલી)
	    word.length = 0;
	નહીંતર
	    word.length = compl_shown_match->cp_str.length;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    જો (c == Ctrl_E)
    {
	char_u *p = ખાલી;
	size_t	plen = 0;

	ins_compl_delete();
	જો (compl_leader.string != ખાલી)
	{
	    p = compl_leader.string;
	    plen = compl_leader.length;
	}
	નહીંતર જો (compl_first_match != ખાલી)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.length;
	}
	જો (p != ખાલી)
	{
	    પૂર્ણાંક	    compl_len = get_compl_len();

	    જો ((પૂર્ણાંક)plen > compl_len)
		ins_compl_ઉમેરો_bytes(p + compl_len, (પૂર્ણાંક)plen - compl_len);
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
    જો (!shortmess(SHM_COMPLETIONMENU))
	સંદેશ_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    જો (edit_submode != ખાલી)
    {
	edit_submode = ખાલી;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_અંક_bests = 0;
    compl_ins_end_col = 0;

    જો (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    જો (want_cindent && in_cinkeys(ચાવી_COMPLETE, ' ', inindent(0)))
	do_c_પદાવલિ_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(prev_mode, &word);
    vim_free(word.string);

    પાછા retval;
}

/*
 * Cancel completion.
 */
    પૂર્ણાંક
ins_compl_cancel(શૂન્ય)
{
    પાછા ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    પૂર્ણાંક
ins_compl_prep(પૂર્ણાંક c)
{
    પૂર્ણાંક	    retval = FALSE;
    પૂર્ણાંક	    prev_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    જો (c != Ctrl_R && vim_is_ctrl_x_ચાવી(c))
	edit_submode_extra = ખાલી;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    જો (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	પાછા retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    જો (is_mouse_ચાવી(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	જો (c == K_LEFTRELEASE
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
	    પાછા retval;
	જો (popup_visible)
	{
	    પૂર્ણાંક	    row = mouse_row;
	    પૂર્ણાંક	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    જો (wp != ખાલી && WIN_IS_POPUP(wp))
		પાછા retval;
	}
    }
#endif

    જો (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	જો (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_ચાવી(c)
		|| !vim_is_ctrl_x_ચાવી(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    જો (c == Ctrl_Z)
		retval = TRUE;
	}
	નહીંતર
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    જો (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    જો (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    નહીંતર જો (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	જો (c != Ctrl_R && !vim_is_ctrl_x_ચાવી(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = ખાલી;
	}
	showmode();
    }

    જો (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	જો ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_ચાવી(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, prev_mode, retval);
    }
    નહીંતર જો (ctrl_x_mode == CTRL_X_LOCAL_સંદેશ)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, ખાલી);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    જો (c != Ctrl_R && !vim_is_ctrl_x_ચાવી(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    પાછા retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    સ્થિર શૂન્ય
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    પૂર્ણાંક	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    જો (ptr == ખાલી)
    {
	જો (compl_leader.string != ખાલી)
	    ptr = compl_leader.string;
	નહીંતર
	    પાછા;  // nothing to do
    }
    જો (compl_orig_text.string != ખાલી)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	જ્યારે (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	જો (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	માટે (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    જો (ptr != ખાલી)
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
    સ્થિર buf_T *
ins_compl_next_buf(buf_T *buf, પૂર્ણાંક flag)
{
    સ્થિર win_T    *wp = ખાલી;
    પૂર્ણાંક		    skip_buffer;

    જો (flag == 'w')		// just windows
    {
	જો (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	જ્યારે (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_next != ખાલી) ? wp->w_next : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    જો (wp == curwin || !wp->w_buffer->b_scanned)
		તોડો;
	}
	buf = wp->w_buffer;
    }
    નહીંતર
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	જ્યારે (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_next != ખાલી) ? buf->b_next : firstbuf;
	    // Break if we're back at start buffer
	    જો (buf == curbuf)
		તોડો;

	    // Check buffer conditions based on flag
	    જો (flag == 'U')
		skip_buffer = buf->b_p_bl;
	    નહીંતર
		skip_buffer = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ખાલી) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    જો (!skip_buffer && !buf->b_scanned)
		તોડો;
	}
    }
    પાછા buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    સ્થિર પૂર્ણાંક
get_cpt_sources_ગણતરી(char_u *cpt)
{
    char_u  dummy[LSIZE];
    પૂર્ણાંક	    ગણતરી = 0;
    char_u  *p;

    માટે (p = cpt; *p != NUL; )
    {
	જ્યારે (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	જો (*p != NUL)
	{
	    (શૂન્ય)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    ગણતરી++;
	}
    }

    પાછા ગણતરી;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
સ્થિર callback_T cfu_cb;	    // 'completefunc' callback function
સ્થિર callback_T ofu_cb;	    // 'omnifunc' callback function
સ્થિર callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
સ્થિર callback_T *cpt_cb;	    // Callback functions associated with F{func}
સ્થિર પૂર્ણાંક	  cpt_cb_ગણતરી;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    સ્થિર શૂન્ય
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    જો (globcb->cb_name != ખાલી && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    અક્ષર *
did_set_completefunc(optset_T *args)
{
    પૂર્ણાંક	retval;

    જો (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    નહીંતર
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	જો (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    પાછા retval == FAIL ? e_invalid_argument : ખાલી;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    શૂન્ય
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
    અક્ષર *
did_set_omnifunc(optset_T *args)
{
    પૂર્ણાંક	retval;

    જો (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    નહીંતર
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	જો (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    પાછા retval == FAIL ? e_invalid_argument : ખાલી;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    શૂન્ય
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    શૂન્ય
clear_cpt_callbacks(callback_T **callbacks, પૂર્ણાંક ગણતરી)
{
    જો (callbacks == ખાલી || *callbacks == ખાલી)
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < ગણતરી; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    સ્થિર પૂર્ણાંક
copy_cpt_callbacks(callback_T **dest, પૂર્ણાંક *dest_cnt, callback_T *src, પૂર્ણાંક cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    જો (cnt == 0)
	પાછા OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    જો (*dest == ખાલી)
	પાછા FAIL;

    *dest_cnt = cnt;

    માટે (પૂર્ણાંક i = 0; i < cnt; i++)
	જો (src[i].cb_name != ખાલી && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    પાછા OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    શૂન્ય
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    જો (buf == ખાલી)
	પાછા;
    (શૂન્ય)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_ગણતરી, cpt_cb,
	    cpt_cb_ગણતરી);
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
    સ્થિર પૂર્ણાંક
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, પૂર્ણાંક *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    પૂર્ણાંક	    idx = 0;
    પૂર્ણાંક	    slen;
    પૂર્ણાંક	    ગણતરી;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    ગણતરી = get_cpt_sources_ગણતરી(cpt);
    જો (ગણતરી == 0)
	પાછા OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, ગણતરી);
    જો (*cbp == ખાલી)
	પાછા FAIL;
    *cnt = ગણતરી;

    માટે (p = cpt; *p != NUL; )
    {
	જ્યારે (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	જો (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    જો (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		જો (caret != ખાલી)
		    *caret = NUL;

		જો (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ખાલી;
	    }
	    idx++;
	}
    }

    પાછા OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    પૂર્ણાંક
set_cpt_callbacks(optset_T *args)
{
    પૂર્ણાંક	    opt_flags = args->os_flags;

    જો (curbuf == ખાલી)
	પાછા FAIL;

    // ":setglobal" does not change the buffer-local option value
    જો (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_ગણતરી) != OK)
	પાછા FAIL;

    // ":setlocal" does not change the global option value
    જો (opt_flags & OPT_LOCAL)
	પાછા OK;

    // Cache the callbacks for the global value
    જો (opt_flags & OPT_GLOBAL)
	પાછા parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_ગણતરી);

    // set case
    પાછા copy_cpt_callbacks(&cpt_cb, &cpt_cb_ગણતરી, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_ગણતરી);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    અક્ષર *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    પૂર્ણાંક	retval;

    જો (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    નહીંતર
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	જો (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    પાછા retval == FAIL ? e_invalid_argument : ખાલી;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    પૂર્ણાંક
set_ref_in_cpt_callbacks(callback_T *callbacks, પૂર્ણાંક ગણતરી, પૂર્ણાંક copyID)
{
    પૂર્ણાંક	abort = FALSE;

    જો (callbacks == ખાલી)
	પાછા FALSE;

    માટે (પૂર્ણાંક i = 0; i < ગણતરી; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    પાછા abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    પૂર્ણાંક
set_ref_in_insexpand_funcs(પૂર્ણાંક copyID)
{
    પૂર્ણાંક abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_ગણતરી, copyID);

    પાછા abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    સ્થિર char_u *
get_complete_funcname(પૂર્ણાંક type)
{
    પસંદગી (type)
    {
	સ્થિતિ CTRL_X_FUNCTION:
	    પાછા curbuf->b_p_cfu;
	સ્થિતિ CTRL_X_OMNI:
	    પાછા curbuf->b_p_ofu;
	સ્થિતિ CTRL_X_THESAURUS:
	    પાછા *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	મૂળભૂત:
	    પાછા (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    સ્થિર callback_T *
get_ઉમેરો_callback(પૂર્ણાંક type)
{
    જો (type == CTRL_X_FUNCTION)
	પાછા &curbuf->b_cfu_cb;
    જો (type == CTRL_X_OMNI)
	પાછા &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    પાછા (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    સ્થિર શૂન્ય
expand_by_function(પૂર્ણાંક type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = ખાલી;
    dict_T	*matchdict = ખાલી;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    પૂર્ણાંક		save_State = State;
    પૂર્ણાંક		retval;
    પૂર્ણાંક		is_cpt_function = (cb != ખાલી);
    પૂર્ણાંક		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    જો (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	જો (*funcname == NUL)
	    પાછા;
	cb = get_ઉમેરો_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_સંખ્યા;
    args[0].vval.v_સંખ્યા = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != ખાલી ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    જો (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    જો (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    જો (retval == OK)
    {
	પસંદગી (rettv.v_type)
	{
	    સ્થિતિ VAR_LIST:
		matchlist = rettv.vval.v_list;
		તોડો;
	    સ્થિતિ VAR_DICT:
		matchdict = rettv.vval.v_dict;
		તોડો;
	    સ્થિતિ VAR_SPECIAL:
		જો (rettv.vval.v_સંખ્યા == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    મૂળભૂત:
		// TODO: Give error message?
		clear_tv(&rettv);
		તોડો;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    જો (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	જાઓ theend;
    }

    જો (matchlist != ખાલી)
	ins_compl_add_list(matchlist);
    નહીંતર જો (matchdict != ખાલી)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    જો (matchdict != ખાલી)
	dict_unref(matchdict);
    જો (matchlist != ખાલી)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#જો defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    સ્થિર અંતઃરેખ પૂર્ણાંક
get_user_highlight_id(char_u *hlname)
{
    જો (hlname != ખાલી && *hlname != NUL)
	પાછા syn_check_group(hlname, (પૂર્ણાંક)STRLEN(hlname));
    પાછા 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    સ્થિર પૂર્ણાંક
ins_compl_add_tv(typval_T *tv, પૂર્ણાંક dir, પૂર્ણાંક fast)
{
    char_u	*word;
    પૂર્ણાંક		dup = FALSE;
    પૂર્ણાંક		empty = FALSE;
    પૂર્ણાંક		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_ગણતરી]);
    typval_T	user_data;
    પૂર્ણાંક		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    પૂર્ણાંક		user_hl[2] = { 0, 0 };

    user_data.v_type = VAR_UNKNOWN;
    જો (tv->v_type == VAR_DICT && tv->vval.v_dict != ખાલી)
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
	જો (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ખાલી
				  && dict_get_સંખ્યા(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	જો (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ખાલી)
	    dup = dict_get_સંખ્યા(tv->vval.v_dict, "dup");
	જો (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ખાલી)
	    empty = dict_get_સંખ્યા(tv->vval.v_dict, "empty");
	જો (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ખાલી
				  && dict_get_સંખ્યા(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    નહીંતર
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    જો (word == ખાલી || (!empty && *word == NUL))
    {
	clear_tv(&user_data);
	પાછા FAIL;
    }
    status = ins_compl_add(word, -1, ખાલી, cptext,
	    &user_data, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    જો (status != OK)
	clear_tv(&user_data);
    પાછા status;
}

/*
 * Add completions from a list.
 */
    સ્થિર શૂન્ય
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    પૂર્ણાંક		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_વસ્તુઓ(list, li)
    {
	જો (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	નહીંતર જો (did_emsg)
	    તોડો;
    }
}

/*
 * Add completions from a dict.
 */
    સ્થિર શૂન્ય
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_શબ્દો;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    જો (di_refresh != ખાલી && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	જો (v != ખાલી && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_શબ્દો = dict_find(dict, (char_u *)"words", 5);
    જો (di_શબ્દો != ખાલી && di_શબ્દો->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_શબ્દો->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    સ્થિર શૂન્ય
set_completion(colnr_T startcol, list_T *list)
{
    પૂર્ણાંક save_w_wrow = curwin->w_wrow;
    પૂર્ણાંક save_w_leftcol = curwin->w_leftcol;
    પૂર્ણાંક flags = CP_ORIGINAL_TEXT;
    unsigned પૂર્ણાંક cur_cot_flags = get_cot_flags();
    પૂર્ણાંક compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    પૂર્ણાંક compl_no_ઉમેરો = (cur_cot_flags & COT_NOINSERT) != 0;
    પૂર્ણાંક compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    જો (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    જો (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_length = (પૂર્ણાંક)curwin->w_cursor.col - (પૂર્ણાંક)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_length);
    જો (p_ic)
	flags |= CP_ICASE;
    જો (compl_orig_text.string == ખાલી)
    {
	compl_orig_text.length = 0;
	પાછા;
    }
    compl_orig_text.length = (size_t)compl_length;
    જો (ins_compl_add(compl_orig_text.string,
			(પૂર્ણાંક)compl_orig_text.length, ખાલી, ખાલી, ખાલી, 0,
			flags | CP_FAST, FALSE, ખાલી, FUZZY_SCORE_NONE) != OK)
	પાછા;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    પૂર્ણાંક no_select = compl_no_select || compl_longest;
    જો (compl_no_ઉમેરો || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	જો (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    નહીંતર
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_ઉમેરો;

    // Lazily show the popup menu, unless we got interrupted.
    જો (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    શૂન્ય
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	પાછા;

    જો ((State & MODE_ઉમેરો) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_ઉમેરો_mode));
	પાછા;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    જો (!undo_allowed())
	પાછા;

    જો (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	પૂર્ણાંક startcol = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી);
	જો (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    શૂન્ય
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    શૂન્ય
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_ચાવીઓ(0, TRUE);
    rettv->vval.v_સંખ્યા = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    સ્થિર શૂન્ય
ins_compl_mode(string_T *ret)
{
    જો (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == ખાલી) ? (char_u *)"" : mode->string;
	ret->length = mode->length;
    }
    નહીંતર
    {
	ret->string = (char_u *)"";
	ret->length = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    સ્થિર શૂન્ય
ins_compl_update_sequence_numbers(શૂન્ય)
{
    પૂર્ણાંક		સંખ્યા = 0;
    compl_T	*match;

    જો (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	માટે (match = compl_curr_match->cp_prev; match != ખાલી
		&& !is_first_match(match); match = match->cp_prev)
	    જો (match->cp_સંખ્યા != -1)
	    {
		સંખ્યા = match->cp_સંખ્યા;
		તોડો;
	    }
	જો (match != ખાલી)
	    // go up and assign all numbers which are not assigned yet
	    માટે (match = match->cp_next;
		    match != ખાલી && match->cp_સંખ્યા == -1;
					   match = match->cp_next)
		match->cp_સંખ્યા = ++સંખ્યા;
    }
    નહીંતર // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	માટે (match = compl_curr_match->cp_next; match != ખાલી
		&& !is_first_match(match); match = match->cp_next)
	{
	    જો (match->cp_સંખ્યા != -1)
	    {
		સંખ્યા = match->cp_સંખ્યા;
		તોડો;
	    }
	}
	જો (match != ખાલી)
	{
	    // go down and assign all numbers which are not assigned yet
	    માટે (match = match->cp_prev; match
		    && match->cp_સંખ્યા == -1;
					   match = match->cp_prev)
		match->cp_સંખ્યા = ++સંખ્યા;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    સ્થિર શૂન્ય
fill_complete_info_dict(dict_T *di, compl_T *match, પૂર્ણાંક add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (પૂર્ણાંક)match->cp_str.length);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    જો (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    જો (match->cp_user_data.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    નહીંતર
	dict_add_tv(di, "user_data", &match->cp_user_data);
}

/*
 * Get complete information
 */
    સ્થિર શૂન્ય
get_complete_info(list_T *what_list, dict_T *retdict)
{
    પૂર્ણાંક		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_વસ્તુઓ		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    પૂર્ણાંક		what_flag;

    જો (what_list == ખાલી)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    નહીંતર
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_વસ્તુઓ(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    જો (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    નહીંતર જો (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    નહીંતર જો (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_વસ્તુઓ;
	    નહીંતર જો (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    નહીંતર જો (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    નહીંતર જો (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    નહીંતર જો (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    નહીંતર જો (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    જો (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (પૂર્ણાંક)mode.length);
    }

    જો (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_સંખ્યા(retdict, "pum_visible", pum_visible());

    જો (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_સંખ્યા(retdict, "auto", compl_autostarted);

    જો (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*લીટી = ml_get_curline();
	પૂર્ણાંક	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? લીટી + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    જો (ret == OK && (what_flag & (CI_WHAT_વસ્તુઓ | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ખાલી;
	dict_T	    *di;
	compl_T     *match;
	પૂર્ણાંક         selected_idx = -1;
	પૂર્ણાંક	    has_વસ્તુઓ = what_flag & CI_WHAT_વસ્તુઓ;
	પૂર્ણાંક	    has_matches = what_flag & CI_WHAT_MATCHES;
	પૂર્ણાંક	    has_completed = what_flag & CI_WHAT_COMPLETED;

	જો (has_વસ્તુઓ || has_matches)
	{
	    li = list_alloc();
	    જો (li == ખાલી)
		પાછા;
	    ret = dict_add_list(retdict, (has_matches && !has_વસ્તુઓ)
						? "matches" : "items", li);
	    જો (ret == FAIL)
		list_unref(li);
	}
	જો (ret == OK && what_flag & CI_WHAT_SELECTED)
	    જો (compl_curr_match != ખાલી && compl_curr_match->cp_સંખ્યા == -1)
		ins_compl_update_sequence_numbers();
	જો (ret == OK && compl_first_match != ખાલી)
	{
	    પૂર્ણાંક list_idx = 0;
	    match = compl_first_match;
	    કરો
	    {
		જો (!match_at_original_text(match))
		{
		    જો (has_વસ્તુઓ
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			જો (di == ખાલી)
			    પાછા;
			ret = list_append_dict(li, di);
			જો (ret != OK)
			{
			    dict_unref(di);
			    પાછા;
			}
			fill_complete_info_dict(di, match, has_matches && has_વસ્તુઓ);
		    }
		    જો (compl_curr_match != ખાલી
			    && compl_curr_match->cp_સંખ્યા == match->cp_સંખ્યા)
			selected_idx = list_idx;
		    જો (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_next;
	    }
	    જ્યારે (match != ખાલી && !is_first_match(match));
	}
	જો (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_સંખ્યા(retdict, "selected", selected_idx);

	જો (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    જો (di == ખાલી)
		પાછા;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    જો (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    શૂન્ય
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ખાલી;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	જો (check_for_list_arg(argvars, 0) == FAIL)
	    પાછા;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    સ્થિર પૂર્ણાંક
thesaurus_func_complete(પૂર્ણાંક type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    પાછા type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#નહીંતર
    પાછા FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    સ્થિર પૂર્ણાંક
may_advance_cpt_index(char_u *cpt)
{
    char_u  *p = cpt;

    જો (cpt_sources_index == -1)
	પાછા FALSE;
    જ્યારે (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    પાછા (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
ગણના
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	prev_match_pos;		// previous match position
    પૂર્ણાંક		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    પૂર્ણાંક		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    પૂર્ણાંક		dict_f;			// "dict" is an exact file name or not
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
    સ્થિર પૂર્ણાંક
process_next_cpt_કિંમત(
	ins_compl_next_state_T *st,
	પૂર્ણાંક		*compl_type_arg,
	pos_T		*start_match_pos,
	પૂર્ણાંક		fuzzy_collect,
	પૂર્ણાંક		*advance_cpt_idx)
{
    પૂર્ણાંક	    compl_type = -1;
    પૂર્ણાંક	    status = INS_COMPL_CPT_OK;
    પૂર્ણાંક	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    જ્યારે (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    જો (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	જો (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_લીટી_ગણતરી;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    નહીંતર જો (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ખાલી
	    && (st->ins_buf = ins_compl_next_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	જો (st->ins_buf->b_ml.ml_mfp != ખાલી)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_લીટી_ગણતરી + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	નહીંતર	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    જો (st->ins_buf->b_fname == ખાલી)
	    {
		status = INS_COMPL_CPT_CONT;
		જાઓ done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	જો (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    સંદેશ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ખાલી
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ખાલી
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (શૂન્ય)સંદેશ_trunc_attr((અક્ષર *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    નહીંતર જો (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    નહીંતર
    {
	જો (ctrl_x_mode_લીટી_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	નહીંતર જો (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_index);
	    જો (!st->func_cb)
		compl_type = -1;
	}
#endif
	નહીંતર જો (!skip_source)
	{
	    જો (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		જો (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		નહીંતર
		    compl_type = CTRL_X_THESAURUS;
		જો (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    નહીંતર જો (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    નહીંતર જો (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    નહીંતર જો (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		જો (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    સંદેશ_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Scanning tags."));
		    (શૂન્ય)સંદેશ_trunc_attr((અક્ષર *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    નહીંતર
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(શૂન્ય)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_index(st->e_cpt);

	st->found_all = TRUE;
	જો (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    પાછા status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    સ્થિર શૂન્ય
get_next_include_file_completion(પૂર્ણાંક compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (પૂર્ણાંક)compl_pattern.length, FALSE, FALSE,
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
    સ્થિર શૂન્ય
get_next_dict_tsr_completion(પૂર્ણાંક compl_type, char_u *dict, પૂર્ણાંક dict_f)
{
#ifdef FEAT_COMPL_FUNC
    જો (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, ખાલી);
    નહીંતર
#endif
	ins_compl_dictionaries(
		dict != ખાલી ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != ખાલી ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    સ્થિર શૂન્ય
get_next_tag_completion(શૂન્ય)
{
    પૂર્ણાંક		save_p_ic;
    char_u	**matches;
    પૂર્ણાંક		અંક_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    જો (find_tags(compl_pattern.string, &અંક_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && અંક_matches > 0)
	ins_compl_add_matches(અંક_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    સ્થિર પૂર્ણાંક
compare_scores(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    પૂર્ણાંક idx_a = *(અચલ પૂર્ણાંક *)a;
    પૂર્ણાંક idx_b = *(અચલ પૂર્ણાંક *)b;
    પૂર્ણાંક score_a = compl_fuzzy_scores[idx_a];
    પૂર્ણાંક score_b = compl_fuzzy_scores[idx_b];
    પાછા score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    સ્થિર શૂન્ય
ins_compl_longest_ઉમેરો(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_ઉમેરો_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    સ્થિર શૂન્ય
fuzzy_longest_match(શૂન્ય)
{
    char_u	*prefix = ખાલી;
    પૂર્ણાંક		prefix_len = 0;
    પૂર્ણાંક		i = 0;
    પૂર્ણાંક		j = 0;
    char_u	*match_str = ખાલી;
    char_u	*prefix_ptr = ખાલી;
    char_u	*match_ptr = ખાલી;
    char_u	*leader = ખાલી;
    size_t	leader_len = 0;
    compl_T	*compl = ખાલી;
    પૂર્ણાંક		more_candidates = FALSE;
    compl_T	*nn_compl = ખાલી;

    જો (compl_અંક_bests == 0)
	પાછા;

    nn_compl = compl_first_match->cp_next->cp_next;
    જો (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_લીટી() ? compl_first_match
				    : compl_first_match->cp_next;
    જો (compl_અંક_bests == 1)
    {
	// no more candidates insert the match str
	જો (!more_candidates)
	{
	    ins_compl_longest_ઉમેરો(compl->cp_str.string);
	    compl_અંક_bests = 0;
	}
	compl_અંક_bests = 0;
	પાછા;
    }

    જો ((size_t)compl_અંક_bests > SIZE_MAX / કદ(compl_T *))
	પાછા;
    compl_best_matches = (compl_T **)alloc(compl_અંક_bests * કદ(compl_T *));
    જો (compl_best_matches == ખાલી)
	પાછા;
    જ્યારે (compl != ખાલી && i < compl_અંક_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_next;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (પૂર્ણાંક)compl_best_matches[0]->cp_str.length;

    માટે (i = 1; i < compl_અંક_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	જ્યારે (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    જો (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		તોડો;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	જો (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    જો (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	જાઓ end;

    prefix = vim_strnsave(prefix, prefix_len);
    જો (prefix != ખાલી)
    {
	ins_compl_longest_ઉમેરો(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = ખાલી;
    compl_અંક_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    સ્થિર શૂન્ય
get_next_filename_completion(શૂન્ય)
{
    char_u	**matches;
    પૂર્ણાંક		અંક_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    પૂર્ણાંક		i;
    પૂર્ણાંક		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    પૂર્ણાંક		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    પૂર્ણાંક		*fuzzy_indices_data;
    char_u	*last_sep = ખાલી;
    પૂર્ણાંક		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    પૂર્ણાંક		max_score = 0;
    પૂર્ણાંક		current_score = 0;
    પૂર્ણાંક		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    અક્ષર pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#નહીંતર
    અક્ષર pathsep = PATHSEP;
#endif

    જો (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	જો (curbuf->b_p_csl[0] == 's')
	{
	    માટે (i = 0; i < (પૂર્ણાંક)leader_len; i++)
	    {
		જો (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	નહીંતર જો (curbuf->b_p_csl[0] == 'b')
	{
	    માટે (i = 0; i < (પૂર્ણાંક)leader_len; i++)
	    {
		જો (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	જો (last_sep == ખાલી)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    જો (compl_pattern.string == ખાલી)
		પાછા;
	    compl_pattern.length = 1;
	}
	નહીંતર જો (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	નહીંતર
	{
	    // Split leader into path and file parts
	    પૂર્ણાંક path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    જો (path_with_wildcard != ખાલી)
	    {
		vim_snprintf((અક્ષર *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.length = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    જો (expand_wildcards(1, &compl_pattern.string, &અંક_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	પાછા;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, અંક_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    જો (curbuf->b_p_csl[0] != NUL)
    {
	માટે (i = 0; i < અંક_matches; ++i)
	{
	    ptr = matches[i];
	    જ્યારે (*ptr != NUL)
	    {
		જો (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		નહીંતર જો (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    જો (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, કદ(પૂર્ણાંક), 10);
	compl_fuzzy_scores = (પૂર્ણાંક *)alloc(કદ(પૂર્ણાંક) * અંક_matches);

	માટે (i = 0; i < અંક_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    જો (score != FUZZY_SCORE_NONE)
	    {
		જો (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((પૂર્ણાંક *)fuzzy_indices.ga_data)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	જો (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = ખાલી;
	    fuzzy_indices_data = (પૂર્ણાંક *)fuzzy_indices.ga_data;
	    qsort(fuzzy_indices_data, fuzzy_indices.ga_len, કદ(પૂર્ણાંક), compare_scores);

	    માટે (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_data[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_data[i]];
		જો (ins_compl_add(match, -1, ખાલી, ખાલી, ખાલી, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ખાલી, current_score) == OK)
		    dir = FORWARD;

		જો (need_collect_bests)
		{
		    જો (i == 0 || current_score == max_score)
		    {
			compl_અંક_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(અંક_matches, matches);
	}
	નહીંતર જો (leader_len > 0)
	{
	    FreeWild(અંક_matches, matches);
	    અંક_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	જો (compl_અંક_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	પાછા;
    }

    જો (અંક_matches > 0)
	ins_compl_add_matches(અંક_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    સ્થિર શૂન્ય
get_next_cmdline_completion(શૂન્ય)
{
    char_u	**matches;
    પૂર્ણાંક		અંક_matches;

    જો (expand_cmdline(&compl_xp, compl_pattern.string,
		(પૂર્ણાંક)compl_pattern.length, &અંક_matches, &matches) == EXPAND_OK)
    {
	પૂર્ણાંક		add_r = OK;
	પૂર્ણાંક		dir = compl_direction;

	માટે (પૂર્ણાંક i = 0; i < અંક_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_ગણતરી]) = {ખાલી, ખાલી, ખાલી, ખાલી};

	    જો (compl_xp.xp_files_abbr != ખાલી)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    જો (compl_xp.xp_files_kind != ખાલી)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    જો (compl_xp.xp_files_menu != ખાલી)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    જો (compl_xp.xp_files_info != ખાલી)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, ખાલી, cptext, ખાલી, dir,
		    CP_FAST, FALSE, ખાલી, FUZZY_SCORE_NONE);
	    જો (add_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(અંક_matches, matches);
	free_xp_files_extra(&compl_xp, અંક_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    સ્થિર શૂન્ય
get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    પૂર્ણાંક		અંક_matches;

    અંક_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    જો (અંક_matches > 0)
	ins_compl_add_matches(અંક_matches, matches, p_ic);
    નહીંતર
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    સ્થિર char_u *
ins_compl_get_next_word_or_લીટી(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	પૂર્ણાંક	*match_len,
	પૂર્ણાંક	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    પૂર્ણાંક		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (પૂર્ણાંક)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    જો (ctrl_x_mode_લીટી_or_eval())
    {
	જો (compl_status_adding())
	{
	    જો (cur_match_pos->lnum >= ins_buf->b_ml.ml_લીટી_ગણતરી)
		પાછા ખાલી;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    જો (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (પૂર્ણાંક)(ptr - tmp_ptr);
	    }
	}
    }
    નહીંતર
    {
	char_u	*tmp_ptr = ptr;

	જો (compl_status_adding() && compl_length <= len)
	{
	    tmp_ptr += compl_length;
	    // Skip if already inside a word.
	    જો (vim_iswordp(tmp_ptr))
		પાછા ખાલી;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (પૂર્ણાંક)(tmp_ptr - ptr);

	જો (compl_status_adding() && len == compl_length)
	{
	    જો (cur_match_pos->lnum < ins_buf->b_ml.ml_લીટી_ગણતરી)
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
		જો (tmp_ptr > ptr)
		{
		    જો (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			જો (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			જો (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ખાલી
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    જો (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (પૂર્ણાંક)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    જો (len == compl_length)
		પાછા ખાલી;
	}
    }

    *match_len = len;
    પાછા ptr;
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
    સ્થિર પૂર્ણાંક
get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    પૂર્ણાંક		found_new_match = FAIL;
    પૂર્ણાંક		save_p_scs;
    પૂર્ણાંક		save_p_ws;
    પૂર્ણાંક		looped_around = FALSE;
    char_u	*ptr = ખાલી;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_length > 0;
    char_u	*leader = ins_compl_leader();
    પૂર્ણાંક		score = FUZZY_SCORE_NONE;
    પૂર્ણાંક		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    જો (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    જો (!in_curbuf)
	p_ws = FALSE;
    નહીંતર જો (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    માટે (;;)
    {
	પૂર્ણાંક	cont_s_ipos = FALSE;

	++સંદેશ_silent;  // Don't want messages for wrapscan.

	જો (in_fuzzy_collect)
	{
	    found_new_match = શોધ_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	નહીંતર જો (ctrl_x_mode_whole_લીટી() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = શોધ_for_exact_લીટી(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	નહીંતર
	    found_new_match = searchit(ખાલી, st->ins_buf, st->cur_match_pos,
				ખાલી, compl_direction, compl_pattern.string, (પૂર્ણાંક)compl_pattern.length,
				1L, શોધ_KEEP + શોધ_NFMSG, RE_LAST, ખાલી);
	--સંદેશ_silent;
	જો (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	નહીંતર જો (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	નહીંતર જો (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    જો (looped_around)
		found_new_match = FAIL;
	    નહીંતર
		looped_around = TRUE;
	}
	નહીંતર જો (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    જો (looped_around)
		found_new_match = FAIL;
	    નહીંતર
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	જો (found_new_match == FAIL)
	    તોડો;

	// when ADDING, the text before the cursor matches, skip it
	જો (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    ચાલુ;

	જો (!in_fuzzy_collect)
	    ptr = ins_compl_get_next_word_or_લીટી(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	જો (ptr == ખાલી || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    ચાલુ;

	જો (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    જો (score < 0)
		score = -score;
	}

	જો (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? ખાલી : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    જો (in_fuzzy_collect && score == compl_first_match->cp_next->cp_score)
		compl_અંક_bests++;
	    found_new_match = OK;
	    તોડો;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    પાછા found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    સ્થિર callback_T *
get_callback_if_cpt_func(char_u *p, પૂર્ણાંક idx)
{
    જો (*p == 'o')
	પાછા &curbuf->b_ofu_cb;

    જો (*p == 'F')
    {
	જો (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    જો (curbuf->b_p_cpt_cb == ખાલી
		    || idx < 0 || idx >= curbuf->b_p_cpt_ગણતરી)
		પાછા ખાલી;
	    પાછા curbuf->b_p_cpt_cb[idx].cb_name != ખાલી
		? &curbuf->b_p_cpt_cb[idx] : ખાલી;
	}
	નહીંતર
	    પાછા &curbuf->b_cfu_cb; // 'cfu'
    }

    પાછા ખાલી;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    સ્થિર શૂન્ય
get_register_completion(શૂન્ય)
{
    પૂર્ણાંક		dir = compl_direction;
    yankreg_T	*reg = ખાલી;
    શૂન્ય	*reg_ptr = ખાલી;
    પૂર્ણાંક		adding_mode = compl_status_adding();

    માટે (પૂર્ણાંક i = 0; i < અંક_REGISTERS; i++)
    {
	પૂર્ણાંક regname = 0;
	જો (i == 0)
	    regname = '"';    // unnamed register
	નહીંતર જો (i < 10)
	    regname = '0' + i;
	નહીંતર જો (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	નહીંતર જો (i == STAR_REGISTER)
	    regname = '*';
	નહીંતર જો (i == PLUS_REGISTER)
	    regname = '+';
#endif
	નહીંતર
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	જો (!valid_yank_reg(regname, FALSE) || regname == '_')
	    ચાલુ;

	reg_ptr = get_register(regname, FALSE);
	જો (reg_ptr == ખાલી)
	    ચાલુ;

	reg = (yankreg_T *)reg_ptr;

	માટે (પૂર્ણાંક j = 0; j < reg->y_size; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    જો (str == ખાલી)
		ચાલુ;

	    જો (adding_mode)
	    {
		પૂર્ણાંક str_len = (પૂર્ણાંક)STRLEN(str);
		જો (str_len == 0)
		    ચાલુ;

		જો (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0))
		{
		    જો (ins_compl_add_infercase(str, str_len, p_ic, ખાલી,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    નહીંતર
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		જ્યારે (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    જો (p >= str_end || *p == NUL)
			તોડો;

		    char_u *word_end = find_word_end(p);

		    જો (word_end <= p)
		    {
			જો (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			નહીંતર
			    word_end = p + 1;
		    }

		    જો (word_end > str_end)
			word_end = str_end;

		    પૂર્ણાંક len = (પૂર્ણાંક)(word_end - p);
		    જો (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0)))
		    {
			જો (ins_compl_add_infercase(p, len, p_ic, ખાલી,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    જો (p <= old_p)
		    {
			p = old_p + 1;
			જો (has_mbyte && p < str_end)
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
    સ્થિર પૂર્ણાંક
get_next_completion_match(પૂર્ણાંક type, ins_compl_next_state_T *st, pos_T *ini)
{
    પૂર્ણાંક	found_new_match = FALSE;

    પસંદગી (type)
    {
	સ્થિતિ -1:
	    તોડો;
#ifdef FEAT_FIND_ID
	સ્થિતિ CTRL_X_PATH_PATTERNS:
	સ્થિતિ CTRL_X_PATH_DEFINES:
	    get_next_include_file_completion(type);
	    તોડો;
#endif

	સ્થિતિ CTRL_X_DICTIONARY:
	સ્થિતિ CTRL_X_THESAURUS:
	    get_next_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ખાલી;
	    તોડો;

	સ્થિતિ CTRL_X_TAGS:
	    get_next_tag_completion();
	    તોડો;

	સ્થિતિ CTRL_X_FILES:
	    get_next_filename_completion();
	    તોડો;

	સ્થિતિ CTRL_X_CMDLINE:
	સ્થિતિ CTRL_X_CMDLINE_CTRL_X:
	    get_next_cmdline_completion();
	    તોડો;

#ifdef FEAT_COMPL_FUNC
	સ્થિતિ CTRL_X_FUNCTION:
	    જો (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    નહીંતર
		expand_by_function(type, compl_pattern.string, ખાલી);
	    તોડો;
	સ્થિતિ CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, ખાલી);
	    તોડો;
#endif

	સ્થિતિ CTRL_X_SPELL:
	    get_next_spell_completion(st->first_match_pos.lnum);
	    તોડો;

	સ્થિતિ CTRL_X_REGISTER:
	    get_register_completion();
	    તોડો;

	મૂળભૂત:	// normal ^P/^N and ^X^L
	    found_new_match = get_next_default_completion(st, ini);
	    જો (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    જો (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    પાછા found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    સ્થિર શૂન્ય
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *પઠ = str, *લિખ = str, *p;

    જો (str == ખાલી)
	પાછા;

    જ્યારે (*પઠ)
    {
	જો (*પઠ == '^')
	{
	    p = પઠ + 1;
	    જ્યારે (vim_isdigit(*p))
		p++;
	    જો ((*p == ',' || *p == '\0') && p != પઠ + 1)
	    {
		પઠ = p;
		ચાલુ;
	    }
	    નહીંતર
		*લિખ++ = *પઠ++;
	}
	નહીંતર
	    *લિખ++ = *પઠ++;
    }
    *લિખ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    સ્થિર પૂર્ણાંક
prepare_cpt_compl_funcs(શૂન્ય)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ખાલી;
    પૂર્ણાંક		idx = 0;
    પૂર્ણાંક		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    જો (cpt == ખાલી)
	પાછા FAIL;
    strip_caret_numbers_in_place(cpt);

    માટે (p = cpt; *p;)
    {
	જ્યારે (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	જો (*p == NUL)
	    તોડો;

	cb = get_callback_if_cpt_func(p, idx);
	જો (cb)
	{
	    જો (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		જો (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		નહીંતર
		    startcol = -2;
	    }
	    નહીંતર જો (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	નહીંતર
	    cpt_sources_array[idx].cs_startcol = -3;

	(શૂન્ય)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    પાછા OK;
#endif
    પાછા FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    સ્થિર શૂન્ય
compl_source_start_timer(પૂર્ણાંક source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    જો (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    સ્થિર પૂર્ણાંક
advance_cpt_sources_index_safe(શૂન્ય)
{
    જો (cpt_sources_index >= 0 && cpt_sources_index < cpt_sources_ગણતરી - 1)
    {
	cpt_sources_index++;
	પાછા OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_index_out_of_range_nr), cpt_sources_index);
#endif
    પાછા FAIL;
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
    સ્થિર પૂર્ણાંક
ins_compl_get_exp(pos_T *ini)
{
    સ્થિર ins_compl_next_state_T   st;
    સ્થિર પૂર્ણાંક			    st_cleared = FALSE;
    પૂર્ણાંક		match_ગણતરી;
    પૂર્ણાંક		found_new_match;
    પૂર્ણાંક		type = ctrl_x_mode;
    પૂર્ણાંક		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    પૂર્ણાંક		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    જો (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	જો (!st_cleared)
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
	st.e_cpt = st.e_cpt_copy == ખાલી ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_લીટી_ગણતરી	1000
	જો (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_લીટી_ગણતરી);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    નહીંતર જો (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_લીટી_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != ખાલી;
    જો (normal_mode_strict)
    {
	cpt_sources_index = 0;
	જો (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    માટે (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	જો ((ctrl_x_mode_normal() || ctrl_x_mode_લીટી_or_eval())
					&& (!compl_started || st.found_all))
	{
	    પૂર્ણાંક status = process_next_cpt_કિંમત(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    જો (status == INS_COMPL_CPT_END)
		તોડો;
	    જો (status == INS_COMPL_CPT_CONT)
	    {
		જો (may_advance_cpt_idx)
		{
		    જો (!advance_cpt_sources_index_safe())
			તોડો;
		    compl_source_start_timer(cpt_sources_index);
		}
		ચાલુ;
	    }
	}

	જો (normal_mode_strict && type == CTRL_X_FUNCTION
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
	જો (compl_pattern.string == ખાલી)
	    તોડો;

	જો (may_advance_cpt_idx)
	{
	    જો (!advance_cpt_sources_index_safe())
		તોડો;
	    compl_source_start_timer(cpt_sources_index);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	જો ((ctrl_x_mode_not_default() && !ctrl_x_mode_લીટી_or_eval())
						|| found_new_match != FAIL)
	{
	    જો (got_int)
		તોડો;
	    // Fill the popup menu as soon as possible.
	    જો (type != -1)
		ins_compl_check_ચાવીઓ(0, FALSE);

	    જો ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_લીટી_or_eval()) || compl_interrupted)
		તોડો;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	નહીંતર
	{
	    // Mark a buffer scanned when it has been scanned completely
	    જો (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	જો (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	જો (!compl_dir_forward())
	    જ્યારે (compl_curr_match->cp_prev
		    && !match_at_original_text(compl_curr_match->cp_prev))
		compl_curr_match = compl_curr_match->cp_prev;
    }
    cpt_sources_index = -1;
    compl_started = TRUE;

    જો ((ctrl_x_mode_normal() || ctrl_x_mode_લીટી_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_ગણતરી = -1;		// total of matches, unknown
    જો (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_લીટી_or_eval()))
	match_ગણતરી = ins_compl_make_cyclic();

    જો (cot_fuzzy() && compl_get_longest && compl_અંક_bests > 0)
	fuzzy_longest_match();

    જો (compl_old_match != ખાલી)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_next
						    : compl_old_match->cp_prev;
	જો (compl_curr_match == ખાલી)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    જો (match_ગણતરી > 0 && !ctrl_x_mode_spell())
    {
	જો (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	જો (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    પાછા match_ગણતરી;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    સ્થિર શૂન્ય
ins_compl_update_shown_match(શૂન્ય)
{
    string_T	*leader;

    (શૂન્ય)get_leader_for_startcol(ખાલી, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    જ્યારે (!ins_compl_equal(compl_shown_match,
		leader->string, (પૂર્ણાંક)leader->length)
	    && compl_shown_match->cp_next != ખાલી
	    && !is_first_match(compl_shown_match->cp_next))
    {
	compl_shown_match = compl_shown_match->cp_next;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    જો (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (પૂર્ણાંક)leader->length)
	    && (compl_shown_match->cp_next == ખાલી
		|| is_first_match(compl_shown_match->cp_next)))
    {
	જ્યારે (!ins_compl_equal(compl_shown_match,
		    leader->string, (પૂર્ણાંક)leader->length)
		&& compl_shown_match->cp_prev != ખાલી
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
    શૂન્ય
ins_compl_delete(શૂન્ય)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    પૂર્ણાંક col = compl_col + (compl_status_adding() ? compl_length : 0);
    string_T	remaining = {ખાલી, 0};
    પૂર્ણાંક	    orig_col;

    જો (ins_compl_preinsert_effect())
    {
	col += (પૂર્ણાંક)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    જો (curwin->w_cursor.lnum > compl_lnum)
    {
	જો (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *લીટી = ml_get_cursor();
	    remaining.length = ml_get_cursor_len();
	    remaining.string = vim_strnsave(લીટી, remaining.length);
	    જો (remaining.string == ખાલી)
		પાછા;
	}
	જ્યારે (curwin->w_cursor.lnum > compl_lnum)
	{
	    જો (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		જો (remaining.string)
		    vim_free(remaining.string);
		પાછા;
	    }
	    deleted_લીટીઓ_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    જો ((પૂર્ણાંક)curwin->w_cursor.col > col)
    {
	જો (stop_arrow() == FAIL)
	{
	    જો (remaining.string)
		vim_free(remaining.string);
	    પાછા;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    જો (remaining.string != ખાલી)
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
    સ્થિર શૂન્ય
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    પૂર્ણાંક		base_indent = get_indent();

    જ્યારે (*curr != NUL)
    {
	જો (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    જો (curr > start)
		ins_char_bytes(start, (પૂર્ણાંક)(curr - start));

	    // Handle newline
	    open_લીટી(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, ખાલી);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    જો (curr > start)
	ins_char_bytes(start, (પૂર્ણાંક)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    સ્થિર char_u *
find_common_prefix(size_t *prefix_len, પૂર્ણાંક curbuf_only)
{
    compl_T     *compl;
    પૂર્ણાંક		*match_ગણતરી;
    char_u	*first = ખાલી;
    પૂર્ણાંક		len = -1;
    પૂર્ણાંક		is_cpt_completion = (cpt_sources_array != ખાલી);

    જો (!is_cpt_completion)
	પાછા ખાલી;

    match_ગણતરી = ALLOC_CLEAR_MULT(પૂર્ણાંક, cpt_sources_ગણતરી);
    જો (match_ગણતરી == ખાલી)
	પાછા ખાલી;

    (શૂન્ય)get_leader_for_startcol(ખાલી, TRUE); // Clear the cache

    compl = compl_first_match;
    કરો
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	જો (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	જો (!match_at_original_text(compl)
		&& (leader->string == ખાલી
		    || ins_compl_equal_sc(compl, leader->string,
			(પૂર્ણાંક)leader->length)))
	{
	    // Limit number of items from each source if max_items is set.
	    પૂર્ણાંક match_limit_exceeded = FALSE;
	    પૂર્ણાંક cur_source = compl->cp_cpt_source_idx;

	    જો (cur_source != -1)
	    {
		match_ગણતરી[cur_source]++;
		પૂર્ણાંક max_matches = cpt_sources_array[cur_source].cs_max_matches;
		જો (max_matches > 0 && match_ગણતરી[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    જો (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		જો (first == ખાલી && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (પૂર્ણાંક)STRLEN(first);
		}
		નહીંતર જો (first != ખાલી)
		{
		    પૂર્ણાંક j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    જ્યારે (j < len && *s1 != NUL && *s2 != NUL)
		    {
			જો (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    તોડો;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    જો (len == 0)
			તોડો;
		}
	    }
	}
	compl = compl->cp_next;
    } જ્યારે (compl != ખાલી && !is_first_match(compl));

    vim_free(match_ગણતરી);

    જો (len > (પૂર્ણાંક)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	જો (len == (પૂર્ણાંક)STRLEN(first))
	{
	    char_u *લીટી = ml_get_curline();
	    char_u *p = લીટી + curwin->w_cursor.col;
	    જો (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		પૂર્ણાંક text_len = end - p;
		જો (text_len > 0
			&& text_len < (len - (પૂર્ણાંક)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	પાછા first;
    }
    પાછા ખાલી;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    શૂન્ય
ins_compl_ઉમેરો(પૂર્ણાંક move_cursor, પૂર્ણાંક ઉમેરો_prefix)
{
    પૂર્ણાંક		compl_len = get_compl_len();
    પૂર્ણાંક		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.length;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    જો (ઉમેરો_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	જો (cp_str == ખાલી)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    જો (cp_str == ખાલી)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.length;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    નહીંતર જો (cpt_sources_array != ખાલી)
    {
	પૂર્ણાંક	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	જો (cpt_idx >= 0 && compl_col >= 0)
	{
	    પૂર્ણાંક	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    જો (startcol >= 0 && startcol < (પૂર્ણાંક)compl_col)
	    {
		પૂર્ણાંક skip = (પૂર્ણાંક)compl_col - startcol;
		જો ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    જો (compl_len < (પૂર્ણાંક)cp_str_len)
    {
	જો (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	નહીંતર
	{
	    ins_compl_ઉમેરો_bytes(cp_str + compl_len,
		    ઉમેરો_prefix ? (પૂર્ણાંક)cp_str_len - compl_len : -1);
	    જો ((preinsert || ઉમેરો_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    જો (match_at_original_text(compl_shown_match)
	    || (preinsert && !ઉમેરો_prefix))
	compl_used_match = FALSE;
    નહીંતર
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = ઉમેરો_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    સ્થિર શૂન્ય
ins_compl_show_filename(શૂન્ય)
{
    અક્ષર	*lead = _("match in file");
    પૂર્ણાંક		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    જો (space <= 0)
	પાછા;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    માટે (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	જ્યારે (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    જો (!compl_autocomplete)
    {
	સંદેશ_hist_off = TRUE;
	vim_snprintf((અક્ષર *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	સંદેશ((અક્ષર *)IObuff);
	સંદેશ_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    સ્થિર compl_T *
find_next_match_in_menu(શૂન્ય)
{
    પૂર્ણાંક	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    કરો
	match = is_forward ? match->cp_next : match->cp_prev;
    જ્યારે (match->cp_next && !match->cp_in_match_array
	    && !match_at_original_text(match));
    પાછા match;
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
    સ્થિર પૂર્ણાંક
find_next_completion_match(
	પૂર્ણાંક	allow_get_expansion,
	પૂર્ણાંક	todo,		// repeat completion this many times
	પૂર્ણાંક	advance,
	પૂર્ણાંક	*અંક_matches)
{
    પૂર્ણાંક		found_end = FALSE;
    compl_T	*found_compl = ખાલી;
    unsigned પૂર્ણાંક cur_cot_flags = get_cot_flags();
    પૂર્ણાંક		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    જ્યારે (--todo >= 0)
    {
	જો (compl_shows_dir_forward() && compl_shown_match->cp_next != ખાલી)
	{
	    જો (compl_match_array != ખાલી)
		compl_shown_match = find_next_match_in_menu();
	    નહીંતર
		compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != ખાલી
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	નહીંતર જો (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != ખાલી)
	{
	    found_end = is_first_match(compl_shown_match);
	    જો (compl_match_array != ખાલી)
		compl_shown_match = find_next_match_in_menu();
	    નહીંતર
		compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	નહીંતર
	{
	    જો (!allow_get_expansion)
	    {
		જો (advance)
		{
		    જો (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    નહીંતર
			compl_pending += todo + 1;
		}
		પાછા -1;
	    }

	    જો (!compl_no_select && advance)
	    {
		જો (compl_shows_dir_backward())
		    --compl_pending;
		નહીંતર
		    ++compl_pending;
	    }

	    // Find matches.
	    *અંક_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    જ્યારે (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		જો (compl_pending > 0 && compl_shown_match->cp_next != ખાલી)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		નહીંતર જો (compl_pending < 0 && compl_shown_match->cp_prev != ખાલી)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		નહીંતર
		    તોડો;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	જો (!match_at_original_text(compl_shown_match)
		&& leader->string != ખાલી
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (પૂર્ણાંક)leader->length)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	નહીંતર
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	જો (found_end)
	{
	    જો (found_compl != ખાલી)
	    {
		compl_shown_match = found_compl;
		તોડો;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
ins_compl_next(
    પૂર્ણાંક	    allow_get_expansion,
    પૂર્ણાંક	    ગણતરી,		// repeat completion this many times; should
				// be at least 1
    પૂર્ણાંક	    ઉમેરો_match)	// Insert the newly selected match
{
    પૂર્ણાંક	    અંક_matches = -1;
    પૂર્ણાંક	    todo = ગણતરી;
    પૂર્ણાંક	    advance;
    પૂર્ણાંક	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned પૂર્ણાંક cur_cot_flags = get_cot_flags();
    પૂર્ણાંક	    compl_no_ઉમેરો = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    પૂર્ણાંક	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    જો (compl_shown_match == ખાલી)
	પાછા -1;

    જો (compl_leader.string != ખાલી
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    જો (allow_get_expansion && ઉમેરો_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = ગણતરી != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    જો (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    જો (find_next_completion_match(allow_get_expansion, todo, advance,
							&અંક_matches) == -1)
	પાછા -1;

    જો (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	પાછા -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    જો (!started && ins_compl_preinsert_longest())
	ins_compl_ઉમેરો(TRUE, TRUE);
    નહીંતર જો (compl_no_ઉમેરો && !started && !compl_preinsert)
    {
	ins_compl_ઉમેરો_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    નહીંતર જો (ઉમેરો_match)
    {
	જો (!compl_get_longest || compl_used_match)
	{
	    પૂર્ણાંક preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_ઉમેરો(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	નહીંતર
	    ins_compl_ઉમેરો_bytes(compl_leader.string + get_compl_len(), -1);
    }
    નહીંતર
	compl_used_match = FALSE;

    જો (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	જો (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	નહીંતર
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	જો (gui.in_use)
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
    જો (compl_no_ઉમેરો && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    નહીંતર
	compl_enter_selects = !ઉમેરો_match && compl_match_array != ખાલી;

    // Show the file name for the match (if any)
    જો (compl_shown_match->cp_fname != ખાલી)
	ins_compl_show_filename();

    પાછા અંક_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    સ્થિર શૂન્ય
check_elapsed_time(શૂન્ય)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_index].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    જો (elapsed_ms > compl_timeout_ms)
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
    શૂન્ય
ins_compl_check_ચાવીઓ(પૂર્ણાંક frequency, પૂર્ણાંક in_compl_func)
{
    સ્થિર પૂર્ણાંક	ગણતરી = 0;
    પૂર્ણાંક		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    જો (!in_compl_func && (using_script() || ex_normal_busy))
	પાછા;

    // Only do this at regular intervals
    જો (++ગણતરી < frequency)
	પાછા;
    ગણતરી = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    જો (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	જો (vim_is_ctrl_x_ચાવી(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (શૂન્ય)ins_compl_next(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	નહીંતર
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    જો (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		જો (c != Ctrl_R && (ચાવી_Typed
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    નહીંતર
    {
	પૂર્ણાંક normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_લીટી_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != ખાલી && cpt_sources_index >= 0;
	જો (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    જો (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	પૂર્ણાંક todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(શૂન્ય)ins_compl_next(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    સ્થિર પૂર્ણાંક
ins_compl_key2dir(પૂર્ણાંક c)
{
    જો (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	પાછા BACKWARD;
    પાછા FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    સ્થિર પૂર્ણાંક
ins_compl_pum_ચાવી(પૂર્ણાંક c)
{
    પાછા pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    સ્થિર પૂર્ણાંક
ins_compl_key2count(પૂર્ણાંક c)
{
    જો (ins_compl_pum_ચાવી(c) && c != K_UP && c != K_DOWN)
    {
	પૂર્ણાંક h = pum_get_height();
	જો (h > 3)
	    h -= 2; // keep some context
	પાછા h;
    }
    પાછા 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    સ્થિર પૂર્ણાંક
ins_compl_use_match(પૂર્ણાંક c)
{
    પસંદગી (c)
    {
	સ્થિતિ K_UP:
	સ્થિતિ K_DOWN:
	સ્થિતિ K_PAGEDOWN:
	સ્થિતિ K_KPAGEDOWN:
	સ્થિતિ K_S_DOWN:
	સ્થિતિ K_PAGEUP:
	સ્થિતિ K_KPAGEUP:
	સ્થિતિ K_S_UP:
	    પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    સ્થિર પૂર્ણાંક
get_normal_compl_info(char_u *લીટી, પૂર્ણાંક startcol, colnr_T curs_col)
{
    જો ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	જો (!compl_status_adding())
	{
	    જ્યારે (--startcol >= 0 && vim_isIDc(લીટી[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	જો (p_ic)
	{
	    compl_pattern.string = str_foldcase(લીટી + compl_col,
		    compl_length, ખાલી, 0);
	    જો (compl_pattern.string == ખાલી)
	    {
		compl_pattern.length = 0;
		પાછા FAIL;
	    }
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	નહીંતર
	{
	    compl_pattern.string = vim_strnsave(લીટી + compl_col, (size_t)compl_length);
	    જો (compl_pattern.string == ખાલી)
	    {
		compl_pattern.length = 0;
		પાછા FAIL;
	    }
	    compl_pattern.length = (size_t)compl_length;
	}
    }
    નહીંતર જો (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	જો (!vim_iswordp(લીટી + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(લીટી, લીટી + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ખાલી, લીટી + compl_col, compl_length) + prefixlen;
	compl_pattern.string = alloc(n);
	જો (compl_pattern.string == ખાલી)
	{
	    compl_pattern.length = 0;
	    પાછા FAIL;
	}
	STRCPY((અક્ષર *)compl_pattern.string, prefix);
	(શૂન્ય)quote_meta(compl_pattern.string + prefixlen,
		લીટી + compl_col, compl_length);
	compl_pattern.length = n - 1;
    }
    નહીંતર જો (--startcol < 0
	    || !vim_iswordp(mb_prevptr(લીટી, લીટી + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	જો (compl_pattern.string == ખાલી)
	{
	    compl_pattern.length = 0;
	    પાછા FAIL;
	}
	compl_pattern.length = len;
	compl_col += curs_col;
	compl_length = 0;
	compl_from_nonkeyword = TRUE;
    }
    નહીંતર
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	જો (has_mbyte)
	{
	    પૂર્ણાંક base_class;
	    પૂર્ણાંક head_off;

	    startcol -= (*mb_head_off)(લીટી, લીટી + startcol);
	    base_class = mb_get_class(લીટી + startcol);
	    જ્યારે (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(લીટી, લીટી + startcol);
		જો (base_class != mb_get_class(લીટી + startcol
			    - head_off))
		    તોડો;
		startcol -= head_off;
	    }
	}
	નહીંતર
	{
	    જ્યારે (--startcol >= 0 && vim_iswordc(લીટી[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_length = (પૂર્ણાંક)curs_col - startcol;
	જો (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    જો (compl_pattern.string == ખાલી)
	    {
		compl_pattern.length = 0;
		પાછા FAIL;
	    }
	    STRCPY((અક્ષર *)compl_pattern.string, "\\<");
	    (શૂન્ય)quote_meta(compl_pattern.string + 2, લીટી + compl_col, 1);
	    STRCAT((અક્ષર *)compl_pattern.string, "\\k");
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	નહીંતર
	{
	    size_t  n = quote_meta(ખાલી, લીટી + compl_col, compl_length) + 2;

	    compl_pattern.string = alloc(n);
	    જો (compl_pattern.string == ખાલી)
	    {
		compl_pattern.length = 0;
		પાછા FAIL;
	    }
	    STRCPY((અક્ષર *)compl_pattern.string, "\\<");
	    (શૂન્ય)quote_meta(compl_pattern.string + 2, લીટી + compl_col,
		    compl_length);
	    compl_pattern.length = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    જો (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	જો (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    સ્થિર પૂર્ણાંક
get_wholeline_compl_info(char_u *લીટી, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(લીટી);
    compl_length = (પૂર્ણાંક)curs_col - (પૂર્ણાંક)compl_col;
    જો (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    જો (p_ic)
    {
	compl_pattern.string = str_foldcase(લીટી + compl_col, compl_length,
		ખાલી, 0);
	જો (compl_pattern.string == ખાલી)
	{
	    compl_pattern.length = 0;
	    પાછા FAIL;
	}
	compl_pattern.length = STRLEN(compl_pattern.string);
    }
    નહીંતર
    {
	compl_pattern.string = vim_strnsave(લીટી + compl_col, (size_t)compl_length);
	જો (compl_pattern.string == ખાલી)
	{
	    compl_pattern.length = 0;
	    પાછા FAIL;
	}
	compl_pattern.length = (size_t)compl_length;
    }

    પાછા OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    સ્થિર પૂર્ણાંક
get_filename_compl_info(char_u *લીટી, પૂર્ણાંક startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    જો (startcol > 0)
    {
	char_u	*p = લીટી + startcol;

	MB_PTR_BACK(લીટી, p);
	જ્યારે (p > લીટી && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(લીટી, p);
	જો (p == લીટી && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	નહીંતર
	    startcol = (પૂર્ણાંક)(p - લીટી) + 1;
    }

    compl_col += startcol;
    compl_length = (પૂર્ણાંક)curs_col - startcol;
    compl_pattern.string = addstar(લીટી + compl_col, compl_length, EXPAND_FILES);
    જો (compl_pattern.string == ખાલી)
    {
	compl_pattern.length = 0;
	પાછા FAIL;
    }

    compl_pattern.length = STRLEN(compl_pattern.string);

    પાછા OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    સ્થિર પૂર્ણાંક
get_cmdline_compl_info(char_u *લીટી, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(લીટી, curs_col);
    જો (compl_pattern.string == ખાલી)
    {
	compl_pattern.length = 0;
	પાછા FAIL;
    }
    compl_pattern.length = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (પૂર્ણાંક)compl_pattern.length, curs_col, FALSE);
    જો (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    નહીંતર
	compl_col = (પૂર્ણાંક)(compl_xp.xp_pattern - compl_pattern.string);
    compl_length = curs_col - compl_col;

    પાછા OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    સ્થિર પૂર્ણાંક
set_compl_globals(
    પૂર્ણાંક		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    પૂર્ણાંક		is_cpt_compl UNUSED)
{
    જો (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	જો (startcol < compl_col)
	    પાછા prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	નહીંતર
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.length);
	    cpt_compl_pattern.length = compl_orig_text.length;
	}
    }
    નહીંતર
    {
	જો (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*લીટી = ml_get(curwin->w_cursor.lnum);
	પૂર્ણાંક	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(લીટી + startcol, (size_t)len);
	જો (compl_pattern.string == ખાલી)
	{
	    compl_pattern.length = 0;
	    પાછા FAIL;
	}
	compl_pattern.length = (size_t)len;
	compl_col = startcol;
	compl_length = len;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    પૂર્ણાંક		*startcol UNUSED)
{
    પૂર્ણાંક		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    પૂર્ણાંક		col;
    char_u	*funcname = ખાલી;
    pos_T	pos;
    પૂર્ણાંક		save_State = State;
    પૂર્ણાંક		is_cpt_function = (cb != ખાલી);
    પૂર્ણાંક		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    જો (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	જો (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    પાછા FAIL;
	}
	cb = get_ઉમેરો_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_સંખ્યા;
    args[0].vval.v_સંખ્યા = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    જો (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    જો (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    જો (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	પાછા FAIL;
    }

    જો (startcol != ખાલી)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    જો (col == -2 || aborting())
	પાછા FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    જો (col == -3)
    {
	જો (is_cpt_function)
	    પાછા FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = ખાલી;
	જો (!shortmess(SHM_COMPLETIONMENU))
	    સંદેશ_clr_cmdline();
	પાછા FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    પાછા ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    સ્થિર પૂર્ણાંક
get_spell_compl_info(પૂર્ણાંક startcol UNUSED, colnr_T curs_col UNUSED)
{
    પૂર્ણાંક		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*લીટી = ખાલી;

    જો (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    નહીંતર
	compl_col = spell_word_start(startcol);

    જો (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    નહીંતર
    {
	spell_expand_check_cap(compl_col);
	compl_length = (પૂર્ણાંક)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    લીટી = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(લીટી + compl_col, (size_t)compl_length);
    જો (compl_pattern.string == ખાલી)
    {
	compl_pattern.length = 0;
	પાછા FAIL;
    }

    compl_pattern.length = (size_t)compl_length;
    ret = OK;
#endif

    પાછા ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    સ્થિર પૂર્ણાંક
compl_get_info(char_u *લીટી, પૂર્ણાંક startcol, colnr_T curs_col, પૂર્ણાંક *લીટી_invalid)
{
    જો (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	જો (get_normal_compl_info(લીટી, startcol, curs_col) != OK)
	    પાછા FAIL;
	*લીટી_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    નહીંતર જો (ctrl_x_mode_લીટી_or_eval())
    {
	પાછા get_wholeline_compl_info(લીટી, curs_col);
    }
    નહીંતર જો (ctrl_x_mode_files())
    {
	પાછા get_filename_compl_info(લીટી, startcol, curs_col);
    }
    નહીંતર જો (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	પાછા get_cmdline_compl_info(લીટી, curs_col);
    }
    નહીંતર જો (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	જો (get_userdefined_compl_info(curs_col, ખાલી, ખાલી) != OK)
	    પાછા FAIL;
	*લીટી_invalid = TRUE;	// "line" may have become invalid
    }
    નહીંતર જો (ctrl_x_mode_spell())
    {
	જો (get_spell_compl_info(startcol, curs_col) == FAIL)
	    પાછા FAIL;
	*લીટી_invalid = TRUE;	// "line" may have become invalid
    }
    નહીંતર
    {
	internal_error("ins_complete()");
	પાછા FAIL;
    }

    પાછા OK;
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
    સ્થિર શૂન્ય
ins_compl_continue_શોધ(char_u *લીટી)
{
    // it is a continued search
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    જો (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	જો (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(લીટી);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	નહીંતર
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    જો (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    લીટી + compl_length
			    + compl_startpos.col) - લીટી);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_length = curwin->w_cursor.col - (પૂર્ણાંક)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	જો (compl_length > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_length = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_length;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	જો (compl_length < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    નહીંતર જો (ctrl_x_mode_લીટી_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    નહીંતર
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    સ્થિર પૂર્ણાંક
ins_compl_start(શૂન્ય)
{
    char_u	*લીટી = ખાલી;
    પૂર્ણાંક		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    પૂર્ણાંક		લીટી_invalid = FALSE;
    પૂર્ણાંક		save_did_ai = did_ai;
    પૂર્ણાંક		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    જો (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	પાછા FAIL;
    }

    લીટી = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    જો ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_શોધ(લીટી);
    નહીંતર
	compl_cont_status &= CONT_LOCAL;

    જો (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	જો (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (પૂર્ણાંક)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    જો (compl_get_info(લીટી, startcol, curs_col, &લીટી_invalid) == FAIL)
    {
	જો (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	પાછા FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    જો (લીટી_invalid)
	લીટી = ml_get(curwin->w_cursor.lnum);

    જો (compl_status_adding())
    {
	જો (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	જો (ctrl_x_mode_લીટી_or_eval())
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
    નહીંતર
    {
	edit_submode_pre = ખાલી;
	compl_startpos.col = compl_col;
    }

    જો (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	જો (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_સંદેશ]);
	નહીંતર
	    edit_submode = (char_u *)_(CTRL_X_સંદેશ(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ખાલી);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.length = (size_t)compl_length;
    compl_orig_text.string = vim_strnsave(લીટી + compl_col, (size_t)compl_length);
    જો (p_ic)
	flags |= CP_ICASE;
    જો (compl_orig_text.string == ખાલી
	    || ins_compl_add(compl_orig_text.string,
		(પૂર્ણાંક)compl_orig_text.length,
		ખાલી, ખાલી, ખાલી, 0, flags, FALSE, ખાલી, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	પાછા FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    જો (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_ગણતરી;
	showmode();
	edit_submode_extra = ખાલી;
	out_flush();
    }

    did_ai = save_did_ai;
    પાછા OK;
}

/*
 * display the completion status message
 */
    સ્થિર શૂન્ય
ins_compl_show_statusmsg(શૂન્ય)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    જો (is_first_match(compl_first_match->cp_next))
    {
	edit_submode_extra = compl_status_adding() && compl_length > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    જો (edit_submode_extra == ખાલી)
    {
	જો (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	નહીંતર જો (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_ગણતરી;
	}
	નહીંતર જો (compl_curr_match->cp_next == compl_curr_match->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_ગણતરી;
	    compl_curr_match->cp_સંખ્યા = 1;
	}
	નહીંતર
	{
#જો defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    જો (compl_curr_match->cp_સંખ્યા == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    જો (compl_curr_match->cp_સંખ્યા != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		સ્થિર char_u match_ref[81];

		જો (compl_matches > 0)
		    vim_snprintf((અક્ષર *)match_ref, કદ(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_સંખ્યા, compl_matches);
		નહીંતર
		    vim_snprintf((અક્ષર *)match_ref, કદ(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_સંખ્યા);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		જો (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    જો (!compl_opt_suppress_empty)
    {
	showmode();
	જો (!shortmess(SHM_COMPLETIONMENU))
	{
	    જો (edit_submode_extra != ખાલી)
	    {
		જો (!p_smd)
		{
		    સંદેશ_hist_off = TRUE;
		    સંદેશ_attr((અક્ષર *)edit_submode_extra,
			    edit_submode_highl < HLF_ગણતરી
			    ? HL_ATTR(edit_submode_highl) : 0);
		    સંદેશ_hist_off = FALSE;
		}
	    }
	    નહીંતર
		સંદેશ_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    પૂર્ણાંક
ins_complete(પૂર્ણાંક c, પૂર્ણાંક enable_pum)
{
    પૂર્ણાંક		n;
    પૂર્ણાંક		save_w_wrow;
    પૂર્ણાંક		save_w_leftcol;
    પૂર્ણાંક		ઉમેરો_match;
    પૂર્ણાંક		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    ઉમેરો_match = ins_compl_use_match(c);

    જો (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	જો (ins_compl_start() == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (ઉમેરો_match && stop_arrow() == FAIL)
	પાછા FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_buffer;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_અંક_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), ઉમેરો_match);

    // Reset autocompletion timer expiry flag
    જો (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    જો (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    જો (got_int && !global_busy)
    {
	(શૂન્ય)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_next);
    જો (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	જો (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    જો (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    નહીંતર
	compl_cont_status &= ~CONT_S_IPOS;

    જો (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    જો (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    પાછા OK;
}

/*
 * Enable autocompletion
 */
    શૂન્ય
ins_compl_enable_autocomplete(શૂન્ય)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
}

/*
 * Disable autocompletion
 */
    શૂન્ય
ins_compl_disable_autocomplete(શૂન્ય)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    શૂન્ય
ins_compl_arm_autostart(શૂન્ય)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    શૂન્ય
ins_compl_disarm_autostart(શૂન્ય)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(શૂન્ય)
{
#ifdef ELAPSED_FUNC
    જો (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	પાછા true;
    }
#endif
    પાછા false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    શૂન્ય
ins_compl_clear_autocomplete_delay(શૂન્ય)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(શૂન્ય)
{
    પાછા compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(શૂન્ય)
{
#ifdef ELAPSED_FUNC
    પાછા ELAPSED_FUNC(compl_autocomplete_start_tv);
#નહીંતર
    પાછા 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    સ્થિર શૂન્ય
show_pum(પૂર્ણાંક prev_w_wrow, પૂર્ણાંક prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    જો (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    સ્થિર unsigned
quote_meta(char_u *dest, char_u *src, પૂર્ણાંક len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    માટે ( ; --len >= 0; src++)
    {
	પસંદગી (*src)
	{
	    સ્થિતિ '.':
	    સ્થિતિ '*':
	    સ્થિતિ '[':
		જો (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    તોડો;
		// FALLTHROUGH
	    સ્થિતિ '~':
		જો (!magic_isset())	// quote these only if magic is set
		    તોડો;
		// FALLTHROUGH
	    સ્થિતિ '\\':
		જો (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    તોડો;
		// FALLTHROUGH
	    સ્થિતિ '^':		// currently it's not needed.
	    સ્થિતિ '$':
		m++;
		જો (dest != ખાલી)
		    *dest++ = '\\';
		તોડો;
	}
	જો (dest != ખાલી)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	જો (has_mbyte)
	{
	    પૂર્ણાંક mb_len = (*mb_ptr2len)(src) - 1;
	    જો (mb_len > 0 && len >= mb_len)
		માટે (પૂર્ણાંક i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    જો (dest != ખાલી)
			*dest++ = *src;
		}
	}
    }
    જો (dest != ખાલી)
	*dest = NUL;

    પાછા m;
}

#જો defined(EXITFREE)
    શૂન્ય
free_insexpand_stuff(શૂન્ય)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_ગણતરી);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    સ્થિર શૂન્ય
spell_back_to_badword(શૂન્ય)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, ખાલી);
    જો (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    સ્થિર શૂન્ય
cpt_sources_clear(શૂન્ય)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_index = -1;
    cpt_sources_ગણતરી = 0;
}

/*
 * Setup completion sources.
 */
    સ્થિર પૂર્ણાંક
setup_cpt_sources(શૂન્ય)
{
    char_u  buf[LSIZE];
    પૂર્ણાંક	    slen;
    પૂર્ણાંક	    idx = 0;
    પૂર્ણાંક	    ગણતરી;
    char_u  *p;

    cpt_sources_clear();

    ગણતરી = get_cpt_sources_ગણતરી(curbuf->b_p_cpt);
    જો (ગણતરી == 0)
	પાછા OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, ગણતરી);
    જો (cpt_sources_array == ખાલી)
	પાછા FAIL;

    માટે (p = curbuf->b_p_cpt; *p;)
    {
	જ્યારે (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	જો (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    જો (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		જો (caret != ખાલી)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((અક્ષર *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_ગણતરી = ગણતરી;
    પાછા OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    સ્થિર પૂર્ણાંક
is_cpt_func_refresh_always(શૂન્ય)
{
#ifdef FEAT_COMPL_FUNC
    માટે (પૂર્ણાંક i = 0; i < cpt_sources_ગણતરી; i++)
	જો (cpt_sources_array[i].cs_refresh_always)
	    પાછા TRUE;
#endif
    પાછા FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    સ્થિર શૂન્ય
ins_compl_make_linear(શૂન્ય)
{
    compl_T *m;

    જો (compl_first_match == ખાલી || compl_first_match->cp_prev == ખાલી)
	પાછા;
    m = compl_first_match->cp_prev;
    m->cp_next = ખાલી;
    compl_first_match->cp_prev = ખાલી;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    સ્થિર શૂન્ય
remove_old_matches(શૂન્ય)
{
    compl_T *current;
    પૂર્ણાંક	    shown_match_removed = FALSE;
    પૂર્ણાંક	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    જો (cpt_sources_index < 0)
	પાછા;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    માટે (current = compl_first_match; current != ખાલી; )
    {
	જો (current->cp_cpt_source_idx == cpt_sources_index)
	{
	    compl_T *to_delete = current;

	    જો (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_next;

	    જો (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_next;
		compl_first_match->cp_prev = ખાલી;
	    }
	    નહીંતર જો (to_delete->cp_next == ખાલી) // node to remove is at tail
		to_delete->cp_prev->cp_next = ખાલી;
	    નહીંતર // node is in the moddle
	    {
		to_delete->cp_prev->cp_next = to_delete->cp_next;
		to_delete->cp_next->cp_prev = to_delete->cp_prev;
	    }
	    ins_compl_item_free(to_delete);
	}
	નહીંતર
	    current = current->cp_next;
    }

    // Re-assign compl_shown_match if necessary
    જો (shown_match_removed)
    {
	જો (forward)
	    compl_shown_match = compl_first_match;
	નહીંતર
	{  // Last node will have the prefix that is being completed
	    માટે (current = compl_first_match; current->cp_next != ખાલી;
		    current = current->cp_next)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    માટે (current = compl_first_match; current != ખાલી; )
    {
	જો ((forward ? current->cp_cpt_source_idx < cpt_sources_index
		    : current->cp_cpt_source_idx > cpt_sources_index))
	{
	    compl_curr_match = forward ? current : current->cp_next;
	    current = current->cp_next;
	}
	નહીંતર
	    તોડો;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    સ્થિર શૂન્ય
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_index];
    પૂર્ણાંક		    startcol = cpt_src->cs_startcol;

    જો (startcol == -2 || startcol == -3)
	પાછા;

    જો (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	જો (!cpt_src->cs_refresh_always)
	    ins_compl_ઉમેરો_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	જો (!cpt_src->cs_refresh_always)
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
    સ્થિર શૂન્ય
cpt_compl_refresh(શૂન્ય)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ખાલી;
    પૂર્ણાંક		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_index = 0;
    માટે (p = cpt; *p;)
    {
	જ્યારે (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	જો (*p == NUL)
	    તોડો;

	જો (cpt_sources_array[cpt_sources_index].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_index);
	    જો (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		જો (ret == FAIL)
		{
		    જો (startcol == -3)
			cpt_sources_array[cpt_sources_index].cs_refresh_always
			    = FALSE;
		    નહીંતર
			startcol = -2;
		}
		નહીંતર જો (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_index].cs_startcol = startcol;
		જો (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_index);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(શૂન્ય)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	જો (may_advance_cpt_index(p))
	    (શૂન્ય)advance_cpt_sources_index_safe();
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
    શૂન્ય
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    જો (ins_compl_preinsert_effect())
	rettv->vval.v_સંખ્યા = 1;
}
