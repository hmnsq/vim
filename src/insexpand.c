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

#ضمن "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#وصف_ڪريو CTRL_X_WANT_IDENT	0x100

#وصف_ڪريو CTRL_X_عام		0  // CTRL-N CTRL-P completion, default
#وصف_ڪريو CTRL_X_NOT_DEFINED_YET	1
#وصف_ڪريو CTRL_X_SCROLL		2
#وصف_ڪريو CTRL_X_سڄو_سٽ	3
#وصف_ڪريو CTRL_X_FILES		4
#وصف_ڪريو CTRL_X_ٽيگ_فهرست		(5 + CTRL_X_WANT_IDENT)
#وصف_ڪريو CTRL_X_رستو_PATTERNS	(6 + CTRL_X_WANT_IDENT)
#وصف_ڪريو CTRL_X_رستو_مقرر_ڪري	(7 + CTRL_X_WANT_IDENT)
#وصف_ڪريو CTRL_X_ختم		8
#وصف_ڪريو CTRL_X_DICTIONARY	(9 + CTRL_X_WANT_IDENT)
#وصف_ڪريو CTRL_X_THESAURUS	(10 + CTRL_X_WANT_IDENT)
#وصف_ڪريو CTRL_X_CMDLINE		11
#وصف_ڪريو CTRL_X_FUNCTION	12
#وصف_ڪريو CTRL_X_OMNI		13
#وصف_ڪريو CTRL_X_SPELL		14
#وصف_ڪريو CTRL_X_LOCAL_نياپو	15	// only used in "ctrl_x_msgs"
#وصف_ڪريو CTRL_X_EVAL		16	// for builtin function complete()
#وصف_ڪريو CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#وصف_ڪريو CTRL_X_رجسٽر	18	// complete words from registers

#وصف_ڪريو CTRL_X_نياپو(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
ساكن حرف *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    عدم, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    عدم, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    عدم,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#إذا defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ساكن string_T ctrl_x_گهڻائي_نالا[] = {
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
    {عدم, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#جيڪڏهن_پورو

/*
 * Structure used to store one match for insert completion.
 */
تعريفنوع بنية compl_S compl_T;
بنية compl_S
{
    compl_T	*cp_ايندڙ;
    compl_T	*cp_prev;
    compl_T	*cp_ميلاپ_ايندڙ;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_متن[CPT_ڳڻپ]);	// text for the menu
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    typval_T	cp_user_معلومات;
#جيڪڏهن_پورو
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    صحيح		cp_flags;		// CP_ values
    صحيح		cp_عدد;		// sequence number
    صحيح		cp_ترتيب;		// list position when first scored
    صحيح		cp_score;		// fuzzy match score or proximity score
    صحيح		cp_اندريون_ميلاپ_array;	// collected by compl_match_array
    صحيح		cp_user_abbr_hl_id;	// highlight group ID for abbr
    صحيح		cp_user_قسم_hl_id;	// highlight group ID for kind
    صحيح		cp_cpt_ذريعو_idx;	// index of this match's source in 'cpt' option
};

// values for cp_flags
#وصف_ڪريو CP_ORIGINAL_متن   1	// the original text when the expansion begun
#وصف_ڪريو CP_FREE_FNAME	    2	// cp_fname is allocated
#وصف_ڪريو CP_CONT_S_IPOS	    4	// use CONT_S_IPOS for compl_cont_status
#وصف_ڪريو CP_EQUAL	    8	// ins_compl_equal() always returns TRUE
#وصف_ڪريو CP_ICASE	    16	// ins_compl_equal() ignores case
#وصف_ڪريو CP_FAST	    32	// use fast_breakcheck instead of ui_breakcheck

/*
 * All the current matches are stored in a list.
 * "compl_first_match" points to the start of the list.
 * "compl_curr_match" points to the currently selected entry.
 * "compl_shown_match" is different from compl_curr_match during
 * ins_compl_get_exp(), when new matches are added to the list.
 * "compl_old_match" points to previous "compl_curr_match".
 */
ساكن compl_T    *compl_پهريون_ميلاپ = عدم;
ساكن compl_T    *compl_curr_ميلاپ = عدم;
ساكن compl_T    *compl_shown_ميلاپ = عدم;
ساكن compl_T    *compl_old_ميلاپ = عدم;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
تعريفنوع بنية
{
    صحيح	    cse_ڳڻپ;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#وصف_ڪريو CSE_OFF		((صحيح)offsetof(complstr_T, cse_str))
#وصف_ڪريو HI2CSE(hi)	((complstr_T *)((hi)->hi_ڪنجي - CSE_OFF))

ساكن hashtab_T  compl_لڙيون_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    ساكن فراغ
compl_لڙيون_جوڙيو(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*داخلا;

    إذا (compl_لڙيون_ht.ht_array == عدم)
	hash_init(&compl_لڙيون_ht);
    إذا (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_لڙيون_ht, str, hash);
    إذا (HASHITEM_خالي_جايون(hi))
    {
	داخلا = alloc(CSE_OFF + len + 1);
	إذا (داخلا == عدم)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    أرجع;
	داخلا->cse_ڳڻپ = 1;
	vim_strncpy(داخلا->cse_str, str, len);
	إذا (hash_جوڙيو_شي(&compl_لڙيون_ht, hi, داخلا->cse_str, hash)
								      == FAIL)
	    vim_free(داخلا);
    }
    وإلا
	++HI2CSE(hi)->cse_ڳڻپ;
}

// list used to store the compl_T which have the max score
ساكن compl_T	  **compl_بهترين_matches = عدم;
ساكن صحيح	  compl_انگ_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
ساكن صحيح	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
ساكن string_T	  compl_leader = {عدم, 0};

ساكن صحيح	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
ساكن صحيح	  compl_استعمال_ٿيل_ميلاپ;

// didn't finish finding completions.
ساكن صحيح	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
ساكن صحيح	  compl_interrupted = FALSE;

ساكن صحيح	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
ساكن صحيح	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
ساكن صحيح	  ctrl_x_گهڻائي = CTRL_X_عام;

ساكن صحيح	  compl_matches = 0;	    // number of completion matches
ساكن string_T	  compl_نمونو = {عدم, 0};	 // search pattern for matching items
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
ساكن string_T	  cpt_compl_نمونو = {عدم, 0}; // pattern returned by func in 'cpt'
#جيڪڏهن_پورو
ساكن صحيح	  compl_direction = FORWARD;
ساكن صحيح	  compl_shows_dir = FORWARD;
ساكن صحيح	  compl_pending = 0;	    // > 1 for postponed CTRL-N
ساكن pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
ساكن صحيح	  compl_ڊيگهائي = 0;
ساكن linenr_T	  compl_lnum = 0;           // lnum where the completion start
ساكن colnr_T	  compl_کانو = 0;	    // column where the text starts
					    // that is being completed
ساكن colnr_T	  compl_ins_پڇاڙي_کانو = 0;
ساكن colnr_T	  compl_longest_پڇاڙي_کانو = 0;  // end of 'longest' inserted text
ساكن string_T	  compl_orig_متن = {عدم, 0};  // text as it was before
					    // completion started
ساكن صحيح	  compl_cont_گهڻائي = 0;
ساكن expand_T	  compl_xp;

ساكن win_T	  *compl_curr_win = عدم;  // win where completion is active
ساكن buf_T	  *compl_curr_buf = عدم;  // buf where completion is active

#وصف_ڪريو COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
ساكن صحيح	  compl_autocomplete = FALSE;	    // whether autocompletion is active
ساكن bool	  compl_autostarted = false;	    // Vim started this completion
ساكن bool	  compl_autostart_pending = false;  // the trigger armed one
ساكن bool	  compl_autocomplete_pending = false;
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
ساكن elapsed_T  compl_autocomplete_شروعات_tv;	    // when the delay was armed
#جيڪڏهن_پورو
ساكن صحيح	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
ساكن صحيح	  compl_وقت_ٽڪرو_expired = FALSE; // time budget exceeded for current source
ساكن صحيح	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
ساكن صحيح	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#وصف_ڪريو COMPL_MIN_TIMEOUT_MS	5
#وصف_ڪريو DECAY_COMPL_TIMEOUT() \
    افعل { \
	إذا (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } طالما (0)

// Timeout values for F{func}, F and o values in 'complete'
#وصف_ڪريو COMPL_FUNC_TIMEOUT_MS		300
#وصف_ڪريو COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
ساكن صحيح	  compl_cont_حالت = 0;
#وصف_ڪريو CONT_ADDING	1	// "normal" or "adding" expansion
#وصف_ڪريو CONT_INTRPT	(2 + 4)	// a ^X interrupted the current expansion
				// it's set only iff N_ADDS is set
#وصف_ڪريو CONT_N_ADDS	4	// next ^X<> will add-new or expand-current
#وصف_ڪريو CONT_S_IPOS	8	// next ^X<> will set initial_pos?
				// if so, word-wise-expansion will set SOL
#وصف_ڪريو CONT_SOL	16	// pattern includes start of line, just for
				// word-wise expansion, not set for ^X^L
#وصف_ڪريو CONT_LOCAL	32	// for ctrl_x_mode 0, ^X^P/^X^N do a local
				// expansion, (eg use complete=.)

ساكن صحيح	  compl_opt_refresh_always = FALSE;
ساكن صحيح	  compl_opt_suppress_خالي_جايون = FALSE;

ساكن صحيح	  compl_selected_شي = -1;

ساكن صحيح	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
تعريفنوع بنية cpt_ذريعو_T
{
    صحيح	cs_refresh_always;  // Whether 'refresh:always' is set for func
    صحيح	cs_startcol;	    // Start column returned by func
    صحيح	cs_max_matches;	    // Max items to display from this source
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    elapsed_T	compl_شروعات_tv;	    // Timestamp when match collection starts
#جيڪڏهن_پورو
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_ذريعو_T;

ساكن cpt_ذريعو_T *cpt_sources_array; // Pointer to the array of completion sources
ساكن صحيح	    cpt_sources_ڳڻپ;  // Total number of completion sources specified in the 'cpt' option
ساكن صحيح	    cpt_sources_اشاريو = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ساكن pumitem_T *compl_ميلاپ_array = عدم;
ساكن صحيح compl_ميلاپ_arraysize;

ساكن صحيح ins_compl_جوڙيو(char_u *str, صحيح len, char_u *fname, char_u **cptext, typval_T *user_معلومات, صحيح cdir, صحيح flags, صحيح adup, صحيح *user_hl, صحيح score);
ساكن فراغ ins_compl_longest_ميلاپ(compl_T *ميلاپ);
ساكن فراغ ins_compl_del_pum(فراغ);
ساكن فراغ ins_compl_files(صحيح ڳڻپ, char_u **files, صحيح thesaurus, صحيح flags, regmatch_T *regmatch, char_u *buf, صحيح *dir);
ساكن فراغ ins_compl_free(فراغ);
ساكن صحيح  ins_compl_need_restart(فراغ);
ساكن فراغ ins_compl_new_leader(فراغ);
ساكن صحيح  get_compl_len(فراغ);
ساكن فراغ ins_compl_restart(فراغ);
ساكن فراغ ins_compl_سيٽ_original_متن(char_u *str, size_t len);
ساكن فراغ ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#إذا defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ساكن فراغ ins_compl_جوڙيو_list(list_T *list);
ساكن فراغ ins_compl_جوڙيو_dict(dict_T *dict);
ساكن صحيح get_userdefined_compl_info(colnr_T curs_کانو, callback_T *cb, صحيح *startcol);
ساكن فراغ get_cpt_func_completion_matches(callback_T *cb);
ساكن callback_T *get_callback_if_cpt_func(char_u *p, صحيح idx);
ساكن فراغ fill_complete_info_dict(dict_T *di, compl_T *ميلاپ, صحيح جوڙيو_ميلاپ);
#جيڪڏهن_پورو
ساكن صحيح تياري_cpt_sources(فراغ);
ساكن صحيح is_cpt_func_refresh_always(فراغ);
ساكن فراغ cpt_sources_clear(فراغ);
ساكن فراغ cpt_compl_refresh(فراغ);
ساكن صحيح  ins_compl_key2dir(صحيح c);
ساكن صحيح  ins_compl_pum_ڪنجي(صحيح c);
ساكن صحيح  ins_compl_key2count(صحيح c);
ساكن فراغ show_pum(صحيح prev_w_wrow, صحيح prev_w_leftcol);
ساكن بنانشان  quote_meta(char_u *dest, char_u *str, صحيح len);
ساكن صحيح ins_compl_has_گڻو(فراغ);
ساكن فراغ ins_compl_expand_گڻو(char_u *str);
ساكن فراغ ins_compl_longest_وجهو(char_u *اڳياڙي);
ساكن فراغ ins_compl_make_لڪيري(فراغ);
ساكن صحيح ins_compl_make_cyclic(فراغ);

#جيڪڏهن_وصف_ٿيل FEAT_SPELL
ساكن فراغ spell_پٺ_منزل_badword(فراغ);
ساكن صحيح  spell_bad_len = 0;	// length of located bad word
#جيڪڏهن_پورو

/*
 * CTRL-X pressed in Insert mode.
 */
    فراغ
ins_ctrl_x(فراغ)
{
    إذا (!ctrl_x_گهڻائي_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	إذا (compl_cont_حالت & CONT_N_ADDS)
	    compl_cont_حالت |= CONT_INTRPT;
	وإلا
	    compl_cont_حالت = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_گهڻائي = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_نياپو(ctrl_x_گهڻائي));
	edit_submode_pre = عدم;
	showmode();
    }
    وإلا
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_گهڻائي = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
صحيح ctrl_x_گهڻائي_none(فراغ)
    { أرجع ctrl_x_گهڻائي == 0; }
صحيح ctrl_x_گهڻائي_عام(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_عام; }
صحيح ctrl_x_گهڻائي_scroll(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_SCROLL; }
صحيح ctrl_x_گهڻائي_سڄو_سٽ(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_سڄو_سٽ; }
صحيح ctrl_x_گهڻائي_files(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_FILES; }
صحيح ctrl_x_گهڻائي_ٽيگ_فهرست(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_ٽيگ_فهرست; }
صحيح ctrl_x_گهڻائي_رستو_patterns(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_رستو_PATTERNS; }
صحيح ctrl_x_گهڻائي_رستو_مقرر_ڪري(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_رستو_مقرر_ڪري; }
صحيح ctrl_x_گهڻائي_dictionary(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_DICTIONARY; }
صحيح ctrl_x_گهڻائي_thesaurus(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_THESAURUS; }
صحيح ctrl_x_گهڻائي_cmdline(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_CMDLINE
		|| ctrl_x_گهڻائي == CTRL_X_CMDLINE_CTRL_X; }
صحيح ctrl_x_گهڻائي_function(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_FUNCTION; }
صحيح ctrl_x_گهڻائي_omni(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_OMNI; }
صحيح ctrl_x_گهڻائي_spell(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_SPELL; }
صحيح ctrl_x_گهڻائي_eval(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_EVAL; }
صحيح ctrl_x_گهڻائي_سٽ_or_eval(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_سڄو_سٽ || ctrl_x_گهڻائي == CTRL_X_EVAL; }
صحيح ctrl_x_گهڻائي_رجسٽر(فراغ)
    { أرجع ctrl_x_گهڻائي == CTRL_X_رجسٽر; }

/*
 * Whether other than default completion has been selected.
 */
    صحيح
ctrl_x_گهڻائي_not_default(فراغ)
{
    أرجع ctrl_x_گهڻائي != CTRL_X_عام;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    صحيح
ctrl_x_گهڻائي_not_defined_yet(فراغ)
{
    أرجع ctrl_x_گهڻائي == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    صحيح
compl_حالت_adding(فراغ)
{
    أرجع compl_cont_حالت & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    صحيح
compl_حالت_sol(فراغ)
{
    أرجع compl_cont_حالت & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    صحيح
compl_حالت_local(فراغ)
{
    أرجع compl_cont_حالت & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    فراغ
compl_حالت_clear(فراغ)
{
    compl_cont_حالت = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    ساكن صحيح
compl_dir_forward(فراغ)
{
    أرجع compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    ساكن صحيح
compl_shows_dir_forward(فراغ)
{
    أرجع compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    ساكن صحيح
compl_shows_dir_backward(فراغ)
{
    أرجع compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    صحيح
has_compl_option(صحيح dict_opt)
{
    إذا (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
							&& !curwin->w_p_spell
#جيڪڏهن_پورو
							)
		 : (*curbuf->b_p_tsr == NUL && *p_tsr == NUL
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
		     && *curbuf->b_p_tsrfu == NUL && *p_tsrfu == NUL
#جيڪڏهن_پورو
		   ))
    {
	ctrl_x_گهڻائي = CTRL_X_عام;
	edit_submode = عدم;
	نياپو_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	إذا (emsg_silent == 0 && !اندريون_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    ٻاهريون_flush();
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (!get_vim_var_nr(VV_TESTING))
#جيڪڏهن_پورو
		ui_delay(2004L, FALSE);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    صحيح
vim_is_ctrl_x_ڪنجي(صحيح c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    إذا (ins_compl_pum_ڪنجي(c))
	أرجع TRUE;

    بدل (ctrl_x_گهڻائي)
    {
	حالة 0:		    // Not in any CTRL-X mode
	    أرجع (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	حالة CTRL_X_NOT_DEFINED_YET:
	حالة CTRL_X_CMDLINE_CTRL_X:
	    أرجع (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	حالة CTRL_X_SCROLL:
	    أرجع (c == Ctrl_Y || c == Ctrl_E);
	حالة CTRL_X_سڄو_سٽ:
	    أرجع (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_FILES:
	    أرجع (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_DICTIONARY:
	    أرجع (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_THESAURUS:
	    أرجع (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_ٽيگ_فهرست:
	    أرجع (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
	حالة CTRL_X_رستو_PATTERNS:
	    أرجع (c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_رستو_مقرر_ڪري:
	    أرجع (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#جيڪڏهن_پورو
	حالة CTRL_X_CMDLINE:
	    أرجع (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
	حالة CTRL_X_FUNCTION:
	    أرجع (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_OMNI:
	    أرجع (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#جيڪڏهن_پورو
	حالة CTRL_X_SPELL:
	    أرجع (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_EVAL:
	    أرجع (c == Ctrl_P || c == Ctrl_N);
	حالة CTRL_X_رجسٽر:
	    أرجع (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    اندروني_error("vim_is_ctrl_x_key()");
    أرجع FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    ساكن صحيح
ميلاپ_at_original_متن(compl_T *ميلاپ)
{
    أرجع ميلاپ->cp_flags & CP_ORIGINAL_متن;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    ساكن صحيح
is_پهريون_ميلاپ(compl_T *ميلاپ)
{
    أرجع ميلاپ == compl_پهريون_ميلاپ;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    ساكن compl_T *
ڳوليو_original_متن_ميلاپ(فراغ)
{
    إذا (compl_پهريون_ميلاپ == عدم)
	أرجع عدم;
    إذا (ميلاپ_at_original_متن(compl_پهريون_ميلاپ))
	أرجع compl_پهريون_ميلاپ;
    إذا (compl_پهريون_ميلاپ->cp_prev != عدم
	    && ميلاپ_at_original_متن(compl_پهريون_ميلاپ->cp_prev))
	أرجع compl_پهريون_ميلاپ->cp_prev;
    أرجع عدم;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    صحيح
ins_compl_accept_char(صحيح c)
{
    إذا (compl_autocomplete && compl_from_nonkeyword)
	أرجع FALSE;

    إذا (ctrl_x_گهڻائي & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	أرجع vim_isIDc(c);

    بدل (ctrl_x_گهڻائي)
    {
	حالة CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    أرجع vim_isfilec(c) && !vim_ispathsep(c);

	حالة CTRL_X_CMDLINE:
	حالة CTRL_X_CMDLINE_CTRL_X:
	حالة CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    أرجع vim_isprintc(c) && !VIM_ISWHITE(c);

	حالة CTRL_X_سڄو_سٽ:
	    // For while line completion a space can be part of the line.
	    أرجع vim_isprintc(c);
    }
    أرجع vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    ساكن char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    صحيح	    char_len,
    صحيح	    compl_char_len,
    صحيح	    min_len,
    char_u  **tofree)
{
    صحيح		*wca;			// Wide character array.
    char_u	*p;
    صحيح		i, c;
    صحيح		has_هيٺاهون = FALSE;
    صحيح		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(صحيح, char_len);
    إذا (wca == عدم)
	أرجع IObuff;

    p = str;
    لكل (i = 0; i < char_len; ++i)
    {
	إذا (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	وإلا
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_متن.string;
    لكل (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	إذا (MB_ISLOWER(c))
	{
	    has_هيٺاهون = TRUE;
	    إذا (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		لكل (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		اكسر;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    إذا (!has_هيٺاهون)
    {
	p = compl_orig_متن.string;
	لكل (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    إذا (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		لكل (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		اكسر;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_متن.string;
    لكل (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	إذا (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	وإلا إذا (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    طالما (i < char_len)
    {
	إذا (gap.ga_معلومات != عدم)
	{
	    إذا (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		أرجع (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_معلومات + gap.ga_len;
	    إذا (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    وإلا
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	وإلا إذا ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    إذا (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		أرجع (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_معلومات, IObuff);
	    gap.ga_len = (صحيح)(p - IObuff);
	}
	وإلا إذا (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	وإلا
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    إذا (gap.ga_معلومات != عدم)
    {
	*tofree = gap.ga_معلومات;
	أرجع gap.ga_معلومات;
    }

    *p = NUL;
    أرجع IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    صحيح
ins_compl_جوڙيو_infercase(
    char_u	*str_arg,
    صحيح		len,
    صحيح		icase,
    char_u	*fname,
    صحيح		dir,
    صحيح		cont_s_ipos,  // next ^X<> will set initial_pos
    صحيح		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    صحيح		char_len;		// count multi-byte characters
    صحيح		compl_char_len;
    صحيح		min_len;
    صحيح		flags = 0;
    صحيح		res;
    char_u	*tofree = عدم;

    إذا (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	إذا (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    طالما (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	وإلا
	{
	    char_len = len;
	}

	// Find actual length of original text.
	إذا (has_mbyte)
	{
	    p = compl_orig_متن.string;
	    compl_char_len = 0;
	    طالما (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	وإلا
	{
	    compl_char_len = compl_ڊيگهائي;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    إذا (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    إذا (icase)
	flags |= CP_ICASE;

    res = ins_compl_جوڙيو(str, len, fname, عدم, عدم, dir, flags, FALSE, عدم, score);
    vim_free(tofree);
    أرجع res;
}

/*
 * Check if fuzzy matching is enabled
 */
    ساكن صحيح
cot_fuzzy(فراغ)
{
    أرجع (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_گهڻائي_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    ساكن صحيح
is_سڀ_کان_ويجهو_سرگرم(فراغ)
{
    أرجع (compl_autocomplete || (get_cot_flags() & COT_سڀ_کان_ويجهو))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    صحيح
ins_compl_is_ميلاپ_selected(فراغ)
{
    أرجع compl_shown_ميلاپ != عدم && !is_پهريون_ميلاپ(compl_shown_ميلاپ);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    صحيح
ins_compl_preinsert_longest(فراغ)
{
    أرجع compl_autocomplete
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
    ساكن صحيح
ins_compl_جوڙيو(
    char_u	*str,
    صحيح		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_معلومات اڻ_استعمال_ٿيل,  // "user_data" entry or NULL
    صحيح		cdir,
    صحيح		flags_arg,
    صحيح		adup,		    // accept duplicate match
    صحيح		*user_hl,           // user abbr/kind hlattr
    صحيح		score)
{
    compl_T	*ميلاپ, *current, *prev;
    صحيح		dir = (cdir == 0 ? compl_direction : cdir);
    صحيح		flags = flags_arg;
    صحيح		inserted = FALSE;
    char_u	*new_str = عدم;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    إذا (flags & CP_FAST)
	fast_breakcheck();
    وإلا
	ui_breakcheck();
    إذا (got_int)
	أرجع FAIL;
    إذا (len < 0)
	len = (صحيح)STRLEN(str);

    // If the same match is already present, don't add it.
    إذا (compl_پهريون_ميلاپ != عدم && !adup && compl_لڙيون_ht.ht_استعمال_ٿيل > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *ڪنجي;
	hashitem_T  *hi;

	إذا (len < (صحيح)حجم(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    ڪنجي = keybuf;
	}
	وإلا
	{
	    new_str = vim_strnsave(str, len);
	    إذا (new_str == عدم)
		أرجع FAIL;
	    ڪنجي = new_str;
	}
	str_hash = hash_hash(ڪنجي);
	hi = hash_lookup(&compl_لڙيون_ht, ڪنجي, str_hash);
	إذا (!HASHITEM_خالي_جايون(hi))
	{
	    إذا (is_سڀ_کان_ويجهو_سرگرم() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		ميلاپ = compl_پهريون_ميلاپ;
		افعل
		{
		    إذا (!ميلاپ_at_original_متن(ميلاپ)
			    && STRNCMP(ميلاپ->cp_str.string, str, len) == 0
			    && ((صحيح)ميلاپ->cp_str.ڊيگهائي <= len
					  || ميلاپ->cp_str.string[len] == NUL))
		    {
			إذا (score < ميلاپ->cp_score)
			    ميلاپ->cp_score = score;
			اكسر;
		    }
		    ميلاپ = ميلاپ->cp_ايندڙ;
		} طالما (ميلاپ != عدم && !is_پهريون_ميلاپ(ميلاپ));
	    }
	    vim_free(new_str);
	    أرجع NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    ميلاپ = ALLOC_CLEAR_ONE(compl_T);
    إذا (ميلاپ == عدم)
    {
	vim_free(new_str);
	أرجع FAIL;
    }
    ميلاپ->cp_عدد = flags & CP_ORIGINAL_متن ? 0 : -1;
    ميلاپ->cp_ترتيب = -1;
    إذا (new_str == عدم && (new_str = vim_strnsave(str, len)) == عدم)
    {
	vim_free(ميلاپ);
	أرجع FAIL;
    }
    ميلاپ->cp_str.string = new_str;
    ميلاپ->cp_str.ڊيگهائي = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    إذا (fname != عدم
	    && compl_curr_ميلاپ != عدم
	    && compl_curr_ميلاپ->cp_fname != عدم
	    && STRCMP(fname, compl_curr_ميلاپ->cp_fname) == 0)
	ميلاپ->cp_fname = compl_curr_ميلاپ->cp_fname;
    وإلا إذا (fname != عدم)
    {
	ميلاپ->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    وإلا
	ميلاپ->cp_fname = عدم;
    ميلاپ->cp_flags = flags;
    ميلاپ->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    ميلاپ->cp_user_قسم_hl_id = user_hl ? user_hl[1] : 0;
    ميلاپ->cp_score = score;
    ميلاپ->cp_cpt_ذريعو_idx = cpt_sources_اشاريو;

    إذا (cptext != عدم)
    {
	لكل (صحيح i = 0; i < CPT_ڳڻپ; ++i)
	{
	    إذا (cptext[i] != عدم && *cptext[i] != NUL)
		ميلاپ->cp_متن[i] = vim_strsave(cptext[i]);
	}
    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (user_معلومات != عدم)
	ميلاپ->cp_user_معلومات = *user_معلومات;
#جيڪڏهن_پورو

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    إذا (compl_پهريون_ميلاپ == عدم)
	ميلاپ->cp_ايندڙ = ميلاپ->cp_prev = عدم;
    وإلا إذا (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_پهريون_ميلاپ->cp_ايندڙ;
	prev = compl_پهريون_ميلاپ;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	طالما (current != عدم && current != compl_پهريون_ميلاپ)
	{
	    إذا (current->cp_score < score)
	    {
		ميلاپ->cp_ايندڙ = current;
		ميلاپ->cp_prev = current->cp_prev;
		إذا (current->cp_prev)
		    current->cp_prev->cp_ايندڙ = ميلاپ;
		current->cp_prev = ميلاپ;
		inserted = TRUE;
		اكسر;
	    }
	    prev = current;
	    current = current->cp_ايندڙ;
	}
	إذا (!inserted)
	{
	    prev->cp_ايندڙ = ميلاپ;
	    ميلاپ->cp_prev = prev;
	    ميلاپ->cp_ايندڙ = compl_پهريون_ميلاپ;
	    compl_پهريون_ميلاپ->cp_prev = ميلاپ;
	}
    }
    وإلا إذا (dir == FORWARD)
    {
	ميلاپ->cp_ايندڙ = compl_curr_ميلاپ->cp_ايندڙ;
	ميلاپ->cp_prev = compl_curr_ميلاپ;
    }
    وإلا	// BACKWARD
    {
	ميلاپ->cp_ايندڙ = compl_curr_ميلاپ;
	ميلاپ->cp_prev = compl_curr_ميلاپ->cp_prev;
    }
    إذا (ميلاپ->cp_ايندڙ)
	ميلاپ->cp_ايندڙ->cp_prev = ميلاپ;
    إذا (ميلاپ->cp_prev)
	ميلاپ->cp_prev->cp_ايندڙ = ميلاپ;
    وإلا	// if there's nothing before, it is the first match
	compl_پهريون_ميلاپ = ميلاپ;
    compl_curr_ميلاپ = ميلاپ;

    إذا (!ميلاپ_at_original_متن(ميلاپ))
	compl_لڙيون_جوڙيو(ميلاپ->cp_str.string, ميلاپ->cp_str.ڊيگهائي,
								    str_hash);

    // Find the longest common string if still doing that.
    إذا (compl_get_longest && (flags & CP_ORIGINAL_متن) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_گهڻائي_thesaurus())
	ins_compl_longest_ميلاپ(ميلاپ);

    أرجع OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    ساكن صحيح
ins_compl_equal(compl_T *ميلاپ, char_u *str, صحيح len)
{
    إذا (ميلاپ->cp_flags & CP_EQUAL)
	أرجع TRUE;
    إذا (ميلاپ->cp_flags & CP_ICASE)
	أرجع STRNICMP(ميلاپ->cp_str.string, str, (size_t)len) == 0;
    أرجع STRNCMP(ميلاپ->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    ساكن صحيح
ins_compl_equal_sc(compl_T *ميلاپ, char_u *str, صحيح len)
{
    صحيح	typed = compl_ڊيگهائي;
    صحيح	longest_پڇاڙي = (compl_get_longest && compl_longest_پڇاڙي_کانو > compl_کانو)
			    ? (صحيح)(compl_longest_پڇاڙي_کانو - compl_کانو) : typed;

    إذا ((ميلاپ->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_پڇاڙي <= typed)
	أرجع ins_compl_equal(ميلاپ, str, len);

    إذا ((صحيح)ميلاپ->cp_str.ڊيگهائي < len)
	أرجع FALSE;

    لكل (صحيح i = 0; i < len; ++i)
    {
	إذا (i >= typed && i < longest_پڇاڙي
		? TOLOWER_LOC(ميلاپ->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: ميلاپ->cp_str.string[i] != str[i])
	    أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    ساكن فراغ
ins_compl_وجهو_بائيٽون(char_u *p, صحيح len)
{
    إذا (len == -1)
	len = (صحيح)STRLEN(p);
    ins_بائيٽون_len(p, len);
    compl_ins_پڇاڙي_کانو = curwin->w_cursor.کانو;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(فراغ)
{
    أرجع compl_leader.string != عدم ? compl_leader.string : compl_orig_متن.string;
}

/*
 * Get current completion leader length
 */
    ساكن size_t
ins_compl_leader_len(فراغ)
{
    أرجع compl_leader.string != عدم ? compl_leader.ڊيگهائي : compl_orig_متن.ڊيگهائي;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    صحيح
ins_compl_کانو_range_attr(linenr_T lnum, صحيح کانو)
{
    صحيح	    شروعات_کانو;
    صحيح	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    صحيح	    attr;

    إذا (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	أرجع -1;

    شروعات_کانو = compl_کانو + (صحيح)ins_compl_leader_len();
    إذا (!ins_compl_has_گڻو())
	أرجع (کانو >= شروعات_کانو && کانو < compl_ins_پڇاڙي_کانو) ? attr : -1;

    // Multiple lines
    إذا ((lnum == compl_lnum && کانو >= شروعات_کانو && کانو < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && کانو <= compl_ins_پڇاڙي_کانو))
	أرجع attr;

    أرجع -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    ساكن صحيح
ins_compl_has_گڻو(فراغ)
{
    أرجع vim_strchr(compl_shown_ميلاپ->cp_str.string, '\n') != عدم;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    صحيح
ins_compl_lnum_اندريون_range(linenr_T lnum)
{
    إذا (!ins_compl_has_گڻو())
	أرجع FALSE;
    أرجع lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    ساكن فراغ
ins_compl_longest_ميلاپ(compl_T *ميلاپ)
{
    char_u	*p, *s;
    صحيح		c1, c2;
    صحيح		had_ميلاپ;

    إذا (compl_leader.string == عدم)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(ميلاپ->cp_str.string, ميلاپ->cp_str.ڊيگهائي);
	إذا (compl_leader.string == عدم)
	    أرجع;

	compl_leader.ڊيگهائي = ميلاپ->cp_str.ڊيگهائي;
	had_ميلاپ = (curwin->w_cursor.کانو > compl_کانو);
	ins_compl_longest_وجهو(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	إذا (!had_ميلاپ)
	    ins_compl_delete();
	compl_استعمال_ٿيل_ميلاپ = FALSE;

	أرجع;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = ميلاپ->cp_str.string;
    طالما (*p != NUL)
    {
	إذا (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	وإلا
	{
	    c1 = *p;
	    c2 = *s;
	}
	إذا ((ميلاپ->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    اكسر;
	إذا (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	وإلا
	{
	    ++p;
	    ++s;
	}
    }

    إذا (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.ڊيگهائي = (size_t)(p - compl_leader.string);

	had_ميلاپ = (curwin->w_cursor.کانو > compl_کانو);
	ins_compl_longest_وجهو(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	إذا (!had_ميلاپ)
	    ins_compl_delete();
    }

    compl_استعمال_ٿيل_ميلاپ = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    ساكن فراغ
ins_compl_جوڙيو_matches(
    صحيح		انگ_matches,
    char_u	**matches,
    صحيح		icase)
{
    صحيح		جوڙيو_r = OK;
    صحيح		dir = compl_direction;

    لكل (صحيح i = 0; i < انگ_matches && جوڙيو_r != FAIL; i++)
    {
	جوڙيو_r = ins_compl_جوڙيو(matches[i], -1, عدم, عدم, عدم, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, عدم,
		FUZZY_SCORE_NONE);
	إذا (جوڙيو_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(انگ_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    ساكن صحيح
ins_compl_make_cyclic(فراغ)
{
    compl_T *ميلاپ;
    صحيح	    ڳڻپ = 0;

    إذا (compl_پهريون_ميلاپ == عدم)
	أرجع 0;

    // Find the end of the list.
    ميلاپ = compl_پهريون_ميلاپ;
    // there's always an entry for the compl_orig_text, it doesn't count.
    طالما (ميلاپ->cp_ايندڙ != عدم && !is_پهريون_ميلاپ(ميلاپ->cp_ايندڙ))
    {
	ميلاپ = ميلاپ->cp_ايندڙ;
	++ڳڻپ;
    }
    ميلاپ->cp_ايندڙ = compl_پهريون_ميلاپ;
    compl_پهريون_ميلاپ->cp_prev = ميلاپ;

    أرجع ڳڻپ;
}

/*
 * Return whether there currently is a shown match.
 */
    صحيح
ins_compl_has_shown_ميلاپ(فراغ)
{
    أرجع compl_shown_ميلاپ == عدم
	|| compl_shown_ميلاپ != compl_shown_ميلاپ->cp_ايندڙ;
}

/*
 * Return whether the shown match is long enough.
 */
    صحيح
ins_compl_long_shown_ميلاپ(فراغ)
{
    أرجع (صحيح)compl_shown_ميلاپ->cp_str.ڊيگهائي
					    > curwin->w_cursor.کانو - compl_کانو;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    بنانشان صحيح
get_cot_flags(فراغ)
{
    أرجع curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    ساكن فراغ
ins_compl_upd_pum(فراغ)
{
    إذا (compl_ميلاپ_array == عدم)
	أرجع;

    صحيح h = curwin->w_cline_اوچائي;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    إذا (h != curwin->w_cline_اوچائي)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    ساكن فراغ
ins_compl_del_pum(فراغ)
{
    إذا (compl_ميلاپ_array == عدم)
	أرجع;

    pum_undisplay();
    VIM_CLEAR(compl_ميلاپ_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    صحيح
pum_wanted(فراغ)
{
    // 'completeopt' must contain "menu" or "menuone"
    أرجع (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    ساكن صحيح
pum_enough_matches(فراغ)
{
    compl_T     *compl;
    صحيح		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_پهريون_ميلاپ;
    افعل
    {
	إذا (compl == عدم || (!ميلاپ_at_original_متن(compl) && ++i == 2))
	    اكسر;
	compl = compl->cp_ايندڙ;
    } طالما (!is_پهريون_ميلاپ(compl));

    إذا ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	أرجع (i >= 1);
    أرجع (i >= 2);
}

#إذا defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    ساكن dict_T *
ins_compl_dict_alloc(compl_T *ميلاپ)
{
    dict_T *dict = dict_alloc_تالو(VAR_FIXED);

    إذا (dict == عدم)
	أرجع عدم;
    fill_complete_info_dict(dict, ميلاپ, FALSE);
    أرجع dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    ساكن فراغ
trigger_complete_بدليو_event(صحيح cur)
{
    dict_T	    *v_event;
    dict_T	    *شي;
    ساكن صحيح	    recursive = FALSE;
    save_v_event_T  save_v_event;

    إذا (recursive)
	أرجع;

    شي = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_ميلاپ);
    إذا (شي == عدم)
	أرجع;
    v_event = get_v_event(&save_v_event);
    dict_جوڙيو_dict(v_event, "completed_item", شي);
    pum_سيٽ_event_info(v_event);
    dict_سيٽ_شيون_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, عدم, عدم, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#جيڪڏهن_پورو

/*
 * Helper functions for mergesort_list().
 */
    ساكن فراغ*
cp_get_ايندڙ(فراغ *ڳنڍ)
{
    أرجع ((compl_T*)ڳنڍ)->cp_ايندڙ;
}

    ساكن فراغ
cp_سيٽ_ايندڙ(فراغ *ڳنڍ, فراغ *ايندڙ)
{
    ((compl_T*)ڳنڍ)->cp_ايندڙ = (compl_T*)ايندڙ;
}

    ساكن فراغ*
cp_get_prev(فراغ* ڳنڍ)
{
    أرجع ((compl_T*)ڳنڍ)->cp_prev;
}

    ساكن فراغ
cp_سيٽ_prev(فراغ* ڳنڍ, فراغ* prev)
{
    ((compl_T*)ڳنڍ)->cp_prev = (compl_T*)prev;
}

    ساكن صحيح
cp_compare_fuzzy(ثابت فراغ* a, ثابت فراغ* b)
{
    صحيح score_a = ((compl_T*)a)->cp_score;
    صحيح score_b = ((compl_T*)b)->cp_score;

    إذا (score_a != score_b)
	أرجع (score_b > score_a) ? 1 : -1;

    أرجع ((compl_T *)a)->cp_ترتيب - ((compl_T *)b)->cp_ترتيب;
}

    ساكن صحيح
cp_compare_سڀ_کان_ويجهو(ثابت فراغ* a, ثابت فراغ* b)
{
    صحيح score_a = ((compl_T*)a)->cp_score;
    صحيح score_b = ((compl_T*)b)->cp_score;
    إذا (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	أرجع 0;
    أرجع (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    ساكن صحيح
prepend_startcol_متن(string_T *dest, string_T *src, صحيح startcol)
{
    صحيح prepend_len = compl_کانو - startcol;
    صحيح new_ڊيگهائي = prepend_len + (صحيح)src->ڊيگهائي;

    dest->ڊيگهائي = (size_t)new_ڊيگهائي;
    dest->string = alloc(new_ڊيگهائي + 1);  // +1 for NUL
    إذا (dest->string == عدم)
    {
	dest->ڊيگهائي = 0;
	أرجع FAIL;
    }

    char_u	*سٽ = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, سٽ + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->ڊيگهائي);
    dest->string[new_ڊيگهائي] = NUL;
    أرجع OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    ساكن string_T *
get_leader_for_startcol(compl_T *ميلاپ, صحيح cached)
{
    ساكن string_T adjusted_leader = {عدم, 0};

    إذا (ميلاپ == عدم)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	أرجع عدم;
    }

    إذا (cpt_sources_array == عدم)
	اذهب theend;

    صحيح	cpt_idx = ميلاپ->cp_cpt_ذريعو_idx;
    إذا (cpt_idx < 0)
	اذهب theend;
    صحيح	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    إذا (compl_leader.string == عدم)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	إذا (startcol < 0 || startcol >= compl_کانو)
	    أرجع &compl_orig_متن;
	أرجع &compl_leader;  // pass through (startcol < compl_col)
    }

    إذا (compl_کانو <= 0)
	اذهب theend;

    إذا (startcol >= 0 && startcol < compl_کانو)
    {
	صحيح prepend_len = compl_کانو - startcol;
	صحيح new_ڊيگهائي = prepend_len + (صحيح)compl_leader.ڊيگهائي;
	إذا (cached && (size_t)new_ڊيگهائي == adjusted_leader.ڊيگهائي
		&& adjusted_leader.string != عدم)
	    أرجع &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	إذا (prepend_startcol_متن(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    اذهب theend;

	أرجع &adjusted_leader;
    }
theend:
    أرجع &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    ساكن فراغ
سيٽ_fuzzy_score(فراغ)
{
    compl_T *compl;
    char_u  *نمونو;
    صحيح	    use_leader;

    إذا (compl_پهريون_ميلاپ == عدم)
	أرجع;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != عدم && compl_leader.ڊيگهائي > 0);
    إذا (!use_leader)
    {
	إذا (compl_orig_متن.string == عدم || compl_orig_متن.ڊيگهائي == 0)
	    أرجع;
	نمونو = compl_orig_متن.string;
    }
    وإلا
    {
	/* Clear the leader cache once before the loop */
	(فراغ)get_leader_for_startcol(عدم, TRUE);
	نمونو = عدم;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    صحيح ايندڙ = 0;
    compl = compl_پهريون_ميلاپ;
    افعل
    {
	إذا (compl->cp_ترتيب < 0)
	    compl->cp_ترتيب = ايندڙ;
	ايندڙ = MAX(ايندڙ, compl->cp_ترتيب + 1);

	إذا (use_leader)
	    نمونو = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_ميلاپ_str(compl->cp_str.string, نمونو);
	compl = compl->cp_ايندڙ;
    } طالما (compl != عدم && !is_پهريون_ميلاپ(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    ساكن فراغ
ترتيب_ڏيو_compl_ميلاپ_list(صحيح (*compare)(ثابت فراغ *, ثابت فراغ *))
{
    compl_T	*orig_متن;

    إذا (!compl_پهريون_ميلاپ || is_پهريون_ميلاپ(compl_پهريون_ميلاپ->cp_ايندڙ))
	أرجع;

    orig_متن = ڳوليو_original_متن_ميلاپ();
    إذا (orig_متن == عدم)
	أرجع;

    ins_compl_make_لڪيري();
    إذا (orig_متن == compl_پهريون_ميلاپ)
    {
	compl_پهريون_ميلاپ->cp_ايندڙ->cp_prev = عدم;
	compl_پهريون_ميلاپ->cp_ايندڙ = mergesort_list(compl_پهريون_ميلاپ->cp_ايندڙ,
		cp_get_ايندڙ, cp_سيٽ_ايندڙ, cp_get_prev, cp_سيٽ_prev, compare);
	compl_پهريون_ميلاپ->cp_ايندڙ->cp_prev = compl_پهريون_ميلاپ;
    }
    وإلا
    {
	compl_T	*پڇ;

	orig_متن->cp_prev->cp_ايندڙ = عدم;
	compl_پهريون_ميلاپ = mergesort_list(compl_پهريون_ميلاپ, cp_get_ايندڙ,
		cp_سيٽ_ايندڙ, cp_get_prev, cp_سيٽ_prev, compare);
	پڇ = compl_پهريون_ميلاپ;
	طالما (پڇ->cp_ايندڙ != عدم)
	    پڇ = پڇ->cp_ايندڙ;
	پڇ->cp_ايندڙ = orig_متن;
	orig_متن->cp_prev = پڇ;
    }
    (فراغ)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    ساكن صحيح
get_user_highlight_attr(صحيح hl_id)
{
    صحيح	    attr;

    إذا (hl_id <= 0)
	أرجع -1;
    attr = syn_id2attr(hl_id);
    أرجع attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    ساكن صحيح
ins_compl_build_pum(فراغ)
{
    compl_T     *compl;
    compl_T     *shown_compl = عدم;
    صحيح		did_ڳوليو_shown_ميلاپ = FALSE;
    صحيح		shown_ميلاپ_ok = FALSE;
    صحيح		i = 0;
    صحيح		cur = -1;
    صحيح		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*ميلاپ_سر = عدم;
    compl_T	*ميلاپ_پڇ = عدم;
    compl_T	*ميلاپ_ايندڙ = عدم;
    صحيح		*ميلاپ_ڳڻپ = عدم;
    صحيح		is_forward = compl_shows_dir_forward();
    صحيح		is_cpt_completion = (cpt_sources_array != عدم);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_ميلاپ_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    إذا (ميلاپ_at_original_متن(compl_shown_ميلاپ))
	shown_ميلاپ_ok = TRUE;

    إذا (compl_leader.string != عدم
	    && STRCMP(compl_leader.string, compl_orig_متن.string) == 0
	    && shown_ميلاپ_ok == FALSE)
	compl_shown_ميلاپ = compl_no_select ? compl_پهريون_ميلاپ
					    : compl_پهريون_ميلاپ->cp_ايندڙ;

    إذا (is_cpt_completion)
    {
	ميلاپ_ڳڻپ = ALLOC_CLEAR_MULT(صحيح, cpt_sources_ڳڻپ);
	إذا (ميلاپ_ڳڻپ == عدم)
	    أرجع -1;
    }

    (فراغ)get_leader_for_startcol(عدم, TRUE); // Clear the cache

    compl = compl_پهريون_ميلاپ;
    افعل
    {
	compl->cp_اندريون_ميلاپ_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	إذا (ctrl_x_گهڻائي_عام() && !p_inf && compl_orig_متن.string
		&& !ignorecase(compl_orig_متن.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	إذا (!ميلاپ_at_original_متن(compl)
		&& (leader->string == عدم
		    || ins_compl_equal_sc(compl, leader->string,
			(صحيح)leader->ڊيگهائي)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    صحيح ميلاپ_حد_بندي_exceeded = FALSE;
	    صحيح cur_ذريعو = compl->cp_cpt_ذريعو_idx;
	    إذا (is_forward && cur_ذريعو != -1 && is_cpt_completion)
	    {
		ميلاپ_ڳڻپ[cur_ذريعو]++;
		صحيح max_matches = cpt_sources_array[cur_ذريعو].cs_max_matches;
		إذا (max_matches > 0 && ميلاپ_ڳڻپ[cur_ذريعو] > max_matches)
		    ميلاپ_حد_بندي_exceeded = TRUE;
	    }

	    إذا (!ميلاپ_حد_بندي_exceeded)
	    {
		++compl_ميلاپ_arraysize;
		compl->cp_اندريون_ميلاپ_array = TRUE;
		إذا (ميلاپ_سر == عدم)
		    ميلاپ_سر = compl;
		وإلا
		    ميلاپ_پڇ->cp_ميلاپ_ايندڙ = compl;
		ميلاپ_پڇ = compl;

		إذا (!shown_ميلاپ_ok && !cot_fuzzy())
		{
		    إذا (compl == compl_shown_ميلاپ || did_ڳوليو_shown_ميلاپ)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_ميلاپ = compl;
			did_ڳوليو_shown_ميلاپ = TRUE;
			shown_ميلاپ_ok = TRUE;
		    }
		    وإلا
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		وإلا إذا (cot_fuzzy())
		{
		    إذا (i == 0)
			shown_compl = compl;

		    إذا (!shown_ميلاپ_ok && compl == compl_shown_ميلاپ)
		    {
			cur = i;
			shown_ميلاپ_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	إذا (compl == compl_shown_ميلاپ && !cot_fuzzy())
	{
	    did_ڳوليو_shown_ميلاپ = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    إذا (ميلاپ_at_original_متن(compl))
		shown_ميلاپ_ok = TRUE;

	    إذا (!shown_ميلاپ_ok && shown_compl != عدم)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_ميلاپ = shown_compl;
		shown_ميلاپ_ok = TRUE;
	    }
	}
	compl = compl->cp_ايندڙ;
    } طالما (compl != عدم && !is_پهريون_ميلاپ(compl));

    vim_free(ميلاپ_ڳڻپ);

    إذا (compl_ميلاپ_arraysize == 0)
	أرجع -1;

    إذا (cot_fuzzy() && !compl_no_select && !shown_ميلاپ_ok)
    {
	compl_shown_ميلاپ = shown_compl;
	shown_ميلاپ_ok = TRUE;
	cur = 0;
    }

    compl_ميلاپ_array = ALLOC_CLEAR_MULT(pumitem_T, compl_ميلاپ_arraysize);
    إذا (compl_ميلاپ_array == عدم)
	أرجع -1;

    compl = ميلاپ_سر;
    i = 0;
    طالما (compl != عدم)
    {
	compl_ميلاپ_array[i].pum_متن = compl->cp_متن[CPT_ABBR] != عدم
			    ? compl->cp_متن[CPT_ABBR] : compl->cp_str.string;
	compl_ميلاپ_array[i].pum_قسم = compl->cp_متن[CPT_قسم];
	compl_ميلاپ_array[i].pum_info = compl->cp_متن[CPT_INFO];
	compl_ميلاپ_array[i].pum_cpt_ذريعو_idx = compl->cp_cpt_ذريعو_idx;
	compl_ميلاپ_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_ميلاپ_array[i].pum_user_قسم_hlattr =
			get_user_highlight_attr(compl->cp_user_قسم_hl_id);
	compl_ميلاپ_array[i++].pum_extra = compl->cp_متن[CPT_MENU] != عدم
			    ? compl->cp_متن[CPT_MENU] : compl->cp_fname;
	ميلاپ_ايندڙ = compl->cp_ميلاپ_ايندڙ;
	compl->cp_ميلاپ_ايندڙ = عدم;
	compl = ميلاپ_ايندڙ;
    }

    إذا (!shown_ميلاپ_ok)    // no displayed match at all
	cur = -1;

    أرجع cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    فراغ
ins_compl_show_pum(فراغ)
{
    صحيح		i;
    صحيح		cur = -1;
    colnr_T	کانو;

    إذا (!pum_wanted() || !pum_enough_matches())
	أرجع;

    // Avoid redrawing the screen under a pum that stays in place.
    إذا (!pum_redraw_اندريون_same_position())
	pum_call_update_screen();

    إذا (compl_ميلاپ_array == عدم)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    وإلا
    {
	// popup menu already exists, only need to find the current item.
	لكل (i = 0; i < compl_ميلاپ_arraysize; ++i)
	{
	    إذا (compl_ميلاپ_array[i].pum_متن == compl_shown_ميلاپ->cp_str.string
		    || compl_ميلاپ_array[i].pum_متن
				      == compl_shown_ميلاپ->cp_متن[CPT_ABBR])
	    {
		cur = i;
		اكسر;
	    }
	}
    }

    إذا (compl_ميلاپ_array == عدم)
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (compl_started && has_completechanged())
	    trigger_complete_بدليو_event(cur);
#جيڪڏهن_پورو
	أرجع;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    کانو = curwin->w_cursor.کانو;
    curwin->w_cursor.کانو = compl_کانو;
    validate_cursor_کانو();
    صحيح pum_wcol = curwin->w_wcol;
    curwin->w_cursor.کانو = کانو;
    validate_cursor_کانو();
    compl_selected_شي = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_حالت = true;
    pum_display(compl_ميلاپ_array, compl_ميلاپ_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    إذا (compl_started && compl_curr_ميلاپ != compl_shown_ميلاپ)
	compl_curr_ميلاپ = compl_shown_ميلاپ;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (has_completechanged())
	trigger_complete_بدليو_event(cur);
#جيڪڏهن_پورو
}

#وصف_ڪريو DICT_پهريون	(1)	// use just first element in "dict"
#وصف_ڪريو DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    ساكن فراغ
ins_compl_dictionaries(
    char_u	*dict_شروعات,
    char_u	*pat,
    صحيح		flags,		// DICT_FIRST and/or DICT_EXACT
    صحيح		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_شروعات;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    صحيح		ڳڻپ;
    صحيح		save_p_scs;
    صحيح		dir = compl_direction;

    إذا (*dict == NUL)
    {
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	إذا (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	وإلا
#جيڪڏهن_پورو
	    أرجع;
    }

    buf = alloc(LSIZE);
    إذا (buf == عدم)
	أرجع;
    regmatch.regprog = عدم;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    إذا (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    إذا (ctrl_x_گهڻائي_سٽ_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	إذا (pat_esc == عدم)
	    اذهب theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	إذا (ptr == عدم)
	{
	    vim_free(pat_esc);
	    اذهب theend;
	}
	vim_snprintf((حرف *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    وإلا
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	إذا (regmatch.regprog == عدم)
	    اذهب theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    طالما (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	إذا (flags == DICT_EXACT)
	{
	    ڳڻپ = 1;
	    files = &dict;
	}
	وإلا
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
	    إذا (!thesaurus && STRCMP(buf, "spell") == 0)
		ڳڻپ = -1;
	    وإلا
#جيڪڏهن_پورو
		إذا (vim_strchr(buf, '`') != عدم
		    || expand_wildcards(1, &buf, &ڳڻپ, &files,
						     EW_FILE|EW_SILENT) != OK)
		ڳڻپ = 0;
	}

#جيڪڏهن_وصف_ٿيل FEAT_SPELL
	إذا (ڳڻپ == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    إذا (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    وإلا
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	وإلا
#جيڪڏهن_پورو
	    إذا (ڳڻپ > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(ڳڻپ, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    إذا (flags != DICT_EXACT)
		FreeWild(ڳڻپ, files);
	}
	إذا (flags != 0)
	    اكسر;
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
    ساكن صحيح
thesaurus_جوڙيو_لفظ_اندريون_سٽ(
    char_u	*fname,
    char_u	**buf_arg,
    صحيح		dir,
    char_u	*skip_لفظ)
{
    صحيح		حالت = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    طالما (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = ڳوليو_لفظ_شروعات(ptr);
	إذا (*ptr == NUL || *ptr == NL)
	    اكسر;
	wstart = ptr;

	// Find end of the word.
	إذا (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    طالما (*ptr != NUL)
	    {
		صحيح l = (*mb_ptr2len)(ptr);

		إذا (l < 2 && !vim_iswordc(*ptr))
		    اكسر;
		ptr += l;
	    }
	وإلا
	    ptr = ڳوليو_لفظ_پڇاڙي(ptr);

	// Add the word. Skip the regexp match.
	إذا (wstart != skip_لفظ)
	{
	    حالت = ins_compl_جوڙيو_infercase(wstart, (صحيح)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    إذا (حالت == FAIL)
		اكسر;
	}
    }

    *buf_arg = ptr;
    أرجع حالت;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    ساكن فراغ
ins_compl_files(
    صحيح		ڳڻپ,
    char_u	**files,
    صحيح		thesaurus,
    صحيح		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    صحيح		*dir)
{
    char_u	*ptr;
    صحيح		i;
    FILE	*fp;
    صحيح		جوڙيو_r;
    char_u	*leader = عدم;
    صحيح		leader_len = 0;
    صحيح		score = 0;
    صحيح		len = 0;
    char_u	*سٽ_پڇاڙي = عدم;

    إذا (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (صحيح)ins_compl_leader_len();
    }

    لكل (i = 0; i < ڳڻپ && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((حرف *)files[i], "r");  // open dictionary file
	إذا (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    نياپو_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((حرف *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (حرف *)files[i]);
	    (فراغ)نياپو_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	إذا (fp == عدم)
	    استمر;

	// Read dictionary file line by line.
	// Check each line for a match.
	طالما (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    إذا (cot_fuzzy() && leader_len > 0)
	    {
		سٽ_پڇاڙي = ڳوليو_سٽ_پڇاڙي(ptr);
		طالما (ptr < سٽ_پڇاڙي)
		{
		    إذا (fuzzy_ميلاپ_str_اندريون_سٽ(&ptr, leader, &len, عدم, &score))
		    {
			char_u *end_ptr = ctrl_x_گهڻائي_سٽ_or_eval()
					? ڳوليو_سٽ_پڇاڙي(ptr) : ڳوليو_لفظ_پڇاڙي(ptr);
			جوڙيو_r = ins_compl_جوڙيو_infercase(ptr, (صحيح)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			إذا (جوڙيو_r == FAIL)
			    اكسر;
			ptr = end_ptr;  // start from next word
			إذا (compl_get_longest && ctrl_x_گهڻائي_عام()
				&& compl_پهريون_ميلاپ->cp_ايندڙ
				&& score == compl_پهريون_ميلاپ->cp_ايندڙ->cp_score)
			    compl_انگ_bests++;
		    }
		}
	    }
	    وإلا إذا (regmatch != عدم)
	    {
		طالما (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_گهڻائي_سٽ_or_eval() ? ڳوليو_سٽ_پڇاڙي(ptr)
						    : ڳوليو_لفظ_پڇاڙي(ptr);
		    جوڙيو_r = ins_compl_جوڙيو_infercase(regmatch->startp[0],
			    (صحيح)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    إذا (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			جوڙيو_r = thesaurus_جوڙيو_لفظ_اندريون_سٽ(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    إذا (جوڙيو_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    وإلا إذا (جوڙيو_r == FAIL)
			اكسر;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    إذا (*ptr == '\n' || got_int)
			اكسر;
		}
	    }
	    سٽ_breakcheck();
	    ins_compl_check_ڪنجيون(50, FALSE);
	}
	fclose(fp);
    }
}

/*
 * Find the start of the next word.
 * Returns a pointer to the first char of the word.  Also stops at a NUL.
 */
    char_u *
ڳوليو_لفظ_شروعات(char_u *ptr)
{
    إذا (has_mbyte)
    {
	طالما (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    وإلا
    {
	طالما (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    أرجع ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
ڳوليو_لفظ_پڇاڙي(char_u *ptr)
{
    إذا (has_mbyte)
    {
	صحيح شروعات_class = mb_get_class(ptr);
	إذا (شروعات_class > 1)
	    طالما (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		إذا (mb_get_class(ptr) != شروعات_class)
		    اكسر;
	    }
    }
    وإلا
	طالما (vim_iswordc(*ptr))
	    ++ptr;
    أرجع ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
ڳوليو_سٽ_پڇاڙي(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    طالما (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    أرجع s;
}

/*
 * Free a completion item in the list
 */
    ساكن فراغ
ins_compl_شي_free(compl_T *ميلاپ)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    إذا (compl_لڙيون_ht.ht_استعمال_ٿيل > 0 && ميلاپ->cp_str.string != عدم
					   && !ميلاپ_at_original_متن(ميلاپ))
    {
	hashitem_T *hi = hash_ڳوليو(&compl_لڙيون_ht, ميلاپ->cp_str.string);

	إذا (!HASHITEM_خالي_جايون(hi))
	{
	    complstr_T *داخلا = HI2CSE(hi);

	    إذا (--داخلا->cse_ڳڻپ <= 0)
	    {
		hash_remove(&compl_لڙيون_ht, hi, "completion match");
		vim_free(داخلا);
	    }
	}
    }
    VIM_CLEAR_STRING(ميلاپ->cp_str);
    // several entries may use the same fname, free it just once.
    إذا (ميلاپ->cp_flags & CP_FREE_FNAME)
	vim_free(ميلاپ->cp_fname);
    لكل (صحيح i = 0; i < CPT_ڳڻپ; ++i)
	vim_free(ميلاپ->cp_متن[i]);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    clear_tv(&ميلاپ->cp_user_معلومات);
#جيڪڏهن_پورو
    vim_free(ميلاپ);
}

/*
 * Free the list of completions
 */
    ساكن فراغ
ins_compl_free(فراغ)
{
    compl_T *ميلاپ;

    VIM_CLEAR_STRING(compl_نمونو);
    VIM_CLEAR_STRING(compl_leader);

    إذا (compl_پهريون_ميلاپ == عدم)
	أرجع;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_لڙيون_ht, CSE_OFF);
    hash_init(&compl_لڙيون_ht);

    compl_curr_ميلاپ = compl_پهريون_ميلاپ;
    افعل
    {
	ميلاپ = compl_curr_ميلاپ;
	compl_curr_ميلاپ = compl_curr_ميلاپ->cp_ايندڙ;
	ins_compl_شي_free(ميلاپ);
    } طالما (compl_curr_ميلاپ != عدم && !is_پهريون_ميلاپ(compl_curr_ميلاپ));
    compl_پهريون_ميلاپ = compl_curr_ميلاپ = عدم;
    compl_shown_ميلاپ = عدم;
    compl_old_ميلاپ = عدم;
}

/*
 * Reset/clear the completion state.
 */
    فراغ
ins_compl_clear(فراغ)
{
    compl_cont_حالت = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_شي = -1;
    compl_ins_پڇاڙي_کانو = 0;
    compl_longest_پڇاڙي_کانو = 0;
    compl_curr_win = عدم;
    compl_curr_buf = عدم;
    VIM_CLEAR_STRING(compl_نمونو);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = عدم;
    VIM_CLEAR_STRING(compl_orig_متن);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_انگ_bests = 0;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // clear v:completed_item
    سيٽ_vim_var_dict(VV_COMPLETED_شي, dict_alloc_تالو(VAR_FIXED));
#جيڪڏهن_پورو
}

/*
 * Return TRUE when Insert completion is active.
 */
    صحيح
ins_compl_سرگرم(فراغ)
{
    أرجع compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    صحيح
ins_compl_win_سرگرم(win_T *wp)
{
    أرجع ins_compl_سرگرم() && wp == compl_curr_win
	&& wp->w_بفر == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    صحيح
ins_compl_استعمال_ٿيل_ميلاپ(فراغ)
{
    أرجع compl_استعمال_ٿيل_ميلاپ;
}

/*
 * Initialize get longest common string.
 */
    فراغ
ins_compl_init_get_longest(فراغ)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    صحيح
ins_compl_interrupted(فراغ)
{
    أرجع compl_interrupted || compl_وقت_ٽڪرو_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    صحيح
ins_compl_enter_selects(فراغ)
{
    أرجع compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_کانو(فراغ)
{
    أرجع compl_کانو;
}

/*
 * Return the length in bytes of the text being completed
 */
    صحيح
ins_compl_len(فراغ)
{
    أرجع compl_ڊيگهائي;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    صحيح
ins_compl_has_preinsert(فراغ)
{
    صحيح cur_cot_flags = get_cot_flags();
    إذا (compl_autocomplete && p_ic && !p_inf)
	أرجع FALSE;
    أرجع !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    صحيح
ins_compl_preinsert_effect(فراغ)
{
    إذا (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	أرجع FALSE;

    أرجع curwin->w_cursor.کانو < compl_ins_پڇاڙي_کانو;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    صحيح
ins_compl_bs(فراغ)
{
    char_u	*سٽ;
    char_u	*p;

    إذا (ins_compl_preinsert_effect())
	ins_compl_delete();

    سٽ = ml_get_curline();
    p = سٽ + curwin->w_cursor.کانو;
    MB_PTR_پٺ(سٽ, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    إذا ((صحيح)(p - سٽ) - (صحيح)compl_کانو < 0
	    || ((صحيح)(p - سٽ) - (صحيح)compl_کانو == 0 && !ctrl_x_گهڻائي_omni())
	    || ctrl_x_گهڻائي_eval()
	    || (!can_bs(BS_شروعات) && (صحيح)(p - سٽ) - (صحيح)compl_کانو
							- compl_ڊيگهائي < 0))
	أرجع K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    إذا (curwin->w_cursor.کانو <= compl_کانو + compl_ڊيگهائي
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.ڊيگهائي = (size_t)((p - سٽ) - compl_کانو);
    compl_leader.string = vim_strnsave(سٽ + compl_کانو, compl_leader.ڊيگهائي);
    إذا (compl_leader.string == عدم)
    {
	compl_leader.ڊيگهائي = 0;
	أرجع K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    إذا (compl_autocomplete && compl_پهريون_ميلاپ && !ins_compl_has_preinsert())
	compl_shown_ميلاپ = compl_پهريون_ميلاپ;

    ins_compl_new_leader();
    إذا (compl_shown_ميلاپ != عدم)
	// Make sure current match is not a hidden item.
	compl_curr_ميلاپ = compl_shown_ميلاپ;
    أرجع NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    ساكن صحيح
ins_compl_need_restart(فراغ)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    أرجع compl_was_interrupted
	|| ((ctrl_x_گهڻائي_function() || ctrl_x_گهڻائي_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    صحيح
ins_compl_has_autocomplete(فراغ)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    أرجع curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    ساكن فراغ
ins_compl_fuzzy_ترتيب_ڏيو(فراغ)
{
    صحيح	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    سيٽ_fuzzy_score();
    إذا (!(cur_cot_flags & COT_NOSORT))
    {
	ترتيب_ڏيو_compl_ميلاپ_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	إذا ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    صحيح none_selected = compl_shown_ميلاپ == (compl_shows_dir_forward()
		    ? compl_پهريون_ميلاپ : compl_پهريون_ميلاپ->cp_prev);
	    إذا (!none_selected)
		compl_shown_ميلاپ
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_پهريون_ميلاپ->cp_ايندڙ : compl_پهريون_ميلاپ;
	}
    }
}

/*
 * Called after changing "compl_leader".
 * Show the popup menu with a different set of matches.
 * May also search for matches again if the previous search was interrupted.
 */
    ساكن فراغ
ins_compl_new_leader(فراغ)
{
    صحيح	    save_w_wrow = curwin->w_wrow;
    صحيح	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_وجهو_بائيٽون(compl_leader.string + get_compl_len(), -1);
    compl_استعمال_ٿيل_ميلاپ = FALSE;

    إذا (compl_started)
    {
	ins_compl_سيٽ_original_متن(compl_leader.string, compl_leader.ڊيگهائي);
	إذا (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	إذا (cot_fuzzy())
	    ins_compl_fuzzy_ترتيب_ڏيو();
    }
    وإلا
    {
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#جيڪڏهن_پورو
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (gui.اندريون_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    ٻاهريون_flush_cursor(FALSE, FALSE);
	}
#جيڪڏهن_پورو
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	إذا (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	وإلا
	    compl_autocomplete = FALSE;
	إذا (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_حالت = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_استعمال_ٿيل_ميلاپ && compl_selected_شي != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    إذا (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    إذا (compl_ميلاپ_array == عدم)
	compl_enter_selects = FALSE;
    وإلا إذا (ins_compl_has_preinsert() && compl_leader.ڊيگهائي > 0)
	ins_compl_وجهو(TRUE, FALSE);
    وإلا إذا (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.ڊيگهائي > 0 && !ins_compl_preinsert_effect())
	ins_compl_وجهو(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    ساكن صحيح
get_compl_len(فراغ)
{
    صحيح off = (صحيح)curwin->w_cursor.کانو - (صحيح)compl_کانو;
    أرجع MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    فراغ
ins_compl_addleader(صحيح c)
{
    صحيح	    cc;

    إذا (ins_compl_preinsert_effect())
	ins_compl_delete();

    إذا (stop_arrow() == FAIL)
	أرجع;
    إذا (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_بائيٽون(buf, cc);
	إذا (compl_opt_refresh_always)
	    Append_منزل_Redobuff(buf);
    }
    وإلا
    {
	ins_char(c);
	إذا (compl_opt_refresh_always)
	    Append_Char_منزل_Redobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    إذا (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    إذا (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.ڊيگهائي = (size_t)(curwin->w_cursor.کانو - compl_کانو);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_کانو,
		compl_leader.ڊيگهائي);
	إذا (compl_leader.string == عدم)
	{
	    compl_leader.ڊيگهائي = 0;
	    أرجع;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    ساكن فراغ
ins_compl_restart(فراغ)
{
    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    compl_cont_حالت = 0;
    compl_cont_گهڻائي = 0;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_انگ_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    ساكن فراغ
ins_compl_سيٽ_original_متن(char_u *str, size_t len)
{
    compl_T	*ميلاپ = ڳوليو_original_متن_ميلاپ();
    char_u	*p;

    // Replace the original text entry.
    إذا (ميلاپ == عدم)
	أرجع;

    p = vim_strnsave(str, len);
    إذا (p == عدم)
	أرجع;

    VIM_CLEAR_STRING(ميلاپ->cp_str);
    ميلاپ->cp_str.string = p;
    ميلاپ->cp_str.ڊيگهائي = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    فراغ
ins_compl_addfrommatch(فراغ)
{
    char_u	*p;
    صحيح		len = (صحيح)curwin->w_cursor.کانو - (صحيح)compl_کانو;
    صحيح		c;

    p = compl_shown_ميلاپ->cp_str.string;
    إذا ((صحيح)compl_shown_ميلاپ->cp_str.ڊيگهائي <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	إذا (!ميلاپ_at_original_متن(compl_shown_ميلاپ))
	    أرجع;

	p = عدم;
	plen = 0;
	لكل (cp = compl_shown_ميلاپ->cp_ايندڙ; cp != عدم
		&& !is_پهريون_ميلاپ(cp); cp = cp->cp_ايندڙ)
	{
	    إذا (compl_leader.string == عدم
		    || ins_compl_equal(cp, compl_leader.string,
			(صحيح)compl_leader.ڊيگهائي))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.ڊيگهائي;
		اكسر;
	    }
	}
	إذا (p == عدم || (صحيح)plen <= len)
	    أرجع;
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
    ساكن صحيح
سيٽ_ctrl_x_گهڻائي(صحيح c)
{
    صحيح	    retval = FALSE;

    بدل (c)
    {
	حالة Ctrl_E:
	حالة Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_گهڻائي = CTRL_X_SCROLL;
	    إذا (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    وإلا
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = عدم;
	    showmode();
	    اكسر;
	حالة Ctrl_L:
	    // complete whole line
	    ctrl_x_گهڻائي = CTRL_X_سڄو_سٽ;
	    اكسر;
	حالة Ctrl_F:
	    // complete filenames
	    ctrl_x_گهڻائي = CTRL_X_FILES;
	    اكسر;
	حالة Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_گهڻائي = CTRL_X_DICTIONARY;
	    اكسر;
	حالة Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    إذا (vpeekc() == '=')
		اكسر;
	    ctrl_x_گهڻائي = CTRL_X_رجسٽر;
	    اكسر;
	حالة Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_گهڻائي = CTRL_X_THESAURUS;
	    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
	حالة Ctrl_U:
	    // user defined completion
	    ctrl_x_گهڻائي = CTRL_X_FUNCTION;
	    اكسر;
	حالة Ctrl_O:
	    // omni completion
	    ctrl_x_گهڻائي = CTRL_X_OMNI;
	    اكسر;
#جيڪڏهن_پورو
	حالة 's':
	حالة Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_گهڻائي = CTRL_X_SPELL;
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_پٺ_منزل_badword();
	    --emsg_off;
#جيڪڏهن_پورو
	    اكسر;
	حالة Ctrl_RSB:
	    // complete tag names
	    ctrl_x_گهڻائي = CTRL_X_ٽيگ_فهرست;
	    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
	حالة Ctrl_I:
	حالة K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_گهڻائي = CTRL_X_رستو_PATTERNS;
	    اكسر;
	حالة Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_گهڻائي = CTRL_X_رستو_مقرر_ڪري;
	    اكسر;
#جيڪڏهن_پورو
	حالة Ctrl_V:
	حالة Ctrl_Q:
	    // complete vim commands
	    ctrl_x_گهڻائي = CTRL_X_CMDLINE;
	    اكسر;
	حالة Ctrl_Z:
	    // stop completion
	    ctrl_x_گهڻائي = CTRL_X_عام;
	    edit_submode = عدم;
	    showmode();
	    retval = TRUE;
	    اكسر;
	حالة Ctrl_P:
	حالة Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    إذا (!(compl_cont_حالت & CONT_INTRPT))
		compl_cont_حالت |= CONT_LOCAL;
	    وإلا إذا (compl_cont_گهڻائي != 0)
		compl_cont_حالت &= ~CONT_LOCAL;
	    // FALLTHROUGH
	افتراضي:
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
	    إذا (c == Ctrl_X)
	    {
		إذا (compl_cont_گهڻائي != 0)
		    compl_cont_حالت = 0;
		وإلا
		    compl_cont_گهڻائي = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_گهڻائي = CTRL_X_عام;
	    edit_submode = عدم;
	    showmode();
	    اكسر;
    }

    أرجع retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    ساكن فراغ
trigger_complete_پورو_event(صحيح گهڻائي اڻ_استعمال_ٿيل, string_T *لفظ اڻ_استعمال_ٿيل)
{
#إذا defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*گهڻائي_str;

    إذا (لفظ == عدم || لفظ->string == عدم)
	(فراغ)dict_جوڙيو_string_len(v_event, "complete_word", (char_u *)"", 0);
    وإلا
	(فراغ)dict_جوڙيو_string_len(v_event, "complete_word", لفظ->string, (صحيح)لفظ->ڊيگهائي);

    گهڻائي_str = &ctrl_x_گهڻائي_نالا[گهڻائي & ~CTRL_X_WANT_IDENT];
    (فراغ)dict_جوڙيو_string_len(v_event, "complete_type",
	(گهڻائي_str->string == عدم) ? (char_u *)"" : گهڻائي_str->string, (صحيح)گهڻائي_str->ڊيگهائي);

    dict_سيٽ_شيون_ro(v_event);
#جيڪڏهن_پورو
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#إذا defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#جيڪڏهن_پورو
}

/*
 * Stop insert completion mode
 */
    ساكن صحيح
ins_compl_stop(صحيح c, صحيح prev_گهڻائي, صحيح retval)
{
    صحيح		want_cindent;
    string_T	لفظ = {عدم, 0};

    // Remove pre-inserted text when present.
    إذا (ins_compl_preinsert_effect() && ins_compl_win_سرگرم(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    إذا (compl_curr_ميلاپ != عدم || compl_leader.string != عدم || c == Ctrl_E)
    {
	char_u	*ptr = عدم;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	إذا (compl_curr_ميلاپ != عدم && compl_استعمال_ٿيل_ميلاپ && c != Ctrl_E)
	    ptr = compl_curr_ميلاپ->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    إذا (compl_cont_گهڻائي == CTRL_X_سڄو_سٽ)
    {
	// re-indent the current line
	إذا (want_cindent)
	{
	    do_c_اظهار_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    وإلا إذا (!compl_autocomplete || compl_استعمال_ٿيل_ميلاپ)
    {
	صحيح prev_کانو = curwin->w_cursor.کانو;

	// put the cursor on the last char, for 'tw' formatting
	إذا (prev_کانو > 0)
	    dec_cursor();
	// only format when something was inserted
	إذا (!arrow_استعمال_ٿيل && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	إذا (prev_کانو > 0
		&& ml_get_curline()[curwin->w_cursor.کانو] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    إذا ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	لفظ.string = vim_strnsave(compl_shown_ميلاپ->cp_str.string,
	    compl_shown_ميلاپ->cp_str.ڊيگهائي);
	إذا (لفظ.string == عدم)
	    لفظ.ڊيگهائي = 0;
	وإلا
	    لفظ.ڊيگهائي = compl_shown_ميلاپ->cp_str.ڊيگهائي;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    إذا (c == Ctrl_E)
    {
	char_u *p = عدم;
	size_t	plen = 0;

	ins_compl_delete();
	إذا (compl_leader.string != عدم)
	{
	    p = compl_leader.string;
	    plen = compl_leader.ڊيگهائي;
	}
	وإلا إذا (compl_پهريون_ميلاپ != عدم)
	{
	    p = compl_orig_متن.string;
	    plen = compl_orig_متن.ڊيگهائي;
	}
	إذا (p != عدم)
	{
	    صحيح	    compl_len = get_compl_len();

	    إذا ((صحيح)plen > compl_len)
		ins_compl_وجهو_بائيٽون(p + compl_len, (صحيح)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_گهڻائي = prev_گهڻائي;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    إذا (!shortmess(SHM_COMPLETIONMENU))
	نياپو_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_گهڻائي = CTRL_X_عام;
    compl_enter_selects = FALSE;
    إذا (edit_submode != عدم)
    {
	edit_submode = عدم;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_انگ_bests = 0;
    compl_ins_پڇاڙي_کانو = 0;

    إذا (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    إذا (want_cindent && اندريون_cinkeys(ڪنجي_COMPLETE, ' ', inindent(0)))
	do_c_اظهار_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_پورو_event(prev_گهڻائي, &لفظ);
    vim_free(لفظ.string);

    أرجع retval;
}

/*
 * Cancel completion.
 */
    صحيح
ins_compl_cancel(فراغ)
{
    أرجع ins_compl_stop(' ', ctrl_x_گهڻائي, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    صحيح
ins_compl_prep(صحيح c)
{
    صحيح	    retval = FALSE;
    صحيح	    prev_گهڻائي = ctrl_x_گهڻائي;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    إذا (c != Ctrl_R && vim_is_ctrl_x_ڪنجي(c))
	edit_submode_extra = عدم;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    إذا (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	أرجع retval;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    إذا (is_mouse_ڪنجي(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	إذا (c == K_LEFTRELEASE
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
	    أرجع retval;
	إذا (popup_visible)
	{
	    صحيح	    صف = mouse_صف;
	    صحيح	    کانو = mouse_کانو;
	    win_T   *wp = mouse_ڳوليو_win(&صف, &کانو, ڳوليو_POPUP);

	    إذا (wp != عدم && WIN_IS_POPUP(wp))
		أرجع retval;
	}
    }
#جيڪڏهن_پورو

    إذا (ctrl_x_گهڻائي == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	إذا (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_ڪنجي(c)
		|| !vim_is_ctrl_x_ڪنجي(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_گهڻائي = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    إذا (c == Ctrl_Z)
		retval = TRUE;
	}
	وإلا
	{
	    ctrl_x_گهڻائي = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_گهڻائي = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    إذا (ctrl_x_گهڻائي_not_defined_yet()
			   || (ctrl_x_گهڻائي_عام() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_استعمال_ٿيل_ميلاپ = TRUE;
    }

    إذا (ctrl_x_گهڻائي_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = سيٽ_ctrl_x_گهڻائي(c);
    وإلا إذا (ctrl_x_گهڻائي_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	إذا (c != Ctrl_R && !vim_is_ctrl_x_ڪنجي(c))
	{
	    ctrl_x_گهڻائي = ctrl_x_گهڻائي_scroll() ? CTRL_X_عام : CTRL_X_ختم;
	    edit_submode = عدم;
	}
	showmode();
    }

    إذا (compl_started || ctrl_x_گهڻائي == CTRL_X_ختم)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	إذا ((ctrl_x_گهڻائي_عام() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_ڪنجي(c))
		|| ctrl_x_گهڻائي == CTRL_X_ختم)
	    retval = ins_compl_stop(c, prev_گهڻائي, retval);
    }
    وإلا إذا (ctrl_x_گهڻائي == CTRL_X_LOCAL_نياپو)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_پورو_event(ctrl_x_گهڻائي, عدم);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    إذا (c != Ctrl_R && !vim_is_ctrl_x_ڪنجي(c))
    {
	compl_cont_حالت = 0;
	compl_cont_گهڻائي = 0;
    }

    أرجع retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    ساكن فراغ
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    صحيح	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    إذا (ptr == عدم)
    {
	إذا (compl_leader.string != عدم)
	    ptr = compl_leader.string;
	وإلا
	    أرجع;  // nothing to do
    }
    إذا (compl_orig_متن.string != عدم)
    {
	p = compl_orig_متن.string;
	// Find length of common prefix between original text and new completion
	طالما (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	إذا (len > 0)
	    len -= (*mb_سر_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	لكل (p += len; *p != NUL; MB_PTR_ADV(p))
	    Append_Char_منزل_Redobuff(K_BS);
    }
    إذا (ptr != عدم)
	Append_منزل_Redobuff_Lit(ptr + len, -1);
}

/*
 * Loops through the list of windows, loaded-buffers or non-loaded-buffers
 * (depending on flag) starting from buf and looking for a non-scanned
 * buffer (other than curbuf).	curbuf is special, if it is called with
 * buf=curbuf then it has to be the first call for a given flag/expansion.
 *
 * Returns the buffer to scan, if any, otherwise returns curbuf -- Acevedo
 */
    ساكن buf_T *
ins_compl_ايندڙ_buf(buf_T *buf, صحيح flag)
{
    ساكن win_T    *wp = عدم;
    صحيح		    skip_بفر;

    إذا (flag == 'w')		// just windows
    {
	إذا (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	طالما (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_ايندڙ != عدم) ? wp->w_ايندڙ : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    إذا (wp == curwin || !wp->w_بفر->b_scanned)
		اكسر;
	}
	buf = wp->w_بفر;
    }
    وإلا
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	طالما (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_ايندڙ != عدم) ? buf->b_ايندڙ : firstbuf;
	    // Break if we're back at start buffer
	    إذا (buf == curbuf)
		اكسر;

	    // Check buffer conditions based on flag
	    إذا (flag == 'U')
		skip_بفر = buf->b_p_bl;
	    وإلا
		skip_بفر = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == عدم) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    إذا (!skip_بفر && !buf->b_scanned)
		اكسر;
	}
    }
    أرجع buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    ساكن صحيح
get_cpt_sources_ڳڻپ(char_u *cpt)
{
    char_u  dummy[LSIZE];
    صحيح	    ڳڻپ = 0;
    char_u  *p;

    لكل (p = cpt; *p != NUL; )
    {
	طالما (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	إذا (*p != NUL)
	{
	    (فراغ)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    ڳڻپ++;
	}
    }

    أرجع ڳڻپ;
}

#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
ساكن callback_T cfu_cb;	    // 'completefunc' callback function
ساكن callback_T ofu_cb;	    // 'omnifunc' callback function
ساكن callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
ساكن callback_T *cpt_cb;	    // Callback functions associated with F{func}
ساكن صحيح	  cpt_cb_ڳڻپ;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    ساكن فراغ
copy_global_منزل_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    إذا (globcb->cb_name != عدم && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    حرف *
did_سيٽ_completefunc(optset_T *دليلون)
{
    صحيح	retval;

    إذا (دليلون->os_flags & OPT_LOCAL)
	retval = option_سيٽ_callback_func(دليلون->os_newval.string,
							    &curbuf->b_cfu_cb);
    وإلا
    {
	retval = option_سيٽ_callback_func(دليلون->os_newval.string, &cfu_cb);
	إذا (retval == OK && !(دليلون->os_flags & OPT_GLOBAL))
	    سيٽ_buflocal_cfu_callback(curbuf);
    }

    أرجع retval == FAIL ? e_invalid_دليل : عدم;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    فراغ
سيٽ_buflocal_cfu_callback(buf_T *buf اڻ_استعمال_ٿيل)
{
# ifdef FEAT_EVAL
    copy_global_منزل_buflocal_cb(&cfu_cb, &buf->b_cfu_cb);
# endif
}

/*
 * Parse the 'omnifunc' option value and set the callback function.
 * Invoked when the 'omnifunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    حرف *
did_سيٽ_omnifunc(optset_T *دليلون)
{
    صحيح	retval;

    إذا (دليلون->os_flags & OPT_LOCAL)
	retval = option_سيٽ_callback_func(دليلون->os_newval.string,
							    &curbuf->b_ofu_cb);
    وإلا
    {
	retval = option_سيٽ_callback_func(دليلون->os_newval.string, &ofu_cb);
	إذا (retval == OK && !(دليلون->os_flags & OPT_GLOBAL))
	    سيٽ_buflocal_ofu_callback(curbuf);
    }

    أرجع retval == FAIL ? e_invalid_دليل : عدم;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    فراغ
سيٽ_buflocal_ofu_callback(buf_T *buf اڻ_استعمال_ٿيل)
{
# ifdef FEAT_EVAL
    copy_global_منزل_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    فراغ
clear_cpt_callbacks(callback_T **callbacks, صحيح ڳڻپ)
{
    إذا (callbacks == عدم || *callbacks == عدم)
	أرجع;

    لكل (صحيح i = 0; i < ڳڻپ; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    ساكن صحيح
copy_cpt_callbacks(callback_T **dest, صحيح *dest_cnt, callback_T *src, صحيح cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    إذا (cnt == 0)
	أرجع OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    إذا (*dest == عدم)
	أرجع FAIL;

    *dest_cnt = cnt;

    لكل (صحيح i = 0; i < cnt; i++)
	إذا (src[i].cb_name != عدم && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    أرجع OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    فراغ
سيٽ_buflocal_cpt_callbacks(buf_T *buf اڻ_استعمال_ٿيل)
{
# ifdef FEAT_EVAL
    إذا (buf == عدم)
	أرجع;
    (فراغ)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_ڳڻپ, cpt_cb,
	    cpt_cb_ڳڻپ);
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
    ساكن صحيح
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, صحيح *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    صحيح	    idx = 0;
    صحيح	    slen;
    صحيح	    ڳڻپ;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    ڳڻپ = get_cpt_sources_ڳڻپ(cpt);
    إذا (ڳڻپ == 0)
	أرجع OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, ڳڻپ);
    إذا (*cbp == عدم)
	أرجع FAIL;
    *cnt = ڳڻپ;

    لكل (p = cpt; *p != NUL; )
    {
	طالما (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	إذا (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    إذا (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		إذا (caret != عدم)
		    *caret = NUL;

		إذا (option_سيٽ_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = عدم;
	    }
	    idx++;
	}
    }

    أرجع OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    صحيح
سيٽ_cpt_callbacks(optset_T *دليلون)
{
    صحيح	    opt_flags = دليلون->os_flags;

    إذا (curbuf == عدم)
	أرجع FAIL;

    // ":setglobal" does not change the buffer-local option value
    إذا (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_ڳڻپ) != OK)
	أرجع FAIL;

    // ":setlocal" does not change the global option value
    إذا (opt_flags & OPT_LOCAL)
	أرجع OK;

    // Cache the callbacks for the global value
    إذا (opt_flags & OPT_GLOBAL)
	أرجع parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_ڳڻپ);

    // set case
    أرجع copy_cpt_callbacks(&cpt_cb, &cpt_cb_ڳڻپ, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_ڳڻپ);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    حرف *
did_سيٽ_thesaurusfunc(optset_T *دليلون اڻ_استعمال_ٿيل)
{
    صحيح	retval;

    إذا (دليلون->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_سيٽ_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    وإلا
    {
	// global option set
	retval = option_سيٽ_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	إذا (!(دليلون->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    أرجع retval == FAIL ? e_invalid_دليل : عدم;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    صحيح
سيٽ_ref_اندريون_cpt_callbacks(callback_T *callbacks, صحيح ڳڻپ, صحيح copyID)
{
    صحيح	abort = FALSE;

    إذا (callbacks == عدم)
	أرجع FALSE;

    لكل (صحيح i = 0; i < ڳڻپ; i++)
	abort = abort || سيٽ_ref_اندريون_callback(&callbacks[i], copyID);
    أرجع abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    صحيح
سيٽ_ref_اندريون_insexpand_funcs(صحيح copyID)
{
    صحيح abort = سيٽ_ref_اندريون_callback(&cfu_cb, copyID);
    abort = abort || سيٽ_ref_اندريون_callback(&ofu_cb, copyID);
    abort = abort || سيٽ_ref_اندريون_callback(&tsrfu_cb, copyID);
    abort = abort || سيٽ_ref_اندريون_cpt_callbacks(cpt_cb, cpt_cb_ڳڻپ, copyID);

    أرجع abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    ساكن char_u *
get_complete_funcname(صحيح type)
{
    بدل (type)
    {
	حالة CTRL_X_FUNCTION:
	    أرجع curbuf->b_p_cfu;
	حالة CTRL_X_OMNI:
	    أرجع curbuf->b_p_ofu;
	حالة CTRL_X_THESAURUS:
	    أرجع *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	افتراضي:
	    أرجع (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    ساكن callback_T *
get_وجهو_callback(صحيح type)
{
    إذا (type == CTRL_X_FUNCTION)
	أرجع &curbuf->b_cfu_cb;
    إذا (type == CTRL_X_OMNI)
	أرجع &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    أرجع (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    ساكن فراغ
expand_by_function(صحيح type, char_u *بنياد, callback_T *cb)
{
    list_T      *matchlist = عدم;
    dict_T	*matchdict = عدم;
    typval_T	دليلون[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    صحيح		save_State = State;
    صحيح		retval;
    صحيح		is_cpt_function = (cb != عدم);
    صحيح		use_sandbox = is_cpt_function
			    && was_سيٽ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    إذا (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	إذا (*funcname == NUL)
	    أرجع;
	cb = get_وجهو_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    دليلون[0].v_type = VAR_عدد;
    دليلون[0].vval.v_عدد = 0;
    دليلون[1].v_type = VAR_STRING;
    دليلون[1].vval.v_string = بنياد != عدم ? بنياد : (char_u *)"";
    دليلون[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    إذا (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, دليلون);
    إذا (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    إذا (retval == OK)
    {
	بدل (rettv.v_type)
	{
	    حالة VAR_LIST:
		matchlist = rettv.vval.v_list;
		اكسر;
	    حالة VAR_DICT:
		matchdict = rettv.vval.v_dict;
		اكسر;
	    حالة VAR_SPECIAL:
		إذا (rettv.vval.v_عدد == VVAL_NONE)
		    compl_opt_suppress_خالي_جايون = TRUE;
		// FALLTHROUGH
	    افتراضي:
		// TODO: Give error message?
		clear_tv(&rettv);
		اكسر;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    إذا (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_متن));
	اذهب theend;
    }

    إذا (matchlist != عدم)
	ins_compl_جوڙيو_list(matchlist);
    وإلا إذا (matchdict != عدم)
	ins_compl_جوڙيو_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    إذا (matchdict != عدم)
	dict_unref(matchdict);
    إذا (matchlist != عدم)
	list_unref(matchlist);
}
#جيڪڏهن_پورو // FEAT_COMPL_FUNC

#إذا defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    ساكن مضمن صحيح
get_user_highlight_id(char_u *hlname)
{
    إذا (hlname != عدم && *hlname != NUL)
	أرجع syn_check_گروهه(hlname, (صحيح)STRLEN(hlname));
    أرجع 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    ساكن صحيح
ins_compl_جوڙيو_tv(typval_T *tv, صحيح dir, صحيح fast)
{
    char_u	*لفظ;
    صحيح		dup = FALSE;
    صحيح		خالي_جايون_2 = FALSE;
    صحيح		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_ڳڻپ]);
    typval_T	user_معلومات;
    صحيح		حالت;
    char_u	*user_abbr_hlname;
    char_u	*user_قسم_hlname;
    صحيح		user_hl[2] = { 0, 0 };

    user_معلومات.v_type = VAR_UNKNOWN;
    إذا (tv->v_type == VAR_DICT && tv->vval.v_dict != عدم)
    {
	لفظ = dict_get_string(tv->vval.v_dict, "word", FALSE);
	cptext[CPT_ABBR] = dict_get_string(tv->vval.v_dict, "abbr", FALSE);
	cptext[CPT_MENU] = dict_get_string(tv->vval.v_dict, "menu", FALSE);
	cptext[CPT_قسم] = dict_get_string(tv->vval.v_dict, "kind", FALSE);
	cptext[CPT_INFO] = dict_get_string(tv->vval.v_dict, "info", FALSE);

	user_abbr_hlname = dict_get_string(tv->vval.v_dict, "abbr_hlgroup", FALSE);
	user_hl[0] = get_user_highlight_id(user_abbr_hlname);

	user_قسم_hlname = dict_get_string(tv->vval.v_dict, "kind_hlgroup", FALSE);
	user_hl[1] = get_user_highlight_id(user_قسم_hlname);

	dict_get_tv(tv->vval.v_dict, "user_data", &user_معلومات);
	إذا (dict_get_string(tv->vval.v_dict, "icase", FALSE) != عدم
				  && dict_get_عدد(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	إذا (dict_get_string(tv->vval.v_dict, "dup", FALSE) != عدم)
	    dup = dict_get_عدد(tv->vval.v_dict, "dup");
	إذا (dict_get_string(tv->vval.v_dict, "empty", FALSE) != عدم)
	    خالي_جايون_2 = dict_get_عدد(tv->vval.v_dict, "empty");
	إذا (dict_get_string(tv->vval.v_dict, "equal", FALSE) != عدم
				  && dict_get_عدد(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    وإلا
    {
	لفظ = tv_get_string_chk(tv);
	CLEAR_ميدان(cptext);
    }
    إذا (لفظ == عدم || (!خالي_جايون_2 && *لفظ == NUL))
    {
	clear_tv(&user_معلومات);
	أرجع FAIL;
    }
    حالت = ins_compl_جوڙيو(لفظ, -1, عدم, cptext,
	    &user_معلومات, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    إذا (حالت != OK)
	clear_tv(&user_معلومات);
    أرجع حالت;
}

/*
 * Add completions from a list.
 */
    ساكن فراغ
ins_compl_جوڙيو_list(list_T *list)
{
    listitem_T	*li;
    صحيح		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_شيون(list, li)
    {
	إذا (ins_compl_جوڙيو_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	وإلا إذا (did_emsg)
	    اكسر;
    }
}

/*
 * Add completions from a dict.
 */
    ساكن فراغ
ins_compl_جوڙيو_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_لفظ;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_ڳوليو(dict, (char_u *)"refresh", 7);
    إذا (di_refresh != عدم && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	إذا (v != عدم && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_لفظ = dict_ڳوليو(dict, (char_u *)"words", 5);
    إذا (di_لفظ != عدم && di_لفظ->di_tv.v_type == VAR_LIST)
	ins_compl_جوڙيو_list(di_لفظ->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    ساكن فراغ
سيٽ_completion(colnr_T startcol, list_T *list)
{
    صحيح save_w_wrow = curwin->w_wrow;
    صحيح save_w_leftcol = curwin->w_leftcol;
    صحيح flags = CP_ORIGINAL_متن;
    بنانشان صحيح cur_cot_flags = get_cot_flags();
    صحيح compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    صحيح compl_no_وجهو = (cur_cot_flags & COT_NOINSERT) != 0;
    صحيح compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    إذا (compl_started || ctrl_x_گهڻائي_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    إذا (startcol > curwin->w_cursor.کانو)
	startcol = curwin->w_cursor.کانو;
    compl_کانو = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_ڊيگهائي = (صحيح)curwin->w_cursor.کانو - (صحيح)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_متن.string = vim_strnsave(ml_get_curline() + compl_کانو,
							(size_t)compl_ڊيگهائي);
    إذا (p_ic)
	flags |= CP_ICASE;
    إذا (compl_orig_متن.string == عدم)
    {
	compl_orig_متن.ڊيگهائي = 0;
	أرجع;
    }
    compl_orig_متن.ڊيگهائي = (size_t)compl_ڊيگهائي;
    إذا (ins_compl_جوڙيو(compl_orig_متن.string,
			(صحيح)compl_orig_متن.ڊيگهائي, عدم, عدم, عدم, 0,
			flags | CP_FAST, FALSE, عدم, FUZZY_SCORE_NONE) != OK)
	أرجع;

    ctrl_x_گهڻائي = CTRL_X_EVAL;

    ins_compl_جوڙيو_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_استعمال_ٿيل_ميلاپ = TRUE;
    compl_cont_حالت = 0;

    compl_curr_ميلاپ = compl_پهريون_ميلاپ;
    صحيح no_select = compl_no_select || compl_longest;
    إذا (compl_no_وجهو || no_select)
    {
	ins_complete(K_هيٺ_ڏانهن, FALSE);
	إذا (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_مٿي, FALSE);
    }
    وإلا
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_وجهو;

    // Lazily show the popup menu, unless we got interrupted.
    إذا (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    ٻاهريون_flush();
}

/*
 * "complete()" function
 */
    فراغ
f_complete(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    إذا ((State & گهڻائي_وجهو) == 0)
    {
	emsg(_(e_complete_can_only_be_استعمال_ٿيل_اندريون_وجهو_گهڻائي));
	أرجع;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    إذا (!undo_allowed())
	أرجع;

    إذا (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	صحيح startcol = (صحيح)tv_get_عدد_chk(&argvars[0], عدم);
	إذا (startcol > 0)
	    سيٽ_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    فراغ
f_complete_جوڙيو(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_عدد = ins_compl_جوڙيو_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    فراغ
f_complete_check(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_ڪنجيون(0, TRUE);
    rettv->vval.v_عدد = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    ساكن فراغ
ins_compl_گهڻائي(string_T *ret)
{
    إذا (ctrl_x_گهڻائي_not_defined_yet() || ctrl_x_گهڻائي_scroll() || compl_started)
    {
	string_T    *گهڻائي;

	گهڻائي = &ctrl_x_گهڻائي_نالا[ctrl_x_گهڻائي & ~CTRL_X_WANT_IDENT];
	ret->string = (گهڻائي->string == عدم) ? (char_u *)"" : گهڻائي->string;
	ret->ڊيگهائي = گهڻائي->ڊيگهائي;
    }
    وإلا
    {
	ret->string = (char_u *)"";
	ret->ڊيگهائي = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    ساكن فراغ
ins_compl_update_sequence_numbers(فراغ)
{
    صحيح		عدد = 0;
    compl_T	*ميلاپ;

    إذا (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	لكل (ميلاپ = compl_curr_ميلاپ->cp_prev; ميلاپ != عدم
		&& !is_پهريون_ميلاپ(ميلاپ); ميلاپ = ميلاپ->cp_prev)
	    إذا (ميلاپ->cp_عدد != -1)
	    {
		عدد = ميلاپ->cp_عدد;
		اكسر;
	    }
	إذا (ميلاپ != عدم)
	    // go up and assign all numbers which are not assigned yet
	    لكل (ميلاپ = ميلاپ->cp_ايندڙ;
		    ميلاپ != عدم && ميلاپ->cp_عدد == -1;
					   ميلاپ = ميلاپ->cp_ايندڙ)
		ميلاپ->cp_عدد = ++عدد;
    }
    وإلا // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	لكل (ميلاپ = compl_curr_ميلاپ->cp_ايندڙ; ميلاپ != عدم
		&& !is_پهريون_ميلاپ(ميلاپ); ميلاپ = ميلاپ->cp_ايندڙ)
	{
	    إذا (ميلاپ->cp_عدد != -1)
	    {
		عدد = ميلاپ->cp_عدد;
		اكسر;
	    }
	}
	إذا (ميلاپ != عدم)
	{
	    // go down and assign all numbers which are not assigned yet
	    لكل (ميلاپ = ميلاپ->cp_prev; ميلاپ
		    && ميلاپ->cp_عدد == -1;
					   ميلاپ = ميلاپ->cp_prev)
		ميلاپ->cp_عدد = ++عدد;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    ساكن فراغ
fill_complete_info_dict(dict_T *di, compl_T *ميلاپ, صحيح جوڙيو_ميلاپ)
{
    dict_جوڙيو_string_len(di, "word", ميلاپ->cp_str.string, (صحيح)ميلاپ->cp_str.ڊيگهائي);
    dict_جوڙيو_string(di, "abbr", ميلاپ->cp_متن[CPT_ABBR]);
    dict_جوڙيو_string(di, "menu", ميلاپ->cp_متن[CPT_MENU]);
    dict_جوڙيو_string(di, "kind", ميلاپ->cp_متن[CPT_قسم]);
    dict_جوڙيو_string(di, "info", ميلاپ->cp_متن[CPT_INFO]);
    dict_جوڙيو_string(di, "abbr_hlgroup", syn_id2name(ميلاپ->cp_user_abbr_hl_id));
    dict_جوڙيو_string(di, "kind_hlgroup", syn_id2name(ميلاپ->cp_user_قسم_hl_id));
    إذا (جوڙيو_ميلاپ)
	dict_جوڙيو_bool(di, "match", ميلاپ->cp_اندريون_ميلاپ_array);
    إذا (ميلاپ->cp_user_معلومات.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_جوڙيو_string_len(di, "user_data", (char_u *)"", 0);
    وإلا
	dict_جوڙيو_tv(di, "user_data", &ميلاپ->cp_user_معلومات);
}

/*
 * Get complete information
 */
    ساكن فراغ
get_complete_info(list_T *what_list, dict_T *retdict)
{
    صحيح		ret = OK;
    listitem_T	*شي;
# define CI_WHAT_گهڻائي		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_شيون		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_متن   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    صحيح		what_flag;

    إذا (what_list == عدم)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    وإلا
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_شيون(what_list, شي)
	{
	    char_u *what = tv_get_string(&شي->li_tv);

	    إذا (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_گهڻائي;
	    وإلا إذا (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    وإلا إذا (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_شيون;
	    وإلا إذا (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    وإلا إذا (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    وإلا إذا (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_متن;
	    وإلا إذا (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    وإلا إذا (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    إذا (ret == OK && (what_flag & CI_WHAT_گهڻائي))
    {
	string_T    گهڻائي;

	ins_compl_گهڻائي(&گهڻائي);
	ret = dict_جوڙيو_string_len(retdict, "mode", گهڻائي.string, (صحيح)گهڻائي.ڊيگهائي);
    }

    إذا (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_جوڙيو_عدد(retdict, "pum_visible", pum_visible());

    إذا (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_جوڙيو_عدد(retdict, "auto", compl_autostarted);

    إذا (ret == OK && (what_flag & CI_WHAT_PREINSERTED_متن))
    {
	char_u	*سٽ = ml_get_curline();
	صحيح	len = compl_ins_پڇاڙي_کانو - curwin->w_cursor.کانو;

	ret = dict_جوڙيو_string_len(retdict, "preinserted_text",
		(len > 0) ? سٽ + curwin->w_cursor.کانو : (char_u *)"", (len > 0) ? len : 0);
    }

    إذا (ret == OK && (what_flag & (CI_WHAT_شيون | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = عدم;
	dict_T	    *di;
	compl_T     *ميلاپ;
	صحيح         selected_idx = -1;
	صحيح	    has_شيون = what_flag & CI_WHAT_شيون;
	صحيح	    has_matches = what_flag & CI_WHAT_MATCHES;
	صحيح	    has_completed = what_flag & CI_WHAT_COMPLETED;

	إذا (has_شيون || has_matches)
	{
	    li = list_alloc();
	    إذا (li == عدم)
		أرجع;
	    ret = dict_جوڙيو_list(retdict, (has_matches && !has_شيون)
						? "matches" : "items", li);
	    إذا (ret == FAIL)
		list_unref(li);
	}
	إذا (ret == OK && what_flag & CI_WHAT_SELECTED)
	    إذا (compl_curr_ميلاپ != عدم && compl_curr_ميلاپ->cp_عدد == -1)
		ins_compl_update_sequence_numbers();
	إذا (ret == OK && compl_پهريون_ميلاپ != عدم)
	{
	    صحيح list_idx = 0;
	    ميلاپ = compl_پهريون_ميلاپ;
	    افعل
	    {
		إذا (!ميلاپ_at_original_متن(ميلاپ))
		{
		    إذا (has_شيون
			    || (has_matches && ميلاپ->cp_اندريون_ميلاپ_array))
		    {
			di = dict_alloc();
			إذا (di == عدم)
			    أرجع;
			ret = list_append_dict(li, di);
			إذا (ret != OK)
			{
			    dict_unref(di);
			    أرجع;
			}
			fill_complete_info_dict(di, ميلاپ, has_matches && has_شيون);
		    }
		    إذا (compl_curr_ميلاپ != عدم
			    && compl_curr_ميلاپ->cp_عدد == ميلاپ->cp_عدد)
			selected_idx = list_idx;
		    إذا (!has_matches || ميلاپ->cp_اندريون_ميلاپ_array)
			list_idx++;
		}
		ميلاپ = ميلاپ->cp_ايندڙ;
	    }
	    طالما (ميلاپ != عدم && !is_پهريون_ميلاپ(ميلاپ));
	}
	إذا (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_جوڙيو_عدد(retdict, "selected", selected_idx);

	إذا (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    إذا (di == عدم)
		أرجع;
	    fill_complete_info_dict(di, compl_curr_ميلاپ, FALSE);
	    ret = dict_جوڙيو_dict(retdict, "completed", di);
	    إذا (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    فراغ
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = عدم;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	إذا (check_for_list_arg(argvars, 0) == FAIL)
	    أرجع;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#جيڪڏهن_پورو

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    ساكن صحيح
thesaurus_func_complete(صحيح type اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    أرجع type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#وإلا
    أرجع FALSE;
#جيڪڏهن_پورو
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    ساكن صحيح
may_advance_cpt_اشاريو(char_u *cpt)
{
    char_u  *p = cpt;

    إذا (cpt_sources_اشاريو == -1)
	أرجع FALSE;
    طالما (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    أرجع (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
تعداد
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_پڇاڙي
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
تعريفنوع بنية
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_ميلاپ_pos;		// current match position
    pos_T	prev_ميلاپ_pos;		// previous match position
    صحيح		سيٽ_ميلاپ_pos;		// save first_match_pos/last_match_pos
    pos_T	پهريون_ميلاپ_pos;	// first match position
    pos_T	آخري_ميلاپ_pos;		// last match position
    صحيح		مليو_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    صحيح		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_ايندڙ_state_T;

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
    ساكن صحيح
process_ايندڙ_cpt_قدر(
	ins_compl_ايندڙ_state_T *st,
	صحيح		*compl_type_arg,
	pos_T		*شروعات_ميلاپ_pos,
	صحيح		fuzzy_collect,
	صحيح		*advance_cpt_idx)
{
    صحيح	    compl_type = -1;
    صحيح	    حالت = INS_COMPL_CPT_OK;
    صحيح	    skip_ذريعو = compl_autocomplete && compl_from_nonkeyword;

    st->مليو_all = FALSE;
    *advance_cpt_idx = FALSE;

    طالما (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    إذا (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_ذريعو
	    && !compl_وقت_ٽڪرو_expired)
    {
	st->ins_buf = curbuf;
	st->پهريون_ميلاپ_pos = *شروعات_ميلاپ_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	إذا (ctrl_x_گهڻائي_عام() && (!fuzzy_collect && dec(&st->پهريون_ميلاپ_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->پهريون_ميلاپ_pos.lnum = st->ins_buf->b_ml.ml_سٽ_ڳڻپ;
	    st->پهريون_ميلاپ_pos.کانو = ml_get_len(st->پهريون_ميلاپ_pos.lnum);
	}
	st->آخري_ميلاپ_pos = st->پهريون_ميلاپ_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->سيٽ_ميلاپ_pos = TRUE;
    }
    وإلا إذا (!skip_ذريعو && !compl_وقت_ٽڪرو_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != عدم
	    && (st->ins_buf = ins_compl_ايندڙ_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	إذا (st->ins_buf->b_ml.ml_mfp != عدم)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->پهريون_ميلاپ_pos.کانو = st->آخري_ميلاپ_pos.کانو = 0;
	    st->پهريون_ميلاپ_pos.lnum = st->ins_buf->b_ml.ml_سٽ_ڳڻپ + 1;
	    st->آخري_ميلاپ_pos.lnum = 0;
	    compl_type = 0;
	}
	وإلا	// unloaded buffer, scan like dictionary
	{
	    st->مليو_all = TRUE;
	    إذا (st->ins_buf->b_fname == عدم)
	    {
		حالت = INS_COMPL_CPT_CONT;
		اذهب پورو;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	إذا (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    نياپو_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((حرف *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == عدم
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == عدم
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (فراغ)نياپو_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    وإلا إذا (*st->e_cpt == NUL)
	حالت = INS_COMPL_CPT_پڇاڙي;
    وإلا
    {
	إذا (ctrl_x_گهڻائي_سٽ_or_eval())
	    compl_type = -1;
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
	وإلا إذا (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_اشاريو);
	    إذا (!st->func_cb)
		compl_type = -1;
	}
#جيڪڏهن_پورو
	وإلا إذا (!skip_ذريعو)
	{
	    إذا (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		إذا (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		وإلا
		    compl_type = CTRL_X_THESAURUS;
		إذا (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_پهريون;
		}
	    }
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
	    وإلا إذا (*st->e_cpt == 'i')
		compl_type = CTRL_X_رستو_PATTERNS;
	    وإلا إذا (*st->e_cpt == 'd')
		compl_type = CTRL_X_رستو_مقرر_ڪري;
#جيڪڏهن_پورو
	    وإلا إذا (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_ٽيگ_فهرست;
		إذا (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    نياپو_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((حرف *)IObuff, IOSIZE, _("Scanning tags."));
		    (فراغ)نياپو_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    وإلا
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(فراغ)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_اشاريو(st->e_cpt);

	st->مليو_all = TRUE;
	إذا (compl_type == -1)
	    حالت = INS_COMPL_CPT_CONT;
    }

پورو:
    *compl_type_arg = compl_type;
    أرجع حالت;
}

#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    ساكن فراغ
get_ايندڙ_include_file_completion(صحيح compl_type)
{
    ڳوليو_نمونو_اندريون_رستو(compl_نمونو.string, compl_direction,
	    (صحيح)compl_نمونو.ڊيگهائي, FALSE, FALSE,
	    (compl_type == CTRL_X_رستو_مقرر_ڪري
	     && !(compl_cont_حالت & CONT_SOL))
	    ? ڳوليو_DEFINE : ڳوليو_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM, FALSE, compl_autocomplete);
}
#جيڪڏهن_پورو

/*
 * Get the next set of words matching "compl_pattern" in dictionary or
 * thesaurus files.
 */
    ساكن فراغ
get_ايندڙ_dict_tsr_completion(صحيح compl_type, char_u *dict, صحيح dict_f)
{
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    إذا (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_نمونو.string, عدم);
    وإلا
#جيڪڏهن_پورو
	ins_compl_dictionaries(
		dict != عدم ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_نمونو.string,
		dict != عدم ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    ساكن فراغ
get_ايندڙ_ٽيگ_completion(فراغ)
{
    صحيح		save_p_ic;
    char_u	**matches;
    صحيح		انگ_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_نمونو.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_ٽيگ_at_cursor = TRUE;
    إذا (ڳوليو_ٽيگ_فهرست(compl_نمونو.string, &انگ_matches, &matches,
		ٽيگ_REGEXP | ٽيگ_نالا | ٽيگ_NOIC | ٽيگ_INS_COMP
		| (ctrl_x_گهڻائي_not_default() ? ٽيگ_VERBOSE : 0),
		ٽيگ_MANY, curbuf->b_ffname) == OK && انگ_matches > 0)
	ins_compl_جوڙيو_matches(انگ_matches, matches, p_ic);
    g_ٽيگ_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    ساكن صحيح
compare_scores(ثابت فراغ *a, ثابت فراغ *b)
{
    صحيح idx_a = *(ثابت صحيح *)a;
    صحيح idx_b = *(ثابت صحيح *)b;
    صحيح score_a = compl_fuzzy_scores[idx_a];
    صحيح score_b = compl_fuzzy_scores[idx_b];
    أرجع score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    ساكن فراغ
ins_compl_longest_وجهو(char_u *اڳياڙي)
{
    ins_compl_delete();
    ins_compl_وجهو_بائيٽون(اڳياڙي + get_compl_len(), -1);
    compl_longest_پڇاڙي_کانو = curwin->w_cursor.کانو;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    ساكن فراغ
fuzzy_longest_ميلاپ(فراغ)
{
    char_u	*اڳياڙي = عدم;
    صحيح		اڳياڙي_len = 0;
    صحيح		i = 0;
    صحيح		j = 0;
    char_u	*ميلاپ_str = عدم;
    char_u	*اڳياڙي_ptr = عدم;
    char_u	*ميلاپ_ptr = عدم;
    char_u	*leader = عدم;
    size_t	leader_len = 0;
    compl_T	*compl = عدم;
    صحيح		وڌيڪ_candidates = FALSE;
    compl_T	*nn_compl = عدم;

    إذا (compl_انگ_bests == 0)
	أرجع;

    nn_compl = compl_پهريون_ميلاپ->cp_ايندڙ->cp_ايندڙ;
    إذا (nn_compl && nn_compl != compl_پهريون_ميلاپ)
	وڌيڪ_candidates = TRUE;

    compl = ctrl_x_گهڻائي_سڄو_سٽ() ? compl_پهريون_ميلاپ
				    : compl_پهريون_ميلاپ->cp_ايندڙ;
    إذا (compl_انگ_bests == 1)
    {
	// no more candidates insert the match str
	إذا (!وڌيڪ_candidates)
	{
	    ins_compl_longest_وجهو(compl->cp_str.string);
	    compl_انگ_bests = 0;
	}
	compl_انگ_bests = 0;
	أرجع;
    }

    إذا ((size_t)compl_انگ_bests > سائيز_MAX / حجم(compl_T *))
	أرجع;
    compl_بهترين_matches = (compl_T **)alloc(compl_انگ_bests * حجم(compl_T *));
    إذا (compl_بهترين_matches == عدم)
	أرجع;
    طالما (compl != عدم && i < compl_انگ_bests)
    {
	compl_بهترين_matches[i] = compl;
	compl = compl->cp_ايندڙ;
	i++;
    }

    اڳياڙي = compl_بهترين_matches[0]->cp_str.string;
    اڳياڙي_len = (صحيح)compl_بهترين_matches[0]->cp_str.ڊيگهائي;

    لكل (i = 1; i < compl_انگ_bests; i++)
    {
	ميلاپ_str = compl_بهترين_matches[i]->cp_str.string;
	اڳياڙي_ptr = اڳياڙي;
	ميلاپ_ptr = ميلاپ_str;
	j = 0;

	طالما (j < اڳياڙي_len && *ميلاپ_ptr != NUL && *اڳياڙي_ptr != NUL)
	{
	    إذا (STRNCMP(اڳياڙي_ptr, ميلاپ_ptr, mb_ptr2len(اڳياڙي_ptr)) != 0)
		اكسر;

	    MB_PTR_ADV(اڳياڙي_ptr);
	    MB_PTR_ADV(ميلاپ_ptr);
	    j++;
	}

	إذا (j > 0)
	    اڳياڙي_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    إذا (leader_len > 0 && STRNCMP(اڳياڙي, leader, leader_len) != 0)
	اذهب پڇاڙي;

    اڳياڙي = vim_strnsave(اڳياڙي, اڳياڙي_len);
    إذا (اڳياڙي != عدم)
    {
	ins_compl_longest_وجهو(اڳياڙي);
	vim_free(اڳياڙي);
    }

پڇاڙي:
    vim_free(compl_بهترين_matches);
    compl_بهترين_matches = عدم;
    compl_انگ_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    ساكن فراغ
get_ايندڙ_filename_completion(فراغ)
{
    char_u	**matches;
    صحيح		انگ_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    صحيح		i;
    صحيح		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    صحيح		اندريون_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    صحيح		*fuzzy_indices_معلومات;
    char_u	*آخري_sep = عدم;
    صحيح		need_collect_bests = اندريون_fuzzy_collect && compl_get_longest;
    صحيح		max_score = 0;
    صحيح		current_score = 0;
    صحيح		dir = compl_direction;

#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    حرف pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#وإلا
    حرف pathsep = PATHSEP;
#جيڪڏهن_پورو

    إذا (اندريون_fuzzy_collect)
    {
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
	إذا (curbuf->b_p_csl[0] == 's')
	{
	    لكل (i = 0; i < (صحيح)leader_len; i++)
	    {
		إذا (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	وإلا إذا (curbuf->b_p_csl[0] == 'b')
	{
	    لكل (i = 0; i < (صحيح)leader_len; i++)
	    {
		إذا (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#جيڪڏهن_پورو
	آخري_sep = vim_strrchr(leader, pathsep);
	إذا (آخري_sep == عدم)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_نمونو);
	    compl_نمونو.string = vim_strnsave((char_u *)"*", 1);
	    إذا (compl_نمونو.string == عدم)
		أرجع;
	    compl_نمونو.ڊيگهائي = 1;
	}
	وإلا إذا (*(آخري_sep + 1) == '\0')
	    اندريون_fuzzy_collect = FALSE;
	وإلا
	{
	    // Split leader into path and file parts
	    صحيح رستو_len = آخري_sep - leader + 1;
	    char_u  *رستو_with_wildcard;

	    رستو_with_wildcard = alloc(رستو_len + 2);
	    إذا (رستو_with_wildcard != عدم)
	    {
		vim_snprintf((حرف *)رستو_with_wildcard, رستو_len + 2, "%*.*s*", رستو_len, رستو_len, leader);
		VIM_CLEAR_STRING(compl_نمونو);
		compl_نمونو.string = رستو_with_wildcard;
		compl_نمونو.ڊيگهائي = رستو_len + 1;

		// Move leader to the file part
		leader = آخري_sep + 1;
		leader_len -= رستو_len;
	    }
	}
    }

    إذا (expand_wildcards(1, &compl_نمونو.string, &انگ_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	أرجع;

    // May change home directory back to "~".
    tilde_replace(compl_نمونو.string, انگ_matches, matches);
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    إذا (curbuf->b_p_csl[0] != NUL)
    {
	لكل (i = 0; i < انگ_matches; ++i)
	{
	    ptr = matches[i];
	    طالما (*ptr != NUL)
	    {
		إذا (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		وإلا إذا (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#جيڪڏهن_پورو

    إذا (اندريون_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, حجم(صحيح), 10);
	compl_fuzzy_scores = (صحيح *)alloc(حجم(صحيح) * انگ_matches);

	لكل (i = 0; i < انگ_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_ميلاپ_str(ptr, leader);
	    إذا (score != FUZZY_SCORE_NONE)
	    {
		إذا (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((صحيح *)fuzzy_indices.ga_معلومات)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	إذا (fuzzy_indices.ga_len > 0)
	{
	    char_u	*ميلاپ = عدم;
	    fuzzy_indices_معلومات = (صحيح *)fuzzy_indices.ga_معلومات;
	    qsort(fuzzy_indices_معلومات, fuzzy_indices.ga_len, حجم(صحيح), compare_scores);

	    لكل (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		ميلاپ = matches[fuzzy_indices_معلومات[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_معلومات[i]];
		إذا (ins_compl_جوڙيو(ميلاپ, -1, عدم, عدم, عدم, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, عدم, current_score) == OK)
		    dir = FORWARD;

		إذا (need_collect_bests)
		{
		    إذا (i == 0 || current_score == max_score)
		    {
			compl_انگ_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(انگ_matches, matches);
	}
	وإلا إذا (leader_len > 0)
	{
	    FreeWild(انگ_matches, matches);
	    انگ_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	إذا (compl_انگ_bests > 0 && compl_get_longest)
	    fuzzy_longest_ميلاپ();
	أرجع;
    }

    إذا (انگ_matches > 0)
	ins_compl_جوڙيو_matches(انگ_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    ساكن فراغ
get_ايندڙ_cmdline_completion(فراغ)
{
    char_u	**matches;
    صحيح		انگ_matches;

    إذا (expand_cmdline(&compl_xp, compl_نمونو.string,
		(صحيح)compl_نمونو.ڊيگهائي, &انگ_matches, &matches) == EXPAND_OK)
    {
	صحيح		جوڙيو_r = OK;
	صحيح		dir = compl_direction;

	لكل (صحيح i = 0; i < انگ_matches && جوڙيو_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_ڳڻپ]) = {عدم, عدم, عدم, عدم};

	    إذا (compl_xp.xp_files_abbr != عدم)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    إذا (compl_xp.xp_files_قسم != عدم)
		cptext[CPT_قسم] = compl_xp.xp_files_قسم[i];
	    إذا (compl_xp.xp_files_menu != عدم)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    إذا (compl_xp.xp_files_info != عدم)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    جوڙيو_r = ins_compl_جوڙيو(matches[i], -1, عدم, cptext, عدم, dir,
		    CP_FAST, FALSE, عدم, FUZZY_SCORE_NONE);
	    إذا (جوڙيو_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(انگ_matches, matches);
	free_xp_files_extra(&compl_xp, انگ_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    ساكن فراغ
get_ايندڙ_spell_completion(linenr_T lnum اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    char_u	**matches;
    صحيح		انگ_matches;

    انگ_matches = expand_spelling(lnum, compl_نمونو.string, &matches);
    إذا (انگ_matches > 0)
	ins_compl_جوڙيو_matches(انگ_matches, matches, p_ic);
    وإلا
	vim_free(matches);
#جيڪڏهن_پورو
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    ساكن char_u *
ins_compl_get_ايندڙ_لفظ_or_سٽ(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_ميلاپ_pos,		// current match position
	صحيح	*ميلاپ_len,
	صحيح	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    صحيح		len;

    *ميلاپ_len = 0;
    ptr = ml_get_buf(ins_buf, cur_ميلاپ_pos->lnum, FALSE) +
	cur_ميلاپ_pos->کانو;
    len = (صحيح)ml_get_buf_len(ins_buf, cur_ميلاپ_pos->lnum) - cur_ميلاپ_pos->کانو;
    إذا (ctrl_x_گهڻائي_سٽ_or_eval())
    {
	إذا (compl_حالت_adding())
	{
	    إذا (cur_ميلاپ_pos->lnum >= ins_buf->b_ml.ml_سٽ_ڳڻپ)
		أرجع عدم;
	    ptr = ml_get_buf(ins_buf, cur_ميلاپ_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_ميلاپ_pos->lnum + 1);
	    إذا (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (صحيح)(ptr - tmp_ptr);
	    }
	}
    }
    وإلا
    {
	char_u	*tmp_ptr = ptr;

	إذا (compl_حالت_adding() && compl_ڊيگهائي <= len)
	{
	    tmp_ptr += compl_ڊيگهائي;
	    // Skip if already inside a word.
	    إذا (vim_iswordp(tmp_ptr))
		أرجع عدم;
	    // Find start of next word.
	    tmp_ptr = ڳوليو_لفظ_شروعات(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = ڳوليو_لفظ_پڇاڙي(tmp_ptr);
	len = (صحيح)(tmp_ptr - ptr);

	إذا (compl_حالت_adding() && len == compl_ڊيگهائي)
	{
	    إذا (cur_ميلاپ_pos->lnum < ins_buf->b_ml.ml_سٽ_ڳڻپ)
	    {
		// Try next line, if any. the new word will be
		// "join" as if the normal command "J" was used.
		// IOSIZE is always greater than
		// compl_length, so the next STRNCPY always
		// works -- Acevedo
		STRNCPY(IObuff, ptr, len);
		ptr = ml_get_buf(ins_buf, cur_ميلاپ_pos->lnum + 1, FALSE);
		tmp_ptr = ptr = skipwhite(ptr);
		// Find start of next word.
		tmp_ptr = ڳوليو_لفظ_شروعات(tmp_ptr);
		// Find end of next word.
		tmp_ptr = ڳوليو_لفظ_پڇاڙي(tmp_ptr);
		إذا (tmp_ptr > ptr)
		{
		    إذا (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			إذا (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			إذا (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== عدم
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    إذا (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (صحيح)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    إذا (len == compl_ڊيگهائي)
		أرجع عدم;
	}
    }

    *ميلاپ_len = len;
    أرجع ptr;
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
    ساكن صحيح
get_ايندڙ_default_completion(ins_compl_ايندڙ_state_T *st, pos_T *شروعات_pos)
{
    صحيح		مليو_new_ميلاپ = FAIL;
    صحيح		save_p_scs;
    صحيح		save_p_ws;
    صحيح		looped_around = FALSE;
    char_u	*ptr = عدم;
    صحيح		len = 0;
    صحيح		اندريون_fuzzy_collect = !compl_حالت_adding()
			&& cot_fuzzy() && compl_ڊيگهائي > 0;
    char_u	*leader = ins_compl_leader();
    صحيح		score = FUZZY_SCORE_NONE;
    صحيح		اندريون_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    إذا (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    إذا (!اندريون_curbuf)
	p_ws = FALSE;
    وإلا إذا (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    لكل (;;)
    {
	صحيح	cont_s_ipos = FALSE;

	++نياپو_silent;  // Don't want messages for wrapscan.

	إذا (اندريون_fuzzy_collect)
	{
	    مليو_new_ميلاپ = ڳولا_for_fuzzy_ميلاپ(st->ins_buf,
			    st->cur_ميلاپ_pos, leader, compl_direction,
			    شروعات_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	وإلا إذا (ctrl_x_گهڻائي_سڄو_سٽ() || ctrl_x_گهڻائي_eval() || (compl_cont_حالت & CONT_SOL))
	    مليو_new_ميلاپ = ڳولا_for_exact_سٽ(st->ins_buf,
			    st->cur_ميلاپ_pos, compl_direction, compl_نمونو.string);
	وإلا
	    مليو_new_ميلاپ = searchit(عدم, st->ins_buf, st->cur_ميلاپ_pos,
				عدم, compl_direction, compl_نمونو.string, (صحيح)compl_نمونو.ڊيگهائي,
				1L, ڳولا_KEEP + ڳولا_NFMSG, RE_آخري, عدم);
	--نياپو_silent;
	إذا (!compl_started || st->سيٽ_ميلاپ_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->پهريون_ميلاپ_pos = *st->cur_ميلاپ_pos;
	    st->آخري_ميلاپ_pos = *st->cur_ميلاپ_pos;
	    st->سيٽ_ميلاپ_pos = FALSE;
	}
	وإلا إذا (st->پهريون_ميلاپ_pos.lnum == st->آخري_ميلاپ_pos.lnum
		&& st->پهريون_ميلاپ_pos.کانو == st->آخري_ميلاپ_pos.کانو)
	{
	    مليو_new_ميلاپ = FAIL;
	}
	وإلا إذا (compl_dir_forward()
		&& (st->prev_ميلاپ_pos.lnum > st->cur_ميلاپ_pos->lnum
		    || (st->prev_ميلاپ_pos.lnum == st->cur_ميلاپ_pos->lnum
			&& st->prev_ميلاپ_pos.کانو >= st->cur_ميلاپ_pos->کانو)))
	{
	    إذا (looped_around)
		مليو_new_ميلاپ = FAIL;
	    وإلا
		looped_around = TRUE;
	}
	وإلا إذا (!compl_dir_forward()
		&& (st->prev_ميلاپ_pos.lnum < st->cur_ميلاپ_pos->lnum
		    || (st->prev_ميلاپ_pos.lnum == st->cur_ميلاپ_pos->lnum
			&& st->prev_ميلاپ_pos.کانو <= st->cur_ميلاپ_pos->کانو)))
	{
	    إذا (looped_around)
		مليو_new_ميلاپ = FAIL;
	    وإلا
		looped_around = TRUE;
	}
	st->prev_ميلاپ_pos = *st->cur_ميلاپ_pos;
	إذا (مليو_new_ميلاپ == FAIL)
	    اكسر;

	// when ADDING, the text before the cursor matches, skip it
	إذا (compl_حالت_adding() && اندريون_curbuf
		&& شروعات_pos->lnum == st->cur_ميلاپ_pos->lnum
		&& شروعات_pos->کانو  == st->cur_ميلاپ_pos->کانو)
	    استمر;

	إذا (!اندريون_fuzzy_collect)
	    ptr = ins_compl_get_ايندڙ_لفظ_or_سٽ(st->ins_buf,
		    st->cur_ميلاپ_pos, &len, &cont_s_ipos);
	إذا (ptr == عدم || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    استمر;

	إذا (is_سڀ_کان_ويجهو_سرگرم() && اندريون_curbuf)
	{
	    score = st->cur_ميلاپ_pos->lnum - curwin->w_cursor.lnum;
	    إذا (score < 0)
		score = -score;
	}

	إذا (ins_compl_جوڙيو_infercase(ptr, len, p_ic,
			اندريون_curbuf ? عدم : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    إذا (اندريون_fuzzy_collect && score == compl_پهريون_ميلاپ->cp_ايندڙ->cp_score)
		compl_انگ_bests++;
	    مليو_new_ميلاپ = OK;
	    اكسر;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    أرجع مليو_new_ميلاپ;
}

#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    ساكن callback_T *
get_callback_if_cpt_func(char_u *p, صحيح idx)
{
    إذا (*p == 'o')
	أرجع &curbuf->b_ofu_cb;

    إذا (*p == 'F')
    {
	إذا (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    إذا (curbuf->b_p_cpt_cb == عدم
		    || idx < 0 || idx >= curbuf->b_p_cpt_ڳڻپ)
		أرجع عدم;
	    أرجع curbuf->b_p_cpt_cb[idx].cb_name != عدم
		? &curbuf->b_p_cpt_cb[idx] : عدم;
	}
	وإلا
	    أرجع &curbuf->b_cfu_cb; // 'cfu'
    }

    أرجع عدم;
}
#جيڪڏهن_پورو

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    ساكن فراغ
get_رجسٽر_completion(فراغ)
{
    صحيح		dir = compl_direction;
    yankreg_T	*reg = عدم;
    فراغ	*reg_ptr = عدم;
    صحيح		adding_گهڻائي = compl_حالت_adding();

    لكل (صحيح i = 0; i < انگ_REGISTERS; i++)
    {
	صحيح regname = 0;
	إذا (i == 0)
	    regname = '"';    // unnamed register
	وإلا إذا (i < 10)
	    regname = '0' + i;
	وإلا إذا (i == DELETION_رجسٽر)
	    regname = '-';
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	وإلا إذا (i == تارو_رجسٽر)
	    regname = '*';
	وإلا إذا (i == PLUS_رجسٽر)
	    regname = '+';
#جيڪڏهن_پورو
	وإلا
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	إذا (!valid_yank_reg(regname, FALSE) || regname == '_')
	    استمر;

	reg_ptr = get_رجسٽر(regname, FALSE);
	إذا (reg_ptr == عدم)
	    استمر;

	reg = (yankreg_T *)reg_ptr;

	لكل (صحيح j = 0; j < reg->y_سائيز; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    إذا (str == عدم)
		استمر;

	    إذا (adding_گهڻائي)
	    {
		صحيح str_len = (صحيح)STRLEN(str);
		إذا (str_len == 0)
		    استمر;

		إذا (!compl_orig_متن.string
			|| (p_ic ? STRNICMP(str, compl_orig_متن.string,
					    compl_orig_متن.ڊيگهائي) == 0
				: STRNCMP(str, compl_orig_متن.string,
					    compl_orig_متن.ڊيگهائي) == 0))
		{
		    إذا (ins_compl_جوڙيو_infercase(str, str_len, p_ic, عدم,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    وإلا
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_پڇاڙي = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		طالما (p < str_پڇاڙي && *p != NUL)
		{
		    char_u *old_p = p;
		    p = ڳوليو_لفظ_شروعات(p);
		    إذا (p >= str_پڇاڙي || *p == NUL)
			اكسر;

		    char_u *لفظ_پڇاڙي = ڳوليو_لفظ_پڇاڙي(p);

		    إذا (لفظ_پڇاڙي <= p)
		    {
			إذا (has_mbyte)
			    لفظ_پڇاڙي = p + (*mb_ptr2len)(p);
			وإلا
			    لفظ_پڇاڙي = p + 1;
		    }

		    إذا (لفظ_پڇاڙي > str_پڇاڙي)
			لفظ_پڇاڙي = str_پڇاڙي;

		    صحيح len = (صحيح)(لفظ_پڇاڙي - p);
		    إذا (len > 0 && (!compl_orig_متن.string
				|| (p_ic ? STRNICMP(p, compl_orig_متن.string,
						    compl_orig_متن.ڊيگهائي) == 0
					: STRNCMP(p, compl_orig_متن.string,
						    compl_orig_متن.ڊيگهائي) == 0)))
		    {
			إذا (ins_compl_جوڙيو_infercase(p, len, p_ic, عدم,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = لفظ_پڇاڙي;

		    إذا (p <= old_p)
		    {
			p = old_p + 1;
			إذا (has_mbyte && p < str_پڇاڙي)
			    p = old_p + (*mb_ptr2len)(old_p);
		    }
		}
	    }
	}

	// Free the register copy
	put_رجسٽر(regname, reg_ptr);
    }
}

/*
 * get the next set of completion matches for "type".
 * Returns TRUE if a new match is found. Otherwise returns FALSE.
 */
    ساكن صحيح
get_ايندڙ_completion_ميلاپ(صحيح type, ins_compl_ايندڙ_state_T *st, pos_T *ini)
{
    صحيح	مليو_new_ميلاپ = FALSE;

    بدل (type)
    {
	حالة -1:
	    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
	حالة CTRL_X_رستو_PATTERNS:
	حالة CTRL_X_رستو_مقرر_ڪري:
	    get_ايندڙ_include_file_completion(type);
	    اكسر;
#جيڪڏهن_پورو

	حالة CTRL_X_DICTIONARY:
	حالة CTRL_X_THESAURUS:
	    get_ايندڙ_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = عدم;
	    اكسر;

	حالة CTRL_X_ٽيگ_فهرست:
	    get_ايندڙ_ٽيگ_completion();
	    اكسر;

	حالة CTRL_X_FILES:
	    get_ايندڙ_filename_completion();
	    اكسر;

	حالة CTRL_X_CMDLINE:
	حالة CTRL_X_CMDLINE_CTRL_X:
	    get_ايندڙ_cmdline_completion();
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
	حالة CTRL_X_FUNCTION:
	    إذا (ctrl_x_گهڻائي_عام())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    وإلا
		expand_by_function(type, compl_نمونو.string, عدم);
	    اكسر;
	حالة CTRL_X_OMNI:
	    expand_by_function(type, compl_نمونو.string, عدم);
	    اكسر;
#جيڪڏهن_پورو

	حالة CTRL_X_SPELL:
	    get_ايندڙ_spell_completion(st->پهريون_ميلاپ_pos.lnum);
	    اكسر;

	حالة CTRL_X_رجسٽر:
	    get_رجسٽر_completion();
	    اكسر;

	افتراضي:	// normal ^P/^N and ^X^L
	    مليو_new_ميلاپ = get_ايندڙ_default_completion(st, ini);
	    إذا (مليو_new_ميلاپ == FAIL && st->ins_buf == curbuf)
		st->مليو_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    إذا (type != 0 && compl_curr_ميلاپ != compl_old_ميلاپ)
	مليو_new_ميلاپ = OK;

    أرجع مليو_new_ميلاپ;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    ساكن فراغ
strip_caret_numbers_اندريون_رکو(char_u *str)
{
    char_u  *اقرأ = str, *اكتب = str, *p;

    إذا (str == عدم)
	أرجع;

    طالما (*اقرأ)
    {
	إذا (*اقرأ == '^')
	{
	    p = اقرأ + 1;
	    طالما (vim_isdigit(*p))
		p++;
	    إذا ((*p == ',' || *p == '\0') && p != اقرأ + 1)
	    {
		اقرأ = p;
		استمر;
	    }
	    وإلا
		*اكتب++ = *اقرأ++;
	}
	وإلا
	    *اكتب++ = *اقرأ++;
    }
    *اكتب = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    ساكن صحيح
prepare_cpt_compl_funcs(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = عدم;
    صحيح		idx = 0;
    صحيح		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    إذا (cpt == عدم)
	أرجع FAIL;
    strip_caret_numbers_اندريون_رکو(cpt);

    لكل (p = cpt; *p;)
    {
	طالما (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	إذا (*p == NUL)
	    اكسر;

	cb = get_callback_if_cpt_func(p, idx);
	إذا (cb)
	{
	    إذا (get_userdefined_compl_info(curwin->w_cursor.کانو, cb, &startcol)
		    == FAIL)
	    {
		إذا (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		وإلا
		    startcol = -2;
	    }
	    وإلا إذا (startcol < 0 || startcol > curwin->w_cursor.کانو)
		startcol = curwin->w_cursor.کانو;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	وإلا
	    cpt_sources_array[idx].cs_startcol = -3;

	(فراغ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    أرجع OK;
#جيڪڏهن_پورو
    أرجع FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    ساكن فراغ
compl_ذريعو_شروعات_timer(صحيح ذريعو_idx اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    إذا (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[ذريعو_idx].compl_شروعات_tv);
	compl_وقت_ٽڪرو_expired = FALSE;
    }
#جيڪڏهن_پورو
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    ساكن صحيح
advance_cpt_sources_اشاريو_محفوظ(فراغ)
{
    إذا (cpt_sources_اشاريو >= 0 && cpt_sources_اشاريو < cpt_sources_ڳڻپ - 1)
    {
	cpt_sources_اشاريو++;
	أرجع OK;
    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    semsg(_(e_list_اشاريو_ٻاهريون_جو_range_nr), cpt_sources_اشاريو);
#جيڪڏهن_پورو
    أرجع FAIL;
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
    ساكن صحيح
ins_compl_get_exp(pos_T *ini)
{
    ساكن ins_compl_ايندڙ_state_T   st;
    ساكن صحيح			    st_cleared = FALSE;
    صحيح		ميلاپ_ڳڻپ;
    صحيح		مليو_new_ميلاپ;
    صحيح		type = ctrl_x_گهڻائي;
    صحيح		may_advance_cpt_idx = FALSE;
    pos_T	شروعات_pos = *ini;
    صحيح		عام_گهڻائي_strict = FALSE;
    ڊگهو	compl_timeout_save = 0;

    إذا (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	إذا (!st_cleared)
	{
	    CLEAR_ميدان(st);
	    st_cleared = TRUE;
	}
	st.مليو_all = FALSE;
	st.ins_buf = curbuf;
	vim_free(st.e_cpt_copy);
	// Make a copy of 'complete', in case the buffer is wiped out.
	st.e_cpt_copy = vim_strsave((compl_cont_حالت & CONT_LOCAL)
					    ? (char_u *)"." : curbuf->b_p_cpt);
	strip_caret_numbers_اندريون_رکو(st.e_cpt_copy);
	st.e_cpt = st.e_cpt_copy == عدم ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#وصف_ڪريو LOOKBACK_سٽ_ڳڻپ	1000
	إذا (compl_autocomplete && is_سڀ_کان_ويجهو_سرگرم())
	{
	    شروعات_pos.lnum = MAX(1, شروعات_pos.lnum - LOOKBACK_سٽ_ڳڻپ);
	    شروعات_pos.کانو = 0;
	}
	st.آخري_ميلاپ_pos = st.پهريون_ميلاپ_pos = شروعات_pos;
    }
    وإلا إذا (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_ميلاپ = compl_curr_ميلاپ;	// remember the last current match
    st.cur_ميلاپ_pos = (compl_dir_forward())
				    ? &st.آخري_ميلاپ_pos : &st.پهريون_ميلاپ_pos;

    عام_گهڻائي_strict = ctrl_x_گهڻائي_عام() && !ctrl_x_گهڻائي_سٽ_or_eval()
	&& !(compl_cont_حالت & CONT_LOCAL) && cpt_sources_array != عدم;
    إذا (عام_گهڻائي_strict)
    {
	cpt_sources_اشاريو = 0;
	إذا (compl_autocomplete || p_cto > 0)
	{
	    compl_ذريعو_شروعات_timer(0);
	    compl_وقت_ٽڪرو_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    لكل (;;)
    {
	مليو_new_ميلاپ = FAIL;
	st.سيٽ_ميلاپ_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	إذا ((ctrl_x_گهڻائي_عام() || ctrl_x_گهڻائي_سٽ_or_eval())
					&& (!compl_started || st.مليو_all))
	{
	    صحيح حالت = process_ايندڙ_cpt_قدر(&st, &type, &شروعات_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    إذا (حالت == INS_COMPL_CPT_پڇاڙي)
		اكسر;
	    إذا (حالت == INS_COMPL_CPT_CONT)
	    {
		إذا (may_advance_cpt_idx)
		{
		    إذا (!advance_cpt_sources_اشاريو_محفوظ())
			اكسر;
		    compl_ذريعو_شروعات_timer(cpt_sources_اشاريو);
		}
		استمر;
	    }
	}

	إذا (عام_گهڻائي_strict && type == CTRL_X_FUNCTION
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
	مليو_new_ميلاپ = get_ايندڙ_completion_ميلاپ(type, &st, &شروعات_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	إذا (compl_نمونو.string == عدم)
	    اكسر;

	إذا (may_advance_cpt_idx)
	{
	    إذا (!advance_cpt_sources_اشاريو_محفوظ())
		اكسر;
	    compl_ذريعو_شروعات_timer(cpt_sources_اشاريو);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	//ميلاپ
	إذا ((ctrl_x_گهڻائي_not_default() && !ctrl_x_گهڻائي_سٽ_or_eval())
						|| مليو_new_ميلاپ != FAIL)
	{
	    إذا (got_int)
		اكسر;
	    // Fill the popup menu as soon as possible.
	    إذا (type != -1)
		ins_compl_check_ڪنجيون(0, FALSE);

	    إذا ((ctrl_x_گهڻائي_not_default()
			&& !ctrl_x_گهڻائي_سٽ_or_eval()) || compl_interrupted)
		اكسر;
	    compl_started = compl_وقت_ٽڪرو_expired ? FALSE : TRUE;
	}
	وإلا
	{
	    // Mark a buffer scanned when it has been scanned completely
	    إذا (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_رستو_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	إذا (عام_گهڻائي_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	إذا (!compl_dir_forward())
	    طالما (compl_curr_ميلاپ->cp_prev
		    && !ميلاپ_at_original_متن(compl_curr_ميلاپ->cp_prev))
		compl_curr_ميلاپ = compl_curr_ميلاپ->cp_prev;
    }
    cpt_sources_اشاريو = -1;
    compl_started = TRUE;

    إذا ((ctrl_x_گهڻائي_عام() || ctrl_x_گهڻائي_سٽ_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	مليو_new_ميلاپ = FAIL;

    ميلاپ_ڳڻپ = -1;		// total of matches, unknown
    إذا (مليو_new_ميلاپ == FAIL || (ctrl_x_گهڻائي_not_default()
		&& !ctrl_x_گهڻائي_سٽ_or_eval()))
	ميلاپ_ڳڻپ = ins_compl_make_cyclic();

    إذا (cot_fuzzy() && compl_get_longest && compl_انگ_bests > 0)
	fuzzy_longest_ميلاپ();

    إذا (compl_old_ميلاپ != عدم)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_ميلاپ = compl_dir_forward() ? compl_old_ميلاپ->cp_ايندڙ
						    : compl_old_ميلاپ->cp_prev;
	إذا (compl_curr_ميلاپ == عدم)
	    compl_curr_ميلاپ = compl_old_ميلاپ;
    }
    may_trigger_modechanged();

    إذا (ميلاپ_ڳڻپ > 0 && !ctrl_x_گهڻائي_spell())
    {
	إذا (is_سڀ_کان_ويجهو_سرگرم() && !ins_compl_has_preinsert())
	    ترتيب_ڏيو_compl_ميلاپ_list(cp_compare_سڀ_کان_ويجهو);

	إذا (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_ترتيب_ڏيو();
    }

    أرجع ميلاپ_ڳڻپ;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    ساكن فراغ
ins_compl_update_shown_ميلاپ(فراغ)
{
    string_T	*leader;

    (فراغ)get_leader_for_startcol(عدم, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_ميلاپ, TRUE);

    طالما (!ins_compl_equal(compl_shown_ميلاپ,
		leader->string, (صحيح)leader->ڊيگهائي)
	    && compl_shown_ميلاپ->cp_ايندڙ != عدم
	    && !is_پهريون_ميلاپ(compl_shown_ميلاپ->cp_ايندڙ))
    {
	compl_shown_ميلاپ = compl_shown_ميلاپ->cp_ايندڙ;
	leader = get_leader_for_startcol(compl_shown_ميلاپ, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    إذا (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_ميلاپ,
		leader->string, (صحيح)leader->ڊيگهائي)
	    && (compl_shown_ميلاپ->cp_ايندڙ == عدم
		|| is_پهريون_ميلاپ(compl_shown_ميلاپ->cp_ايندڙ)))
    {
	طالما (!ins_compl_equal(compl_shown_ميلاپ,
		    leader->string, (صحيح)leader->ڊيگهائي)
		&& compl_shown_ميلاپ->cp_prev != عدم
		&& !is_پهريون_ميلاپ(compl_shown_ميلاپ->cp_prev))
	{
	    compl_shown_ميلاپ = compl_shown_ميلاپ->cp_prev;
	    leader = get_leader_for_startcol(compl_shown_ميلاپ, TRUE);
	}
    }
}

/*
 * Delete the old text being completed.
 */
    فراغ
ins_compl_delete(فراغ)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    صحيح کانو = compl_کانو + (compl_حالت_adding() ? compl_ڊيگهائي : 0);
    string_T	بچيل = {عدم, 0};
    صحيح	    orig_کانو;

    إذا (ins_compl_preinsert_effect())
    {
	کانو += (صحيح)ins_compl_leader_len();
	curwin->w_cursor.کانو = compl_ins_پڇاڙي_کانو;
    }

    إذا (curwin->w_cursor.lnum > compl_lnum)
    {
	إذا (curwin->w_cursor.کانو < ml_get_curline_len())
	{
	    char_u *سٽ = ml_get_cursor();
	    بچيل.ڊيگهائي = ml_get_cursor_len();
	    بچيل.string = vim_strnsave(سٽ, بچيل.ڊيگهائي);
	    إذا (بچيل.string == عدم)
		أرجع;
	}
	طالما (curwin->w_cursor.lnum > compl_lnum)
	{
	    إذا (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		إذا (بچيل.string)
		    vim_free(بچيل.string);
		أرجع;
	    }
	    deleted_سٽون_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.کانو = ml_get_curline_len();
    }

    إذا ((صحيح)curwin->w_cursor.کانو > کانو)
    {
	إذا (stop_arrow() == FAIL)
	{
	    إذا (بچيل.string)
		vim_free(بچيل.string);
	    أرجع;
	}
	backspace_until_ڪالم(کانو);
	compl_ins_پڇاڙي_کانو = curwin->w_cursor.کانو;
    }

    إذا (بچيل.string != عدم)
    {
	orig_کانو = curwin->w_cursor.کانو;
	ins_str(بچيل.string, بچيل.ڊيگهائي);
	curwin->w_cursor.کانو = orig_کانو;
	vim_free(بچيل.string);
    }
    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    بدليو_cline_bef_curs();
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // clear v:completed_item
    سيٽ_vim_var_dict(VV_COMPLETED_شي, dict_alloc_تالو(VAR_FIXED));
#جيڪڏهن_پورو
}

/*
 * Insert a completion string that contains newlines.
 * The string is split and inserted line by line.
 */
    ساكن فراغ
ins_compl_expand_گڻو(char_u *str)
{
    char_u	*شروعات = str;
    char_u	*curr = str;
    صحيح		بنياد_indent = get_indent();

    طالما (*curr != NUL)
    {
	إذا (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    إذا (curr > شروعات)
		ins_char_بائيٽون(شروعات, (صحيح)(curr - شروعات));

	    // Handle newline
	    open_سٽ(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, بنياد_indent, عدم);
	    شروعات = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    إذا (curr > شروعات)
	ins_char_بائيٽون(شروعات, (صحيح)(curr - شروعات));

    compl_ins_پڇاڙي_کانو = curwin->w_cursor.کانو;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    ساكن char_u *
ڳوليو_common_اڳياڙي(size_t *اڳياڙي_len, صحيح curbuf_only)
{
    compl_T     *compl;
    صحيح		*ميلاپ_ڳڻپ;
    char_u	*پهريون = عدم;
    صحيح		len = -1;
    صحيح		is_cpt_completion = (cpt_sources_array != عدم);

    إذا (!is_cpt_completion)
	أرجع عدم;

    ميلاپ_ڳڻپ = ALLOC_CLEAR_MULT(صحيح, cpt_sources_ڳڻپ);
    إذا (ميلاپ_ڳڻپ == عدم)
	أرجع عدم;

    (فراغ)get_leader_for_startcol(عدم, TRUE); // Clear the cache

    compl = compl_پهريون_ميلاپ;
    افعل
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	إذا (ctrl_x_گهڻائي_عام() && !p_inf && compl_orig_متن.string
		&& !ignorecase(compl_orig_متن.string))
	    compl->cp_flags &= ~CP_ICASE;

	إذا (!ميلاپ_at_original_متن(compl)
		&& (leader->string == عدم
		    || ins_compl_equal_sc(compl, leader->string,
			(صحيح)leader->ڊيگهائي)))
	{
	    // Limit number of items from each source if max_items is set.
	    صحيح ميلاپ_حد_بندي_exceeded = FALSE;
	    صحيح cur_ذريعو = compl->cp_cpt_ذريعو_idx;

	    إذا (cur_ذريعو != -1)
	    {
		ميلاپ_ڳڻپ[cur_ذريعو]++;
		صحيح max_matches = cpt_sources_array[cur_ذريعو].cs_max_matches;
		إذا (max_matches > 0 && ميلاپ_ڳڻپ[cur_ذريعو] > max_matches)
		    ميلاپ_حد_بندي_exceeded = TRUE;
	    }

	    إذا (!ميلاپ_حد_بندي_exceeded && (!curbuf_only
			|| (cur_ذريعو != -1
			    && cpt_sources_array[cur_ذريعو].cs_flag == '.')))
	    {
		إذا (پهريون == عدم && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    پهريون = compl->cp_str.string;
		    len = (صحيح)STRLEN(پهريون);
		}
		وإلا إذا (پهريون != عدم)
		{
		    صحيح j = 0;  // count in bytes
		    char_u *s1 = پهريون;
		    char_u *s2 = compl->cp_str.string;

		    طالما (j < len && *s1 != NUL && *s2 != NUL)
		    {
			إذا (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    اكسر;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    إذا (len == 0)
			اكسر;
		}
	    }
	}
	compl = compl->cp_ايندڙ;
    } طالما (compl != عدم && !is_پهريون_ميلاپ(compl));

    vim_free(ميلاپ_ڳڻپ);

    إذا (len > (صحيح)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	إذا (len == (صحيح)STRLEN(پهريون))
	{
	    char_u *سٽ = ml_get_curline();
	    char_u *p = سٽ + curwin->w_cursor.کانو;
	    إذا (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *پڇاڙي = ڳوليو_لفظ_پڇاڙي(p);
		صحيح متن_len = پڇاڙي - p;
		إذا (متن_len > 0
			&& متن_len < (len - (صحيح)ins_compl_leader_len())
			&& STRNCMP(پهريون + len - متن_len, p, متن_len) == 0)
		    len -= متن_len;
	    }
	}
	*اڳياڙي_len = (size_t)len;
	أرجع پهريون;
    }
    أرجع عدم;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    فراغ
ins_compl_وجهو(صحيح move_cursor, صحيح وجهو_اڳياڙي)
{
    صحيح		compl_len = get_compl_len();
    صحيح		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_ميلاپ->cp_str.string;
    size_t	cp_str_len = compl_shown_ميلاپ->cp_str.ڊيگهائي;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_گڻو = vim_strchr(cp_str, '\n');

    إذا (وجهو_اڳياڙي)
    {
	cp_str = ڳوليو_common_اڳياڙي(&cp_str_len, FALSE);
	إذا (cp_str == عدم)
	{
	    cp_str = ڳوليو_common_اڳياڙي(&cp_str_len, TRUE);
	    إذا (cp_str == عدم)
	    {
		cp_str = compl_shown_ميلاپ->cp_str.string;
		cp_str_len = compl_shown_ميلاپ->cp_str.ڊيگهائي;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    وإلا إذا (cpt_sources_array != عدم)
    {
	صحيح	cpt_idx = compl_shown_ميلاپ->cp_cpt_ذريعو_idx;
	إذا (cpt_idx >= 0 && compl_کانو >= 0)
	{
	    صحيح	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    إذا (startcol >= 0 && startcol < (صحيح)compl_کانو)
	    {
		صحيح skip = (صحيح)compl_کانو - startcol;
		إذا ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    إذا (compl_len < (صحيح)cp_str_len)
    {
	إذا (has_گڻو)
	    ins_compl_expand_گڻو(cp_str + compl_len);
	وإلا
	{
	    ins_compl_وجهو_بائيٽون(cp_str + compl_len,
		    وجهو_اڳياڙي ? (صحيح)cp_str_len - compl_len : -1);
	    إذا ((preinsert || وجهو_اڳياڙي) && move_cursor)
		curwin->w_cursor.کانو -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    إذا (ميلاپ_at_original_متن(compl_shown_ميلاپ)
	    || (preinsert && !وجهو_اڳياڙي))
	compl_استعمال_ٿيل_ميلاپ = FALSE;
    وإلا
	compl_استعمال_ٿيل_ميلاپ = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_ميلاپ);

	سيٽ_vim_var_dict(VV_COMPLETED_شي, dict);
    }
#جيڪڏهن_پورو
    compl_hi_on_autocompl_longest = وجهو_اڳياڙي && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    ساكن فراغ
ins_compl_show_filename(فراغ)
{
    حرف	*lead = _("match in file");
    صحيح		space = sc_کانو - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    إذا (space <= 0)
	أرجع;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    لكل (s = e = compl_shown_ميلاپ->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	طالما (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    إذا (!compl_autocomplete)
    {
	نياپو_hist_off = TRUE;
	vim_snprintf((حرف *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_ميلاپ->cp_fname ? "<" : "", s);
	نياپو((حرف *)IObuff);
	نياپو_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    ساكن compl_T *
ڳوليو_ايندڙ_ميلاپ_اندريون_menu(فراغ)
{
    صحيح	    is_forward = compl_shows_dir_forward();
    compl_T *ميلاپ = compl_shown_ميلاپ;

    افعل
	ميلاپ = is_forward ? ميلاپ->cp_ايندڙ : ميلاپ->cp_prev;
    طالما (ميلاپ->cp_ايندڙ && !ميلاپ->cp_اندريون_ميلاپ_array
	    && !ميلاپ_at_original_متن(ميلاپ));
    أرجع ميلاپ;
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
    ساكن صحيح
ڳوليو_ايندڙ_completion_ميلاپ(
	صحيح	allow_get_expansion,
	صحيح	todo,		// repeat completion this many times
	صحيح	advance,
	صحيح	*انگ_matches)
{
    صحيح		مليو_پڇاڙي = FALSE;
    compl_T	*مليو_compl = عدم;
    بنانشان صحيح cur_cot_flags = get_cot_flags();
    صحيح		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    طالما (--todo >= 0)
    {
	إذا (compl_shows_dir_forward() && compl_shown_ميلاپ->cp_ايندڙ != عدم)
	{
	    إذا (compl_ميلاپ_array != عدم)
		compl_shown_ميلاپ = ڳوليو_ايندڙ_ميلاپ_اندريون_menu();
	    وإلا
		compl_shown_ميلاپ = compl_shown_ميلاپ->cp_ايندڙ;
	    مليو_پڇاڙي = (compl_پهريون_ميلاپ != عدم
		    && (is_پهريون_ميلاپ(compl_shown_ميلاپ->cp_ايندڙ)
			|| is_پهريون_ميلاپ(compl_shown_ميلاپ)));
	}
	وإلا إذا (compl_shows_dir_backward()
		&& compl_shown_ميلاپ->cp_prev != عدم)
	{
	    مليو_پڇاڙي = is_پهريون_ميلاپ(compl_shown_ميلاپ);
	    إذا (compl_ميلاپ_array != عدم)
		compl_shown_ميلاپ = ڳوليو_ايندڙ_ميلاپ_اندريون_menu();
	    وإلا
		compl_shown_ميلاپ = compl_shown_ميلاپ->cp_prev;
	    مليو_پڇاڙي |= is_پهريون_ميلاپ(compl_shown_ميلاپ);
	}
	وإلا
	{
	    إذا (!allow_get_expansion)
	    {
		إذا (advance)
		{
		    إذا (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    وإلا
			compl_pending += todo + 1;
		}
		أرجع -1;
	    }

	    إذا (!compl_no_select && advance)
	    {
		إذا (compl_shows_dir_backward())
		    --compl_pending;
		وإلا
		    ++compl_pending;
	    }

	    // Find matches.
	    *انگ_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    طالما (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		إذا (compl_pending > 0 && compl_shown_ميلاپ->cp_ايندڙ != عدم)
		{
		    compl_shown_ميلاپ = compl_shown_ميلاپ->cp_ايندڙ;
		    --compl_pending;
		}
		وإلا إذا (compl_pending < 0 && compl_shown_ميلاپ->cp_prev != عدم)
		{
		    compl_shown_ميلاپ = compl_shown_ميلاپ->cp_prev;
		    ++compl_pending;
		}
		وإلا
		    اكسر;
	    }
	    مليو_پڇاڙي = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_ميلاپ, FALSE);

	إذا (!ميلاپ_at_original_متن(compl_shown_ميلاپ)
		&& leader->string != عدم
		&& !ins_compl_equal(compl_shown_ميلاپ,
		    leader->string, (صحيح)leader->ڊيگهائي)
		&& !(cot_fuzzy()
		    && compl_shown_ميلاپ->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	وإلا
	    // Remember a matching item.
	    مليو_compl = compl_shown_ميلاپ;

	// Stop at the end of the list when we found a usable match.
	إذا (مليو_پڇاڙي)
	{
	    إذا (مليو_compl != عدم)
	    {
		compl_shown_ميلاپ = مليو_compl;
		اكسر;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    أرجع OK;
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
    ساكن صحيح
ins_compl_ايندڙ(
    صحيح	    allow_get_expansion,
    صحيح	    ڳڻپ,		// repeat completion this many times; should
				// be at least 1
    صحيح	    وجهو_ميلاپ)	// Insert the newly selected match
{
    صحيح	    انگ_matches = -1;
    صحيح	    todo = ڳڻپ;
    صحيح	    advance;
    صحيح	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    بنانشان صحيح cur_cot_flags = get_cot_flags();
    صحيح	    compl_no_وجهو = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    صحيح	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    إذا (compl_shown_ميلاپ == عدم)
	أرجع -1;

    إذا (compl_leader.string != عدم
	    && !ميلاپ_at_original_متن(compl_shown_ميلاپ)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_ميلاپ();

    إذا (allow_get_expansion && وجهو_ميلاپ
	    && (!compl_get_longest || compl_استعمال_ٿيل_ميلاپ))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = ڳڻپ != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    إذا (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    إذا (ڳوليو_ايندڙ_completion_ميلاپ(allow_get_expansion, todo, advance,
							&انگ_matches) == -1)
	أرجع -1;

    إذا (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	أرجع -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    إذا (!started && ins_compl_preinsert_longest())
	ins_compl_وجهو(TRUE, TRUE);
    وإلا إذا (compl_no_وجهو && !started && !compl_preinsert)
    {
	ins_compl_وجهو_بائيٽون(compl_orig_متن.string + get_compl_len(), -1);
	compl_استعمال_ٿيل_ميلاپ = FALSE;
    }
    وإلا إذا (وجهو_ميلاپ)
    {
	إذا (!compl_get_longest || compl_استعمال_ٿيل_ميلاپ)
	{
	    صحيح preinsert_longest = ins_compl_preinsert_longest()
		&& ميلاپ_at_original_متن(compl_shown_ميلاپ); // none selected
	    ins_compl_وجهو(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	وإلا
	    ins_compl_وجهو_بائيٽون(compl_leader.string + get_compl_len(), -1);
    }
    وإلا
	compl_استعمال_ٿيل_ميلاپ = FALSE;

    إذا (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	إذا (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	وإلا
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (gui.اندريون_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    ٻاهريون_flush_cursor(FALSE, FALSE);
	}
#جيڪڏهن_پورو

	// Delete old text to be replaced, since we're still searching and
	// don't want to match ourselves!
	ins_compl_delete();
    }

    // Enter will select a match when the match wasn't inserted and the popup
    // menu is visible.
    إذا (compl_no_وجهو && !started
				 && !ميلاپ_at_original_متن(compl_shown_ميلاپ))
	compl_enter_selects = TRUE;
    وإلا
	compl_enter_selects = !وجهو_ميلاپ && compl_ميلاپ_array != عدم;

    // Show the file name for the match (if any)
    إذا (compl_shown_ميلاپ->cp_fname != عدم)
	ins_compl_show_filename();

    أرجع انگ_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    ساكن فراغ
check_elapsed_وقت(فراغ)
{
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    elapsed_T	*شروعات_tv
	= &cpt_sources_array[cpt_sources_اشاريو].compl_شروعات_tv;
    ڊگهو	elapsed_ms = ELAPSED_FUNC(*شروعات_tv);

    إذا (elapsed_ms > compl_timeout_ms)
    {
	compl_وقت_ٽڪرو_expired = TRUE;
	DECAY_COMPL_TIMEOUT();
    }
#جيڪڏهن_پورو
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
    فراغ
ins_compl_check_ڪنجيون(صحيح frequency, صحيح اندريون_compl_func)
{
    ساكن صحيح	ڳڻپ = 0;
    صحيح		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    إذا (!اندريون_compl_func && (using_script() || ex_عام_busy))
	أرجع;

    // Only do this at regular intervals
    إذا (++ڳڻپ < frequency)
	أرجع;
    ڳڻپ = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    إذا (c != NUL
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#جيڪڏهن_پورو
       )
    {
	إذا (vim_is_ctrl_x_ڪنجي(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = محفوظ_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (فراغ)ins_compl_ايندڙ(FALSE, ins_compl_key2count(c),
				      c != K_مٿي && c != K_هيٺ_ڏانهن);
	}
	وإلا
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = محفوظ_vgetc();
	    إذا (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		إذا (c != Ctrl_R && (ڪنجي_Typed
			    || (اندريون_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    وإلا
    {
	صحيح عام_گهڻائي_strict = ctrl_x_گهڻائي_عام()
	    && !ctrl_x_گهڻائي_سٽ_or_eval() && !(compl_cont_حالت & CONT_LOCAL)
	    && cpt_sources_array != عدم && cpt_sources_اشاريو >= 0;
	إذا (عام_گهڻائي_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_وقت();
    }

    إذا (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	صحيح todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(فراغ)ins_compl_ايندڙ(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    ساكن صحيح
ins_compl_key2dir(صحيح c)
{
    إذا (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_مٿي || c == K_مٿي)
	أرجع BACKWARD;
    أرجع FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    ساكن صحيح
ins_compl_pum_ڪنجي(صحيح c)
{
    أرجع pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_مٿي
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_هيٺ_ڏانهن
		     || c == K_مٿي || c == K_هيٺ_ڏانهن);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    ساكن صحيح
ins_compl_key2count(صحيح c)
{
    إذا (ins_compl_pum_ڪنجي(c) && c != K_مٿي && c != K_هيٺ_ڏانهن)
    {
	صحيح h = pum_get_اوچائي();
	إذا (h > 3)
	    h -= 2; // keep some context
	أرجع h;
    }
    أرجع 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    ساكن صحيح
ins_compl_use_ميلاپ(صحيح c)
{
    بدل (c)
    {
	حالة K_مٿي:
	حالة K_هيٺ_ڏانهن:
	حالة K_PAGEDOWN:
	حالة K_KPAGEDOWN:
	حالة K_S_هيٺ_ڏانهن:
	حالة K_PAGEUP:
	حالة K_KPAGEUP:
	حالة K_S_مٿي:
	    أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    ساكن صحيح
get_عام_compl_info(char_u *سٽ, صحيح startcol, colnr_T curs_کانو)
{
    إذا ((compl_cont_حالت & CONT_SOL) || ctrl_x_گهڻائي_رستو_مقرر_ڪري())
    {
	إذا (!compl_حالت_adding())
	{
	    طالما (--startcol >= 0 && vim_isIDc(سٽ[startcol]))
		;
	    compl_کانو += ++startcol;
	    compl_ڊيگهائي = curs_کانو - startcol;
	}
	إذا (p_ic)
	{
	    compl_نمونو.string = str_foldcase(سٽ + compl_کانو,
		    compl_ڊيگهائي, عدم, 0);
	    إذا (compl_نمونو.string == عدم)
	    {
		compl_نمونو.ڊيگهائي = 0;
		أرجع FAIL;
	    }
	    compl_نمونو.ڊيگهائي = STRLEN(compl_نمونو.string);
	}
	وإلا
	{
	    compl_نمونو.string = vim_strnsave(سٽ + compl_کانو, (size_t)compl_ڊيگهائي);
	    إذا (compl_نمونو.string == عدم)
	    {
		compl_نمونو.ڊيگهائي = 0;
		أرجع FAIL;
	    }
	    compl_نمونو.ڊيگهائي = (size_t)compl_ڊيگهائي;
	}
    }
    وإلا إذا (compl_حالت_adding())
    {
	char_u	    *اڳياڙي = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	إذا (!vim_iswordp(سٽ + compl_کانو)
		|| (compl_کانو > 0
		    && (vim_iswordp(mb_prevptr(سٽ, سٽ + compl_کانو)))))
	{
	    اڳياڙي = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(عدم, سٽ + compl_کانو, compl_ڊيگهائي) + prefixlen;
	compl_نمونو.string = alloc(n);
	إذا (compl_نمونو.string == عدم)
	{
	    compl_نمونو.ڊيگهائي = 0;
	    أرجع FAIL;
	}
	STRCPY((حرف *)compl_نمونو.string, اڳياڙي);
	(فراغ)quote_meta(compl_نمونو.string + prefixlen,
		سٽ + compl_کانو, compl_ڊيگهائي);
	compl_نمونو.ڊيگهائي = n - 1;
    }
    وإلا إذا (--startcol < 0
	    || !vim_iswordp(mb_prevptr(سٽ, سٽ + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_نمونو.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	إذا (compl_نمونو.string == عدم)
	{
	    compl_نمونو.ڊيگهائي = 0;
	    أرجع FAIL;
	}
	compl_نمونو.ڊيگهائي = len;
	compl_کانو += curs_کانو;
	compl_ڊيگهائي = 0;
	compl_from_nonkeyword = TRUE;
    }
    وإلا
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	إذا (has_mbyte)
	{
	    صحيح بنياد_class;
	    صحيح سر_off;

	    startcol -= (*mb_سر_off)(سٽ, سٽ + startcol);
	    بنياد_class = mb_get_class(سٽ + startcol);
	    طالما (--startcol >= 0)
	    {
		سر_off = (*mb_سر_off)(سٽ, سٽ + startcol);
		إذا (بنياد_class != mb_get_class(سٽ + startcol
			    - سر_off))
		    اكسر;
		startcol -= سر_off;
	    }
	}
	وإلا
	{
	    طالما (--startcol >= 0 && vim_iswordc(سٽ[startcol]))
		;
	}

	compl_کانو += ++startcol;
	compl_ڊيگهائي = (صحيح)curs_کانو - startcol;
	إذا (compl_ڊيگهائي == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_نمونو.string = alloc(7);
	    إذا (compl_نمونو.string == عدم)
	    {
		compl_نمونو.ڊيگهائي = 0;
		أرجع FAIL;
	    }
	    STRCPY((حرف *)compl_نمونو.string, "\\<");
	    (فراغ)quote_meta(compl_نمونو.string + 2, سٽ + compl_کانو, 1);
	    STRCAT((حرف *)compl_نمونو.string, "\\k");
	    compl_نمونو.ڊيگهائي = STRLEN(compl_نمونو.string);
	}
	وإلا
	{
	    size_t  n = quote_meta(عدم, سٽ + compl_کانو, compl_ڊيگهائي) + 2;

	    compl_نمونو.string = alloc(n);
	    إذا (compl_نمونو.string == عدم)
	    {
		compl_نمونو.ڊيگهائي = 0;
		أرجع FAIL;
	    }
	    STRCPY((حرف *)compl_نمونو.string, "\\<");
	    (فراغ)quote_meta(compl_نمونو.string + 2, سٽ + compl_کانو,
		    compl_ڊيگهائي);
	    compl_نمونو.ڊيگهائي = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    إذا (ctrl_x_گهڻائي_عام() && !(compl_cont_حالت & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	إذا (تياري_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ساكن صحيح
get_wholeline_compl_info(char_u *سٽ, colnr_T curs_کانو)
{
    compl_کانو = (colnr_T)getwhitecols(سٽ);
    compl_ڊيگهائي = (صحيح)curs_کانو - (صحيح)compl_کانو;
    إذا (compl_ڊيگهائي < 0)	// cursor in indent: empty pattern
	compl_ڊيگهائي = 0;
    إذا (p_ic)
    {
	compl_نمونو.string = str_foldcase(سٽ + compl_کانو, compl_ڊيگهائي,
		عدم, 0);
	إذا (compl_نمونو.string == عدم)
	{
	    compl_نمونو.ڊيگهائي = 0;
	    أرجع FAIL;
	}
	compl_نمونو.ڊيگهائي = STRLEN(compl_نمونو.string);
    }
    وإلا
    {
	compl_نمونو.string = vim_strnsave(سٽ + compl_کانو, (size_t)compl_ڊيگهائي);
	إذا (compl_نمونو.string == عدم)
	{
	    compl_نمونو.ڊيگهائي = 0;
	    أرجع FAIL;
	}
	compl_نمونو.ڊيگهائي = (size_t)compl_ڊيگهائي;
    }

    أرجع OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ساكن صحيح
get_filename_compl_info(char_u *سٽ, صحيح startcol, colnr_T curs_کانو)
{
    // Go back to just before the first filename character.
    إذا (startcol > 0)
    {
	char_u	*p = سٽ + startcol;

	MB_PTR_پٺ(سٽ, p);
	طالما (p > سٽ && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_پٺ(سٽ, p);
	إذا (p == سٽ && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	وإلا
	    startcol = (صحيح)(p - سٽ) + 1;
    }

    compl_کانو += startcol;
    compl_ڊيگهائي = (صحيح)curs_کانو - startcol;
    compl_نمونو.string = addstar(سٽ + compl_کانو, compl_ڊيگهائي, EXPAND_FILES);
    إذا (compl_نمونو.string == عدم)
    {
	compl_نمونو.ڊيگهائي = 0;
	أرجع FAIL;
    }

    compl_نمونو.ڊيگهائي = STRLEN(compl_نمونو.string);

    أرجع OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ساكن صحيح
get_cmdline_compl_info(char_u *سٽ, colnr_T curs_کانو)
{
    compl_نمونو.string = vim_strnsave(سٽ, curs_کانو);
    إذا (compl_نمونو.string == عدم)
    {
	compl_نمونو.ڊيگهائي = 0;
	أرجع FAIL;
    }
    compl_نمونو.ڊيگهائي = curs_کانو;
    سيٽ_cmd_context(&compl_xp, compl_نمونو.string,
	    (صحيح)compl_نمونو.ڊيگهائي, curs_کانو, FALSE);
    إذا (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_کانو = curs_کانو;
    وإلا
	compl_کانو = (صحيح)(compl_xp.xp_نمونو - compl_نمونو.string);
    compl_ڊيگهائي = curs_کانو - compl_کانو;

    أرجع OK;
}

#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    ساكن صحيح
سيٽ_compl_globals(
    صحيح		startcol اڻ_استعمال_ٿيل,
    colnr_T	curs_کانو اڻ_استعمال_ٿيل,
    صحيح		is_cpt_compl اڻ_استعمال_ٿيل)
{
    إذا (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_نمونو);
	إذا (startcol < compl_کانو)
	    أرجع prepend_startcol_متن(&cpt_compl_نمونو, &compl_orig_متن,
		    startcol);
	وإلا
	{
	    cpt_compl_نمونو.string = vim_strnsave(compl_orig_متن.string,
		    compl_orig_متن.ڊيگهائي);
	    cpt_compl_نمونو.ڊيگهائي = compl_orig_متن.ڊيگهائي;
	}
    }
    وإلا
    {
	إذا (startcol < 0 || startcol > curs_کانو)
	    startcol = curs_کانو;

	// Re-obtain line in case it has changed
	char_u	*سٽ = ml_get(curwin->w_cursor.lnum);
	صحيح	len = curs_کانو - startcol;

	compl_نمونو.string = vim_strnsave(سٽ + startcol, (size_t)len);
	إذا (compl_نمونو.string == عدم)
	{
	    compl_نمونو.ڊيگهائي = 0;
	    أرجع FAIL;
	}
	compl_نمونو.ڊيگهائي = (size_t)len;
	compl_کانو = startcol;
	compl_ڊيگهائي = len;
    }

    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Get the pattern, column and length for user defined completion ('omnifunc',
 * 'completefunc' and 'thesaurusfunc')
 * Uses the global variable: spell_bad_len
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 * "startcol", when not NULL, contains the column returned by function.
 */
    ساكن صحيح
get_userdefined_compl_info(
    colnr_T	curs_کانو اڻ_استعمال_ٿيل,
    callback_T	*cb اڻ_استعمال_ٿيل,
    صحيح		*startcol اڻ_استعمال_ٿيل)
{
    صحيح		ret = FAIL;

#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	دليلون[3];
    صحيح		کانو;
    char_u	*funcname = عدم;
    pos_T	pos;
    صحيح		save_State = State;
    صحيح		is_cpt_function = (cb != عدم);
    صحيح		use_sandbox = is_cpt_function
			    && was_سيٽ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    إذا (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_گهڻائي);
	إذا (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_سيٽ), ctrl_x_گهڻائي_function()
		    ? "completefunc" : "omnifunc");
	    أرجع FAIL;
	}
	cb = get_وجهو_callback(ctrl_x_گهڻائي);
    }

    دليلون[0].v_type = VAR_عدد;
    دليلون[0].vval.v_عدد = 1;
    دليلون[1].v_type = VAR_STRING;
    دليلون[1].vval.v_string = (char_u *)"";
    دليلون[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    إذا (use_sandbox)
	++sandbox;
    کانو = call_callback_retnr(cb, 2, دليلون);
    إذا (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    إذا (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_متن));
	أرجع FAIL;
    }

    إذا (startcol != عدم)
	*startcol = کانو;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    إذا (کانو == -2 || aborting())
	أرجع FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    إذا (کانو == -3)
    {
	إذا (is_cpt_function)
	    أرجع FAIL;
	ctrl_x_گهڻائي = CTRL_X_عام;
	edit_submode = عدم;
	إذا (!shortmess(SHM_COMPLETIONMENU))
	    نياپو_clr_cmdline();
	أرجع FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_خالي_جايون = FALSE;

    ret = !is_cpt_function ? سيٽ_compl_globals(کانو, curs_کانو, FALSE) : OK;
#جيڪڏهن_پورو

    أرجع ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    ساكن صحيح
get_spell_compl_info(صحيح startcol اڻ_استعمال_ٿيل, colnr_T curs_کانو اڻ_استعمال_ٿيل)
{
    صحيح		ret = FAIL;
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    char_u	*سٽ = عدم;

    إذا (spell_bad_len > 0)
	compl_کانو = curs_کانو - spell_bad_len;
    وإلا
	compl_کانو = spell_لفظ_شروعات(startcol);

    إذا (compl_کانو >= (colnr_T)startcol)
    {
	compl_ڊيگهائي = 0;
	compl_کانو = curs_کانو;
    }
    وإلا
    {
	spell_expand_check_cap(compl_کانو);
	compl_ڊيگهائي = (صحيح)curs_کانو - compl_کانو;
    }
    // Need to obtain "line" again, it may have become invalid.
    سٽ = ml_get(curwin->w_cursor.lnum);
    compl_نمونو.string = vim_strnsave(سٽ + compl_کانو, (size_t)compl_ڊيگهائي);
    إذا (compl_نمونو.string == عدم)
    {
	compl_نمونو.ڊيگهائي = 0;
	أرجع FAIL;
    }

    compl_نمونو.ڊيگهائي = (size_t)compl_ڊيگهائي;
    ret = OK;
#جيڪڏهن_پورو

    أرجع ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    ساكن صحيح
compl_get_info(char_u *سٽ, صحيح startcol, colnr_T curs_کانو, صحيح *سٽ_invalid)
{
    إذا (ctrl_x_گهڻائي_عام() || ctrl_x_گهڻائي_رجسٽر()
	    || (ctrl_x_گهڻائي & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_گهڻائي)))
    {
	إذا (get_عام_compl_info(سٽ, startcol, curs_کانو) != OK)
	    أرجع FAIL;
	*سٽ_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    وإلا إذا (ctrl_x_گهڻائي_سٽ_or_eval())
    {
	أرجع get_wholeline_compl_info(سٽ, curs_کانو);
    }
    وإلا إذا (ctrl_x_گهڻائي_files())
    {
	أرجع get_filename_compl_info(سٽ, startcol, curs_کانو);
    }
    وإلا إذا (ctrl_x_گهڻائي == CTRL_X_CMDLINE)
    {
	أرجع get_cmdline_compl_info(سٽ, curs_کانو);
    }
    وإلا إذا (ctrl_x_گهڻائي_function() || ctrl_x_گهڻائي_omni()
	    || thesaurus_func_complete(ctrl_x_گهڻائي))
    {
	إذا (get_userdefined_compl_info(curs_کانو, عدم, عدم) != OK)
	    أرجع FAIL;
	*سٽ_invalid = TRUE;	// "line" may have become invalid
    }
    وإلا إذا (ctrl_x_گهڻائي_spell())
    {
	إذا (get_spell_compl_info(startcol, curs_کانو) == FAIL)
	    أرجع FAIL;
	*سٽ_invalid = TRUE;	// "line" may have become invalid
    }
    وإلا
    {
	اندروني_error("ins_complete()");
	أرجع FAIL;
    }

    أرجع OK;
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
    ساكن فراغ
ins_compl_continue_ڳولا(char_u *سٽ)
{
    // it is a continued search
    compl_cont_حالت &= ~CONT_INTRPT;	// remove INTRPT
    إذا (ctrl_x_گهڻائي_عام() || ctrl_x_گهڻائي_رستو_patterns()
						|| ctrl_x_گهڻائي_رستو_مقرر_ڪري())
    {
	إذا (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_کانو = (colnr_T)getwhitecols(سٽ);
	    compl_startpos.کانو = compl_کانو;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_حالت &= ~CONT_SOL;   // clear SOL if present
	}
	وإلا
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    إذا (compl_cont_حالت & CONT_S_IPOS)
	    {
		compl_cont_حالت |= CONT_SOL;
		compl_startpos.کانو = (colnr_T)(skipwhite(
			    سٽ + compl_ڊيگهائي
			    + compl_startpos.کانو) - سٽ);
	    }
	    compl_کانو = compl_startpos.کانو;
	}
	compl_ڊيگهائي = curwin->w_cursor.کانو - (صحيح)compl_کانو;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#وصف_ڪريو	MIN_SPACE 75
	إذا (compl_ڊيگهائي > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_حالت &= ~CONT_SOL;
	    compl_ڊيگهائي = (IOSIZE - MIN_SPACE);
	    compl_کانو = curwin->w_cursor.کانو - compl_ڊيگهائي;
	}
	compl_cont_حالت |= CONT_ADDING | CONT_N_ADDS;
	إذا (compl_ڊيگهائي < 1)
	    compl_cont_حالت &= CONT_LOCAL;
    }
    وإلا إذا (ctrl_x_گهڻائي_سٽ_or_eval() || ctrl_x_گهڻائي_رجسٽر())
	compl_cont_حالت = CONT_ADDING | CONT_N_ADDS;
    وإلا
	compl_cont_حالت = 0;
}

/*
 * start insert mode completion
 */
    ساكن صحيح
ins_compl_شروعات(فراغ)
{
    char_u	*سٽ = عدم;
    صحيح		startcol = 0;	    // column where searched text starts
    colnr_T	curs_کانو;	    // cursor column
    صحيح		سٽ_invalid = FALSE;
    صحيح		save_did_ai = did_ai;
    صحيح		flags = CP_ORIGINAL_متن;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٺ = FALSE;
    إذا (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	أرجع FAIL;
    }

    سٽ = ml_get(curwin->w_cursor.lnum);
    curs_کانو = curwin->w_cursor.کانو;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    إذا ((compl_cont_حالت & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_گهڻائي == ctrl_x_گهڻائي)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_ڳولا(سٽ);
    وإلا
	compl_cont_حالت &= CONT_LOCAL;

    إذا (!compl_حالت_adding())	// normal expansion
    {
	compl_cont_گهڻائي = ctrl_x_گهڻائي;
	إذا (ctrl_x_گهڻائي_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_حالت = 0;
	compl_cont_حالت |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (صحيح)curs_کانو;
	compl_کانو = 0;
    }

    // Work out completion pattern and original text -- webb
    إذا (compl_get_info(سٽ, startcol, curs_کانو, &سٽ_invalid) == FAIL)
    {
	إذا (ctrl_x_گهڻائي_function() || ctrl_x_گهڻائي_omni()
				|| thesaurus_func_complete(ctrl_x_گهڻائي))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	أرجع FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    إذا (سٽ_invalid)
	سٽ = ml_get(curwin->w_cursor.lnum);

    إذا (compl_حالت_adding())
    {
	إذا (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	إذا (ctrl_x_گهڻائي_سٽ_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.کانو = compl_کانو;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_ڊيگهائي = 0;
	    compl_کانو = curwin->w_cursor.کانو;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    وإلا
    {
	edit_submode_pre = عدم;
	compl_startpos.کانو = compl_کانو;
    }

    إذا (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	إذا (compl_cont_حالت & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_نياپو]);
	وإلا
	    edit_submode = (char_u *)_(CTRL_X_نياپو(ctrl_x_گهڻائي));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(عدم);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_متن);
    compl_orig_متن.ڊيگهائي = (size_t)compl_ڊيگهائي;
    compl_orig_متن.string = vim_strnsave(سٽ + compl_کانو, (size_t)compl_ڊيگهائي);
    إذا (p_ic)
	flags |= CP_ICASE;
    إذا (compl_orig_متن.string == عدم
	    || ins_compl_جوڙيو(compl_orig_متن.string,
		(صحيح)compl_orig_متن.ڊيگهائي,
		عدم, عدم, عدم, 0, flags, FALSE, عدم, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_نمونو);
	VIM_CLEAR_STRING(compl_orig_متن);
	did_ai = save_did_ai;
	أرجع FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    إذا (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_ڳڻپ;
	showmode();
	edit_submode_extra = عدم;
	ٻاهريون_flush();
    }

    did_ai = save_did_ai;
    أرجع OK;
}

/*
 * display the completion status message
 */
    ساكن فراغ
ins_compl_show_statusmsg(فراغ)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    إذا (is_پهريون_ميلاپ(compl_پهريون_ميلاپ->cp_ايندڙ))
    {
	edit_submode_extra = compl_حالت_adding() && compl_ڊيگهائي > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    إذا (edit_submode_extra == عدم)
    {
	إذا (ميلاپ_at_original_متن(compl_curr_ميلاپ))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	وإلا إذا (compl_cont_حالت & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_ڳڻپ;
	}
	وإلا إذا (compl_curr_ميلاپ->cp_ايندڙ == compl_curr_ميلاپ->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_ڳڻپ;
	    compl_curr_ميلاپ->cp_عدد = 1;
	}
	وإلا
	{
#إذا defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    إذا (compl_curr_ميلاپ->cp_عدد == -1)
		ins_compl_update_sequence_numbers();
#جيڪڏهن_پورو
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    إذا (compl_curr_ميلاپ->cp_عدد != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		ساكن char_u ميلاپ_ref[81];

		إذا (compl_matches > 0)
		    vim_snprintf((حرف *)ميلاپ_ref, حجم(ميلاپ_ref),
			    _("match %d of %d"),
			    compl_curr_ميلاپ->cp_عدد, compl_matches);
		وإلا
		    vim_snprintf((حرف *)ميلاپ_ref, حجم(ميلاپ_ref),
			    _("match %d"),
			    compl_curr_ميلاپ->cp_عدد);
		edit_submode_extra = ميلاپ_ref;
		edit_submode_highl = HLF_R;
		إذا (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    إذا (!compl_opt_suppress_خالي_جايون)
    {
	showmode();
	إذا (!shortmess(SHM_COMPLETIONMENU))
	{
	    إذا (edit_submode_extra != عدم)
	    {
		إذا (!p_smd)
		{
		    نياپو_hist_off = TRUE;
		    نياپو_attr((حرف *)edit_submode_extra,
			    edit_submode_highl < HLF_ڳڻپ
			    ? HL_ATTR(edit_submode_highl) : 0);
		    نياپو_hist_off = FALSE;
		}
	    }
	    وإلا
		نياپو_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    صحيح
ins_complete(صحيح c, صحيح enable_pum)
{
    صحيح		n;
    صحيح		save_w_wrow;
    صحيح		save_w_leftcol;
    صحيح		وجهو_ميلاپ;
    صحيح		no_matches_مليو;

    compl_direction = ins_compl_key2dir(c);
    وجهو_ميلاپ = ins_compl_use_ميلاپ(c);

    إذا (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	إذا (ins_compl_شروعات() == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (وجهو_ميلاپ && stop_arrow() == FAIL)
	أرجع FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_بفر;
    compl_shown_ميلاپ = compl_curr_ميلاپ;
    compl_shows_dir = compl_direction;
    compl_انگ_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_ايندڙ(TRUE, ins_compl_key2count(c), وجهو_ميلاپ);

    // Reset autocompletion timer expiry flag
    إذا (compl_autocomplete)
	compl_وقت_ٽڪرو_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    إذا (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_ميلاپ = compl_shown_ميلاپ;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    إذا (got_int && !global_busy)
    {
	(فراغ)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_مليو = is_پهريون_ميلاپ(compl_پهريون_ميلاپ->cp_ايندڙ);
    إذا (no_matches_مليو)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	إذا (compl_ڊيگهائي > 1
		|| compl_حالت_adding()
		|| (ctrl_x_گهڻائي_not_default()
		    && !ctrl_x_گهڻائي_رستو_patterns()
		    && !ctrl_x_گهڻائي_رستو_مقرر_ڪري()))
	    compl_cont_حالت &= ~CONT_N_ADDS;
    }

    إذا (compl_curr_ميلاپ->cp_flags & CP_CONT_S_IPOS)
	compl_cont_حالت |= CONT_S_IPOS;
    وإلا
	compl_cont_حالت &= ~CONT_S_IPOS;

    إذا (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    إذا (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    أرجع OK;
}

/*
 * Enable autocompletion
 */
    فراغ
ins_compl_enable_autocomplete(فراغ)
{
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#جيڪڏهن_پورو
}

/*
 * Disable autocompletion
 */
    فراغ
ins_compl_disable_autocomplete(فراغ)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    فراغ
ins_compl_arm_autostart(فراغ)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    فراغ
ins_compl_disarm_autostart(فراغ)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(فراغ)
{
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    إذا (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_شروعات_tv);
	compl_autocomplete_pending = true;
	أرجع true;
    }
#جيڪڏهن_پورو
    أرجع false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    فراغ
ins_compl_clear_autocomplete_delay(فراغ)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(فراغ)
{
    أرجع compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    ڊگهو
ins_compl_autocomplete_elapsed(فراغ)
{
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    أرجع ELAPSED_FUNC(compl_autocomplete_شروعات_tv);
#وإلا
    أرجع 0;
#جيڪڏهن_پورو
}

/*
 * Remove (if needed) and show the popup menu
 */
    ساكن فراغ
show_pum(صحيح prev_w_wrow, صحيح prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    إذا (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    ساكن بنانشان
quote_meta(char_u *dest, char_u *src, صحيح len)
{
    بنانشان	m = (بنانشان)len + 1;  // one extra for the NUL

    لكل ( ; --len >= 0; src++)
    {
	بدل (*src)
	{
	    حالة '.':
	    حالة '*':
	    حالة '[':
		إذا (ctrl_x_گهڻائي_dictionary() || ctrl_x_گهڻائي_thesaurus())
		    اكسر;
		// FALLTHROUGH
	    حالة '~':
		إذا (!magic_isset())	// quote these only if magic is set
		    اكسر;
		// FALLTHROUGH
	    حالة '\\':
		إذا (ctrl_x_گهڻائي_dictionary() || ctrl_x_گهڻائي_thesaurus())
		    اكسر;
		// FALLTHROUGH
	    حالة '^':		// currently it's not needed.
	    حالة '$':
		m++;
		إذا (dest != عدم)
		    *dest++ = '\\';
		اكسر;
	}
	إذا (dest != عدم)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	إذا (has_mbyte)
	{
	    صحيح mb_len = (*mb_ptr2len)(src) - 1;
	    إذا (mb_len > 0 && len >= mb_len)
		لكل (صحيح i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    إذا (dest != عدم)
			*dest++ = *src;
		}
	}
    }
    إذا (dest != عدم)
	*dest = NUL;

    أرجع m;
}

#إذا defined(EXITFREE)
    فراغ
free_insexpand_stuff(فراغ)
{
    VIM_CLEAR_STRING(compl_orig_متن);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_ڳڻپ);
# endif
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    ساكن فراغ
spell_پٺ_منزل_badword(فراغ)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_منزل(curwin, BACKWARD, SMT_ALL, TRUE, عدم);
    إذا (curwin->w_cursor.کانو != tpos.کانو)
	شروعات_arrow(&tpos);
}
#جيڪڏهن_پورو

/*
 * Reset the info associated with completion sources.
 */
    ساكن فراغ
cpt_sources_clear(فراغ)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_اشاريو = -1;
    cpt_sources_ڳڻپ = 0;
}

/*
 * Setup completion sources.
 */
    ساكن صحيح
تياري_cpt_sources(فراغ)
{
    char_u  buf[LSIZE];
    صحيح	    slen;
    صحيح	    idx = 0;
    صحيح	    ڳڻپ;
    char_u  *p;

    cpt_sources_clear();

    ڳڻپ = get_cpt_sources_ڳڻپ(curbuf->b_p_cpt);
    إذا (ڳڻپ == 0)
	أرجع OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_ذريعو_T, ڳڻپ);
    إذا (cpt_sources_array == عدم)
	أرجع FAIL;

    لكل (p = curbuf->b_p_cpt; *p;)
    {
	طالما (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	إذا (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    إذا (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		إذا (caret != عدم)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((حرف *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_ڳڻپ = ڳڻپ;
    أرجع OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    ساكن صحيح
is_cpt_func_refresh_always(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    لكل (صحيح i = 0; i < cpt_sources_ڳڻپ; i++)
	إذا (cpt_sources_array[i].cs_refresh_always)
	    أرجع TRUE;
#جيڪڏهن_پورو
    أرجع FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    ساكن فراغ
ins_compl_make_لڪيري(فراغ)
{
    compl_T *m;

    إذا (compl_پهريون_ميلاپ == عدم || compl_پهريون_ميلاپ->cp_prev == عدم)
	أرجع;
    m = compl_پهريون_ميلاپ->cp_prev;
    m->cp_ايندڙ = عدم;
    compl_پهريون_ميلاپ->cp_prev = عدم;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    ساكن فراغ
remove_old_matches(فراغ)
{
    compl_T *current;
    صحيح	    shown_ميلاپ_removed = FALSE;
    صحيح	    forward = (compl_پهريون_ميلاپ->cp_cpt_ذريعو_idx < 0);

    إذا (cpt_sources_اشاريو < 0)
	أرجع;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    لكل (current = compl_پهريون_ميلاپ; current != عدم; )
    {
	إذا (current->cp_cpt_ذريعو_idx == cpt_sources_اشاريو)
	{
	    compl_T *منزل_delete = current;

	    إذا (!shown_ميلاپ_removed && compl_shown_ميلاپ == current)
		shown_ميلاپ_removed = TRUE;

	    current = current->cp_ايندڙ;

	    إذا (منزل_delete == compl_پهريون_ميلاپ)  // node to remove is at head
	    {
		compl_پهريون_ميلاپ = منزل_delete->cp_ايندڙ;
		compl_پهريون_ميلاپ->cp_prev = عدم;
	    }
	    وإلا إذا (منزل_delete->cp_ايندڙ == عدم) // node to remove is at tail
		منزل_delete->cp_prev->cp_ايندڙ = عدم;
	    وإلا // node is in the moddle
	    {
		منزل_delete->cp_prev->cp_ايندڙ = منزل_delete->cp_ايندڙ;
		منزل_delete->cp_ايندڙ->cp_prev = منزل_delete->cp_prev;
	    }
	    ins_compl_شي_free(منزل_delete);
	}
	وإلا
	    current = current->cp_ايندڙ;
    }

    // Re-assign compl_shown_match if necessary
    إذا (shown_ميلاپ_removed)
    {
	إذا (forward)
	    compl_shown_ميلاپ = compl_پهريون_ميلاپ;
	وإلا
	{  // Last node will have the prefix that is being completed
	    لكل (current = compl_پهريون_ميلاپ; current->cp_ايندڙ != عدم;
		    current = current->cp_ايندڙ)
		;
	    compl_shown_ميلاپ = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_ميلاپ = compl_پهريون_ميلاپ;
    لكل (current = compl_پهريون_ميلاپ; current != عدم; )
    {
	إذا ((forward ? current->cp_cpt_ذريعو_idx < cpt_sources_اشاريو
		    : current->cp_cpt_ذريعو_idx > cpt_sources_اشاريو))
	{
	    compl_curr_ميلاپ = forward ? current : current->cp_ايندڙ;
	    current = current->cp_ايندڙ;
	}
	وإلا
	    اكسر;
    }
}
#جيڪڏهن_پورو

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    ساكن فراغ
get_cpt_func_completion_matches(callback_T *cb اڻ_استعمال_ٿيل)
{
    cpt_ذريعو_T    *cpt_src = &cpt_sources_array[cpt_sources_اشاريو];
    صحيح		    startcol = cpt_src->cs_startcol;

    إذا (startcol == -2 || startcol == -3)
	أرجع;

    إذا (سيٽ_compl_globals(startcol, curwin->w_cursor.کانو, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	إذا (!cpt_src->cs_refresh_always)
	    ins_compl_وجهو_بائيٽون(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_نمونو.string, cb);

	إذا (!cpt_src->cs_refresh_always)
	    ins_compl_delete();

	cpt_src->cs_refresh_always = compl_opt_refresh_always;
	compl_opt_refresh_always = FALSE;
    }
}
#جيڪڏهن_پورو

/*
 * Retrieve completion matches from functions in the 'cpt' option where the
 * 'refresh:always' flag is set.
 */
    ساكن فراغ
cpt_compl_refresh(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = عدم;
    صحيح		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_لڪيري();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_اندريون_رکو(cpt);

    cpt_sources_اشاريو = 0;
    لكل (p = cpt; *p;)
    {
	طالما (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	إذا (*p == NUL)
	    اكسر;

	إذا (cpt_sources_array[cpt_sources_اشاريو].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_اشاريو);
	    إذا (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.کانو, cb,
			&startcol);
		إذا (ret == FAIL)
		{
		    إذا (startcol == -3)
			cpt_sources_array[cpt_sources_اشاريو].cs_refresh_always
			    = FALSE;
		    وإلا
			startcol = -2;
		}
		وإلا إذا (startcol < 0 || startcol > curwin->w_cursor.کانو)
		    startcol = curwin->w_cursor.کانو;
		cpt_sources_array[cpt_sources_اشاريو].cs_startcol = startcol;
		إذا (ret == OK)
		{
		    compl_ذريعو_شروعات_timer(cpt_sources_اشاريو);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(فراغ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	إذا (may_advance_cpt_اشاريو(p))
	    (فراغ)advance_cpt_sources_اشاريو_محفوظ();
    }
    cpt_sources_اشاريو = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#جيڪڏهن_پورو
}

/*
 * "preinserted()" function
 */
    فراغ
f_preinserted(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (ins_compl_preinsert_effect())
	rettv->vval.v_عدد = 1;
}
