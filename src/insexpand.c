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

#அந்தர்பாவ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_வரி	3
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
#define CTRL_X_LOCAL_செய்தி	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_செய்தி(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
நிலையான எழுத்து *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    வெற்று, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    வெற்று, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    வெற்று,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#எனில் defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
நிலையான string_T ctrl_x_mode_names[] = {
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
    {வெற்று, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
வகைவரை கட்டமைப்பு compl_S compl_T;
கட்டமைப்பு compl_S
{
    compl_T	*cp_next;
    compl_T	*cp_prev;
    compl_T	*cp_match_next;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_மொத்தம்]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_data;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    முழு		cp_flags;		// CP_ values
    முழு		cp_எண்;		// sequence number
    முழு		cp_order;		// list position when first scored
    முழு		cp_score;		// fuzzy match score or proximity score
    முழு		cp_in_match_array;	// collected by compl_match_array
    முழு		cp_user_abbr_hl_id;	// highlight group ID for abbr
    முழு		cp_user_kind_hl_id;	// highlight group ID for kind
    முழு		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
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
நிலையான compl_T    *compl_first_match = வெற்று;
நிலையான compl_T    *compl_curr_match = வெற்று;
நிலையான compl_T    *compl_shown_match = வெற்று;
நிலையான compl_T    *compl_old_match = வெற்று;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
வகைவரை கட்டமைப்பு
{
    முழு	    cse_மொத்தம்;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((முழு)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_சாவி - CSE_OFF))

நிலையான hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    நிலையான வெறுமை
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    எனில் (compl_strings_ht.ht_array == வெற்று)
	hash_init(&compl_strings_ht);
    எனில் (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    எனில் (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	எனில் (entry == வெற்று)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    திரும்பு;
	entry->cse_மொத்தம் = 1;
	vim_strncpy(entry->cse_str, str, len);
	எனில் (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    இல்லையெனில்
	++HI2CSE(hi)->cse_மொத்தம்;
}

// list used to store the compl_T which have the max score
நிலையான compl_T	  **compl_best_matches = வெற்று;
நிலையான முழு	  compl_இலக்கம்_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
நிலையான முழு	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
நிலையான string_T	  compl_leader = {வெற்று, 0};

நிலையான முழு	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
நிலையான முழு	  compl_used_match;

// didn't finish finding completions.
நிலையான முழு	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
நிலையான முழு	  compl_interrupted = FALSE;

நிலையான முழு	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
நிலையான முழு	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
நிலையான முழு	  ctrl_x_mode = CTRL_X_NORMAL;

நிலையான முழு	  compl_matches = 0;	    // number of completion matches
நிலையான string_T	  compl_pattern = {வெற்று, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
நிலையான string_T	  cpt_compl_pattern = {வெற்று, 0}; // pattern returned by func in 'cpt'
#endif
நிலையான முழு	  compl_direction = FORWARD;
நிலையான முழு	  compl_shows_dir = FORWARD;
நிலையான முழு	  compl_pending = 0;	    // > 1 for postponed CTRL-N
நிலையான pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
நிலையான முழு	  compl_length = 0;
நிலையான linenr_T	  compl_lnum = 0;           // lnum where the completion start
நிலையான colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
நிலையான colnr_T	  compl_ins_end_col = 0;
நிலையான colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
நிலையான string_T	  compl_orig_text = {வெற்று, 0};  // text as it was before
					    // completion started
நிலையான முழு	  compl_cont_mode = 0;
நிலையான expand_T	  compl_xp;

நிலையான win_T	  *compl_curr_win = வெற்று;  // win where completion is active
நிலையான buf_T	  *compl_curr_buf = வெற்று;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
நிலையான முழு	  compl_autocomplete = FALSE;	    // whether autocompletion is active
நிலையான bool	  compl_autostarted = false;	    // Vim started this completion
நிலையான bool	  compl_autostart_pending = false;  // the trigger armed one
நிலையான bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
நிலையான elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
நிலையான முழு	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
நிலையான முழு	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
நிலையான முழு	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
நிலையான முழு	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    செய் { \
	எனில் (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } போது (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
நிலையான முழு	  compl_cont_status = 0;
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

நிலையான முழு	  compl_opt_refresh_always = FALSE;
நிலையான முழு	  compl_opt_suppress_empty = FALSE;

நிலையான முழு	  compl_selected_item = -1;

நிலையான முழு	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
வகைவரை கட்டமைப்பு cpt_source_T
{
    முழு	cs_refresh_always;  // Whether 'refresh:always' is set for func
    முழு	cs_startcol;	    // Start column returned by func
    முழு	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

நிலையான cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
நிலையான முழு	    cpt_sources_மொத்தம்;  // Total number of completion sources specified in the 'cpt' option
நிலையான முழு	    cpt_sources_index = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
நிலையான pumitem_T *compl_match_array = வெற்று;
நிலையான முழு compl_match_arraysize;

நிலையான முழு ins_compl_add(char_u *str, முழு len, char_u *fname, char_u **cptext, typval_T *user_data, முழு cdir, முழு flags, முழு adup, முழு *user_hl, முழு score);
நிலையான வெறுமை ins_compl_longest_match(compl_T *match);
நிலையான வெறுமை ins_compl_del_pum(வெறுமை);
நிலையான வெறுமை ins_compl_files(முழு மொத்தம், char_u **files, முழு thesaurus, முழு flags, regmatch_T *regmatch, char_u *buf, முழு *dir);
நிலையான வெறுமை ins_compl_free(வெறுமை);
நிலையான முழு  ins_compl_need_restart(வெறுமை);
நிலையான வெறுமை ins_compl_new_leader(வெறுமை);
நிலையான முழு  get_compl_len(வெறுமை);
நிலையான வெறுமை ins_compl_restart(வெறுமை);
நிலையான வெறுமை ins_compl_set_original_text(char_u *str, size_t len);
நிலையான வெறுமை ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#எனில் defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
நிலையான வெறுமை ins_compl_add_list(list_T *list);
நிலையான வெறுமை ins_compl_add_dict(dict_T *dict);
நிலையான முழு get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, முழு *startcol);
நிலையான வெறுமை get_cpt_func_completion_matches(callback_T *cb);
நிலையான callback_T *get_callback_if_cpt_func(char_u *p, முழு idx);
நிலையான வெறுமை fill_complete_info_dict(dict_T *di, compl_T *match, முழு add_match);
#endif
நிலையான முழு setup_cpt_sources(வெறுமை);
நிலையான முழு is_cpt_func_refresh_always(வெறுமை);
நிலையான வெறுமை cpt_sources_clear(வெறுமை);
நிலையான வெறுமை cpt_compl_refresh(வெறுமை);
நிலையான முழு  ins_compl_key2dir(முழு c);
நிலையான முழு  ins_compl_pum_சாவி(முழு c);
நிலையான முழு  ins_compl_key2count(முழு c);
நிலையான வெறுமை show_pum(முழு prev_w_wrow, முழு prev_w_leftcol);
நிலையான unsigned  quote_meta(char_u *dest, char_u *str, முழு len);
நிலையான முழு ins_compl_has_multiple(வெறுமை);
நிலையான வெறுமை ins_compl_expand_multiple(char_u *str);
நிலையான வெறுமை ins_compl_longest_செருகு(char_u *prefix);
நிலையான வெறுமை ins_compl_make_linear(வெறுமை);
நிலையான முழு ins_compl_make_cyclic(வெறுமை);

#ifdef FEAT_SPELL
நிலையான வெறுமை spell_back_to_badword(வெறுமை);
நிலையான முழு  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    வெறுமை
ins_ctrl_x(வெறுமை)
{
    எனில் (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	எனில் (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	இல்லையெனில்
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_செய்தி(ctrl_x_mode));
	edit_submode_pre = வெற்று;
	showmode();
    }
    இல்லையெனில்
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
முழு ctrl_x_mode_none(வெறுமை)
    { திரும்பு ctrl_x_mode == 0; }
முழு ctrl_x_mode_normal(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_NORMAL; }
முழு ctrl_x_mode_scroll(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_SCROLL; }
முழு ctrl_x_mode_whole_வரி(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_WHOLE_வரி; }
முழு ctrl_x_mode_files(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_FILES; }
முழு ctrl_x_mode_tags(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_TAGS; }
முழு ctrl_x_mode_path_patterns(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
முழு ctrl_x_mode_path_defines(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_PATH_DEFINES; }
முழு ctrl_x_mode_dictionary(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_DICTIONARY; }
முழு ctrl_x_mode_thesaurus(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_THESAURUS; }
முழு ctrl_x_mode_cmdline(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
முழு ctrl_x_mode_function(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_FUNCTION; }
முழு ctrl_x_mode_omni(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_OMNI; }
முழு ctrl_x_mode_spell(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_SPELL; }
முழு ctrl_x_mode_eval(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_EVAL; }
முழு ctrl_x_mode_வரி_or_eval(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_WHOLE_வரி || ctrl_x_mode == CTRL_X_EVAL; }
முழு ctrl_x_mode_register(வெறுமை)
    { திரும்பு ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    முழு
ctrl_x_mode_not_default(வெறுமை)
{
    திரும்பு ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    முழு
ctrl_x_mode_not_defined_yet(வெறுமை)
{
    திரும்பு ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    முழு
compl_status_adding(வெறுமை)
{
    திரும்பு compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    முழு
compl_status_sol(வெறுமை)
{
    திரும்பு compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    முழு
compl_status_local(வெறுமை)
{
    திரும்பு compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    வெறுமை
compl_status_clear(வெறுமை)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    நிலையான முழு
compl_dir_forward(வெறுமை)
{
    திரும்பு compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    நிலையான முழு
compl_shows_dir_forward(வெறுமை)
{
    திரும்பு compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    நிலையான முழு
compl_shows_dir_backward(வெறுமை)
{
    திரும்பு compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    முழு
has_compl_option(முழு dict_opt)
{
    எனில் (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
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
	edit_submode = வெற்று;
	செய்தி_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	எனில் (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    எனில் (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    முழு
vim_is_ctrl_x_சாவி(முழு c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    எனில் (ins_compl_pum_சாவி(c))
	திரும்பு TRUE;

    தேர்வு (ctrl_x_mode)
    {
	நிலை 0:		    // Not in any CTRL-X mode
	    திரும்பு (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	நிலை CTRL_X_NOT_DEFINED_YET:
	நிலை CTRL_X_CMDLINE_CTRL_X:
	    திரும்பு (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	நிலை CTRL_X_SCROLL:
	    திரும்பு (c == Ctrl_Y || c == Ctrl_E);
	நிலை CTRL_X_WHOLE_வரி:
	    திரும்பு (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_FILES:
	    திரும்பு (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_DICTIONARY:
	    திரும்பு (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_THESAURUS:
	    திரும்பு (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_TAGS:
	    திரும்பு (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	நிலை CTRL_X_PATH_PATTERNS:
	    திரும்பு (c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_PATH_DEFINES:
	    திரும்பு (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	நிலை CTRL_X_CMDLINE:
	    திரும்பு (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	நிலை CTRL_X_FUNCTION:
	    திரும்பு (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_OMNI:
	    திரும்பு (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	நிலை CTRL_X_SPELL:
	    திரும்பு (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_EVAL:
	    திரும்பு (c == Ctrl_P || c == Ctrl_N);
	நிலை CTRL_X_REGISTER:
	    திரும்பு (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    திரும்பு FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    நிலையான முழு
match_at_original_text(compl_T *match)
{
    திரும்பு match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    நிலையான முழு
is_first_match(compl_T *match)
{
    திரும்பு match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    நிலையான compl_T *
find_original_text_match(வெறுமை)
{
    எனில் (compl_first_match == வெற்று)
	திரும்பு வெற்று;
    எனில் (match_at_original_text(compl_first_match))
	திரும்பு compl_first_match;
    எனில் (compl_first_match->cp_prev != வெற்று
	    && match_at_original_text(compl_first_match->cp_prev))
	திரும்பு compl_first_match->cp_prev;
    திரும்பு வெற்று;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    முழு
ins_compl_accept_char(முழு c)
{
    எனில் (compl_autocomplete && compl_from_nonkeyword)
	திரும்பு FALSE;

    எனில் (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	திரும்பு vim_isIDc(c);

    தேர்வு (ctrl_x_mode)
    {
	நிலை CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    திரும்பு vim_isfilec(c) && !vim_ispathsep(c);

	நிலை CTRL_X_CMDLINE:
	நிலை CTRL_X_CMDLINE_CTRL_X:
	நிலை CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    திரும்பு vim_isprintc(c) && !VIM_ISWHITE(c);

	நிலை CTRL_X_WHOLE_வரி:
	    // For while line completion a space can be part of the line.
	    திரும்பு vim_isprintc(c);
    }
    திரும்பு vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    நிலையான char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    முழு	    char_len,
    முழு	    compl_char_len,
    முழு	    min_len,
    char_u  **tofree)
{
    முழு		*wca;			// Wide character array.
    char_u	*p;
    முழு		i, c;
    முழு		has_lower = FALSE;
    முழு		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(முழு, char_len);
    எனில் (wca == வெற்று)
	திரும்பு IObuff;

    p = str;
    க்கு (i = 0; i < char_len; ++i)
    {
	எனில் (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	இல்லையெனில்
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    க்கு (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	எனில் (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    எனில் (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		க்கு (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		நிறுத்து;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    எனில் (!has_lower)
    {
	p = compl_orig_text.string;
	க்கு (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    எனில் (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		க்கு (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		நிறுத்து;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    க்கு (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	எனில் (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	இல்லையெனில் எனில் (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    போது (i < char_len)
    {
	எனில் (gap.ga_data != வெற்று)
	{
	    எனில் (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		திரும்பு (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    எனில் (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    இல்லையெனில்
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	இல்லையெனில் எனில் ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    எனில் (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		திரும்பு (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (முழு)(p - IObuff);
	}
	இல்லையெனில் எனில் (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	இல்லையெனில்
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    எனில் (gap.ga_data != வெற்று)
    {
	*tofree = gap.ga_data;
	திரும்பு gap.ga_data;
    }

    *p = NUL;
    திரும்பு IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    முழு
ins_compl_add_infercase(
    char_u	*str_arg,
    முழு		len,
    முழு		icase,
    char_u	*fname,
    முழு		dir,
    முழு		cont_s_ipos,  // next ^X<> will set initial_pos
    முழு		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    முழு		char_len;		// count multi-byte characters
    முழு		compl_char_len;
    முழு		min_len;
    முழு		flags = 0;
    முழு		res;
    char_u	*tofree = வெற்று;

    எனில் (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	எனில் (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    போது (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	இல்லையெனில்
	{
	    char_len = len;
	}

	// Find actual length of original text.
	எனில் (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    போது (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	இல்லையெனில்
	{
	    compl_char_len = compl_length;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    எனில் (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    எனில் (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, வெற்று, வெற்று, dir, flags, FALSE, வெற்று, score);
    vim_free(tofree);
    திரும்பு res;
}

/*
 * Check if fuzzy matching is enabled
 */
    நிலையான முழு
cot_fuzzy(வெறுமை)
{
    திரும்பு (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    நிலையான முழு
is_nearest_active(வெறுமை)
{
    திரும்பு (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    முழு
ins_compl_is_match_selected(வெறுமை)
{
    திரும்பு compl_shown_match != வெற்று && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    முழு
ins_compl_preinsert_longest(வெறுமை)
{
    திரும்பு compl_autocomplete
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
    நிலையான முழு
ins_compl_add(
    char_u	*str,
    முழு		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_data UNUSED,  // "user_data" entry or NULL
    முழு		cdir,
    முழு		flags_arg,
    முழு		adup,		    // accept duplicate match
    முழு		*user_hl,           // user abbr/kind hlattr
    முழு		score)
{
    compl_T	*match, *current, *prev;
    முழு		dir = (cdir == 0 ? compl_direction : cdir);
    முழு		flags = flags_arg;
    முழு		inserted = FALSE;
    char_u	*new_str = வெற்று;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    எனில் (flags & CP_FAST)
	fast_breakcheck();
    இல்லையெனில்
	ui_breakcheck();
    எனில் (got_int)
	திரும்பு FAIL;
    எனில் (len < 0)
	len = (முழு)STRLEN(str);

    // If the same match is already present, don't add it.
    எனில் (compl_first_match != வெற்று && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *சாவி;
	hashitem_T  *hi;

	எனில் (len < (முழு)அளவு(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    சாவி = keybuf;
	}
	இல்லையெனில்
	{
	    new_str = vim_strnsave(str, len);
	    எனில் (new_str == வெற்று)
		திரும்பு FAIL;
	    சாவி = new_str;
	}
	str_hash = hash_hash(சாவி);
	hi = hash_lookup(&compl_strings_ht, சாவி, str_hash);
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    எனில் (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		செய்
		{
		    எனில் (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((முழு)match->cp_str.length <= len
					  || match->cp_str.string[len] == NUL))
		    {
			எனில் (score < match->cp_score)
			    match->cp_score = score;
			நிறுத்து;
		    }
		    match = match->cp_next;
		} போது (match != வெற்று && !is_first_match(match));
	    }
	    vim_free(new_str);
	    திரும்பு NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    எனில் (match == வெற்று)
    {
	vim_free(new_str);
	திரும்பு FAIL;
    }
    match->cp_எண் = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    எனில் (new_str == வெற்று && (new_str = vim_strnsave(str, len)) == வெற்று)
    {
	vim_free(match);
	திரும்பு FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.length = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    எனில் (fname != வெற்று
	    && compl_curr_match != வெற்று
	    && compl_curr_match->cp_fname != வெற்று
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    இல்லையெனில் எனில் (fname != வெற்று)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    இல்லையெனில்
	match->cp_fname = வெற்று;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_index;

    எனில் (cptext != வெற்று)
    {
	க்கு (முழு i = 0; i < CPT_மொத்தம்; ++i)
	{
	    எனில் (cptext[i] != வெற்று && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    எனில் (user_data != வெற்று)
	match->cp_user_data = *user_data;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    எனில் (compl_first_match == வெற்று)
	match->cp_next = match->cp_prev = வெற்று;
    இல்லையெனில் எனில் (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_next;
	prev = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	போது (current != வெற்று && current != compl_first_match)
	{
	    எனில் (current->cp_score < score)
	    {
		match->cp_next = current;
		match->cp_prev = current->cp_prev;
		எனில் (current->cp_prev)
		    current->cp_prev->cp_next = match;
		current->cp_prev = match;
		inserted = TRUE;
		நிறுத்து;
	    }
	    prev = current;
	    current = current->cp_next;
	}
	எனில் (!inserted)
	{
	    prev->cp_next = match;
	    match->cp_prev = prev;
	    match->cp_next = compl_first_match;
	    compl_first_match->cp_prev = match;
	}
    }
    இல்லையெனில் எனில் (dir == FORWARD)
    {
	match->cp_next = compl_curr_match->cp_next;
	match->cp_prev = compl_curr_match;
    }
    இல்லையெனில்	// BACKWARD
    {
	match->cp_next = compl_curr_match;
	match->cp_prev = compl_curr_match->cp_prev;
    }
    எனில் (match->cp_next)
	match->cp_next->cp_prev = match;
    எனில் (match->cp_prev)
	match->cp_prev->cp_next = match;
    இல்லையெனில்	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    எனில் (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.length,
								    str_hash);

    // Find the longest common string if still doing that.
    எனில் (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    திரும்பு OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    நிலையான முழு
ins_compl_equal(compl_T *match, char_u *str, முழு len)
{
    எனில் (match->cp_flags & CP_EQUAL)
	திரும்பு TRUE;
    எனில் (match->cp_flags & CP_ICASE)
	திரும்பு STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    திரும்பு STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    நிலையான முழு
ins_compl_equal_sc(compl_T *match, char_u *str, முழு len)
{
    முழு	typed = compl_length;
    முழு	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (முழு)(compl_longest_end_col - compl_col) : typed;

    எனில் ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	திரும்பு ins_compl_equal(match, str, len);

    எனில் ((முழு)match->cp_str.length < len)
	திரும்பு FALSE;

    க்கு (முழு i = 0; i < len; ++i)
    {
	எனில் (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    நிலையான வெறுமை
ins_compl_செருகு_bytes(char_u *p, முழு len)
{
    எனில் (len == -1)
	len = (முழு)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(வெறுமை)
{
    திரும்பு compl_leader.string != வெற்று ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    நிலையான size_t
ins_compl_leader_len(வெறுமை)
{
    திரும்பு compl_leader.string != வெற்று ? compl_leader.length : compl_orig_text.length;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    முழு
ins_compl_col_range_attr(linenr_T lnum, முழு col)
{
    முழு	    start_col;
    முழு	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    முழு	    attr;

    எனில் (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	திரும்பு -1;

    start_col = compl_col + (முழு)ins_compl_leader_len();
    எனில் (!ins_compl_has_multiple())
	திரும்பு (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    எனில் ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	திரும்பு attr;

    திரும்பு -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    நிலையான முழு
ins_compl_has_multiple(வெறுமை)
{
    திரும்பு vim_strchr(compl_shown_match->cp_str.string, '\n') != வெற்று;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    முழு
ins_compl_lnum_in_range(linenr_T lnum)
{
    எனில் (!ins_compl_has_multiple())
	திரும்பு FALSE;
    திரும்பு lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    நிலையான வெறுமை
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    முழு		c1, c2;
    முழு		had_match;

    எனில் (compl_leader.string == வெற்று)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.length);
	எனில் (compl_leader.string == வெற்று)
	    திரும்பு;

	compl_leader.length = match->cp_str.length;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_செருகு(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	எனில் (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	திரும்பு;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    போது (*p != NUL)
    {
	எனில் (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	இல்லையெனில்
	{
	    c1 = *p;
	    c2 = *s;
	}
	எனில் ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    நிறுத்து;
	எனில் (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	இல்லையெனில்
	{
	    ++p;
	    ++s;
	}
    }

    எனில் (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.length = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_செருகு(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	எனில் (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    நிலையான வெறுமை
ins_compl_add_matches(
    முழு		இலக்கம்_matches,
    char_u	**matches,
    முழு		icase)
{
    முழு		add_r = OK;
    முழு		dir = compl_direction;

    க்கு (முழு i = 0; i < இலக்கம்_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, வெற்று, வெற்று, வெற்று, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, வெற்று,
		FUZZY_SCORE_NONE);
	எனில் (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(இலக்கம்_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    நிலையான முழு
ins_compl_make_cyclic(வெறுமை)
{
    compl_T *match;
    முழு	    மொத்தம் = 0;

    எனில் (compl_first_match == வெற்று)
	திரும்பு 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    போது (match->cp_next != வெற்று && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++மொத்தம்;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    திரும்பு மொத்தம்;
}

/*
 * Return whether there currently is a shown match.
 */
    முழு
ins_compl_has_shown_match(வெறுமை)
{
    திரும்பு compl_shown_match == வெற்று
	|| compl_shown_match != compl_shown_match->cp_next;
}

/*
 * Return whether the shown match is long enough.
 */
    முழு
ins_compl_long_shown_match(வெறுமை)
{
    திரும்பு (முழு)compl_shown_match->cp_str.length
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned முழு
get_cot_flags(வெறுமை)
{
    திரும்பு curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    நிலையான வெறுமை
ins_compl_upd_pum(வெறுமை)
{
    எனில் (compl_match_array == வெற்று)
	திரும்பு;

    முழு h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    எனில் (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    நிலையான வெறுமை
ins_compl_del_pum(வெறுமை)
{
    எனில் (compl_match_array == வெற்று)
	திரும்பு;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    முழு
pum_wanted(வெறுமை)
{
    // 'completeopt' must contain "menu" or "menuone"
    திரும்பு (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    நிலையான முழு
pum_enough_matches(வெறுமை)
{
    compl_T     *compl;
    முழு		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    செய்
    {
	எனில் (compl == வெற்று || (!match_at_original_text(compl) && ++i == 2))
	    நிறுத்து;
	compl = compl->cp_next;
    } போது (!is_first_match(compl));

    எனில் ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	திரும்பு (i >= 1);
    திரும்பு (i >= 2);
}

#எனில் defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    நிலையான dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    எனில் (dict == வெற்று)
	திரும்பு வெற்று;
    fill_complete_info_dict(dict, match, FALSE);
    திரும்பு dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    நிலையான வெறுமை
trigger_complete_changed_event(முழு cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    நிலையான முழு	    recursive = FALSE;
    save_v_event_T  save_v_event;

    எனில் (recursive)
	திரும்பு;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    எனில் (item == வெற்று)
	திரும்பு;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_பொருட்கள்_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, வெற்று, வெற்று, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    நிலையான வெறுமை*
cp_get_next(வெறுமை *முனை)
{
    திரும்பு ((compl_T*)முனை)->cp_next;
}

    நிலையான வெறுமை
cp_set_next(வெறுமை *முனை, வெறுமை *next)
{
    ((compl_T*)முனை)->cp_next = (compl_T*)next;
}

    நிலையான வெறுமை*
cp_get_prev(வெறுமை* முனை)
{
    திரும்பு ((compl_T*)முனை)->cp_prev;
}

    நிலையான வெறுமை
cp_set_prev(வெறுமை* முனை, வெறுமை* prev)
{
    ((compl_T*)முனை)->cp_prev = (compl_T*)prev;
}

    நிலையான முழு
cp_compare_fuzzy(மாறிலி வெறுமை* a, மாறிலி வெறுமை* b)
{
    முழு score_a = ((compl_T*)a)->cp_score;
    முழு score_b = ((compl_T*)b)->cp_score;

    எனில் (score_a != score_b)
	திரும்பு (score_b > score_a) ? 1 : -1;

    திரும்பு ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    நிலையான முழு
cp_compare_nearest(மாறிலி வெறுமை* a, மாறிலி வெறுமை* b)
{
    முழு score_a = ((compl_T*)a)->cp_score;
    முழு score_b = ((compl_T*)b)->cp_score;
    எனில் (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	திரும்பு 0;
    திரும்பு (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    நிலையான முழு
prepend_startcol_text(string_T *dest, string_T *src, முழு startcol)
{
    முழு prepend_len = compl_col - startcol;
    முழு new_length = prepend_len + (முழு)src->length;

    dest->length = (size_t)new_length;
    dest->string = alloc(new_length + 1);  // +1 for NUL
    எனில் (dest->string == வெற்று)
    {
	dest->length = 0;
	திரும்பு FAIL;
    }

    char_u	*வரி = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, வரி + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->length);
    dest->string[new_length] = NUL;
    திரும்பு OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    நிலையான string_T *
get_leader_for_startcol(compl_T *match, முழு cached)
{
    நிலையான string_T adjusted_leader = {வெற்று, 0};

    எனில் (match == வெற்று)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	திரும்பு வெற்று;
    }

    எனில் (cpt_sources_array == வெற்று)
	செல் theend;

    முழு	cpt_idx = match->cp_cpt_source_idx;
    எனில் (cpt_idx < 0)
	செல் theend;
    முழு	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    எனில் (compl_leader.string == வெற்று)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	எனில் (startcol < 0 || startcol >= compl_col)
	    திரும்பு &compl_orig_text;
	திரும்பு &compl_leader;  // pass through (startcol < compl_col)
    }

    எனில் (compl_col <= 0)
	செல் theend;

    எனில் (startcol >= 0 && startcol < compl_col)
    {
	முழு prepend_len = compl_col - startcol;
	முழு new_length = prepend_len + (முழு)compl_leader.length;
	எனில் (cached && (size_t)new_length == adjusted_leader.length
		&& adjusted_leader.string != வெற்று)
	    திரும்பு &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	எனில் (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    செல் theend;

	திரும்பு &adjusted_leader;
    }
theend:
    திரும்பு &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    நிலையான வெறுமை
set_fuzzy_score(வெறுமை)
{
    compl_T *compl;
    char_u  *pattern;
    முழு	    use_leader;

    எனில் (compl_first_match == வெற்று)
	திரும்பு;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != வெற்று && compl_leader.length > 0);
    எனில் (!use_leader)
    {
	எனில் (compl_orig_text.string == வெற்று || compl_orig_text.length == 0)
	    திரும்பு;
	pattern = compl_orig_text.string;
    }
    இல்லையெனில்
    {
	/* Clear the leader cache once before the loop */
	(வெறுமை)get_leader_for_startcol(வெற்று, TRUE);
	pattern = வெற்று;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    முழு next = 0;
    compl = compl_first_match;
    செய்
    {
	எனில் (compl->cp_order < 0)
	    compl->cp_order = next;
	next = MAX(next, compl->cp_order + 1);

	எனில் (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_next;
    } போது (compl != வெற்று && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    நிலையான வெறுமை
sort_compl_match_list(முழு (*compare)(மாறிலி வெறுமை *, மாறிலி வெறுமை *))
{
    compl_T	*orig_text;

    எனில் (!compl_first_match || is_first_match(compl_first_match->cp_next))
	திரும்பு;

    orig_text = find_original_text_match();
    எனில் (orig_text == வெற்று)
	திரும்பு;

    ins_compl_make_linear();
    எனில் (orig_text == compl_first_match)
    {
	compl_first_match->cp_next->cp_prev = வெற்று;
	compl_first_match->cp_next = mergesort_list(compl_first_match->cp_next,
		cp_get_next, cp_set_next, cp_get_prev, cp_set_prev, compare);
	compl_first_match->cp_next->cp_prev = compl_first_match;
    }
    இல்லையெனில்
    {
	compl_T	*tail;

	orig_text->cp_prev->cp_next = வெற்று;
	compl_first_match = mergesort_list(compl_first_match, cp_get_next,
		cp_set_next, cp_get_prev, cp_set_prev, compare);
	tail = compl_first_match;
	போது (tail->cp_next != வெற்று)
	    tail = tail->cp_next;
	tail->cp_next = orig_text;
	orig_text->cp_prev = tail;
    }
    (வெறுமை)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    நிலையான முழு
get_user_highlight_attr(முழு hl_id)
{
    முழு	    attr;

    எனில் (hl_id <= 0)
	திரும்பு -1;
    attr = syn_id2attr(hl_id);
    திரும்பு attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    நிலையான முழு
ins_compl_build_pum(வெறுமை)
{
    compl_T     *compl;
    compl_T     *shown_compl = வெற்று;
    முழு		did_find_shown_match = FALSE;
    முழு		shown_match_ok = FALSE;
    முழு		i = 0;
    முழு		cur = -1;
    முழு		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_head = வெற்று;
    compl_T	*match_tail = வெற்று;
    compl_T	*match_next = வெற்று;
    முழு		*match_மொத்தம் = வெற்று;
    முழு		is_forward = compl_shows_dir_forward();
    முழு		is_cpt_completion = (cpt_sources_array != வெற்று);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    எனில் (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    எனில் (compl_leader.string != வெற்று
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_next;

    எனில் (is_cpt_completion)
    {
	match_மொத்தம் = ALLOC_CLEAR_MULT(முழு, cpt_sources_மொத்தம்);
	எனில் (match_மொத்தம் == வெற்று)
	    திரும்பு -1;
    }

    (வெறுமை)get_leader_for_startcol(வெற்று, TRUE); // Clear the cache

    compl = compl_first_match;
    செய்
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	எனில் (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	எனில் (!match_at_original_text(compl)
		&& (leader->string == வெற்று
		    || ins_compl_equal_sc(compl, leader->string,
			(முழு)leader->length)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    முழு match_limit_exceeded = FALSE;
	    முழு cur_source = compl->cp_cpt_source_idx;
	    எனில் (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_மொத்தம்[cur_source]++;
		முழு max_matches = cpt_sources_array[cur_source].cs_max_matches;
		எனில் (max_matches > 0 && match_மொத்தம்[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    எனில் (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		எனில் (match_head == வெற்று)
		    match_head = compl;
		இல்லையெனில்
		    match_tail->cp_match_next = compl;
		match_tail = compl;

		எனில் (!shown_match_ok && !cot_fuzzy())
		{
		    எனில் (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    இல்லையெனில்
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		இல்லையெனில் எனில் (cot_fuzzy())
		{
		    எனில் (i == 0)
			shown_compl = compl;

		    எனில் (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	எனில் (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    எனில் (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    எனில் (!shown_match_ok && shown_compl != வெற்று)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } போது (compl != வெற்று && !is_first_match(compl));

    vim_free(match_மொத்தம்);

    எனில் (compl_match_arraysize == 0)
	திரும்பு -1;

    எனில் (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    எனில் (compl_match_array == வெற்று)
	திரும்பு -1;

    compl = match_head;
    i = 0;
    போது (compl != வெற்று)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != வெற்று
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != வெற்று
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_next = compl->cp_match_next;
	compl->cp_match_next = வெற்று;
	compl = match_next;
    }

    எனில் (!shown_match_ok)    // no displayed match at all
	cur = -1;

    திரும்பு cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    வெறுமை
ins_compl_show_pum(வெறுமை)
{
    முழு		i;
    முழு		cur = -1;
    colnr_T	col;

    எனில் (!pum_wanted() || !pum_enough_matches())
	திரும்பு;

    // Avoid redrawing the screen under a pum that stays in place.
    எனில் (!pum_redraw_in_same_position())
	pum_call_update_screen();

    எனில் (compl_match_array == வெற்று)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    இல்லையெனில்
    {
	// popup menu already exists, only need to find the current item.
	க்கு (i = 0; i < compl_match_arraysize; ++i)
	{
	    எனில் (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		நிறுத்து;
	    }
	}
    }

    எனில் (compl_match_array == வெற்று)
    {
#ifdef FEAT_EVAL
	எனில் (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	திரும்பு;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    முழு pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    எனில் (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    எனில் (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    நிலையான வெறுமை
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    முழு		flags,		// DICT_FIRST and/or DICT_EXACT
    முழு		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    முழு		மொத்தம்;
    முழு		save_p_scs;
    முழு		dir = compl_direction;

    எனில் (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	எனில் (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	இல்லையெனில்
#endif
	    திரும்பு;
    }

    buf = alloc(LSIZE);
    எனில் (buf == வெற்று)
	திரும்பு;
    regmatch.regprog = வெற்று;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    எனில் (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    எனில் (ctrl_x_mode_வரி_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	எனில் (pat_esc == வெற்று)
	    செல் theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	எனில் (ptr == வெற்று)
	{
	    vim_free(pat_esc);
	    செல் theend;
	}
	vim_snprintf((எழுத்து *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    இல்லையெனில்
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	எனில் (regmatch.regprog == வெற்று)
	    செல் theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    போது (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	எனில் (flags == DICT_EXACT)
	{
	    மொத்தம் = 1;
	    files = &dict;
	}
	இல்லையெனில்
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    எனில் (!thesaurus && STRCMP(buf, "spell") == 0)
		மொத்தம் = -1;
	    இல்லையெனில்
#endif
		எனில் (vim_strchr(buf, '`') != வெற்று
		    || expand_wildcards(1, &buf, &மொத்தம், &files,
						     EW_FILE|EW_SILENT) != OK)
		மொத்தம் = 0;
	}

#ifdef FEAT_SPELL
	எனில் (மொத்தம் == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    எனில் (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    இல்லையெனில்
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	இல்லையெனில்
#endif
	    எனில் (மொத்தம் > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(மொத்தம், files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    எனில் (flags != DICT_EXACT)
		FreeWild(மொத்தம், files);
	}
	எனில் (flags != 0)
	    நிறுத்து;
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
    நிலையான முழு
thesaurus_add_சொற்கள்_in_வரி(
    char_u	*fname,
    char_u	**buf_arg,
    முழு		dir,
    char_u	*skip_word)
{
    முழு		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    போது (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	எனில் (*ptr == NUL || *ptr == NL)
	    நிறுத்து;
	wstart = ptr;

	// Find end of the word.
	எனில் (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    போது (*ptr != NUL)
	    {
		முழு l = (*mb_ptr2len)(ptr);

		எனில் (l < 2 && !vim_iswordc(*ptr))
		    நிறுத்து;
		ptr += l;
	    }
	இல்லையெனில்
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	எனில் (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (முழு)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    எனில் (status == FAIL)
		நிறுத்து;
	}
    }

    *buf_arg = ptr;
    திரும்பு status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    நிலையான வெறுமை
ins_compl_files(
    முழு		மொத்தம்,
    char_u	**files,
    முழு		thesaurus,
    முழு		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    முழு		*dir)
{
    char_u	*ptr;
    முழு		i;
    FILE	*fp;
    முழு		add_r;
    char_u	*leader = வெற்று;
    முழு		leader_len = 0;
    முழு		score = 0;
    முழு		len = 0;
    char_u	*வரி_end = வெற்று;

    எனில் (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (முழு)ins_compl_leader_len();
    }

    க்கு (i = 0; i < மொத்தம் && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((எழுத்து *)files[i], "r");  // open dictionary file
	எனில் (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    செய்தி_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (எழுத்து *)files[i]);
	    (வெறுமை)செய்தி_trunc_attr((எழுத்து *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	எனில் (fp == வெற்று)
	    தொடர்;

	// Read dictionary file line by line.
	// Check each line for a match.
	போது (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    எனில் (cot_fuzzy() && leader_len > 0)
	    {
		வரி_end = find_வரி_end(ptr);
		போது (ptr < வரி_end)
		{
		    எனில் (fuzzy_match_str_in_வரி(&ptr, leader, &len, வெற்று, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_வரி_or_eval()
					? find_வரி_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (முழு)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			எனில் (add_r == FAIL)
			    நிறுத்து;
			ptr = end_ptr;  // start from next word
			எனில் (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_next
				&& score == compl_first_match->cp_next->cp_score)
			    compl_இலக்கம்_bests++;
		    }
		}
	    }
	    இல்லையெனில் எனில் (regmatch != வெற்று)
	    {
		போது (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_வரி_or_eval() ? find_வரி_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (முழு)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    எனில் (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_சொற்கள்_in_வரி(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    எனில் (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    இல்லையெனில் எனில் (add_r == FAIL)
			நிறுத்து;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    எனில் (*ptr == '\n' || got_int)
			நிறுத்து;
		}
	    }
	    வரி_breakcheck();
	    ins_compl_check_சாவிகள்(50, FALSE);
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
    எனில் (has_mbyte)
    {
	போது (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    இல்லையெனில்
    {
	போது (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    திரும்பு ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    எனில் (has_mbyte)
    {
	முழு start_class = mb_get_class(ptr);
	எனில் (start_class > 1)
	    போது (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		எனில் (mb_get_class(ptr) != start_class)
		    நிறுத்து;
	    }
    }
    இல்லையெனில்
	போது (vim_iswordc(*ptr))
	    ++ptr;
    திரும்பு ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_வரி_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    போது (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    திரும்பு s;
}

/*
 * Free a completion item in the list
 */
    நிலையான வெறுமை
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    எனில் (compl_strings_ht.ht_used > 0 && match->cp_str.string != வெற்று
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	எனில் (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    எனில் (--entry->cse_மொத்தம் <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    எனில் (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    க்கு (முழு i = 0; i < CPT_மொத்தம்; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_data);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    நிலையான வெறுமை
ins_compl_free(வெறுமை)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    எனில் (compl_first_match == வெற்று)
	திரும்பு;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    செய்
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	ins_compl_item_free(match);
    } போது (compl_curr_match != வெற்று && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = வெற்று;
    compl_shown_match = வெற்று;
    compl_old_match = வெற்று;
}

/*
 * Reset/clear the completion state.
 */
    வெறுமை
ins_compl_clear(வெறுமை)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = வெற்று;
    compl_curr_buf = வெற்று;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = வெற்று;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_இலக்கம்_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    முழு
ins_compl_active(வெறுமை)
{
    திரும்பு compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    முழு
ins_compl_win_active(win_T *wp)
{
    திரும்பு ins_compl_active() && wp == compl_curr_win
	&& wp->w_buffer == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    முழு
ins_compl_used_match(வெறுமை)
{
    திரும்பு compl_used_match;
}

/*
 * Initialize get longest common string.
 */
    வெறுமை
ins_compl_init_get_longest(வெறுமை)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    முழு
ins_compl_interrupted(வெறுமை)
{
    திரும்பு compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    முழு
ins_compl_enter_selects(வெறுமை)
{
    திரும்பு compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(வெறுமை)
{
    திரும்பு compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    முழு
ins_compl_len(வெறுமை)
{
    திரும்பு compl_length;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    முழு
ins_compl_has_preinsert(வெறுமை)
{
    முழு cur_cot_flags = get_cot_flags();
    எனில் (compl_autocomplete && p_ic && !p_inf)
	திரும்பு FALSE;
    திரும்பு !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    முழு
ins_compl_preinsert_effect(வெறுமை)
{
    எனில் (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	திரும்பு FALSE;

    திரும்பு curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    முழு
ins_compl_bs(வெறுமை)
{
    char_u	*வரி;
    char_u	*p;

    எனில் (ins_compl_preinsert_effect())
	ins_compl_delete();

    வரி = ml_get_curline();
    p = வரி + curwin->w_cursor.col;
    MB_PTR_BACK(வரி, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    எனில் ((முழு)(p - வரி) - (முழு)compl_col < 0
	    || ((முழு)(p - வரி) - (முழு)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (முழு)(p - வரி) - (முழு)compl_col
							- compl_length < 0))
	திரும்பு K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    எனில் (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.length = (size_t)((p - வரி) - compl_col);
    compl_leader.string = vim_strnsave(வரி + compl_col, compl_leader.length);
    எனில் (compl_leader.string == வெற்று)
    {
	compl_leader.length = 0;
	திரும்பு K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    எனில் (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    எனில் (compl_shown_match != வெற்று)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    திரும்பு NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    நிலையான முழு
ins_compl_need_restart(வெறுமை)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    திரும்பு compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    முழு
ins_compl_has_autocomplete(வெறுமை)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    திரும்பு curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    நிலையான வெறுமை
ins_compl_fuzzy_sort(வெறுமை)
{
    முழு	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    எனில் (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	எனில் ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    முழு none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_prev);
	    எனில் (!none_selected)
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
    நிலையான வெறுமை
ins_compl_new_leader(வெறுமை)
{
    முழு	    save_w_wrow = curwin->w_wrow;
    முழு	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_செருகு_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    எனில் (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.length);
	எனில் (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	எனில் (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    இல்லையெனில்
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	எனில் (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	எனில் (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	இல்லையெனில்
	    compl_autocomplete = FALSE;
	எனில் (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    எனில் (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    எனில் (compl_match_array == வெற்று)
	compl_enter_selects = FALSE;
    இல்லையெனில் எனில் (ins_compl_has_preinsert() && compl_leader.length > 0)
	ins_compl_செருகு(TRUE, FALSE);
    இல்லையெனில் எனில் (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.length > 0 && !ins_compl_preinsert_effect())
	ins_compl_செருகு(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    நிலையான முழு
get_compl_len(வெறுமை)
{
    முழு off = (முழு)curwin->w_cursor.col - (முழு)compl_col;
    திரும்பு MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    வெறுமை
ins_compl_addleader(முழு c)
{
    முழு	    cc;

    எனில் (ins_compl_preinsert_effect())
	ins_compl_delete();

    எனில் (stop_arrow() == FAIL)
	திரும்பு;
    எனில் (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	எனில் (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    இல்லையெனில்
    {
	ins_char(c);
	எனில் (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    எனில் (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    எனில் (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.length = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.length);
	எனில் (compl_leader.string == வெற்று)
	{
	    compl_leader.length = 0;
	    திரும்பு;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    நிலையான வெறுமை
ins_compl_restart(வெறுமை)
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
    compl_இலக்கம்_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    நிலையான வெறுமை
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    எனில் (match == வெற்று)
	திரும்பு;

    p = vim_strnsave(str, len);
    எனில் (p == வெற்று)
	திரும்பு;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.length = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    வெறுமை
ins_compl_addfrommatch(வெறுமை)
{
    char_u	*p;
    முழு		len = (முழு)curwin->w_cursor.col - (முழு)compl_col;
    முழு		c;

    p = compl_shown_match->cp_str.string;
    எனில் ((முழு)compl_shown_match->cp_str.length <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	எனில் (!match_at_original_text(compl_shown_match))
	    திரும்பு;

	p = வெற்று;
	plen = 0;
	க்கு (cp = compl_shown_match->cp_next; cp != வெற்று
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    எனில் (compl_leader.string == வெற்று
		    || ins_compl_equal(cp, compl_leader.string,
			(முழு)compl_leader.length))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.length;
		நிறுத்து;
	    }
	}
	எனில் (p == வெற்று || (முழு)plen <= len)
	    திரும்பு;
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
    நிலையான முழு
set_ctrl_x_mode(முழு c)
{
    முழு	    retval = FALSE;

    தேர்வு (c)
    {
	நிலை Ctrl_E:
	நிலை Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    எனில் (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    இல்லையெனில்
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = வெற்று;
	    showmode();
	    நிறுத்து;
	நிலை Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_வரி;
	    நிறுத்து;
	நிலை Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    நிறுத்து;
	நிலை Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    நிறுத்து;
	நிலை Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    எனில் (vpeekc() == '=')
		நிறுத்து;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    நிறுத்து;
	நிலை Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    நிறுத்து;
#ifdef FEAT_COMPL_FUNC
	நிலை Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    நிறுத்து;
	நிலை Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    நிறுத்து;
#endif
	நிலை 's':
	நிலை Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    நிறுத்து;
	நிலை Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    நிறுத்து;
#ifdef FEAT_FIND_ID
	நிலை Ctrl_I:
	நிலை K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    நிறுத்து;
	நிலை Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    நிறுத்து;
#endif
	நிலை Ctrl_V:
	நிலை Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    நிறுத்து;
	நிலை Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = வெற்று;
	    showmode();
	    retval = TRUE;
	    நிறுத்து;
	நிலை Ctrl_P:
	நிலை Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    எனில் (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    இல்லையெனில் எனில் (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	இயல்பு:
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
	    எனில் (c == Ctrl_X)
	    {
		எனில் (compl_cont_mode != 0)
		    compl_cont_status = 0;
		இல்லையெனில்
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = வெற்று;
	    showmode();
	    நிறுத்து;
    }

    திரும்பு retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    நிலையான வெறுமை
trigger_complete_done_event(முழு mode UNUSED, string_T *word UNUSED)
{
#எனில் defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    எனில் (word == வெற்று || word->string == வெற்று)
	(வெறுமை)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    இல்லையெனில்
	(வெறுமை)dict_add_string_len(v_event, "complete_word", word->string, (முழு)word->length);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (வெறுமை)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == வெற்று) ? (char_u *)"" : mode_str->string, (முழு)mode_str->length);

    dict_set_பொருட்கள்_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#எனில் defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    நிலையான முழு
ins_compl_stop(முழு c, முழு prev_mode, முழு retval)
{
    முழு		want_cindent;
    string_T	word = {வெற்று, 0};

    // Remove pre-inserted text when present.
    எனில் (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    எனில் (compl_curr_match != வெற்று || compl_leader.string != வெற்று || c == Ctrl_E)
    {
	char_u	*ptr = வெற்று;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	எனில் (compl_curr_match != வெற்று && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    எனில் (compl_cont_mode == CTRL_X_WHOLE_வரி)
    {
	// re-indent the current line
	எனில் (want_cindent)
	{
	    do_c_கோவை_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    இல்லையெனில் எனில் (!compl_autocomplete || compl_used_match)
    {
	முழு prev_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	எனில் (prev_col > 0)
	    dec_cursor();
	// only format when something was inserted
	எனில் (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	எனில் (prev_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    எனில் ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.length);
	எனில் (word.string == வெற்று)
	    word.length = 0;
	இல்லையெனில்
	    word.length = compl_shown_match->cp_str.length;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    எனில் (c == Ctrl_E)
    {
	char_u *p = வெற்று;
	size_t	plen = 0;

	ins_compl_delete();
	எனில் (compl_leader.string != வெற்று)
	{
	    p = compl_leader.string;
	    plen = compl_leader.length;
	}
	இல்லையெனில் எனில் (compl_first_match != வெற்று)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.length;
	}
	எனில் (p != வெற்று)
	{
	    முழு	    compl_len = get_compl_len();

	    எனில் ((முழு)plen > compl_len)
		ins_compl_செருகு_bytes(p + compl_len, (முழு)plen - compl_len);
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
    எனில் (!shortmess(SHM_COMPLETIONMENU))
	செய்தி_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    எனில் (edit_submode != வெற்று)
    {
	edit_submode = வெற்று;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_இலக்கம்_bests = 0;
    compl_ins_end_col = 0;

    எனில் (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    எனில் (want_cindent && in_cinkeys(சாவி_COMPLETE, ' ', inindent(0)))
	do_c_கோவை_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(prev_mode, &word);
    vim_free(word.string);

    திரும்பு retval;
}

/*
 * Cancel completion.
 */
    முழு
ins_compl_cancel(வெறுமை)
{
    திரும்பு ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    முழு
ins_compl_prep(முழு c)
{
    முழு	    retval = FALSE;
    முழு	    prev_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    எனில் (c != Ctrl_R && vim_is_ctrl_x_சாவி(c))
	edit_submode_extra = வெற்று;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    எனில் (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	திரும்பு retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    எனில் (is_mouse_சாவி(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	எனில் (c == K_LEFTRELEASE
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
	    திரும்பு retval;
	எனில் (popup_visible)
	{
	    முழு	    row = mouse_row;
	    முழு	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    எனில் (wp != வெற்று && WIN_IS_POPUP(wp))
		திரும்பு retval;
	}
    }
#endif

    எனில் (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	எனில் (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_சாவி(c)
		|| !vim_is_ctrl_x_சாவி(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    எனில் (c == Ctrl_Z)
		retval = TRUE;
	}
	இல்லையெனில்
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    எனில் (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    எனில் (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    இல்லையெனில் எனில் (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	எனில் (c != Ctrl_R && !vim_is_ctrl_x_சாவி(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = வெற்று;
	}
	showmode();
    }

    எனில் (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	எனில் ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_சாவி(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, prev_mode, retval);
    }
    இல்லையெனில் எனில் (ctrl_x_mode == CTRL_X_LOCAL_செய்தி)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, வெற்று);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    எனில் (c != Ctrl_R && !vim_is_ctrl_x_சாவி(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    திரும்பு retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    நிலையான வெறுமை
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    முழு	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    எனில் (ptr == வெற்று)
    {
	எனில் (compl_leader.string != வெற்று)
	    ptr = compl_leader.string;
	இல்லையெனில்
	    திரும்பு;  // nothing to do
    }
    எனில் (compl_orig_text.string != வெற்று)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	போது (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	எனில் (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	க்கு (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    எனில் (ptr != வெற்று)
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
    நிலையான buf_T *
ins_compl_next_buf(buf_T *buf, முழு flag)
{
    நிலையான win_T    *wp = வெற்று;
    முழு		    skip_buffer;

    எனில் (flag == 'w')		// just windows
    {
	எனில் (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	போது (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_next != வெற்று) ? wp->w_next : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    எனில் (wp == curwin || !wp->w_buffer->b_scanned)
		நிறுத்து;
	}
	buf = wp->w_buffer;
    }
    இல்லையெனில்
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	போது (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_next != வெற்று) ? buf->b_next : firstbuf;
	    // Break if we're back at start buffer
	    எனில் (buf == curbuf)
		நிறுத்து;

	    // Check buffer conditions based on flag
	    எனில் (flag == 'U')
		skip_buffer = buf->b_p_bl;
	    இல்லையெனில்
		skip_buffer = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == வெற்று) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    எனில் (!skip_buffer && !buf->b_scanned)
		நிறுத்து;
	}
    }
    திரும்பு buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    நிலையான முழு
get_cpt_sources_மொத்தம்(char_u *cpt)
{
    char_u  dummy[LSIZE];
    முழு	    மொத்தம் = 0;
    char_u  *p;

    க்கு (p = cpt; *p != NUL; )
    {
	போது (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	எனில் (*p != NUL)
	{
	    (வெறுமை)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    மொத்தம்++;
	}
    }

    திரும்பு மொத்தம்;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
நிலையான callback_T cfu_cb;	    // 'completefunc' callback function
நிலையான callback_T ofu_cb;	    // 'omnifunc' callback function
நிலையான callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
நிலையான callback_T *cpt_cb;	    // Callback functions associated with F{func}
நிலையான முழு	  cpt_cb_மொத்தம்;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    நிலையான வெறுமை
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    எனில் (globcb->cb_name != வெற்று && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    எழுத்து *
did_set_completefunc(optset_T *args)
{
    முழு	retval;

    எனில் (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    இல்லையெனில்
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	எனில் (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    திரும்பு retval == FAIL ? e_invalid_argument : வெற்று;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    வெறுமை
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
    எழுத்து *
did_set_omnifunc(optset_T *args)
{
    முழு	retval;

    எனில் (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    இல்லையெனில்
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	எனில் (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    திரும்பு retval == FAIL ? e_invalid_argument : வெற்று;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    வெறுமை
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    வெறுமை
clear_cpt_callbacks(callback_T **callbacks, முழு மொத்தம்)
{
    எனில் (callbacks == வெற்று || *callbacks == வெற்று)
	திரும்பு;

    க்கு (முழு i = 0; i < மொத்தம்; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    நிலையான முழு
copy_cpt_callbacks(callback_T **dest, முழு *dest_cnt, callback_T *src, முழு cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    எனில் (cnt == 0)
	திரும்பு OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    எனில் (*dest == வெற்று)
	திரும்பு FAIL;

    *dest_cnt = cnt;

    க்கு (முழு i = 0; i < cnt; i++)
	எனில் (src[i].cb_name != வெற்று && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    திரும்பு OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    வெறுமை
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    எனில் (buf == வெற்று)
	திரும்பு;
    (வெறுமை)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_மொத்தம், cpt_cb,
	    cpt_cb_மொத்தம்);
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
    நிலையான முழு
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, முழு *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    முழு	    idx = 0;
    முழு	    slen;
    முழு	    மொத்தம்;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    மொத்தம் = get_cpt_sources_மொத்தம்(cpt);
    எனில் (மொத்தம் == 0)
	திரும்பு OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, மொத்தம்);
    எனில் (*cbp == வெற்று)
	திரும்பு FAIL;
    *cnt = மொத்தம்;

    க்கு (p = cpt; *p != NUL; )
    {
	போது (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	எனில் (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    எனில் (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		எனில் (caret != வெற்று)
		    *caret = NUL;

		எனில் (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = வெற்று;
	    }
	    idx++;
	}
    }

    திரும்பு OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    முழு
set_cpt_callbacks(optset_T *args)
{
    முழு	    opt_flags = args->os_flags;

    எனில் (curbuf == வெற்று)
	திரும்பு FAIL;

    // ":setglobal" does not change the buffer-local option value
    எனில் (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_மொத்தம்) != OK)
	திரும்பு FAIL;

    // ":setlocal" does not change the global option value
    எனில் (opt_flags & OPT_LOCAL)
	திரும்பு OK;

    // Cache the callbacks for the global value
    எனில் (opt_flags & OPT_GLOBAL)
	திரும்பு parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_மொத்தம்);

    // set case
    திரும்பு copy_cpt_callbacks(&cpt_cb, &cpt_cb_மொத்தம், curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_மொத்தம்);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    எழுத்து *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    முழு	retval;

    எனில் (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    இல்லையெனில்
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	எனில் (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    திரும்பு retval == FAIL ? e_invalid_argument : வெற்று;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    முழு
set_ref_in_cpt_callbacks(callback_T *callbacks, முழு மொத்தம், முழு copyID)
{
    முழு	abort = FALSE;

    எனில் (callbacks == வெற்று)
	திரும்பு FALSE;

    க்கு (முழு i = 0; i < மொத்தம்; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    திரும்பு abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    முழு
set_ref_in_insexpand_funcs(முழு copyID)
{
    முழு abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_மொத்தம், copyID);

    திரும்பு abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    நிலையான char_u *
get_complete_funcname(முழு type)
{
    தேர்வு (type)
    {
	நிலை CTRL_X_FUNCTION:
	    திரும்பு curbuf->b_p_cfu;
	நிலை CTRL_X_OMNI:
	    திரும்பு curbuf->b_p_ofu;
	நிலை CTRL_X_THESAURUS:
	    திரும்பு *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	இயல்பு:
	    திரும்பு (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    நிலையான callback_T *
get_செருகு_callback(முழு type)
{
    எனில் (type == CTRL_X_FUNCTION)
	திரும்பு &curbuf->b_cfu_cb;
    எனில் (type == CTRL_X_OMNI)
	திரும்பு &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    திரும்பு (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    நிலையான வெறுமை
expand_by_function(முழு type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = வெற்று;
    dict_T	*matchdict = வெற்று;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    முழு		save_State = State;
    முழு		retval;
    முழு		is_cpt_function = (cb != வெற்று);
    முழு		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    எனில் (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	எனில் (*funcname == NUL)
	    திரும்பு;
	cb = get_செருகு_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_எண்;
    args[0].vval.v_எண் = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != வெற்று ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    எனில் (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    எனில் (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    எனில் (retval == OK)
    {
	தேர்வு (rettv.v_type)
	{
	    நிலை VAR_LIST:
		matchlist = rettv.vval.v_list;
		நிறுத்து;
	    நிலை VAR_DICT:
		matchdict = rettv.vval.v_dict;
		நிறுத்து;
	    நிலை VAR_SPECIAL:
		எனில் (rettv.vval.v_எண் == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    இயல்பு:
		// TODO: Give error message?
		clear_tv(&rettv);
		நிறுத்து;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    எனில் (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	செல் theend;
    }

    எனில் (matchlist != வெற்று)
	ins_compl_add_list(matchlist);
    இல்லையெனில் எனில் (matchdict != வெற்று)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    எனில் (matchdict != வெற்று)
	dict_unref(matchdict);
    எனில் (matchlist != வெற்று)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#எனில் defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    நிலையான உள்வரி முழு
get_user_highlight_id(char_u *hlname)
{
    எனில் (hlname != வெற்று && *hlname != NUL)
	திரும்பு syn_check_group(hlname, (முழு)STRLEN(hlname));
    திரும்பு 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    நிலையான முழு
ins_compl_add_tv(typval_T *tv, முழு dir, முழு fast)
{
    char_u	*word;
    முழு		dup = FALSE;
    முழு		empty = FALSE;
    முழு		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_மொத்தம்]);
    typval_T	user_data;
    முழு		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    முழு		user_hl[2] = { 0, 0 };

    user_data.v_type = VAR_UNKNOWN;
    எனில் (tv->v_type == VAR_DICT && tv->vval.v_dict != வெற்று)
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
	எனில் (dict_get_string(tv->vval.v_dict, "icase", FALSE) != வெற்று
				  && dict_get_எண்(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	எனில் (dict_get_string(tv->vval.v_dict, "dup", FALSE) != வெற்று)
	    dup = dict_get_எண்(tv->vval.v_dict, "dup");
	எனில் (dict_get_string(tv->vval.v_dict, "empty", FALSE) != வெற்று)
	    empty = dict_get_எண்(tv->vval.v_dict, "empty");
	எனில் (dict_get_string(tv->vval.v_dict, "equal", FALSE) != வெற்று
				  && dict_get_எண்(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    இல்லையெனில்
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    எனில் (word == வெற்று || (!empty && *word == NUL))
    {
	clear_tv(&user_data);
	திரும்பு FAIL;
    }
    status = ins_compl_add(word, -1, வெற்று, cptext,
	    &user_data, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    எனில் (status != OK)
	clear_tv(&user_data);
    திரும்பு status;
}

/*
 * Add completions from a list.
 */
    நிலையான வெறுமை
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    முழு		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_பொருட்கள்(list, li)
    {
	எனில் (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	இல்லையெனில் எனில் (did_emsg)
	    நிறுத்து;
    }
}

/*
 * Add completions from a dict.
 */
    நிலையான வெறுமை
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_சொற்கள்;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    எனில் (di_refresh != வெற்று && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	எனில் (v != வெற்று && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_சொற்கள் = dict_find(dict, (char_u *)"words", 5);
    எனில் (di_சொற்கள் != வெற்று && di_சொற்கள்->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_சொற்கள்->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    நிலையான வெறுமை
set_completion(colnr_T startcol, list_T *list)
{
    முழு save_w_wrow = curwin->w_wrow;
    முழு save_w_leftcol = curwin->w_leftcol;
    முழு flags = CP_ORIGINAL_TEXT;
    unsigned முழு cur_cot_flags = get_cot_flags();
    முழு compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    முழு compl_no_செருகு = (cur_cot_flags & COT_NOINSERT) != 0;
    முழு compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    எனில் (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    எனில் (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_length = (முழு)curwin->w_cursor.col - (முழு)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_length);
    எனில் (p_ic)
	flags |= CP_ICASE;
    எனில் (compl_orig_text.string == வெற்று)
    {
	compl_orig_text.length = 0;
	திரும்பு;
    }
    compl_orig_text.length = (size_t)compl_length;
    எனில் (ins_compl_add(compl_orig_text.string,
			(முழு)compl_orig_text.length, வெற்று, வெற்று, வெற்று, 0,
			flags | CP_FAST, FALSE, வெற்று, FUZZY_SCORE_NONE) != OK)
	திரும்பு;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    முழு no_select = compl_no_select || compl_longest;
    எனில் (compl_no_செருகு || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	எனில் (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    இல்லையெனில்
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_செருகு;

    // Lazily show the popup menu, unless we got interrupted.
    எனில் (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    வெறுமை
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் ((State & MODE_செருகு) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_செருகு_mode));
	திரும்பு;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    எனில் (!undo_allowed())
	திரும்பு;

    எனில் (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	முழு startcol = (முழு)tv_get_எண்_chk(&argvars[0], வெற்று);
	எனில் (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    வெறுமை
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    வெறுமை
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_சாவிகள்(0, TRUE);
    rettv->vval.v_எண் = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    நிலையான வெறுமை
ins_compl_mode(string_T *ret)
{
    எனில் (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == வெற்று) ? (char_u *)"" : mode->string;
	ret->length = mode->length;
    }
    இல்லையெனில்
    {
	ret->string = (char_u *)"";
	ret->length = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    நிலையான வெறுமை
ins_compl_update_sequence_numbers(வெறுமை)
{
    முழு		எண் = 0;
    compl_T	*match;

    எனில் (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	க்கு (match = compl_curr_match->cp_prev; match != வெற்று
		&& !is_first_match(match); match = match->cp_prev)
	    எனில் (match->cp_எண் != -1)
	    {
		எண் = match->cp_எண்;
		நிறுத்து;
	    }
	எனில் (match != வெற்று)
	    // go up and assign all numbers which are not assigned yet
	    க்கு (match = match->cp_next;
		    match != வெற்று && match->cp_எண் == -1;
					   match = match->cp_next)
		match->cp_எண் = ++எண்;
    }
    இல்லையெனில் // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	க்கு (match = compl_curr_match->cp_next; match != வெற்று
		&& !is_first_match(match); match = match->cp_next)
	{
	    எனில் (match->cp_எண் != -1)
	    {
		எண் = match->cp_எண்;
		நிறுத்து;
	    }
	}
	எனில் (match != வெற்று)
	{
	    // go down and assign all numbers which are not assigned yet
	    க்கு (match = match->cp_prev; match
		    && match->cp_எண் == -1;
					   match = match->cp_prev)
		match->cp_எண் = ++எண்;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    நிலையான வெறுமை
fill_complete_info_dict(dict_T *di, compl_T *match, முழு add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (முழு)match->cp_str.length);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    எனில் (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    எனில் (match->cp_user_data.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    இல்லையெனில்
	dict_add_tv(di, "user_data", &match->cp_user_data);
}

/*
 * Get complete information
 */
    நிலையான வெறுமை
get_complete_info(list_T *what_list, dict_T *retdict)
{
    முழு		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_பொருட்கள்		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    முழு		what_flag;

    எனில் (what_list == வெற்று)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    இல்லையெனில்
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_பொருட்கள்(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    எனில் (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    இல்லையெனில் எனில் (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    இல்லையெனில் எனில் (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_பொருட்கள்;
	    இல்லையெனில் எனில் (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    இல்லையெனில் எனில் (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    இல்லையெனில் எனில் (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    இல்லையெனில் எனில் (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    இல்லையெனில் எனில் (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    எனில் (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (முழு)mode.length);
    }

    எனில் (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_எண்(retdict, "pum_visible", pum_visible());

    எனில் (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_எண்(retdict, "auto", compl_autostarted);

    எனில் (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*வரி = ml_get_curline();
	முழு	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? வரி + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    எனில் (ret == OK && (what_flag & (CI_WHAT_பொருட்கள் | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = வெற்று;
	dict_T	    *di;
	compl_T     *match;
	முழு         selected_idx = -1;
	முழு	    has_பொருட்கள் = what_flag & CI_WHAT_பொருட்கள்;
	முழு	    has_matches = what_flag & CI_WHAT_MATCHES;
	முழு	    has_completed = what_flag & CI_WHAT_COMPLETED;

	எனில் (has_பொருட்கள் || has_matches)
	{
	    li = list_alloc();
	    எனில் (li == வெற்று)
		திரும்பு;
	    ret = dict_add_list(retdict, (has_matches && !has_பொருட்கள்)
						? "matches" : "items", li);
	    எனில் (ret == FAIL)
		list_unref(li);
	}
	எனில் (ret == OK && what_flag & CI_WHAT_SELECTED)
	    எனில் (compl_curr_match != வெற்று && compl_curr_match->cp_எண் == -1)
		ins_compl_update_sequence_numbers();
	எனில் (ret == OK && compl_first_match != வெற்று)
	{
	    முழு list_idx = 0;
	    match = compl_first_match;
	    செய்
	    {
		எனில் (!match_at_original_text(match))
		{
		    எனில் (has_பொருட்கள்
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			எனில் (di == வெற்று)
			    திரும்பு;
			ret = list_append_dict(li, di);
			எனில் (ret != OK)
			{
			    dict_unref(di);
			    திரும்பு;
			}
			fill_complete_info_dict(di, match, has_matches && has_பொருட்கள்);
		    }
		    எனில் (compl_curr_match != வெற்று
			    && compl_curr_match->cp_எண் == match->cp_எண்)
			selected_idx = list_idx;
		    எனில் (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_next;
	    }
	    போது (match != வெற்று && !is_first_match(match));
	}
	எனில் (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_எண்(retdict, "selected", selected_idx);

	எனில் (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    எனில் (di == வெற்று)
		திரும்பு;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    எனில் (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    வெறுமை
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = வெற்று;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	எனில் (check_for_list_arg(argvars, 0) == FAIL)
	    திரும்பு;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    நிலையான முழு
thesaurus_func_complete(முழு type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    திரும்பு type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    நிலையான முழு
may_advance_cpt_index(char_u *cpt)
{
    char_u  *p = cpt;

    எனில் (cpt_sources_index == -1)
	திரும்பு FALSE;
    போது (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    திரும்பு (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
எண்ணிக்கை
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
வகைவரை கட்டமைப்பு
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	prev_match_pos;		// previous match position
    முழு		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    முழு		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    முழு		dict_f;			// "dict" is an exact file name or not
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
    நிலையான முழு
process_next_cpt_மதிப்பு(
	ins_compl_next_state_T *st,
	முழு		*compl_type_arg,
	pos_T		*start_match_pos,
	முழு		fuzzy_collect,
	முழு		*advance_cpt_idx)
{
    முழு	    compl_type = -1;
    முழு	    status = INS_COMPL_CPT_OK;
    முழு	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    போது (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    எனில் (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	எனில் (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_வரி_மொத்தம்;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    இல்லையெனில் எனில் (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != வெற்று
	    && (st->ins_buf = ins_compl_next_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	எனில் (st->ins_buf->b_ml.ml_mfp != வெற்று)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_வரி_மொத்தம் + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	இல்லையெனில்	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    எனில் (st->ins_buf->b_fname == வெற்று)
	    {
		status = INS_COMPL_CPT_CONT;
		செல் done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	எனில் (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    செய்தி_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == வெற்று
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == வெற்று
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (வெறுமை)செய்தி_trunc_attr((எழுத்து *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    இல்லையெனில் எனில் (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    இல்லையெனில்
    {
	எனில் (ctrl_x_mode_வரி_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	இல்லையெனில் எனில் (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_index);
	    எனில் (!st->func_cb)
		compl_type = -1;
	}
#endif
	இல்லையெனில் எனில் (!skip_source)
	{
	    எனில் (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		எனில் (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		இல்லையெனில்
		    compl_type = CTRL_X_THESAURUS;
		எனில் (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    இல்லையெனில் எனில் (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    இல்லையெனில் எனில் (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    இல்லையெனில் எனில் (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		எனில் (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    செய்தி_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("Scanning tags."));
		    (வெறுமை)செய்தி_trunc_attr((எழுத்து *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    இல்லையெனில்
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(வெறுமை)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_index(st->e_cpt);

	st->found_all = TRUE;
	எனில் (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    திரும்பு status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    நிலையான வெறுமை
get_next_include_file_completion(முழு compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (முழு)compl_pattern.length, FALSE, FALSE,
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
    நிலையான வெறுமை
get_next_dict_tsr_completion(முழு compl_type, char_u *dict, முழு dict_f)
{
#ifdef FEAT_COMPL_FUNC
    எனில் (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, வெற்று);
    இல்லையெனில்
#endif
	ins_compl_dictionaries(
		dict != வெற்று ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != வெற்று ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    நிலையான வெறுமை
get_next_tag_completion(வெறுமை)
{
    முழு		save_p_ic;
    char_u	**matches;
    முழு		இலக்கம்_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    எனில் (find_tags(compl_pattern.string, &இலக்கம்_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && இலக்கம்_matches > 0)
	ins_compl_add_matches(இலக்கம்_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    நிலையான முழு
compare_scores(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    முழு idx_a = *(மாறிலி முழு *)a;
    முழு idx_b = *(மாறிலி முழு *)b;
    முழு score_a = compl_fuzzy_scores[idx_a];
    முழு score_b = compl_fuzzy_scores[idx_b];
    திரும்பு score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    நிலையான வெறுமை
ins_compl_longest_செருகு(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_செருகு_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    நிலையான வெறுமை
fuzzy_longest_match(வெறுமை)
{
    char_u	*prefix = வெற்று;
    முழு		prefix_len = 0;
    முழு		i = 0;
    முழு		j = 0;
    char_u	*match_str = வெற்று;
    char_u	*prefix_ptr = வெற்று;
    char_u	*match_ptr = வெற்று;
    char_u	*leader = வெற்று;
    size_t	leader_len = 0;
    compl_T	*compl = வெற்று;
    முழு		more_candidates = FALSE;
    compl_T	*nn_compl = வெற்று;

    எனில் (compl_இலக்கம்_bests == 0)
	திரும்பு;

    nn_compl = compl_first_match->cp_next->cp_next;
    எனில் (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_வரி() ? compl_first_match
				    : compl_first_match->cp_next;
    எனில் (compl_இலக்கம்_bests == 1)
    {
	// no more candidates insert the match str
	எனில் (!more_candidates)
	{
	    ins_compl_longest_செருகு(compl->cp_str.string);
	    compl_இலக்கம்_bests = 0;
	}
	compl_இலக்கம்_bests = 0;
	திரும்பு;
    }

    எனில் ((size_t)compl_இலக்கம்_bests > SIZE_MAX / அளவு(compl_T *))
	திரும்பு;
    compl_best_matches = (compl_T **)alloc(compl_இலக்கம்_bests * அளவு(compl_T *));
    எனில் (compl_best_matches == வெற்று)
	திரும்பு;
    போது (compl != வெற்று && i < compl_இலக்கம்_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_next;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (முழு)compl_best_matches[0]->cp_str.length;

    க்கு (i = 1; i < compl_இலக்கம்_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	போது (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    எனில் (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		நிறுத்து;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	எனில் (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    எனில் (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	செல் end;

    prefix = vim_strnsave(prefix, prefix_len);
    எனில் (prefix != வெற்று)
    {
	ins_compl_longest_செருகு(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = வெற்று;
    compl_இலக்கம்_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    நிலையான வெறுமை
get_next_filename_completion(வெறுமை)
{
    char_u	**matches;
    முழு		இலக்கம்_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    முழு		i;
    முழு		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    முழு		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    முழு		*fuzzy_indices_data;
    char_u	*last_sep = வெற்று;
    முழு		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    முழு		max_score = 0;
    முழு		current_score = 0;
    முழு		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    எழுத்து pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#இல்லையெனில்
    எழுத்து pathsep = PATHSEP;
#endif

    எனில் (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	எனில் (curbuf->b_p_csl[0] == 's')
	{
	    க்கு (i = 0; i < (முழு)leader_len; i++)
	    {
		எனில் (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	இல்லையெனில் எனில் (curbuf->b_p_csl[0] == 'b')
	{
	    க்கு (i = 0; i < (முழு)leader_len; i++)
	    {
		எனில் (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	எனில் (last_sep == வெற்று)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    எனில் (compl_pattern.string == வெற்று)
		திரும்பு;
	    compl_pattern.length = 1;
	}
	இல்லையெனில் எனில் (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	இல்லையெனில்
	{
	    // Split leader into path and file parts
	    முழு path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    எனில் (path_with_wildcard != வெற்று)
	    {
		vim_snprintf((எழுத்து *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.length = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    எனில் (expand_wildcards(1, &compl_pattern.string, &இலக்கம்_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	திரும்பு;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, இலக்கம்_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    எனில் (curbuf->b_p_csl[0] != NUL)
    {
	க்கு (i = 0; i < இலக்கம்_matches; ++i)
	{
	    ptr = matches[i];
	    போது (*ptr != NUL)
	    {
		எனில் (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		இல்லையெனில் எனில் (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    எனில் (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, அளவு(முழு), 10);
	compl_fuzzy_scores = (முழு *)alloc(அளவு(முழு) * இலக்கம்_matches);

	க்கு (i = 0; i < இலக்கம்_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    எனில் (score != FUZZY_SCORE_NONE)
	    {
		எனில் (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((முழு *)fuzzy_indices.ga_data)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	எனில் (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = வெற்று;
	    fuzzy_indices_data = (முழு *)fuzzy_indices.ga_data;
	    qsort(fuzzy_indices_data, fuzzy_indices.ga_len, அளவு(முழு), compare_scores);

	    க்கு (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_data[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_data[i]];
		எனில் (ins_compl_add(match, -1, வெற்று, வெற்று, வெற்று, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, வெற்று, current_score) == OK)
		    dir = FORWARD;

		எனில் (need_collect_bests)
		{
		    எனில் (i == 0 || current_score == max_score)
		    {
			compl_இலக்கம்_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(இலக்கம்_matches, matches);
	}
	இல்லையெனில் எனில் (leader_len > 0)
	{
	    FreeWild(இலக்கம்_matches, matches);
	    இலக்கம்_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	எனில் (compl_இலக்கம்_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	திரும்பு;
    }

    எனில் (இலக்கம்_matches > 0)
	ins_compl_add_matches(இலக்கம்_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    நிலையான வெறுமை
get_next_cmdline_completion(வெறுமை)
{
    char_u	**matches;
    முழு		இலக்கம்_matches;

    எனில் (expand_cmdline(&compl_xp, compl_pattern.string,
		(முழு)compl_pattern.length, &இலக்கம்_matches, &matches) == EXPAND_OK)
    {
	முழு		add_r = OK;
	முழு		dir = compl_direction;

	க்கு (முழு i = 0; i < இலக்கம்_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_மொத்தம்]) = {வெற்று, வெற்று, வெற்று, வெற்று};

	    எனில் (compl_xp.xp_files_abbr != வெற்று)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    எனில் (compl_xp.xp_files_kind != வெற்று)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    எனில் (compl_xp.xp_files_menu != வெற்று)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    எனில் (compl_xp.xp_files_info != வெற்று)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, வெற்று, cptext, வெற்று, dir,
		    CP_FAST, FALSE, வெற்று, FUZZY_SCORE_NONE);
	    எனில் (add_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(இலக்கம்_matches, matches);
	free_xp_files_extra(&compl_xp, இலக்கம்_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    நிலையான வெறுமை
get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    முழு		இலக்கம்_matches;

    இலக்கம்_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    எனில் (இலக்கம்_matches > 0)
	ins_compl_add_matches(இலக்கம்_matches, matches, p_ic);
    இல்லையெனில்
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    நிலையான char_u *
ins_compl_get_next_word_or_வரி(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	முழு	*match_len,
	முழு	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    முழு		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (முழு)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    எனில் (ctrl_x_mode_வரி_or_eval())
    {
	எனில் (compl_status_adding())
	{
	    எனில் (cur_match_pos->lnum >= ins_buf->b_ml.ml_வரி_மொத்தம்)
		திரும்பு வெற்று;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    எனில் (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (முழு)(ptr - tmp_ptr);
	    }
	}
    }
    இல்லையெனில்
    {
	char_u	*tmp_ptr = ptr;

	எனில் (compl_status_adding() && compl_length <= len)
	{
	    tmp_ptr += compl_length;
	    // Skip if already inside a word.
	    எனில் (vim_iswordp(tmp_ptr))
		திரும்பு வெற்று;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (முழு)(tmp_ptr - ptr);

	எனில் (compl_status_adding() && len == compl_length)
	{
	    எனில் (cur_match_pos->lnum < ins_buf->b_ml.ml_வரி_மொத்தம்)
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
		எனில் (tmp_ptr > ptr)
		{
		    எனில் (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			எனில் (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			எனில் (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== வெற்று
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    எனில் (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (முழு)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    எனில் (len == compl_length)
		திரும்பு வெற்று;
	}
    }

    *match_len = len;
    திரும்பு ptr;
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
    நிலையான முழு
get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    முழு		found_new_match = FAIL;
    முழு		save_p_scs;
    முழு		save_p_ws;
    முழு		looped_around = FALSE;
    char_u	*ptr = வெற்று;
    முழு		len = 0;
    முழு		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_length > 0;
    char_u	*leader = ins_compl_leader();
    முழு		score = FUZZY_SCORE_NONE;
    முழு		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    எனில் (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    எனில் (!in_curbuf)
	p_ws = FALSE;
    இல்லையெனில் எனில் (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    க்கு (;;)
    {
	முழு	cont_s_ipos = FALSE;

	++செய்தி_silent;  // Don't want messages for wrapscan.

	எனில் (in_fuzzy_collect)
	{
	    found_new_match = தேடல்_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	இல்லையெனில் எனில் (ctrl_x_mode_whole_வரி() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = தேடல்_for_exact_வரி(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	இல்லையெனில்
	    found_new_match = searchit(வெற்று, st->ins_buf, st->cur_match_pos,
				வெற்று, compl_direction, compl_pattern.string, (முழு)compl_pattern.length,
				1L, தேடல்_KEEP + தேடல்_NFMSG, RE_LAST, வெற்று);
	--செய்தி_silent;
	எனில் (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	இல்லையெனில் எனில் (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	இல்லையெனில் எனில் (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    எனில் (looped_around)
		found_new_match = FAIL;
	    இல்லையெனில்
		looped_around = TRUE;
	}
	இல்லையெனில் எனில் (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    எனில் (looped_around)
		found_new_match = FAIL;
	    இல்லையெனில்
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	எனில் (found_new_match == FAIL)
	    நிறுத்து;

	// when ADDING, the text before the cursor matches, skip it
	எனில் (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    தொடர்;

	எனில் (!in_fuzzy_collect)
	    ptr = ins_compl_get_next_word_or_வரி(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	எனில் (ptr == வெற்று || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    தொடர்;

	எனில் (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    எனில் (score < 0)
		score = -score;
	}

	எனில் (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? வெற்று : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    எனில் (in_fuzzy_collect && score == compl_first_match->cp_next->cp_score)
		compl_இலக்கம்_bests++;
	    found_new_match = OK;
	    நிறுத்து;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    திரும்பு found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    நிலையான callback_T *
get_callback_if_cpt_func(char_u *p, முழு idx)
{
    எனில் (*p == 'o')
	திரும்பு &curbuf->b_ofu_cb;

    எனில் (*p == 'F')
    {
	எனில் (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    எனில் (curbuf->b_p_cpt_cb == வெற்று
		    || idx < 0 || idx >= curbuf->b_p_cpt_மொத்தம்)
		திரும்பு வெற்று;
	    திரும்பு curbuf->b_p_cpt_cb[idx].cb_name != வெற்று
		? &curbuf->b_p_cpt_cb[idx] : வெற்று;
	}
	இல்லையெனில்
	    திரும்பு &curbuf->b_cfu_cb; // 'cfu'
    }

    திரும்பு வெற்று;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    நிலையான வெறுமை
get_register_completion(வெறுமை)
{
    முழு		dir = compl_direction;
    yankreg_T	*reg = வெற்று;
    வெறுமை	*reg_ptr = வெற்று;
    முழு		adding_mode = compl_status_adding();

    க்கு (முழு i = 0; i < இலக்கம்_REGISTERS; i++)
    {
	முழு regname = 0;
	எனில் (i == 0)
	    regname = '"';    // unnamed register
	இல்லையெனில் எனில் (i < 10)
	    regname = '0' + i;
	இல்லையெனில் எனில் (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	இல்லையெனில் எனில் (i == STAR_REGISTER)
	    regname = '*';
	இல்லையெனில் எனில் (i == PLUS_REGISTER)
	    regname = '+';
#endif
	இல்லையெனில்
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	எனில் (!valid_yank_reg(regname, FALSE) || regname == '_')
	    தொடர்;

	reg_ptr = get_register(regname, FALSE);
	எனில் (reg_ptr == வெற்று)
	    தொடர்;

	reg = (yankreg_T *)reg_ptr;

	க்கு (முழு j = 0; j < reg->y_size; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    எனில் (str == வெற்று)
		தொடர்;

	    எனில் (adding_mode)
	    {
		முழு str_len = (முழு)STRLEN(str);
		எனில் (str_len == 0)
		    தொடர்;

		எனில் (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0))
		{
		    எனில் (ins_compl_add_infercase(str, str_len, p_ic, வெற்று,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    இல்லையெனில்
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		போது (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    எனில் (p >= str_end || *p == NUL)
			நிறுத்து;

		    char_u *word_end = find_word_end(p);

		    எனில் (word_end <= p)
		    {
			எனில் (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			இல்லையெனில்
			    word_end = p + 1;
		    }

		    எனில் (word_end > str_end)
			word_end = str_end;

		    முழு len = (முழு)(word_end - p);
		    எனில் (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0)))
		    {
			எனில் (ins_compl_add_infercase(p, len, p_ic, வெற்று,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    எனில் (p <= old_p)
		    {
			p = old_p + 1;
			எனில் (has_mbyte && p < str_end)
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
    நிலையான முழு
get_next_completion_match(முழு type, ins_compl_next_state_T *st, pos_T *ini)
{
    முழு	found_new_match = FALSE;

    தேர்வு (type)
    {
	நிலை -1:
	    நிறுத்து;
#ifdef FEAT_FIND_ID
	நிலை CTRL_X_PATH_PATTERNS:
	நிலை CTRL_X_PATH_DEFINES:
	    get_next_include_file_completion(type);
	    நிறுத்து;
#endif

	நிலை CTRL_X_DICTIONARY:
	நிலை CTRL_X_THESAURUS:
	    get_next_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = வெற்று;
	    நிறுத்து;

	நிலை CTRL_X_TAGS:
	    get_next_tag_completion();
	    நிறுத்து;

	நிலை CTRL_X_FILES:
	    get_next_filename_completion();
	    நிறுத்து;

	நிலை CTRL_X_CMDLINE:
	நிலை CTRL_X_CMDLINE_CTRL_X:
	    get_next_cmdline_completion();
	    நிறுத்து;

#ifdef FEAT_COMPL_FUNC
	நிலை CTRL_X_FUNCTION:
	    எனில் (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    இல்லையெனில்
		expand_by_function(type, compl_pattern.string, வெற்று);
	    நிறுத்து;
	நிலை CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, வெற்று);
	    நிறுத்து;
#endif

	நிலை CTRL_X_SPELL:
	    get_next_spell_completion(st->first_match_pos.lnum);
	    நிறுத்து;

	நிலை CTRL_X_REGISTER:
	    get_register_completion();
	    நிறுத்து;

	இயல்பு:	// normal ^P/^N and ^X^L
	    found_new_match = get_next_default_completion(st, ini);
	    எனில் (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    எனில் (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    திரும்பு found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    நிலையான வெறுமை
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *பட = str, *லிக = str, *p;

    எனில் (str == வெற்று)
	திரும்பு;

    போது (*பட)
    {
	எனில் (*பட == '^')
	{
	    p = பட + 1;
	    போது (vim_isdigit(*p))
		p++;
	    எனில் ((*p == ',' || *p == '\0') && p != பட + 1)
	    {
		பட = p;
		தொடர்;
	    }
	    இல்லையெனில்
		*லிக++ = *பட++;
	}
	இல்லையெனில்
	    *லிக++ = *பட++;
    }
    *லிக = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    நிலையான முழு
prepare_cpt_compl_funcs(வெறுமை)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = வெற்று;
    முழு		idx = 0;
    முழு		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    எனில் (cpt == வெற்று)
	திரும்பு FAIL;
    strip_caret_numbers_in_place(cpt);

    க்கு (p = cpt; *p;)
    {
	போது (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	எனில் (*p == NUL)
	    நிறுத்து;

	cb = get_callback_if_cpt_func(p, idx);
	எனில் (cb)
	{
	    எனில் (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		எனில் (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		இல்லையெனில்
		    startcol = -2;
	    }
	    இல்லையெனில் எனில் (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	இல்லையெனில்
	    cpt_sources_array[idx].cs_startcol = -3;

	(வெறுமை)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    திரும்பு OK;
#endif
    திரும்பு FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    நிலையான வெறுமை
compl_source_start_timer(முழு source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    எனில் (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    நிலையான முழு
advance_cpt_sources_index_safe(வெறுமை)
{
    எனில் (cpt_sources_index >= 0 && cpt_sources_index < cpt_sources_மொத்தம் - 1)
    {
	cpt_sources_index++;
	திரும்பு OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_index_out_of_range_nr), cpt_sources_index);
#endif
    திரும்பு FAIL;
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
    நிலையான முழு
ins_compl_get_exp(pos_T *ini)
{
    நிலையான ins_compl_next_state_T   st;
    நிலையான முழு			    st_cleared = FALSE;
    முழு		match_மொத்தம்;
    முழு		found_new_match;
    முழு		type = ctrl_x_mode;
    முழு		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    முழு		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    எனில் (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	எனில் (!st_cleared)
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
	st.e_cpt = st.e_cpt_copy == வெற்று ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_வரி_மொத்தம்	1000
	எனில் (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_வரி_மொத்தம்);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    இல்லையெனில் எனில் (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_வரி_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != வெற்று;
    எனில் (normal_mode_strict)
    {
	cpt_sources_index = 0;
	எனில் (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    க்கு (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	எனில் ((ctrl_x_mode_normal() || ctrl_x_mode_வரி_or_eval())
					&& (!compl_started || st.found_all))
	{
	    முழு status = process_next_cpt_மதிப்பு(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    எனில் (status == INS_COMPL_CPT_END)
		நிறுத்து;
	    எனில் (status == INS_COMPL_CPT_CONT)
	    {
		எனில் (may_advance_cpt_idx)
		{
		    எனில் (!advance_cpt_sources_index_safe())
			நிறுத்து;
		    compl_source_start_timer(cpt_sources_index);
		}
		தொடர்;
	    }
	}

	எனில் (normal_mode_strict && type == CTRL_X_FUNCTION
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
	எனில் (compl_pattern.string == வெற்று)
	    நிறுத்து;

	எனில் (may_advance_cpt_idx)
	{
	    எனில் (!advance_cpt_sources_index_safe())
		நிறுத்து;
	    compl_source_start_timer(cpt_sources_index);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	எனில் ((ctrl_x_mode_not_default() && !ctrl_x_mode_வரி_or_eval())
						|| found_new_match != FAIL)
	{
	    எனில் (got_int)
		நிறுத்து;
	    // Fill the popup menu as soon as possible.
	    எனில் (type != -1)
		ins_compl_check_சாவிகள்(0, FALSE);

	    எனில் ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_வரி_or_eval()) || compl_interrupted)
		நிறுத்து;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	இல்லையெனில்
	{
	    // Mark a buffer scanned when it has been scanned completely
	    எனில் (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	எனில் (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	எனில் (!compl_dir_forward())
	    போது (compl_curr_match->cp_prev
		    && !match_at_original_text(compl_curr_match->cp_prev))
		compl_curr_match = compl_curr_match->cp_prev;
    }
    cpt_sources_index = -1;
    compl_started = TRUE;

    எனில் ((ctrl_x_mode_normal() || ctrl_x_mode_வரி_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_மொத்தம் = -1;		// total of matches, unknown
    எனில் (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_வரி_or_eval()))
	match_மொத்தம் = ins_compl_make_cyclic();

    எனில் (cot_fuzzy() && compl_get_longest && compl_இலக்கம்_bests > 0)
	fuzzy_longest_match();

    எனில் (compl_old_match != வெற்று)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_next
						    : compl_old_match->cp_prev;
	எனில் (compl_curr_match == வெற்று)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    எனில் (match_மொத்தம் > 0 && !ctrl_x_mode_spell())
    {
	எனில் (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	எனில் (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    திரும்பு match_மொத்தம்;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    நிலையான வெறுமை
ins_compl_update_shown_match(வெறுமை)
{
    string_T	*leader;

    (வெறுமை)get_leader_for_startcol(வெற்று, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    போது (!ins_compl_equal(compl_shown_match,
		leader->string, (முழு)leader->length)
	    && compl_shown_match->cp_next != வெற்று
	    && !is_first_match(compl_shown_match->cp_next))
    {
	compl_shown_match = compl_shown_match->cp_next;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    எனில் (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (முழு)leader->length)
	    && (compl_shown_match->cp_next == வெற்று
		|| is_first_match(compl_shown_match->cp_next)))
    {
	போது (!ins_compl_equal(compl_shown_match,
		    leader->string, (முழு)leader->length)
		&& compl_shown_match->cp_prev != வெற்று
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
    வெறுமை
ins_compl_delete(வெறுமை)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    முழு col = compl_col + (compl_status_adding() ? compl_length : 0);
    string_T	remaining = {வெற்று, 0};
    முழு	    orig_col;

    எனில் (ins_compl_preinsert_effect())
    {
	col += (முழு)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    எனில் (curwin->w_cursor.lnum > compl_lnum)
    {
	எனில் (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *வரி = ml_get_cursor();
	    remaining.length = ml_get_cursor_len();
	    remaining.string = vim_strnsave(வரி, remaining.length);
	    எனில் (remaining.string == வெற்று)
		திரும்பு;
	}
	போது (curwin->w_cursor.lnum > compl_lnum)
	{
	    எனில் (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		எனில் (remaining.string)
		    vim_free(remaining.string);
		திரும்பு;
	    }
	    deleted_வரிகள்_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    எனில் ((முழு)curwin->w_cursor.col > col)
    {
	எனில் (stop_arrow() == FAIL)
	{
	    எனில் (remaining.string)
		vim_free(remaining.string);
	    திரும்பு;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    எனில் (remaining.string != வெற்று)
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
    நிலையான வெறுமை
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    முழு		base_indent = get_indent();

    போது (*curr != NUL)
    {
	எனில் (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    எனில் (curr > start)
		ins_char_bytes(start, (முழு)(curr - start));

	    // Handle newline
	    open_வரி(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, வெற்று);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    எனில் (curr > start)
	ins_char_bytes(start, (முழு)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    நிலையான char_u *
find_common_prefix(size_t *prefix_len, முழு curbuf_only)
{
    compl_T     *compl;
    முழு		*match_மொத்தம்;
    char_u	*first = வெற்று;
    முழு		len = -1;
    முழு		is_cpt_completion = (cpt_sources_array != வெற்று);

    எனில் (!is_cpt_completion)
	திரும்பு வெற்று;

    match_மொத்தம் = ALLOC_CLEAR_MULT(முழு, cpt_sources_மொத்தம்);
    எனில் (match_மொத்தம் == வெற்று)
	திரும்பு வெற்று;

    (வெறுமை)get_leader_for_startcol(வெற்று, TRUE); // Clear the cache

    compl = compl_first_match;
    செய்
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	எனில் (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	எனில் (!match_at_original_text(compl)
		&& (leader->string == வெற்று
		    || ins_compl_equal_sc(compl, leader->string,
			(முழு)leader->length)))
	{
	    // Limit number of items from each source if max_items is set.
	    முழு match_limit_exceeded = FALSE;
	    முழு cur_source = compl->cp_cpt_source_idx;

	    எனில் (cur_source != -1)
	    {
		match_மொத்தம்[cur_source]++;
		முழு max_matches = cpt_sources_array[cur_source].cs_max_matches;
		எனில் (max_matches > 0 && match_மொத்தம்[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    எனில் (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		எனில் (first == வெற்று && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (முழு)STRLEN(first);
		}
		இல்லையெனில் எனில் (first != வெற்று)
		{
		    முழு j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    போது (j < len && *s1 != NUL && *s2 != NUL)
		    {
			எனில் (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    நிறுத்து;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    எனில் (len == 0)
			நிறுத்து;
		}
	    }
	}
	compl = compl->cp_next;
    } போது (compl != வெற்று && !is_first_match(compl));

    vim_free(match_மொத்தம்);

    எனில் (len > (முழு)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	எனில் (len == (முழு)STRLEN(first))
	{
	    char_u *வரி = ml_get_curline();
	    char_u *p = வரி + curwin->w_cursor.col;
	    எனில் (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		முழு text_len = end - p;
		எனில் (text_len > 0
			&& text_len < (len - (முழு)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	திரும்பு first;
    }
    திரும்பு வெற்று;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    வெறுமை
ins_compl_செருகு(முழு move_cursor, முழு செருகு_prefix)
{
    முழு		compl_len = get_compl_len();
    முழு		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.length;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    எனில் (செருகு_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	எனில் (cp_str == வெற்று)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    எனில் (cp_str == வெற்று)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.length;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    இல்லையெனில் எனில் (cpt_sources_array != வெற்று)
    {
	முழு	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	எனில் (cpt_idx >= 0 && compl_col >= 0)
	{
	    முழு	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    எனில் (startcol >= 0 && startcol < (முழு)compl_col)
	    {
		முழு skip = (முழு)compl_col - startcol;
		எனில் ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    எனில் (compl_len < (முழு)cp_str_len)
    {
	எனில் (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	இல்லையெனில்
	{
	    ins_compl_செருகு_bytes(cp_str + compl_len,
		    செருகு_prefix ? (முழு)cp_str_len - compl_len : -1);
	    எனில் ((preinsert || செருகு_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    எனில் (match_at_original_text(compl_shown_match)
	    || (preinsert && !செருகு_prefix))
	compl_used_match = FALSE;
    இல்லையெனில்
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = செருகு_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    நிலையான வெறுமை
ins_compl_show_filename(வெறுமை)
{
    எழுத்து	*lead = _("match in file");
    முழு		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    எனில் (space <= 0)
	திரும்பு;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    க்கு (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	போது (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    எனில் (!compl_autocomplete)
    {
	செய்தி_hist_off = TRUE;
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	செய்தி((எழுத்து *)IObuff);
	செய்தி_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    நிலையான compl_T *
find_next_match_in_menu(வெறுமை)
{
    முழு	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    செய்
	match = is_forward ? match->cp_next : match->cp_prev;
    போது (match->cp_next && !match->cp_in_match_array
	    && !match_at_original_text(match));
    திரும்பு match;
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
    நிலையான முழு
find_next_completion_match(
	முழு	allow_get_expansion,
	முழு	todo,		// repeat completion this many times
	முழு	advance,
	முழு	*இலக்கம்_matches)
{
    முழு		found_end = FALSE;
    compl_T	*found_compl = வெற்று;
    unsigned முழு cur_cot_flags = get_cot_flags();
    முழு		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    போது (--todo >= 0)
    {
	எனில் (compl_shows_dir_forward() && compl_shown_match->cp_next != வெற்று)
	{
	    எனில் (compl_match_array != வெற்று)
		compl_shown_match = find_next_match_in_menu();
	    இல்லையெனில்
		compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != வெற்று
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	இல்லையெனில் எனில் (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != வெற்று)
	{
	    found_end = is_first_match(compl_shown_match);
	    எனில் (compl_match_array != வெற்று)
		compl_shown_match = find_next_match_in_menu();
	    இல்லையெனில்
		compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	இல்லையெனில்
	{
	    எனில் (!allow_get_expansion)
	    {
		எனில் (advance)
		{
		    எனில் (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    இல்லையெனில்
			compl_pending += todo + 1;
		}
		திரும்பு -1;
	    }

	    எனில் (!compl_no_select && advance)
	    {
		எனில் (compl_shows_dir_backward())
		    --compl_pending;
		இல்லையெனில்
		    ++compl_pending;
	    }

	    // Find matches.
	    *இலக்கம்_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    போது (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		எனில் (compl_pending > 0 && compl_shown_match->cp_next != வெற்று)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		இல்லையெனில் எனில் (compl_pending < 0 && compl_shown_match->cp_prev != வெற்று)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		இல்லையெனில்
		    நிறுத்து;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	எனில் (!match_at_original_text(compl_shown_match)
		&& leader->string != வெற்று
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (முழு)leader->length)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	இல்லையெனில்
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	எனில் (found_end)
	{
	    எனில் (found_compl != வெற்று)
	    {
		compl_shown_match = found_compl;
		நிறுத்து;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    திரும்பு OK;
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
    நிலையான முழு
ins_compl_next(
    முழு	    allow_get_expansion,
    முழு	    மொத்தம்,		// repeat completion this many times; should
				// be at least 1
    முழு	    செருகு_match)	// Insert the newly selected match
{
    முழு	    இலக்கம்_matches = -1;
    முழு	    todo = மொத்தம்;
    முழு	    advance;
    முழு	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned முழு cur_cot_flags = get_cot_flags();
    முழு	    compl_no_செருகு = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    முழு	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    எனில் (compl_shown_match == வெற்று)
	திரும்பு -1;

    எனில் (compl_leader.string != வெற்று
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    எனில் (allow_get_expansion && செருகு_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = மொத்தம் != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    எனில் (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    எனில் (find_next_completion_match(allow_get_expansion, todo, advance,
							&இலக்கம்_matches) == -1)
	திரும்பு -1;

    எனில் (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	திரும்பு -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    எனில் (!started && ins_compl_preinsert_longest())
	ins_compl_செருகு(TRUE, TRUE);
    இல்லையெனில் எனில் (compl_no_செருகு && !started && !compl_preinsert)
    {
	ins_compl_செருகு_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    இல்லையெனில் எனில் (செருகு_match)
    {
	எனில் (!compl_get_longest || compl_used_match)
	{
	    முழு preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_செருகு(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	இல்லையெனில்
	    ins_compl_செருகு_bytes(compl_leader.string + get_compl_len(), -1);
    }
    இல்லையெனில்
	compl_used_match = FALSE;

    எனில் (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	எனில் (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	இல்லையெனில்
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	எனில் (gui.in_use)
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
    எனில் (compl_no_செருகு && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    இல்லையெனில்
	compl_enter_selects = !செருகு_match && compl_match_array != வெற்று;

    // Show the file name for the match (if any)
    எனில் (compl_shown_match->cp_fname != வெற்று)
	ins_compl_show_filename();

    திரும்பு இலக்கம்_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    நிலையான வெறுமை
check_elapsed_time(வெறுமை)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_index].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    எனில் (elapsed_ms > compl_timeout_ms)
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
    வெறுமை
ins_compl_check_சாவிகள்(முழு frequency, முழு in_compl_func)
{
    நிலையான முழு	மொத்தம் = 0;
    முழு		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    எனில் (!in_compl_func && (using_script() || ex_normal_busy))
	திரும்பு;

    // Only do this at regular intervals
    எனில் (++மொத்தம் < frequency)
	திரும்பு;
    மொத்தம் = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    எனில் (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	எனில் (vim_is_ctrl_x_சாவி(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (வெறுமை)ins_compl_next(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	இல்லையெனில்
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    எனில் (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		எனில் (c != Ctrl_R && (சாவி_Typed
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    இல்லையெனில்
    {
	முழு normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_வரி_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != வெற்று && cpt_sources_index >= 0;
	எனில் (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    எனில் (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	முழு todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(வெறுமை)ins_compl_next(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    நிலையான முழு
ins_compl_key2dir(முழு c)
{
    எனில் (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	திரும்பு BACKWARD;
    திரும்பு FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    நிலையான முழு
ins_compl_pum_சாவி(முழு c)
{
    திரும்பு pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    நிலையான முழு
ins_compl_key2count(முழு c)
{
    எனில் (ins_compl_pum_சாவி(c) && c != K_UP && c != K_DOWN)
    {
	முழு h = pum_get_height();
	எனில் (h > 3)
	    h -= 2; // keep some context
	திரும்பு h;
    }
    திரும்பு 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    நிலையான முழு
ins_compl_use_match(முழு c)
{
    தேர்வு (c)
    {
	நிலை K_UP:
	நிலை K_DOWN:
	நிலை K_PAGEDOWN:
	நிலை K_KPAGEDOWN:
	நிலை K_S_DOWN:
	நிலை K_PAGEUP:
	நிலை K_KPAGEUP:
	நிலை K_S_UP:
	    திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    நிலையான முழு
get_normal_compl_info(char_u *வரி, முழு startcol, colnr_T curs_col)
{
    எனில் ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	எனில் (!compl_status_adding())
	{
	    போது (--startcol >= 0 && vim_isIDc(வரி[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	எனில் (p_ic)
	{
	    compl_pattern.string = str_foldcase(வரி + compl_col,
		    compl_length, வெற்று, 0);
	    எனில் (compl_pattern.string == வெற்று)
	    {
		compl_pattern.length = 0;
		திரும்பு FAIL;
	    }
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	இல்லையெனில்
	{
	    compl_pattern.string = vim_strnsave(வரி + compl_col, (size_t)compl_length);
	    எனில் (compl_pattern.string == வெற்று)
	    {
		compl_pattern.length = 0;
		திரும்பு FAIL;
	    }
	    compl_pattern.length = (size_t)compl_length;
	}
    }
    இல்லையெனில் எனில் (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	எனில் (!vim_iswordp(வரி + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(வரி, வரி + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(வெற்று, வரி + compl_col, compl_length) + prefixlen;
	compl_pattern.string = alloc(n);
	எனில் (compl_pattern.string == வெற்று)
	{
	    compl_pattern.length = 0;
	    திரும்பு FAIL;
	}
	STRCPY((எழுத்து *)compl_pattern.string, prefix);
	(வெறுமை)quote_meta(compl_pattern.string + prefixlen,
		வரி + compl_col, compl_length);
	compl_pattern.length = n - 1;
    }
    இல்லையெனில் எனில் (--startcol < 0
	    || !vim_iswordp(mb_prevptr(வரி, வரி + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	எனில் (compl_pattern.string == வெற்று)
	{
	    compl_pattern.length = 0;
	    திரும்பு FAIL;
	}
	compl_pattern.length = len;
	compl_col += curs_col;
	compl_length = 0;
	compl_from_nonkeyword = TRUE;
    }
    இல்லையெனில்
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	எனில் (has_mbyte)
	{
	    முழு base_class;
	    முழு head_off;

	    startcol -= (*mb_head_off)(வரி, வரி + startcol);
	    base_class = mb_get_class(வரி + startcol);
	    போது (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(வரி, வரி + startcol);
		எனில் (base_class != mb_get_class(வரி + startcol
			    - head_off))
		    நிறுத்து;
		startcol -= head_off;
	    }
	}
	இல்லையெனில்
	{
	    போது (--startcol >= 0 && vim_iswordc(வரி[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_length = (முழு)curs_col - startcol;
	எனில் (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    எனில் (compl_pattern.string == வெற்று)
	    {
		compl_pattern.length = 0;
		திரும்பு FAIL;
	    }
	    STRCPY((எழுத்து *)compl_pattern.string, "\\<");
	    (வெறுமை)quote_meta(compl_pattern.string + 2, வரி + compl_col, 1);
	    STRCAT((எழுத்து *)compl_pattern.string, "\\k");
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	இல்லையெனில்
	{
	    size_t  n = quote_meta(வெற்று, வரி + compl_col, compl_length) + 2;

	    compl_pattern.string = alloc(n);
	    எனில் (compl_pattern.string == வெற்று)
	    {
		compl_pattern.length = 0;
		திரும்பு FAIL;
	    }
	    STRCPY((எழுத்து *)compl_pattern.string, "\\<");
	    (வெறுமை)quote_meta(compl_pattern.string + 2, வரி + compl_col,
		    compl_length);
	    compl_pattern.length = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    எனில் (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	எனில் (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    நிலையான முழு
get_wholeline_compl_info(char_u *வரி, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(வரி);
    compl_length = (முழு)curs_col - (முழு)compl_col;
    எனில் (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    எனில் (p_ic)
    {
	compl_pattern.string = str_foldcase(வரி + compl_col, compl_length,
		வெற்று, 0);
	எனில் (compl_pattern.string == வெற்று)
	{
	    compl_pattern.length = 0;
	    திரும்பு FAIL;
	}
	compl_pattern.length = STRLEN(compl_pattern.string);
    }
    இல்லையெனில்
    {
	compl_pattern.string = vim_strnsave(வரி + compl_col, (size_t)compl_length);
	எனில் (compl_pattern.string == வெற்று)
	{
	    compl_pattern.length = 0;
	    திரும்பு FAIL;
	}
	compl_pattern.length = (size_t)compl_length;
    }

    திரும்பு OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    நிலையான முழு
get_filename_compl_info(char_u *வரி, முழு startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    எனில் (startcol > 0)
    {
	char_u	*p = வரி + startcol;

	MB_PTR_BACK(வரி, p);
	போது (p > வரி && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(வரி, p);
	எனில் (p == வரி && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	இல்லையெனில்
	    startcol = (முழு)(p - வரி) + 1;
    }

    compl_col += startcol;
    compl_length = (முழு)curs_col - startcol;
    compl_pattern.string = addstar(வரி + compl_col, compl_length, EXPAND_FILES);
    எனில் (compl_pattern.string == வெற்று)
    {
	compl_pattern.length = 0;
	திரும்பு FAIL;
    }

    compl_pattern.length = STRLEN(compl_pattern.string);

    திரும்பு OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    நிலையான முழு
get_cmdline_compl_info(char_u *வரி, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(வரி, curs_col);
    எனில் (compl_pattern.string == வெற்று)
    {
	compl_pattern.length = 0;
	திரும்பு FAIL;
    }
    compl_pattern.length = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (முழு)compl_pattern.length, curs_col, FALSE);
    எனில் (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    இல்லையெனில்
	compl_col = (முழு)(compl_xp.xp_pattern - compl_pattern.string);
    compl_length = curs_col - compl_col;

    திரும்பு OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    நிலையான முழு
set_compl_globals(
    முழு		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    முழு		is_cpt_compl UNUSED)
{
    எனில் (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	எனில் (startcol < compl_col)
	    திரும்பு prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	இல்லையெனில்
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.length);
	    cpt_compl_pattern.length = compl_orig_text.length;
	}
    }
    இல்லையெனில்
    {
	எனில் (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*வரி = ml_get(curwin->w_cursor.lnum);
	முழு	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(வரி + startcol, (size_t)len);
	எனில் (compl_pattern.string == வெற்று)
	{
	    compl_pattern.length = 0;
	    திரும்பு FAIL;
	}
	compl_pattern.length = (size_t)len;
	compl_col = startcol;
	compl_length = len;
    }

    திரும்பு OK;
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
    நிலையான முழு
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    முழு		*startcol UNUSED)
{
    முழு		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    முழு		col;
    char_u	*funcname = வெற்று;
    pos_T	pos;
    முழு		save_State = State;
    முழு		is_cpt_function = (cb != வெற்று);
    முழு		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    எனில் (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	எனில் (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    திரும்பு FAIL;
	}
	cb = get_செருகு_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_எண்;
    args[0].vval.v_எண் = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    எனில் (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    எனில் (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    எனில் (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	திரும்பு FAIL;
    }

    எனில் (startcol != வெற்று)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    எனில் (col == -2 || aborting())
	திரும்பு FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    எனில் (col == -3)
    {
	எனில் (is_cpt_function)
	    திரும்பு FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = வெற்று;
	எனில் (!shortmess(SHM_COMPLETIONMENU))
	    செய்தி_clr_cmdline();
	திரும்பு FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    திரும்பு ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    நிலையான முழு
get_spell_compl_info(முழு startcol UNUSED, colnr_T curs_col UNUSED)
{
    முழு		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*வரி = வெற்று;

    எனில் (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    இல்லையெனில்
	compl_col = spell_word_start(startcol);

    எனில் (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    இல்லையெனில்
    {
	spell_expand_check_cap(compl_col);
	compl_length = (முழு)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    வரி = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(வரி + compl_col, (size_t)compl_length);
    எனில் (compl_pattern.string == வெற்று)
    {
	compl_pattern.length = 0;
	திரும்பு FAIL;
    }

    compl_pattern.length = (size_t)compl_length;
    ret = OK;
#endif

    திரும்பு ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    நிலையான முழு
compl_get_info(char_u *வரி, முழு startcol, colnr_T curs_col, முழு *வரி_invalid)
{
    எனில் (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	எனில் (get_normal_compl_info(வரி, startcol, curs_col) != OK)
	    திரும்பு FAIL;
	*வரி_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    இல்லையெனில் எனில் (ctrl_x_mode_வரி_or_eval())
    {
	திரும்பு get_wholeline_compl_info(வரி, curs_col);
    }
    இல்லையெனில் எனில் (ctrl_x_mode_files())
    {
	திரும்பு get_filename_compl_info(வரி, startcol, curs_col);
    }
    இல்லையெனில் எனில் (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	திரும்பு get_cmdline_compl_info(வரி, curs_col);
    }
    இல்லையெனில் எனில் (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	எனில் (get_userdefined_compl_info(curs_col, வெற்று, வெற்று) != OK)
	    திரும்பு FAIL;
	*வரி_invalid = TRUE;	// "line" may have become invalid
    }
    இல்லையெனில் எனில் (ctrl_x_mode_spell())
    {
	எனில் (get_spell_compl_info(startcol, curs_col) == FAIL)
	    திரும்பு FAIL;
	*வரி_invalid = TRUE;	// "line" may have become invalid
    }
    இல்லையெனில்
    {
	internal_error("ins_complete()");
	திரும்பு FAIL;
    }

    திரும்பு OK;
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
    நிலையான வெறுமை
ins_compl_continue_தேடல்(char_u *வரி)
{
    // it is a continued search
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    எனில் (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	எனில் (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(வரி);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	இல்லையெனில்
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    எனில் (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    வரி + compl_length
			    + compl_startpos.col) - வரி);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_length = curwin->w_cursor.col - (முழு)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	எனில் (compl_length > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_length = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_length;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	எனில் (compl_length < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    இல்லையெனில் எனில் (ctrl_x_mode_வரி_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    இல்லையெனில்
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    நிலையான முழு
ins_compl_start(வெறுமை)
{
    char_u	*வரி = வெற்று;
    முழு		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    முழு		வரி_invalid = FALSE;
    முழு		save_did_ai = did_ai;
    முழு		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    எனில் (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	திரும்பு FAIL;
    }

    வரி = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    எனில் ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_தேடல்(வரி);
    இல்லையெனில்
	compl_cont_status &= CONT_LOCAL;

    எனில் (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	எனில் (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (முழு)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    எனில் (compl_get_info(வரி, startcol, curs_col, &வரி_invalid) == FAIL)
    {
	எனில் (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	திரும்பு FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    எனில் (வரி_invalid)
	வரி = ml_get(curwin->w_cursor.lnum);

    எனில் (compl_status_adding())
    {
	எனில் (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	எனில் (ctrl_x_mode_வரி_or_eval())
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
    இல்லையெனில்
    {
	edit_submode_pre = வெற்று;
	compl_startpos.col = compl_col;
    }

    எனில் (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	எனில் (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_செய்தி]);
	இல்லையெனில்
	    edit_submode = (char_u *)_(CTRL_X_செய்தி(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(வெற்று);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.length = (size_t)compl_length;
    compl_orig_text.string = vim_strnsave(வரி + compl_col, (size_t)compl_length);
    எனில் (p_ic)
	flags |= CP_ICASE;
    எனில் (compl_orig_text.string == வெற்று
	    || ins_compl_add(compl_orig_text.string,
		(முழு)compl_orig_text.length,
		வெற்று, வெற்று, வெற்று, 0, flags, FALSE, வெற்று, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	திரும்பு FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    எனில் (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_மொத்தம்;
	showmode();
	edit_submode_extra = வெற்று;
	out_flush();
    }

    did_ai = save_did_ai;
    திரும்பு OK;
}

/*
 * display the completion status message
 */
    நிலையான வெறுமை
ins_compl_show_statusmsg(வெறுமை)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    எனில் (is_first_match(compl_first_match->cp_next))
    {
	edit_submode_extra = compl_status_adding() && compl_length > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    எனில் (edit_submode_extra == வெற்று)
    {
	எனில் (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	இல்லையெனில் எனில் (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_மொத்தம்;
	}
	இல்லையெனில் எனில் (compl_curr_match->cp_next == compl_curr_match->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_மொத்தம்;
	    compl_curr_match->cp_எண் = 1;
	}
	இல்லையெனில்
	{
#எனில் defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    எனில் (compl_curr_match->cp_எண் == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    எனில் (compl_curr_match->cp_எண் != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		நிலையான char_u match_ref[81];

		எனில் (compl_matches > 0)
		    vim_snprintf((எழுத்து *)match_ref, அளவு(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_எண், compl_matches);
		இல்லையெனில்
		    vim_snprintf((எழுத்து *)match_ref, அளவு(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_எண்);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		எனில் (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    எனில் (!compl_opt_suppress_empty)
    {
	showmode();
	எனில் (!shortmess(SHM_COMPLETIONMENU))
	{
	    எனில் (edit_submode_extra != வெற்று)
	    {
		எனில் (!p_smd)
		{
		    செய்தி_hist_off = TRUE;
		    செய்தி_attr((எழுத்து *)edit_submode_extra,
			    edit_submode_highl < HLF_மொத்தம்
			    ? HL_ATTR(edit_submode_highl) : 0);
		    செய்தி_hist_off = FALSE;
		}
	    }
	    இல்லையெனில்
		செய்தி_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    முழு
ins_complete(முழு c, முழு enable_pum)
{
    முழு		n;
    முழு		save_w_wrow;
    முழு		save_w_leftcol;
    முழு		செருகு_match;
    முழு		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    செருகு_match = ins_compl_use_match(c);

    எனில் (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	எனில் (ins_compl_start() == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (செருகு_match && stop_arrow() == FAIL)
	திரும்பு FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_buffer;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_இலக்கம்_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), செருகு_match);

    // Reset autocompletion timer expiry flag
    எனில் (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    எனில் (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    எனில் (got_int && !global_busy)
    {
	(வெறுமை)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_next);
    எனில் (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	எனில் (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    எனில் (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    இல்லையெனில்
	compl_cont_status &= ~CONT_S_IPOS;

    எனில் (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    எனில் (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    திரும்பு OK;
}

/*
 * Enable autocompletion
 */
    வெறுமை
ins_compl_enable_autocomplete(வெறுமை)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
}

/*
 * Disable autocompletion
 */
    வெறுமை
ins_compl_disable_autocomplete(வெறுமை)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    வெறுமை
ins_compl_arm_autostart(வெறுமை)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    வெறுமை
ins_compl_disarm_autostart(வெறுமை)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(வெறுமை)
{
#ifdef ELAPSED_FUNC
    எனில் (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	திரும்பு true;
    }
#endif
    திரும்பு false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    வெறுமை
ins_compl_clear_autocomplete_delay(வெறுமை)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(வெறுமை)
{
    திரும்பு compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(வெறுமை)
{
#ifdef ELAPSED_FUNC
    திரும்பு ELAPSED_FUNC(compl_autocomplete_start_tv);
#இல்லையெனில்
    திரும்பு 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    நிலையான வெறுமை
show_pum(முழு prev_w_wrow, முழு prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    எனில் (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    நிலையான unsigned
quote_meta(char_u *dest, char_u *src, முழு len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    க்கு ( ; --len >= 0; src++)
    {
	தேர்வு (*src)
	{
	    நிலை '.':
	    நிலை '*':
	    நிலை '[':
		எனில் (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    நிறுத்து;
		// FALLTHROUGH
	    நிலை '~':
		எனில் (!magic_isset())	// quote these only if magic is set
		    நிறுத்து;
		// FALLTHROUGH
	    நிலை '\\':
		எனில் (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    நிறுத்து;
		// FALLTHROUGH
	    நிலை '^':		// currently it's not needed.
	    நிலை '$':
		m++;
		எனில் (dest != வெற்று)
		    *dest++ = '\\';
		நிறுத்து;
	}
	எனில் (dest != வெற்று)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	எனில் (has_mbyte)
	{
	    முழு mb_len = (*mb_ptr2len)(src) - 1;
	    எனில் (mb_len > 0 && len >= mb_len)
		க்கு (முழு i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    எனில் (dest != வெற்று)
			*dest++ = *src;
		}
	}
    }
    எனில் (dest != வெற்று)
	*dest = NUL;

    திரும்பு m;
}

#எனில் defined(EXITFREE)
    வெறுமை
free_insexpand_stuff(வெறுமை)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_மொத்தம்);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    நிலையான வெறுமை
spell_back_to_badword(வெறுமை)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, வெற்று);
    எனில் (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    நிலையான வெறுமை
cpt_sources_clear(வெறுமை)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_index = -1;
    cpt_sources_மொத்தம் = 0;
}

/*
 * Setup completion sources.
 */
    நிலையான முழு
setup_cpt_sources(வெறுமை)
{
    char_u  buf[LSIZE];
    முழு	    slen;
    முழு	    idx = 0;
    முழு	    மொத்தம்;
    char_u  *p;

    cpt_sources_clear();

    மொத்தம் = get_cpt_sources_மொத்தம்(curbuf->b_p_cpt);
    எனில் (மொத்தம் == 0)
	திரும்பு OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, மொத்தம்);
    எனில் (cpt_sources_array == வெற்று)
	திரும்பு FAIL;

    க்கு (p = curbuf->b_p_cpt; *p;)
    {
	போது (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	எனில் (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    எனில் (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		எனில் (caret != வெற்று)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((எழுத்து *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_மொத்தம் = மொத்தம்;
    திரும்பு OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    நிலையான முழு
is_cpt_func_refresh_always(வெறுமை)
{
#ifdef FEAT_COMPL_FUNC
    க்கு (முழு i = 0; i < cpt_sources_மொத்தம்; i++)
	எனில் (cpt_sources_array[i].cs_refresh_always)
	    திரும்பு TRUE;
#endif
    திரும்பு FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    நிலையான வெறுமை
ins_compl_make_linear(வெறுமை)
{
    compl_T *m;

    எனில் (compl_first_match == வெற்று || compl_first_match->cp_prev == வெற்று)
	திரும்பு;
    m = compl_first_match->cp_prev;
    m->cp_next = வெற்று;
    compl_first_match->cp_prev = வெற்று;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    நிலையான வெறுமை
remove_old_matches(வெறுமை)
{
    compl_T *current;
    முழு	    shown_match_removed = FALSE;
    முழு	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    எனில் (cpt_sources_index < 0)
	திரும்பு;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    க்கு (current = compl_first_match; current != வெற்று; )
    {
	எனில் (current->cp_cpt_source_idx == cpt_sources_index)
	{
	    compl_T *to_delete = current;

	    எனில் (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_next;

	    எனில் (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_next;
		compl_first_match->cp_prev = வெற்று;
	    }
	    இல்லையெனில் எனில் (to_delete->cp_next == வெற்று) // node to remove is at tail
		to_delete->cp_prev->cp_next = வெற்று;
	    இல்லையெனில் // node is in the moddle
	    {
		to_delete->cp_prev->cp_next = to_delete->cp_next;
		to_delete->cp_next->cp_prev = to_delete->cp_prev;
	    }
	    ins_compl_item_free(to_delete);
	}
	இல்லையெனில்
	    current = current->cp_next;
    }

    // Re-assign compl_shown_match if necessary
    எனில் (shown_match_removed)
    {
	எனில் (forward)
	    compl_shown_match = compl_first_match;
	இல்லையெனில்
	{  // Last node will have the prefix that is being completed
	    க்கு (current = compl_first_match; current->cp_next != வெற்று;
		    current = current->cp_next)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    க்கு (current = compl_first_match; current != வெற்று; )
    {
	எனில் ((forward ? current->cp_cpt_source_idx < cpt_sources_index
		    : current->cp_cpt_source_idx > cpt_sources_index))
	{
	    compl_curr_match = forward ? current : current->cp_next;
	    current = current->cp_next;
	}
	இல்லையெனில்
	    நிறுத்து;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    நிலையான வெறுமை
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_index];
    முழு		    startcol = cpt_src->cs_startcol;

    எனில் (startcol == -2 || startcol == -3)
	திரும்பு;

    எனில் (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	எனில் (!cpt_src->cs_refresh_always)
	    ins_compl_செருகு_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	எனில் (!cpt_src->cs_refresh_always)
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
    நிலையான வெறுமை
cpt_compl_refresh(வெறுமை)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = வெற்று;
    முழு		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_index = 0;
    க்கு (p = cpt; *p;)
    {
	போது (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	எனில் (*p == NUL)
	    நிறுத்து;

	எனில் (cpt_sources_array[cpt_sources_index].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_index);
	    எனில் (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		எனில் (ret == FAIL)
		{
		    எனில் (startcol == -3)
			cpt_sources_array[cpt_sources_index].cs_refresh_always
			    = FALSE;
		    இல்லையெனில்
			startcol = -2;
		}
		இல்லையெனில் எனில் (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_index].cs_startcol = startcol;
		எனில் (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_index);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(வெறுமை)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	எனில் (may_advance_cpt_index(p))
	    (வெறுமை)advance_cpt_sources_index_safe();
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
    வெறுமை
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    எனில் (ins_compl_preinsert_effect())
	rettv->vval.v_எண் = 1;
}
