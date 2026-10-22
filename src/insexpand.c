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

#include "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#define CTRL_X_WANT_IDENT	0x100

#define CTRL_X_NORMAL		0  // CTRL-N CTRL-P completion, default
#define CTRL_X_NOT_DEFINED_YET	1
#define CTRL_X_SCROLL		2
#define CTRL_X_WHOLE_LINE	3
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
#define CTRL_X_LOCAL_MSG	15	// only used in "ctrl_x_msgs"
#define CTRL_X_EVAL		16	// for builtin function complete()
#define CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#define CTRL_X_REGISTER	18	// complete words from registers

#define CTRL_X_MSG(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
static char *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    NULL, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    NULL, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    NULL,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#ако defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
static string_T ctrl_x_mode_names[] = {
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
    {NULL, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#endif

/*
 * Structure used to store one match for insert completion.
 */
typedef struct compl_S compl_T;
struct compl_S
{
    compl_T	*cp_next;
    compl_T	*cp_prev;
    compl_T	*cp_match_next;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_text[CPT_COUNT]);	// text for the menu
#ifdef FEAT_EVAL
    typval_T	cp_user_data;
#endif
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    цяло		cp_flags;		// CP_ values
    цяло		cp_number;		// sequence number
    цяло		cp_order;		// list position when first scored
    цяло		cp_score;		// fuzzy match score or proximity score
    цяло		cp_in_match_array;	// collected by compl_match_array
    цяло		cp_user_abbr_hl_id;	// highlight group ID for abbr
    цяло		cp_user_kind_hl_id;	// highlight group ID for kind
    цяло		cp_cpt_source_idx;	// index of this match's source in 'cpt' option
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
static compl_T    *compl_first_match = NULL;
static compl_T    *compl_curr_match = NULL;
static compl_T    *compl_shown_match = NULL;
static compl_T    *compl_old_match = NULL;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
typedef struct
{
    цяло	    cse_count;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#define CSE_OFF		((цяло)offsetof(complstr_T, cse_str))
#define HI2CSE(hi)	((complstr_T *)((hi)->hi_key - CSE_OFF))

static hashtab_T  compl_strings_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    static void
compl_strings_add(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*entry;

    ако (compl_strings_ht.ht_array == NULL)
	hash_init(&compl_strings_ht);
    ако (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_strings_ht, str, hash);
    ако (HASHITEM_EMPTY(hi))
    {
	entry = alloc(CSE_OFF + len + 1);
	ако (entry == NULL)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    върни;
	entry->cse_count = 1;
	vim_strncpy(entry->cse_str, str, len);
	ако (hash_add_item(&compl_strings_ht, hi, entry->cse_str, hash)
								      == FAIL)
	    vim_free(entry);
    }
    иначе
	++HI2CSE(hi)->cse_count;
}

// list used to store the compl_T which have the max score
static compl_T	  **compl_best_matches = NULL;
static цяло	  compl_num_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
static цяло	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
static string_T	  compl_leader = {NULL, 0};

static цяло	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
static цяло	  compl_used_match;

// didn't finish finding completions.
static цяло	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
static цяло	  compl_interrupted = FALSE;

static цяло	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
static цяло	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
static цяло	  ctrl_x_mode = CTRL_X_NORMAL;

static цяло	  compl_matches = 0;	    // number of completion matches
static string_T	  compl_pattern = {NULL, 0};	 // search pattern for matching items
#ifdef FEAT_COMPL_FUNC
static string_T	  cpt_compl_pattern = {NULL, 0}; // pattern returned by func in 'cpt'
#endif
static цяло	  compl_direction = FORWARD;
static цяло	  compl_shows_dir = FORWARD;
static цяло	  compl_pending = 0;	    // > 1 for postponed CTRL-N
static pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
static цяло	  compl_length = 0;
static linenr_T	  compl_lnum = 0;           // lnum where the completion start
static colnr_T	  compl_col = 0;	    // column where the text starts
					    // that is being completed
static colnr_T	  compl_ins_end_col = 0;
static colnr_T	  compl_longest_end_col = 0;  // end of 'longest' inserted text
static string_T	  compl_orig_text = {NULL, 0};  // text as it was before
					    // completion started
static цяло	  compl_cont_mode = 0;
static expand_T	  compl_xp;

static win_T	  *compl_curr_win = NULL;  // win where completion is active
static buf_T	  *compl_curr_buf = NULL;  // buf where completion is active

#define COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
static цяло	  compl_autocomplete = FALSE;	    // whether autocompletion is active
static bool	  compl_autostarted = false;	    // Vim started this completion
static bool	  compl_autostart_pending = false;  // the trigger armed one
static bool	  compl_autocomplete_pending = false;
#ifdef ELAPSED_FUNC
static elapsed_T  compl_autocomplete_start_tv;	    // when the delay was armed
#endif
static цяло	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
static цяло	  compl_time_slice_expired = FALSE; // time budget exceeded for current source
static цяло	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
static цяло	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#define COMPL_MIN_TIMEOUT_MS	5
#define DECAY_COMPL_TIMEOUT() \
    do { \
	ако (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } докато (0)

// Timeout values for F{func}, F and o values in 'complete'
#define COMPL_FUNC_TIMEOUT_MS		300
#define COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
static цяло	  compl_cont_status = 0;
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

static цяло	  compl_opt_refresh_always = FALSE;
static цяло	  compl_opt_suppress_empty = FALSE;

static цяло	  compl_selected_item = -1;

static цяло	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
typedef struct cpt_source_T
{
    цяло	cs_refresh_always;  // Whether 'refresh:always' is set for func
    цяло	cs_startcol;	    // Start column returned by func
    цяло	cs_max_matches;	    // Max items to display from this source
#ifdef ELAPSED_FUNC
    elapsed_T	compl_start_tv;	    // Timestamp when match collection starts
#endif
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_source_T;

static cpt_source_T *cpt_sources_array; // Pointer to the array of completion sources
static цяло	    cpt_sources_count;  // Total number of completion sources specified in the 'cpt' option
static цяло	    cpt_sources_index = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
static pumitem_T *compl_match_array = NULL;
static цяло compl_match_arraysize;

static цяло ins_compl_add(char_u *str, цяло len, char_u *fname, char_u **cptext, typval_T *user_data, цяло cdir, цяло flags, цяло adup, цяло *user_hl, цяло score);
static void ins_compl_longest_match(compl_T *match);
static void ins_compl_del_pum(void);
static void ins_compl_files(цяло count, char_u **files, цяло thesaurus, цяло flags, regmatch_T *regmatch, char_u *buf, цяло *dir);
static void ins_compl_free(void);
static цяло  ins_compl_need_restart(void);
static void ins_compl_new_leader(void);
static цяло  get_compl_len(void);
static void ins_compl_restart(void);
static void ins_compl_set_original_text(char_u *str, size_t len);
static void ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#ако defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
static void ins_compl_add_list(list_T *list);
static void ins_compl_add_dict(dict_T *dict);
static цяло get_userdefined_compl_info(colnr_T curs_col, callback_T *cb, цяло *startcol);
static void get_cpt_func_completion_matches(callback_T *cb);
static callback_T *get_callback_if_cpt_func(char_u *p, цяло idx);
static void fill_complete_info_dict(dict_T *di, compl_T *match, цяло add_match);
#endif
static цяло setup_cpt_sources(void);
static цяло is_cpt_func_refresh_always(void);
static void cpt_sources_clear(void);
static void cpt_compl_refresh(void);
static цяло  ins_compl_key2dir(цяло c);
static цяло  ins_compl_pum_key(цяло c);
static цяло  ins_compl_key2count(цяло c);
static void show_pum(цяло prev_w_wrow, цяло prev_w_leftcol);
static unsigned  quote_meta(char_u *dest, char_u *str, цяло len);
static цяло ins_compl_has_multiple(void);
static void ins_compl_expand_multiple(char_u *str);
static void ins_compl_longest_insert(char_u *prefix);
static void ins_compl_make_linear(void);
static цяло ins_compl_make_cyclic(void);

#ifdef FEAT_SPELL
static void spell_back_to_badword(void);
static цяло  spell_bad_len = 0;	// length of located bad word
#endif

/*
 * CTRL-X pressed in Insert mode.
 */
    void
ins_ctrl_x(void)
{
    ако (!ctrl_x_mode_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	ако (compl_cont_status & CONT_N_ADDS)
	    compl_cont_status |= CONT_INTRPT;
	иначе
	    compl_cont_status = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_MSG(ctrl_x_mode));
	edit_submode_pre = NULL;
	showmode();
    }
    иначе
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_mode = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
цяло ctrl_x_mode_none(void)
    { върни ctrl_x_mode == 0; }
цяло ctrl_x_mode_normal(void)
    { върни ctrl_x_mode == CTRL_X_NORMAL; }
цяло ctrl_x_mode_scroll(void)
    { върни ctrl_x_mode == CTRL_X_SCROLL; }
цяло ctrl_x_mode_whole_line(void)
    { върни ctrl_x_mode == CTRL_X_WHOLE_LINE; }
цяло ctrl_x_mode_files(void)
    { върни ctrl_x_mode == CTRL_X_FILES; }
цяло ctrl_x_mode_tags(void)
    { върни ctrl_x_mode == CTRL_X_TAGS; }
цяло ctrl_x_mode_path_patterns(void)
    { върни ctrl_x_mode == CTRL_X_PATH_PATTERNS; }
цяло ctrl_x_mode_path_defines(void)
    { върни ctrl_x_mode == CTRL_X_PATH_DEFINES; }
цяло ctrl_x_mode_dictionary(void)
    { върни ctrl_x_mode == CTRL_X_DICTIONARY; }
цяло ctrl_x_mode_thesaurus(void)
    { върни ctrl_x_mode == CTRL_X_THESAURUS; }
цяло ctrl_x_mode_cmdline(void)
    { върни ctrl_x_mode == CTRL_X_CMDLINE
		|| ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X; }
цяло ctrl_x_mode_function(void)
    { върни ctrl_x_mode == CTRL_X_FUNCTION; }
цяло ctrl_x_mode_omni(void)
    { върни ctrl_x_mode == CTRL_X_OMNI; }
цяло ctrl_x_mode_spell(void)
    { върни ctrl_x_mode == CTRL_X_SPELL; }
цяло ctrl_x_mode_eval(void)
    { върни ctrl_x_mode == CTRL_X_EVAL; }
цяло ctrl_x_mode_line_or_eval(void)
    { върни ctrl_x_mode == CTRL_X_WHOLE_LINE || ctrl_x_mode == CTRL_X_EVAL; }
цяло ctrl_x_mode_register(void)
    { върни ctrl_x_mode == CTRL_X_REGISTER; }

/*
 * Whether other than default completion has been selected.
 */
    цяло
ctrl_x_mode_not_default(void)
{
    върни ctrl_x_mode != CTRL_X_NORMAL;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    цяло
ctrl_x_mode_not_defined_yet(void)
{
    върни ctrl_x_mode == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    цяло
compl_status_adding(void)
{
    върни compl_cont_status & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    цяло
compl_status_sol(void)
{
    върни compl_cont_status & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    цяло
compl_status_local(void)
{
    върни compl_cont_status & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    void
compl_status_clear(void)
{
    compl_cont_status = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    static цяло
compl_dir_forward(void)
{
    върни compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    static цяло
compl_shows_dir_forward(void)
{
    върни compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    static цяло
compl_shows_dir_backward(void)
{
    върни compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    цяло
has_compl_option(цяло dict_opt)
{
    ако (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
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
	edit_submode = NULL;
	msg_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	ако (emsg_silent == 0 && !in_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    out_flush();
#ifdef FEAT_EVAL
	    ако (!get_vim_var_nr(VV_TESTING))
#endif
		ui_delay(2004L, FALSE);
	}
	върни FALSE;
    }
    върни TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    цяло
vim_is_ctrl_x_key(цяло c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    ако (ins_compl_pum_key(c))
	върни TRUE;

    switch (ctrl_x_mode)
    {
	case 0:		    // Not in any CTRL-X mode
	    върни (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	case CTRL_X_NOT_DEFINED_YET:
	case CTRL_X_CMDLINE_CTRL_X:
	    върни (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	case CTRL_X_SCROLL:
	    върни (c == Ctrl_Y || c == Ctrl_E);
	case CTRL_X_WHOLE_LINE:
	    върни (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_FILES:
	    върни (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_DICTIONARY:
	    върни (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_THESAURUS:
	    върни (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_TAGS:
	    върни (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ifdef FEAT_FIND_ID
	case CTRL_X_PATH_PATTERNS:
	    върни (c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_PATH_DEFINES:
	    върни (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#endif
	case CTRL_X_CMDLINE:
	    върни (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ifdef FEAT_COMPL_FUNC
	case CTRL_X_FUNCTION:
	    върни (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_OMNI:
	    върни (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#endif
	case CTRL_X_SPELL:
	    върни (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_EVAL:
	    върни (c == Ctrl_P || c == Ctrl_N);
	case CTRL_X_REGISTER:
	    върни (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    internal_error("vim_is_ctrl_x_key()");
    върни FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    static цяло
match_at_original_text(compl_T *match)
{
    върни match->cp_flags & CP_ORIGINAL_TEXT;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    static цяло
is_first_match(compl_T *match)
{
    върни match == compl_first_match;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    static compl_T *
find_original_text_match(void)
{
    ако (compl_first_match == NULL)
	върни NULL;
    ако (match_at_original_text(compl_first_match))
	върни compl_first_match;
    ако (compl_first_match->cp_prev != NULL
	    && match_at_original_text(compl_first_match->cp_prev))
	върни compl_first_match->cp_prev;
    върни NULL;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    цяло
ins_compl_accept_char(цяло c)
{
    ако (compl_autocomplete && compl_from_nonkeyword)
	върни FALSE;

    ако (ctrl_x_mode & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	върни vim_isIDc(c);

    switch (ctrl_x_mode)
    {
	case CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    върни vim_isfilec(c) && !vim_ispathsep(c);

	case CTRL_X_CMDLINE:
	case CTRL_X_CMDLINE_CTRL_X:
	case CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    върни vim_isprintc(c) && !VIM_ISWHITE(c);

	case CTRL_X_WHOLE_LINE:
	    // For while line completion a space can be part of the line.
	    върни vim_isprintc(c);
    }
    върни vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    static char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    цяло	    char_len,
    цяло	    compl_char_len,
    цяло	    min_len,
    char_u  **tofree)
{
    цяло		*wca;			// Wide character array.
    char_u	*p;
    цяло		i, c;
    цяло		has_lower = FALSE;
    цяло		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(цяло, char_len);
    ако (wca == NULL)
	върни IObuff;

    p = str;
    за (i = 0; i < char_len; ++i)
    {
	ако (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	иначе
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text.string;
    за (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ако (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    ако (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		за (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		break;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    ако (!has_lower)
    {
	p = compl_orig_text.string;
	за (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    ако (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		за (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		break;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text.string;
    за (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ако (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	иначе ако (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    докато (i < char_len)
    {
	ако (gap.ga_data != NULL)
	{
	    ако (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		върни (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    ако (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    иначе
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	иначе ако ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    ако (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		върни (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (цяло)(p - IObuff);
	}
	иначе ако (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	иначе
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    ако (gap.ga_data != NULL)
    {
	*tofree = gap.ga_data;
	върни gap.ga_data;
    }

    *p = NUL;
    върни IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    цяло
ins_compl_add_infercase(
    char_u	*str_arg,
    цяло		len,
    цяло		icase,
    char_u	*fname,
    цяло		dir,
    цяло		cont_s_ipos,  // next ^X<> will set initial_pos
    цяло		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    цяло		char_len;		// count multi-byte characters
    цяло		compl_char_len;
    цяло		min_len;
    цяло		flags = 0;
    цяло		res;
    char_u	*tofree = NULL;

    ако (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	ако (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    докато (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	иначе
	{
	    char_len = len;
	}

	// Find actual length of original text.
	ако (has_mbyte)
	{
	    p = compl_orig_text.string;
	    compl_char_len = 0;
	    докато (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	иначе
	{
	    compl_char_len = compl_length;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    ако (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    ако (icase)
	flags |= CP_ICASE;

    res = ins_compl_add(str, len, fname, NULL, NULL, dir, flags, FALSE, NULL, score);
    vim_free(tofree);
    върни res;
}

/*
 * Check if fuzzy matching is enabled
 */
    static цяло
cot_fuzzy(void)
{
    върни (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_mode_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    static цяло
is_nearest_active(void)
{
    върни (compl_autocomplete || (get_cot_flags() & COT_NEAREST))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    цяло
ins_compl_is_match_selected(void)
{
    върни compl_shown_match != NULL && !is_first_match(compl_shown_match);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    цяло
ins_compl_preinsert_longest(void)
{
    върни compl_autocomplete
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
    static цяло
ins_compl_add(
    char_u	*str,
    цяло		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_data UNUSED,  // "user_data" entry or NULL
    цяло		cdir,
    цяло		flags_arg,
    цяло		adup,		    // accept duplicate match
    цяло		*user_hl,           // user abbr/kind hlattr
    цяло		score)
{
    compl_T	*match, *current, *prev;
    цяло		dir = (cdir == 0 ? compl_direction : cdir);
    цяло		flags = flags_arg;
    цяло		inserted = FALSE;
    char_u	*new_str = NULL;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    ако (flags & CP_FAST)
	fast_breakcheck();
    иначе
	ui_breakcheck();
    ако (got_int)
	върни FAIL;
    ако (len < 0)
	len = (цяло)STRLEN(str);

    // If the same match is already present, don't add it.
    ако (compl_first_match != NULL && !adup && compl_strings_ht.ht_used > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *key;
	hashitem_T  *hi;

	ако (len < (цяло)sizeof(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    key = keybuf;
	}
	иначе
	{
	    new_str = vim_strnsave(str, len);
	    ако (new_str == NULL)
		върни FAIL;
	    key = new_str;
	}
	str_hash = hash_hash(key);
	hi = hash_lookup(&compl_strings_ht, key, str_hash);
	ако (!HASHITEM_EMPTY(hi))
	{
	    ако (is_nearest_active() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		match = compl_first_match;
		do
		{
		    ако (!match_at_original_text(match)
			    && STRNCMP(match->cp_str.string, str, len) == 0
			    && ((цяло)match->cp_str.length <= len
					  || match->cp_str.string[len] == NUL))
		    {
			ако (score < match->cp_score)
			    match->cp_score = score;
			break;
		    }
		    match = match->cp_next;
		} докато (match != NULL && !is_first_match(match));
	    }
	    vim_free(new_str);
	    върни NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    match = ALLOC_CLEAR_ONE(compl_T);
    ако (match == NULL)
    {
	vim_free(new_str);
	върни FAIL;
    }
    match->cp_number = flags & CP_ORIGINAL_TEXT ? 0 : -1;
    match->cp_order = -1;
    ако (new_str == NULL && (new_str = vim_strnsave(str, len)) == NULL)
    {
	vim_free(match);
	върни FAIL;
    }
    match->cp_str.string = new_str;
    match->cp_str.length = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    ако (fname != NULL
	    && compl_curr_match != NULL
	    && compl_curr_match->cp_fname != NULL
	    && STRCMP(fname, compl_curr_match->cp_fname) == 0)
	match->cp_fname = compl_curr_match->cp_fname;
    иначе ако (fname != NULL)
    {
	match->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    иначе
	match->cp_fname = NULL;
    match->cp_flags = flags;
    match->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    match->cp_user_kind_hl_id = user_hl ? user_hl[1] : 0;
    match->cp_score = score;
    match->cp_cpt_source_idx = cpt_sources_index;

    ако (cptext != NULL)
    {
	за (цяло i = 0; i < CPT_COUNT; ++i)
	{
	    ако (cptext[i] != NULL && *cptext[i] != NUL)
		match->cp_text[i] = vim_strsave(cptext[i]);
	}
    }
#ifdef FEAT_EVAL
    ако (user_data != NULL)
	match->cp_user_data = *user_data;
#endif

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    ако (compl_first_match == NULL)
	match->cp_next = match->cp_prev = NULL;
    иначе ако (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_first_match->cp_next;
	prev = compl_first_match;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	докато (current != NULL && current != compl_first_match)
	{
	    ако (current->cp_score < score)
	    {
		match->cp_next = current;
		match->cp_prev = current->cp_prev;
		ако (current->cp_prev)
		    current->cp_prev->cp_next = match;
		current->cp_prev = match;
		inserted = TRUE;
		break;
	    }
	    prev = current;
	    current = current->cp_next;
	}
	ако (!inserted)
	{
	    prev->cp_next = match;
	    match->cp_prev = prev;
	    match->cp_next = compl_first_match;
	    compl_first_match->cp_prev = match;
	}
    }
    иначе ако (dir == FORWARD)
    {
	match->cp_next = compl_curr_match->cp_next;
	match->cp_prev = compl_curr_match;
    }
    иначе	// BACKWARD
    {
	match->cp_next = compl_curr_match;
	match->cp_prev = compl_curr_match->cp_prev;
    }
    ако (match->cp_next)
	match->cp_next->cp_prev = match;
    ако (match->cp_prev)
	match->cp_prev->cp_next = match;
    иначе	// if there's nothing before, it is the first match
	compl_first_match = match;
    compl_curr_match = match;

    ако (!match_at_original_text(match))
	compl_strings_add(match->cp_str.string, match->cp_str.length,
								    str_hash);

    // Find the longest common string if still doing that.
    ако (compl_get_longest && (flags & CP_ORIGINAL_TEXT) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_mode_thesaurus())
	ins_compl_longest_match(match);

    върни OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    static цяло
ins_compl_equal(compl_T *match, char_u *str, цяло len)
{
    ако (match->cp_flags & CP_EQUAL)
	върни TRUE;
    ако (match->cp_flags & CP_ICASE)
	върни STRNICMP(match->cp_str.string, str, (size_t)len) == 0;
    върни STRNCMP(match->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    static цяло
ins_compl_equal_sc(compl_T *match, char_u *str, цяло len)
{
    цяло	typed = compl_length;
    цяло	longest_end = (compl_get_longest && compl_longest_end_col > compl_col)
			    ? (цяло)(compl_longest_end_col - compl_col) : typed;

    ако ((match->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_end <= typed)
	върни ins_compl_equal(match, str, len);

    ако ((цяло)match->cp_str.length < len)
	върни FALSE;

    за (цяло i = 0; i < len; ++i)
    {
	ако (i >= typed && i < longest_end
		? TOLOWER_LOC(match->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: match->cp_str.string[i] != str[i])
	    върни FALSE;
    }
    върни TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    static void
ins_compl_insert_bytes(char_u *p, цяло len)
{
    ако (len == -1)
	len = (цяло)STRLEN(p);
    ins_bytes_len(p, len);
    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(void)
{
    върни compl_leader.string != NULL ? compl_leader.string : compl_orig_text.string;
}

/*
 * Get current completion leader length
 */
    static size_t
ins_compl_leader_len(void)
{
    върни compl_leader.string != NULL ? compl_leader.length : compl_orig_text.length;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    цяло
ins_compl_col_range_attr(linenr_T lnum, цяло col)
{
    цяло	    start_col;
    цяло	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    цяло	    attr;

    ако (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	върни -1;

    start_col = compl_col + (цяло)ins_compl_leader_len();
    ако (!ins_compl_has_multiple())
	върни (col >= start_col && col < compl_ins_end_col) ? attr : -1;

    // Multiple lines
    ако ((lnum == compl_lnum && col >= start_col && col < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && col <= compl_ins_end_col))
	върни attr;

    върни -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    static цяло
ins_compl_has_multiple(void)
{
    върни vim_strchr(compl_shown_match->cp_str.string, '\n') != NULL;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    цяло
ins_compl_lnum_in_range(linenr_T lnum)
{
    ако (!ins_compl_has_multiple())
	върни FALSE;
    върни lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    static void
ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    цяло		c1, c2;
    цяло		had_match;

    ако (compl_leader.string == NULL)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(match->cp_str.string, match->cp_str.length);
	ако (compl_leader.string == NULL)
	    върни;

	compl_leader.length = match->cp_str.length;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_insert(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ако (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	върни;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = match->cp_str.string;
    докато (*p != NUL)
    {
	ако (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	иначе
	{
	    c1 = *p;
	    c2 = *s;
	}
	ако ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    break;
	ако (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	иначе
	{
	    ++p;
	    ++s;
	}
    }

    ако (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.length = (size_t)(p - compl_leader.string);

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_longest_insert(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ако (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    static void
ins_compl_add_matches(
    цяло		num_matches,
    char_u	**matches,
    цяло		icase)
{
    цяло		add_r = OK;
    цяло		dir = compl_direction;

    за (цяло i = 0; i < num_matches && add_r != FAIL; i++)
    {
	add_r = ins_compl_add(matches[i], -1, NULL, NULL, NULL, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, NULL,
		FUZZY_SCORE_NONE);
	ако (add_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(num_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    static цяло
ins_compl_make_cyclic(void)
{
    compl_T *match;
    цяло	    count = 0;

    ако (compl_first_match == NULL)
	върни 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    докато (match->cp_next != NULL && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++count;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    върни count;
}

/*
 * Return whether there currently is a shown match.
 */
    цяло
ins_compl_has_shown_match(void)
{
    върни compl_shown_match == NULL
	|| compl_shown_match != compl_shown_match->cp_next;
}

/*
 * Return whether the shown match is long enough.
 */
    цяло
ins_compl_long_shown_match(void)
{
    върни (цяло)compl_shown_match->cp_str.length
					    > curwin->w_cursor.col - compl_col;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    unsigned цяло
get_cot_flags(void)
{
    върни curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    static void
ins_compl_upd_pum(void)
{
    ако (compl_match_array == NULL)
	върни;

    цяло h = curwin->w_cline_height;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    ако (h != curwin->w_cline_height)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    static void
ins_compl_del_pum(void)
{
    ако (compl_match_array == NULL)
	върни;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    цяло
pum_wanted(void)
{
    // 'completeopt' must contain "menu" or "menuone"
    върни (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    static цяло
pum_enough_matches(void)
{
    compl_T     *compl;
    цяло		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    do
    {
	ако (compl == NULL || (!match_at_original_text(compl) && ++i == 2))
	    break;
	compl = compl->cp_next;
    } докато (!is_first_match(compl));

    ако ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	върни (i >= 1);
    върни (i >= 2);
}

#ако defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    static dict_T *
ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    ако (dict == NULL)
	върни NULL;
    fill_complete_info_dict(dict, match, FALSE);
    върни dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    static void
trigger_complete_changed_event(цяло cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    static цяло	    recursive = FALSE;
    save_v_event_T  save_v_event;

    ако (recursive)
	върни;

    item = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_match);
    ако (item == NULL)
	върни;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_items_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, NULL, NULL, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#endif

/*
 * Helper functions for mergesort_list().
 */
    static void*
cp_get_next(void *node)
{
    върни ((compl_T*)node)->cp_next;
}

    static void
cp_set_next(void *node, void *next)
{
    ((compl_T*)node)->cp_next = (compl_T*)next;
}

    static void*
cp_get_prev(void* node)
{
    върни ((compl_T*)node)->cp_prev;
}

    static void
cp_set_prev(void* node, void* prev)
{
    ((compl_T*)node)->cp_prev = (compl_T*)prev;
}

    static цяло
cp_compare_fuzzy(const void* a, const void* b)
{
    цяло score_a = ((compl_T*)a)->cp_score;
    цяло score_b = ((compl_T*)b)->cp_score;

    ако (score_a != score_b)
	върни (score_b > score_a) ? 1 : -1;

    върни ((compl_T *)a)->cp_order - ((compl_T *)b)->cp_order;
}

    static цяло
cp_compare_nearest(const void* a, const void* b)
{
    цяло score_a = ((compl_T*)a)->cp_score;
    цяло score_b = ((compl_T*)b)->cp_score;
    ако (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	върни 0;
    върни (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    static цяло
prepend_startcol_text(string_T *dest, string_T *src, цяло startcol)
{
    цяло prepend_len = compl_col - startcol;
    цяло new_length = prepend_len + (цяло)src->length;

    dest->length = (size_t)new_length;
    dest->string = alloc(new_length + 1);  // +1 for NUL
    ако (dest->string == NULL)
    {
	dest->length = 0;
	върни FAIL;
    }

    char_u	*line = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, line + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->length);
    dest->string[new_length] = NUL;
    върни OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    static string_T *
get_leader_for_startcol(compl_T *match, цяло cached)
{
    static string_T adjusted_leader = {NULL, 0};

    ако (match == NULL)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	върни NULL;
    }

    ако (cpt_sources_array == NULL)
	goto theend;

    цяло	cpt_idx = match->cp_cpt_source_idx;
    ако (cpt_idx < 0)
	goto theend;
    цяло	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    ако (compl_leader.string == NULL)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	ако (startcol < 0 || startcol >= compl_col)
	    върни &compl_orig_text;
	върни &compl_leader;  // pass through (startcol < compl_col)
    }

    ако (compl_col <= 0)
	goto theend;

    ако (startcol >= 0 && startcol < compl_col)
    {
	цяло prepend_len = compl_col - startcol;
	цяло new_length = prepend_len + (цяло)compl_leader.length;
	ако (cached && (size_t)new_length == adjusted_leader.length
		&& adjusted_leader.string != NULL)
	    върни &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	ако (prepend_startcol_text(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    goto theend;

	върни &adjusted_leader;
    }
theend:
    върни &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    static void
set_fuzzy_score(void)
{
    compl_T *compl;
    char_u  *pattern;
    цяло	    use_leader;

    ако (compl_first_match == NULL)
	върни;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != NULL && compl_leader.length > 0);
    ако (!use_leader)
    {
	ако (compl_orig_text.string == NULL || compl_orig_text.length == 0)
	    върни;
	pattern = compl_orig_text.string;
    }
    иначе
    {
	/* Clear the leader cache once before the loop */
	(void)get_leader_for_startcol(NULL, TRUE);
	pattern = NULL;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    цяло next = 0;
    compl = compl_first_match;
    do
    {
	ако (compl->cp_order < 0)
	    compl->cp_order = next;
	next = MAX(next, compl->cp_order + 1);

	ако (use_leader)
	    pattern = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_match_str(compl->cp_str.string, pattern);
	compl = compl->cp_next;
    } докато (compl != NULL && !is_first_match(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    static void
sort_compl_match_list(цяло (*compare)(const void *, const void *))
{
    compl_T	*orig_text;

    ако (!compl_first_match || is_first_match(compl_first_match->cp_next))
	върни;

    orig_text = find_original_text_match();
    ако (orig_text == NULL)
	върни;

    ins_compl_make_linear();
    ако (orig_text == compl_first_match)
    {
	compl_first_match->cp_next->cp_prev = NULL;
	compl_first_match->cp_next = mergesort_list(compl_first_match->cp_next,
		cp_get_next, cp_set_next, cp_get_prev, cp_set_prev, compare);
	compl_first_match->cp_next->cp_prev = compl_first_match;
    }
    иначе
    {
	compl_T	*tail;

	orig_text->cp_prev->cp_next = NULL;
	compl_first_match = mergesort_list(compl_first_match, cp_get_next,
		cp_set_next, cp_get_prev, cp_set_prev, compare);
	tail = compl_first_match;
	докато (tail->cp_next != NULL)
	    tail = tail->cp_next;
	tail->cp_next = orig_text;
	orig_text->cp_prev = tail;
    }
    (void)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    static цяло
get_user_highlight_attr(цяло hl_id)
{
    цяло	    attr;

    ако (hl_id <= 0)
	върни -1;
    attr = syn_id2attr(hl_id);
    върни attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    static цяло
ins_compl_build_pum(void)
{
    compl_T     *compl;
    compl_T     *shown_compl = NULL;
    цяло		did_find_shown_match = FALSE;
    цяло		shown_match_ok = FALSE;
    цяло		i = 0;
    цяло		cur = -1;
    цяло		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*match_head = NULL;
    compl_T	*match_tail = NULL;
    compl_T	*match_next = NULL;
    цяло		*match_count = NULL;
    цяло		is_forward = compl_shows_dir_forward();
    цяло		is_cpt_completion = (cpt_sources_array != NULL);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    ако (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    ако (compl_leader.string != NULL
	    && STRCMP(compl_leader.string, compl_orig_text.string) == 0
	    && shown_match_ok == FALSE)
	compl_shown_match = compl_no_select ? compl_first_match
					    : compl_first_match->cp_next;

    ако (is_cpt_completion)
    {
	match_count = ALLOC_CLEAR_MULT(цяло, cpt_sources_count);
	ако (match_count == NULL)
	    върни -1;
    }

    (void)get_leader_for_startcol(NULL, TRUE); // Clear the cache

    compl = compl_first_match;
    do
    {
	compl->cp_in_match_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	ако (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	ако (!match_at_original_text(compl)
		&& (leader->string == NULL
		    || ins_compl_equal_sc(compl, leader->string,
			(цяло)leader->length)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    цяло match_limit_exceeded = FALSE;
	    цяло cur_source = compl->cp_cpt_source_idx;
	    ако (is_forward && cur_source != -1 && is_cpt_completion)
	    {
		match_count[cur_source]++;
		цяло max_matches = cpt_sources_array[cur_source].cs_max_matches;
		ако (max_matches > 0 && match_count[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    ако (!match_limit_exceeded)
	    {
		++compl_match_arraysize;
		compl->cp_in_match_array = TRUE;
		ако (match_head == NULL)
		    match_head = compl;
		иначе
		    match_tail->cp_match_next = compl;
		match_tail = compl;

		ако (!shown_match_ok && !cot_fuzzy())
		{
		    ако (compl == compl_shown_match || did_find_shown_match)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_match = compl;
			did_find_shown_match = TRUE;
			shown_match_ok = TRUE;
		    }
		    иначе
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		иначе ако (cot_fuzzy())
		{
		    ако (i == 0)
			shown_compl = compl;

		    ако (!shown_match_ok && compl == compl_shown_match)
		    {
			cur = i;
			shown_match_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	ако (compl == compl_shown_match && !cot_fuzzy())
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    ако (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    ако (!shown_match_ok && shown_compl != NULL)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } докато (compl != NULL && !is_first_match(compl));

    vim_free(match_count);

    ако (compl_match_arraysize == 0)
	върни -1;

    ако (cot_fuzzy() && !compl_no_select && !shown_match_ok)
    {
	compl_shown_match = shown_compl;
	shown_match_ok = TRUE;
	cur = 0;
    }

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    ако (compl_match_array == NULL)
	върни -1;

    compl = match_head;
    i = 0;
    докато (compl != NULL)
    {
	compl_match_array[i].pum_text = compl->cp_text[CPT_ABBR] != NULL
			    ? compl->cp_text[CPT_ABBR] : compl->cp_str.string;
	compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	compl_match_array[i].pum_cpt_source_idx = compl->cp_cpt_source_idx;
	compl_match_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_match_array[i].pum_user_kind_hlattr =
			get_user_highlight_attr(compl->cp_user_kind_hl_id);
	compl_match_array[i++].pum_extra = compl->cp_text[CPT_MENU] != NULL
			    ? compl->cp_text[CPT_MENU] : compl->cp_fname;
	match_next = compl->cp_match_next;
	compl->cp_match_next = NULL;
	compl = match_next;
    }

    ако (!shown_match_ok)    // no displayed match at all
	cur = -1;

    върни cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    void
ins_compl_show_pum(void)
{
    цяло		i;
    цяло		cur = -1;
    colnr_T	col;

    ако (!pum_wanted() || !pum_enough_matches())
	върни;

    // Avoid redrawing the screen under a pum that stays in place.
    ако (!pum_redraw_in_same_position())
	pum_call_update_screen();

    ако (compl_match_array == NULL)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    иначе
    {
	// popup menu already exists, only need to find the current item.
	за (i = 0; i < compl_match_arraysize; ++i)
	{
	    ако (compl_match_array[i].pum_text == compl_shown_match->cp_str.string
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		break;
	    }
	}
    }

    ако (compl_match_array == NULL)
    {
#ifdef FEAT_EVAL
	ако (compl_started && has_completechanged())
	    trigger_complete_changed_event(cur);
#endif
	върни;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    validate_cursor_col();
    цяло pum_wcol = curwin->w_wcol;
    curwin->w_cursor.col = col;
    validate_cursor_col();
    compl_selected_item = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_status = true;
    pum_display(compl_match_array, compl_match_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    ако (compl_started && compl_curr_match != compl_shown_match)
	compl_curr_match = compl_shown_match;

#ifdef FEAT_EVAL
    ако (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}

#define DICT_FIRST	(1)	// use just first element in "dict"
#define DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    static void
ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    цяло		flags,		// DICT_FIRST and/or DICT_EXACT
    цяло		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    цяло		count;
    цяло		save_p_scs;
    цяло		dir = compl_direction;

    ако (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	ако (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	иначе
#endif
	    върни;
    }

    buf = alloc(LSIZE);
    ако (buf == NULL)
	върни;
    regmatch.regprog = NULL;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ако (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    ако (ctrl_x_mode_line_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	ако (pat_esc == NULL)
	    goto theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	ако (ptr == NULL)
	{
	    vim_free(pat_esc);
	    goto theend;
	}
	vim_snprintf((char *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    иначе
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ако (regmatch.regprog == NULL)
	    goto theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    докато (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	ако (flags == DICT_EXACT)
	{
	    count = 1;
	    files = &dict;
	}
	иначе
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ifdef FEAT_SPELL
	    ако (!thesaurus && STRCMP(buf, "spell") == 0)
		count = -1;
	    иначе
#endif
		ако (vim_strchr(buf, '`') != NULL
		    || expand_wildcards(1, &buf, &count, &files,
						     EW_FILE|EW_SILENT) != OK)
		count = 0;
	}

#ifdef FEAT_SPELL
	ако (count == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    ако (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    иначе
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	иначе
#endif
	    ако (count > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(count, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    ако (flags != DICT_EXACT)
		FreeWild(count, files);
	}
	ако (flags != 0)
	    break;
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
    static цяло
thesaurus_add_words_in_line(
    char_u	*fname,
    char_u	**buf_arg,
    цяло		dir,
    char_u	*skip_word)
{
    цяло		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    докато (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	ако (*ptr == NUL || *ptr == NL)
	    break;
	wstart = ptr;

	// Find end of the word.
	ако (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    докато (*ptr != NUL)
	    {
		цяло l = (*mb_ptr2len)(ptr);

		ако (l < 2 && !vim_iswordc(*ptr))
		    break;
		ptr += l;
	    }
	иначе
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	ако (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (цяло)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    ако (status == FAIL)
		break;
	}
    }

    *buf_arg = ptr;
    върни status;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    static void
ins_compl_files(
    цяло		count,
    char_u	**files,
    цяло		thesaurus,
    цяло		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    цяло		*dir)
{
    char_u	*ptr;
    цяло		i;
    FILE	*fp;
    цяло		add_r;
    char_u	*leader = NULL;
    цяло		leader_len = 0;
    цяло		score = 0;
    цяло		len = 0;
    char_u	*line_end = NULL;

    ако (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (цяло)ins_compl_leader_len();
    }

    за (i = 0; i < count && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((char *)files[i], "r");  // open dictionary file
	ако (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    msg_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((char *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (char *)files[i]);
	    (void)msg_trunc_attr((char *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	ако (fp == NULL)
	    continue;

	// Read dictionary file line by line.
	// Check each line for a match.
	докато (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    ако (cot_fuzzy() && leader_len > 0)
	    {
		line_end = find_line_end(ptr);
		докато (ptr < line_end)
		{
		    ако (fuzzy_match_str_in_line(&ptr, leader, &len, NULL, &score))
		    {
			char_u *end_ptr = ctrl_x_mode_line_or_eval()
					? find_line_end(ptr) : find_word_end(ptr);
			add_r = ins_compl_add_infercase(ptr, (цяло)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			ако (add_r == FAIL)
			    break;
			ptr = end_ptr;  // start from next word
			ако (compl_get_longest && ctrl_x_mode_normal()
				&& compl_first_match->cp_next
				&& score == compl_first_match->cp_next->cp_score)
			    compl_num_bests++;
		    }
		}
	    }
	    иначе ако (regmatch != NULL)
	    {
		докато (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_mode_line_or_eval() ? find_line_end(ptr)
						    : find_word_end(ptr);
		    add_r = ins_compl_add_infercase(regmatch->startp[0],
			    (цяло)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    ако (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			add_r = thesaurus_add_words_in_line(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    ако (add_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    иначе ако (add_r == FAIL)
			break;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    ако (*ptr == '\n' || got_int)
			break;
		}
	    }
	    line_breakcheck();
	    ins_compl_check_keys(50, FALSE);
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
    ако (has_mbyte)
    {
	докато (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    иначе
    {
	докато (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    върни ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
find_word_end(char_u *ptr)
{
    ако (has_mbyte)
    {
	цяло start_class = mb_get_class(ptr);
	ако (start_class > 1)
	    докато (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		ако (mb_get_class(ptr) != start_class)
		    break;
	    }
    }
    иначе
	докато (vim_iswordc(*ptr))
	    ++ptr;
    върни ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
find_line_end(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    докато (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    върни s;
}

/*
 * Free a completion item in the list
 */
    static void
ins_compl_item_free(compl_T *match)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    ако (compl_strings_ht.ht_used > 0 && match->cp_str.string != NULL
					   && !match_at_original_text(match))
    {
	hashitem_T *hi = hash_find(&compl_strings_ht, match->cp_str.string);

	ако (!HASHITEM_EMPTY(hi))
	{
	    complstr_T *entry = HI2CSE(hi);

	    ако (--entry->cse_count <= 0)
	    {
		hash_remove(&compl_strings_ht, hi, "completion match");
		vim_free(entry);
	    }
	}
    }
    VIM_CLEAR_STRING(match->cp_str);
    // several entries may use the same fname, free it just once.
    ако (match->cp_flags & CP_FREE_FNAME)
	vim_free(match->cp_fname);
    за (цяло i = 0; i < CPT_COUNT; ++i)
	vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
    clear_tv(&match->cp_user_data);
#endif
    vim_free(match);
}

/*
 * Free the list of completions
 */
    static void
ins_compl_free(void)
{
    compl_T *match;

    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);

    ако (compl_first_match == NULL)
	върни;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_strings_ht, CSE_OFF);
    hash_init(&compl_strings_ht);

    compl_curr_match = compl_first_match;
    do
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	ins_compl_item_free(match);
    } докато (compl_curr_match != NULL && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = NULL;
    compl_shown_match = NULL;
    compl_old_match = NULL;
}

/*
 * Reset/clear the completion state.
 */
    void
ins_compl_clear(void)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_item = -1;
    compl_ins_end_col = 0;
    compl_longest_end_col = 0;
    compl_curr_win = NULL;
    compl_curr_buf = NULL;
    VIM_CLEAR_STRING(compl_pattern);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = NULL;
    VIM_CLEAR_STRING(compl_orig_text);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_num_bests = 0;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}

/*
 * Return TRUE when Insert completion is active.
 */
    цяло
ins_compl_active(void)
{
    върни compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    цяло
ins_compl_win_active(win_T *wp)
{
    върни ins_compl_active() && wp == compl_curr_win
	&& wp->w_buffer == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    цяло
ins_compl_used_match(void)
{
    върни compl_used_match;
}

/*
 * Initialize get longest common string.
 */
    void
ins_compl_init_get_longest(void)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    цяло
ins_compl_interrupted(void)
{
    върни compl_interrupted || compl_time_slice_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    цяло
ins_compl_enter_selects(void)
{
    върни compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_col(void)
{
    върни compl_col;
}

/*
 * Return the length in bytes of the text being completed
 */
    цяло
ins_compl_len(void)
{
    върни compl_length;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    цяло
ins_compl_has_preinsert(void)
{
    цяло cur_cot_flags = get_cot_flags();
    ако (compl_autocomplete && p_ic && !p_inf)
	върни FALSE;
    върни !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    цяло
ins_compl_preinsert_effect(void)
{
    ако (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	върни FALSE;

    върни curwin->w_cursor.col < compl_ins_end_col;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    цяло
ins_compl_bs(void)
{
    char_u	*line;
    char_u	*p;

    ако (ins_compl_preinsert_effect())
	ins_compl_delete();

    line = ml_get_curline();
    p = line + curwin->w_cursor.col;
    MB_PTR_BACK(line, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    ако ((цяло)(p - line) - (цяло)compl_col < 0
	    || ((цяло)(p - line) - (цяло)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (цяло)(p - line) - (цяло)compl_col
							- compl_length < 0))
	върни K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    ако (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.length = (size_t)((p - line) - compl_col);
    compl_leader.string = vim_strnsave(line + compl_col, compl_leader.length);
    ако (compl_leader.string == NULL)
    {
	compl_leader.length = 0;
	върни K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    ако (compl_autocomplete && compl_first_match && !ins_compl_has_preinsert())
	compl_shown_match = compl_first_match;

    ins_compl_new_leader();
    ако (compl_shown_match != NULL)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    върни NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    static цяло
ins_compl_need_restart(void)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    върни compl_was_interrupted
	|| ((ctrl_x_mode_function() || ctrl_x_mode_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    цяло
ins_compl_has_autocomplete(void)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    върни curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    static void
ins_compl_fuzzy_sort(void)
{
    цяло	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    set_fuzzy_score();
    ако (!(cur_cot_flags & COT_NOSORT))
    {
	sort_compl_match_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	ако ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    цяло none_selected = compl_shown_match == (compl_shows_dir_forward()
		    ? compl_first_match : compl_first_match->cp_prev);
	    ако (!none_selected)
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
    static void
ins_compl_new_leader(void)
{
    цяло	    save_w_wrow = curwin->w_wrow;
    цяло	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_insert_bytes(compl_leader.string + get_compl_len(), -1);
    compl_used_match = FALSE;

    ако (compl_started)
    {
	ins_compl_set_original_text(compl_leader.string, compl_leader.length);
	ако (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	ако (cot_fuzzy())
	    ins_compl_fuzzy_sort();
    }
    иначе
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	ако (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	ако (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	иначе
	    compl_autocomplete = FALSE;
	ако (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match && compl_selected_item != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    ако (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    ако (compl_match_array == NULL)
	compl_enter_selects = FALSE;
    иначе ако (ins_compl_has_preinsert() && compl_leader.length > 0)
	ins_compl_insert(TRUE, FALSE);
    иначе ако (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.length > 0 && !ins_compl_preinsert_effect())
	ins_compl_insert(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    static цяло
get_compl_len(void)
{
    цяло off = (цяло)curwin->w_cursor.col - (цяло)compl_col;
    върни MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    void
ins_compl_addleader(цяло c)
{
    цяло	    cc;

    ако (ins_compl_preinsert_effect())
	ins_compl_delete();

    ако (stop_arrow() == FAIL)
	върни;
    ако (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	ако (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    иначе
    {
	ins_char(c);
	ако (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    ако (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    ако (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.length = (size_t)(curwin->w_cursor.col - compl_col);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_col,
		compl_leader.length);
	ако (compl_leader.string == NULL)
	{
	    compl_leader.length = 0;
	    върни;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    static void
ins_compl_restart(void)
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
    compl_num_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    static void
ins_compl_set_original_text(char_u *str, size_t len)
{
    compl_T	*match = find_original_text_match();
    char_u	*p;

    // Replace the original text entry.
    ако (match == NULL)
	върни;

    p = vim_strnsave(str, len);
    ако (p == NULL)
	върни;

    VIM_CLEAR_STRING(match->cp_str);
    match->cp_str.string = p;
    match->cp_str.length = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    void
ins_compl_addfrommatch(void)
{
    char_u	*p;
    цяло		len = (цяло)curwin->w_cursor.col - (цяло)compl_col;
    цяло		c;

    p = compl_shown_match->cp_str.string;
    ако ((цяло)compl_shown_match->cp_str.length <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	ако (!match_at_original_text(compl_shown_match))
	    върни;

	p = NULL;
	plen = 0;
	за (cp = compl_shown_match->cp_next; cp != NULL
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    ако (compl_leader.string == NULL
		    || ins_compl_equal(cp, compl_leader.string,
			(цяло)compl_leader.length))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.length;
		break;
	    }
	}
	ако (p == NULL || (цяло)plen <= len)
	    върни;
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
    static цяло
set_ctrl_x_mode(цяло c)
{
    цяло	    retval = FALSE;

    switch (c)
    {
	case Ctrl_E:
	case Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_mode = CTRL_X_SCROLL;
	    ако (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    иначе
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = NULL;
	    showmode();
	    break;
	case Ctrl_L:
	    // complete whole line
	    ctrl_x_mode = CTRL_X_WHOLE_LINE;
	    break;
	case Ctrl_F:
	    // complete filenames
	    ctrl_x_mode = CTRL_X_FILES;
	    break;
	case Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_mode = CTRL_X_DICTIONARY;
	    break;
	case Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    ако (vpeekc() == '=')
		break;
	    ctrl_x_mode = CTRL_X_REGISTER;
	    break;
	case Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_mode = CTRL_X_THESAURUS;
	    break;
#ifdef FEAT_COMPL_FUNC
	case Ctrl_U:
	    // user defined completion
	    ctrl_x_mode = CTRL_X_FUNCTION;
	    break;
	case Ctrl_O:
	    // omni completion
	    ctrl_x_mode = CTRL_X_OMNI;
	    break;
#endif
	case 's':
	case Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_mode = CTRL_X_SPELL;
#ifdef FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_back_to_badword();
	    --emsg_off;
#endif
	    break;
	case Ctrl_RSB:
	    // complete tag names
	    ctrl_x_mode = CTRL_X_TAGS;
	    break;
#ifdef FEAT_FIND_ID
	case Ctrl_I:
	case K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_mode = CTRL_X_PATH_PATTERNS;
	    break;
	case Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_mode = CTRL_X_PATH_DEFINES;
	    break;
#endif
	case Ctrl_V:
	case Ctrl_Q:
	    // complete vim commands
	    ctrl_x_mode = CTRL_X_CMDLINE;
	    break;
	case Ctrl_Z:
	    // stop completion
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = NULL;
	    showmode();
	    retval = TRUE;
	    break;
	case Ctrl_P:
	case Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    ако (!(compl_cont_status & CONT_INTRPT))
		compl_cont_status |= CONT_LOCAL;
	    иначе ако (compl_cont_mode != 0)
		compl_cont_status &= ~CONT_LOCAL;
	    // FALLTHROUGH
	default:
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
	    ако (c == Ctrl_X)
	    {
		ако (compl_cont_mode != 0)
		    compl_cont_status = 0;
		иначе
		    compl_cont_mode = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_mode = CTRL_X_NORMAL;
	    edit_submode = NULL;
	    showmode();
	    break;
    }

    върни retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    static void
trigger_complete_done_event(цяло mode UNUSED, string_T *word UNUSED)
{
#ако defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*mode_str;

    ако (word == NULL || word->string == NULL)
	(void)dict_add_string_len(v_event, "complete_word", (char_u *)"", 0);
    иначе
	(void)dict_add_string_len(v_event, "complete_word", word->string, (цяло)word->length);

    mode_str = &ctrl_x_mode_names[mode & ~CTRL_X_WANT_IDENT];
    (void)dict_add_string_len(v_event, "complete_type",
	(mode_str->string == NULL) ? (char_u *)"" : mode_str->string, (цяло)mode_str->length);

    dict_set_items_ro(v_event);
#endif
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#ако defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Stop insert completion mode
 */
    static цяло
ins_compl_stop(цяло c, цяло prev_mode, цяло retval)
{
    цяло		want_cindent;
    string_T	word = {NULL, 0};

    // Remove pre-inserted text when present.
    ако (ins_compl_preinsert_effect() && ins_compl_win_active(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    ако (compl_curr_match != NULL || compl_leader.string != NULL || c == Ctrl_E)
    {
	char_u	*ptr = NULL;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	ако (compl_curr_match != NULL && compl_used_match && c != Ctrl_E)
	    ptr = compl_curr_match->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    ако (compl_cont_mode == CTRL_X_WHOLE_LINE)
    {
	// re-indent the current line
	ако (want_cindent)
	{
	    do_c_expr_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    иначе ако (!compl_autocomplete || compl_used_match)
    {
	цяло prev_col = curwin->w_cursor.col;

	// put the cursor on the last char, for 'tw' formatting
	ако (prev_col > 0)
	    dec_cursor();
	// only format when something was inserted
	ако (!arrow_used && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	ако (prev_col > 0
		&& ml_get_curline()[curwin->w_cursor.col] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    ако ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	word.string = vim_strnsave(compl_shown_match->cp_str.string,
	    compl_shown_match->cp_str.length);
	ако (word.string == NULL)
	    word.length = 0;
	иначе
	    word.length = compl_shown_match->cp_str.length;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    ако (c == Ctrl_E)
    {
	char_u *p = NULL;
	size_t	plen = 0;

	ins_compl_delete();
	ако (compl_leader.string != NULL)
	{
	    p = compl_leader.string;
	    plen = compl_leader.length;
	}
	иначе ако (compl_first_match != NULL)
	{
	    p = compl_orig_text.string;
	    plen = compl_orig_text.length;
	}
	ако (p != NULL)
	{
	    цяло	    compl_len = get_compl_len();

	    ако ((цяло)plen > compl_len)
		ins_compl_insert_bytes(p + compl_len, (цяло)plen - compl_len);
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
    ако (!shortmess(SHM_COMPLETIONMENU))
	msg_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_mode = CTRL_X_NORMAL;
    compl_enter_selects = FALSE;
    ако (edit_submode != NULL)
    {
	edit_submode = NULL;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_num_bests = 0;
    compl_ins_end_col = 0;

    ако (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    ако (want_cindent && in_cinkeys(KEY_COMPLETE, ' ', inindent(0)))
	do_c_expr_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_done_event(prev_mode, &word);
    vim_free(word.string);

    върни retval;
}

/*
 * Cancel completion.
 */
    цяло
ins_compl_cancel(void)
{
    върни ins_compl_stop(' ', ctrl_x_mode, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    цяло
ins_compl_prep(цяло c)
{
    цяло	    retval = FALSE;
    цяло	    prev_mode = ctrl_x_mode;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    ако (c != Ctrl_R && vim_is_ctrl_x_key(c))
	edit_submode_extra = NULL;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    ако (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	върни retval;

#ifdef FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    ако (is_mouse_key(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	ако (c == K_LEFTRELEASE
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
	    върни retval;
	ако (popup_visible)
	{
	    цяло	    row = mouse_row;
	    цяло	    col = mouse_col;
	    win_T   *wp = mouse_find_win(&row, &col, FIND_POPUP);

	    ако (wp != NULL && WIN_IS_POPUP(wp))
		върни retval;
	}
    }
#endif

    ако (ctrl_x_mode == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	ако (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_key(c)
		|| !vim_is_ctrl_x_key(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    ако (c == Ctrl_Z)
		retval = TRUE;
	}
	иначе
	{
	    ctrl_x_mode = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_mode = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    ако (ctrl_x_mode_not_defined_yet()
			   || (ctrl_x_mode_normal() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_used_match = TRUE;
    }

    ако (ctrl_x_mode_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = set_ctrl_x_mode(c);
    иначе ако (ctrl_x_mode_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	ако (c != Ctrl_R && !vim_is_ctrl_x_key(c))
	{
	    ctrl_x_mode = ctrl_x_mode_scroll() ? CTRL_X_NORMAL : CTRL_X_FINISHED;
	    edit_submode = NULL;
	}
	showmode();
    }

    ако (compl_started || ctrl_x_mode == CTRL_X_FINISHED)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	ако ((ctrl_x_mode_normal() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_key(c))
		|| ctrl_x_mode == CTRL_X_FINISHED)
	    retval = ins_compl_stop(c, prev_mode, retval);
    }
    иначе ако (ctrl_x_mode == CTRL_X_LOCAL_MSG)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_done_event(ctrl_x_mode, NULL);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    ако (c != Ctrl_R && !vim_is_ctrl_x_key(c))
    {
	compl_cont_status = 0;
	compl_cont_mode = 0;
    }

    върни retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    static void
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    цяло	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    ако (ptr == NULL)
    {
	ако (compl_leader.string != NULL)
	    ptr = compl_leader.string;
	иначе
	    върни;  // nothing to do
    }
    ако (compl_orig_text.string != NULL)
    {
	p = compl_orig_text.string;
	// Find length of common prefix between original text and new completion
	докато (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	ако (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	за (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    ако (ptr != NULL)
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
    static buf_T *
ins_compl_next_buf(buf_T *buf, цяло flag)
{
    static win_T    *wp = NULL;
    цяло		    skip_buffer;

    ако (flag == 'w')		// just windows
    {
	ако (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	докато (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_next != NULL) ? wp->w_next : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    ако (wp == curwin || !wp->w_buffer->b_scanned)
		break;
	}
	buf = wp->w_buffer;
    }
    иначе
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	докато (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_next != NULL) ? buf->b_next : firstbuf;
	    // Break if we're back at start buffer
	    ако (buf == curbuf)
		break;

	    // Check buffer conditions based on flag
	    ако (flag == 'U')
		skip_buffer = buf->b_p_bl;
	    иначе
		skip_buffer = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == NULL) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    ако (!skip_buffer && !buf->b_scanned)
		break;
	}
    }
    върни buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    static цяло
get_cpt_sources_count(char_u *cpt)
{
    char_u  dummy[LSIZE];
    цяло	    count = 0;
    char_u  *p;

    за (p = cpt; *p != NUL; )
    {
	докато (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	ако (*p != NUL)
	{
	    (void)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    count++;
	}
    }

    върни count;
}

#ifdef FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
static callback_T cfu_cb;	    // 'completefunc' callback function
static callback_T ofu_cb;	    // 'omnifunc' callback function
static callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
static callback_T *cpt_cb;	    // Callback functions associated with F{func}
static цяло	  cpt_cb_count;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    static void
copy_global_to_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    ако (globcb->cb_name != NULL && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    char *
did_set_completefunc(optset_T *args)
{
    цяло	retval;

    ако (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_cfu_cb);
    иначе
    {
	retval = option_set_callback_func(args->os_newval.string, &cfu_cb);
	ако (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_cfu_callback(curbuf);
    }

    върни retval == FAIL ? e_invalid_argument : NULL;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    void
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
    char *
did_set_omnifunc(optset_T *args)
{
    цяло	retval;

    ако (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_ofu_cb);
    иначе
    {
	retval = option_set_callback_func(args->os_newval.string, &ofu_cb);
	ако (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_ofu_callback(curbuf);
    }

    върни retval == FAIL ? e_invalid_argument : NULL;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    void
set_buflocal_ofu_callback(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    copy_global_to_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    void
clear_cpt_callbacks(callback_T **callbacks, цяло count)
{
    ако (callbacks == NULL || *callbacks == NULL)
	върни;

    за (цяло i = 0; i < count; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    static цяло
copy_cpt_callbacks(callback_T **dest, цяло *dest_cnt, callback_T *src, цяло cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    ако (cnt == 0)
	върни OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    ако (*dest == NULL)
	върни FAIL;

    *dest_cnt = cnt;

    за (цяло i = 0; i < cnt; i++)
	ако (src[i].cb_name != NULL && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    върни OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    void
set_buflocal_cpt_callbacks(buf_T *buf UNUSED)
{
# ifdef FEAT_EVAL
    ако (buf == NULL)
	върни;
    (void)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_count, cpt_cb,
	    cpt_cb_count);
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
    static цяло
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, цяло *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    цяло	    idx = 0;
    цяло	    slen;
    цяло	    count;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    count = get_cpt_sources_count(cpt);
    ако (count == 0)
	върни OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, count);
    ако (*cbp == NULL)
	върни FAIL;
    *cnt = count;

    за (p = cpt; *p != NUL; )
    {
	докато (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	ако (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ако (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		ако (caret != NULL)
		    *caret = NUL;

		ако (option_set_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = NULL;
	    }
	    idx++;
	}
    }

    върни OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    цяло
set_cpt_callbacks(optset_T *args)
{
    цяло	    opt_flags = args->os_flags;

    ако (curbuf == NULL)
	върни FAIL;

    // ":setglobal" does not change the buffer-local option value
    ако (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_count) != OK)
	върни FAIL;

    // ":setlocal" does not change the global option value
    ако (opt_flags & OPT_LOCAL)
	върни OK;

    // Cache the callbacks for the global value
    ако (opt_flags & OPT_GLOBAL)
	върни parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_count);

    // set case
    върни copy_cpt_callbacks(&cpt_cb, &cpt_cb_count, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_count);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    char *
did_set_thesaurusfunc(optset_T *args UNUSED)
{
    цяло	retval;

    ако (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    иначе
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	ако (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    върни retval == FAIL ? e_invalid_argument : NULL;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    цяло
set_ref_in_cpt_callbacks(callback_T *callbacks, цяло count, цяло copyID)
{
    цяло	abort = FALSE;

    ако (callbacks == NULL)
	върни FALSE;

    за (цяло i = 0; i < count; i++)
	abort = abort || set_ref_in_callback(&callbacks[i], copyID);
    върни abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    цяло
set_ref_in_insexpand_funcs(цяло copyID)
{
    цяло abort = set_ref_in_callback(&cfu_cb, copyID);
    abort = abort || set_ref_in_callback(&ofu_cb, copyID);
    abort = abort || set_ref_in_callback(&tsrfu_cb, copyID);
    abort = abort || set_ref_in_cpt_callbacks(cpt_cb, cpt_cb_count, copyID);

    върни abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    static char_u *
get_complete_funcname(цяло type)
{
    switch (type)
    {
	case CTRL_X_FUNCTION:
	    върни curbuf->b_p_cfu;
	case CTRL_X_OMNI:
	    върни curbuf->b_p_ofu;
	case CTRL_X_THESAURUS:
	    върни *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	default:
	    върни (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    static callback_T *
get_insert_callback(цяло type)
{
    ако (type == CTRL_X_FUNCTION)
	върни &curbuf->b_cfu_cb;
    ако (type == CTRL_X_OMNI)
	върни &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    върни (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    static void
expand_by_function(цяло type, char_u *base, callback_T *cb)
{
    list_T      *matchlist = NULL;
    dict_T	*matchdict = NULL;
    typval_T	args[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    цяло		save_State = State;
    цяло		retval;
    цяло		is_cpt_function = (cb != NULL);
    цяло		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ако (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	ако (*funcname == NUL)
	    върни;
	cb = get_insert_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    args[0].v_type = VAR_NUMBER;
    args[0].vval.v_number = 0;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = base != NULL ? base : (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    ако (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, args);
    ако (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    ако (retval == OK)
    {
	switch (rettv.v_type)
	{
	    case VAR_LIST:
		matchlist = rettv.vval.v_list;
		break;
	    case VAR_DICT:
		matchdict = rettv.vval.v_dict;
		break;
	    case VAR_SPECIAL:
		ако (rettv.vval.v_number == VVAL_NONE)
		    compl_opt_suppress_empty = TRUE;
		// FALLTHROUGH
	    default:
		// TODO: Give error message?
		clear_tv(&rettv);
		break;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ако (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	goto theend;
    }

    ако (matchlist != NULL)
	ins_compl_add_list(matchlist);
    иначе ако (matchdict != NULL)
	ins_compl_add_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    ако (matchdict != NULL)
	dict_unref(matchdict);
    ако (matchlist != NULL)
	list_unref(matchlist);
}
#endif // FEAT_COMPL_FUNC

#ако defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    static inline цяло
get_user_highlight_id(char_u *hlname)
{
    ако (hlname != NULL && *hlname != NUL)
	върни syn_check_group(hlname, (цяло)STRLEN(hlname));
    върни 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    static цяло
ins_compl_add_tv(typval_T *tv, цяло dir, цяло fast)
{
    char_u	*word;
    цяло		dup = FALSE;
    цяло		empty = FALSE;
    цяло		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_COUNT]);
    typval_T	user_data;
    цяло		status;
    char_u	*user_abbr_hlname;
    char_u	*user_kind_hlname;
    цяло		user_hl[2] = { 0, 0 };

    user_data.v_type = VAR_UNKNOWN;
    ако (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
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
	ако (dict_get_string(tv->vval.v_dict, "icase", FALSE) != NULL
				  && dict_get_number(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	ако (dict_get_string(tv->vval.v_dict, "dup", FALSE) != NULL)
	    dup = dict_get_number(tv->vval.v_dict, "dup");
	ако (dict_get_string(tv->vval.v_dict, "empty", FALSE) != NULL)
	    empty = dict_get_number(tv->vval.v_dict, "empty");
	ако (dict_get_string(tv->vval.v_dict, "equal", FALSE) != NULL
				  && dict_get_number(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    иначе
    {
	word = tv_get_string_chk(tv);
	CLEAR_FIELD(cptext);
    }
    ако (word == NULL || (!empty && *word == NUL))
    {
	clear_tv(&user_data);
	върни FAIL;
    }
    status = ins_compl_add(word, -1, NULL, cptext,
	    &user_data, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    ако (status != OK)
	clear_tv(&user_data);
    върни status;
}

/*
 * Add completions from a list.
 */
    static void
ins_compl_add_list(list_T *list)
{
    listitem_T	*li;
    цяло		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ITEMS(list, li)
    {
	ако (ins_compl_add_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	иначе ако (did_emsg)
	    break;
    }
}

/*
 * Add completions from a dict.
 */
    static void
ins_compl_add_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_words;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_find(dict, (char_u *)"refresh", 7);
    ако (di_refresh != NULL && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	ако (v != NULL && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_words = dict_find(dict, (char_u *)"words", 5);
    ако (di_words != NULL && di_words->di_tv.v_type == VAR_LIST)
	ins_compl_add_list(di_words->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    static void
set_completion(colnr_T startcol, list_T *list)
{
    цяло save_w_wrow = curwin->w_wrow;
    цяло save_w_leftcol = curwin->w_leftcol;
    цяло flags = CP_ORIGINAL_TEXT;
    unsigned цяло cur_cot_flags = get_cot_flags();
    цяло compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    цяло compl_no_insert = (cur_cot_flags & COT_NOINSERT) != 0;
    цяло compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    ако (compl_started || ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    ако (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_length = (цяло)curwin->w_cursor.col - (цяло)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text.string = vim_strnsave(ml_get_curline() + compl_col,
							(size_t)compl_length);
    ако (p_ic)
	flags |= CP_ICASE;
    ако (compl_orig_text.string == NULL)
    {
	compl_orig_text.length = 0;
	върни;
    }
    compl_orig_text.length = (size_t)compl_length;
    ако (ins_compl_add(compl_orig_text.string,
			(цяло)compl_orig_text.length, NULL, NULL, NULL, 0,
			flags | CP_FAST, FALSE, NULL, FUZZY_SCORE_NONE) != OK)
	върни;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    цяло no_select = compl_no_select || compl_longest;
    ако (compl_no_insert || no_select)
    {
	ins_complete(K_DOWN, FALSE);
	ако (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    иначе
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_insert;

    // Lazily show the popup menu, unless we got interrupted.
    ако (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}

/*
 * "complete()" function
 */
    void
f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    ако (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	върни;

    ако ((State & MODE_INSERT) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_insert_mode));
	върни;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    ако (!undo_allowed())
	върни;

    ако (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	цяло startcol = (цяло)tv_get_number_chk(&argvars[0], NULL);
	ако (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    void
f_complete_add(typval_T *argvars, typval_T *rettv)
{
    ако (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	върни;

    rettv->vval.v_number = ins_compl_add_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    void
f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    цяло save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_keys(0, TRUE);
    rettv->vval.v_number = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    static void
ins_compl_mode(string_T *ret)
{
    ако (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
    {
	string_T    *mode;

	mode = &ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];
	ret->string = (mode->string == NULL) ? (char_u *)"" : mode->string;
	ret->length = mode->length;
    }
    иначе
    {
	ret->string = (char_u *)"";
	ret->length = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    static void
ins_compl_update_sequence_numbers(void)
{
    цяло		number = 0;
    compl_T	*match;

    ако (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	за (match = compl_curr_match->cp_prev; match != NULL
		&& !is_first_match(match); match = match->cp_prev)
	    ако (match->cp_number != -1)
	    {
		number = match->cp_number;
		break;
	    }
	ако (match != NULL)
	    // go up and assign all numbers which are not assigned yet
	    за (match = match->cp_next;
		    match != NULL && match->cp_number == -1;
					   match = match->cp_next)
		match->cp_number = ++number;
    }
    иначе // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	за (match = compl_curr_match->cp_next; match != NULL
		&& !is_first_match(match); match = match->cp_next)
	{
	    ако (match->cp_number != -1)
	    {
		number = match->cp_number;
		break;
	    }
	}
	ако (match != NULL)
	{
	    // go down and assign all numbers which are not assigned yet
	    за (match = match->cp_prev; match
		    && match->cp_number == -1;
					   match = match->cp_prev)
		match->cp_number = ++number;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    static void
fill_complete_info_dict(dict_T *di, compl_T *match, цяло add_match)
{
    dict_add_string_len(di, "word", match->cp_str.string, (цяло)match->cp_str.length);
    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
    dict_add_string(di, "abbr_hlgroup", syn_id2name(match->cp_user_abbr_hl_id));
    dict_add_string(di, "kind_hlgroup", syn_id2name(match->cp_user_kind_hl_id));
    ако (add_match)
	dict_add_bool(di, "match", match->cp_in_match_array);
    ако (match->cp_user_data.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_add_string_len(di, "user_data", (char_u *)"", 0);
    иначе
	dict_add_tv(di, "user_data", &match->cp_user_data);
}

/*
 * Get complete information
 */
    static void
get_complete_info(list_T *what_list, dict_T *retdict)
{
    цяло		ret = OK;
    listitem_T	*item;
# define CI_WHAT_MODE		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_ITEMS		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_TEXT   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    цяло		what_flag;

    ако (what_list == NULL)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    иначе
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ITEMS(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    ако (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    иначе ако (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    иначе ако (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ITEMS;
	    иначе ако (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    иначе ако (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    иначе ако (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_TEXT;
	    иначе ако (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    иначе ако (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    ако (ret == OK && (what_flag & CI_WHAT_MODE))
    {
	string_T    mode;

	ins_compl_mode(&mode);
	ret = dict_add_string_len(retdict, "mode", mode.string, (цяло)mode.length);
    }

    ако (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_number(retdict, "pum_visible", pum_visible());

    ако (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_add_number(retdict, "auto", compl_autostarted);

    ако (ret == OK && (what_flag & CI_WHAT_PREINSERTED_TEXT))
    {
	char_u	*line = ml_get_curline();
	цяло	len = compl_ins_end_col - curwin->w_cursor.col;

	ret = dict_add_string_len(retdict, "preinserted_text",
		(len > 0) ? line + curwin->w_cursor.col : (char_u *)"", (len > 0) ? len : 0);
    }

    ако (ret == OK && (what_flag & (CI_WHAT_ITEMS | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = NULL;
	dict_T	    *di;
	compl_T     *match;
	цяло         selected_idx = -1;
	цяло	    has_items = what_flag & CI_WHAT_ITEMS;
	цяло	    has_matches = what_flag & CI_WHAT_MATCHES;
	цяло	    has_completed = what_flag & CI_WHAT_COMPLETED;

	ако (has_items || has_matches)
	{
	    li = list_alloc();
	    ако (li == NULL)
		върни;
	    ret = dict_add_list(retdict, (has_matches && !has_items)
						? "matches" : "items", li);
	    ако (ret == FAIL)
		list_unref(li);
	}
	ако (ret == OK && what_flag & CI_WHAT_SELECTED)
	    ако (compl_curr_match != NULL && compl_curr_match->cp_number == -1)
		ins_compl_update_sequence_numbers();
	ако (ret == OK && compl_first_match != NULL)
	{
	    цяло list_idx = 0;
	    match = compl_first_match;
	    do
	    {
		ако (!match_at_original_text(match))
		{
		    ако (has_items
			    || (has_matches && match->cp_in_match_array))
		    {
			di = dict_alloc();
			ако (di == NULL)
			    върни;
			ret = list_append_dict(li, di);
			ако (ret != OK)
			{
			    dict_unref(di);
			    върни;
			}
			fill_complete_info_dict(di, match, has_matches && has_items);
		    }
		    ако (compl_curr_match != NULL
			    && compl_curr_match->cp_number == match->cp_number)
			selected_idx = list_idx;
		    ако (!has_matches || match->cp_in_match_array)
			list_idx++;
		}
		match = match->cp_next;
	    }
	    докато (match != NULL && !is_first_match(match));
	}
	ако (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_add_number(retdict, "selected", selected_idx);

	ако (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    ако (di == NULL)
		върни;
	    fill_complete_info_dict(di, compl_curr_match, FALSE);
	    ret = dict_add_dict(retdict, "completed", di);
	    ако (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    void
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = NULL;

    ако (rettv_dict_alloc(rettv) == FAIL)
	върни;

    ако (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	върни;

    ако (argvars[0].v_type != VAR_UNKNOWN)
    {
	ако (check_for_list_arg(argvars, 0) == FAIL)
	    върни;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#endif

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    static цяло
thesaurus_func_complete(цяло type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    върни type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#иначе
    върни FALSE;
#endif
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    static цяло
may_advance_cpt_index(char_u *cpt)
{
    char_u  *p = cpt;

    ако (cpt_sources_index == -1)
	върни FALSE;
    докато (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    върни (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
enum
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_END
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
typedef struct
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_match_pos;		// current match position
    pos_T	prev_match_pos;		// previous match position
    цяло		set_match_pos;		// save first_match_pos/last_match_pos
    pos_T	first_match_pos;	// first match position
    pos_T	last_match_pos;		// last match position
    цяло		found_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    цяло		dict_f;			// "dict" is an exact file name or not
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
    static цяло
process_next_cpt_value(
	ins_compl_next_state_T *st,
	цяло		*compl_type_arg,
	pos_T		*start_match_pos,
	цяло		fuzzy_collect,
	цяло		*advance_cpt_idx)
{
    цяло	    compl_type = -1;
    цяло	    status = INS_COMPL_CPT_OK;
    цяло	    skip_source = compl_autocomplete && compl_from_nonkeyword;

    st->found_all = FALSE;
    *advance_cpt_idx = FALSE;

    докато (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    ако (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_source
	    && !compl_time_slice_expired)
    {
	st->ins_buf = curbuf;
	st->first_match_pos = *start_match_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	ако (ctrl_x_mode_normal() && (!fuzzy_collect && dec(&st->first_match_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_line_count;
	    st->first_match_pos.col = ml_get_len(st->first_match_pos.lnum);
	}
	st->last_match_pos = st->first_match_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->set_match_pos = TRUE;
    }
    иначе ако (!skip_source && !compl_time_slice_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != NULL
	    && (st->ins_buf = ins_compl_next_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	ако (st->ins_buf->b_ml.ml_mfp != NULL)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->first_match_pos.col = st->last_match_pos.col = 0;
	    st->first_match_pos.lnum = st->ins_buf->b_ml.ml_line_count + 1;
	    st->last_match_pos.lnum = 0;
	    compl_type = 0;
	}
	иначе	// unloaded buffer, scan like dictionary
	{
	    st->found_all = TRUE;
	    ако (st->ins_buf->b_fname == NULL)
	    {
		status = INS_COMPL_CPT_CONT;
		goto done;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	ако (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    msg_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((char *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == NULL
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == NULL
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (void)msg_trunc_attr((char *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    иначе ако (*st->e_cpt == NUL)
	status = INS_COMPL_CPT_END;
    иначе
    {
	ако (ctrl_x_mode_line_or_eval())
	    compl_type = -1;
#ifdef FEAT_COMPL_FUNC
	иначе ако (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_index);
	    ако (!st->func_cb)
		compl_type = -1;
	}
#endif
	иначе ако (!skip_source)
	{
	    ако (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		ако (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		иначе
		    compl_type = CTRL_X_THESAURUS;
		ако (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_FIRST;
		}
	    }
#ifdef FEAT_FIND_ID
	    иначе ако (*st->e_cpt == 'i')
		compl_type = CTRL_X_PATH_PATTERNS;
	    иначе ако (*st->e_cpt == 'd')
		compl_type = CTRL_X_PATH_DEFINES;
#endif
	    иначе ако (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_TAGS;
		ако (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    msg_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((char *)IObuff, IOSIZE, _("Scanning tags."));
		    (void)msg_trunc_attr((char *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    иначе
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(void)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_index(st->e_cpt);

	st->found_all = TRUE;
	ако (compl_type == -1)
	    status = INS_COMPL_CPT_CONT;
    }

done:
    *compl_type_arg = compl_type;
    върни status;
}

#ifdef FEAT_FIND_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    static void
get_next_include_file_completion(цяло compl_type)
{
    find_pattern_in_path(compl_pattern.string, compl_direction,
	    (цяло)compl_pattern.length, FALSE, FALSE,
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
    static void
get_next_dict_tsr_completion(цяло compl_type, char_u *dict, цяло dict_f)
{
#ifdef FEAT_COMPL_FUNC
    ако (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern.string, NULL);
    иначе
#endif
	ins_compl_dictionaries(
		dict != NULL ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern.string,
		dict != NULL ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    static void
get_next_tag_completion(void)
{
    цяло		save_p_ic;
    char_u	**matches;
    цяло		num_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_pattern.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_tag_at_cursor = TRUE;
    ако (find_tags(compl_pattern.string, &num_matches, &matches,
		TAG_REGEXP | TAG_NAMES | TAG_NOIC | TAG_INS_COMP
		| (ctrl_x_mode_not_default() ? TAG_VERBOSE : 0),
		TAG_MANY, curbuf->b_ffname) == OK && num_matches > 0)
	ins_compl_add_matches(num_matches, matches, p_ic);
    g_tag_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    static цяло
compare_scores(const void *a, const void *b)
{
    цяло idx_a = *(const цяло *)a;
    цяло idx_b = *(const цяло *)b;
    цяло score_a = compl_fuzzy_scores[idx_a];
    цяло score_b = compl_fuzzy_scores[idx_b];
    върни score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    static void
ins_compl_longest_insert(char_u *prefix)
{
    ins_compl_delete();
    ins_compl_insert_bytes(prefix + get_compl_len(), -1);
    compl_longest_end_col = curwin->w_cursor.col;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    static void
fuzzy_longest_match(void)
{
    char_u	*prefix = NULL;
    цяло		prefix_len = 0;
    цяло		i = 0;
    цяло		j = 0;
    char_u	*match_str = NULL;
    char_u	*prefix_ptr = NULL;
    char_u	*match_ptr = NULL;
    char_u	*leader = NULL;
    size_t	leader_len = 0;
    compl_T	*compl = NULL;
    цяло		more_candidates = FALSE;
    compl_T	*nn_compl = NULL;

    ако (compl_num_bests == 0)
	върни;

    nn_compl = compl_first_match->cp_next->cp_next;
    ако (nn_compl && nn_compl != compl_first_match)
	more_candidates = TRUE;

    compl = ctrl_x_mode_whole_line() ? compl_first_match
				    : compl_first_match->cp_next;
    ако (compl_num_bests == 1)
    {
	// no more candidates insert the match str
	ако (!more_candidates)
	{
	    ins_compl_longest_insert(compl->cp_str.string);
	    compl_num_bests = 0;
	}
	compl_num_bests = 0;
	върни;
    }

    ако ((size_t)compl_num_bests > SIZE_MAX / sizeof(compl_T *))
	върни;
    compl_best_matches = (compl_T **)alloc(compl_num_bests * sizeof(compl_T *));
    ако (compl_best_matches == NULL)
	върни;
    докато (compl != NULL && i < compl_num_bests)
    {
	compl_best_matches[i] = compl;
	compl = compl->cp_next;
	i++;
    }

    prefix = compl_best_matches[0]->cp_str.string;
    prefix_len = (цяло)compl_best_matches[0]->cp_str.length;

    за (i = 1; i < compl_num_bests; i++)
    {
	match_str = compl_best_matches[i]->cp_str.string;
	prefix_ptr = prefix;
	match_ptr = match_str;
	j = 0;

	докато (j < prefix_len && *match_ptr != NUL && *prefix_ptr != NUL)
	{
	    ако (STRNCMP(prefix_ptr, match_ptr, mb_ptr2len(prefix_ptr)) != 0)
		break;

	    MB_PTR_ADV(prefix_ptr);
	    MB_PTR_ADV(match_ptr);
	    j++;
	}

	ако (j > 0)
	    prefix_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    ако (leader_len > 0 && STRNCMP(prefix, leader, leader_len) != 0)
	goto end;

    prefix = vim_strnsave(prefix, prefix_len);
    ако (prefix != NULL)
    {
	ins_compl_longest_insert(prefix);
	vim_free(prefix);
    }

end:
    vim_free(compl_best_matches);
    compl_best_matches = NULL;
    compl_num_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    static void
get_next_filename_completion(void)
{
    char_u	**matches;
    цяло		num_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    цяло		i;
    цяло		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    цяло		in_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    цяло		*fuzzy_indices_data;
    char_u	*last_sep = NULL;
    цяло		need_collect_bests = in_fuzzy_collect && compl_get_longest;
    цяло		max_score = 0;
    цяло		current_score = 0;
    цяло		dir = compl_direction;

#ifdef BACKSLASH_IN_FILENAME
    char pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#иначе
    char pathsep = PATHSEP;
#endif

    ако (in_fuzzy_collect)
    {
#ifdef BACKSLASH_IN_FILENAME
	ако (curbuf->b_p_csl[0] == 's')
	{
	    за (i = 0; i < (цяло)leader_len; i++)
	    {
		ако (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	иначе ако (curbuf->b_p_csl[0] == 'b')
	{
	    за (i = 0; i < (цяло)leader_len; i++)
	    {
		ако (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#endif
	last_sep = vim_strrchr(leader, pathsep);
	ако (last_sep == NULL)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_pattern);
	    compl_pattern.string = vim_strnsave((char_u *)"*", 1);
	    ако (compl_pattern.string == NULL)
		върни;
	    compl_pattern.length = 1;
	}
	иначе ако (*(last_sep + 1) == '\0')
	    in_fuzzy_collect = FALSE;
	иначе
	{
	    // Split leader into path and file parts
	    цяло path_len = last_sep - leader + 1;
	    char_u  *path_with_wildcard;

	    path_with_wildcard = alloc(path_len + 2);
	    ако (path_with_wildcard != NULL)
	    {
		vim_snprintf((char *)path_with_wildcard, path_len + 2, "%*.*s*", path_len, path_len, leader);
		VIM_CLEAR_STRING(compl_pattern);
		compl_pattern.string = path_with_wildcard;
		compl_pattern.length = path_len + 1;

		// Move leader to the file part
		leader = last_sep + 1;
		leader_len -= path_len;
	    }
	}
    }

    ако (expand_wildcards(1, &compl_pattern.string, &num_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	върни;

    // May change home directory back to "~".
    tilde_replace(compl_pattern.string, num_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    ако (curbuf->b_p_csl[0] != NUL)
    {
	за (i = 0; i < num_matches; ++i)
	{
	    ptr = matches[i];
	    докато (*ptr != NUL)
	    {
		ако (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		иначе ако (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif

    ако (in_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, sizeof(цяло), 10);
	compl_fuzzy_scores = (цяло *)alloc(sizeof(цяло) * num_matches);

	за (i = 0; i < num_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_match_str(ptr, leader);
	    ако (score != FUZZY_SCORE_NONE)
	    {
		ако (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((цяло *)fuzzy_indices.ga_data)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	ако (fuzzy_indices.ga_len > 0)
	{
	    char_u	*match = NULL;
	    fuzzy_indices_data = (цяло *)fuzzy_indices.ga_data;
	    qsort(fuzzy_indices_data, fuzzy_indices.ga_len, sizeof(цяло), compare_scores);

	    за (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		match = matches[fuzzy_indices_data[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_data[i]];
		ако (ins_compl_add(match, -1, NULL, NULL, NULL, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, NULL, current_score) == OK)
		    dir = FORWARD;

		ако (need_collect_bests)
		{
		    ако (i == 0 || current_score == max_score)
		    {
			compl_num_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(num_matches, matches);
	}
	иначе ако (leader_len > 0)
	{
	    FreeWild(num_matches, matches);
	    num_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	ако (compl_num_bests > 0 && compl_get_longest)
	    fuzzy_longest_match();
	върни;
    }

    ако (num_matches > 0)
	ins_compl_add_matches(num_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    static void
get_next_cmdline_completion(void)
{
    char_u	**matches;
    цяло		num_matches;

    ако (expand_cmdline(&compl_xp, compl_pattern.string,
		(цяло)compl_pattern.length, &num_matches, &matches) == EXPAND_OK)
    {
	цяло		add_r = OK;
	цяло		dir = compl_direction;

	за (цяло i = 0; i < num_matches && add_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_COUNT]) = {NULL, NULL, NULL, NULL};

	    ако (compl_xp.xp_files_abbr != NULL)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    ако (compl_xp.xp_files_kind != NULL)
		cptext[CPT_KIND] = compl_xp.xp_files_kind[i];
	    ако (compl_xp.xp_files_menu != NULL)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    ако (compl_xp.xp_files_info != NULL)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    add_r = ins_compl_add(matches[i], -1, NULL, cptext, NULL, dir,
		    CP_FAST, FALSE, NULL, FUZZY_SCORE_NONE);
	    ако (add_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(num_matches, matches);
	free_xp_files_extra(&compl_xp, num_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    static void
get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    цяло		num_matches;

    num_matches = expand_spelling(lnum, compl_pattern.string, &matches);
    ако (num_matches > 0)
	ins_compl_add_matches(num_matches, matches, p_ic);
    иначе
	vim_free(matches);
#endif
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    static char_u *
ins_compl_get_next_word_or_line(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_match_pos,		// current match position
	цяло	*match_len,
	цяло	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    цяло		len;

    *match_len = 0;
    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum, FALSE) +
	cur_match_pos->col;
    len = (цяло)ml_get_buf_len(ins_buf, cur_match_pos->lnum) - cur_match_pos->col;
    ако (ctrl_x_mode_line_or_eval())
    {
	ако (compl_status_adding())
	{
	    ако (cur_match_pos->lnum >= ins_buf->b_ml.ml_line_count)
		върни NULL;
	    ptr = ml_get_buf(ins_buf, cur_match_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_match_pos->lnum + 1);
	    ако (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (цяло)(ptr - tmp_ptr);
	    }
	}
    }
    иначе
    {
	char_u	*tmp_ptr = ptr;

	ако (compl_status_adding() && compl_length <= len)
	{
	    tmp_ptr += compl_length;
	    // Skip if already inside a word.
	    ако (vim_iswordp(tmp_ptr))
		върни NULL;
	    // Find start of next word.
	    tmp_ptr = find_word_start(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = find_word_end(tmp_ptr);
	len = (цяло)(tmp_ptr - ptr);

	ако (compl_status_adding() && len == compl_length)
	{
	    ако (cur_match_pos->lnum < ins_buf->b_ml.ml_line_count)
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
		ако (tmp_ptr > ptr)
		{
		    ако (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			ако (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			ако (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== NULL
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    ако (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (цяло)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    ако (len == compl_length)
		върни NULL;
	}
    }

    *match_len = len;
    върни ptr;
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
    static цяло
get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    цяло		found_new_match = FAIL;
    цяло		save_p_scs;
    цяло		save_p_ws;
    цяло		looped_around = FALSE;
    char_u	*ptr = NULL;
    цяло		len = 0;
    цяло		in_fuzzy_collect = !compl_status_adding()
			&& cot_fuzzy() && compl_length > 0;
    char_u	*leader = ins_compl_leader();
    цяло		score = FUZZY_SCORE_NONE;
    цяло		in_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ако (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    ако (!in_curbuf)
	p_ws = FALSE;
    иначе ако (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    за (;;)
    {
	цяло	cont_s_ipos = FALSE;

	++msg_silent;  // Don't want messages for wrapscan.

	ако (in_fuzzy_collect)
	{
	    found_new_match = search_for_fuzzy_match(st->ins_buf,
			    st->cur_match_pos, leader, compl_direction,
			    start_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	иначе ако (ctrl_x_mode_whole_line() || ctrl_x_mode_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = search_for_exact_line(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern.string);
	иначе
	    found_new_match = searchit(NULL, st->ins_buf, st->cur_match_pos,
				NULL, compl_direction, compl_pattern.string, (цяло)compl_pattern.length,
				1L, SEARCH_KEEP + SEARCH_NFMSG, RE_LAST, NULL);
	--msg_silent;
	ако (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	иначе ако (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	иначе ако (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    ако (looped_around)
		found_new_match = FAIL;
	    иначе
		looped_around = TRUE;
	}
	иначе ако (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    ако (looped_around)
		found_new_match = FAIL;
	    иначе
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	ако (found_new_match == FAIL)
	    break;

	// when ADDING, the text before the cursor matches, skip it
	ако (compl_status_adding() && in_curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    continue;

	ако (!in_fuzzy_collect)
	    ptr = ins_compl_get_next_word_or_line(st->ins_buf,
		    st->cur_match_pos, &len, &cont_s_ipos);
	ако (ptr == NULL || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    continue;

	ако (is_nearest_active() && in_curbuf)
	{
	    score = st->cur_match_pos->lnum - curwin->w_cursor.lnum;
	    ако (score < 0)
		score = -score;
	}

	ако (ins_compl_add_infercase(ptr, len, p_ic,
			in_curbuf ? NULL : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    ако (in_fuzzy_collect && score == compl_first_match->cp_next->cp_score)
		compl_num_bests++;
	    found_new_match = OK;
	    break;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    върни found_new_match;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    static callback_T *
get_callback_if_cpt_func(char_u *p, цяло idx)
{
    ако (*p == 'o')
	върни &curbuf->b_ofu_cb;

    ако (*p == 'F')
    {
	ако (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    ако (curbuf->b_p_cpt_cb == NULL
		    || idx < 0 || idx >= curbuf->b_p_cpt_count)
		върни NULL;
	    върни curbuf->b_p_cpt_cb[idx].cb_name != NULL
		? &curbuf->b_p_cpt_cb[idx] : NULL;
	}
	иначе
	    върни &curbuf->b_cfu_cb; // 'cfu'
    }

    върни NULL;
}
#endif

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    static void
get_register_completion(void)
{
    цяло		dir = compl_direction;
    yankreg_T	*reg = NULL;
    void	*reg_ptr = NULL;
    цяло		adding_mode = compl_status_adding();

    за (цяло i = 0; i < NUM_REGISTERS; i++)
    {
	цяло regname = 0;
	ако (i == 0)
	    regname = '"';    // unnamed register
	иначе ако (i < 10)
	    regname = '0' + i;
	иначе ако (i == DELETION_REGISTER)
	    regname = '-';
#ifdef FEAT_CLIPBOARD
	иначе ако (i == STAR_REGISTER)
	    regname = '*';
	иначе ако (i == PLUS_REGISTER)
	    regname = '+';
#endif
	иначе
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	ако (!valid_yank_reg(regname, FALSE) || regname == '_')
	    continue;

	reg_ptr = get_register(regname, FALSE);
	ако (reg_ptr == NULL)
	    continue;

	reg = (yankreg_T *)reg_ptr;

	за (цяло j = 0; j < reg->y_size; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    ако (str == NULL)
		continue;

	    ако (adding_mode)
	    {
		цяло str_len = (цяло)STRLEN(str);
		ако (str_len == 0)
		    continue;

		ако (!compl_orig_text.string
			|| (p_ic ? STRNICMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0
				: STRNCMP(str, compl_orig_text.string,
					    compl_orig_text.length) == 0))
		{
		    ако (ins_compl_add_infercase(str, str_len, p_ic, NULL,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    иначе
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_end = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		докато (p < str_end && *p != NUL)
		{
		    char_u *old_p = p;
		    p = find_word_start(p);
		    ако (p >= str_end || *p == NUL)
			break;

		    char_u *word_end = find_word_end(p);

		    ако (word_end <= p)
		    {
			ако (has_mbyte)
			    word_end = p + (*mb_ptr2len)(p);
			иначе
			    word_end = p + 1;
		    }

		    ако (word_end > str_end)
			word_end = str_end;

		    цяло len = (цяло)(word_end - p);
		    ако (len > 0 && (!compl_orig_text.string
				|| (p_ic ? STRNICMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0
					: STRNCMP(p, compl_orig_text.string,
						    compl_orig_text.length) == 0)))
		    {
			ако (ins_compl_add_infercase(p, len, p_ic, NULL,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = word_end;

		    ако (p <= old_p)
		    {
			p = old_p + 1;
			ако (has_mbyte && p < str_end)
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
    static цяло
get_next_completion_match(цяло type, ins_compl_next_state_T *st, pos_T *ini)
{
    цяло	found_new_match = FALSE;

    switch (type)
    {
	case -1:
	    break;
#ifdef FEAT_FIND_ID
	case CTRL_X_PATH_PATTERNS:
	case CTRL_X_PATH_DEFINES:
	    get_next_include_file_completion(type);
	    break;
#endif

	case CTRL_X_DICTIONARY:
	case CTRL_X_THESAURUS:
	    get_next_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = NULL;
	    break;

	case CTRL_X_TAGS:
	    get_next_tag_completion();
	    break;

	case CTRL_X_FILES:
	    get_next_filename_completion();
	    break;

	case CTRL_X_CMDLINE:
	case CTRL_X_CMDLINE_CTRL_X:
	    get_next_cmdline_completion();
	    break;

#ifdef FEAT_COMPL_FUNC
	case CTRL_X_FUNCTION:
	    ако (ctrl_x_mode_normal())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    иначе
		expand_by_function(type, compl_pattern.string, NULL);
	    break;
	case CTRL_X_OMNI:
	    expand_by_function(type, compl_pattern.string, NULL);
	    break;
#endif

	case CTRL_X_SPELL:
	    get_next_spell_completion(st->first_match_pos.lnum);
	    break;

	case CTRL_X_REGISTER:
	    get_register_completion();
	    break;

	default:	// normal ^P/^N and ^X^L
	    found_new_match = get_next_default_completion(st, ini);
	    ако (found_new_match == FAIL && st->ins_buf == curbuf)
		st->found_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    ако (type != 0 && compl_curr_match != compl_old_match)
	found_new_match = OK;

    върни found_new_match;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    static void
strip_caret_numbers_in_place(char_u *str)
{
    char_u  *read = str, *write = str, *p;

    ако (str == NULL)
	върни;

    докато (*read)
    {
	ако (*read == '^')
	{
	    p = read + 1;
	    докато (vim_isdigit(*p))
		p++;
	    ако ((*p == ',' || *p == '\0') && p != read + 1)
	    {
		read = p;
		continue;
	    }
	    иначе
		*write++ = *read++;
	}
	иначе
	    *write++ = *read++;
    }
    *write = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    static цяло
prepare_cpt_compl_funcs(void)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = NULL;
    цяло		idx = 0;
    цяло		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    ако (cpt == NULL)
	върни FAIL;
    strip_caret_numbers_in_place(cpt);

    за (p = cpt; *p;)
    {
	докато (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ако (*p == NUL)
	    break;

	cb = get_callback_if_cpt_func(p, idx);
	ако (cb)
	{
	    ако (get_userdefined_compl_info(curwin->w_cursor.col, cb, &startcol)
		    == FAIL)
	    {
		ако (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		иначе
		    startcol = -2;
	    }
	    иначе ако (startcol < 0 || startcol > curwin->w_cursor.col)
		startcol = curwin->w_cursor.col;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	иначе
	    cpt_sources_array[idx].cs_startcol = -3;

	(void)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    върни OK;
#endif
    върни FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    static void
compl_source_start_timer(цяло source_idx UNUSED)
{
#ifdef ELAPSED_FUNC
    ако (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[source_idx].compl_start_tv);
	compl_time_slice_expired = FALSE;
    }
#endif
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    static цяло
advance_cpt_sources_index_safe(void)
{
    ако (cpt_sources_index >= 0 && cpt_sources_index < cpt_sources_count - 1)
    {
	cpt_sources_index++;
	върни OK;
    }
#ifdef FEAT_EVAL
    semsg(_(e_list_index_out_of_range_nr), cpt_sources_index);
#endif
    върни FAIL;
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
    static цяло
ins_compl_get_exp(pos_T *ini)
{
    static ins_compl_next_state_T   st;
    static цяло			    st_cleared = FALSE;
    цяло		match_count;
    цяло		found_new_match;
    цяло		type = ctrl_x_mode;
    цяло		may_advance_cpt_idx = FALSE;
    pos_T	start_pos = *ini;
    цяло		normal_mode_strict = FALSE;
    long	compl_timeout_save = 0;

    ако (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	ако (!st_cleared)
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
	st.e_cpt = st.e_cpt_copy == NULL ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#define LOOKBACK_LINE_COUNT	1000
	ако (compl_autocomplete && is_nearest_active())
	{
	    start_pos.lnum = MAX(1, start_pos.lnum - LOOKBACK_LINE_COUNT);
	    start_pos.col = 0;
	}
	st.last_match_pos = st.first_match_pos = start_pos;
    }
    иначе ако (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_match = compl_curr_match;	// remember the last current match
    st.cur_match_pos = (compl_dir_forward())
				    ? &st.last_match_pos : &st.first_match_pos;

    normal_mode_strict = ctrl_x_mode_normal() && !ctrl_x_mode_line_or_eval()
	&& !(compl_cont_status & CONT_LOCAL) && cpt_sources_array != NULL;
    ако (normal_mode_strict)
    {
	cpt_sources_index = 0;
	ако (compl_autocomplete || p_cto > 0)
	{
	    compl_source_start_timer(0);
	    compl_time_slice_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    за (;;)
    {
	found_new_match = FAIL;
	st.set_match_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	ако ((ctrl_x_mode_normal() || ctrl_x_mode_line_or_eval())
					&& (!compl_started || st.found_all))
	{
	    цяло status = process_next_cpt_value(&st, &type, &start_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    ако (status == INS_COMPL_CPT_END)
		break;
	    ако (status == INS_COMPL_CPT_CONT)
	    {
		ако (may_advance_cpt_idx)
		{
		    ако (!advance_cpt_sources_index_safe())
			break;
		    compl_source_start_timer(cpt_sources_index);
		}
		continue;
	    }
	}

	ако (normal_mode_strict && type == CTRL_X_FUNCTION
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
	ако (compl_pattern.string == NULL)
	    break;

	ако (may_advance_cpt_idx)
	{
	    ако (!advance_cpt_sources_index_safe())
		break;
	    compl_source_start_timer(cpt_sources_index);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	// match
	ако ((ctrl_x_mode_not_default() && !ctrl_x_mode_line_or_eval())
						|| found_new_match != FAIL)
	{
	    ако (got_int)
		break;
	    // Fill the popup menu as soon as possible.
	    ако (type != -1)
		ins_compl_check_keys(0, FALSE);

	    ако ((ctrl_x_mode_not_default()
			&& !ctrl_x_mode_line_or_eval()) || compl_interrupted)
		break;
	    compl_started = compl_time_slice_expired ? FALSE : TRUE;
	}
	иначе
	{
	    // Mark a buffer scanned when it has been scanned completely
	    ако (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_PATH_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	ако (normal_mode_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	ако (!compl_dir_forward())
	    докато (compl_curr_match->cp_prev
		    && !match_at_original_text(compl_curr_match->cp_prev))
		compl_curr_match = compl_curr_match->cp_prev;
    }
    cpt_sources_index = -1;
    compl_started = TRUE;

    ако ((ctrl_x_mode_normal() || ctrl_x_mode_line_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	found_new_match = FAIL;

    match_count = -1;		// total of matches, unknown
    ако (found_new_match == FAIL || (ctrl_x_mode_not_default()
		&& !ctrl_x_mode_line_or_eval()))
	match_count = ins_compl_make_cyclic();

    ако (cot_fuzzy() && compl_get_longest && compl_num_bests > 0)
	fuzzy_longest_match();

    ако (compl_old_match != NULL)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_match = compl_dir_forward() ? compl_old_match->cp_next
						    : compl_old_match->cp_prev;
	ако (compl_curr_match == NULL)
	    compl_curr_match = compl_old_match;
    }
    may_trigger_modechanged();

    ако (match_count > 0 && !ctrl_x_mode_spell())
    {
	ако (is_nearest_active() && !ins_compl_has_preinsert())
	    sort_compl_match_list(cp_compare_nearest);

	ако (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_sort();
    }

    върни match_count;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    static void
ins_compl_update_shown_match(void)
{
    string_T	*leader;

    (void)get_leader_for_startcol(NULL, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_match, TRUE);

    докато (!ins_compl_equal(compl_shown_match,
		leader->string, (цяло)leader->length)
	    && compl_shown_match->cp_next != NULL
	    && !is_first_match(compl_shown_match->cp_next))
    {
	compl_shown_match = compl_shown_match->cp_next;
	leader = get_leader_for_startcol(compl_shown_match, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    ако (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		leader->string, (цяло)leader->length)
	    && (compl_shown_match->cp_next == NULL
		|| is_first_match(compl_shown_match->cp_next)))
    {
	докато (!ins_compl_equal(compl_shown_match,
		    leader->string, (цяло)leader->length)
		&& compl_shown_match->cp_prev != NULL
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
    void
ins_compl_delete(void)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    цяло col = compl_col + (compl_status_adding() ? compl_length : 0);
    string_T	remaining = {NULL, 0};
    цяло	    orig_col;

    ако (ins_compl_preinsert_effect())
    {
	col += (цяло)ins_compl_leader_len();
	curwin->w_cursor.col = compl_ins_end_col;
    }

    ако (curwin->w_cursor.lnum > compl_lnum)
    {
	ако (curwin->w_cursor.col < ml_get_curline_len())
	{
	    char_u *line = ml_get_cursor();
	    remaining.length = ml_get_cursor_len();
	    remaining.string = vim_strnsave(line, remaining.length);
	    ако (remaining.string == NULL)
		върни;
	}
	докато (curwin->w_cursor.lnum > compl_lnum)
	{
	    ако (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		ако (remaining.string)
		    vim_free(remaining.string);
		върни;
	    }
	    deleted_lines_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.col = ml_get_curline_len();
    }

    ако ((цяло)curwin->w_cursor.col > col)
    {
	ако (stop_arrow() == FAIL)
	{
	    ако (remaining.string)
		vim_free(remaining.string);
	    върни;
	}
	backspace_until_column(col);
	compl_ins_end_col = curwin->w_cursor.col;
    }

    ако (remaining.string != NULL)
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
    static void
ins_compl_expand_multiple(char_u *str)
{
    char_u	*start = str;
    char_u	*curr = str;
    цяло		base_indent = get_indent();

    докато (*curr != NUL)
    {
	ако (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    ако (curr > start)
		ins_char_bytes(start, (цяло)(curr - start));

	    // Handle newline
	    open_line(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, base_indent, NULL);
	    start = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    ако (curr > start)
	ins_char_bytes(start, (цяло)(curr - start));

    compl_ins_end_col = curwin->w_cursor.col;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    static char_u *
find_common_prefix(size_t *prefix_len, цяло curbuf_only)
{
    compl_T     *compl;
    цяло		*match_count;
    char_u	*first = NULL;
    цяло		len = -1;
    цяло		is_cpt_completion = (cpt_sources_array != NULL);

    ако (!is_cpt_completion)
	върни NULL;

    match_count = ALLOC_CLEAR_MULT(цяло, cpt_sources_count);
    ако (match_count == NULL)
	върни NULL;

    (void)get_leader_for_startcol(NULL, TRUE); // Clear the cache

    compl = compl_first_match;
    do
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	ако (ctrl_x_mode_normal() && !p_inf && compl_orig_text.string
		&& !ignorecase(compl_orig_text.string))
	    compl->cp_flags &= ~CP_ICASE;

	ако (!match_at_original_text(compl)
		&& (leader->string == NULL
		    || ins_compl_equal_sc(compl, leader->string,
			(цяло)leader->length)))
	{
	    // Limit number of items from each source if max_items is set.
	    цяло match_limit_exceeded = FALSE;
	    цяло cur_source = compl->cp_cpt_source_idx;

	    ако (cur_source != -1)
	    {
		match_count[cur_source]++;
		цяло max_matches = cpt_sources_array[cur_source].cs_max_matches;
		ако (max_matches > 0 && match_count[cur_source] > max_matches)
		    match_limit_exceeded = TRUE;
	    }

	    ако (!match_limit_exceeded && (!curbuf_only
			|| (cur_source != -1
			    && cpt_sources_array[cur_source].cs_flag == '.')))
	    {
		ако (first == NULL && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    first = compl->cp_str.string;
		    len = (цяло)STRLEN(first);
		}
		иначе ако (first != NULL)
		{
		    цяло j = 0;  // count in bytes
		    char_u *s1 = first;
		    char_u *s2 = compl->cp_str.string;

		    докато (j < len && *s1 != NUL && *s2 != NUL)
		    {
			ако (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    break;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    ако (len == 0)
			break;
		}
	    }
	}
	compl = compl->cp_next;
    } докато (compl != NULL && !is_first_match(compl));

    vim_free(match_count);

    ако (len > (цяло)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	ако (len == (цяло)STRLEN(first))
	{
	    char_u *line = ml_get_curline();
	    char_u *p = line + curwin->w_cursor.col;
	    ако (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *end = find_word_end(p);
		цяло text_len = end - p;
		ако (text_len > 0
			&& text_len < (len - (цяло)ins_compl_leader_len())
			&& STRNCMP(first + len - text_len, p, text_len) == 0)
		    len -= text_len;
	    }
	}
	*prefix_len = (size_t)len;
	върни first;
    }
    върни NULL;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    void
ins_compl_insert(цяло move_cursor, цяло insert_prefix)
{
    цяло		compl_len = get_compl_len();
    цяло		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_match->cp_str.string;
    size_t	cp_str_len = compl_shown_match->cp_str.length;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_multiple = vim_strchr(cp_str, '\n');

    ако (insert_prefix)
    {
	cp_str = find_common_prefix(&cp_str_len, FALSE);
	ако (cp_str == NULL)
	{
	    cp_str = find_common_prefix(&cp_str_len, TRUE);
	    ако (cp_str == NULL)
	    {
		cp_str = compl_shown_match->cp_str.string;
		cp_str_len = compl_shown_match->cp_str.length;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    иначе ако (cpt_sources_array != NULL)
    {
	цяло	cpt_idx = compl_shown_match->cp_cpt_source_idx;
	ако (cpt_idx >= 0 && compl_col >= 0)
	{
	    цяло	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    ако (startcol >= 0 && startcol < (цяло)compl_col)
	    {
		цяло skip = (цяло)compl_col - startcol;
		ако ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    ако (compl_len < (цяло)cp_str_len)
    {
	ако (has_multiple)
	    ins_compl_expand_multiple(cp_str + compl_len);
	иначе
	{
	    ins_compl_insert_bytes(cp_str + compl_len,
		    insert_prefix ? (цяло)cp_str_len - compl_len : -1);
	    ако ((preinsert || insert_prefix) && move_cursor)
		curwin->w_cursor.col -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    ако (match_at_original_text(compl_shown_match)
	    || (preinsert && !insert_prefix))
	compl_used_match = FALSE;
    иначе
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    compl_hi_on_autocompl_longest = insert_prefix && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    static void
ins_compl_show_filename(void)
{
    char	*lead = _("match in file");
    цяло		space = sc_col - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    ако (space <= 0)
	върни;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    за (s = e = compl_shown_match->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	докато (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    ако (!compl_autocomplete)
    {
	msg_hist_off = TRUE;
	vim_snprintf((char *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_match->cp_fname ? "<" : "", s);
	msg((char *)IObuff);
	msg_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    static compl_T *
find_next_match_in_menu(void)
{
    цяло	    is_forward = compl_shows_dir_forward();
    compl_T *match = compl_shown_match;

    do
	match = is_forward ? match->cp_next : match->cp_prev;
    докато (match->cp_next && !match->cp_in_match_array
	    && !match_at_original_text(match));
    върни match;
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
    static цяло
find_next_completion_match(
	цяло	allow_get_expansion,
	цяло	todo,		// repeat completion this many times
	цяло	advance,
	цяло	*num_matches)
{
    цяло		found_end = FALSE;
    compl_T	*found_compl = NULL;
    unsigned цяло cur_cot_flags = get_cot_flags();
    цяло		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    докато (--todo >= 0)
    {
	ако (compl_shows_dir_forward() && compl_shown_match->cp_next != NULL)
	{
	    ако (compl_match_array != NULL)
		compl_shown_match = find_next_match_in_menu();
	    иначе
		compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != NULL
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	иначе ако (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != NULL)
	{
	    found_end = is_first_match(compl_shown_match);
	    ако (compl_match_array != NULL)
		compl_shown_match = find_next_match_in_menu();
	    иначе
		compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	иначе
	{
	    ако (!allow_get_expansion)
	    {
		ако (advance)
		{
		    ако (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    иначе
			compl_pending += todo + 1;
		}
		върни -1;
	    }

	    ако (!compl_no_select && advance)
	    {
		ако (compl_shows_dir_backward())
		    --compl_pending;
		иначе
		    ++compl_pending;
	    }

	    // Find matches.
	    *num_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    докато (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		ако (compl_pending > 0 && compl_shown_match->cp_next != NULL)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		иначе ако (compl_pending < 0 && compl_shown_match->cp_prev != NULL)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		иначе
		    break;
	    }
	    found_end = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_match, FALSE);

	ако (!match_at_original_text(compl_shown_match)
		&& leader->string != NULL
		&& !ins_compl_equal(compl_shown_match,
		    leader->string, (цяло)leader->length)
		&& !(cot_fuzzy()
		    && compl_shown_match->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	иначе
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	ако (found_end)
	{
	    ако (found_compl != NULL)
	    {
		compl_shown_match = found_compl;
		break;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    върни OK;
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
    static цяло
ins_compl_next(
    цяло	    allow_get_expansion,
    цяло	    count,		// repeat completion this many times; should
				// be at least 1
    цяло	    insert_match)	// Insert the newly selected match
{
    цяло	    num_matches = -1;
    цяло	    todo = count;
    цяло	    advance;
    цяло	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    unsigned цяло cur_cot_flags = get_cot_flags();
    цяло	    compl_no_insert = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    цяло	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    ако (compl_shown_match == NULL)
	върни -1;

    ако (compl_leader.string != NULL
	    && !match_at_original_text(compl_shown_match)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_match();

    ако (allow_get_expansion && insert_match
	    && (!compl_get_longest || compl_used_match))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = count != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    ако (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    ако (find_next_completion_match(allow_get_expansion, todo, advance,
							&num_matches) == -1)
	върни -1;

    ако (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	върни -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    ако (!started && ins_compl_preinsert_longest())
	ins_compl_insert(TRUE, TRUE);
    иначе ако (compl_no_insert && !started && !compl_preinsert)
    {
	ins_compl_insert_bytes(compl_orig_text.string + get_compl_len(), -1);
	compl_used_match = FALSE;
    }
    иначе ако (insert_match)
    {
	ако (!compl_get_longest || compl_used_match)
	{
	    цяло preinsert_longest = ins_compl_preinsert_longest()
		&& match_at_original_text(compl_shown_match); // none selected
	    ins_compl_insert(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	иначе
	    ins_compl_insert_bytes(compl_leader.string + get_compl_len(), -1);
    }
    иначе
	compl_used_match = FALSE;

    ако (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	ако (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	иначе
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ifdef FEAT_GUI
	ако (gui.in_use)
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
    ако (compl_no_insert && !started
				 && !match_at_original_text(compl_shown_match))
	compl_enter_selects = TRUE;
    иначе
	compl_enter_selects = !insert_match && compl_match_array != NULL;

    // Show the file name for the match (if any)
    ако (compl_shown_match->cp_fname != NULL)
	ins_compl_show_filename();

    върни num_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    static void
check_elapsed_time(void)
{
#ifdef ELAPSED_FUNC
    elapsed_T	*start_tv
	= &cpt_sources_array[cpt_sources_index].compl_start_tv;
    long	elapsed_ms = ELAPSED_FUNC(*start_tv);

    ако (elapsed_ms > compl_timeout_ms)
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
    void
ins_compl_check_keys(цяло frequency, цяло in_compl_func)
{
    static цяло	count = 0;
    цяло		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    ако (!in_compl_func && (using_script() || ex_normal_busy))
	върни;

    // Only do this at regular intervals
    ако (++count < frequency)
	върни;
    count = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    ако (c != NUL
#ifdef FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#endif
       )
    {
	ако (vim_is_ctrl_x_key(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = safe_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (void)ins_compl_next(FALSE, ins_compl_key2count(c),
				      c != K_UP && c != K_DOWN);
	}
	иначе
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = safe_vgetc();
	    ако (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		ако (c != Ctrl_R && (KeyTyped
			    || (in_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    иначе
    {
	цяло normal_mode_strict = ctrl_x_mode_normal()
	    && !ctrl_x_mode_line_or_eval() && !(compl_cont_status & CONT_LOCAL)
	    && cpt_sources_array != NULL && cpt_sources_index >= 0;
	ако (normal_mode_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_time();
    }

    ако (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	цяло todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(void)ins_compl_next(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    static цяло
ins_compl_key2dir(цяло c)
{
    ако (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	върни BACKWARD;
    върни FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    static цяло
ins_compl_pum_key(цяло c)
{
    върни pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_DOWN
		     || c == K_UP || c == K_DOWN);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    static цяло
ins_compl_key2count(цяло c)
{
    ако (ins_compl_pum_key(c) && c != K_UP && c != K_DOWN)
    {
	цяло h = pum_get_height();
	ако (h > 3)
	    h -= 2; // keep some context
	върни h;
    }
    върни 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    static цяло
ins_compl_use_match(цяло c)
{
    switch (c)
    {
	case K_UP:
	case K_DOWN:
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
	case K_S_DOWN:
	case K_PAGEUP:
	case K_KPAGEUP:
	case K_S_UP:
	    върни FALSE;
    }
    върни TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    static цяло
get_normal_compl_info(char_u *line, цяло startcol, colnr_T curs_col)
{
    ако ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	ако (!compl_status_adding())
	{
	    докато (--startcol >= 0 && vim_isIDc(line[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	ако (p_ic)
	{
	    compl_pattern.string = str_foldcase(line + compl_col,
		    compl_length, NULL, 0);
	    ако (compl_pattern.string == NULL)
	    {
		compl_pattern.length = 0;
		върни FAIL;
	    }
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	иначе
	{
	    compl_pattern.string = vim_strnsave(line + compl_col, (size_t)compl_length);
	    ако (compl_pattern.string == NULL)
	    {
		compl_pattern.length = 0;
		върни FAIL;
	    }
	    compl_pattern.length = (size_t)compl_length;
	}
    }
    иначе ако (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	ако (!vim_iswordp(line + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(line, line + compl_col)))))
	{
	    prefix = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(NULL, line + compl_col, compl_length) + prefixlen;
	compl_pattern.string = alloc(n);
	ако (compl_pattern.string == NULL)
	{
	    compl_pattern.length = 0;
	    върни FAIL;
	}
	STRCPY((char *)compl_pattern.string, prefix);
	(void)quote_meta(compl_pattern.string + prefixlen,
		line + compl_col, compl_length);
	compl_pattern.length = n - 1;
    }
    иначе ако (--startcol < 0
	    || !vim_iswordp(mb_prevptr(line, line + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_pattern.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	ако (compl_pattern.string == NULL)
	{
	    compl_pattern.length = 0;
	    върни FAIL;
	}
	compl_pattern.length = len;
	compl_col += curs_col;
	compl_length = 0;
	compl_from_nonkeyword = TRUE;
    }
    иначе
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	ако (has_mbyte)
	{
	    цяло base_class;
	    цяло head_off;

	    startcol -= (*mb_head_off)(line, line + startcol);
	    base_class = mb_get_class(line + startcol);
	    докато (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(line, line + startcol);
		ако (base_class != mb_get_class(line + startcol
			    - head_off))
		    break;
		startcol -= head_off;
	    }
	}
	иначе
	{
	    докато (--startcol >= 0 && vim_iswordc(line[startcol]))
		;
	}

	compl_col += ++startcol;
	compl_length = (цяло)curs_col - startcol;
	ако (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern.string = alloc(7);
	    ако (compl_pattern.string == NULL)
	    {
		compl_pattern.length = 0;
		върни FAIL;
	    }
	    STRCPY((char *)compl_pattern.string, "\\<");
	    (void)quote_meta(compl_pattern.string + 2, line + compl_col, 1);
	    STRCAT((char *)compl_pattern.string, "\\k");
	    compl_pattern.length = STRLEN(compl_pattern.string);
	}
	иначе
	{
	    size_t  n = quote_meta(NULL, line + compl_col, compl_length) + 2;

	    compl_pattern.string = alloc(n);
	    ако (compl_pattern.string == NULL)
	    {
		compl_pattern.length = 0;
		върни FAIL;
	    }
	    STRCPY((char *)compl_pattern.string, "\\<");
	    (void)quote_meta(compl_pattern.string + 2, line + compl_col,
		    compl_length);
	    compl_pattern.length = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    ако (ctrl_x_mode_normal() && !(compl_cont_status & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	ако (setup_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    върни FAIL;
    }

    върни OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    static цяло
get_wholeline_compl_info(char_u *line, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(line);
    compl_length = (цяло)curs_col - (цяло)compl_col;
    ако (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    ако (p_ic)
    {
	compl_pattern.string = str_foldcase(line + compl_col, compl_length,
		NULL, 0);
	ако (compl_pattern.string == NULL)
	{
	    compl_pattern.length = 0;
	    върни FAIL;
	}
	compl_pattern.length = STRLEN(compl_pattern.string);
    }
    иначе
    {
	compl_pattern.string = vim_strnsave(line + compl_col, (size_t)compl_length);
	ако (compl_pattern.string == NULL)
	{
	    compl_pattern.length = 0;
	    върни FAIL;
	}
	compl_pattern.length = (size_t)compl_length;
    }

    върни OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    static цяло
get_filename_compl_info(char_u *line, цяло startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    ако (startcol > 0)
    {
	char_u	*p = line + startcol;

	MB_PTR_BACK(line, p);
	докато (p > line && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(line, p);
	ако (p == line && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	иначе
	    startcol = (цяло)(p - line) + 1;
    }

    compl_col += startcol;
    compl_length = (цяло)curs_col - startcol;
    compl_pattern.string = addstar(line + compl_col, compl_length, EXPAND_FILES);
    ако (compl_pattern.string == NULL)
    {
	compl_pattern.length = 0;
	върни FAIL;
    }

    compl_pattern.length = STRLEN(compl_pattern.string);

    върни OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    static цяло
get_cmdline_compl_info(char_u *line, colnr_T curs_col)
{
    compl_pattern.string = vim_strnsave(line, curs_col);
    ако (compl_pattern.string == NULL)
    {
	compl_pattern.length = 0;
	върни FAIL;
    }
    compl_pattern.length = curs_col;
    set_cmd_context(&compl_xp, compl_pattern.string,
	    (цяло)compl_pattern.length, curs_col, FALSE);
    ако (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    иначе
	compl_col = (цяло)(compl_xp.xp_pattern - compl_pattern.string);
    compl_length = curs_col - compl_col;

    върни OK;
}

#ifdef FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    static цяло
set_compl_globals(
    цяло		startcol UNUSED,
    colnr_T	curs_col UNUSED,
    цяло		is_cpt_compl UNUSED)
{
    ако (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_pattern);
	ако (startcol < compl_col)
	    върни prepend_startcol_text(&cpt_compl_pattern, &compl_orig_text,
		    startcol);
	иначе
	{
	    cpt_compl_pattern.string = vim_strnsave(compl_orig_text.string,
		    compl_orig_text.length);
	    cpt_compl_pattern.length = compl_orig_text.length;
	}
    }
    иначе
    {
	ако (startcol < 0 || startcol > curs_col)
	    startcol = curs_col;

	// Re-obtain line in case it has changed
	char_u	*line = ml_get(curwin->w_cursor.lnum);
	цяло	len = curs_col - startcol;

	compl_pattern.string = vim_strnsave(line + startcol, (size_t)len);
	ако (compl_pattern.string == NULL)
	{
	    compl_pattern.length = 0;
	    върни FAIL;
	}
	compl_pattern.length = (size_t)len;
	compl_col = startcol;
	compl_length = len;
    }

    върни OK;
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
    static цяло
get_userdefined_compl_info(
    colnr_T	curs_col UNUSED,
    callback_T	*cb UNUSED,
    цяло		*startcol UNUSED)
{
    цяло		ret = FAIL;

#ifdef FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	args[3];
    цяло		col;
    char_u	*funcname = NULL;
    pos_T	pos;
    цяло		save_State = State;
    цяло		is_cpt_function = (cb != NULL);
    цяло		use_sandbox = is_cpt_function
			    && was_set_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ако (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_mode);
	ако (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_set), ctrl_x_mode_function()
		    ? "completefunc" : "omnifunc");
	    върни FAIL;
	}
	cb = get_insert_callback(ctrl_x_mode);
    }

    args[0].v_type = VAR_NUMBER;
    args[0].vval.v_number = 1;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = (char_u *)"";
    args[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    ако (use_sandbox)
	++sandbox;
    col = call_callback_retnr(cb, 2, args);
    ако (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ако (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_text));
	върни FAIL;
    }

    ако (startcol != NULL)
	*startcol = col;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    ако (col == -2 || aborting())
	върни FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    ако (col == -3)
    {
	ако (is_cpt_function)
	    върни FAIL;
	ctrl_x_mode = CTRL_X_NORMAL;
	edit_submode = NULL;
	ако (!shortmess(SHM_COMPLETIONMENU))
	    msg_clr_cmdline();
	върни FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_empty = FALSE;

    ret = !is_cpt_function ? set_compl_globals(col, curs_col, FALSE) : OK;
#endif

    върни ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    static цяло
get_spell_compl_info(цяло startcol UNUSED, colnr_T curs_col UNUSED)
{
    цяло		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*line = NULL;

    ако (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    иначе
	compl_col = spell_word_start(startcol);

    ако (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    иначе
    {
	spell_expand_check_cap(compl_col);
	compl_length = (цяло)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    line = ml_get(curwin->w_cursor.lnum);
    compl_pattern.string = vim_strnsave(line + compl_col, (size_t)compl_length);
    ако (compl_pattern.string == NULL)
    {
	compl_pattern.length = 0;
	върни FAIL;
    }

    compl_pattern.length = (size_t)compl_length;
    ret = OK;
#endif

    върни ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    static цяло
compl_get_info(char_u *line, цяло startcol, colnr_T curs_col, цяло *line_invalid)
{
    ако (ctrl_x_mode_normal() || ctrl_x_mode_register()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	ако (get_normal_compl_info(line, startcol, curs_col) != OK)
	    върни FAIL;
	*line_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    иначе ако (ctrl_x_mode_line_or_eval())
    {
	върни get_wholeline_compl_info(line, curs_col);
    }
    иначе ако (ctrl_x_mode_files())
    {
	върни get_filename_compl_info(line, startcol, curs_col);
    }
    иначе ако (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	върни get_cmdline_compl_info(line, curs_col);
    }
    иначе ако (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	ако (get_userdefined_compl_info(curs_col, NULL, NULL) != OK)
	    върни FAIL;
	*line_invalid = TRUE;	// "line" may have become invalid
    }
    иначе ако (ctrl_x_mode_spell())
    {
	ако (get_spell_compl_info(startcol, curs_col) == FAIL)
	    върни FAIL;
	*line_invalid = TRUE;	// "line" may have become invalid
    }
    иначе
    {
	internal_error("ins_complete()");
	върни FAIL;
    }

    върни OK;
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
    static void
ins_compl_continue_search(char_u *line)
{
    // it is a continued search
    compl_cont_status &= ~CONT_INTRPT;	// remove INTRPT
    ако (ctrl_x_mode_normal() || ctrl_x_mode_path_patterns()
						|| ctrl_x_mode_path_defines())
    {
	ако (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_col = (colnr_T)getwhitecols(line);
	    compl_startpos.col = compl_col;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_status &= ~CONT_SOL;   // clear SOL if present
	}
	иначе
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    ако (compl_cont_status & CONT_S_IPOS)
	    {
		compl_cont_status |= CONT_SOL;
		compl_startpos.col = (colnr_T)(skipwhite(
			    line + compl_length
			    + compl_startpos.col) - line);
	    }
	    compl_col = compl_startpos.col;
	}
	compl_length = curwin->w_cursor.col - (цяло)compl_col;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#define	MIN_SPACE 75
	ако (compl_length > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_status &= ~CONT_SOL;
	    compl_length = (IOSIZE - MIN_SPACE);
	    compl_col = curwin->w_cursor.col - compl_length;
	}
	compl_cont_status |= CONT_ADDING | CONT_N_ADDS;
	ако (compl_length < 1)
	    compl_cont_status &= CONT_LOCAL;
    }
    иначе ако (ctrl_x_mode_line_or_eval() || ctrl_x_mode_register())
	compl_cont_status = CONT_ADDING | CONT_N_ADDS;
    иначе
	compl_cont_status = 0;
}

/*
 * start insert mode completion
 */
    static цяло
ins_compl_start(void)
{
    char_u	*line = NULL;
    цяло		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    цяло		line_invalid = FALSE;
    цяло		save_did_ai = did_ai;
    цяло		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    ако (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	върни FAIL;
    }

    line = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    ако ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_search(line);
    иначе
	compl_cont_status &= CONT_LOCAL;

    ако (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	ако (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (цяло)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    ако (compl_get_info(line, startcol, curs_col, &line_invalid) == FAIL)
    {
	ако (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	върни FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    ако (line_invalid)
	line = ml_get(curwin->w_cursor.lnum);

    ако (compl_status_adding())
    {
	ако (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	ако (ctrl_x_mode_line_or_eval())
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
    иначе
    {
	edit_submode_pre = NULL;
	compl_startpos.col = compl_col;
    }

    ако (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	ако (compl_cont_status & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_MSG]);
	иначе
	    edit_submode = (char_u *)_(CTRL_X_MSG(ctrl_x_mode));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(NULL);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_text);
    compl_orig_text.length = (size_t)compl_length;
    compl_orig_text.string = vim_strnsave(line + compl_col, (size_t)compl_length);
    ако (p_ic)
	flags |= CP_ICASE;
    ако (compl_orig_text.string == NULL
	    || ins_compl_add(compl_orig_text.string,
		(цяло)compl_orig_text.length,
		NULL, NULL, NULL, 0, flags, FALSE, NULL, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_pattern);
	VIM_CLEAR_STRING(compl_orig_text);
	did_ai = save_did_ai;
	върни FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    ако (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_COUNT;
	showmode();
	edit_submode_extra = NULL;
	out_flush();
    }

    did_ai = save_did_ai;
    върни OK;
}

/*
 * display the completion status message
 */
    static void
ins_compl_show_statusmsg(void)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    ако (is_first_match(compl_first_match->cp_next))
    {
	edit_submode_extra = compl_status_adding() && compl_length > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    ако (edit_submode_extra == NULL)
    {
	ако (match_at_original_text(compl_curr_match))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	иначе ако (compl_cont_status & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_COUNT;
	}
	иначе ако (compl_curr_match->cp_next == compl_curr_match->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_COUNT;
	    compl_curr_match->cp_number = 1;
	}
	иначе
	{
#ако defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    ако (compl_curr_match->cp_number == -1)
		ins_compl_update_sequence_numbers();
#endif
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    ако (compl_curr_match->cp_number != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		static char_u match_ref[81];

		ако (compl_matches > 0)
		    vim_snprintf((char *)match_ref, sizeof(match_ref),
			    _("match %d of %d"),
			    compl_curr_match->cp_number, compl_matches);
		иначе
		    vim_snprintf((char *)match_ref, sizeof(match_ref),
			    _("match %d"),
			    compl_curr_match->cp_number);
		edit_submode_extra = match_ref;
		edit_submode_highl = HLF_R;
		ако (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    ако (!compl_opt_suppress_empty)
    {
	showmode();
	ако (!shortmess(SHM_COMPLETIONMENU))
	{
	    ако (edit_submode_extra != NULL)
	    {
		ако (!p_smd)
		{
		    msg_hist_off = TRUE;
		    msg_attr((char *)edit_submode_extra,
			    edit_submode_highl < HLF_COUNT
			    ? HL_ATTR(edit_submode_highl) : 0);
		    msg_hist_off = FALSE;
		}
	    }
	    иначе
		msg_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    цяло
ins_complete(цяло c, цяло enable_pum)
{
    цяло		n;
    цяло		save_w_wrow;
    цяло		save_w_leftcol;
    цяло		insert_match;
    цяло		no_matches_found;

    compl_direction = ins_compl_key2dir(c);
    insert_match = ins_compl_use_match(c);

    ако (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	ако (ins_compl_start() == FAIL)
	    върни FAIL;
    }
    иначе ако (insert_match && stop_arrow() == FAIL)
	върни FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_buffer;
    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;
    compl_num_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), insert_match);

    // Reset autocompletion timer expiry flag
    ако (compl_autocomplete)
	compl_time_slice_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    ако (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    ако (got_int && !global_busy)
    {
	(void)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_found = is_first_match(compl_first_match->cp_next);
    ако (no_matches_found)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	ако (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    ако (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    иначе
	compl_cont_status &= ~CONT_S_IPOS;

    ако (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    ако (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    върни OK;
}

/*
 * Enable autocompletion
 */
    void
ins_compl_enable_autocomplete(void)
{
#ifdef ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#endif
}

/*
 * Disable autocompletion
 */
    void
ins_compl_disable_autocomplete(void)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    void
ins_compl_arm_autostart(void)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    void
ins_compl_disarm_autostart(void)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(void)
{
#ifdef ELAPSED_FUNC
    ако (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_start_tv);
	compl_autocomplete_pending = true;
	върни true;
    }
#endif
    върни false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    void
ins_compl_clear_autocomplete_delay(void)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(void)
{
    върни compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    long
ins_compl_autocomplete_elapsed(void)
{
#ifdef ELAPSED_FUNC
    върни ELAPSED_FUNC(compl_autocomplete_start_tv);
#иначе
    върни 0;
#endif
}

/*
 * Remove (if needed) and show the popup menu
 */
    static void
show_pum(цяло prev_w_wrow, цяло prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    цяло save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    ако (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    static unsigned
quote_meta(char_u *dest, char_u *src, цяло len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    за ( ; --len >= 0; src++)
    {
	switch (*src)
	{
	    case '.':
	    case '*':
	    case '[':
		ако (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    break;
		// FALLTHROUGH
	    case '~':
		ако (!magic_isset())	// quote these only if magic is set
		    break;
		// FALLTHROUGH
	    case '\\':
		ако (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    break;
		// FALLTHROUGH
	    case '^':		// currently it's not needed.
	    case '$':
		m++;
		ако (dest != NULL)
		    *dest++ = '\\';
		break;
	}
	ако (dest != NULL)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	ако (has_mbyte)
	{
	    цяло mb_len = (*mb_ptr2len)(src) - 1;
	    ако (mb_len > 0 && len >= mb_len)
		за (цяло i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    ако (dest != NULL)
			*dest++ = *src;
		}
	}
    }
    ако (dest != NULL)
	*dest = NUL;

    върни m;
}

#ако defined(EXITFREE)
    void
free_insexpand_stuff(void)
{
    VIM_CLEAR_STRING(compl_orig_text);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_count);
# endif
}
#endif

#ifdef FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    static void
spell_back_to_badword(void)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, SMT_ALL, TRUE, NULL);
    ако (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}
#endif

/*
 * Reset the info associated with completion sources.
 */
    static void
cpt_sources_clear(void)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_index = -1;
    cpt_sources_count = 0;
}

/*
 * Setup completion sources.
 */
    static цяло
setup_cpt_sources(void)
{
    char_u  buf[LSIZE];
    цяло	    slen;
    цяло	    idx = 0;
    цяло	    count;
    char_u  *p;

    cpt_sources_clear();

    count = get_cpt_sources_count(curbuf->b_p_cpt);
    ако (count == 0)
	върни OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_source_T, count);
    ако (cpt_sources_array == NULL)
	върни FAIL;

    за (p = curbuf->b_p_cpt; *p;)
    {
	докато (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ако (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ако (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		ако (caret != NULL)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((char *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_count = count;
    върни OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    static цяло
is_cpt_func_refresh_always(void)
{
#ifdef FEAT_COMPL_FUNC
    за (цяло i = 0; i < cpt_sources_count; i++)
	ако (cpt_sources_array[i].cs_refresh_always)
	    върни TRUE;
#endif
    върни FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    static void
ins_compl_make_linear(void)
{
    compl_T *m;

    ако (compl_first_match == NULL || compl_first_match->cp_prev == NULL)
	върни;
    m = compl_first_match->cp_prev;
    m->cp_next = NULL;
    compl_first_match->cp_prev = NULL;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ifdef FEAT_COMPL_FUNC
    static void
remove_old_matches(void)
{
    compl_T *current;
    цяло	    shown_match_removed = FALSE;
    цяло	    forward = (compl_first_match->cp_cpt_source_idx < 0);

    ако (cpt_sources_index < 0)
	върни;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    за (current = compl_first_match; current != NULL; )
    {
	ако (current->cp_cpt_source_idx == cpt_sources_index)
	{
	    compl_T *to_delete = current;

	    ако (!shown_match_removed && compl_shown_match == current)
		shown_match_removed = TRUE;

	    current = current->cp_next;

	    ако (to_delete == compl_first_match)  // node to remove is at head
	    {
		compl_first_match = to_delete->cp_next;
		compl_first_match->cp_prev = NULL;
	    }
	    иначе ако (to_delete->cp_next == NULL) // node to remove is at tail
		to_delete->cp_prev->cp_next = NULL;
	    иначе // node is in the moddle
	    {
		to_delete->cp_prev->cp_next = to_delete->cp_next;
		to_delete->cp_next->cp_prev = to_delete->cp_prev;
	    }
	    ins_compl_item_free(to_delete);
	}
	иначе
	    current = current->cp_next;
    }

    // Re-assign compl_shown_match if necessary
    ако (shown_match_removed)
    {
	ако (forward)
	    compl_shown_match = compl_first_match;
	иначе
	{  // Last node will have the prefix that is being completed
	    за (current = compl_first_match; current->cp_next != NULL;
		    current = current->cp_next)
		;
	    compl_shown_match = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_match = compl_first_match;
    за (current = compl_first_match; current != NULL; )
    {
	ако ((forward ? current->cp_cpt_source_idx < cpt_sources_index
		    : current->cp_cpt_source_idx > cpt_sources_index))
	{
	    compl_curr_match = forward ? current : current->cp_next;
	    current = current->cp_next;
	}
	иначе
	    break;
    }
}
#endif

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ifdef FEAT_COMPL_FUNC
    static void
get_cpt_func_completion_matches(callback_T *cb UNUSED)
{
    cpt_source_T    *cpt_src = &cpt_sources_array[cpt_sources_index];
    цяло		    startcol = cpt_src->cs_startcol;

    ако (startcol == -2 || startcol == -3)
	върни;

    ако (set_compl_globals(startcol, curwin->w_cursor.col, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	ако (!cpt_src->cs_refresh_always)
	    ins_compl_insert_bytes(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_pattern.string, cb);

	ако (!cpt_src->cs_refresh_always)
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
    static void
cpt_compl_refresh(void)
{
#ifdef FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = NULL;
    цяло		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_linear();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_in_place(cpt);

    cpt_sources_index = 0;
    за (p = cpt; *p;)
    {
	докато (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ако (*p == NUL)
	    break;

	ако (cpt_sources_array[cpt_sources_index].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_index);
	    ако (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.col, cb,
			&startcol);
		ако (ret == FAIL)
		{
		    ако (startcol == -3)
			cpt_sources_array[cpt_sources_index].cs_refresh_always
			    = FALSE;
		    иначе
			startcol = -2;
		}
		иначе ако (startcol < 0 || startcol > curwin->w_cursor.col)
		    startcol = curwin->w_cursor.col;
		cpt_sources_array[cpt_sources_index].cs_startcol = startcol;
		ако (ret == OK)
		{
		    compl_source_start_timer(cpt_sources_index);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(void)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	ако (may_advance_cpt_index(p))
	    (void)advance_cpt_sources_index_safe();
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
    void
f_preinserted(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ако (ins_compl_preinsert_effect())
	rettv->vval.v_number = 1;
}
