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

#అన్తర్భావ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#నిర్వచించు CTRL_X_WANT_IDENT	0x100

#నిర్వచించు CTRL_X_సాధారణ		0  // CTRL-N CTRL-P completion, default
#నిర్వచించు CTRL_X_NOT_DEFINED_YET	1
#నిర్వచించు CTRL_X_SCROLL		2
#నిర్వచించు CTRL_X_సంపూర్ణ_పంక్తి	3
#నిర్వచించు CTRL_X_FILES		4
#నిర్వచించు CTRL_X_ట్యాగులు		(5 + CTRL_X_WANT_IDENT)
#నిర్వచించు CTRL_X_మార్గం_PATTERNS	(6 + CTRL_X_WANT_IDENT)
#నిర్వచించు CTRL_X_మార్గం_నిర్వచిస్తుంది	(7 + CTRL_X_WANT_IDENT)
#నిర్వచించు CTRL_X_ముగిసిన		8
#నిర్వచించు CTRL_X_DICTIONARY	(9 + CTRL_X_WANT_IDENT)
#నిర్వచించు CTRL_X_THESAURUS	(10 + CTRL_X_WANT_IDENT)
#నిర్వచించు CTRL_X_CMDLINE		11
#నిర్వచించు CTRL_X_FUNCTION	12
#నిర్వచించు CTRL_X_OMNI		13
#నిర్వచించు CTRL_X_SPELL		14
#నిర్వచించు CTRL_X_LOCAL_సందేశం	15	// only used in "ctrl_x_msgs"
#నిర్వచించు CTRL_X_EVAL		16	// for builtin function complete()
#నిర్వచించు CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#నిర్వచించు CTRL_X_రిజిస్టరు	18	// complete words from registers

#నిర్వచించు CTRL_X_సందేశం(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
స్థిర అక్షరం *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ఖాళీ, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ఖాళీ, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ఖాళీ,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#ఒకవేళ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
స్థిర string_T ctrl_x_బహుళకం_పేర్లు[] = {
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
    {ఖాళీ, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#అయితే_ముగింపు

/*
 * Structure used to store one match for insert completion.
 */
రకనిర్వచనం నిర్మాణం compl_S compl_T;
నిర్మాణం compl_S
{
    compl_T	*cp_తదుపరి;
    compl_T	*cp_prev;
    compl_T	*cp_సరిపోలిక_తదుపరి;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_పాఠ్యం[CPT_లెక్క]);	// text for the menu
#నిర్వచితమైతే FEAT_EVAL
    typval_T	cp_user_దత్తాంశం;
#అయితే_ముగింపు
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    పూర్ణాంకం		cp_flags;		// CP_ values
    పూర్ణాంకం		cp_సంఖ్య;		// sequence number
    పూర్ణాంకం		cp_క్రమం;		// list position when first scored
    పూర్ణాంకం		cp_score;		// fuzzy match score or proximity score
    పూర్ణాంకం		cp_లోపల_సరిపోలిక_array;	// collected by compl_match_array
    పూర్ణాంకం		cp_user_abbr_hl_id;	// highlight group ID for abbr
    పూర్ణాంకం		cp_user_రకం_hl_id;	// highlight group ID for kind
    పూర్ణాంకం		cp_cpt_ఆధారం_idx;	// index of this match's source in 'cpt' option
};

// values for cp_flags
#నిర్వచించు CP_ORIGINAL_పాఠ్యం   1	// the original text when the expansion begun
#నిర్వచించు CP_FREE_FNAME	    2	// cp_fname is allocated
#నిర్వచించు CP_CONT_S_IPOS	    4	// use CONT_S_IPOS for compl_cont_status
#నిర్వచించు CP_EQUAL	    8	// ins_compl_equal() always returns TRUE
#నిర్వచించు CP_ICASE	    16	// ins_compl_equal() ignores case
#నిర్వచించు CP_FAST	    32	// use fast_breakcheck instead of ui_breakcheck

/*
 * All the current matches are stored in a list.
 * "compl_first_match" points to the start of the list.
 * "compl_curr_match" points to the currently selected entry.
 * "compl_shown_match" is different from compl_curr_match during
 * ins_compl_get_exp(), when new matches are added to the list.
 * "compl_old_match" points to previous "compl_curr_match".
 */
స్థిర compl_T    *compl_మొదటి_సరిపోలిక = ఖాళీ;
స్థిర compl_T    *compl_curr_సరిపోలిక = ఖాళీ;
స్థిర compl_T    *compl_shown_సరిపోలిక = ఖాళీ;
స్థిర compl_T    *compl_old_సరిపోలిక = ఖాళీ;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    cse_లెక్క;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#నిర్వచించు CSE_OFF		((పూర్ణాంకం)offsetof(complstr_T, cse_str))
#నిర్వచించు HI2CSE(hi)	((complstr_T *)((hi)->hi_కీలకం - CSE_OFF))

స్థిర hashtab_T  compl_అక్షరశ్రేణులు_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    స్థిర శూన్య
compl_అక్షరశ్రేణులు_కూడు(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*నమోదు_2;

    ఒకవేళ (compl_అక్షరశ్రేణులు_ht.ht_array == ఖాళీ)
	hash_init(&compl_అక్షరశ్రేణులు_ht);
    ఒకవేళ (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_అక్షరశ్రేణులు_ht, str, hash);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	నమోదు_2 = alloc(CSE_OFF + len + 1);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    తిరిగి;
	నమోదు_2->cse_లెక్క = 1;
	vim_strncpy(నమోదు_2->cse_str, str, len);
	ఒకవేళ (hash_కూడు_అంశం(&compl_అక్షరశ్రేణులు_ht, hi, నమోదు_2->cse_str, hash)
								      == FAIL)
	    vim_free(నమోదు_2);
    }
    లేకపోతే
	++HI2CSE(hi)->cse_లెక్క;
}

// list used to store the compl_T which have the max score
స్థిర compl_T	  **compl_ఉత్తమ_matches = ఖాళీ;
స్థిర పూర్ణాంకం	  compl_అంకె_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
స్థిర పూర్ణాంకం	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
స్థిర string_T	  compl_leader = {ఖాళీ, 0};

స్థిర పూర్ణాంకం	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
స్థిర పూర్ణాంకం	  compl_వాడిన_సరిపోలిక;

// didn't finish finding completions.
స్థిర పూర్ణాంకం	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
స్థిర పూర్ణాంకం	  compl_interrupted = FALSE;

స్థిర పూర్ణాంకం	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
స్థిర పూర్ణాంకం	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
స్థిర పూర్ణాంకం	  ctrl_x_బహుళకం = CTRL_X_సాధారణ;

స్థిర పూర్ణాంకం	  compl_matches = 0;	    // number of completion matches
స్థిర string_T	  compl_నమూనా = {ఖాళీ, 0};	 // search pattern for matching items
#నిర్వచితమైతే FEAT_COMPL_FUNC
స్థిర string_T	  cpt_compl_నమూనా = {ఖాళీ, 0}; // pattern returned by func in 'cpt'
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	  compl_direction = FORWARD;
స్థిర పూర్ణాంకం	  compl_shows_dir = FORWARD;
స్థిర పూర్ణాంకం	  compl_pending = 0;	    // > 1 for postponed CTRL-N
స్థిర pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
స్థిర పూర్ణాంకం	  compl_నిడివి = 0;
స్థిర linenr_T	  compl_lnum = 0;           // lnum where the completion start
స్థిర colnr_T	  compl_నిలువు = 0;	    // column where the text starts
					    // that is being completed
స్థిర colnr_T	  compl_ins_చివర_నిలువు = 0;
స్థిర colnr_T	  compl_longest_చివర_నిలువు = 0;  // end of 'longest' inserted text
స్థిర string_T	  compl_orig_పాఠ్యం = {ఖాళీ, 0};  // text as it was before
					    // completion started
స్థిర పూర్ణాంకం	  compl_cont_బహుళకం = 0;
స్థిర expand_T	  compl_xp;

స్థిర win_T	  *compl_curr_win = ఖాళీ;  // win where completion is active
స్థిర buf_T	  *compl_curr_buf = ఖాళీ;  // buf where completion is active

#నిర్వచించు COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
స్థిర పూర్ణాంకం	  compl_autocomplete = FALSE;	    // whether autocompletion is active
స్థిర bool	  compl_autostarted = false;	    // Vim started this completion
స్థిర bool	  compl_autostart_pending = false;  // the trigger armed one
స్థిర bool	  compl_autocomplete_pending = false;
#నిర్వచితమైతే ELAPSED_FUNC
స్థిర elapsed_T  compl_autocomplete_ప్రారంభం_tv;	    // when the delay was armed
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
స్థిర పూర్ణాంకం	  compl_సమయం_ముక్క_expired = FALSE; // time budget exceeded for current source
స్థిర పూర్ణాంకం	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
స్థిర పూర్ణాంకం	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#నిర్వచించు COMPL_MIN_TIMEOUT_MS	5
#నిర్వచించు DECAY_COMPL_TIMEOUT() \
    చేయి { \
	ఒకవేళ (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } అయితే (0)

// Timeout values for F{func}, F and o values in 'complete'
#నిర్వచించు COMPL_FUNC_TIMEOUT_MS		300
#నిర్వచించు COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
స్థిర పూర్ణాంకం	  compl_cont_స్థితి = 0;
#నిర్వచించు CONT_ADDING	1	// "normal" or "adding" expansion
#నిర్వచించు CONT_INTRPT	(2 + 4)	// a ^X interrupted the current expansion
				// it's set only iff N_ADDS is set
#నిర్వచించు CONT_N_ADDS	4	// next ^X<> will add-new or expand-current
#నిర్వచించు CONT_S_IPOS	8	// next ^X<> will set initial_pos?
				// if so, word-wise-expansion will set SOL
#నిర్వచించు CONT_SOL	16	// pattern includes start of line, just for
				// word-wise expansion, not set for ^X^L
#నిర్వచించు CONT_LOCAL	32	// for ctrl_x_mode 0, ^X^P/^X^N do a local
				// expansion, (eg use complete=.)

స్థిర పూర్ణాంకం	  compl_opt_refresh_always = FALSE;
స్థిర పూర్ణాంకం	  compl_opt_suppress_ఖాళీలు = FALSE;

స్థిర పూర్ణాంకం	  compl_selected_అంశం = -1;

స్థిర పూర్ణాంకం	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
రకనిర్వచనం నిర్మాణం cpt_ఆధారం_T
{
    పూర్ణాంకం	cs_refresh_always;  // Whether 'refresh:always' is set for func
    పూర్ణాంకం	cs_startcol;	    // Start column returned by func
    పూర్ణాంకం	cs_max_matches;	    // Max items to display from this source
#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T	compl_ప్రారంభం_tv;	    // Timestamp when match collection starts
#అయితే_ముగింపు
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_ఆధారం_T;

స్థిర cpt_ఆధారం_T *cpt_sources_array; // Pointer to the array of completion sources
స్థిర పూర్ణాంకం	    cpt_sources_లెక్క;  // Total number of completion sources specified in the 'cpt' option
స్థిర పూర్ణాంకం	    cpt_sources_సూచీ = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
స్థిర pumitem_T *compl_సరిపోలిక_array = ఖాళీ;
స్థిర పూర్ణాంకం compl_సరిపోలిక_arraysize;

స్థిర పూర్ణాంకం ins_compl_కూడు(char_u *str, పూర్ణాంకం len, char_u *fname, char_u **cptext, typval_T *user_దత్తాంశం, పూర్ణాంకం cdir, పూర్ణాంకం flags, పూర్ణాంకం adup, పూర్ణాంకం *user_hl, పూర్ణాంకం score);
స్థిర శూన్య ins_compl_longest_సరిపోలిక(compl_T *సరిపోలిక);
స్థిర శూన్య ins_compl_del_pum(శూన్య);
స్థిర శూన్య ins_compl_files(పూర్ణాంకం లెక్క, char_u **files, పూర్ణాంకం thesaurus, పూర్ణాంకం flags, regmatch_T *regmatch, char_u *buf, పూర్ణాంకం *dir);
స్థిర శూన్య ins_compl_free(శూన్య);
స్థిర పూర్ణాంకం  ins_compl_need_restart(శూన్య);
స్థిర శూన్య ins_compl_new_leader(శూన్య);
స్థిర పూర్ణాంకం  get_compl_len(శూన్య);
స్థిర శూన్య ins_compl_restart(శూన్య);
స్థిర శూన్య ins_compl_సమితి_original_పాఠ్యం(char_u *str, size_t len);
స్థిర శూన్య ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#ఒకవేళ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
స్థిర శూన్య ins_compl_కూడు_list(list_T *list);
స్థిర శూన్య ins_compl_కూడు_dict(dict_T *dict);
స్థిర పూర్ణాంకం get_userdefined_compl_info(colnr_T curs_నిలువు, callback_T *cb, పూర్ణాంకం *startcol);
స్థిర శూన్య get_cpt_func_completion_matches(callback_T *cb);
స్థిర callback_T *get_callback_if_cpt_func(char_u *p, పూర్ణాంకం idx);
స్థిర శూన్య fill_complete_info_dict(dict_T *di, compl_T *సరిపోలిక, పూర్ణాంకం కూడు_సరిపోలిక);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం అమరిక_cpt_sources(శూన్య);
స్థిర పూర్ణాంకం is_cpt_func_refresh_always(శూన్య);
స్థిర శూన్య cpt_sources_clear(శూన్య);
స్థిర శూన్య cpt_compl_refresh(శూన్య);
స్థిర పూర్ణాంకం  ins_compl_key2dir(పూర్ణాంకం c);
స్థిర పూర్ణాంకం  ins_compl_pum_కీలకం(పూర్ణాంకం c);
స్థిర పూర్ణాంకం  ins_compl_key2count(పూర్ణాంకం c);
స్థిర శూన్య show_pum(పూర్ణాంకం prev_w_wrow, పూర్ణాంకం prev_w_leftcol);
స్థిర గుర్తులేని  quote_meta(char_u *dest, char_u *str, పూర్ణాంకం len);
స్థిర పూర్ణాంకం ins_compl_has_గుణిజం(శూన్య);
స్థిర శూన్య ins_compl_expand_గుణిజం(char_u *str);
స్థిర శూన్య ins_compl_longest_చొప్పించు(char_u *ఉపసర్గ);
స్థిర శూన్య ins_compl_make_రేఖీయ(శూన్య);
స్థిర పూర్ణాంకం ins_compl_make_cyclic(శూన్య);

#నిర్వచితమైతే FEAT_SPELL
స్థిర శూన్య spell_వెనుక_గమ్యం_badword(శూన్య);
స్థిర పూర్ణాంకం  spell_bad_len = 0;	// length of located bad word
#అయితే_ముగింపు

/*
 * CTRL-X pressed in Insert mode.
 */
    శూన్య
ins_ctrl_x(శూన్య)
{
    ఒకవేళ (!ctrl_x_బహుళకం_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	ఒకవేళ (compl_cont_స్థితి & CONT_N_ADDS)
	    compl_cont_స్థితి |= CONT_INTRPT;
	లేకపోతే
	    compl_cont_స్థితి = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_బహుళకం = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_సందేశం(ctrl_x_బహుళకం));
	edit_submode_pre = ఖాళీ;
	showmode();
    }
    లేకపోతే
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_బహుళకం = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
పూర్ణాంకం ctrl_x_బహుళకం_none(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == 0; }
పూర్ణాంకం ctrl_x_బహుళకం_సాధారణ(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_సాధారణ; }
పూర్ణాంకం ctrl_x_బహుళకం_scroll(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_SCROLL; }
పూర్ణాంకం ctrl_x_బహుళకం_సంపూర్ణ_పంక్తి(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_సంపూర్ణ_పంక్తి; }
పూర్ణాంకం ctrl_x_బహుళకం_files(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_FILES; }
పూర్ణాంకం ctrl_x_బహుళకం_ట్యాగులు(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_ట్యాగులు; }
పూర్ణాంకం ctrl_x_బహుళకం_మార్గం_patterns(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_మార్గం_PATTERNS; }
పూర్ణాంకం ctrl_x_బహుళకం_మార్గం_నిర్వచిస్తుంది(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_మార్గం_నిర్వచిస్తుంది; }
పూర్ణాంకం ctrl_x_బహుళకం_dictionary(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_DICTIONARY; }
పూర్ణాంకం ctrl_x_బహుళకం_thesaurus(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_THESAURUS; }
పూర్ణాంకం ctrl_x_బహుళకం_cmdline(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_CMDLINE
		|| ctrl_x_బహుళకం == CTRL_X_CMDLINE_CTRL_X; }
పూర్ణాంకం ctrl_x_బహుళకం_function(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_FUNCTION; }
పూర్ణాంకం ctrl_x_బహుళకం_omni(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_OMNI; }
పూర్ణాంకం ctrl_x_బహుళకం_spell(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_SPELL; }
పూర్ణాంకం ctrl_x_బహుళకం_eval(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_EVAL; }
పూర్ణాంకం ctrl_x_బహుళకం_పంక్తి_or_eval(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_సంపూర్ణ_పంక్తి || ctrl_x_బహుళకం == CTRL_X_EVAL; }
పూర్ణాంకం ctrl_x_బహుళకం_రిజిస్టరు(శూన్య)
    { తిరిగి ctrl_x_బహుళకం == CTRL_X_రిజిస్టరు; }

/*
 * Whether other than default completion has been selected.
 */
    పూర్ణాంకం
ctrl_x_బహుళకం_not_default(శూన్య)
{
    తిరిగి ctrl_x_బహుళకం != CTRL_X_సాధారణ;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    పూర్ణాంకం
ctrl_x_బహుళకం_not_defined_yet(శూన్య)
{
    తిరిగి ctrl_x_బహుళకం == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    పూర్ణాంకం
compl_స్థితి_adding(శూన్య)
{
    తిరిగి compl_cont_స్థితి & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    పూర్ణాంకం
compl_స్థితి_sol(శూన్య)
{
    తిరిగి compl_cont_స్థితి & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    పూర్ణాంకం
compl_స్థితి_local(శూన్య)
{
    తిరిగి compl_cont_స్థితి & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    శూన్య
compl_స్థితి_clear(శూన్య)
{
    compl_cont_స్థితి = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    స్థిర పూర్ణాంకం
compl_dir_forward(శూన్య)
{
    తిరిగి compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    స్థిర పూర్ణాంకం
compl_shows_dir_forward(శూన్య)
{
    తిరిగి compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    స్థిర పూర్ణాంకం
compl_shows_dir_backward(శూన్య)
{
    తిరిగి compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    పూర్ణాంకం
has_compl_option(పూర్ణాంకం dict_opt)
{
    ఒకవేళ (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
#నిర్వచితమైతే FEAT_SPELL
							&& !curwin->w_p_spell
#అయితే_ముగింపు
							)
		 : (*curbuf->b_p_tsr == NUL && *p_tsr == NUL
#నిర్వచితమైతే FEAT_COMPL_FUNC
		     && *curbuf->b_p_tsrfu == NUL && *p_tsrfu == NUL
#అయితే_ముగింపు
		   ))
    {
	ctrl_x_బహుళకం = CTRL_X_సాధారణ;
	edit_submode = ఖాళీ;
	సందేశం_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	ఒకవేళ (emsg_silent == 0 && !లోపల_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    బహిర్గతం_flush();
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (!get_vim_var_nr(VV_TESTING))
#అయితే_ముగింపు
		ui_delay(2004L, FALSE);
	}
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    పూర్ణాంకం
vim_is_ctrl_x_కీలకం(పూర్ణాంకం c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    ఒకవేళ (ins_compl_pum_కీలకం(c))
	తిరిగి TRUE;

    ఎంపిక (ctrl_x_బహుళకం)
    {
	స్థితి 0:		    // Not in any CTRL-X mode
	    తిరిగి (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	స్థితి CTRL_X_NOT_DEFINED_YET:
	స్థితి CTRL_X_CMDLINE_CTRL_X:
	    తిరిగి (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	స్థితి CTRL_X_SCROLL:
	    తిరిగి (c == Ctrl_Y || c == Ctrl_E);
	స్థితి CTRL_X_సంపూర్ణ_పంక్తి:
	    తిరిగి (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_FILES:
	    తిరిగి (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_DICTIONARY:
	    తిరిగి (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_THESAURUS:
	    తిరిగి (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_ట్యాగులు:
	    తిరిగి (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#నిర్వచితమైతే FEAT_కనుగొను_ID
	స్థితి CTRL_X_మార్గం_PATTERNS:
	    తిరిగి (c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_మార్గం_నిర్వచిస్తుంది:
	    తిరిగి (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#అయితే_ముగింపు
	స్థితి CTRL_X_CMDLINE:
	    తిరిగి (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#నిర్వచితమైతే FEAT_COMPL_FUNC
	స్థితి CTRL_X_FUNCTION:
	    తిరిగి (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_OMNI:
	    తిరిగి (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#అయితే_ముగింపు
	స్థితి CTRL_X_SPELL:
	    తిరిగి (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_EVAL:
	    తిరిగి (c == Ctrl_P || c == Ctrl_N);
	స్థితి CTRL_X_రిజిస్టరు:
	    తిరిగి (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    అంతర్గత_error("vim_is_ctrl_x_key()");
    తిరిగి FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_at_original_పాఠ్యం(compl_T *సరిపోలిక)
{
    తిరిగి సరిపోలిక->cp_flags & CP_ORIGINAL_పాఠ్యం;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    స్థిర పూర్ణాంకం
is_మొదటి_సరిపోలిక(compl_T *సరిపోలిక)
{
    తిరిగి సరిపోలిక == compl_మొదటి_సరిపోలిక;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    స్థిర compl_T *
కనుగొను_original_పాఠ్యం_సరిపోలిక(శూన్య)
{
    ఒకవేళ (compl_మొదటి_సరిపోలిక == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (సరిపోలిక_at_original_పాఠ్యం(compl_మొదటి_సరిపోలిక))
	తిరిగి compl_మొదటి_సరిపోలిక;
    ఒకవేళ (compl_మొదటి_సరిపోలిక->cp_prev != ఖాళీ
	    && సరిపోలిక_at_original_పాఠ్యం(compl_మొదటి_సరిపోలిక->cp_prev))
	తిరిగి compl_మొదటి_సరిపోలిక->cp_prev;
    తిరిగి ఖాళీ;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    పూర్ణాంకం
ins_compl_accept_char(పూర్ణాంకం c)
{
    ఒకవేళ (compl_autocomplete && compl_from_nonkeyword)
	తిరిగి FALSE;

    ఒకవేళ (ctrl_x_బహుళకం & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	తిరిగి vim_isIDc(c);

    ఎంపిక (ctrl_x_బహుళకం)
    {
	స్థితి CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    తిరిగి vim_isfilec(c) && !vim_ispathsep(c);

	స్థితి CTRL_X_CMDLINE:
	స్థితి CTRL_X_CMDLINE_CTRL_X:
	స్థితి CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    తిరిగి vim_isprintc(c) && !VIM_ISWHITE(c);

	స్థితి CTRL_X_సంపూర్ణ_పంక్తి:
	    // For while line completion a space can be part of the line.
	    తిరిగి vim_isprintc(c);
    }
    తిరిగి vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    స్థిర char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    పూర్ణాంకం	    char_len,
    పూర్ణాంకం	    compl_char_len,
    పూర్ణాంకం	    min_len,
    char_u  **tofree)
{
    పూర్ణాంకం		*wca;			// Wide character array.
    char_u	*p;
    పూర్ణాంకం		i, c;
    పూర్ణాంకం		has_క్రింది = FALSE;
    పూర్ణాంకం		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(పూర్ణాంకం, char_len);
    ఒకవేళ (wca == ఖాళీ)
	తిరిగి IObuff;

    p = str;
    కొరకు (i = 0; i < char_len; ++i)
    {
	ఒకవేళ (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	లేకపోతే
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_పాఠ్యం.string;
    కొరకు (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ఒకవేళ (MB_ISLOWER(c))
	{
	    has_క్రింది = TRUE;
	    ఒకవేళ (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		కొరకు (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		ఆపు;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    ఒకవేళ (!has_క్రింది)
    {
	p = compl_orig_పాఠ్యం.string;
	కొరకు (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    ఒకవేళ (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		కొరకు (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		ఆపు;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_పాఠ్యం.string;
    కొరకు (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ఒకవేళ (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	లేకపోతే ఒకవేళ (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    అయితే (i < char_len)
    {
	ఒకవేళ (gap.ga_దత్తాంశం != ఖాళీ)
	{
	    ఒకవేళ (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		తిరిగి (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_దత్తాంశం + gap.ga_len;
	    ఒకవేళ (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    లేకపోతే
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	లేకపోతే ఒకవేళ ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    ఒకవేళ (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		తిరిగి (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_దత్తాంశం, IObuff);
	    gap.ga_len = (పూర్ణాంకం)(p - IObuff);
	}
	లేకపోతే ఒకవేళ (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	లేకపోతే
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    ఒకవేళ (gap.ga_దత్తాంశం != ఖాళీ)
    {
	*tofree = gap.ga_దత్తాంశం;
	తిరిగి gap.ga_దత్తాంశం;
    }

    *p = NUL;
    తిరిగి IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    పూర్ణాంకం
ins_compl_కూడు_infercase(
    char_u	*str_arg,
    పూర్ణాంకం		len,
    పూర్ణాంకం		icase,
    char_u	*fname,
    పూర్ణాంకం		dir,
    పూర్ణాంకం		cont_s_ipos,  // next ^X<> will set initial_pos
    పూర్ణాంకం		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    పూర్ణాంకం		char_len;		// count multi-byte characters
    పూర్ణాంకం		compl_char_len;
    పూర్ణాంకం		min_len;
    పూర్ణాంకం		flags = 0;
    పూర్ణాంకం		res;
    char_u	*tofree = ఖాళీ;

    ఒకవేళ (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	ఒకవేళ (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    అయితే (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	లేకపోతే
	{
	    char_len = len;
	}

	// Find actual length of original text.
	ఒకవేళ (has_mbyte)
	{
	    p = compl_orig_పాఠ్యం.string;
	    compl_char_len = 0;
	    అయితే (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	లేకపోతే
	{
	    compl_char_len = compl_నిడివి;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    ఒకవేళ (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    ఒకవేళ (icase)
	flags |= CP_ICASE;

    res = ins_compl_కూడు(str, len, fname, ఖాళీ, ఖాళీ, dir, flags, FALSE, ఖాళీ, score);
    vim_free(tofree);
    తిరిగి res;
}

/*
 * Check if fuzzy matching is enabled
 */
    స్థిర పూర్ణాంకం
cot_fuzzy(శూన్య)
{
    తిరిగి (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_బహుళకం_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    స్థిర పూర్ణాంకం
is_సమీప_చురుకైన(శూన్య)
{
    తిరిగి (compl_autocomplete || (get_cot_flags() & COT_సమీప))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    పూర్ణాంకం
ins_compl_is_సరిపోలిక_selected(శూన్య)
{
    తిరిగి compl_shown_సరిపోలిక != ఖాళీ && !is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    పూర్ణాంకం
ins_compl_preinsert_longest(శూన్య)
{
    తిరిగి compl_autocomplete
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
    స్థిర పూర్ణాంకం
ins_compl_కూడు(
    char_u	*str,
    పూర్ణాంకం		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_దత్తాంశం వాడని,  // "user_data" entry or NULL
    పూర్ణాంకం		cdir,
    పూర్ణాంకం		flags_arg,
    పూర్ణాంకం		adup,		    // accept duplicate match
    పూర్ణాంకం		*user_hl,           // user abbr/kind hlattr
    పూర్ణాంకం		score)
{
    compl_T	*సరిపోలిక, *current, *prev;
    పూర్ణాంకం		dir = (cdir == 0 ? compl_direction : cdir);
    పూర్ణాంకం		flags = flags_arg;
    పూర్ణాంకం		inserted = FALSE;
    char_u	*new_str = ఖాళీ;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    ఒకవేళ (flags & CP_FAST)
	fast_breakcheck();
    లేకపోతే
	ui_breakcheck();
    ఒకవేళ (got_int)
	తిరిగి FAIL;
    ఒకవేళ (len < 0)
	len = (పూర్ణాంకం)STRLEN(str);

    // If the same match is already present, don't add it.
    ఒకవేళ (compl_మొదటి_సరిపోలిక != ఖాళీ && !adup && compl_అక్షరశ్రేణులు_ht.ht_వాడిన > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *కీలకం;
	hashitem_T  *hi;

	ఒకవేళ (len < (పూర్ణాంకం)పరిమాణం(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    కీలకం = keybuf;
	}
	లేకపోతే
	{
	    new_str = vim_strnsave(str, len);
	    ఒకవేళ (new_str == ఖాళీ)
		తిరిగి FAIL;
	    కీలకం = new_str;
	}
	str_hash = hash_hash(కీలకం);
	hi = hash_lookup(&compl_అక్షరశ్రేణులు_ht, కీలకం, str_hash);
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    ఒకవేళ (is_సమీప_చురుకైన() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		సరిపోలిక = compl_మొదటి_సరిపోలిక;
		చేయి
		{
		    ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(సరిపోలిక)
			    && STRNCMP(సరిపోలిక->cp_str.string, str, len) == 0
			    && ((పూర్ణాంకం)సరిపోలిక->cp_str.నిడివి <= len
					  || సరిపోలిక->cp_str.string[len] == NUL))
		    {
			ఒకవేళ (score < సరిపోలిక->cp_score)
			    సరిపోలిక->cp_score = score;
			ఆపు;
		    }
		    సరిపోలిక = సరిపోలిక->cp_తదుపరి;
		} అయితే (సరిపోలిక != ఖాళీ && !is_మొదటి_సరిపోలిక(సరిపోలిక));
	    }
	    vim_free(new_str);
	    తిరిగి NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    సరిపోలిక = ALLOC_CLEAR_ONE(compl_T);
    ఒకవేళ (సరిపోలిక == ఖాళీ)
    {
	vim_free(new_str);
	తిరిగి FAIL;
    }
    సరిపోలిక->cp_సంఖ్య = flags & CP_ORIGINAL_పాఠ్యం ? 0 : -1;
    సరిపోలిక->cp_క్రమం = -1;
    ఒకవేళ (new_str == ఖాళీ && (new_str = vim_strnsave(str, len)) == ఖాళీ)
    {
	vim_free(సరిపోలిక);
	తిరిగి FAIL;
    }
    సరిపోలిక->cp_str.string = new_str;
    సరిపోలిక->cp_str.నిడివి = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    ఒకవేళ (fname != ఖాళీ
	    && compl_curr_సరిపోలిక != ఖాళీ
	    && compl_curr_సరిపోలిక->cp_fname != ఖాళీ
	    && STRCMP(fname, compl_curr_సరిపోలిక->cp_fname) == 0)
	సరిపోలిక->cp_fname = compl_curr_సరిపోలిక->cp_fname;
    లేకపోతే ఒకవేళ (fname != ఖాళీ)
    {
	సరిపోలిక->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    లేకపోతే
	సరిపోలిక->cp_fname = ఖాళీ;
    సరిపోలిక->cp_flags = flags;
    సరిపోలిక->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    సరిపోలిక->cp_user_రకం_hl_id = user_hl ? user_hl[1] : 0;
    సరిపోలిక->cp_score = score;
    సరిపోలిక->cp_cpt_ఆధారం_idx = cpt_sources_సూచీ;

    ఒకవేళ (cptext != ఖాళీ)
    {
	కొరకు (పూర్ణాంకం i = 0; i < CPT_లెక్క; ++i)
	{
	    ఒకవేళ (cptext[i] != ఖాళీ && *cptext[i] != NUL)
		సరిపోలిక->cp_పాఠ్యం[i] = vim_strsave(cptext[i]);
	}
    }
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (user_దత్తాంశం != ఖాళీ)
	సరిపోలిక->cp_user_దత్తాంశం = *user_దత్తాంశం;
#అయితే_ముగింపు

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    ఒకవేళ (compl_మొదటి_సరిపోలిక == ఖాళీ)
	సరిపోలిక->cp_తదుపరి = సరిపోలిక->cp_prev = ఖాళీ;
    లేకపోతే ఒకవేళ (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_మొదటి_సరిపోలిక->cp_తదుపరి;
	prev = compl_మొదటి_సరిపోలిక;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	అయితే (current != ఖాళీ && current != compl_మొదటి_సరిపోలిక)
	{
	    ఒకవేళ (current->cp_score < score)
	    {
		సరిపోలిక->cp_తదుపరి = current;
		సరిపోలిక->cp_prev = current->cp_prev;
		ఒకవేళ (current->cp_prev)
		    current->cp_prev->cp_తదుపరి = సరిపోలిక;
		current->cp_prev = సరిపోలిక;
		inserted = TRUE;
		ఆపు;
	    }
	    prev = current;
	    current = current->cp_తదుపరి;
	}
	ఒకవేళ (!inserted)
	{
	    prev->cp_తదుపరి = సరిపోలిక;
	    సరిపోలిక->cp_prev = prev;
	    సరిపోలిక->cp_తదుపరి = compl_మొదటి_సరిపోలిక;
	    compl_మొదటి_సరిపోలిక->cp_prev = సరిపోలిక;
	}
    }
    లేకపోతే ఒకవేళ (dir == FORWARD)
    {
	సరిపోలిక->cp_తదుపరి = compl_curr_సరిపోలిక->cp_తదుపరి;
	సరిపోలిక->cp_prev = compl_curr_సరిపోలిక;
    }
    లేకపోతే	// BACKWARD
    {
	సరిపోలిక->cp_తదుపరి = compl_curr_సరిపోలిక;
	సరిపోలిక->cp_prev = compl_curr_సరిపోలిక->cp_prev;
    }
    ఒకవేళ (సరిపోలిక->cp_తదుపరి)
	సరిపోలిక->cp_తదుపరి->cp_prev = సరిపోలిక;
    ఒకవేళ (సరిపోలిక->cp_prev)
	సరిపోలిక->cp_prev->cp_తదుపరి = సరిపోలిక;
    లేకపోతే	// if there's nothing before, it is the first match
	compl_మొదటి_సరిపోలిక = సరిపోలిక;
    compl_curr_సరిపోలిక = సరిపోలిక;

    ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(సరిపోలిక))
	compl_అక్షరశ్రేణులు_కూడు(సరిపోలిక->cp_str.string, సరిపోలిక->cp_str.నిడివి,
								    str_hash);

    // Find the longest common string if still doing that.
    ఒకవేళ (compl_get_longest && (flags & CP_ORIGINAL_పాఠ్యం) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_బహుళకం_thesaurus())
	ins_compl_longest_సరిపోలిక(సరిపోలిక);

    తిరిగి OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    స్థిర పూర్ణాంకం
ins_compl_equal(compl_T *సరిపోలిక, char_u *str, పూర్ణాంకం len)
{
    ఒకవేళ (సరిపోలిక->cp_flags & CP_EQUAL)
	తిరిగి TRUE;
    ఒకవేళ (సరిపోలిక->cp_flags & CP_ICASE)
	తిరిగి STRNICMP(సరిపోలిక->cp_str.string, str, (size_t)len) == 0;
    తిరిగి STRNCMP(సరిపోలిక->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    స్థిర పూర్ణాంకం
ins_compl_equal_sc(compl_T *సరిపోలిక, char_u *str, పూర్ణాంకం len)
{
    పూర్ణాంకం	typed = compl_నిడివి;
    పూర్ణాంకం	longest_చివర = (compl_get_longest && compl_longest_చివర_నిలువు > compl_నిలువు)
			    ? (పూర్ణాంకం)(compl_longest_చివర_నిలువు - compl_నిలువు) : typed;

    ఒకవేళ ((సరిపోలిక->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_చివర <= typed)
	తిరిగి ins_compl_equal(సరిపోలిక, str, len);

    ఒకవేళ ((పూర్ణాంకం)సరిపోలిక->cp_str.నిడివి < len)
	తిరిగి FALSE;

    కొరకు (పూర్ణాంకం i = 0; i < len; ++i)
    {
	ఒకవేళ (i >= typed && i < longest_చివర
		? TOLOWER_LOC(సరిపోలిక->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: సరిపోలిక->cp_str.string[i] != str[i])
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    స్థిర శూన్య
ins_compl_చొప్పించు_బైట్లు(char_u *p, పూర్ణాంకం len)
{
    ఒకవేళ (len == -1)
	len = (పూర్ణాంకం)STRLEN(p);
    ins_బైట్లు_len(p, len);
    compl_ins_చివర_నిలువు = curwin->w_cursor.నిలువు;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(శూన్య)
{
    తిరిగి compl_leader.string != ఖాళీ ? compl_leader.string : compl_orig_పాఠ్యం.string;
}

/*
 * Get current completion leader length
 */
    స్థిర size_t
ins_compl_leader_len(శూన్య)
{
    తిరిగి compl_leader.string != ఖాళీ ? compl_leader.నిడివి : compl_orig_పాఠ్యం.నిడివి;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    పూర్ణాంకం
ins_compl_నిలువు_range_attr(linenr_T lnum, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం	    ప్రారంభం_నిలువు;
    పూర్ణాంకం	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    పూర్ణాంకం	    attr;

    ఒకవేళ (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	తిరిగి -1;

    ప్రారంభం_నిలువు = compl_నిలువు + (పూర్ణాంకం)ins_compl_leader_len();
    ఒకవేళ (!ins_compl_has_గుణిజం())
	తిరిగి (నిలువు >= ప్రారంభం_నిలువు && నిలువు < compl_ins_చివర_నిలువు) ? attr : -1;

    // Multiple lines
    ఒకవేళ ((lnum == compl_lnum && నిలువు >= ప్రారంభం_నిలువు && నిలువు < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && నిలువు <= compl_ins_చివర_నిలువు))
	తిరిగి attr;

    తిరిగి -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    స్థిర పూర్ణాంకం
ins_compl_has_గుణిజం(శూన్య)
{
    తిరిగి vim_strchr(compl_shown_సరిపోలిక->cp_str.string, '\n') != ఖాళీ;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    పూర్ణాంకం
ins_compl_lnum_లోపల_range(linenr_T lnum)
{
    ఒకవేళ (!ins_compl_has_గుణిజం())
	తిరిగి FALSE;
    తిరిగి lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    స్థిర శూన్య
ins_compl_longest_సరిపోలిక(compl_T *సరిపోలిక)
{
    char_u	*p, *s;
    పూర్ణాంకం		c1, c2;
    పూర్ణాంకం		had_సరిపోలిక;

    ఒకవేళ (compl_leader.string == ఖాళీ)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(సరిపోలిక->cp_str.string, సరిపోలిక->cp_str.నిడివి);
	ఒకవేళ (compl_leader.string == ఖాళీ)
	    తిరిగి;

	compl_leader.నిడివి = సరిపోలిక->cp_str.నిడివి;
	had_సరిపోలిక = (curwin->w_cursor.నిలువు > compl_నిలువు);
	ins_compl_longest_చొప్పించు(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ఒకవేళ (!had_సరిపోలిక)
	    ins_compl_delete();
	compl_వాడిన_సరిపోలిక = FALSE;

	తిరిగి;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = సరిపోలిక->cp_str.string;
    అయితే (*p != NUL)
    {
	ఒకవేళ (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	లేకపోతే
	{
	    c1 = *p;
	    c2 = *s;
	}
	ఒకవేళ ((సరిపోలిక->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    ఆపు;
	ఒకవేళ (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	లేకపోతే
	{
	    ++p;
	    ++s;
	}
    }

    ఒకవేళ (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.నిడివి = (size_t)(p - compl_leader.string);

	had_సరిపోలిక = (curwin->w_cursor.నిలువు > compl_నిలువు);
	ins_compl_longest_చొప్పించు(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ఒకవేళ (!had_సరిపోలిక)
	    ins_compl_delete();
    }

    compl_వాడిన_సరిపోలిక = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    స్థిర శూన్య
ins_compl_కూడు_matches(
    పూర్ణాంకం		అంకె_matches,
    char_u	**matches,
    పూర్ణాంకం		icase)
{
    పూర్ణాంకం		కూడు_r = OK;
    పూర్ణాంకం		dir = compl_direction;

    కొరకు (పూర్ణాంకం i = 0; i < అంకె_matches && కూడు_r != FAIL; i++)
    {
	కూడు_r = ins_compl_కూడు(matches[i], -1, ఖాళీ, ఖాళీ, ఖాళీ, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ఖాళీ,
		FUZZY_SCORE_NONE);
	ఒకవేళ (కూడు_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(అంకె_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    స్థిర పూర్ణాంకం
ins_compl_make_cyclic(శూన్య)
{
    compl_T *సరిపోలిక;
    పూర్ణాంకం	    లెక్క = 0;

    ఒకవేళ (compl_మొదటి_సరిపోలిక == ఖాళీ)
	తిరిగి 0;

    // Find the end of the list.
    సరిపోలిక = compl_మొదటి_సరిపోలిక;
    // there's always an entry for the compl_orig_text, it doesn't count.
    అయితే (సరిపోలిక->cp_తదుపరి != ఖాళీ && !is_మొదటి_సరిపోలిక(సరిపోలిక->cp_తదుపరి))
    {
	సరిపోలిక = సరిపోలిక->cp_తదుపరి;
	++లెక్క;
    }
    సరిపోలిక->cp_తదుపరి = compl_మొదటి_సరిపోలిక;
    compl_మొదటి_సరిపోలిక->cp_prev = సరిపోలిక;

    తిరిగి లెక్క;
}

/*
 * Return whether there currently is a shown match.
 */
    పూర్ణాంకం
ins_compl_has_shown_సరిపోలిక(శూన్య)
{
    తిరిగి compl_shown_సరిపోలిక == ఖాళీ
	|| compl_shown_సరిపోలిక != compl_shown_సరిపోలిక->cp_తదుపరి;
}

/*
 * Return whether the shown match is long enough.
 */
    పూర్ణాంకం
ins_compl_long_shown_సరిపోలిక(శూన్య)
{
    తిరిగి (పూర్ణాంకం)compl_shown_సరిపోలిక->cp_str.నిడివి
					    > curwin->w_cursor.నిలువు - compl_నిలువు;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    గుర్తులేని పూర్ణాంకం
get_cot_flags(శూన్య)
{
    తిరిగి curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    స్థిర శూన్య
ins_compl_upd_pum(శూన్య)
{
    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం h = curwin->w_cline_ఎత్తు;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    ఒకవేళ (h != curwin->w_cline_ఎత్తు)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    స్థిర శూన్య
ins_compl_del_pum(శూన్య)
{
    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
	తిరిగి;

    pum_undisplay();
    VIM_CLEAR(compl_సరిపోలిక_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    పూర్ణాంకం
pum_wanted(శూన్య)
{
    // 'completeopt' must contain "menu" or "menuone"
    తిరిగి (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    స్థిర పూర్ణాంకం
pum_enough_matches(శూన్య)
{
    compl_T     *compl;
    పూర్ణాంకం		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_మొదటి_సరిపోలిక;
    చేయి
    {
	ఒకవేళ (compl == ఖాళీ || (!సరిపోలిక_at_original_పాఠ్యం(compl) && ++i == 2))
	    ఆపు;
	compl = compl->cp_తదుపరి;
    } అయితే (!is_మొదటి_సరిపోలిక(compl));

    ఒకవేళ ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	తిరిగి (i >= 1);
    తిరిగి (i >= 2);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    స్థిర dict_T *
ins_compl_dict_alloc(compl_T *సరిపోలిక)
{
    dict_T *dict = dict_alloc_తాళం(VAR_FIXED);

    ఒకవేళ (dict == ఖాళీ)
	తిరిగి ఖాళీ;
    fill_complete_info_dict(dict, సరిపోలిక, FALSE);
    తిరిగి dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    స్థిర శూన్య
trigger_complete_మారింది_event(పూర్ణాంకం cur)
{
    dict_T	    *v_event;
    dict_T	    *అంశం;
    స్థిర పూర్ణాంకం	    recursive = FALSE;
    save_v_event_T  save_v_event;

    ఒకవేళ (recursive)
	తిరిగి;

    అంశం = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_సరిపోలిక);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి;
    v_event = get_v_event(&save_v_event);
    dict_కూడు_dict(v_event, "completed_item", అంశం);
    pum_సమితి_event_info(v_event);
    dict_సమితి_వస్తువులు_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ఖాళీ, ఖాళీ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#అయితే_ముగింపు

/*
 * Helper functions for mergesort_list().
 */
    స్థిర శూన్య*
cp_get_తదుపరి(శూన్య *కణుపు)
{
    తిరిగి ((compl_T*)కణుపు)->cp_తదుపరి;
}

    స్థిర శూన్య
cp_సమితి_తదుపరి(శూన్య *కణుపు, శూన్య *తదుపరి)
{
    ((compl_T*)కణుపు)->cp_తదుపరి = (compl_T*)తదుపరి;
}

    స్థిర శూన్య*
cp_get_prev(శూన్య* కణుపు)
{
    తిరిగి ((compl_T*)కణుపు)->cp_prev;
}

    స్థిర శూన్య
cp_సమితి_prev(శూన్య* కణుపు, శూన్య* prev)
{
    ((compl_T*)కణుపు)->cp_prev = (compl_T*)prev;
}

    స్థిర పూర్ణాంకం
cp_compare_fuzzy(స్థిరరాశి శూన్య* a, స్థిరరాశి శూన్య* b)
{
    పూర్ణాంకం score_a = ((compl_T*)a)->cp_score;
    పూర్ణాంకం score_b = ((compl_T*)b)->cp_score;

    ఒకవేళ (score_a != score_b)
	తిరిగి (score_b > score_a) ? 1 : -1;

    తిరిగి ((compl_T *)a)->cp_క్రమం - ((compl_T *)b)->cp_క్రమం;
}

    స్థిర పూర్ణాంకం
cp_compare_సమీప(స్థిరరాశి శూన్య* a, స్థిరరాశి శూన్య* b)
{
    పూర్ణాంకం score_a = ((compl_T*)a)->cp_score;
    పూర్ణాంకం score_b = ((compl_T*)b)->cp_score;
    ఒకవేళ (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	తిరిగి 0;
    తిరిగి (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
prepend_startcol_పాఠ్యం(string_T *dest, string_T *src, పూర్ణాంకం startcol)
{
    పూర్ణాంకం prepend_len = compl_నిలువు - startcol;
    పూర్ణాంకం new_నిడివి = prepend_len + (పూర్ణాంకం)src->నిడివి;

    dest->నిడివి = (size_t)new_నిడివి;
    dest->string = alloc(new_నిడివి + 1);  // +1 for NUL
    ఒకవేళ (dest->string == ఖాళీ)
    {
	dest->నిడివి = 0;
	తిరిగి FAIL;
    }

    char_u	*పంక్తి = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, పంక్తి + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->నిడివి);
    dest->string[new_నిడివి] = NUL;
    తిరిగి OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    స్థిర string_T *
get_leader_for_startcol(compl_T *సరిపోలిక, పూర్ణాంకం cached)
{
    స్థిర string_T adjusted_leader = {ఖాళీ, 0};

    ఒకవేళ (సరిపోలిక == ఖాళీ)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (cpt_sources_array == ఖాళీ)
	వెళ్ళు theend;

    పూర్ణాంకం	cpt_idx = సరిపోలిక->cp_cpt_ఆధారం_idx;
    ఒకవేళ (cpt_idx < 0)
	వెళ్ళు theend;
    పూర్ణాంకం	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    ఒకవేళ (compl_leader.string == ఖాళీ)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	ఒకవేళ (startcol < 0 || startcol >= compl_నిలువు)
	    తిరిగి &compl_orig_పాఠ్యం;
	తిరిగి &compl_leader;  // pass through (startcol < compl_col)
    }

    ఒకవేళ (compl_నిలువు <= 0)
	వెళ్ళు theend;

    ఒకవేళ (startcol >= 0 && startcol < compl_నిలువు)
    {
	పూర్ణాంకం prepend_len = compl_నిలువు - startcol;
	పూర్ణాంకం new_నిడివి = prepend_len + (పూర్ణాంకం)compl_leader.నిడివి;
	ఒకవేళ (cached && (size_t)new_నిడివి == adjusted_leader.నిడివి
		&& adjusted_leader.string != ఖాళీ)
	    తిరిగి &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	ఒకవేళ (prepend_startcol_పాఠ్యం(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    వెళ్ళు theend;

	తిరిగి &adjusted_leader;
    }
theend:
    తిరిగి &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    స్థిర శూన్య
సమితి_fuzzy_score(శూన్య)
{
    compl_T *compl;
    char_u  *నమూనా;
    పూర్ణాంకం	    use_leader;

    ఒకవేళ (compl_మొదటి_సరిపోలిక == ఖాళీ)
	తిరిగి;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ఖాళీ && compl_leader.నిడివి > 0);
    ఒకవేళ (!use_leader)
    {
	ఒకవేళ (compl_orig_పాఠ్యం.string == ఖాళీ || compl_orig_పాఠ్యం.నిడివి == 0)
	    తిరిగి;
	నమూనా = compl_orig_పాఠ్యం.string;
    }
    లేకపోతే
    {
	/* Clear the leader cache once before the loop */
	(శూన్య)get_leader_for_startcol(ఖాళీ, TRUE);
	నమూనా = ఖాళీ;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    పూర్ణాంకం తదుపరి = 0;
    compl = compl_మొదటి_సరిపోలిక;
    చేయి
    {
	ఒకవేళ (compl->cp_క్రమం < 0)
	    compl->cp_క్రమం = తదుపరి;
	తదుపరి = MAX(తదుపరి, compl->cp_క్రమం + 1);

	ఒకవేళ (use_leader)
	    నమూనా = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_సరిపోలిక_str(compl->cp_str.string, నమూనా);
	compl = compl->cp_తదుపరి;
    } అయితే (compl != ఖాళీ && !is_మొదటి_సరిపోలిక(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    స్థిర శూన్య
క్రమీకరించు_compl_సరిపోలిక_list(పూర్ణాంకం (*compare)(స్థిరరాశి శూన్య *, స్థిరరాశి శూన్య *))
{
    compl_T	*orig_పాఠ్యం;

    ఒకవేళ (!compl_మొదటి_సరిపోలిక || is_మొదటి_సరిపోలిక(compl_మొదటి_సరిపోలిక->cp_తదుపరి))
	తిరిగి;

    orig_పాఠ్యం = కనుగొను_original_పాఠ్యం_సరిపోలిక();
    ఒకవేళ (orig_పాఠ్యం == ఖాళీ)
	తిరిగి;

    ins_compl_make_రేఖీయ();
    ఒకవేళ (orig_పాఠ్యం == compl_మొదటి_సరిపోలిక)
    {
	compl_మొదటి_సరిపోలిక->cp_తదుపరి->cp_prev = ఖాళీ;
	compl_మొదటి_సరిపోలిక->cp_తదుపరి = mergesort_list(compl_మొదటి_సరిపోలిక->cp_తదుపరి,
		cp_get_తదుపరి, cp_సమితి_తదుపరి, cp_get_prev, cp_సమితి_prev, compare);
	compl_మొదటి_సరిపోలిక->cp_తదుపరి->cp_prev = compl_మొదటి_సరిపోలిక;
    }
    లేకపోతే
    {
	compl_T	*తోక;

	orig_పాఠ్యం->cp_prev->cp_తదుపరి = ఖాళీ;
	compl_మొదటి_సరిపోలిక = mergesort_list(compl_మొదటి_సరిపోలిక, cp_get_తదుపరి,
		cp_సమితి_తదుపరి, cp_get_prev, cp_సమితి_prev, compare);
	తోక = compl_మొదటి_సరిపోలిక;
	అయితే (తోక->cp_తదుపరి != ఖాళీ)
	    తోక = తోక->cp_తదుపరి;
	తోక->cp_తదుపరి = orig_పాఠ్యం;
	orig_పాఠ్యం->cp_prev = తోక;
    }
    (శూన్య)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    స్థిర పూర్ణాంకం
get_user_highlight_attr(పూర్ణాంకం hl_id)
{
    పూర్ణాంకం	    attr;

    ఒకవేళ (hl_id <= 0)
	తిరిగి -1;
    attr = syn_id2attr(hl_id);
    తిరిగి attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    స్థిర పూర్ణాంకం
ins_compl_build_pum(శూన్య)
{
    compl_T     *compl;
    compl_T     *shown_compl = ఖాళీ;
    పూర్ణాంకం		did_కనుగొను_shown_సరిపోలిక = FALSE;
    పూర్ణాంకం		shown_సరిపోలిక_ok = FALSE;
    పూర్ణాంకం		i = 0;
    పూర్ణాంకం		cur = -1;
    పూర్ణాంకం		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*సరిపోలిక_తల = ఖాళీ;
    compl_T	*సరిపోలిక_తోక = ఖాళీ;
    compl_T	*సరిపోలిక_తదుపరి = ఖాళీ;
    పూర్ణాంకం		*సరిపోలిక_లెక్క = ఖాళీ;
    పూర్ణాంకం		is_forward = compl_shows_dir_forward();
    పూర్ణాంకం		is_cpt_completion = (cpt_sources_array != ఖాళీ);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_సరిపోలిక_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    ఒకవేళ (సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక))
	shown_సరిపోలిక_ok = TRUE;

    ఒకవేళ (compl_leader.string != ఖాళీ
	    && STRCMP(compl_leader.string, compl_orig_పాఠ్యం.string) == 0
	    && shown_సరిపోలిక_ok == FALSE)
	compl_shown_సరిపోలిక = compl_no_select ? compl_మొదటి_సరిపోలిక
					    : compl_మొదటి_సరిపోలిక->cp_తదుపరి;

    ఒకవేళ (is_cpt_completion)
    {
	సరిపోలిక_లెక్క = ALLOC_CLEAR_MULT(పూర్ణాంకం, cpt_sources_లెక్క);
	ఒకవేళ (సరిపోలిక_లెక్క == ఖాళీ)
	    తిరిగి -1;
    }

    (శూన్య)get_leader_for_startcol(ఖాళీ, TRUE); // Clear the cache

    compl = compl_మొదటి_సరిపోలిక;
    చేయి
    {
	compl->cp_లోపల_సరిపోలిక_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	ఒకవేళ (ctrl_x_బహుళకం_సాధారణ() && !p_inf && compl_orig_పాఠ్యం.string
		&& !ignorecase(compl_orig_పాఠ్యం.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(compl)
		&& (leader->string == ఖాళీ
		    || ins_compl_equal_sc(compl, leader->string,
			(పూర్ణాంకం)leader->నిడివి)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    పూర్ణాంకం సరిపోలిక_హద్దు_exceeded = FALSE;
	    పూర్ణాంకం cur_ఆధారం = compl->cp_cpt_ఆధారం_idx;
	    ఒకవేళ (is_forward && cur_ఆధారం != -1 && is_cpt_completion)
	    {
		సరిపోలిక_లెక్క[cur_ఆధారం]++;
		పూర్ణాంకం max_matches = cpt_sources_array[cur_ఆధారం].cs_max_matches;
		ఒకవేళ (max_matches > 0 && సరిపోలిక_లెక్క[cur_ఆధారం] > max_matches)
		    సరిపోలిక_హద్దు_exceeded = TRUE;
	    }

	    ఒకవేళ (!సరిపోలిక_హద్దు_exceeded)
	    {
		++compl_సరిపోలిక_arraysize;
		compl->cp_లోపల_సరిపోలిక_array = TRUE;
		ఒకవేళ (సరిపోలిక_తల == ఖాళీ)
		    సరిపోలిక_తల = compl;
		లేకపోతే
		    సరిపోలిక_తోక->cp_సరిపోలిక_తదుపరి = compl;
		సరిపోలిక_తోక = compl;

		ఒకవేళ (!shown_సరిపోలిక_ok && !cot_fuzzy())
		{
		    ఒకవేళ (compl == compl_shown_సరిపోలిక || did_కనుగొను_shown_సరిపోలిక)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_సరిపోలిక = compl;
			did_కనుగొను_shown_సరిపోలిక = TRUE;
			shown_సరిపోలిక_ok = TRUE;
		    }
		    లేకపోతే
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		లేకపోతే ఒకవేళ (cot_fuzzy())
		{
		    ఒకవేళ (i == 0)
			shown_compl = compl;

		    ఒకవేళ (!shown_సరిపోలిక_ok && compl == compl_shown_సరిపోలిక)
		    {
			cur = i;
			shown_సరిపోలిక_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	ఒకవేళ (compl == compl_shown_సరిపోలిక && !cot_fuzzy())
	{
	    did_కనుగొను_shown_సరిపోలిక = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    ఒకవేళ (సరిపోలిక_at_original_పాఠ్యం(compl))
		shown_సరిపోలిక_ok = TRUE;

	    ఒకవేళ (!shown_సరిపోలిక_ok && shown_compl != ఖాళీ)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_సరిపోలిక = shown_compl;
		shown_సరిపోలిక_ok = TRUE;
	    }
	}
	compl = compl->cp_తదుపరి;
    } అయితే (compl != ఖాళీ && !is_మొదటి_సరిపోలిక(compl));

    vim_free(సరిపోలిక_లెక్క);

    ఒకవేళ (compl_సరిపోలిక_arraysize == 0)
	తిరిగి -1;

    ఒకవేళ (cot_fuzzy() && !compl_no_select && !shown_సరిపోలిక_ok)
    {
	compl_shown_సరిపోలిక = shown_compl;
	shown_సరిపోలిక_ok = TRUE;
	cur = 0;
    }

    compl_సరిపోలిక_array = ALLOC_CLEAR_MULT(pumitem_T, compl_సరిపోలిక_arraysize);
    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
	తిరిగి -1;

    compl = సరిపోలిక_తల;
    i = 0;
    అయితే (compl != ఖాళీ)
    {
	compl_సరిపోలిక_array[i].pum_పాఠ్యం = compl->cp_పాఠ్యం[CPT_ABBR] != ఖాళీ
			    ? compl->cp_పాఠ్యం[CPT_ABBR] : compl->cp_str.string;
	compl_సరిపోలిక_array[i].pum_రకం = compl->cp_పాఠ్యం[CPT_రకం];
	compl_సరిపోలిక_array[i].pum_info = compl->cp_పాఠ్యం[CPT_INFO];
	compl_సరిపోలిక_array[i].pum_cpt_ఆధారం_idx = compl->cp_cpt_ఆధారం_idx;
	compl_సరిపోలిక_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_సరిపోలిక_array[i].pum_user_రకం_hlattr =
			get_user_highlight_attr(compl->cp_user_రకం_hl_id);
	compl_సరిపోలిక_array[i++].pum_extra = compl->cp_పాఠ్యం[CPT_MENU] != ఖాళీ
			    ? compl->cp_పాఠ్యం[CPT_MENU] : compl->cp_fname;
	సరిపోలిక_తదుపరి = compl->cp_సరిపోలిక_తదుపరి;
	compl->cp_సరిపోలిక_తదుపరి = ఖాళీ;
	compl = సరిపోలిక_తదుపరి;
    }

    ఒకవేళ (!shown_సరిపోలిక_ok)    // no displayed match at all
	cur = -1;

    తిరిగి cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    శూన్య
ins_compl_show_pum(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		cur = -1;
    colnr_T	నిలువు;

    ఒకవేళ (!pum_wanted() || !pum_enough_matches())
	తిరిగి;

    // Avoid redrawing the screen under a pum that stays in place.
    ఒకవేళ (!pum_redraw_లోపల_same_position())
	pum_call_update_screen();

    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    లేకపోతే
    {
	// popup menu already exists, only need to find the current item.
	కొరకు (i = 0; i < compl_సరిపోలిక_arraysize; ++i)
	{
	    ఒకవేళ (compl_సరిపోలిక_array[i].pum_పాఠ్యం == compl_shown_సరిపోలిక->cp_str.string
		    || compl_సరిపోలిక_array[i].pum_పాఠ్యం
				      == compl_shown_సరిపోలిక->cp_పాఠ్యం[CPT_ABBR])
	    {
		cur = i;
		ఆపు;
	    }
	}
    }

    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
    {
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (compl_started && has_completechanged())
	    trigger_complete_మారింది_event(cur);
#అయితే_ముగింపు
	తిరిగి;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    నిలువు = curwin->w_cursor.నిలువు;
    curwin->w_cursor.నిలువు = compl_నిలువు;
    validate_cursor_నిలువు();
    పూర్ణాంకం pum_wcol = curwin->w_wcol;
    curwin->w_cursor.నిలువు = నిలువు;
    validate_cursor_నిలువు();
    compl_selected_అంశం = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_స్థితి = true;
    pum_display(compl_సరిపోలిక_array, compl_సరిపోలిక_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    ఒకవేళ (compl_started && compl_curr_సరిపోలిక != compl_shown_సరిపోలిక)
	compl_curr_సరిపోలిక = compl_shown_సరిపోలిక;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (has_completechanged())
	trigger_complete_మారింది_event(cur);
#అయితే_ముగింపు
}

#నిర్వచించు DICT_మొదటి	(1)	// use just first element in "dict"
#నిర్వచించు DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    స్థిర శూన్య
ins_compl_dictionaries(
    char_u	*dict_ప్రారంభం,
    char_u	*pat,
    పూర్ణాంకం		flags,		// DICT_FIRST and/or DICT_EXACT
    పూర్ణాంకం		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_ప్రారంభం;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		save_p_scs;
    పూర్ణాంకం		dir = compl_direction;

    ఒకవేళ (*dict == NUL)
    {
#నిర్వచితమైతే FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	ఒకవేళ (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	లేకపోతే
#అయితే_ముగింపు
	    తిరిగి;
    }

    buf = alloc(LSIZE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    regmatch.regprog = ఖాళీ;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ఒకవేళ (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    ఒకవేళ (ctrl_x_బహుళకం_పంక్తి_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	ఒకవేళ (pat_esc == ఖాళీ)
	    వెళ్ళు theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	ఒకవేళ (ptr == ఖాళీ)
	{
	    vim_free(pat_esc);
	    వెళ్ళు theend;
	}
	vim_snprintf((అక్షరం *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    లేకపోతే
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ఒకవేళ (regmatch.regprog == ఖాళీ)
	    వెళ్ళు theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    అయితే (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	ఒకవేళ (flags == DICT_EXACT)
	{
	    లెక్క = 1;
	    files = &dict;
	}
	లేకపోతే
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#నిర్వచితమైతే FEAT_SPELL
	    ఒకవేళ (!thesaurus && STRCMP(buf, "spell") == 0)
		లెక్క = -1;
	    లేకపోతే
#అయితే_ముగింపు
		ఒకవేళ (vim_strchr(buf, '`') != ఖాళీ
		    || expand_wildcards(1, &buf, &లెక్క, &files,
						     EW_FILE|EW_SILENT) != OK)
		లెక్క = 0;
	}

#నిర్వచితమైతే FEAT_SPELL
	ఒకవేళ (లెక్క == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    ఒకవేళ (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    లేకపోతే
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (లెక్క > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(లెక్క, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    ఒకవేళ (flags != DICT_EXACT)
		FreeWild(లెక్క, files);
	}
	ఒకవేళ (flags != 0)
	    ఆపు;
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
    స్థిర పూర్ణాంకం
thesaurus_కూడు_పదాలు_లోపల_పంక్తి(
    char_u	*fname,
    char_u	**buf_arg,
    పూర్ణాంకం		dir,
    char_u	*skip_మాట)
{
    పూర్ణాంకం		స్థితి_2 = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    అయితే (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = కనుగొను_మాట_ప్రారంభం(ptr);
	ఒకవేళ (*ptr == NUL || *ptr == NL)
	    ఆపు;
	wstart = ptr;

	// Find end of the word.
	ఒకవేళ (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    అయితే (*ptr != NUL)
	    {
		పూర్ణాంకం l = (*mb_ptr2len)(ptr);

		ఒకవేళ (l < 2 && !vim_iswordc(*ptr))
		    ఆపు;
		ptr += l;
	    }
	లేకపోతే
	    ptr = కనుగొను_మాట_చివర(ptr);

	// Add the word. Skip the regexp match.
	ఒకవేళ (wstart != skip_మాట)
	{
	    స్థితి_2 = ins_compl_కూడు_infercase(wstart, (పూర్ణాంకం)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    ఒకవేళ (స్థితి_2 == FAIL)
		ఆపు;
	}
    }

    *buf_arg = ptr;
    తిరిగి స్థితి_2;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    స్థిర శూన్య
ins_compl_files(
    పూర్ణాంకం		లెక్క,
    char_u	**files,
    పూర్ణాంకం		thesaurus,
    పూర్ణాంకం		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    పూర్ణాంకం		*dir)
{
    char_u	*ptr;
    పూర్ణాంకం		i;
    FILE	*fp;
    పూర్ణాంకం		కూడు_r;
    char_u	*leader = ఖాళీ;
    పూర్ణాంకం		leader_len = 0;
    పూర్ణాంకం		score = 0;
    పూర్ణాంకం		len = 0;
    char_u	*పంక్తి_చివర = ఖాళీ;

    ఒకవేళ (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (పూర్ణాంకం)ins_compl_leader_len();
    }

    కొరకు (i = 0; i < లెక్క && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((అక్షరం *)files[i], "r");  // open dictionary file
	ఒకవేళ (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    సందేశం_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (అక్షరం *)files[i]);
	    (శూన్య)సందేశం_trunc_attr((అక్షరం *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	ఒకవేళ (fp == ఖాళీ)
	    కొనసాగు;

	// Read dictionary file line by line.
	// Check each line for a match.
	అయితే (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    ఒకవేళ (cot_fuzzy() && leader_len > 0)
	    {
		పంక్తి_చివర = కనుగొను_పంక్తి_చివర(ptr);
		అయితే (ptr < పంక్తి_చివర)
		{
		    ఒకవేళ (fuzzy_సరిపోలిక_str_లోపల_పంక్తి(&ptr, leader, &len, ఖాళీ, &score))
		    {
			char_u *end_ptr = ctrl_x_బహుళకం_పంక్తి_or_eval()
					? కనుగొను_పంక్తి_చివర(ptr) : కనుగొను_మాట_చివర(ptr);
			కూడు_r = ins_compl_కూడు_infercase(ptr, (పూర్ణాంకం)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			ఒకవేళ (కూడు_r == FAIL)
			    ఆపు;
			ptr = end_ptr;  // start from next word
			ఒకవేళ (compl_get_longest && ctrl_x_బహుళకం_సాధారణ()
				&& compl_మొదటి_సరిపోలిక->cp_తదుపరి
				&& score == compl_మొదటి_సరిపోలిక->cp_తదుపరి->cp_score)
			    compl_అంకె_bests++;
		    }
		}
	    }
	    లేకపోతే ఒకవేళ (regmatch != ఖాళీ)
	    {
		అయితే (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_బహుళకం_పంక్తి_or_eval() ? కనుగొను_పంక్తి_చివర(ptr)
						    : కనుగొను_మాట_చివర(ptr);
		    కూడు_r = ins_compl_కూడు_infercase(regmatch->startp[0],
			    (పూర్ణాంకం)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    ఒకవేళ (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			కూడు_r = thesaurus_కూడు_పదాలు_లోపల_పంక్తి(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    ఒకవేళ (కూడు_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    లేకపోతే ఒకవేళ (కూడు_r == FAIL)
			ఆపు;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    ఒకవేళ (*ptr == '\n' || got_int)
			ఆపు;
		}
	    }
	    పంక్తి_breakcheck();
	    ins_compl_check_కీలకాలు(50, FALSE);
	}
	fclose(fp);
    }
}

/*
 * Find the start of the next word.
 * Returns a pointer to the first char of the word.  Also stops at a NUL.
 */
    char_u *
కనుగొను_మాట_ప్రారంభం(char_u *ptr)
{
    ఒకవేళ (has_mbyte)
    {
	అయితే (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    లేకపోతే
    {
	అయితే (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    తిరిగి ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
కనుగొను_మాట_చివర(char_u *ptr)
{
    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం ప్రారంభం_class = mb_get_class(ptr);
	ఒకవేళ (ప్రారంభం_class > 1)
	    అయితే (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		ఒకవేళ (mb_get_class(ptr) != ప్రారంభం_class)
		    ఆపు;
	    }
    }
    లేకపోతే
	అయితే (vim_iswordc(*ptr))
	    ++ptr;
    తిరిగి ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
కనుగొను_పంక్తి_చివర(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    అయితే (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    తిరిగి s;
}

/*
 * Free a completion item in the list
 */
    స్థిర శూన్య
ins_compl_అంశం_free(compl_T *సరిపోలిక)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    ఒకవేళ (compl_అక్షరశ్రేణులు_ht.ht_వాడిన > 0 && సరిపోలిక->cp_str.string != ఖాళీ
					   && !సరిపోలిక_at_original_పాఠ్యం(సరిపోలిక))
    {
	hashitem_T *hi = hash_కనుగొను(&compl_అక్షరశ్రేణులు_ht, సరిపోలిక->cp_str.string);

	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    complstr_T *నమోదు_2 = HI2CSE(hi);

	    ఒకవేళ (--నమోదు_2->cse_లెక్క <= 0)
	    {
		hash_remove(&compl_అక్షరశ్రేణులు_ht, hi, "completion match");
		vim_free(నమోదు_2);
	    }
	}
    }
    VIM_CLEAR_STRING(సరిపోలిక->cp_str);
    // several entries may use the same fname, free it just once.
    ఒకవేళ (సరిపోలిక->cp_flags & CP_FREE_FNAME)
	vim_free(సరిపోలిక->cp_fname);
    కొరకు (పూర్ణాంకం i = 0; i < CPT_లెక్క; ++i)
	vim_free(సరిపోలిక->cp_పాఠ్యం[i]);
#నిర్వచితమైతే FEAT_EVAL
    clear_tv(&సరిపోలిక->cp_user_దత్తాంశం);
#అయితే_ముగింపు
    vim_free(సరిపోలిక);
}

/*
 * Free the list of completions
 */
    స్థిర శూన్య
ins_compl_free(శూన్య)
{
    compl_T *సరిపోలిక;

    VIM_CLEAR_STRING(compl_నమూనా);
    VIM_CLEAR_STRING(compl_leader);

    ఒకవేళ (compl_మొదటి_సరిపోలిక == ఖాళీ)
	తిరిగి;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_అక్షరశ్రేణులు_ht, CSE_OFF);
    hash_init(&compl_అక్షరశ్రేణులు_ht);

    compl_curr_సరిపోలిక = compl_మొదటి_సరిపోలిక;
    చేయి
    {
	సరిపోలిక = compl_curr_సరిపోలిక;
	compl_curr_సరిపోలిక = compl_curr_సరిపోలిక->cp_తదుపరి;
	ins_compl_అంశం_free(సరిపోలిక);
    } అయితే (compl_curr_సరిపోలిక != ఖాళీ && !is_మొదటి_సరిపోలిక(compl_curr_సరిపోలిక));
    compl_మొదటి_సరిపోలిక = compl_curr_సరిపోలిక = ఖాళీ;
    compl_shown_సరిపోలిక = ఖాళీ;
    compl_old_సరిపోలిక = ఖాళీ;
}

/*
 * Reset/clear the completion state.
 */
    శూన్య
ins_compl_clear(శూన్య)
{
    compl_cont_స్థితి = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_అంశం = -1;
    compl_ins_చివర_నిలువు = 0;
    compl_longest_చివర_నిలువు = 0;
    compl_curr_win = ఖాళీ;
    compl_curr_buf = ఖాళీ;
    VIM_CLEAR_STRING(compl_నమూనా);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ఖాళీ;
    VIM_CLEAR_STRING(compl_orig_పాఠ్యం);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_అంకె_bests = 0;
#నిర్వచితమైతే FEAT_EVAL
    // clear v:completed_item
    సమితి_vim_var_dict(VV_COMPLETED_అంశం, dict_alloc_తాళం(VAR_FIXED));
#అయితే_ముగింపు
}

/*
 * Return TRUE when Insert completion is active.
 */
    పూర్ణాంకం
ins_compl_చురుకైన(శూన్య)
{
    తిరిగి compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    పూర్ణాంకం
ins_compl_win_చురుకైన(win_T *wp)
{
    తిరిగి ins_compl_చురుకైన() && wp == compl_curr_win
	&& wp->w_బఫరు == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    పూర్ణాంకం
ins_compl_వాడిన_సరిపోలిక(శూన్య)
{
    తిరిగి compl_వాడిన_సరిపోలిక;
}

/*
 * Initialize get longest common string.
 */
    శూన్య
ins_compl_init_get_longest(శూన్య)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    పూర్ణాంకం
ins_compl_interrupted(శూన్య)
{
    తిరిగి compl_interrupted || compl_సమయం_ముక్క_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    పూర్ణాంకం
ins_compl_enter_selects(శూన్య)
{
    తిరిగి compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_నిలువు(శూన్య)
{
    తిరిగి compl_నిలువు;
}

/*
 * Return the length in bytes of the text being completed
 */
    పూర్ణాంకం
ins_compl_len(శూన్య)
{
    తిరిగి compl_నిడివి;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    పూర్ణాంకం
ins_compl_has_preinsert(శూన్య)
{
    పూర్ణాంకం cur_cot_flags = get_cot_flags();
    ఒకవేళ (compl_autocomplete && p_ic && !p_inf)
	తిరిగి FALSE;
    తిరిగి !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    పూర్ణాంకం
ins_compl_preinsert_effect(శూన్య)
{
    ఒకవేళ (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	తిరిగి FALSE;

    తిరిగి curwin->w_cursor.నిలువు < compl_ins_చివర_నిలువు;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    పూర్ణాంకం
ins_compl_bs(శూన్య)
{
    char_u	*పంక్తి;
    char_u	*p;

    ఒకవేళ (ins_compl_preinsert_effect())
	ins_compl_delete();

    పంక్తి = ml_get_curline();
    p = పంక్తి + curwin->w_cursor.నిలువు;
    MB_PTR_వెనుక(పంక్తి, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    ఒకవేళ ((పూర్ణాంకం)(p - పంక్తి) - (పూర్ణాంకం)compl_నిలువు < 0
	    || ((పూర్ణాంకం)(p - పంక్తి) - (పూర్ణాంకం)compl_నిలువు == 0 && !ctrl_x_బహుళకం_omni())
	    || ctrl_x_బహుళకం_eval()
	    || (!can_bs(BS_ప్రారంభం) && (పూర్ణాంకం)(p - పంక్తి) - (పూర్ణాంకం)compl_నిలువు
							- compl_నిడివి < 0))
	తిరిగి K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    ఒకవేళ (curwin->w_cursor.నిలువు <= compl_నిలువు + compl_నిడివి
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.నిడివి = (size_t)((p - పంక్తి) - compl_నిలువు);
    compl_leader.string = vim_strnsave(పంక్తి + compl_నిలువు, compl_leader.నిడివి);
    ఒకవేళ (compl_leader.string == ఖాళీ)
    {
	compl_leader.నిడివి = 0;
	తిరిగి K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    ఒకవేళ (compl_autocomplete && compl_మొదటి_సరిపోలిక && !ins_compl_has_preinsert())
	compl_shown_సరిపోలిక = compl_మొదటి_సరిపోలిక;

    ins_compl_new_leader();
    ఒకవేళ (compl_shown_సరిపోలిక != ఖాళీ)
	// Make sure current match is not a hidden item.
	compl_curr_సరిపోలిక = compl_shown_సరిపోలిక;
    తిరిగి NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    స్థిర పూర్ణాంకం
ins_compl_need_restart(శూన్య)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    తిరిగి compl_was_interrupted
	|| ((ctrl_x_బహుళకం_function() || ctrl_x_బహుళకం_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    పూర్ణాంకం
ins_compl_has_autocomplete(శూన్య)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    తిరిగి curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    స్థిర శూన్య
ins_compl_fuzzy_క్రమీకరించు(శూన్య)
{
    పూర్ణాంకం	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    సమితి_fuzzy_score();
    ఒకవేళ (!(cur_cot_flags & COT_NOSORT))
    {
	క్రమీకరించు_compl_సరిపోలిక_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	ఒకవేళ ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    పూర్ణాంకం none_selected = compl_shown_సరిపోలిక == (compl_shows_dir_forward()
		    ? compl_మొదటి_సరిపోలిక : compl_మొదటి_సరిపోలిక->cp_prev);
	    ఒకవేళ (!none_selected)
		compl_shown_సరిపోలిక
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_మొదటి_సరిపోలిక->cp_తదుపరి : compl_మొదటి_సరిపోలిక;
	}
    }
}

/*
 * Called after changing "compl_leader".
 * Show the popup menu with a different set of matches.
 * May also search for matches again if the previous search was interrupted.
 */
    స్థిర శూన్య
ins_compl_new_leader(శూన్య)
{
    పూర్ణాంకం	    save_w_wrow = curwin->w_wrow;
    పూర్ణాంకం	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_చొప్పించు_బైట్లు(compl_leader.string + get_compl_len(), -1);
    compl_వాడిన_సరిపోలిక = FALSE;

    ఒకవేళ (compl_started)
    {
	ins_compl_సమితి_original_పాఠ్యం(compl_leader.string, compl_leader.నిడివి);
	ఒకవేళ (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	ఒకవేళ (cot_fuzzy())
	    ins_compl_fuzzy_క్రమీకరించు();
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#అయితే_ముగింపు
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    బహిర్గతం_flush_cursor(FALSE, FALSE);
	}
#అయితే_ముగింపు
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	ఒకవేళ (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	లేకపోతే
	    compl_autocomplete = FALSE;
	ఒకవేళ (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_స్థితి = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_వాడిన_సరిపోలిక && compl_selected_అంశం != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    ఒకవేళ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
	compl_enter_selects = FALSE;
    లేకపోతే ఒకవేళ (ins_compl_has_preinsert() && compl_leader.నిడివి > 0)
	ins_compl_చొప్పించు(TRUE, FALSE);
    లేకపోతే ఒకవేళ (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.నిడివి > 0 && !ins_compl_preinsert_effect())
	ins_compl_చొప్పించు(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    స్థిర పూర్ణాంకం
get_compl_len(శూన్య)
{
    పూర్ణాంకం off = (పూర్ణాంకం)curwin->w_cursor.నిలువు - (పూర్ణాంకం)compl_నిలువు;
    తిరిగి MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    శూన్య
ins_compl_addleader(పూర్ణాంకం c)
{
    పూర్ణాంకం	    cc;

    ఒకవేళ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ఒకవేళ (stop_arrow() == FAIL)
	తిరిగి;
    ఒకవేళ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_బైట్లు(buf, cc);
	ఒకవేళ (compl_opt_refresh_always)
	    Append_గమ్యం_Redobuff(buf);
    }
    లేకపోతే
    {
	ins_char(c);
	ఒకవేళ (compl_opt_refresh_always)
	    Append_Char_గమ్యం_Redobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    ఒకవేళ (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    ఒకవేళ (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.నిడివి = (size_t)(curwin->w_cursor.నిలువు - compl_నిలువు);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_నిలువు,
		compl_leader.నిడివి);
	ఒకవేళ (compl_leader.string == ఖాళీ)
	{
	    compl_leader.నిడివి = 0;
	    తిరిగి;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    స్థిర శూన్య
ins_compl_restart(శూన్య)
{
    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    compl_cont_స్థితి = 0;
    compl_cont_బహుళకం = 0;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_అంకె_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    స్థిర శూన్య
ins_compl_సమితి_original_పాఠ్యం(char_u *str, size_t len)
{
    compl_T	*సరిపోలిక = కనుగొను_original_పాఠ్యం_సరిపోలిక();
    char_u	*p;

    // Replace the original text entry.
    ఒకవేళ (సరిపోలిక == ఖాళీ)
	తిరిగి;

    p = vim_strnsave(str, len);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    VIM_CLEAR_STRING(సరిపోలిక->cp_str);
    సరిపోలిక->cp_str.string = p;
    సరిపోలిక->cp_str.నిడివి = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    శూన్య
ins_compl_addfrommatch(శూన్య)
{
    char_u	*p;
    పూర్ణాంకం		len = (పూర్ణాంకం)curwin->w_cursor.నిలువు - (పూర్ణాంకం)compl_నిలువు;
    పూర్ణాంకం		c;

    p = compl_shown_సరిపోలిక->cp_str.string;
    ఒకవేళ ((పూర్ణాంకం)compl_shown_సరిపోలిక->cp_str.నిడివి <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక))
	    తిరిగి;

	p = ఖాళీ;
	plen = 0;
	కొరకు (cp = compl_shown_సరిపోలిక->cp_తదుపరి; cp != ఖాళీ
		&& !is_మొదటి_సరిపోలిక(cp); cp = cp->cp_తదుపరి)
	{
	    ఒకవేళ (compl_leader.string == ఖాళీ
		    || ins_compl_equal(cp, compl_leader.string,
			(పూర్ణాంకం)compl_leader.నిడివి))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.నిడివి;
		ఆపు;
	    }
	}
	ఒకవేళ (p == ఖాళీ || (పూర్ణాంకం)plen <= len)
	    తిరిగి;
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
    స్థిర పూర్ణాంకం
సమితి_ctrl_x_బహుళకం(పూర్ణాంకం c)
{
    పూర్ణాంకం	    retval = FALSE;

    ఎంపిక (c)
    {
	స్థితి Ctrl_E:
	స్థితి Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_బహుళకం = CTRL_X_SCROLL;
	    ఒకవేళ (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    లేకపోతే
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ఖాళీ;
	    showmode();
	    ఆపు;
	స్థితి Ctrl_L:
	    // complete whole line
	    ctrl_x_బహుళకం = CTRL_X_సంపూర్ణ_పంక్తి;
	    ఆపు;
	స్థితి Ctrl_F:
	    // complete filenames
	    ctrl_x_బహుళకం = CTRL_X_FILES;
	    ఆపు;
	స్థితి Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_బహుళకం = CTRL_X_DICTIONARY;
	    ఆపు;
	స్థితి Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    ఒకవేళ (vpeekc() == '=')
		ఆపు;
	    ctrl_x_బహుళకం = CTRL_X_రిజిస్టరు;
	    ఆపు;
	స్థితి Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_బహుళకం = CTRL_X_THESAURUS;
	    ఆపు;
#నిర్వచితమైతే FEAT_COMPL_FUNC
	స్థితి Ctrl_U:
	    // user defined completion
	    ctrl_x_బహుళకం = CTRL_X_FUNCTION;
	    ఆపు;
	స్థితి Ctrl_O:
	    // omni completion
	    ctrl_x_బహుళకం = CTRL_X_OMNI;
	    ఆపు;
#అయితే_ముగింపు
	స్థితి 's':
	స్థితి Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_బహుళకం = CTRL_X_SPELL;
#నిర్వచితమైతే FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_వెనుక_గమ్యం_badword();
	    --emsg_off;
#అయితే_ముగింపు
	    ఆపు;
	స్థితి Ctrl_RSB:
	    // complete tag names
	    ctrl_x_బహుళకం = CTRL_X_ట్యాగులు;
	    ఆపు;
#నిర్వచితమైతే FEAT_కనుగొను_ID
	స్థితి Ctrl_I:
	స్థితి K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_బహుళకం = CTRL_X_మార్గం_PATTERNS;
	    ఆపు;
	స్థితి Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_బహుళకం = CTRL_X_మార్గం_నిర్వచిస్తుంది;
	    ఆపు;
#అయితే_ముగింపు
	స్థితి Ctrl_V:
	స్థితి Ctrl_Q:
	    // complete vim commands
	    ctrl_x_బహుళకం = CTRL_X_CMDLINE;
	    ఆపు;
	స్థితి Ctrl_Z:
	    // stop completion
	    ctrl_x_బహుళకం = CTRL_X_సాధారణ;
	    edit_submode = ఖాళీ;
	    showmode();
	    retval = TRUE;
	    ఆపు;
	స్థితి Ctrl_P:
	స్థితి Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    ఒకవేళ (!(compl_cont_స్థితి & CONT_INTRPT))
		compl_cont_స్థితి |= CONT_LOCAL;
	    లేకపోతే ఒకవేళ (compl_cont_బహుళకం != 0)
		compl_cont_స్థితి &= ~CONT_LOCAL;
	    // FALLTHROUGH
	అప్రమేయ:
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
	    ఒకవేళ (c == Ctrl_X)
	    {
		ఒకవేళ (compl_cont_బహుళకం != 0)
		    compl_cont_స్థితి = 0;
		లేకపోతే
		    compl_cont_బహుళకం = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_బహుళకం = CTRL_X_సాధారణ;
	    edit_submode = ఖాళీ;
	    showmode();
	    ఆపు;
    }

    తిరిగి retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    స్థిర శూన్య
trigger_complete_పూర్తి_event(పూర్ణాంకం బహుళకం వాడని, string_T *మాట వాడని)
{
#ఒకవేళ defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*బహుళకం_str;

    ఒకవేళ (మాట == ఖాళీ || మాట->string == ఖాళీ)
	(శూన్య)dict_కూడు_string_len(v_event, "complete_word", (char_u *)"", 0);
    లేకపోతే
	(శూన్య)dict_కూడు_string_len(v_event, "complete_word", మాట->string, (పూర్ణాంకం)మాట->నిడివి);

    బహుళకం_str = &ctrl_x_బహుళకం_పేర్లు[బహుళకం & ~CTRL_X_WANT_IDENT];
    (శూన్య)dict_కూడు_string_len(v_event, "complete_type",
	(బహుళకం_str->string == ఖాళీ) ? (char_u *)"" : బహుళకం_str->string, (పూర్ణాంకం)బహుళకం_str->నిడివి);

    dict_సమితి_వస్తువులు_ro(v_event);
#అయితే_ముగింపు
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#ఒకవేళ defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#అయితే_ముగింపు
}

/*
 * Stop insert completion mode
 */
    స్థిర పూర్ణాంకం
ins_compl_stop(పూర్ణాంకం c, పూర్ణాంకం prev_బహుళకం, పూర్ణాంకం retval)
{
    పూర్ణాంకం		want_cindent;
    string_T	మాట = {ఖాళీ, 0};

    // Remove pre-inserted text when present.
    ఒకవేళ (ins_compl_preinsert_effect() && ins_compl_win_చురుకైన(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    ఒకవేళ (compl_curr_సరిపోలిక != ఖాళీ || compl_leader.string != ఖాళీ || c == Ctrl_E)
    {
	char_u	*ptr = ఖాళీ;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	ఒకవేళ (compl_curr_సరిపోలిక != ఖాళీ && compl_వాడిన_సరిపోలిక && c != Ctrl_E)
	    ptr = compl_curr_సరిపోలిక->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    ఒకవేళ (compl_cont_బహుళకం == CTRL_X_సంపూర్ణ_పంక్తి)
    {
	// re-indent the current line
	ఒకవేళ (want_cindent)
	{
	    do_c_సమాసం_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    లేకపోతే ఒకవేళ (!compl_autocomplete || compl_వాడిన_సరిపోలిక)
    {
	పూర్ణాంకం prev_నిలువు = curwin->w_cursor.నిలువు;

	// put the cursor on the last char, for 'tw' formatting
	ఒకవేళ (prev_నిలువు > 0)
	    dec_cursor();
	// only format when something was inserted
	ఒకవేళ (!arrow_వాడిన && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	ఒకవేళ (prev_నిలువు > 0
		&& ml_get_curline()[curwin->w_cursor.నిలువు] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    ఒకవేళ ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	మాట.string = vim_strnsave(compl_shown_సరిపోలిక->cp_str.string,
	    compl_shown_సరిపోలిక->cp_str.నిడివి);
	ఒకవేళ (మాట.string == ఖాళీ)
	    మాట.నిడివి = 0;
	లేకపోతే
	    మాట.నిడివి = compl_shown_సరిపోలిక->cp_str.నిడివి;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    ఒకవేళ (c == Ctrl_E)
    {
	char_u *p = ఖాళీ;
	size_t	plen = 0;

	ins_compl_delete();
	ఒకవేళ (compl_leader.string != ఖాళీ)
	{
	    p = compl_leader.string;
	    plen = compl_leader.నిడివి;
	}
	లేకపోతే ఒకవేళ (compl_మొదటి_సరిపోలిక != ఖాళీ)
	{
	    p = compl_orig_పాఠ్యం.string;
	    plen = compl_orig_పాఠ్యం.నిడివి;
	}
	ఒకవేళ (p != ఖాళీ)
	{
	    పూర్ణాంకం	    compl_len = get_compl_len();

	    ఒకవేళ ((పూర్ణాంకం)plen > compl_len)
		ins_compl_చొప్పించు_బైట్లు(p + compl_len, (పూర్ణాంకం)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_బహుళకం = prev_బహుళకం;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    ఒకవేళ (!shortmess(SHM_COMPLETIONMENU))
	సందేశం_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_బహుళకం = CTRL_X_సాధారణ;
    compl_enter_selects = FALSE;
    ఒకవేళ (edit_submode != ఖాళీ)
    {
	edit_submode = ఖాళీ;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_అంకె_bests = 0;
    compl_ins_చివర_నిలువు = 0;

    ఒకవేళ (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    ఒకవేళ (want_cindent && లోపల_cinkeys(కీలకం_COMPLETE, ' ', inindent(0)))
	do_c_సమాసం_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_పూర్తి_event(prev_బహుళకం, &మాట);
    vim_free(మాట.string);

    తిరిగి retval;
}

/*
 * Cancel completion.
 */
    పూర్ణాంకం
ins_compl_cancel(శూన్య)
{
    తిరిగి ins_compl_stop(' ', ctrl_x_బహుళకం, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    పూర్ణాంకం
ins_compl_prep(పూర్ణాంకం c)
{
    పూర్ణాంకం	    retval = FALSE;
    పూర్ణాంకం	    prev_బహుళకం = ctrl_x_బహుళకం;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    ఒకవేళ (c != Ctrl_R && vim_is_ctrl_x_కీలకం(c))
	edit_submode_extra = ఖాళీ;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    ఒకవేళ (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	తిరిగి retval;

#నిర్వచితమైతే FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    ఒకవేళ (is_mouse_కీలకం(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	ఒకవేళ (c == K_LEFTRELEASE
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
	    తిరిగి retval;
	ఒకవేళ (popup_visible)
	{
	    పూర్ణాంకం	    అడ్డువరుస = mouse_అడ్డువరుస;
	    పూర్ణాంకం	    నిలువు = mouse_నిలువు;
	    win_T   *wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);

	    ఒకవేళ (wp != ఖాళీ && WIN_IS_POPUP(wp))
		తిరిగి retval;
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (ctrl_x_బహుళకం == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	ఒకవేళ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_కీలకం(c)
		|| !vim_is_ctrl_x_కీలకం(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_బహుళకం = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    ఒకవేళ (c == Ctrl_Z)
		retval = TRUE;
	}
	లేకపోతే
	{
	    ctrl_x_బహుళకం = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_బహుళకం = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    ఒకవేళ (ctrl_x_బహుళకం_not_defined_yet()
			   || (ctrl_x_బహుళకం_సాధారణ() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_వాడిన_సరిపోలిక = TRUE;
    }

    ఒకవేళ (ctrl_x_బహుళకం_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = సమితి_ctrl_x_బహుళకం(c);
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	ఒకవేళ (c != Ctrl_R && !vim_is_ctrl_x_కీలకం(c))
	{
	    ctrl_x_బహుళకం = ctrl_x_బహుళకం_scroll() ? CTRL_X_సాధారణ : CTRL_X_ముగిసిన;
	    edit_submode = ఖాళీ;
	}
	showmode();
    }

    ఒకవేళ (compl_started || ctrl_x_బహుళకం == CTRL_X_ముగిసిన)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	ఒకవేళ ((ctrl_x_బహుళకం_సాధారణ() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_కీలకం(c))
		|| ctrl_x_బహుళకం == CTRL_X_ముగిసిన)
	    retval = ins_compl_stop(c, prev_బహుళకం, retval);
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం == CTRL_X_LOCAL_సందేశం)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_పూర్తి_event(ctrl_x_బహుళకం, ఖాళీ);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    ఒకవేళ (c != Ctrl_R && !vim_is_ctrl_x_కీలకం(c))
    {
	compl_cont_స్థితి = 0;
	compl_cont_బహుళకం = 0;
    }

    తిరిగి retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    స్థిర శూన్య
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    పూర్ణాంకం	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    ఒకవేళ (ptr == ఖాళీ)
    {
	ఒకవేళ (compl_leader.string != ఖాళీ)
	    ptr = compl_leader.string;
	లేకపోతే
	    తిరిగి;  // nothing to do
    }
    ఒకవేళ (compl_orig_పాఠ్యం.string != ఖాళీ)
    {
	p = compl_orig_పాఠ్యం.string;
	// Find length of common prefix between original text and new completion
	అయితే (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	ఒకవేళ (len > 0)
	    len -= (*mb_తల_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	కొరకు (p += len; *p != NUL; MB_PTR_ADV(p))
	    Append_Char_గమ్యం_Redobuff(K_BS);
    }
    ఒకవేళ (ptr != ఖాళీ)
	Append_గమ్యం_Redobuff_Lit(ptr + len, -1);
}

/*
 * Loops through the list of windows, loaded-buffers or non-loaded-buffers
 * (depending on flag) starting from buf and looking for a non-scanned
 * buffer (other than curbuf).	curbuf is special, if it is called with
 * buf=curbuf then it has to be the first call for a given flag/expansion.
 *
 * Returns the buffer to scan, if any, otherwise returns curbuf -- Acevedo
 */
    స్థిర buf_T *
ins_compl_తదుపరి_buf(buf_T *buf, పూర్ణాంకం flag)
{
    స్థిర win_T    *wp = ఖాళీ;
    పూర్ణాంకం		    skip_బఫరు;

    ఒకవేళ (flag == 'w')		// just windows
    {
	ఒకవేళ (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	అయితే (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_తదుపరి != ఖాళీ) ? wp->w_తదుపరి : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    ఒకవేళ (wp == curwin || !wp->w_బఫరు->b_scanned)
		ఆపు;
	}
	buf = wp->w_బఫరు;
    }
    లేకపోతే
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	అయితే (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_తదుపరి != ఖాళీ) ? buf->b_తదుపరి : firstbuf;
	    // Break if we're back at start buffer
	    ఒకవేళ (buf == curbuf)
		ఆపు;

	    // Check buffer conditions based on flag
	    ఒకవేళ (flag == 'U')
		skip_బఫరు = buf->b_p_bl;
	    లేకపోతే
		skip_బఫరు = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ఖాళీ) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    ఒకవేళ (!skip_బఫరు && !buf->b_scanned)
		ఆపు;
	}
    }
    తిరిగి buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    స్థిర పూర్ణాంకం
get_cpt_sources_లెక్క(char_u *cpt)
{
    char_u  dummy[LSIZE];
    పూర్ణాంకం	    లెక్క = 0;
    char_u  *p;

    కొరకు (p = cpt; *p != NUL; )
    {
	అయితే (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	ఒకవేళ (*p != NUL)
	{
	    (శూన్య)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    లెక్క++;
	}
    }

    తిరిగి లెక్క;
}

#నిర్వచితమైతే FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
స్థిర callback_T cfu_cb;	    // 'completefunc' callback function
స్థిర callback_T ofu_cb;	    // 'omnifunc' callback function
స్థిర callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
స్థిర callback_T *cpt_cb;	    // Callback functions associated with F{func}
స్థిర పూర్ణాంకం	  cpt_cb_లెక్క;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    స్థిర శూన్య
copy_global_గమ్యం_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    ఒకవేళ (globcb->cb_name != ఖాళీ && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    అక్షరం *
did_సమితి_completefunc(optset_T *వాదనలు)
{
    పూర్ణాంకం	retval;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
	retval = option_సమితి_callback_func(వాదనలు->os_newval.string,
							    &curbuf->b_cfu_cb);
    లేకపోతే
    {
	retval = option_సమితి_callback_func(వాదనలు->os_newval.string, &cfu_cb);
	ఒకవేళ (retval == OK && !(వాదనలు->os_flags & OPT_GLOBAL))
	    సమితి_buflocal_cfu_callback(curbuf);
    }

    తిరిగి retval == FAIL ? e_invalid_వాదం : ఖాళీ;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    శూన్య
సమితి_buflocal_cfu_callback(buf_T *buf వాడని)
{
# ifdef FEAT_EVAL
    copy_global_గమ్యం_buflocal_cb(&cfu_cb, &buf->b_cfu_cb);
# endif
}

/*
 * Parse the 'omnifunc' option value and set the callback function.
 * Invoked when the 'omnifunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    అక్షరం *
did_సమితి_omnifunc(optset_T *వాదనలు)
{
    పూర్ణాంకం	retval;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
	retval = option_సమితి_callback_func(వాదనలు->os_newval.string,
							    &curbuf->b_ofu_cb);
    లేకపోతే
    {
	retval = option_సమితి_callback_func(వాదనలు->os_newval.string, &ofu_cb);
	ఒకవేళ (retval == OK && !(వాదనలు->os_flags & OPT_GLOBAL))
	    సమితి_buflocal_ofu_callback(curbuf);
    }

    తిరిగి retval == FAIL ? e_invalid_వాదం : ఖాళీ;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    శూన్య
సమితి_buflocal_ofu_callback(buf_T *buf వాడని)
{
# ifdef FEAT_EVAL
    copy_global_గమ్యం_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    శూన్య
clear_cpt_callbacks(callback_T **callbacks, పూర్ణాంకం లెక్క)
{
    ఒకవేళ (callbacks == ఖాళీ || *callbacks == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    స్థిర పూర్ణాంకం
copy_cpt_callbacks(callback_T **dest, పూర్ణాంకం *dest_cnt, callback_T *src, పూర్ణాంకం cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    ఒకవేళ (cnt == 0)
	తిరిగి OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    ఒకవేళ (*dest == ఖాళీ)
	తిరిగి FAIL;

    *dest_cnt = cnt;

    కొరకు (పూర్ణాంకం i = 0; i < cnt; i++)
	ఒకవేళ (src[i].cb_name != ఖాళీ && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    తిరిగి OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    శూన్య
సమితి_buflocal_cpt_callbacks(buf_T *buf వాడని)
{
# ifdef FEAT_EVAL
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    (శూన్య)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_లెక్క, cpt_cb,
	    cpt_cb_లెక్క);
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
    స్థిర పూర్ణాంకం
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, పూర్ణాంకం *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    పూర్ణాంకం	    idx = 0;
    పూర్ణాంకం	    slen;
    పూర్ణాంకం	    లెక్క;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    లెక్క = get_cpt_sources_లెక్క(cpt);
    ఒకవేళ (లెక్క == 0)
	తిరిగి OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, లెక్క);
    ఒకవేళ (*cbp == ఖాళీ)
	తిరిగి FAIL;
    *cnt = లెక్క;

    కొరకు (p = cpt; *p != NUL; )
    {
	అయితే (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	ఒకవేళ (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ఒకవేళ (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		ఒకవేళ (caret != ఖాళీ)
		    *caret = NUL;

		ఒకవేళ (option_సమితి_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ఖాళీ;
	    }
	    idx++;
	}
    }

    తిరిగి OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    పూర్ణాంకం
సమితి_cpt_callbacks(optset_T *వాదనలు)
{
    పూర్ణాంకం	    opt_flags = వాదనలు->os_flags;

    ఒకవేళ (curbuf == ఖాళీ)
	తిరిగి FAIL;

    // ":setglobal" does not change the buffer-local option value
    ఒకవేళ (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_లెక్క) != OK)
	తిరిగి FAIL;

    // ":setlocal" does not change the global option value
    ఒకవేళ (opt_flags & OPT_LOCAL)
	తిరిగి OK;

    // Cache the callbacks for the global value
    ఒకవేళ (opt_flags & OPT_GLOBAL)
	తిరిగి parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_లెక్క);

    // set case
    తిరిగి copy_cpt_callbacks(&cpt_cb, &cpt_cb_లెక్క, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_లెక్క);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    అక్షరం *
did_సమితి_thesaurusfunc(optset_T *వాదనలు వాడని)
{
    పూర్ణాంకం	retval;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_సమితి_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    లేకపోతే
    {
	// global option set
	retval = option_సమితి_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	ఒకవేళ (!(వాదనలు->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    తిరిగి retval == FAIL ? e_invalid_వాదం : ఖాళీ;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_cpt_callbacks(callback_T *callbacks, పూర్ణాంకం లెక్క, పూర్ణాంకం copyID)
{
    పూర్ణాంకం	abort = FALSE;

    ఒకవేళ (callbacks == ఖాళీ)
	తిరిగి FALSE;

    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; i++)
	abort = abort || సమితి_ref_లోపల_callback(&callbacks[i], copyID);
    తిరిగి abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_insexpand_funcs(పూర్ణాంకం copyID)
{
    పూర్ణాంకం abort = సమితి_ref_లోపల_callback(&cfu_cb, copyID);
    abort = abort || సమితి_ref_లోపల_callback(&ofu_cb, copyID);
    abort = abort || సమితి_ref_లోపల_callback(&tsrfu_cb, copyID);
    abort = abort || సమితి_ref_లోపల_cpt_callbacks(cpt_cb, cpt_cb_లెక్క, copyID);

    తిరిగి abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    స్థిర char_u *
get_complete_funcname(పూర్ణాంకం type)
{
    ఎంపిక (type)
    {
	స్థితి CTRL_X_FUNCTION:
	    తిరిగి curbuf->b_p_cfu;
	స్థితి CTRL_X_OMNI:
	    తిరిగి curbuf->b_p_ofu;
	స్థితి CTRL_X_THESAURUS:
	    తిరిగి *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	అప్రమేయ:
	    తిరిగి (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    స్థిర callback_T *
get_చొప్పించు_callback(పూర్ణాంకం type)
{
    ఒకవేళ (type == CTRL_X_FUNCTION)
	తిరిగి &curbuf->b_cfu_cb;
    ఒకవేళ (type == CTRL_X_OMNI)
	తిరిగి &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    తిరిగి (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    స్థిర శూన్య
expand_by_function(పూర్ణాంకం type, char_u *పునాది, callback_T *cb)
{
    list_T      *matchlist = ఖాళీ;
    dict_T	*matchdict = ఖాళీ;
    typval_T	వాదనలు[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    పూర్ణాంకం		save_State = State;
    పూర్ణాంకం		retval;
    పూర్ణాంకం		is_cpt_function = (cb != ఖాళీ);
    పూర్ణాంకం		use_sandbox = is_cpt_function
			    && was_సమితి_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ఒకవేళ (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	ఒకవేళ (*funcname == NUL)
	    తిరిగి;
	cb = get_చొప్పించు_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    వాదనలు[0].v_type = VAR_సంఖ్య;
    వాదనలు[0].vval.v_సంఖ్య = 0;
    వాదనలు[1].v_type = VAR_STRING;
    వాదనలు[1].vval.v_string = పునాది != ఖాళీ ? పునాది : (char_u *)"";
    వాదనలు[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    ఒకవేళ (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, వాదనలు);
    ఒకవేళ (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    ఒకవేళ (retval == OK)
    {
	ఎంపిక (rettv.v_type)
	{
	    స్థితి VAR_LIST:
		matchlist = rettv.vval.v_list;
		ఆపు;
	    స్థితి VAR_DICT:
		matchdict = rettv.vval.v_dict;
		ఆపు;
	    స్థితి VAR_SPECIAL:
		ఒకవేళ (rettv.vval.v_సంఖ్య == VVAL_NONE)
		    compl_opt_suppress_ఖాళీలు = TRUE;
		// FALLTHROUGH
	    అప్రమేయ:
		// TODO: Give error message?
		clear_tv(&rettv);
		ఆపు;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ఒకవేళ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_పాఠ్యం));
	వెళ్ళు theend;
    }

    ఒకవేళ (matchlist != ఖాళీ)
	ins_compl_కూడు_list(matchlist);
    లేకపోతే ఒకవేళ (matchdict != ఖాళీ)
	ins_compl_కూడు_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    ఒకవేళ (matchdict != ఖాళీ)
	dict_unref(matchdict);
    ఒకవేళ (matchlist != ఖాళీ)
	list_unref(matchlist);
}
#అయితే_ముగింపు // FEAT_COMPL_FUNC

#ఒకవేళ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    స్థిర అంతఃవరుస పూర్ణాంకం
get_user_highlight_id(char_u *hlname)
{
    ఒకవేళ (hlname != ఖాళీ && *hlname != NUL)
	తిరిగి syn_check_సమూహం(hlname, (పూర్ణాంకం)STRLEN(hlname));
    తిరిగి 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    స్థిర పూర్ణాంకం
ins_compl_కూడు_tv(typval_T *tv, పూర్ణాంకం dir, పూర్ణాంకం fast)
{
    char_u	*మాట;
    పూర్ణాంకం		dup = FALSE;
    పూర్ణాంకం		ఖాళీలు_2 = FALSE;
    పూర్ణాంకం		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_లెక్క]);
    typval_T	user_దత్తాంశం;
    పూర్ణాంకం		స్థితి_2;
    char_u	*user_abbr_hlname;
    char_u	*user_రకం_hlname;
    పూర్ణాంకం		user_hl[2] = { 0, 0 };

    user_దత్తాంశం.v_type = VAR_UNKNOWN;
    ఒకవేళ (tv->v_type == VAR_DICT && tv->vval.v_dict != ఖాళీ)
    {
	మాట = dict_get_string(tv->vval.v_dict, "word", FALSE);
	cptext[CPT_ABBR] = dict_get_string(tv->vval.v_dict, "abbr", FALSE);
	cptext[CPT_MENU] = dict_get_string(tv->vval.v_dict, "menu", FALSE);
	cptext[CPT_రకం] = dict_get_string(tv->vval.v_dict, "kind", FALSE);
	cptext[CPT_INFO] = dict_get_string(tv->vval.v_dict, "info", FALSE);

	user_abbr_hlname = dict_get_string(tv->vval.v_dict, "abbr_hlgroup", FALSE);
	user_hl[0] = get_user_highlight_id(user_abbr_hlname);

	user_రకం_hlname = dict_get_string(tv->vval.v_dict, "kind_hlgroup", FALSE);
	user_hl[1] = get_user_highlight_id(user_రకం_hlname);

	dict_get_tv(tv->vval.v_dict, "user_data", &user_దత్తాంశం);
	ఒకవేళ (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ఖాళీ
				  && dict_get_సంఖ్య(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	ఒకవేళ (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ఖాళీ)
	    dup = dict_get_సంఖ్య(tv->vval.v_dict, "dup");
	ఒకవేళ (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ఖాళీ)
	    ఖాళీలు_2 = dict_get_సంఖ్య(tv->vval.v_dict, "empty");
	ఒకవేళ (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ఖాళీ
				  && dict_get_సంఖ్య(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    లేకపోతే
    {
	మాట = tv_get_string_chk(tv);
	CLEAR_క్షేత్రం(cptext);
    }
    ఒకవేళ (మాట == ఖాళీ || (!ఖాళీలు_2 && *మాట == NUL))
    {
	clear_tv(&user_దత్తాంశం);
	తిరిగి FAIL;
    }
    స్థితి_2 = ins_compl_కూడు(మాట, -1, ఖాళీ, cptext,
	    &user_దత్తాంశం, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    ఒకవేళ (స్థితి_2 != OK)
	clear_tv(&user_దత్తాంశం);
    తిరిగి స్థితి_2;
}

/*
 * Add completions from a list.
 */
    స్థిర శూన్య
ins_compl_కూడు_list(list_T *list)
{
    listitem_T	*li;
    పూర్ణాంకం		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_వస్తువులు(list, li)
    {
	ఒకవేళ (ins_compl_కూడు_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	లేకపోతే ఒకవేళ (did_emsg)
	    ఆపు;
    }
}

/*
 * Add completions from a dict.
 */
    స్థిర శూన్య
ins_compl_కూడు_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_పదాలు;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_కనుగొను(dict, (char_u *)"refresh", 7);
    ఒకవేళ (di_refresh != ఖాళీ && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	ఒకవేళ (v != ఖాళీ && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_పదాలు = dict_కనుగొను(dict, (char_u *)"words", 5);
    ఒకవేళ (di_పదాలు != ఖాళీ && di_పదాలు->di_tv.v_type == VAR_LIST)
	ins_compl_కూడు_list(di_పదాలు->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    స్థిర శూన్య
సమితి_completion(colnr_T startcol, list_T *list)
{
    పూర్ణాంకం save_w_wrow = curwin->w_wrow;
    పూర్ణాంకం save_w_leftcol = curwin->w_leftcol;
    పూర్ణాంకం flags = CP_ORIGINAL_పాఠ్యం;
    గుర్తులేని పూర్ణాంకం cur_cot_flags = get_cot_flags();
    పూర్ణాంకం compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    పూర్ణాంకం compl_no_చొప్పించు = (cur_cot_flags & COT_NOINSERT) != 0;
    పూర్ణాంకం compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    ఒకవేళ (compl_started || ctrl_x_బహుళకం_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    ఒకవేళ (startcol > curwin->w_cursor.నిలువు)
	startcol = curwin->w_cursor.నిలువు;
    compl_నిలువు = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_నిడివి = (పూర్ణాంకం)curwin->w_cursor.నిలువు - (పూర్ణాంకం)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_పాఠ్యం.string = vim_strnsave(ml_get_curline() + compl_నిలువు,
							(size_t)compl_నిడివి);
    ఒకవేళ (p_ic)
	flags |= CP_ICASE;
    ఒకవేళ (compl_orig_పాఠ్యం.string == ఖాళీ)
    {
	compl_orig_పాఠ్యం.నిడివి = 0;
	తిరిగి;
    }
    compl_orig_పాఠ్యం.నిడివి = (size_t)compl_నిడివి;
    ఒకవేళ (ins_compl_కూడు(compl_orig_పాఠ్యం.string,
			(పూర్ణాంకం)compl_orig_పాఠ్యం.నిడివి, ఖాళీ, ఖాళీ, ఖాళీ, 0,
			flags | CP_FAST, FALSE, ఖాళీ, FUZZY_SCORE_NONE) != OK)
	తిరిగి;

    ctrl_x_బహుళకం = CTRL_X_EVAL;

    ins_compl_కూడు_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_వాడిన_సరిపోలిక = TRUE;
    compl_cont_స్థితి = 0;

    compl_curr_సరిపోలిక = compl_మొదటి_సరిపోలిక;
    పూర్ణాంకం no_select = compl_no_select || compl_longest;
    ఒకవేళ (compl_no_చొప్పించు || no_select)
    {
	ins_complete(K_కిందికి, FALSE);
	ఒకవేళ (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_పైకి, FALSE);
    }
    లేకపోతే
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_చొప్పించు;

    // Lazily show the popup menu, unless we got interrupted.
    ఒకవేళ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    బహిర్గతం_flush();
}

/*
 * "complete()" function
 */
    శూన్య
f_complete(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ ((State & బహుళకం_చొప్పించు) == 0)
    {
	emsg(_(e_complete_can_only_be_వాడిన_లోపల_చొప్పించు_బహుళకం));
	తిరిగి;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    ఒకవేళ (!undo_allowed())
	తిరిగి;

    ఒకవేళ (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	పూర్ణాంకం startcol = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ);
	ఒకవేళ (startcol > 0)
	    సమితి_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    శూన్య
f_complete_కూడు(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = ins_compl_కూడు_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    శూన్య
f_complete_check(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_కీలకాలు(0, TRUE);
    rettv->vval.v_సంఖ్య = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    స్థిర శూన్య
ins_compl_బహుళకం(string_T *ret)
{
    ఒకవేళ (ctrl_x_బహుళకం_not_defined_yet() || ctrl_x_బహుళకం_scroll() || compl_started)
    {
	string_T    *బహుళకం;

	బహుళకం = &ctrl_x_బహుళకం_పేర్లు[ctrl_x_బహుళకం & ~CTRL_X_WANT_IDENT];
	ret->string = (బహుళకం->string == ఖాళీ) ? (char_u *)"" : బహుళకం->string;
	ret->నిడివి = బహుళకం->నిడివి;
    }
    లేకపోతే
    {
	ret->string = (char_u *)"";
	ret->నిడివి = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    స్థిర శూన్య
ins_compl_update_sequence_numbers(శూన్య)
{
    పూర్ణాంకం		సంఖ్య = 0;
    compl_T	*సరిపోలిక;

    ఒకవేళ (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	కొరకు (సరిపోలిక = compl_curr_సరిపోలిక->cp_prev; సరిపోలిక != ఖాళీ
		&& !is_మొదటి_సరిపోలిక(సరిపోలిక); సరిపోలిక = సరిపోలిక->cp_prev)
	    ఒకవేళ (సరిపోలిక->cp_సంఖ్య != -1)
	    {
		సంఖ్య = సరిపోలిక->cp_సంఖ్య;
		ఆపు;
	    }
	ఒకవేళ (సరిపోలిక != ఖాళీ)
	    // go up and assign all numbers which are not assigned yet
	    కొరకు (సరిపోలిక = సరిపోలిక->cp_తదుపరి;
		    సరిపోలిక != ఖాళీ && సరిపోలిక->cp_సంఖ్య == -1;
					   సరిపోలిక = సరిపోలిక->cp_తదుపరి)
		సరిపోలిక->cp_సంఖ్య = ++సంఖ్య;
    }
    లేకపోతే // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	కొరకు (సరిపోలిక = compl_curr_సరిపోలిక->cp_తదుపరి; సరిపోలిక != ఖాళీ
		&& !is_మొదటి_సరిపోలిక(సరిపోలిక); సరిపోలిక = సరిపోలిక->cp_తదుపరి)
	{
	    ఒకవేళ (సరిపోలిక->cp_సంఖ్య != -1)
	    {
		సంఖ్య = సరిపోలిక->cp_సంఖ్య;
		ఆపు;
	    }
	}
	ఒకవేళ (సరిపోలిక != ఖాళీ)
	{
	    // go down and assign all numbers which are not assigned yet
	    కొరకు (సరిపోలిక = సరిపోలిక->cp_prev; సరిపోలిక
		    && సరిపోలిక->cp_సంఖ్య == -1;
					   సరిపోలిక = సరిపోలిక->cp_prev)
		సరిపోలిక->cp_సంఖ్య = ++సంఖ్య;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    స్థిర శూన్య
fill_complete_info_dict(dict_T *di, compl_T *సరిపోలిక, పూర్ణాంకం కూడు_సరిపోలిక)
{
    dict_కూడు_string_len(di, "word", సరిపోలిక->cp_str.string, (పూర్ణాంకం)సరిపోలిక->cp_str.నిడివి);
    dict_కూడు_string(di, "abbr", సరిపోలిక->cp_పాఠ్యం[CPT_ABBR]);
    dict_కూడు_string(di, "menu", సరిపోలిక->cp_పాఠ్యం[CPT_MENU]);
    dict_కూడు_string(di, "kind", సరిపోలిక->cp_పాఠ్యం[CPT_రకం]);
    dict_కూడు_string(di, "info", సరిపోలిక->cp_పాఠ్యం[CPT_INFO]);
    dict_కూడు_string(di, "abbr_hlgroup", syn_id2name(సరిపోలిక->cp_user_abbr_hl_id));
    dict_కూడు_string(di, "kind_hlgroup", syn_id2name(సరిపోలిక->cp_user_రకం_hl_id));
    ఒకవేళ (కూడు_సరిపోలిక)
	dict_కూడు_bool(di, "match", సరిపోలిక->cp_లోపల_సరిపోలిక_array);
    ఒకవేళ (సరిపోలిక->cp_user_దత్తాంశం.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_కూడు_string_len(di, "user_data", (char_u *)"", 0);
    లేకపోతే
	dict_కూడు_tv(di, "user_data", &సరిపోలిక->cp_user_దత్తాంశం);
}

/*
 * Get complete information
 */
    స్థిర శూన్య
get_complete_info(list_T *what_list, dict_T *retdict)
{
    పూర్ణాంకం		ret = OK;
    listitem_T	*అంశం;
# define CI_WHAT_బహుళకం		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_వస్తువులు		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_పాఠ్యం   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    పూర్ణాంకం		what_flag;

    ఒకవేళ (what_list == ఖాళీ)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    లేకపోతే
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_వస్తువులు(what_list, అంశం)
	{
	    char_u *what = tv_get_string(&అంశం->li_tv);

	    ఒకవేళ (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_బహుళకం;
	    లేకపోతే ఒకవేళ (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    లేకపోతే ఒకవేళ (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_వస్తువులు;
	    లేకపోతే ఒకవేళ (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    లేకపోతే ఒకవేళ (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    లేకపోతే ఒకవేళ (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_పాఠ్యం;
	    లేకపోతే ఒకవేళ (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    లేకపోతే ఒకవేళ (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    ఒకవేళ (ret == OK && (what_flag & CI_WHAT_బహుళకం))
    {
	string_T    బహుళకం;

	ins_compl_బహుళకం(&బహుళకం);
	ret = dict_కూడు_string_len(retdict, "mode", బహుళకం.string, (పూర్ణాంకం)బహుళకం.నిడివి);
    }

    ఒకవేళ (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_కూడు_సంఖ్య(retdict, "pum_visible", pum_visible());

    ఒకవేళ (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_కూడు_సంఖ్య(retdict, "auto", compl_autostarted);

    ఒకవేళ (ret == OK && (what_flag & CI_WHAT_PREINSERTED_పాఠ్యం))
    {
	char_u	*పంక్తి = ml_get_curline();
	పూర్ణాంకం	len = compl_ins_చివర_నిలువు - curwin->w_cursor.నిలువు;

	ret = dict_కూడు_string_len(retdict, "preinserted_text",
		(len > 0) ? పంక్తి + curwin->w_cursor.నిలువు : (char_u *)"", (len > 0) ? len : 0);
    }

    ఒకవేళ (ret == OK && (what_flag & (CI_WHAT_వస్తువులు | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ఖాళీ;
	dict_T	    *di;
	compl_T     *సరిపోలిక;
	పూర్ణాంకం         selected_idx = -1;
	పూర్ణాంకం	    has_వస్తువులు = what_flag & CI_WHAT_వస్తువులు;
	పూర్ణాంకం	    has_matches = what_flag & CI_WHAT_MATCHES;
	పూర్ణాంకం	    has_completed = what_flag & CI_WHAT_COMPLETED;

	ఒకవేళ (has_వస్తువులు || has_matches)
	{
	    li = list_alloc();
	    ఒకవేళ (li == ఖాళీ)
		తిరిగి;
	    ret = dict_కూడు_list(retdict, (has_matches && !has_వస్తువులు)
						? "matches" : "items", li);
	    ఒకవేళ (ret == FAIL)
		list_unref(li);
	}
	ఒకవేళ (ret == OK && what_flag & CI_WHAT_SELECTED)
	    ఒకవేళ (compl_curr_సరిపోలిక != ఖాళీ && compl_curr_సరిపోలిక->cp_సంఖ్య == -1)
		ins_compl_update_sequence_numbers();
	ఒకవేళ (ret == OK && compl_మొదటి_సరిపోలిక != ఖాళీ)
	{
	    పూర్ణాంకం list_idx = 0;
	    సరిపోలిక = compl_మొదటి_సరిపోలిక;
	    చేయి
	    {
		ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(సరిపోలిక))
		{
		    ఒకవేళ (has_వస్తువులు
			    || (has_matches && సరిపోలిక->cp_లోపల_సరిపోలిక_array))
		    {
			di = dict_alloc();
			ఒకవేళ (di == ఖాళీ)
			    తిరిగి;
			ret = list_append_dict(li, di);
			ఒకవేళ (ret != OK)
			{
			    dict_unref(di);
			    తిరిగి;
			}
			fill_complete_info_dict(di, సరిపోలిక, has_matches && has_వస్తువులు);
		    }
		    ఒకవేళ (compl_curr_సరిపోలిక != ఖాళీ
			    && compl_curr_సరిపోలిక->cp_సంఖ్య == సరిపోలిక->cp_సంఖ్య)
			selected_idx = list_idx;
		    ఒకవేళ (!has_matches || సరిపోలిక->cp_లోపల_సరిపోలిక_array)
			list_idx++;
		}
		సరిపోలిక = సరిపోలిక->cp_తదుపరి;
	    }
	    అయితే (సరిపోలిక != ఖాళీ && !is_మొదటి_సరిపోలిక(సరిపోలిక));
	}
	ఒకవేళ (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_కూడు_సంఖ్య(retdict, "selected", selected_idx);

	ఒకవేళ (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    ఒకవేళ (di == ఖాళీ)
		తిరిగి;
	    fill_complete_info_dict(di, compl_curr_సరిపోలిక, FALSE);
	    ret = dict_కూడు_dict(retdict, "completed", di);
	    ఒకవేళ (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    శూన్య
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ఖాళీ;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	    తిరిగి;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#అయితే_ముగింపు

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    స్థిర పూర్ణాంకం
thesaurus_func_complete(పూర్ణాంకం type వాడని)
{
#నిర్వచితమైతే FEAT_COMPL_FUNC
    తిరిగి type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    స్థిర పూర్ణాంకం
may_advance_cpt_సూచీ(char_u *cpt)
{
    char_u  *p = cpt;

    ఒకవేళ (cpt_sources_సూచీ == -1)
	తిరిగి FALSE;
    అయితే (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    తిరిగి (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
గణన
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_చివర
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_సరిపోలిక_pos;		// current match position
    pos_T	prev_సరిపోలిక_pos;		// previous match position
    పూర్ణాంకం		సమితి_సరిపోలిక_pos;		// save first_match_pos/last_match_pos
    pos_T	మొదటి_సరిపోలిక_pos;	// first match position
    pos_T	చివరి_సరిపోలిక_pos;		// last match position
    పూర్ణాంకం		దొరికింది_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    పూర్ణాంకం		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_తదుపరి_state_T;

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
    స్థిర పూర్ణాంకం
process_తదుపరి_cpt_విలువ(
	ins_compl_తదుపరి_state_T *st,
	పూర్ణాంకం		*compl_type_arg,
	pos_T		*ప్రారంభం_సరిపోలిక_pos,
	పూర్ణాంకం		fuzzy_collect,
	పూర్ణాంకం		*advance_cpt_idx)
{
    పూర్ణాంకం	    compl_type = -1;
    పూర్ణాంకం	    స్థితి_2 = INS_COMPL_CPT_OK;
    పూర్ణాంకం	    skip_ఆధారం = compl_autocomplete && compl_from_nonkeyword;

    st->దొరికింది_all = FALSE;
    *advance_cpt_idx = FALSE;

    అయితే (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    ఒకవేళ (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_ఆధారం
	    && !compl_సమయం_ముక్క_expired)
    {
	st->ins_buf = curbuf;
	st->మొదటి_సరిపోలిక_pos = *ప్రారంభం_సరిపోలిక_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	ఒకవేళ (ctrl_x_బహుళకం_సాధారణ() && (!fuzzy_collect && dec(&st->మొదటి_సరిపోలిక_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->మొదటి_సరిపోలిక_pos.lnum = st->ins_buf->b_ml.ml_పంక్తి_లెక్క;
	    st->మొదటి_సరిపోలిక_pos.నిలువు = ml_get_len(st->మొదటి_సరిపోలిక_pos.lnum);
	}
	st->చివరి_సరిపోలిక_pos = st->మొదటి_సరిపోలిక_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->సమితి_సరిపోలిక_pos = TRUE;
    }
    లేకపోతే ఒకవేళ (!skip_ఆధారం && !compl_సమయం_ముక్క_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ఖాళీ
	    && (st->ins_buf = ins_compl_తదుపరి_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	ఒకవేళ (st->ins_buf->b_ml.ml_mfp != ఖాళీ)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->మొదటి_సరిపోలిక_pos.నిలువు = st->చివరి_సరిపోలిక_pos.నిలువు = 0;
	    st->మొదటి_సరిపోలిక_pos.lnum = st->ins_buf->b_ml.ml_పంక్తి_లెక్క + 1;
	    st->చివరి_సరిపోలిక_pos.lnum = 0;
	    compl_type = 0;
	}
	లేకపోతే	// unloaded buffer, scan like dictionary
	{
	    st->దొరికింది_all = TRUE;
	    ఒకవేళ (st->ins_buf->b_fname == ఖాళీ)
	    {
		స్థితి_2 = INS_COMPL_CPT_CONT;
		వెళ్ళు పూర్తి;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	ఒకవేళ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    సందేశం_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ఖాళీ
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ఖాళీ
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (శూన్య)సందేశం_trunc_attr((అక్షరం *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    లేకపోతే ఒకవేళ (*st->e_cpt == NUL)
	స్థితి_2 = INS_COMPL_CPT_చివర;
    లేకపోతే
    {
	ఒకవేళ (ctrl_x_బహుళకం_పంక్తి_or_eval())
	    compl_type = -1;
#నిర్వచితమైతే FEAT_COMPL_FUNC
	లేకపోతే ఒకవేళ (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_సూచీ);
	    ఒకవేళ (!st->func_cb)
		compl_type = -1;
	}
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (!skip_ఆధారం)
	{
	    ఒకవేళ (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		ఒకవేళ (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		లేకపోతే
		    compl_type = CTRL_X_THESAURUS;
		ఒకవేళ (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_మొదటి;
		}
	    }
#నిర్వచితమైతే FEAT_కనుగొను_ID
	    లేకపోతే ఒకవేళ (*st->e_cpt == 'i')
		compl_type = CTRL_X_మార్గం_PATTERNS;
	    లేకపోతే ఒకవేళ (*st->e_cpt == 'd')
		compl_type = CTRL_X_మార్గం_నిర్వచిస్తుంది;
#అయితే_ముగింపు
	    లేకపోతే ఒకవేళ (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_ట్యాగులు;
		ఒకవేళ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    సందేశం_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Scanning tags."));
		    (శూన్య)సందేశం_trunc_attr((అక్షరం *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    లేకపోతే
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(శూన్య)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_సూచీ(st->e_cpt);

	st->దొరికింది_all = TRUE;
	ఒకవేళ (compl_type == -1)
	    స్థితి_2 = INS_COMPL_CPT_CONT;
    }

పూర్తి:
    *compl_type_arg = compl_type;
    తిరిగి స్థితి_2;
}

#నిర్వచితమైతే FEAT_కనుగొను_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    స్థిర శూన్య
get_తదుపరి_include_file_completion(పూర్ణాంకం compl_type)
{
    కనుగొను_నమూనా_లోపల_మార్గం(compl_నమూనా.string, compl_direction,
	    (పూర్ణాంకం)compl_నమూనా.నిడివి, FALSE, FALSE,
	    (compl_type == CTRL_X_మార్గం_నిర్వచిస్తుంది
	     && !(compl_cont_స్థితి & CONT_SOL))
	    ? కనుగొను_DEFINE : కనుగొను_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM, FALSE, compl_autocomplete);
}
#అయితే_ముగింపు

/*
 * Get the next set of words matching "compl_pattern" in dictionary or
 * thesaurus files.
 */
    స్థిర శూన్య
get_తదుపరి_dict_tsr_completion(పూర్ణాంకం compl_type, char_u *dict, పూర్ణాంకం dict_f)
{
#నిర్వచితమైతే FEAT_COMPL_FUNC
    ఒకవేళ (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_నమూనా.string, ఖాళీ);
    లేకపోతే
#అయితే_ముగింపు
	ins_compl_dictionaries(
		dict != ఖాళీ ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_నమూనా.string,
		dict != ఖాళీ ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    స్థిర శూన్య
get_తదుపరి_ట్యాగ్_completion(శూన్య)
{
    పూర్ణాంకం		save_p_ic;
    char_u	**matches;
    పూర్ణాంకం		అంకె_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_నమూనా.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_ట్యాగ్_at_cursor = TRUE;
    ఒకవేళ (కనుగొను_ట్యాగులు(compl_నమూనా.string, &అంకె_matches, &matches,
		ట్యాగ్_REGEXP | ట్యాగ్_పేర్లు | ట్యాగ్_NOIC | ట్యాగ్_INS_COMP
		| (ctrl_x_బహుళకం_not_default() ? ట్యాగ్_VERBOSE : 0),
		ట్యాగ్_MANY, curbuf->b_ffname) == OK && అంకె_matches > 0)
	ins_compl_కూడు_matches(అంకె_matches, matches, p_ic);
    g_ట్యాగ్_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    స్థిర పూర్ణాంకం
compare_scores(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    పూర్ణాంకం idx_a = *(స్థిరరాశి పూర్ణాంకం *)a;
    పూర్ణాంకం idx_b = *(స్థిరరాశి పూర్ణాంకం *)b;
    పూర్ణాంకం score_a = compl_fuzzy_scores[idx_a];
    పూర్ణాంకం score_b = compl_fuzzy_scores[idx_b];
    తిరిగి score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    స్థిర శూన్య
ins_compl_longest_చొప్పించు(char_u *ఉపసర్గ)
{
    ins_compl_delete();
    ins_compl_చొప్పించు_బైట్లు(ఉపసర్గ + get_compl_len(), -1);
    compl_longest_చివర_నిలువు = curwin->w_cursor.నిలువు;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    స్థిర శూన్య
fuzzy_longest_సరిపోలిక(శూన్య)
{
    char_u	*ఉపసర్గ = ఖాళీ;
    పూర్ణాంకం		ఉపసర్గ_len = 0;
    పూర్ణాంకం		i = 0;
    పూర్ణాంకం		j = 0;
    char_u	*సరిపోలిక_str = ఖాళీ;
    char_u	*ఉపసర్గ_ptr = ఖాళీ;
    char_u	*సరిపోలిక_ptr = ఖాళీ;
    char_u	*leader = ఖాళీ;
    size_t	leader_len = 0;
    compl_T	*compl = ఖాళీ;
    పూర్ణాంకం		మరిన్ని_candidates = FALSE;
    compl_T	*nn_compl = ఖాళీ;

    ఒకవేళ (compl_అంకె_bests == 0)
	తిరిగి;

    nn_compl = compl_మొదటి_సరిపోలిక->cp_తదుపరి->cp_తదుపరి;
    ఒకవేళ (nn_compl && nn_compl != compl_మొదటి_సరిపోలిక)
	మరిన్ని_candidates = TRUE;

    compl = ctrl_x_బహుళకం_సంపూర్ణ_పంక్తి() ? compl_మొదటి_సరిపోలిక
				    : compl_మొదటి_సరిపోలిక->cp_తదుపరి;
    ఒకవేళ (compl_అంకె_bests == 1)
    {
	// no more candidates insert the match str
	ఒకవేళ (!మరిన్ని_candidates)
	{
	    ins_compl_longest_చొప్పించు(compl->cp_str.string);
	    compl_అంకె_bests = 0;
	}
	compl_అంకె_bests = 0;
	తిరిగి;
    }

    ఒకవేళ ((size_t)compl_అంకె_bests > కొలత_MAX / పరిమాణం(compl_T *))
	తిరిగి;
    compl_ఉత్తమ_matches = (compl_T **)alloc(compl_అంకె_bests * పరిమాణం(compl_T *));
    ఒకవేళ (compl_ఉత్తమ_matches == ఖాళీ)
	తిరిగి;
    అయితే (compl != ఖాళీ && i < compl_అంకె_bests)
    {
	compl_ఉత్తమ_matches[i] = compl;
	compl = compl->cp_తదుపరి;
	i++;
    }

    ఉపసర్గ = compl_ఉత్తమ_matches[0]->cp_str.string;
    ఉపసర్గ_len = (పూర్ణాంకం)compl_ఉత్తమ_matches[0]->cp_str.నిడివి;

    కొరకు (i = 1; i < compl_అంకె_bests; i++)
    {
	సరిపోలిక_str = compl_ఉత్తమ_matches[i]->cp_str.string;
	ఉపసర్గ_ptr = ఉపసర్గ;
	సరిపోలిక_ptr = సరిపోలిక_str;
	j = 0;

	అయితే (j < ఉపసర్గ_len && *సరిపోలిక_ptr != NUL && *ఉపసర్గ_ptr != NUL)
	{
	    ఒకవేళ (STRNCMP(ఉపసర్గ_ptr, సరిపోలిక_ptr, mb_ptr2len(ఉపసర్గ_ptr)) != 0)
		ఆపు;

	    MB_PTR_ADV(ఉపసర్గ_ptr);
	    MB_PTR_ADV(సరిపోలిక_ptr);
	    j++;
	}

	ఒకవేళ (j > 0)
	    ఉపసర్గ_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    ఒకవేళ (leader_len > 0 && STRNCMP(ఉపసర్గ, leader, leader_len) != 0)
	వెళ్ళు చివర;

    ఉపసర్గ = vim_strnsave(ఉపసర్గ, ఉపసర్గ_len);
    ఒకవేళ (ఉపసర్గ != ఖాళీ)
    {
	ins_compl_longest_చొప్పించు(ఉపసర్గ);
	vim_free(ఉపసర్గ);
    }

చివర:
    vim_free(compl_ఉత్తమ_matches);
    compl_ఉత్తమ_matches = ఖాళీ;
    compl_అంకె_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    స్థిర శూన్య
get_తదుపరి_filename_completion(శూన్య)
{
    char_u	**matches;
    పూర్ణాంకం		అంకె_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    పూర్ణాంకం		i;
    పూర్ణాంకం		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    పూర్ణాంకం		లోపల_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    పూర్ణాంకం		*fuzzy_indices_దత్తాంశం;
    char_u	*చివరి_sep = ఖాళీ;
    పూర్ణాంకం		need_collect_bests = లోపల_fuzzy_collect && compl_get_longest;
    పూర్ణాంకం		max_score = 0;
    పూర్ణాంకం		current_score = 0;
    పూర్ణాంకం		dir = compl_direction;

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    అక్షరం pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#లేకపోతే
    అక్షరం pathsep = PATHSEP;
#అయితే_ముగింపు

    ఒకవేళ (లోపల_fuzzy_collect)
    {
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	ఒకవేళ (curbuf->b_p_csl[0] == 's')
	{
	    కొరకు (i = 0; i < (పూర్ణాంకం)leader_len; i++)
	    {
		ఒకవేళ (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	లేకపోతే ఒకవేళ (curbuf->b_p_csl[0] == 'b')
	{
	    కొరకు (i = 0; i < (పూర్ణాంకం)leader_len; i++)
	    {
		ఒకవేళ (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#అయితే_ముగింపు
	చివరి_sep = vim_strrchr(leader, pathsep);
	ఒకవేళ (చివరి_sep == ఖాళీ)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_నమూనా);
	    compl_నమూనా.string = vim_strnsave((char_u *)"*", 1);
	    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
		తిరిగి;
	    compl_నమూనా.నిడివి = 1;
	}
	లేకపోతే ఒకవేళ (*(చివరి_sep + 1) == '\0')
	    లోపల_fuzzy_collect = FALSE;
	లేకపోతే
	{
	    // Split leader into path and file parts
	    పూర్ణాంకం మార్గం_len = చివరి_sep - leader + 1;
	    char_u  *మార్గం_with_wildcard;

	    మార్గం_with_wildcard = alloc(మార్గం_len + 2);
	    ఒకవేళ (మార్గం_with_wildcard != ఖాళీ)
	    {
		vim_snprintf((అక్షరం *)మార్గం_with_wildcard, మార్గం_len + 2, "%*.*s*", మార్గం_len, మార్గం_len, leader);
		VIM_CLEAR_STRING(compl_నమూనా);
		compl_నమూనా.string = మార్గం_with_wildcard;
		compl_నమూనా.నిడివి = మార్గం_len + 1;

		// Move leader to the file part
		leader = చివరి_sep + 1;
		leader_len -= మార్గం_len;
	    }
	}
    }

    ఒకవేళ (expand_wildcards(1, &compl_నమూనా.string, &అంకె_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	తిరిగి;

    // May change home directory back to "~".
    tilde_replace(compl_నమూనా.string, అంకె_matches, matches);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    ఒకవేళ (curbuf->b_p_csl[0] != NUL)
    {
	కొరకు (i = 0; i < అంకె_matches; ++i)
	{
	    ptr = matches[i];
	    అయితే (*ptr != NUL)
	    {
		ఒకవేళ (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		లేకపోతే ఒకవేళ (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (లోపల_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, పరిమాణం(పూర్ణాంకం), 10);
	compl_fuzzy_scores = (పూర్ణాంకం *)alloc(పరిమాణం(పూర్ణాంకం) * అంకె_matches);

	కొరకు (i = 0; i < అంకె_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_సరిపోలిక_str(ptr, leader);
	    ఒకవేళ (score != FUZZY_SCORE_NONE)
	    {
		ఒకవేళ (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((పూర్ణాంకం *)fuzzy_indices.ga_దత్తాంశం)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	ఒకవేళ (fuzzy_indices.ga_len > 0)
	{
	    char_u	*సరిపోలిక = ఖాళీ;
	    fuzzy_indices_దత్తాంశం = (పూర్ణాంకం *)fuzzy_indices.ga_దత్తాంశం;
	    qsort(fuzzy_indices_దత్తాంశం, fuzzy_indices.ga_len, పరిమాణం(పూర్ణాంకం), compare_scores);

	    కొరకు (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		సరిపోలిక = matches[fuzzy_indices_దత్తాంశం[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_దత్తాంశం[i]];
		ఒకవేళ (ins_compl_కూడు(సరిపోలిక, -1, ఖాళీ, ఖాళీ, ఖాళీ, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ఖాళీ, current_score) == OK)
		    dir = FORWARD;

		ఒకవేళ (need_collect_bests)
		{
		    ఒకవేళ (i == 0 || current_score == max_score)
		    {
			compl_అంకె_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(అంకె_matches, matches);
	}
	లేకపోతే ఒకవేళ (leader_len > 0)
	{
	    FreeWild(అంకె_matches, matches);
	    అంకె_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	ఒకవేళ (compl_అంకె_bests > 0 && compl_get_longest)
	    fuzzy_longest_సరిపోలిక();
	తిరిగి;
    }

    ఒకవేళ (అంకె_matches > 0)
	ins_compl_కూడు_matches(అంకె_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    స్థిర శూన్య
get_తదుపరి_cmdline_completion(శూన్య)
{
    char_u	**matches;
    పూర్ణాంకం		అంకె_matches;

    ఒకవేళ (expand_cmdline(&compl_xp, compl_నమూనా.string,
		(పూర్ణాంకం)compl_నమూనా.నిడివి, &అంకె_matches, &matches) == EXPAND_OK)
    {
	పూర్ణాంకం		కూడు_r = OK;
	పూర్ణాంకం		dir = compl_direction;

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_matches && కూడు_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_లెక్క]) = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};

	    ఒకవేళ (compl_xp.xp_files_abbr != ఖాళీ)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    ఒకవేళ (compl_xp.xp_files_రకం != ఖాళీ)
		cptext[CPT_రకం] = compl_xp.xp_files_రకం[i];
	    ఒకవేళ (compl_xp.xp_files_menu != ఖాళీ)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    ఒకవేళ (compl_xp.xp_files_info != ఖాళీ)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    కూడు_r = ins_compl_కూడు(matches[i], -1, ఖాళీ, cptext, ఖాళీ, dir,
		    CP_FAST, FALSE, ఖాళీ, FUZZY_SCORE_NONE);
	    ఒకవేళ (కూడు_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(అంకె_matches, matches);
	free_xp_files_extra(&compl_xp, అంకె_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    స్థిర శూన్య
get_తదుపరి_spell_completion(linenr_T lnum వాడని)
{
#నిర్వచితమైతే FEAT_SPELL
    char_u	**matches;
    పూర్ణాంకం		అంకె_matches;

    అంకె_matches = expand_spelling(lnum, compl_నమూనా.string, &matches);
    ఒకవేళ (అంకె_matches > 0)
	ins_compl_కూడు_matches(అంకె_matches, matches, p_ic);
    లేకపోతే
	vim_free(matches);
#అయితే_ముగింపు
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    స్థిర char_u *
ins_compl_get_తదుపరి_మాట_or_పంక్తి(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_సరిపోలిక_pos,		// current match position
	పూర్ణాంకం	*సరిపోలిక_len,
	పూర్ణాంకం	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    పూర్ణాంకం		len;

    *సరిపోలిక_len = 0;
    ptr = ml_get_buf(ins_buf, cur_సరిపోలిక_pos->lnum, FALSE) +
	cur_సరిపోలిక_pos->నిలువు;
    len = (పూర్ణాంకం)ml_get_buf_len(ins_buf, cur_సరిపోలిక_pos->lnum) - cur_సరిపోలిక_pos->నిలువు;
    ఒకవేళ (ctrl_x_బహుళకం_పంక్తి_or_eval())
    {
	ఒకవేళ (compl_స్థితి_adding())
	{
	    ఒకవేళ (cur_సరిపోలిక_pos->lnum >= ins_buf->b_ml.ml_పంక్తి_లెక్క)
		తిరిగి ఖాళీ;
	    ptr = ml_get_buf(ins_buf, cur_సరిపోలిక_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_సరిపోలిక_pos->lnum + 1);
	    ఒకవేళ (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (పూర్ణాంకం)(ptr - tmp_ptr);
	    }
	}
    }
    లేకపోతే
    {
	char_u	*tmp_ptr = ptr;

	ఒకవేళ (compl_స్థితి_adding() && compl_నిడివి <= len)
	{
	    tmp_ptr += compl_నిడివి;
	    // Skip if already inside a word.
	    ఒకవేళ (vim_iswordp(tmp_ptr))
		తిరిగి ఖాళీ;
	    // Find start of next word.
	    tmp_ptr = కనుగొను_మాట_ప్రారంభం(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = కనుగొను_మాట_చివర(tmp_ptr);
	len = (పూర్ణాంకం)(tmp_ptr - ptr);

	ఒకవేళ (compl_స్థితి_adding() && len == compl_నిడివి)
	{
	    ఒకవేళ (cur_సరిపోలిక_pos->lnum < ins_buf->b_ml.ml_పంక్తి_లెక్క)
	    {
		// Try next line, if any. the new word will be
		// "join" as if the normal command "J" was used.
		// IOSIZE is always greater than
		// compl_length, so the next STRNCPY always
		// works -- Acevedo
		STRNCPY(IObuff, ptr, len);
		ptr = ml_get_buf(ins_buf, cur_సరిపోలిక_pos->lnum + 1, FALSE);
		tmp_ptr = ptr = skipwhite(ptr);
		// Find start of next word.
		tmp_ptr = కనుగొను_మాట_ప్రారంభం(tmp_ptr);
		// Find end of next word.
		tmp_ptr = కనుగొను_మాట_చివర(tmp_ptr);
		ఒకవేళ (tmp_ptr > ptr)
		{
		    ఒకవేళ (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			ఒకవేళ (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			ఒకవేళ (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ఖాళీ
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    ఒకవేళ (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (పూర్ణాంకం)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    ఒకవేళ (len == compl_నిడివి)
		తిరిగి ఖాళీ;
	}
    }

    *సరిపోలిక_len = len;
    తిరిగి ptr;
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
    స్థిర పూర్ణాంకం
get_తదుపరి_default_completion(ins_compl_తదుపరి_state_T *st, pos_T *ప్రారంభం_pos)
{
    పూర్ణాంకం		దొరికింది_new_సరిపోలిక = FAIL;
    పూర్ణాంకం		save_p_scs;
    పూర్ణాంకం		save_p_ws;
    పూర్ణాంకం		looped_around = FALSE;
    char_u	*ptr = ఖాళీ;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		లోపల_fuzzy_collect = !compl_స్థితి_adding()
			&& cot_fuzzy() && compl_నిడివి > 0;
    char_u	*leader = ins_compl_leader();
    పూర్ణాంకం		score = FUZZY_SCORE_NONE;
    పూర్ణాంకం		లోపల_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ఒకవేళ (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    ఒకవేళ (!లోపల_curbuf)
	p_ws = FALSE;
    లేకపోతే ఒకవేళ (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    కొరకు (;;)
    {
	పూర్ణాంకం	cont_s_ipos = FALSE;

	++సందేశం_silent;  // Don't want messages for wrapscan.

	ఒకవేళ (లోపల_fuzzy_collect)
	{
	    దొరికింది_new_సరిపోలిక = శోధన_for_fuzzy_సరిపోలిక(st->ins_buf,
			    st->cur_సరిపోలిక_pos, leader, compl_direction,
			    ప్రారంభం_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_సంపూర్ణ_పంక్తి() || ctrl_x_బహుళకం_eval() || (compl_cont_స్థితి & CONT_SOL))
	    దొరికింది_new_సరిపోలిక = శోధన_for_exact_పంక్తి(st->ins_buf,
			    st->cur_సరిపోలిక_pos, compl_direction, compl_నమూనా.string);
	లేకపోతే
	    దొరికింది_new_సరిపోలిక = searchit(ఖాళీ, st->ins_buf, st->cur_సరిపోలిక_pos,
				ఖాళీ, compl_direction, compl_నమూనా.string, (పూర్ణాంకం)compl_నమూనా.నిడివి,
				1L, శోధన_KEEP + శోధన_NFMSG, RE_చివరి, ఖాళీ);
	--సందేశం_silent;
	ఒకవేళ (!compl_started || st->సమితి_సరిపోలిక_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->మొదటి_సరిపోలిక_pos = *st->cur_సరిపోలిక_pos;
	    st->చివరి_సరిపోలిక_pos = *st->cur_సరిపోలిక_pos;
	    st->సమితి_సరిపోలిక_pos = FALSE;
	}
	లేకపోతే ఒకవేళ (st->మొదటి_సరిపోలిక_pos.lnum == st->చివరి_సరిపోలిక_pos.lnum
		&& st->మొదటి_సరిపోలిక_pos.నిలువు == st->చివరి_సరిపోలిక_pos.నిలువు)
	{
	    దొరికింది_new_సరిపోలిక = FAIL;
	}
	లేకపోతే ఒకవేళ (compl_dir_forward()
		&& (st->prev_సరిపోలిక_pos.lnum > st->cur_సరిపోలిక_pos->lnum
		    || (st->prev_సరిపోలిక_pos.lnum == st->cur_సరిపోలిక_pos->lnum
			&& st->prev_సరిపోలిక_pos.నిలువు >= st->cur_సరిపోలిక_pos->నిలువు)))
	{
	    ఒకవేళ (looped_around)
		దొరికింది_new_సరిపోలిక = FAIL;
	    లేకపోతే
		looped_around = TRUE;
	}
	లేకపోతే ఒకవేళ (!compl_dir_forward()
		&& (st->prev_సరిపోలిక_pos.lnum < st->cur_సరిపోలిక_pos->lnum
		    || (st->prev_సరిపోలిక_pos.lnum == st->cur_సరిపోలిక_pos->lnum
			&& st->prev_సరిపోలిక_pos.నిలువు <= st->cur_సరిపోలిక_pos->నిలువు)))
	{
	    ఒకవేళ (looped_around)
		దొరికింది_new_సరిపోలిక = FAIL;
	    లేకపోతే
		looped_around = TRUE;
	}
	st->prev_సరిపోలిక_pos = *st->cur_సరిపోలిక_pos;
	ఒకవేళ (దొరికింది_new_సరిపోలిక == FAIL)
	    ఆపు;

	// when ADDING, the text before the cursor matches, skip it
	ఒకవేళ (compl_స్థితి_adding() && లోపల_curbuf
		&& ప్రారంభం_pos->lnum == st->cur_సరిపోలిక_pos->lnum
		&& ప్రారంభం_pos->నిలువు  == st->cur_సరిపోలిక_pos->నిలువు)
	    కొనసాగు;

	ఒకవేళ (!లోపల_fuzzy_collect)
	    ptr = ins_compl_get_తదుపరి_మాట_or_పంక్తి(st->ins_buf,
		    st->cur_సరిపోలిక_pos, &len, &cont_s_ipos);
	ఒకవేళ (ptr == ఖాళీ || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    కొనసాగు;

	ఒకవేళ (is_సమీప_చురుకైన() && లోపల_curbuf)
	{
	    score = st->cur_సరిపోలిక_pos->lnum - curwin->w_cursor.lnum;
	    ఒకవేళ (score < 0)
		score = -score;
	}

	ఒకవేళ (ins_compl_కూడు_infercase(ptr, len, p_ic,
			లోపల_curbuf ? ఖాళీ : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    ఒకవేళ (లోపల_fuzzy_collect && score == compl_మొదటి_సరిపోలిక->cp_తదుపరి->cp_score)
		compl_అంకె_bests++;
	    దొరికింది_new_సరిపోలిక = OK;
	    ఆపు;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    తిరిగి దొరికింది_new_సరిపోలిక;
}

#నిర్వచితమైతే FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    స్థిర callback_T *
get_callback_if_cpt_func(char_u *p, పూర్ణాంకం idx)
{
    ఒకవేళ (*p == 'o')
	తిరిగి &curbuf->b_ofu_cb;

    ఒకవేళ (*p == 'F')
    {
	ఒకవేళ (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    ఒకవేళ (curbuf->b_p_cpt_cb == ఖాళీ
		    || idx < 0 || idx >= curbuf->b_p_cpt_లెక్క)
		తిరిగి ఖాళీ;
	    తిరిగి curbuf->b_p_cpt_cb[idx].cb_name != ఖాళీ
		? &curbuf->b_p_cpt_cb[idx] : ఖాళీ;
	}
	లేకపోతే
	    తిరిగి &curbuf->b_cfu_cb; // 'cfu'
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    స్థిర శూన్య
get_రిజిస్టరు_completion(శూన్య)
{
    పూర్ణాంకం		dir = compl_direction;
    yankreg_T	*reg = ఖాళీ;
    శూన్య	*reg_ptr = ఖాళీ;
    పూర్ణాంకం		adding_బహుళకం = compl_స్థితి_adding();

    కొరకు (పూర్ణాంకం i = 0; i < అంకె_REGISTERS; i++)
    {
	పూర్ణాంకం regname = 0;
	ఒకవేళ (i == 0)
	    regname = '"';    // unnamed register
	లేకపోతే ఒకవేళ (i < 10)
	    regname = '0' + i;
	లేకపోతే ఒకవేళ (i == DELETION_రిజిస్టరు)
	    regname = '-';
#నిర్వచితమైతే FEAT_CLIPBOARD
	లేకపోతే ఒకవేళ (i == నక్షత్రం_రిజిస్టరు)
	    regname = '*';
	లేకపోతే ఒకవేళ (i == PLUS_రిజిస్టరు)
	    regname = '+';
#అయితే_ముగింపు
	లేకపోతే
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	ఒకవేళ (!valid_yank_reg(regname, FALSE) || regname == '_')
	    కొనసాగు;

	reg_ptr = get_రిజిస్టరు(regname, FALSE);
	ఒకవేళ (reg_ptr == ఖాళీ)
	    కొనసాగు;

	reg = (yankreg_T *)reg_ptr;

	కొరకు (పూర్ణాంకం j = 0; j < reg->y_కొలత; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    ఒకవేళ (str == ఖాళీ)
		కొనసాగు;

	    ఒకవేళ (adding_బహుళకం)
	    {
		పూర్ణాంకం str_len = (పూర్ణాంకం)STRLEN(str);
		ఒకవేళ (str_len == 0)
		    కొనసాగు;

		ఒకవేళ (!compl_orig_పాఠ్యం.string
			|| (p_ic ? STRNICMP(str, compl_orig_పాఠ్యం.string,
					    compl_orig_పాఠ్యం.నిడివి) == 0
				: STRNCMP(str, compl_orig_పాఠ్యం.string,
					    compl_orig_పాఠ్యం.నిడివి) == 0))
		{
		    ఒకవేళ (ins_compl_కూడు_infercase(str, str_len, p_ic, ఖాళీ,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    లేకపోతే
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_చివర = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		అయితే (p < str_చివర && *p != NUL)
		{
		    char_u *old_p = p;
		    p = కనుగొను_మాట_ప్రారంభం(p);
		    ఒకవేళ (p >= str_చివర || *p == NUL)
			ఆపు;

		    char_u *మాట_చివర = కనుగొను_మాట_చివర(p);

		    ఒకవేళ (మాట_చివర <= p)
		    {
			ఒకవేళ (has_mbyte)
			    మాట_చివర = p + (*mb_ptr2len)(p);
			లేకపోతే
			    మాట_చివర = p + 1;
		    }

		    ఒకవేళ (మాట_చివర > str_చివర)
			మాట_చివర = str_చివర;

		    పూర్ణాంకం len = (పూర్ణాంకం)(మాట_చివర - p);
		    ఒకవేళ (len > 0 && (!compl_orig_పాఠ్యం.string
				|| (p_ic ? STRNICMP(p, compl_orig_పాఠ్యం.string,
						    compl_orig_పాఠ్యం.నిడివి) == 0
					: STRNCMP(p, compl_orig_పాఠ్యం.string,
						    compl_orig_పాఠ్యం.నిడివి) == 0)))
		    {
			ఒకవేళ (ins_compl_కూడు_infercase(p, len, p_ic, ఖాళీ,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = మాట_చివర;

		    ఒకవేళ (p <= old_p)
		    {
			p = old_p + 1;
			ఒకవేళ (has_mbyte && p < str_చివర)
			    p = old_p + (*mb_ptr2len)(old_p);
		    }
		}
	    }
	}

	// Free the register copy
	put_రిజిస్టరు(regname, reg_ptr);
    }
}

/*
 * get the next set of completion matches for "type".
 * Returns TRUE if a new match is found. Otherwise returns FALSE.
 */
    స్థిర పూర్ణాంకం
get_తదుపరి_completion_సరిపోలిక(పూర్ణాంకం type, ins_compl_తదుపరి_state_T *st, pos_T *ini)
{
    పూర్ణాంకం	దొరికింది_new_సరిపోలిక = FALSE;

    ఎంపిక (type)
    {
	స్థితి -1:
	    ఆపు;
#నిర్వచితమైతే FEAT_కనుగొను_ID
	స్థితి CTRL_X_మార్గం_PATTERNS:
	స్థితి CTRL_X_మార్గం_నిర్వచిస్తుంది:
	    get_తదుపరి_include_file_completion(type);
	    ఆపు;
#అయితే_ముగింపు

	స్థితి CTRL_X_DICTIONARY:
	స్థితి CTRL_X_THESAURUS:
	    get_తదుపరి_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ఖాళీ;
	    ఆపు;

	స్థితి CTRL_X_ట్యాగులు:
	    get_తదుపరి_ట్యాగ్_completion();
	    ఆపు;

	స్థితి CTRL_X_FILES:
	    get_తదుపరి_filename_completion();
	    ఆపు;

	స్థితి CTRL_X_CMDLINE:
	స్థితి CTRL_X_CMDLINE_CTRL_X:
	    get_తదుపరి_cmdline_completion();
	    ఆపు;

#నిర్వచితమైతే FEAT_COMPL_FUNC
	స్థితి CTRL_X_FUNCTION:
	    ఒకవేళ (ctrl_x_బహుళకం_సాధారణ())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    లేకపోతే
		expand_by_function(type, compl_నమూనా.string, ఖాళీ);
	    ఆపు;
	స్థితి CTRL_X_OMNI:
	    expand_by_function(type, compl_నమూనా.string, ఖాళీ);
	    ఆపు;
#అయితే_ముగింపు

	స్థితి CTRL_X_SPELL:
	    get_తదుపరి_spell_completion(st->మొదటి_సరిపోలిక_pos.lnum);
	    ఆపు;

	స్థితి CTRL_X_రిజిస్టరు:
	    get_రిజిస్టరు_completion();
	    ఆపు;

	అప్రమేయ:	// normal ^P/^N and ^X^L
	    దొరికింది_new_సరిపోలిక = get_తదుపరి_default_completion(st, ini);
	    ఒకవేళ (దొరికింది_new_సరిపోలిక == FAIL && st->ins_buf == curbuf)
		st->దొరికింది_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    ఒకవేళ (type != 0 && compl_curr_సరిపోలిక != compl_old_సరిపోలిక)
	దొరికింది_new_సరిపోలిక = OK;

    తిరిగి దొరికింది_new_సరిపోలిక;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    స్థిర శూన్య
strip_caret_numbers_లోపల_ఉంచు(char_u *str)
{
    char_u  *పఠ = str, *లిఖ = str, *p;

    ఒకవేళ (str == ఖాళీ)
	తిరిగి;

    అయితే (*పఠ)
    {
	ఒకవేళ (*పఠ == '^')
	{
	    p = పఠ + 1;
	    అయితే (vim_isdigit(*p))
		p++;
	    ఒకవేళ ((*p == ',' || *p == '\0') && p != పఠ + 1)
	    {
		పఠ = p;
		కొనసాగు;
	    }
	    లేకపోతే
		*లిఖ++ = *పఠ++;
	}
	లేకపోతే
	    *లిఖ++ = *పఠ++;
    }
    *లిఖ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    స్థిర పూర్ణాంకం
prepare_cpt_compl_funcs(శూన్య)
{
#నిర్వచితమైతే FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ఖాళీ;
    పూర్ణాంకం		idx = 0;
    పూర్ణాంకం		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    ఒకవేళ (cpt == ఖాళీ)
	తిరిగి FAIL;
    strip_caret_numbers_లోపల_ఉంచు(cpt);

    కొరకు (p = cpt; *p;)
    {
	అయితే (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ఒకవేళ (*p == NUL)
	    ఆపు;

	cb = get_callback_if_cpt_func(p, idx);
	ఒకవేళ (cb)
	{
	    ఒకవేళ (get_userdefined_compl_info(curwin->w_cursor.నిలువు, cb, &startcol)
		    == FAIL)
	    {
		ఒకవేళ (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		లేకపోతే
		    startcol = -2;
	    }
	    లేకపోతే ఒకవేళ (startcol < 0 || startcol > curwin->w_cursor.నిలువు)
		startcol = curwin->w_cursor.నిలువు;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	లేకపోతే
	    cpt_sources_array[idx].cs_startcol = -3;

	(శూన్య)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    తిరిగి OK;
#అయితే_ముగింపు
    తిరిగి FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    స్థిర శూన్య
compl_ఆధారం_ప్రారంభం_timer(పూర్ణాంకం ఆధారం_idx వాడని)
{
#నిర్వచితమైతే ELAPSED_FUNC
    ఒకవేళ (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[ఆధారం_idx].compl_ప్రారంభం_tv);
	compl_సమయం_ముక్క_expired = FALSE;
    }
#అయితే_ముగింపు
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    స్థిర పూర్ణాంకం
advance_cpt_sources_సూచీ_సురక్షిత(శూన్య)
{
    ఒకవేళ (cpt_sources_సూచీ >= 0 && cpt_sources_సూచీ < cpt_sources_లెక్క - 1)
    {
	cpt_sources_సూచీ++;
	తిరిగి OK;
    }
#నిర్వచితమైతే FEAT_EVAL
    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), cpt_sources_సూచీ);
#అయితే_ముగింపు
    తిరిగి FAIL;
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
    స్థిర పూర్ణాంకం
ins_compl_get_exp(pos_T *ini)
{
    స్థిర ins_compl_తదుపరి_state_T   st;
    స్థిర పూర్ణాంకం			    st_cleared = FALSE;
    పూర్ణాంకం		సరిపోలిక_లెక్క;
    పూర్ణాంకం		దొరికింది_new_సరిపోలిక;
    పూర్ణాంకం		type = ctrl_x_బహుళకం;
    పూర్ణాంకం		may_advance_cpt_idx = FALSE;
    pos_T	ప్రారంభం_pos = *ini;
    పూర్ణాంకం		సాధారణ_బహుళకం_strict = FALSE;
    దీర్ఘ	compl_timeout_save = 0;

    ఒకవేళ (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	ఒకవేళ (!st_cleared)
	{
	    CLEAR_క్షేత్రం(st);
	    st_cleared = TRUE;
	}
	st.దొరికింది_all = FALSE;
	st.ins_buf = curbuf;
	vim_free(st.e_cpt_copy);
	// Make a copy of 'complete', in case the buffer is wiped out.
	st.e_cpt_copy = vim_strsave((compl_cont_స్థితి & CONT_LOCAL)
					    ? (char_u *)"." : curbuf->b_p_cpt);
	strip_caret_numbers_లోపల_ఉంచు(st.e_cpt_copy);
	st.e_cpt = st.e_cpt_copy == ఖాళీ ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#నిర్వచించు LOOKBACK_పంక్తి_లెక్క	1000
	ఒకవేళ (compl_autocomplete && is_సమీప_చురుకైన())
	{
	    ప్రారంభం_pos.lnum = MAX(1, ప్రారంభం_pos.lnum - LOOKBACK_పంక్తి_లెక్క);
	    ప్రారంభం_pos.నిలువు = 0;
	}
	st.చివరి_సరిపోలిక_pos = st.మొదటి_సరిపోలిక_pos = ప్రారంభం_pos;
    }
    లేకపోతే ఒకవేళ (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_సరిపోలిక = compl_curr_సరిపోలిక;	// remember the last current match
    st.cur_సరిపోలిక_pos = (compl_dir_forward())
				    ? &st.చివరి_సరిపోలిక_pos : &st.మొదటి_సరిపోలిక_pos;

    సాధారణ_బహుళకం_strict = ctrl_x_బహుళకం_సాధారణ() && !ctrl_x_బహుళకం_పంక్తి_or_eval()
	&& !(compl_cont_స్థితి & CONT_LOCAL) && cpt_sources_array != ఖాళీ;
    ఒకవేళ (సాధారణ_బహుళకం_strict)
    {
	cpt_sources_సూచీ = 0;
	ఒకవేళ (compl_autocomplete || p_cto > 0)
	{
	    compl_ఆధారం_ప్రారంభం_timer(0);
	    compl_సమయం_ముక్క_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    కొరకు (;;)
    {
	దొరికింది_new_సరిపోలిక = FAIL;
	st.సమితి_సరిపోలిక_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	ఒకవేళ ((ctrl_x_బహుళకం_సాధారణ() || ctrl_x_బహుళకం_పంక్తి_or_eval())
					&& (!compl_started || st.దొరికింది_all))
	{
	    పూర్ణాంకం స్థితి_2 = process_తదుపరి_cpt_విలువ(&st, &type, &ప్రారంభం_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    ఒకవేళ (స్థితి_2 == INS_COMPL_CPT_చివర)
		ఆపు;
	    ఒకవేళ (స్థితి_2 == INS_COMPL_CPT_CONT)
	    {
		ఒకవేళ (may_advance_cpt_idx)
		{
		    ఒకవేళ (!advance_cpt_sources_సూచీ_సురక్షిత())
			ఆపు;
		    compl_ఆధారం_ప్రారంభం_timer(cpt_sources_సూచీ);
		}
		కొనసాగు;
	    }
	}

	ఒకవేళ (సాధారణ_బహుళకం_strict && type == CTRL_X_FUNCTION
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
	దొరికింది_new_సరిపోలిక = get_తదుపరి_completion_సరిపోలిక(type, &st, &ప్రారంభం_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	    ఆపు;

	ఒకవేళ (may_advance_cpt_idx)
	{
	    ఒకవేళ (!advance_cpt_sources_సూచీ_సురక్షిత())
		ఆపు;
	    compl_ఆధారం_ప్రారంభం_timer(cpt_sources_సూచీ);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	//సరిపోలిక
	ఒకవేళ ((ctrl_x_బహుళకం_not_default() && !ctrl_x_బహుళకం_పంక్తి_or_eval())
						|| దొరికింది_new_సరిపోలిక != FAIL)
	{
	    ఒకవేళ (got_int)
		ఆపు;
	    // Fill the popup menu as soon as possible.
	    ఒకవేళ (type != -1)
		ins_compl_check_కీలకాలు(0, FALSE);

	    ఒకవేళ ((ctrl_x_బహుళకం_not_default()
			&& !ctrl_x_బహుళకం_పంక్తి_or_eval()) || compl_interrupted)
		ఆపు;
	    compl_started = compl_సమయం_ముక్క_expired ? FALSE : TRUE;
	}
	లేకపోతే
	{
	    // Mark a buffer scanned when it has been scanned completely
	    ఒకవేళ (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_మార్గం_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	ఒకవేళ (సాధారణ_బహుళకం_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	ఒకవేళ (!compl_dir_forward())
	    అయితే (compl_curr_సరిపోలిక->cp_prev
		    && !సరిపోలిక_at_original_పాఠ్యం(compl_curr_సరిపోలిక->cp_prev))
		compl_curr_సరిపోలిక = compl_curr_సరిపోలిక->cp_prev;
    }
    cpt_sources_సూచీ = -1;
    compl_started = TRUE;

    ఒకవేళ ((ctrl_x_బహుళకం_సాధారణ() || ctrl_x_బహుళకం_పంక్తి_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	దొరికింది_new_సరిపోలిక = FAIL;

    సరిపోలిక_లెక్క = -1;		// total of matches, unknown
    ఒకవేళ (దొరికింది_new_సరిపోలిక == FAIL || (ctrl_x_బహుళకం_not_default()
		&& !ctrl_x_బహుళకం_పంక్తి_or_eval()))
	సరిపోలిక_లెక్క = ins_compl_make_cyclic();

    ఒకవేళ (cot_fuzzy() && compl_get_longest && compl_అంకె_bests > 0)
	fuzzy_longest_సరిపోలిక();

    ఒకవేళ (compl_old_సరిపోలిక != ఖాళీ)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_సరిపోలిక = compl_dir_forward() ? compl_old_సరిపోలిక->cp_తదుపరి
						    : compl_old_సరిపోలిక->cp_prev;
	ఒకవేళ (compl_curr_సరిపోలిక == ఖాళీ)
	    compl_curr_సరిపోలిక = compl_old_సరిపోలిక;
    }
    may_trigger_modechanged();

    ఒకవేళ (సరిపోలిక_లెక్క > 0 && !ctrl_x_బహుళకం_spell())
    {
	ఒకవేళ (is_సమీప_చురుకైన() && !ins_compl_has_preinsert())
	    క్రమీకరించు_compl_సరిపోలిక_list(cp_compare_సమీప);

	ఒకవేళ (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_క్రమీకరించు();
    }

    తిరిగి సరిపోలిక_లెక్క;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    స్థిర శూన్య
ins_compl_update_shown_సరిపోలిక(శూన్య)
{
    string_T	*leader;

    (శూన్య)get_leader_for_startcol(ఖాళీ, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_సరిపోలిక, TRUE);

    అయితే (!ins_compl_equal(compl_shown_సరిపోలిక,
		leader->string, (పూర్ణాంకం)leader->నిడివి)
	    && compl_shown_సరిపోలిక->cp_తదుపరి != ఖాళీ
	    && !is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక->cp_తదుపరి))
    {
	compl_shown_సరిపోలిక = compl_shown_సరిపోలిక->cp_తదుపరి;
	leader = get_leader_for_startcol(compl_shown_సరిపోలిక, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    ఒకవేళ (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_సరిపోలిక,
		leader->string, (పూర్ణాంకం)leader->నిడివి)
	    && (compl_shown_సరిపోలిక->cp_తదుపరి == ఖాళీ
		|| is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక->cp_తదుపరి)))
    {
	అయితే (!ins_compl_equal(compl_shown_సరిపోలిక,
		    leader->string, (పూర్ణాంకం)leader->నిడివి)
		&& compl_shown_సరిపోలిక->cp_prev != ఖాళీ
		&& !is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక->cp_prev))
	{
	    compl_shown_సరిపోలిక = compl_shown_సరిపోలిక->cp_prev;
	    leader = get_leader_for_startcol(compl_shown_సరిపోలిక, TRUE);
	}
    }
}

/*
 * Delete the old text being completed.
 */
    శూన్య
ins_compl_delete(శూన్య)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    పూర్ణాంకం నిలువు = compl_నిలువు + (compl_స్థితి_adding() ? compl_నిడివి : 0);
    string_T	మిగిలిన = {ఖాళీ, 0};
    పూర్ణాంకం	    orig_నిలువు;

    ఒకవేళ (ins_compl_preinsert_effect())
    {
	నిలువు += (పూర్ణాంకం)ins_compl_leader_len();
	curwin->w_cursor.నిలువు = compl_ins_చివర_నిలువు;
    }

    ఒకవేళ (curwin->w_cursor.lnum > compl_lnum)
    {
	ఒకవేళ (curwin->w_cursor.నిలువు < ml_get_curline_len())
	{
	    char_u *పంక్తి = ml_get_cursor();
	    మిగిలిన.నిడివి = ml_get_cursor_len();
	    మిగిలిన.string = vim_strnsave(పంక్తి, మిగిలిన.నిడివి);
	    ఒకవేళ (మిగిలిన.string == ఖాళీ)
		తిరిగి;
	}
	అయితే (curwin->w_cursor.lnum > compl_lnum)
	{
	    ఒకవేళ (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		ఒకవేళ (మిగిలిన.string)
		    vim_free(మిగిలిన.string);
		తిరిగి;
	    }
	    deleted_పంక్తులు_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.నిలువు = ml_get_curline_len();
    }

    ఒకవేళ ((పూర్ణాంకం)curwin->w_cursor.నిలువు > నిలువు)
    {
	ఒకవేళ (stop_arrow() == FAIL)
	{
	    ఒకవేళ (మిగిలిన.string)
		vim_free(మిగిలిన.string);
	    తిరిగి;
	}
	backspace_until_స్తంభం(నిలువు);
	compl_ins_చివర_నిలువు = curwin->w_cursor.నిలువు;
    }

    ఒకవేళ (మిగిలిన.string != ఖాళీ)
    {
	orig_నిలువు = curwin->w_cursor.నిలువు;
	ins_str(మిగిలిన.string, మిగిలిన.నిడివి);
	curwin->w_cursor.నిలువు = orig_నిలువు;
	vim_free(మిగిలిన.string);
    }
    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    మారింది_cline_bef_curs();
#నిర్వచితమైతే FEAT_EVAL
    // clear v:completed_item
    సమితి_vim_var_dict(VV_COMPLETED_అంశం, dict_alloc_తాళం(VAR_FIXED));
#అయితే_ముగింపు
}

/*
 * Insert a completion string that contains newlines.
 * The string is split and inserted line by line.
 */
    స్థిర శూన్య
ins_compl_expand_గుణిజం(char_u *str)
{
    char_u	*ప్రారంభం = str;
    char_u	*curr = str;
    పూర్ణాంకం		పునాది_indent = get_indent();

    అయితే (*curr != NUL)
    {
	ఒకవేళ (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    ఒకవేళ (curr > ప్రారంభం)
		ins_char_బైట్లు(ప్రారంభం, (పూర్ణాంకం)(curr - ప్రారంభం));

	    // Handle newline
	    open_పంక్తి(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, పునాది_indent, ఖాళీ);
	    ప్రారంభం = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    ఒకవేళ (curr > ప్రారంభం)
	ins_char_బైట్లు(ప్రారంభం, (పూర్ణాంకం)(curr - ప్రారంభం));

    compl_ins_చివర_నిలువు = curwin->w_cursor.నిలువు;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    స్థిర char_u *
కనుగొను_common_ఉపసర్గ(size_t *ఉపసర్గ_len, పూర్ణాంకం curbuf_only)
{
    compl_T     *compl;
    పూర్ణాంకం		*సరిపోలిక_లెక్క;
    char_u	*మొదటి = ఖాళీ;
    పూర్ణాంకం		len = -1;
    పూర్ణాంకం		is_cpt_completion = (cpt_sources_array != ఖాళీ);

    ఒకవేళ (!is_cpt_completion)
	తిరిగి ఖాళీ;

    సరిపోలిక_లెక్క = ALLOC_CLEAR_MULT(పూర్ణాంకం, cpt_sources_లెక్క);
    ఒకవేళ (సరిపోలిక_లెక్క == ఖాళీ)
	తిరిగి ఖాళీ;

    (శూన్య)get_leader_for_startcol(ఖాళీ, TRUE); // Clear the cache

    compl = compl_మొదటి_సరిపోలిక;
    చేయి
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	ఒకవేళ (ctrl_x_బహుళకం_సాధారణ() && !p_inf && compl_orig_పాఠ్యం.string
		&& !ignorecase(compl_orig_పాఠ్యం.string))
	    compl->cp_flags &= ~CP_ICASE;

	ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(compl)
		&& (leader->string == ఖాళీ
		    || ins_compl_equal_sc(compl, leader->string,
			(పూర్ణాంకం)leader->నిడివి)))
	{
	    // Limit number of items from each source if max_items is set.
	    పూర్ణాంకం సరిపోలిక_హద్దు_exceeded = FALSE;
	    పూర్ణాంకం cur_ఆధారం = compl->cp_cpt_ఆధారం_idx;

	    ఒకవేళ (cur_ఆధారం != -1)
	    {
		సరిపోలిక_లెక్క[cur_ఆధారం]++;
		పూర్ణాంకం max_matches = cpt_sources_array[cur_ఆధారం].cs_max_matches;
		ఒకవేళ (max_matches > 0 && సరిపోలిక_లెక్క[cur_ఆధారం] > max_matches)
		    సరిపోలిక_హద్దు_exceeded = TRUE;
	    }

	    ఒకవేళ (!సరిపోలిక_హద్దు_exceeded && (!curbuf_only
			|| (cur_ఆధారం != -1
			    && cpt_sources_array[cur_ఆధారం].cs_flag == '.')))
	    {
		ఒకవేళ (మొదటి == ఖాళీ && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    మొదటి = compl->cp_str.string;
		    len = (పూర్ణాంకం)STRLEN(మొదటి);
		}
		లేకపోతే ఒకవేళ (మొదటి != ఖాళీ)
		{
		    పూర్ణాంకం j = 0;  // count in bytes
		    char_u *s1 = మొదటి;
		    char_u *s2 = compl->cp_str.string;

		    అయితే (j < len && *s1 != NUL && *s2 != NUL)
		    {
			ఒకవేళ (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    ఆపు;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    ఒకవేళ (len == 0)
			ఆపు;
		}
	    }
	}
	compl = compl->cp_తదుపరి;
    } అయితే (compl != ఖాళీ && !is_మొదటి_సరిపోలిక(compl));

    vim_free(సరిపోలిక_లెక్క);

    ఒకవేళ (len > (పూర్ణాంకం)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	ఒకవేళ (len == (పూర్ణాంకం)STRLEN(మొదటి))
	{
	    char_u *పంక్తి = ml_get_curline();
	    char_u *p = పంక్తి + curwin->w_cursor.నిలువు;
	    ఒకవేళ (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *చివర = కనుగొను_మాట_చివర(p);
		పూర్ణాంకం పాఠ్యం_len = చివర - p;
		ఒకవేళ (పాఠ్యం_len > 0
			&& పాఠ్యం_len < (len - (పూర్ణాంకం)ins_compl_leader_len())
			&& STRNCMP(మొదటి + len - పాఠ్యం_len, p, పాఠ్యం_len) == 0)
		    len -= పాఠ్యం_len;
	    }
	}
	*ఉపసర్గ_len = (size_t)len;
	తిరిగి మొదటి;
    }
    తిరిగి ఖాళీ;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    శూన్య
ins_compl_చొప్పించు(పూర్ణాంకం move_cursor, పూర్ణాంకం చొప్పించు_ఉపసర్గ)
{
    పూర్ణాంకం		compl_len = get_compl_len();
    పూర్ణాంకం		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_సరిపోలిక->cp_str.string;
    size_t	cp_str_len = compl_shown_సరిపోలిక->cp_str.నిడివి;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_గుణిజం = vim_strchr(cp_str, '\n');

    ఒకవేళ (చొప్పించు_ఉపసర్గ)
    {
	cp_str = కనుగొను_common_ఉపసర్గ(&cp_str_len, FALSE);
	ఒకవేళ (cp_str == ఖాళీ)
	{
	    cp_str = కనుగొను_common_ఉపసర్గ(&cp_str_len, TRUE);
	    ఒకవేళ (cp_str == ఖాళీ)
	    {
		cp_str = compl_shown_సరిపోలిక->cp_str.string;
		cp_str_len = compl_shown_సరిపోలిక->cp_str.నిడివి;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    లేకపోతే ఒకవేళ (cpt_sources_array != ఖాళీ)
    {
	పూర్ణాంకం	cpt_idx = compl_shown_సరిపోలిక->cp_cpt_ఆధారం_idx;
	ఒకవేళ (cpt_idx >= 0 && compl_నిలువు >= 0)
	{
	    పూర్ణాంకం	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    ఒకవేళ (startcol >= 0 && startcol < (పూర్ణాంకం)compl_నిలువు)
	    {
		పూర్ణాంకం skip = (పూర్ణాంకం)compl_నిలువు - startcol;
		ఒకవేళ ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    ఒకవేళ (compl_len < (పూర్ణాంకం)cp_str_len)
    {
	ఒకవేళ (has_గుణిజం)
	    ins_compl_expand_గుణిజం(cp_str + compl_len);
	లేకపోతే
	{
	    ins_compl_చొప్పించు_బైట్లు(cp_str + compl_len,
		    చొప్పించు_ఉపసర్గ ? (పూర్ణాంకం)cp_str_len - compl_len : -1);
	    ఒకవేళ ((preinsert || చొప్పించు_ఉపసర్గ) && move_cursor)
		curwin->w_cursor.నిలువు -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    ఒకవేళ (సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక)
	    || (preinsert && !చొప్పించు_ఉపసర్గ))
	compl_వాడిన_సరిపోలిక = FALSE;
    లేకపోతే
	compl_వాడిన_సరిపోలిక = TRUE;
#నిర్వచితమైతే FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_సరిపోలిక);

	సమితి_vim_var_dict(VV_COMPLETED_అంశం, dict);
    }
#అయితే_ముగింపు
    compl_hi_on_autocompl_longest = చొప్పించు_ఉపసర్గ && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    స్థిర శూన్య
ins_compl_show_filename(శూన్య)
{
    అక్షరం	*lead = _("match in file");
    పూర్ణాంకం		space = sc_నిలువు - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    ఒకవేళ (space <= 0)
	తిరిగి;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    కొరకు (s = e = compl_shown_సరిపోలిక->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	అయితే (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    ఒకవేళ (!compl_autocomplete)
    {
	సందేశం_hist_off = TRUE;
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_సరిపోలిక->cp_fname ? "<" : "", s);
	సందేశం((అక్షరం *)IObuff);
	సందేశం_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    స్థిర compl_T *
కనుగొను_తదుపరి_సరిపోలిక_లోపల_menu(శూన్య)
{
    పూర్ణాంకం	    is_forward = compl_shows_dir_forward();
    compl_T *సరిపోలిక = compl_shown_సరిపోలిక;

    చేయి
	సరిపోలిక = is_forward ? సరిపోలిక->cp_తదుపరి : సరిపోలిక->cp_prev;
    అయితే (సరిపోలిక->cp_తదుపరి && !సరిపోలిక->cp_లోపల_సరిపోలిక_array
	    && !సరిపోలిక_at_original_పాఠ్యం(సరిపోలిక));
    తిరిగి సరిపోలిక;
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
    స్థిర పూర్ణాంకం
కనుగొను_తదుపరి_completion_సరిపోలిక(
	పూర్ణాంకం	allow_get_expansion,
	పూర్ణాంకం	todo,		// repeat completion this many times
	పూర్ణాంకం	advance,
	పూర్ణాంకం	*అంకె_matches)
{
    పూర్ణాంకం		దొరికింది_చివర = FALSE;
    compl_T	*దొరికింది_compl = ఖాళీ;
    గుర్తులేని పూర్ణాంకం cur_cot_flags = get_cot_flags();
    పూర్ణాంకం		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    అయితే (--todo >= 0)
    {
	ఒకవేళ (compl_shows_dir_forward() && compl_shown_సరిపోలిక->cp_తదుపరి != ఖాళీ)
	{
	    ఒకవేళ (compl_సరిపోలిక_array != ఖాళీ)
		compl_shown_సరిపోలిక = కనుగొను_తదుపరి_సరిపోలిక_లోపల_menu();
	    లేకపోతే
		compl_shown_సరిపోలిక = compl_shown_సరిపోలిక->cp_తదుపరి;
	    దొరికింది_చివర = (compl_మొదటి_సరిపోలిక != ఖాళీ
		    && (is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక->cp_తదుపరి)
			|| is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక)));
	}
	లేకపోతే ఒకవేళ (compl_shows_dir_backward()
		&& compl_shown_సరిపోలిక->cp_prev != ఖాళీ)
	{
	    దొరికింది_చివర = is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక);
	    ఒకవేళ (compl_సరిపోలిక_array != ఖాళీ)
		compl_shown_సరిపోలిక = కనుగొను_తదుపరి_సరిపోలిక_లోపల_menu();
	    లేకపోతే
		compl_shown_సరిపోలిక = compl_shown_సరిపోలిక->cp_prev;
	    దొరికింది_చివర |= is_మొదటి_సరిపోలిక(compl_shown_సరిపోలిక);
	}
	లేకపోతే
	{
	    ఒకవేళ (!allow_get_expansion)
	    {
		ఒకవేళ (advance)
		{
		    ఒకవేళ (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    లేకపోతే
			compl_pending += todo + 1;
		}
		తిరిగి -1;
	    }

	    ఒకవేళ (!compl_no_select && advance)
	    {
		ఒకవేళ (compl_shows_dir_backward())
		    --compl_pending;
		లేకపోతే
		    ++compl_pending;
	    }

	    // Find matches.
	    *అంకె_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    అయితే (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		ఒకవేళ (compl_pending > 0 && compl_shown_సరిపోలిక->cp_తదుపరి != ఖాళీ)
		{
		    compl_shown_సరిపోలిక = compl_shown_సరిపోలిక->cp_తదుపరి;
		    --compl_pending;
		}
		లేకపోతే ఒకవేళ (compl_pending < 0 && compl_shown_సరిపోలిక->cp_prev != ఖాళీ)
		{
		    compl_shown_సరిపోలిక = compl_shown_సరిపోలిక->cp_prev;
		    ++compl_pending;
		}
		లేకపోతే
		    ఆపు;
	    }
	    దొరికింది_చివర = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_సరిపోలిక, FALSE);

	ఒకవేళ (!సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక)
		&& leader->string != ఖాళీ
		&& !ins_compl_equal(compl_shown_సరిపోలిక,
		    leader->string, (పూర్ణాంకం)leader->నిడివి)
		&& !(cot_fuzzy()
		    && compl_shown_సరిపోలిక->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	లేకపోతే
	    // Remember a matching item.
	    దొరికింది_compl = compl_shown_సరిపోలిక;

	// Stop at the end of the list when we found a usable match.
	ఒకవేళ (దొరికింది_చివర)
	{
	    ఒకవేళ (దొరికింది_compl != ఖాళీ)
	    {
		compl_shown_సరిపోలిక = దొరికింది_compl;
		ఆపు;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
ins_compl_తదుపరి(
    పూర్ణాంకం	    allow_get_expansion,
    పూర్ణాంకం	    లెక్క,		// repeat completion this many times; should
				// be at least 1
    పూర్ణాంకం	    చొప్పించు_సరిపోలిక)	// Insert the newly selected match
{
    పూర్ణాంకం	    అంకె_matches = -1;
    పూర్ణాంకం	    todo = లెక్క;
    పూర్ణాంకం	    advance;
    పూర్ణాంకం	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    గుర్తులేని పూర్ణాంకం cur_cot_flags = get_cot_flags();
    పూర్ణాంకం	    compl_no_చొప్పించు = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    పూర్ణాంకం	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    ఒకవేళ (compl_shown_సరిపోలిక == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (compl_leader.string != ఖాళీ
	    && !సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_సరిపోలిక();

    ఒకవేళ (allow_get_expansion && చొప్పించు_సరిపోలిక
	    && (!compl_get_longest || compl_వాడిన_సరిపోలిక))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = లెక్క != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    ఒకవేళ (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    ఒకవేళ (కనుగొను_తదుపరి_completion_సరిపోలిక(allow_get_expansion, todo, advance,
							&అంకె_matches) == -1)
	తిరిగి -1;

    ఒకవేళ (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	తిరిగి -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    ఒకవేళ (!started && ins_compl_preinsert_longest())
	ins_compl_చొప్పించు(TRUE, TRUE);
    లేకపోతే ఒకవేళ (compl_no_చొప్పించు && !started && !compl_preinsert)
    {
	ins_compl_చొప్పించు_బైట్లు(compl_orig_పాఠ్యం.string + get_compl_len(), -1);
	compl_వాడిన_సరిపోలిక = FALSE;
    }
    లేకపోతే ఒకవేళ (చొప్పించు_సరిపోలిక)
    {
	ఒకవేళ (!compl_get_longest || compl_వాడిన_సరిపోలిక)
	{
	    పూర్ణాంకం preinsert_longest = ins_compl_preinsert_longest()
		&& సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక); // none selected
	    ins_compl_చొప్పించు(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	లేకపోతే
	    ins_compl_చొప్పించు_బైట్లు(compl_leader.string + get_compl_len(), -1);
    }
    లేకపోతే
	compl_వాడిన_సరిపోలిక = FALSE;

    ఒకవేళ (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	ఒకవేళ (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	లేకపోతే
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    బహిర్గతం_flush_cursor(FALSE, FALSE);
	}
#అయితే_ముగింపు

	// Delete old text to be replaced, since we're still searching and
	// don't want to match ourselves!
	ins_compl_delete();
    }

    // Enter will select a match when the match wasn't inserted and the popup
    // menu is visible.
    ఒకవేళ (compl_no_చొప్పించు && !started
				 && !సరిపోలిక_at_original_పాఠ్యం(compl_shown_సరిపోలిక))
	compl_enter_selects = TRUE;
    లేకపోతే
	compl_enter_selects = !చొప్పించు_సరిపోలిక && compl_సరిపోలిక_array != ఖాళీ;

    // Show the file name for the match (if any)
    ఒకవేళ (compl_shown_సరిపోలిక->cp_fname != ఖాళీ)
	ins_compl_show_filename();

    తిరిగి అంకె_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    స్థిర శూన్య
check_elapsed_సమయం(శూన్య)
{
#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T	*ప్రారంభం_tv
	= &cpt_sources_array[cpt_sources_సూచీ].compl_ప్రారంభం_tv;
    దీర్ఘ	elapsed_ms = ELAPSED_FUNC(*ప్రారంభం_tv);

    ఒకవేళ (elapsed_ms > compl_timeout_ms)
    {
	compl_సమయం_ముక్క_expired = TRUE;
	DECAY_COMPL_TIMEOUT();
    }
#అయితే_ముగింపు
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
    శూన్య
ins_compl_check_కీలకాలు(పూర్ణాంకం frequency, పూర్ణాంకం లోపల_compl_func)
{
    స్థిర పూర్ణాంకం	లెక్క = 0;
    పూర్ణాంకం		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    ఒకవేళ (!లోపల_compl_func && (using_script() || ex_సాధారణ_busy))
	తిరిగి;

    // Only do this at regular intervals
    ఒకవేళ (++లెక్క < frequency)
	తిరిగి;
    లెక్క = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    ఒకవేళ (c != NUL
#నిర్వచితమైతే FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#అయితే_ముగింపు
       )
    {
	ఒకవేళ (vim_is_ctrl_x_కీలకం(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = సురక్షిత_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (శూన్య)ins_compl_తదుపరి(FALSE, ins_compl_key2count(c),
				      c != K_పైకి && c != K_కిందికి);
	}
	లేకపోతే
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = సురక్షిత_vgetc();
	    ఒకవేళ (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		ఒకవేళ (c != Ctrl_R && (కీలకం_Typed
			    || (లోపల_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం సాధారణ_బహుళకం_strict = ctrl_x_బహుళకం_సాధారణ()
	    && !ctrl_x_బహుళకం_పంక్తి_or_eval() && !(compl_cont_స్థితి & CONT_LOCAL)
	    && cpt_sources_array != ఖాళీ && cpt_sources_సూచీ >= 0;
	ఒకవేళ (సాధారణ_బహుళకం_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_సమయం();
    }

    ఒకవేళ (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	పూర్ణాంకం todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(శూన్య)ins_compl_తదుపరి(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    స్థిర పూర్ణాంకం
ins_compl_key2dir(పూర్ణాంకం c)
{
    ఒకవేళ (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_పైకి || c == K_పైకి)
	తిరిగి BACKWARD;
    తిరిగి FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    స్థిర పూర్ణాంకం
ins_compl_pum_కీలకం(పూర్ణాంకం c)
{
    తిరిగి pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_పైకి
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_కిందికి
		     || c == K_పైకి || c == K_కిందికి);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    స్థిర పూర్ణాంకం
ins_compl_key2count(పూర్ణాంకం c)
{
    ఒకవేళ (ins_compl_pum_కీలకం(c) && c != K_పైకి && c != K_కిందికి)
    {
	పూర్ణాంకం h = pum_get_ఎత్తు();
	ఒకవేళ (h > 3)
	    h -= 2; // keep some context
	తిరిగి h;
    }
    తిరిగి 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    స్థిర పూర్ణాంకం
ins_compl_use_సరిపోలిక(పూర్ణాంకం c)
{
    ఎంపిక (c)
    {
	స్థితి K_పైకి:
	స్థితి K_కిందికి:
	స్థితి K_PAGEDOWN:
	స్థితి K_KPAGEDOWN:
	స్థితి K_S_కిందికి:
	స్థితి K_PAGEUP:
	స్థితి K_KPAGEUP:
	స్థితి K_S_పైకి:
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    స్థిర పూర్ణాంకం
get_సాధారణ_compl_info(char_u *పంక్తి, పూర్ణాంకం startcol, colnr_T curs_నిలువు)
{
    ఒకవేళ ((compl_cont_స్థితి & CONT_SOL) || ctrl_x_బహుళకం_మార్గం_నిర్వచిస్తుంది())
    {
	ఒకవేళ (!compl_స్థితి_adding())
	{
	    అయితే (--startcol >= 0 && vim_isIDc(పంక్తి[startcol]))
		;
	    compl_నిలువు += ++startcol;
	    compl_నిడివి = curs_నిలువు - startcol;
	}
	ఒకవేళ (p_ic)
	{
	    compl_నమూనా.string = str_foldcase(పంక్తి + compl_నిలువు,
		    compl_నిడివి, ఖాళీ, 0);
	    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	    {
		compl_నమూనా.నిడివి = 0;
		తిరిగి FAIL;
	    }
	    compl_నమూనా.నిడివి = STRLEN(compl_నమూనా.string);
	}
	లేకపోతే
	{
	    compl_నమూనా.string = vim_strnsave(పంక్తి + compl_నిలువు, (size_t)compl_నిడివి);
	    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	    {
		compl_నమూనా.నిడివి = 0;
		తిరిగి FAIL;
	    }
	    compl_నమూనా.నిడివి = (size_t)compl_నిడివి;
	}
    }
    లేకపోతే ఒకవేళ (compl_స్థితి_adding())
    {
	char_u	    *ఉపసర్గ = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	ఒకవేళ (!vim_iswordp(పంక్తి + compl_నిలువు)
		|| (compl_నిలువు > 0
		    && (vim_iswordp(mb_prevptr(పంక్తి, పంక్తి + compl_నిలువు)))))
	{
	    ఉపసర్గ = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ఖాళీ, పంక్తి + compl_నిలువు, compl_నిడివి) + prefixlen;
	compl_నమూనా.string = alloc(n);
	ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	{
	    compl_నమూనా.నిడివి = 0;
	    తిరిగి FAIL;
	}
	STRCPY((అక్షరం *)compl_నమూనా.string, ఉపసర్గ);
	(శూన్య)quote_meta(compl_నమూనా.string + prefixlen,
		పంక్తి + compl_నిలువు, compl_నిడివి);
	compl_నమూనా.నిడివి = n - 1;
    }
    లేకపోతే ఒకవేళ (--startcol < 0
	    || !vim_iswordp(mb_prevptr(పంక్తి, పంక్తి + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_నమూనా.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	{
	    compl_నమూనా.నిడివి = 0;
	    తిరిగి FAIL;
	}
	compl_నమూనా.నిడివి = len;
	compl_నిలువు += curs_నిలువు;
	compl_నిడివి = 0;
	compl_from_nonkeyword = TRUE;
    }
    లేకపోతే
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం పునాది_class;
	    పూర్ణాంకం తల_off;

	    startcol -= (*mb_తల_off)(పంక్తి, పంక్తి + startcol);
	    పునాది_class = mb_get_class(పంక్తి + startcol);
	    అయితే (--startcol >= 0)
	    {
		తల_off = (*mb_తల_off)(పంక్తి, పంక్తి + startcol);
		ఒకవేళ (పునాది_class != mb_get_class(పంక్తి + startcol
			    - తల_off))
		    ఆపు;
		startcol -= తల_off;
	    }
	}
	లేకపోతే
	{
	    అయితే (--startcol >= 0 && vim_iswordc(పంక్తి[startcol]))
		;
	}

	compl_నిలువు += ++startcol;
	compl_నిడివి = (పూర్ణాంకం)curs_నిలువు - startcol;
	ఒకవేళ (compl_నిడివి == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_నమూనా.string = alloc(7);
	    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	    {
		compl_నమూనా.నిడివి = 0;
		తిరిగి FAIL;
	    }
	    STRCPY((అక్షరం *)compl_నమూనా.string, "\\<");
	    (శూన్య)quote_meta(compl_నమూనా.string + 2, పంక్తి + compl_నిలువు, 1);
	    STRCAT((అక్షరం *)compl_నమూనా.string, "\\k");
	    compl_నమూనా.నిడివి = STRLEN(compl_నమూనా.string);
	}
	లేకపోతే
	{
	    size_t  n = quote_meta(ఖాళీ, పంక్తి + compl_నిలువు, compl_నిడివి) + 2;

	    compl_నమూనా.string = alloc(n);
	    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	    {
		compl_నమూనా.నిడివి = 0;
		తిరిగి FAIL;
	    }
	    STRCPY((అక్షరం *)compl_నమూనా.string, "\\<");
	    (శూన్య)quote_meta(compl_నమూనా.string + 2, పంక్తి + compl_నిలువు,
		    compl_నిడివి);
	    compl_నమూనా.నిడివి = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    ఒకవేళ (ctrl_x_బహుళకం_సాధారణ() && !(compl_cont_స్థితి & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	ఒకవేళ (అమరిక_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    స్థిర పూర్ణాంకం
get_wholeline_compl_info(char_u *పంక్తి, colnr_T curs_నిలువు)
{
    compl_నిలువు = (colnr_T)getwhitecols(పంక్తి);
    compl_నిడివి = (పూర్ణాంకం)curs_నిలువు - (పూర్ణాంకం)compl_నిలువు;
    ఒకవేళ (compl_నిడివి < 0)	// cursor in indent: empty pattern
	compl_నిడివి = 0;
    ఒకవేళ (p_ic)
    {
	compl_నమూనా.string = str_foldcase(పంక్తి + compl_నిలువు, compl_నిడివి,
		ఖాళీ, 0);
	ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	{
	    compl_నమూనా.నిడివి = 0;
	    తిరిగి FAIL;
	}
	compl_నమూనా.నిడివి = STRLEN(compl_నమూనా.string);
    }
    లేకపోతే
    {
	compl_నమూనా.string = vim_strnsave(పంక్తి + compl_నిలువు, (size_t)compl_నిడివి);
	ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	{
	    compl_నమూనా.నిడివి = 0;
	    తిరిగి FAIL;
	}
	compl_నమూనా.నిడివి = (size_t)compl_నిడివి;
    }

    తిరిగి OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    స్థిర పూర్ణాంకం
get_filename_compl_info(char_u *పంక్తి, పూర్ణాంకం startcol, colnr_T curs_నిలువు)
{
    // Go back to just before the first filename character.
    ఒకవేళ (startcol > 0)
    {
	char_u	*p = పంక్తి + startcol;

	MB_PTR_వెనుక(పంక్తి, p);
	అయితే (p > పంక్తి && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_వెనుక(పంక్తి, p);
	ఒకవేళ (p == పంక్తి && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	లేకపోతే
	    startcol = (పూర్ణాంకం)(p - పంక్తి) + 1;
    }

    compl_నిలువు += startcol;
    compl_నిడివి = (పూర్ణాంకం)curs_నిలువు - startcol;
    compl_నమూనా.string = addstar(పంక్తి + compl_నిలువు, compl_నిడివి, EXPAND_FILES);
    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
    {
	compl_నమూనా.నిడివి = 0;
	తిరిగి FAIL;
    }

    compl_నమూనా.నిడివి = STRLEN(compl_నమూనా.string);

    తిరిగి OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    స్థిర పూర్ణాంకం
get_cmdline_compl_info(char_u *పంక్తి, colnr_T curs_నిలువు)
{
    compl_నమూనా.string = vim_strnsave(పంక్తి, curs_నిలువు);
    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
    {
	compl_నమూనా.నిడివి = 0;
	తిరిగి FAIL;
    }
    compl_నమూనా.నిడివి = curs_నిలువు;
    సమితి_cmd_context(&compl_xp, compl_నమూనా.string,
	    (పూర్ణాంకం)compl_నమూనా.నిడివి, curs_నిలువు, FALSE);
    ఒకవేళ (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_నిలువు = curs_నిలువు;
    లేకపోతే
	compl_నిలువు = (పూర్ణాంకం)(compl_xp.xp_నమూనా - compl_నమూనా.string);
    compl_నిడివి = curs_నిలువు - compl_నిలువు;

    తిరిగి OK;
}

#నిర్వచితమైతే FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    స్థిర పూర్ణాంకం
సమితి_compl_globals(
    పూర్ణాంకం		startcol వాడని,
    colnr_T	curs_నిలువు వాడని,
    పూర్ణాంకం		is_cpt_compl వాడని)
{
    ఒకవేళ (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_నమూనా);
	ఒకవేళ (startcol < compl_నిలువు)
	    తిరిగి prepend_startcol_పాఠ్యం(&cpt_compl_నమూనా, &compl_orig_పాఠ్యం,
		    startcol);
	లేకపోతే
	{
	    cpt_compl_నమూనా.string = vim_strnsave(compl_orig_పాఠ్యం.string,
		    compl_orig_పాఠ్యం.నిడివి);
	    cpt_compl_నమూనా.నిడివి = compl_orig_పాఠ్యం.నిడివి;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (startcol < 0 || startcol > curs_నిలువు)
	    startcol = curs_నిలువు;

	// Re-obtain line in case it has changed
	char_u	*పంక్తి = ml_get(curwin->w_cursor.lnum);
	పూర్ణాంకం	len = curs_నిలువు - startcol;

	compl_నమూనా.string = vim_strnsave(పంక్తి + startcol, (size_t)len);
	ఒకవేళ (compl_నమూనా.string == ఖాళీ)
	{
	    compl_నమూనా.నిడివి = 0;
	    తిరిగి FAIL;
	}
	compl_నమూనా.నిడివి = (size_t)len;
	compl_నిలువు = startcol;
	compl_నిడివి = len;
    }

    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Get the pattern, column and length for user defined completion ('omnifunc',
 * 'completefunc' and 'thesaurusfunc')
 * Uses the global variable: spell_bad_len
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 * "startcol", when not NULL, contains the column returned by function.
 */
    స్థిర పూర్ణాంకం
get_userdefined_compl_info(
    colnr_T	curs_నిలువు వాడని,
    callback_T	*cb వాడని,
    పూర్ణాంకం		*startcol వాడని)
{
    పూర్ణాంకం		ret = FAIL;

#నిర్వచితమైతే FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	వాదనలు[3];
    పూర్ణాంకం		నిలువు;
    char_u	*funcname = ఖాళీ;
    pos_T	pos;
    పూర్ణాంకం		save_State = State;
    పూర్ణాంకం		is_cpt_function = (cb != ఖాళీ);
    పూర్ణాంకం		use_sandbox = is_cpt_function
			    && was_సమితి_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ఒకవేళ (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_బహుళకం);
	ఒకవేళ (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_సమితి), ctrl_x_బహుళకం_function()
		    ? "completefunc" : "omnifunc");
	    తిరిగి FAIL;
	}
	cb = get_చొప్పించు_callback(ctrl_x_బహుళకం);
    }

    వాదనలు[0].v_type = VAR_సంఖ్య;
    వాదనలు[0].vval.v_సంఖ్య = 1;
    వాదనలు[1].v_type = VAR_STRING;
    వాదనలు[1].vval.v_string = (char_u *)"";
    వాదనలు[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    ఒకవేళ (use_sandbox)
	++sandbox;
    నిలువు = call_callback_retnr(cb, 2, వాదనలు);
    ఒకవేళ (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ఒకవేళ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_పాఠ్యం));
	తిరిగి FAIL;
    }

    ఒకవేళ (startcol != ఖాళీ)
	*startcol = నిలువు;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    ఒకవేళ (నిలువు == -2 || aborting())
	తిరిగి FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    ఒకవేళ (నిలువు == -3)
    {
	ఒకవేళ (is_cpt_function)
	    తిరిగి FAIL;
	ctrl_x_బహుళకం = CTRL_X_సాధారణ;
	edit_submode = ఖాళీ;
	ఒకవేళ (!shortmess(SHM_COMPLETIONMENU))
	    సందేశం_clr_cmdline();
	తిరిగి FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_ఖాళీలు = FALSE;

    ret = !is_cpt_function ? సమితి_compl_globals(నిలువు, curs_నిలువు, FALSE) : OK;
#అయితే_ముగింపు

    తిరిగి ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    స్థిర పూర్ణాంకం
get_spell_compl_info(పూర్ణాంకం startcol వాడని, colnr_T curs_నిలువు వాడని)
{
    పూర్ణాంకం		ret = FAIL;
#నిర్వచితమైతే FEAT_SPELL
    char_u	*పంక్తి = ఖాళీ;

    ఒకవేళ (spell_bad_len > 0)
	compl_నిలువు = curs_నిలువు - spell_bad_len;
    లేకపోతే
	compl_నిలువు = spell_మాట_ప్రారంభం(startcol);

    ఒకవేళ (compl_నిలువు >= (colnr_T)startcol)
    {
	compl_నిడివి = 0;
	compl_నిలువు = curs_నిలువు;
    }
    లేకపోతే
    {
	spell_expand_check_cap(compl_నిలువు);
	compl_నిడివి = (పూర్ణాంకం)curs_నిలువు - compl_నిలువు;
    }
    // Need to obtain "line" again, it may have become invalid.
    పంక్తి = ml_get(curwin->w_cursor.lnum);
    compl_నమూనా.string = vim_strnsave(పంక్తి + compl_నిలువు, (size_t)compl_నిడివి);
    ఒకవేళ (compl_నమూనా.string == ఖాళీ)
    {
	compl_నమూనా.నిడివి = 0;
	తిరిగి FAIL;
    }

    compl_నమూనా.నిడివి = (size_t)compl_నిడివి;
    ret = OK;
#అయితే_ముగింపు

    తిరిగి ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    స్థిర పూర్ణాంకం
compl_get_info(char_u *పంక్తి, పూర్ణాంకం startcol, colnr_T curs_నిలువు, పూర్ణాంకం *పంక్తి_invalid)
{
    ఒకవేళ (ctrl_x_బహుళకం_సాధారణ() || ctrl_x_బహుళకం_రిజిస్టరు()
	    || (ctrl_x_బహుళకం & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_బహుళకం)))
    {
	ఒకవేళ (get_సాధారణ_compl_info(పంక్తి, startcol, curs_నిలువు) != OK)
	    తిరిగి FAIL;
	*పంక్తి_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_పంక్తి_or_eval())
    {
	తిరిగి get_wholeline_compl_info(పంక్తి, curs_నిలువు);
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_files())
    {
	తిరిగి get_filename_compl_info(పంక్తి, startcol, curs_నిలువు);
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం == CTRL_X_CMDLINE)
    {
	తిరిగి get_cmdline_compl_info(పంక్తి, curs_నిలువు);
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_function() || ctrl_x_బహుళకం_omni()
	    || thesaurus_func_complete(ctrl_x_బహుళకం))
    {
	ఒకవేళ (get_userdefined_compl_info(curs_నిలువు, ఖాళీ, ఖాళీ) != OK)
	    తిరిగి FAIL;
	*పంక్తి_invalid = TRUE;	// "line" may have become invalid
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_spell())
    {
	ఒకవేళ (get_spell_compl_info(startcol, curs_నిలువు) == FAIL)
	    తిరిగి FAIL;
	*పంక్తి_invalid = TRUE;	// "line" may have become invalid
    }
    లేకపోతే
    {
	అంతర్గత_error("ins_complete()");
	తిరిగి FAIL;
    }

    తిరిగి OK;
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
    స్థిర శూన్య
ins_compl_continue_శోధన(char_u *పంక్తి)
{
    // it is a continued search
    compl_cont_స్థితి &= ~CONT_INTRPT;	// remove INTRPT
    ఒకవేళ (ctrl_x_బహుళకం_సాధారణ() || ctrl_x_బహుళకం_మార్గం_patterns()
						|| ctrl_x_బహుళకం_మార్గం_నిర్వచిస్తుంది())
    {
	ఒకవేళ (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_నిలువు = (colnr_T)getwhitecols(పంక్తి);
	    compl_startpos.నిలువు = compl_నిలువు;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_స్థితి &= ~CONT_SOL;   // clear SOL if present
	}
	లేకపోతే
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    ఒకవేళ (compl_cont_స్థితి & CONT_S_IPOS)
	    {
		compl_cont_స్థితి |= CONT_SOL;
		compl_startpos.నిలువు = (colnr_T)(skipwhite(
			    పంక్తి + compl_నిడివి
			    + compl_startpos.నిలువు) - పంక్తి);
	    }
	    compl_నిలువు = compl_startpos.నిలువు;
	}
	compl_నిడివి = curwin->w_cursor.నిలువు - (పూర్ణాంకం)compl_నిలువు;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#నిర్వచించు	MIN_SPACE 75
	ఒకవేళ (compl_నిడివి > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_స్థితి &= ~CONT_SOL;
	    compl_నిడివి = (IOSIZE - MIN_SPACE);
	    compl_నిలువు = curwin->w_cursor.నిలువు - compl_నిడివి;
	}
	compl_cont_స్థితి |= CONT_ADDING | CONT_N_ADDS;
	ఒకవేళ (compl_నిడివి < 1)
	    compl_cont_స్థితి &= CONT_LOCAL;
    }
    లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_పంక్తి_or_eval() || ctrl_x_బహుళకం_రిజిస్టరు())
	compl_cont_స్థితి = CONT_ADDING | CONT_N_ADDS;
    లేకపోతే
	compl_cont_స్థితి = 0;
}

/*
 * start insert mode completion
 */
    స్థిర పూర్ణాంకం
ins_compl_ప్రారంభం(శూన్య)
{
    char_u	*పంక్తి = ఖాళీ;
    పూర్ణాంకం		startcol = 0;	    // column where searched text starts
    colnr_T	curs_నిలువు;	    // cursor column
    పూర్ణాంకం		పంక్తి_invalid = FALSE;
    పూర్ణాంకం		save_did_ai = did_ai;
    పూర్ణాంకం		flags = CP_ORIGINAL_పాఠ్యం;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;
    ఒకవేళ (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	తిరిగి FAIL;
    }

    పంక్తి = ml_get(curwin->w_cursor.lnum);
    curs_నిలువు = curwin->w_cursor.నిలువు;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    ఒకవేళ ((compl_cont_స్థితి & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_బహుళకం == ctrl_x_బహుళకం)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_శోధన(పంక్తి);
    లేకపోతే
	compl_cont_స్థితి &= CONT_LOCAL;

    ఒకవేళ (!compl_స్థితి_adding())	// normal expansion
    {
	compl_cont_బహుళకం = ctrl_x_బహుళకం;
	ఒకవేళ (ctrl_x_బహుళకం_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_స్థితి = 0;
	compl_cont_స్థితి |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (పూర్ణాంకం)curs_నిలువు;
	compl_నిలువు = 0;
    }

    // Work out completion pattern and original text -- webb
    ఒకవేళ (compl_get_info(పంక్తి, startcol, curs_నిలువు, &పంక్తి_invalid) == FAIL)
    {
	ఒకవేళ (ctrl_x_బహుళకం_function() || ctrl_x_బహుళకం_omni()
				|| thesaurus_func_complete(ctrl_x_బహుళకం))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	తిరిగి FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    ఒకవేళ (పంక్తి_invalid)
	పంక్తి = ml_get(curwin->w_cursor.lnum);

    ఒకవేళ (compl_స్థితి_adding())
    {
	ఒకవేళ (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	ఒకవేళ (ctrl_x_బహుళకం_పంక్తి_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.నిలువు = compl_నిలువు;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_నిడివి = 0;
	    compl_నిలువు = curwin->w_cursor.నిలువు;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    లేకపోతే
    {
	edit_submode_pre = ఖాళీ;
	compl_startpos.నిలువు = compl_నిలువు;
    }

    ఒకవేళ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	ఒకవేళ (compl_cont_స్థితి & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_సందేశం]);
	లేకపోతే
	    edit_submode = (char_u *)_(CTRL_X_సందేశం(ctrl_x_బహుళకం));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ఖాళీ);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_పాఠ్యం);
    compl_orig_పాఠ్యం.నిడివి = (size_t)compl_నిడివి;
    compl_orig_పాఠ్యం.string = vim_strnsave(పంక్తి + compl_నిలువు, (size_t)compl_నిడివి);
    ఒకవేళ (p_ic)
	flags |= CP_ICASE;
    ఒకవేళ (compl_orig_పాఠ్యం.string == ఖాళీ
	    || ins_compl_కూడు(compl_orig_పాఠ్యం.string,
		(పూర్ణాంకం)compl_orig_పాఠ్యం.నిడివి,
		ఖాళీ, ఖాళీ, ఖాళీ, 0, flags, FALSE, ఖాళీ, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_నమూనా);
	VIM_CLEAR_STRING(compl_orig_పాఠ్యం);
	did_ai = save_did_ai;
	తిరిగి FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    ఒకవేళ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_లెక్క;
	showmode();
	edit_submode_extra = ఖాళీ;
	బహిర్గతం_flush();
    }

    did_ai = save_did_ai;
    తిరిగి OK;
}

/*
 * display the completion status message
 */
    స్థిర శూన్య
ins_compl_show_statusmsg(శూన్య)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    ఒకవేళ (is_మొదటి_సరిపోలిక(compl_మొదటి_సరిపోలిక->cp_తదుపరి))
    {
	edit_submode_extra = compl_స్థితి_adding() && compl_నిడివి > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    ఒకవేళ (edit_submode_extra == ఖాళీ)
    {
	ఒకవేళ (సరిపోలిక_at_original_పాఠ్యం(compl_curr_సరిపోలిక))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	లేకపోతే ఒకవేళ (compl_cont_స్థితి & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_లెక్క;
	}
	లేకపోతే ఒకవేళ (compl_curr_సరిపోలిక->cp_తదుపరి == compl_curr_సరిపోలిక->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_లెక్క;
	    compl_curr_సరిపోలిక->cp_సంఖ్య = 1;
	}
	లేకపోతే
	{
#ఒకవేళ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    ఒకవేళ (compl_curr_సరిపోలిక->cp_సంఖ్య == -1)
		ins_compl_update_sequence_numbers();
#అయితే_ముగింపు
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    ఒకవేళ (compl_curr_సరిపోలిక->cp_సంఖ్య != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		స్థిర char_u సరిపోలిక_ref[81];

		ఒకవేళ (compl_matches > 0)
		    vim_snprintf((అక్షరం *)సరిపోలిక_ref, పరిమాణం(సరిపోలిక_ref),
			    _("match %d of %d"),
			    compl_curr_సరిపోలిక->cp_సంఖ్య, compl_matches);
		లేకపోతే
		    vim_snprintf((అక్షరం *)సరిపోలిక_ref, పరిమాణం(సరిపోలిక_ref),
			    _("match %d"),
			    compl_curr_సరిపోలిక->cp_సంఖ్య);
		edit_submode_extra = సరిపోలిక_ref;
		edit_submode_highl = HLF_R;
		ఒకవేళ (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    ఒకవేళ (!compl_opt_suppress_ఖాళీలు)
    {
	showmode();
	ఒకవేళ (!shortmess(SHM_COMPLETIONMENU))
	{
	    ఒకవేళ (edit_submode_extra != ఖాళీ)
	    {
		ఒకవేళ (!p_smd)
		{
		    సందేశం_hist_off = TRUE;
		    సందేశం_attr((అక్షరం *)edit_submode_extra,
			    edit_submode_highl < HLF_లెక్క
			    ? HL_ATTR(edit_submode_highl) : 0);
		    సందేశం_hist_off = FALSE;
		}
	    }
	    లేకపోతే
		సందేశం_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    పూర్ణాంకం
ins_complete(పూర్ణాంకం c, పూర్ణాంకం enable_pum)
{
    పూర్ణాంకం		n;
    పూర్ణాంకం		save_w_wrow;
    పూర్ణాంకం		save_w_leftcol;
    పూర్ణాంకం		చొప్పించు_సరిపోలిక;
    పూర్ణాంకం		no_matches_దొరికింది;

    compl_direction = ins_compl_key2dir(c);
    చొప్పించు_సరిపోలిక = ins_compl_use_సరిపోలిక(c);

    ఒకవేళ (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	ఒకవేళ (ins_compl_ప్రారంభం() == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (చొప్పించు_సరిపోలిక && stop_arrow() == FAIL)
	తిరిగి FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_బఫరు;
    compl_shown_సరిపోలిక = compl_curr_సరిపోలిక;
    compl_shows_dir = compl_direction;
    compl_అంకె_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_తదుపరి(TRUE, ins_compl_key2count(c), చొప్పించు_సరిపోలిక);

    // Reset autocompletion timer expiry flag
    ఒకవేళ (compl_autocomplete)
	compl_సమయం_ముక్క_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    ఒకవేళ (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_సరిపోలిక = compl_shown_సరిపోలిక;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    ఒకవేళ (got_int && !global_busy)
    {
	(శూన్య)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_దొరికింది = is_మొదటి_సరిపోలిక(compl_మొదటి_సరిపోలిక->cp_తదుపరి);
    ఒకవేళ (no_matches_దొరికింది)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	ఒకవేళ (compl_నిడివి > 1
		|| compl_స్థితి_adding()
		|| (ctrl_x_బహుళకం_not_default()
		    && !ctrl_x_బహుళకం_మార్గం_patterns()
		    && !ctrl_x_బహుళకం_మార్గం_నిర్వచిస్తుంది()))
	    compl_cont_స్థితి &= ~CONT_N_ADDS;
    }

    ఒకవేళ (compl_curr_సరిపోలిక->cp_flags & CP_CONT_S_IPOS)
	compl_cont_స్థితి |= CONT_S_IPOS;
    లేకపోతే
	compl_cont_స్థితి &= ~CONT_S_IPOS;

    ఒకవేళ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    ఒకవేళ (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    తిరిగి OK;
}

/*
 * Enable autocompletion
 */
    శూన్య
ins_compl_enable_autocomplete(శూన్య)
{
#నిర్వచితమైతే ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#అయితే_ముగింపు
}

/*
 * Disable autocompletion
 */
    శూన్య
ins_compl_disable_autocomplete(శూన్య)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    శూన్య
ins_compl_arm_autostart(శూన్య)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    శూన్య
ins_compl_disarm_autostart(శూన్య)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(శూన్య)
{
#నిర్వచితమైతే ELAPSED_FUNC
    ఒకవేళ (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_ప్రారంభం_tv);
	compl_autocomplete_pending = true;
	తిరిగి true;
    }
#అయితే_ముగింపు
    తిరిగి false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    శూన్య
ins_compl_clear_autocomplete_delay(శూన్య)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(శూన్య)
{
    తిరిగి compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    దీర్ఘ
ins_compl_autocomplete_elapsed(శూన్య)
{
#నిర్వచితమైతే ELAPSED_FUNC
    తిరిగి ELAPSED_FUNC(compl_autocomplete_ప్రారంభం_tv);
#లేకపోతే
    తిరిగి 0;
#అయితే_ముగింపు
}

/*
 * Remove (if needed) and show the popup menu
 */
    స్థిర శూన్య
show_pum(పూర్ణాంకం prev_w_wrow, పూర్ణాంకం prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    ఒకవేళ (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    స్థిర గుర్తులేని
quote_meta(char_u *dest, char_u *src, పూర్ణాంకం len)
{
    గుర్తులేని	m = (గుర్తులేని)len + 1;  // one extra for the NUL

    కొరకు ( ; --len >= 0; src++)
    {
	ఎంపిక (*src)
	{
	    స్థితి '.':
	    స్థితి '*':
	    స్థితి '[':
		ఒకవేళ (ctrl_x_బహుళకం_dictionary() || ctrl_x_బహుళకం_thesaurus())
		    ఆపు;
		// FALLTHROUGH
	    స్థితి '~':
		ఒకవేళ (!magic_isset())	// quote these only if magic is set
		    ఆపు;
		// FALLTHROUGH
	    స్థితి '\\':
		ఒకవేళ (ctrl_x_బహుళకం_dictionary() || ctrl_x_బహుళకం_thesaurus())
		    ఆపు;
		// FALLTHROUGH
	    స్థితి '^':		// currently it's not needed.
	    స్థితి '$':
		m++;
		ఒకవేళ (dest != ఖాళీ)
		    *dest++ = '\\';
		ఆపు;
	}
	ఒకవేళ (dest != ఖాళీ)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం mb_len = (*mb_ptr2len)(src) - 1;
	    ఒకవేళ (mb_len > 0 && len >= mb_len)
		కొరకు (పూర్ణాంకం i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    ఒకవేళ (dest != ఖాళీ)
			*dest++ = *src;
		}
	}
    }
    ఒకవేళ (dest != ఖాళీ)
	*dest = NUL;

    తిరిగి m;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_insexpand_stuff(శూన్య)
{
    VIM_CLEAR_STRING(compl_orig_పాఠ్యం);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_లెక్క);
# endif
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    స్థిర శూన్య
spell_వెనుక_గమ్యం_badword(శూన్య)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_గమ్యం(curwin, BACKWARD, SMT_ALL, TRUE, ఖాళీ);
    ఒకవేళ (curwin->w_cursor.నిలువు != tpos.నిలువు)
	ప్రారంభం_arrow(&tpos);
}
#అయితే_ముగింపు

/*
 * Reset the info associated with completion sources.
 */
    స్థిర శూన్య
cpt_sources_clear(శూన్య)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_సూచీ = -1;
    cpt_sources_లెక్క = 0;
}

/*
 * Setup completion sources.
 */
    స్థిర పూర్ణాంకం
అమరిక_cpt_sources(శూన్య)
{
    char_u  buf[LSIZE];
    పూర్ణాంకం	    slen;
    పూర్ణాంకం	    idx = 0;
    పూర్ణాంకం	    లెక్క;
    char_u  *p;

    cpt_sources_clear();

    లెక్క = get_cpt_sources_లెక్క(curbuf->b_p_cpt);
    ఒకవేళ (లెక్క == 0)
	తిరిగి OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_ఆధారం_T, లెక్క);
    ఒకవేళ (cpt_sources_array == ఖాళీ)
	తిరిగి FAIL;

    కొరకు (p = curbuf->b_p_cpt; *p;)
    {
	అయితే (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ఒకవేళ (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ఒకవేళ (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		ఒకవేళ (caret != ఖాళీ)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((అక్షరం *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_లెక్క = లెక్క;
    తిరిగి OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    స్థిర పూర్ణాంకం
is_cpt_func_refresh_always(శూన్య)
{
#నిర్వచితమైతే FEAT_COMPL_FUNC
    కొరకు (పూర్ణాంకం i = 0; i < cpt_sources_లెక్క; i++)
	ఒకవేళ (cpt_sources_array[i].cs_refresh_always)
	    తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    స్థిర శూన్య
ins_compl_make_రేఖీయ(శూన్య)
{
    compl_T *m;

    ఒకవేళ (compl_మొదటి_సరిపోలిక == ఖాళీ || compl_మొదటి_సరిపోలిక->cp_prev == ఖాళీ)
	తిరిగి;
    m = compl_మొదటి_సరిపోలిక->cp_prev;
    m->cp_తదుపరి = ఖాళీ;
    compl_మొదటి_సరిపోలిక->cp_prev = ఖాళీ;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#నిర్వచితమైతే FEAT_COMPL_FUNC
    స్థిర శూన్య
remove_old_matches(శూన్య)
{
    compl_T *current;
    పూర్ణాంకం	    shown_సరిపోలిక_removed = FALSE;
    పూర్ణాంకం	    forward = (compl_మొదటి_సరిపోలిక->cp_cpt_ఆధారం_idx < 0);

    ఒకవేళ (cpt_sources_సూచీ < 0)
	తిరిగి;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    కొరకు (current = compl_మొదటి_సరిపోలిక; current != ఖాళీ; )
    {
	ఒకవేళ (current->cp_cpt_ఆధారం_idx == cpt_sources_సూచీ)
	{
	    compl_T *గమ్యం_delete = current;

	    ఒకవేళ (!shown_సరిపోలిక_removed && compl_shown_సరిపోలిక == current)
		shown_సరిపోలిక_removed = TRUE;

	    current = current->cp_తదుపరి;

	    ఒకవేళ (గమ్యం_delete == compl_మొదటి_సరిపోలిక)  // node to remove is at head
	    {
		compl_మొదటి_సరిపోలిక = గమ్యం_delete->cp_తదుపరి;
		compl_మొదటి_సరిపోలిక->cp_prev = ఖాళీ;
	    }
	    లేకపోతే ఒకవేళ (గమ్యం_delete->cp_తదుపరి == ఖాళీ) // node to remove is at tail
		గమ్యం_delete->cp_prev->cp_తదుపరి = ఖాళీ;
	    లేకపోతే // node is in the moddle
	    {
		గమ్యం_delete->cp_prev->cp_తదుపరి = గమ్యం_delete->cp_తదుపరి;
		గమ్యం_delete->cp_తదుపరి->cp_prev = గమ్యం_delete->cp_prev;
	    }
	    ins_compl_అంశం_free(గమ్యం_delete);
	}
	లేకపోతే
	    current = current->cp_తదుపరి;
    }

    // Re-assign compl_shown_match if necessary
    ఒకవేళ (shown_సరిపోలిక_removed)
    {
	ఒకవేళ (forward)
	    compl_shown_సరిపోలిక = compl_మొదటి_సరిపోలిక;
	లేకపోతే
	{  // Last node will have the prefix that is being completed
	    కొరకు (current = compl_మొదటి_సరిపోలిక; current->cp_తదుపరి != ఖాళీ;
		    current = current->cp_తదుపరి)
		;
	    compl_shown_సరిపోలిక = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_సరిపోలిక = compl_మొదటి_సరిపోలిక;
    కొరకు (current = compl_మొదటి_సరిపోలిక; current != ఖాళీ; )
    {
	ఒకవేళ ((forward ? current->cp_cpt_ఆధారం_idx < cpt_sources_సూచీ
		    : current->cp_cpt_ఆధారం_idx > cpt_sources_సూచీ))
	{
	    compl_curr_సరిపోలిక = forward ? current : current->cp_తదుపరి;
	    current = current->cp_తదుపరి;
	}
	లేకపోతే
	    ఆపు;
    }
}
#అయితే_ముగింపు

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#నిర్వచితమైతే FEAT_COMPL_FUNC
    స్థిర శూన్య
get_cpt_func_completion_matches(callback_T *cb వాడని)
{
    cpt_ఆధారం_T    *cpt_src = &cpt_sources_array[cpt_sources_సూచీ];
    పూర్ణాంకం		    startcol = cpt_src->cs_startcol;

    ఒకవేళ (startcol == -2 || startcol == -3)
	తిరిగి;

    ఒకవేళ (సమితి_compl_globals(startcol, curwin->w_cursor.నిలువు, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	ఒకవేళ (!cpt_src->cs_refresh_always)
	    ins_compl_చొప్పించు_బైట్లు(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_నమూనా.string, cb);

	ఒకవేళ (!cpt_src->cs_refresh_always)
	    ins_compl_delete();

	cpt_src->cs_refresh_always = compl_opt_refresh_always;
	compl_opt_refresh_always = FALSE;
    }
}
#అయితే_ముగింపు

/*
 * Retrieve completion matches from functions in the 'cpt' option where the
 * 'refresh:always' flag is set.
 */
    స్థిర శూన్య
cpt_compl_refresh(శూన్య)
{
#నిర్వచితమైతే FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ఖాళీ;
    పూర్ణాంకం		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_రేఖీయ();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_లోపల_ఉంచు(cpt);

    cpt_sources_సూచీ = 0;
    కొరకు (p = cpt; *p;)
    {
	అయితే (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ఒకవేళ (*p == NUL)
	    ఆపు;

	ఒకవేళ (cpt_sources_array[cpt_sources_సూచీ].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_సూచీ);
	    ఒకవేళ (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.నిలువు, cb,
			&startcol);
		ఒకవేళ (ret == FAIL)
		{
		    ఒకవేళ (startcol == -3)
			cpt_sources_array[cpt_sources_సూచీ].cs_refresh_always
			    = FALSE;
		    లేకపోతే
			startcol = -2;
		}
		లేకపోతే ఒకవేళ (startcol < 0 || startcol > curwin->w_cursor.నిలువు)
		    startcol = curwin->w_cursor.నిలువు;
		cpt_sources_array[cpt_sources_సూచీ].cs_startcol = startcol;
		ఒకవేళ (ret == OK)
		{
		    compl_ఆధారం_ప్రారంభం_timer(cpt_sources_సూచీ);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(శూన్య)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	ఒకవేళ (may_advance_cpt_సూచీ(p))
	    (శూన్య)advance_cpt_sources_సూచీ_సురక్షిత();
    }
    cpt_sources_సూచీ = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#అయితే_ముగింపు
}

/*
 * "preinserted()" function
 */
    శూన్య
f_preinserted(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    ఒకవేళ (ins_compl_preinsert_effect())
	rettv->vval.v_సంఖ్య = 1;
}
