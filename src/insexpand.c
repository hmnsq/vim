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

#অন্তর্ভাব "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_সারি	3
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
#define CTRL_X_LOCAL_বার্তা	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_বার্তা(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
স্থির অক্ষর *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ফাঁকা, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ফাঁকা, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ফাঁকা,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#যদি defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
স্থির string_T ctrl_x_mode_names[] = {
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
    {ফাঁকা, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
প্রকারলেখ কাঠামো compl_S compl_T;
কাঠামো compl_S
{
    compl_T	*cp_পরবর্তী;
    compl_T	*cp_পূর্ব;
    compl_T	*cp_match_পরবর্তী;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_গুনতি]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_তথ্য;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    পূর্ণসংখ্যা		cp_flags;		// CP_ values
    পূর্ণসংখ্যা		cp_সংখ্যা;		// sequence number
    পূর্ণসংখ্যা		cp_order;		// list position when first scored
    পূর্ণসংখ্যা		cp_score;		// fuzzy match score or proximity score
    পূর্ণসংখ্যা		cp_in_match_array;	// collected by compl_match_array
    পূর্ণসংখ্যা		cp_user_abbr_hl_id;	// highlight group ID for abbr
    পূর্ণসংখ্যা		cp_user_kind_hl_id;	// highlight group ID for kind
    পূর্ণসংখ্যা		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
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
স্থির compl_T    *compl_first_match = ফাঁকা;
স্থির compl_T    *compl_curr_match = ফাঁকা;
স্থির compl_T    *compl_shown_match = ফাঁকা;
স্থির compl_T    *compl_old_match = ফাঁকা;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা	    cse_গুনতি;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((পূর্ণসংখ্যা)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_চাবি - CSE_OFF))

স্থির hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    স্থির শূন্য
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    যদি (compl_strings_ht.ht_array == ফাঁকা)
	hash_init(&compl_strings_ht);
    যদি (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    যদি (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	যদি (entry == ফাঁকা)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    ফেরত;
	entry->cse_গুনতি = 1;
	vim_strncpy(entry->cse_str, str, len);
	যদি (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    অন্যথা
	++HI2CSE(hi)->cse_গুনতি;
}

// list used to store the compl_T which have the max score
স্থির compl_T	  **compl_best_matches = ফাঁকা;
স্থির পূর্ণসংখ্যা	  compl_অঙ্ক_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
স্থির পূর্ণসংখ্যা	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
স্থির string_T	  compl_leader = {ফাঁকা, 0};

স্থির পূর্ণসংখ্যা	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
স্থির পূর্ণসংখ্যা	  compl_used_match;

// didn't finish finding completions.
স্থির পূর্ণসংখ্যা	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
স্থির পূর্ণসংখ্যা	  compl_interrupted = FALSE;

স্থির পূর্ণসংখ্যা	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
স্থির পূর্ণসংখ্যা	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
স্থির পূর্ণসংখ্যা	  ctrl_x_mode = CTRL_X_NORMAL;

স্থির পূর্ণসংখ্যা	  compl_matches = 0;	    // number of completion matches
স্থির string_T	  compl_pattern = {ফাঁকা, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
স্থির string_T	  cpt_compl_pattern = {ফাঁকা, 0}; // pattern returned by func in 'cpt'
#endif
স্থির পূর্ণসংখ্যা	  compl_direction = FORWARD;
স্থির পূর্ণসংখ্যা	  compl_shows_dir = FORWARD;
স্থির পূর্ণসংখ্যা	  compl_pending = 0;	    // > 1 for postponed CTRL-N
স্থির pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
স্থির পূর্ণসংখ্যা	  compl_দৈর্ঘ্য = 0;
স্থির linenr_T	  compl_lnum = 0;           // lnum where the completion start
স্থির colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
স্থির colnr_T	  compl_ins_end_col = 0;
স্থির colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
স্থির string_T	  compl_orig_text = {ফাঁকা, 0};  // text as it was before
					    // completion started
স্থির পূর্ণসংখ্যা	  compl_cont_mode = 0;
স্থির expand_T	  compl_xp;

স্থির win_T	  *compl_curr_win = ফাঁকা;  // win where completion is active
স্থির buf_T	  *compl_curr_buf = ফাঁকা;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
স্থির পূর্ণসংখ্যা	  compl_autocomplete = FALSE;	    // whether autocompletion is active
স্থির bool	  compl_autostarted = false;	    // Vim started this completion
স্থির bool	  compl_autostart_pending = false;  // the trigger armed one
স্থির bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
স্থির elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
স্থির পূর্ণসংখ্যা	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
স্থির পূর্ণসংখ্যা	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
স্থির পূর্ণসংখ্যা	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
স্থির পূর্ণসংখ্যা	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    করো { \
	যদি (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } যখন (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
স্থির পূর্ণসংখ্যা	  compl_cont_status = 0;
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

স্থির পূর্ণসংখ্যা	  compl_opt_refresh_always = FALSE;
স্থির পূর্ণসংখ্যা	  compl_opt_suppress_empty = FALSE;

স্থির পূর্ণসংখ্যা	  compl_selected_item = -1;

স্থির পূর্ণসংখ্যা	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
প্রকারলেখ কাঠামো cpt_source_T
{
    পূর্ণসংখ্যা	cs_refresh_always;  // Whether 'refresh:always' is set for func
    পূর্ণসংখ্যা	cs_startcol;	    // Start column returned by func
    পূর্ণসংখ্যা	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

স্থির cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
স্থির পূর্ণসংখ্যা	    cpt_sources_গুনতি;  // Total number of completion sources specified in the 'cpt' option
স্থির পূর্ণসংখ্যা	    cpt_sources_সূচক = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
স্থির pumitem_T *compl_match_array = ফাঁকা;
স্থির পূর্ণসংখ্যা compl_match_arraysize;

স্থির পূর্ণসংখ্যা ins_compl_add(char_u *str, পূর্ণসংখ্যা len, char_u *fname, char_u **cptext, typval_T *user_তথ্য, পূর্ণসংখ্যা cdir, পূর্ণসংখ্যা flags, পূর্ণসংখ্যা adup, পূর্ণসংখ্যা *user_hl, পূর্ণসংখ্যা score);
স্থির শূন্য ins_compl_longest_match(compl_T *match);
স্থির শূন্য ins_compl_del_pum(শূন্য);
স্থির শূন্য ins_compl_files(পূর্ণসংখ্যা গুনতি, char_u **files, পূর্ণসংখ্যা thesaurus, পূর্ণসংখ্যা flags, regmatch_T *regmatch, char_u *buf, পূর্ণসংখ্যা *dir);
স্থির শূন্য ins_compl_free(শূন্য);
স্থির পূর্ণসংখ্যা  ins_compl_need_restart(শূন্য);
স্থির শূন্য ins_compl_new_leader(শূন্য);
স্থির পূর্ণসংখ্যা  get_compl_len(শূন্য);
স্থির শূন্য ins_compl_restart(শূন্য);
স্থির শূন্য ins_compl_set_original_text(char_u *str, size_t len);
স্থির শূন্য ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#যদি defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
স্থির শূন্য ins_compl_add_list(list_T *list);
স্থির শূন্য ins_compl_add_dict(dict_T *dict);
স্থির পূর্ণসংখ্যা get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, পূর্ণসংখ্যা *startcol);
স্থির শূন্য get_cpt_func_completion_matches(callback_T *cb);
স্থির callback_T *get_callback_if_cpt_func(char_u *p, পূর্ণসংখ্যা idx);
স্থির শূন্য fill_complete_info_dict(dict_T *di, compl_T *match, পূর্ণসংখ্যা add_match);
#endif
স্থির পূর্ণসংখ্যা setup_cpt_sources(শূন্য);
স্থির পূর্ণসংখ্যা is_cpt_func_refresh_always(শূন্য);
স্থির শূন্য cpt_sources_clear(শূন্য);
স্থির শূন্য cpt_compl_refresh(শূন্য);
স্থির পূর্ণসংখ্যা  ins_compl_key2dir(পূর্ণসংখ্যা c);
স্থির পূর্ণসংখ্যা  ins_compl_pum_চাবি(পূর্ণসংখ্যা c);
স্থির পূর্ণসংখ্যা  ins_compl_key2count(পূর্ণসংখ্যা c);
স্থির শূন্য show_pum(পূর্ণসংখ্যা পূর্ব_w_wrow, পূর্ণসংখ্যা পূর্ব_w_leftcol);
স্থির unsigned  quote_meta(char_u *dest, char_u *str, পূর্ণসংখ্যা len);
স্থির পূর্ণসংখ্যা ins_compl_has_multiple(শূন্য);
স্থির শূন্য ins_compl_expand_multiple(char_u *str);
স্থির শূন্য ins_compl_longest_ঢোকাও(char_u *prefix);
স্থির শূন্য ins_compl_make_linear(শূন্য);
স্থির পূর্ণসংখ্যা ins_compl_make_cyclic(শূন্য);

#ifdef FEAT_SPELL
স্থির শূন্য spell_back_to_badword(শূন্য);
স্থির পূর্ণসংখ্যা  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    শূন্য
ins_ctrl_x(শূন্য)
{
    যদি (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	যদি (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	অন্যথা
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_বার্তা(ctrl_x_mode));
	edit_submode_pre = ফাঁকা;
	showmode();
    }
    অন্যথা
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
পূর্ণসংখ্যা ctrl_x_mode_none(শূন্য)
    { ফেরত ctrl_x_mode == 0; }
পূর্ণসংখ্যা ctrl_x_mode_normal(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_NORMAL; }
পূর্ণসংখ্যা ctrl_x_mode_scroll(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_SCROLL; }
পূর্ণসংখ্যা ctrl_x_mode_whole_সারি(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_WHOLE_সারি; }
পূর্ণসংখ্যা ctrl_x_mode_files(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_FILES; }
পূর্ণসংখ্যা ctrl_x_mode_tags(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_TAGS; }
পূর্ণসংখ্যা ctrl_x_mode_path_patterns(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
পূর্ণসংখ্যা ctrl_x_mode_path_defines(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_PATH_DEFINES; }
পূর্ণসংখ্যা ctrl_x_mode_dictionary(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_DICTIONARY; }
পূর্ণসংখ্যা ctrl_x_mode_thesaurus(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_THESAURUS; }
পূর্ণসংখ্যা ctrl_x_mode_cmdline(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
পূর্ণসংখ্যা ctrl_x_mode_function(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_FUNCTION; }
পূর্ণসংখ্যা ctrl_x_mode_omni(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_OMNI; }
পূর্ণসংখ্যা ctrl_x_mode_spell(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_SPELL; }
পূর্ণসংখ্যা ctrl_x_mode_eval(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_EVAL; }
পূর্ণসংখ্যা ctrl_x_mode_সারি_or_eval(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_WHOLE_সারি || ctrl_x_mode == CTRL_X_EVAL; }
পূর্ণসংখ্যা ctrl_x_mode_register(শূন্য)
    { ফেরত ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    পূর্ণসংখ্যা
ctrl_x_mode_not_default(শূন্য)
{
    ফেরত ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    পূর্ণসংখ্যা
ctrl_x_mode_not_defined_yet(শূন্য)
{
    ফেরত ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    পূর্ণসংখ্যা
compl_status_adding(শূন্য)
{
    ফেরত compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    পূর্ণসংখ্যা
compl_status_sol(শূন্য)
{
    ফেরত compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    পূর্ণসংখ্যা
compl_status_local(শূন্য)
{
    ফেরত compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    শূন্য
compl_status_clear(শূন্য)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    স্থির পূর্ণসংখ্যা
compl_dir_forward(শূন্য)
{
    ফেরত compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    স্থির পূর্ণসংখ্যা
compl_shows_dir_forward(শূন্য)
{
    ফেরত compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    স্থির পূর্ণসংখ্যা
compl_shows_dir_backward(শূন্য)
{
    ফেরত compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    পূর্ণসংখ্যা
has_compl_option(পূর্ণসংখ্যা dict_opt)
{
    যদি (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
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
	edit_submode = ফাঁকা;
	বার্তা_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	যদি (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    যদি (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    পূর্ণসংখ্যা
vim_is_ctrl_x_চাবি(পূর্ণসংখ্যা c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    যদি (ins_compl_pum_চাবি(c))
	ফেরত TRUE;

    নির্বাচন (ctrl_x_mode)
    {
	ক্ষেত্র 0:		    // Not in any CTRL-X mode
	    ফেরত (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	ক্ষেত্র CTRL_X_NOT_DEFINED_YET:
	ক্ষেত্র CTRL_X_CMDLINE_CTRL_X:
	    ফেরত (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	ক্ষেত্র CTRL_X_SCROLL:
	    ফেরত (c == Ctrl_Y || c == Ctrl_E);
	ক্ষেত্র CTRL_X_WHOLE_সারি:
	    ফেরত (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_FILES:
	    ফেরত (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_DICTIONARY:
	    ফেরত (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_THESAURUS:
	    ফেরত (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_TAGS:
	    ফেরত (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	ক্ষেত্র CTRL_X_PATH_PATTERNS:
	    ফেরত (c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_PATH_DEFINES:
	    ফেরত (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	ক্ষেত্র CTRL_X_CMDLINE:
	    ফেরত (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	ক্ষেত্র CTRL_X_FUNCTION:
	    ফেরত (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_OMNI:
	    ফেরত (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	ক্ষেত্র CTRL_X_SPELL:
	    ফেরত (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_EVAL:
	    ফেরত (c == Ctrl_P || c == Ctrl_N);
	ক্ষেত্র CTRL_X_REGISTER:
	    ফেরত (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    ফেরত FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    স্থির পূর্ণসংখ্যা
match_at_original_text(compl_T *match)
{
    ফেরত match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    স্থির পূর্ণসংখ্যা
is_first_match(compl_T *match)
{
    ফেরত match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    স্থির compl_T *
find_original_text_match(শূন্য)
{
    যদি (compl_first_match == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (match_at_original_text(compl_first_match))
	ফেরত compl_first_match;
    যদি (compl_first_match->cp_পূর্ব != ফাঁকা
	    && match_at_original_text(compl_first_match->cp_পূর্ব))
	ফেরত compl_first_match->cp_পূর্ব;
    ফেরত ফাঁকা;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    পূর্ণসংখ্যা
ins_compl_accept_char(পূর্ণসংখ্যা c)
{
    যদি (compl_autocomplete && compl_from_nonkeyword)
	ফেরত FALSE;

    যদি (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	ফেরত vim_isIDc(c);

    নির্বাচন (ctrl_x_mode)
    {
	ক্ষেত্র CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    ফেরত vim_isfilec(c) && !vim_ispathsep(c);

	ক্ষেত্র CTRL_X_CMDLINE:
	ক্ষেত্র CTRL_X_CMDLINE_CTRL_X:
	ক্ষেত্র CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    ফেরত vim_isprintc(c) && !VIM_ISWHITE(c);

	ক্ষেত্র CTRL_X_WHOLE_সারি:
	    // For while line completion a space can be part of the line.
	    ফেরত vim_isprintc(c);
    }
    ফেরত vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    স্থির char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    পূর্ণসংখ্যা	    char_len,
    পূর্ণসংখ্যা	    compl_char_len,
    পূর্ণসংখ্যা	    min_len,
    char_u  **tofree)
{
    পূর্ণসংখ্যা		*wca;			// Wide character array.
    char_u	*p;
    পূর্ণসংখ্যা		i, c;
    পূর্ণসংখ্যা		has_lower = FALSE;
    পূর্ণসংখ্যা		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(পূর্ণসংখ্যা, char_len);
    যদি (wca == ফাঁকা)
	ফেরত IObuff;

    p = str;
    জন্য (i = 0; i < char_len; ++i)
    {
	যদি (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	অন্যথা
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    জন্য (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	যদি (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    যদি (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		জন্য (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		বিরতি;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    যদি (!has_lower)
    {
	p = compl_orig_text.string;
	জন্য (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    যদি (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		জন্য (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		বিরতি;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    জন্য (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	যদি (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	অন্যথা যদি (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    যখন (i < char_len)
    {
	যদি (gap.ga_তথ্য != ফাঁকা)
	{
	    যদি (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		ফেরত (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_তথ্য + gap.ga_len;
	    যদি (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    অন্যথা
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	অন্যথা যদি ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    যদি (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		ফেরত (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_তথ্য, IObuff);
	    gap.ga_len = (পূর্ণসংখ্যা)(p - IObuff);
	}
	অন্যথা যদি (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	অন্যথা
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    যদি (gap.ga_তথ্য != ফাঁকা)
    {
	*tofree = gap.ga_তথ্য;
	ফেরত gap.ga_তথ্য;
    }

    *p = NUL;
    ফেরত IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    পূর্ণসংখ্যা
ins_compl_add_infercase(
    char_u	*str_arg,
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		icase,
    char_u	*fname,
    পূর্ণসংখ্যা		dir,
    পূর্ণসংখ্যা		cont_s_ipos,  // next ^X<> will set initial_pos
    পূর্ণসংখ্যা		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    পূর্ণসংখ্যা		char_len;		// count multi-byte characters
    পূর্ণসংখ্যা		compl_char_len;
    পূর্ণসংখ্যা		min_len;
    পূর্ণসংখ্যা		flags = 0;
    পূর্ণসংখ্যা		res;
    char_u	*tofree = ফাঁকা;

    যদি (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	যদি (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    যখন (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	অন্যথা
	{
	    char_len = len;
	}

	// Find actual length of original text.
	যদি (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    যখন (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	অন্যথা
	{
	    compl_char_len = compl_দৈর্ঘ্য;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    যদি (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    যদি (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, ফাঁকা, ফাঁকা, dir, flags, FALSE, ফাঁকা, score);
    vim_free(tofree);
    ফেরত res;
}

/*
 * Check if fuzzy matching is enabled
 */
    স্থির পূর্ণসংখ্যা
cot_fuzzy(শূন্য)
{
    ফেরত (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    স্থির পূর্ণসংখ্যা
is_nearest_active(শূন্য)
{
    ফেরত (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    পূর্ণসংখ্যা
ins_compl_is_match_selected(শূন্য)
{
    ফেরত compl_shown_match != ফাঁকা && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    পূর্ণসংখ্যা
ins_compl_preinsert_longest(শূন্য)
{
    ফেরত compl_autocomplete
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
    স্থির পূর্ণসংখ্যা
ins_compl_add(
    char_u	*str,
    পূর্ণসংখ্যা		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_তথ্য UNUSED,  // "user_data" entry or NULL
    পূর্ণসংখ্যা		cdir,
    পূর্ণসংখ্যা		flags_arg,
    পূর্ণসংখ্যা		adup,		    // accept duplicate match
    পূর্ণসংখ্যা		*user_hl,           // user abbr/kind hlattr
    পূর্ণসংখ্যা		score)
{
    compl_T	*match, *current, *পূর্ব;
    পূর্ণসংখ্যা		dir = (cdir == 0 ? compl_direction : cdir);
    পূর্ণসংখ্যা		flags = flags_arg;
    পূর্ণসংখ্যা		inserted = FALSE;
    char_u	*new_str = ফাঁকা;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    যদি (flags & CP_FAST)
	fast_breakcheck();
    অন্যথা
	ui_breakcheck();
    যদি (got_int)
	ফেরত FAIL;
    যদি (len < 0)
	len = (পূর্ণসংখ্যা)STRLEN(str);

    // If the same match is already present, don't add it.
    যদি (compl_first_match != ফাঁকা && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *চাবি;
	hashitem_T  *hi;

	যদি (len < (পূর্ণসংখ্যা)আকার(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    চাবি = keybuf;
	}
	অন্যথা
	{
	    new_str = vim_strnsave(str, len);
	    যদি (new_str == ফাঁকা)
		ফেরত FAIL;
	    চাবি = new_str;
	}
	str_hash = hash_hash(চাবি);
	hi = hash_lookup(&compl_strings_ht, চাবি, str_hash);
	যদি (!HASHITEM_EMPTY(hi))
	{
	    যদি (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		করো
		{
		    যদি (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((পূর্ণসংখ্যা)match->cp_str.দৈর্ঘ্য <= len
					  || match->cp_str.string[len] == NUL))
		    {
			যদি (score < match->cp_score)
			    match->cp_score = score;
			বিরতি;
		    }
		    match = match->cp_পরবর্তী;
		} যখন (match != ফাঁকা && !is_first_match(match));
	    }
	    vim_free(new_str);
	    ফেরত NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    যদি (match == ফাঁকা)
    {
	vim_free(new_str);
	ফেরত FAIL;
    }
    match->cp_সংখ্যা = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    যদি (new_str == ফাঁকা && (new_str = vim_strnsave(str, len)) == ফাঁকা)
    {
	vim_free(match);
	ফেরত FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.দৈর্ঘ্য = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    যদি (fname != ফাঁকা
	    && compl_curr_match != ফাঁকা
	    && compl_curr_match->cp_fname != ফাঁকা
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    অন্যথা যদি (fname != ফাঁকা)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    অন্যথা
	match->cp_fname = ফাঁকা;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_সূচক;

    যদি (cptext != ফাঁকা)
    {
	জন্য (পূর্ণসংখ্যা i = 0; i < CPT_গুনতি; ++i)
	{
	    যদি (cptext[i] != ফাঁকা && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    যদি (user_তথ্য != ফাঁকা)
	match->cp_user_তথ্য = *user_তথ্য;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    যদি (compl_first_match == ফাঁকা)
	match->cp_পরবর্তী = match->cp_পূর্ব = ফাঁকা;
    অন্যথা যদি (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_পরবর্তী;
	পূর্ব = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	যখন (current != ফাঁকা && current != compl_first_match)
	{
	    যদি (current->cp_score < score)
	    {
		match->cp_পরবর্তী = current;
		match->cp_পূর্ব = current->cp_পূর্ব;
		যদি (current->cp_পূর্ব)
		    current->cp_পূর্ব->cp_পরবর্তী = match;
		current->cp_পূর্ব = match;
		inserted = TRUE;
		বিরতি;
	    }
	    পূর্ব = current;
	    current = current->cp_পরবর্তী;
	}
	যদি (!inserted)
	{
	    পূর্ব->cp_পরবর্তী = match;
	    match->cp_পূর্ব = পূর্ব;
	    match->cp_পরবর্তী = compl_first_match;
	    compl_first_match->cp_পূর্ব = match;
	}
    }
    অন্যথা যদি (dir == FORWARD)
    {
	match->cp_পরবর্তী = compl_curr_match->cp_পরবর্তী;
	match->cp_পূর্ব = compl_curr_match;
    }
    অন্যথা	// BACKWARD
    {
	match->cp_পরবর্তী = compl_curr_match;
	match->cp_পূর্ব = compl_curr_match->cp_পূর্ব;
    }
    যদি (match->cp_পরবর্তী)
	match->cp_পরবর্তী->cp_পূর্ব = match;
    যদি (match->cp_পূর্ব)
	match->cp_পূর্ব->cp_পরবর্তী = match;
    অন্যথা	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    যদি (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.দৈর্ঘ্য,
								    str_hash);

    // Find the longest common string if still doing that.
    যদি (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    ফেরত OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_equal(compl_T *match, char_u *str, পূর্ণসংখ্যা len)
{
    যদি (match->cp_flags & CP_EQUAL)
	ফেরত TRUE;
    যদি (match->cp_flags & CP_ICASE)
	ফেরত STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    ফেরত STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_equal_sc(compl_T *match, char_u *str, পূর্ণসংখ্যা len)
{
    পূর্ণসংখ্যা	typed = compl_দৈর্ঘ্য;
    পূর্ণসংখ্যা	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (পূর্ণসংখ্যা)(compl_longest_end_col - compl_col) : typed;

    যদি ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	ফেরত ins_compl_equal(match, str, len);

    যদি ((পূর্ণসংখ্যা)match->cp_str.দৈর্ঘ্য < len)
	ফেরত FALSE;

    জন্য (পূর্ণসংখ্যা i = 0; i < len; ++i)
    {
	যদি (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    স্থির শূন্য
ins_compl_ঢোকাও_bytes(char_u *p, পূর্ণসংখ্যা len)
{
    যদি (len == -1)
	len = (পূর্ণসংখ্যা)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(শূন্য)
{
    ফেরত compl_leader.string != ফাঁকা ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    স্থির size_t
ins_compl_leader_len(শূন্য)
{
    ফেরত compl_leader.string != ফাঁকা ? compl_leader.দৈর্ঘ্য : compl_orig_text.দৈর্ঘ্য;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    পূর্ণসংখ্যা
ins_compl_col_range_attr(linenr_T lnum, পূর্ণসংখ্যা col)
{
    পূর্ণসংখ্যা	    start_col;
    পূর্ণসংখ্যা	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    পূর্ণসংখ্যা	    attr;

    যদি (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	ফেরত -1;

    start_col = compl_col + (পূর্ণসংখ্যা)ins_compl_leader_len();
    যদি (!ins_compl_has_multiple())
	ফেরত (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    যদি ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	ফেরত attr;

    ফেরত -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_has_multiple(শূন্য)
{
    ফেরত vim_strchr(compl_shown_match->cp_str.string, '\n') != ফাঁকা;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    পূর্ণসংখ্যা
ins_compl_lnum_in_range(linenr_T lnum)
{
    যদি (!ins_compl_has_multiple())
	ফেরত FALSE;
    ফেরত lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    স্থির শূন্য
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    পূর্ণসংখ্যা		c1, c2;
    পূর্ণসংখ্যা		had_match;

    যদি (compl_leader.string == ফাঁকা)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.দৈর্ঘ্য);
	যদি (compl_leader.string == ফাঁকা)
	    ফেরত;

	compl_leader.দৈর্ঘ্য = match->cp_str.দৈর্ঘ্য;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ঢোকাও(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	যদি (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	ফেরত;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    যখন (*p != NUL)
    {
	যদি (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	অন্যথা
	{
	    c1 = *p;
	    c2 = *s;
	}
	যদি ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    বিরতি;
	যদি (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	অন্যথা
	{
	    ++p;
	    ++s;
	}
    }

    যদি (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.দৈর্ঘ্য = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_ঢোকাও(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	যদি (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    স্থির শূন্য
ins_compl_add_matches(
    পূর্ণসংখ্যা		অঙ্ক_matches,
    char_u	**matches,
    পূর্ণসংখ্যা		icase)
{
    পূর্ণসংখ্যা		add_r = OK;
    পূর্ণসংখ্যা		dir = compl_direction;

    জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, ফাঁকা, ফাঁকা, ফাঁকা, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ফাঁকা,
		FUZZY_SCORE_NONE);
	যদি (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(অঙ্ক_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    স্থির পূর্ণসংখ্যা
ins_compl_make_cyclic(শূন্য)
{
    compl_T *match;
    পূর্ণসংখ্যা	    গুনতি = 0;

    যদি (compl_first_match == ফাঁকা)
	ফেরত 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    যখন (match->cp_পরবর্তী != ফাঁকা && !is_first_match(match->cp_পরবর্তী))
    {
	match = match->cp_পরবর্তী;
	++গুনতি;
    }
    match->cp_পরবর্তী = compl_first_match;
    compl_first_match->cp_পূর্ব = match;

    ফেরত গুনতি;
}

/*
 * Return whether there currently is a shown match.
 */
    পূর্ণসংখ্যা
ins_compl_has_shown_match(শূন্য)
{
    ফেরত compl_shown_match == ফাঁকা
	|| compl_shown_match != compl_shown_match->cp_পরবর্তী;
}

/*
 * Return whether the shown match is long enough.
 */
    পূর্ণসংখ্যা
ins_compl_long_shown_match(শূন্য)
{
    ফেরত (পূর্ণসংখ্যা)compl_shown_match->cp_str.দৈর্ঘ্য
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned পূর্ণসংখ্যা
get_cot_flags(শূন্য)
{
    ফেরত curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    স্থির শূন্য
ins_compl_upd_pum(শূন্য)
{
    যদি (compl_match_array == ফাঁকা)
	ফেরত;

    পূর্ণসংখ্যা h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    যদি (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    স্থির শূন্য
ins_compl_del_pum(শূন্য)
{
    যদি (compl_match_array == ফাঁকা)
	ফেরত;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    পূর্ণসংখ্যা
pum_wanted(শূন্য)
{
    // 'completeopt' must contain "menu" or "menuone"
    ফেরত (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    স্থির পূর্ণসংখ্যা
pum_enough_matches(শূন্য)
{
    compl_T     *compl;
    পূর্ণসংখ্যা		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    করো
    {
	যদি (compl == ফাঁকা || (!match_at_original_text(compl) && ++i == 2))
	    বিরতি;
	compl = compl->cp_পরবর্তী;
    } যখন (!is_first_match(compl));

    যদি ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	ফেরত (i >= 1);
    ফেরত (i >= 2);
}

#যদি defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    স্থির dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    যদি (dict == ফাঁকা)
	ফেরত ফাঁকা;
    fill_complete_info_dict(dict, match, FALSE);
    ফেরত dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    স্থির শূন্য
trigger_complete_changed_event(পূর্ণসংখ্যা cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    স্থির পূর্ণসংখ্যা	    recursive = FALSE;
    save_v_event_T  save_v_event;

    যদি (recursive)
	ফেরত;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    যদি (item == ফাঁকা)
	ফেরত;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_বস্তুসমূহ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ফাঁকা, ফাঁকা, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    স্থির শূন্য*
cp_get_পরবর্তী(শূন্য *গ্রন্থি)
{
    ফেরত ((compl_T*)গ্রন্থি)->cp_পরবর্তী;
}

    স্থির শূন্য
cp_set_পরবর্তী(শূন্য *গ্রন্থি, শূন্য *পরবর্তী)
{
    ((compl_T*)গ্রন্থি)->cp_পরবর্তী = (compl_T*)পরবর্তী;
}

    স্থির শূন্য*
cp_get_পূর্ব(শূন্য* গ্রন্থি)
{
    ফেরত ((compl_T*)গ্রন্থি)->cp_পূর্ব;
}

    স্থির শূন্য
cp_set_পূর্ব(শূন্য* গ্রন্থি, শূন্য* পূর্ব)
{
    ((compl_T*)গ্রন্থি)->cp_পূর্ব = (compl_T*)পূর্ব;
}

    স্থির পূর্ণসংখ্যা
cp_compare_fuzzy(ধ্রুবক শূন্য* a, ধ্রুবক শূন্য* b)
{
    পূর্ণসংখ্যা score_a = ((compl_T*)a)->cp_score;
    পূর্ণসংখ্যা score_b = ((compl_T*)b)->cp_score;

    যদি (score_a != score_b)
	ফেরত (score_b > score_a) ? 1 : -1;

    ফেরত ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    স্থির পূর্ণসংখ্যা
cp_compare_nearest(ধ্রুবক শূন্য* a, ধ্রুবক শূন্য* b)
{
    পূর্ণসংখ্যা score_a = ((compl_T*)a)->cp_score;
    পূর্ণসংখ্যা score_b = ((compl_T*)b)->cp_score;
    যদি (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	ফেরত 0;
    ফেরত (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
prepend_startcol_text(string_T *dest, string_T *src, পূর্ণসংখ্যা startcol)
{
    পূর্ণসংখ্যা prepend_len = compl_col - startcol;
    পূর্ণসংখ্যা new_দৈর্ঘ্য = prepend_len + (পূর্ণসংখ্যা)src->দৈর্ঘ্য;

    dest->দৈর্ঘ্য = (size_t)new_দৈর্ঘ্য;
    dest->string = alloc(new_দৈর্ঘ্য + 1);  // +1 for NUL
    যদি (dest->string == ফাঁকা)
    {
	dest->দৈর্ঘ্য = 0;
	ফেরত FAIL;
    }

    char_u	*সারি = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, সারি + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->দৈর্ঘ্য);
    dest->string[new_দৈর্ঘ্য] = NUL;
    ফেরত OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    স্থির string_T *
get_leader_for_startcol(compl_T *match, পূর্ণসংখ্যা cached)
{
    স্থির string_T adjusted_leader = {ফাঁকা, 0};

    যদি (match == ফাঁকা)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	ফেরত ফাঁকা;
    }

    যদি (cpt_sources_array == ফাঁকা)
	যাও theend;

    পূর্ণসংখ্যা	cpt_idx = match->cp_cpt_source_idx;
    যদি (cpt_idx < 0)
	যাও theend;
    পূর্ণসংখ্যা	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    যদি (compl_leader.string == ফাঁকা)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	যদি (startcol < 0 || startcol >= compl_col)
	    ফেরত &compl_orig_text;
	ফেরত &compl_leader;  // pass through (startcol < compl_col)
    }

    যদি (compl_col <= 0)
	যাও theend;

    যদি (startcol >= 0 && startcol < compl_col)
    {
	পূর্ণসংখ্যা prepend_len = compl_col - startcol;
	পূর্ণসংখ্যা new_দৈর্ঘ্য = prepend_len + (পূর্ণসংখ্যা)compl_leader.দৈর্ঘ্য;
	যদি (cached && (size_t)new_দৈর্ঘ্য == adjusted_leader.দৈর্ঘ্য
		&& adjusted_leader.string != ফাঁকা)
	    ফেরত &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	যদি (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    যাও theend;

	ফেরত &adjusted_leader;
    }
theend:
    ফেরত &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    স্থির শূন্য
set_fuzzy_score(শূন্য)
{
    compl_T *compl;
    char_u  *pattern;
    পূর্ণসংখ্যা	    use_leader;

    যদি (compl_first_match == ফাঁকা)
	ফেরত;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ফাঁকা && compl_leader.দৈর্ঘ্য > 0);
    যদি (!use_leader)
    {
	যদি (compl_orig_text.string == ফাঁকা || compl_orig_text.দৈর্ঘ্য == 0)
	    ফেরত;
	pattern = compl_orig_text.string;
    }
    অন্যথা
    {
	/* Clear the leader cache once before the loop */
	(শূন্য)get_leader_for_startcol(ফাঁকা, TRUE);
	pattern = ফাঁকা;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    পূর্ণসংখ্যা পরবর্তী = 0;
    compl = compl_first_match;
    করো
    {
	যদি (compl->cp_order < 0)
	    compl->cp_order = পরবর্তী;
	পরবর্তী = MAX(পরবর্তী, compl->cp_order + 1);

	যদি (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_পরবর্তী;
    } যখন (compl != ফাঁকা && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    স্থির শূন্য
sort_compl_match_list(পূর্ণসংখ্যা (*compare)(ধ্রুবক শূন্য *, ধ্রুবক শূন্য *))
{
    compl_T	*orig_text;

    যদি (!compl_first_match || is_first_match(compl_first_match->cp_পরবর্তী))
	ফেরত;

    orig_text = find_original_text_match();
    যদি (orig_text == ফাঁকা)
	ফেরত;

    ins_compl_make_linear();
    যদি (orig_text == compl_first_match)
    {
	compl_first_match->cp_পরবর্তী->cp_পূর্ব = ফাঁকা;
	compl_first_match->cp_পরবর্তী = mergesort_list(compl_first_match->cp_পরবর্তী,
		cp_get_পরবর্তী, cp_set_পরবর্তী, cp_get_পূর্ব, cp_set_পূর্ব, compare);
	compl_first_match->cp_পরবর্তী->cp_পূর্ব = compl_first_match;
    }
    অন্যথা
    {
	compl_T	*লেজ;

	orig_text->cp_পূর্ব->cp_পরবর্তী = ফাঁকা;
	compl_first_match = mergesort_list(compl_first_match, cp_get_পরবর্তী,
		cp_set_পরবর্তী, cp_get_পূর্ব, cp_set_পূর্ব, compare);
	লেজ = compl_first_match;
	যখন (লেজ->cp_পরবর্তী != ফাঁকা)
	    লেজ = লেজ->cp_পরবর্তী;
	লেজ->cp_পরবর্তী = orig_text;
	orig_text->cp_পূর্ব = লেজ;
    }
    (শূন্য)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    স্থির পূর্ণসংখ্যা
get_user_highlight_attr(পূর্ণসংখ্যা hl_id)
{
    পূর্ণসংখ্যা	    attr;

    যদি (hl_id <= 0)
	ফেরত -1;
    attr = syn_id2attr(hl_id);
    ফেরত attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_build_pum(শূন্য)
{
    compl_T     *compl;
    compl_T     *shown_compl = ফাঁকা;
    পূর্ণসংখ্যা		did_find_shown_match = FALSE;
    পূর্ণসংখ্যা		shown_match_ok = FALSE;
    পূর্ণসংখ্যা		i = 0;
    পূর্ণসংখ্যা		cur = -1;
    পূর্ণসংখ্যা		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_শীর্ষ = ফাঁকা;
    compl_T	*match_লেজ = ফাঁকা;
    compl_T	*match_পরবর্তী = ফাঁকা;
    পূর্ণসংখ্যা		*match_গুনতি = ফাঁকা;
    পূর্ণসংখ্যা		is_forward = compl_shows_dir_forward();
    পূর্ণসংখ্যা		is_cpt_completion = (cpt_sources_array != ফাঁকা);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    যদি (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    যদি (compl_leader.string != ফাঁকা
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_পরবর্তী;

    যদি (is_cpt_completion)
    {
	match_গুনতি = ALLOC_CLEAR_MULT(পূর্ণসংখ্যা, cpt_sources_গুনতি);
	যদি (match_গুনতি == ফাঁকা)
	    ফেরত -1;
    }

    (শূন্য)get_leader_for_startcol(ফাঁকা, TRUE); // Clear the cache

    compl = compl_first_match;
    করো
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	যদি (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	যদি (!match_at_original_text(compl)
		&& (leader->string == ফাঁকা
		    || ins_compl_equal_sc(compl, leader->string,
			(পূর্ণসংখ্যা)leader->দৈর্ঘ্য)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    পূর্ণসংখ্যা match_limit_exceeded = FALSE;
	    পূর্ণসংখ্যা cur_source = compl->cp_cpt_source_idx;
	    যদি (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_গুনতি[cur_source]++;
		পূর্ণসংখ্যা max_matches = cpt_sources_array[cur_source].cs_max_matches;
		যদি (max_matches > 0 && match_গুনতি[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    যদি (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		যদি (match_শীর্ষ == ফাঁকা)
		    match_শীর্ষ = compl;
		অন্যথা
		    match_লেজ->cp_match_পরবর্তী = compl;
		match_লেজ = compl;

		যদি (!shown_match_ok && !cot_fuzzy())
		{
		    যদি (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    অন্যথা
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		অন্যথা যদি (cot_fuzzy())
		{
		    যদি (i == 0)
			shown_compl = compl;

		    যদি (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	যদি (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    যদি (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    যদি (!shown_match_ok && shown_compl != ফাঁকা)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_পরবর্তী;
    } যখন (compl != ফাঁকা && !is_first_match(compl));

    vim_free(match_গুনতি);

    যদি (compl_match_arraysize == 0)
	ফেরত -1;

    যদি (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    যদি (compl_match_array == ফাঁকা)
	ফেরত -1;

    compl = match_শীর্ষ;
    i = 0;
    যখন (compl != ফাঁকা)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != ফাঁকা
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != ফাঁকা
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_পরবর্তী = compl->cp_match_পরবর্তী;
	compl->cp_match_পরবর্তী = ফাঁকা;
	compl = match_পরবর্তী;
    }

    যদি (!shown_match_ok)    // no displayed match at all
	cur = -1;

    ফেরত cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    শূন্য
ins_compl_show_pum(শূন্য)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		cur = -1;
    colnr_T	col;

    যদি (!pum_wanted() || !pum_enough_matches())
	ফেরত;

    // Avoid redrawing the screen under a pum that stays in place.
    যদি (!pum_redraw_in_same_position())
	pum_call_update_screen();

    যদি (compl_match_array == ফাঁকা)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    অন্যথা
    {
	// popup menu already exists, only need to find the current item.
	জন্য (i = 0; i < compl_match_arraysize; ++i)
	{
	    যদি (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		বিরতি;
	    }
	}
    }

    যদি (compl_match_array == ফাঁকা)
    {
#ifdef FEAT_EVAL
	যদি (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	ফেরত;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    পূর্ণসংখ্যা pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    যদি (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    যদি (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    স্থির শূন্য
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    পূর্ণসংখ্যা		flags,		// DICT_FIRST and/or DICT_EXACT
    পূর্ণসংখ্যা		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    পূর্ণসংখ্যা		গুনতি;
    পূর্ণসংখ্যা		save_p_scs;
    পূর্ণসংখ্যা		dir = compl_direction;

    যদি (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	যদি (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	অন্যথা
#endif
	    ফেরত;
    }

    buf = alloc(LSIZE);
    যদি (buf == ফাঁকা)
	ফেরত;
    regmatch.regprog = ফাঁকা;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    যদি (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    যদি (ctrl_x_mode_সারি_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	যদি (pat_esc == ফাঁকা)
	    যাও theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	যদি (ptr == ফাঁকা)
	{
	    vim_free(pat_esc);
	    যাও theend;
	}
	vim_snprintf((অক্ষর *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    অন্যথা
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	যদি (regmatch.regprog == ফাঁকা)
	    যাও theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    যখন (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	যদি (flags == DICT_EXACT)
	{
	    গুনতি = 1;
	    files = &dict;
	}
	অন্যথা
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    যদি (!thesaurus && STRCMP(buf, "spell") == 0)
		গুনতি = -1;
	    অন্যথা
#endif
		যদি (vim_strchr(buf, '`') != ফাঁকা
		    || expand_wildcards(1, &buf, &গুনতি, &files,
						     EW_FILE|EW_SILENT) != OK)
		গুনতি = 0;
	}

#ifdef FEAT_SPELL
	যদি (গুনতি == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    যদি (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    অন্যথা
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	অন্যথা
#endif
	    যদি (গুনতি > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(গুনতি, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    যদি (flags != DICT_EXACT)
		FreeWild(গুনতি, files);
	}
	যদি (flags != 0)
	    বিরতি;
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
    স্থির পূর্ণসংখ্যা
thesaurus_add_শব্দগুলি_in_সারি(
    char_u	*fname,
    char_u	**buf_arg,
    পূর্ণসংখ্যা		dir,
    char_u	*skip_word)
{
    পূর্ণসংখ্যা		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    যখন (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	যদি (*ptr == NUL || *ptr == NL)
	    বিরতি;
	wstart = ptr;

	// Find end of the word.
	যদি (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    যখন (*ptr != NUL)
	    {
		পূর্ণসংখ্যা l = (*mb_ptr2len)(ptr);

		যদি (l < 2 && !vim_iswordc(*ptr))
		    বিরতি;
		ptr += l;
	    }
	অন্যথা
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	যদি (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (পূর্ণসংখ্যা)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    যদি (status == FAIL)
		বিরতি;
	}
    }

    *buf_arg = ptr;
    ফেরত status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    স্থির শূন্য
ins_compl_files(
    পূর্ণসংখ্যা		গুনতি,
    char_u	**files,
    পূর্ণসংখ্যা		thesaurus,
    পূর্ণসংখ্যা		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    পূর্ণসংখ্যা		*dir)
{
    char_u	*ptr;
    পূর্ণসংখ্যা		i;
    FILE	*fp;
    পূর্ণসংখ্যা		add_r;
    char_u	*leader = ফাঁকা;
    পূর্ণসংখ্যা		leader_len = 0;
    পূর্ণসংখ্যা		score = 0;
    পূর্ণসংখ্যা		len = 0;
    char_u	*সারি_end = ফাঁকা;

    যদি (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (পূর্ণসংখ্যা)ins_compl_leader_len();
    }

    জন্য (i = 0; i < গুনতি && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((অক্ষর *)files[i], "r");  // open dictionary file
	যদি (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    বার্তা_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((অক্ষর *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (অক্ষর *)files[i]);
	    (শূন্য)বার্তা_trunc_attr((অক্ষর *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	যদি (fp == ফাঁকা)
	    চালু;

	// Read dictionary file line by line.
	// Check each line for a match.
	যখন (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    যদি (cot_fuzzy() && leader_len > 0)
	    {
		সারি_end = find_সারি_end(ptr);
		যখন (ptr < সারি_end)
		{
		    যদি (fuzzy_match_str_in_সারি(&ptr, leader, &len, ফাঁকা, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_সারি_or_eval()
					? find_সারি_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (পূর্ণসংখ্যা)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			যদি (add_r == FAIL)
			    বিরতি;
			ptr = end_ptr;  // start from next word
			যদি (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_পরবর্তী
				&& score == compl_first_match->cp_পরবর্তী->cp_score)
			    compl_অঙ্ক_bests++;
		    }
		}
	    }
	    অন্যথা যদি (regmatch != ফাঁকা)
	    {
		যখন (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_সারি_or_eval() ? find_সারি_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (পূর্ণসংখ্যা)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    যদি (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_শব্দগুলি_in_সারি(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    যদি (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    অন্যথা যদি (add_r == FAIL)
			বিরতি;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    যদি (*ptr == '\n' || got_int)
			বিরতি;
		}
	    }
	    সারি_breakcheck();
	    ins_compl_check_চাবিগুলি(50, FALSE);
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
    যদি (has_mbyte)
    {
	যখন (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    অন্যথা
    {
	যখন (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    ফেরত ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    যদি (has_mbyte)
    {
	পূর্ণসংখ্যা start_class = mb_get_class(ptr);
	যদি (start_class > 1)
	    যখন (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		যদি (mb_get_class(ptr) != start_class)
		    বিরতি;
	    }
    }
    অন্যথা
	যখন (vim_iswordc(*ptr))
	    ++ptr;
    ফেরত ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_সারি_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    যখন (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    ফেরত s;
}

/*
 * Free a completion item in the list
 */
    স্থির শূন্য
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    যদি (compl_strings_ht.ht_used > 0 && match->cp_str.string != ফাঁকা
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	যদি (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    যদি (--entry->cse_গুনতি <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    যদি (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    জন্য (পূর্ণসংখ্যা i = 0; i < CPT_গুনতি; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_তথ্য);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    স্থির শূন্য
ins_compl_free(শূন্য)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    যদি (compl_first_match == ফাঁকা)
	ফেরত;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    করো
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_পরবর্তী;
	ins_compl_item_free(match);
    } যখন (compl_curr_match != ফাঁকা && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = ফাঁকা;
    compl_shown_match = ফাঁকা;
    compl_old_match = ফাঁকা;
}

/*
 * Reset/clear the completion state.
 */
    শূন্য
ins_compl_clear(শূন্য)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = ফাঁকা;
    compl_curr_buf = ফাঁকা;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ফাঁকা;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_অঙ্ক_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    পূর্ণসংখ্যা
ins_compl_active(শূন্য)
{
    ফেরত compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    পূর্ণসংখ্যা
ins_compl_win_active(win_T *wp)
{
    ফেরত ins_compl_active() && wp == compl_curr_win
	&& wp->w_বাফার == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    পূর্ণসংখ্যা
ins_compl_used_match(শূন্য)
{
    ফেরত compl_used_match;
}

/*
 * Initialize get longest common string.
 */
    শূন্য
ins_compl_init_get_longest(শূন্য)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    পূর্ণসংখ্যা
ins_compl_interrupted(শূন্য)
{
    ফেরত compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    পূর্ণসংখ্যা
ins_compl_enter_selects(শূন্য)
{
    ফেরত compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(শূন্য)
{
    ফেরত compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    পূর্ণসংখ্যা
ins_compl_len(শূন্য)
{
    ফেরত compl_দৈর্ঘ্য;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    পূর্ণসংখ্যা
ins_compl_has_preinsert(শূন্য)
{
    পূর্ণসংখ্যা cur_cot_flags = get_cot_flags();
    যদি (compl_autocomplete && p_ic && !p_inf)
	ফেরত FALSE;
    ফেরত !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    পূর্ণসংখ্যা
ins_compl_preinsert_effect(শূন্য)
{
    যদি (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	ফেরত FALSE;

    ফেরত curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    পূর্ণসংখ্যা
ins_compl_bs(শূন্য)
{
    char_u	*সারি;
    char_u	*p;

    যদি (ins_compl_preinsert_effect())
	ins_compl_delete();

    সারি = ml_get_curline();
    p = সারি + curwin->w_cursor.col;
    MB_PTR_BACK(সারি, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    যদি ((পূর্ণসংখ্যা)(p - সারি) - (পূর্ণসংখ্যা)compl_col < 0
	    || ((পূর্ণসংখ্যা)(p - সারি) - (পূর্ণসংখ্যা)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (পূর্ণসংখ্যা)(p - সারি) - (পূর্ণসংখ্যা)compl_col
							- compl_দৈর্ঘ্য < 0))
	ফেরত K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    যদি (curwin->w_cursor.col <= compl_col + compl_দৈর্ঘ্য
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.দৈর্ঘ্য = (size_t)((p - সারি) - compl_col);
    compl_leader.string = vim_strnsave(সারি + compl_col, compl_leader.দৈর্ঘ্য);
    যদি (compl_leader.string == ফাঁকা)
    {
	compl_leader.দৈর্ঘ্য = 0;
	ফেরত K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    যদি (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    যদি (compl_shown_match != ফাঁকা)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    ফেরত NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_need_restart(শূন্য)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    ফেরত compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    পূর্ণসংখ্যা
ins_compl_has_autocomplete(শূন্য)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    ফেরত curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    স্থির শূন্য
ins_compl_fuzzy_sort(শূন্য)
{
    পূর্ণসংখ্যা	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    যদি (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	যদি ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    পূর্ণসংখ্যা none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_পূর্ব);
	    যদি (!none_selected)
		compl_shown_match
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_first_match->cp_পরবর্তী : compl_first_match;
	}
    }
}

/*
 * Called after changing "compl_leader".
 * Show the popup menu with a different set of matches.
 * May also search for matches again if the previous search was interrupted.
 */
    স্থির শূন্য
ins_compl_new_leader(শূন্য)
{
    পূর্ণসংখ্যা	    save_w_wrow = curwin->w_wrow;
    পূর্ণসংখ্যা	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_ঢোকাও_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    যদি (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.দৈর্ঘ্য);
	যদি (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	যদি (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    অন্যথা
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	যদি (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	যদি (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	অন্যথা
	    compl_autocomplete = FALSE;
	যদি (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    যদি (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    যদি (compl_match_array == ফাঁকা)
	compl_enter_selects = FALSE;
    অন্যথা যদি (ins_compl_has_preinsert() && compl_leader.দৈর্ঘ্য > 0)
	ins_compl_ঢোকাও(TRUE, FALSE);
    অন্যথা যদি (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.দৈর্ঘ্য > 0 && !ins_compl_preinsert_effect())
	ins_compl_ঢোকাও(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    স্থির পূর্ণসংখ্যা
get_compl_len(শূন্য)
{
    পূর্ণসংখ্যা off = (পূর্ণসংখ্যা)curwin->w_cursor.col - (পূর্ণসংখ্যা)compl_col;
    ফেরত MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    শূন্য
ins_compl_addleader(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা	    cc;

    যদি (ins_compl_preinsert_effect())
	ins_compl_delete();

    যদি (stop_arrow() == FAIL)
	ফেরত;
    যদি (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	যদি (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    অন্যথা
    {
	ins_char(c);
	যদি (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    যদি (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    যদি (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.দৈর্ঘ্য = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.দৈর্ঘ্য);
	যদি (compl_leader.string == ফাঁকা)
	{
	    compl_leader.দৈর্ঘ্য = 0;
	    ফেরত;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    স্থির শূন্য
ins_compl_restart(শূন্য)
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
    compl_অঙ্ক_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    স্থির শূন্য
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    যদি (match == ফাঁকা)
	ফেরত;

    p = vim_strnsave(str, len);
    যদি (p == ফাঁকা)
	ফেরত;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.দৈর্ঘ্য = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    শূন্য
ins_compl_addfrommatch(শূন্য)
{
    char_u	*p;
    পূর্ণসংখ্যা		len = (পূর্ণসংখ্যা)curwin->w_cursor.col - (পূর্ণসংখ্যা)compl_col;
    পূর্ণসংখ্যা		c;

    p = compl_shown_match->cp_str.string;
    যদি ((পূর্ণসংখ্যা)compl_shown_match->cp_str.দৈর্ঘ্য <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	যদি (!match_at_original_text(compl_shown_match))
	    ফেরত;

	p = ফাঁকা;
	plen = 0;
	জন্য (cp = compl_shown_match->cp_পরবর্তী; cp != ফাঁকা
		&& !is_first_match(cp); cp = cp->cp_পরবর্তী)
	{
	    যদি (compl_leader.string == ফাঁকা
		    || ins_compl_equal(cp, compl_leader.string,
			(পূর্ণসংখ্যা)compl_leader.দৈর্ঘ্য))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.দৈর্ঘ্য;
		বিরতি;
	    }
	}
	যদি (p == ফাঁকা || (পূর্ণসংখ্যা)plen <= len)
	    ফেরত;
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
    স্থির পূর্ণসংখ্যা
set_ctrl_x_mode(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা	    retval = FALSE;

    নির্বাচন (c)
    {
	ক্ষেত্র Ctrl_E:
	ক্ষেত্র Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    যদি (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    অন্যথা
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ফাঁকা;
	    showmode();
	    বিরতি;
	ক্ষেত্র Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_সারি;
	    বিরতি;
	ক্ষেত্র Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    বিরতি;
	ক্ষেত্র Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    বিরতি;
	ক্ষেত্র Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    যদি (vpeekc() == '=')
		বিরতি;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    বিরতি;
	ক্ষেত্র Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    বিরতি;
#ifdef FEAT_COMPL_FUNC
	ক্ষেত্র Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    বিরতি;
	ক্ষেত্র Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    বিরতি;
#endif
	ক্ষেত্র 's':
	ক্ষেত্র Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    বিরতি;
	ক্ষেত্র Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    বিরতি;
#ifdef FEAT_FIND_ID
	ক্ষেত্র Ctrl_I:
	ক্ষেত্র K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    বিরতি;
	ক্ষেত্র Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    বিরতি;
#endif
	ক্ষেত্র Ctrl_V:
	ক্ষেত্র Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    বিরতি;
	ক্ষেত্র Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ফাঁকা;
	    showmode();
	    retval = TRUE;
	    বিরতি;
	ক্ষেত্র Ctrl_P:
	ক্ষেত্র Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    যদি (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    অন্যথা যদি (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	পূর্বনির্ধারিত:
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
	    যদি (c == Ctrl_X)
	    {
		যদি (compl_cont_mode != 0)
		    compl_cont_status = 0;
		অন্যথা
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = ফাঁকা;
	    showmode();
	    বিরতি;
    }

    ফেরত retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    স্থির শূন্য
trigger_complete_done_event(পূর্ণসংখ্যা mode UNUSED, string_T *word UNUSED)
{
#যদি defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    যদি (word == ফাঁকা || word->string == ফাঁকা)
	(শূন্য)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    অন্যথা
	(শূন্য)dict_add_string_len(v_event, "complete_word", word->string, (পূর্ণসংখ্যা)word->দৈর্ঘ্য);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (শূন্য)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == ফাঁকা) ? (char_u *)"" : mode_str->string, (পূর্ণসংখ্যা)mode_str->দৈর্ঘ্য);

    dict_set_বস্তুসমূহ_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#যদি defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    স্থির পূর্ণসংখ্যা
ins_compl_stop(পূর্ণসংখ্যা c, পূর্ণসংখ্যা পূর্ব_mode, পূর্ণসংখ্যা retval)
{
    পূর্ণসংখ্যা		want_cindent;
    string_T	word = {ফাঁকা, 0};

    // Remove pre-inserted text when present.
    যদি (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    যদি (compl_curr_match != ফাঁকা || compl_leader.string != ফাঁকা || c == Ctrl_E)
    {
	char_u	*ptr = ফাঁকা;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	যদি (compl_curr_match != ফাঁকা && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    যদি (compl_cont_mode == CTRL_X_WHOLE_সারি)
    {
	// re-indent the current line
	যদি (want_cindent)
	{
	    do_c_রাশিমালা_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    অন্যথা যদি (!compl_autocomplete || compl_used_match)
    {
	পূর্ণসংখ্যা পূর্ব_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	যদি (পূর্ব_col > 0)
	    dec_cursor();
	// only format when something was inserted
	যদি (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	যদি (পূর্ব_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    যদি ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.দৈর্ঘ্য);
	যদি (word.string == ফাঁকা)
	    word.দৈর্ঘ্য = 0;
	অন্যথা
	    word.দৈর্ঘ্য = compl_shown_match->cp_str.দৈর্ঘ্য;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    যদি (c == Ctrl_E)
    {
	char_u *p = ফাঁকা;
	size_t	plen = 0;

	ins_compl_delete();
	যদি (compl_leader.string != ফাঁকা)
	{
	    p = compl_leader.string;
	    plen = compl_leader.দৈর্ঘ্য;
	}
	অন্যথা যদি (compl_first_match != ফাঁকা)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.দৈর্ঘ্য;
	}
	যদি (p != ফাঁকা)
	{
	    পূর্ণসংখ্যা	    compl_len = get_compl_len();

	    যদি ((পূর্ণসংখ্যা)plen > compl_len)
		ins_compl_ঢোকাও_bytes(p + compl_len, (পূর্ণসংখ্যা)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_mode = পূর্ব_mode;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    যদি (!shortmess(SHM_COMPLETIONMENU))
	বার্তা_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    যদি (edit_submode != ফাঁকা)
    {
	edit_submode = ফাঁকা;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_অঙ্ক_bests = 0;
    compl_ins_end_col = 0;

    যদি (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    যদি (want_cindent && in_cinkeys(চাবি_COMPLETE, ' ', inindent(0)))
	do_c_রাশিমালা_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(পূর্ব_mode, &word);
    vim_free(word.string);

    ফেরত retval;
}

/*
 * Cancel completion.
 */
    পূর্ণসংখ্যা
ins_compl_cancel(শূন্য)
{
    ফেরত ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    পূর্ণসংখ্যা
ins_compl_prep(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা	    retval = FALSE;
    পূর্ণসংখ্যা	    পূর্ব_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    যদি (c != Ctrl_R && vim_is_ctrl_x_চাবি(c))
	edit_submode_extra = ফাঁকা;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    যদি (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	ফেরত retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    যদি (is_mouse_চাবি(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	যদি (c == K_LEFTRELEASE
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
	    ফেরত retval;
	যদি (popup_visible)
	{
	    পূর্ণসংখ্যা	    row = mouse_row;
	    পূর্ণসংখ্যা	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    যদি (wp != ফাঁকা && WIN_IS_POPUP(wp))
		ফেরত retval;
	}
    }
#endif

    যদি (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	যদি (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_চাবি(c)
		|| !vim_is_ctrl_x_চাবি(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    যদি (c == Ctrl_Z)
		retval = TRUE;
	}
	অন্যথা
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    যদি (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    যদি (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    অন্যথা যদি (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	যদি (c != Ctrl_R && !vim_is_ctrl_x_চাবি(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = ফাঁকা;
	}
	showmode();
    }

    যদি (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	যদি ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_চাবি(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, পূর্ব_mode, retval);
    }
    অন্যথা যদি (ctrl_x_mode == CTRL_X_LOCAL_বার্তা)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, ফাঁকা);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    যদি (c != Ctrl_R && !vim_is_ctrl_x_চাবি(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    ফেরত retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    স্থির শূন্য
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    পূর্ণসংখ্যা	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    যদি (ptr == ফাঁকা)
    {
	যদি (compl_leader.string != ফাঁকা)
	    ptr = compl_leader.string;
	অন্যথা
	    ফেরত;  // nothing to do
    }
    যদি (compl_orig_text.string != ফাঁকা)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	যখন (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	যদি (len > 0)
	    len -= (*mb_শীর্ষ_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	জন্য (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    যদি (ptr != ফাঁকা)
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
    স্থির buf_T *
ins_compl_পরবর্তী_buf(buf_T *buf, পূর্ণসংখ্যা flag)
{
    স্থির win_T    *wp = ফাঁকা;
    পূর্ণসংখ্যা		    skip_বাফার;

    যদি (flag == 'w')		// just windows
    {
	যদি (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	যখন (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_পরবর্তী != ফাঁকা) ? wp->w_পরবর্তী : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    যদি (wp == curwin || !wp->w_বাফার->b_scanned)
		বিরতি;
	}
	buf = wp->w_বাফার;
    }
    অন্যথা
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	যখন (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_পরবর্তী != ফাঁকা) ? buf->b_পরবর্তী : firstbuf;
	    // Break if we're back at start buffer
	    যদি (buf == curbuf)
		বিরতি;

	    // Check buffer conditions based on flag
	    যদি (flag == 'U')
		skip_বাফার = buf->b_p_bl;
	    অন্যথা
		skip_বাফার = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ফাঁকা) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    যদি (!skip_বাফার && !buf->b_scanned)
		বিরতি;
	}
    }
    ফেরত buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    স্থির পূর্ণসংখ্যা
get_cpt_sources_গুনতি(char_u *cpt)
{
    char_u  dummy[LSIZE];
    পূর্ণসংখ্যা	    গুনতি = 0;
    char_u  *p;

    জন্য (p = cpt; *p != NUL; )
    {
	যখন (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	যদি (*p != NUL)
	{
	    (শূন্য)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    গুনতি++;
	}
    }

    ফেরত গুনতি;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
স্থির callback_T cfu_cb;	    // 'completefunc' callback function
স্থির callback_T ofu_cb;	    // 'omnifunc' callback function
স্থির callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
স্থির callback_T *cpt_cb;	    // Callback functions associated with F{func}
স্থির পূর্ণসংখ্যা	  cpt_cb_গুনতি;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    স্থির শূন্য
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    যদি (globcb->cb_name != ফাঁকা && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    অক্ষর *
did_set_completefunc(optset_T *args)
{
    পূর্ণসংখ্যা	retval;

    যদি (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    অন্যথা
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	যদি (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    ফেরত retval == FAIL ? e_invalid_argument : ফাঁকা;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    শূন্য
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
    অক্ষর *
did_set_omnifunc(optset_T *args)
{
    পূর্ণসংখ্যা	retval;

    যদি (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    অন্যথা
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	যদি (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    ফেরত retval == FAIL ? e_invalid_argument : ফাঁকা;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    শূন্য
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    শূন্য
clear_cpt_callbacks(callback_T **callbacks, পূর্ণসংখ্যা গুনতি)
{
    যদি (callbacks == ফাঁকা || *callbacks == ফাঁকা)
	ফেরত;

    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    স্থির পূর্ণসংখ্যা
copy_cpt_callbacks(callback_T **dest, পূর্ণসংখ্যা *dest_cnt, callback_T *src, পূর্ণসংখ্যা cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    যদি (cnt == 0)
	ফেরত OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    যদি (*dest == ফাঁকা)
	ফেরত FAIL;

    *dest_cnt = cnt;

    জন্য (পূর্ণসংখ্যা i = 0; i < cnt; i++)
	যদি (src[i].cb_name != ফাঁকা && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    ফেরত OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    শূন্য
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    যদি (buf == ফাঁকা)
	ফেরত;
    (শূন্য)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_গুনতি, cpt_cb,
	    cpt_cb_গুনতি);
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
    স্থির পূর্ণসংখ্যা
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, পূর্ণসংখ্যা *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    পূর্ণসংখ্যা	    idx = 0;
    পূর্ণসংখ্যা	    slen;
    পূর্ণসংখ্যা	    গুনতি;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    গুনতি = get_cpt_sources_গুনতি(cpt);
    যদি (গুনতি == 0)
	ফেরত OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, গুনতি);
    যদি (*cbp == ফাঁকা)
	ফেরত FAIL;
    *cnt = গুনতি;

    জন্য (p = cpt; *p != NUL; )
    {
	যখন (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	যদি (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    যদি (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		যদি (caret != ফাঁকা)
		    *caret = NUL;

		যদি (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ফাঁকা;
	    }
	    idx++;
	}
    }

    ফেরত OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    পূর্ণসংখ্যা
set_cpt_callbacks(optset_T *args)
{
    পূর্ণসংখ্যা	    opt_flags = args->os_flags;

    যদি (curbuf == ফাঁকা)
	ফেরত FAIL;

    // ":setglobal" does not change the buffer-local option value
    যদি (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_গুনতি) != OK)
	ফেরত FAIL;

    // ":setlocal" does not change the global option value
    যদি (opt_flags & OPT_LOCAL)
	ফেরত OK;

    // Cache the callbacks for the global value
    যদি (opt_flags & OPT_GLOBAL)
	ফেরত parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_গুনতি);

    // set case
    ফেরত copy_cpt_callbacks(&cpt_cb, &cpt_cb_গুনতি, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_গুনতি);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    অক্ষর *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    পূর্ণসংখ্যা	retval;

    যদি (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    অন্যথা
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	যদি (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    ফেরত retval == FAIL ? e_invalid_argument : ফাঁকা;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    পূর্ণসংখ্যা
set_ref_in_cpt_callbacks(callback_T *callbacks, পূর্ণসংখ্যা গুনতি, পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা	abort = FALSE;

    যদি (callbacks == ফাঁকা)
	ফেরত FALSE;

    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    ফেরত abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    পূর্ণসংখ্যা
set_ref_in_insexpand_funcs(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_গুনতি, copyID);

    ফেরত abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    স্থির char_u *
get_complete_funcname(পূর্ণসংখ্যা type)
{
    নির্বাচন (type)
    {
	ক্ষেত্র CTRL_X_FUNCTION:
	    ফেরত curbuf->b_p_cfu;
	ক্ষেত্র CTRL_X_OMNI:
	    ফেরত curbuf->b_p_ofu;
	ক্ষেত্র CTRL_X_THESAURUS:
	    ফেরত *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	পূর্বনির্ধারিত:
	    ফেরত (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    স্থির callback_T *
get_ঢোকাও_callback(পূর্ণসংখ্যা type)
{
    যদি (type == CTRL_X_FUNCTION)
	ফেরত &curbuf->b_cfu_cb;
    যদি (type == CTRL_X_OMNI)
	ফেরত &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    ফেরত (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    স্থির শূন্য
expand_by_function(পূর্ণসংখ্যা type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = ফাঁকা;
    dict_T	*matchdict = ফাঁকা;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    পূর্ণসংখ্যা		save_State = State;
    পূর্ণসংখ্যা		retval;
    পূর্ণসংখ্যা		is_cpt_function = (cb != ফাঁকা);
    পূর্ণসংখ্যা		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    যদি (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	যদি (*funcname == NUL)
	    ফেরত;
	cb = get_ঢোকাও_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_সংখ্যা;
    args[0].vval.v_সংখ্যা = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != ফাঁকা ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    যদি (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    যদি (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    যদি (retval == OK)
    {
	নির্বাচন (rettv.v_type)
	{
	    ক্ষেত্র VAR_LIST:
		matchlist = rettv.vval.v_list;
		বিরতি;
	    ক্ষেত্র VAR_DICT:
		matchdict = rettv.vval.v_dict;
		বিরতি;
	    ক্ষেত্র VAR_SPECIAL:
		যদি (rettv.vval.v_সংখ্যা == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    পূর্বনির্ধারিত:
		// TODO: Give error message?
		clear_tv(&rettv);
		বিরতি;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    যদি (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	যাও theend;
    }

    যদি (matchlist != ফাঁকা)
	ins_compl_add_list(matchlist);
    অন্যথা যদি (matchdict != ফাঁকা)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    যদি (matchdict != ফাঁকা)
	dict_unref(matchdict);
    যদি (matchlist != ফাঁকা)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#যদি defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    স্থির অন্তঃরেখ পূর্ণসংখ্যা
get_user_highlight_id(char_u *hlname)
{
    যদি (hlname != ফাঁকা && *hlname != NUL)
	ফেরত syn_check_group(hlname, (পূর্ণসংখ্যা)STRLEN(hlname));
    ফেরত 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    স্থির পূর্ণসংখ্যা
ins_compl_add_tv(typval_T *tv, পূর্ণসংখ্যা dir, পূর্ণসংখ্যা fast)
{
    char_u	*word;
    পূর্ণসংখ্যা		dup = FALSE;
    পূর্ণসংখ্যা		empty = FALSE;
    পূর্ণসংখ্যা		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_গুনতি]);
    typval_T	user_তথ্য;
    পূর্ণসংখ্যা		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    পূর্ণসংখ্যা		user_hl[2] = { 0, 0 };

    user_তথ্য.v_type = VAR_UNKNOWN;
    যদি (tv->v_type == VAR_DICT && tv->vval.v_dict != ফাঁকা)
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

	dict_get_tv(tv->vval.v_dict, "user_data", &user_তথ্য);
	যদি (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ফাঁকা
				  && dict_get_সংখ্যা(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	যদি (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ফাঁকা)
	    dup = dict_get_সংখ্যা(tv->vval.v_dict, "dup");
	যদি (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ফাঁকা)
	    empty = dict_get_সংখ্যা(tv->vval.v_dict, "empty");
	যদি (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ফাঁকা
				  && dict_get_সংখ্যা(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    অন্যথা
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    যদি (word == ফাঁকা || (!empty && *word == NUL))
    {
	clear_tv(&user_তথ্য);
	ফেরত FAIL;
    }
    status = ins_compl_add(word, -1, ফাঁকা, cptext,
	    &user_তথ্য, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    যদি (status != OK)
	clear_tv(&user_তথ্য);
    ফেরত status;
}

/*
 * Add completions from a list.
 */
    স্থির শূন্য
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    পূর্ণসংখ্যা		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_বস্তুসমূহ(list, li)
    {
	যদি (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	অন্যথা যদি (did_emsg)
	    বিরতি;
    }
}

/*
 * Add completions from a dict.
 */
    স্থির শূন্য
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_শব্দগুলি;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    যদি (di_refresh != ফাঁকা && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	যদি (v != ফাঁকা && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_শব্দগুলি = dict_find(dict, (char_u *)"words", 5);
    যদি (di_শব্দগুলি != ফাঁকা && di_শব্দগুলি->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_শব্দগুলি->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    স্থির শূন্য
set_completion(colnr_T startcol, list_T *list)
{
    পূর্ণসংখ্যা save_w_wrow = curwin->w_wrow;
    পূর্ণসংখ্যা save_w_leftcol = curwin->w_leftcol;
    পূর্ণসংখ্যা flags = CP_ORIGINAL_TEXT;
    unsigned পূর্ণসংখ্যা cur_cot_flags = get_cot_flags();
    পূর্ণসংখ্যা compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    পূর্ণসংখ্যা compl_no_ঢোকাও = (cur_cot_flags & COT_NOINSERT) != 0;
    পূর্ণসংখ্যা compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    যদি (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    যদি (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_দৈর্ঘ্য = (পূর্ণসংখ্যা)curwin->w_cursor.col - (পূর্ণসংখ্যা)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_দৈর্ঘ্য);
    যদি (p_ic)
	flags |= CP_ICASE;
    যদি (compl_orig_text.string == ফাঁকা)
    {
	compl_orig_text.দৈর্ঘ্য = 0;
	ফেরত;
    }
    compl_orig_text.দৈর্ঘ্য = (size_t)compl_দৈর্ঘ্য;
    যদি (ins_compl_add(compl_orig_text.string,
			(পূর্ণসংখ্যা)compl_orig_text.দৈর্ঘ্য, ফাঁকা, ফাঁকা, ফাঁকা, 0,
			flags | CP_FAST, FALSE, ফাঁকা, FUZZY_SCORE_NONE) != OK)
	ফেরত;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    পূর্ণসংখ্যা no_select = compl_no_select || compl_longest;
    যদি (compl_no_ঢোকাও || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	যদি (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    অন্যথা
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_ঢোকাও;

    // Lazily show the popup menu, unless we got interrupted.
    যদি (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    শূন্য
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি ((State & MODE_ঢোকাও) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_ঢোকাও_mode));
	ফেরত;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    যদি (!undo_allowed())
	ফেরত;

    যদি (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	পূর্ণসংখ্যা startcol = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা);
	যদি (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    শূন্য
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    শূন্য
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_চাবিগুলি(0, TRUE);
    rettv->vval.v_সংখ্যা = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    স্থির শূন্য
ins_compl_mode(string_T *ret)
{
    যদি (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == ফাঁকা) ? (char_u *)"" : mode->string;
	ret->দৈর্ঘ্য = mode->দৈর্ঘ্য;
    }
    অন্যথা
    {
	ret->string = (char_u *)"";
	ret->দৈর্ঘ্য = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    স্থির শূন্য
ins_compl_update_sequence_numbers(শূন্য)
{
    পূর্ণসংখ্যা		সংখ্যা = 0;
    compl_T	*match;

    যদি (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	জন্য (match = compl_curr_match->cp_পূর্ব; match != ফাঁকা
		&& !is_first_match(match); match = match->cp_পূর্ব)
	    যদি (match->cp_সংখ্যা != -1)
	    {
		সংখ্যা = match->cp_সংখ্যা;
		বিরতি;
	    }
	যদি (match != ফাঁকা)
	    // go up and assign all numbers which are not assigned yet
	    জন্য (match = match->cp_পরবর্তী;
		    match != ফাঁকা && match->cp_সংখ্যা == -1;
					   match = match->cp_পরবর্তী)
		match->cp_সংখ্যা = ++সংখ্যা;
    }
    অন্যথা // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	জন্য (match = compl_curr_match->cp_পরবর্তী; match != ফাঁকা
		&& !is_first_match(match); match = match->cp_পরবর্তী)
	{
	    যদি (match->cp_সংখ্যা != -1)
	    {
		সংখ্যা = match->cp_সংখ্যা;
		বিরতি;
	    }
	}
	যদি (match != ফাঁকা)
	{
	    // go down and assign all numbers which are not assigned yet
	    জন্য (match = match->cp_পূর্ব; match
		    && match->cp_সংখ্যা == -1;
					   match = match->cp_পূর্ব)
		match->cp_সংখ্যা = ++সংখ্যা;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    স্থির শূন্য
fill_complete_info_dict(dict_T *di, compl_T *match, পূর্ণসংখ্যা add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (পূর্ণসংখ্যা)match->cp_str.দৈর্ঘ্য);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    যদি (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    যদি (match->cp_user_তথ্য.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    অন্যথা
	dict_add_tv(di, "user_data", &match->cp_user_তথ্য);
}

/*
 * Get complete information
 */
    স্থির শূন্য
get_complete_info(list_T *what_list, dict_T *retdict)
{
    পূর্ণসংখ্যা		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_বস্তুসমূহ		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    পূর্ণসংখ্যা		what_flag;

    যদি (what_list == ফাঁকা)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    অন্যথা
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_বস্তুসমূহ(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    যদি (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    অন্যথা যদি (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    অন্যথা যদি (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_বস্তুসমূহ;
	    অন্যথা যদি (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    অন্যথা যদি (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    অন্যথা যদি (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    অন্যথা যদি (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    অন্যথা যদি (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    যদি (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (পূর্ণসংখ্যা)mode.দৈর্ঘ্য);
    }

    যদি (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_সংখ্যা(retdict, "pum_visible", pum_visible());

    যদি (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_সংখ্যা(retdict, "auto", compl_autostarted);

    যদি (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*সারি = ml_get_curline();
	পূর্ণসংখ্যা	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? সারি + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    যদি (ret == OK && (what_flag & (CI_WHAT_বস্তুসমূহ | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ফাঁকা;
	dict_T	    *di;
	compl_T     *match;
	পূর্ণসংখ্যা         selected_idx = -1;
	পূর্ণসংখ্যা	    has_বস্তুসমূহ = what_flag & CI_WHAT_বস্তুসমূহ;
	পূর্ণসংখ্যা	    has_matches = what_flag & CI_WHAT_MATCHES;
	পূর্ণসংখ্যা	    has_completed = what_flag & CI_WHAT_COMPLETED;

	যদি (has_বস্তুসমূহ || has_matches)
	{
	    li = list_alloc();
	    যদি (li == ফাঁকা)
		ফেরত;
	    ret = dict_add_list(retdict, (has_matches && !has_বস্তুসমূহ)
						? "matches" : "items", li);
	    যদি (ret == FAIL)
		list_unref(li);
	}
	যদি (ret == OK && what_flag & CI_WHAT_SELECTED)
	    যদি (compl_curr_match != ফাঁকা && compl_curr_match->cp_সংখ্যা == -1)
		ins_compl_update_sequence_numbers();
	যদি (ret == OK && compl_first_match != ফাঁকা)
	{
	    পূর্ণসংখ্যা list_idx = 0;
	    match = compl_first_match;
	    করো
	    {
		যদি (!match_at_original_text(match))
		{
		    যদি (has_বস্তুসমূহ
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			যদি (di == ফাঁকা)
			    ফেরত;
			ret = list_append_dict(li, di);
			যদি (ret != OK)
			{
			    dict_unref(di);
			    ফেরত;
			}
			fill_complete_info_dict(di, match, has_matches && has_বস্তুসমূহ);
		    }
		    যদি (compl_curr_match != ফাঁকা
			    && compl_curr_match->cp_সংখ্যা == match->cp_সংখ্যা)
			selected_idx = list_idx;
		    যদি (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_পরবর্তী;
	    }
	    যখন (match != ফাঁকা && !is_first_match(match));
	}
	যদি (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_সংখ্যা(retdict, "selected", selected_idx);

	যদি (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    যদি (di == ফাঁকা)
		ফেরত;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    যদি (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    শূন্য
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ফাঁকা;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	যদি (check_for_list_arg(argvars, 0) == FAIL)
	    ফেরত;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    স্থির পূর্ণসংখ্যা
thesaurus_func_complete(পূর্ণসংখ্যা type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    ফেরত type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#অন্যথা
    ফেরত FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    স্থির পূর্ণসংখ্যা
may_advance_cpt_সূচক(char_u *cpt)
{
    char_u  *p = cpt;

    যদি (cpt_sources_সূচক == -1)
	ফেরত FALSE;
    যখন (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    ফেরত (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
গণনা
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
প্রকারলেখ কাঠামো
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	পূর্ব_match_pos;		// previous match position
    পূর্ণসংখ্যা		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    পূর্ণসংখ্যা		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    পূর্ণসংখ্যা		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_পরবর্তী_state_T;

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
    স্থির পূর্ণসংখ্যা
process_পরবর্তী_cpt_মান(
	ins_compl_পরবর্তী_state_T *st,
	পূর্ণসংখ্যা		*compl_type_arg,
	pos_T		*start_match_pos,
	পূর্ণসংখ্যা		fuzzy_collect,
	পূর্ণসংখ্যা		*advance_cpt_idx)
{
    পূর্ণসংখ্যা	    compl_type = -1;
    পূর্ণসংখ্যা	    status = INS_COMPL_CPT_OK;
    পূর্ণসংখ্যা	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    যখন (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    যদি (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	যদি (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_সারি_গুনতি;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    অন্যথা যদি (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ফাঁকা
	    && (st->ins_buf = ins_compl_পরবর্তী_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	যদি (st->ins_buf->b_ml.ml_mfp != ফাঁকা)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_সারি_গুনতি + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	অন্যথা	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    যদি (st->ins_buf->b_fname == ফাঁকা)
	    {
		status = INS_COMPL_CPT_CONT;
		যাও done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	যদি (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    বার্তা_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((অক্ষর *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ফাঁকা
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ফাঁকা
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (শূন্য)বার্তা_trunc_attr((অক্ষর *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    অন্যথা যদি (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    অন্যথা
    {
	যদি (ctrl_x_mode_সারি_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	অন্যথা যদি (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_সূচক);
	    যদি (!st->func_cb)
		compl_type = -1;
	}
#endif
	অন্যথা যদি (!skip_source)
	{
	    যদি (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		যদি (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		অন্যথা
		    compl_type = CTRL_X_THESAURUS;
		যদি (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    অন্যথা যদি (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    অন্যথা যদি (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    অন্যথা যদি (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		যদি (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    বার্তা_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((অক্ষর *)IObuff, IOSIZE, _("Scanning tags."));
		    (শূন্য)বার্তা_trunc_attr((অক্ষর *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    অন্যথা
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(শূন্য)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_সূচক(st->e_cpt);

	st->found_all = TRUE;
	যদি (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    ফেরত status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    স্থির শূন্য
get_পরবর্তী_include_file_completion(পূর্ণসংখ্যা compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (পূর্ণসংখ্যা)compl_pattern.দৈর্ঘ্য, FALSE, FALSE,
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
    স্থির শূন্য
get_পরবর্তী_dict_tsr_completion(পূর্ণসংখ্যা compl_type, char_u *dict, পূর্ণসংখ্যা dict_f)
{
#ifdef FEAT_COMPL_FUNC
    যদি (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, ফাঁকা);
    অন্যথা
#endif
	ins_compl_dictionaries(
		dict != ফাঁকা ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != ফাঁকা ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    স্থির শূন্য
get_পরবর্তী_tag_completion(শূন্য)
{
    পূর্ণসংখ্যা		save_p_ic;
    char_u	**matches;
    পূর্ণসংখ্যা		অঙ্ক_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    যদি (find_tags(compl_pattern.string, &অঙ্ক_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && অঙ্ক_matches > 0)
	ins_compl_add_matches(অঙ্ক_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    স্থির পূর্ণসংখ্যা
compare_scores(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
{
    পূর্ণসংখ্যা idx_a = *(ধ্রুবক পূর্ণসংখ্যা *)a;
    পূর্ণসংখ্যা idx_b = *(ধ্রুবক পূর্ণসংখ্যা *)b;
    পূর্ণসংখ্যা score_a = compl_fuzzy_scores[idx_a];
    পূর্ণসংখ্যা score_b = compl_fuzzy_scores[idx_b];
    ফেরত score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    স্থির শূন্য
ins_compl_longest_ঢোকাও(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_ঢোকাও_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    স্থির শূন্য
fuzzy_longest_match(শূন্য)
{
    char_u	*prefix = ফাঁকা;
    পূর্ণসংখ্যা		prefix_len = 0;
    পূর্ণসংখ্যা		i = 0;
    পূর্ণসংখ্যা		j = 0;
    char_u	*match_str = ফাঁকা;
    char_u	*prefix_ptr = ফাঁকা;
    char_u	*match_ptr = ফাঁকা;
    char_u	*leader = ফাঁকা;
    size_t	leader_len = 0;
    compl_T	*compl = ফাঁকা;
    পূর্ণসংখ্যা		more_candidates = FALSE;
    compl_T	*nn_compl = ফাঁকা;

    যদি (compl_অঙ্ক_bests == 0)
	ফেরত;

    nn_compl = compl_first_match->cp_পরবর্তী->cp_পরবর্তী;
    যদি (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_সারি() ? compl_first_match
				    : compl_first_match->cp_পরবর্তী;
    যদি (compl_অঙ্ক_bests == 1)
    {
	// no more candidates insert the match str
	যদি (!more_candidates)
	{
	    ins_compl_longest_ঢোকাও(compl->cp_str.string);
	    compl_অঙ্ক_bests = 0;
	}
	compl_অঙ্ক_bests = 0;
	ফেরত;
    }

    যদি ((size_t)compl_অঙ্ক_bests > আকার_MAX / আকার(compl_T *))
	ফেরত;
    compl_best_matches = (compl_T **)alloc(compl_অঙ্ক_bests * আকার(compl_T *));
    যদি (compl_best_matches == ফাঁকা)
	ফেরত;
    যখন (compl != ফাঁকা && i < compl_অঙ্ক_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_পরবর্তী;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (পূর্ণসংখ্যা)compl_best_matches[0]->cp_str.দৈর্ঘ্য;

    জন্য (i = 1; i < compl_অঙ্ক_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	যখন (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    যদি (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		বিরতি;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	যদি (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    যদি (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	যাও end;

    prefix = vim_strnsave(prefix, prefix_len);
    যদি (prefix != ফাঁকা)
    {
	ins_compl_longest_ঢোকাও(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = ফাঁকা;
    compl_অঙ্ক_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    স্থির শূন্য
get_পরবর্তী_filename_completion(শূন্য)
{
    char_u	**matches;
    পূর্ণসংখ্যা		অঙ্ক_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    পূর্ণসংখ্যা		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    পূর্ণসংখ্যা		*fuzzy_indices_তথ্য;
    char_u	*last_sep = ফাঁকা;
    পূর্ণসংখ্যা		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    পূর্ণসংখ্যা		max_score = 0;
    পূর্ণসংখ্যা		current_score = 0;
    পূর্ণসংখ্যা		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    অক্ষর pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#অন্যথা
    অক্ষর pathsep = PATHSEP;
#endif

    যদি (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	যদি (curbuf->b_p_csl[0] == 's')
	{
	    জন্য (i = 0; i < (পূর্ণসংখ্যা)leader_len; i++)
	    {
		যদি (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	অন্যথা যদি (curbuf->b_p_csl[0] == 'b')
	{
	    জন্য (i = 0; i < (পূর্ণসংখ্যা)leader_len; i++)
	    {
		যদি (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	যদি (last_sep == ফাঁকা)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    যদি (compl_pattern.string == ফাঁকা)
		ফেরত;
	    compl_pattern.দৈর্ঘ্য = 1;
	}
	অন্যথা যদি (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	অন্যথা
	{
	    // Split leader into path and file parts
	    পূর্ণসংখ্যা path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    যদি (path_with_wildcard != ফাঁকা)
	    {
		vim_snprintf((অক্ষর *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.দৈর্ঘ্য = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    যদি (expand_wildcards(1, &compl_pattern.string, &অঙ্ক_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	ফেরত;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, অঙ্ক_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    যদি (curbuf->b_p_csl[0] != NUL)
    {
	জন্য (i = 0; i < অঙ্ক_matches; ++i)
	{
	    ptr = matches[i];
	    যখন (*ptr != NUL)
	    {
		যদি (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		অন্যথা যদি (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    যদি (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, আকার(পূর্ণসংখ্যা), 10);
	compl_fuzzy_scores = (পূর্ণসংখ্যা *)alloc(আকার(পূর্ণসংখ্যা) * অঙ্ক_matches);

	জন্য (i = 0; i < অঙ্ক_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    যদি (score != FUZZY_SCORE_NONE)
	    {
		যদি (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((পূর্ণসংখ্যা *)fuzzy_indices.ga_তথ্য)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	যদি (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = ফাঁকা;
	    fuzzy_indices_তথ্য = (পূর্ণসংখ্যা *)fuzzy_indices.ga_তথ্য;
	    qsort(fuzzy_indices_তথ্য, fuzzy_indices.ga_len, আকার(পূর্ণসংখ্যা), compare_scores);

	    জন্য (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_তথ্য[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_তথ্য[i]];
		যদি (ins_compl_add(match, -1, ফাঁকা, ফাঁকা, ফাঁকা, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ফাঁকা, current_score) == OK)
		    dir = FORWARD;

		যদি (need_collect_bests)
		{
		    যদি (i == 0 || current_score == max_score)
		    {
			compl_অঙ্ক_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(অঙ্ক_matches, matches);
	}
	অন্যথা যদি (leader_len > 0)
	{
	    FreeWild(অঙ্ক_matches, matches);
	    অঙ্ক_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	যদি (compl_অঙ্ক_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	ফেরত;
    }

    যদি (অঙ্ক_matches > 0)
	ins_compl_add_matches(অঙ্ক_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    স্থির শূন্য
get_পরবর্তী_cmdline_completion(শূন্য)
{
    char_u	**matches;
    পূর্ণসংখ্যা		অঙ্ক_matches;

    যদি (expand_cmdline(&compl_xp, compl_pattern.string,
		(পূর্ণসংখ্যা)compl_pattern.দৈর্ঘ্য, &অঙ্ক_matches, &matches) == EXPAND_OK)
    {
	পূর্ণসংখ্যা		add_r = OK;
	পূর্ণসংখ্যা		dir = compl_direction;

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_গুনতি]) = {ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা};

	    যদি (compl_xp.xp_files_abbr != ফাঁকা)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    যদি (compl_xp.xp_files_kind != ফাঁকা)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    যদি (compl_xp.xp_files_menu != ফাঁকা)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    যদি (compl_xp.xp_files_info != ফাঁকা)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, ফাঁকা, cptext, ফাঁকা, dir,
		    CP_FAST, FALSE, ফাঁকা, FUZZY_SCORE_NONE);
	    যদি (add_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(অঙ্ক_matches, matches);
	free_xp_files_extra(&compl_xp, অঙ্ক_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    স্থির শূন্য
get_পরবর্তী_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    পূর্ণসংখ্যা		অঙ্ক_matches;

    অঙ্ক_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    যদি (অঙ্ক_matches > 0)
	ins_compl_add_matches(অঙ্ক_matches, matches, p_ic);
    অন্যথা
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    স্থির char_u *
ins_compl_get_পরবর্তী_word_or_সারি(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	পূর্ণসংখ্যা	*match_len,
	পূর্ণসংখ্যা	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    পূর্ণসংখ্যা		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (পূর্ণসংখ্যা)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    যদি (ctrl_x_mode_সারি_or_eval())
    {
	যদি (compl_status_adding())
	{
	    যদি (cur_match_pos->lnum >= ins_buf->b_ml.ml_সারি_গুনতি)
		ফেরত ফাঁকা;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    যদি (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (পূর্ণসংখ্যা)(ptr - tmp_ptr);
	    }
	}
    }
    অন্যথা
    {
	char_u	*tmp_ptr = ptr;

	যদি (compl_status_adding() && compl_দৈর্ঘ্য <= len)
	{
	    tmp_ptr += compl_দৈর্ঘ্য;
	    // Skip if already inside a word.
	    যদি (vim_iswordp(tmp_ptr))
		ফেরত ফাঁকা;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (পূর্ণসংখ্যা)(tmp_ptr - ptr);

	যদি (compl_status_adding() && len == compl_দৈর্ঘ্য)
	{
	    যদি (cur_match_pos->lnum < ins_buf->b_ml.ml_সারি_গুনতি)
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
		যদি (tmp_ptr > ptr)
		{
		    যদি (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			যদি (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			যদি (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ফাঁকা
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    যদি (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (পূর্ণসংখ্যা)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    যদি (len == compl_দৈর্ঘ্য)
		ফেরত ফাঁকা;
	}
    }

    *match_len = len;
    ফেরত ptr;
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
    স্থির পূর্ণসংখ্যা
get_পরবর্তী_default_completion(ins_compl_পরবর্তী_state_T *st, pos_T *start_pos)
{
    পূর্ণসংখ্যা		found_new_match = FAIL;
    পূর্ণসংখ্যা		save_p_scs;
    পূর্ণসংখ্যা		save_p_ws;
    পূর্ণসংখ্যা		looped_around = FALSE;
    char_u	*ptr = ফাঁকা;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_দৈর্ঘ্য > 0;
    char_u	*leader = ins_compl_leader();
    পূর্ণসংখ্যা		score = FUZZY_SCORE_NONE;
    পূর্ণসংখ্যা		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    যদি (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    যদি (!in_curbuf)
	p_ws = FALSE;
    অন্যথা যদি (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    জন্য (;;)
    {
	পূর্ণসংখ্যা	cont_s_ipos = FALSE;

	++বার্তা_silent;  // Don't want messages for wrapscan.

	যদি (in_fuzzy_collect)
	{
	    found_new_match = অনুসন্ধান_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	অন্যথা যদি (ctrl_x_mode_whole_সারি() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = অনুসন্ধান_for_exact_সারি(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	অন্যথা
	    found_new_match = searchit(ফাঁকা, st->ins_buf, st->cur_match_pos,
				ফাঁকা, compl_direction, compl_pattern.string, (পূর্ণসংখ্যা)compl_pattern.দৈর্ঘ্য,
				1L, অনুসন্ধান_KEEP + অনুসন্ধান_NFMSG, RE_LAST, ফাঁকা);
	--বার্তা_silent;
	যদি (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	অন্যথা যদি (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	অন্যথা যদি (compl_dir_forward()
		&& (st->পূর্ব_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->পূর্ব_match_pos.lnum == st->cur_match_pos->lnum
			&& st->পূর্ব_match_pos.col >= st->cur_match_pos->col)))
	{
	    যদি (looped_around)
		found_new_match = FAIL;
	    অন্যথা
		looped_around = TRUE;
	}
	অন্যথা যদি (!compl_dir_forward()
		&& (st->পূর্ব_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->পূর্ব_match_pos.lnum == st->cur_match_pos->lnum
			&& st->পূর্ব_match_pos.col <= st->cur_match_pos->col)))
	{
	    যদি (looped_around)
		found_new_match = FAIL;
	    অন্যথা
		looped_around = TRUE;
	}
	st->পূর্ব_match_pos = *st->cur_match_pos;
	যদি (found_new_match == FAIL)
	    বিরতি;

	// when ADDING, the text before the cursor matches, skip it
	যদি (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    চালু;

	যদি (!in_fuzzy_collect)
	    ptr = ins_compl_get_পরবর্তী_word_or_সারি(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	যদি (ptr == ফাঁকা || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    চালু;

	যদি (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    যদি (score < 0)
		score = -score;
	}

	যদি (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? ফাঁকা : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    যদি (in_fuzzy_collect && score == compl_first_match->cp_পরবর্তী->cp_score)
		compl_অঙ্ক_bests++;
	    found_new_match = OK;
	    বিরতি;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    ফেরত found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    স্থির callback_T *
get_callback_if_cpt_func(char_u *p, পূর্ণসংখ্যা idx)
{
    যদি (*p == 'o')
	ফেরত &curbuf->b_ofu_cb;

    যদি (*p == 'F')
    {
	যদি (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    যদি (curbuf->b_p_cpt_cb == ফাঁকা
		    || idx < 0 || idx >= curbuf->b_p_cpt_গুনতি)
		ফেরত ফাঁকা;
	    ফেরত curbuf->b_p_cpt_cb[idx].cb_name != ফাঁকা
		? &curbuf->b_p_cpt_cb[idx] : ফাঁকা;
	}
	অন্যথা
	    ফেরত &curbuf->b_cfu_cb; // 'cfu'
    }

    ফেরত ফাঁকা;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    স্থির শূন্য
get_register_completion(শূন্য)
{
    পূর্ণসংখ্যা		dir = compl_direction;
    yankreg_T	*reg = ফাঁকা;
    শূন্য	*reg_ptr = ফাঁকা;
    পূর্ণসংখ্যা		adding_mode = compl_status_adding();

    জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_REGISTERS; i++)
    {
	পূর্ণসংখ্যা regname = 0;
	যদি (i == 0)
	    regname = '"';    // unnamed register
	অন্যথা যদি (i < 10)
	    regname = '0' + i;
	অন্যথা যদি (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	অন্যথা যদি (i == STAR_REGISTER)
	    regname = '*';
	অন্যথা যদি (i == PLUS_REGISTER)
	    regname = '+';
#endif
	অন্যথা
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	যদি (!valid_yank_reg(regname, FALSE) || regname == '_')
	    চালু;

	reg_ptr = get_register(regname, FALSE);
	যদি (reg_ptr == ফাঁকা)
	    চালু;

	reg = (yankreg_T *)reg_ptr;

	জন্য (পূর্ণসংখ্যা j = 0; j < reg->y_আকার; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    যদি (str == ফাঁকা)
		চালু;

	    যদি (adding_mode)
	    {
		পূর্ণসংখ্যা str_len = (পূর্ণসংখ্যা)STRLEN(str);
		যদি (str_len == 0)
		    চালু;

		যদি (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.দৈর্ঘ্য) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.দৈর্ঘ্য) == 0))
		{
		    যদি (ins_compl_add_infercase(str, str_len, p_ic, ফাঁকা,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    অন্যথা
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		যখন (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    যদি (p >= str_end || *p == NUL)
			বিরতি;

		    char_u *word_end = find_word_end(p);

		    যদি (word_end <= p)
		    {
			যদি (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			অন্যথা
			    word_end = p + 1;
		    }

		    যদি (word_end > str_end)
			word_end = str_end;

		    পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)(word_end - p);
		    যদি (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.দৈর্ঘ্য) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.দৈর্ঘ্য) == 0)))
		    {
			যদি (ins_compl_add_infercase(p, len, p_ic, ফাঁকা,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    যদি (p <= old_p)
		    {
			p = old_p + 1;
			যদি (has_mbyte && p < str_end)
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
    স্থির পূর্ণসংখ্যা
get_পরবর্তী_completion_match(পূর্ণসংখ্যা type, ins_compl_পরবর্তী_state_T *st, pos_T *ini)
{
    পূর্ণসংখ্যা	found_new_match = FALSE;

    নির্বাচন (type)
    {
	ক্ষেত্র -1:
	    বিরতি;
#ifdef FEAT_FIND_ID
	ক্ষেত্র CTRL_X_PATH_PATTERNS:
	ক্ষেত্র CTRL_X_PATH_DEFINES:
	    get_পরবর্তী_include_file_completion(type);
	    বিরতি;
#endif

	ক্ষেত্র CTRL_X_DICTIONARY:
	ক্ষেত্র CTRL_X_THESAURUS:
	    get_পরবর্তী_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ফাঁকা;
	    বিরতি;

	ক্ষেত্র CTRL_X_TAGS:
	    get_পরবর্তী_tag_completion();
	    বিরতি;

	ক্ষেত্র CTRL_X_FILES:
	    get_পরবর্তী_filename_completion();
	    বিরতি;

	ক্ষেত্র CTRL_X_CMDLINE:
	ক্ষেত্র CTRL_X_CMDLINE_CTRL_X:
	    get_পরবর্তী_cmdline_completion();
	    বিরতি;

#ifdef FEAT_COMPL_FUNC
	ক্ষেত্র CTRL_X_FUNCTION:
	    যদি (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    অন্যথা
		expand_by_function(type, compl_pattern.string, ফাঁকা);
	    বিরতি;
	ক্ষেত্র CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, ফাঁকা);
	    বিরতি;
#endif

	ক্ষেত্র CTRL_X_SPELL:
	    get_পরবর্তী_spell_completion(st->first_match_pos.lnum);
	    বিরতি;

	ক্ষেত্র CTRL_X_REGISTER:
	    get_register_completion();
	    বিরতি;

	পূর্বনির্ধারিত:	// normal ^P/^N and ^X^L
	    found_new_match = get_পরবর্তী_default_completion(st, ini);
	    যদি (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    যদি (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    ফেরত found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    স্থির শূন্য
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *পঠ = str, *লিখ = str, *p;

    যদি (str == ফাঁকা)
	ফেরত;

    যখন (*পঠ)
    {
	যদি (*পঠ == '^')
	{
	    p = পঠ + 1;
	    যখন (vim_isdigit(*p))
		p++;
	    যদি ((*p == ',' || *p == '\0') && p != পঠ + 1)
	    {
		পঠ = p;
		চালু;
	    }
	    অন্যথা
		*লিখ++ = *পঠ++;
	}
	অন্যথা
	    *লিখ++ = *পঠ++;
    }
    *লিখ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    স্থির পূর্ণসংখ্যা
prepare_cpt_compl_funcs(শূন্য)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ফাঁকা;
    পূর্ণসংখ্যা		idx = 0;
    পূর্ণসংখ্যা		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    যদি (cpt == ফাঁকা)
	ফেরত FAIL;
    strip_caret_numbers_in_place(cpt);

    জন্য (p = cpt; *p;)
    {
	যখন (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	যদি (*p == NUL)
	    বিরতি;

	cb = get_callback_if_cpt_func(p, idx);
	যদি (cb)
	{
	    যদি (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		যদি (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		অন্যথা
		    startcol = -2;
	    }
	    অন্যথা যদি (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	অন্যথা
	    cpt_sources_array[idx].cs_startcol = -3;

	(শূন্য)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    ফেরত OK;
#endif
    ফেরত FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    স্থির শূন্য
compl_source_start_timer(পূর্ণসংখ্যা source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    যদি (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    স্থির পূর্ণসংখ্যা
advance_cpt_sources_সূচক_safe(শূন্য)
{
    যদি (cpt_sources_সূচক >= 0 && cpt_sources_সূচক < cpt_sources_গুনতি - 1)
    {
	cpt_sources_সূচক++;
	ফেরত OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_সূচক_out_of_range_nr), cpt_sources_সূচক);
#endif
    ফেরত FAIL;
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
    স্থির পূর্ণসংখ্যা
ins_compl_get_exp(pos_T *ini)
{
    স্থির ins_compl_পরবর্তী_state_T   st;
    স্থির পূর্ণসংখ্যা			    st_cleared = FALSE;
    পূর্ণসংখ্যা		match_গুনতি;
    পূর্ণসংখ্যা		found_new_match;
    পূর্ণসংখ্যা		type = ctrl_x_mode;
    পূর্ণসংখ্যা		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    পূর্ণসংখ্যা		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    যদি (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	যদি (!st_cleared)
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
	st.e_cpt = st.e_cpt_copy == ফাঁকা ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_সারি_গুনতি	1000
	যদি (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_সারি_গুনতি);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    অন্যথা যদি (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_সারি_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != ফাঁকা;
    যদি (normal_mode_strict)
    {
	cpt_sources_সূচক = 0;
	যদি (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    জন্য (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	যদি ((ctrl_x_mode_normal() || ctrl_x_mode_সারি_or_eval())
					&& (!compl_started || st.found_all))
	{
	    পূর্ণসংখ্যা status = process_পরবর্তী_cpt_মান(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    যদি (status == INS_COMPL_CPT_END)
		বিরতি;
	    যদি (status == INS_COMPL_CPT_CONT)
	    {
		যদি (may_advance_cpt_idx)
		{
		    যদি (!advance_cpt_sources_সূচক_safe())
			বিরতি;
		    compl_source_start_timer(cpt_sources_সূচক);
		}
		চালু;
	    }
	}

	যদি (normal_mode_strict && type == CTRL_X_FUNCTION
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
	found_new_match = get_পরবর্তী_completion_match(type, &st, &start_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	যদি (compl_pattern.string == ফাঁকা)
	    বিরতি;

	যদি (may_advance_cpt_idx)
	{
	    যদি (!advance_cpt_sources_সূচক_safe())
		বিরতি;
	    compl_source_start_timer(cpt_sources_সূচক);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	যদি ((ctrl_x_mode_not_default() && !ctrl_x_mode_সারি_or_eval())
						|| found_new_match != FAIL)
	{
	    যদি (got_int)
		বিরতি;
	    // Fill the popup menu as soon as possible.
	    যদি (type != -1)
		ins_compl_check_চাবিগুলি(0, FALSE);

	    যদি ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_সারি_or_eval()) || compl_interrupted)
		বিরতি;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	অন্যথা
	{
	    // Mark a buffer scanned when it has been scanned completely
	    যদি (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	যদি (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	যদি (!compl_dir_forward())
	    যখন (compl_curr_match->cp_পূর্ব
		    && !match_at_original_text(compl_curr_match->cp_পূর্ব))
		compl_curr_match = compl_curr_match->cp_পূর্ব;
    }
    cpt_sources_সূচক = -1;
    compl_started = TRUE;

    যদি ((ctrl_x_mode_normal() || ctrl_x_mode_সারি_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_গুনতি = -1;		// total of matches, unknown
    যদি (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_সারি_or_eval()))
	match_গুনতি = ins_compl_make_cyclic();

    যদি (cot_fuzzy() && compl_get_longest && compl_অঙ্ক_bests > 0)
	fuzzy_longest_match();

    যদি (compl_old_match != ফাঁকা)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_পরবর্তী
						    : compl_old_match->cp_পূর্ব;
	যদি (compl_curr_match == ফাঁকা)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    যদি (match_গুনতি > 0 && !ctrl_x_mode_spell())
    {
	যদি (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	যদি (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    ফেরত match_গুনতি;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    স্থির শূন্য
ins_compl_update_shown_match(শূন্য)
{
    string_T	*leader;

    (শূন্য)get_leader_for_startcol(ফাঁকা, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    যখন (!ins_compl_equal(compl_shown_match,
		leader->string, (পূর্ণসংখ্যা)leader->দৈর্ঘ্য)
	    && compl_shown_match->cp_পরবর্তী != ফাঁকা
	    && !is_first_match(compl_shown_match->cp_পরবর্তী))
    {
	compl_shown_match = compl_shown_match->cp_পরবর্তী;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    যদি (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (পূর্ণসংখ্যা)leader->দৈর্ঘ্য)
	    && (compl_shown_match->cp_পরবর্তী == ফাঁকা
		|| is_first_match(compl_shown_match->cp_পরবর্তী)))
    {
	যখন (!ins_compl_equal(compl_shown_match,
		    leader->string, (পূর্ণসংখ্যা)leader->দৈর্ঘ্য)
		&& compl_shown_match->cp_পূর্ব != ফাঁকা
		&& !is_first_match(compl_shown_match->cp_পূর্ব))
	{
	    compl_shown_match = compl_shown_match->cp_পূর্ব;
	    leader = get_leader_for_startcol(compl_shown_match, TRUE);
	}
    }
}

/*
 * Delete the old text being completed.
 */
    শূন্য
ins_compl_delete(শূন্য)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    পূর্ণসংখ্যা col = compl_col + (compl_status_adding() ? compl_দৈর্ঘ্য : 0);
    string_T	remaining = {ফাঁকা, 0};
    পূর্ণসংখ্যা	    orig_col;

    যদি (ins_compl_preinsert_effect())
    {
	col += (পূর্ণসংখ্যা)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    যদি (curwin->w_cursor.lnum > compl_lnum)
    {
	যদি (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *সারি = ml_get_cursor();
	    remaining.দৈর্ঘ্য = ml_get_cursor_len();
	    remaining.string = vim_strnsave(সারি, remaining.দৈর্ঘ্য);
	    যদি (remaining.string == ফাঁকা)
		ফেরত;
	}
	যখন (curwin->w_cursor.lnum > compl_lnum)
	{
	    যদি (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		যদি (remaining.string)
		    vim_free(remaining.string);
		ফেরত;
	    }
	    deleted_সারিগুলি_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    যদি ((পূর্ণসংখ্যা)curwin->w_cursor.col > col)
    {
	যদি (stop_arrow() == FAIL)
	{
	    যদি (remaining.string)
		vim_free(remaining.string);
	    ফেরত;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    যদি (remaining.string != ফাঁকা)
    {
	orig_col = curwin->w_cursor.col;
	ins_str(remaining.string, remaining.দৈর্ঘ্য);
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
    স্থির শূন্য
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    পূর্ণসংখ্যা		base_indent = get_indent();

    যখন (*curr != NUL)
    {
	যদি (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    যদি (curr > start)
		ins_char_bytes(start, (পূর্ণসংখ্যা)(curr - start));

	    // Handle newline
	    open_সারি(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, ফাঁকা);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    যদি (curr > start)
	ins_char_bytes(start, (পূর্ণসংখ্যা)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    স্থির char_u *
find_common_prefix(size_t *prefix_len, পূর্ণসংখ্যা curbuf_only)
{
    compl_T     *compl;
    পূর্ণসংখ্যা		*match_গুনতি;
    char_u	*first = ফাঁকা;
    পূর্ণসংখ্যা		len = -1;
    পূর্ণসংখ্যা		is_cpt_completion = (cpt_sources_array != ফাঁকা);

    যদি (!is_cpt_completion)
	ফেরত ফাঁকা;

    match_গুনতি = ALLOC_CLEAR_MULT(পূর্ণসংখ্যা, cpt_sources_গুনতি);
    যদি (match_গুনতি == ফাঁকা)
	ফেরত ফাঁকা;

    (শূন্য)get_leader_for_startcol(ফাঁকা, TRUE); // Clear the cache

    compl = compl_first_match;
    করো
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	যদি (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	যদি (!match_at_original_text(compl)
		&& (leader->string == ফাঁকা
		    || ins_compl_equal_sc(compl, leader->string,
			(পূর্ণসংখ্যা)leader->দৈর্ঘ্য)))
	{
	    // Limit number of items from each source if max_items is set.
	    পূর্ণসংখ্যা match_limit_exceeded = FALSE;
	    পূর্ণসংখ্যা cur_source = compl->cp_cpt_source_idx;

	    যদি (cur_source != -1)
	    {
		match_গুনতি[cur_source]++;
		পূর্ণসংখ্যা max_matches = cpt_sources_array[cur_source].cs_max_matches;
		যদি (max_matches > 0 && match_গুনতি[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    যদি (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		যদি (first == ফাঁকা && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (পূর্ণসংখ্যা)STRLEN(first);
		}
		অন্যথা যদি (first != ফাঁকা)
		{
		    পূর্ণসংখ্যা j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    যখন (j < len && *s1 != NUL && *s2 != NUL)
		    {
			যদি (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    বিরতি;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    যদি (len == 0)
			বিরতি;
		}
	    }
	}
	compl = compl->cp_পরবর্তী;
    } যখন (compl != ফাঁকা && !is_first_match(compl));

    vim_free(match_গুনতি);

    যদি (len > (পূর্ণসংখ্যা)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	যদি (len == (পূর্ণসংখ্যা)STRLEN(first))
	{
	    char_u *সারি = ml_get_curline();
	    char_u *p = সারি + curwin->w_cursor.col;
	    যদি (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		পূর্ণসংখ্যা text_len = end - p;
		যদি (text_len > 0
			&& text_len < (len - (পূর্ণসংখ্যা)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	ফেরত first;
    }
    ফেরত ফাঁকা;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    শূন্য
ins_compl_ঢোকাও(পূর্ণসংখ্যা move_cursor, পূর্ণসংখ্যা ঢোকাও_prefix)
{
    পূর্ণসংখ্যা		compl_len = get_compl_len();
    পূর্ণসংখ্যা		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.দৈর্ঘ্য;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    যদি (ঢোকাও_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	যদি (cp_str == ফাঁকা)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    যদি (cp_str == ফাঁকা)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.দৈর্ঘ্য;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    অন্যথা যদি (cpt_sources_array != ফাঁকা)
    {
	পূর্ণসংখ্যা	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	যদি (cpt_idx >= 0 && compl_col >= 0)
	{
	    পূর্ণসংখ্যা	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    যদি (startcol >= 0 && startcol < (পূর্ণসংখ্যা)compl_col)
	    {
		পূর্ণসংখ্যা skip = (পূর্ণসংখ্যা)compl_col - startcol;
		যদি ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    যদি (compl_len < (পূর্ণসংখ্যা)cp_str_len)
    {
	যদি (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	অন্যথা
	{
	    ins_compl_ঢোকাও_bytes(cp_str + compl_len,
		    ঢোকাও_prefix ? (পূর্ণসংখ্যা)cp_str_len - compl_len : -1);
	    যদি ((preinsert || ঢোকাও_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    যদি (match_at_original_text(compl_shown_match)
	    || (preinsert && !ঢোকাও_prefix))
	compl_used_match = FALSE;
    অন্যথা
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = ঢোকাও_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    স্থির শূন্য
ins_compl_show_filename(শূন্য)
{
    অক্ষর	*lead = _("match in file");
    পূর্ণসংখ্যা		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    যদি (space <= 0)
	ফেরত;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    জন্য (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	যখন (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    যদি (!compl_autocomplete)
    {
	বার্তা_hist_off = TRUE;
	vim_snprintf((অক্ষর *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	বার্তা((অক্ষর *)IObuff);
	বার্তা_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    স্থির compl_T *
find_পরবর্তী_match_in_menu(শূন্য)
{
    পূর্ণসংখ্যা	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    করো
	match = is_forward ? match->cp_পরবর্তী : match->cp_পূর্ব;
    যখন (match->cp_পরবর্তী && !match->cp_in_match_array
	    && !match_at_original_text(match));
    ফেরত match;
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
    স্থির পূর্ণসংখ্যা
find_পরবর্তী_completion_match(
	পূর্ণসংখ্যা	allow_get_expansion,
	পূর্ণসংখ্যা	todo,		// repeat completion this many times
	পূর্ণসংখ্যা	advance,
	পূর্ণসংখ্যা	*অঙ্ক_matches)
{
    পূর্ণসংখ্যা		found_end = FALSE;
    compl_T	*found_compl = ফাঁকা;
    unsigned পূর্ণসংখ্যা cur_cot_flags = get_cot_flags();
    পূর্ণসংখ্যা		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    যখন (--todo >= 0)
    {
	যদি (compl_shows_dir_forward() && compl_shown_match->cp_পরবর্তী != ফাঁকা)
	{
	    যদি (compl_match_array != ফাঁকা)
		compl_shown_match = find_পরবর্তী_match_in_menu();
	    অন্যথা
		compl_shown_match = compl_shown_match->cp_পরবর্তী;
	    found_end = (compl_first_match != ফাঁকা
		    && (is_first_match(compl_shown_match->cp_পরবর্তী)
			|| is_first_match(compl_shown_match)));
	}
	অন্যথা যদি (compl_shows_dir_backward()
		&& compl_shown_match->cp_পূর্ব != ফাঁকা)
	{
	    found_end = is_first_match(compl_shown_match);
	    যদি (compl_match_array != ফাঁকা)
		compl_shown_match = find_পরবর্তী_match_in_menu();
	    অন্যথা
		compl_shown_match = compl_shown_match->cp_পূর্ব;
	    found_end |= is_first_match(compl_shown_match);
	}
	অন্যথা
	{
	    যদি (!allow_get_expansion)
	    {
		যদি (advance)
		{
		    যদি (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    অন্যথা
			compl_pending += todo + 1;
		}
		ফেরত -1;
	    }

	    যদি (!compl_no_select && advance)
	    {
		যদি (compl_shows_dir_backward())
		    --compl_pending;
		অন্যথা
		    ++compl_pending;
	    }

	    // Find matches.
	    *অঙ্ক_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    যখন (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		যদি (compl_pending > 0 && compl_shown_match->cp_পরবর্তী != ফাঁকা)
		{
		    compl_shown_match = compl_shown_match->cp_পরবর্তী;
		    --compl_pending;
		}
		অন্যথা যদি (compl_pending < 0 && compl_shown_match->cp_পূর্ব != ফাঁকা)
		{
		    compl_shown_match = compl_shown_match->cp_পূর্ব;
		    ++compl_pending;
		}
		অন্যথা
		    বিরতি;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	যদি (!match_at_original_text(compl_shown_match)
		&& leader->string != ফাঁকা
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (পূর্ণসংখ্যা)leader->দৈর্ঘ্য)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	অন্যথা
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	যদি (found_end)
	{
	    যদি (found_compl != ফাঁকা)
	    {
		compl_shown_match = found_compl;
		বিরতি;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
ins_compl_পরবর্তী(
    পূর্ণসংখ্যা	    allow_get_expansion,
    পূর্ণসংখ্যা	    গুনতি,		// repeat completion this many times; should
				// be at least 1
    পূর্ণসংখ্যা	    ঢোকাও_match)	// Insert the newly selected match
{
    পূর্ণসংখ্যা	    অঙ্ক_matches = -1;
    পূর্ণসংখ্যা	    todo = গুনতি;
    পূর্ণসংখ্যা	    advance;
    পূর্ণসংখ্যা	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned পূর্ণসংখ্যা cur_cot_flags = get_cot_flags();
    পূর্ণসংখ্যা	    compl_no_ঢোকাও = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    পূর্ণসংখ্যা	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    যদি (compl_shown_match == ফাঁকা)
	ফেরত -1;

    যদি (compl_leader.string != ফাঁকা
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    যদি (allow_get_expansion && ঢোকাও_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = গুনতি != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    যদি (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    যদি (find_পরবর্তী_completion_match(allow_get_expansion, todo, advance,
							&অঙ্ক_matches) == -1)
	ফেরত -1;

    যদি (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	ফেরত -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    যদি (!started && ins_compl_preinsert_longest())
	ins_compl_ঢোকাও(TRUE, TRUE);
    অন্যথা যদি (compl_no_ঢোকাও && !started && !compl_preinsert)
    {
	ins_compl_ঢোকাও_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    অন্যথা যদি (ঢোকাও_match)
    {
	যদি (!compl_get_longest || compl_used_match)
	{
	    পূর্ণসংখ্যা preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_ঢোকাও(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	অন্যথা
	    ins_compl_ঢোকাও_bytes(compl_leader.string + get_compl_len(), -1);
    }
    অন্যথা
	compl_used_match = FALSE;

    যদি (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	যদি (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	অন্যথা
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	যদি (gui.in_use)
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
    যদি (compl_no_ঢোকাও && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    অন্যথা
	compl_enter_selects = !ঢোকাও_match && compl_match_array != ফাঁকা;

    // Show the file name for the match (if any)
    যদি (compl_shown_match->cp_fname != ফাঁকা)
	ins_compl_show_filename();

    ফেরত অঙ্ক_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    স্থির শূন্য
check_elapsed_time(শূন্য)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_সূচক].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    যদি (elapsed_ms > compl_timeout_ms)
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
    শূন্য
ins_compl_check_চাবিগুলি(পূর্ণসংখ্যা frequency, পূর্ণসংখ্যা in_compl_func)
{
    স্থির পূর্ণসংখ্যা	গুনতি = 0;
    পূর্ণসংখ্যা		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    যদি (!in_compl_func && (using_script() || ex_normal_busy))
	ফেরত;

    // Only do this at regular intervals
    যদি (++গুনতি < frequency)
	ফেরত;
    গুনতি = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    যদি (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	যদি (vim_is_ctrl_x_চাবি(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (শূন্য)ins_compl_পরবর্তী(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	অন্যথা
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    যদি (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		যদি (c != Ctrl_R && (চাবি_Typed
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    অন্যথা
    {
	পূর্ণসংখ্যা normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_সারি_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != ফাঁকা && cpt_sources_সূচক >= 0;
	যদি (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    যদি (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	পূর্ণসংখ্যা todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(শূন্য)ins_compl_পরবর্তী(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_key2dir(পূর্ণসংখ্যা c)
{
    যদি (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	ফেরত BACKWARD;
    ফেরত FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_pum_চাবি(পূর্ণসংখ্যা c)
{
    ফেরত pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_key2count(পূর্ণসংখ্যা c)
{
    যদি (ins_compl_pum_চাবি(c) && c != K_UP && c != K_DOWN)
    {
	পূর্ণসংখ্যা h = pum_get_height();
	যদি (h > 3)
	    h -= 2; // keep some context
	ফেরত h;
    }
    ফেরত 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    স্থির পূর্ণসংখ্যা
ins_compl_use_match(পূর্ণসংখ্যা c)
{
    নির্বাচন (c)
    {
	ক্ষেত্র K_UP:
	ক্ষেত্র K_DOWN:
	ক্ষেত্র K_PAGEDOWN:
	ক্ষেত্র K_KPAGEDOWN:
	ক্ষেত্র K_S_DOWN:
	ক্ষেত্র K_PAGEUP:
	ক্ষেত্র K_KPAGEUP:
	ক্ষেত্র K_S_UP:
	    ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    স্থির পূর্ণসংখ্যা
get_normal_compl_info(char_u *সারি, পূর্ণসংখ্যা startcol, colnr_T curs_col)
{
    যদি ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	যদি (!compl_status_adding())
	{
	    যখন (--startcol >= 0 && vim_isIDc(সারি[startcol]))
		;
	    compl_col += ++startcol;
	    compl_দৈর্ঘ্য = curs_col - startcol;
	}
	যদি (p_ic)
	{
	    compl_pattern.string = str_foldcase(সারি + compl_col,
		    compl_দৈর্ঘ্য, ফাঁকা, 0);
	    যদি (compl_pattern.string == ফাঁকা)
	    {
		compl_pattern.দৈর্ঘ্য = 0;
		ফেরত FAIL;
	    }
	    compl_pattern.দৈর্ঘ্য = STRLEN(compl_pattern.string);
	}
	অন্যথা
	{
	    compl_pattern.string = vim_strnsave(সারি + compl_col, (size_t)compl_দৈর্ঘ্য);
	    যদি (compl_pattern.string == ফাঁকা)
	    {
		compl_pattern.দৈর্ঘ্য = 0;
		ফেরত FAIL;
	    }
	    compl_pattern.দৈর্ঘ্য = (size_t)compl_দৈর্ঘ্য;
	}
    }
    অন্যথা যদি (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	যদি (!vim_iswordp(সারি + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(সারি, সারি + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ফাঁকা, সারি + compl_col, compl_দৈর্ঘ্য) + prefixlen;
	compl_pattern.string = alloc(n);
	যদি (compl_pattern.string == ফাঁকা)
	{
	    compl_pattern.দৈর্ঘ্য = 0;
	    ফেরত FAIL;
	}
	STRCPY((অক্ষর *)compl_pattern.string, prefix);
	(শূন্য)quote_meta(compl_pattern.string + prefixlen,
		সারি + compl_col, compl_দৈর্ঘ্য);
	compl_pattern.দৈর্ঘ্য = n - 1;
    }
    অন্যথা যদি (--startcol < 0
	    || !vim_iswordp(mb_prevptr(সারি, সারি + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	যদি (compl_pattern.string == ফাঁকা)
	{
	    compl_pattern.দৈর্ঘ্য = 0;
	    ফেরত FAIL;
	}
	compl_pattern.দৈর্ঘ্য = len;
	compl_col += curs_col;
	compl_দৈর্ঘ্য = 0;
	compl_from_nonkeyword = TRUE;
    }
    অন্যথা
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা base_class;
	    পূর্ণসংখ্যা শীর্ষ_off;

	    startcol -= (*mb_শীর্ষ_off)(সারি, সারি + startcol);
	    base_class = mb_get_class(সারি + startcol);
	    যখন (--startcol >= 0)
	    {
		শীর্ষ_off = (*mb_শীর্ষ_off)(সারি, সারি + startcol);
		যদি (base_class != mb_get_class(সারি + startcol
			    - শীর্ষ_off))
		    বিরতি;
		startcol -= শীর্ষ_off;
	    }
	}
	অন্যথা
	{
	    যখন (--startcol >= 0 && vim_iswordc(সারি[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_দৈর্ঘ্য = (পূর্ণসংখ্যা)curs_col - startcol;
	যদি (compl_দৈর্ঘ্য == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    যদি (compl_pattern.string == ফাঁকা)
	    {
		compl_pattern.দৈর্ঘ্য = 0;
		ফেরত FAIL;
	    }
	    STRCPY((অক্ষর *)compl_pattern.string, "\\<");
	    (শূন্য)quote_meta(compl_pattern.string + 2, সারি + compl_col, 1);
	    STRCAT((অক্ষর *)compl_pattern.string, "\\k");
	    compl_pattern.দৈর্ঘ্য = STRLEN(compl_pattern.string);
	}
	অন্যথা
	{
	    size_t  n = quote_meta(ফাঁকা, সারি + compl_col, compl_দৈর্ঘ্য) + 2;

	    compl_pattern.string = alloc(n);
	    যদি (compl_pattern.string == ফাঁকা)
	    {
		compl_pattern.দৈর্ঘ্য = 0;
		ফেরত FAIL;
	    }
	    STRCPY((অক্ষর *)compl_pattern.string, "\\<");
	    (শূন্য)quote_meta(compl_pattern.string + 2, সারি + compl_col,
		    compl_দৈর্ঘ্য);
	    compl_pattern.দৈর্ঘ্য = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    যদি (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	যদি (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    স্থির পূর্ণসংখ্যা
get_wholeline_compl_info(char_u *সারি, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(সারি);
    compl_দৈর্ঘ্য = (পূর্ণসংখ্যা)curs_col - (পূর্ণসংখ্যা)compl_col;
    যদি (compl_দৈর্ঘ্য < 0)	// cursor in indent: empty pattern
	compl_দৈর্ঘ্য = 0;
    যদি (p_ic)
    {
	compl_pattern.string = str_foldcase(সারি + compl_col, compl_দৈর্ঘ্য,
		ফাঁকা, 0);
	যদি (compl_pattern.string == ফাঁকা)
	{
	    compl_pattern.দৈর্ঘ্য = 0;
	    ফেরত FAIL;
	}
	compl_pattern.দৈর্ঘ্য = STRLEN(compl_pattern.string);
    }
    অন্যথা
    {
	compl_pattern.string = vim_strnsave(সারি + compl_col, (size_t)compl_দৈর্ঘ্য);
	যদি (compl_pattern.string == ফাঁকা)
	{
	    compl_pattern.দৈর্ঘ্য = 0;
	    ফেরত FAIL;
	}
	compl_pattern.দৈর্ঘ্য = (size_t)compl_দৈর্ঘ্য;
    }

    ফেরত OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    স্থির পূর্ণসংখ্যা
get_filename_compl_info(char_u *সারি, পূর্ণসংখ্যা startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    যদি (startcol > 0)
    {
	char_u	*p = সারি + startcol;

	MB_PTR_BACK(সারি, p);
	যখন (p > সারি && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(সারি, p);
	যদি (p == সারি && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	অন্যথা
	    startcol = (পূর্ণসংখ্যা)(p - সারি) + 1;
    }

    compl_col += startcol;
    compl_দৈর্ঘ্য = (পূর্ণসংখ্যা)curs_col - startcol;
    compl_pattern.string = addstar(সারি + compl_col, compl_দৈর্ঘ্য, EXPAND_FILES);
    যদি (compl_pattern.string == ফাঁকা)
    {
	compl_pattern.দৈর্ঘ্য = 0;
	ফেরত FAIL;
    }

    compl_pattern.দৈর্ঘ্য = STRLEN(compl_pattern.string);

    ফেরত OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    স্থির পূর্ণসংখ্যা
get_cmdline_compl_info(char_u *সারি, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(সারি, curs_col);
    যদি (compl_pattern.string == ফাঁকা)
    {
	compl_pattern.দৈর্ঘ্য = 0;
	ফেরত FAIL;
    }
    compl_pattern.দৈর্ঘ্য = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (পূর্ণসংখ্যা)compl_pattern.দৈর্ঘ্য, curs_col, FALSE);
    যদি (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    অন্যথা
	compl_col = (পূর্ণসংখ্যা)(compl_xp.xp_pattern - compl_pattern.string);
    compl_দৈর্ঘ্য = curs_col - compl_col;

    ফেরত OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    স্থির পূর্ণসংখ্যা
set_compl_globals(
    পূর্ণসংখ্যা		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    পূর্ণসংখ্যা		is_cpt_compl UNUSED)
{
    যদি (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	যদি (startcol < compl_col)
	    ফেরত prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	অন্যথা
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.দৈর্ঘ্য);
	    cpt_compl_pattern.দৈর্ঘ্য = compl_orig_text.দৈর্ঘ্য;
	}
    }
    অন্যথা
    {
	যদি (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*সারি = ml_get(curwin->w_cursor.lnum);
	পূর্ণসংখ্যা	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(সারি + startcol, (size_t)len);
	যদি (compl_pattern.string == ফাঁকা)
	{
	    compl_pattern.দৈর্ঘ্য = 0;
	    ফেরত FAIL;
	}
	compl_pattern.দৈর্ঘ্য = (size_t)len;
	compl_col = startcol;
	compl_দৈর্ঘ্য = len;
    }

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    পূর্ণসংখ্যা		*startcol UNUSED)
{
    পূর্ণসংখ্যা		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    পূর্ণসংখ্যা		col;
    char_u	*funcname = ফাঁকা;
    pos_T	pos;
    পূর্ণসংখ্যা		save_State = State;
    পূর্ণসংখ্যা		is_cpt_function = (cb != ফাঁকা);
    পূর্ণসংখ্যা		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    যদি (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	যদি (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    ফেরত FAIL;
	}
	cb = get_ঢোকাও_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_সংখ্যা;
    args[0].vval.v_সংখ্যা = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    যদি (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    যদি (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    যদি (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	ফেরত FAIL;
    }

    যদি (startcol != ফাঁকা)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    যদি (col == -2 || aborting())
	ফেরত FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    যদি (col == -3)
    {
	যদি (is_cpt_function)
	    ফেরত FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = ফাঁকা;
	যদি (!shortmess(SHM_COMPLETIONMENU))
	    বার্তা_clr_cmdline();
	ফেরত FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    ফেরত ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    স্থির পূর্ণসংখ্যা
get_spell_compl_info(পূর্ণসংখ্যা startcol UNUSED, colnr_T curs_col UNUSED)
{
    পূর্ণসংখ্যা		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*সারি = ফাঁকা;

    যদি (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    অন্যথা
	compl_col = spell_word_start(startcol);

    যদি (compl_col >= (colnr_T)startcol)
    {
	compl_দৈর্ঘ্য = 0;
	compl_col = curs_col;
    }
    অন্যথা
    {
	spell_expand_check_cap(compl_col);
	compl_দৈর্ঘ্য = (পূর্ণসংখ্যা)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    সারি = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(সারি + compl_col, (size_t)compl_দৈর্ঘ্য);
    যদি (compl_pattern.string == ফাঁকা)
    {
	compl_pattern.দৈর্ঘ্য = 0;
	ফেরত FAIL;
    }

    compl_pattern.দৈর্ঘ্য = (size_t)compl_দৈর্ঘ্য;
    ret = OK;
#endif

    ফেরত ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    স্থির পূর্ণসংখ্যা
compl_get_info(char_u *সারি, পূর্ণসংখ্যা startcol, colnr_T curs_col, পূর্ণসংখ্যা *সারি_invalid)
{
    যদি (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	যদি (get_normal_compl_info(সারি, startcol, curs_col) != OK)
	    ফেরত FAIL;
	*সারি_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    অন্যথা যদি (ctrl_x_mode_সারি_or_eval())
    {
	ফেরত get_wholeline_compl_info(সারি, curs_col);
    }
    অন্যথা যদি (ctrl_x_mode_files())
    {
	ফেরত get_filename_compl_info(সারি, startcol, curs_col);
    }
    অন্যথা যদি (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	ফেরত get_cmdline_compl_info(সারি, curs_col);
    }
    অন্যথা যদি (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	যদি (get_userdefined_compl_info(curs_col, ফাঁকা, ফাঁকা) != OK)
	    ফেরত FAIL;
	*সারি_invalid = TRUE;	// "line" may have become invalid
    }
    অন্যথা যদি (ctrl_x_mode_spell())
    {
	যদি (get_spell_compl_info(startcol, curs_col) == FAIL)
	    ফেরত FAIL;
	*সারি_invalid = TRUE;	// "line" may have become invalid
    }
    অন্যথা
    {
	internal_error("ins_complete()");
	ফেরত FAIL;
    }

    ফেরত OK;
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
    স্থির শূন্য
ins_compl_continue_অনুসন্ধান(char_u *সারি)
{
    // it is a continued search
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    যদি (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	যদি (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(সারি);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	অন্যথা
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    যদি (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    সারি + compl_দৈর্ঘ্য
			    + compl_startpos.col) - সারি);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_দৈর্ঘ্য = curwin->w_cursor.col - (পূর্ণসংখ্যা)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	যদি (compl_দৈর্ঘ্য > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_দৈর্ঘ্য = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_দৈর্ঘ্য;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	যদি (compl_দৈর্ঘ্য < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    অন্যথা যদি (ctrl_x_mode_সারি_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    অন্যথা
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    স্থির পূর্ণসংখ্যা
ins_compl_start(শূন্য)
{
    char_u	*সারি = ফাঁকা;
    পূর্ণসংখ্যা		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    পূর্ণসংখ্যা		সারি_invalid = FALSE;
    পূর্ণসংখ্যা		save_did_ai = did_ai;
    পূর্ণসংখ্যা		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    যদি (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	ফেরত FAIL;
    }

    সারি = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    যদি ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_অনুসন্ধান(সারি);
    অন্যথা
	compl_cont_status &= CONT_LOCAL;

    যদি (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	যদি (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (পূর্ণসংখ্যা)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    যদি (compl_get_info(সারি, startcol, curs_col, &সারি_invalid) == FAIL)
    {
	যদি (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	ফেরত FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    যদি (সারি_invalid)
	সারি = ml_get(curwin->w_cursor.lnum);

    যদি (compl_status_adding())
    {
	যদি (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	যদি (ctrl_x_mode_সারি_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.col = compl_col;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_দৈর্ঘ্য = 0;
	    compl_col = curwin->w_cursor.col;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    অন্যথা
    {
	edit_submode_pre = ফাঁকা;
	compl_startpos.col = compl_col;
    }

    যদি (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	যদি (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_বার্তা]);
	অন্যথা
	    edit_submode = (char_u *)_(CTRL_X_বার্তা(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ফাঁকা);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.দৈর্ঘ্য = (size_t)compl_দৈর্ঘ্য;
    compl_orig_text.string = vim_strnsave(সারি + compl_col, (size_t)compl_দৈর্ঘ্য);
    যদি (p_ic)
	flags |= CP_ICASE;
    যদি (compl_orig_text.string == ফাঁকা
	    || ins_compl_add(compl_orig_text.string,
		(পূর্ণসংখ্যা)compl_orig_text.দৈর্ঘ্য,
		ফাঁকা, ফাঁকা, ফাঁকা, 0, flags, FALSE, ফাঁকা, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	ফেরত FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    যদি (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_গুনতি;
	showmode();
	edit_submode_extra = ফাঁকা;
	out_flush();
    }

    did_ai = save_did_ai;
    ফেরত OK;
}

/*
 * display the completion status message
 */
    স্থির শূন্য
ins_compl_show_statusmsg(শূন্য)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    যদি (is_first_match(compl_first_match->cp_পরবর্তী))
    {
	edit_submode_extra = compl_status_adding() && compl_দৈর্ঘ্য > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    যদি (edit_submode_extra == ফাঁকা)
    {
	যদি (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	অন্যথা যদি (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_গুনতি;
	}
	অন্যথা যদি (compl_curr_match->cp_পরবর্তী == compl_curr_match->cp_পূর্ব)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_গুনতি;
	    compl_curr_match->cp_সংখ্যা = 1;
	}
	অন্যথা
	{
#যদি defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    যদি (compl_curr_match->cp_সংখ্যা == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    যদি (compl_curr_match->cp_সংখ্যা != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		স্থির char_u match_ref[81];

		যদি (compl_matches > 0)
		    vim_snprintf((অক্ষর *)match_ref, আকার(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_সংখ্যা, compl_matches);
		অন্যথা
		    vim_snprintf((অক্ষর *)match_ref, আকার(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_সংখ্যা);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		যদি (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    যদি (!compl_opt_suppress_empty)
    {
	showmode();
	যদি (!shortmess(SHM_COMPLETIONMENU))
	{
	    যদি (edit_submode_extra != ফাঁকা)
	    {
		যদি (!p_smd)
		{
		    বার্তা_hist_off = TRUE;
		    বার্তা_attr((অক্ষর *)edit_submode_extra,
			    edit_submode_highl < HLF_গুনতি
			    ? HL_ATTR(edit_submode_highl) : 0);
		    বার্তা_hist_off = FALSE;
		}
	    }
	    অন্যথা
		বার্তা_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    পূর্ণসংখ্যা
ins_complete(পূর্ণসংখ্যা c, পূর্ণসংখ্যা enable_pum)
{
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		save_w_wrow;
    পূর্ণসংখ্যা		save_w_leftcol;
    পূর্ণসংখ্যা		ঢোকাও_match;
    পূর্ণসংখ্যা		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    ঢোকাও_match = ins_compl_use_match(c);

    যদি (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	যদি (ins_compl_start() == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (ঢোকাও_match && stop_arrow() == FAIL)
	ফেরত FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_বাফার;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_অঙ্ক_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_পরবর্তী(TRUE, ins_compl_key2count(c), ঢোকাও_match);

    // Reset autocompletion timer expiry flag
    যদি (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    যদি (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    যদি (got_int && !global_busy)
    {
	(শূন্য)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_পরবর্তী);
    যদি (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	যদি (compl_দৈর্ঘ্য > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    যদি (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    অন্যথা
	compl_cont_status &= ~CONT_S_IPOS;

    যদি (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    যদি (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    ফেরত OK;
}

/*
 * Enable autocompletion
 */
    শূন্য
ins_compl_enable_autocomplete(শূন্য)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
}

/*
 * Disable autocompletion
 */
    শূন্য
ins_compl_disable_autocomplete(শূন্য)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    শূন্য
ins_compl_arm_autostart(শূন্য)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    শূন্য
ins_compl_disarm_autostart(শূন্য)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(শূন্য)
{
#ifdef ELAPSED_FUNC
    যদি (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	ফেরত true;
    }
#endif
    ফেরত false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    শূন্য
ins_compl_clear_autocomplete_delay(শূন্য)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(শূন্য)
{
    ফেরত compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(শূন্য)
{
#ifdef ELAPSED_FUNC
    ফেরত ELAPSED_FUNC(compl_autocomplete_start_tv);
#অন্যথা
    ফেরত 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    স্থির শূন্য
show_pum(পূর্ণসংখ্যা পূর্ব_w_wrow, পূর্ণসংখ্যা পূর্ব_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    যদি (পূর্ব_w_wrow != curwin->w_wrow || পূর্ব_w_leftcol != curwin->w_leftcol)
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
    স্থির unsigned
quote_meta(char_u *dest, char_u *src, পূর্ণসংখ্যা len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    জন্য ( ; --len >= 0; src++)
    {
	নির্বাচন (*src)
	{
	    ক্ষেত্র '.':
	    ক্ষেত্র '*':
	    ক্ষেত্র '[':
		যদি (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    বিরতি;
		// FALLTHROUGH
	    ক্ষেত্র '~':
		যদি (!magic_isset())	// quote these only if magic is set
		    বিরতি;
		// FALLTHROUGH
	    ক্ষেত্র '\\':
		যদি (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    বিরতি;
		// FALLTHROUGH
	    ক্ষেত্র '^':		// currently it's not needed.
	    ক্ষেত্র '$':
		m++;
		যদি (dest != ফাঁকা)
		    *dest++ = '\\';
		বিরতি;
	}
	যদি (dest != ফাঁকা)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা mb_len = (*mb_ptr2len)(src) - 1;
	    যদি (mb_len > 0 && len >= mb_len)
		জন্য (পূর্ণসংখ্যা i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    যদি (dest != ফাঁকা)
			*dest++ = *src;
		}
	}
    }
    যদি (dest != ফাঁকা)
	*dest = NUL;

    ফেরত m;
}

#যদি defined(EXITFREE)
    শূন্য
free_insexpand_stuff(শূন্য)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_গুনতি);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    স্থির শূন্য
spell_back_to_badword(শূন্য)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, ফাঁকা);
    যদি (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    স্থির শূন্য
cpt_sources_clear(শূন্য)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_সূচক = -1;
    cpt_sources_গুনতি = 0;
}

/*
 * Setup completion sources.
 */
    স্থির পূর্ণসংখ্যা
setup_cpt_sources(শূন্য)
{
    char_u  buf[LSIZE];
    পূর্ণসংখ্যা	    slen;
    পূর্ণসংখ্যা	    idx = 0;
    পূর্ণসংখ্যা	    গুনতি;
    char_u  *p;

    cpt_sources_clear();

    গুনতি = get_cpt_sources_গুনতি(curbuf->b_p_cpt);
    যদি (গুনতি == 0)
	ফেরত OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, গুনতি);
    যদি (cpt_sources_array == ফাঁকা)
	ফেরত FAIL;

    জন্য (p = curbuf->b_p_cpt; *p;)
    {
	যখন (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	যদি (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    যদি (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		যদি (caret != ফাঁকা)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((অক্ষর *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_গুনতি = গুনতি;
    ফেরত OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    স্থির পূর্ণসংখ্যা
is_cpt_func_refresh_always(শূন্য)
{
#ifdef FEAT_COMPL_FUNC
    জন্য (পূর্ণসংখ্যা i = 0; i < cpt_sources_গুনতি; i++)
	যদি (cpt_sources_array[i].cs_refresh_always)
	    ফেরত TRUE;
#endif
    ফেরত FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    স্থির শূন্য
ins_compl_make_linear(শূন্য)
{
    compl_T *m;

    যদি (compl_first_match == ফাঁকা || compl_first_match->cp_পূর্ব == ফাঁকা)
	ফেরত;
    m = compl_first_match->cp_পূর্ব;
    m->cp_পরবর্তী = ফাঁকা;
    compl_first_match->cp_পূর্ব = ফাঁকা;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    স্থির শূন্য
remove_old_matches(শূন্য)
{
    compl_T *current;
    পূর্ণসংখ্যা	    shown_match_removed = FALSE;
    পূর্ণসংখ্যা	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    যদি (cpt_sources_সূচক < 0)
	ফেরত;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    জন্য (current = compl_first_match; current != ফাঁকা; )
    {
	যদি (current->cp_cpt_source_idx == cpt_sources_সূচক)
	{
	    compl_T *to_delete = current;

	    যদি (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_পরবর্তী;

	    যদি (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_পরবর্তী;
		compl_first_match->cp_পূর্ব = ফাঁকা;
	    }
	    অন্যথা যদি (to_delete->cp_পরবর্তী == ফাঁকা) // node to remove is at tail
		to_delete->cp_পূর্ব->cp_পরবর্তী = ফাঁকা;
	    অন্যথা // node is in the moddle
	    {
		to_delete->cp_পূর্ব->cp_পরবর্তী = to_delete->cp_পরবর্তী;
		to_delete->cp_পরবর্তী->cp_পূর্ব = to_delete->cp_পূর্ব;
	    }
	    ins_compl_item_free(to_delete);
	}
	অন্যথা
	    current = current->cp_পরবর্তী;
    }

    // Re-assign compl_shown_match if necessary
    যদি (shown_match_removed)
    {
	যদি (forward)
	    compl_shown_match = compl_first_match;
	অন্যথা
	{  // Last node will have the prefix that is being completed
	    জন্য (current = compl_first_match; current->cp_পরবর্তী != ফাঁকা;
		    current = current->cp_পরবর্তী)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    জন্য (current = compl_first_match; current != ফাঁকা; )
    {
	যদি ((forward ? current->cp_cpt_source_idx < cpt_sources_সূচক
		    : current->cp_cpt_source_idx > cpt_sources_সূচক))
	{
	    compl_curr_match = forward ? current : current->cp_পরবর্তী;
	    current = current->cp_পরবর্তী;
	}
	অন্যথা
	    বিরতি;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    স্থির শূন্য
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_সূচক];
    পূর্ণসংখ্যা		    startcol = cpt_src->cs_startcol;

    যদি (startcol == -2 || startcol == -3)
	ফেরত;

    যদি (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	যদি (!cpt_src->cs_refresh_always)
	    ins_compl_ঢোকাও_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	যদি (!cpt_src->cs_refresh_always)
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
    স্থির শূন্য
cpt_compl_refresh(শূন্য)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ফাঁকা;
    পূর্ণসংখ্যা		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_সূচক = 0;
    জন্য (p = cpt; *p;)
    {
	যখন (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	যদি (*p == NUL)
	    বিরতি;

	যদি (cpt_sources_array[cpt_sources_সূচক].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_সূচক);
	    যদি (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		যদি (ret == FAIL)
		{
		    যদি (startcol == -3)
			cpt_sources_array[cpt_sources_সূচক].cs_refresh_always
			    = FALSE;
		    অন্যথা
			startcol = -2;
		}
		অন্যথা যদি (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_সূচক].cs_startcol = startcol;
		যদি (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_সূচক);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(শূন্য)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	যদি (may_advance_cpt_সূচক(p))
	    (শূন্য)advance_cpt_sources_সূচক_safe();
    }
    cpt_sources_সূচক = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#endif
}

/*
 * "preinserted()" function
 */
    শূন্য
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    যদি (ins_compl_preinsert_effect())
	rettv->vval.v_সংখ্যা = 1;
}
