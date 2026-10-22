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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Definitions used for CTRL-X submode.
 * Note: If you change CTRL-X submode, you must also maintain ctrl_x_msgs[] and
 * ctrl_x_mode_names[] below.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_WANT_IDENT	0x100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਸਾਧਾਰਨ		0  // CTRL-N CTRL-P completion, default
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_NOT_DEFINED_YET	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_SCROLL		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਸਾਰਾ_ਸਤਰ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_FILES		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਟੈਗ_ਸੂਚੀ		(5 + CTRL_X_WANT_IDENT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਰਸਤਾ_PATTERNS	(6 + CTRL_X_WANT_IDENT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ	(7 + CTRL_X_WANT_IDENT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਮੁਕੰਮਲ		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_DICTIONARY	(9 + CTRL_X_WANT_IDENT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_THESAURUS	(10 + CTRL_X_WANT_IDENT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_CMDLINE		11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_FUNCTION	12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_OMNI		13
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_SPELL		14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_LOCAL_ਸੁਨੇਹਾ	15	// only used in "ctrl_x_msgs"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_EVAL		16	// for builtin function complete()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_CMDLINE_CTRL_X	17	// CTRL-X typed in CTRL_X_CMDLINE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਰਜਿਸਟਰ	18	// complete words from registers

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_X_ਸੁਨੇਹਾ(i) ctrl_x_msgs[(i) & ~CTRL_X_WANT_IDENT]

// Message for CTRL-X mode, index is ctrl_x_mode.
ਸਥਿਰ ਅੱਖਰ *ctrl_x_msgs[] =
{
    N_(" Keyword completion (^N^P)"), // CTRL_X_NORMAL, ^P/^N compl.
    N_(" ^X mode (^]^D^E^F^I^K^L^N^O^P^Rs^U^V^Y)"),
    ਨਿਰਾ, // CTRL_X_SCROLL: depends on state
    N_(" Whole line completion (^L^N^P)"),
    N_(" File name completion (^F^N^P)"),
    N_(" Tag completion (^]^N^P)"),
    N_(" Path pattern completion (^N^P)"),
    N_(" Definition completion (^D^N^P)"),
    ਨਿਰਾ, // CTRL_X_FINISHED
    N_(" Dictionary completion (^K^N^P)"),
    N_(" Thesaurus completion (^T^N^P)"),
    N_(" Command-line completion (^V^N^P)"),
    N_(" User defined completion (^U^N^P)"),
    N_(" Omni completion (^O^N^P)"),
    N_(" Spelling suggestion (^S^N^P)"),
    N_(" Keyword Local completion (^N^P)"),
    ਨਿਰਾ,   // CTRL_X_EVAL doesn't use msg.
    N_(" Command-line completion (^V^N^P)"),
    N_(" Register completion (^N^P)"),
};

#ਜੇ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ਸਥਿਰ string_T ctrl_x_ਬਹੁਲਕ_ਨਾਂ[] = {
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
    {ਨਿਰਾ, 0},		    // CTRL_X_LOCAL_MSG is only used in "ctrl_x_msgs"
    STR_LITERAL_INIT("eval"),
    STR_LITERAL_INIT("cmdline"),
    STR_LITERAL_INIT("register"),
};
#ਜੇ_ਅੰਤ

/*
 * Structure used to store one match for insert completion.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ compl_S compl_T;
ਬਣਤਰ compl_S
{
    compl_T	*cp_ਅਗਲਾ;
    compl_T	*cp_prev;
    compl_T	*cp_ਮੇਲ_ਅਗਲਾ;		// matched next compl_T
    string_T	cp_str;			// matched text
    char_u	*(cp_ਪਾਠ[CPT_ਗਿਣਤੀ]);	// text for the menu
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    typval_T	cp_user_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
    char_u	*cp_fname;		// file containing the match, allocated when
					// cp_flags has CP_FREE_FNAME
    ਪੂਰਨ		cp_flags;		// CP_ values
    ਪੂਰਨ		cp_ਸੰਖਿਆ;		// sequence number
    ਪੂਰਨ		cp_ਕ੍ਰਮ;		// list position when first scored
    ਪੂਰਨ		cp_score;		// fuzzy match score or proximity score
    ਪੂਰਨ		cp_ਵਿੱਚ_ਮੇਲ_array;	// collected by compl_match_array
    ਪੂਰਨ		cp_user_abbr_hl_id;	// highlight group ID for abbr
    ਪੂਰਨ		cp_user_ਕਿਸਮ_hl_id;	// highlight group ID for kind
    ਪੂਰਨ		cp_cpt_ਸਰੋਤ_idx;	// index of this match's source in 'cpt' option
};

// values for cp_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CP_ORIGINAL_ਪਾਠ   1	// the original text when the expansion begun
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CP_FREE_FNAME	    2	// cp_fname is allocated
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CP_CONT_S_IPOS	    4	// use CONT_S_IPOS for compl_cont_status
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CP_EQUAL	    8	// ins_compl_equal() always returns TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CP_ICASE	    16	// ins_compl_equal() ignores case
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CP_FAST	    32	// use fast_breakcheck instead of ui_breakcheck

/*
 * All the current matches are stored in a list.
 * "compl_first_match" points to the start of the list.
 * "compl_curr_match" points to the currently selected entry.
 * "compl_shown_match" is different from compl_curr_match during
 * ins_compl_get_exp(), when new matches are added to the list.
 * "compl_old_match" points to previous "compl_curr_match".
 */
ਸਥਿਰ compl_T    *compl_ਪਹਿਲਾ_ਮੇਲ = ਨਿਰਾ;
ਸਥਿਰ compl_T    *compl_curr_ਮੇਲ = ਨਿਰਾ;
ਸਥਿਰ compl_T    *compl_shown_ਮੇਲ = ਨਿਰਾ;
ਸਥਿਰ compl_T    *compl_old_ਮੇਲ = ਨਿਰਾ;

// Hashtab with the strings of the matches in the list above, except the
// original-text entries.  Each entry owns a copy of the string and counts
// the matches with that string, so that when matches were added with "adup"
// the entry remains until the last match with the string is removed.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    cse_ਗਿਣਤੀ;	// number of matches with this string
    char_u  cse_str[1];	// the string, actually longer
} complstr_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSE_OFF		((ਪੂਰਨ)offsetof(complstr_T, cse_str))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2CSE(hi)	((complstr_T *)((hi)->hi_ਕੁੰਜੀ - CSE_OFF))

ਸਥਿਰ hashtab_T  compl_ਲੜੀਆਂ_ht;

/*
 * Count the string of a new match in the duplicate-check hashtab.
 * "hash" is the hash of "str" when it is not zero.
 */
    ਸਥਿਰ ਖਾਲੀ
compl_ਲੜੀਆਂ_ਜੋੜੋ(char_u *str, size_t len, hash_T hash)
{
    hashitem_T	*hi;
    complstr_T	*ਇੰਦਰਾਜ;

    ਜੇ (compl_ਲੜੀਆਂ_ht.ht_array == ਨਿਰਾ)
	hash_init(&compl_ਲੜੀਆਂ_ht);
    ਜੇ (hash == 0)
	hash = hash_hash(str);
    hi = hash_lookup(&compl_ਲੜੀਆਂ_ht, str, hash);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	ਇੰਦਰਾਜ = alloc(CSE_OFF + len + 1);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	    // Out of memory: the duplicate check degrades, an equal string
	    // added later is not recognized as a duplicate.
	    ਵਾਪਸ;
	ਇੰਦਰਾਜ->cse_ਗਿਣਤੀ = 1;
	vim_strncpy(ਇੰਦਰਾਜ->cse_str, str, len);
	ਜੇ (hash_ਜੋੜੋ_ਵਸਤੂ(&compl_ਲੜੀਆਂ_ht, hi, ਇੰਦਰਾਜ->cse_str, hash)
								      == FAIL)
	    vim_free(ਇੰਦਰਾਜ);
    }
    ਨਹੀਂਤਾਂ
	++HI2CSE(hi)->cse_ਗਿਣਤੀ;
}

// list used to store the compl_T which have the max score
ਸਥਿਰ compl_T	  **compl_ਉੱਤਮ_matches = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	  compl_ਅੰਕ_bests = 0;

// After using a cursor key <Enter> selects a match in the popup menu,
// otherwise it inserts a line break.
ਸਥਿਰ ਪੂਰਨ	  compl_enter_selects = FALSE;

// When "compl_leader" is not NULL only matches that start with this string
// are used.
ਸਥਿਰ string_T	  compl_leader = {ਨਿਰਾ, 0};

ਸਥਿਰ ਪੂਰਨ	  compl_get_longest = FALSE;	// put longest common string
						// in compl_leader

// This flag is FALSE when no match is selected (by ^N/^P) or the match was
// edited or using the longest common string.
ਸਥਿਰ ਪੂਰਨ	  compl_ਵਰਤਿਆ_ਮੇਲ;

// didn't finish finding completions.
ਸਥਿਰ ਪੂਰਨ	  compl_was_interrupted = FALSE;

// Set when character typed while looking for matches and it means we should
// stop looking for matches.
ਸਥਿਰ ਪੂਰਨ	  compl_interrupted = FALSE;

ਸਥਿਰ ਪੂਰਨ	  compl_restarting = FALSE;	// don't insert match

// When the first completion is done "compl_started" is set.  When it's
// FALSE the word to be completed must be located.
ਸਥਿਰ ਪੂਰਨ	  compl_started = FALSE;

// Which Ctrl-X mode are we in?
ਸਥਿਰ ਪੂਰਨ	  ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਧਾਰਨ;

ਸਥਿਰ ਪੂਰਨ	  compl_matches = 0;	    // number of completion matches
ਸਥਿਰ string_T	  compl_ਨਮੂਨਾ = {ਨਿਰਾ, 0};	 // search pattern for matching items
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
ਸਥਿਰ string_T	  cpt_compl_ਨਮੂਨਾ = {ਨਿਰਾ, 0}; // pattern returned by func in 'cpt'
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	  compl_direction = FORWARD;
ਸਥਿਰ ਪੂਰਨ	  compl_shows_dir = FORWARD;
ਸਥਿਰ ਪੂਰਨ	  compl_pending = 0;	    // > 1 for postponed CTRL-N
ਸਥਿਰ pos_T	  compl_startpos;
// Length in bytes of the text being completed (this is deleted to be replaced
// by the match.)
ਸਥਿਰ ਪੂਰਨ	  compl_ਲੰਮਾਈ = 0;
ਸਥਿਰ linenr_T	  compl_lnum = 0;           // lnum where the completion start
ਸਥਿਰ colnr_T	  compl_ਖਾਨਾ = 0;	    // column where the text starts
					    // that is being completed
ਸਥਿਰ colnr_T	  compl_ins_ਸਿਰਾ_ਖਾਨਾ = 0;
ਸਥਿਰ colnr_T	  compl_longest_ਸਿਰਾ_ਖਾਨਾ = 0;  // end of 'longest' inserted text
ਸਥਿਰ string_T	  compl_orig_ਪਾਠ = {ਨਿਰਾ, 0};  // text as it was before
					    // completion started
ਸਥਿਰ ਪੂਰਨ	  compl_cont_ਬਹੁਲਕ = 0;
ਸਥਿਰ expand_T	  compl_xp;

ਸਥਿਰ win_T	  *compl_curr_win = ਨਿਰਾ;  // win where completion is active
ਸਥਿਰ buf_T	  *compl_curr_buf = ਨਿਰਾ;  // buf where completion is active

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPL_INITIAL_TIMEOUT_MS    80
// Autocomplete uses a decaying timeout: starting from COMPL_INITIAL_TIMEOUT_MS,
// if the current source exceeds its timeout, it is interrupted and the next
// begins with half the time. A small minimum timeout ensures every source
// gets at least a brief chance.
// Special case: when 'complete' contains "F" or "o" (function sources), a
// longer fixed timeout is used (COMPL_FUNC_TIMEOUT_MS or
// COMPL_FUNC_TIMEOUT_NON_KW_MS). - girish
ਸਥਿਰ ਪੂਰਨ	  compl_autocomplete = FALSE;	    // whether autocompletion is active
ਸਥਿਰ bool	  compl_autostarted = false;	    // Vim started this completion
ਸਥਿਰ bool	  compl_autostart_pending = false;  // the trigger armed one
ਸਥਿਰ bool	  compl_autocomplete_pending = false;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
ਸਥਿਰ elapsed_T  compl_autocomplete_ਸ਼ੁਰੂ_tv;	    // when the delay was armed
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	  compl_timeout_ms = COMPL_INITIAL_TIMEOUT_MS;
ਸਥਿਰ ਪੂਰਨ	  compl_ਸਮਾਂ_ਟੁਕੜਾ_expired = FALSE; // time budget exceeded for current source
ਸਥਿਰ ਪੂਰਨ	  compl_from_nonkeyword = FALSE;    // completion started from non-keyword
ਸਥਿਰ ਪੂਰਨ	  compl_hi_on_autocompl_longest = FALSE;    // apply "PreInsert" highlight

// Halve the current completion timeout, simulating exponential decay.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPL_MIN_TIMEOUT_MS	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECAY_COMPL_TIMEOUT() \
    ਕਰੋ { \
	ਜੇ (compl_timeout_ms > COMPL_MIN_TIMEOUT_MS) \
	    compl_timeout_ms /= 2; \
    } ਜਦੋਂ (0)

// Timeout values for F{func}, F and o values in 'complete'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPL_FUNC_TIMEOUT_MS		300
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPL_FUNC_TIMEOUT_NON_KW_MS	1000

// List of flags for method of completion.
ਸਥਿਰ ਪੂਰਨ	  compl_cont_ਸਥਿਤੀ = 0;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONT_ADDING	1	// "normal" or "adding" expansion
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONT_INTRPT	(2 + 4)	// a ^X interrupted the current expansion
				// it's set only iff N_ADDS is set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONT_N_ADDS	4	// next ^X<> will add-new or expand-current
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONT_S_IPOS	8	// next ^X<> will set initial_pos?
				// if so, word-wise-expansion will set SOL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONT_SOL	16	// pattern includes start of line, just for
				// word-wise expansion, not set for ^X^L
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONT_LOCAL	32	// for ctrl_x_mode 0, ^X^P/^X^N do a local
				// expansion, (eg use complete=.)

ਸਥਿਰ ਪੂਰਨ	  compl_opt_refresh_always = FALSE;
ਸਥਿਰ ਪੂਰਨ	  compl_opt_suppress_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;

ਸਥਿਰ ਪੂਰਨ	  compl_selected_ਵਸਤੂ = -1;

ਸਥਿਰ ਪੂਰਨ	  *compl_fuzzy_scores;

// Define the structure for completion source (in 'cpt' option) information
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cpt_ਸਰੋਤ_T
{
    ਪੂਰਨ	cs_refresh_always;  // Whether 'refresh:always' is set for func
    ਪੂਰਨ	cs_startcol;	    // Start column returned by func
    ਪੂਰਨ	cs_max_matches;	    // Max items to display from this source
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T	compl_ਸ਼ੁਰੂ_tv;	    // Timestamp when match collection starts
#ਜੇ_ਅੰਤ
    char_u  cs_flag;	    // flag indicating the type of source
} cpt_ਸਰੋਤ_T;

ਸਥਿਰ cpt_ਸਰੋਤ_T *cpt_sources_array; // Pointer to the array of completion sources
ਸਥਿਰ ਪੂਰਨ	    cpt_sources_ਗਿਣਤੀ;  // Total number of completion sources specified in the 'cpt' option
ਸਥਿਰ ਪੂਰਨ	    cpt_sources_ਸੂਚਕਾਂਕ = -1;  // Index of the current completion source being expanded

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ਸਥਿਰ pumitem_T *compl_ਮੇਲ_array = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ compl_ਮੇਲ_arraysize;

ਸਥਿਰ ਪੂਰਨ ins_compl_ਜੋੜੋ(char_u *str, ਪੂਰਨ len, char_u *fname, char_u **cptext, typval_T *user_ਅੰਕੜੇ, ਪੂਰਨ cdir, ਪੂਰਨ flags, ਪੂਰਨ adup, ਪੂਰਨ *user_hl, ਪੂਰਨ score);
ਸਥਿਰ ਖਾਲੀ ins_compl_longest_ਮੇਲ(compl_T *ਮੇਲ);
ਸਥਿਰ ਖਾਲੀ ins_compl_del_pum(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_compl_files(ਪੂਰਨ ਗਿਣਤੀ_2, char_u **files, ਪੂਰਨ thesaurus, ਪੂਰਨ flags, regmatch_T *regmatch, char_u *buf, ਪੂਰਨ *dir);
ਸਥਿਰ ਖਾਲੀ ins_compl_free(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  ins_compl_need_restart(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_compl_new_leader(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  get_compl_len(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_compl_restart(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_compl_ਸੈੱਟ_original_ਪਾਠ(char_u *str, size_t len);
ਸਥਿਰ ਖਾਲੀ ins_compl_fixRedoBufForLeader(char_u *ptr_arg);
#ਜੇ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
ਸਥਿਰ ਖਾਲੀ ins_compl_ਜੋੜੋ_list(list_T *list);
ਸਥਿਰ ਖਾਲੀ ins_compl_ਜੋੜੋ_dict(dict_T *dict);
ਸਥਿਰ ਪੂਰਨ get_userdefined_compl_info(colnr_T curs_ਖਾਨਾ, callback_T *cb, ਪੂਰਨ *startcol);
ਸਥਿਰ ਖਾਲੀ get_cpt_func_completion_matches(callback_T *cb);
ਸਥਿਰ callback_T *get_callback_if_cpt_func(char_u *p, ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ fill_complete_info_dict(dict_T *di, compl_T *ਮੇਲ, ਪੂਰਨ ਜੋੜੋ_ਮੇਲ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ਤਿਆਰੀ_cpt_sources(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ is_cpt_func_refresh_always(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ cpt_sources_clear(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ cpt_compl_refresh(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  ins_compl_key2dir(ਪੂਰਨ c);
ਸਥਿਰ ਪੂਰਨ  ins_compl_pum_ਕੁੰਜੀ(ਪੂਰਨ c);
ਸਥਿਰ ਪੂਰਨ  ins_compl_key2count(ਪੂਰਨ c);
ਸਥਿਰ ਖਾਲੀ show_pum(ਪੂਰਨ prev_w_wrow, ਪੂਰਨ prev_w_leftcol);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ  quote_meta(char_u *dest, char_u *str, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ ins_compl_has_ਗੁਣਜ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_compl_expand_ਗੁਣਜ(char_u *str);
ਸਥਿਰ ਖਾਲੀ ins_compl_longest_ਪਾਓ(char_u *ਅਗੇਤਰ);
ਸਥਿਰ ਖਾਲੀ ins_compl_make_ਰੇਖਿਕ(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ ins_compl_make_cyclic(ਖਾਲੀ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
ਸਥਿਰ ਖਾਲੀ spell_ਪਿੱਠ_ਮੰਜ਼ਿਲ_badword(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  spell_bad_len = 0;	// length of located bad word
#ਜੇ_ਅੰਤ

/*
 * CTRL-X pressed in Insert mode.
 */
    ਖਾਲੀ
ins_ctrl_x(ਖਾਲੀ)
{
    ਜੇ (!ctrl_x_ਬਹੁਲਕ_cmdline())
    {
	// if the next ^X<> won't ADD nothing, then reset compl_cont_status
	ਜੇ (compl_cont_ਸਥਿਤੀ & CONT_N_ADDS)
	    compl_cont_ਸਥਿਤੀ |= CONT_INTRPT;
	ਨਹੀਂਤਾਂ
	    compl_cont_ਸਥਿਤੀ = 0;
	// We're not sure which CTRL-X mode it will be yet
	ctrl_x_ਬਹੁਲਕ = CTRL_X_NOT_DEFINED_YET;
	edit_submode = (char_u *)_(CTRL_X_ਸੁਨੇਹਾ(ctrl_x_ਬਹੁਲਕ));
	edit_submode_pre = ਨਿਰਾ;
	showmode();
    }
    ਨਹੀਂਤਾਂ
	// CTRL-X in CTRL-X CTRL-V mode behaves differently to make CTRL-X
	// CTRL-V look like CTRL-N
	ctrl_x_ਬਹੁਲਕ = CTRL_X_CMDLINE_CTRL_X;

    may_trigger_modechanged();
}

/*
 * Functions to check the current CTRL-X mode.
 */
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_none(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == 0; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਸਾਧਾਰਨ; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_scroll(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_SCROLL; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਸਾਰਾ_ਸਤਰ(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਸਾਰਾ_ਸਤਰ; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_files(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_FILES; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਟੈਗ_ਸੂਚੀ(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਟੈਗ_ਸੂਚੀ; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_patterns(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਰਸਤਾ_PATTERNS; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_dictionary(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_DICTIONARY; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_thesaurus(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_THESAURUS; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_cmdline(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_CMDLINE
		|| ctrl_x_ਬਹੁਲਕ == CTRL_X_CMDLINE_CTRL_X; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_function(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_FUNCTION; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_omni(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_OMNI; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_spell(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_SPELL; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_eval(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_EVAL; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਸਾਰਾ_ਸਤਰ || ctrl_x_ਬਹੁਲਕ == CTRL_X_EVAL; }
ਪੂਰਨ ctrl_x_ਬਹੁਲਕ_ਰਜਿਸਟਰ(ਖਾਲੀ)
    { ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_ਰਜਿਸਟਰ; }

/*
 * Whether other than default completion has been selected.
 */
    ਪੂਰਨ
ctrl_x_ਬਹੁਲਕ_not_default(ਖਾਲੀ)
{
    ਵਾਪਸ ctrl_x_ਬਹੁਲਕ != CTRL_X_ਸਾਧਾਰਨ;
}

/*
 * Whether CTRL-X was typed without a following character,
 * not including when in CTRL-X CTRL-V mode.
 */
    ਪੂਰਨ
ctrl_x_ਬਹੁਲਕ_not_defined_yet(ਖਾਲੀ)
{
    ਵਾਪਸ ctrl_x_ਬਹੁਲਕ == CTRL_X_NOT_DEFINED_YET;
}

/*
 * Return TRUE if currently in "normal" or "adding" insert completion matches
 * state
 */
    ਪੂਰਨ
compl_ਸਥਿਤੀ_adding(ਖਾਲੀ)
{
    ਵਾਪਸ compl_cont_ਸਥਿਤੀ & CONT_ADDING;
}

/*
 * Return TRUE if the completion pattern includes start of line, just for
 * word-wise expansion.
 */
    ਪੂਰਨ
compl_ਸਥਿਤੀ_sol(ਖਾਲੀ)
{
    ਵਾਪਸ compl_cont_ਸਥਿਤੀ & CONT_SOL;
}

/*
 * Return TRUE if ^X^P/^X^N will do a local completion (i.e. use complete=.)
 */
    ਪੂਰਨ
compl_ਸਥਿਤੀ_local(ਖਾਲੀ)
{
    ਵਾਪਸ compl_cont_ਸਥਿਤੀ & CONT_LOCAL;
}

/*
 * Clear the completion status flags
 */
    ਖਾਲੀ
compl_ਸਥਿਤੀ_clear(ਖਾਲੀ)
{
    compl_cont_ਸਥਿਤੀ = 0;
}

/*
 * Return TRUE if completion is using the forward direction matches
 */
    ਸਥਿਰ ਪੂਰਨ
compl_dir_forward(ਖਾਲੀ)
{
    ਵਾਪਸ compl_direction == FORWARD;
}

/*
 * Return TRUE if currently showing forward completion matches
 */
    ਸਥਿਰ ਪੂਰਨ
compl_shows_dir_forward(ਖਾਲੀ)
{
    ਵਾਪਸ compl_shows_dir == FORWARD;
}

/*
 * Return TRUE if currently showing backward completion matches
 */
    ਸਥਿਰ ਪੂਰਨ
compl_shows_dir_backward(ਖਾਲੀ)
{
    ਵਾਪਸ compl_shows_dir == BACKWARD;
}

/*
 * Return TRUE if the 'dictionary' or 'thesaurus' option can be used.
 */
    ਪੂਰਨ
has_compl_option(ਪੂਰਨ dict_opt)
{
    ਜੇ (dict_opt ? (*curbuf->b_p_dict == NUL && *p_dict == NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
							&& !curwin->w_p_spell
#ਜੇ_ਅੰਤ
							)
		 : (*curbuf->b_p_tsr == NUL && *p_tsr == NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
		     && *curbuf->b_p_tsrfu == NUL && *p_tsrfu == NUL
#ਜੇ_ਅੰਤ
		   ))
    {
	ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਧਾਰਨ;
	edit_submode = ਨਿਰਾ;
	ਸੁਨੇਹਾ_attr(dict_opt ? _("'dictionary' option is empty")
			  : _("'thesaurus' option is empty"),
							      HL_ATTR(HLF_E));
	ਜੇ (emsg_silent == 0 && !ਵਿੱਚ_assert_fails)
	{
	    vim_beep(BO_COMPL);
	    setcursor();
	    ਬਾਹਰਲਾ_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (!get_vim_var_nr(VV_TESTING))
#ਜੇ_ਅੰਤ
		ui_delay(2004L, FALSE);
	}
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Is the character "c" a valid key to go to or keep us in CTRL-X mode?
 * This depends on the current mode.
 */
    ਪੂਰਨ
vim_is_ctrl_x_ਕੁੰਜੀ(ਪੂਰਨ c)
{
    // Accept <PageUp> and <PageDown> if the popup menu is visible.
    ਜੇ (ins_compl_pum_ਕੁੰਜੀ(c))
	ਵਾਪਸ TRUE;

    ਚੋਣ (ctrl_x_ਬਹੁਲਕ)
    {
	ਹਾਲਤ 0:		    // Not in any CTRL-X mode
	    ਵਾਪਸ (c == Ctrl_N || c == Ctrl_P || c == Ctrl_X);
	ਹਾਲਤ CTRL_X_NOT_DEFINED_YET:
	ਹਾਲਤ CTRL_X_CMDLINE_CTRL_X:
	    ਵਾਪਸ (   c == Ctrl_X || c == Ctrl_Y || c == Ctrl_E
		    || c == Ctrl_L || c == Ctrl_F || c == Ctrl_RSB
		    || c == Ctrl_I || c == Ctrl_D || c == Ctrl_P
		    || c == Ctrl_N || c == Ctrl_T || c == Ctrl_V
		    || c == Ctrl_Q || c == Ctrl_U || c == Ctrl_O
		    || c == Ctrl_S || c == Ctrl_K || c == 's'
		    || c == Ctrl_Z || c == Ctrl_R);
	ਹਾਲਤ CTRL_X_SCROLL:
	    ਵਾਪਸ (c == Ctrl_Y || c == Ctrl_E);
	ਹਾਲਤ CTRL_X_ਸਾਰਾ_ਸਤਰ:
	    ਵਾਪਸ (c == Ctrl_L || c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_FILES:
	    ਵਾਪਸ (c == Ctrl_F || c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_DICTIONARY:
	    ਵਾਪਸ (c == Ctrl_K || c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_THESAURUS:
	    ਵਾਪਸ (c == Ctrl_T || c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_ਟੈਗ_ਸੂਚੀ:
	    ਵਾਪਸ (c == Ctrl_RSB || c == Ctrl_P || c == Ctrl_N);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	ਹਾਲਤ CTRL_X_ਰਸਤਾ_PATTERNS:
	    ਵਾਪਸ (c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ:
	    ਵਾਪਸ (c == Ctrl_D || c == Ctrl_P || c == Ctrl_N);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CTRL_X_CMDLINE:
	    ਵਾਪਸ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_P || c == Ctrl_N
		    || c == Ctrl_X);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	ਹਾਲਤ CTRL_X_FUNCTION:
	    ਵਾਪਸ (c == Ctrl_U || c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_OMNI:
	    ਵਾਪਸ (c == Ctrl_O || c == Ctrl_P || c == Ctrl_N);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CTRL_X_SPELL:
	    ਵਾਪਸ (c == Ctrl_S || c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_EVAL:
	    ਵਾਪਸ (c == Ctrl_P || c == Ctrl_N);
	ਹਾਲਤ CTRL_X_ਰਜਿਸਟਰ:
	    ਵਾਪਸ (c == Ctrl_R || c == Ctrl_P || c == Ctrl_N);
    }
    ਅੰਦਰੂਨੀ_error("vim_is_ctrl_x_key()");
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if "match" is the original text when the completion began.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_at_original_ਪਾਠ(compl_T *ਮੇਲ)
{
    ਵਾਪਸ ਮੇਲ->cp_flags & CP_ORIGINAL_ਪਾਠ;
}

/*
 * Returns TRUE if "match" is the first match in the completion list.
 */
    ਸਥਿਰ ਪੂਰਨ
is_ਪਹਿਲਾ_ਮੇਲ(compl_T *ਮੇਲ)
{
    ਵਾਪਸ ਮੇਲ == compl_ਪਹਿਲਾ_ਮੇਲ;
}

/*
 * Return the entry holding the original text, NULL if not found.
 * It is the first item, or the last one for backward completion.
 */
    ਸਥਿਰ compl_T *
ਲੱਭੋ_original_ਪਾਠ_ਮੇਲ(ਖਾਲੀ)
{
    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (ਮੇਲ_at_original_ਪਾਠ(compl_ਪਹਿਲਾ_ਮੇਲ))
	ਵਾਪਸ compl_ਪਹਿਲਾ_ਮੇਲ;
    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev != ਨਿਰਾ
	    && ਮੇਲ_at_original_ਪਾਠ(compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev))
	ਵਾਪਸ compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return TRUE when character "c" is part of the item currently being
 * completed.  Used to decide whether to abandon complete mode when the menu
 * is visible.
 */
    ਪੂਰਨ
ins_compl_accept_char(ਪੂਰਨ c)
{
    ਜੇ (compl_autocomplete && compl_from_nonkeyword)
	ਵਾਪਸ FALSE;

    ਜੇ (ctrl_x_ਬਹੁਲਕ & CTRL_X_WANT_IDENT)
	// When expanding an identifier only accept identifier chars.
	ਵਾਪਸ vim_isIDc(c);

    ਚੋਣ (ctrl_x_ਬਹੁਲਕ)
    {
	ਹਾਲਤ CTRL_X_FILES:
	    // When expanding file name only accept file name chars. But not
	    // path separators, so that "proto/<Tab>" expands files in
	    // "proto", not "proto/" as a whole
	    ਵਾਪਸ vim_isfilec(c) && !vim_ispathsep(c);

	ਹਾਲਤ CTRL_X_CMDLINE:
	ਹਾਲਤ CTRL_X_CMDLINE_CTRL_X:
	ਹਾਲਤ CTRL_X_OMNI:
	    // Command line and Omni completion can work with just about any
	    // printable character, but do stop at white space.
	    ਵਾਪਸ vim_isprintc(c) && !VIM_ISWHITE(c);

	ਹਾਲਤ CTRL_X_ਸਾਰਾ_ਸਤਰ:
	    // For while line completion a space can be part of the line.
	    ਵਾਪਸ vim_isprintc(c);
    }
    ਵਾਪਸ vim_iswordc(c);
}

/*
 * Get the completed text by inferring the case of the originally typed text.
 * If the result is in allocated memory "tofree" is set to it.
 */
    ਸਥਿਰ char_u *
ins_compl_infercase_gettext(
    char_u  *str,
    ਪੂਰਨ	    char_len,
    ਪੂਰਨ	    compl_char_len,
    ਪੂਰਨ	    min_len,
    char_u  **tofree)
{
    ਪੂਰਨ		*wca;			// Wide character array.
    char_u	*p;
    ਪੂਰਨ		i, c;
    ਪੂਰਨ		has_ਹੇਠਲਾ = FALSE;
    ਪੂਰਨ		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(ਪੂਰਨ, char_len);
    ਜੇ (wca == ਨਿਰਾ)
	ਵਾਪਸ IObuff;

    p = str;
    ਲਈ (i = 0; i < char_len; ++i)
    {
	ਜੇ (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	ਨਹੀਂਤਾਂ
	    wca[i] = *(p++);
    }

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_ਪਾਠ.string;
    ਲਈ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ਜੇ (MB_ISLOWER(c))
	{
	    has_ਹੇਠਲਾ = TRUE;
	    ਜੇ (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		ਲਈ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		ਰੋਕੋ;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    ਜੇ (!has_ਹੇਠਲਾ)
    {
	p = compl_orig_ਪਾਠ.string;
	ਲਈ (i = 0; i < min_len; ++i)
	{
	    c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	    ਜੇ (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		ਲਈ (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		ਰੋਕੋ;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_ਪਾਠ.string;
    ਲਈ (i = 0; i < min_len; ++i)
    {
	c = has_mbyte ? mb_ptr2char_adv(&p) : *(p++);
	ਜੇ (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	ਨਹੀਂਤਾਂ ਜੇ (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    ਜਦੋਂ (i < char_len)
    {
	ਜੇ (gap.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	{
	    ਜੇ (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		vim_free(wca);
		ਵਾਪਸ (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_ਅੰਕੜੇ + gap.ga_len;
	    ਜੇ (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    ਨਹੀਂਤਾਂ
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    ਜੇ (ga_grow(&gap, IOSIZE) == FAIL)
	    {
		vim_free(wca);
		ਵਾਪਸ (char_u *)"[failed]";
	    }
	    *p = NUL;
	    STRCPY(gap.ga_ਅੰਕੜੇ, IObuff);
	    gap.ga_len = (ਪੂਰਨ)(p - IObuff);
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	ਨਹੀਂਤਾਂ
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    ਜੇ (gap.ga_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	*tofree = gap.ga_ਅੰਕੜੇ;
	ਵਾਪਸ gap.ga_ਅੰਕੜੇ;
    }

    *p = NUL;
    ਵਾਪਸ IObuff;
}

/*
 * This is like ins_compl_add(), but if 'ic' and 'inf' are set, then the
 * case of the originally typed text is used, and the case of the completed
 * text is inferred, ie this tries to work out what case you probably wanted
 * the rest of the word to be in -- webb
 */
    ਪੂਰਨ
ins_compl_ਜੋੜੋ_infercase(
    char_u	*str_arg,
    ਪੂਰਨ		len,
    ਪੂਰਨ		icase,
    char_u	*fname,
    ਪੂਰਨ		dir,
    ਪੂਰਨ		cont_s_ipos,  // next ^X<> will set initial_pos
    ਪੂਰਨ		score)
{
    char_u	*str = str_arg;
    char_u	*p;
    ਪੂਰਨ		char_len;		// count multi-byte characters
    ਪੂਰਨ		compl_char_len;
    ਪੂਰਨ		min_len;
    ਪੂਰਨ		flags = 0;
    ਪੂਰਨ		res;
    char_u	*tofree = ਨਿਰਾ;

    ਜੇ (p_ic && curbuf->b_p_inf && len > 0)
    {
	// Infer case of completed part.

	// Find actual length of completion.
	ਜੇ (has_mbyte)
	{
	    p = str;
	    char_len = 0;
	    ਜਦੋਂ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++char_len;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    char_len = len;
	}

	// Find actual length of original text.
	ਜੇ (has_mbyte)
	{
	    p = compl_orig_ਪਾਠ.string;
	    compl_char_len = 0;
	    ਜਦੋਂ (*p != NUL)
	    {
		MB_PTR_ADV(p);
		++compl_char_len;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    compl_char_len = compl_ਲੰਮਾਈ;
	}

	// "char_len" may be smaller than "compl_char_len" when using
	// thesaurus, only use the minimum when comparing.
	min_len = MIN(char_len, compl_char_len);

	str = ins_compl_infercase_gettext(str, char_len,
					  compl_char_len, min_len, &tofree);
    }
    ਜੇ (cont_s_ipos)
	flags |= CP_CONT_S_IPOS;
    ਜੇ (icase)
	flags |= CP_ICASE;

    res = ins_compl_ਜੋੜੋ(str, len, fname, ਨਿਰਾ, ਨਿਰਾ, dir, flags, FALSE, ਨਿਰਾ, score);
    vim_free(tofree);
    ਵਾਪਸ res;
}

/*
 * Check if fuzzy matching is enabled
 */
    ਸਥਿਰ ਪੂਰਨ
cot_fuzzy(ਖਾਲੀ)
{
    ਵਾਪਸ (get_cot_flags() & COT_FUZZY) != 0 && !ctrl_x_ਬਹੁਲਕ_thesaurus();
}

/*
 * Returns TRUE if matches should be sorted based on proximity to the cursor.
 */
    ਸਥਿਰ ਪੂਰਨ
is_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਵਾਪਸ (compl_autocomplete || (get_cot_flags() & COT_ਸਭ_ਤੋਂ_ਨੇੜੇ))
	&& !cot_fuzzy();
}

/*
 * TRUE if a match is selected (even if it is not inserted).
 */
    ਪੂਰਨ
ins_compl_is_ਮੇਲ_selected(ਖਾਲੀ)
{
    ਵਾਪਸ compl_shown_ਮੇਲ != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ);
}

/*
 * Returns TRUE if autocomplete is active and the pre-insert effect targets the
 * longest prefix.
 */
    ਪੂਰਨ
ins_compl_preinsert_longest(ਖਾਲੀ)
{
    ਵਾਪਸ compl_autocomplete
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
    ਸਥਿਰ ਪੂਰਨ
ins_compl_ਜੋੜੋ(
    char_u	*str,
    ਪੂਰਨ		len,
    char_u	*fname,
    char_u	**cptext,	    // extra text for popup menu or NULL
    typval_T	*user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ,  // "user_data" entry or NULL
    ਪੂਰਨ		cdir,
    ਪੂਰਨ		flags_arg,
    ਪੂਰਨ		adup,		    // accept duplicate match
    ਪੂਰਨ		*user_hl,           // user abbr/kind hlattr
    ਪੂਰਨ		score)
{
    compl_T	*ਮੇਲ, *current, *prev;
    ਪੂਰਨ		dir = (cdir == 0 ? compl_direction : cdir);
    ਪੂਰਨ		flags = flags_arg;
    ਪੂਰਨ		inserted = FALSE;
    char_u	*new_str = ਨਿਰਾ;
    hash_T	str_hash = 0;	    // hash of the match string, when not 0

    ਜੇ (flags & CP_FAST)
	fast_breakcheck();
    ਨਹੀਂਤਾਂ
	ui_breakcheck();
    ਜੇ (got_int)
	ਵਾਪਸ FAIL;
    ਜੇ (len < 0)
	len = (ਪੂਰਨ)STRLEN(str);

    // If the same match is already present, don't add it.
    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ != ਨਿਰਾ && !adup && compl_ਲੜੀਆਂ_ht.ht_ਵਰਤਿਆ > 0)
    {
	// The key must be NUL terminated.
	char_u	    keybuf[128];
	char_u	    *ਕੁੰਜੀ;
	hashitem_T  *hi;

	ਜੇ (len < (ਪੂਰਨ)ਆਕਾਰ(keybuf))
	{
	    mch_memmove(keybuf, str, (size_t)len);
	    keybuf[len] = NUL;
	    ਕੁੰਜੀ = keybuf;
	}
	ਨਹੀਂਤਾਂ
	{
	    new_str = vim_strnsave(str, len);
	    ਜੇ (new_str == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    ਕੁੰਜੀ = new_str;
	}
	str_hash = hash_hash(ਕੁੰਜੀ);
	hi = hash_lookup(&compl_ਲੜੀਆਂ_ht, ਕੁੰਜੀ, str_hash);
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ਜੇ (is_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਸਰਗਰਮ() && score > 0)
	    {
		// The duplicate may need its score updated, scan the
		// matches to find it.
		ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
		ਕਰੋ
		{
		    ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(ਮੇਲ)
			    && STRNCMP(ਮੇਲ->cp_str.string, str, len) == 0
			    && ((ਪੂਰਨ)ਮੇਲ->cp_str.ਲੰਮਾਈ <= len
					  || ਮੇਲ->cp_str.string[len] == NUL))
		    {
			ਜੇ (score < ਮੇਲ->cp_score)
			    ਮੇਲ->cp_score = score;
			ਰੋਕੋ;
		    }
		    ਮੇਲ = ਮੇਲ->cp_ਅਗਲਾ;
		} ਜਦੋਂ (ਮੇਲ != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(ਮੇਲ));
	    }
	    vim_free(new_str);
	    ਵਾਪਸ NOTDONE;
	}
    }

    // Remove any popup menu before changing the list of matches.
    ins_compl_del_pum();

    // Allocate a new match structure.
    // Copy the values to the new match structure.
    ਮੇਲ = ALLOC_CLEAR_ONE(compl_T);
    ਜੇ (ਮੇਲ == ਨਿਰਾ)
    {
	vim_free(new_str);
	ਵਾਪਸ FAIL;
    }
    ਮੇਲ->cp_ਸੰਖਿਆ = flags & CP_ORIGINAL_ਪਾਠ ? 0 : -1;
    ਮੇਲ->cp_ਕ੍ਰਮ = -1;
    ਜੇ (new_str == ਨਿਰਾ && (new_str = vim_strnsave(str, len)) == ਨਿਰਾ)
    {
	vim_free(ਮੇਲ);
	ਵਾਪਸ FAIL;
    }
    ਮੇਲ->cp_str.string = new_str;
    ਮੇਲ->cp_str.ਲੰਮਾਈ = len;

    // match-fname is:
    // - compl_curr_match->cp_fname if it is a string equal to fname.
    // - a copy of fname, CP_FREE_FNAME is set to free later THE allocated mem.
    // - NULL otherwise.	--Acevedo
    ਜੇ (fname != ਨਿਰਾ
	    && compl_curr_ਮੇਲ != ਨਿਰਾ
	    && compl_curr_ਮੇਲ->cp_fname != ਨਿਰਾ
	    && STRCMP(fname, compl_curr_ਮੇਲ->cp_fname) == 0)
	ਮੇਲ->cp_fname = compl_curr_ਮੇਲ->cp_fname;
    ਨਹੀਂਤਾਂ ਜੇ (fname != ਨਿਰਾ)
    {
	ਮੇਲ->cp_fname = vim_strsave(fname);
	flags |= CP_FREE_FNAME;
    }
    ਨਹੀਂਤਾਂ
	ਮੇਲ->cp_fname = ਨਿਰਾ;
    ਮੇਲ->cp_flags = flags;
    ਮੇਲ->cp_user_abbr_hl_id = user_hl ? user_hl[0] : 0;
    ਮੇਲ->cp_user_ਕਿਸਮ_hl_id = user_hl ? user_hl[1] : 0;
    ਮੇਲ->cp_score = score;
    ਮੇਲ->cp_cpt_ਸਰੋਤ_idx = cpt_sources_ਸੂਚਕਾਂਕ;

    ਜੇ (cptext != ਨਿਰਾ)
    {
	ਲਈ (ਪੂਰਨ i = 0; i < CPT_ਗਿਣਤੀ; ++i)
	{
	    ਜੇ (cptext[i] != ਨਿਰਾ && *cptext[i] != NUL)
		ਮੇਲ->cp_ਪਾਠ[i] = vim_strsave(cptext[i]);
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (user_ਅੰਕੜੇ != ਨਿਰਾ)
	ਮੇਲ->cp_user_ਅੰਕੜੇ = *user_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

    // Link the new match structure after (FORWARD) or before (BACKWARD) the
    // current match in the list of matches .
    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ == ਨਿਰਾ)
	ਮੇਲ->cp_ਅਗਲਾ = ਮੇਲ->cp_prev = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (cot_fuzzy() && score != FUZZY_SCORE_NONE && compl_get_longest)
    {
	current = compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ;
	prev = compl_ਪਹਿਲਾ_ਮੇਲ;
	inserted = FALSE;
	// The direction is ignored when using longest and fuzzy match, because
	// matches are inserted and sorted by score.
	ਜਦੋਂ (current != ਨਿਰਾ && current != compl_ਪਹਿਲਾ_ਮੇਲ)
	{
	    ਜੇ (current->cp_score < score)
	    {
		ਮੇਲ->cp_ਅਗਲਾ = current;
		ਮੇਲ->cp_prev = current->cp_prev;
		ਜੇ (current->cp_prev)
		    current->cp_prev->cp_ਅਗਲਾ = ਮੇਲ;
		current->cp_prev = ਮੇਲ;
		inserted = TRUE;
		ਰੋਕੋ;
	    }
	    prev = current;
	    current = current->cp_ਅਗਲਾ;
	}
	ਜੇ (!inserted)
	{
	    prev->cp_ਅਗਲਾ = ਮੇਲ;
	    ਮੇਲ->cp_prev = prev;
	    ਮੇਲ->cp_ਅਗਲਾ = compl_ਪਹਿਲਾ_ਮੇਲ;
	    compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev = ਮੇਲ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (dir == FORWARD)
    {
	ਮੇਲ->cp_ਅਗਲਾ = compl_curr_ਮੇਲ->cp_ਅਗਲਾ;
	ਮੇਲ->cp_prev = compl_curr_ਮੇਲ;
    }
    ਨਹੀਂਤਾਂ	// BACKWARD
    {
	ਮੇਲ->cp_ਅਗਲਾ = compl_curr_ਮੇਲ;
	ਮੇਲ->cp_prev = compl_curr_ਮੇਲ->cp_prev;
    }
    ਜੇ (ਮੇਲ->cp_ਅਗਲਾ)
	ਮੇਲ->cp_ਅਗਲਾ->cp_prev = ਮੇਲ;
    ਜੇ (ਮੇਲ->cp_prev)
	ਮੇਲ->cp_prev->cp_ਅਗਲਾ = ਮੇਲ;
    ਨਹੀਂਤਾਂ	// if there's nothing before, it is the first match
	compl_ਪਹਿਲਾ_ਮੇਲ = ਮੇਲ;
    compl_curr_ਮੇਲ = ਮੇਲ;

    ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(ਮੇਲ))
	compl_ਲੜੀਆਂ_ਜੋੜੋ(ਮੇਲ->cp_str.string, ਮੇਲ->cp_str.ਲੰਮਾਈ,
								    str_hash);

    // Find the longest common string if still doing that.
    ਜੇ (compl_get_longest && (flags & CP_ORIGINAL_ਪਾਠ) == 0 && !cot_fuzzy()
	    && !ins_compl_preinsert_longest() && !ctrl_x_ਬਹੁਲਕ_thesaurus())
	ins_compl_longest_ਮੇਲ(ਮੇਲ);

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "str[len]" matches with match->cp_str, considering
 * match->cp_flags.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_equal(compl_T *ਮੇਲ, char_u *str, ਪੂਰਨ len)
{
    ਜੇ (ਮੇਲ->cp_flags & CP_EQUAL)
	ਵਾਪਸ TRUE;
    ਜੇ (ਮੇਲ->cp_flags & CP_ICASE)
	ਵਾਪਸ STRNICMP(ਮੇਲ->cp_str.string, str, (size_t)len) == 0;
    ਵਾਪਸ STRNCMP(ਮੇਲ->cp_str.string, str, (size_t)len) == 0;
}

/*
 * Like ins_compl_equal(), but ignore case in the 'longest'-inserted part of
 * the leader, so CTRL-N and CTRL-P filter the same way.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_equal_sc(compl_T *ਮੇਲ, char_u *str, ਪੂਰਨ len)
{
    ਪੂਰਨ	typed = compl_ਲੰਮਾਈ;
    ਪੂਰਨ	longest_ਸਿਰਾ = (compl_get_longest && compl_longest_ਸਿਰਾ_ਖਾਨਾ > compl_ਖਾਨਾ)
			    ? (ਪੂਰਨ)(compl_longest_ਸਿਰਾ_ਖਾਨਾ - compl_ਖਾਨਾ) : typed;

    ਜੇ ((ਮੇਲ->cp_flags & (CP_EQUAL | CP_ICASE)) || longest_ਸਿਰਾ <= typed)
	ਵਾਪਸ ins_compl_equal(ਮੇਲ, str, len);

    ਜੇ ((ਪੂਰਨ)ਮੇਲ->cp_str.ਲੰਮਾਈ < len)
	ਵਾਪਸ FALSE;

    ਲਈ (ਪੂਰਨ i = 0; i < len; ++i)
    {
	ਜੇ (i >= typed && i < longest_ਸਿਰਾ
		? TOLOWER_LOC(ਮੇਲ->cp_str.string[i]) != TOLOWER_LOC(str[i])
		: ਮੇਲ->cp_str.string[i] != str[i])
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * when len is -1 mean use whole length of p otherwise part of p
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(char_u *p, ਪੂਰਨ len)
{
    ਜੇ (len == -1)
	len = (ਪੂਰਨ)STRLEN(p);
    ins_ਬਾਈਟਾਂ_len(p, len);
    compl_ins_ਸਿਰਾ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
}

/*
 * Get current completion leader
 */
    char_u *
ins_compl_leader(ਖਾਲੀ)
{
    ਵਾਪਸ compl_leader.string != ਨਿਰਾ ? compl_leader.string : compl_orig_ਪਾਠ.string;
}

/*
 * Get current completion leader length
 */
    ਸਥਿਰ size_t
ins_compl_leader_len(ਖਾਲੀ)
{
    ਵਾਪਸ compl_leader.string != ਨਿਰਾ ? compl_leader.ਲੰਮਾਈ : compl_orig_ਪਾਠ.ਲੰਮਾਈ;
}

/*
 * Checks if the column is within the currently inserted completion text
 * column range. If it is, it returns a special highlight attribute.
 * -1 means normal item.
 */
    ਪੂਰਨ
ins_compl_ਖਾਨਾ_range_attr(linenr_T lnum, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ	    ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਪੂਰਨ	    has_preinsert = ins_compl_has_preinsert()
			|| ins_compl_preinsert_longest();
    ਪੂਰਨ	    attr;

    ਜੇ (cot_fuzzy()
	    || (!compl_hi_on_autocompl_longest && ins_compl_preinsert_longest())
	    || (attr = syn_name2attr(has_preinsert
		    ? (char_u *)"PreInsert" : (char_u *)"ComplMatchIns")) == 0)
	ਵਾਪਸ -1;

    ਸ਼ੁਰੂ_ਖਾਨਾ = compl_ਖਾਨਾ + (ਪੂਰਨ)ins_compl_leader_len();
    ਜੇ (!ins_compl_has_ਗੁਣਜ())
	ਵਾਪਸ (ਖਾਨਾ >= ਸ਼ੁਰੂ_ਖਾਨਾ && ਖਾਨਾ < compl_ins_ਸਿਰਾ_ਖਾਨਾ) ? attr : -1;

    // Multiple lines
    ਜੇ ((lnum == compl_lnum && ਖਾਨਾ >= ਸ਼ੁਰੂ_ਖਾਨਾ && ਖਾਨਾ < MAXCOL) ||
	(lnum > compl_lnum && lnum < curwin->w_cursor.lnum) ||
	(lnum == curwin->w_cursor.lnum && ਖਾਨਾ <= compl_ins_ਸਿਰਾ_ਖਾਨਾ))
	ਵਾਪਸ attr;

    ਵਾਪਸ -1;
}

/*
 * Returns TRUE if the current completion string contains newline characters,
 * indicating it's a multi-line completion.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_has_ਗੁਣਜ(ਖਾਲੀ)
{
    ਵਾਪਸ vim_strchr(compl_shown_ਮੇਲ->cp_str.string, '\n') != ਨਿਰਾ;
}

/*
 * Returns TRUE if the given line number falls within the range of a multi-line
 * completion, i.e. between the starting line (compl_lnum) and current cursor
 * line. Always returns FALSE for single-line completions.
 */
    ਪੂਰਨ
ins_compl_lnum_ਵਿੱਚ_range(linenr_T lnum)
{
    ਜੇ (!ins_compl_has_ਗੁਣਜ())
	ਵਾਪਸ FALSE;
    ਵਾਪਸ lnum >= compl_lnum && lnum <= curwin->w_cursor.lnum;
}

/*
 * Reduce the longest common string for match "match".
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_longest_ਮੇਲ(compl_T *ਮੇਲ)
{
    char_u	*p, *s;
    ਪੂਰਨ		c1, c2;
    ਪੂਰਨ		had_ਮੇਲ;

    ਜੇ (compl_leader.string == ਨਿਰਾ)
    {
	// First match, use it as a whole.
	compl_leader.string = vim_strnsave(ਮੇਲ->cp_str.string, ਮੇਲ->cp_str.ਲੰਮਾਈ);
	ਜੇ (compl_leader.string == ਨਿਰਾ)
	    ਵਾਪਸ;

	compl_leader.ਲੰਮਾਈ = ਮੇਲ->cp_str.ਲੰਮਾਈ;
	had_ਮੇਲ = (curwin->w_cursor.ਖਾਨਾ > compl_ਖਾਨਾ);
	ins_compl_longest_ਪਾਓ(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ਜੇ (!had_ਮੇਲ)
	    ins_compl_delete();
	compl_ਵਰਤਿਆ_ਮੇਲ = FALSE;

	ਵਾਪਸ;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader.string;
    s = ਮੇਲ->cp_str.string;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	ਨਹੀਂਤਾਂ
	{
	    c1 = *p;
	    c2 = *s;
	}
	ਜੇ ((ਮੇਲ->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    ਰੋਕੋ;
	ਜੇ (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	ਨਹੀਂਤਾਂ
	{
	    ++p;
	    ++s;
	}
    }

    ਜੇ (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	compl_leader.ਲੰਮਾਈ = (size_t)(p - compl_leader.string);

	had_ਮੇਲ = (curwin->w_cursor.ਖਾਨਾ > compl_ਖਾਨਾ);
	ins_compl_longest_ਪਾਓ(compl_leader.string);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	ਜੇ (!had_ਮੇਲ)
	    ins_compl_delete();
    }

    compl_ਵਰਤਿਆ_ਮੇਲ = FALSE;
}

/*
 * Add an array of matches to the list of matches.
 * Frees matches[].
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਜੋੜੋ_matches(
    ਪੂਰਨ		ਅੰਕ_matches,
    char_u	**matches,
    ਪੂਰਨ		icase)
{
    ਪੂਰਨ		ਜੋੜੋ_r = OK;
    ਪੂਰਨ		dir = compl_direction;

    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_matches && ਜੋੜੋ_r != FAIL; i++)
    {
	ਜੋੜੋ_r = ins_compl_ਜੋੜੋ(matches[i], -1, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, dir,
		CP_FAST | (icase ? CP_ICASE : 0), FALSE, ਨਿਰਾ,
		FUZZY_SCORE_NONE);
	ਜੇ (ਜੋੜੋ_r == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    }
    FreeWild(ਅੰਕ_matches, matches);
}

/*
 * Make the completion list cyclic.
 * Return the number of matches (excluding the original).
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_make_cyclic(ਖਾਲੀ)
{
    compl_T *ਮੇਲ;
    ਪੂਰਨ	    ਗਿਣਤੀ_2 = 0;

    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ 0;

    // Find the end of the list.
    ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
    // there's always an entry for the compl_orig_text, it doesn't count.
    ਜਦੋਂ (ਮੇਲ->cp_ਅਗਲਾ != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(ਮੇਲ->cp_ਅਗਲਾ))
    {
	ਮੇਲ = ਮੇਲ->cp_ਅਗਲਾ;
	++ਗਿਣਤੀ_2;
    }
    ਮੇਲ->cp_ਅਗਲਾ = compl_ਪਹਿਲਾ_ਮੇਲ;
    compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev = ਮੇਲ;

    ਵਾਪਸ ਗਿਣਤੀ_2;
}

/*
 * Return whether there currently is a shown match.
 */
    ਪੂਰਨ
ins_compl_has_shown_ਮੇਲ(ਖਾਲੀ)
{
    ਵਾਪਸ compl_shown_ਮੇਲ == ਨਿਰਾ
	|| compl_shown_ਮੇਲ != compl_shown_ਮੇਲ->cp_ਅਗਲਾ;
}

/*
 * Return whether the shown match is long enough.
 */
    ਪੂਰਨ
ins_compl_long_shown_ਮੇਲ(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪੂਰਨ)compl_shown_ਮੇਲ->cp_str.ਲੰਮਾਈ
					    > curwin->w_cursor.ਖਾਨਾ - compl_ਖਾਨਾ;
}

/*
 * Get the local or global value of 'completeopt' flags.
 */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ
get_cot_flags(ਖਾਲੀ)
{
    ਵਾਪਸ curbuf->b_cot_flags != 0 ? curbuf->b_cot_flags : cot_flags;
}

/*
 * Update the screen and when there is any scrolling remove the popup menu.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_upd_pum(ਖਾਲੀ)
{
    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ h = curwin->w_cline_ਉਚਾਈ;
    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();
    ਜੇ (h != curwin->w_cline_ਉਚਾਈ)
	ins_compl_del_pum();
}

/*
 * Remove any popup menu.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_del_pum(ਖਾਲੀ)
{
    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
	ਵਾਪਸ;

    pum_undisplay();
    VIM_CLEAR(compl_ਮੇਲ_array);
}

/*
 * Return TRUE if the popup menu should be displayed.
 */
    ਪੂਰਨ
pum_wanted(ਖਾਲੀ)
{
    // 'completeopt' must contain "menu" or "menuone"
    ਵਾਪਸ (get_cot_flags() & COT_ANY_MENU) != 0 || compl_autocomplete;
}

/*
 * Return TRUE if there are two or more matches to be shown in the popup menu.
 * One if 'completopt' contains "menuone".
 */
    ਸਥਿਰ ਪੂਰਨ
pum_enough_matches(ਖਾਲੀ)
{
    compl_T     *compl;
    ਪੂਰਨ		i = 0;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਕਰੋ
    {
	ਜੇ (compl == ਨਿਰਾ || (!ਮੇਲ_at_original_ਪਾਠ(compl) && ++i == 2))
	    ਰੋਕੋ;
	compl = compl->cp_ਅਗਲਾ;
    } ਜਦੋਂ (!is_ਪਹਿਲਾ_ਮੇਲ(compl));

    ਜੇ ((get_cot_flags() & COT_MENUONE) || compl_autocomplete)
	ਵਾਪਸ (i >= 1);
    ਵਾਪਸ (i >= 2);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Allocate Dict for the completed item.
 * { word, abbr, menu, kind, info }
 */
    ਸਥਿਰ dict_T *
ins_compl_dict_alloc(compl_T *ਮੇਲ)
{
    dict_T *dict = dict_alloc_ਤਾਲਾ(VAR_FIXED);

    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    fill_complete_info_dict(dict, ਮੇਲ, FALSE);
    ਵਾਪਸ dict;
}

/*
 * Trigger the CompleteChanged autocmd event. Invoked each time the Insert mode
 * completion menu is changed.
 */
    ਸਥਿਰ ਖਾਲੀ
trigger_complete_ਬਦਲਿਆ_event(ਪੂਰਨ cur)
{
    dict_T	    *v_event;
    dict_T	    *ਵਸਤੂ;
    ਸਥਿਰ ਪੂਰਨ	    recursive = FALSE;
    save_v_event_T  save_v_event;

    ਜੇ (recursive)
	ਵਾਪਸ;

    ਵਸਤੂ = cur < 0 ? dict_alloc() : ins_compl_dict_alloc(compl_curr_ਮੇਲ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ;
    v_event = get_v_event(&save_v_event);
    dict_ਜੋੜੋ_dict(v_event, "completed_item", ਵਸਤੂ);
    pum_ਸੈੱਟ_event_info(v_event);
    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}
#ਜੇ_ਅੰਤ

/*
 * Helper functions for mergesort_list().
 */
    ਸਥਿਰ ਖਾਲੀ*
cp_get_ਅਗਲਾ(ਖਾਲੀ *ਗੰਢ)
{
    ਵਾਪਸ ((compl_T*)ਗੰਢ)->cp_ਅਗਲਾ;
}

    ਸਥਿਰ ਖਾਲੀ
cp_ਸੈੱਟ_ਅਗਲਾ(ਖਾਲੀ *ਗੰਢ, ਖਾਲੀ *ਅਗਲਾ)
{
    ((compl_T*)ਗੰਢ)->cp_ਅਗਲਾ = (compl_T*)ਅਗਲਾ;
}

    ਸਥਿਰ ਖਾਲੀ*
cp_get_prev(ਖਾਲੀ* ਗੰਢ)
{
    ਵਾਪਸ ((compl_T*)ਗੰਢ)->cp_prev;
}

    ਸਥਿਰ ਖਾਲੀ
cp_ਸੈੱਟ_prev(ਖਾਲੀ* ਗੰਢ, ਖਾਲੀ* prev)
{
    ((compl_T*)ਗੰਢ)->cp_prev = (compl_T*)prev;
}

    ਸਥਿਰ ਪੂਰਨ
cp_compare_fuzzy(ਅਚਲ ਖਾਲੀ* a, ਅਚਲ ਖਾਲੀ* b)
{
    ਪੂਰਨ score_a = ((compl_T*)a)->cp_score;
    ਪੂਰਨ score_b = ((compl_T*)b)->cp_score;

    ਜੇ (score_a != score_b)
	ਵਾਪਸ (score_b > score_a) ? 1 : -1;

    ਵਾਪਸ ((compl_T *)a)->cp_ਕ੍ਰਮ - ((compl_T *)b)->cp_ਕ੍ਰਮ;
}

    ਸਥਿਰ ਪੂਰਨ
cp_compare_ਸਭ_ਤੋਂ_ਨੇੜੇ(ਅਚਲ ਖਾਲੀ* a, ਅਚਲ ਖਾਲੀ* b)
{
    ਪੂਰਨ score_a = ((compl_T*)a)->cp_score;
    ਪੂਰਨ score_b = ((compl_T*)b)->cp_score;
    ਜੇ (score_a == FUZZY_SCORE_NONE || score_b == FUZZY_SCORE_NONE)
	ਵਾਪਸ 0;
    ਵਾਪਸ (score_a > score_b) ? 1 : (score_a < score_b) ? -1 : 0;
}

/*
 * Constructs a new string by prepending text from the current line (from
 * startcol to compl_col) to the given source string. Stores the result in
 * dest. Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
prepend_startcol_ਪਾਠ(string_T *dest, string_T *src, ਪੂਰਨ startcol)
{
    ਪੂਰਨ prepend_len = compl_ਖਾਨਾ - startcol;
    ਪੂਰਨ new_ਲੰਮਾਈ = prepend_len + (ਪੂਰਨ)src->ਲੰਮਾਈ;

    dest->ਲੰਮਾਈ = (size_t)new_ਲੰਮਾਈ;
    dest->string = alloc(new_ਲੰਮਾਈ + 1);  // +1 for NUL
    ਜੇ (dest->string == ਨਿਰਾ)
    {
	dest->ਲੰਮਾਈ = 0;
	ਵਾਪਸ FAIL;
    }

    char_u	*ਸਤਰ = ml_get(curwin->w_cursor.lnum);

    mch_memmove(dest->string, ਸਤਰ + startcol, prepend_len);
    mch_memmove(dest->string + prepend_len, src->string, src->ਲੰਮਾਈ);
    dest->string[new_ਲੰਮਾਈ] = NUL;
    ਵਾਪਸ OK;
}

/*
 * Returns the completion leader string adjusted for a specific source's
 * startcol. If the source's startcol is before compl_col, prepends text from
 * the buffer line to the original compl_leader.
 */
    ਸਥਿਰ string_T *
get_leader_for_startcol(compl_T *ਮੇਲ, ਪੂਰਨ cached)
{
    ਸਥਿਰ string_T adjusted_leader = {ਨਿਰਾ, 0};

    ਜੇ (ਮੇਲ == ਨਿਰਾ)
    {
	VIM_CLEAR_STRING(adjusted_leader);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (cpt_sources_array == ਨਿਰਾ)
	ਜਾਓ theend;

    ਪੂਰਨ	cpt_idx = ਮੇਲ->cp_cpt_ਸਰੋਤ_idx;
    ਜੇ (cpt_idx < 0)
	ਜਾਓ theend;
    ਪੂਰਨ	startcol = cpt_sources_array[cpt_idx].cs_startcol;

    ਜੇ (compl_leader.string == ਨਿਰਾ)
    {
	// When leader is not set (e.g. 'autocomplete' first fires before
	// compl_leader is initialised), fall back to compl_orig_text for
	// matches starting at or after compl_col.  Matches starting before
	// compl_col carry pre-compl_col text and must not be compared with
	// compl_orig_text, so return &compl_leader (NULL string) to signal
	// "pass through" (no prefix filter).
	ਜੇ (startcol < 0 || startcol >= compl_ਖਾਨਾ)
	    ਵਾਪਸ &compl_orig_ਪਾਠ;
	ਵਾਪਸ &compl_leader;  // pass through (startcol < compl_col)
    }

    ਜੇ (compl_ਖਾਨਾ <= 0)
	ਜਾਓ theend;

    ਜੇ (startcol >= 0 && startcol < compl_ਖਾਨਾ)
    {
	ਪੂਰਨ prepend_len = compl_ਖਾਨਾ - startcol;
	ਪੂਰਨ new_ਲੰਮਾਈ = prepend_len + (ਪੂਰਨ)compl_leader.ਲੰਮਾਈ;
	ਜੇ (cached && (size_t)new_ਲੰਮਾਈ == adjusted_leader.ਲੰਮਾਈ
		&& adjusted_leader.string != ਨਿਰਾ)
	    ਵਾਪਸ &adjusted_leader;

	VIM_CLEAR_STRING(adjusted_leader);
	ਜੇ (prepend_startcol_ਪਾਠ(&adjusted_leader, &compl_leader,
		    startcol) != OK)
	    ਜਾਓ theend;

	ਵਾਪਸ &adjusted_leader;
    }
theend:
    ਵਾਪਸ &compl_leader;
}

/*
 * Set fuzzy score for completion matches.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_fuzzy_score(ਖਾਲੀ)
{
    compl_T *compl;
    char_u  *ਨਮੂਨਾ;
    ਪੂਰਨ	    use_leader;

    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ;

    /* Determine the pattern to match against */
    use_leader = (compl_leader.string != ਨਿਰਾ && compl_leader.ਲੰਮਾਈ > 0);
    ਜੇ (!use_leader)
    {
	ਜੇ (compl_orig_ਪਾਠ.string == ਨਿਰਾ || compl_orig_ਪਾਠ.ਲੰਮਾਈ == 0)
	    ਵਾਪਸ;
	ਨਮੂਨਾ = compl_orig_ਪਾਠ.string;
    }
    ਨਹੀਂਤਾਂ
    {
	/* Clear the leader cache once before the loop */
	(ਖਾਲੀ)get_leader_for_startcol(ਨਿਰਾ, TRUE);
	ਨਮੂਨਾ = ਨਿਰਾ;  /* Will be computed per-completion */
    }

    /* Score all completion matches */
    ਪੂਰਨ ਅਗਲਾ = 0;
    compl = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਕਰੋ
    {
	ਜੇ (compl->cp_ਕ੍ਰਮ < 0)
	    compl->cp_ਕ੍ਰਮ = ਅਗਲਾ;
	ਅਗਲਾ = MAX(ਅਗਲਾ, compl->cp_ਕ੍ਰਮ + 1);

	ਜੇ (use_leader)
	    ਨਮੂਨਾ = get_leader_for_startcol(compl, TRUE)->string;

	compl->cp_score = fuzzy_ਮੇਲ_str(compl->cp_str.string, ਨਮੂਨਾ);
	compl = compl->cp_ਅਗਲਾ;
    } ਜਦੋਂ (compl != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(compl));
}

/*
 * Sort completion matches, leaving the entry with the original text in place.
 */
    ਸਥਿਰ ਖਾਲੀ
ਛਾਂਟੋ_compl_ਮੇਲ_list(ਪੂਰਨ (*compare)(ਅਚਲ ਖਾਲੀ *, ਅਚਲ ਖਾਲੀ *))
{
    compl_T	*orig_ਪਾਠ;

    ਜੇ (!compl_ਪਹਿਲਾ_ਮੇਲ || is_ਪਹਿਲਾ_ਮੇਲ(compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ))
	ਵਾਪਸ;

    orig_ਪਾਠ = ਲੱਭੋ_original_ਪਾਠ_ਮੇਲ();
    ਜੇ (orig_ਪਾਠ == ਨਿਰਾ)
	ਵਾਪਸ;

    ins_compl_make_ਰੇਖਿਕ();
    ਜੇ (orig_ਪਾਠ == compl_ਪਹਿਲਾ_ਮੇਲ)
    {
	compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ->cp_prev = ਨਿਰਾ;
	compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ = mergesort_list(compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ,
		cp_get_ਅਗਲਾ, cp_ਸੈੱਟ_ਅਗਲਾ, cp_get_prev, cp_ਸੈੱਟ_prev, compare);
	compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ->cp_prev = compl_ਪਹਿਲਾ_ਮੇਲ;
    }
    ਨਹੀਂਤਾਂ
    {
	compl_T	*ਪੂਛ;

	orig_ਪਾਠ->cp_prev->cp_ਅਗਲਾ = ਨਿਰਾ;
	compl_ਪਹਿਲਾ_ਮੇਲ = mergesort_list(compl_ਪਹਿਲਾ_ਮੇਲ, cp_get_ਅਗਲਾ,
		cp_ਸੈੱਟ_ਅਗਲਾ, cp_get_prev, cp_ਸੈੱਟ_prev, compare);
	ਪੂਛ = compl_ਪਹਿਲਾ_ਮੇਲ;
	ਜਦੋਂ (ਪੂਛ->cp_ਅਗਲਾ != ਨਿਰਾ)
	    ਪੂਛ = ਪੂਛ->cp_ਅਗਲਾ;
	ਪੂਛ->cp_ਅਗਲਾ = orig_ਪਾਠ;
	orig_ਪਾਠ->cp_prev = ਪੂਛ;
    }
    (ਖਾਲੀ)ins_compl_make_cyclic();
}

/*
 * Return the attribute for highlight group "hl_id", -1 when it has none.
 */
    ਸਥਿਰ ਪੂਰਨ
get_user_highlight_attr(ਪੂਰਨ hl_id)
{
    ਪੂਰਨ	    attr;

    ਜੇ (hl_id <= 0)
	ਵਾਪਸ -1;
    attr = syn_id2attr(hl_id);
    ਵਾਪਸ attr > 0 ? attr : -1;
}

/*
 * Build a popup menu to show the completion matches.
 * Returns the popup menu entry that should be selected. Returns -1 if nothing
 * should be selected.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_build_pum(ਖਾਲੀ)
{
    compl_T     *compl;
    compl_T     *shown_compl = ਨਿਰਾ;
    ਪੂਰਨ		did_ਲੱਭੋ_shown_ਮੇਲ = FALSE;
    ਪੂਰਨ		shown_ਮੇਲ_ok = FALSE;
    ਪੂਰਨ		i = 0;
    ਪੂਰਨ		cur = -1;
    ਪੂਰਨ		compl_no_select = (get_cot_flags() & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    compl_T	*ਮੇਲ_ਸਿਰ = ਨਿਰਾ;
    compl_T	*ਮੇਲ_ਪੂਛ = ਨਿਰਾ;
    compl_T	*ਮੇਲ_ਅਗਲਾ = ਨਿਰਾ;
    ਪੂਰਨ		*ਮੇਲ_ਗਿਣਤੀ = ਨਿਰਾ;
    ਪੂਰਨ		is_forward = compl_shows_dir_forward();
    ਪੂਰਨ		is_cpt_completion = (cpt_sources_array != ਨਿਰਾ);
    string_T	*leader;

    // Need to build the popup menu list.
    compl_ਮੇਲ_arraysize = 0;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    ਜੇ (ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ))
	shown_ਮੇਲ_ok = TRUE;

    ਜੇ (compl_leader.string != ਨਿਰਾ
	    && STRCMP(compl_leader.string, compl_orig_ਪਾਠ.string) == 0
	    && shown_ਮੇਲ_ok == FALSE)
	compl_shown_ਮੇਲ = compl_no_select ? compl_ਪਹਿਲਾ_ਮੇਲ
					    : compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ;

    ਜੇ (is_cpt_completion)
    {
	ਮੇਲ_ਗਿਣਤੀ = ALLOC_CLEAR_MULT(ਪੂਰਨ, cpt_sources_ਗਿਣਤੀ);
	ਜੇ (ਮੇਲ_ਗਿਣਤੀ == ਨਿਰਾ)
	    ਵਾਪਸ -1;
    }

    (ਖਾਲੀ)get_leader_for_startcol(ਨਿਰਾ, TRUE); // Clear the cache

    compl = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਕਰੋ
    {
	compl->cp_ਵਿੱਚ_ਮੇਲ_array = FALSE;

	leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase': judge case from compl_orig_text, not the leader
	// which 'longest' may fill with uppercase the user never typed.
	ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && !p_inf && compl_orig_ਪਾਠ.string
		&& !ignorecase(compl_orig_ਪਾਠ.string) && !cot_fuzzy())
	    compl->cp_flags &= ~CP_ICASE;

	ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(compl)
		&& (leader->string == ਨਿਰਾ
		    || ins_compl_equal_sc(compl, leader->string,
			(ਪੂਰਨ)leader->ਲੰਮਾਈ)
		    || (cot_fuzzy() && compl->cp_score != FUZZY_SCORE_NONE)))
	{
	    // Limit number of items from each source if max_items is set.
	    ਪੂਰਨ ਮੇਲ_ਹੱਦਬੰਦੀ_exceeded = FALSE;
	    ਪੂਰਨ cur_ਸਰੋਤ = compl->cp_cpt_ਸਰੋਤ_idx;
	    ਜੇ (is_forward && cur_ਸਰੋਤ != -1 && is_cpt_completion)
	    {
		ਮੇਲ_ਗਿਣਤੀ[cur_ਸਰੋਤ]++;
		ਪੂਰਨ max_matches = cpt_sources_array[cur_ਸਰੋਤ].cs_max_matches;
		ਜੇ (max_matches > 0 && ਮੇਲ_ਗਿਣਤੀ[cur_ਸਰੋਤ] > max_matches)
		    ਮੇਲ_ਹੱਦਬੰਦੀ_exceeded = TRUE;
	    }

	    ਜੇ (!ਮੇਲ_ਹੱਦਬੰਦੀ_exceeded)
	    {
		++compl_ਮੇਲ_arraysize;
		compl->cp_ਵਿੱਚ_ਮੇਲ_array = TRUE;
		ਜੇ (ਮੇਲ_ਸਿਰ == ਨਿਰਾ)
		    ਮੇਲ_ਸਿਰ = compl;
		ਨਹੀਂਤਾਂ
		    ਮੇਲ_ਪੂਛ->cp_ਮੇਲ_ਅਗਲਾ = compl;
		ਮੇਲ_ਪੂਛ = compl;

		ਜੇ (!shown_ਮੇਲ_ok && !cot_fuzzy())
		{
		    ਜੇ (compl == compl_shown_ਮੇਲ || did_ਲੱਭੋ_shown_ਮੇਲ)
		    {
			// This item is the shown match or this is the
			// first displayed item after the shown match.
			compl_shown_ਮੇਲ = compl;
			did_ਲੱਭੋ_shown_ਮੇਲ = TRUE;
			shown_ਮੇਲ_ok = TRUE;
		    }
		    ਨਹੀਂਤਾਂ
			// Remember this displayed match for when the
			// shown match is just below it.
			shown_compl = compl;
		    cur = i;
		}
		ਨਹੀਂਤਾਂ ਜੇ (cot_fuzzy())
		{
		    ਜੇ (i == 0)
			shown_compl = compl;

		    ਜੇ (!shown_ਮੇਲ_ok && compl == compl_shown_ਮੇਲ)
		    {
			cur = i;
			shown_ਮੇਲ_ok = TRUE;
		    }
		}
		i++;
	    }
	}

	ਜੇ (compl == compl_shown_ਮੇਲ && !cot_fuzzy())
	{
	    did_ਲੱਭੋ_shown_ਮੇਲ = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    ਜੇ (ਮੇਲ_at_original_ਪਾਠ(compl))
		shown_ਮੇਲ_ok = TRUE;

	    ਜੇ (!shown_ਮੇਲ_ok && shown_compl != ਨਿਰਾ)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_ਮੇਲ = shown_compl;
		shown_ਮੇਲ_ok = TRUE;
	    }
	}
	compl = compl->cp_ਅਗਲਾ;
    } ਜਦੋਂ (compl != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(compl));

    vim_free(ਮੇਲ_ਗਿਣਤੀ);

    ਜੇ (compl_ਮੇਲ_arraysize == 0)
	ਵਾਪਸ -1;

    ਜੇ (cot_fuzzy() && !compl_no_select && !shown_ਮੇਲ_ok)
    {
	compl_shown_ਮੇਲ = shown_compl;
	shown_ਮੇਲ_ok = TRUE;
	cur = 0;
    }

    compl_ਮੇਲ_array = ALLOC_CLEAR_MULT(pumitem_T, compl_ਮੇਲ_arraysize);
    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
	ਵਾਪਸ -1;

    compl = ਮੇਲ_ਸਿਰ;
    i = 0;
    ਜਦੋਂ (compl != ਨਿਰਾ)
    {
	compl_ਮੇਲ_array[i].pum_ਪਾਠ = compl->cp_ਪਾਠ[CPT_ABBR] != ਨਿਰਾ
			    ? compl->cp_ਪਾਠ[CPT_ABBR] : compl->cp_str.string;
	compl_ਮੇਲ_array[i].pum_ਕਿਸਮ = compl->cp_ਪਾਠ[CPT_ਕਿਸਮ];
	compl_ਮੇਲ_array[i].pum_info = compl->cp_ਪਾਠ[CPT_INFO];
	compl_ਮੇਲ_array[i].pum_cpt_ਸਰੋਤ_idx = compl->cp_cpt_ਸਰੋਤ_idx;
	compl_ਮੇਲ_array[i].pum_user_abbr_hlattr =
			get_user_highlight_attr(compl->cp_user_abbr_hl_id);
	compl_ਮੇਲ_array[i].pum_user_ਕਿਸਮ_hlattr =
			get_user_highlight_attr(compl->cp_user_ਕਿਸਮ_hl_id);
	compl_ਮੇਲ_array[i++].pum_extra = compl->cp_ਪਾਠ[CPT_MENU] != ਨਿਰਾ
			    ? compl->cp_ਪਾਠ[CPT_MENU] : compl->cp_fname;
	ਮੇਲ_ਅਗਲਾ = compl->cp_ਮੇਲ_ਅਗਲਾ;
	compl->cp_ਮੇਲ_ਅਗਲਾ = ਨਿਰਾ;
	compl = ਮੇਲ_ਅਗਲਾ;
    }

    ਜੇ (!shown_ਮੇਲ_ok)    // no displayed match at all
	cur = -1;

    ਵਾਪਸ cur;
}

/*
 * Show the popup menu for the list of matches.
 * Also adjusts "compl_shown_match" to an entry that is actually displayed.
 */
    ਖਾਲੀ
ins_compl_show_pum(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		cur = -1;
    colnr_T	ਖਾਨਾ;

    ਜੇ (!pum_wanted() || !pum_enough_matches())
	ਵਾਪਸ;

    // Avoid redrawing the screen under a pum that stays in place.
    ਜੇ (!pum_redraw_ਵਿੱਚ_same_position())
	pum_call_update_screen();

    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    ਨਹੀਂਤਾਂ
    {
	// popup menu already exists, only need to find the current item.
	ਲਈ (i = 0; i < compl_ਮੇਲ_arraysize; ++i)
	{
	    ਜੇ (compl_ਮੇਲ_array[i].pum_ਪਾਠ == compl_shown_ਮੇਲ->cp_str.string
		    || compl_ਮੇਲ_array[i].pum_ਪਾਠ
				      == compl_shown_ਮੇਲ->cp_ਪਾਠ[CPT_ABBR])
	    {
		cur = i;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (compl_started && has_completechanged())
	    trigger_complete_ਬਦਲਿਆ_event(cur);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Position the menu at the completion start without moving the cursor
    // there, so the ruler keeps showing the real cursor column.
    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    curwin->w_cursor.ਖਾਨਾ = compl_ਖਾਨਾ;
    validate_cursor_ਖਾਨਾ();
    ਪੂਰਨ pum_wcol = curwin->w_wcol;
    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
    validate_cursor_ਖਾਨਾ();
    compl_selected_ਵਸਤੂ = cur;
    // Flag the status line so the ruler is redrawn for the real cursor column
    // when the menu update redraws the screen.
    curwin->w_redr_ਸਥਿਤੀ = true;
    pum_display(compl_ਮੇਲ_array, compl_ਮੇਲ_arraysize, cur, pum_wcol);

    // After adding leader, set the current match to shown match.
    ਜੇ (compl_started && compl_curr_ਮੇਲ != compl_shown_ਮੇਲ)
	compl_curr_ਮੇਲ = compl_shown_ਮੇਲ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (has_completechanged())
	trigger_complete_ਬਦਲਿਆ_event(cur);
#ਜੇ_ਅੰਤ
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_ਪਹਿਲਾ	(1)	// use just first element in "dict"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_EXACT	(2)	// "dict" is the exact name of a file

/*
 * Add any identifiers that match the given pattern "pat" in the list of
 * dictionary files "dict_start" to the list of completions.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_dictionaries(
    char_u	*dict_ਸ਼ੁਰੂ,
    char_u	*pat,
    ਪੂਰਨ		flags,		// DICT_FIRST and/or DICT_EXACT
    ਪੂਰਨ		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_ਸ਼ੁਰੂ;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		save_p_scs;
    ਪੂਰਨ		dir = compl_direction;

    ਜੇ (*dict == NUL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	ਜੇ (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ;
    }

    buf = alloc(LSIZE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    regmatch.regprog = ਨਿਰਾ;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ਜੇ (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");

	ਜੇ (pat_esc == ਨਿਰਾ)
	    ਜਾਓ theend;
	size_t len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	ਜੇ (ptr == ਨਿਰਾ)
	{
	    vim_free(pat_esc);
	    ਜਾਓ theend;
	}
	vim_snprintf((ਅੱਖਰ *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    ਨਹੀਂਤਾਂ
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	    ਜਾਓ theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    ਜਦੋਂ (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	ਜੇ (flags == DICT_EXACT)
	{
	    ਗਿਣਤੀ_2 = 1;
	    files = &dict;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	    ਜੇ (!thesaurus && STRCMP(buf, "spell") == 0)
		ਗਿਣਤੀ_2 = -1;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਜੇ (vim_strchr(buf, '`') != ਨਿਰਾ
		    || expand_wildcards(1, &buf, &ਗਿਣਤੀ_2, &files,
						     EW_FILE|EW_SILENT) != OK)
		ਗਿਣਤੀ_2 = 0;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	ਜੇ (ਗਿਣਤੀ_2 == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    ਜੇ (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    ਨਹੀਂਤਾਂ
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (ਗਿਣਤੀ_2 > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(ਗਿਣਤੀ_2, files, thesaurus, flags, &regmatch, buf,
		    &dir);
	    ਜੇ (flags != DICT_EXACT)
		FreeWild(ਗਿਣਤੀ_2, files);
	}
	ਜੇ (flags != 0)
	    ਰੋਕੋ;
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
    ਸਥਿਰ ਪੂਰਨ
thesaurus_ਜੋੜੋ_ਸ਼ਬਦ_ਵਿੱਚ_ਸਤਰ(
    char_u	*fname,
    char_u	**buf_arg,
    ਪੂਰਨ		dir,
    char_u	*skip_ਸ਼ਬਦ)
{
    ਪੂਰਨ		ਸਥਿਤੀ = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    ਜਦੋਂ (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(ptr);
	ਜੇ (*ptr == NUL || *ptr == NL)
	    ਰੋਕੋ;
	wstart = ptr;

	// Find end of the word.
	ਜੇ (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    ਜਦੋਂ (*ptr != NUL)
	    {
		ਪੂਰਨ l = (*mb_ptr2len)(ptr);

		ਜੇ (l < 2 && !vim_iswordc(*ptr))
		    ਰੋਕੋ;
		ptr += l;
	    }
	ਨਹੀਂਤਾਂ
	    ptr = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(ptr);

	// Add the word. Skip the regexp match.
	ਜੇ (wstart != skip_ਸ਼ਬਦ)
	{
	    ਸਥਿਤੀ = ins_compl_ਜੋੜੋ_infercase(wstart, (ਪੂਰਨ)(ptr - wstart), p_ic,
		    fname, dir, FALSE, FUZZY_SCORE_NONE);
	    ਜੇ (ਸਥਿਤੀ == FAIL)
		ਰੋਕੋ;
	}
    }

    *buf_arg = ptr;
    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Process "count" dictionary/thesaurus "files" and add the text matching
 * "regmatch".
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_files(
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    char_u	**files,
    ਪੂਰਨ		thesaurus,
    ਪੂਰਨ		flags,
    regmatch_T	*regmatch,
    char_u	*buf,
    ਪੂਰਨ		*dir)
{
    char_u	*ptr;
    ਪੂਰਨ		i;
    FILE	*fp;
    ਪੂਰਨ		ਜੋੜੋ_r;
    char_u	*leader = ਨਿਰਾ;
    ਪੂਰਨ		leader_len = 0;
    ਪੂਰਨ		score = 0;
    ਪੂਰਨ		len = 0;
    char_u	*ਸਤਰ_ਸਿਰਾ = ਨਿਰਾ;

    ਜੇ (cot_fuzzy())
    {
	leader = ins_compl_leader();
	leader_len = (ਪੂਰਨ)ins_compl_leader_len();
    }

    ਲਈ (i = 0; i < ਗਿਣਤੀ_2 && !got_int && !ins_compl_interrupted(); i++)
    {
	fp = mch_fopen((ਅੱਖਰ *)files[i], "r");  // open dictionary file
	ਜੇ (flags != DICT_EXACT && !shortmess(SHM_COMPLETIONSCAN)
		&& !compl_autocomplete)
	{
	    ਸੁਨੇਹਾ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			      _("Scanning dictionary: %s"), (ਅੱਖਰ *)files[i]);
	    (ਖਾਲੀ)ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, TRUE, HL_ATTR(HLF_R));
	}

	ਜੇ (fp == ਨਿਰਾ)
	    ਜਾਰੀ;

	// Read dictionary file line by line.
	// Check each line for a match.
	ਜਦੋਂ (!got_int && !ins_compl_interrupted()
	       && !vim_fgets(buf, LSIZE, fp))
	{
	    ptr = buf;
	    ਜੇ (cot_fuzzy() && leader_len > 0)
	    {
		ਸਤਰ_ਸਿਰਾ = ਲੱਭੋ_ਸਤਰ_ਸਿਰਾ(ptr);
		ਜਦੋਂ (ptr < ਸਤਰ_ਸਿਰਾ)
		{
		    ਜੇ (fuzzy_ਮੇਲ_str_ਵਿੱਚ_ਸਤਰ(&ptr, leader, &len, ਨਿਰਾ, &score))
		    {
			char_u *end_ptr = ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval()
					? ਲੱਭੋ_ਸਤਰ_ਸਿਰਾ(ptr) : ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(ptr);
			ਜੋੜੋ_r = ins_compl_ਜੋੜੋ_infercase(ptr, (ਪੂਰਨ)(end_ptr - ptr),
					    p_ic, files[i], *dir, FALSE, score);
			ਜੇ (ਜੋੜੋ_r == FAIL)
			    ਰੋਕੋ;
			ptr = end_ptr;  // start from next word
			ਜੇ (compl_get_longest && ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ()
				&& compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ
				&& score == compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ->cp_score)
			    compl_ਅੰਕ_bests++;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (regmatch != ਨਿਰਾ)
	    {
		ਜਦੋਂ (vim_regexec(regmatch, buf, (colnr_T)(ptr - buf)))
		{
		    ptr = regmatch->startp[0];
		    ptr = ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval() ? ਲੱਭੋ_ਸਤਰ_ਸਿਰਾ(ptr)
						    : ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(ptr);
		    ਜੋੜੋ_r = ins_compl_ਜੋੜੋ_infercase(regmatch->startp[0],
			    (ਪੂਰਨ)(ptr - regmatch->startp[0]),
			    p_ic, files[i], *dir, FALSE, FUZZY_SCORE_NONE);
		    ਜੇ (thesaurus)
		    {
			// For a thesaurus, add all the words in the line
			ptr = buf;
			ਜੋੜੋ_r = thesaurus_ਜੋੜੋ_ਸ਼ਬਦ_ਵਿੱਚ_ਸਤਰ(files[i], &ptr, *dir,
							    regmatch->startp[0]);
		    }
		    ਜੇ (ਜੋੜੋ_r == OK)
			// if dir was BACKWARD then honor it just once
			*dir = FORWARD;
		    ਨਹੀਂਤਾਂ ਜੇ (ਜੋੜੋ_r == FAIL)
			ਰੋਕੋ;
		    // avoid expensive call to vim_regexec() when at end
		    // of line
		    ਜੇ (*ptr == '\n' || got_int)
			ਰੋਕੋ;
		}
	    }
	    ਸਤਰ_breakcheck();
	    ins_compl_check_ਕੁੰਜੀਆਂ(50, FALSE);
	}
	fclose(fp);
    }
}

/*
 * Find the start of the next word.
 * Returns a pointer to the first char of the word.  Also stops at a NUL.
 */
    char_u *
ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(char_u *ptr)
{
    ਜੇ (has_mbyte)
    {
	ਜਦੋਂ (*ptr != NUL && *ptr != '\n' && mb_get_class(ptr) <= 1)
	    ptr += (*mb_ptr2len)(ptr);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (*ptr != NUL && *ptr != '\n' && !vim_iswordc(*ptr))
	    ++ptr;
    }
    ਵਾਪਸ ptr;
}

/*
 * Find the end of the word.  Assumes it starts inside a word.
 * Returns a pointer to just after the word.
 */
    char_u *
ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(char_u *ptr)
{
    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ ਸ਼ੁਰੂ_class = mb_get_class(ptr);
	ਜੇ (ਸ਼ੁਰੂ_class > 1)
	    ਜਦੋਂ (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		ਜੇ (mb_get_class(ptr) != ਸ਼ੁਰੂ_class)
		    ਰੋਕੋ;
	    }
    }
    ਨਹੀਂਤਾਂ
	ਜਦੋਂ (vim_iswordc(*ptr))
	    ++ptr;
    ਵਾਪਸ ptr;
}

/*
 * Find the end of the line, omitting CR and NL at the end.
 * Returns a pointer to just after the line.
 */
    char_u *
ਲੱਭੋ_ਸਤਰ_ਸਿਰਾ(char_u *ptr)
{
    char_u *s = ptr + STRLEN(ptr);
    ਜਦੋਂ (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    ਵਾਪਸ s;
}

/*
 * Free a completion item in the list
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਵਸਤੂ_free(compl_T *ਮੇਲ)
{
    // Uncount the match string in the duplicate-check hashtab; the entry is
    // only removed with its last match.  The hashtab is empty when it was
    // already cleared as a whole by ins_compl_free().
    ਜੇ (compl_ਲੜੀਆਂ_ht.ht_ਵਰਤਿਆ > 0 && ਮੇਲ->cp_str.string != ਨਿਰਾ
					   && !ਮੇਲ_at_original_ਪਾਠ(ਮੇਲ))
    {
	hashitem_T *hi = hash_ਲੱਭੋ(&compl_ਲੜੀਆਂ_ht, ਮੇਲ->cp_str.string);

	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    complstr_T *ਇੰਦਰਾਜ = HI2CSE(hi);

	    ਜੇ (--ਇੰਦਰਾਜ->cse_ਗਿਣਤੀ <= 0)
	    {
		hash_remove(&compl_ਲੜੀਆਂ_ht, hi, "completion match");
		vim_free(ਇੰਦਰਾਜ);
	    }
	}
    }
    VIM_CLEAR_STRING(ਮੇਲ->cp_str);
    // several entries may use the same fname, free it just once.
    ਜੇ (ਮੇਲ->cp_flags & CP_FREE_FNAME)
	vim_free(ਮੇਲ->cp_fname);
    ਲਈ (ਪੂਰਨ i = 0; i < CPT_ਗਿਣਤੀ; ++i)
	vim_free(ਮੇਲ->cp_ਪਾਠ[i]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    clear_tv(&ਮੇਲ->cp_user_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
    vim_free(ਮੇਲ);
}

/*
 * Free the list of completions
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_free(ਖਾਲੀ)
{
    compl_T *ਮੇਲ;

    VIM_CLEAR_STRING(compl_ਨਮੂਨਾ);
    VIM_CLEAR_STRING(compl_leader);

    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ;

    ins_compl_del_pum();
    pum_clear();

    // Free the duplicate-check hashtab entries all at once, then freeing
    // the matches below does not need to uncount them one by one.
    hash_clear_all(&compl_ਲੜੀਆਂ_ht, CSE_OFF);
    hash_init(&compl_ਲੜੀਆਂ_ht);

    compl_curr_ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਕਰੋ
    {
	ਮੇਲ = compl_curr_ਮੇਲ;
	compl_curr_ਮੇਲ = compl_curr_ਮੇਲ->cp_ਅਗਲਾ;
	ins_compl_ਵਸਤੂ_free(ਮੇਲ);
    } ਜਦੋਂ (compl_curr_ਮੇਲ != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(compl_curr_ਮੇਲ));
    compl_ਪਹਿਲਾ_ਮੇਲ = compl_curr_ਮੇਲ = ਨਿਰਾ;
    compl_shown_ਮੇਲ = ਨਿਰਾ;
    compl_old_ਮੇਲ = ਨਿਰਾ;
}

/*
 * Reset/clear the completion state.
 */
    ਖਾਲੀ
ins_compl_clear(ਖਾਲੀ)
{
    compl_cont_ਸਥਿਤੀ = 0;
    compl_started = FALSE;
    compl_matches = 0;
    compl_selected_ਵਸਤੂ = -1;
    compl_ins_ਸਿਰਾ_ਖਾਨਾ = 0;
    compl_longest_ਸਿਰਾ_ਖਾਨਾ = 0;
    compl_curr_win = ਨਿਰਾ;
    compl_curr_buf = ਨਿਰਾ;
    VIM_CLEAR_STRING(compl_ਨਮੂਨਾ);
    VIM_CLEAR_STRING(compl_leader);
    edit_submode_extra = ਨਿਰਾ;
    VIM_CLEAR_STRING(compl_orig_ਪਾਠ);
    compl_enter_selects = FALSE;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ਅੰਕ_bests = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // clear v:completed_item
    ਸੈੱਟ_vim_var_dict(VV_COMPLETED_ਵਸਤੂ, dict_alloc_ਤਾਲਾ(VAR_FIXED));
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE when Insert completion is active.
 */
    ਪੂਰਨ
ins_compl_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਵਾਪਸ compl_started;
}

/*
 * Return True when wp is the actual completion window
 */
    ਪੂਰਨ
ins_compl_win_ਸਰਗਰਮ(win_T *wp)
{
    ਵਾਪਸ ins_compl_ਸਰਗਰਮ() && wp == compl_curr_win
	&& wp->w_ਬਫਰ == compl_curr_buf;
}

/*
 * Selected one of the matches.  When FALSE, the match was either edited or
 * using the longest common string.
 */
    ਪੂਰਨ
ins_compl_ਵਰਤਿਆ_ਮੇਲ(ਖਾਲੀ)
{
    ਵਾਪਸ compl_ਵਰਤਿਆ_ਮੇਲ;
}

/*
 * Initialize get longest common string.
 */
    ਖਾਲੀ
ins_compl_init_get_longest(ਖਾਲੀ)
{
    compl_get_longest = FALSE;
}

/*
 * Returns TRUE when insert completion is interrupted.
 */
    ਪੂਰਨ
ins_compl_interrupted(ਖਾਲੀ)
{
    ਵਾਪਸ compl_interrupted || compl_ਸਮਾਂ_ਟੁਕੜਾ_expired;
}

/*
 * Returns TRUE if the <Enter> key selects a match in the completion popup
 * menu.
 */
    ਪੂਰਨ
ins_compl_enter_selects(ਖਾਲੀ)
{
    ਵਾਪਸ compl_enter_selects;
}

/*
 * Return the column where the text starts that is being completed
 */
    colnr_T
ins_compl_ਖਾਨਾ(ਖਾਲੀ)
{
    ਵਾਪਸ compl_ਖਾਨਾ;
}

/*
 * Return the length in bytes of the text being completed
 */
    ਪੂਰਨ
ins_compl_len(ਖਾਲੀ)
{
    ਵਾਪਸ compl_ਲੰਮਾਈ;
}

/*
 * Return TRUE when the 'completeopt' "preinsert" flag is in effect,
 * otherwise return FALSE.
 */
    ਪੂਰਨ
ins_compl_has_preinsert(ਖਾਲੀ)
{
    ਪੂਰਨ cur_cot_flags = get_cot_flags();
    ਜੇ (compl_autocomplete && p_ic && !p_inf)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ !compl_autocomplete
	? (cur_cot_flags & (COT_PREINSERT | COT_FUZZY | COT_MENUONE))
		== (COT_PREINSERT | COT_MENUONE)
	: (cur_cot_flags & (COT_PREINSERT | COT_FUZZY)) == COT_PREINSERT;
}

/*
 * Returns TRUE if the pre-insert effect is valid and the cursor is within
 * the `compl_ins_end_col` range.
 */
    ਪੂਰਨ
ins_compl_preinsert_effect(ਖਾਲੀ)
{
    ਜੇ (!ins_compl_has_preinsert() && !ins_compl_preinsert_longest())
	ਵਾਪਸ FALSE;

    ਵਾਪਸ curwin->w_cursor.ਖਾਨਾ < compl_ins_ਸਿਰਾ_ਖਾਨਾ;
}

/*
 * Delete one character before the cursor and show the subset of the matches
 * that match the word that is now before the cursor.
 * Returns the character to be used, NUL if the work is done and another char
 * to be got from the user.
 */
    ਪੂਰਨ
ins_compl_bs(ਖਾਲੀ)
{
    char_u	*ਸਤਰ;
    char_u	*p;

    ਜੇ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ਸਤਰ = ml_get_curline();
    p = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ;
    MB_PTR_ਪਿੱਠ(ਸਤਰ, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    ਜੇ ((ਪੂਰਨ)(p - ਸਤਰ) - (ਪੂਰਨ)compl_ਖਾਨਾ < 0
	    || ((ਪੂਰਨ)(p - ਸਤਰ) - (ਪੂਰਨ)compl_ਖਾਨਾ == 0 && !ctrl_x_ਬਹੁਲਕ_omni())
	    || ctrl_x_ਬਹੁਲਕ_eval()
	    || (!can_bs(BS_ਸ਼ੁਰੂ) && (ਪੂਰਨ)(p - ਸਤਰ) - (ਪੂਰਨ)compl_ਖਾਨਾ
							- compl_ਲੰਮਾਈ < 0))
	ਵਾਪਸ K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    ਜੇ (curwin->w_cursor.ਖਾਨਾ <= compl_ਖਾਨਾ + compl_ਲੰਮਾਈ
						  || ins_compl_need_restart())
	ins_compl_restart();

    VIM_CLEAR_STRING(compl_leader);
    compl_leader.ਲੰਮਾਈ = (size_t)((p - ਸਤਰ) - compl_ਖਾਨਾ);
    compl_leader.string = vim_strnsave(ਸਤਰ + compl_ਖਾਨਾ, compl_leader.ਲੰਮਾਈ);
    ਜੇ (compl_leader.string == ਨਿਰਾ)
    {
	compl_leader.ਲੰਮਾਈ = 0;
	ਵਾਪਸ K_BS;
    }

    // Clear selection if a menu item is currently selected in autocompletion
    ਜੇ (compl_autocomplete && compl_ਪਹਿਲਾ_ਮੇਲ && !ins_compl_has_preinsert())
	compl_shown_ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;

    ins_compl_new_leader();
    ਜੇ (compl_shown_ਮੇਲ != ਨਿਰਾ)
	// Make sure current match is not a hidden item.
	compl_curr_ਮੇਲ = compl_shown_ਮੇਲ;
    ਵਾਪਸ NUL;
}

/*
 * Return TRUE when we need to find matches again, ins_compl_restart() is to
 * be called.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_need_restart(ਖਾਲੀ)
{
    // Return TRUE if we didn't complete finding matches or when the
    // 'completefunc' returned "always" in the "refresh" dictionary item.
    ਵਾਪਸ compl_was_interrupted
	|| ((ctrl_x_ਬਹੁਲਕ_function() || ctrl_x_ਬਹੁਲਕ_omni())
						  && compl_opt_refresh_always);
}

/*
 * Return TRUE if 'autocomplete' option is set
 */
    ਪੂਰਨ
ins_compl_has_autocomplete(ਖਾਲੀ)
{
    // Use buffer-local setting if defined (>= 0), otherwise use global
    ਵਾਪਸ curbuf->b_p_ac >= 0 ? curbuf->b_p_ac : p_ac;
}

/*
 * Calculate fuzzy score and sort completion matches unless sorting is disabled.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_fuzzy_ਛਾਂਟੋ(ਖਾਲੀ)
{
    ਪੂਰਨ	    cur_cot_flags = get_cot_flags();

    // Set the fuzzy score in cp_score and sort
    ਸੈੱਟ_fuzzy_score();
    ਜੇ (!(cur_cot_flags & COT_NOSORT))
    {
	ਛਾਂਟੋ_compl_ਮੇਲ_list(cp_compare_fuzzy);
	// Reset the shown item since sorting reorders items
	ਜੇ ((cur_cot_flags & (COT_NOINSERT | COT_NOSELECT)) == COT_NOINSERT)
	{
	    ਪੂਰਨ none_selected = compl_shown_ਮੇਲ == (compl_shows_dir_forward()
		    ? compl_ਪਹਿਲਾ_ਮੇਲ : compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev);
	    ਜੇ (!none_selected)
		compl_shown_ਮੇਲ
		    = (!compl_autocomplete && compl_shows_dir_forward())
		    ? compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ : compl_ਪਹਿਲਾ_ਮੇਲ;
	}
    }
}

/*
 * Called after changing "compl_leader".
 * Show the popup menu with a different set of matches.
 * May also search for matches again if the previous search was interrupted.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_new_leader(ਖਾਲੀ)
{
    ਪੂਰਨ	    save_w_wrow = curwin->w_wrow;
    ਪੂਰਨ	    save_w_leftcol = curwin->w_leftcol;

    ins_compl_del_pum();
    ins_compl_delete();
    ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(compl_leader.string + get_compl_len(), -1);
    compl_ਵਰਤਿਆ_ਮੇਲ = FALSE;

    ਜੇ (compl_started)
    {
	ins_compl_ਸੈੱਟ_original_ਪਾਠ(compl_leader.string, compl_leader.ਲੰਮਾਈ);
	ਜੇ (is_cpt_func_refresh_always())
	    cpt_compl_refresh();
	ਜੇ (cot_fuzzy())
	    ins_compl_fuzzy_ਛਾਂਟੋ();
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#ਜੇ_ਅੰਤ
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
	}
#ਜੇ_ਅੰਤ
	save_w_wrow = curwin->w_wrow;
	save_w_leftcol = curwin->w_leftcol;
	compl_restarting = TRUE;
	ਜੇ (ins_compl_has_autocomplete())
	    ins_compl_enable_autocomplete();
	ਨਹੀਂਤਾਂ
	    compl_autocomplete = FALSE;
	ਜੇ (ins_complete(Ctrl_N, FALSE) == FAIL)
	    compl_cont_ਸਥਿਤੀ = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_ਵਰਤਿਆ_ਮੇਲ && compl_selected_ਵਸਤੂ != -1;

    // Show the popup menu with a different set of matches.  With
    // 'autocompletedelay' the menu is already visible here, so update it
    // immediately rather than re-arming the delay, like a zero delay does.
    ਜੇ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    // Don't let Enter select the original text when there is no popup menu.
    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
	compl_enter_selects = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (ins_compl_has_preinsert() && compl_leader.ਲੰਮਾਈ > 0)
	ins_compl_ਪਾਓ(TRUE, FALSE);
    ਨਹੀਂਤਾਂ ਜੇ (compl_started && ins_compl_preinsert_longest()
	    && compl_leader.ਲੰਮਾਈ > 0 && !ins_compl_preinsert_effect())
	ins_compl_ਪਾਓ(TRUE, TRUE);
}

/*
 * Return the length of the completion, from the completion start column to
 * the cursor column.  Making sure it never goes below zero.
 */
    ਸਥਿਰ ਪੂਰਨ
get_compl_len(ਖਾਲੀ)
{
    ਪੂਰਨ off = (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ - (ਪੂਰਨ)compl_ਖਾਨਾ;
    ਵਾਪਸ MAX(0, off);
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ਖਾਲੀ
ins_compl_addleader(ਪੂਰਨ c)
{
    ਪੂਰਨ	    cc;

    ਜੇ (ins_compl_preinsert_effect())
	ins_compl_delete();

    ਜੇ (stop_arrow() == FAIL)
	ਵਾਪਸ;
    ਜੇ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_ਬਾਈਟਾਂ(buf, cc);
	ਜੇ (compl_opt_refresh_always)
	    Append_ਮੰਜ਼ਿਲ_Redobuff(buf);
    }
    ਨਹੀਂਤਾਂ
    {
	ins_char(c);
	ਜੇ (compl_opt_refresh_always)
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    ਜੇ (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point to the original position, changing compl_leader
    // would break redo.
    ਜੇ (!compl_opt_refresh_always)
    {
	VIM_CLEAR_STRING(compl_leader);
	compl_leader.ਲੰਮਾਈ = (size_t)(curwin->w_cursor.ਖਾਨਾ - compl_ਖਾਨਾ);
	compl_leader.string = vim_strnsave(ml_get_curline() + compl_ਖਾਨਾ,
		compl_leader.ਲੰਮਾਈ);
	ਜੇ (compl_leader.string == ਨਿਰਾ)
	{
	    compl_leader.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ;
	}

	ins_compl_new_leader();
    }
}

/*
 * Setup for finding completions again without leaving CTRL-X mode.  Used when
 * BS or a key was typed while still searching for matches.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_restart(ਖਾਲੀ)
{
    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    compl_cont_ਸਥਿਤੀ = 0;
    compl_cont_ਬਹੁਲਕ = 0;
    cpt_sources_clear();
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ਅੰਕ_bests = 0;
}

/*
 * Set the first match, the original text.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਸੈੱਟ_original_ਪਾਠ(char_u *str, size_t len)
{
    compl_T	*ਮੇਲ = ਲੱਭੋ_original_ਪਾਠ_ਮੇਲ();
    char_u	*p;

    // Replace the original text entry.
    ਜੇ (ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ;

    p = vim_strnsave(str, len);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    VIM_CLEAR_STRING(ਮੇਲ->cp_str);
    ਮੇਲ->cp_str.string = p;
    ਮੇਲ->cp_str.ਲੰਮਾਈ = len;
}

/*
 * Append one character to the match leader.  May reduce the number of
 * matches.
 */
    ਖਾਲੀ
ins_compl_addfrommatch(ਖਾਲੀ)
{
    char_u	*p;
    ਪੂਰਨ		len = (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ - (ਪੂਰਨ)compl_ਖਾਨਾ;
    ਪੂਰਨ		c;

    p = compl_shown_ਮੇਲ->cp_str.string;
    ਜੇ ((ਪੂਰਨ)compl_shown_ਮੇਲ->cp_str.ਲੰਮਾਈ <= len)   // the match is too short
    {
	size_t	plen;
	compl_T	*cp;

	// When still at the original match use the first entry that matches
	// the leader.
	ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ))
	    ਵਾਪਸ;

	p = ਨਿਰਾ;
	plen = 0;
	ਲਈ (cp = compl_shown_ਮੇਲ->cp_ਅਗਲਾ; cp != ਨਿਰਾ
		&& !is_ਪਹਿਲਾ_ਮੇਲ(cp); cp = cp->cp_ਅਗਲਾ)
	{
	    ਜੇ (compl_leader.string == ਨਿਰਾ
		    || ins_compl_equal(cp, compl_leader.string,
			(ਪੂਰਨ)compl_leader.ਲੰਮਾਈ))
	    {
		p = cp->cp_str.string;
		plen = cp->cp_str.ਲੰਮਾਈ;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (p == ਨਿਰਾ || (ਪੂਰਨ)plen <= len)
	    ਵਾਪਸ;
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
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ctrl_x_ਬਹੁਲਕ(ਪੂਰਨ c)
{
    ਪੂਰਨ	    retval = FALSE;

    ਚੋਣ (c)
    {
	ਹਾਲਤ Ctrl_E:
	ਹਾਲਤ Ctrl_Y:
	    // scroll the window one line up or down
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_SCROLL;
	    ਜੇ (!(State & REPLACE_FLAG))
		edit_submode = (char_u *)_(" (insert) Scroll (^E/^Y)");
	    ਨਹੀਂਤਾਂ
		edit_submode = (char_u *)_(" (replace) Scroll (^E/^Y)");
	    edit_submode_pre = ਨਿਰਾ;
	    showmode();
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_L:
	    // complete whole line
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਰਾ_ਸਤਰ;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_F:
	    // complete filenames
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_FILES;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_K:
	    // complete words from a dictionary
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_DICTIONARY;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_R:
	    // When CTRL-R is followed by '=', don't trigger register completion
	    // This allows expressions like <C-R>=func()<CR> to work normally
	    ਜੇ (vpeekc() == '=')
		ਰੋਕੋ;
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਰਜਿਸਟਰ;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_T:
	    // complete words from a thesaurus
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_THESAURUS;
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	ਹਾਲਤ Ctrl_U:
	    // user defined completion
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_FUNCTION;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_O:
	    // omni completion
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_OMNI;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ 's':
	ਹਾਲਤ Ctrl_S:
	    // complete spelling suggestions
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_SPELL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	    ++emsg_off;	// Avoid getting the E756 error twice.
	    spell_ਪਿੱਠ_ਮੰਜ਼ਿਲ_badword();
	    --emsg_off;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_RSB:
	    // complete tag names
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਟੈਗ_ਸੂਚੀ;
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	ਹਾਲਤ Ctrl_I:
	ਹਾਲਤ K_S_TAB:
	    // complete keywords from included files
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਰਸਤਾ_PATTERNS;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_D:
	    // complete definitions from included files
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ Ctrl_V:
	ਹਾਲਤ Ctrl_Q:
	    // complete vim commands
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_CMDLINE;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_Z:
	    // stop completion
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਧਾਰਨ;
	    edit_submode = ਨਿਰਾ;
	    showmode();
	    retval = TRUE;
	    ਰੋਕੋ;
	ਹਾਲਤ Ctrl_P:
	ਹਾਲਤ Ctrl_N:
	    // ^X^P means LOCAL expansion if nothing interrupted (eg we
	    // just started ^X mode, or there were enough ^X's to cancel
	    // the previous mode, say ^X^F^X^X^P or ^P^X^X^X^P, see below)
	    // do normal expansion when interrupting a different mode (say
	    // ^X^F^X^P or ^P^X^X^P, see below)
	    // nothing changes if interrupting mode 0, (eg, the flag
	    // doesn't change when going to ADDING mode  -- Acevedo
	    ਜੇ (!(compl_cont_ਸਥਿਤੀ & CONT_INTRPT))
		compl_cont_ਸਥਿਤੀ |= CONT_LOCAL;
	    ਨਹੀਂਤਾਂ ਜੇ (compl_cont_ਬਹੁਲਕ != 0)
		compl_cont_ਸਥਿਤੀ &= ~CONT_LOCAL;
	    // FALLTHROUGH
	ਮੂਲ:
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
	    ਜੇ (c == Ctrl_X)
	    {
		ਜੇ (compl_cont_ਬਹੁਲਕ != 0)
		    compl_cont_ਸਥਿਤੀ = 0;
		ਨਹੀਂਤਾਂ
		    compl_cont_ਬਹੁਲਕ = CTRL_X_NOT_DEFINED_YET;
	    }
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਧਾਰਨ;
	    edit_submode = ਨਿਰਾ;
	    showmode();
	    ਰੋਕੋ;
    }

    ਵਾਪਸ retval;
}

/*
 * Trigger CompleteDone event and adds relevant information to v:event
 */
    ਸਥਿਰ ਖਾਲੀ
trigger_complete_ਪੂਰਾ_event(ਪੂਰਨ ਬਹੁਲਕ ਅਣਵਰਤਿਆ, string_T *ਸ਼ਬਦ ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_EVAL)
    save_v_event_T	save_v_event;
    dict_T		*v_event = get_v_event(&save_v_event);
    string_T		*ਬਹੁਲਕ_str;

    ਜੇ (ਸ਼ਬਦ == ਨਿਰਾ || ਸ਼ਬਦ->string == ਨਿਰਾ)
	(ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "complete_word", (char_u *)"", 0);
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "complete_word", ਸ਼ਬਦ->string, (ਪੂਰਨ)ਸ਼ਬਦ->ਲੰਮਾਈ);

    ਬਹੁਲਕ_str = &ctrl_x_ਬਹੁਲਕ_ਨਾਂ[ਬਹੁਲਕ & ~CTRL_X_WANT_IDENT];
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(v_event, "complete_type",
	(ਬਹੁਲਕ_str->string == ਨਿਰਾ) ? (char_u *)"" : ਬਹੁਲਕ_str->string, (ਪੂਰਨ)ਬਹੁਲਕ_str->ਲੰਮਾਈ);

    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);
#ਜੇ_ਅੰਤ
    ins_apply_autocmds(EVENT_COMPLETEDONE);

#ਜੇ defined(FEAT_EVAL)
    restore_v_event(v_event, &save_v_event);
#ਜੇ_ਅੰਤ
}

/*
 * Stop insert completion mode
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_stop(ਪੂਰਨ c, ਪੂਰਨ prev_ਬਹੁਲਕ, ਪੂਰਨ retval)
{
    ਪੂਰਨ		want_cindent;
    string_T	ਸ਼ਬਦ = {ਨਿਰਾ, 0};

    // Remove pre-inserted text when present.
    ਜੇ (ins_compl_preinsert_effect() && ins_compl_win_ਸਰਗਰਮ(curwin))
	ins_compl_delete();

    // Get here when we have finished typing a sequence of ^N and
    // ^P or other completion characters in CTRL-X mode.  Free up
    // memory that was used, and make sure we can redo the insert.
    ਜੇ (compl_curr_ਮੇਲ != ਨਿਰਾ || compl_leader.string != ਨਿਰਾ || c == Ctrl_E)
    {
	char_u	*ptr = ਨਿਰਾ;

	// If any of the original typed text has been changed, eg when
	// ignorecase is set, we must add back-spaces to the redo
	// buffer.  We add as few as necessary to delete just the part
	// of the original text that has changed.
	// When using the longest match, edited the match or used
	// CTRL-E then don't use the current match.
	ਜੇ (compl_curr_ਮੇਲ != ਨਿਰਾ && compl_ਵਰਤਿਆ_ਮੇਲ && c != Ctrl_E)
	    ptr = compl_curr_ਮੇਲ->cp_str.string;
	ins_compl_fixRedoBufForLeader(ptr);
    }

    want_cindent = (get_can_cindent() && cindent_on());

    // When completing whole lines: fix indent for 'cindent'.
    // Otherwise, break line if it's too long.
    ਜੇ (compl_cont_ਬਹੁਲਕ == CTRL_X_ਸਾਰਾ_ਸਤਰ)
    {
	// re-indent the current line
	ਜੇ (want_cindent)
	{
	    do_c_ਵਿਅੰਜਕ_indent();
	    want_cindent = FALSE;	// don't do it again
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!compl_autocomplete || compl_ਵਰਤਿਆ_ਮੇਲ)
    {
	ਪੂਰਨ prev_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

	// put the cursor on the last char, for 'tw' formatting
	ਜੇ (prev_ਖਾਨਾ > 0)
	    dec_cursor();
	// only format when something was inserted
	ਜੇ (!arrow_ਵਰਤਿਆ && !ins_need_undo_get() && c != Ctrl_E)
	    insertchar(NUL, 0, -1);
	ਜੇ (prev_ਖਾਨਾ > 0
		&& ml_get_curline()[curwin->w_cursor.ਖਾਨਾ] != NUL)
	    inc_cursor();
    }

    // If the popup menu is displayed pressing CTRL-Y means accepting
    // the selection without inserting anything.  When
    // compl_enter_selects is set the Enter key does the same.
    ਜੇ ((c == Ctrl_Y || (compl_enter_selects
		    && (c == CAR || c == K_KENTER || c == NL)))
	    && pum_visible())
    {
	ਸ਼ਬਦ.string = vim_strnsave(compl_shown_ਮੇਲ->cp_str.string,
	    compl_shown_ਮੇਲ->cp_str.ਲੰਮਾਈ);
	ਜੇ (ਸ਼ਬਦ.string == ਨਿਰਾ)
	    ਸ਼ਬਦ.ਲੰਮਾਈ = 0;
	ਨਹੀਂਤਾਂ
	    ਸ਼ਬਦ.ਲੰਮਾਈ = compl_shown_ਮੇਲ->cp_str.ਲੰਮਾਈ;
	retval = TRUE;
    }

    // CTRL-E means completion is Ended, go back to the typed text.
    // but only do this, if the Popup is still visible
    ਜੇ (c == Ctrl_E)
    {
	char_u *p = ਨਿਰਾ;
	size_t	plen = 0;

	ins_compl_delete();
	ਜੇ (compl_leader.string != ਨਿਰਾ)
	{
	    p = compl_leader.string;
	    plen = compl_leader.ਲੰਮਾਈ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ != ਨਿਰਾ)
	{
	    p = compl_orig_ਪਾਠ.string;
	    plen = compl_orig_ਪਾਠ.ਲੰਮਾਈ;
	}
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਪੂਰਨ	    compl_len = get_compl_len();

	    ਜੇ ((ਪੂਰਨ)plen > compl_len)
		ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(p + compl_len, (ਪੂਰਨ)plen - compl_len);
	}
	retval = TRUE;
    }

    auto_format(FALSE, TRUE);

    // Trigger the CompleteDonePre event to give scripts a chance to
    // act upon the completion before clearing the info, and restore
    // ctrl_x_mode, so that complete_info() can be used.
    ctrl_x_ਬਹੁਲਕ = prev_ਬਹੁਲਕ;
    ins_apply_autocmds(EVENT_COMPLETEDONEPRE);

    ins_compl_free();
    compl_started = FALSE;
    compl_matches = 0;
    ਜੇ (!shortmess(SHM_COMPLETIONMENU))
	ਸੁਨੇਹਾ_clr_cmdline();	// necessary for "noshowmode"
    ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਧਾਰਨ;
    compl_enter_selects = FALSE;
    ਜੇ (edit_submode != ਨਿਰਾ)
    {
	edit_submode = ਨਿਰਾ;
	showmode();
    }
    compl_autocomplete = FALSE;
    compl_from_nonkeyword = FALSE;
    compl_autostarted = false;
    compl_ਅੰਕ_bests = 0;
    compl_ins_ਸਿਰਾ_ਖਾਨਾ = 0;

    ਜੇ (c == Ctrl_C && cmdwin_type != 0)
	// Avoid the popup menu remains displayed when leaving the
	// command line window.
	update_screen(0);
    // Indent now if a key was typed that is in 'cinkeys'.
    ਜੇ (want_cindent && ਵਿੱਚ_cinkeys(ਕੁੰਜੀ_COMPLETE, ' ', inindent(0)))
	do_c_ਵਿਅੰਜਕ_indent();
    // Trigger the CompleteDone event to give scripts a chance to act
    // upon the end of completion.
    trigger_complete_ਪੂਰਾ_event(prev_ਬਹੁਲਕ, &ਸ਼ਬਦ);
    vim_free(ਸ਼ਬਦ.string);

    ਵਾਪਸ retval;
}

/*
 * Cancel completion.
 */
    ਪੂਰਨ
ins_compl_cancel(ਖਾਲੀ)
{
    ਵਾਪਸ ins_compl_stop(' ', ctrl_x_ਬਹੁਲਕ, TRUE);
}

/*
 * Prepare for Insert mode completion, or stop it.
 * Called just after typing a character in Insert mode.
 * Returns TRUE when the character is not to be inserted;
 */
    ਪੂਰਨ
ins_compl_prep(ਪੂਰਨ c)
{
    ਪੂਰਨ	    retval = FALSE;
    ਪੂਰਨ	    prev_ਬਹੁਲਕ = ctrl_x_ਬਹੁਲਕ;

    // Forget any previous 'special' messages if this is actually
    // a ^X mode key - bar ^R, in which case we wait to see what it gives us.
    ਜੇ (c != Ctrl_R && vim_is_ctrl_x_ਕੁੰਜੀ(c))
	edit_submode_extra = ਨਿਰਾ;

    // Ignore end of Select mode mapping and mouse scroll/movement.
    ਜੇ (c == K_SELECT || c == K_MOUSEDOWN || c == K_MOUSEUP
	    || c == K_MOUSELEFT || c == K_MOUSERIGHT || c == K_MOUSEMOVE
	    || c == K_COMMAND || c == K_SCRIPT_COMMAND)
	ਵਾਪਸ retval;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // Ignore mouse events in a popup window
    ਜੇ (is_mouse_ਕੁੰਜੀ(c))
    {
	// Ignore drag and release events, the position does not need to be in
	// the popup and it may have just closed.
	ਜੇ (c == K_LEFTRELEASE
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
	    ਵਾਪਸ retval;
	ਜੇ (popup_visible)
	{
	    ਪੂਰਨ	    ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	    ਪੂਰਨ	    ਖਾਨਾ = mouse_ਖਾਨਾ;
	    win_T   *wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);

	    ਜੇ (wp != ਨਿਰਾ && WIN_IS_POPUP(wp))
		ਵਾਪਸ retval;
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (ctrl_x_ਬਹੁਲਕ == CTRL_X_CMDLINE_CTRL_X && c != Ctrl_X)
    {
	ਜੇ (c == Ctrl_V || c == Ctrl_Q || c == Ctrl_Z || ins_compl_pum_ਕੁੰਜੀ(c)
		|| !vim_is_ctrl_x_ਕੁੰਜੀ(c))
	{
	    // Not starting another completion mode.
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_CMDLINE;

	    // CTRL-X CTRL-Z should stop completion without inserting anything
	    ਜੇ (c == Ctrl_Z)
		retval = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_CMDLINE;

	    // Other CTRL-X keys first stop completion, then start another
	    // completion mode.
	    ins_compl_prep(' ');
	    ctrl_x_ਬਹੁਲਕ = CTRL_X_NOT_DEFINED_YET;
	}
    }

    // Set "compl_get_longest" when finding the first matches.
    ਜੇ (ctrl_x_ਬਹੁਲਕ_not_defined_yet()
			   || (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && !compl_started))
    {
	compl_get_longest = (get_cot_flags() & COT_LONGEST);
	compl_ਵਰਤਿਆ_ਮੇਲ = TRUE;
    }

    ਜੇ (ctrl_x_ਬਹੁਲਕ_not_defined_yet())
	// We have just typed CTRL-X and aren't quite sure which CTRL-X mode
	// it will be yet.  Now we decide.
	retval = ਸੈੱਟ_ctrl_x_ਬਹੁਲਕ(c);
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_not_default())
    {
	// We're already in CTRL-X mode, do we stay in it?
	ਜੇ (c != Ctrl_R && !vim_is_ctrl_x_ਕੁੰਜੀ(c))
	{
	    ctrl_x_ਬਹੁਲਕ = ctrl_x_ਬਹੁਲਕ_scroll() ? CTRL_X_ਸਾਧਾਰਨ : CTRL_X_ਮੁਕੰਮਲ;
	    edit_submode = ਨਿਰਾ;
	}
	showmode();
    }

    ਜੇ (compl_started || ctrl_x_ਬਹੁਲਕ == CTRL_X_ਮੁਕੰਮਲ)
    {
	// Show error message from attempted keyword completion (probably
	// 'Pattern not found') until another key is hit, then go back to
	// showing what mode we are in.
	showmode();
	ਜੇ ((ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && c != Ctrl_N && c != Ctrl_P
				       && c != Ctrl_R && !ins_compl_pum_ਕੁੰਜੀ(c))
		|| ctrl_x_ਬਹੁਲਕ == CTRL_X_ਮੁਕੰਮਲ)
	    retval = ins_compl_stop(c, prev_ਬਹੁਲਕ, retval);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ == CTRL_X_LOCAL_ਸੁਨੇਹਾ)
	// Trigger the CompleteDone event to give scripts a chance to act
	// upon the (possibly failed) completion.
	trigger_complete_ਪੂਰਾ_event(ctrl_x_ਬਹੁਲਕ, ਨਿਰਾ);

    may_trigger_modechanged();

    // reset continue_* if we left expansion-mode, if we stay they'll be
    // (re)set properly in ins_complete()
    ਜੇ (c != Ctrl_R && !vim_is_ctrl_x_ਕੁੰਜੀ(c))
    {
	compl_cont_ਸਥਿਤੀ = 0;
	compl_cont_ਬਹੁਲਕ = 0;
    }

    ਵਾਪਸ retval;
}

/*
 * Fix the redo buffer for the completion leader replacing some of the typed
 * text.  This inserts backspaces and appends the changed text.
 * "ptr" is the known leader text or NUL.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    ਪੂਰਨ	    len = 0;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    ਜੇ (ptr == ਨਿਰਾ)
    {
	ਜੇ (compl_leader.string != ਨਿਰਾ)
	    ptr = compl_leader.string;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ;  // nothing to do
    }
    ਜੇ (compl_orig_ਪਾਠ.string != ਨਿਰਾ)
    {
	p = compl_orig_ਪਾਠ.string;
	// Find length of common prefix between original text and new completion
	ਜਦੋਂ (p[len] != NUL && p[len] == ptr[len])
	    len++;
	// Adjust length to not break inside a multi-byte character
	ਜੇ (len > 0)
	    len -= (*mb_ਸਿਰ_off)(p, p + len);
	// Add backspace characters for each remaining character in
	// original text
	ਲਈ (p += len; *p != NUL; MB_PTR_ADV(p))
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_BS);
    }
    ਜੇ (ptr != ਨਿਰਾ)
	Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(ptr + len, -1);
}

/*
 * Loops through the list of windows, loaded-buffers or non-loaded-buffers
 * (depending on flag) starting from buf and looking for a non-scanned
 * buffer (other than curbuf).	curbuf is special, if it is called with
 * buf=curbuf then it has to be the first call for a given flag/expansion.
 *
 * Returns the buffer to scan, if any, otherwise returns curbuf -- Acevedo
 */
    ਸਥਿਰ buf_T *
ins_compl_ਅਗਲਾ_buf(buf_T *buf, ਪੂਰਨ flag)
{
    ਸਥਿਰ win_T    *wp = ਨਿਰਾ;
    ਪੂਰਨ		    skip_ਬਫਰ;

    ਜੇ (flag == 'w')		// just windows
    {
	ਜੇ (buf == curbuf || !win_valid(wp))
	    // first call for this flag/expansion or window was closed
	    wp = curwin;

	ਜਦੋਂ (TRUE)
	{
	    // Move to next window (wrap to first window if at the end)
	    wp = (wp->w_ਅਗਲਾ != ਨਿਰਾ) ? wp->w_ਅਗਲਾ : firstwin;
	    // Break if we're back at start or found an unscanned buffer
	    ਜੇ (wp == curwin || !wp->w_ਬਫਰ->b_scanned)
		ਰੋਕੋ;
	}
	buf = wp->w_ਬਫਰ;
    }
    ਨਹੀਂਤਾਂ
    {
	// 'b' (just loaded buffers), 'u' (just non-loaded buffers) or 'U'
	// (unlisted buffers)
	// When completing whole lines skip unloaded buffers.
	ਜਦੋਂ (TRUE)
	{
	    // Move to next buffer (wrap to first buffer if at the end)
	    buf = (buf->b_ਅਗਲਾ != ਨਿਰਾ) ? buf->b_ਅਗਲਾ : firstbuf;
	    // Break if we're back at start buffer
	    ਜੇ (buf == curbuf)
		ਰੋਕੋ;

	    // Check buffer conditions based on flag
	    ਜੇ (flag == 'U')
		skip_ਬਫਰ = buf->b_p_bl;
	    ਨਹੀਂਤਾਂ
		skip_ਬਫਰ = !buf->b_p_bl ||
		    (buf->b_ml.ml_mfp == ਨਿਰਾ) != (flag == 'u');

	    // Break if we found a buffer that matches our criteria
	    ਜੇ (!skip_ਬਫਰ && !buf->b_scanned)
		ਰੋਕੋ;
	}
    }
    ਵਾਪਸ buf;
}

/*
 * Count the number of entries in the 'complete' option value "cpt".
 * Each non-empty, comma-separated segment is counted as one entry.
 */
    ਸਥਿਰ ਪੂਰਨ
get_cpt_sources_ਗਿਣਤੀ(char_u *cpt)
{
    char_u  dummy[LSIZE];
    ਪੂਰਨ	    ਗਿਣਤੀ_2 = 0;
    char_u  *p;

    ਲਈ (p = cpt; *p != NUL; )
    {
	ਜਦੋਂ (*p == ',' || *p == ' ')
	    p++;  // Skip delimiters

	ਜੇ (*p != NUL)
	{
	    (ਖਾਲੀ)copy_option_part(&p, dummy, LSIZE, ",");  // Advance p
	    ਗਿਣਤੀ_2++;
	}
    }

    ਵਾਪਸ ਗਿਣਤੀ_2;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC

# ifdef FEAT_EVAL
ਸਥਿਰ callback_T cfu_cb;	    // 'completefunc' callback function
ਸਥਿਰ callback_T ofu_cb;	    // 'omnifunc' callback function
ਸਥਿਰ callback_T tsrfu_cb;	    // 'thesaurusfunc' callback function
ਸਥਿਰ callback_T *cpt_cb;	    // Callback functions associated with F{func}
ਸਥਿਰ ਪੂਰਨ	  cpt_cb_ਗਿਣਤੀ;	    // Number of cpt callbacks
# endif

/*
 * Copy a global callback function to a buffer local callback.
 */
    ਸਥਿਰ ਖਾਲੀ
copy_global_ਮੰਜ਼ਿਲ_buflocal_cb(callback_T *globcb, callback_T *bufcb)
{
    free_callback(bufcb);
    ਜੇ (globcb->cb_name != ਨਿਰਾ && *globcb->cb_name != NUL)
	copy_callback(bufcb, globcb);
}

/*
 * Parse the 'completefunc' option value and set the callback function.
 * Invoked when the 'completefunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_completefunc(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ	retval;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
	retval = option_ਸੈੱਟ_callback_func(ਦਲੀਲਾਂ->os_newval.string,
							    &curbuf->b_cfu_cb);
    ਨਹੀਂਤਾਂ
    {
	retval = option_ਸੈੱਟ_callback_func(ਦਲੀਲਾਂ->os_newval.string, &cfu_cb);
	ਜੇ (retval == OK && !(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	    ਸੈੱਟ_buflocal_cfu_callback(curbuf);
    }

    ਵਾਪਸ retval == FAIL ? e_invalid_ਦਲੀਲ : ਨਿਰਾ;
}

/*
 * Copy the global 'completefunc' callback function to the buffer-local
 * 'completefunc' callback for "buf".
 */
    ਖਾਲੀ
ਸੈੱਟ_buflocal_cfu_callback(buf_T *buf ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    copy_global_ਮੰਜ਼ਿਲ_buflocal_cb(&cfu_cb, &buf->b_cfu_cb);
# endif
}

/*
 * Parse the 'omnifunc' option value and set the callback function.
 * Invoked when the 'omnifunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_omnifunc(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ	retval;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
	retval = option_ਸੈੱਟ_callback_func(ਦਲੀਲਾਂ->os_newval.string,
							    &curbuf->b_ofu_cb);
    ਨਹੀਂਤਾਂ
    {
	retval = option_ਸੈੱਟ_callback_func(ਦਲੀਲਾਂ->os_newval.string, &ofu_cb);
	ਜੇ (retval == OK && !(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	    ਸੈੱਟ_buflocal_ofu_callback(curbuf);
    }

    ਵਾਪਸ retval == FAIL ? e_invalid_ਦਲੀਲ : ਨਿਰਾ;
}

/*
 * Copy the global 'omnifunc' callback function to the buffer-local 'omnifunc'
 * callback for "buf".
 */
    ਖਾਲੀ
ਸੈੱਟ_buflocal_ofu_callback(buf_T *buf ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    copy_global_ਮੰਜ਼ਿਲ_buflocal_cb(&ofu_cb, &buf->b_ofu_cb);
# endif
}

/*
 * Free an array of 'complete' F{func} callbacks and set the pointer to NULL.
 */
    ਖਾਲੀ
clear_cpt_callbacks(callback_T **callbacks, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (callbacks == ਨਿਰਾ || *callbacks == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; i++)
	free_callback(&(*callbacks)[i]);

    VIM_CLEAR(*callbacks);
}

/*
 * Copies a list of callback_T structs from src to *dest, clearing any existing
 * entries and allocating memory for the destination.
 */
    ਸਥਿਰ ਪੂਰਨ
copy_cpt_callbacks(callback_T **dest, ਪੂਰਨ *dest_cnt, callback_T *src, ਪੂਰਨ cnt)
{
    clear_cpt_callbacks(dest, *dest_cnt);
    *dest_cnt = 0;

    ਜੇ (cnt == 0)
	ਵਾਪਸ OK;

    *dest = ALLOC_CLEAR_MULT(callback_T, cnt);
    ਜੇ (*dest == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    *dest_cnt = cnt;

    ਲਈ (ਪੂਰਨ i = 0; i < cnt; i++)
	ਜੇ (src[i].cb_name != ਨਿਰਾ && *(src[i].cb_name) != NUL)
	    copy_callback(&(*dest)[i], &src[i]);

    ਵਾਪਸ OK;
}

/*
 * Copy global 'complete' F{func} callbacks into the given buffer's local
 * callback array. Clears any existing buffer-local callbacks first.
 */
    ਖਾਲੀ
ਸੈੱਟ_buflocal_cpt_callbacks(buf_T *buf ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    (ਖਾਲੀ)copy_cpt_callbacks(&buf->b_p_cpt_cb, &buf->b_p_cpt_ਗਿਣਤੀ, cpt_cb,
	    cpt_cb_ਗਿਣਤੀ);
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
    ਸਥਿਰ ਪੂਰਨ
parse_cpt_callbacks(char_u *cpt, callback_T **cbp, ਪੂਰਨ *cnt)
{
    char_u  buf[LSIZE];
    char_u  *p;
    ਪੂਰਨ	    idx = 0;
    ਪੂਰਨ	    slen;
    ਪੂਰਨ	    ਗਿਣਤੀ_2;

    clear_cpt_callbacks(cbp, *cnt);
    *cnt = 0;

    ਗਿਣਤੀ_2 = get_cpt_sources_ਗਿਣਤੀ(cpt);
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ OK;

    *cbp = ALLOC_CLEAR_MULT(callback_T, ਗਿਣਤੀ_2);
    ਜੇ (*cbp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    *cnt = ਗਿਣਤੀ_2;

    ਲਈ (p = cpt; *p != NUL; )
    {
	ਜਦੋਂ (*p == ',' || *p == ' ')
	    p++; // Skip delimiters

	ਜੇ (*p != NUL)
	{
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ਜੇ (slen > 0 && buf[0] == 'F' && buf[1] != NUL)
	    {
		char_u	*caret = vim_strchr(buf, '^');

		ਜੇ (caret != ਨਿਰਾ)
		    *caret = NUL;

		ਜੇ (option_ਸੈੱਟ_callback_func(buf + 1, &(*cbp)[idx]) != OK)
		    (*cbp)[idx].cb_name = ਨਿਰਾ;
	    }
	    idx++;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Parse 'complete' option and initialize F{func} callbacks.
 * Frees any existing callbacks and allocates new ones.
 */
    ਪੂਰਨ
ਸੈੱਟ_cpt_callbacks(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ	    opt_flags = ਦਲੀਲਾਂ->os_flags;

    ਜੇ (curbuf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // ":setglobal" does not change the buffer-local option value
    ਜੇ (!(opt_flags & OPT_GLOBAL)
	    && parse_cpt_callbacks(curbuf->b_p_cpt, &curbuf->b_p_cpt_cb,
				   &curbuf->b_p_cpt_ਗਿਣਤੀ) != OK)
	ਵਾਪਸ FAIL;

    // ":setlocal" does not change the global option value
    ਜੇ (opt_flags & OPT_LOCAL)
	ਵਾਪਸ OK;

    // Cache the callbacks for the global value
    ਜੇ (opt_flags & OPT_GLOBAL)
	ਵਾਪਸ parse_cpt_callbacks(p_cpt, &cpt_cb, &cpt_cb_ਗਿਣਤੀ);

    // set case
    ਵਾਪਸ copy_cpt_callbacks(&cpt_cb, &cpt_cb_ਗਿਣਤੀ, curbuf->b_p_cpt_cb,
			      curbuf->b_p_cpt_ਗਿਣਤੀ);
}

/*
 * Parse the 'thesaurusfunc' option value and set the callback function.
 * Invoked when the 'thesaurusfunc' option is set. The option value can be a
 * name of a function (string), or function(<name>) or funcref(<name>) or a
 * lambda expression.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_thesaurusfunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	retval;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_ਸੈੱਟ_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    ਨਹੀਂਤਾਂ
    {
	// global option set
	retval = option_ਸੈੱਟ_callback_func(p_tsrfu, &tsrfu_cb);
	// when using :set, free the local callback
	ਜੇ (!(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_tsrfu_cb);
    }

    ਵਾਪਸ retval == FAIL ? e_invalid_ਦਲੀਲ : ਨਿਰਾ;
}

/*
 * Mark "copyID" references in an array of F{func} callbacks so that they are
 * not garbage collected.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_cpt_callbacks(callback_T *callbacks, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ copyID)
{
    ਪੂਰਨ	abort = FALSE;

    ਜੇ (callbacks == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; i++)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&callbacks[i], copyID);
    ਵਾਪਸ abort;
}

/*
 * Mark the global 'completefunc' 'omnifunc' and 'thesaurusfunc' callbacks with
 * "copyID" so that they are not garbage collected.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_insexpand_funcs(ਪੂਰਨ copyID)
{
    ਪੂਰਨ abort = ਸੈੱਟ_ref_ਵਿੱਚ_callback(&cfu_cb, copyID);
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&ofu_cb, copyID);
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&tsrfu_cb, copyID);
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_cpt_callbacks(cpt_cb, cpt_cb_ਗਿਣਤੀ, copyID);

    ਵਾਪਸ abort;
}

/*
 * Get the user-defined completion function name for completion "type"
 */
    ਸਥਿਰ char_u *
get_complete_funcname(ਪੂਰਨ type)
{
    ਚੋਣ (type)
    {
	ਹਾਲਤ CTRL_X_FUNCTION:
	    ਵਾਪਸ curbuf->b_p_cfu;
	ਹਾਲਤ CTRL_X_OMNI:
	    ਵਾਪਸ curbuf->b_p_ofu;
	ਹਾਲਤ CTRL_X_THESAURUS:
	    ਵਾਪਸ *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	ਮੂਲ:
	    ਵਾਪਸ (char_u *)"";
    }
}

/*
 * Get the callback to use for insert mode completion.
 */
    ਸਥਿਰ callback_T *
get_ਪਾਓ_callback(ਪੂਰਨ type)
{
    ਜੇ (type == CTRL_X_FUNCTION)
	ਵਾਪਸ &curbuf->b_cfu_cb;
    ਜੇ (type == CTRL_X_OMNI)
	ਵਾਪਸ &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    ਵਾਪਸ (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}

/*
 * Execute user defined complete function 'completefunc', 'omnifunc' or
 * 'thesaurusfunc', and get matches in "matches".
 * "type" can be one of CTRL_X_OMNI, CTRL_X_FUNCTION, or CTRL_X_THESAURUS.
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 */
    ਸਥਿਰ ਖਾਲੀ
expand_by_function(ਪੂਰਨ type, char_u *ਅਧਾਰ, callback_T *cb)
{
    list_T      *matchlist = ਨਿਰਾ;
    dict_T	*matchdict = ਨਿਰਾ;
    typval_T	ਦਲੀਲਾਂ[3];
    char_u	*funcname;
    pos_T	pos;
    typval_T	rettv;
    ਪੂਰਨ		save_State = State;
    ਪੂਰਨ		retval;
    ਪੂਰਨ		is_cpt_function = (cb != ਨਿਰਾ);
    ਪੂਰਨ		use_sandbox = is_cpt_function
			    && was_ਸੈੱਟ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ਜੇ (!is_cpt_function)
    {
	funcname = get_complete_funcname(type);
	ਜੇ (*funcname == NUL)
	    ਵਾਪਸ;
	cb = get_ਪਾਓ_callback(type);
    }

    // Call 'completefunc' to obtain the list of matches.
    ਦਲੀਲਾਂ[0].v_type = VAR_ਸੰਖਿਆ;
    ਦਲੀਲਾਂ[0].vval.v_ਸੰਖਿਆ = 0;
    ਦਲੀਲਾਂ[1].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[1].vval.v_string = ਅਧਾਰ != ਨਿਰਾ ? ਅਧਾਰ : (char_u *)"";
    ਦਲੀਲਾਂ[2].v_type = VAR_UNKNOWN;

    pos = curwin->w_cursor;
    // Lock the text to avoid weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;
    ਜੇ (use_sandbox)
	++sandbox;

    retval = call_callback(cb, 0, &rettv, 2, ਦਲੀਲਾਂ);
    ਜੇ (use_sandbox)
	--sandbox;

    // Call a function, which returns a list or dict.
    ਜੇ (retval == OK)
    {
	ਚੋਣ (rettv.v_type)
	{
	    ਹਾਲਤ VAR_LIST:
		matchlist = rettv.vval.v_list;
		ਰੋਕੋ;
	    ਹਾਲਤ VAR_DICT:
		matchdict = rettv.vval.v_dict;
		ਰੋਕੋ;
	    ਹਾਲਤ VAR_SPECIAL:
		ਜੇ (rettv.vval.v_ਸੰਖਿਆ == VVAL_NONE)
		    compl_opt_suppress_ਖਾਲੀ_ਥਾਵਾਂ = TRUE;
		// FALLTHROUGH
	    ਮੂਲ:
		// TODO: Give error message?
		clear_tv(&rettv);
		ਰੋਕੋ;
	}
    }
    --textlock;

    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ਜੇ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_ਪਾਠ));
	ਜਾਓ theend;
    }

    ਜੇ (matchlist != ਨਿਰਾ)
	ins_compl_ਜੋੜੋ_list(matchlist);
    ਨਹੀਂਤਾਂ ਜੇ (matchdict != ਨਿਰਾ)
	ins_compl_ਜੋੜੋ_dict(matchdict);

theend:
    // Restore State, it might have been changed.
    State = save_State;

    ਜੇ (matchdict != ਨਿਰਾ)
	dict_unref(matchdict);
    ਜੇ (matchlist != ਨਿਰਾ)
	list_unref(matchlist);
}
#ਜੇ_ਅੰਤ // FEAT_COMPL_FUNC

#ਜੇ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)

    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
get_user_highlight_id(char_u *hlname)
{
    ਜੇ (hlname != ਨਿਰਾ && *hlname != NUL)
	ਵਾਪਸ syn_check_ਸਮੂਹ(hlname, (ਪੂਰਨ)STRLEN(hlname));
    ਵਾਪਸ 0;
}
/*
 * Add a match to the list of matches from a typeval_T.
 * If the given string is already in the list of completions, then return
 * NOTDONE, otherwise add it to the list and return OK.  If there is an error,
 * maybe because alloc() returns NULL, then FAIL is returned.
 * When "fast" is TRUE use fast_breakcheck() instead of ui_breakcheck().
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_ਜੋੜੋ_tv(typval_T *tv, ਪੂਰਨ dir, ਪੂਰਨ fast)
{
    char_u	*ਸ਼ਬਦ;
    ਪੂਰਨ		dup = FALSE;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2 = FALSE;
    ਪੂਰਨ		flags = fast ? CP_FAST : 0;
    char_u	*(cptext[CPT_ਗਿਣਤੀ]);
    typval_T	user_ਅੰਕੜੇ;
    ਪੂਰਨ		ਸਥਿਤੀ;
    char_u	*user_abbr_hlname;
    char_u	*user_ਕਿਸਮ_hlname;
    ਪੂਰਨ		user_hl[2] = { 0, 0 };

    user_ਅੰਕੜੇ.v_type = VAR_UNKNOWN;
    ਜੇ (tv->v_type == VAR_DICT && tv->vval.v_dict != ਨਿਰਾ)
    {
	ਸ਼ਬਦ = dict_get_string(tv->vval.v_dict, "word", FALSE);
	cptext[CPT_ABBR] = dict_get_string(tv->vval.v_dict, "abbr", FALSE);
	cptext[CPT_MENU] = dict_get_string(tv->vval.v_dict, "menu", FALSE);
	cptext[CPT_ਕਿਸਮ] = dict_get_string(tv->vval.v_dict, "kind", FALSE);
	cptext[CPT_INFO] = dict_get_string(tv->vval.v_dict, "info", FALSE);

	user_abbr_hlname = dict_get_string(tv->vval.v_dict, "abbr_hlgroup", FALSE);
	user_hl[0] = get_user_highlight_id(user_abbr_hlname);

	user_ਕਿਸਮ_hlname = dict_get_string(tv->vval.v_dict, "kind_hlgroup", FALSE);
	user_hl[1] = get_user_highlight_id(user_ਕਿਸਮ_hlname);

	dict_get_tv(tv->vval.v_dict, "user_data", &user_ਅੰਕੜੇ);
	ਜੇ (dict_get_string(tv->vval.v_dict, "icase", FALSE) != ਨਿਰਾ
				  && dict_get_ਸੰਖਿਆ(tv->vval.v_dict, "icase"))
	    flags |= CP_ICASE;
	ਜੇ (dict_get_string(tv->vval.v_dict, "dup", FALSE) != ਨਿਰਾ)
	    dup = dict_get_ਸੰਖਿਆ(tv->vval.v_dict, "dup");
	ਜੇ (dict_get_string(tv->vval.v_dict, "empty", FALSE) != ਨਿਰਾ)
	    ਖਾਲੀ_ਥਾਵਾਂ_2 = dict_get_ਸੰਖਿਆ(tv->vval.v_dict, "empty");
	ਜੇ (dict_get_string(tv->vval.v_dict, "equal", FALSE) != ਨਿਰਾ
				  && dict_get_ਸੰਖਿਆ(tv->vval.v_dict, "equal"))
	    flags |= CP_EQUAL;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸ਼ਬਦ = tv_get_string_chk(tv);
	CLEAR_ਖੇਤਰ(cptext);
    }
    ਜੇ (ਸ਼ਬਦ == ਨਿਰਾ || (!ਖਾਲੀ_ਥਾਵਾਂ_2 && *ਸ਼ਬਦ == NUL))
    {
	clear_tv(&user_ਅੰਕੜੇ);
	ਵਾਪਸ FAIL;
    }
    ਸਥਿਤੀ = ins_compl_ਜੋੜੋ(ਸ਼ਬਦ, -1, ਨਿਰਾ, cptext,
	    &user_ਅੰਕੜੇ, dir, flags, dup, user_hl, FUZZY_SCORE_NONE);
    ਜੇ (ਸਥਿਤੀ != OK)
	clear_tv(&user_ਅੰਕੜੇ);
    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Add completions from a list.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਜੋੜੋ_list(list_T *list)
{
    listitem_T	*li;
    ਪੂਰਨ		dir = compl_direction;

    // Go through the List with matches and add each of them.
    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, li)
    {
	ਜੇ (ins_compl_ਜੋੜੋ_tv(&li->li_tv, dir, TRUE) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
	ਨਹੀਂਤਾਂ ਜੇ (did_emsg)
	    ਰੋਕੋ;
    }
}

/*
 * Add completions from a dict.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਜੋੜੋ_dict(dict_T *dict)
{
    dictitem_T	*di_refresh;
    dictitem_T	*di_ਸ਼ਬਦ;

    // Check for optional "refresh" item.
    compl_opt_refresh_always = FALSE;
    di_refresh = dict_ਲੱਭੋ(dict, (char_u *)"refresh", 7);
    ਜੇ (di_refresh != ਨਿਰਾ && di_refresh->di_tv.v_type == VAR_STRING)
    {
	char_u	*v = di_refresh->di_tv.vval.v_string;

	ਜੇ (v != ਨਿਰਾ && STRCMP(v, (char_u *)"always") == 0)
	    compl_opt_refresh_always = TRUE;
    }

    // Add completions from a "words" list.
    di_ਸ਼ਬਦ = dict_ਲੱਭੋ(dict, (char_u *)"words", 5);
    ਜੇ (di_ਸ਼ਬਦ != ਨਿਰਾ && di_ਸ਼ਬਦ->di_tv.v_type == VAR_LIST)
	ins_compl_ਜੋੜੋ_list(di_ਸ਼ਬਦ->di_tv.vval.v_list);
}

/*
 * Start completion for the complete() function.
 * "startcol" is where the matched text starts (1 is first column).
 * "list" is the list of matches.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_completion(colnr_T startcol, list_T *list)
{
    ਪੂਰਨ save_w_wrow = curwin->w_wrow;
    ਪੂਰਨ save_w_leftcol = curwin->w_leftcol;
    ਪੂਰਨ flags = CP_ORIGINAL_ਪਾਠ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_cot_flags = get_cot_flags();
    ਪੂਰਨ compl_longest = (cur_cot_flags & COT_LONGEST) != 0;
    ਪੂਰਨ compl_no_ਪਾਓ = (cur_cot_flags & COT_NOINSERT) != 0;
    ਪੂਰਨ compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0;

    // If already doing completions stop it.
    ਜੇ (compl_started || ctrl_x_ਬਹੁਲਕ_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();
    compl_get_longest = compl_longest;

    compl_direction = FORWARD;
    ਜੇ (startcol > curwin->w_cursor.ਖਾਨਾ)
	startcol = curwin->w_cursor.ਖਾਨਾ;
    compl_ਖਾਨਾ = startcol;
    compl_lnum = curwin->w_cursor.lnum;
    compl_ਲੰਮਾਈ = (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ - (ਪੂਰਨ)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_ਪਾਠ.string = vim_strnsave(ml_get_curline() + compl_ਖਾਨਾ,
							(size_t)compl_ਲੰਮਾਈ);
    ਜੇ (p_ic)
	flags |= CP_ICASE;
    ਜੇ (compl_orig_ਪਾਠ.string == ਨਿਰਾ)
    {
	compl_orig_ਪਾਠ.ਲੰਮਾਈ = 0;
	ਵਾਪਸ;
    }
    compl_orig_ਪਾਠ.ਲੰਮਾਈ = (size_t)compl_ਲੰਮਾਈ;
    ਜੇ (ins_compl_ਜੋੜੋ(compl_orig_ਪਾਠ.string,
			(ਪੂਰਨ)compl_orig_ਪਾਠ.ਲੰਮਾਈ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, 0,
			flags | CP_FAST, FALSE, ਨਿਰਾ, FUZZY_SCORE_NONE) != OK)
	ਵਾਪਸ;

    ctrl_x_ਬਹੁਲਕ = CTRL_X_EVAL;

    ins_compl_ਜੋੜੋ_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_ਵਰਤਿਆ_ਮੇਲ = TRUE;
    compl_cont_ਸਥਿਤੀ = 0;

    compl_curr_ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਪੂਰਨ no_select = compl_no_select || compl_longest;
    ਜੇ (compl_no_ਪਾਓ || no_select)
    {
	ins_complete(K_ਥੱਲੇ, FALSE);
	ਜੇ (no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_ਉੱਪਰ, FALSE);
    }
    ਨਹੀਂਤਾਂ
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_ਪਾਓ;

    // Lazily show the popup menu, unless we got interrupted.
    ਜੇ (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    ਬਾਹਰਲਾ_flush();
}

/*
 * "complete()" function
 */
    ਖਾਲੀ
f_complete(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) == 0)
    {
	emsg(_(e_complete_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_ਪਾਓ_ਬਹੁਲਕ));
	ਵਾਪਸ;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    ਜੇ (!undo_allowed())
	ਵਾਪਸ;

    ਜੇ (check_for_nonnull_list_arg(argvars, 1) != FAIL)
    {
	ਪੂਰਨ startcol = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ);
	ਜੇ (startcol > 0)
	    ਸੈੱਟ_completion(startcol - 1, argvars[1].vval.v_list);
    }
}

/*
 * "complete_add()" function
 */
    ਖਾਲੀ
f_complete_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = ins_compl_ਜੋੜੋ_tv(&argvars[0], 0, FALSE);
}

/*
 * "complete_check()" function
 */
    ਖਾਲੀ
f_complete_check(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ins_compl_check_ਕੁੰਜੀਆਂ(0, TRUE);
    rettv->vval.v_ਸੰਖਿਆ = ins_compl_interrupted();

    RedrawingDisabled = save_RedrawingDisabled;
}

/*
 * Return Insert completion mode name string
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_ਬਹੁਲਕ(string_T *ret)
{
    ਜੇ (ctrl_x_ਬਹੁਲਕ_not_defined_yet() || ctrl_x_ਬਹੁਲਕ_scroll() || compl_started)
    {
	string_T    *ਬਹੁਲਕ;

	ਬਹੁਲਕ = &ctrl_x_ਬਹੁਲਕ_ਨਾਂ[ctrl_x_ਬਹੁਲਕ & ~CTRL_X_WANT_IDENT];
	ret->string = (ਬਹੁਲਕ->string == ਨਿਰਾ) ? (char_u *)"" : ਬਹੁਲਕ->string;
	ret->ਲੰਮਾਈ = ਬਹੁਲਕ->ਲੰਮਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	ret->string = (char_u *)"";
	ret->ਲੰਮਾਈ = 0;
    }
}

/*
 * Assign the sequence number to all the completion matches which don't have
 * one assigned yet.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_update_sequence_numbers(ਖਾਲੀ)
{
    ਪੂਰਨ		ਸੰਖਿਆ = 0;
    compl_T	*ਮੇਲ;

    ਜੇ (compl_dir_forward())
    {
	// Search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	ਲਈ (ਮੇਲ = compl_curr_ਮੇਲ->cp_prev; ਮੇਲ != ਨਿਰਾ
		&& !is_ਪਹਿਲਾ_ਮੇਲ(ਮੇਲ); ਮੇਲ = ਮੇਲ->cp_prev)
	    ਜੇ (ਮੇਲ->cp_ਸੰਖਿਆ != -1)
	    {
		ਸੰਖਿਆ = ਮੇਲ->cp_ਸੰਖਿਆ;
		ਰੋਕੋ;
	    }
	ਜੇ (ਮੇਲ != ਨਿਰਾ)
	    // go up and assign all numbers which are not assigned yet
	    ਲਈ (ਮੇਲ = ਮੇਲ->cp_ਅਗਲਾ;
		    ਮੇਲ != ਨਿਰਾ && ਮੇਲ->cp_ਸੰਖਿਆ == -1;
					   ਮੇਲ = ਮੇਲ->cp_ਅਗਲਾ)
		ਮੇਲ->cp_ਸੰਖਿਆ = ++ਸੰਖਿਆ;
    }
    ਨਹੀਂਤਾਂ // BACKWARD
    {
	// Search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	ਲਈ (ਮੇਲ = compl_curr_ਮੇਲ->cp_ਅਗਲਾ; ਮੇਲ != ਨਿਰਾ
		&& !is_ਪਹਿਲਾ_ਮੇਲ(ਮੇਲ); ਮੇਲ = ਮੇਲ->cp_ਅਗਲਾ)
	{
	    ਜੇ (ਮੇਲ->cp_ਸੰਖਿਆ != -1)
	    {
		ਸੰਖਿਆ = ਮੇਲ->cp_ਸੰਖਿਆ;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਮੇਲ != ਨਿਰਾ)
	{
	    // go down and assign all numbers which are not assigned yet
	    ਲਈ (ਮੇਲ = ਮੇਲ->cp_prev; ਮੇਲ
		    && ਮੇਲ->cp_ਸੰਖਿਆ == -1;
					   ਮੇਲ = ਮੇਲ->cp_prev)
		ਮੇਲ->cp_ਸੰਖਿਆ = ++ਸੰਖਿਆ;
	}
    }
}

/*
 * Fill the dict of complete_info
 */
    ਸਥਿਰ ਖਾਲੀ
fill_complete_info_dict(dict_T *di, compl_T *ਮੇਲ, ਪੂਰਨ ਜੋੜੋ_ਮੇਲ)
{
    dict_ਜੋੜੋ_string_len(di, "word", ਮੇਲ->cp_str.string, (ਪੂਰਨ)ਮੇਲ->cp_str.ਲੰਮਾਈ);
    dict_ਜੋੜੋ_string(di, "abbr", ਮੇਲ->cp_ਪਾਠ[CPT_ABBR]);
    dict_ਜੋੜੋ_string(di, "menu", ਮੇਲ->cp_ਪਾਠ[CPT_MENU]);
    dict_ਜੋੜੋ_string(di, "kind", ਮੇਲ->cp_ਪਾਠ[CPT_ਕਿਸਮ]);
    dict_ਜੋੜੋ_string(di, "info", ਮੇਲ->cp_ਪਾਠ[CPT_INFO]);
    dict_ਜੋੜੋ_string(di, "abbr_hlgroup", syn_id2name(ਮੇਲ->cp_user_abbr_hl_id));
    dict_ਜੋੜੋ_string(di, "kind_hlgroup", syn_id2name(ਮੇਲ->cp_user_ਕਿਸਮ_hl_id));
    ਜੇ (ਜੋੜੋ_ਮੇਲ)
	dict_ਜੋੜੋ_bool(di, "match", ਮੇਲ->cp_ਵਿੱਚ_ਮੇਲ_array);
    ਜੇ (ਮੇਲ->cp_user_ਅੰਕੜੇ.v_type == VAR_UNKNOWN)
	// Add an empty string for backwards compatibility
	dict_ਜੋੜੋ_string_len(di, "user_data", (char_u *)"", 0);
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_tv(di, "user_data", &ਮੇਲ->cp_user_ਅੰਕੜੇ);
}

/*
 * Get complete information
 */
    ਸਥਿਰ ਖਾਲੀ
get_complete_info(list_T *what_list, dict_T *retdict)
{
    ਪੂਰਨ		ret = OK;
    listitem_T	*ਵਸਤੂ;
# define CI_WHAT_ਬਹੁਲਕ		    0x01
# define CI_WHAT_PUM_VISIBLE	    0x02
# define CI_WHAT_ਚੀਜ਼ਾਂ		    0x04
# define CI_WHAT_SELECTED	    0x08
# define CI_WHAT_COMPLETED	    0x10
# define CI_WHAT_MATCHES	    0x20
# define CI_WHAT_PREINSERTED_ਪਾਠ   0x40
# define CI_WHAT_AUTO		    0x80
# define CI_WHAT_ALL		    0xff
    ਪੂਰਨ		what_flag;

    ਜੇ (what_list == ਨਿਰਾ)
	what_flag = CI_WHAT_ALL
	    & ~(CI_WHAT_MATCHES | CI_WHAT_COMPLETED | CI_WHAT_AUTO);
    ਨਹੀਂਤਾਂ
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(what_list, ਵਸਤੂ)
	{
	    char_u *what = tv_get_string(&ਵਸਤੂ->li_tv);

	    ਜੇ (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_ਬਹੁਲਕ;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ਚੀਜ਼ਾਂ;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "completed") == 0)
		what_flag |= CI_WHAT_COMPLETED;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "preinserted_text") == 0)
		what_flag |= CI_WHAT_PREINSERTED_ਪਾਠ;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "matches") == 0)
		what_flag |= CI_WHAT_MATCHES;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "auto") == 0)
		what_flag |= CI_WHAT_AUTO;
	}
    }

    ਜੇ (ret == OK && (what_flag & CI_WHAT_ਬਹੁਲਕ))
    {
	string_T    ਬਹੁਲਕ;

	ins_compl_ਬਹੁਲਕ(&ਬਹੁਲਕ);
	ret = dict_ਜੋੜੋ_string_len(retdict, "mode", ਬਹੁਲਕ.string, (ਪੂਰਨ)ਬਹੁਲਕ.ਲੰਮਾਈ);
    }

    ਜੇ (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "pum_visible", pum_visible());

    ਜੇ (ret == OK && (what_flag & CI_WHAT_AUTO))
	ret = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "auto", compl_autostarted);

    ਜੇ (ret == OK && (what_flag & CI_WHAT_PREINSERTED_ਪਾਠ))
    {
	char_u	*ਸਤਰ = ml_get_curline();
	ਪੂਰਨ	len = compl_ins_ਸਿਰਾ_ਖਾਨਾ - curwin->w_cursor.ਖਾਨਾ;

	ret = dict_ਜੋੜੋ_string_len(retdict, "preinserted_text",
		(len > 0) ? ਸਤਰ + curwin->w_cursor.ਖਾਨਾ : (char_u *)"", (len > 0) ? len : 0);
    }

    ਜੇ (ret == OK && (what_flag & (CI_WHAT_ਚੀਜ਼ਾਂ | CI_WHAT_SELECTED
				    | CI_WHAT_MATCHES | CI_WHAT_COMPLETED)))
    {
	list_T	    *li = ਨਿਰਾ;
	dict_T	    *di;
	compl_T     *ਮੇਲ;
	ਪੂਰਨ         selected_idx = -1;
	ਪੂਰਨ	    has_ਚੀਜ਼ਾਂ = what_flag & CI_WHAT_ਚੀਜ਼ਾਂ;
	ਪੂਰਨ	    has_matches = what_flag & CI_WHAT_MATCHES;
	ਪੂਰਨ	    has_completed = what_flag & CI_WHAT_COMPLETED;

	ਜੇ (has_ਚੀਜ਼ਾਂ || has_matches)
	{
	    li = list_alloc();
	    ਜੇ (li == ਨਿਰਾ)
		ਵਾਪਸ;
	    ret = dict_ਜੋੜੋ_list(retdict, (has_matches && !has_ਚੀਜ਼ਾਂ)
						? "matches" : "items", li);
	    ਜੇ (ret == FAIL)
		list_unref(li);
	}
	ਜੇ (ret == OK && what_flag & CI_WHAT_SELECTED)
	    ਜੇ (compl_curr_ਮੇਲ != ਨਿਰਾ && compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ == -1)
		ins_compl_update_sequence_numbers();
	ਜੇ (ret == OK && compl_ਪਹਿਲਾ_ਮੇਲ != ਨਿਰਾ)
	{
	    ਪੂਰਨ list_idx = 0;
	    ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
	    ਕਰੋ
	    {
		ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(ਮੇਲ))
		{
		    ਜੇ (has_ਚੀਜ਼ਾਂ
			    || (has_matches && ਮੇਲ->cp_ਵਿੱਚ_ਮੇਲ_array))
		    {
			di = dict_alloc();
			ਜੇ (di == ਨਿਰਾ)
			    ਵਾਪਸ;
			ret = list_append_dict(li, di);
			ਜੇ (ret != OK)
			{
			    dict_unref(di);
			    ਵਾਪਸ;
			}
			fill_complete_info_dict(di, ਮੇਲ, has_matches && has_ਚੀਜ਼ਾਂ);
		    }
		    ਜੇ (compl_curr_ਮੇਲ != ਨਿਰਾ
			    && compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ == ਮੇਲ->cp_ਸੰਖਿਆ)
			selected_idx = list_idx;
		    ਜੇ (!has_matches || ਮੇਲ->cp_ਵਿੱਚ_ਮੇਲ_array)
			list_idx++;
		}
		ਮੇਲ = ਮੇਲ->cp_ਅਗਲਾ;
	    }
	    ਜਦੋਂ (ਮੇਲ != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(ਮੇਲ));
	}
	ਜੇ (ret == OK && (what_flag & CI_WHAT_SELECTED))
	    ret = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "selected", selected_idx);

	ਜੇ (ret == OK && selected_idx != -1 && has_completed)
	{
	    di = dict_alloc();
	    ਜੇ (di == ਨਿਰਾ)
		ਵਾਪਸ;
	    fill_complete_info_dict(di, compl_curr_ਮੇਲ, FALSE);
	    ret = dict_ਜੋੜੋ_dict(retdict, "completed", di);
	    ਜੇ (ret == FAIL)
		dict_unref(di);
	}
    }
}

/*
 * "complete_info()" function
 */
    ਖਾਲੀ
f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = ਨਿਰਾ;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	    ਵਾਪਸ;
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}
#ਜੇ_ਅੰਤ

/*
 * Returns TRUE when using a user-defined function for thesaurus completion.
 */
    ਸਥਿਰ ਪੂਰਨ
thesaurus_func_complete(ਪੂਰਨ type ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    ਵਾਪਸ type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Check if 'cpt' list index can be advanced to the next completion source.
 */
    ਸਥਿਰ ਪੂਰਨ
may_advance_cpt_ਸੂਚਕਾਂਕ(char_u *cpt)
{
    char_u  *p = cpt;

    ਜੇ (cpt_sources_ਸੂਚਕਾਂਕ == -1)
	ਵਾਪਸ FALSE;
    ਜਦੋਂ (*p == ',' || *p == ' ') // Skip delimiters
	p++;
    ਵਾਪਸ (*p != NUL);
}

/*
 * Return value of process_next_cpt_value()
 */
ਗਿਣਤੀ
{
    INS_COMPL_CPT_OK = 1,
    INS_COMPL_CPT_CONT,
    INS_COMPL_CPT_ਸਿਰਾ
};

/*
 * state information used for getting the next set of insert completion
 * matches.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*e_cpt_copy;		// copy of 'complete'
    char_u	*e_cpt;			// current entry in "e_cpt_copy"
    buf_T	*ins_buf;		// buffer being scanned
    pos_T	*cur_ਮੇਲ_pos;		// current match position
    pos_T	prev_ਮੇਲ_pos;		// previous match position
    ਪੂਰਨ		ਸੈੱਟ_ਮੇਲ_pos;		// save first_match_pos/last_match_pos
    pos_T	ਪਹਿਲਾ_ਮੇਲ_pos;	// first match position
    pos_T	ਆਖਰੀ_ਮੇਲ_pos;		// last match position
    ਪੂਰਨ		ਮਿਲਿਆ_all;		// found all matches of a certain type.
    char_u	*dict;			// dictionary file to search
    ਪੂਰਨ		dict_f;			// "dict" is an exact file name or not
    callback_T	*func_cb;		// callback of function in 'cpt' option
} ins_compl_ਅਗਲਾ_state_T;

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
    ਸਥਿਰ ਪੂਰਨ
process_ਅਗਲਾ_cpt_ਮੁੱਲ(
	ins_compl_ਅਗਲਾ_state_T *st,
	ਪੂਰਨ		*compl_type_arg,
	pos_T		*ਸ਼ੁਰੂ_ਮੇਲ_pos,
	ਪੂਰਨ		fuzzy_collect,
	ਪੂਰਨ		*advance_cpt_idx)
{
    ਪੂਰਨ	    compl_type = -1;
    ਪੂਰਨ	    ਸਥਿਤੀ = INS_COMPL_CPT_OK;
    ਪੂਰਨ	    skip_ਸਰੋਤ = compl_autocomplete && compl_from_nonkeyword;

    st->ਮਿਲਿਆ_all = FALSE;
    *advance_cpt_idx = FALSE;

    ਜਦੋਂ (*st->e_cpt == ',' || *st->e_cpt == ' ')
	st->e_cpt++;

    ਜੇ (*st->e_cpt == '.' && !curbuf->b_scanned && !skip_ਸਰੋਤ
	    && !compl_ਸਮਾਂ_ਟੁਕੜਾ_expired)
    {
	st->ins_buf = curbuf;
	st->ਪਹਿਲਾ_ਮੇਲ_pos = *ਸ਼ੁਰੂ_ਮੇਲ_pos;
	// Move the cursor back one character so that ^N can match the
	// word immediately after the cursor.
	ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && (!fuzzy_collect && dec(&st->ਪਹਿਲਾ_ਮੇਲ_pos) < 0))
	{
	    // Move the cursor to after the last character in the
	    // buffer, so that word at start of buffer is found
	    // correctly.
	    st->ਪਹਿਲਾ_ਮੇਲ_pos.lnum = st->ins_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    st->ਪਹਿਲਾ_ਮੇਲ_pos.ਖਾਨਾ = ml_get_len(st->ਪਹਿਲਾ_ਮੇਲ_pos.lnum);
	}
	st->ਆਖਰੀ_ਮੇਲ_pos = st->ਪਹਿਲਾ_ਮੇਲ_pos;
	compl_type = 0;

	// Remember the first match so that the loop stops when we
	// wrap and come back there a second time.
	st->ਸੈੱਟ_ਮੇਲ_pos = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!skip_ਸਰੋਤ && !compl_ਸਮਾਂ_ਟੁਕੜਾ_expired
	    && vim_strchr((char_u *)"buwU", *st->e_cpt) != ਨਿਰਾ
	    && (st->ins_buf = ins_compl_ਅਗਲਾ_buf(
					   st->ins_buf, *st->e_cpt)) != curbuf)
    {
	// Scan a buffer, but not the current one.
	ਜੇ (st->ins_buf->b_ml.ml_mfp != ਨਿਰਾ)   // loaded buffer
	{
	    compl_started = TRUE;
	    st->ਪਹਿਲਾ_ਮੇਲ_pos.ਖਾਨਾ = st->ਆਖਰੀ_ਮੇਲ_pos.ਖਾਨਾ = 0;
	    st->ਪਹਿਲਾ_ਮੇਲ_pos.lnum = st->ins_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
	    st->ਆਖਰੀ_ਮੇਲ_pos.lnum = 0;
	    compl_type = 0;
	}
	ਨਹੀਂਤਾਂ	// unloaded buffer, scan like dictionary
	{
	    st->ਮਿਲਿਆ_all = TRUE;
	    ਜੇ (st->ins_buf->b_fname == ਨਿਰਾ)
	    {
		ਸਥਿਤੀ = INS_COMPL_CPT_CONT;
		ਜਾਓ ਪੂਰਾ;
	    }
	    compl_type = CTRL_X_DICTIONARY;
	    st->dict = st->ins_buf->b_fname;
	    st->dict_f = DICT_EXACT;
	}
	ਜੇ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
	{
	    ਸੁਨੇਹਾ_hist_off = TRUE;	// reset in msg_trunc_attr()
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Scanning: %s"),
		    st->ins_buf->b_fname == ਨਿਰਾ
			? buf_spname(st->ins_buf)
			: st->ins_buf->b_sfname == ਨਿਰਾ
			    ? st->ins_buf->b_fname
			    : st->ins_buf->b_sfname);
	    (ਖਾਲੀ)ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, TRUE, HL_ATTR(HLF_R));
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (*st->e_cpt == NUL)
	ਸਥਿਤੀ = INS_COMPL_CPT_ਸਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
	    compl_type = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	ਨਹੀਂਤਾਂ ਜੇ (*st->e_cpt == 'F' || *st->e_cpt == 'o')
	{
	    compl_type = CTRL_X_FUNCTION;
	    st->func_cb = get_callback_if_cpt_func(st->e_cpt, cpt_sources_ਸੂਚਕਾਂਕ);
	    ਜੇ (!st->func_cb)
		compl_type = -1;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (!skip_ਸਰੋਤ)
	{
	    ਜੇ (*st->e_cpt == 'k' || *st->e_cpt == 's')
	    {
		ਜੇ (*st->e_cpt == 'k')
		    compl_type = CTRL_X_DICTIONARY;
		ਨਹੀਂਤਾਂ
		    compl_type = CTRL_X_THESAURUS;
		ਜੇ (*++st->e_cpt != ',' && *st->e_cpt != NUL)
		{
		    st->dict = st->e_cpt;
		    st->dict_f = DICT_ਪਹਿਲਾ;
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	    ਨਹੀਂਤਾਂ ਜੇ (*st->e_cpt == 'i')
		compl_type = CTRL_X_ਰਸਤਾ_PATTERNS;
	    ਨਹੀਂਤਾਂ ਜੇ (*st->e_cpt == 'd')
		compl_type = CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ;
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ ਜੇ (*st->e_cpt == ']' || *st->e_cpt == 't')
	    {
		compl_type = CTRL_X_ਟੈਗ_ਸੂਚੀ;
		ਜੇ (!shortmess(SHM_COMPLETIONSCAN) && !compl_autocomplete)
		{
		    ਸੁਨੇਹਾ_hist_off = TRUE;	// reset in msg_trunc_attr()
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Scanning tags."));
		    (ਖਾਲੀ)ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, TRUE, HL_ATTR(HLF_R));
		}
	    }
	    ਨਹੀਂਤਾਂ
		compl_type = -1;
	}

	// in any case e_cpt is advanced to the next entry
	(ਖਾਲੀ)copy_option_part(&st->e_cpt, IObuff, IOSIZE, ",");
	*advance_cpt_idx = may_advance_cpt_ਸੂਚਕਾਂਕ(st->e_cpt);

	st->ਮਿਲਿਆ_all = TRUE;
	ਜੇ (compl_type == -1)
	    ਸਥਿਤੀ = INS_COMPL_CPT_CONT;
    }

ਪੂਰਾ:
    *compl_type_arg = compl_type;
    ਵਾਪਸ ਸਥਿਤੀ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
/*
 * Get the next set of identifiers or defines matching "compl_pattern" in
 * included files.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਅਗਲਾ_include_file_completion(ਪੂਰਨ compl_type)
{
    ਲੱਭੋ_ਨਮੂਨਾ_ਵਿੱਚ_ਰਸਤਾ(compl_ਨਮੂਨਾ.string, compl_direction,
	    (ਪੂਰਨ)compl_ਨਮੂਨਾ.ਲੰਮਾਈ, FALSE, FALSE,
	    (compl_type == CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ
	     && !(compl_cont_ਸਥਿਤੀ & CONT_SOL))
	    ? ਲੱਭੋ_DEFINE : ਲੱਭੋ_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM, FALSE, compl_autocomplete);
}
#ਜੇ_ਅੰਤ

/*
 * Get the next set of words matching "compl_pattern" in dictionary or
 * thesaurus files.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਅਗਲਾ_dict_tsr_completion(ਪੂਰਨ compl_type, char_u *dict, ਪੂਰਨ dict_f)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    ਜੇ (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_ਨਮੂਨਾ.string, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ins_compl_dictionaries(
		dict != ਨਿਰਾ ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_ਨਮੂਨਾ.string,
		dict != ਨਿਰਾ ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}

/*
 * Get the next set of tag names matching "compl_pattern".
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਅਗਲਾ_ਟੈਗ_completion(ਖਾਲੀ)
{
    ਪੂਰਨ		save_p_ic;
    char_u	**matches;
    ਪੂਰਨ		ਅੰਕ_matches;

    // set p_ic according to p_ic, p_scs and pat for find_tags().
    save_p_ic = p_ic;
    p_ic = ignorecase(compl_ਨਮੂਨਾ.string);

    // Find up to TAG_MANY matches.  Avoids that an enormous number
    // of matches is found when compl_pattern is empty
    g_ਟੈਗ_at_cursor = TRUE;
    ਜੇ (ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(compl_ਨਮੂਨਾ.string, &ਅੰਕ_matches, &matches,
		ਟੈਗ_REGEXP | ਟੈਗ_ਨਾਂ | ਟੈਗ_NOIC | ਟੈਗ_INS_COMP
		| (ctrl_x_ਬਹੁਲਕ_not_default() ? ਟੈਗ_VERBOSE : 0),
		ਟੈਗ_MANY, curbuf->b_ffname) == OK && ਅੰਕ_matches > 0)
	ins_compl_ਜੋੜੋ_matches(ਅੰਕ_matches, matches, p_ic);
    g_ਟੈਗ_at_cursor = FALSE;
    p_ic = save_p_ic;
}

/*
 * Compare function for qsort
 */
    ਸਥਿਰ ਪੂਰਨ
compare_scores(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    ਪੂਰਨ idx_a = *(ਅਚਲ ਪੂਰਨ *)a;
    ਪੂਰਨ idx_b = *(ਅਚਲ ਪੂਰਨ *)b;
    ਪੂਰਨ score_a = compl_fuzzy_scores[idx_a];
    ਪੂਰਨ score_b = compl_fuzzy_scores[idx_b];
    ਵਾਪਸ score_a == score_b ? (idx_a == idx_b ? 0 : (idx_a < idx_b ? -1 : 1))
			: (score_a > score_b ? -1 : 1);
}

/*
 * insert prefix with redraw
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_longest_ਪਾਓ(char_u *ਅਗੇਤਰ)
{
    ins_compl_delete();
    ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(ਅਗੇਤਰ + get_compl_len(), -1);
    compl_longest_ਸਿਰਾ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    ins_redraw(FALSE);
}

/*
 * Calculate the longest common prefix among the best fuzzy matches
 * stored in compl_best_matches, and insert it as the longest.
 */
    ਸਥਿਰ ਖਾਲੀ
fuzzy_longest_ਮੇਲ(ਖਾਲੀ)
{
    char_u	*ਅਗੇਤਰ = ਨਿਰਾ;
    ਪੂਰਨ		ਅਗੇਤਰ_len = 0;
    ਪੂਰਨ		i = 0;
    ਪੂਰਨ		j = 0;
    char_u	*ਮੇਲ_str = ਨਿਰਾ;
    char_u	*ਅਗੇਤਰ_ptr = ਨਿਰਾ;
    char_u	*ਮੇਲ_ptr = ਨਿਰਾ;
    char_u	*leader = ਨਿਰਾ;
    size_t	leader_len = 0;
    compl_T	*compl = ਨਿਰਾ;
    ਪੂਰਨ		ਹੋਰ_candidates = FALSE;
    compl_T	*nn_compl = ਨਿਰਾ;

    ਜੇ (compl_ਅੰਕ_bests == 0)
	ਵਾਪਸ;

    nn_compl = compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ->cp_ਅਗਲਾ;
    ਜੇ (nn_compl && nn_compl != compl_ਪਹਿਲਾ_ਮੇਲ)
	ਹੋਰ_candidates = TRUE;

    compl = ctrl_x_ਬਹੁਲਕ_ਸਾਰਾ_ਸਤਰ() ? compl_ਪਹਿਲਾ_ਮੇਲ
				    : compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ;
    ਜੇ (compl_ਅੰਕ_bests == 1)
    {
	// no more candidates insert the match str
	ਜੇ (!ਹੋਰ_candidates)
	{
	    ins_compl_longest_ਪਾਓ(compl->cp_str.string);
	    compl_ਅੰਕ_bests = 0;
	}
	compl_ਅੰਕ_bests = 0;
	ਵਾਪਸ;
    }

    ਜੇ ((size_t)compl_ਅੰਕ_bests > ਮਾਪ_MAX / ਆਕਾਰ(compl_T *))
	ਵਾਪਸ;
    compl_ਉੱਤਮ_matches = (compl_T **)alloc(compl_ਅੰਕ_bests * ਆਕਾਰ(compl_T *));
    ਜੇ (compl_ਉੱਤਮ_matches == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜਦੋਂ (compl != ਨਿਰਾ && i < compl_ਅੰਕ_bests)
    {
	compl_ਉੱਤਮ_matches[i] = compl;
	compl = compl->cp_ਅਗਲਾ;
	i++;
    }

    ਅਗੇਤਰ = compl_ਉੱਤਮ_matches[0]->cp_str.string;
    ਅਗੇਤਰ_len = (ਪੂਰਨ)compl_ਉੱਤਮ_matches[0]->cp_str.ਲੰਮਾਈ;

    ਲਈ (i = 1; i < compl_ਅੰਕ_bests; i++)
    {
	ਮੇਲ_str = compl_ਉੱਤਮ_matches[i]->cp_str.string;
	ਅਗੇਤਰ_ptr = ਅਗੇਤਰ;
	ਮੇਲ_ptr = ਮੇਲ_str;
	j = 0;

	ਜਦੋਂ (j < ਅਗੇਤਰ_len && *ਮੇਲ_ptr != NUL && *ਅਗੇਤਰ_ptr != NUL)
	{
	    ਜੇ (STRNCMP(ਅਗੇਤਰ_ptr, ਮੇਲ_ptr, mb_ptr2len(ਅਗੇਤਰ_ptr)) != 0)
		ਰੋਕੋ;

	    MB_PTR_ADV(ਅਗੇਤਰ_ptr);
	    MB_PTR_ADV(ਮੇਲ_ptr);
	    j++;
	}

	ਜੇ (j > 0)
	    ਅਗੇਤਰ_len = j;
    }

    leader = ins_compl_leader();
    leader_len = ins_compl_leader_len();

    // skip non-consecutive prefixes
    ਜੇ (leader_len > 0 && STRNCMP(ਅਗੇਤਰ, leader, leader_len) != 0)
	ਜਾਓ ਸਿਰਾ;

    ਅਗੇਤਰ = vim_strnsave(ਅਗੇਤਰ, ਅਗੇਤਰ_len);
    ਜੇ (ਅਗੇਤਰ != ਨਿਰਾ)
    {
	ins_compl_longest_ਪਾਓ(ਅਗੇਤਰ);
	vim_free(ਅਗੇਤਰ);
    }

ਸਿਰਾ:
    vim_free(compl_ਉੱਤਮ_matches);
    compl_ਉੱਤਮ_matches = ਨਿਰਾ;
    compl_ਅੰਕ_bests = 0;
}

/*
 * Get the next set of filename matching "compl_pattern".
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਅਗਲਾ_filename_completion(ਖਾਲੀ)
{
    char_u	**matches;
    ਪੂਰਨ		ਅੰਕ_matches;
    char_u	*ptr;
    garray_T	fuzzy_indices;
    ਪੂਰਨ		i;
    ਪੂਰਨ		score;
    char_u	*leader = ins_compl_leader();
    size_t	leader_len = ins_compl_leader_len();
    ਪੂਰਨ		ਵਿੱਚ_fuzzy_collect = (cot_fuzzy() && leader_len > 0);
    ਪੂਰਨ		*fuzzy_indices_ਅੰਕੜੇ;
    char_u	*ਆਖਰੀ_sep = ਨਿਰਾ;
    ਪੂਰਨ		need_collect_bests = ਵਿੱਚ_fuzzy_collect && compl_get_longest;
    ਪੂਰਨ		max_score = 0;
    ਪੂਰਨ		current_score = 0;
    ਪੂਰਨ		dir = compl_direction;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    ਅੱਖਰ pathsep = (curbuf->b_p_csl[0] == 's') ?
	'/' : (curbuf->b_p_csl[0] == 'b') ? '\\' : PATHSEP;
#ਨਹੀਂਤਾਂ
    ਅੱਖਰ pathsep = PATHSEP;
#ਜੇ_ਅੰਤ

    ਜੇ (ਵਿੱਚ_fuzzy_collect)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	ਜੇ (curbuf->b_p_csl[0] == 's')
	{
	    ਲਈ (i = 0; i < (ਪੂਰਨ)leader_len; i++)
	    {
		ਜੇ (leader[i] == '\\')
		    leader[i] = '/';
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_csl[0] == 'b')
	{
	    ਲਈ (i = 0; i < (ਪੂਰਨ)leader_len; i++)
	    {
		ਜੇ (leader[i] == '/')
		    leader[i] = '\\';
	    }
	}
#ਜੇ_ਅੰਤ
	ਆਖਰੀ_sep = vim_strrchr(leader, pathsep);
	ਜੇ (ਆਖਰੀ_sep == ਨਿਰਾ)
	{
	    // No path separator or separator is the last character,
	    // fuzzy match the whole leader
	    VIM_CLEAR_STRING(compl_ਨਮੂਨਾ);
	    compl_ਨਮੂਨਾ.string = vim_strnsave((char_u *)"*", 1);
	    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
		ਵਾਪਸ;
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*(ਆਖਰੀ_sep + 1) == '\0')
	    ਵਿੱਚ_fuzzy_collect = FALSE;
	ਨਹੀਂਤਾਂ
	{
	    // Split leader into path and file parts
	    ਪੂਰਨ ਰਸਤਾ_len = ਆਖਰੀ_sep - leader + 1;
	    char_u  *ਰਸਤਾ_with_wildcard;

	    ਰਸਤਾ_with_wildcard = alloc(ਰਸਤਾ_len + 2);
	    ਜੇ (ਰਸਤਾ_with_wildcard != ਨਿਰਾ)
	    {
		vim_snprintf((ਅੱਖਰ *)ਰਸਤਾ_with_wildcard, ਰਸਤਾ_len + 2, "%*.*s*", ਰਸਤਾ_len, ਰਸਤਾ_len, leader);
		VIM_CLEAR_STRING(compl_ਨਮੂਨਾ);
		compl_ਨਮੂਨਾ.string = ਰਸਤਾ_with_wildcard;
		compl_ਨਮੂਨਾ.ਲੰਮਾਈ = ਰਸਤਾ_len + 1;

		// Move leader to the file part
		leader = ਆਖਰੀ_sep + 1;
		leader_len -= ਰਸਤਾ_len;
	    }
	}
    }

    ਜੇ (expand_wildcards(1, &compl_ਨਮੂਨਾ.string, &ਅੰਕ_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	ਵਾਪਸ;

    // May change home directory back to "~".
    tilde_replace(compl_ਨਮੂਨਾ.string, ਅੰਕ_matches, matches);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    ਜੇ (curbuf->b_p_csl[0] != NUL)
    {
	ਲਈ (i = 0; i < ਅੰਕ_matches; ++i)
	{
	    ptr = matches[i];
	    ਜਦੋਂ (*ptr != NUL)
	    {
		ਜੇ (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (ਵਿੱਚ_fuzzy_collect)
    {
	ga_init2(&fuzzy_indices, ਆਕਾਰ(ਪੂਰਨ), 10);
	compl_fuzzy_scores = (ਪੂਰਨ *)alloc(ਆਕਾਰ(ਪੂਰਨ) * ਅੰਕ_matches);

	ਲਈ (i = 0; i < ਅੰਕ_matches; i++)
	{
	    ptr = matches[i];
	    score = fuzzy_ਮੇਲ_str(ptr, leader);
	    ਜੇ (score != FUZZY_SCORE_NONE)
	    {
		ਜੇ (ga_grow(&fuzzy_indices, 1) == OK)
		{
		    ((ਪੂਰਨ *)fuzzy_indices.ga_ਅੰਕੜੇ)[fuzzy_indices.ga_len] = i;
		    compl_fuzzy_scores[i] = score;
		    fuzzy_indices.ga_len++;
		}
	    }
	}

	// prevent qsort from deref NULL pointer
	ਜੇ (fuzzy_indices.ga_len > 0)
	{
	    char_u	*ਮੇਲ = ਨਿਰਾ;
	    fuzzy_indices_ਅੰਕੜੇ = (ਪੂਰਨ *)fuzzy_indices.ga_ਅੰਕੜੇ;
	    qsort(fuzzy_indices_ਅੰਕੜੇ, fuzzy_indices.ga_len, ਆਕਾਰ(ਪੂਰਨ), compare_scores);

	    ਲਈ (i = 0; i < fuzzy_indices.ga_len; ++i)
	    {
		ਮੇਲ = matches[fuzzy_indices_ਅੰਕੜੇ[i]];
		current_score = compl_fuzzy_scores[fuzzy_indices_ਅੰਕੜੇ[i]];
		ਜੇ (ins_compl_ਜੋੜੋ(ਮੇਲ, -1, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, dir,
			CP_FAST | ((p_fic || p_wic) ? CP_ICASE : 0),
			FALSE, ਨਿਰਾ, current_score) == OK)
		    dir = FORWARD;

		ਜੇ (need_collect_bests)
		{
		    ਜੇ (i == 0 || current_score == max_score)
		    {
			compl_ਅੰਕ_bests++;
			max_score = current_score;
		    }
		}
	    }

	    FreeWild(ਅੰਕ_matches, matches);
	}
	ਨਹੀਂਤਾਂ ਜੇ (leader_len > 0)
	{
	    FreeWild(ਅੰਕ_matches, matches);
	    ਅੰਕ_matches = 0;
	}

	vim_free(compl_fuzzy_scores);
	ga_clear(&fuzzy_indices);

	ਜੇ (compl_ਅੰਕ_bests > 0 && compl_get_longest)
	    fuzzy_longest_ਮੇਲ();
	ਵਾਪਸ;
    }

    ਜੇ (ਅੰਕ_matches > 0)
	ins_compl_ਜੋੜੋ_matches(ਅੰਕ_matches, matches, p_fic || p_wic);
}

/*
 * Get the next set of command-line completions matching "compl_pattern".
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਅਗਲਾ_cmdline_completion(ਖਾਲੀ)
{
    char_u	**matches;
    ਪੂਰਨ		ਅੰਕ_matches;

    ਜੇ (expand_cmdline(&compl_xp, compl_ਨਮੂਨਾ.string,
		(ਪੂਰਨ)compl_ਨਮੂਨਾ.ਲੰਮਾਈ, &ਅੰਕ_matches, &matches) == EXPAND_OK)
    {
	ਪੂਰਨ		ਜੋੜੋ_r = OK;
	ਪੂਰਨ		dir = compl_direction;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_matches && ਜੋੜੋ_r != FAIL; i++)
	{
	    char_u	*(cptext[CPT_ਗਿਣਤੀ]) = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};

	    ਜੇ (compl_xp.xp_files_abbr != ਨਿਰਾ)
		cptext[CPT_ABBR] = compl_xp.xp_files_abbr[i];
	    ਜੇ (compl_xp.xp_files_ਕਿਸਮ != ਨਿਰਾ)
		cptext[CPT_ਕਿਸਮ] = compl_xp.xp_files_ਕਿਸਮ[i];
	    ਜੇ (compl_xp.xp_files_menu != ਨਿਰਾ)
		cptext[CPT_MENU] = compl_xp.xp_files_menu[i];
	    ਜੇ (compl_xp.xp_files_info != ਨਿਰਾ)
		cptext[CPT_INFO] = compl_xp.xp_files_info[i];

	    ਜੋੜੋ_r = ins_compl_ਜੋੜੋ(matches[i], -1, ਨਿਰਾ, cptext, ਨਿਰਾ, dir,
		    CP_FAST, FALSE, ਨਿਰਾ, FUZZY_SCORE_NONE);
	    ਜੇ (ਜੋੜੋ_r == OK)
		// if dir was BACKWARD then honor it just once
		dir = FORWARD;
	}
	FreeWild(ਅੰਕ_matches, matches);
	free_xp_files_extra(&compl_xp, ਅੰਕ_matches);
    }
}

/*
 * Get the next set of spell suggestions matching "compl_pattern".
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਅਗਲਾ_spell_completion(linenr_T lnum ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    char_u	**matches;
    ਪੂਰਨ		ਅੰਕ_matches;

    ਅੰਕ_matches = expand_spelling(lnum, compl_ਨਮੂਨਾ.string, &matches);
    ਜੇ (ਅੰਕ_matches > 0)
	ins_compl_ਜੋੜੋ_matches(ਅੰਕ_matches, matches, p_ic);
    ਨਹੀਂਤਾਂ
	vim_free(matches);
#ਜੇ_ਅੰਤ
}

/*
 * Return the next word or line from buffer "ins_buf" at position
 * "cur_match_pos" for completion.  The length of the match is set in "len".
 */
    ਸਥਿਰ char_u *
ins_compl_get_ਅਗਲਾ_ਸ਼ਬਦ_or_ਸਤਰ(
	buf_T	*ins_buf,		// buffer being scanned
	pos_T	*cur_ਮੇਲ_pos,		// current match position
	ਪੂਰਨ	*ਮੇਲ_len,
	ਪੂਰਨ	*cont_s_ipos)		// next ^X<> will set initial_pos
{
    char_u	*ptr;
    ਪੂਰਨ		len;

    *ਮੇਲ_len = 0;
    ptr = ml_get_buf(ins_buf, cur_ਮੇਲ_pos->lnum, FALSE) +
	cur_ਮੇਲ_pos->ਖਾਨਾ;
    len = (ਪੂਰਨ)ml_get_buf_len(ins_buf, cur_ਮੇਲ_pos->lnum) - cur_ਮੇਲ_pos->ਖਾਨਾ;
    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
    {
	ਜੇ (compl_ਸਥਿਤੀ_adding())
	{
	    ਜੇ (cur_ਮੇਲ_pos->lnum >= ins_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਵਾਪਸ ਨਿਰਾ;
	    ptr = ml_get_buf(ins_buf, cur_ਮੇਲ_pos->lnum + 1, FALSE);
	    len = ml_get_buf_len(ins_buf, cur_ਮੇਲ_pos->lnum + 1);
	    ਜੇ (!p_paste)
	    {
		char_u	*tmp_ptr = ptr;

		ptr = skipwhite(tmp_ptr);
		len -= (ਪੂਰਨ)(ptr - tmp_ptr);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	*tmp_ptr = ptr;

	ਜੇ (compl_ਸਥਿਤੀ_adding() && compl_ਲੰਮਾਈ <= len)
	{
	    tmp_ptr += compl_ਲੰਮਾਈ;
	    // Skip if already inside a word.
	    ਜੇ (vim_iswordp(tmp_ptr))
		ਵਾਪਸ ਨਿਰਾ;
	    // Find start of next word.
	    tmp_ptr = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(tmp_ptr);
	}
	// Find end of this word.
	tmp_ptr = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(tmp_ptr);
	len = (ਪੂਰਨ)(tmp_ptr - ptr);

	ਜੇ (compl_ਸਥਿਤੀ_adding() && len == compl_ਲੰਮਾਈ)
	{
	    ਜੇ (cur_ਮੇਲ_pos->lnum < ins_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		// Try next line, if any. the new word will be
		// "join" as if the normal command "J" was used.
		// IOSIZE is always greater than
		// compl_length, so the next STRNCPY always
		// works -- Acevedo
		STRNCPY(IObuff, ptr, len);
		ptr = ml_get_buf(ins_buf, cur_ਮੇਲ_pos->lnum + 1, FALSE);
		tmp_ptr = ptr = skipwhite(ptr);
		// Find start of next word.
		tmp_ptr = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(tmp_ptr);
		// Find end of next word.
		tmp_ptr = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(tmp_ptr);
		ਜੇ (tmp_ptr > ptr)
		{
		    ਜੇ (*ptr != ')' && IObuff[len - 1] != TAB)
		    {
			ਜੇ (IObuff[len - 1] != ' ')
			    IObuff[len++] = ' ';
			// IObuf =~ "\k.* ", thus len >= 2
			ਜੇ (p_js
				&& (IObuff[len - 2] == '.'
				    || (vim_strchr(p_cpo, CPO_JOINSP)
					== ਨਿਰਾ
					&& (IObuff[len - 2] == '?'
					    || IObuff[len - 2] == '!'))))
			    IObuff[len++] = ' ';
		    }
		    // copy as much as possible of the new word
		    ਜੇ (tmp_ptr - ptr >= IOSIZE - len)
			tmp_ptr = ptr + IOSIZE - len - 1;
		    STRNCPY(IObuff + len, ptr, tmp_ptr - ptr);
		    len += (ਪੂਰਨ)(tmp_ptr - ptr);
		    *cont_s_ipos = TRUE;
		}
		IObuff[len] = NUL;
		ptr = IObuff;
	    }
	    ਜੇ (len == compl_ਲੰਮਾਈ)
		ਵਾਪਸ ਨਿਰਾ;
	}
    }

    *ਮੇਲ_len = len;
    ਵਾਪਸ ptr;
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
    ਸਥਿਰ ਪੂਰਨ
get_ਅਗਲਾ_default_completion(ins_compl_ਅਗਲਾ_state_T *st, pos_T *ਸ਼ੁਰੂ_pos)
{
    ਪੂਰਨ		ਮਿਲਿਆ_new_ਮੇਲ = FAIL;
    ਪੂਰਨ		save_p_scs;
    ਪੂਰਨ		save_p_ws;
    ਪੂਰਨ		looped_around = FALSE;
    char_u	*ptr = ਨਿਰਾ;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		ਵਿੱਚ_fuzzy_collect = !compl_ਸਥਿਤੀ_adding()
			&& cot_fuzzy() && compl_ਲੰਮਾਈ > 0;
    char_u	*leader = ins_compl_leader();
    ਪੂਰਨ		score = FUZZY_SCORE_NONE;
    ਪੂਰਨ		ਵਿੱਚ_curbuf = st->ins_buf == curbuf;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    ਜੇ (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    ਜੇ (!ਵਿੱਚ_curbuf)
	p_ws = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    ਲਈ (;;)
    {
	ਪੂਰਨ	cont_s_ipos = FALSE;

	++ਸੁਨੇਹਾ_silent;  // Don't want messages for wrapscan.

	ਜੇ (ਵਿੱਚ_fuzzy_collect)
	{
	    ਮਿਲਿਆ_new_ਮੇਲ = ਖੋਜ_for_fuzzy_ਮੇਲ(st->ins_buf,
			    st->cur_ਮੇਲ_pos, leader, compl_direction,
			    ਸ਼ੁਰੂ_pos, &len, &ptr, &score);
	}
	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਰਾ_ਸਤਰ() || ctrl_x_ਬਹੁਲਕ_eval() || (compl_cont_ਸਥਿਤੀ & CONT_SOL))
	    ਮਿਲਿਆ_new_ਮੇਲ = ਖੋਜ_for_exact_ਸਤਰ(st->ins_buf,
			    st->cur_ਮੇਲ_pos, compl_direction, compl_ਨਮੂਨਾ.string);
	ਨਹੀਂਤਾਂ
	    ਮਿਲਿਆ_new_ਮੇਲ = searchit(ਨਿਰਾ, st->ins_buf, st->cur_ਮੇਲ_pos,
				ਨਿਰਾ, compl_direction, compl_ਨਮੂਨਾ.string, (ਪੂਰਨ)compl_ਨਮੂਨਾ.ਲੰਮਾਈ,
				1L, ਖੋਜ_KEEP + ਖੋਜ_NFMSG, RE_ਆਖਰੀ, ਨਿਰਾ);
	--ਸੁਨੇਹਾ_silent;
	ਜੇ (!compl_started || st->ਸੈੱਟ_ਮੇਲ_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->ਪਹਿਲਾ_ਮੇਲ_pos = *st->cur_ਮੇਲ_pos;
	    st->ਆਖਰੀ_ਮੇਲ_pos = *st->cur_ਮੇਲ_pos;
	    st->ਸੈੱਟ_ਮੇਲ_pos = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (st->ਪਹਿਲਾ_ਮੇਲ_pos.lnum == st->ਆਖਰੀ_ਮੇਲ_pos.lnum
		&& st->ਪਹਿਲਾ_ਮੇਲ_pos.ਖਾਨਾ == st->ਆਖਰੀ_ਮੇਲ_pos.ਖਾਨਾ)
	{
	    ਮਿਲਿਆ_new_ਮੇਲ = FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (compl_dir_forward()
		&& (st->prev_ਮੇਲ_pos.lnum > st->cur_ਮੇਲ_pos->lnum
		    || (st->prev_ਮੇਲ_pos.lnum == st->cur_ਮੇਲ_pos->lnum
			&& st->prev_ਮੇਲ_pos.ਖਾਨਾ >= st->cur_ਮੇਲ_pos->ਖਾਨਾ)))
	{
	    ਜੇ (looped_around)
		ਮਿਲਿਆ_new_ਮੇਲ = FAIL;
	    ਨਹੀਂਤਾਂ
		looped_around = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!compl_dir_forward()
		&& (st->prev_ਮੇਲ_pos.lnum < st->cur_ਮੇਲ_pos->lnum
		    || (st->prev_ਮੇਲ_pos.lnum == st->cur_ਮੇਲ_pos->lnum
			&& st->prev_ਮੇਲ_pos.ਖਾਨਾ <= st->cur_ਮੇਲ_pos->ਖਾਨਾ)))
	{
	    ਜੇ (looped_around)
		ਮਿਲਿਆ_new_ਮੇਲ = FAIL;
	    ਨਹੀਂਤਾਂ
		looped_around = TRUE;
	}
	st->prev_ਮੇਲ_pos = *st->cur_ਮੇਲ_pos;
	ਜੇ (ਮਿਲਿਆ_new_ਮੇਲ == FAIL)
	    ਰੋਕੋ;

	// when ADDING, the text before the cursor matches, skip it
	ਜੇ (compl_ਸਥਿਤੀ_adding() && ਵਿੱਚ_curbuf
		&& ਸ਼ੁਰੂ_pos->lnum == st->cur_ਮੇਲ_pos->lnum
		&& ਸ਼ੁਰੂ_pos->ਖਾਨਾ  == st->cur_ਮੇਲ_pos->ਖਾਨਾ)
	    ਜਾਰੀ;

	ਜੇ (!ਵਿੱਚ_fuzzy_collect)
	    ptr = ins_compl_get_ਅਗਲਾ_ਸ਼ਬਦ_or_ਸਤਰ(st->ins_buf,
		    st->cur_ਮੇਲ_pos, &len, &cont_s_ipos);
	ਜੇ (ptr == ਨਿਰਾ || (ins_compl_has_preinsert()
		    && STRCMP(ptr, ins_compl_leader()) == 0))
	    ਜਾਰੀ;

	ਜੇ (is_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਸਰਗਰਮ() && ਵਿੱਚ_curbuf)
	{
	    score = st->cur_ਮੇਲ_pos->lnum - curwin->w_cursor.lnum;
	    ਜੇ (score < 0)
		score = -score;
	}

	ਜੇ (ins_compl_ਜੋੜੋ_infercase(ptr, len, p_ic,
			ਵਿੱਚ_curbuf ? ਨਿਰਾ : st->ins_buf->b_sfname,
			0, cont_s_ipos, score) != NOTDONE)
	{
	    ਜੇ (ਵਿੱਚ_fuzzy_collect && score == compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ->cp_score)
		compl_ਅੰਕ_bests++;
	    ਮਿਲਿਆ_new_ਮੇਲ = OK;
	    ਰੋਕੋ;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    ਵਾਪਸ ਮਿਲਿਆ_new_ਮੇਲ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
/*
 * Return the callback function associated with "p" if it refers to a
 * user-defined function in the 'complete' option.
 * The "idx" parameter is used for indexing callback entries.
 */
    ਸਥਿਰ callback_T *
get_callback_if_cpt_func(char_u *p, ਪੂਰਨ idx)
{
    ਜੇ (*p == 'o')
	ਵਾਪਸ &curbuf->b_ofu_cb;

    ਜੇ (*p == 'F')
    {
	ਜੇ (*++p != ',' && *p != NUL)
	{
	    // 'F{func}' case
	    ਜੇ (curbuf->b_p_cpt_cb == ਨਿਰਾ
		    || idx < 0 || idx >= curbuf->b_p_cpt_ਗਿਣਤੀ)
		ਵਾਪਸ ਨਿਰਾ;
	    ਵਾਪਸ curbuf->b_p_cpt_cb[idx].cb_name != ਨਿਰਾ
		? &curbuf->b_p_cpt_cb[idx] : ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ &curbuf->b_cfu_cb; // 'cfu'
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Get completion matches from register contents.
 * Extracts words from all available registers and adds them to the completion list.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਰਜਿਸਟਰ_completion(ਖਾਲੀ)
{
    ਪੂਰਨ		dir = compl_direction;
    yankreg_T	*reg = ਨਿਰਾ;
    ਖਾਲੀ	*reg_ptr = ਨਿਰਾ;
    ਪੂਰਨ		adding_ਬਹੁਲਕ = compl_ਸਥਿਤੀ_adding();

    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_REGISTERS; i++)
    {
	ਪੂਰਨ regname = 0;
	ਜੇ (i == 0)
	    regname = '"';    // unnamed register
	ਨਹੀਂਤਾਂ ਜੇ (i < 10)
	    regname = '0' + i;
	ਨਹੀਂਤਾਂ ਜੇ (i == DELETION_ਰਜਿਸਟਰ)
	    regname = '-';
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	ਨਹੀਂਤਾਂ ਜੇ (i == ਤਾਰਾ_ਰਜਿਸਟਰ)
	    regname = '*';
	ਨਹੀਂਤਾਂ ਜੇ (i == PLUS_ਰਜਿਸਟਰ)
	    regname = '+';
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    regname = 'a' + i - 10;

	// Skip invalid or black hole register
	ਜੇ (!valid_yank_reg(regname, FALSE) || regname == '_')
	    ਜਾਰੀ;

	reg_ptr = get_ਰਜਿਸਟਰ(regname, FALSE);
	ਜੇ (reg_ptr == ਨਿਰਾ)
	    ਜਾਰੀ;

	reg = (yankreg_T *)reg_ptr;

	ਲਈ (ਪੂਰਨ j = 0; j < reg->y_ਮਾਪ; j++)
	{
	    char_u *str = reg->y_array[j].string;
	    ਜੇ (str == ਨਿਰਾ)
		ਜਾਰੀ;

	    ਜੇ (adding_ਬਹੁਲਕ)
	    {
		ਪੂਰਨ str_len = (ਪੂਰਨ)STRLEN(str);
		ਜੇ (str_len == 0)
		    ਜਾਰੀ;

		ਜੇ (!compl_orig_ਪਾਠ.string
			|| (p_ic ? STRNICMP(str, compl_orig_ਪਾਠ.string,
					    compl_orig_ਪਾਠ.ਲੰਮਾਈ) == 0
				: STRNCMP(str, compl_orig_ਪਾਠ.string,
					    compl_orig_ਪਾਠ.ਲੰਮਾਈ) == 0))
		{
		    ਜੇ (ins_compl_ਜੋੜੋ_infercase(str, str_len, p_ic, ਨਿਰਾ,
				dir, FALSE, FUZZY_SCORE_NONE) == OK)
			dir = FORWARD;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Calculate the safe end of string to avoid null byte issues
		char_u *str_ਸਿਰਾ = str + STRLEN(str);
		char_u *p = str;

		// Safely iterate through the string
		ਜਦੋਂ (p < str_ਸਿਰਾ && *p != NUL)
		{
		    char_u *old_p = p;
		    p = ਲੱਭੋ_ਸ਼ਬਦ_ਸ਼ੁਰੂ(p);
		    ਜੇ (p >= str_ਸਿਰਾ || *p == NUL)
			ਰੋਕੋ;

		    char_u *ਸ਼ਬਦ_ਸਿਰਾ = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(p);

		    ਜੇ (ਸ਼ਬਦ_ਸਿਰਾ <= p)
		    {
			ਜੇ (has_mbyte)
			    ਸ਼ਬਦ_ਸਿਰਾ = p + (*mb_ptr2len)(p);
			ਨਹੀਂਤਾਂ
			    ਸ਼ਬਦ_ਸਿਰਾ = p + 1;
		    }

		    ਜੇ (ਸ਼ਬਦ_ਸਿਰਾ > str_ਸਿਰਾ)
			ਸ਼ਬਦ_ਸਿਰਾ = str_ਸਿਰਾ;

		    ਪੂਰਨ len = (ਪੂਰਨ)(ਸ਼ਬਦ_ਸਿਰਾ - p);
		    ਜੇ (len > 0 && (!compl_orig_ਪਾਠ.string
				|| (p_ic ? STRNICMP(p, compl_orig_ਪਾਠ.string,
						    compl_orig_ਪਾਠ.ਲੰਮਾਈ) == 0
					: STRNCMP(p, compl_orig_ਪਾਠ.string,
						    compl_orig_ਪਾਠ.ਲੰਮਾਈ) == 0)))
		    {
			ਜੇ (ins_compl_ਜੋੜੋ_infercase(p, len, p_ic, ਨਿਰਾ,
				    dir, FALSE, FUZZY_SCORE_NONE) == OK)
			    dir = FORWARD;
		    }

		    p = ਸ਼ਬਦ_ਸਿਰਾ;

		    ਜੇ (p <= old_p)
		    {
			p = old_p + 1;
			ਜੇ (has_mbyte && p < str_ਸਿਰਾ)
			    p = old_p + (*mb_ptr2len)(old_p);
		    }
		}
	    }
	}

	// Free the register copy
	put_ਰਜਿਸਟਰ(regname, reg_ptr);
    }
}

/*
 * get the next set of completion matches for "type".
 * Returns TRUE if a new match is found. Otherwise returns FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
get_ਅਗਲਾ_completion_ਮੇਲ(ਪੂਰਨ type, ins_compl_ਅਗਲਾ_state_T *st, pos_T *ini)
{
    ਪੂਰਨ	ਮਿਲਿਆ_new_ਮੇਲ = FALSE;

    ਚੋਣ (type)
    {
	ਹਾਲਤ -1:
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	ਹਾਲਤ CTRL_X_ਰਸਤਾ_PATTERNS:
	ਹਾਲਤ CTRL_X_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ:
	    get_ਅਗਲਾ_include_file_completion(type);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ CTRL_X_DICTIONARY:
	ਹਾਲਤ CTRL_X_THESAURUS:
	    get_ਅਗਲਾ_dict_tsr_completion(type, st->dict, st->dict_f);
	    st->dict = ਨਿਰਾ;
	    ਰੋਕੋ;

	ਹਾਲਤ CTRL_X_ਟੈਗ_ਸੂਚੀ:
	    get_ਅਗਲਾ_ਟੈਗ_completion();
	    ਰੋਕੋ;

	ਹਾਲਤ CTRL_X_FILES:
	    get_ਅਗਲਾ_filename_completion();
	    ਰੋਕੋ;

	ਹਾਲਤ CTRL_X_CMDLINE:
	ਹਾਲਤ CTRL_X_CMDLINE_CTRL_X:
	    get_ਅਗਲਾ_cmdline_completion();
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	ਹਾਲਤ CTRL_X_FUNCTION:
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ())  // Invoked by a func in 'cpt' option
		get_cpt_func_completion_matches(st->func_cb);
	    ਨਹੀਂਤਾਂ
		expand_by_function(type, compl_ਨਮੂਨਾ.string, ਨਿਰਾ);
	    ਰੋਕੋ;
	ਹਾਲਤ CTRL_X_OMNI:
	    expand_by_function(type, compl_ਨਮੂਨਾ.string, ਨਿਰਾ);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ CTRL_X_SPELL:
	    get_ਅਗਲਾ_spell_completion(st->ਪਹਿਲਾ_ਮੇਲ_pos.lnum);
	    ਰੋਕੋ;

	ਹਾਲਤ CTRL_X_ਰਜਿਸਟਰ:
	    get_ਰਜਿਸਟਰ_completion();
	    ਰੋਕੋ;

	ਮੂਲ:	// normal ^P/^N and ^X^L
	    ਮਿਲਿਆ_new_ਮੇਲ = get_ਅਗਲਾ_default_completion(st, ini);
	    ਜੇ (ਮਿਲਿਆ_new_ਮੇਲ == FAIL && st->ins_buf == curbuf)
		st->ਮਿਲਿਆ_all = TRUE;
    }

    // check if compl_curr_match has changed, (e.g. other type of
    // expansion added something)
    ਜੇ (type != 0 && compl_curr_ਮੇਲ != compl_old_ਮੇਲ)
	ਮਿਲਿਆ_new_ਮੇਲ = OK;

    ਵਾਪਸ ਮਿਲਿਆ_new_ਮੇਲ;
}

/*
 * Strips carets followed by numbers. This suffix typically represents the
 * max_matches setting.
 */
    ਸਥਿਰ ਖਾਲੀ
strip_caret_numbers_ਵਿੱਚ_ਰੱਖੋ(char_u *str)
{
    char_u  *ਪਠ = str, *ਲਿਖ = str, *p;

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜਦੋਂ (*ਪਠ)
    {
	ਜੇ (*ਪਠ == '^')
	{
	    p = ਪਠ + 1;
	    ਜਦੋਂ (vim_isdigit(*p))
		p++;
	    ਜੇ ((*p == ',' || *p == '\0') && p != ਪਠ + 1)
	    {
		ਪਠ = p;
		ਜਾਰੀ;
	    }
	    ਨਹੀਂਤਾਂ
		*ਲਿਖ++ = *ਪਠ++;
	}
	ਨਹੀਂਤਾਂ
	    *ਲਿਖ++ = *ਪਠ++;
    }
    *ਲਿਖ = '\0';
}

/*
 * Call functions specified in the 'cpt' option with findstart=1,
 * and retrieve the startcol.
 */
    ਸਥਿਰ ਪੂਰਨ
prepare_cpt_compl_funcs(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ਨਿਰਾ;
    ਪੂਰਨ		idx = 0;
    ਪੂਰਨ		startcol;

    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    ਜੇ (cpt == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    strip_caret_numbers_ਵਿੱਚ_ਰੱਖੋ(cpt);

    ਲਈ (p = cpt; *p;)
    {
	ਜਦੋਂ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ਜੇ (*p == NUL)
	    ਰੋਕੋ;

	cb = get_callback_if_cpt_func(p, idx);
	ਜੇ (cb)
	{
	    ਜੇ (get_userdefined_compl_info(curwin->w_cursor.ਖਾਨਾ, cb, &startcol)
		    == FAIL)
	    {
		ਜੇ (startcol == -3)
		    cpt_sources_array[idx].cs_refresh_always = FALSE;
		ਨਹੀਂਤਾਂ
		    startcol = -2;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (startcol < 0 || startcol > curwin->w_cursor.ਖਾਨਾ)
		startcol = curwin->w_cursor.ਖਾਨਾ;
	    cpt_sources_array[idx].cs_startcol = startcol;
	}
	ਨਹੀਂਤਾਂ
	    cpt_sources_array[idx].cs_startcol = -3;

	(ਖਾਲੀ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	idx++;
    }

    vim_free(cpt);
    ਵਾਪਸ OK;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FAIL;
}

/*
 * Start the timer for the current completion source.
 */
    ਸਥਿਰ ਖਾਲੀ
compl_ਸਰੋਤ_ਸ਼ੁਰੂ_timer(ਪੂਰਨ ਸਰੋਤ_idx ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    ਜੇ (compl_autocomplete || p_cto > 0)
    {
	ELAPSED_INIT(cpt_sources_array[ਸਰੋਤ_idx].compl_ਸ਼ੁਰੂ_tv);
	compl_ਸਮਾਂ_ਟੁਕੜਾ_expired = FALSE;
    }
#ਜੇ_ਅੰਤ
}

/*
 * Safely advance the cpt_sources_index by one.
 */
    ਸਥਿਰ ਪੂਰਨ
advance_cpt_sources_ਸੂਚਕਾਂਕ_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ)
{
    ਜੇ (cpt_sources_ਸੂਚਕਾਂਕ >= 0 && cpt_sources_ਸੂਚਕਾਂਕ < cpt_sources_ਗਿਣਤੀ - 1)
    {
	cpt_sources_ਸੂਚਕਾਂਕ++;
	ਵਾਪਸ OK;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), cpt_sources_ਸੂਚਕਾਂਕ);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FAIL;
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
    ਸਥਿਰ ਪੂਰਨ
ins_compl_get_exp(pos_T *ini)
{
    ਸਥਿਰ ins_compl_ਅਗਲਾ_state_T   st;
    ਸਥਿਰ ਪੂਰਨ			    st_cleared = FALSE;
    ਪੂਰਨ		ਮੇਲ_ਗਿਣਤੀ;
    ਪੂਰਨ		ਮਿਲਿਆ_new_ਮੇਲ;
    ਪੂਰਨ		type = ctrl_x_ਬਹੁਲਕ;
    ਪੂਰਨ		may_advance_cpt_idx = FALSE;
    pos_T	ਸ਼ੁਰੂ_pos = *ini;
    ਪੂਰਨ		ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict = FALSE;
    ਲੰਮਾ	compl_timeout_save = 0;

    ਜੇ (!compl_started)
    {
	buf_T *buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_scanned = false;
	ਜੇ (!st_cleared)
	{
	    CLEAR_ਖੇਤਰ(st);
	    st_cleared = TRUE;
	}
	st.ਮਿਲਿਆ_all = FALSE;
	st.ins_buf = curbuf;
	vim_free(st.e_cpt_copy);
	// Make a copy of 'complete', in case the buffer is wiped out.
	st.e_cpt_copy = vim_strsave((compl_cont_ਸਥਿਤੀ & CONT_LOCAL)
					    ? (char_u *)"." : curbuf->b_p_cpt);
	strip_caret_numbers_ਵਿੱਚ_ਰੱਖੋ(st.e_cpt_copy);
	st.e_cpt = st.e_cpt_copy == ਨਿਰਾ ? (char_u *)"" : st.e_cpt_copy;

	// In large buffers, timeout may miss nearby matches — search above cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKBACK_ਸਤਰ_ਗਿਣਤੀ	1000
	ਜੇ (compl_autocomplete && is_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਸਰਗਰਮ())
	{
	    ਸ਼ੁਰੂ_pos.lnum = MAX(1, ਸ਼ੁਰੂ_pos.lnum - LOOKBACK_ਸਤਰ_ਗਿਣਤੀ);
	    ਸ਼ੁਰੂ_pos.ਖਾਨਾ = 0;
	}
	st.ਆਖਰੀ_ਮੇਲ_pos = st.ਪਹਿਲਾ_ਮੇਲ_pos = ਸ਼ੁਰੂ_pos;
    }
    ਨਹੀਂਤਾਂ ਜੇ (st.ins_buf != curbuf && !buf_valid(st.ins_buf))
	st.ins_buf = curbuf;  // In case the buffer was wiped out.

    compl_old_ਮੇਲ = compl_curr_ਮੇਲ;	// remember the last current match
    st.cur_ਮੇਲ_pos = (compl_dir_forward())
				    ? &st.ਆਖਰੀ_ਮੇਲ_pos : &st.ਪਹਿਲਾ_ਮੇਲ_pos;

    ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict = ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && !ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval()
	&& !(compl_cont_ਸਥਿਤੀ & CONT_LOCAL) && cpt_sources_array != ਨਿਰਾ;
    ਜੇ (ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict)
    {
	cpt_sources_ਸੂਚਕਾਂਕ = 0;
	ਜੇ (compl_autocomplete || p_cto > 0)
	{
	    compl_ਸਰੋਤ_ਸ਼ੁਰੂ_timer(0);
	    compl_ਸਮਾਂ_ਟੁਕੜਾ_expired = FALSE;
	    compl_timeout_ms = compl_autocomplete
		? MAX(COMPL_INITIAL_TIMEOUT_MS, p_act)
		: p_cto;
	}
    }

    // For ^N/^P loop over all the flags/windows/buffers in 'complete'.
    ਲਈ (;;)
    {
	ਮਿਲਿਆ_new_ਮੇਲ = FAIL;
	st.ਸੈੱਟ_ਮੇਲ_pos = FALSE;

	// For ^N/^P pick a new entry from e_cpt if compl_started is off,
	// or if found_all says this entry is done.  For ^X^L only use the
	// entries from 'complete' that look in loaded buffers.
	ਜੇ ((ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() || ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
					&& (!compl_started || st.ਮਿਲਿਆ_all))
	{
	    ਪੂਰਨ ਸਥਿਤੀ = process_ਅਗਲਾ_cpt_ਮੁੱਲ(&st, &type, &ਸ਼ੁਰੂ_pos,
		    cot_fuzzy(), &may_advance_cpt_idx);

	    ਜੇ (ਸਥਿਤੀ == INS_COMPL_CPT_ਸਿਰਾ)
		ਰੋਕੋ;
	    ਜੇ (ਸਥਿਤੀ == INS_COMPL_CPT_CONT)
	    {
		ਜੇ (may_advance_cpt_idx)
		{
		    ਜੇ (!advance_cpt_sources_ਸੂਚਕਾਂਕ_ਸੁਰੱਖਿਅਤ())
			ਰੋਕੋ;
		    compl_ਸਰੋਤ_ਸ਼ੁਰੂ_timer(cpt_sources_ਸੂਚਕਾਂਕ);
		}
		ਜਾਰੀ;
	    }
	}

	ਜੇ (ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict && type == CTRL_X_FUNCTION
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
	ਮਿਲਿਆ_new_ਮੇਲ = get_ਅਗਲਾ_completion_ਮੇਲ(type, &st, &ਸ਼ੁਰੂ_pos);

	// If complete() was called then compl_pattern has been reset.  The
	// following won't work then, bail out.
	ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	    ਰੋਕੋ;

	ਜੇ (may_advance_cpt_idx)
	{
	    ਜੇ (!advance_cpt_sources_ਸੂਚਕਾਂਕ_ਸੁਰੱਖਿਅਤ())
		ਰੋਕੋ;
	    compl_ਸਰੋਤ_ਸ਼ੁਰੂ_timer(cpt_sources_ਸੂਚਕਾਂਕ);
	}

	// break the loop for specialized modes (use 'complete' just for the
	// generic ctrl_x_mode == CTRL_X_NORMAL) or when we've found a new
	//ਮੇਲ
	ਜੇ ((ctrl_x_ਬਹੁਲਕ_not_default() && !ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
						|| ਮਿਲਿਆ_new_ਮੇਲ != FAIL)
	{
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    // Fill the popup menu as soon as possible.
	    ਜੇ (type != -1)
		ins_compl_check_ਕੁੰਜੀਆਂ(0, FALSE);

	    ਜੇ ((ctrl_x_ਬਹੁਲਕ_not_default()
			&& !ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval()) || compl_interrupted)
		ਰੋਕੋ;
	    compl_started = compl_ਸਮਾਂ_ਟੁਕੜਾ_expired ? FALSE : TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Mark a buffer scanned when it has been scanned completely
	    ਜੇ (buf_valid(st.ins_buf) && (type == 0 || type == CTRL_X_ਰਸਤਾ_PATTERNS))
		st.ins_buf->b_scanned = true;

	    compl_started = FALSE;
	}

	// Restore the timeout after collecting matches from function source
	ਜੇ (ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict && type == CTRL_X_FUNCTION
		&& (compl_autocomplete || p_cto > 0))
	    compl_timeout_ms = compl_timeout_save;

	// For `^P` completion, reset `compl_curr_match` to the head to avoid
	// mixing matches from different sources.
	ਜੇ (!compl_dir_forward())
	    ਜਦੋਂ (compl_curr_ਮੇਲ->cp_prev
		    && !ਮੇਲ_at_original_ਪਾਠ(compl_curr_ਮੇਲ->cp_prev))
		compl_curr_ਮੇਲ = compl_curr_ਮੇਲ->cp_prev;
    }
    cpt_sources_ਸੂਚਕਾਂਕ = -1;
    compl_started = TRUE;

    ਜੇ ((ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() || ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
	    && *st.e_cpt == NUL)		// Got to end of 'complete'
	ਮਿਲਿਆ_new_ਮੇਲ = FAIL;

    ਮੇਲ_ਗਿਣਤੀ = -1;		// total of matches, unknown
    ਜੇ (ਮਿਲਿਆ_new_ਮੇਲ == FAIL || (ctrl_x_ਬਹੁਲਕ_not_default()
		&& !ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval()))
	ਮੇਲ_ਗਿਣਤੀ = ins_compl_make_cyclic();

    ਜੇ (cot_fuzzy() && compl_get_longest && compl_ਅੰਕ_bests > 0)
	fuzzy_longest_ਮੇਲ();

    ਜੇ (compl_old_ਮੇਲ != ਨਿਰਾ)
    {
	// If several matches were added (FORWARD) or the search failed and has
	// just been made cyclic then we have to move compl_curr_match to the
	// next or previous entry (if any) -- Acevedo
	compl_curr_ਮੇਲ = compl_dir_forward() ? compl_old_ਮੇਲ->cp_ਅਗਲਾ
						    : compl_old_ਮੇਲ->cp_prev;
	ਜੇ (compl_curr_ਮੇਲ == ਨਿਰਾ)
	    compl_curr_ਮੇਲ = compl_old_ਮੇਲ;
    }
    may_trigger_modechanged();

    ਜੇ (ਮੇਲ_ਗਿਣਤੀ > 0 && !ctrl_x_ਬਹੁਲਕ_spell())
    {
	ਜੇ (is_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਸਰਗਰਮ() && !ins_compl_has_preinsert())
	    ਛਾਂਟੋ_compl_ਮੇਲ_list(cp_compare_ਸਭ_ਤੋਂ_ਨੇੜੇ);

	ਜੇ (cot_fuzzy() && ins_compl_leader_len() > 0)
	    ins_compl_fuzzy_ਛਾਂਟੋ();
    }

    ਵਾਪਸ ਮੇਲ_ਗਿਣਤੀ;
}

/*
 * Update "compl_shown_match" to the actually shown match, it may differ when
 * "compl_leader" is used to omit some of the matches.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_update_shown_ਮੇਲ(ਖਾਲੀ)
{
    string_T	*leader;

    (ਖਾਲੀ)get_leader_for_startcol(ਨਿਰਾ, TRUE); // Clear the cache
    leader = get_leader_for_startcol(compl_shown_ਮੇਲ, TRUE);

    ਜਦੋਂ (!ins_compl_equal(compl_shown_ਮੇਲ,
		leader->string, (ਪੂਰਨ)leader->ਲੰਮਾਈ)
	    && compl_shown_ਮੇਲ->cp_ਅਗਲਾ != ਨਿਰਾ
	    && !is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ->cp_ਅਗਲਾ))
    {
	compl_shown_ਮੇਲ = compl_shown_ਮੇਲ->cp_ਅਗਲਾ;
	leader = get_leader_for_startcol(compl_shown_ਮੇਲ, TRUE);
    }

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    ਜੇ (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_ਮੇਲ,
		leader->string, (ਪੂਰਨ)leader->ਲੰਮਾਈ)
	    && (compl_shown_ਮੇਲ->cp_ਅਗਲਾ == ਨਿਰਾ
		|| is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ->cp_ਅਗਲਾ)))
    {
	ਜਦੋਂ (!ins_compl_equal(compl_shown_ਮੇਲ,
		    leader->string, (ਪੂਰਨ)leader->ਲੰਮਾਈ)
		&& compl_shown_ਮੇਲ->cp_prev != ਨਿਰਾ
		&& !is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ->cp_prev))
	{
	    compl_shown_ਮੇਲ = compl_shown_ਮੇਲ->cp_prev;
	    leader = get_leader_for_startcol(compl_shown_ਮੇਲ, TRUE);
	}
    }
}

/*
 * Delete the old text being completed.
 */
    ਖਾਲੀ
ins_compl_delete(ਖਾਲੀ)
{
    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    ਪੂਰਨ ਖਾਨਾ = compl_ਖਾਨਾ + (compl_ਸਥਿਤੀ_adding() ? compl_ਲੰਮਾਈ : 0);
    string_T	ਬਚਿਆ = {ਨਿਰਾ, 0};
    ਪੂਰਨ	    orig_ਖਾਨਾ;

    ਜੇ (ins_compl_preinsert_effect())
    {
	ਖਾਨਾ += (ਪੂਰਨ)ins_compl_leader_len();
	curwin->w_cursor.ਖਾਨਾ = compl_ins_ਸਿਰਾ_ਖਾਨਾ;
    }

    ਜੇ (curwin->w_cursor.lnum > compl_lnum)
    {
	ਜੇ (curwin->w_cursor.ਖਾਨਾ < ml_get_curline_len())
	{
	    char_u *ਸਤਰ = ml_get_cursor();
	    ਬਚਿਆ.ਲੰਮਾਈ = ml_get_cursor_len();
	    ਬਚਿਆ.string = vim_strnsave(ਸਤਰ, ਬਚਿਆ.ਲੰਮਾਈ);
	    ਜੇ (ਬਚਿਆ.string == ਨਿਰਾ)
		ਵਾਪਸ;
	}
	ਜਦੋਂ (curwin->w_cursor.lnum > compl_lnum)
	{
	    ਜੇ (ml_delete(curwin->w_cursor.lnum) == FAIL)
	    {
		ਜੇ (ਬਚਿਆ.string)
		    vim_free(ਬਚਿਆ.string);
		ਵਾਪਸ;
	    }
	    deleted_ਸਤਰਾਂ_mark(curwin->w_cursor.lnum, 1L);
	    curwin->w_cursor.lnum--;
	}
	// move cursor to end of line
	curwin->w_cursor.ਖਾਨਾ = ml_get_curline_len();
    }

    ਜੇ ((ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ > ਖਾਨਾ)
    {
	ਜੇ (stop_arrow() == FAIL)
	{
	    ਜੇ (ਬਚਿਆ.string)
		vim_free(ਬਚਿਆ.string);
	    ਵਾਪਸ;
	}
	backspace_until_ਕਾਲਮ(ਖਾਨਾ);
	compl_ins_ਸਿਰਾ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    }

    ਜੇ (ਬਚਿਆ.string != ਨਿਰਾ)
    {
	orig_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
	ins_str(ਬਚਿਆ.string, ਬਚਿਆ.ਲੰਮਾਈ);
	curwin->w_cursor.ਖਾਨਾ = orig_ਖਾਨਾ;
	vim_free(ਬਚਿਆ.string);
    }
    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    ਬਦਲਿਆ_cline_bef_curs();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // clear v:completed_item
    ਸੈੱਟ_vim_var_dict(VV_COMPLETED_ਵਸਤੂ, dict_alloc_ਤਾਲਾ(VAR_FIXED));
#ਜੇ_ਅੰਤ
}

/*
 * Insert a completion string that contains newlines.
 * The string is split and inserted line by line.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_expand_ਗੁਣਜ(char_u *str)
{
    char_u	*ਸ਼ੁਰੂ = str;
    char_u	*curr = str;
    ਪੂਰਨ		ਅਧਾਰ_indent = get_indent();

    ਜਦੋਂ (*curr != NUL)
    {
	ਜੇ (*curr == '\n')
	{
	    // Insert the text chunk before newline
	    ਜੇ (curr > ਸ਼ੁਰੂ)
		ins_char_ਬਾਈਟਾਂ(ਸ਼ੁਰੂ, (ਪੂਰਨ)(curr - ਸ਼ੁਰੂ));

	    // Handle newline
	    open_ਸਤਰ(FORWARD, OPENLINE_KEEPTRAIL | OPENLINE_FORCE_INDENT, ਅਧਾਰ_indent, ਨਿਰਾ);
	    ਸ਼ੁਰੂ = curr + 1;
	}
	curr++;
    }

    // Handle remaining text after last newline (if any)
    ਜੇ (curr > ਸ਼ੁਰੂ)
	ins_char_ਬਾਈਟਾਂ(ਸ਼ੁਰੂ, (ਪੂਰਨ)(curr - ਸ਼ੁਰੂ));

    compl_ins_ਸਿਰਾ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
}

/*
 * Find the longest common prefix among the current completion matches.
 * Returns a pointer to the first match string, with *prefix_len set to
 * the length of the common prefix.
 * If "curbuf_only" is TRUE, restrict matches to the current buffer
 * ('.' source in 'complete').
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_common_ਅਗੇਤਰ(size_t *ਅਗੇਤਰ_len, ਪੂਰਨ curbuf_only)
{
    compl_T     *compl;
    ਪੂਰਨ		*ਮੇਲ_ਗਿਣਤੀ;
    char_u	*ਪਹਿਲਾ = ਨਿਰਾ;
    ਪੂਰਨ		len = -1;
    ਪੂਰਨ		is_cpt_completion = (cpt_sources_array != ਨਿਰਾ);

    ਜੇ (!is_cpt_completion)
	ਵਾਪਸ ਨਿਰਾ;

    ਮੇਲ_ਗਿਣਤੀ = ALLOC_CLEAR_MULT(ਪੂਰਨ, cpt_sources_ਗਿਣਤੀ);
    ਜੇ (ਮੇਲ_ਗਿਣਤੀ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    (ਖਾਲੀ)get_leader_for_startcol(ਨਿਰਾ, TRUE); // Clear the cache

    compl = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਕਰੋ
    {
	string_T *leader = get_leader_for_startcol(compl, TRUE);

	// Apply 'smartcase' behavior during normal mode
	ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && !p_inf && compl_orig_ਪਾਠ.string
		&& !ignorecase(compl_orig_ਪਾਠ.string))
	    compl->cp_flags &= ~CP_ICASE;

	ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(compl)
		&& (leader->string == ਨਿਰਾ
		    || ins_compl_equal_sc(compl, leader->string,
			(ਪੂਰਨ)leader->ਲੰਮਾਈ)))
	{
	    // Limit number of items from each source if max_items is set.
	    ਪੂਰਨ ਮੇਲ_ਹੱਦਬੰਦੀ_exceeded = FALSE;
	    ਪੂਰਨ cur_ਸਰੋਤ = compl->cp_cpt_ਸਰੋਤ_idx;

	    ਜੇ (cur_ਸਰੋਤ != -1)
	    {
		ਮੇਲ_ਗਿਣਤੀ[cur_ਸਰੋਤ]++;
		ਪੂਰਨ max_matches = cpt_sources_array[cur_ਸਰੋਤ].cs_max_matches;
		ਜੇ (max_matches > 0 && ਮੇਲ_ਗਿਣਤੀ[cur_ਸਰੋਤ] > max_matches)
		    ਮੇਲ_ਹੱਦਬੰਦੀ_exceeded = TRUE;
	    }

	    ਜੇ (!ਮੇਲ_ਹੱਦਬੰਦੀ_exceeded && (!curbuf_only
			|| (cur_ਸਰੋਤ != -1
			    && cpt_sources_array[cur_ਸਰੋਤ].cs_flag == '.')))
	    {
		ਜੇ (ਪਹਿਲਾ == ਨਿਰਾ && STRNCMP(ins_compl_leader(),
			    compl->cp_str.string, ins_compl_leader_len()) == 0)
		{
		    ਪਹਿਲਾ = compl->cp_str.string;
		    len = (ਪੂਰਨ)STRLEN(ਪਹਿਲਾ);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ != ਨਿਰਾ)
		{
		    ਪੂਰਨ j = 0;  // count in bytes
		    char_u *s1 = ਪਹਿਲਾ;
		    char_u *s2 = compl->cp_str.string;

		    ਜਦੋਂ (j < len && *s1 != NUL && *s2 != NUL)
		    {
			ਜੇ (MB_BYTE2LEN(*s1) != MB_BYTE2LEN(*s2)
				|| memcmp(s1, s2, MB_BYTE2LEN(*s1)) != 0)
			    ਰੋਕੋ;

			j += MB_BYTE2LEN(*s1);
			MB_PTR_ADV(s1);
			MB_PTR_ADV(s2);
		    }
		    len = j;

		    ਜੇ (len == 0)
			ਰੋਕੋ;
		}
	    }
	}
	compl = compl->cp_ਅਗਲਾ;
    } ਜਦੋਂ (compl != ਨਿਰਾ && !is_ਪਹਿਲਾ_ਮੇਲ(compl));

    vim_free(ਮੇਲ_ਗਿਣਤੀ);

    ਜੇ (len > (ਪੂਰਨ)ins_compl_leader_len())
    {
	// Avoid inserting text that duplicates the text already present
	// after the cursor.
	ਜੇ (len == (ਪੂਰਨ)STRLEN(ਪਹਿਲਾ))
	{
	    char_u *ਸਤਰ = ml_get_curline();
	    char_u *p = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ;
	    ਜੇ (p && !IS_WHITE_OR_NUL(*p))
	    {
		char_u *ਸਿਰਾ = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(p);
		ਪੂਰਨ ਪਾਠ_len = ਸਿਰਾ - p;
		ਜੇ (ਪਾਠ_len > 0
			&& ਪਾਠ_len < (len - (ਪੂਰਨ)ins_compl_leader_len())
			&& STRNCMP(ਪਹਿਲਾ + len - ਪਾਠ_len, p, ਪਾਠ_len) == 0)
		    len -= ਪਾਠ_len;
	    }
	}
	*ਅਗੇਤਰ_len = (size_t)len;
	ਵਾਪਸ ਪਹਿਲਾ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Insert the new text being completed.
 * "move_cursor" is used when 'completeopt' includes "preinsert" and when TRUE
 * cursor needs to move back from the inserted text to the compl_leader.
 * When "insert_prefix" is TRUE the longest common prefix is inserted instead
 * of shown match.
 */
    ਖਾਲੀ
ins_compl_ਪਾਓ(ਪੂਰਨ move_cursor, ਪੂਰਨ ਪਾਓ_ਅਗੇਤਰ)
{
    ਪੂਰਨ		compl_len = get_compl_len();
    ਪੂਰਨ		preinsert = ins_compl_has_preinsert();
    char_u	*cp_str = compl_shown_ਮੇਲ->cp_str.string;
    size_t	cp_str_len = compl_shown_ਮੇਲ->cp_str.ਲੰਮਾਈ;
    size_t	leader_len = ins_compl_leader_len();
    char_u	*has_ਗੁਣਜ = vim_strchr(cp_str, '\n');

    ਜੇ (ਪਾਓ_ਅਗੇਤਰ)
    {
	cp_str = ਲੱਭੋ_common_ਅਗੇਤਰ(&cp_str_len, FALSE);
	ਜੇ (cp_str == ਨਿਰਾ)
	{
	    cp_str = ਲੱਭੋ_common_ਅਗੇਤਰ(&cp_str_len, TRUE);
	    ਜੇ (cp_str == ਨਿਰਾ)
	    {
		cp_str = compl_shown_ਮੇਲ->cp_str.string;
		cp_str_len = compl_shown_ਮੇਲ->cp_str.ਲੰਮਾਈ;
	    }
	}
    }
    // Since completion sources may provide matches with varying start
    // positions, insert only the portion of the match that corresponds to the
    // intended replacement range.
    ਨਹੀਂਤਾਂ ਜੇ (cpt_sources_array != ਨਿਰਾ)
    {
	ਪੂਰਨ	cpt_idx = compl_shown_ਮੇਲ->cp_cpt_ਸਰੋਤ_idx;
	ਜੇ (cpt_idx >= 0 && compl_ਖਾਨਾ >= 0)
	{
	    ਪੂਰਨ	startcol = cpt_sources_array[cpt_idx].cs_startcol;
	    ਜੇ (startcol >= 0 && startcol < (ਪੂਰਨ)compl_ਖਾਨਾ)
	    {
		ਪੂਰਨ skip = (ਪੂਰਨ)compl_ਖਾਨਾ - startcol;
		ਜੇ ((size_t)skip <= cp_str_len)
		{
		    cp_str_len -= skip;
		    cp_str += skip;
		}
	    }
	}
    }

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    ਜੇ (compl_len < (ਪੂਰਨ)cp_str_len)
    {
	ਜੇ (has_ਗੁਣਜ)
	    ins_compl_expand_ਗੁਣਜ(cp_str + compl_len);
	ਨਹੀਂਤਾਂ
	{
	    ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(cp_str + compl_len,
		    ਪਾਓ_ਅਗੇਤਰ ? (ਪੂਰਨ)cp_str_len - compl_len : -1);
	    ਜੇ ((preinsert || ਪਾਓ_ਅਗੇਤਰ) && move_cursor)
		curwin->w_cursor.ਖਾਨਾ -= (colnr_T)(cp_str_len - leader_len);
	}
    }
    ਜੇ (ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ)
	    || (preinsert && !ਪਾਓ_ਅਗੇਤਰ))
	compl_ਵਰਤਿਆ_ਮੇਲ = FALSE;
    ਨਹੀਂਤਾਂ
	compl_ਵਰਤਿਆ_ਮੇਲ = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_ਮੇਲ);

	ਸੈੱਟ_vim_var_dict(VV_COMPLETED_ਵਸਤੂ, dict);
    }
#ਜੇ_ਅੰਤ
    compl_hi_on_autocompl_longest = ਪਾਓ_ਅਗੇਤਰ && move_cursor;
}

/*
 * show the file name for the completion match (if any).  Truncate the file
 * name to avoid a wait for return.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_show_filename(ਖਾਲੀ)
{
    ਅੱਖਰ	*lead = _("match in file");
    ਪੂਰਨ		space = sc_ਖਾਨਾ - vim_strsize((char_u *)lead) - 2;
    char_u	*s;
    char_u	*e;

    ਜੇ (space <= 0)
	ਵਾਪਸ;

    // We need the tail that fits.  With double-byte encoding going
    // back from the end is very slow, thus go from the start and keep
    // the text that fits in "space" between "s" and "e".
    ਲਈ (s = e = compl_shown_ਮੇਲ->cp_fname; *e != NUL; MB_PTR_ADV(e))
    {
	space -= ptr2cells(e);
	ਜਦੋਂ (space < 0)
	{
	    space += ptr2cells(s);
	    MB_PTR_ADV(s);
	}
    }
    ਜੇ (!compl_autocomplete)
    {
	ਸੁਨੇਹਾ_hist_off = TRUE;
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%s %s%s", lead,
		s > compl_shown_ਮੇਲ->cp_fname ? "<" : "", s);
	ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
	ਸੁਨੇਹਾ_hist_off = FALSE;
	redraw_cmdline = FALSE;	    // don't overwrite!
    }
}

/*
 * Find the appropriate completion item when 'complete' ('cpt') includes
 * a 'max_matches' postfix. In this case, we search for a match where
 * 'cp_in_match_array' is set, indicating that the match is also present
 * in 'compl_match_array'.
 */
    ਸਥਿਰ compl_T *
ਲੱਭੋ_ਅਗਲਾ_ਮੇਲ_ਵਿੱਚ_menu(ਖਾਲੀ)
{
    ਪੂਰਨ	    is_forward = compl_shows_dir_forward();
    compl_T *ਮੇਲ = compl_shown_ਮੇਲ;

    ਕਰੋ
	ਮੇਲ = is_forward ? ਮੇਲ->cp_ਅਗਲਾ : ਮੇਲ->cp_prev;
    ਜਦੋਂ (ਮੇਲ->cp_ਅਗਲਾ && !ਮੇਲ->cp_ਵਿੱਚ_ਮੇਲ_array
	    && !ਮੇਲ_at_original_ਪਾਠ(ਮੇਲ));
    ਵਾਪਸ ਮੇਲ;
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
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਅਗਲਾ_completion_ਮੇਲ(
	ਪੂਰਨ	allow_get_expansion,
	ਪੂਰਨ	todo,		// repeat completion this many times
	ਪੂਰਨ	advance,
	ਪੂਰਨ	*ਅੰਕ_matches)
{
    ਪੂਰਨ		ਮਿਲਿਆ_ਸਿਰਾ = FALSE;
    compl_T	*ਮਿਲਿਆ_compl = ਨਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_cot_flags = get_cot_flags();
    ਪੂਰਨ		compl_no_select = (cur_cot_flags & COT_NOSELECT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    string_T	*leader;


    ਜਦੋਂ (--todo >= 0)
    {
	ਜੇ (compl_shows_dir_forward() && compl_shown_ਮੇਲ->cp_ਅਗਲਾ != ਨਿਰਾ)
	{
	    ਜੇ (compl_ਮੇਲ_array != ਨਿਰਾ)
		compl_shown_ਮੇਲ = ਲੱਭੋ_ਅਗਲਾ_ਮੇਲ_ਵਿੱਚ_menu();
	    ਨਹੀਂਤਾਂ
		compl_shown_ਮੇਲ = compl_shown_ਮੇਲ->cp_ਅਗਲਾ;
	    ਮਿਲਿਆ_ਸਿਰਾ = (compl_ਪਹਿਲਾ_ਮੇਲ != ਨਿਰਾ
		    && (is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ->cp_ਅਗਲਾ)
			|| is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ)));
	}
	ਨਹੀਂਤਾਂ ਜੇ (compl_shows_dir_backward()
		&& compl_shown_ਮੇਲ->cp_prev != ਨਿਰਾ)
	{
	    ਮਿਲਿਆ_ਸਿਰਾ = is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ);
	    ਜੇ (compl_ਮੇਲ_array != ਨਿਰਾ)
		compl_shown_ਮੇਲ = ਲੱਭੋ_ਅਗਲਾ_ਮੇਲ_ਵਿੱਚ_menu();
	    ਨਹੀਂਤਾਂ
		compl_shown_ਮੇਲ = compl_shown_ਮੇਲ->cp_prev;
	    ਮਿਲਿਆ_ਸਿਰਾ |= is_ਪਹਿਲਾ_ਮੇਲ(compl_shown_ਮੇਲ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!allow_get_expansion)
	    {
		ਜੇ (advance)
		{
		    ਜੇ (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    ਨਹੀਂਤਾਂ
			compl_pending += todo + 1;
		}
		ਵਾਪਸ -1;
	    }

	    ਜੇ (!compl_no_select && advance)
	    {
		ਜੇ (compl_shows_dir_backward())
		    --compl_pending;
		ਨਹੀਂਤਾਂ
		    ++compl_pending;
	    }

	    // Find matches.
	    *ਅੰਕ_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    ਜਦੋਂ (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		ਜੇ (compl_pending > 0 && compl_shown_ਮੇਲ->cp_ਅਗਲਾ != ਨਿਰਾ)
		{
		    compl_shown_ਮੇਲ = compl_shown_ਮੇਲ->cp_ਅਗਲਾ;
		    --compl_pending;
		}
		ਨਹੀਂਤਾਂ ਜੇ (compl_pending < 0 && compl_shown_ਮੇਲ->cp_prev != ਨਿਰਾ)
		{
		    compl_shown_ਮੇਲ = compl_shown_ਮੇਲ->cp_prev;
		    ++compl_pending;
		}
		ਨਹੀਂਤਾਂ
		    ਰੋਕੋ;
	    }
	    ਮਿਲਿਆ_ਸਿਰਾ = FALSE;
	}

	leader = get_leader_for_startcol(compl_shown_ਮੇਲ, FALSE);

	ਜੇ (!ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ)
		&& leader->string != ਨਿਰਾ
		&& !ins_compl_equal(compl_shown_ਮੇਲ,
		    leader->string, (ਪੂਰਨ)leader->ਲੰਮਾਈ)
		&& !(cot_fuzzy()
		    && compl_shown_ਮੇਲ->cp_score != FUZZY_SCORE_NONE))
	    ++todo;
	ਨਹੀਂਤਾਂ
	    // Remember a matching item.
	    ਮਿਲਿਆ_compl = compl_shown_ਮੇਲ;

	// Stop at the end of the list when we found a usable match.
	ਜੇ (ਮਿਲਿਆ_ਸਿਰਾ)
	{
	    ਜੇ (ਮਿਲਿਆ_compl != ਨਿਰਾ)
	    {
		compl_shown_ਮੇਲ = ਮਿਲਿਆ_compl;
		ਰੋਕੋ;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
ins_compl_ਅਗਲਾ(
    ਪੂਰਨ	    allow_get_expansion,
    ਪੂਰਨ	    ਗਿਣਤੀ_2,		// repeat completion this many times; should
				// be at least 1
    ਪੂਰਨ	    ਪਾਓ_ਮੇਲ)	// Insert the newly selected match
{
    ਪੂਰਨ	    ਅੰਕ_matches = -1;
    ਪੂਰਨ	    todo = ਗਿਣਤੀ_2;
    ਪੂਰਨ	    advance;
    ਪੂਰਨ	    started = compl_started;
    buf_T   *orig_curbuf = curbuf;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_cot_flags = get_cot_flags();
    ਪੂਰਨ	    compl_no_ਪਾਓ = (cur_cot_flags & COT_NOINSERT) != 0
		    || (compl_autocomplete && !ins_compl_has_preinsert());
    ਪੂਰਨ	    compl_preinsert = ins_compl_has_preinsert();

    // When user complete function return -1 for findstart which is next
    // time of 'always', compl_shown_match become NULL.
    ਜੇ (compl_shown_ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (compl_leader.string != ਨਿਰਾ
	    && !ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ)
	    && !cot_fuzzy())
	// Update "compl_shown_match" to the actually shown match
	ins_compl_update_shown_ਮੇਲ();

    ਜੇ (allow_get_expansion && ਪਾਓ_ਮੇਲ
	    && (!compl_get_longest || compl_ਵਰਤਿਆ_ਮੇਲ))
	// Delete old text to be replaced
	ins_compl_delete();

    // When finding the longest common text we stick at the original text,
    // don't let CTRL-N or CTRL-P move to the first match.
    advance = ਗਿਣਤੀ_2 != 1 || !allow_get_expansion || !compl_get_longest;

    // When restarting the search don't insert the first match either.
    ਜੇ (compl_restarting)
    {
	advance = FALSE;
	compl_restarting = FALSE;
    }

    // Repeat this for when <PageUp> or <PageDown> is typed.  But don't wrap
    // around.
    ਜੇ (ਲੱਭੋ_ਅਗਲਾ_completion_ਮੇਲ(allow_get_expansion, todo, advance,
							&ਅੰਕ_matches) == -1)
	ਵਾਪਸ -1;

    ਜੇ (curbuf != orig_curbuf)
    {
	// In case some completion function switched buffer, don't want to
	// insert the completion elsewhere.
	ਵਾਪਸ -1;
    }

    // Insert the text of the new completion, or the compl_leader.
    ਜੇ (!started && ins_compl_preinsert_longest())
	ins_compl_ਪਾਓ(TRUE, TRUE);
    ਨਹੀਂਤਾਂ ਜੇ (compl_no_ਪਾਓ && !started && !compl_preinsert)
    {
	ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(compl_orig_ਪਾਠ.string + get_compl_len(), -1);
	compl_ਵਰਤਿਆ_ਮੇਲ = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਪਾਓ_ਮੇਲ)
    {
	ਜੇ (!compl_get_longest || compl_ਵਰਤਿਆ_ਮੇਲ)
	{
	    ਪੂਰਨ preinsert_longest = ins_compl_preinsert_longest()
		&& ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ); // none selected
	    ins_compl_ਪਾਓ(compl_preinsert || preinsert_longest,
		    preinsert_longest);
	}
	ਨਹੀਂਤਾਂ
	    ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(compl_leader.string + get_compl_len(), -1);
    }
    ਨਹੀਂਤਾਂ
	compl_ਵਰਤਿਆ_ਮੇਲ = FALSE;

    ਜੇ (!allow_get_expansion)
    {
	// may undisplay the popup menu first
	ins_compl_upd_pum();

	ਜੇ (pum_enough_matches())
	    // Will display the popup menu, don't redraw yet to avoid flicker.
	    pum_call_update_screen();
	ਨਹੀਂਤਾਂ
	    // Not showing the popup menu yet, redraw to show the user what was
	    // inserted.
	    update_screen(0);

	// display the updated popup menu
	ins_compl_show_pum();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
	}
#ਜੇ_ਅੰਤ

	// Delete old text to be replaced, since we're still searching and
	// don't want to match ourselves!
	ins_compl_delete();
    }

    // Enter will select a match when the match wasn't inserted and the popup
    // menu is visible.
    ਜੇ (compl_no_ਪਾਓ && !started
				 && !ਮੇਲ_at_original_ਪਾਠ(compl_shown_ਮੇਲ))
	compl_enter_selects = TRUE;
    ਨਹੀਂਤਾਂ
	compl_enter_selects = !ਪਾਓ_ਮੇਲ && compl_ਮੇਲ_array != ਨਿਰਾ;

    // Show the file name for the match (if any)
    ਜੇ (compl_shown_ਮੇਲ->cp_fname != ਨਿਰਾ)
	ins_compl_show_filename();

    ਵਾਪਸ ਅੰਕ_matches;
}

/*
 * Check if the current completion source exceeded its timeout. If so, stop
 * collecting, and halve the timeout.
 */
    ਸਥਿਰ ਖਾਲੀ
check_elapsed_ਸਮਾਂ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T	*ਸ਼ੁਰੂ_tv
	= &cpt_sources_array[cpt_sources_ਸੂਚਕਾਂਕ].compl_ਸ਼ੁਰੂ_tv;
    ਲੰਮਾ	elapsed_ms = ELAPSED_FUNC(*ਸ਼ੁਰੂ_tv);

    ਜੇ (elapsed_ms > compl_timeout_ms)
    {
	compl_ਸਮਾਂ_ਟੁਕੜਾ_expired = TRUE;
	DECAY_COMPL_TIMEOUT();
    }
#ਜੇ_ਅੰਤ
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
    ਖਾਲੀ
ins_compl_check_ਕੁੰਜੀਆਂ(ਪੂਰਨ frequency, ਪੂਰਨ ਵਿੱਚ_compl_func)
{
    ਸਥਿਰ ਪੂਰਨ	ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		c;

    // Don't check when reading keys from a script, :normal or feedkeys().
    // That would break the test scripts.  But do check for keys when called
    // from complete_check().
    ਜੇ (!ਵਿੱਚ_compl_func && (using_script() || ex_ਸਾਧਾਰਨ_busy))
	ਵਾਪਸ;

    // Only do this at regular intervals
    ਜੇ (++ਗਿਣਤੀ_2 < frequency)
	ਵਾਪਸ;
    ਗਿਣਤੀ_2 = 0;

    // Check for a typed key.  Do use mappings, otherwise vim_is_ctrl_x_key()
    // can't do its work correctly.
    c = vpeekc_any();
    ਜੇ (c != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // If test_override("char_avail", 1) was called, ignore characters
	    // waiting in the typeahead buffer.
	    && !disable_char_avail_for_testing
#ਜੇ_ਅੰਤ
       )
    {
	ਜੇ (vim_is_ctrl_x_ਕੁੰਜੀ(c) && c != Ctrl_X && c != Ctrl_R)
	{
	    c = ਸੁਰੱਖਿਅਤ_vgetc();	// Eat the character
	    compl_shows_dir = ins_compl_key2dir(c);
	    (ਖਾਲੀ)ins_compl_ਅਗਲਾ(FALSE, ins_compl_key2count(c),
				      c != K_ਉੱਪਰ && c != K_ਥੱਲੇ);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Need to get the character to have KeyTyped set.  We'll put it
	    // back with vungetc() below.  But skip K_IGNORE.
	    c = ਸੁਰੱਖਿਅਤ_vgetc();
	    ਜੇ (c != K_IGNORE)
	    {
		// Typed keys that get mapped lose KeyTyped. Still let
		// complete_check() interrupt, except during @r replay.
		ਜੇ (c != Ctrl_R && (ਕੁੰਜੀ_Typed
			    || (ਵਿੱਚ_compl_func && reg_executing == 0)))
		    compl_interrupted = TRUE;

		vungetc(c);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict = ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ()
	    && !ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval() && !(compl_cont_ਸਥਿਤੀ & CONT_LOCAL)
	    && cpt_sources_array != ਨਿਰਾ && cpt_sources_ਸੂਚਕਾਂਕ >= 0;
	ਜੇ (ਸਾਧਾਰਨ_ਬਹੁਲਕ_strict && (compl_autocomplete || p_cto > 0))
	    check_elapsed_ਸਮਾਂ();
    }

    ਜੇ (compl_pending
	    && !got_int
	    && !(cot_flags & (COT_NOINSERT | COT_FUZZY))
	    && (!compl_autocomplete || ins_compl_has_preinsert()))
    {
	// Insert the first match immediately and advance compl_shown_match,
	// before finding other matches.
	ਪੂਰਨ todo = compl_pending > 0 ? compl_pending : -compl_pending;

	compl_pending = 0;
	(ਖਾਲੀ)ins_compl_ਅਗਲਾ(FALSE, todo, TRUE);
    }
}

/*
 * Decide the direction of Insert mode complete from the key typed.
 * Returns BACKWARD or FORWARD.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_key2dir(ਪੂਰਨ c)
{
    ਜੇ (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_ਉੱਪਰ || c == K_ਉੱਪਰ)
	ਵਾਪਸ BACKWARD;
    ਵਾਪਸ FORWARD;
}

/*
 * Return TRUE for keys that are used for completion only when the popup menu
 * is visible.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_pum_ਕੁੰਜੀ(ਪੂਰਨ c)
{
    ਵਾਪਸ pum_visible() && (c == K_PAGEUP || c == K_KPAGEUP || c == K_S_ਉੱਪਰ
		     || c == K_PAGEDOWN || c == K_KPAGEDOWN || c == K_S_ਥੱਲੇ
		     || c == K_ਉੱਪਰ || c == K_ਥੱਲੇ);
}

/*
 * Decide the number of completions to move forward.
 * Returns 1 for most keys, height of the popup menu for page-up/down keys.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_key2count(ਪੂਰਨ c)
{
    ਜੇ (ins_compl_pum_ਕੁੰਜੀ(c) && c != K_ਉੱਪਰ && c != K_ਥੱਲੇ)
    {
	ਪੂਰਨ h = pum_get_ਉਚਾਈ();
	ਜੇ (h > 3)
	    h -= 2; // keep some context
	ਵਾਪਸ h;
    }
    ਵਾਪਸ 1;
}

/*
 * Return TRUE if completion with "c" should insert the match, FALSE if only
 * to change the currently selected completion.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_use_ਮੇਲ(ਪੂਰਨ c)
{
    ਚੋਣ (c)
    {
	ਹਾਲਤ K_ਉੱਪਰ:
	ਹਾਲਤ K_ਥੱਲੇ:
	ਹਾਲਤ K_PAGEDOWN:
	ਹਾਲਤ K_KPAGEDOWN:
	ਹਾਲਤ K_S_ਥੱਲੇ:
	ਹਾਲਤ K_PAGEUP:
	ਹਾਲਤ K_KPAGEUP:
	ਹਾਲਤ K_S_ਉੱਪਰ:
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Get the pattern, column and length for normal completion (CTRL-N CTRL-P
 * completion)
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variables: compl_cont_status and ctrl_x_mode
 */
    ਸਥਿਰ ਪੂਰਨ
get_ਸਾਧਾਰਨ_compl_info(char_u *ਸਤਰ, ਪੂਰਨ startcol, colnr_T curs_ਖਾਨਾ)
{
    ਜੇ ((compl_cont_ਸਥਿਤੀ & CONT_SOL) || ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ())
    {
	ਜੇ (!compl_ਸਥਿਤੀ_adding())
	{
	    ਜਦੋਂ (--startcol >= 0 && vim_isIDc(ਸਤਰ[startcol]))
		;
	    compl_ਖਾਨਾ += ++startcol;
	    compl_ਲੰਮਾਈ = curs_ਖਾਨਾ - startcol;
	}
	ਜੇ (p_ic)
	{
	    compl_ਨਮੂਨਾ.string = str_foldcase(ਸਤਰ + compl_ਖਾਨਾ,
		    compl_ਲੰਮਾਈ, ਨਿਰਾ, 0);
	    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	    {
		compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
		ਵਾਪਸ FAIL;
	    }
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = STRLEN(compl_ਨਮੂਨਾ.string);
	}
	ਨਹੀਂਤਾਂ
	{
	    compl_ਨਮੂਨਾ.string = vim_strnsave(ਸਤਰ + compl_ਖਾਨਾ, (size_t)compl_ਲੰਮਾਈ);
	    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	    {
		compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
		ਵਾਪਸ FAIL;
	    }
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = (size_t)compl_ਲੰਮਾਈ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (compl_ਸਥਿਤੀ_adding())
    {
	char_u	    *ਅਗੇਤਰ = (char_u *)"\\<";
	size_t	    prefixlen = STRLEN_LITERAL("\\<");
	size_t	    n;

	ਜੇ (!vim_iswordp(ਸਤਰ + compl_ਖਾਨਾ)
		|| (compl_ਖਾਨਾ > 0
		    && (vim_iswordp(mb_prevptr(ਸਤਰ, ਸਤਰ + compl_ਖਾਨਾ)))))
	{
	    ਅਗੇਤਰ = (char_u *)"";
	    prefixlen = 0;
	}

	// we need up to 2 extra chars for the prefix
	n = quote_meta(ਨਿਰਾ, ਸਤਰ + compl_ਖਾਨਾ, compl_ਲੰਮਾਈ) + prefixlen;
	compl_ਨਮੂਨਾ.string = alloc(n);
	ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	{
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ FAIL;
	}
	STRCPY((ਅੱਖਰ *)compl_ਨਮੂਨਾ.string, ਅਗੇਤਰ);
	(ਖਾਲੀ)quote_meta(compl_ਨਮੂਨਾ.string + prefixlen,
		ਸਤਰ + compl_ਖਾਨਾ, compl_ਲੰਮਾਈ);
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = n - 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (--startcol < 0
	    || !vim_iswordp(mb_prevptr(ਸਤਰ, ਸਤਰ + startcol + 1)))
    {
	size_t	len = STRLEN_LITERAL("\\<\\k\\k");

	// Match any word of at least two chars
	compl_ਨਮੂਨਾ.string = vim_strnsave((char_u *)"\\<\\k\\k", len);
	ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	{
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ FAIL;
	}
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = len;
	compl_ਖਾਨਾ += curs_ਖਾਨਾ;
	compl_ਲੰਮਾਈ = 0;
	compl_from_nonkeyword = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ ਅਧਾਰ_class;
	    ਪੂਰਨ ਸਿਰ_off;

	    startcol -= (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + startcol);
	    ਅਧਾਰ_class = mb_get_class(ਸਤਰ + startcol);
	    ਜਦੋਂ (--startcol >= 0)
	    {
		ਸਿਰ_off = (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + startcol);
		ਜੇ (ਅਧਾਰ_class != mb_get_class(ਸਤਰ + startcol
			    - ਸਿਰ_off))
		    ਰੋਕੋ;
		startcol -= ਸਿਰ_off;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜਦੋਂ (--startcol >= 0 && vim_iswordc(ਸਤਰ[startcol]))
		;
	}

	compl_ਖਾਨਾ += ++startcol;
	compl_ਲੰਮਾਈ = (ਪੂਰਨ)curs_ਖਾਨਾ - startcol;
	ਜੇ (compl_ਲੰਮਾਈ == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_ਨਮੂਨਾ.string = alloc(7);
	    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	    {
		compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
		ਵਾਪਸ FAIL;
	    }
	    STRCPY((ਅੱਖਰ *)compl_ਨਮੂਨਾ.string, "\\<");
	    (ਖਾਲੀ)quote_meta(compl_ਨਮੂਨਾ.string + 2, ਸਤਰ + compl_ਖਾਨਾ, 1);
	    STRCAT((ਅੱਖਰ *)compl_ਨਮੂਨਾ.string, "\\k");
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = STRLEN(compl_ਨਮੂਨਾ.string);
	}
	ਨਹੀਂਤਾਂ
	{
	    size_t  n = quote_meta(ਨਿਰਾ, ਸਤਰ + compl_ਖਾਨਾ, compl_ਲੰਮਾਈ) + 2;

	    compl_ਨਮੂਨਾ.string = alloc(n);
	    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	    {
		compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
		ਵਾਪਸ FAIL;
	    }
	    STRCPY((ਅੱਖਰ *)compl_ਨਮੂਨਾ.string, "\\<");
	    (ਖਾਲੀ)quote_meta(compl_ਨਮੂਨਾ.string + 2, ਸਤਰ + compl_ਖਾਨਾ,
		    compl_ਲੰਮਾਈ);
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = n - 1;
	}
    }

    // Call functions in 'complete' with 'findstart=1'
    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() && !(compl_cont_ਸਥਿਤੀ & CONT_LOCAL))
    {
	// ^N completion, not complete() or ^X^N
	ਜੇ (ਤਿਆਰੀ_cpt_sources() == FAIL || prepare_cpt_compl_funcs() == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Get the pattern, column and length for whole line completion or for the
 * complete() function.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
get_wholeline_compl_info(char_u *ਸਤਰ, colnr_T curs_ਖਾਨਾ)
{
    compl_ਖਾਨਾ = (colnr_T)getwhitecols(ਸਤਰ);
    compl_ਲੰਮਾਈ = (ਪੂਰਨ)curs_ਖਾਨਾ - (ਪੂਰਨ)compl_ਖਾਨਾ;
    ਜੇ (compl_ਲੰਮਾਈ < 0)	// cursor in indent: empty pattern
	compl_ਲੰਮਾਈ = 0;
    ਜੇ (p_ic)
    {
	compl_ਨਮੂਨਾ.string = str_foldcase(ਸਤਰ + compl_ਖਾਨਾ, compl_ਲੰਮਾਈ,
		ਨਿਰਾ, 0);
	ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	{
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ FAIL;
	}
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = STRLEN(compl_ਨਮੂਨਾ.string);
    }
    ਨਹੀਂਤਾਂ
    {
	compl_ਨਮੂਨਾ.string = vim_strnsave(ਸਤਰ + compl_ਖਾਨਾ, (size_t)compl_ਲੰਮਾਈ);
	ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	{
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ FAIL;
	}
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = (size_t)compl_ਲੰਮਾਈ;
    }

    ਵਾਪਸ OK;
}

/*
 * Get the pattern, column and length for filename completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
get_filename_compl_info(char_u *ਸਤਰ, ਪੂਰਨ startcol, colnr_T curs_ਖਾਨਾ)
{
    // Go back to just before the first filename character.
    ਜੇ (startcol > 0)
    {
	char_u	*p = ਸਤਰ + startcol;

	MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
	ਜਦੋਂ (p > ਸਤਰ && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
	ਜੇ (p == ਸਤਰ && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	ਨਹੀਂਤਾਂ
	    startcol = (ਪੂਰਨ)(p - ਸਤਰ) + 1;
    }

    compl_ਖਾਨਾ += startcol;
    compl_ਲੰਮਾਈ = (ਪੂਰਨ)curs_ਖਾਨਾ - startcol;
    compl_ਨਮੂਨਾ.string = addstar(ਸਤਰ + compl_ਖਾਨਾ, compl_ਲੰਮਾਈ, EXPAND_FILES);
    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
    {
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	ਵਾਪਸ FAIL;
    }

    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = STRLEN(compl_ਨਮੂਨਾ.string);

    ਵਾਪਸ OK;
}

/*
 * Get the pattern, column and length for command-line completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
get_cmdline_compl_info(char_u *ਸਤਰ, colnr_T curs_ਖਾਨਾ)
{
    compl_ਨਮੂਨਾ.string = vim_strnsave(ਸਤਰ, curs_ਖਾਨਾ);
    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
    {
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	ਵਾਪਸ FAIL;
    }
    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = curs_ਖਾਨਾ;
    ਸੈੱਟ_cmd_context(&compl_xp, compl_ਨਮੂਨਾ.string,
	    (ਪੂਰਨ)compl_ਨਮੂਨਾ.ਲੰਮਾਈ, curs_ਖਾਨਾ, FALSE);
    ਜੇ (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_ਖਾਨਾ = curs_ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
	compl_ਖਾਨਾ = (ਪੂਰਨ)(compl_xp.xp_ਨਮੂਨਾ - compl_ਨਮੂਨਾ.string);
    compl_ਲੰਮਾਈ = curs_ਖਾਨਾ - compl_ਖਾਨਾ;

    ਵਾਪਸ OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
/*
 * Set global variables related to completion:
 * compl_col, compl_length, compl_pattern, and cpt_compl_pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_compl_globals(
    ਪੂਰਨ		startcol ਅਣਵਰਤਿਆ,
    colnr_T	curs_ਖਾਨਾ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		is_cpt_compl ਅਣਵਰਤਿਆ)
{
    ਜੇ (is_cpt_compl)
    {
	VIM_CLEAR_STRING(cpt_compl_ਨਮੂਨਾ);
	ਜੇ (startcol < compl_ਖਾਨਾ)
	    ਵਾਪਸ prepend_startcol_ਪਾਠ(&cpt_compl_ਨਮੂਨਾ, &compl_orig_ਪਾਠ,
		    startcol);
	ਨਹੀਂਤਾਂ
	{
	    cpt_compl_ਨਮੂਨਾ.string = vim_strnsave(compl_orig_ਪਾਠ.string,
		    compl_orig_ਪਾਠ.ਲੰਮਾਈ);
	    cpt_compl_ਨਮੂਨਾ.ਲੰਮਾਈ = compl_orig_ਪਾਠ.ਲੰਮਾਈ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (startcol < 0 || startcol > curs_ਖਾਨਾ)
	    startcol = curs_ਖਾਨਾ;

	// Re-obtain line in case it has changed
	char_u	*ਸਤਰ = ml_get(curwin->w_cursor.lnum);
	ਪੂਰਨ	len = curs_ਖਾਨਾ - startcol;

	compl_ਨਮੂਨਾ.string = vim_strnsave(ਸਤਰ + startcol, (size_t)len);
	ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
	{
	    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ FAIL;
	}
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = (size_t)len;
	compl_ਖਾਨਾ = startcol;
	compl_ਲੰਮਾਈ = len;
    }

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Get the pattern, column and length for user defined completion ('omnifunc',
 * 'completefunc' and 'thesaurusfunc')
 * Uses the global variable: spell_bad_len
 * Callback function "cb" is set if triggered by a function in the 'cpt'
 * option; otherwise, it is NULL.
 * "startcol", when not NULL, contains the column returned by function.
 */
    ਸਥਿਰ ਪੂਰਨ
get_userdefined_compl_info(
    colnr_T	curs_ਖਾਨਾ ਅਣਵਰਤਿਆ,
    callback_T	*cb ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		*startcol ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ret = FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    // Call user defined function 'completefunc' with "a:findstart"
    // set to 1 to obtain the length of text to use for completion.
    typval_T	ਦਲੀਲਾਂ[3];
    ਪੂਰਨ		ਖਾਨਾ;
    char_u	*funcname = ਨਿਰਾ;
    pos_T	pos;
    ਪੂਰਨ		save_State = State;
    ਪੂਰਨ		is_cpt_function = (cb != ਨਿਰਾ);
    ਪੂਰਨ		use_sandbox = is_cpt_function
			    && was_ਸੈੱਟ_insecurely(curwin,
					(char_u *)"complete", OPT_LOCAL);

    ਜੇ (!is_cpt_function)
    {
	// Call 'completefunc' or 'omnifunc' or 'thesaurusfunc' and get pattern
	// length as a string
	funcname = get_complete_funcname(ctrl_x_ਬਹੁਲਕ);
	ਜੇ (*funcname == NUL)
	{
	    semsg(_(e_option_str_is_not_ਸੈੱਟ), ctrl_x_ਬਹੁਲਕ_function()
		    ? "completefunc" : "omnifunc");
	    ਵਾਪਸ FAIL;
	}
	cb = get_ਪਾਓ_callback(ctrl_x_ਬਹੁਲਕ);
    }

    ਦਲੀਲਾਂ[0].v_type = VAR_ਸੰਖਿਆ;
    ਦਲੀਲਾਂ[0].vval.v_ਸੰਖਿਆ = 1;
    ਦਲੀਲਾਂ[1].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[1].vval.v_string = (char_u *)"";
    ਦਲੀਲਾਂ[2].v_type = VAR_UNKNOWN;
    pos = curwin->w_cursor;
    ++textlock;
    ਜੇ (use_sandbox)
	++sandbox;
    ਖਾਨਾ = call_callback_retnr(cb, 2, ਦਲੀਲਾਂ);
    ਜੇ (use_sandbox)
	--sandbox;
    --textlock;

    State = save_State;
    curwin->w_cursor = pos;	// restore the cursor position
    check_cursor();  // make sure cursor position is valid, just in case
    validate_cursor();
    ਜੇ (!EQUAL_POS(curwin->w_cursor, pos))
    {
	emsg(_(e_complete_function_deleted_ਪਾਠ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (startcol != ਨਿਰਾ)
	*startcol = ਖਾਨਾ;

    // Return value -2 means the user complete function wants to cancel the
    // complete without an error, do the same if the function did not execute
    // successfully.
    ਜੇ (ਖਾਨਾ == -2 || aborting())
	ਵਾਪਸ FAIL;

    // Return value -3 does the same as -2 and leaves CTRL-X mode.
    ਜੇ (ਖਾਨਾ == -3)
    {
	ਜੇ (is_cpt_function)
	    ਵਾਪਸ FAIL;
	ctrl_x_ਬਹੁਲਕ = CTRL_X_ਸਾਧਾਰਨ;
	edit_submode = ਨਿਰਾ;
	ਜੇ (!shortmess(SHM_COMPLETIONMENU))
	    ਸੁਨੇਹਾ_clr_cmdline();
	ਵਾਪਸ FAIL;
    }

    // Reset extended parameters of completion, when starting new
    // completion.
    compl_opt_refresh_always = FALSE;
    compl_opt_suppress_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;

    ret = !is_cpt_function ? ਸੈੱਟ_compl_globals(ਖਾਨਾ, curs_ਖਾਨਾ, FALSE) : OK;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ret;
}

/*
 * Get the pattern, column and length for spell completion.
 * Sets the global variables: compl_col, compl_length and compl_pattern.
 * Uses the global variable: spell_bad_len
 */
    ਸਥਿਰ ਪੂਰਨ
get_spell_compl_info(ਪੂਰਨ startcol ਅਣਵਰਤਿਆ, colnr_T curs_ਖਾਨਾ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ret = FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    char_u	*ਸਤਰ = ਨਿਰਾ;

    ਜੇ (spell_bad_len > 0)
	compl_ਖਾਨਾ = curs_ਖਾਨਾ - spell_bad_len;
    ਨਹੀਂਤਾਂ
	compl_ਖਾਨਾ = spell_ਸ਼ਬਦ_ਸ਼ੁਰੂ(startcol);

    ਜੇ (compl_ਖਾਨਾ >= (colnr_T)startcol)
    {
	compl_ਲੰਮਾਈ = 0;
	compl_ਖਾਨਾ = curs_ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	spell_expand_check_cap(compl_ਖਾਨਾ);
	compl_ਲੰਮਾਈ = (ਪੂਰਨ)curs_ਖਾਨਾ - compl_ਖਾਨਾ;
    }
    // Need to obtain "line" again, it may have become invalid.
    ਸਤਰ = ml_get(curwin->w_cursor.lnum);
    compl_ਨਮੂਨਾ.string = vim_strnsave(ਸਤਰ + compl_ਖਾਨਾ, (size_t)compl_ਲੰਮਾਈ);
    ਜੇ (compl_ਨਮੂਨਾ.string == ਨਿਰਾ)
    {
	compl_ਨਮੂਨਾ.ਲੰਮਾਈ = 0;
	ਵਾਪਸ FAIL;
    }

    compl_ਨਮੂਨਾ.ਲੰਮਾਈ = (size_t)compl_ਲੰਮਾਈ;
    ret = OK;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ret;
}

/*
 * Get the completion pattern, column and length.
 * "startcol" - start column number of the completion pattern/text
 * "cur_col" - current cursor column
 * On return, "line_invalid" is set to TRUE, if the current line may have
 * become invalid and needs to be fetched again.
 * Returns OK on success.
 */
    ਸਥਿਰ ਪੂਰਨ
compl_get_info(char_u *ਸਤਰ, ਪੂਰਨ startcol, colnr_T curs_ਖਾਨਾ, ਪੂਰਨ *ਸਤਰ_invalid)
{
    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() || ctrl_x_ਬਹੁਲਕ_ਰਜਿਸਟਰ()
	    || (ctrl_x_ਬਹੁਲਕ & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_ਬਹੁਲਕ)))
    {
	ਜੇ (get_ਸਾਧਾਰਨ_compl_info(ਸਤਰ, startcol, curs_ਖਾਨਾ) != OK)
	    ਵਾਪਸ FAIL;
	*ਸਤਰ_invalid = TRUE; // 'cpt' func may have invalidated "line"
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
    {
	ਵਾਪਸ get_wholeline_compl_info(ਸਤਰ, curs_ਖਾਨਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_files())
    {
	ਵਾਪਸ get_filename_compl_info(ਸਤਰ, startcol, curs_ਖਾਨਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ == CTRL_X_CMDLINE)
    {
	ਵਾਪਸ get_cmdline_compl_info(ਸਤਰ, curs_ਖਾਨਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_function() || ctrl_x_ਬਹੁਲਕ_omni()
	    || thesaurus_func_complete(ctrl_x_ਬਹੁਲਕ))
    {
	ਜੇ (get_userdefined_compl_info(curs_ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ) != OK)
	    ਵਾਪਸ FAIL;
	*ਸਤਰ_invalid = TRUE;	// "line" may have become invalid
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_spell())
    {
	ਜੇ (get_spell_compl_info(startcol, curs_ਖਾਨਾ) == FAIL)
	    ਵਾਪਸ FAIL;
	*ਸਤਰ_invalid = TRUE;	// "line" may have become invalid
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੰਦਰੂਨੀ_error("ins_complete()");
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਖਾਲੀ
ins_compl_continue_ਖੋਜ(char_u *ਸਤਰ)
{
    // it is a continued search
    compl_cont_ਸਥਿਤੀ &= ~CONT_INTRPT;	// remove INTRPT
    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() || ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_patterns()
						|| ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ())
    {
	ਜੇ (compl_startpos.lnum != curwin->w_cursor.lnum)
	{
	    // line (probably) wrapped, set compl_startpos to the
	    // first non_blank in the line, if it is not a wordchar
	    // include it to get a better pattern, but then we don't
	    // want the "\\<" prefix, check it below
	    compl_ਖਾਨਾ = (colnr_T)getwhitecols(ਸਤਰ);
	    compl_startpos.ਖਾਨਾ = compl_ਖਾਨਾ;
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_cont_ਸਥਿਤੀ &= ~CONT_SOL;   // clear SOL if present
	}
	ਨਹੀਂਤਾਂ
	{
	    // S_IPOS was set when we inserted a word that was at the
	    // beginning of the line, which means that we'll go to SOL
	    // mode but first we need to redefine compl_startpos
	    ਜੇ (compl_cont_ਸਥਿਤੀ & CONT_S_IPOS)
	    {
		compl_cont_ਸਥਿਤੀ |= CONT_SOL;
		compl_startpos.ਖਾਨਾ = (colnr_T)(skipwhite(
			    ਸਤਰ + compl_ਲੰਮਾਈ
			    + compl_startpos.ਖਾਨਾ) - ਸਤਰ);
	    }
	    compl_ਖਾਨਾ = compl_startpos.ਖਾਨਾ;
	}
	compl_ਲੰਮਾਈ = curwin->w_cursor.ਖਾਨਾ - (ਪੂਰਨ)compl_ਖਾਨਾ;
	// IObuff is used to add a "word from the next line" would we
	// have enough space?  just being paranoid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	MIN_SPACE 75
	ਜੇ (compl_ਲੰਮਾਈ > (IOSIZE - MIN_SPACE))
	{
	    compl_cont_ਸਥਿਤੀ &= ~CONT_SOL;
	    compl_ਲੰਮਾਈ = (IOSIZE - MIN_SPACE);
	    compl_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ - compl_ਲੰਮਾਈ;
	}
	compl_cont_ਸਥਿਤੀ |= CONT_ADDING | CONT_N_ADDS;
	ਜੇ (compl_ਲੰਮਾਈ < 1)
	    compl_cont_ਸਥਿਤੀ &= CONT_LOCAL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval() || ctrl_x_ਬਹੁਲਕ_ਰਜਿਸਟਰ())
	compl_cont_ਸਥਿਤੀ = CONT_ADDING | CONT_N_ADDS;
    ਨਹੀਂਤਾਂ
	compl_cont_ਸਥਿਤੀ = 0;
}

/*
 * start insert mode completion
 */
    ਸਥਿਰ ਪੂਰਨ
ins_compl_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    char_u	*ਸਤਰ = ਨਿਰਾ;
    ਪੂਰਨ		startcol = 0;	    // column where searched text starts
    colnr_T	curs_ਖਾਨਾ;	    // cursor column
    ਪੂਰਨ		ਸਤਰ_invalid = FALSE;
    ਪੂਰਨ		save_did_ai = did_ai;
    ਪੂਰਨ		flags = CP_ORIGINAL_ਪਾਠ;

    // First time we hit ^N or ^P (in a row, I mean)
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;
    ਜੇ (stop_arrow() == FAIL)
    {
	did_ai = save_did_ai;
	ਵਾਪਸ FAIL;
    }

    ਸਤਰ = ml_get(curwin->w_cursor.lnum);
    curs_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    compl_pending = 0;
    compl_lnum = curwin->w_cursor.lnum;

    ਜੇ ((compl_cont_ਸਥਿਤੀ & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_ਬਹੁਲਕ == ctrl_x_ਬਹੁਲਕ)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_ਖੋਜ(ਸਤਰ);
    ਨਹੀਂਤਾਂ
	compl_cont_ਸਥਿਤੀ &= CONT_LOCAL;

    ਜੇ (!compl_ਸਥਿਤੀ_adding())	// normal expansion
    {
	compl_cont_ਬਹੁਲਕ = ctrl_x_ਬਹੁਲਕ;
	ਜੇ (ctrl_x_ਬਹੁਲਕ_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_ਸਥਿਤੀ = 0;
	compl_cont_ਸਥਿਤੀ |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (ਪੂਰਨ)curs_ਖਾਨਾ;
	compl_ਖਾਨਾ = 0;
    }

    // Work out completion pattern and original text -- webb
    ਜੇ (compl_get_info(ਸਤਰ, startcol, curs_ਖਾਨਾ, &ਸਤਰ_invalid) == FAIL)
    {
	ਜੇ (ctrl_x_ਬਹੁਲਕ_function() || ctrl_x_ਬਹੁਲਕ_omni()
				|| thesaurus_func_complete(ctrl_x_ਬਹੁਲਕ))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	ਵਾਪਸ FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    ਜੇ (ਸਤਰ_invalid)
	ਸਤਰ = ml_get(curwin->w_cursor.lnum);

    ਜੇ (compl_ਸਥਿਤੀ_adding())
    {
	ਜੇ (!shortmess(SHM_COMPLETIONMENU))
	    edit_submode_pre = (char_u *)_(" Adding");
	ਜੇ (ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.ਖਾਨਾ = compl_ਖਾਨਾ;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_ਲੰਮਾਈ = 0;
	    compl_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
	    compl_lnum = curwin->w_cursor.lnum;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	edit_submode_pre = ਨਿਰਾ;
	compl_startpos.ਖਾਨਾ = compl_ਖਾਨਾ;
    }

    ਜੇ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	ਜੇ (compl_cont_ਸਥਿਤੀ & CONT_LOCAL)
	    edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_ਸੁਨੇਹਾ]);
	ਨਹੀਂਤਾਂ
	    edit_submode = (char_u *)_(CTRL_X_ਸੁਨੇਹਾ(ctrl_x_ਬਹੁਲਕ));
    }

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(ਨਿਰਾ);

    // Always add completion for the original text.
    VIM_CLEAR_STRING(compl_orig_ਪਾਠ);
    compl_orig_ਪਾਠ.ਲੰਮਾਈ = (size_t)compl_ਲੰਮਾਈ;
    compl_orig_ਪਾਠ.string = vim_strnsave(ਸਤਰ + compl_ਖਾਨਾ, (size_t)compl_ਲੰਮਾਈ);
    ਜੇ (p_ic)
	flags |= CP_ICASE;
    ਜੇ (compl_orig_ਪਾਠ.string == ਨਿਰਾ
	    || ins_compl_ਜੋੜੋ(compl_orig_ਪਾਠ.string,
		(ਪੂਰਨ)compl_orig_ਪਾਠ.ਲੰਮਾਈ,
		ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, 0, flags, FALSE, ਨਿਰਾ, FUZZY_SCORE_NONE) != OK)
    {
	VIM_CLEAR_STRING(compl_ਨਮੂਨਾ);
	VIM_CLEAR_STRING(compl_orig_ਪਾਠ);
	did_ai = save_did_ai;
	ਵਾਪਸ FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    ਜੇ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
    {
	edit_submode_extra = (char_u *)_("-- Searching...");
	edit_submode_highl = HLF_ਗਿਣਤੀ;
	showmode();
	edit_submode_extra = ਨਿਰਾ;
	ਬਾਹਰਲਾ_flush();
    }

    did_ai = save_did_ai;
    ਵਾਪਸ OK;
}

/*
 * display the completion status message
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_show_statusmsg(ਖਾਲੀ)
{
    // we found no match if the list has only the "compl_orig_text"-entry
    ਜੇ (is_ਪਹਿਲਾ_ਮੇਲ(compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ))
    {
	edit_submode_extra = compl_ਸਥਿਤੀ_adding() && compl_ਲੰਮਾਈ > 1
				? (char_u *)_("Hit end of paragraph")
				: (char_u *)_("Pattern not found");
	edit_submode_highl = HLF_E;
    }

    ਜੇ (edit_submode_extra == ਨਿਰਾ)
    {
	ਜੇ (ਮੇਲ_at_original_ਪਾਠ(compl_curr_ਮੇਲ))
	{
	    edit_submode_extra = (char_u *)_("Back at original");
	    edit_submode_highl = HLF_W;
	}
	ਨਹੀਂਤਾਂ ਜੇ (compl_cont_ਸਥਿਤੀ & CONT_S_IPOS)
	{
	    edit_submode_extra = (char_u *)_("Word from other line");
	    edit_submode_highl = HLF_ਗਿਣਤੀ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (compl_curr_ਮੇਲ->cp_ਅਗਲਾ == compl_curr_ਮੇਲ->cp_prev)
	{
	    edit_submode_extra = (char_u *)_("The only match");
	    edit_submode_highl = HLF_ਗਿਣਤੀ;
	    compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ = 1;
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ defined(FEAT_COMPL_FUNC) || defined(FEAT_EVAL)
	    // Update completion sequence number when needed.
	    ਜੇ (compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ == -1)
		ins_compl_update_sequence_numbers();
#ਜੇ_ਅੰਤ
	    // The match should always have a sequence number now, this is
	    // just a safety check.
	    ਜੇ (compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ != -1)
	    {
		// Space for 10 text chars. + 2x10-digit no.s = 31.
		// Translations may need more than twice that.
		ਸਥਿਰ char_u ਮੇਲ_ref[81];

		ਜੇ (compl_matches > 0)
		    vim_snprintf((ਅੱਖਰ *)ਮੇਲ_ref, ਆਕਾਰ(ਮੇਲ_ref),
			    _("match %d of %d"),
			    compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ, compl_matches);
		ਨਹੀਂਤਾਂ
		    vim_snprintf((ਅੱਖਰ *)ਮੇਲ_ref, ਆਕਾਰ(ਮੇਲ_ref),
			    _("match %d"),
			    compl_curr_ਮੇਲ->cp_ਸੰਖਿਆ);
		edit_submode_extra = ਮੇਲ_ref;
		edit_submode_highl = HLF_R;
		ਜੇ (dollar_vcol >= 0)
		    curs_columns(FALSE);
	    }
	}
    }

    // Show a message about what (completion) mode we're in.
    ਜੇ (!compl_opt_suppress_ਖਾਲੀ_ਥਾਵਾਂ)
    {
	showmode();
	ਜੇ (!shortmess(SHM_COMPLETIONMENU))
	{
	    ਜੇ (edit_submode_extra != ਨਿਰਾ)
	    {
		ਜੇ (!p_smd)
		{
		    ਸੁਨੇਹਾ_hist_off = TRUE;
		    ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)edit_submode_extra,
			    edit_submode_highl < HLF_ਗਿਣਤੀ
			    ? HL_ATTR(edit_submode_highl) : 0);
		    ਸੁਨੇਹਾ_hist_off = FALSE;
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_clr_cmdline();	// necessary for "noshowmode"
	}
    }
}

/*
 * Do Insert mode completion.
 * Called when character "c" was typed, which has a meaning for completion.
 * Returns OK if completion was done, FAIL if something failed (out of mem).
 */
    ਪੂਰਨ
ins_complete(ਪੂਰਨ c, ਪੂਰਨ enable_pum)
{
    ਪੂਰਨ		n;
    ਪੂਰਨ		save_w_wrow;
    ਪੂਰਨ		save_w_leftcol;
    ਪੂਰਨ		ਪਾਓ_ਮੇਲ;
    ਪੂਰਨ		no_matches_ਮਿਲਿਆ;

    compl_direction = ins_compl_key2dir(c);
    ਪਾਓ_ਮੇਲ = ins_compl_use_ਮੇਲ(c);

    ਜੇ (!compl_started)
    {
	// Only what the automatic trigger armed counts as started by Vim.
	compl_autostarted = compl_autostart_pending;
	compl_autostart_pending = false;
	ਜੇ (ins_compl_ਸ਼ੁਰੂ() == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਪਾਓ_ਮੇਲ && stop_arrow() == FAIL)
	ਵਾਪਸ FAIL;

    compl_curr_win = curwin;
    compl_curr_buf = curwin->w_ਬਫਰ;
    compl_shown_ਮੇਲ = compl_curr_ਮੇਲ;
    compl_shows_dir = compl_direction;
    compl_ਅੰਕ_bests = 0;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_ਅਗਲਾ(TRUE, ins_compl_key2count(c), ਪਾਓ_ਮੇਲ);

    // Reset autocompletion timer expiry flag
    ਜੇ (compl_autocomplete)
	compl_ਸਮਾਂ_ਟੁਕੜਾ_expired = FALSE;

    // may undisplay the popup menu
    ins_compl_upd_pum();

    ਜੇ (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_ਮੇਲ = compl_shown_ਮੇਲ;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    ਜੇ (got_int && !global_busy)
    {
	(ਖਾਲੀ)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    no_matches_ਮਿਲਿਆ = is_ਪਹਿਲਾ_ਮੇਲ(compl_ਪਹਿਲਾ_ਮੇਲ->cp_ਅਗਲਾ);
    ਜੇ (no_matches_ਮਿਲਿਆ)
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	ਜੇ (compl_ਲੰਮਾਈ > 1
		|| compl_ਸਥਿਤੀ_adding()
		|| (ctrl_x_ਬਹੁਲਕ_not_default()
		    && !ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_patterns()
		    && !ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ()))
	    compl_cont_ਸਥਿਤੀ &= ~CONT_N_ADDS;
    }

    ਜੇ (compl_curr_ਮੇਲ->cp_flags & CP_CONT_S_IPOS)
	compl_cont_ਸਥਿਤੀ |= CONT_S_IPOS;
    ਨਹੀਂਤਾਂ
	compl_cont_ਸਥਿਤੀ &= ~CONT_S_IPOS;

    ਜੇ (!shortmess(SHM_COMPLETIONMENU) && !compl_autocomplete)
	ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    ਜੇ (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    ਵਾਪਸ OK;
}

/*
 * Enable autocompletion
 */
    ਖਾਲੀ
ins_compl_enable_autocomplete(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    compl_autocomplete = TRUE;
    compl_get_longest = FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Disable autocompletion
 */
    ਖਾਲੀ
ins_compl_disable_autocomplete(ਖਾਲੀ)
{
    compl_autocomplete = FALSE;
}

/*
 * Remember that Vim is about to start a completion by itself, rather than
 * because a key was typed to ask for one.
 */
    ਖਾਲੀ
ins_compl_arm_autostart(ਖਾਲੀ)
{
    compl_autostart_pending = true;
}

/*
 * Forget what the automatic trigger armed, another key was typed since.
 */
    ਖਾਲੀ
ins_compl_disarm_autostart(ਖਾਲੀ)
{
    compl_autostart_pending = false;
}

/*
 * Arm the 'autocompletedelay' timer when the delay is in effect.
 * Return true when the popup should be deferred, false to trigger it now.
 */
    bool
ins_compl_arm_autocomplete_delay(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    ਜੇ (p_acl > 0)
    {
	ELAPSED_INIT(compl_autocomplete_ਸ਼ੁਰੂ_tv);
	compl_autocomplete_pending = true;
	ਵਾਪਸ true;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ false;
}

/*
 * Clear the pending 'autocompletedelay' state.
 */
    ਖਾਲੀ
ins_compl_clear_autocomplete_delay(ਖਾਲੀ)
{
    compl_autocomplete_pending = false;
}

/*
 * Return true while waiting for 'autocompletedelay' to expire.
 */
    bool
ins_compl_autocomplete_pending(ਖਾਲੀ)
{
    ਵਾਪਸ compl_autocomplete_pending;
}

/*
 * Return the time in msec since the 'autocompletedelay' was armed.
 */
    ਲੰਮਾ
ins_compl_autocomplete_elapsed(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    ਵਾਪਸ ELAPSED_FUNC(compl_autocomplete_ਸ਼ੁਰੂ_tv);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

/*
 * Remove (if needed) and show the popup menu
 */
    ਸਥਿਰ ਖਾਲੀ
show_pum(ਪੂਰਨ prev_w_wrow, ਪੂਰਨ prev_w_leftcol)
{
    // RedrawingDisabled may be set when invoked through complete().
    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    // If the cursor moved or the display scrolled we need to remove the pum
    // first.
    setcursor();
    ਜੇ (prev_w_wrow != curwin->w_wrow || prev_w_leftcol != curwin->w_leftcol)
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
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ
quote_meta(char_u *dest, char_u *src, ਪੂਰਨ len)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ	m = (ਬਿਨਾਨਿਸ਼ਾਨੀ)len + 1;  // one extra for the NUL

    ਲਈ ( ; --len >= 0; src++)
    {
	ਚੋਣ (*src)
	{
	    ਹਾਲਤ '.':
	    ਹਾਲਤ '*':
	    ਹਾਲਤ '[':
		ਜੇ (ctrl_x_ਬਹੁਲਕ_dictionary() || ctrl_x_ਬਹੁਲਕ_thesaurus())
		    ਰੋਕੋ;
		// FALLTHROUGH
	    ਹਾਲਤ '~':
		ਜੇ (!magic_isset())	// quote these only if magic is set
		    ਰੋਕੋ;
		// FALLTHROUGH
	    ਹਾਲਤ '\\':
		ਜੇ (ctrl_x_ਬਹੁਲਕ_dictionary() || ctrl_x_ਬਹੁਲਕ_thesaurus())
		    ਰੋਕੋ;
		// FALLTHROUGH
	    ਹਾਲਤ '^':		// currently it's not needed.
	    ਹਾਲਤ '$':
		m++;
		ਜੇ (dest != ਨਿਰਾ)
		    *dest++ = '\\';
		ਰੋਕੋ;
	}
	ਜੇ (dest != ਨਿਰਾ)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ mb_len = (*mb_ptr2len)(src) - 1;
	    ਜੇ (mb_len > 0 && len >= mb_len)
		ਲਈ (ਪੂਰਨ i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    ਜੇ (dest != ਨਿਰਾ)
			*dest++ = *src;
		}
	}
    }
    ਜੇ (dest != ਨਿਰਾ)
	*dest = NUL;

    ਵਾਪਸ m;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_insexpand_stuff(ਖਾਲੀ)
{
    VIM_CLEAR_STRING(compl_orig_ਪਾਠ);
# ifdef FEAT_EVAL
    free_callback(&cfu_cb);
    free_callback(&ofu_cb);
    free_callback(&tsrfu_cb);
    clear_cpt_callbacks(&cpt_cb, cpt_cb_ਗਿਣਤੀ);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
/*
 * Called when starting CTRL_X_SPELL mode: Move backwards to a previous badly
 * spelled word, if there is one.
 */
    ਸਥਿਰ ਖਾਲੀ
spell_ਪਿੱਠ_ਮੰਜ਼ਿਲ_badword(ਖਾਲੀ)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_ਮੰਜ਼ਿਲ(curwin, BACKWARD, SMT_ALL, TRUE, ਨਿਰਾ);
    ਜੇ (curwin->w_cursor.ਖਾਨਾ != tpos.ਖਾਨਾ)
	ਸ਼ੁਰੂ_arrow(&tpos);
}
#ਜੇ_ਅੰਤ

/*
 * Reset the info associated with completion sources.
 */
    ਸਥਿਰ ਖਾਲੀ
cpt_sources_clear(ਖਾਲੀ)
{
    VIM_CLEAR(cpt_sources_array);
    cpt_sources_ਸੂਚਕਾਂਕ = -1;
    cpt_sources_ਗਿਣਤੀ = 0;
}

/*
 * Setup completion sources.
 */
    ਸਥਿਰ ਪੂਰਨ
ਤਿਆਰੀ_cpt_sources(ਖਾਲੀ)
{
    char_u  buf[LSIZE];
    ਪੂਰਨ	    slen;
    ਪੂਰਨ	    idx = 0;
    ਪੂਰਨ	    ਗਿਣਤੀ_2;
    char_u  *p;

    cpt_sources_clear();

    ਗਿਣਤੀ_2 = get_cpt_sources_ਗਿਣਤੀ(curbuf->b_p_cpt);
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ OK;

    cpt_sources_array = ALLOC_CLEAR_MULT(cpt_ਸਰੋਤ_T, ਗਿਣਤੀ_2);
    ਜੇ (cpt_sources_array == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਲਈ (p = curbuf->b_p_cpt; *p;)
    {
	ਜਦੋਂ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ਜੇ (*p) // If not end of string, count this segment
	{
	    cpt_sources_array[idx].cs_flag = *p;
	    slen = copy_option_part(&p, buf, LSIZE, ","); // Advance p
	    ਜੇ (slen > 0)
	    {
		char_u	*caret = vim_strchr(buf, '^');
		ਜੇ (caret != ਨਿਰਾ)
		    cpt_sources_array[idx].cs_max_matches
			= atoi((ਅੱਖਰ *)caret + 1);
	    }
	    idx++;
	}
    }

    cpt_sources_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if any of the completion sources have 'refresh' set to 'always'.
 */
    ਸਥਿਰ ਪੂਰਨ
is_cpt_func_refresh_always(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    ਲਈ (ਪੂਰਨ i = 0; i < cpt_sources_ਗਿਣਤੀ; i++)
	ਜੇ (cpt_sources_array[i].cs_refresh_always)
	    ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FALSE;
}

/*
 * Make the completion list non-cyclic.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_compl_make_ਰੇਖਿਕ(ਖਾਲੀ)
{
    compl_T *m;

    ਜੇ (compl_ਪਹਿਲਾ_ਮੇਲ == ਨਿਰਾ || compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev == ਨਿਰਾ)
	ਵਾਪਸ;
    m = compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev;
    m->cp_ਅਗਲਾ = ਨਿਰਾ;
    compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev = ਨਿਰਾ;
}

/*
 * Remove the matches linked to the current completion source (as indicated by
 * cpt_sources_index) from the completion list.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    ਸਥਿਰ ਖਾਲੀ
remove_old_matches(ਖਾਲੀ)
{
    compl_T *current;
    ਪੂਰਨ	    shown_ਮੇਲ_removed = FALSE;
    ਪੂਰਨ	    forward = (compl_ਪਹਿਲਾ_ਮੇਲ->cp_cpt_ਸਰੋਤ_idx < 0);

    ਜੇ (cpt_sources_ਸੂਚਕਾਂਕ < 0)
	ਵਾਪਸ;

    compl_direction = forward ? FORWARD : BACKWARD;
    compl_shows_dir = compl_direction;

    // When 'fuzzy' is enabled, items are not ordered by their original source
    // order (cpt_sources_index). So, remove items one by one.
    ਲਈ (current = compl_ਪਹਿਲਾ_ਮੇਲ; current != ਨਿਰਾ; )
    {
	ਜੇ (current->cp_cpt_ਸਰੋਤ_idx == cpt_sources_ਸੂਚਕਾਂਕ)
	{
	    compl_T *ਮੰਜ਼ਿਲ_delete = current;

	    ਜੇ (!shown_ਮੇਲ_removed && compl_shown_ਮੇਲ == current)
		shown_ਮੇਲ_removed = TRUE;

	    current = current->cp_ਅਗਲਾ;

	    ਜੇ (ਮੰਜ਼ਿਲ_delete == compl_ਪਹਿਲਾ_ਮੇਲ)  // node to remove is at head
	    {
		compl_ਪਹਿਲਾ_ਮੇਲ = ਮੰਜ਼ਿਲ_delete->cp_ਅਗਲਾ;
		compl_ਪਹਿਲਾ_ਮੇਲ->cp_prev = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਮੰਜ਼ਿਲ_delete->cp_ਅਗਲਾ == ਨਿਰਾ) // node to remove is at tail
		ਮੰਜ਼ਿਲ_delete->cp_prev->cp_ਅਗਲਾ = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ // node is in the moddle
	    {
		ਮੰਜ਼ਿਲ_delete->cp_prev->cp_ਅਗਲਾ = ਮੰਜ਼ਿਲ_delete->cp_ਅਗਲਾ;
		ਮੰਜ਼ਿਲ_delete->cp_ਅਗਲਾ->cp_prev = ਮੰਜ਼ਿਲ_delete->cp_prev;
	    }
	    ins_compl_ਵਸਤੂ_free(ਮੰਜ਼ਿਲ_delete);
	}
	ਨਹੀਂਤਾਂ
	    current = current->cp_ਅਗਲਾ;
    }

    // Re-assign compl_shown_match if necessary
    ਜੇ (shown_ਮੇਲ_removed)
    {
	ਜੇ (forward)
	    compl_shown_ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
	ਨਹੀਂਤਾਂ
	{  // Last node will have the prefix that is being completed
	    ਲਈ (current = compl_ਪਹਿਲਾ_ਮੇਲ; current->cp_ਅਗਲਾ != ਨਿਰਾ;
		    current = current->cp_ਅਗਲਾ)
		;
	    compl_shown_ਮੇਲ = current;
	}
    }

    // Re-assign compl_curr_match
    compl_curr_ਮੇਲ = compl_ਪਹਿਲਾ_ਮੇਲ;
    ਲਈ (current = compl_ਪਹਿਲਾ_ਮੇਲ; current != ਨਿਰਾ; )
    {
	ਜੇ ((forward ? current->cp_cpt_ਸਰੋਤ_idx < cpt_sources_ਸੂਚਕਾਂਕ
		    : current->cp_cpt_ਸਰੋਤ_idx > cpt_sources_ਸੂਚਕਾਂਕ))
	{
	    compl_curr_ਮੇਲ = forward ? current : current->cp_ਅਗਲਾ;
	    current = current->cp_ਅਗਲਾ;
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }
}
#ਜੇ_ਅੰਤ

/*
 * Retrieve completion matches using the callback function "cb" and store the
 * 'refresh:always' flag.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    ਸਥਿਰ ਖਾਲੀ
get_cpt_func_completion_matches(callback_T *cb ਅਣਵਰਤਿਆ)
{
    cpt_ਸਰੋਤ_T    *cpt_src = &cpt_sources_array[cpt_sources_ਸੂਚਕਾਂਕ];
    ਪੂਰਨ		    startcol = cpt_src->cs_startcol;

    ਜੇ (startcol == -2 || startcol == -3)
	ਵਾਪਸ;

    ਜੇ (ਸੈੱਟ_compl_globals(startcol, curwin->w_cursor.ਖਾਨਾ, TRUE) == OK)
    {
	// Insert the leader string (previously removed) before expansion.
	// This prevents flicker when `func` (e.g. an LSP client) is slow and
	// calls 'sleep', which triggers out_flush().
	ਜੇ (!cpt_src->cs_refresh_always)
	    ins_compl_ਪਾਓ_ਬਾਈਟਾਂ(ins_compl_leader(), -1);

	expand_by_function(0, cpt_compl_ਨਮੂਨਾ.string, cb);

	ਜੇ (!cpt_src->cs_refresh_always)
	    ins_compl_delete();

	cpt_src->cs_refresh_always = compl_opt_refresh_always;
	compl_opt_refresh_always = FALSE;
    }
}
#ਜੇ_ਅੰਤ

/*
 * Retrieve completion matches from functions in the 'cpt' option where the
 * 'refresh:always' flag is set.
 */
    ਸਥਿਰ ਖਾਲੀ
cpt_compl_refresh(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    char_u	*cpt;
    char_u	*p;
    callback_T	*cb = ਨਿਰਾ;
    ਪੂਰਨ		startcol, ret;

    // Make the completion list linear (non-cyclic)
    ins_compl_make_ਰੇਖਿਕ();
    // Make a copy of 'cpt' in case the buffer gets wiped out
    cpt = vim_strsave(curbuf->b_p_cpt);
    strip_caret_numbers_ਵਿੱਚ_ਰੱਖੋ(cpt);

    cpt_sources_ਸੂਚਕਾਂਕ = 0;
    ਲਈ (p = cpt; *p;)
    {
	ਜਦੋਂ (*p == ',' || *p == ' ') // Skip delimiters
	    p++;
	ਜੇ (*p == NUL)
	    ਰੋਕੋ;

	ਜੇ (cpt_sources_array[cpt_sources_ਸੂਚਕਾਂਕ].cs_refresh_always)
	{
	    cb = get_callback_if_cpt_func(p, cpt_sources_ਸੂਚਕਾਂਕ);
	    ਜੇ (cb)
	    {
		remove_old_matches();
		ret = get_userdefined_compl_info(curwin->w_cursor.ਖਾਨਾ, cb,
			&startcol);
		ਜੇ (ret == FAIL)
		{
		    ਜੇ (startcol == -3)
			cpt_sources_array[cpt_sources_ਸੂਚਕਾਂਕ].cs_refresh_always
			    = FALSE;
		    ਨਹੀਂਤਾਂ
			startcol = -2;
		}
		ਨਹੀਂਤਾਂ ਜੇ (startcol < 0 || startcol > curwin->w_cursor.ਖਾਨਾ)
		    startcol = curwin->w_cursor.ਖਾਨਾ;
		cpt_sources_array[cpt_sources_ਸੂਚਕਾਂਕ].cs_startcol = startcol;
		ਜੇ (ret == OK)
		{
		    compl_ਸਰੋਤ_ਸ਼ੁਰੂ_timer(cpt_sources_ਸੂਚਕਾਂਕ);
		    get_cpt_func_completion_matches(cb);
		}
	    }
	}

	(ਖਾਲੀ)copy_option_part(&p, IObuff, IOSIZE, ","); // Advance p
	ਜੇ (may_advance_cpt_ਸੂਚਕਾਂਕ(p))
	    (ਖਾਲੀ)advance_cpt_sources_ਸੂਚਕਾਂਕ_ਸੁਰੱਖਿਅਤ();
    }
    cpt_sources_ਸੂਚਕਾਂਕ = -1;

    vim_free(cpt);
    // Make the list cyclic
    compl_matches = ins_compl_make_cyclic();
#ਜੇ_ਅੰਤ
}

/*
 * "preinserted()" function
 */
    ਖਾਲੀ
f_preinserted(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ins_compl_preinsert_effect())
	rettv->vval.v_ਸੰਖਿਆ = 1;
}
