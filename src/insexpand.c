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

#شامل "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#تعریف_کرو CTRL_X_WANT_IDENT	0x100

#تعریف_کرو CTRL_X_معمول		0  // CTRL-N CTRL-P completion, default
#تعریف_کرو CTRL_X_NOT_DEFINED_YET	1
#تعریف_کرو CTRL_X_SCROLL		2
#تعریف_کرو CTRL_X_پورا_سطر	3
#تعریف_کرو CTRL_X_FILES		4
#تعریف_کرو CTRL_X_لیبل_فہرست		(5 + CTRL_X_WANT_IDENT)
#تعریف_کرو CTRL_X_راستہ_PATTERNS	(6 + CTRL_X_WANT_IDENT)
#تعریف_کرو CTRL_X_راستہ_متعین_کرتا	(7 + CTRL_X_WANT_IDENT)
#تعریف_کرو CTRL_X_ختم_شدہ		8
#تعریف_کرو CTRL_X_DICTIONARY	(9 + CTRL_X_WANT_IDENT)
#تعریف_کرو CTRL_X_THESAURUS	(10 + CTRL_X_WANT_IDENT)
#تعریف_کرو CTRL_X_CMDLINE		11
#تعریف_کرو CTRL_X_FUNCTION	12
#تعریف_کرو CTRL_X_OMNI		13
#تعریف_کرو CTRL_X_SPELL		14
#تعریف_کرو CTRL_X_LOCAL_پیغام	15	// only used in "ctrl_x_msgs"
#تعریف_کرو CTRL_X_EVAL		16	// for builtin function complete()
#تعریف_کرو CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#تعریف_کرو CTRL_X_رجسٹر	18	// complete words from registers

#تعریف_کرو CTRL_X_پیغام(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
ساکن حرف *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    تہی, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    تہی, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    تہی,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#اگر defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ساکن string_T ctrl_x_منوال_نام[] = {
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
    {تہی, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#اگر_ختم

/*
 * Structure used to store one match for insert completion.
 */
قسمتعریف ساخت compl_S compl_T;
ساخت compl_S
{
    compl_T	*cp_اگلا;
    compl_T	*cp_پچھلا;
    compl_T	*cp_مطابقت_اگلا;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_متن[CPT_گنتی]);	// text for the menu
#اگر_تعریف_شدہ FEAT_EVAL
    typval_T	cp_user_ڈیٹا;
#اگر_ختم
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    صحیح		cp_flags;		// CP_ values
    صحیح		cp_نمبر;		// sequence number
    صحیح		cp_ترتیب;		// list position when first scored
    صحیح		cp_score;		// fuzzy match score or proximity score
    صحیح		cp_اندر_مطابقت_array;	// collected by compl_match_array
    صحیح		cp_user_abbr_hl_id;	// highlight group ID for abbr
    صحیح		cp_user_قسم_hl_id;	// highlight group ID for kind
    صحیح		cp_cpt_ماخذ_idx;	// index of this match's source in 'cpt' option
};

// values for cp_flags
#تعریف_کرو CP_ORIGINAL_متن   1	// the original text when the expansion begun
#تعریف_کرو CP_FREE_FNAME	    2	// cp_fname is allocated
#تعریف_کرو CP_CONT_S_IPOS	    4	// use CONT_S_IPOS for compl_cont_status
#تعریف_کرو CP_EQUAL	    8	// ins_compl_equal() always returns TRUE
#تعریف_کرو CP_ICASE	    16	// ins_compl_equal() ignores case
#تعریف_کرو CP_FAST	    32	// use fast_breakcheck instead of ui_breakcheck

/*
 * All the current matches are stored in a list.
 * "compl_first_match" points to the start of the list.
 * "compl_curr_match" points to the currently selected entry.
 * "compl_shown_match" is different from compl_curr_match during
 * ins_compl_get_exp(), when new matches are added to the list.
 * "compl_old_match" points to previous "compl_curr_match".
 */
ساکن compl_T    *compl_پہلا_مطابقت = تہی;
ساکن compl_T    *compl_curr_مطابقت = تہی;
ساکن compl_T    *compl_shown_مطابقت = تہی;
ساکن compl_T    *compl_old_مطابقت = تہی;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
قسمتعریف ساخت
{
    صحیح	    cse_گنتی;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#تعریف_کرو CSE_OFF		((صحیح)offsetof(complstr_T, cse_str))
#تعریف_کرو HI2CSE(hi)	((complstr_T *)((hi)->hi_کلید - CSE_OFF))

ساکن hashtab_T  compl_لڑیاں_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    ساکن خالی
compl_لڑیاں_جوڑو(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*اندراج_2;

    اگر (compl_لڑیاں_ht.ht_array == تہی)
	hash_init(&compl_لڑیاں_ht);
    اگر (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_لڑیاں_ht, str, hash);
    اگر (HASHITEM_خالی_خانے(hi))
    {
	اندراج_2 = alloc(CSE_OFF + len + 1);
	اگر (اندراج_2 == تہی)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    واپس;
	اندراج_2->cse_گنتی = 1;
	vim_strncpy(اندراج_2->cse_str, str, len);
	اگر (hash_جوڑو_شے(&compl_لڑیاں_ht, hi, اندراج_2->cse_str, hash)
								      == FAIL)
	    vim_free(اندراج_2);
    }
    ورنہ
	++HI2CSE(hi)->cse_گنتی;
}

// list used to store the compl_T which have the max score
ساکن compl_T	  **compl_بہترین_matches = تہی;
ساکن صحیح	  compl_عدد_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
ساکن صحیح	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
ساکن string_T	  compl_leader = {تہی, 0};

ساکن صحیح	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
ساکن صحیح	  compl_مستعمل_مطابقت;

// didn't finish finding completions.
ساکن صحیح	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
ساکن صحیح	  compl_interrupted = FALSE;

ساکن صحیح	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
ساکن صحیح	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
ساکن صحیح	  ctrl_x_منوال = CTRL_X_معمول;

ساکن صحیح	  compl_matches = 0;	    // number of completion matches
ساکن string_T	  compl_نمونہ = {تہی, 0};	 // search pattern for matching items
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
ساکن string_T	  cpt_compl_نمونہ = {تہی, 0}; // pattern returned by func in 'cpt'
#اگر_ختم
ساکن صحیح	  compl_direction = FORWARD;
ساکن صحیح	  compl_shows_dir = FORWARD;
ساکن صحیح	  compl_pending = 0;	    // > 1 for postponed CTRL-N
ساکن pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
ساکن صحیح	  compl_لمبائی = 0;
ساکن linenr_T	  compl_lnum = 0;           // lnum where the completion start
ساکن colnr_T	  compl_ستون = 0;	    // column where the text starts
					    // that is being completed
ساکن colnr_T	  compl_ins_انتہا_ستون = 0;
ساکن colnr_T	  compl_longest_انتہا_ستون = 0;  // end of 'longest' inserted text
ساکن string_T	  compl_orig_متن = {تہی, 0};  // text as it was before
					    // completion started
ساکن صحیح	  compl_cont_منوال = 0;
ساکن expand_T	  compl_xp;

ساکن win_T	  *compl_curr_win = تہی;  // win where completion is active
ساکن buf_T	  *compl_curr_buf = تہی;  // buf where completion is active

#تعریف_کرو COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
ساکن صحیح	  compl_autocomplete = FALSE;	    // whether autocompletion is active
ساکن bool	  compl_autostarted = false;	    // Vim started this completion
ساکن bool	  compl_autostart_pending = false;  // the trigger armed one
ساکن bool	  compl_autocomplete_pending = false;
#اگر_تعریف_شدہ ELAPSED_FUNC
ساکن elapsed_T  compl_autocomplete_آغاز_tv;	    // when the delay was armed
#اگر_ختم
ساکن صحیح	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
ساکن صحیح	  compl_وقت_قاش_expired = FALSE; // time budget exceeded for current source
ساکن صحیح	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
ساکن صحیح	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#تعریف_کرو COMPL_MIN_TIMEOUT_MS	5
#تعریف_کرو DECAY_COMPL_TIMEOUT() \
    کرو { \
	اگر (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } جب (0)

// Timeout values for F{func}, F and o values in 'complete'
#تعریف_کرو COMPL_FUNC_TIMEOUT_MS		300
#تعریف_کرو COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
ساکن صحیح	  compl_cont_حالت = 0;
#تعریف_کرو CONT_ADDING	1	// "normal" or "adding" expansion
#تعریف_کرو CONT_INTRPT	(2 + 4)	// a ^X interrupted the current expansion
				// it's set only iff N_ADDS is set
#تعریف_کرو CONT_N_ADDS	4	// next ^X<> will add-new or expand-current
#تعریف_کرو CONT_S_IPOS	8	// next ^X<> will set initial_pos?
				// if so, word-wise-expansion will set SOL
#تعریف_کرو CONT_SOL	16	// pattern includes start of line, just for
				// word-wise expansion, not set for ^X^L
#تعریف_کرو CONT_LOCAL	32	// for ctrl_x_mode 0, ^X^P/^X^N do a local
				// expansion, (eg use complete=.)

ساکن صحیح	  compl_opt_refresh_always = FALSE;
ساکن صحیح	  compl_opt_suppress_خالی_خانے = FALSE;

ساکن صحیح	  compl_selected_شے = -1;

ساکن صحیح	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
قسمتعریف ساخت cpt_ماخذ_T
{
    صحیح	cs_refresh_always;  // Whether 'refresh:always' is set for func
    صحیح	cs_startcol;	    // Start column returned by func
    صحیح	cs_max_matches;	    // Max items to display from this source
#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	compl_آغاز_tv;	    // Timestamp when match collection starts
#اگر_ختم
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_ماخذ_T;

ساکن cpt_ماخذ_T *cpt_sources_array; // Pointer to the array of completion sources
ساکن صحیح	    cpt_sources_گنتی;  // Total number of completion sources specified in the 'cpt' option
ساکن صحیح	    cpt_sources_اشاریہ = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ساکن pumitem_T *compl_مطابقت_array = تہی;
ساکن صحیح compl_مطابقت_arraysize;

ساکن صحیح ins_compl_جوڑو(char_u *str, صحیح len, char_u *fname, char_u **cptext, typval_T *user_ڈیٹا, صحیح cdir, صحیح flags, صحیح adup, صحیح *user_hl, صحیح score);
ساکن خالی ins_compl_longest_مطابقت(compl_T *مطابقت);
ساکن خالی ins_compl_del_pum(خالی);
ساکن خالی ins_compl_files(صحیح گنتی, char_u **files, صحیح thesaurus, صحیح flags, regmatch_T *regmatch, char_u *buf, صحیح *dir);
ساکن خالی ins_compl_free(خالی);
ساکن صحیح  ins_compl_need_restart(خالی);
ساکن خالی ins_compl_new_leader(خالی);
ساکن صحیح  get_compl_len(خالی);
ساکن خالی ins_compl_restart(خالی);
ساکن خالی ins_compl_سیٹ_original_متن(char_u *str, size_t len);
ساکن خالی ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#اگر defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ساکن خالی ins_compl_جوڑو_list(list_T *list);
ساکن خالی ins_compl_جوڑو_dict(dict_T *dict);
ساکن صحیح get_userdefined_compl_info(colnr_T curs_ستون, callback_T *cb, صحیح *startcol);
ساکن خالی get_cpt_func_completion_matches(callback_T *cb);
ساکن callback_T *get_callback_if_cpt_func(char_u *p, صحیح idx);
ساکن خالی fill_complete_info_dict(dict_T *di, compl_T *مطابقت, صحیح جوڑو_مطابقت);
#اگر_ختم
ساکن صحیح تیاری_cpt_sources(خالی);
ساکن صحیح is_cpt_func_refresh_always(خالی);
ساکن خالی cpt_sources_clear(خالی);
ساکن خالی cpt_compl_refresh(خالی);
ساکن صحیح  ins_compl_key2dir(صحیح c);
ساکن صحیح  ins_compl_pum_کلید(صحیح c);
ساکن صحیح  ins_compl_key2count(صحیح c);
ساکن خالی show_pum(صحیح پچھلا_w_wrow, صحیح پچھلا_w_leftcol);
ساکن غیرعلامتی  quote_meta(char_u *dest, char_u *str, صحیح len);
ساکن صحیح ins_compl_has_مضرب(خالی);
ساکن خالی ins_compl_expand_مضرب(char_u *str);
ساکن خالی ins_compl_longest_داخل_کرو(char_u *سابقہ);
ساکن خالی ins_compl_make_خطی(خالی);
ساکن صحیح ins_compl_make_cyclic(خالی);

#اگر_تعریف_شدہ FEAT_SPELL
ساکن خالی spell_عقب_مقصد_badword(خالی);
ساکن صحیح  spell_bad_len = 0;	// length of located bad word
#اگر_ختم

/*
 * CTRL-X pressed in Insert mode.
 */
    خالی
ins_ctrl_x(خالی)
{
    اگر (!ctrl_x_منوال_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	اگر (compl_cont_حالت & CONT_N_ADDS)
	    compl_cont_حالت |= CONT_INTRPT;
	ورنہ
	    compl_cont_حالت = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_منوال = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_پیغام(ctrl_x_منوال));
	edit_submode_pre = تہی;
	showmode();
    }
    ورنہ
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_منوال = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
صحیح ctrl_x_منوال_none(خالی)
    { واپس ctrl_x_منوال == 0; }
صحیح ctrl_x_منوال_معمول(خالی)
    { واپس ctrl_x_منوال == CTRL_X_معمول; }
صحیح ctrl_x_منوال_scroll(خالی)
    { واپس ctrl_x_منوال == CTRL_X_SCROLL; }
صحیح ctrl_x_منوال_پورا_سطر(خالی)
    { واپس ctrl_x_منوال == CTRL_X_پورا_سطر; }
صحیح ctrl_x_منوال_files(خالی)
    { واپس ctrl_x_منوال == CTRL_X_FILES; }
صحیح ctrl_x_منوال_لیبل_فہرست(خالی)
    { واپس ctrl_x_منوال == CTRL_X_لیبل_فہرست; }
صحیح ctrl_x_منوال_راستہ_patterns(خالی)
    { واپس ctrl_x_منوال == CTRL_X_راستہ_PATTERNS; }
صحیح ctrl_x_منوال_راستہ_متعین_کرتا(خالی)
    { واپس ctrl_x_منوال == CTRL_X_راستہ_متعین_کرتا; }
صحیح ctrl_x_منوال_dictionary(خالی)
    { واپس ctrl_x_منوال == CTRL_X_DICTIONARY; }
صحیح ctrl_x_منوال_thesaurus(خالی)
    { واپس ctrl_x_منوال == CTRL_X_THESAURUS; }
صحیح ctrl_x_منوال_cmdline(خالی)
    { واپس ctrl_x_منوال == CTRL_X_CMDLINE
		|| ctrl_x_منوال == CTRL_X_CMDLINE_CTRL_X; }
صحیح ctrl_x_منوال_function(خالی)
    { واپس ctrl_x_منوال == CTRL_X_FUNCTION; }
صحیح ctrl_x_منوال_omni(خالی)
    { واپس ctrl_x_منوال == CTRL_X_OMNI; }
صحیح ctrl_x_منوال_spell(خالی)
    { واپس ctrl_x_منوال == CTRL_X_SPELL; }
صحیح ctrl_x_منوال_eval(خالی)
    { واپس ctrl_x_منوال == CTRL_X_EVAL; }
صحیح ctrl_x_منوال_سطر_or_eval(خالی)
    { واپس ctrl_x_منوال == CTRL_X_پورا_سطر || ctrl_x_منوال == CTRL_X_EVAL; }
صحیح ctrl_x_منوال_رجسٹر(خالی)
    { واپس ctrl_x_منوال == CTRL_X_رجسٹر; }

/*
 * Whether other than default completion has been selected.
 */
    صحیح
ctrl_x_منوال_not_default(خالی)
{
    واپس ctrl_x_منوال != CTRL_X_معمول;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    صحیح
ctrl_x_منوال_not_defined_yet(خالی)
{
    واپس ctrl_x_منوال == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    صحیح
compl_حالت_adding(خالی)
{
    واپس compl_cont_حالت & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    صحیح
compl_حالت_sol(خالی)
{
    واپس compl_cont_حالت & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    صحیح
compl_حالت_local(خالی)
{
    واپس compl_cont_حالت & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    خالی
compl_حالت_clear(خالی)
{
    compl_cont_حالت = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    ساکن صحیح
compl_dir_forward(خالی)
{
    واپس compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    ساکن صحیح
compl_shows_dir_forward(خالی)
{
    واپس compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    ساکن صحیح
compl_shows_dir_backward(خالی)
{
    واپس compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    صحیح
has_compl_option(صحیح dict_opt)
{
    اگر (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
#اگر_تعریف_شدہ FEAT_SPELL
							&& !curwin->w_p_spell
#اگر_ختم
							)
		 : (*curbuf->b_p_tsr == NUL && *p_tsr == NUL
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
		     && *curbuf->b_p_tsrfu == NUL && *p_tsrfu == NUL
#اگر_ختم
		   ))
    {
	ctrl_x_منوال = CTRL_X_معمول;
	edit_submode = تہی;
	پیغام_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	اگر (emsg_silent == 0 && !اندر_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    برآمد_flush();
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (!get_vim_var_nr(VV_TESTING))
#اگر_ختم
		ui_delay(2004L, FALSE);
	}
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    صحیح
vim_is_ctrl_x_کلید(صحیح c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    اگر (ins_compl_pum_کلید(c))
	واپس TRUE;

    انتخاب (ctrl_x_منوال)
    {
	حالت 0:		    // Not in any CTRL-X mode
	    واپس (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	حالت CTRL_X_NOT_DEFINED_YET:
	حالت CTRL_X_CMDLINE_CTRL_X:
	    واپس (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	حالت CTRL_X_SCROLL:
	    واپس (c == Ctrl_Y || c == Ctrl_E);
	حالت CTRL_X_پورا_سطر:
	    واپس (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_FILES:
	    واپس (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_DICTIONARY:
	    واپس (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_THESAURUS:
	    واپس (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_لیبل_فہرست:
	    واپس (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	حالت CTRL_X_راستہ_PATTERNS:
	    واپس (c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_راستہ_متعین_کرتا:
	    واپس (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#اگر_ختم
	حالت CTRL_X_CMDLINE:
	    واپس (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	حالت CTRL_X_FUNCTION:
	    واپس (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_OMNI:
	    واپس (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#اگر_ختم
	حالت CTRL_X_SPELL:
	    واپس (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_EVAL:
	    واپس (c == Ctrl_P || c == Ctrl_N);
	حالت CTRL_X_رجسٹر:
	    واپس (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    اندرونی_error("vim_is_ctrl_x_key()");
    واپس FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    ساکن صحیح
مطابقت_at_original_متن(compl_T *مطابقت)
{
    واپس مطابقت->cp_flags & CP_ORIGINAL_متن;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    ساکن صحیح
is_پہلا_مطابقت(compl_T *مطابقت)
{
    واپس مطابقت == compl_پہلا_مطابقت;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    ساکن compl_T *
ڈھونڈو_original_متن_مطابقت(خالی)
{
    اگر (compl_پہلا_مطابقت == تہی)
	واپس تہی;
    اگر (مطابقت_at_original_متن(compl_پہلا_مطابقت))
	واپس compl_پہلا_مطابقت;
    اگر (compl_پہلا_مطابقت->cp_پچھلا != تہی
	    && مطابقت_at_original_متن(compl_پہلا_مطابقت->cp_پچھلا))
	واپس compl_پہلا_مطابقت->cp_پچھلا;
    واپس تہی;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    صحیح
ins_compl_accept_char(صحیح c)
{
    اگر (compl_autocomplete && compl_from_nonkeyword)
	واپس FALSE;

    اگر (ctrl_x_منوال & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	واپس vim_isIDc(c);

    انتخاب (ctrl_x_منوال)
    {
	حالت CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    واپس vim_isfilec(c) && !vim_ispathsep(c);

	حالت CTRL_X_CMDLINE:
	حالت CTRL_X_CMDLINE_CTRL_X:
	حالت CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    واپس vim_isprintc(c) && !VIM_ISWHITE(c);

	حالت CTRL_X_پورا_سطر:
	    // For while line completion a space can be part of the line.
	    واپس vim_isprintc(c);
    }
    واپس vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    ساکن char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    صحیح	    char_len,
    صحیح	    compl_char_len,
    صحیح	    min_len,
    char_u  **tofree)
{
    صحیح		*wca;			// Wide character array.
    char_u	*p;
    صحیح		i, c;
    صحیح		has_زیریں = FALSE;
    صحیح		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(صحیح, char_len);
    اگر (wca == تہی)
	واپس IObuff;

    p = str;
    برائے (i = 0; i < char_len; ++i)
    {
	اگر (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	ورنہ
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_متن.string;
    برائے (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	اگر (MB_ISLOWER(c))
	{
	    has_زیریں = TRUE;
	    اگر (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		برائے (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		توڑو;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    اگر (!has_زیریں)
    {
	p = compl_orig_متن.string;
	برائے (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    اگر (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		برائے (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		توڑو;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_متن.string;
    برائے (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	اگر (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	ورنہ اگر (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    جب (i < char_len)
    {
	اگر (gap.ga_ڈیٹا != تہی)
	{
	    اگر (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		واپس (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_ڈیٹا + gap.ga_len;
	    اگر (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    ورنہ
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	ورنہ اگر ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    اگر (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		واپس (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_ڈیٹا, IObuff);
	    gap.ga_len = (صحیح)(p - IObuff);
	}
	ورنہ اگر (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	ورنہ
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    اگر (gap.ga_ڈیٹا != تہی)
    {
	*tofree = gap.ga_ڈیٹا;
	واپس gap.ga_ڈیٹا;
    }

    *p = NUL;
    واپس IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    صحیح
ins_compl_جوڑو_infercase(
    char_u	*str_arg,
    صحیح		len,
    صحیح		icase,
    char_u	*fname,
    صحیح		dir,
    صحیح		cont_s_ipos,  // next ^X<> will set initial_pos
    صحیح		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    صحیح		char_len;		// count multi-byte characters
    صحیح		compl_char_len;
    صحیح		min_len;
    صحیح		flags = 0;
    صحیح		res;
    char_u	*tofree = تہی;

    اگر (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	اگر (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    جب (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	ورنہ
	{
	    char_len = len;
	}

	// Find actual length of original text.
	اگر (has_mbyte)
	{
	    p = compl_orig_متن.string;
	    compl_char_len = 0;
	    جب (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	ورنہ
	{
	    compl_char_len = compl_لمبائی;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    اگر (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    اگر (icase)
	flags |= CP_ICASE;

    res = ins_compl_جوڑو(str, len, fname, تہی, تہی, dir, flags, FALSE, تہی, score);
    vim_free(tofree);
    واپس res;
}

/*
 * Check if fuzzy matching is enabled
 */
    ساکن صحیح
cot_fuzzy(خالی)
{
    واپس (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_منوال_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    ساکن صحیح
is_قریب_ترین_فعال(خالی)
{
    واپس (compl_autocomplete || (get_cot_flags() & COT_قریب_ترین))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    صحیح
ins_compl_is_مطابقت_selected(خالی)
{
    واپس compl_shown_مطابقت != تہی && !is_پہلا_مطابقت(compl_shown_مطابقت);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    صحیح
ins_compl_preinsert_longest(خالی)
{
    واپس compl_autocomplete
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
    ساکن صحیح
ins_compl_جوڑو(
    char_u	*str,
    صحیح		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_ڈیٹا غیر_مستعمل,  // "user_data" entry or NULL
    صحیح		cdir,
    صحیح		flags_arg,
    صحیح		adup,		    // accept duplicate match
    صحیح		*user_hl,           // user abbr/kind hlattr
    صحیح		score)
{
    compl_T	*مطابقت, *current, *پچھلا;
    صحیح		dir = (cdir == 0 ? compl_direction : cdir);
    صحیح		flags = flags_arg;
    صحیح		inserted = FALSE;
    char_u	*new_str = تہی;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    اگر (flags & CP_FAST)
	fast_breakcheck();
    ورنہ
	ui_breakcheck();
    اگر (got_int)
	واپس FAIL;
    اگر (len < 0)
	len = (صحیح)STRLEN(str);

    // If the same match is already present, don't add it.
    اگر (compl_پہلا_مطابقت != تہی && !adup && compl_لڑیاں_ht.ht_مستعمل > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *کلید;
	hashitem_T  *hi;

	اگر (len < (صحیح)حجم(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    کلید = keybuf;
	}
	ورنہ
	{
	    new_str = vim_strnsave(str, len);
	    اگر (new_str == تہی)
		واپس FAIL;
	    کلید = new_str;
	}
	str_hash = hash_hash(کلید);
	hi = hash_lookup(&compl_لڑیاں_ht, کلید, str_hash);
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    اگر (is_قریب_ترین_فعال() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		مطابقت = compl_پہلا_مطابقت;
		کرو
		{
		    اگر (!مطابقت_at_original_متن(مطابقت)
			    && STRNCMP(مطابقت->cp_str.string, str, len) == 0
			    && ((صحیح)مطابقت->cp_str.لمبائی <= len
					  || مطابقت->cp_str.string[len] == NUL))
		    {
			اگر (score < مطابقت->cp_score)
			    مطابقت->cp_score = score;
			توڑو;
		    }
		    مطابقت = مطابقت->cp_اگلا;
		} جب (مطابقت != تہی && !is_پہلا_مطابقت(مطابقت));
	    }
	    vim_free(new_str);
	    واپس NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    مطابقت = ALLOC_CLEAR_ONE(compl_T);
    اگر (مطابقت == تہی)
    {
	vim_free(new_str);
	واپس FAIL;
    }
    مطابقت->cp_نمبر = flags & CP_ORIGINAL_متن ? 0 : -1;
    مطابقت->cp_ترتیب = -1;
    اگر (new_str == تہی && (new_str = vim_strnsave(str, len)) == تہی)
    {
	vim_free(مطابقت);
	واپس FAIL;
    }
    مطابقت->cp_str.string = new_str;
    مطابقت->cp_str.لمبائی = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    اگر (fname != تہی
	    && compl_curr_مطابقت != تہی
	    && compl_curr_مطابقت->cp_fname != تہی
	    && STRCMP(fname, compl_curr_مطابقت->cp_fname) == 0)
	مطابقت->cp_fname = compl_curr_مطابقت->cp_fname;
    ورنہ اگر (fname != تہی)
    {
	مطابقت->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    ورنہ
	مطابقت->cp_fname = تہی;
    مطابقت->cp_flags = flags;
    مطابقت->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    مطابقت->cp_user_قسم_hl_id = user_hl ? user_hl[1] : 0;
    مطابقت->cp_score = score;
    مطابقت->cp_cpt_ماخذ_idx = cpt_sources_اشاریہ;

    اگر (cptext != تہی)
    {
	برائے (صحیح i = 0; i < CPT_گنتی; ++i)
	{
	    اگر (cptext[i] != تہی && *cptext[i] != NUL)
		مطابقت->cp_متن[i] = vim_strsave(cptext[i]);
	}
    }
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (user_ڈیٹا != تہی)
	مطابقت->cp_user_ڈیٹا = *user_ڈیٹا;
#اگر_ختم

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    اگر (compl_پہلا_مطابقت == تہی)
	مطابقت->cp_اگلا = مطابقت->cp_پچھلا = تہی;
    ورنہ اگر (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_پہلا_مطابقت->cp_اگلا;
	پچھلا = compl_پہلا_مطابقت;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	جب (current != تہی && current != compl_پہلا_مطابقت)
	{
	    اگر (current->cp_score < score)
	    {
		مطابقت->cp_اگلا = current;
		مطابقت->cp_پچھلا = current->cp_پچھلا;
		اگر (current->cp_پچھلا)
		    current->cp_پچھلا->cp_اگلا = مطابقت;
		current->cp_پچھلا = مطابقت;
		inserted = TRUE;
		توڑو;
	    }
	    پچھلا = current;
	    current = current->cp_اگلا;
	}
	اگر (!inserted)
	{
	    پچھلا->cp_اگلا = مطابقت;
	    مطابقت->cp_پچھلا = پچھلا;
	    مطابقت->cp_اگلا = compl_پہلا_مطابقت;
	    compl_پہلا_مطابقت->cp_پچھلا = مطابقت;
	}
    }
    ورنہ اگر (dir == FORWARD)
    {
	مطابقت->cp_اگلا = compl_curr_مطابقت->cp_اگلا;
	مطابقت->cp_پچھلا = compl_curr_مطابقت;
    }
    ورنہ	// BACKWARD
    {
	مطابقت->cp_اگلا = compl_curr_مطابقت;
	مطابقت->cp_پچھلا = compl_curr_مطابقت->cp_پچھلا;
    }
    اگر (مطابقت->cp_اگلا)
	مطابقت->cp_اگلا->cp_پچھلا = مطابقت;
    اگر (مطابقت->cp_پچھلا)
	مطابقت->cp_پچھلا->cp_اگلا = مطابقت;
    ورنہ	// if there's nothing before, it is the first match
	compl_پہلا_مطابقت = مطابقت;
    compl_curr_مطابقت = مطابقت;

    اگر (!مطابقت_at_original_متن(مطابقت))
	compl_لڑیاں_جوڑو(مطابقت->cp_str.string, مطابقت->cp_str.لمبائی,
								    str_hash);

    // Find the longest common string if still doing that.
    اگر (compl_get_longest && (flags & CP_ORIGINAL_متن) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_منوال_thesaurus())
	ins_compl_longest_مطابقت(مطابقت);

    واپس OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    ساکن صحیح
ins_compl_equal(compl_T *مطابقت, char_u *str, صحیح len)
{
    اگر (مطابقت->cp_flags & CP_EQUAL)
	واپس TRUE;
    اگر (مطابقت->cp_flags & CP_ICASE)
	واپس STRNICMP(مطابقت->cp_str.string, str, (size_t)len) == 0;
    واپس STRNCMP(مطابقت->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    ساکن صحیح
ins_compl_equal_sc(compl_T *مطابقت, char_u *str, صحیح len)
{
    صحیح	typed = compl_لمبائی;
    صحیح	longest_انتہا = (compl_get_longest && compl_longest_انتہا_ستون > compl_ستون)
			    ? (صحیح)(compl_longest_انتہا_ستون - compl_ستون) : typed;

    اگر ((مطابقت->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_انتہا <= typed)
	واپس ins_compl_equal(مطابقت, str, len);

    اگر ((صحیح)مطابقت->cp_str.لمبائی < len)
	واپس FALSE;

    برائے (صحیح i = 0; i < len; ++i)
    {
	اگر (i >= typed && i < longest_انتہا
		? TOLOWER_LOC(مطابقت->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: مطابقت->cp_str.string[i] != str[i])
	    واپس FALSE;
    }
    واپس TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    ساکن خالی
ins_compl_داخل_کرو_بائٹیں(char_u *p, صحیح len)
{
    اگر (len == -1)
	len = (صحیح)STRLEN(p);
    ins_بائٹیں_len(p, len);
    compl_ins_انتہا_ستون = curwin->w_cursor.ستون;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(خالی)
{
    واپس compl_leader.string != تہی ? compl_leader.string : compl_orig_متن.string;
}

/*
 * Get current completion leader length
 */
    ساکن size_t
ins_compl_leader_len(خالی)
{
    واپس compl_leader.string != تہی ? compl_leader.لمبائی : compl_orig_متن.لمبائی;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    صحیح
ins_compl_ستون_range_attr(linenr_T lnum, صحیح ستون)
{
    صحیح	    آغاز_ستون;
    صحیح	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    صحیح	    attr;

    اگر (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	واپس -1;

    آغاز_ستون = compl_ستون + (صحیح)ins_compl_leader_len();
    اگر (!ins_compl_has_مضرب())
	واپس (ستون >= آغاز_ستون && ستون < compl_ins_انتہا_ستون) ? attr : -1;

    // Multiple lines
    اگر ((lnum == compl_lnum && ستون >= آغاز_ستون && ستون < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && ستون <= compl_ins_انتہا_ستون))
	واپس attr;

    واپس -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    ساکن صحیح
ins_compl_has_مضرب(خالی)
{
    واپس vim_strchr(compl_shown_مطابقت->cp_str.string, '\n') != تہی;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    صحیح
ins_compl_lnum_اندر_range(linenr_T lnum)
{
    اگر (!ins_compl_has_مضرب())
	واپس FALSE;
    واپس lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    ساکن خالی
ins_compl_longest_مطابقت(compl_T *مطابقت)
{
    char_u	*p, *s;
    صحیح		c1, c2;
    صحیح		had_مطابقت;

    اگر (compl_leader.string == تہی)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(مطابقت->cp_str.string, مطابقت->cp_str.لمبائی);
	اگر (compl_leader.string == تہی)
	    واپس;

	compl_leader.لمبائی = مطابقت->cp_str.لمبائی;
	had_مطابقت = (curwin->w_cursor.ستون > compl_ستون);
	ins_compl_longest_داخل_کرو(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	اگر (!had_مطابقت)
	    ins_compl_delete();
	compl_مستعمل_مطابقت = FALSE;

	واپس;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = مطابقت->cp_str.string;
    جب (*p != NUL)
    {
	اگر (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	ورنہ
	{
	    c1 = *p;
	    c2 = *s;
	}
	اگر ((مطابقت->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    توڑو;
	اگر (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	ورنہ
	{
	    ++p;
	    ++s;
	}
    }

    اگر (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.لمبائی = (size_t)(p - compl_leader.string);

	had_مطابقت = (curwin->w_cursor.ستون > compl_ستون);
	ins_compl_longest_داخل_کرو(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	اگر (!had_مطابقت)
	    ins_compl_delete();
    }

    compl_مستعمل_مطابقت = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    ساکن خالی
ins_compl_جوڑو_matches(
    صحیح		عدد_matches,
    char_u	**matches,
    صحیح		icase)
{
    صحیح		جوڑو_r = OK;
    صحیح		dir = compl_direction;

    برائے (صحیح i = 0; i < عدد_matches && جوڑو_r != FAIL; i++)
    {
	جوڑو_r = ins_compl_جوڑو(matches[i], -1, تہی, تہی, تہی, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, تہی,
		FUZZY_SCORE_NONE);
	اگر (جوڑو_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(عدد_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    ساکن صحیح
ins_compl_make_cyclic(خالی)
{
    compl_T *مطابقت;
    صحیح	    گنتی = 0;

    اگر (compl_پہلا_مطابقت == تہی)
	واپس 0;

    // Find the end of the list.
    مطابقت = compl_پہلا_مطابقت;
    // there's always an entry for the compl_orig_text, it doesn't count.
    جب (مطابقت->cp_اگلا != تہی && !is_پہلا_مطابقت(مطابقت->cp_اگلا))
    {
	مطابقت = مطابقت->cp_اگلا;
	++گنتی;
    }
    مطابقت->cp_اگلا = compl_پہلا_مطابقت;
    compl_پہلا_مطابقت->cp_پچھلا = مطابقت;

    واپس گنتی;
}

/*
 * Return whether there currently is a shown match.
 */
    صحیح
ins_compl_has_shown_مطابقت(خالی)
{
    واپس compl_shown_مطابقت == تہی
	|| compl_shown_مطابقت != compl_shown_مطابقت->cp_اگلا;
}

/*
 * Return whether the shown match is long enough.
 */
    صحیح
ins_compl_long_shown_مطابقت(خالی)
{
    واپس (صحیح)compl_shown_مطابقت->cp_str.لمبائی
					    > curwin->w_cursor.ستون - compl_ستون;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    غیرعلامتی صحیح
get_cot_flags(خالی)
{
    واپس curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    ساکن خالی
ins_compl_upd_pum(خالی)
{
    اگر (compl_مطابقت_array == تہی)
	واپس;

    صحیح h = curwin->w_cline_اونچائی;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    اگر (h != curwin->w_cline_اونچائی)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    ساکن خالی
ins_compl_del_pum(خالی)
{
    اگر (compl_مطابقت_array == تہی)
	واپس;

    pum_undisplay();
    VIM_CLEAR(compl_مطابقت_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    صحیح
pum_wanted(خالی)
{
    // 'completeopt' must contain "menu" or "menuone"
    واپس (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    ساکن صحیح
pum_enough_matches(خالی)
{
    compl_T     *compl;
    صحیح		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_پہلا_مطابقت;
    کرو
    {
	اگر (compl == تہی || (!مطابقت_at_original_متن(compl) && ++i == 2))
	    توڑو;
	compl = compl->cp_اگلا;
    } جب (!is_پہلا_مطابقت(compl));

    اگر ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	واپس (i >= 1);
    واپس (i >= 2);
}

#اگر defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    ساکن dict_T *
ins_compl_dict_alloc(compl_T *مطابقت)
{
    dict_T *dict = dict_alloc_تالا(VAR_FIXED);

    اگر (dict == تہی)
	واپس تہی;
    fill_complete_info_dict(dict, مطابقت, FALSE);
    واپس dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    ساکن خالی
trigger_complete_بدلا_event(صحیح cur)
{
    dict_T	    *v_event;
    dict_T	    *شے;
    ساکن صحیح	    recursive = FALSE;
    save_v_event_T  save_v_event;

    اگر (recursive)
	واپس;

    شے = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_مطابقت);
    اگر (شے == تہی)
	واپس;
    v_event = get_v_event(&save_v_event);
    dict_جوڑو_dict(v_event, "completed_item", شے);
    pum_سیٹ_event_info(v_event);
    dict_سیٹ_اشیاء_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, تہی, تہی, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#اگر_ختم

/*
 * Helper functions for mergesort_list().
 */
    ساکن خالی*
cp_get_اگلا(خالی *گرہ)
{
    واپس ((compl_T*)گرہ)->cp_اگلا;
}

    ساکن خالی
cp_سیٹ_اگلا(خالی *گرہ, خالی *اگلا)
{
    ((compl_T*)گرہ)->cp_اگلا = (compl_T*)اگلا;
}

    ساکن خالی*
cp_get_پچھلا(خالی* گرہ)
{
    واپس ((compl_T*)گرہ)->cp_پچھلا;
}

    ساکن خالی
cp_سیٹ_پچھلا(خالی* گرہ, خالی* پچھلا)
{
    ((compl_T*)گرہ)->cp_پچھلا = (compl_T*)پچھلا;
}

    ساکن صحیح
cp_compare_fuzzy(ثابت خالی* a, ثابت خالی* b)
{
    صحیح score_a = ((compl_T*)a)->cp_score;
    صحیح score_b = ((compl_T*)b)->cp_score;

    اگر (score_a != score_b)
	واپس (score_b > score_a) ? 1 : -1;

    واپس ((compl_T *)a)->cp_ترتیب - ((compl_T *)b)->cp_ترتیب;
}

    ساکن صحیح
cp_compare_قریب_ترین(ثابت خالی* a, ثابت خالی* b)
{
    صحیح score_a = ((compl_T*)a)->cp_score;
    صحیح score_b = ((compl_T*)b)->cp_score;
    اگر (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	واپس 0;
    واپس (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    ساکن صحیح
prepend_startcol_متن(string_T *dest, string_T *src, صحیح startcol)
{
    صحیح prepend_len = compl_ستون - startcol;
    صحیح new_لمبائی = prepend_len + (صحیح)src->لمبائی;

    dest->لمبائی = (size_t)new_لمبائی;
    dest->string = alloc(new_لمبائی + 1);  // +1 for NUL
    اگر (dest->string == تہی)
    {
	dest->لمبائی = 0;
	واپس FAIL;
    }

    char_u	*سطر = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, سطر + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->لمبائی);
    dest->string[new_لمبائی] = NUL;
    واپس OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    ساکن string_T *
get_leader_for_startcol(compl_T *مطابقت, صحیح cached)
{
    ساکن string_T adjusted_leader = {تہی, 0};

    اگر (مطابقت == تہی)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	واپس تہی;
    }

    اگر (cpt_sources_array == تہی)
	جاؤ theend;

    صحیح	cpt_idx = مطابقت->cp_cpt_ماخذ_idx;
    اگر (cpt_idx < 0)
	جاؤ theend;
    صحیح	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    اگر (compl_leader.string == تہی)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	اگر (startcol < 0 || startcol >= compl_ستون)
	    واپس &compl_orig_متن;
	واپس &compl_leader;  // pass through (startcol < compl_col)
    }

    اگر (compl_ستون <= 0)
	جاؤ theend;

    اگر (startcol >= 0 && startcol < compl_ستون)
    {
	صحیح prepend_len = compl_ستون - startcol;
	صحیح new_لمبائی = prepend_len + (صحیح)compl_leader.لمبائی;
	اگر (cached && (size_t)new_لمبائی == adjusted_leader.لمبائی
		&& adjusted_leader.string != تہی)
	    واپس &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	اگر (prepend_startcol_متن(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    جاؤ theend;

	واپس &adjusted_leader;
    }
theend:
    واپس &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    ساکن خالی
سیٹ_fuzzy_score(خالی)
{
    compl_T *compl;
    char_u  *نمونہ;
    صحیح	    use_leader;

    اگر (compl_پہلا_مطابقت == تہی)
	واپس;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != تہی && compl_leader.لمبائی > 0);
    اگر (!use_leader)
    {
	اگر (compl_orig_متن.string == تہی || compl_orig_متن.لمبائی == 0)
	    واپس;
	نمونہ = compl_orig_متن.string;
    }
    ورنہ
    {
	/* Clear the leader cache once before the loop */
	(خالی)get_leader_for_startcol(تہی, TRUE);
	نمونہ = تہی;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    صحیح اگلا = 0;
    compl = compl_پہلا_مطابقت;
    کرو
    {
	اگر (compl->cp_ترتیب < 0)
	    compl->cp_ترتیب = اگلا;
	اگلا = MAX(اگلا, compl->cp_ترتیب + 1);

	اگر (use_leader)
	    نمونہ = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_مطابقت_str(compl->cp_str.string, نمونہ);
	compl = compl->cp_اگلا;
    } جب (compl != تہی && !is_پہلا_مطابقت(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    ساکن خالی
ترتیب_دو_compl_مطابقت_list(صحیح (*compare)(ثابت خالی *, ثابت خالی *))
{
    compl_T	*orig_متن;

    اگر (!compl_پہلا_مطابقت || is_پہلا_مطابقت(compl_پہلا_مطابقت->cp_اگلا))
	واپس;

    orig_متن = ڈھونڈو_original_متن_مطابقت();
    اگر (orig_متن == تہی)
	واپس;

    ins_compl_make_خطی();
    اگر (orig_متن == compl_پہلا_مطابقت)
    {
	compl_پہلا_مطابقت->cp_اگلا->cp_پچھلا = تہی;
	compl_پہلا_مطابقت->cp_اگلا = mergesort_list(compl_پہلا_مطابقت->cp_اگلا,
		cp_get_اگلا, cp_سیٹ_اگلا, cp_get_پچھلا, cp_سیٹ_پچھلا, compare);
	compl_پہلا_مطابقت->cp_اگلا->cp_پچھلا = compl_پہلا_مطابقت;
    }
    ورنہ
    {
	compl_T	*دم;

	orig_متن->cp_پچھلا->cp_اگلا = تہی;
	compl_پہلا_مطابقت = mergesort_list(compl_پہلا_مطابقت, cp_get_اگلا,
		cp_سیٹ_اگلا, cp_get_پچھلا, cp_سیٹ_پچھلا, compare);
	دم = compl_پہلا_مطابقت;
	جب (دم->cp_اگلا != تہی)
	    دم = دم->cp_اگلا;
	دم->cp_اگلا = orig_متن;
	orig_متن->cp_پچھلا = دم;
    }
    (خالی)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    ساکن صحیح
get_user_highlight_attr(صحیح hl_id)
{
    صحیح	    attr;

    اگر (hl_id <= 0)
	واپس -1;
    attr = syn_id2attr(hl_id);
    واپس attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    ساکن صحیح
ins_compl_build_pum(خالی)
{
    compl_T     *compl;
    compl_T     *shown_compl = تہی;
    صحیح		did_ڈھونڈو_shown_مطابقت = FALSE;
    صحیح		shown_مطابقت_ok = FALSE;
    صحیح		i = 0;
    صحیح		cur = -1;
    صحیح		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*مطابقت_سر = تہی;
    compl_T	*مطابقت_دم = تہی;
    compl_T	*مطابقت_اگلا = تہی;
    صحیح		*مطابقت_گنتی = تہی;
    صحیح		is_forward = compl_shows_dir_forward();
    صحیح		is_cpt_completion = (cpt_sources_array != تہی);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_مطابقت_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    اگر (مطابقت_at_original_متن(compl_shown_مطابقت))
	shown_مطابقت_ok = TRUE;

    اگر (compl_leader.string != تہی
	    && STRCMP(compl_leader.string, compl_orig_متن.string) == 0
	    && shown_مطابقت_ok == FALSE)
	compl_shown_مطابقت = compl_no_select ? compl_پہلا_مطابقت
					    : compl_پہلا_مطابقت->cp_اگلا;

    اگر (is_cpt_completion)
    {
	مطابقت_گنتی = ALLOC_CLEAR_MULT(صحیح, cpt_sources_گنتی);
	اگر (مطابقت_گنتی == تہی)
	    واپس -1;
    }

    (خالی)get_leader_for_startcol(تہی, TRUE); // Clear the cache

    compl = compl_پہلا_مطابقت;
    کرو
    {
	compl->cp_اندر_مطابقت_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	اگر (ctrl_x_منوال_معمول() && !p_inf && compl_orig_متن.string
		&& !ignorecase(compl_orig_متن.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	اگر (!مطابقت_at_original_متن(compl)
		&& (leader->string == تہی
		    || ins_compl_equal_sc(compl, leader->string,
			(صحیح)leader->لمبائی)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    صحیح مطابقت_حد_بندی_exceeded = FALSE;
	    صحیح cur_ماخذ = compl->cp_cpt_ماخذ_idx;
	    اگر (is_forward && cur_ماخذ != -1 && is_cpt_completion)
	    {
		مطابقت_گنتی[cur_ماخذ]++;
		صحیح max_matches = cpt_sources_array[cur_ماخذ].cs_max_matches;
		اگر (max_matches > 0 && مطابقت_گنتی[cur_ماخذ] > max_matches)
		    مطابقت_حد_بندی_exceeded = TRUE;
	    }

	    اگر (!مطابقت_حد_بندی_exceeded)
	    {
		++compl_مطابقت_arraysize;
		compl->cp_اندر_مطابقت_array = TRUE;
		اگر (مطابقت_سر == تہی)
		    مطابقت_سر = compl;
		ورنہ
		    مطابقت_دم->cp_مطابقت_اگلا = compl;
		مطابقت_دم = compl;

		اگر (!shown_مطابقت_ok && !cot_fuzzy())
		{
		    اگر (compl == compl_shown_مطابقت || did_ڈھونڈو_shown_مطابقت)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_مطابقت = compl;
			did_ڈھونڈو_shown_مطابقت = TRUE;
			shown_مطابقت_ok = TRUE;
		    }
		    ورنہ
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		ورنہ اگر (cot_fuzzy())
		{
		    اگر (i == 0)
			shown_compl = compl;

		    اگر (!shown_مطابقت_ok && compl == compl_shown_مطابقت)
		    {
			cur = i;
			shown_مطابقت_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	اگر (compl == compl_shown_مطابقت && !cot_fuzzy())
	{
	    did_ڈھونڈو_shown_مطابقت = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    اگر (مطابقت_at_original_متن(compl))
		shown_مطابقت_ok = TRUE;

	    اگر (!shown_مطابقت_ok && shown_compl != تہی)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_مطابقت = shown_compl;
		shown_مطابقت_ok = TRUE;
	    }
	}
	compl = compl->cp_اگلا;
    } جب (compl != تہی && !is_پہلا_مطابقت(compl));

    vim_free(مطابقت_گنتی);

    اگر (compl_مطابقت_arraysize == 0)
	واپس -1;

    اگر (cot_fuzzy() && !compl_no_select && !shown_مطابقت_ok)
    {
	compl_shown_مطابقت = shown_compl;
	shown_مطابقت_ok = TRUE;
	cur = 0;
    }

    compl_مطابقت_array = ALLOC_CLEAR_MULT(pumitem_T, compl_مطابقت_arraysize);
    اگر (compl_مطابقت_array == تہی)
	واپس -1;

    compl = مطابقت_سر;
    i = 0;
    جب (compl != تہی)
    {
	compl_مطابقت_array[i].pum_متن = compl->cp_متن[CPT_ABBR] != تہی
			    ? compl->cp_متن[CPT_ABBR] : compl->cp_str.string;
	compl_مطابقت_array[i].pum_قسم = compl->cp_متن[CPT_قسم];
	compl_مطابقت_array[i].pum_info = compl->cp_متن[CPT_INFO];
	compl_مطابقت_array[i].pum_cpt_ماخذ_idx = compl->cp_cpt_ماخذ_idx;
	compl_مطابقت_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_مطابقت_array[i].pum_user_قسم_hlattr =
			get_user_highlight_attr(compl->cp_user_قسم_hl_id);
	compl_مطابقت_array[i++].pum_extra = compl->cp_متن[CPT_MENU] != تہی
			    ? compl->cp_متن[CPT_MENU] : compl->cp_fname;
	مطابقت_اگلا = compl->cp_مطابقت_اگلا;
	compl->cp_مطابقت_اگلا = تہی;
	compl = مطابقت_اگلا;
    }

    اگر (!shown_مطابقت_ok)    // no displayed match at all
	cur = -1;

    واپس cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    خالی
ins_compl_show_pum(خالی)
{
    صحیح		i;
    صحیح		cur = -1;
    colnr_T	ستون;

    اگر (!pum_wanted() || !pum_enough_matches())
	واپس;

    // Avoid redrawing the screen under a pum that stays in place.
    اگر (!pum_redraw_اندر_same_position())
	pum_call_update_screen();

    اگر (compl_مطابقت_array == تہی)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    ورنہ
    {
	// popup menu already exists, only need to find the current item.
	برائے (i = 0; i < compl_مطابقت_arraysize; ++i)
	{
	    اگر (compl_مطابقت_array[i].pum_متن == compl_shown_مطابقت->cp_str.string
		    || compl_مطابقت_array[i].pum_متن
				      == compl_shown_مطابقت->cp_متن[CPT_ABBR])
	    {
		cur = i;
		توڑو;
	    }
	}
    }

    اگر (compl_مطابقت_array == تہی)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (compl_started && has_completechanged())
	    trigger_complete_بدلا_event(cur);
#اگر_ختم
	واپس;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    ستون = curwin->w_cursor.ستون;
    curwin->w_cursor.ستون = compl_ستون;
    validate_cursor_ستون();
    صحیح pum_wcol = curwin->w_wcol;
    curwin->w_cursor.ستون = ستون;
    validate_cursor_ستون();
    compl_selected_شے = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_حالت = true;
    pum_display(compl_مطابقت_array, compl_مطابقت_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    اگر (compl_started && compl_curr_مطابقت != compl_shown_مطابقت)
	compl_curr_مطابقت = compl_shown_مطابقت;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (has_completechanged())
	trigger_complete_بدلا_event(cur);
#اگر_ختم
}

#تعریف_کرو DICT_پہلا	(1)	// use just first element in "dict"
#تعریف_کرو DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    ساکن خالی
ins_compl_dictionaries(
    char_u	*dict_آغاز,
    char_u	*pat,
    صحیح		flags,		// DICT_FIRST and/or DICT_EXACT
    صحیح		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_آغاز;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    صحیح		گنتی;
    صحیح		save_p_scs;
    صحیح		dir = compl_direction;

    اگر (*dict == NUL)
    {
#اگر_تعریف_شدہ FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	اگر (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	ورنہ
#اگر_ختم
	    واپس;
    }

    buf = alloc(LSIZE);
    اگر (buf == تہی)
	واپس;
    regmatch.regprog = تہی;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    اگر (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    اگر (ctrl_x_منوال_سطر_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	اگر (pat_esc == تہی)
	    جاؤ theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	اگر (ptr == تہی)
	{
	    vim_free(pat_esc);
	    جاؤ theend;
	}
	vim_snprintf((حرف *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    ورنہ
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog == تہی)
	    جاؤ theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    جب (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	اگر (flags == DICT_EXACT)
	{
	    گنتی = 1;
	    files = &dict;
	}
	ورنہ
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#اگر_تعریف_شدہ FEAT_SPELL
	    اگر (!thesaurus && STRCMP(buf, "spell") == 0)
		گنتی = -1;
	    ورنہ
#اگر_ختم
		اگر (vim_strchr(buf, '`') != تہی
		    || expand_wildcards(1, &buf, &گنتی, &files,
						     EW_FILE|EW_SILENT) != OK)
		گنتی = 0;
	}

#اگر_تعریف_شدہ FEAT_SPELL
	اگر (گنتی == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    اگر (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    ورنہ
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	ورنہ
#اگر_ختم
	    اگر (گنتی > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(گنتی, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    اگر (flags != DICT_EXACT)
		FreeWild(گنتی, files);
	}
	اگر (flags != 0)
	    توڑو;
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
    ساکن صحیح
thesaurus_جوڑو_الفاظ_اندر_سطر(
    char_u	*fname,
    char_u	**buf_arg,
    صحیح		dir,
    char_u	*skip_لفظ)
{
    صحیح		حالت_2 = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    جب (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = ڈھونڈو_لفظ_آغاز(ptr);
	اگر (*ptr == NUL || *ptr == NL)
	    توڑو;
	wstart = ptr;

	// Find end of the word.
	اگر (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    جب (*ptr != NUL)
	    {
		صحیح l = (*mb_ptr2len)(ptr);

		اگر (l < 2 && !vim_iswordc(*ptr))
		    توڑو;
		ptr += l;
	    }
	ورنہ
	    ptr = ڈھونڈو_لفظ_انتہا(ptr);

	// Add the word. Skip the regexp match.
	اگر (wstart != skip_لفظ)
	{
	    حالت_2 = ins_compl_جوڑو_infercase(wstart, (صحیح)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    اگر (حالت_2 == FAIL)
		توڑو;
	}
    }

    *buf_arg = ptr;
    واپس حالت_2;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    ساکن خالی
ins_compl_files(
    صحیح		گنتی,
    char_u	**files,
    صحیح		thesaurus,
    صحیح		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    صحیح		*dir)
{
    char_u	*ptr;
    صحیح		i;
    FILE	*fp;
    صحیح		جوڑو_r;
    char_u	*leader = تہی;
    صحیح		leader_len = 0;
    صحیح		score = 0;
    صحیح		len = 0;
    char_u	*سطر_انتہا = تہی;

    اگر (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (صحیح)ins_compl_leader_len();
    }

    برائے (i = 0; i < گنتی && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((حرف *)files[i], "r");  // open dictionary file
	اگر (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    پیغام_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((حرف *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (حرف *)files[i]);
	    (خالی)پیغام_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	اگر (fp == تہی)
	    جاری;

	// Read dictionary file line by line.
	// Check each line for a match.
	جب (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    اگر (cot_fuzzy() && leader_len > 0)
	    {
		سطر_انتہا = ڈھونڈو_سطر_انتہا(ptr);
		جب (ptr < سطر_انتہا)
		{
		    اگر (fuzzy_مطابقت_str_اندر_سطر(&ptr, leader, &len, تہی, &score))
		    {
			char_u *end_ptr = ctrl_x_منوال_سطر_or_eval()
					? ڈھونڈو_سطر_انتہا(ptr) : ڈھونڈو_لفظ_انتہا(ptr);
			جوڑو_r = ins_compl_جوڑو_infercase(ptr, (صحیح)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			اگر (جوڑو_r == FAIL)
			    توڑو;
			ptr = end_ptr;  // start from next word
			اگر (compl_get_longest && ctrl_x_منوال_معمول()
				&& compl_پہلا_مطابقت->cp_اگلا
				&& score == compl_پہلا_مطابقت->cp_اگلا->cp_score)
			    compl_عدد_bests++;
		    }
		}
	    }
	    ورنہ اگر (regmatch != تہی)
	    {
		جب (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_منوال_سطر_or_eval() ? ڈھونڈو_سطر_انتہا(ptr)
						    : ڈھونڈو_لفظ_انتہا(ptr);
		    جوڑو_r = ins_compl_جوڑو_infercase(regmatch->startp[0],
			    (صحیح)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    اگر (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			جوڑو_r = thesaurus_جوڑو_الفاظ_اندر_سطر(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    اگر (جوڑو_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    ورنہ اگر (جوڑو_r == FAIL)
			توڑو;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    اگر (*ptr == '\n' || got_int)
			توڑو;
		}
	    }
	    سطر_breakcheck();
	    ins_compl_check_کلیدیں(50, FALSE);
	}
	fclose(fp);
    }
}

/*
 * Find the start of the next word.
 * Returns a pointer to the first char of the word.  Also stops at a NUL.
 */
    char_u *
ڈھونڈو_لفظ_آغاز(char_u *ptr)
{
    اگر (has_mbyte)
    {
	جب (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    ورنہ
    {
	جب (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    واپس ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
ڈھونڈو_لفظ_انتہا(char_u *ptr)
{
    اگر (has_mbyte)
    {
	صحیح آغاز_class = mb_get_class(ptr);
	اگر (آغاز_class > 1)
	    جب (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		اگر (mb_get_class(ptr) != آغاز_class)
		    توڑو;
	    }
    }
    ورنہ
	جب (vim_iswordc(*ptr))
	    ++ptr;
    واپس ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
ڈھونڈو_سطر_انتہا(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    جب (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    واپس s;
}

/*
 * Free a completion item in the list
 */
    ساکن خالی
ins_compl_شے_free(compl_T *مطابقت)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    اگر (compl_لڑیاں_ht.ht_مستعمل > 0 && مطابقت->cp_str.string != تہی
					   && !مطابقت_at_original_متن(مطابقت))
    {
	hashitem_T *hi = hash_ڈھونڈو(&compl_لڑیاں_ht, مطابقت->cp_str.string);

	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    complstr_T *اندراج_2 = HI2CSE(hi);

	    اگر (--اندراج_2->cse_گنتی <= 0)
	    {
		hash_remove(&compl_لڑیاں_ht, hi, "completion match");
		vim_free(اندراج_2);
	    }
	}
    }
    VIM_CLEAR_STRING(مطابقت->cp_str);
    // several entries may use the same fname, free it just once.
    اگر (مطابقت->cp_flags & CP_FREE_FNAME)
	vim_free(مطابقت->cp_fname);
    برائے (صحیح i = 0; i < CPT_گنتی; ++i)
	vim_free(مطابقت->cp_متن[i]);
#اگر_تعریف_شدہ FEAT_EVAL
    clear_tv(&مطابقت->cp_user_ڈیٹا);
#اگر_ختم
    vim_free(مطابقت);
}

/*
 * Free the list of completions
 */
    ساکن خالی
ins_compl_free(خالی)
{
    compl_T *مطابقت;

    VIM_CLEAR_STRING(compl_نمونہ);
    VIM_CLEAR_STRING(compl_leader);

    اگر (compl_پہلا_مطابقت == تہی)
	واپس;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_لڑیاں_ht, CSE_OFF);
    hash_init(&compl_لڑیاں_ht);

    compl_curr_مطابقت = compl_پہلا_مطابقت;
    کرو
    {
	مطابقت = compl_curr_مطابقت;
	compl_curr_مطابقت = compl_curr_مطابقت->cp_اگلا;
	ins_compl_شے_free(مطابقت);
    } جب (compl_curr_مطابقت != تہی && !is_پہلا_مطابقت(compl_curr_مطابقت));
    compl_پہلا_مطابقت = compl_curr_مطابقت = تہی;
    compl_shown_مطابقت = تہی;
    compl_old_مطابقت = تہی;
}

/*
 * Reset/clear the completion state.
 */
    خالی
ins_compl_clear(خالی)
{
    compl_cont_حالت = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_شے = -1;
    compl_ins_انتہا_ستون = 0;
    compl_longest_انتہا_ستون = 0;
    compl_curr_win = تہی;
    compl_curr_buf = تہی;
    VIM_CLEAR_STRING(compl_نمونہ);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = تہی;
    VIM_CLEAR_STRING(compl_orig_متن);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_عدد_bests = 0;
#اگر_تعریف_شدہ FEAT_EVAL
    // clear v:completed_item
    سیٹ_vim_var_dict(VV_COMPLETED_شے, dict_alloc_تالا(VAR_FIXED));
#اگر_ختم
}

/*
 * Return TRUE when Insert completion is active.
 */
    صحیح
ins_compl_فعال(خالی)
{
    واپس compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    صحیح
ins_compl_win_فعال(win_T *wp)
{
    واپس ins_compl_فعال() && wp == compl_curr_win
	&& wp->w_بفر == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    صحیح
ins_compl_مستعمل_مطابقت(خالی)
{
    واپس compl_مستعمل_مطابقت;
}

/*
 * Initialize get longest common string.
 */
    خالی
ins_compl_init_get_longest(خالی)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    صحیح
ins_compl_interrupted(خالی)
{
    واپس compl_interrupted || compl_وقت_قاش_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    صحیح
ins_compl_enter_selects(خالی)
{
    واپس compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_ستون(خالی)
{
    واپس compl_ستون;
}

/*
 * Return the length in bytes of the text being completed
 */
    صحیح
ins_compl_len(خالی)
{
    واپس compl_لمبائی;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    صحیح
ins_compl_has_preinsert(خالی)
{
    صحیح cur_cot_flags = get_cot_flags();
    اگر (compl_autocomplete && p_ic && !p_inf)
	واپس FALSE;
    واپس !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    صحیح
ins_compl_preinsert_effect(خالی)
{
    اگر (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	واپس FALSE;

    واپس curwin->w_cursor.ستون < compl_ins_انتہا_ستون;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    صحیح
ins_compl_bs(خالی)
{
    char_u	*سطر;
    char_u	*p;

    اگر (ins_compl_preinsert_effect())
	ins_compl_delete();

    سطر = ml_get_curline();
    p = سطر + curwin->w_cursor.ستون;
    MB_PTR_عقب(سطر, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    اگر ((صحیح)(p - سطر) - (صحیح)compl_ستون < 0
	    || ((صحیح)(p - سطر) - (صحیح)compl_ستون == 0 && !ctrl_x_منوال_omni())
	    || ctrl_x_منوال_eval()
	    || (!can_bs(BS_آغاز) && (صحیح)(p - سطر) - (صحیح)compl_ستون
							- compl_لمبائی < 0))
	واپس K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    اگر (curwin->w_cursor.ستون <= compl_ستون + compl_لمبائی
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.لمبائی = (size_t)((p - سطر) - compl_ستون);
    compl_leader.string = vim_strnsave(سطر + compl_ستون, compl_leader.لمبائی);
    اگر (compl_leader.string == تہی)
    {
	compl_leader.لمبائی = 0;
	واپس K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    اگر (compl_autocomplete && compl_پہلا_مطابقت && !ins_compl_has_preinsert())
	compl_shown_مطابقت = compl_پہلا_مطابقت;

    ins_compl_new_leader();
    اگر (compl_shown_مطابقت != تہی)
	// Make sure current match is not a hidden item.
	compl_curr_مطابقت = compl_shown_مطابقت;
    واپس NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    ساکن صحیح
ins_compl_need_restart(خالی)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    واپس compl_was_interrupted
	|| ((ctrl_x_منوال_function() || ctrl_x_منوال_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    صحیح
ins_compl_has_autocomplete(خالی)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    واپس curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    ساکن خالی
ins_compl_fuzzy_ترتیب_دو(خالی)
{
    صحیح	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    سیٹ_fuzzy_score();
    اگر (!(cur_cot_flags & COT_NOSORT))
    {
	ترتیب_دو_compl_مطابقت_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	اگر ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    صحیح none_selected = compl_shown_مطابقت == (compl_shows_dir_forward()
		    ? compl_پہلا_مطابقت : compl_پہلا_مطابقت->cp_پچھلا);
	    اگر (!none_selected)
		compl_shown_مطابقت
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_پہلا_مطابقت->cp_اگلا : compl_پہلا_مطابقت;
	}
    }
}

/*
 * Called after changing "compl_leader".
 * Show the popup menu with a different set of matches.
 * May also search for matches again if the previous search was interrupted.
 */
    ساکن خالی
ins_compl_new_leader(خالی)
{
    صحیح	    save_w_wrow = curwin->w_wrow;
    صحیح	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_داخل_کرو_بائٹیں(compl_leader.string + get_compl_len(), -1);
    compl_مستعمل_مطابقت = FALSE;

    اگر (compl_started)
    {
	ins_compl_سیٹ_original_متن(compl_leader.string, compl_leader.لمبائی);
	اگر (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	اگر (cot_fuzzy())
	    ins_compl_fuzzy_ترتیب_دو();
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#اگر_ختم
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    برآمد_flush_cursor(FALSE, FALSE);
	}
#اگر_ختم
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	اگر (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	ورنہ
	    compl_autocomplete = FALSE;
	اگر (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_حالت = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_مستعمل_مطابقت && compl_selected_شے != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    اگر (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    اگر (compl_مطابقت_array == تہی)
	compl_enter_selects = FALSE;
    ورنہ اگر (ins_compl_has_preinsert() && compl_leader.لمبائی > 0)
	ins_compl_داخل_کرو(TRUE, FALSE);
    ورنہ اگر (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.لمبائی > 0 && !ins_compl_preinsert_effect())
	ins_compl_داخل_کرو(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    ساکن صحیح
get_compl_len(خالی)
{
    صحیح off = (صحیح)curwin->w_cursor.ستون - (صحیح)compl_ستون;
    واپس MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    خالی
ins_compl_addleader(صحیح c)
{
    صحیح	    cc;

    اگر (ins_compl_preinsert_effect())
	ins_compl_delete();

    اگر (stop_arrow() == FAIL)
	واپس;
    اگر (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_بائٹیں(buf, cc);
	اگر (compl_opt_refresh_always)
	    Append_مقصد_Redobuff(buf);
    }
    ورنہ
    {
	ins_char(c);
	اگر (compl_opt_refresh_always)
	    Append_Char_مقصد_Redobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    اگر (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    اگر (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.لمبائی = (size_t)(curwin->w_cursor.ستون - compl_ستون);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_ستون,
		compl_leader.لمبائی);
	اگر (compl_leader.string == تہی)
	{
	    compl_leader.لمبائی = 0;
	    واپس;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    ساکن خالی
ins_compl_restart(خالی)
{
    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    compl_cont_حالت = 0;
    compl_cont_منوال = 0;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_عدد_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    ساکن خالی
ins_compl_سیٹ_original_متن(char_u *str, size_t len)
{
    compl_T	*مطابقت = ڈھونڈو_original_متن_مطابقت();
    char_u	*p;

    // Replace the original text entry.
    اگر (مطابقت == تہی)
	واپس;

    p = vim_strnsave(str, len);
    اگر (p == تہی)
	واپس;

    VIM_CLEAR_STRING(مطابقت->cp_str);
    مطابقت->cp_str.string = p;
    مطابقت->cp_str.لمبائی = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    خالی
ins_compl_addfrommatch(خالی)
{
    char_u	*p;
    صحیح		len = (صحیح)curwin->w_cursor.ستون - (صحیح)compl_ستون;
    صحیح		c;

    p = compl_shown_مطابقت->cp_str.string;
    اگر ((صحیح)compl_shown_مطابقت->cp_str.لمبائی <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	اگر (!مطابقت_at_original_متن(compl_shown_مطابقت))
	    واپس;

	p = تہی;
	plen = 0;
	برائے (cp = compl_shown_مطابقت->cp_اگلا; cp != تہی
		&& !is_پہلا_مطابقت(cp); cp = cp->cp_اگلا)
	{
	    اگر (compl_leader.string == تہی
		    || ins_compl_equal(cp, compl_leader.string,
			(صحیح)compl_leader.لمبائی))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.لمبائی;
		توڑو;
	    }
	}
	اگر (p == تہی || (صحیح)plen <= len)
	    واپس;
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
    ساکن صحیح
سیٹ_ctrl_x_منوال(صحیح c)
{
    صحیح	    retval = FALSE;

    انتخاب (c)
    {
	حالت Ctrl_E:
	حالت Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_منوال = CTRL_X_SCROLL;
	    اگر (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    ورنہ
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = تہی;
	    showmode();
	    توڑو;
	حالت Ctrl_L:
	    // complete whole line
	    ctrl_x_منوال = CTRL_X_پورا_سطر;
	    توڑو;
	حالت Ctrl_F:
	    // complete filenames
	    ctrl_x_منوال = CTRL_X_FILES;
	    توڑو;
	حالت Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_منوال = CTRL_X_DICTIONARY;
	    توڑو;
	حالت Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    اگر (vpeekc() == '=')
		توڑو;
	    ctrl_x_منوال = CTRL_X_رجسٹر;
	    توڑو;
	حالت Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_منوال = CTRL_X_THESAURUS;
	    توڑو;
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	حالت Ctrl_U:
	    // user defined completion
	    ctrl_x_منوال = CTRL_X_FUNCTION;
	    توڑو;
	حالت Ctrl_O:
	    // omni completion
	    ctrl_x_منوال = CTRL_X_OMNI;
	    توڑو;
#اگر_ختم
	حالت 's':
	حالت Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_منوال = CTRL_X_SPELL;
#اگر_تعریف_شدہ FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_عقب_مقصد_badword();
	    --emsg_off;
#اگر_ختم
	    توڑو;
	حالت Ctrl_RSB:
	    // complete tag names
	    ctrl_x_منوال = CTRL_X_لیبل_فہرست;
	    توڑو;
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	حالت Ctrl_I:
	حالت K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_منوال = CTRL_X_راستہ_PATTERNS;
	    توڑو;
	حالت Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_منوال = CTRL_X_راستہ_متعین_کرتا;
	    توڑو;
#اگر_ختم
	حالت Ctrl_V:
	حالت Ctrl_Q:
	    // complete vim commands
	    ctrl_x_منوال = CTRL_X_CMDLINE;
	    توڑو;
	حالت Ctrl_Z:
	    // stop completion
	    ctrl_x_منوال = CTRL_X_معمول;
	    edit_submode = تہی;
	    showmode();
	    retval = TRUE;
	    توڑو;
	حالت Ctrl_P:
	حالت Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    اگر (!(compl_cont_حالت & CONT_INTRPT))
		compl_cont_حالت |= CONT_LOCAL;
	    ورنہ اگر (compl_cont_منوال != 0)
		compl_cont_حالت &= ~CONT_LOCAL;
	    // FALLTHROUGH
	طےشدہ:
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
	    اگر (c == Ctrl_X)
	    {
		اگر (compl_cont_منوال != 0)
		    compl_cont_حالت = 0;
		ورنہ
		    compl_cont_منوال = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_منوال = CTRL_X_معمول;
	    edit_submode = تہی;
	    showmode();
	    توڑو;
    }

    واپس retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    ساکن خالی
trigger_complete_مکمل_event(صحیح منوال غیر_مستعمل, string_T *لفظ غیر_مستعمل)
{
#اگر defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*منوال_str;

    اگر (لفظ == تہی || لفظ->string == تہی)
	(خالی)dict_جوڑو_string_len(v_event, "complete_word", (char_u *)"", 0);
    ورنہ
	(خالی)dict_جوڑو_string_len(v_event, "complete_word", لفظ->string, (صحیح)لفظ->لمبائی);

    منوال_str = &ctrl_x_منوال_نام[منوال & ~CTRL_X_WANT_IDENT];
    (خالی)dict_جوڑو_string_len(v_event, "complete_type",
	(منوال_str->string == تہی) ? (char_u *)"" : منوال_str->string, (صحیح)منوال_str->لمبائی);

    dict_سیٹ_اشیاء_ro(v_event);
#اگر_ختم
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#اگر defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#اگر_ختم
}

/*
 * Stop insert completion mode
 */
    ساکن صحیح
ins_compl_stop(صحیح c, صحیح پچھلا_منوال, صحیح retval)
{
    صحیح		want_cindent;
    string_T	لفظ = {تہی, 0};

    // Remove pre-inserted text when present.
    اگر (ins_compl_preinsert_effect() && ins_compl_win_فعال(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    اگر (compl_curr_مطابقت != تہی || compl_leader.string != تہی || c == Ctrl_E)
    {
	char_u	*ptr = تہی;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	اگر (compl_curr_مطابقت != تہی && compl_مستعمل_مطابقت && c != Ctrl_E)
	    ptr = compl_curr_مطابقت->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    اگر (compl_cont_منوال == CTRL_X_پورا_سطر)
    {
	// re-indent the current line
	اگر (want_cindent)
	{
	    do_c_اظہاریہ_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    ورنہ اگر (!compl_autocomplete || compl_مستعمل_مطابقت)
    {
	صحیح پچھلا_ستون = curwin->w_cursor.ستون;

	// put the cursor on the last char, for 'tw' formatting
	اگر (پچھلا_ستون > 0)
	    dec_cursor();
	// only format when something was inserted
	اگر (!arrow_مستعمل && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	اگر (پچھلا_ستون > 0
		&& ml_get_curline()[curwin->w_cursor.ستون] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    اگر ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	لفظ.string = vim_strnsave(compl_shown_مطابقت->cp_str.string,
	    compl_shown_مطابقت->cp_str.لمبائی);
	اگر (لفظ.string == تہی)
	    لفظ.لمبائی = 0;
	ورنہ
	    لفظ.لمبائی = compl_shown_مطابقت->cp_str.لمبائی;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    اگر (c == Ctrl_E)
    {
	char_u *p = تہی;
	size_t	plen = 0;

	ins_compl_delete();
	اگر (compl_leader.string != تہی)
	{
	    p = compl_leader.string;
	    plen = compl_leader.لمبائی;
	}
	ورنہ اگر (compl_پہلا_مطابقت != تہی)
	{
	    p = compl_orig_متن.string;
	    plen = compl_orig_متن.لمبائی;
	}
	اگر (p != تہی)
	{
	    صحیح	    compl_len = get_compl_len();

	    اگر ((صحیح)plen > compl_len)
		ins_compl_داخل_کرو_بائٹیں(p + compl_len, (صحیح)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_منوال = پچھلا_منوال;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    اگر (!shortmess(SHM_COMPLETIONMENU))
	پیغام_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_منوال = CTRL_X_معمول;
    compl_enter_selects = FALSE;
    اگر (edit_submode != تہی)
    {
	edit_submode = تہی;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_عدد_bests = 0;
    compl_ins_انتہا_ستون = 0;

    اگر (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    اگر (want_cindent && اندر_cinkeys(کلید_COMPLETE, ' ', inindent(0)))
	do_c_اظہاریہ_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_مکمل_event(پچھلا_منوال, &لفظ);
    vim_free(لفظ.string);

    واپس retval;
}

/*
 * Cancel completion.
 */
    صحیح
ins_compl_cancel(خالی)
{
    واپس ins_compl_stop(' ', ctrl_x_منوال, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    صحیح
ins_compl_prep(صحیح c)
{
    صحیح	    retval = FALSE;
    صحیح	    پچھلا_منوال = ctrl_x_منوال;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    اگر (c != Ctrl_R && vim_is_ctrl_x_کلید(c))
	edit_submode_extra = تہی;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    اگر (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	واپس retval;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    اگر (is_mouse_کلید(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	اگر (c == K_LEFTRELEASE
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
	    واپس retval;
	اگر (popup_visible)
	{
	    صحیح	    صف = mouse_صف;
	    صحیح	    ستون = mouse_ستون;
	    win_T   *wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);

	    اگر (wp != تہی && WIN_IS_POPUP(wp))
		واپس retval;
	}
    }
#اگر_ختم

    اگر (ctrl_x_منوال == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	اگر (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_کلید(c)
		|| !vim_is_ctrl_x_کلید(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_منوال = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    اگر (c == Ctrl_Z)
		retval = TRUE;
	}
	ورنہ
	{
	    ctrl_x_منوال = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_منوال = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    اگر (ctrl_x_منوال_not_defined_yet()
			   || (ctrl_x_منوال_معمول() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_مستعمل_مطابقت = TRUE;
    }

    اگر (ctrl_x_منوال_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = سیٹ_ctrl_x_منوال(c);
    ورنہ اگر (ctrl_x_منوال_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	اگر (c != Ctrl_R && !vim_is_ctrl_x_کلید(c))
	{
	    ctrl_x_منوال = ctrl_x_منوال_scroll() ? CTRL_X_معمول : CTRL_X_ختم_شدہ;
	    edit_submode = تہی;
	}
	showmode();
    }

    اگر (compl_started || ctrl_x_منوال == CTRL_X_ختم_شدہ)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	اگر ((ctrl_x_منوال_معمول() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_کلید(c))
		|| ctrl_x_منوال == CTRL_X_ختم_شدہ)
	    retval = ins_compl_stop(c, پچھلا_منوال, retval);
    }
    ورنہ اگر (ctrl_x_منوال == CTRL_X_LOCAL_پیغام)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_مکمل_event(ctrl_x_منوال, تہی);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    اگر (c != Ctrl_R && !vim_is_ctrl_x_کلید(c))
    {
	compl_cont_حالت = 0;
	compl_cont_منوال = 0;
    }

    واپس retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    ساکن خالی
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    صحیح	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    اگر (ptr == تہی)
    {
	اگر (compl_leader.string != تہی)
	    ptr = compl_leader.string;
	ورنہ
	    واپس;  // nothing to do
    }
    اگر (compl_orig_متن.string != تہی)
    {
	p = compl_orig_متن.string;
	// Find length of common prefix between original text and new completion
	جب (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	اگر (len > 0)
	    len -= (*mb_سر_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	برائے (p += len; *p != NUL; MB_PTR_ADV(p))
	    Append_Char_مقصد_Redobuff(K_BS);
    }
    اگر (ptr != تہی)
	Append_مقصد_Redobuff_Lit(ptr + len, -1);
}

/*
 * Loops through the list of windows, loaded-buffers or non-loaded-buffers
 * (depending on flag) starting from buf and looking for a non-scanned
 * buffer (other than curbuf).	curbuf is special, if it is called with
 * buf=curbuf then it has to be the first call for a given flag/expansion.
 *
 * Returns the buffer to scan, if any, otherwise returns curbuf -- Acevedo
 */
    ساکن buf_T *
ins_compl_اگلا_buf(buf_T *buf, صحیح flag)
{
    ساکن win_T    *wp = تہی;
    صحیح		    skip_بفر;

    اگر (flag == 'w')		// just windows
    {
	اگر (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	جب (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_اگلا != تہی) ? wp->w_اگلا : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    اگر (wp == curwin || !wp->w_بفر->b_scanned)
		توڑو;
	}
	buf = wp->w_بفر;
    }
    ورنہ
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	جب (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_اگلا != تہی) ? buf->b_اگلا : firstbuf;
	    // Break if we're back at start buffer
	    اگر (buf == curbuf)
		توڑو;

	    // Check buffer conditions based on flag
	    اگر (flag == 'U')
		skip_بفر = buf->b_p_bl;
	    ورنہ
		skip_بفر = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == تہی) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    اگر (!skip_بفر && !buf->b_scanned)
		توڑو;
	}
    }
    واپس buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    ساکن صحیح
get_cpt_sources_گنتی(char_u *cpt)
{
    char_u  dummy[LSIZE];
    صحیح	    گنتی = 0;
    char_u  *p;

    برائے (p = cpt; *p != NUL; )
    {
	جب (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	اگر (*p != NUL)
	{
	    (خالی)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    گنتی++;
	}
    }

    واپس گنتی;
}

#اگر_تعریف_شدہ FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
ساکن callback_T cfu_cb;	    // 'completefunc' callback function
ساکن callback_T ofu_cb;	    // 'omnifunc' callback function
ساکن callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
ساکن callback_T *cpt_cb;	    // Callback functions associated with F{func}
ساکن صحیح	  cpt_cb_گنتی;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    ساکن خالی
copy_global_مقصد_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    اگر (globcb->cb_name != تہی && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    حرف *
did_سیٹ_completefunc(optset_T *دلائل)
{
    صحیح	retval;

    اگر (دلائل->os_flags & OPT_LOCAL)
	retval = option_سیٹ_callback_func(دلائل->os_newval.string,
							    &curbuf->b_cfu_cb);
    ورنہ
    {
	retval = option_سیٹ_callback_func(دلائل->os_newval.string, &cfu_cb);
	اگر (retval == OK && !(دلائل->os_flags & OPT_GLOBAL))
	    سیٹ_buflocal_cfu_callback(curbuf);
    }

    واپس retval == FAIL ? e_invalid_دلیل : تہی;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    خالی
سیٹ_buflocal_cfu_callback(buf_T *buf غیر_مستعمل)
{
# ifdef FEAT_EVAL
    copy_global_مقصد_buflocal_cb(&cfu_cb, &buf->b_cfu_cb);
# endif
}

/*
 * Parse the 'omnifunc' option value and set the callback function.
 * Invoked when the 'omnifunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    حرف *
did_سیٹ_omnifunc(optset_T *دلائل)
{
    صحیح	retval;

    اگر (دلائل->os_flags & OPT_LOCAL)
	retval = option_سیٹ_callback_func(دلائل->os_newval.string,
							    &curbuf->b_ofu_cb);
    ورنہ
    {
	retval = option_سیٹ_callback_func(دلائل->os_newval.string, &ofu_cb);
	اگر (retval == OK && !(دلائل->os_flags & OPT_GLOBAL))
	    سیٹ_buflocal_ofu_callback(curbuf);
    }

    واپس retval == FAIL ? e_invalid_دلیل : تہی;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    خالی
سیٹ_buflocal_ofu_callback(buf_T *buf غیر_مستعمل)
{
# ifdef FEAT_EVAL
    copy_global_مقصد_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    خالی
clear_cpt_callbacks(callback_T **callbacks, صحیح گنتی)
{
    اگر (callbacks == تہی || *callbacks == تہی)
	واپس;

    برائے (صحیح i = 0; i < گنتی; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    ساکن صحیح
copy_cpt_callbacks(callback_T **dest, صحیح *dest_cnt, callback_T *src, صحیح cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    اگر (cnt == 0)
	واپس OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    اگر (*dest == تہی)
	واپس FAIL;

    *dest_cnt = cnt;

    برائے (صحیح i = 0; i < cnt; i++)
	اگر (src[i].cb_name != تہی && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    واپس OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    خالی
سیٹ_buflocal_cpt_callbacks(buf_T *buf غیر_مستعمل)
{
# ifdef FEAT_EVAL
    اگر (buf == تہی)
	واپس;
    (خالی)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_گنتی, cpt_cb,
	    cpt_cb_گنتی);
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
    ساکن صحیح
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, صحیح *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    صحیح	    idx = 0;
    صحیح	    slen;
    صحیح	    گنتی;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    گنتی = get_cpt_sources_گنتی(cpt);
    اگر (گنتی == 0)
	واپس OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, گنتی);
    اگر (*cbp == تہی)
	واپس FAIL;
    *cnt = گنتی;

    برائے (p = cpt; *p != NUL; )
    {
	جب (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	اگر (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    اگر (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		اگر (caret != تہی)
		    *caret = NUL;

		اگر (option_سیٹ_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = تہی;
	    }
	    idx++;
	}
    }

    واپس OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    صحیح
سیٹ_cpt_callbacks(optset_T *دلائل)
{
    صحیح	    opt_flags = دلائل->os_flags;

    اگر (curbuf == تہی)
	واپس FAIL;

    // ":setglobal" does not change the buffer-local option value
    اگر (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_گنتی) != OK)
	واپس FAIL;

    // ":setlocal" does not change the global option value
    اگر (opt_flags & OPT_LOCAL)
	واپس OK;

    // Cache the callbacks for the global value
    اگر (opt_flags & OPT_GLOBAL)
	واپس parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_گنتی);

    // set case
    واپس copy_cpt_callbacks(&cpt_cb, &cpt_cb_گنتی, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_گنتی);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    حرف *
did_سیٹ_thesaurusfunc(optset_T *دلائل غیر_مستعمل)
{
    صحیح	retval;

    اگر (دلائل->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_سیٹ_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    ورنہ
    {
	// global option set
	retval = option_سیٹ_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	اگر (!(دلائل->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    واپس retval == FAIL ? e_invalid_دلیل : تہی;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    صحیح
سیٹ_ref_اندر_cpt_callbacks(callback_T *callbacks, صحیح گنتی, صحیح copyID)
{
    صحیح	abort = FALSE;

    اگر (callbacks == تہی)
	واپس FALSE;

    برائے (صحیح i = 0; i < گنتی; i++)
	abort = abort || سیٹ_ref_اندر_callback(&callbacks[i], copyID);
    واپس abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    صحیح
سیٹ_ref_اندر_insexpand_funcs(صحیح copyID)
{
    صحیح abort = سیٹ_ref_اندر_callback(&cfu_cb, copyID);
    abort = abort || سیٹ_ref_اندر_callback(&ofu_cb, copyID);
    abort = abort || سیٹ_ref_اندر_callback(&tsrfu_cb, copyID);
    abort = abort || سیٹ_ref_اندر_cpt_callbacks(cpt_cb, cpt_cb_گنتی, copyID);

    واپس abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    ساکن char_u *
get_complete_funcname(صحیح type)
{
    انتخاب (type)
    {
	حالت CTRL_X_FUNCTION:
	    واپس curbuf->b_p_cfu;
	حالت CTRL_X_OMNI:
	    واپس curbuf->b_p_ofu;
	حالت CTRL_X_THESAURUS:
	    واپس *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	طےشدہ:
	    واپس (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    ساکن callback_T *
get_داخل_کرو_callback(صحیح type)
{
    اگر (type == CTRL_X_FUNCTION)
	واپس &curbuf->b_cfu_cb;
    اگر (type == CTRL_X_OMNI)
	واپس &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    واپس (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    ساکن خالی
expand_by_function(صحیح type, char_u *بنیاد, callback_T *cb)
{
    list_T      *matchlist = تہی;
    dict_T	*matchdict = تہی;
    typval_T	دلائل[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    صحیح		save_State = State;
    صحیح		retval;
    صحیح		is_cpt_function = (cb != تہی);
    صحیح		use_sandbox = is_cpt_function
			    && was_سیٹ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    اگر (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	اگر (*funcname == NUL)
	    واپس;
	cb = get_داخل_کرو_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    دلائل[0].v_type = VAR_نمبر;
    دلائل[0].vval.v_نمبر = 0;
    دلائل[1].v_type = VAR_STRING;
    دلائل[1].vval.v_string = بنیاد != تہی ? بنیاد : (char_u *)"";
    دلائل[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    اگر (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, دلائل);
    اگر (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    اگر (retval == OK)
    {
	انتخاب (rettv.v_type)
	{
	    حالت VAR_LIST:
		matchlist = rettv.vval.v_list;
		توڑو;
	    حالت VAR_DICT:
		matchdict = rettv.vval.v_dict;
		توڑو;
	    حالت VAR_SPECIAL:
		اگر (rettv.vval.v_نمبر == VVAL_NONE)
		    compl_opt_suppress_خالی_خانے = TRUE;
		// FALLTHROUGH
	    طےشدہ:
		// TODO: Give error message?
		clear_tv(&rettv);
		توڑو;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    اگر (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_متن));
	جاؤ theend;
    }

    اگر (matchlist != تہی)
	ins_compl_جوڑو_list(matchlist);
    ورنہ اگر (matchdict != تہی)
	ins_compl_جوڑو_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    اگر (matchdict != تہی)
	dict_unref(matchdict);
    اگر (matchlist != تہی)
	list_unref(matchlist);
}
#اگر_ختم // FEAT_COMPL_FUNC

#اگر defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    ساکن اندرونی صحیح
get_user_highlight_id(char_u *hlname)
{
    اگر (hlname != تہی && *hlname != NUL)
	واپس syn_check_گروہ(hlname, (صحیح)STRLEN(hlname));
    واپس 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    ساکن صحیح
ins_compl_جوڑو_tv(typval_T *tv, صحیح dir, صحیح fast)
{
    char_u	*لفظ;
    صحیح		dup = FALSE;
    صحیح		خالی_خانے_2 = FALSE;
    صحیح		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_گنتی]);
    typval_T	user_ڈیٹا;
    صحیح		حالت_2;
    char_u	*user_abbr_hlname;
    char_u	*user_قسم_hlname;
    صحیح		user_hl[2] = { 0, 0 };

    user_ڈیٹا.v_type = VAR_UNKNOWN;
    اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != تہی)
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

	dict_get_tv(tv->vval.v_dict, "user_data", &user_ڈیٹا);
	اگر (dict_get_string(tv->vval.v_dict, "icase", FALSE) != تہی
				  && dict_get_نمبر(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	اگر (dict_get_string(tv->vval.v_dict, "dup", FALSE) != تہی)
	    dup = dict_get_نمبر(tv->vval.v_dict, "dup");
	اگر (dict_get_string(tv->vval.v_dict, "empty", FALSE) != تہی)
	    خالی_خانے_2 = dict_get_نمبر(tv->vval.v_dict, "empty");
	اگر (dict_get_string(tv->vval.v_dict, "equal", FALSE) != تہی
				  && dict_get_نمبر(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    ورنہ
    {
	لفظ = tv_get_string_chk(tv);
	CLEAR_میدان(cptext);
    }
    اگر (لفظ == تہی || (!خالی_خانے_2 && *لفظ == NUL))
    {
	clear_tv(&user_ڈیٹا);
	واپس FAIL;
    }
    حالت_2 = ins_compl_جوڑو(لفظ, -1, تہی, cptext,
	    &user_ڈیٹا, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    اگر (حالت_2 != OK)
	clear_tv(&user_ڈیٹا);
    واپس حالت_2;
}

/*
 * Add completions from a list.
 */
    ساکن خالی
ins_compl_جوڑو_list(list_T *list)
{
    listitem_T	*li;
    صحیح		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_اشیاء(list, li)
    {
	اگر (ins_compl_جوڑو_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	ورنہ اگر (did_emsg)
	    توڑو;
    }
}

/*
 * Add completions from a dict.
 */
    ساکن خالی
ins_compl_جوڑو_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_الفاظ;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_ڈھونڈو(dict, (char_u *)"refresh", 7);
    اگر (di_refresh != تہی && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	اگر (v != تہی && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_الفاظ = dict_ڈھونڈو(dict, (char_u *)"words", 5);
    اگر (di_الفاظ != تہی && di_الفاظ->di_tv.v_type == VAR_LIST)
	ins_compl_جوڑو_list(di_الفاظ->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    ساکن خالی
سیٹ_completion(colnr_T startcol, list_T *list)
{
    صحیح save_w_wrow = curwin->w_wrow;
    صحیح save_w_leftcol = curwin->w_leftcol;
    صحیح flags = CP_ORIGINAL_متن;
    غیرعلامتی صحیح cur_cot_flags = get_cot_flags();
    صحیح compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    صحیح compl_no_داخل_کرو = (cur_cot_flags & COT_NOINSERT) != 0;
    صحیح compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    اگر (compl_started || ctrl_x_منوال_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    اگر (startcol > curwin->w_cursor.ستون)
	startcol = curwin->w_cursor.ستون;
    compl_ستون = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_لمبائی = (صحیح)curwin->w_cursor.ستون - (صحیح)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_متن.string = vim_strnsave(ml_get_curline() + compl_ستون,
							(size_t)compl_لمبائی);
    اگر (p_ic)
	flags |= CP_ICASE;
    اگر (compl_orig_متن.string == تہی)
    {
	compl_orig_متن.لمبائی = 0;
	واپس;
    }
    compl_orig_متن.لمبائی = (size_t)compl_لمبائی;
    اگر (ins_compl_جوڑو(compl_orig_متن.string,
			(صحیح)compl_orig_متن.لمبائی, تہی, تہی, تہی, 0,
			flags | CP_FAST, FALSE, تہی, FUZZY_SCORE_NONE) != OK)
	واپس;

    ctrl_x_منوال = CTRL_X_EVAL;

    ins_compl_جوڑو_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_مستعمل_مطابقت = TRUE;
    compl_cont_حالت = 0;

    compl_curr_مطابقت = compl_پہلا_مطابقت;
    صحیح no_select = compl_no_select || compl_longest;
    اگر (compl_no_داخل_کرو || no_select)
    {
	ins_complete(K_نیچے_کو, FALSE);
	اگر (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_اوپر, FALSE);
    }
    ورنہ
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_داخل_کرو;

    // Lazily show the popup menu, unless we got interrupted.
    اگر (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    برآمد_flush();
}

/*
 * "complete()" function
 */
    خالی
f_complete(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	واپس;

    اگر ((State & منوال_داخل_کرو) == 0)
    {
	emsg(_(e_complete_can_only_be_مستعمل_اندر_داخل_کرو_منوال));
	واپس;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    اگر (!undo_allowed())
	واپس;

    اگر (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	صحیح startcol = (صحیح)tv_get_نمبر_chk(&argvars[0], تہی);
	اگر (startcol > 0)
	    سیٹ_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    خالی
f_complete_جوڑو(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = ins_compl_جوڑو_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    خالی
f_complete_check(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_کلیدیں(0, TRUE);
    rettv->vval.v_نمبر = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    ساکن خالی
ins_compl_منوال(string_T *ret)
{
    اگر (ctrl_x_منوال_not_defined_yet() || ctrl_x_منوال_scroll() || compl_started)
    {
	string_T    *منوال;

	منوال = &ctrl_x_منوال_نام[ctrl_x_منوال & ~CTRL_X_WANT_IDENT];
	ret->string = (منوال->string == تہی) ? (char_u *)"" : منوال->string;
	ret->لمبائی = منوال->لمبائی;
    }
    ورنہ
    {
	ret->string = (char_u *)"";
	ret->لمبائی = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    ساکن خالی
ins_compl_update_sequence_numbers(خالی)
{
    صحیح		نمبر = 0;
    compl_T	*مطابقت;

    اگر (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	برائے (مطابقت = compl_curr_مطابقت->cp_پچھلا; مطابقت != تہی
		&& !is_پہلا_مطابقت(مطابقت); مطابقت = مطابقت->cp_پچھلا)
	    اگر (مطابقت->cp_نمبر != -1)
	    {
		نمبر = مطابقت->cp_نمبر;
		توڑو;
	    }
	اگر (مطابقت != تہی)
	    // go up and assign all numbers which are not assigned yet
	    برائے (مطابقت = مطابقت->cp_اگلا;
		    مطابقت != تہی && مطابقت->cp_نمبر == -1;
					   مطابقت = مطابقت->cp_اگلا)
		مطابقت->cp_نمبر = ++نمبر;
    }
    ورنہ // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	برائے (مطابقت = compl_curr_مطابقت->cp_اگلا; مطابقت != تہی
		&& !is_پہلا_مطابقت(مطابقت); مطابقت = مطابقت->cp_اگلا)
	{
	    اگر (مطابقت->cp_نمبر != -1)
	    {
		نمبر = مطابقت->cp_نمبر;
		توڑو;
	    }
	}
	اگر (مطابقت != تہی)
	{
	    // go down and assign all numbers which are not assigned yet
	    برائے (مطابقت = مطابقت->cp_پچھلا; مطابقت
		    && مطابقت->cp_نمبر == -1;
					   مطابقت = مطابقت->cp_پچھلا)
		مطابقت->cp_نمبر = ++نمبر;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    ساکن خالی
fill_complete_info_dict(dict_T *di, compl_T *مطابقت, صحیح جوڑو_مطابقت)
{
    dict_جوڑو_string_len(di, "word", مطابقت->cp_str.string, (صحیح)مطابقت->cp_str.لمبائی);
    dict_جوڑو_string(di, "abbr", مطابقت->cp_متن[CPT_ABBR]);
    dict_جوڑو_string(di, "menu", مطابقت->cp_متن[CPT_MENU]);
    dict_جوڑو_string(di, "kind", مطابقت->cp_متن[CPT_قسم]);
    dict_جوڑو_string(di, "info", مطابقت->cp_متن[CPT_INFO]);
    dict_جوڑو_string(di, "abbr_hlgroup", syn_id2name(مطابقت->cp_user_abbr_hl_id));
    dict_جوڑو_string(di, "kind_hlgroup", syn_id2name(مطابقت->cp_user_قسم_hl_id));
    اگر (جوڑو_مطابقت)
	dict_جوڑو_bool(di, "match", مطابقت->cp_اندر_مطابقت_array);
    اگر (مطابقت->cp_user_ڈیٹا.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_جوڑو_string_len(di, "user_data", (char_u *)"", 0);
    ورنہ
	dict_جوڑو_tv(di, "user_data", &مطابقت->cp_user_ڈیٹا);
}

/*
 * Get complete information
 */
    ساکن خالی
get_complete_info(list_T *what_list, dict_T *retdict)
{
    صحیح		ret = OK;
    listitem_T	*شے;
# define CI_WHAT_منوال		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_اشیاء		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_متن   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    صحیح		what_flag;

    اگر (what_list == تہی)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    ورنہ
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_اشیاء(what_list, شے)
	{
	    char_u *what = tv_get_string(&شے->li_tv);

	    اگر (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_منوال;
	    ورنہ اگر (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    ورنہ اگر (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_اشیاء;
	    ورنہ اگر (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    ورنہ اگر (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    ورنہ اگر (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_متن;
	    ورنہ اگر (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    ورنہ اگر (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    اگر (ret == OK && (what_flag & CI_WHAT_منوال))
    {
	string_T    منوال;

	ins_compl_منوال(&منوال);
	ret = dict_جوڑو_string_len(retdict, "mode", منوال.string, (صحیح)منوال.لمبائی);
    }

    اگر (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_جوڑو_نمبر(retdict, "pum_visible", pum_visible());

    اگر (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_جوڑو_نمبر(retdict, "auto", compl_autostarted);

    اگر (ret == OK && (what_flag & CI_WHAT_PREINSERTED_متن))
    {
	char_u	*سطر = ml_get_curline();
	صحیح	len = compl_ins_انتہا_ستون - curwin->w_cursor.ستون;

	ret = dict_جوڑو_string_len(retdict, "preinserted_text",
		(len > 0) ? سطر + curwin->w_cursor.ستون : (char_u *)"", (len > 0) ? len : 0);
    }

    اگر (ret == OK && (what_flag & (CI_WHAT_اشیاء | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = تہی;
	dict_T	    *di;
	compl_T     *مطابقت;
	صحیح         selected_idx = -1;
	صحیح	    has_اشیاء = what_flag & CI_WHAT_اشیاء;
	صحیح	    has_matches = what_flag & CI_WHAT_MATCHES;
	صحیح	    has_completed = what_flag & CI_WHAT_COMPLETED;

	اگر (has_اشیاء || has_matches)
	{
	    li = list_alloc();
	    اگر (li == تہی)
		واپس;
	    ret = dict_جوڑو_list(retdict, (has_matches && !has_اشیاء)
						? "matches" : "items", li);
	    اگر (ret == FAIL)
		list_unref(li);
	}
	اگر (ret == OK && what_flag & CI_WHAT_SELECTED)
	    اگر (compl_curr_مطابقت != تہی && compl_curr_مطابقت->cp_نمبر == -1)
		ins_compl_update_sequence_numbers();
	اگر (ret == OK && compl_پہلا_مطابقت != تہی)
	{
	    صحیح list_idx = 0;
	    مطابقت = compl_پہلا_مطابقت;
	    کرو
	    {
		اگر (!مطابقت_at_original_متن(مطابقت))
		{
		    اگر (has_اشیاء
			    || (has_matches && مطابقت->cp_اندر_مطابقت_array))
		    {
			di = dict_alloc();
			اگر (di == تہی)
			    واپس;
			ret = list_append_dict(li, di);
			اگر (ret != OK)
			{
			    dict_unref(di);
			    واپس;
			}
			fill_complete_info_dict(di, مطابقت, has_matches && has_اشیاء);
		    }
		    اگر (compl_curr_مطابقت != تہی
			    && compl_curr_مطابقت->cp_نمبر == مطابقت->cp_نمبر)
			selected_idx = list_idx;
		    اگر (!has_matches || مطابقت->cp_اندر_مطابقت_array)
			list_idx++;
		}
		مطابقت = مطابقت->cp_اگلا;
	    }
	    جب (مطابقت != تہی && !is_پہلا_مطابقت(مطابقت));
	}
	اگر (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_جوڑو_نمبر(retdict, "selected", selected_idx);

	اگر (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    اگر (di == تہی)
		واپس;
	    fill_complete_info_dict(di, compl_curr_مطابقت, FALSE);
	    ret = dict_جوڑو_dict(retdict, "completed", di);
	    اگر (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    خالی
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = تہی;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	اگر (check_for_list_arg(argvars, 0) == FAIL)
	    واپس;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#اگر_ختم

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    ساکن صحیح
thesaurus_func_complete(صحیح type غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    واپس type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#ورنہ
    واپس FALSE;
#اگر_ختم
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    ساکن صحیح
may_advance_cpt_اشاریہ(char_u *cpt)
{
    char_u  *p = cpt;

    اگر (cpt_sources_اشاریہ == -1)
	واپس FALSE;
    جب (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    واپس (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
شمار
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_انتہا
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
قسمتعریف ساخت
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_مطابقت_pos;		// current match position
    pos_T	پچھلا_مطابقت_pos;		// previous match position
    صحیح		سیٹ_مطابقت_pos;		// save first_match_pos/last_match_pos
    pos_T	پہلا_مطابقت_pos;	// first match position
    pos_T	آخری_مطابقت_pos;		// last match position
    صحیح		ملا_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    صحیح		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_اگلا_state_T;

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
    ساکن صحیح
process_اگلا_cpt_قدر(
	ins_compl_اگلا_state_T *st,
	صحیح		*compl_type_arg,
	pos_T		*آغاز_مطابقت_pos,
	صحیح		fuzzy_collect,
	صحیح		*advance_cpt_idx)
{
    صحیح	    compl_type = -1;
    صحیح	    حالت_2 = INS_COMPL_CPT_OK;
    صحیح	    skip_ماخذ = compl_autocomplete && compl_from_nonkeyword;

    st->ملا_all = FALSE;
    *advance_cpt_idx = FALSE;

    جب (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    اگر (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_ماخذ
	    && !compl_وقت_قاش_expired)
    {
	st->ins_buf = curbuf;
	st->پہلا_مطابقت_pos = *آغاز_مطابقت_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	اگر (ctrl_x_منوال_معمول() && (!fuzzy_collect && dec(&st->پہلا_مطابقت_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->پہلا_مطابقت_pos.lnum = st->ins_buf->b_ml.ml_سطر_گنتی;
	    st->پہلا_مطابقت_pos.ستون = ml_get_len(st->پہلا_مطابقت_pos.lnum);
	}
	st->آخری_مطابقت_pos = st->پہلا_مطابقت_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->سیٹ_مطابقت_pos = TRUE;
    }
    ورنہ اگر (!skip_ماخذ && !compl_وقت_قاش_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != تہی
	    && (st->ins_buf = ins_compl_اگلا_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	اگر (st->ins_buf->b_ml.ml_mfp != تہی)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->پہلا_مطابقت_pos.ستون = st->آخری_مطابقت_pos.ستون = 0;
	    st->پہلا_مطابقت_pos.lnum = st->ins_buf->b_ml.ml_سطر_گنتی + 1;
	    st->آخری_مطابقت_pos.lnum = 0;
	    compl_type = 0;
	}
	ورنہ	// unloaded buffer, scan like dictionary
	{
	    st->ملا_all = TRUE;
	    اگر (st->ins_buf->b_fname == تہی)
	    {
		حالت_2 = INS_COMPL_CPT_CONT;
		جاؤ مکمل;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	اگر (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    پیغام_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((حرف *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == تہی
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == تہی
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (خالی)پیغام_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    ورنہ اگر (*st->e_cpt == NUL)
	حالت_2 = INS_COMPL_CPT_انتہا;
    ورنہ
    {
	اگر (ctrl_x_منوال_سطر_or_eval())
	    compl_type = -1;
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	ورنہ اگر (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_اشاریہ);
	    اگر (!st->func_cb)
		compl_type = -1;
	}
#اگر_ختم
	ورنہ اگر (!skip_ماخذ)
	{
	    اگر (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		اگر (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		ورنہ
		    compl_type = CTRL_X_THESAURUS;
		اگر (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_پہلا;
		}
	    }
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	    ورنہ اگر (*st->e_cpt == 'i')
		compl_type = CTRL_X_راستہ_PATTERNS;
	    ورنہ اگر (*st->e_cpt == 'd')
		compl_type = CTRL_X_راستہ_متعین_کرتا;
#اگر_ختم
	    ورنہ اگر (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_لیبل_فہرست;
		اگر (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    پیغام_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((حرف *)IObuff, IOSIZE, _("Scanning tags."));
		    (خالی)پیغام_trunc_attr((حرف *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    ورنہ
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(خالی)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_اشاریہ(st->e_cpt);

	st->ملا_all = TRUE;
	اگر (compl_type == -1)
	    حالت_2 = INS_COMPL_CPT_CONT;
    }

مکمل:
    *compl_type_arg = compl_type;
    واپس حالت_2;
}

#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    ساکن خالی
get_اگلا_include_file_completion(صحیح compl_type)
{
    ڈھونڈو_نمونہ_اندر_راستہ(compl_نمونہ.string, compl_direction,
	    (صحیح)compl_نمونہ.لمبائی, FALSE, FALSE,
	    (compl_type == CTRL_X_راستہ_متعین_کرتا
	     && !(compl_cont_حالت & CONT_SOL))
	    ? ڈھونڈو_DEFINE : ڈھونڈو_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM, FALSE, compl_autocomplete);
}
#اگر_ختم

/*
 * Get the next set of words matching "compl_pattern" in dictionary or
 * thesaurus files.
 */
    ساکن خالی
get_اگلا_dict_tsr_completion(صحیح compl_type, char_u *dict, صحیح dict_f)
{
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    اگر (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_نمونہ.string, تہی);
    ورنہ
#اگر_ختم
	ins_compl_dictionaries(
		dict != تہی ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_نمونہ.string,
		dict != تہی ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    ساکن خالی
get_اگلا_لیبل_completion(خالی)
{
    صحیح		save_p_ic;
    char_u	**matches;
    صحیح		عدد_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_نمونہ.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_لیبل_at_cursor = TRUE;
    اگر (ڈھونڈو_لیبل_فہرست(compl_نمونہ.string, &عدد_matches, &matches,
		لیبل_REGEXP | لیبل_نام | لیبل_NOIC | لیبل_INS_COMP
		| (ctrl_x_منوال_not_default() ? لیبل_VERBOSE : 0),
		لیبل_MANY, curbuf->b_ffname) == OK && عدد_matches > 0)
	ins_compl_جوڑو_matches(عدد_matches, matches, p_ic);
    g_لیبل_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    ساکن صحیح
compare_scores(ثابت خالی *a, ثابت خالی *b)
{
    صحیح idx_a = *(ثابت صحیح *)a;
    صحیح idx_b = *(ثابت صحیح *)b;
    صحیح score_a = compl_fuzzy_scores[idx_a];
    صحیح score_b = compl_fuzzy_scores[idx_b];
    واپس score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    ساکن خالی
ins_compl_longest_داخل_کرو(char_u *سابقہ)
{
    ins_compl_delete();
    ins_compl_داخل_کرو_بائٹیں(سابقہ + get_compl_len(), -1);
    compl_longest_انتہا_ستون = curwin->w_cursor.ستون;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    ساکن خالی
fuzzy_longest_مطابقت(خالی)
{
    char_u	*سابقہ = تہی;
    صحیح		سابقہ_len = 0;
    صحیح		i = 0;
    صحیح		j = 0;
    char_u	*مطابقت_str = تہی;
    char_u	*سابقہ_ptr = تہی;
    char_u	*مطابقت_ptr = تہی;
    char_u	*leader = تہی;
    size_t	leader_len = 0;
    compl_T	*compl = تہی;
    صحیح		مزید_candidates = FALSE;
    compl_T	*nn_compl = تہی;

    اگر (compl_عدد_bests == 0)
	واپس;

    nn_compl = compl_پہلا_مطابقت->cp_اگلا->cp_اگلا;
    اگر (nn_compl && nn_compl != compl_پہلا_مطابقت)
	مزید_candidates = TRUE;

    compl = ctrl_x_منوال_پورا_سطر() ? compl_پہلا_مطابقت
				    : compl_پہلا_مطابقت->cp_اگلا;
    اگر (compl_عدد_bests == 1)
    {
	// no more candidates insert the match str
	اگر (!مزید_candidates)
	{
	    ins_compl_longest_داخل_کرو(compl->cp_str.string);
	    compl_عدد_bests = 0;
	}
	compl_عدد_bests = 0;
	واپس;
    }

    اگر ((size_t)compl_عدد_bests > سائز_MAX / حجم(compl_T *))
	واپس;
    compl_بہترین_matches = (compl_T **)alloc(compl_عدد_bests * حجم(compl_T *));
    اگر (compl_بہترین_matches == تہی)
	واپس;
    جب (compl != تہی && i < compl_عدد_bests)
    {
	compl_بہترین_matches[i] = compl;
	compl = compl->cp_اگلا;
	i++;
    }

    سابقہ = compl_بہترین_matches[0]->cp_str.string;
    سابقہ_len = (صحیح)compl_بہترین_matches[0]->cp_str.لمبائی;

    برائے (i = 1; i < compl_عدد_bests; i++)
    {
	مطابقت_str = compl_بہترین_matches[i]->cp_str.string;
	سابقہ_ptr = سابقہ;
	مطابقت_ptr = مطابقت_str;
	j = 0;

	جب (j < سابقہ_len && *مطابقت_ptr != NUL && *سابقہ_ptr != NUL)
	{
	    اگر (STRNCMP(سابقہ_ptr, مطابقت_ptr, mb_ptr2len(سابقہ_ptr)) != 0)
		توڑو;

	    MB_PTR_ADV(سابقہ_ptr);
	    MB_PTR_ADV(مطابقت_ptr);
	    j++;
	}

	اگر (j > 0)
	    سابقہ_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    اگر (leader_len > 0 && STRNCMP(سابقہ, leader, leader_len) != 0)
	جاؤ انتہا;

    سابقہ = vim_strnsave(سابقہ, سابقہ_len);
    اگر (سابقہ != تہی)
    {
	ins_compl_longest_داخل_کرو(سابقہ);
	vim_free(سابقہ);
    }

انتہا:
    vim_free(compl_بہترین_matches);
    compl_بہترین_matches = تہی;
    compl_عدد_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    ساکن خالی
get_اگلا_filename_completion(خالی)
{
    char_u	**matches;
    صحیح		عدد_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    صحیح		i;
    صحیح		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    صحیح		اندر_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    صحیح		*fuzzy_indices_ڈیٹا;
    char_u	*آخری_sep = تہی;
    صحیح		need_collect_bests = اندر_fuzzy_collect && compl_get_longest;
    صحیح		max_score = 0;
    صحیح		current_score = 0;
    صحیح		dir = compl_direction;

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    حرف pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#ورنہ
    حرف pathsep = PATHSEP;
#اگر_ختم

    اگر (اندر_fuzzy_collect)
    {
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	اگر (curbuf->b_p_csl[0] == 's')
	{
	    برائے (i = 0; i < (صحیح)leader_len; i++)
	    {
		اگر (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	ورنہ اگر (curbuf->b_p_csl[0] == 'b')
	{
	    برائے (i = 0; i < (صحیح)leader_len; i++)
	    {
		اگر (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#اگر_ختم
	آخری_sep = vim_strrchr(leader, pathsep);
	اگر (آخری_sep == تہی)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_نمونہ);
	    compl_نمونہ.string = vim_strnsave((char_u *)"*", 1);
	    اگر (compl_نمونہ.string == تہی)
		واپس;
	    compl_نمونہ.لمبائی = 1;
	}
	ورنہ اگر (*(آخری_sep + 1) == '\0')
	    اندر_fuzzy_collect = FALSE;
	ورنہ
	{
	    // Split leader into path and file parts
	    صحیح راستہ_len = آخری_sep - leader + 1;
	    char_u  *راستہ_with_wildcard;

	    راستہ_with_wildcard = alloc(راستہ_len + 2);
	    اگر (راستہ_with_wildcard != تہی)
	    {
		vim_snprintf((حرف *)راستہ_with_wildcard, راستہ_len + 2, "%*.*s*", راستہ_len, راستہ_len, leader);
		VIM_CLEAR_STRING(compl_نمونہ);
		compl_نمونہ.string = راستہ_with_wildcard;
		compl_نمونہ.لمبائی = راستہ_len + 1;

		// Move leader to the file part
		leader = آخری_sep + 1;
		leader_len -= راستہ_len;
	    }
	}
    }

    اگر (expand_wildcards(1, &compl_نمونہ.string, &عدد_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	واپس;

    // May change home directory back to "~".
    tilde_replace(compl_نمونہ.string, عدد_matches, matches);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    اگر (curbuf->b_p_csl[0] != NUL)
    {
	برائے (i = 0; i < عدد_matches; ++i)
	{
	    ptr = matches[i];
	    جب (*ptr != NUL)
	    {
		اگر (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ورنہ اگر (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#اگر_ختم

    اگر (اندر_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, حجم(صحیح), 10);
	compl_fuzzy_scores = (صحیح *)alloc(حجم(صحیح) * عدد_matches);

	برائے (i = 0; i < عدد_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_مطابقت_str(ptr, leader);
	    اگر (score != FUZZY_SCORE_NONE)
	    {
		اگر (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((صحیح *)fuzzy_indices.ga_ڈیٹا)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	اگر (fuzzy_indices.ga_len > 0)
	{
	    char_u	*مطابقت = تہی;
	    fuzzy_indices_ڈیٹا = (صحیح *)fuzzy_indices.ga_ڈیٹا;
	    qsort(fuzzy_indices_ڈیٹا, fuzzy_indices.ga_len, حجم(صحیح), compare_scores);

	    برائے (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		مطابقت = matches[fuzzy_indices_ڈیٹا[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_ڈیٹا[i]];
		اگر (ins_compl_جوڑو(مطابقت, -1, تہی, تہی, تہی, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, تہی, current_score) == OK)
		    dir = FORWARD;

		اگر (need_collect_bests)
		{
		    اگر (i == 0 || current_score == max_score)
		    {
			compl_عدد_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(عدد_matches, matches);
	}
	ورنہ اگر (leader_len > 0)
	{
	    FreeWild(عدد_matches, matches);
	    عدد_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	اگر (compl_عدد_bests > 0 && compl_get_longest)
	    fuzzy_longest_مطابقت();
	واپس;
    }

    اگر (عدد_matches > 0)
	ins_compl_جوڑو_matches(عدد_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    ساکن خالی
get_اگلا_cmdline_completion(خالی)
{
    char_u	**matches;
    صحیح		عدد_matches;

    اگر (expand_cmdline(&compl_xp, compl_نمونہ.string,
		(صحیح)compl_نمونہ.لمبائی, &عدد_matches, &matches) == EXPAND_OK)
    {
	صحیح		جوڑو_r = OK;
	صحیح		dir = compl_direction;

	برائے (صحیح i = 0; i < عدد_matches && جوڑو_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_گنتی]) = {تہی, تہی, تہی, تہی};

	    اگر (compl_xp.xp_files_abbr != تہی)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    اگر (compl_xp.xp_files_قسم != تہی)
		cptext[CPT_قسم] = compl_xp.xp_files_قسم[i];
	    اگر (compl_xp.xp_files_menu != تہی)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    اگر (compl_xp.xp_files_info != تہی)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    جوڑو_r = ins_compl_جوڑو(matches[i], -1, تہی, cptext, تہی, dir,
		    CP_FAST, FALSE, تہی, FUZZY_SCORE_NONE);
	    اگر (جوڑو_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(عدد_matches, matches);
	free_xp_files_extra(&compl_xp, عدد_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    ساکن خالی
get_اگلا_spell_completion(linenr_T lnum غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_SPELL
    char_u	**matches;
    صحیح		عدد_matches;

    عدد_matches = expand_spelling(lnum, compl_نمونہ.string, &matches);
    اگر (عدد_matches > 0)
	ins_compl_جوڑو_matches(عدد_matches, matches, p_ic);
    ورنہ
	vim_free(matches);
#اگر_ختم
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    ساکن char_u *
ins_compl_get_اگلا_لفظ_or_سطر(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_مطابقت_pos,		// current match position
	صحیح	*مطابقت_len,
	صحیح	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    صحیح		len;

    *مطابقت_len = 0;
    ptr = ml_get_buf(ins_buf, cur_مطابقت_pos->lnum, FALSE) +
	cur_مطابقت_pos->ستون;
    len = (صحیح)ml_get_buf_len(ins_buf, cur_مطابقت_pos->lnum) - cur_مطابقت_pos->ستون;
    اگر (ctrl_x_منوال_سطر_or_eval())
    {
	اگر (compl_حالت_adding())
	{
	    اگر (cur_مطابقت_pos->lnum >= ins_buf->b_ml.ml_سطر_گنتی)
		واپس تہی;
	    ptr = ml_get_buf(ins_buf, cur_مطابقت_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_مطابقت_pos->lnum + 1);
	    اگر (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (صحیح)(ptr - tmp_ptr);
	    }
	}
    }
    ورنہ
    {
	char_u	*tmp_ptr = ptr;

	اگر (compl_حالت_adding() && compl_لمبائی <= len)
	{
	    tmp_ptr += compl_لمبائی;
	    // Skip if already inside a word.
	    اگر (vim_iswordp(tmp_ptr))
		واپس تہی;
	    // Find start of next word.
	    tmp_ptr = ڈھونڈو_لفظ_آغاز(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = ڈھونڈو_لفظ_انتہا(tmp_ptr);
	len = (صحیح)(tmp_ptr - ptr);

	اگر (compl_حالت_adding() && len == compl_لمبائی)
	{
	    اگر (cur_مطابقت_pos->lnum < ins_buf->b_ml.ml_سطر_گنتی)
	    {
		// Try next line, if any. the new word will be
		// "join" as if the normal command "J" was used.
		// IOSIZE is always greater than
		// compl_length, so the next STRNCPY always
		// works -- Acevedo
		STRNCPY(IObuff, ptr, len);
		ptr = ml_get_buf(ins_buf, cur_مطابقت_pos->lnum + 1, FALSE);
		tmp_ptr = ptr = skipwhite(ptr);
		// Find start of next word.
		tmp_ptr = ڈھونڈو_لفظ_آغاز(tmp_ptr);
		// Find end of next word.
		tmp_ptr = ڈھونڈو_لفظ_انتہا(tmp_ptr);
		اگر (tmp_ptr > ptr)
		{
		    اگر (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			اگر (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			اگر (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== تہی
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    اگر (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (صحیح)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    اگر (len == compl_لمبائی)
		واپس تہی;
	}
    }

    *مطابقت_len = len;
    واپس ptr;
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
    ساکن صحیح
get_اگلا_default_completion(ins_compl_اگلا_state_T *st, pos_T *آغاز_pos)
{
    صحیح		ملا_new_مطابقت = FAIL;
    صحیح		save_p_scs;
    صحیح		save_p_ws;
    صحیح		looped_around = FALSE;
    char_u	*ptr = تہی;
    صحیح		len = 0;
    صحیح		اندر_fuzzy_collect = !compl_حالت_adding()
			&& cot_fuzzy() && compl_لمبائی > 0;
    char_u	*leader = ins_compl_leader();
    صحیح		score = FUZZY_SCORE_NONE;
    صحیح		اندر_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    اگر (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    اگر (!اندر_curbuf)
	p_ws = FALSE;
    ورنہ اگر (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    برائے (;;)
    {
	صحیح	cont_s_ipos = FALSE;

	++پیغام_silent;  // Don't want messages for wrapscan.

	اگر (اندر_fuzzy_collect)
	{
	    ملا_new_مطابقت = تلاش_for_fuzzy_مطابقت(st->ins_buf,
			    st->cur_مطابقت_pos, leader, compl_direction,
			    آغاز_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	ورنہ اگر (ctrl_x_منوال_پورا_سطر() || ctrl_x_منوال_eval() || (compl_cont_حالت & CONT_SOL))
	    ملا_new_مطابقت = تلاش_for_exact_سطر(st->ins_buf,
			    st->cur_مطابقت_pos, compl_direction, compl_نمونہ.string);
	ورنہ
	    ملا_new_مطابقت = searchit(تہی, st->ins_buf, st->cur_مطابقت_pos,
				تہی, compl_direction, compl_نمونہ.string, (صحیح)compl_نمونہ.لمبائی,
				1L, تلاش_KEEP + تلاش_NFMSG, RE_آخری, تہی);
	--پیغام_silent;
	اگر (!compl_started || st->سیٹ_مطابقت_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->پہلا_مطابقت_pos = *st->cur_مطابقت_pos;
	    st->آخری_مطابقت_pos = *st->cur_مطابقت_pos;
	    st->سیٹ_مطابقت_pos = FALSE;
	}
	ورنہ اگر (st->پہلا_مطابقت_pos.lnum == st->آخری_مطابقت_pos.lnum
		&& st->پہلا_مطابقت_pos.ستون == st->آخری_مطابقت_pos.ستون)
	{
	    ملا_new_مطابقت = FAIL;
	}
	ورنہ اگر (compl_dir_forward()
		&& (st->پچھلا_مطابقت_pos.lnum > st->cur_مطابقت_pos->lnum
		    || (st->پچھلا_مطابقت_pos.lnum == st->cur_مطابقت_pos->lnum
			&& st->پچھلا_مطابقت_pos.ستون >= st->cur_مطابقت_pos->ستون)))
	{
	    اگر (looped_around)
		ملا_new_مطابقت = FAIL;
	    ورنہ
		looped_around = TRUE;
	}
	ورنہ اگر (!compl_dir_forward()
		&& (st->پچھلا_مطابقت_pos.lnum < st->cur_مطابقت_pos->lnum
		    || (st->پچھلا_مطابقت_pos.lnum == st->cur_مطابقت_pos->lnum
			&& st->پچھلا_مطابقت_pos.ستون <= st->cur_مطابقت_pos->ستون)))
	{
	    اگر (looped_around)
		ملا_new_مطابقت = FAIL;
	    ورنہ
		looped_around = TRUE;
	}
	st->پچھلا_مطابقت_pos = *st->cur_مطابقت_pos;
	اگر (ملا_new_مطابقت == FAIL)
	    توڑو;

	// when ADDING, the text before the cursor matches, skip it
	اگر (compl_حالت_adding() && اندر_curbuf
		&& آغاز_pos->lnum == st->cur_مطابقت_pos->lnum
		&& آغاز_pos->ستون  == st->cur_مطابقت_pos->ستون)
	    جاری;

	اگر (!اندر_fuzzy_collect)
	    ptr = ins_compl_get_اگلا_لفظ_or_سطر(st->ins_buf,
		    st->cur_مطابقت_pos, &len, &cont_s_ipos);
	اگر (ptr == تہی || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    جاری;

	اگر (is_قریب_ترین_فعال() && اندر_curbuf)
	{
	    score = st->cur_مطابقت_pos->lnum - curwin->w_cursor.lnum;
	    اگر (score < 0)
		score = -score;
	}

	اگر (ins_compl_جوڑو_infercase(ptr, len, p_ic,
			اندر_curbuf ? تہی : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    اگر (اندر_fuzzy_collect && score == compl_پہلا_مطابقت->cp_اگلا->cp_score)
		compl_عدد_bests++;
	    ملا_new_مطابقت = OK;
	    توڑو;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    واپس ملا_new_مطابقت;
}

#اگر_تعریف_شدہ FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    ساکن callback_T *
get_callback_if_cpt_func(char_u *p, صحیح idx)
{
    اگر (*p == 'o')
	واپس &curbuf->b_ofu_cb;

    اگر (*p == 'F')
    {
	اگر (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    اگر (curbuf->b_p_cpt_cb == تہی
		    || idx < 0 || idx >= curbuf->b_p_cpt_گنتی)
		واپس تہی;
	    واپس curbuf->b_p_cpt_cb[idx].cb_name != تہی
		? &curbuf->b_p_cpt_cb[idx] : تہی;
	}
	ورنہ
	    واپس &curbuf->b_cfu_cb; // 'cfu'
    }

    واپس تہی;
}
#اگر_ختم

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    ساکن خالی
get_رجسٹر_completion(خالی)
{
    صحیح		dir = compl_direction;
    yankreg_T	*reg = تہی;
    خالی	*reg_ptr = تہی;
    صحیح		adding_منوال = compl_حالت_adding();

    برائے (صحیح i = 0; i < عدد_REGISTERS; i++)
    {
	صحیح regname = 0;
	اگر (i == 0)
	    regname = '"';    // unnamed register
	ورنہ اگر (i < 10)
	    regname = '0' + i;
	ورنہ اگر (i == DELETION_رجسٹر)
	    regname = '-';
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	ورنہ اگر (i == ستارہ_رجسٹر)
	    regname = '*';
	ورنہ اگر (i == PLUS_رجسٹر)
	    regname = '+';
#اگر_ختم
	ورنہ
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	اگر (!valid_yank_reg(regname, FALSE) || regname == '_')
	    جاری;

	reg_ptr = get_رجسٹر(regname, FALSE);
	اگر (reg_ptr == تہی)
	    جاری;

	reg = (yankreg_T *)reg_ptr;

	برائے (صحیح j = 0; j < reg->y_سائز; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    اگر (str == تہی)
		جاری;

	    اگر (adding_منوال)
	    {
		صحیح str_len = (صحیح)STRLEN(str);
		اگر (str_len == 0)
		    جاری;

		اگر (!compl_orig_متن.string
			|| (p_ic ? STRNICMP(str, compl_orig_متن.string,
					    compl_orig_متن.لمبائی) == 0
				: STRNCMP(str, compl_orig_متن.string,
					    compl_orig_متن.لمبائی) == 0))
		{
		    اگر (ins_compl_جوڑو_infercase(str, str_len, p_ic, تہی,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    ورنہ
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_انتہا = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		جب (p < str_انتہا && *p != NUL)
		{
		    char_u *old_p = p;
		    p = ڈھونڈو_لفظ_آغاز(p);
		    اگر (p >= str_انتہا || *p == NUL)
			توڑو;

		    char_u *لفظ_انتہا = ڈھونڈو_لفظ_انتہا(p);

		    اگر (لفظ_انتہا <= p)
		    {
			اگر (has_mbyte)
			    لفظ_انتہا = p + (*mb_ptr2len)(p);
			ورنہ
			    لفظ_انتہا = p + 1;
		    }

		    اگر (لفظ_انتہا > str_انتہا)
			لفظ_انتہا = str_انتہا;

		    صحیح len = (صحیح)(لفظ_انتہا - p);
		    اگر (len > 0 && (!compl_orig_متن.string
				|| (p_ic ? STRNICMP(p, compl_orig_متن.string,
						    compl_orig_متن.لمبائی) == 0
					: STRNCMP(p, compl_orig_متن.string,
						    compl_orig_متن.لمبائی) == 0)))
		    {
			اگر (ins_compl_جوڑو_infercase(p, len, p_ic, تہی,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = لفظ_انتہا;

		    اگر (p <= old_p)
		    {
			p = old_p + 1;
			اگر (has_mbyte && p < str_انتہا)
			    p = old_p + (*mb_ptr2len)(old_p);
		    }
		}
	    }
	}

	// Free the register copy
	put_رجسٹر(regname, reg_ptr);
    }
}

/*
 * get the next set of completion matches for "type".
 * Returns TRUE if a new match is found. Otherwise returns FALSE.
 */
    ساکن صحیح
get_اگلا_completion_مطابقت(صحیح type, ins_compl_اگلا_state_T *st, pos_T *ini)
{
    صحیح	ملا_new_مطابقت = FALSE;

    انتخاب (type)
    {
	حالت -1:
	    توڑو;
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	حالت CTRL_X_راستہ_PATTERNS:
	حالت CTRL_X_راستہ_متعین_کرتا:
	    get_اگلا_include_file_completion(type);
	    توڑو;
#اگر_ختم

	حالت CTRL_X_DICTIONARY:
	حالت CTRL_X_THESAURUS:
	    get_اگلا_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = تہی;
	    توڑو;

	حالت CTRL_X_لیبل_فہرست:
	    get_اگلا_لیبل_completion();
	    توڑو;

	حالت CTRL_X_FILES:
	    get_اگلا_filename_completion();
	    توڑو;

	حالت CTRL_X_CMDLINE:
	حالت CTRL_X_CMDLINE_CTRL_X:
	    get_اگلا_cmdline_completion();
	    توڑو;

#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	حالت CTRL_X_FUNCTION:
	    اگر (ctrl_x_منوال_معمول())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    ورنہ
		expand_by_function(type, compl_نمونہ.string, تہی);
	    توڑو;
	حالت CTRL_X_OMNI:
	    expand_by_function(type, compl_نمونہ.string, تہی);
	    توڑو;
#اگر_ختم

	حالت CTRL_X_SPELL:
	    get_اگلا_spell_completion(st->پہلا_مطابقت_pos.lnum);
	    توڑو;

	حالت CTRL_X_رجسٹر:
	    get_رجسٹر_completion();
	    توڑو;

	طےشدہ:	// normal ^P/^N and ^X^L
	    ملا_new_مطابقت = get_اگلا_default_completion(st, ini);
	    اگر (ملا_new_مطابقت == FAIL && st->ins_buf == curbuf)
		st->ملا_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    اگر (type != 0 && compl_curr_مطابقت != compl_old_مطابقت)
	ملا_new_مطابقت = OK;

    واپس ملا_new_مطابقت;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    ساکن خالی
strip_caret_numbers_اندر_رکھو(char_u *str)
{
    char_u  *اقرأ = str, *اكتب = str, *p;

    اگر (str == تہی)
	واپس;

    جب (*اقرأ)
    {
	اگر (*اقرأ == '^')
	{
	    p = اقرأ + 1;
	    جب (vim_isdigit(*p))
		p++;
	    اگر ((*p == ',' || *p == '\0') && p != اقرأ + 1)
	    {
		اقرأ = p;
		جاری;
	    }
	    ورنہ
		*اكتب++ = *اقرأ++;
	}
	ورنہ
	    *اكتب++ = *اقرأ++;
    }
    *اكتب = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    ساکن صحیح
prepare_cpt_compl_funcs(خالی)
{
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = تہی;
    صحیح		idx = 0;
    صحیح		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    اگر (cpt == تہی)
	واپس FAIL;
    strip_caret_numbers_اندر_رکھو(cpt);

    برائے (p = cpt; *p;)
    {
	جب (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	اگر (*p == NUL)
	    توڑو;

	cb = get_callback_if_cpt_func(p, idx);
	اگر (cb)
	{
	    اگر (get_userdefined_compl_info(curwin->w_cursor.ستون, cb, &startcol)
		    == FAIL)
	    {
		اگر (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		ورنہ
		    startcol = -2;
	    }
	    ورنہ اگر (startcol < 0 || startcol > curwin->w_cursor.ستون)
		startcol = curwin->w_cursor.ستون;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	ورنہ
	    cpt_sources_array[idx].cs_startcol = -3;

	(خالی)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    واپس OK;
#اگر_ختم
    واپس FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    ساکن خالی
compl_ماخذ_آغاز_timer(صحیح ماخذ_idx غیر_مستعمل)
{
#اگر_تعریف_شدہ ELAPSED_FUNC
    اگر (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[ماخذ_idx].compl_آغاز_tv);
	compl_وقت_قاش_expired = FALSE;
    }
#اگر_ختم
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    ساکن صحیح
advance_cpt_sources_اشاریہ_محفوظ(خالی)
{
    اگر (cpt_sources_اشاریہ >= 0 && cpt_sources_اشاریہ < cpt_sources_گنتی - 1)
    {
	cpt_sources_اشاریہ++;
	واپس OK;
    }
#اگر_تعریف_شدہ FEAT_EVAL
    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), cpt_sources_اشاریہ);
#اگر_ختم
    واپس FAIL;
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
    ساکن صحیح
ins_compl_get_exp(pos_T *ini)
{
    ساکن ins_compl_اگلا_state_T   st;
    ساکن صحیح			    st_cleared = FALSE;
    صحیح		مطابقت_گنتی;
    صحیح		ملا_new_مطابقت;
    صحیح		type = ctrl_x_منوال;
    صحیح		may_advance_cpt_idx = FALSE;
    pos_T	آغاز_pos = *ini;
    صحیح		معمول_منوال_strict = FALSE;
    طویل	compl_timeout_save = 0;

    اگر (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	اگر (!st_cleared)
	{
	    CLEAR_میدان(st);
	    st_cleared = TRUE;
	}
	st.ملا_all = FALSE;
	st.ins_buf = curbuf;
	vim_free(st.e_cpt_copy);
	// Make a copy of 'complete', in case the buffer is wiped out.
	st.e_cpt_copy = vim_strsave((compl_cont_حالت & CONT_LOCAL)
					    ? (char_u *)"." : curbuf->b_p_cpt);
	strip_caret_numbers_اندر_رکھو(st.e_cpt_copy);
	st.e_cpt = st.e_cpt_copy == تہی ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#تعریف_کرو LOOKBACK_سطر_گنتی	1000
	اگر (compl_autocomplete && is_قریب_ترین_فعال())
	{
	    آغاز_pos.lnum = MAX(1, آغاز_pos.lnum - LOOKBACK_سطر_گنتی);
	    آغاز_pos.ستون = 0;
	}
	st.آخری_مطابقت_pos = st.پہلا_مطابقت_pos = آغاز_pos;
    }
    ورنہ اگر (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_مطابقت = compl_curr_مطابقت;	// remember the last current match
    st.cur_مطابقت_pos = (compl_dir_forward())
				    ? &st.آخری_مطابقت_pos : &st.پہلا_مطابقت_pos;

    معمول_منوال_strict = ctrl_x_منوال_معمول() && !ctrl_x_منوال_سطر_or_eval()
	&& !(compl_cont_حالت & CONT_LOCAL) && cpt_sources_array != تہی;
    اگر (معمول_منوال_strict)
    {
	cpt_sources_اشاریہ = 0;
	اگر (compl_autocomplete || p_cto > 0)
	{
	    compl_ماخذ_آغاز_timer(0);
	    compl_وقت_قاش_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    برائے (;;)
    {
	ملا_new_مطابقت = FAIL;
	st.سیٹ_مطابقت_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	اگر ((ctrl_x_منوال_معمول() || ctrl_x_منوال_سطر_or_eval())
					&& (!compl_started || st.ملا_all))
	{
	    صحیح حالت_2 = process_اگلا_cpt_قدر(&st, &type, &آغاز_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    اگر (حالت_2 == INS_COMPL_CPT_انتہا)
		توڑو;
	    اگر (حالت_2 == INS_COMPL_CPT_CONT)
	    {
		اگر (may_advance_cpt_idx)
		{
		    اگر (!advance_cpt_sources_اشاریہ_محفوظ())
			توڑو;
		    compl_ماخذ_آغاز_timer(cpt_sources_اشاریہ);
		}
		جاری;
	    }
	}

	اگر (معمول_منوال_strict && type == CTRL_X_FUNCTION
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
	ملا_new_مطابقت = get_اگلا_completion_مطابقت(type, &st, &آغاز_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	اگر (compl_نمونہ.string == تہی)
	    توڑو;

	اگر (may_advance_cpt_idx)
	{
	    اگر (!advance_cpt_sources_اشاریہ_محفوظ())
		توڑو;
	    compl_ماخذ_آغاز_timer(cpt_sources_اشاریہ);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	//مطابقت
	اگر ((ctrl_x_منوال_not_default() && !ctrl_x_منوال_سطر_or_eval())
						|| ملا_new_مطابقت != FAIL)
	{
	    اگر (got_int)
		توڑو;
	    // Fill the popup menu as soon as possible.
	    اگر (type != -1)
		ins_compl_check_کلیدیں(0, FALSE);

	    اگر ((ctrl_x_منوال_not_default()
			&& !ctrl_x_منوال_سطر_or_eval()) || compl_interrupted)
		توڑو;
	    compl_started = compl_وقت_قاش_expired ? FALSE : TRUE;
	}
	ورنہ
	{
	    // Mark a buffer scanned when it has been scanned completely
	    اگر (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_راستہ_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	اگر (معمول_منوال_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	اگر (!compl_dir_forward())
	    جب (compl_curr_مطابقت->cp_پچھلا
		    && !مطابقت_at_original_متن(compl_curr_مطابقت->cp_پچھلا))
		compl_curr_مطابقت = compl_curr_مطابقت->cp_پچھلا;
    }
    cpt_sources_اشاریہ = -1;
    compl_started = TRUE;

    اگر ((ctrl_x_منوال_معمول() || ctrl_x_منوال_سطر_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	ملا_new_مطابقت = FAIL;

    مطابقت_گنتی = -1;		// total of matches, unknown
    اگر (ملا_new_مطابقت == FAIL || (ctrl_x_منوال_not_default()
		&& !ctrl_x_منوال_سطر_or_eval()))
	مطابقت_گنتی = ins_compl_make_cyclic();

    اگر (cot_fuzzy() && compl_get_longest && compl_عدد_bests > 0)
	fuzzy_longest_مطابقت();

    اگر (compl_old_مطابقت != تہی)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_مطابقت = compl_dir_forward() ? compl_old_مطابقت->cp_اگلا
						    : compl_old_مطابقت->cp_پچھلا;
	اگر (compl_curr_مطابقت == تہی)
	    compl_curr_مطابقت = compl_old_مطابقت;
    }
    may_trigger_modechanged();

    اگر (مطابقت_گنتی > 0 && !ctrl_x_منوال_spell())
    {
	اگر (is_قریب_ترین_فعال() && !ins_compl_has_preinsert())
	    ترتیب_دو_compl_مطابقت_list(cp_compare_قریب_ترین);

	اگر (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_ترتیب_دو();
    }

    واپس مطابقت_گنتی;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    ساکن خالی
ins_compl_update_shown_مطابقت(خالی)
{
    string_T	*leader;

    (خالی)get_leader_for_startcol(تہی, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_مطابقت, TRUE);

    جب (!ins_compl_equal(compl_shown_مطابقت,
		leader->string, (صحیح)leader->لمبائی)
	    && compl_shown_مطابقت->cp_اگلا != تہی
	    && !is_پہلا_مطابقت(compl_shown_مطابقت->cp_اگلا))
    {
	compl_shown_مطابقت = compl_shown_مطابقت->cp_اگلا;
	leader = get_leader_for_startcol(compl_shown_مطابقت, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    اگر (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_مطابقت,
		leader->string, (صحیح)leader->لمبائی)
	    && (compl_shown_مطابقت->cp_اگلا == تہی
		|| is_پہلا_مطابقت(compl_shown_مطابقت->cp_اگلا)))
    {
	جب (!ins_compl_equal(compl_shown_مطابقت,
		    leader->string, (صحیح)leader->لمبائی)
		&& compl_shown_مطابقت->cp_پچھلا != تہی
		&& !is_پہلا_مطابقت(compl_shown_مطابقت->cp_پچھلا))
	{
	    compl_shown_مطابقت = compl_shown_مطابقت->cp_پچھلا;
	    leader = get_leader_for_startcol(compl_shown_مطابقت, TRUE);
	}
    }
}

/*
 * Delete the old text being completed.
 */
    خالی
ins_compl_delete(خالی)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    صحیح ستون = compl_ستون + (compl_حالت_adding() ? compl_لمبائی : 0);
    string_T	بقایا = {تہی, 0};
    صحیح	    orig_ستون;

    اگر (ins_compl_preinsert_effect())
    {
	ستون += (صحیح)ins_compl_leader_len();
	curwin->w_cursor.ستون = compl_ins_انتہا_ستون;
    }

    اگر (curwin->w_cursor.lnum > compl_lnum)
    {
	اگر (curwin->w_cursor.ستون < ml_get_curline_len())
	{
	    char_u *سطر = ml_get_cursor();
	    بقایا.لمبائی = ml_get_cursor_len();
	    بقایا.string = vim_strnsave(سطر, بقایا.لمبائی);
	    اگر (بقایا.string == تہی)
		واپس;
	}
	جب (curwin->w_cursor.lnum > compl_lnum)
	{
	    اگر (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		اگر (بقایا.string)
		    vim_free(بقایا.string);
		واپس;
	    }
	    deleted_سطریں_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.ستون = ml_get_curline_len();
    }

    اگر ((صحیح)curwin->w_cursor.ستون > ستون)
    {
	اگر (stop_arrow() == FAIL)
	{
	    اگر (بقایا.string)
		vim_free(بقایا.string);
	    واپس;
	}
	backspace_until_عمود(ستون);
	compl_ins_انتہا_ستون = curwin->w_cursor.ستون;
    }

    اگر (بقایا.string != تہی)
    {
	orig_ستون = curwin->w_cursor.ستون;
	ins_str(بقایا.string, بقایا.لمبائی);
	curwin->w_cursor.ستون = orig_ستون;
	vim_free(بقایا.string);
    }
    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    بدلا_cline_bef_curs();
#اگر_تعریف_شدہ FEAT_EVAL
    // clear v:completed_item
    سیٹ_vim_var_dict(VV_COMPLETED_شے, dict_alloc_تالا(VAR_FIXED));
#اگر_ختم
}

/*
 * Insert a completion string that contains newlines.
 * The string is split and inserted line by line.
 */
    ساکن خالی
ins_compl_expand_مضرب(char_u *str)
{
    char_u	*آغاز = str;
    char_u	*curr = str;
    صحیح		بنیاد_indent = get_indent();

    جب (*curr != NUL)
    {
	اگر (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    اگر (curr > آغاز)
		ins_char_بائٹیں(آغاز, (صحیح)(curr - آغاز));

	    // Handle newline
	    open_سطر(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, بنیاد_indent, تہی);
	    آغاز = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    اگر (curr > آغاز)
	ins_char_بائٹیں(آغاز, (صحیح)(curr - آغاز));

    compl_ins_انتہا_ستون = curwin->w_cursor.ستون;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    ساکن char_u *
ڈھونڈو_common_سابقہ(size_t *سابقہ_len, صحیح curbuf_only)
{
    compl_T     *compl;
    صحیح		*مطابقت_گنتی;
    char_u	*پہلا = تہی;
    صحیح		len = -1;
    صحیح		is_cpt_completion = (cpt_sources_array != تہی);

    اگر (!is_cpt_completion)
	واپس تہی;

    مطابقت_گنتی = ALLOC_CLEAR_MULT(صحیح, cpt_sources_گنتی);
    اگر (مطابقت_گنتی == تہی)
	واپس تہی;

    (خالی)get_leader_for_startcol(تہی, TRUE); // Clear the cache

    compl = compl_پہلا_مطابقت;
    کرو
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	اگر (ctrl_x_منوال_معمول() && !p_inf && compl_orig_متن.string
		&& !ignorecase(compl_orig_متن.string))
	    compl->cp_flags &= ~CP_ICASE;

	اگر (!مطابقت_at_original_متن(compl)
		&& (leader->string == تہی
		    || ins_compl_equal_sc(compl, leader->string,
			(صحیح)leader->لمبائی)))
	{
	    // Limit number of items from each source if max_items is set.
	    صحیح مطابقت_حد_بندی_exceeded = FALSE;
	    صحیح cur_ماخذ = compl->cp_cpt_ماخذ_idx;

	    اگر (cur_ماخذ != -1)
	    {
		مطابقت_گنتی[cur_ماخذ]++;
		صحیح max_matches = cpt_sources_array[cur_ماخذ].cs_max_matches;
		اگر (max_matches > 0 && مطابقت_گنتی[cur_ماخذ] > max_matches)
		    مطابقت_حد_بندی_exceeded = TRUE;
	    }

	    اگر (!مطابقت_حد_بندی_exceeded && (!curbuf_only
			|| (cur_ماخذ != -1
			    && cpt_sources_array[cur_ماخذ].cs_flag == '.')))
	    {
		اگر (پہلا == تہی && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    پہلا = compl->cp_str.string;
		    len = (صحیح)STRLEN(پہلا);
		}
		ورنہ اگر (پہلا != تہی)
		{
		    صحیح j = 0;  // count in bytes
		    char_u *s1 = پہلا;
		    char_u *s2 = compl->cp_str.string;

		    جب (j < len && *s1 != NUL && *s2 != NUL)
		    {
			اگر (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    توڑو;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    اگر (len == 0)
			توڑو;
		}
	    }
	}
	compl = compl->cp_اگلا;
    } جب (compl != تہی && !is_پہلا_مطابقت(compl));

    vim_free(مطابقت_گنتی);

    اگر (len > (صحیح)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	اگر (len == (صحیح)STRLEN(پہلا))
	{
	    char_u *سطر = ml_get_curline();
	    char_u *p = سطر + curwin->w_cursor.ستون;
	    اگر (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *انتہا = ڈھونڈو_لفظ_انتہا(p);
		صحیح متن_len = انتہا - p;
		اگر (متن_len > 0
			&& متن_len < (len - (صحیح)ins_compl_leader_len())
			&& STRNCMP(پہلا + len - متن_len, p, متن_len) == 0)
		    len -= متن_len;
	    }
	}
	*سابقہ_len = (size_t)len;
	واپس پہلا;
    }
    واپس تہی;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    خالی
ins_compl_داخل_کرو(صحیح move_cursor, صحیح داخل_کرو_سابقہ)
{
    صحیح		compl_len = get_compl_len();
    صحیح		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_مطابقت->cp_str.string;
    size_t	cp_str_len = compl_shown_مطابقت->cp_str.لمبائی;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_مضرب = vim_strchr(cp_str, '\n');

    اگر (داخل_کرو_سابقہ)
    {
	cp_str = ڈھونڈو_common_سابقہ(&cp_str_len, FALSE);
	اگر (cp_str == تہی)
	{
	    cp_str = ڈھونڈو_common_سابقہ(&cp_str_len, TRUE);
	    اگر (cp_str == تہی)
	    {
		cp_str = compl_shown_مطابقت->cp_str.string;
		cp_str_len = compl_shown_مطابقت->cp_str.لمبائی;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    ورنہ اگر (cpt_sources_array != تہی)
    {
	صحیح	cpt_idx = compl_shown_مطابقت->cp_cpt_ماخذ_idx;
	اگر (cpt_idx >= 0 && compl_ستون >= 0)
	{
	    صحیح	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    اگر (startcol >= 0 && startcol < (صحیح)compl_ستون)
	    {
		صحیح skip = (صحیح)compl_ستون - startcol;
		اگر ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    اگر (compl_len < (صحیح)cp_str_len)
    {
	اگر (has_مضرب)
	    ins_compl_expand_مضرب(cp_str + compl_len);
	ورنہ
	{
	    ins_compl_داخل_کرو_بائٹیں(cp_str + compl_len,
		    داخل_کرو_سابقہ ? (صحیح)cp_str_len - compl_len : -1);
	    اگر ((preinsert || داخل_کرو_سابقہ) && move_cursor)
		curwin->w_cursor.ستون -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    اگر (مطابقت_at_original_متن(compl_shown_مطابقت)
	    || (preinsert && !داخل_کرو_سابقہ))
	compl_مستعمل_مطابقت = FALSE;
    ورنہ
	compl_مستعمل_مطابقت = TRUE;
#اگر_تعریف_شدہ FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_مطابقت);

	سیٹ_vim_var_dict(VV_COMPLETED_شے, dict);
    }
#اگر_ختم
    compl_hi_on_autocompl_longest = داخل_کرو_سابقہ && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    ساکن خالی
ins_compl_show_filename(خالی)
{
    حرف	*lead = _("match in file");
    صحیح		space = sc_ستون - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    اگر (space <= 0)
	واپس;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    برائے (s = e = compl_shown_مطابقت->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	جب (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    اگر (!compl_autocomplete)
    {
	پیغام_hist_off = TRUE;
	vim_snprintf((حرف *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_مطابقت->cp_fname ? "<" : "", s);
	پیغام((حرف *)IObuff);
	پیغام_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    ساکن compl_T *
ڈھونڈو_اگلا_مطابقت_اندر_menu(خالی)
{
    صحیح	    is_forward = compl_shows_dir_forward();
    compl_T *مطابقت = compl_shown_مطابقت;

    کرو
	مطابقت = is_forward ? مطابقت->cp_اگلا : مطابقت->cp_پچھلا;
    جب (مطابقت->cp_اگلا && !مطابقت->cp_اندر_مطابقت_array
	    && !مطابقت_at_original_متن(مطابقت));
    واپس مطابقت;
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
    ساکن صحیح
ڈھونڈو_اگلا_completion_مطابقت(
	صحیح	allow_get_expansion,
	صحیح	todo,		// repeat completion this many times
	صحیح	advance,
	صحیح	*عدد_matches)
{
    صحیح		ملا_انتہا = FALSE;
    compl_T	*ملا_compl = تہی;
    غیرعلامتی صحیح cur_cot_flags = get_cot_flags();
    صحیح		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    جب (--todo >= 0)
    {
	اگر (compl_shows_dir_forward() && compl_shown_مطابقت->cp_اگلا != تہی)
	{
	    اگر (compl_مطابقت_array != تہی)
		compl_shown_مطابقت = ڈھونڈو_اگلا_مطابقت_اندر_menu();
	    ورنہ
		compl_shown_مطابقت = compl_shown_مطابقت->cp_اگلا;
	    ملا_انتہا = (compl_پہلا_مطابقت != تہی
		    && (is_پہلا_مطابقت(compl_shown_مطابقت->cp_اگلا)
			|| is_پہلا_مطابقت(compl_shown_مطابقت)));
	}
	ورنہ اگر (compl_shows_dir_backward()
		&& compl_shown_مطابقت->cp_پچھلا != تہی)
	{
	    ملا_انتہا = is_پہلا_مطابقت(compl_shown_مطابقت);
	    اگر (compl_مطابقت_array != تہی)
		compl_shown_مطابقت = ڈھونڈو_اگلا_مطابقت_اندر_menu();
	    ورنہ
		compl_shown_مطابقت = compl_shown_مطابقت->cp_پچھلا;
	    ملا_انتہا |= is_پہلا_مطابقت(compl_shown_مطابقت);
	}
	ورنہ
	{
	    اگر (!allow_get_expansion)
	    {
		اگر (advance)
		{
		    اگر (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    ورنہ
			compl_pending += todo + 1;
		}
		واپس -1;
	    }

	    اگر (!compl_no_select && advance)
	    {
		اگر (compl_shows_dir_backward())
		    --compl_pending;
		ورنہ
		    ++compl_pending;
	    }

	    // Find matches.
	    *عدد_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    جب (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		اگر (compl_pending > 0 && compl_shown_مطابقت->cp_اگلا != تہی)
		{
		    compl_shown_مطابقت = compl_shown_مطابقت->cp_اگلا;
		    --compl_pending;
		}
		ورنہ اگر (compl_pending < 0 && compl_shown_مطابقت->cp_پچھلا != تہی)
		{
		    compl_shown_مطابقت = compl_shown_مطابقت->cp_پچھلا;
		    ++compl_pending;
		}
		ورنہ
		    توڑو;
	    }
	    ملا_انتہا = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_مطابقت, FALSE);

	اگر (!مطابقت_at_original_متن(compl_shown_مطابقت)
		&& leader->string != تہی
		&& !ins_compl_equal(compl_shown_مطابقت,
		    leader->string, (صحیح)leader->لمبائی)
		&& !(cot_fuzzy()
		    && compl_shown_مطابقت->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	ورنہ
	    // Remember a matching item.
	    ملا_compl = compl_shown_مطابقت;

	// Stop at the end of the list when we found a usable match.
	اگر (ملا_انتہا)
	{
	    اگر (ملا_compl != تہی)
	    {
		compl_shown_مطابقت = ملا_compl;
		توڑو;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    واپس OK;
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
    ساکن صحیح
ins_compl_اگلا(
    صحیح	    allow_get_expansion,
    صحیح	    گنتی,		// repeat completion this many times; should
				// be at least 1
    صحیح	    داخل_کرو_مطابقت)	// Insert the newly selected match
{
    صحیح	    عدد_matches = -1;
    صحیح	    todo = گنتی;
    صحیح	    advance;
    صحیح	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    غیرعلامتی صحیح cur_cot_flags = get_cot_flags();
    صحیح	    compl_no_داخل_کرو = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    صحیح	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    اگر (compl_shown_مطابقت == تہی)
	واپس -1;

    اگر (compl_leader.string != تہی
	    && !مطابقت_at_original_متن(compl_shown_مطابقت)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_مطابقت();

    اگر (allow_get_expansion && داخل_کرو_مطابقت
	    && (!compl_get_longest || compl_مستعمل_مطابقت))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = گنتی != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    اگر (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    اگر (ڈھونڈو_اگلا_completion_مطابقت(allow_get_expansion, todo, advance,
							&عدد_matches) == -1)
	واپس -1;

    اگر (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	واپس -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    اگر (!started && ins_compl_preinsert_longest())
	ins_compl_داخل_کرو(TRUE, TRUE);
    ورنہ اگر (compl_no_داخل_کرو && !started && !compl_preinsert)
    {
	ins_compl_داخل_کرو_بائٹیں(compl_orig_متن.string + get_compl_len(), -1);
	compl_مستعمل_مطابقت = FALSE;
    }
    ورنہ اگر (داخل_کرو_مطابقت)
    {
	اگر (!compl_get_longest || compl_مستعمل_مطابقت)
	{
	    صحیح preinsert_longest = ins_compl_preinsert_longest()
		&& مطابقت_at_original_متن(compl_shown_مطابقت); // none selected
	    ins_compl_داخل_کرو(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	ورنہ
	    ins_compl_داخل_کرو_بائٹیں(compl_leader.string + get_compl_len(), -1);
    }
    ورنہ
	compl_مستعمل_مطابقت = FALSE;

    اگر (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	اگر (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	ورنہ
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    برآمد_flush_cursor(FALSE, FALSE);
	}
#اگر_ختم

	// Delete old text to be replaced, since we're still searching and
	// don't want to match ourselves!
	ins_compl_delete();
    }

    // Enter will select a match when the match wasn't inserted and the popup
    // menu is visible.
    اگر (compl_no_داخل_کرو && !started
				 && !مطابقت_at_original_متن(compl_shown_مطابقت))
	compl_enter_selects = TRUE;
    ورنہ
	compl_enter_selects = !داخل_کرو_مطابقت && compl_مطابقت_array != تہی;

    // Show the file name for the match (if any)
    اگر (compl_shown_مطابقت->cp_fname != تہی)
	ins_compl_show_filename();

    واپس عدد_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    ساکن خالی
check_elapsed_وقت(خالی)
{
#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	*آغاز_tv
	= &cpt_sources_array[cpt_sources_اشاریہ].compl_آغاز_tv;
    طویل	elapsed_ms = ELAPSED_FUNC(*آغاز_tv);

    اگر (elapsed_ms > compl_timeout_ms)
    {
	compl_وقت_قاش_expired = TRUE;
	DECAY_COMPL_TIMEOUT();
    }
#اگر_ختم
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
    خالی
ins_compl_check_کلیدیں(صحیح frequency, صحیح اندر_compl_func)
{
    ساکن صحیح	گنتی = 0;
    صحیح		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    اگر (!اندر_compl_func && (using_script() || ex_معمول_busy))
	واپس;

    // Only do this at regular intervals
    اگر (++گنتی < frequency)
	واپس;
    گنتی = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    اگر (c != NUL
#اگر_تعریف_شدہ FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#اگر_ختم
       )
    {
	اگر (vim_is_ctrl_x_کلید(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = محفوظ_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (خالی)ins_compl_اگلا(FALSE, ins_compl_key2count(c),
				      c != K_اوپر && c != K_نیچے_کو);
	}
	ورنہ
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = محفوظ_vgetc();
	    اگر (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		اگر (c != Ctrl_R && (کلید_Typed
			    || (اندر_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    ورنہ
    {
	صحیح معمول_منوال_strict = ctrl_x_منوال_معمول()
	    && !ctrl_x_منوال_سطر_or_eval() && !(compl_cont_حالت & CONT_LOCAL)
	    && cpt_sources_array != تہی && cpt_sources_اشاریہ >= 0;
	اگر (معمول_منوال_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_وقت();
    }

    اگر (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	صحیح todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(خالی)ins_compl_اگلا(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    ساکن صحیح
ins_compl_key2dir(صحیح c)
{
    اگر (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_اوپر || c == K_اوپر)
	واپس BACKWARD;
    واپس FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    ساکن صحیح
ins_compl_pum_کلید(صحیح c)
{
    واپس pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_اوپر
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_نیچے_کو
		     || c == K_اوپر || c == K_نیچے_کو);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    ساکن صحیح
ins_compl_key2count(صحیح c)
{
    اگر (ins_compl_pum_کلید(c) && c != K_اوپر && c != K_نیچے_کو)
    {
	صحیح h = pum_get_اونچائی();
	اگر (h > 3)
	    h -= 2; // keep some context
	واپس h;
    }
    واپس 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    ساکن صحیح
ins_compl_use_مطابقت(صحیح c)
{
    انتخاب (c)
    {
	حالت K_اوپر:
	حالت K_نیچے_کو:
	حالت K_PAGEDOWN:
	حالت K_KPAGEDOWN:
	حالت K_S_نیچے_کو:
	حالت K_PAGEUP:
	حالت K_KPAGEUP:
	حالت K_S_اوپر:
	    واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    ساکن صحیح
get_معمول_compl_info(char_u *سطر, صحیح startcol, colnr_T curs_ستون)
{
    اگر ((compl_cont_حالت & CONT_SOL) || ctrl_x_منوال_راستہ_متعین_کرتا())
    {
	اگر (!compl_حالت_adding())
	{
	    جب (--startcol >= 0 && vim_isIDc(سطر[startcol]))
		;
	    compl_ستون += ++startcol;
	    compl_لمبائی = curs_ستون - startcol;
	}
	اگر (p_ic)
	{
	    compl_نمونہ.string = str_foldcase(سطر + compl_ستون,
		    compl_لمبائی, تہی, 0);
	    اگر (compl_نمونہ.string == تہی)
	    {
		compl_نمونہ.لمبائی = 0;
		واپس FAIL;
	    }
	    compl_نمونہ.لمبائی = STRLEN(compl_نمونہ.string);
	}
	ورنہ
	{
	    compl_نمونہ.string = vim_strnsave(سطر + compl_ستون, (size_t)compl_لمبائی);
	    اگر (compl_نمونہ.string == تہی)
	    {
		compl_نمونہ.لمبائی = 0;
		واپس FAIL;
	    }
	    compl_نمونہ.لمبائی = (size_t)compl_لمبائی;
	}
    }
    ورنہ اگر (compl_حالت_adding())
    {
	char_u	    *سابقہ = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	اگر (!vim_iswordp(سطر + compl_ستون)
		|| (compl_ستون > 0
		    && (vim_iswordp(mb_prevptr(سطر, سطر + compl_ستون)))))
	{
	    سابقہ = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(تہی, سطر + compl_ستون, compl_لمبائی) + prefixlen;
	compl_نمونہ.string = alloc(n);
	اگر (compl_نمونہ.string == تہی)
	{
	    compl_نمونہ.لمبائی = 0;
	    واپس FAIL;
	}
	STRCPY((حرف *)compl_نمونہ.string, سابقہ);
	(خالی)quote_meta(compl_نمونہ.string + prefixlen,
		سطر + compl_ستون, compl_لمبائی);
	compl_نمونہ.لمبائی = n - 1;
    }
    ورنہ اگر (--startcol < 0
	    || !vim_iswordp(mb_prevptr(سطر, سطر + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_نمونہ.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	اگر (compl_نمونہ.string == تہی)
	{
	    compl_نمونہ.لمبائی = 0;
	    واپس FAIL;
	}
	compl_نمونہ.لمبائی = len;
	compl_ستون += curs_ستون;
	compl_لمبائی = 0;
	compl_from_nonkeyword = TRUE;
    }
    ورنہ
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	اگر (has_mbyte)
	{
	    صحیح بنیاد_class;
	    صحیح سر_off;

	    startcol -= (*mb_سر_off)(سطر, سطر + startcol);
	    بنیاد_class = mb_get_class(سطر + startcol);
	    جب (--startcol >= 0)
	    {
		سر_off = (*mb_سر_off)(سطر, سطر + startcol);
		اگر (بنیاد_class != mb_get_class(سطر + startcol
			    - سر_off))
		    توڑو;
		startcol -= سر_off;
	    }
	}
	ورنہ
	{
	    جب (--startcol >= 0 && vim_iswordc(سطر[startcol]))
		;
	}

	compl_ستون += ++startcol;
	compl_لمبائی = (صحیح)curs_ستون - startcol;
	اگر (compl_لمبائی == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_نمونہ.string = alloc(7);
	    اگر (compl_نمونہ.string == تہی)
	    {
		compl_نمونہ.لمبائی = 0;
		واپس FAIL;
	    }
	    STRCPY((حرف *)compl_نمونہ.string, "\\<");
	    (خالی)quote_meta(compl_نمونہ.string + 2, سطر + compl_ستون, 1);
	    STRCAT((حرف *)compl_نمونہ.string, "\\k");
	    compl_نمونہ.لمبائی = STRLEN(compl_نمونہ.string);
	}
	ورنہ
	{
	    size_t  n = quote_meta(تہی, سطر + compl_ستون, compl_لمبائی) + 2;

	    compl_نمونہ.string = alloc(n);
	    اگر (compl_نمونہ.string == تہی)
	    {
		compl_نمونہ.لمبائی = 0;
		واپس FAIL;
	    }
	    STRCPY((حرف *)compl_نمونہ.string, "\\<");
	    (خالی)quote_meta(compl_نمونہ.string + 2, سطر + compl_ستون,
		    compl_لمبائی);
	    compl_نمونہ.لمبائی = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    اگر (ctrl_x_منوال_معمول() && !(compl_cont_حالت & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	اگر (تیاری_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ساکن صحیح
get_wholeline_compl_info(char_u *سطر, colnr_T curs_ستون)
{
    compl_ستون = (colnr_T)getwhitecols(سطر);
    compl_لمبائی = (صحیح)curs_ستون - (صحیح)compl_ستون;
    اگر (compl_لمبائی < 0)	// cursor in indent: empty pattern
	compl_لمبائی = 0;
    اگر (p_ic)
    {
	compl_نمونہ.string = str_foldcase(سطر + compl_ستون, compl_لمبائی,
		تہی, 0);
	اگر (compl_نمونہ.string == تہی)
	{
	    compl_نمونہ.لمبائی = 0;
	    واپس FAIL;
	}
	compl_نمونہ.لمبائی = STRLEN(compl_نمونہ.string);
    }
    ورنہ
    {
	compl_نمونہ.string = vim_strnsave(سطر + compl_ستون, (size_t)compl_لمبائی);
	اگر (compl_نمونہ.string == تہی)
	{
	    compl_نمونہ.لمبائی = 0;
	    واپس FAIL;
	}
	compl_نمونہ.لمبائی = (size_t)compl_لمبائی;
    }

    واپس OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ساکن صحیح
get_filename_compl_info(char_u *سطر, صحیح startcol, colnr_T curs_ستون)
{
    // Go back to just before the first filename character.
    اگر (startcol > 0)
    {
	char_u	*p = سطر + startcol;

	MB_PTR_عقب(سطر, p);
	جب (p > سطر && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_عقب(سطر, p);
	اگر (p == سطر && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	ورنہ
	    startcol = (صحیح)(p - سطر) + 1;
    }

    compl_ستون += startcol;
    compl_لمبائی = (صحیح)curs_ستون - startcol;
    compl_نمونہ.string = addstar(سطر + compl_ستون, compl_لمبائی, EXPAND_FILES);
    اگر (compl_نمونہ.string == تہی)
    {
	compl_نمونہ.لمبائی = 0;
	واپس FAIL;
    }

    compl_نمونہ.لمبائی = STRLEN(compl_نمونہ.string);

    واپس OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ساکن صحیح
get_cmdline_compl_info(char_u *سطر, colnr_T curs_ستون)
{
    compl_نمونہ.string = vim_strnsave(سطر, curs_ستون);
    اگر (compl_نمونہ.string == تہی)
    {
	compl_نمونہ.لمبائی = 0;
	واپس FAIL;
    }
    compl_نمونہ.لمبائی = curs_ستون;
    سیٹ_cmd_context(&compl_xp, compl_نمونہ.string,
	    (صحیح)compl_نمونہ.لمبائی, curs_ستون, FALSE);
    اگر (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_ستون = curs_ستون;
    ورنہ
	compl_ستون = (صحیح)(compl_xp.xp_نمونہ - compl_نمونہ.string);
    compl_لمبائی = curs_ستون - compl_ستون;

    واپس OK;
}

#اگر_تعریف_شدہ FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    ساکن صحیح
سیٹ_compl_globals(
    صحیح		startcol غیر_مستعمل,
    colnr_T	curs_ستون غیر_مستعمل,
    صحیح		is_cpt_compl غیر_مستعمل)
{
    اگر (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_نمونہ);
	اگر (startcol < compl_ستون)
	    واپس prepend_startcol_متن(&cpt_compl_نمونہ, &compl_orig_متن,
		    startcol);
	ورنہ
	{
	    cpt_compl_نمونہ.string = vim_strnsave(compl_orig_متن.string,
		    compl_orig_متن.لمبائی);
	    cpt_compl_نمونہ.لمبائی = compl_orig_متن.لمبائی;
	}
    }
    ورنہ
    {
	اگر (startcol < 0 || startcol > curs_ستون)
	    startcol = curs_ستون;

	// Re-obtain line in case it has changed
	char_u	*سطر = ml_get(curwin->w_cursor.lnum);
	صحیح	len = curs_ستون - startcol;

	compl_نمونہ.string = vim_strnsave(سطر + startcol, (size_t)len);
	اگر (compl_نمونہ.string == تہی)
	{
	    compl_نمونہ.لمبائی = 0;
	    واپس FAIL;
	}
	compl_نمونہ.لمبائی = (size_t)len;
	compl_ستون = startcol;
	compl_لمبائی = len;
    }

    واپس OK;
}
#اگر_ختم

/*
 * Get the pattern, column and length for user defined completion ('omnifunc',
 * 'completefunc' and 'thesaurusfunc')
 * Uses the global variable: spell_bad_len
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 * "startcol", when not NULL, contains the column returned by function.
 */
    ساکن صحیح
get_userdefined_compl_info(
    colnr_T	curs_ستون غیر_مستعمل,
    callback_T	*cb غیر_مستعمل,
    صحیح		*startcol غیر_مستعمل)
{
    صحیح		ret = FAIL;

#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	دلائل[3];
    صحیح		ستون;
    char_u	*funcname = تہی;
    pos_T	pos;
    صحیح		save_State = State;
    صحیح		is_cpt_function = (cb != تہی);
    صحیح		use_sandbox = is_cpt_function
			    && was_سیٹ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    اگر (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_منوال);
	اگر (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_سیٹ), ctrl_x_منوال_function()
		    ? "completefunc" : "omnifunc");
	    واپس FAIL;
	}
	cb = get_داخل_کرو_callback(ctrl_x_منوال);
    }

    دلائل[0].v_type = VAR_نمبر;
    دلائل[0].vval.v_نمبر = 1;
    دلائل[1].v_type = VAR_STRING;
    دلائل[1].vval.v_string = (char_u *)"";
    دلائل[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    اگر (use_sandbox)
	++sandbox;
    ستون = call_callback_retnr(cb, 2, دلائل);
    اگر (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    اگر (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_متن));
	واپس FAIL;
    }

    اگر (startcol != تہی)
	*startcol = ستون;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    اگر (ستون == -2 || aborting())
	واپس FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    اگر (ستون == -3)
    {
	اگر (is_cpt_function)
	    واپس FAIL;
	ctrl_x_منوال = CTRL_X_معمول;
	edit_submode = تہی;
	اگر (!shortmess(SHM_COMPLETIONMENU))
	    پیغام_clr_cmdline();
	واپس FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_خالی_خانے = FALSE;

    ret = !is_cpt_function ? سیٹ_compl_globals(ستون, curs_ستون, FALSE) : OK;
#اگر_ختم

    واپس ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    ساکن صحیح
get_spell_compl_info(صحیح startcol غیر_مستعمل, colnr_T curs_ستون غیر_مستعمل)
{
    صحیح		ret = FAIL;
#اگر_تعریف_شدہ FEAT_SPELL
    char_u	*سطر = تہی;

    اگر (spell_bad_len > 0)
	compl_ستون = curs_ستون - spell_bad_len;
    ورنہ
	compl_ستون = spell_لفظ_آغاز(startcol);

    اگر (compl_ستون >= (colnr_T)startcol)
    {
	compl_لمبائی = 0;
	compl_ستون = curs_ستون;
    }
    ورنہ
    {
	spell_expand_check_cap(compl_ستون);
	compl_لمبائی = (صحیح)curs_ستون - compl_ستون;
    }
    // Need to obtain "line" again, it may have become invalid.
    سطر = ml_get(curwin->w_cursor.lnum);
    compl_نمونہ.string = vim_strnsave(سطر + compl_ستون, (size_t)compl_لمبائی);
    اگر (compl_نمونہ.string == تہی)
    {
	compl_نمونہ.لمبائی = 0;
	واپس FAIL;
    }

    compl_نمونہ.لمبائی = (size_t)compl_لمبائی;
    ret = OK;
#اگر_ختم

    واپس ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    ساکن صحیح
compl_get_info(char_u *سطر, صحیح startcol, colnr_T curs_ستون, صحیح *سطر_invalid)
{
    اگر (ctrl_x_منوال_معمول() || ctrl_x_منوال_رجسٹر()
	    || (ctrl_x_منوال & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_منوال)))
    {
	اگر (get_معمول_compl_info(سطر, startcol, curs_ستون) != OK)
	    واپس FAIL;
	*سطر_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    ورنہ اگر (ctrl_x_منوال_سطر_or_eval())
    {
	واپس get_wholeline_compl_info(سطر, curs_ستون);
    }
    ورنہ اگر (ctrl_x_منوال_files())
    {
	واپس get_filename_compl_info(سطر, startcol, curs_ستون);
    }
    ورنہ اگر (ctrl_x_منوال == CTRL_X_CMDLINE)
    {
	واپس get_cmdline_compl_info(سطر, curs_ستون);
    }
    ورنہ اگر (ctrl_x_منوال_function() || ctrl_x_منوال_omni()
	    || thesaurus_func_complete(ctrl_x_منوال))
    {
	اگر (get_userdefined_compl_info(curs_ستون, تہی, تہی) != OK)
	    واپس FAIL;
	*سطر_invalid = TRUE;	// "line" may have become invalid
    }
    ورنہ اگر (ctrl_x_منوال_spell())
    {
	اگر (get_spell_compl_info(startcol, curs_ستون) == FAIL)
	    واپس FAIL;
	*سطر_invalid = TRUE;	// "line" may have become invalid
    }
    ورنہ
    {
	اندرونی_error("ins_complete()");
	واپس FAIL;
    }

    واپس OK;
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
    ساکن خالی
ins_compl_continue_تلاش(char_u *سطر)
{
    // it is a continued search
    compl_cont_حالت &= ~CONT_INTRPT;	// remove INTRPT
    اگر (ctrl_x_منوال_معمول() || ctrl_x_منوال_راستہ_patterns()
						|| ctrl_x_منوال_راستہ_متعین_کرتا())
    {
	اگر (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_ستون = (colnr_T)getwhitecols(سطر);
	    compl_startpos.ستون = compl_ستون;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_حالت &= ~CONT_SOL;   // clear SOL if present
	}
	ورنہ
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    اگر (compl_cont_حالت & CONT_S_IPOS)
	    {
		compl_cont_حالت |= CONT_SOL;
		compl_startpos.ستون = (colnr_T)(skipwhite(
			    سطر + compl_لمبائی
			    + compl_startpos.ستون) - سطر);
	    }
	    compl_ستون = compl_startpos.ستون;
	}
	compl_لمبائی = curwin->w_cursor.ستون - (صحیح)compl_ستون;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#تعریف_کرو	MIN_SPACE 75
	اگر (compl_لمبائی > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_حالت &= ~CONT_SOL;
	    compl_لمبائی = (IOSIZE - MIN_SPACE);
	    compl_ستون = curwin->w_cursor.ستون - compl_لمبائی;
	}
	compl_cont_حالت |= CONT_ADDING | CONT_N_ADDS;
	اگر (compl_لمبائی < 1)
	    compl_cont_حالت &= CONT_LOCAL;
    }
    ورنہ اگر (ctrl_x_منوال_سطر_or_eval() || ctrl_x_منوال_رجسٹر())
	compl_cont_حالت = CONT_ADDING | CONT_N_ADDS;
    ورنہ
	compl_cont_حالت = 0;
}

/*
 * start insert mode completion
 */
    ساکن صحیح
ins_compl_آغاز(خالی)
{
    char_u	*سطر = تہی;
    صحیح		startcol = 0;	    // column where searched text starts
    colnr_T	curs_ستون;	    // cursor column
    صحیح		سطر_invalid = FALSE;
    صحیح		save_did_ai = did_ai;
    صحیح		flags = CP_ORIGINAL_متن;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;
    اگر (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	واپس FAIL;
    }

    سطر = ml_get(curwin->w_cursor.lnum);
    curs_ستون = curwin->w_cursor.ستون;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    اگر ((compl_cont_حالت & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_منوال == ctrl_x_منوال)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_تلاش(سطر);
    ورنہ
	compl_cont_حالت &= CONT_LOCAL;

    اگر (!compl_حالت_adding())	// normal expansion
    {
	compl_cont_منوال = ctrl_x_منوال;
	اگر (ctrl_x_منوال_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_حالت = 0;
	compl_cont_حالت |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (صحیح)curs_ستون;
	compl_ستون = 0;
    }

    // Work out completion pattern and original text -- webb
    اگر (compl_get_info(سطر, startcol, curs_ستون, &سطر_invalid) == FAIL)
    {
	اگر (ctrl_x_منوال_function() || ctrl_x_منوال_omni()
				|| thesaurus_func_complete(ctrl_x_منوال))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	واپس FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    اگر (سطر_invalid)
	سطر = ml_get(curwin->w_cursor.lnum);

    اگر (compl_حالت_adding())
    {
	اگر (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	اگر (ctrl_x_منوال_سطر_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.ستون = compl_ستون;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_لمبائی = 0;
	    compl_ستون = curwin->w_cursor.ستون;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    ورنہ
    {
	edit_submode_pre = تہی;
	compl_startpos.ستون = compl_ستون;
    }

    اگر (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	اگر (compl_cont_حالت & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_پیغام]);
	ورنہ
	    edit_submode = (char_u *)_(CTRL_X_پیغام(ctrl_x_منوال));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(تہی);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_متن);
    compl_orig_متن.لمبائی = (size_t)compl_لمبائی;
    compl_orig_متن.string = vim_strnsave(سطر + compl_ستون, (size_t)compl_لمبائی);
    اگر (p_ic)
	flags |= CP_ICASE;
    اگر (compl_orig_متن.string == تہی
	    || ins_compl_جوڑو(compl_orig_متن.string,
		(صحیح)compl_orig_متن.لمبائی,
		تہی, تہی, تہی, 0, flags, FALSE, تہی, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_نمونہ);
	VIM_CLEAR_STRING(compl_orig_متن);
	did_ai = save_did_ai;
	واپس FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    اگر (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_گنتی;
	showmode();
	edit_submode_extra = تہی;
	برآمد_flush();
    }

    did_ai = save_did_ai;
    واپس OK;
}

/*
 * display the completion status message
 */
    ساکن خالی
ins_compl_show_statusmsg(خالی)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    اگر (is_پہلا_مطابقت(compl_پہلا_مطابقت->cp_اگلا))
    {
	edit_submode_extra = compl_حالت_adding() && compl_لمبائی > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    اگر (edit_submode_extra == تہی)
    {
	اگر (مطابقت_at_original_متن(compl_curr_مطابقت))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	ورنہ اگر (compl_cont_حالت & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_گنتی;
	}
	ورنہ اگر (compl_curr_مطابقت->cp_اگلا == compl_curr_مطابقت->cp_پچھلا)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_گنتی;
	    compl_curr_مطابقت->cp_نمبر = 1;
	}
	ورنہ
	{
#اگر defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    اگر (compl_curr_مطابقت->cp_نمبر == -1)
		ins_compl_update_sequence_numbers();
#اگر_ختم
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    اگر (compl_curr_مطابقت->cp_نمبر != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		ساکن char_u مطابقت_ref[81];

		اگر (compl_matches > 0)
		    vim_snprintf((حرف *)مطابقت_ref, حجم(مطابقت_ref),
			    _("match %d of %d"),
			    compl_curr_مطابقت->cp_نمبر, compl_matches);
		ورنہ
		    vim_snprintf((حرف *)مطابقت_ref, حجم(مطابقت_ref),
			    _("match %d"),
			    compl_curr_مطابقت->cp_نمبر);
		edit_submode_extra = مطابقت_ref;
		edit_submode_highl = HLF_R;
		اگر (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    اگر (!compl_opt_suppress_خالی_خانے)
    {
	showmode();
	اگر (!shortmess(SHM_COMPLETIONMENU))
	{
	    اگر (edit_submode_extra != تہی)
	    {
		اگر (!p_smd)
		{
		    پیغام_hist_off = TRUE;
		    پیغام_attr((حرف *)edit_submode_extra,
			    edit_submode_highl < HLF_گنتی
			    ? HL_ATTR(edit_submode_highl) : 0);
		    پیغام_hist_off = FALSE;
		}
	    }
	    ورنہ
		پیغام_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    صحیح
ins_complete(صحیح c, صحیح enable_pum)
{
    صحیح		n;
    صحیح		save_w_wrow;
    صحیح		save_w_leftcol;
    صحیح		داخل_کرو_مطابقت;
    صحیح		no_matches_ملا;

    compl_direction = ins_compl_key2dir(c);
    داخل_کرو_مطابقت = ins_compl_use_مطابقت(c);

    اگر (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	اگر (ins_compl_آغاز() == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (داخل_کرو_مطابقت && stop_arrow() == FAIL)
	واپس FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_بفر;
    compl_shown_مطابقت = compl_curr_مطابقت;
    compl_shows_dir = compl_direction;
    compl_عدد_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_اگلا(TRUE, ins_compl_key2count(c), داخل_کرو_مطابقت);

    // Reset autocompletion timer expiry flag
    اگر (compl_autocomplete)
	compl_وقت_قاش_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    اگر (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_مطابقت = compl_shown_مطابقت;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    اگر (got_int && !global_busy)
    {
	(خالی)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_ملا = is_پہلا_مطابقت(compl_پہلا_مطابقت->cp_اگلا);
    اگر (no_matches_ملا)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	اگر (compl_لمبائی > 1
		|| compl_حالت_adding()
		|| (ctrl_x_منوال_not_default()
		    && !ctrl_x_منوال_راستہ_patterns()
		    && !ctrl_x_منوال_راستہ_متعین_کرتا()))
	    compl_cont_حالت &= ~CONT_N_ADDS;
    }

    اگر (compl_curr_مطابقت->cp_flags & CP_CONT_S_IPOS)
	compl_cont_حالت |= CONT_S_IPOS;
    ورنہ
	compl_cont_حالت &= ~CONT_S_IPOS;

    اگر (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    اگر (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    واپس OK;
}

/*
 * Enable autocompletion
 */
    خالی
ins_compl_enable_autocomplete(خالی)
{
#اگر_تعریف_شدہ ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#اگر_ختم
}

/*
 * Disable autocompletion
 */
    خالی
ins_compl_disable_autocomplete(خالی)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    خالی
ins_compl_arm_autostart(خالی)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    خالی
ins_compl_disarm_autostart(خالی)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(خالی)
{
#اگر_تعریف_شدہ ELAPSED_FUNC
    اگر (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_آغاز_tv);
	compl_autocomplete_pending = true;
	واپس true;
    }
#اگر_ختم
    واپس false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    خالی
ins_compl_clear_autocomplete_delay(خالی)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(خالی)
{
    واپس compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    طویل
ins_compl_autocomplete_elapsed(خالی)
{
#اگر_تعریف_شدہ ELAPSED_FUNC
    واپس ELAPSED_FUNC(compl_autocomplete_آغاز_tv);
#ورنہ
    واپس 0;
#اگر_ختم
}

/*
 * Remove (if needed) and show the popup menu
 */
    ساکن خالی
show_pum(صحیح پچھلا_w_wrow, صحیح پچھلا_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    اگر (پچھلا_w_wrow != curwin->w_wrow || پچھلا_w_leftcol != curwin->w_leftcol)
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
    ساکن غیرعلامتی
quote_meta(char_u *dest, char_u *src, صحیح len)
{
    غیرعلامتی	m = (غیرعلامتی)len + 1;  // one extra for the NUL

    برائے ( ; --len >= 0; src++)
    {
	انتخاب (*src)
	{
	    حالت '.':
	    حالت '*':
	    حالت '[':
		اگر (ctrl_x_منوال_dictionary() || ctrl_x_منوال_thesaurus())
		    توڑو;
		// FALLTHROUGH
	    حالت '~':
		اگر (!magic_isset())	// quote these only if magic is set
		    توڑو;
		// FALLTHROUGH
	    حالت '\\':
		اگر (ctrl_x_منوال_dictionary() || ctrl_x_منوال_thesaurus())
		    توڑو;
		// FALLTHROUGH
	    حالت '^':		// currently it's not needed.
	    حالت '$':
		m++;
		اگر (dest != تہی)
		    *dest++ = '\\';
		توڑو;
	}
	اگر (dest != تہی)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	اگر (has_mbyte)
	{
	    صحیح mb_len = (*mb_ptr2len)(src) - 1;
	    اگر (mb_len > 0 && len >= mb_len)
		برائے (صحیح i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    اگر (dest != تہی)
			*dest++ = *src;
		}
	}
    }
    اگر (dest != تہی)
	*dest = NUL;

    واپس m;
}

#اگر defined(EXITFREE)
    خالی
free_insexpand_stuff(خالی)
{
    VIM_CLEAR_STRING(compl_orig_متن);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_گنتی);
# endif
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    ساکن خالی
spell_عقب_مقصد_badword(خالی)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_مقصد(curwin, BACKWARD, SMT_ALL, TRUE, تہی);
    اگر (curwin->w_cursor.ستون != tpos.ستون)
	آغاز_arrow(&tpos);
}
#اگر_ختم

/*
 * Reset the info associated with completion sources.
 */
    ساکن خالی
cpt_sources_clear(خالی)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_اشاریہ = -1;
    cpt_sources_گنتی = 0;
}

/*
 * Setup completion sources.
 */
    ساکن صحیح
تیاری_cpt_sources(خالی)
{
    char_u  buf[LSIZE];
    صحیح	    slen;
    صحیح	    idx = 0;
    صحیح	    گنتی;
    char_u  *p;

    cpt_sources_clear();

    گنتی = get_cpt_sources_گنتی(curbuf->b_p_cpt);
    اگر (گنتی == 0)
	واپس OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_ماخذ_T, گنتی);
    اگر (cpt_sources_array == تہی)
	واپس FAIL;

    برائے (p = curbuf->b_p_cpt; *p;)
    {
	جب (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	اگر (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    اگر (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		اگر (caret != تہی)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((حرف *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_گنتی = گنتی;
    واپس OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    ساکن صحیح
is_cpt_func_refresh_always(خالی)
{
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    برائے (صحیح i = 0; i < cpt_sources_گنتی; i++)
	اگر (cpt_sources_array[i].cs_refresh_always)
	    واپس TRUE;
#اگر_ختم
    واپس FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    ساکن خالی
ins_compl_make_خطی(خالی)
{
    compl_T *m;

    اگر (compl_پہلا_مطابقت == تہی || compl_پہلا_مطابقت->cp_پچھلا == تہی)
	واپس;
    m = compl_پہلا_مطابقت->cp_پچھلا;
    m->cp_اگلا = تہی;
    compl_پہلا_مطابقت->cp_پچھلا = تہی;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    ساکن خالی
remove_old_matches(خالی)
{
    compl_T *current;
    صحیح	    shown_مطابقت_removed = FALSE;
    صحیح	    forward = (compl_پہلا_مطابقت->cp_cpt_ماخذ_idx < 0);

    اگر (cpt_sources_اشاریہ < 0)
	واپس;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    برائے (current = compl_پہلا_مطابقت; current != تہی; )
    {
	اگر (current->cp_cpt_ماخذ_idx == cpt_sources_اشاریہ)
	{
	    compl_T *مقصد_delete = current;

	    اگر (!shown_مطابقت_removed && compl_shown_مطابقت == current)
		shown_مطابقت_removed = TRUE;

	    current = current->cp_اگلا;

	    اگر (مقصد_delete == compl_پہلا_مطابقت)  // node to remove is at head
	    {
		compl_پہلا_مطابقت = مقصد_delete->cp_اگلا;
		compl_پہلا_مطابقت->cp_پچھلا = تہی;
	    }
	    ورنہ اگر (مقصد_delete->cp_اگلا == تہی) // node to remove is at tail
		مقصد_delete->cp_پچھلا->cp_اگلا = تہی;
	    ورنہ // node is in the moddle
	    {
		مقصد_delete->cp_پچھلا->cp_اگلا = مقصد_delete->cp_اگلا;
		مقصد_delete->cp_اگلا->cp_پچھلا = مقصد_delete->cp_پچھلا;
	    }
	    ins_compl_شے_free(مقصد_delete);
	}
	ورنہ
	    current = current->cp_اگلا;
    }

    // Re-assign compl_shown_match if necessary
    اگر (shown_مطابقت_removed)
    {
	اگر (forward)
	    compl_shown_مطابقت = compl_پہلا_مطابقت;
	ورنہ
	{  // Last node will have the prefix that is being completed
	    برائے (current = compl_پہلا_مطابقت; current->cp_اگلا != تہی;
		    current = current->cp_اگلا)
		;
	    compl_shown_مطابقت = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_مطابقت = compl_پہلا_مطابقت;
    برائے (current = compl_پہلا_مطابقت; current != تہی; )
    {
	اگر ((forward ? current->cp_cpt_ماخذ_idx < cpt_sources_اشاریہ
		    : current->cp_cpt_ماخذ_idx > cpt_sources_اشاریہ))
	{
	    compl_curr_مطابقت = forward ? current : current->cp_اگلا;
	    current = current->cp_اگلا;
	}
	ورنہ
	    توڑو;
    }
}
#اگر_ختم

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    ساکن خالی
get_cpt_func_completion_matches(callback_T *cb غیر_مستعمل)
{
    cpt_ماخذ_T    *cpt_src = &cpt_sources_array[cpt_sources_اشاریہ];
    صحیح		    startcol = cpt_src->cs_startcol;

    اگر (startcol == -2 || startcol == -3)
	واپس;

    اگر (سیٹ_compl_globals(startcol, curwin->w_cursor.ستون, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	اگر (!cpt_src->cs_refresh_always)
	    ins_compl_داخل_کرو_بائٹیں(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_نمونہ.string, cb);

	اگر (!cpt_src->cs_refresh_always)
	    ins_compl_delete();

	cpt_src->cs_refresh_always = compl_opt_refresh_always;
	compl_opt_refresh_always = FALSE;
    }
}
#اگر_ختم

/*
 * Retrieve completion matches from functions in the 'cpt' option where the
 * 'refresh:always' flag is set.
 */
    ساکن خالی
cpt_compl_refresh(خالی)
{
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = تہی;
    صحیح		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_خطی();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_اندر_رکھو(cpt);

    cpt_sources_اشاریہ = 0;
    برائے (p = cpt; *p;)
    {
	جب (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	اگر (*p == NUL)
	    توڑو;

	اگر (cpt_sources_array[cpt_sources_اشاریہ].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_اشاریہ);
	    اگر (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.ستون, cb,
			&startcol);
		اگر (ret == FAIL)
		{
		    اگر (startcol == -3)
			cpt_sources_array[cpt_sources_اشاریہ].cs_refresh_always
			    = FALSE;
		    ورنہ
			startcol = -2;
		}
		ورنہ اگر (startcol < 0 || startcol > curwin->w_cursor.ستون)
		    startcol = curwin->w_cursor.ستون;
		cpt_sources_array[cpt_sources_اشاریہ].cs_startcol = startcol;
		اگر (ret == OK)
		{
		    compl_ماخذ_آغاز_timer(cpt_sources_اشاریہ);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(خالی)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	اگر (may_advance_cpt_اشاریہ(p))
	    (خالی)advance_cpt_sources_اشاریہ_محفوظ();
    }
    cpt_sources_اشاریہ = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#اگر_ختم
}

/*
 * "preinserted()" function
 */
    خالی
f_preinserted(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    اگر (ins_compl_preinsert_effect())
	rettv->vval.v_نمبر = 1;
}
