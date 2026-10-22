/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * definition of global variables
 */

/*
 * Number of Rows and Columns in the screen.
 * Must be long to be able to use them as options in option.c.
 * Note: Use screen_Rows and screen_Columns to access items in ScreenLines[].
 * They may have different values when the screen wasn't (re)allocated yet
 * after setting Rows or Columns (e.g., when starting up).
 */
EXTERN ದೀರ್ಘ	ಅಡ್ಡಸಾಲುಗಳು			// nr of rows in the screen
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DO_INIT
# ಆದರೆ defined(MSWIN)
		    = 25L
# ಇಲ್ಲದಿದ್ದರೆ
		    = 24L
# endif
#ಆದರೆ_ಅಂತ್ಯ
		    ;
EXTERN ದೀರ್ಘ	Columns INIT(= 80);	// nr of columns in the screen

/*
 * The characters that are currently on the screen are kept in ScreenLines[].
 * It is a single block of characters, the size of the screen plus one line.
 * The attributes for those characters are kept in ScreenAttrs[].
 * The virtual column in the line is kept in ScreenCols[].
 *
 * "LineOffset[n]" is the offset from ScreenLines[] for the start of line 'n'.
 * The same value is used for ScreenLinesUC[], ScreenAttrs[] and ScreenCols[].
 *
 * Note: before the screen is initialized and when out of memory these can be
 * NULL.
 */
EXTERN schar_T	*Screen_ಸಾಲುಗಳು INIT(= ಖಾಲಿ);
EXTERN sattr_T	*ScreenAttrs INIT(= ಖಾಲಿ);
EXTERN colnr_T  *Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ INIT(= ಖಾಲಿ);
EXTERN ಚಿಹ್ನೆರಹಿತ	*ಸಾಲು_ಸ್ಥಾನಾಂತರ INIT(= ಖಾಲಿ);
EXTERN char_u	*ಸಾಲು_Wraps INIT(= ಖಾಲಿ);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*Screen_ಸಾಲುಗಳು_UC INIT(= ಖಾಲಿ);	// decoded UTF-8 characters
EXTERN u8char_T	*Screen_ಸಾಲುಗಳು_C[MAX_MCO];		// composing characters
EXTERN ಪೂರ್ಣಾಂಕ	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= ಖಾಲಿ);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_Screen_ಸಾಲು INIT(= ಖಾಲಿ);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN ಪೂರ್ಣಾಂಕ	screen_cur_ಅಡ್ಡಸಾಲು INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	screen_cur_ಲಂಬಸಾಲು INIT(= 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
// used for 'hlsearch' highlight matching
EXTERN ಹೊಂದಿಕೆ_T	screen_ಹುಡುಕು_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T ಹುಡುಕು_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN ಪೂರ್ಣಾಂಕ	no_hlsearch INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#ಆದರೆ_ಅಂತ್ಯ

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN ಪೂರ್ಣಾಂಕ redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN ಗಿಡ್ಡ	*Tab_ಪುಟ_Idxs INIT(= ಖಾಲಿ);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= ಖಾಲಿ);
EXTERN ಪೂರ್ಣಾಂಕ	tabline_stl_click_ಎಣಿಕೆ INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= ಖಾಲಿ);
EXTERN ಪೂರ್ಣಾಂಕ	tabpanel_stl_click_ಎಣಿಕೆ INIT(= 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN ಗಿಡ್ಡ	*popup_ಮುಖವಾಡ INIT(= ಖಾಲಿ);
EXTERN ಗಿಡ್ಡ	*popup_ಮುಖವಾಡ_ಮುಂದಿನ INIT(= ಖಾಲಿ);
// Array with flags for transparent cells of current popup.
EXTERN ಅಕ್ಷರ	*popup_transparent INIT(= ಖಾಲಿ);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN ಪೂರ್ಣಾಂಕ	popup_ಮುಖವಾಡ_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_ಮುಖವಾಡ_tab INIT(= ಖಾಲಿ);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN ಪೂರ್ಣಾಂಕ	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN ದೀರ್ಘ	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN ಪೂರ್ಣಾಂಕ	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= ಖಾಲಿ);
EXTERN schar_T	*pum_bg_ಸಾಲುಗಳು INIT(= ಖಾಲಿ);
EXTERN u8char_T	*pum_bg_ಸಾಲುಗಳು_UC INIT(= ಖಾಲಿ);
EXTERN u8char_T	*pum_bg_ಸಾಲುಗಳು_C[MAX_MCO];
EXTERN ಪೂರ್ಣಾಂಕ	pum_bg_ತುದಿ INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	pum_bg_bot INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ INIT(= 0);

EXTERN ಪೂರ್ಣಾಂಕ	screen_ಅಡ್ಡಸಾಲುಗಳು INIT(= 0);	    // actual size of ScreenLines[]
EXTERN ಪೂರ್ಣಾಂಕ	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN ಪೂರ್ಣಾಂಕ	mod_ಮುಖವಾಡ INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN ಪೂರ್ಣಾಂಕ	vgetc_mod_ಮುಖವಾಡ INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN ಪೂರ್ಣಾಂಕ	cmdline_ಅಡ್ಡಸಾಲು;
EXTERN ಪೂರ್ಣಾಂಕ	cmdline_ಲಂಬಸಾಲು_off;
EXTERN ಪೂರ್ಣಾಂಕ	cmdline_ಅಗಲ;

EXTERN ಪೂರ್ಣಾಂಕ	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN ಪೂರ್ಣಾಂಕ	redraw_ಬಹುಲಕ INIT(= FALSE);	// mode must be redrawn
EXTERN ಪೂರ್ಣಾಂಕ	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN ಪೂರ್ಣಾಂಕ	ಬಹುಲಕ_displayed INIT(= FALSE);	// mode is being displayed
EXTERN ಪೂರ್ಣಾಂಕ	no_win_do_ಸಾಲುಗಳು_ins INIT(= FALSE); // don't insert lines
#ಆದರೆ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN ಪೂರ್ಣಾಂಕ	cmdline_ನಕ್ಷತ್ರ INIT(= FALSE);	// cmdline is crypted
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	exec_from_reg INIT(= FALSE);	// executing register

EXTERN ಪೂರ್ಣಾಂಕ	screen_cleared INIT(= FALSE);	// screen has been cleared

/*
 * When '$' is included in 'cpoptions' option set:
 * When a change command is given that deletes only part of a line, a dollar
 * is put at the end of the changed text. dollar_vcol is set to the virtual
 * column of this '$'.  -1 is used to indicate no $ is being displayed.
 */
EXTERN colnr_T	dollar_vcol INIT(= -1);

/*
 * Variables for Insert mode completion.
 */

EXTERN char_u	*edit_submode INIT(= ಖಾಲಿ); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= ಖಾಲಿ); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= ಖಾಲಿ);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
EXTERN ಪೂರ್ಣಾಂಕ	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_ಲಂಬಸಾಲು;
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_ಸಂದೇಶ INIT(= ಖಾಲಿ);	    // msg to be shown after redraw
EXTERN ಪೂರ್ಣಾಂಕ	keep_ಸಂದೇಶ_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN ಪೂರ್ಣಾಂಕ	keep_ಸಂದೇಶ_ಇನ್ನಷ್ಟು INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN ಪೂರ್ಣಾಂಕ	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN ಪೂರ್ಣಾಂಕ	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN ಪೂರ್ಣಾಂಕ	info_ಸಂದೇಶ INIT(= FALSE); // printing informative message
EXTERN ಪೂರ್ಣಾಂಕ      ಸಂದೇಶ_hist_off INIT(= FALSE); // don't add messages to history
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN ಪೂರ್ಣಾಂಕ	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN ಪೂರ್ಣಾಂಕ	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_ಸಂದೇಶ INIT(= ಖಾಲಿ);
EXTERN ದೀರ್ಘ	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= ಖಾಲಿ);

EXTERN ಪೂರ್ಣಾಂಕ	did_endif INIT(= FALSE);    // just had ":endif"
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN ಪೂರ್ಣಾಂಕ	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN ಪೂರ್ಣಾಂಕ	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN ಪೂರ್ಣಾಂಕ	called_vim_beep;	    // set if vim_beep() is called
EXTERN ಪೂರ್ಣಾಂಕ	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN ಪೂರ್ಣಾಂಕ	called_emsg;		    // always incremented by emsg()
EXTERN ಪೂರ್ಣಾಂಕ	ಒಳ_echowindow;		    // executing ":echowindow"
EXTERN ಪೂರ್ಣಾಂಕ	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN ಪೂರ್ಣಾಂಕ	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN ಪೂರ್ಣಾಂಕ	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN ಪೂರ್ಣಾಂಕ	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN ಪೂರ್ಣಾಂಕ	need_wait_return INIT(= 0); // need to wait for return later
EXTERN ಪೂರ್ಣಾಂಕ	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN ಪೂರ್ಣಾಂಕ	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN ಪೂರ್ಣಾಂಕ	quit_ಇನ್ನಷ್ಟು INIT(= FALSE);    // 'q' hit at "--more--" msg
#ಆದರೆ defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN ಪೂರ್ಣಾಂಕ	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN ಪೂರ್ಣಾಂಕ	intr_char INIT(= 0);	    // extra interrupt character
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN ಪೂರ್ಣಾಂಕ	x_no_connect INIT(= FALSE); // don't connect to X server
# ಆದರೆ defined(FEAT_CLIENTSERVER)
EXTERN ಪೂರ್ಣಾಂಕ	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN ಪೂರ್ಣಾಂಕ	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN ಪೂರ್ಣಾಂಕ	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN ಪೂರ್ಣಾಂಕ	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN ಪೂರ್ಣಾಂಕ	ಸಾಲುಗಳು_ಎಡ INIT(= -1);	    // lines left for listing
EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, ಗಾತ್ರ(estack_T), 50, ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SOURCING_INFO  (exestack.ga_ದತ್ತಾಂಶ != ಖಾಲಿ && exestack.ga_len > 0)
// name of error message source
#ವ್ಯಾಖ್ಯಾನಿಸು SOURCING_NAME (((estack_T *)exestack.ga_ದತ್ತಾಂಶ)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#ವ್ಯಾಖ್ಯಾನಿಸು SOURCING_LNUM (((estack_T *)exestack.ga_ದತ್ತಾಂಶ)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    INIT4(0, 0, 0, 0);
#ಇಲ್ಲದಿದ್ದರೆ
    INIT(= {0});
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// whether inside compile_def_function()
EXTERN ಪೂರ್ಣಾಂಕ	estack_compiling INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ	ex_nesting_level INIT(= 0);	// nesting level
EXTERN ಪೂರ್ಣಾಂಕ	debug_break_level INIT(= -1);	// break below this level
EXTERN ಪೂರ್ಣಾಂಕ	debug_did_ಸಂದೇಶ INIT(= FALSE);	// did "debug mode" message
EXTERN ಪೂರ್ಣಾಂಕ	debug_tick INIT(= 0);		// breakpoint change count
EXTERN ಪೂರ್ಣಾಂಕ	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN ಪೂರ್ಣಾಂಕ	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_ವಸ್ತುಗಳು INIT5(0, 0, ಗಾತ್ರ(scriptitem_T *), 20, ಖಾಲಿ);
# define SCRIPT_ಅಂಶ(id)    (((scriptitem_T **)script_ವಸ್ತುಗಳು.ga_ದತ್ತಾಂಶ)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_ವಸ್ತುಗಳು.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_ಅಂಶ(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ))[j]

/*
 * The exception currently being thrown.  Used to pass an exception to
 * a different cstack.  Also used for discarding an exception before it is
 * caught or made pending.  Only valid when did_throw is TRUE.
 */
EXTERN except_T *current_exception;

/*
 * did_throw: An exception is being thrown.  Reset when the exception is caught
 * or as long as it is pending in a finally clause.
 */
EXTERN ಪೂರ್ಣಾಂಕ did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN ಪೂರ್ಣಾಂಕ need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN ಪೂರ್ಣಾಂಕ check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN ಪೂರ್ಣಾಂಕ trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN ಪೂರ್ಣಾಂಕ force_abort INIT(= FALSE);

/*
 * "msg_list" points to a variable in the stack of do_cmdline() which keeps
 * the list of arguments of several emsg() calls, one of which is to be
 * converted to an error exception immediately after the failing command
 * returns.  The message to be used for the exception value is pointed to by
 * the "throw_msg" field of the first element in the list.  It is usually the
 * same as the "msg" field of that element, but can be identical to the "msg"
 * field of a later list element, when the "emsg_severe" flag was set when the
 * emsg() call was made.
 */
EXTERN msglist_T **ಸಂದೇಶ_list INIT(= ಖಾಲಿ);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN ಪೂರ್ಣಾಂಕ suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_ರಾಶಿ INIT(= ಖಾಲಿ);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN ಪೂರ್ಣಾಂಕ	may_garbage_collect INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ	want_garbage_collect INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ	garbage_collect_at_exit INIT(= FALSE);


// Array with predefined commonly used types.
//
// For each entry of a regular type the next one has the "const" version.
// E.g. "&t_const_bool == &t_bool + 1"

// t_unknown - used for when the type is really unknown, e.g. global variables.
// Also for when a function may or may not return something.
# define t_unknown		(static_types[0])
# define t_const_unknown		(static_types[1])

// t_any -  used for when the type can be anything, but excludes "void".
# define t_any			(static_types[2])
# define t_const_any		(static_types[3])

// t_void - used for a function not returning anything.
# define t_void			(static_types[4])
# define t_const_void		(static_types[5])

# define t_bool			(static_types[6])
# define t_const_bool		(static_types[7])

# define t_null			(static_types[8])
# define t_const_null		(static_types[9])

# define t_none			(static_types[10])
# define t_const_none		(static_types[11])

# define t_ಸಂಖ್ಯೆ		(static_types[12])
# define t_const_ಸಂಖ್ಯೆ		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_ಸಂಖ್ಯೆ_bool		(static_types[14])
# define t_const_ಸಂಖ್ಯೆ_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_ಸಂಖ್ಯೆ_float		(static_types[16])
# define t_const_ಸಂಖ್ಯೆ_float	(static_types[17])

# define t_float			(static_types[18])
# define t_const_float		(static_types[19])

# define t_string		(static_types[20])
# define t_const_string		(static_types[21])

# define t_blob			(static_types[22])
# define t_const_blob		(static_types[23])

# define t_blob_null		(static_types[24])
# define t_const_blob_null	(static_types[25])

# define t_ಕೆಲಸ			(static_types[26])
# define t_const_ಕೆಲಸ		(static_types[27])

# define t_channel		(static_types[28])
# define t_const_channel		(static_types[29])

// t_number_or_string - Special value used for @#.
# define t_ಸಂಖ್ಯೆ_or_string	(static_types[30])
# define t_const_ಸಂಖ್ಯೆ_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_ಸಂಖ್ಯೆ		(static_types[38])
# define t_const_func_ಸಂಖ್ಯೆ	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_ಸಂಖ್ಯೆ		(static_types[48])
# define t_const_func_0_ಸಂಖ್ಯೆ	(static_types[49])

# define t_func_0_string		(static_types[50])
# define t_const_func_0_string	(static_types[51])

# define t_list_any		(static_types[52])
# define t_const_list_any	(static_types[53])

# define t_dict_any		(static_types[54])
# define t_const_dict_any	(static_types[55])

# define t_list_ಖಾಲಿಗಳು		(static_types[56])
# define t_const_list_ಖಾಲಿಗಳು	(static_types[57])

# define t_dict_ಖಾಲಿಗಳು		(static_types[58])
# define t_const_dict_ಖಾಲಿಗಳು	(static_types[59])

# define t_list_bool		(static_types[60])
# define t_const_list_bool	(static_types[61])

# define t_list_ಸಂಖ್ಯೆ		(static_types[62])
# define t_const_list_ಸಂಖ್ಯೆ	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_ಕೆಲಸ		(static_types[66])
# define t_const_list_ಕೆಲಸ	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_ಸಂಖ್ಯೆ	(static_types[72])
# define t_const_list_list_ಸಂಖ್ಯೆ (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_ಸಂಖ್ಯೆ	(static_types[76])
# define t_const_list_list_list_ಸಂಖ್ಯೆ (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_ಸಂಖ್ಯೆ		(static_types[80])
# define t_const_dict_ಸಂಖ್ಯೆ	(static_types[81])

# define t_dict_string		(static_types[82])
# define t_const_dict_string	(static_types[83])

# define t_super			(static_types[84])
# define t_const_super		(static_types[85])

# define t_object_any		(static_types[86])
# define t_const_object_any	(static_types[87])

# define t_class			(static_types[88])
# define t_const_class		(static_types[89])

# define t_typealias		(static_types[90])
# define t_const_typealias	(static_types[91])

# define t_tuple_any		(static_types[92])
# define t_const_tuple_any	(static_types[93])

# define t_tuple_ಖಾಲಿಗಳು		(static_types[94])
# define t_const_tuple_ಖಾಲಿಗಳು	(static_types[95])


EXTERN type_T static_types[96]
# ifdef DO_INIT
= {
    // 0: t_unknown
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 12: t_number
    {VAR_ಸಂಖ್ಯೆ, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_ಸಂಖ್ಯೆ, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 14: t_number_bool
    {VAR_ಸಂಖ್ಯೆ, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_ಸಂಖ್ಯೆ, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 16: t_number_float
    {VAR_ಸಂಖ್ಯೆ, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_ಸಂಖ್ಯೆ, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ಖಾಲಿ, ಖಾಲಿ},

    // 26: t_job
    {VAR_ಕೆಲಸ, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_ಕೆಲಸ, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ಖಾಲಿ, ಖಾಲಿ},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ಖಾಲಿ, ಖಾಲಿ},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ಖಾಲಿ, ಖಾಲಿ},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ಖಾಲಿ, ಖಾಲಿ},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ಖಾಲಿ, ಖಾಲಿ},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ಖಾಲಿ, ಖಾಲಿ},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ಖಾಲಿ, ಖಾಲಿ},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ಖಾಲಿ, ಖಾಲಿ},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ಖಾಲಿ, ಖಾಲಿ},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ಖಾಲಿ, ಖಾಲಿ},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ಖಾಲಿ, ಖಾಲಿ},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ಖಾಲಿ, ಖಾಲಿ},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ಖಾಲಿ, ಖಾಲಿ},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ಖಾಲಿ, ಖಾಲಿ},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_ಕೆಲಸ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ಕೆಲಸ, ಖಾಲಿ, ಖಾಲಿ},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, ಖಾಲಿ, ಖಾಲಿ},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, ಖಾಲಿ, ಖಾಲಿ},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, ಖಾಲಿ, ಖಾಲಿ},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ಖಾಲಿ, ಖಾಲಿ},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ಖಾಲಿ},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ಖಾಲಿ, ಖಾಲಿ},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ಖಾಲಿ, ಖಾಲಿ},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ},
}
# endif
;

EXTERN ಪೂರ್ಣಾಂಕ	did_ಮೂಲ_packages INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN ಪೂರ್ಣಾಂಕ	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN ಪೂರ್ಣಾಂಕ	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN ಪೂರ್ಣಾಂಕ include_none INIT(= 0);	// when 1 include "None"
EXTERN ಪೂರ್ಣಾಂಕ include_default INIT(= 0);	// when 1 include "default"
EXTERN ಪೂರ್ಣಾಂಕ include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN ಪೂರ್ಣಾಂಕ	highlight_ಹೊಂದಿಕೆ INIT(= FALSE);	// show search match pos
EXTERN linenr_T	ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು;		// lines of matched string
EXTERN colnr_T	ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol;		// col nr of match end
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
EXTERN linenr_T	ಹುಡುಕು_ಮೊದಲ_ಸಾಲು INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN ಪೂರ್ಣಾಂಕ	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN ಪೂರ್ಣಾಂಕ	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN ಪೂರ್ಣಾಂಕ	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN ಪೂರ್ಣಾಂಕ	highlight_attr[HLF_ಎಣಿಕೆ];  // Highl. attr for each context.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
# define USER_HIGHLIGHT
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USER_HIGHLIGHT
EXTERN ಪೂರ್ಣಾಂಕ	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN ಪೂರ್ಣಾಂಕ	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN ಪೂರ್ಣಾಂಕ	highlight_stlterm[9];		// On top of user
EXTERN ಪೂರ್ಣಾಂಕ	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN ಪೂರ್ಣಾಂಕ	skip_ಪದ_loop INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= ಖಾಲಿ);	// "-U" cmdline argument
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	cterm_ಸಾಮಾನ್ಯ_fg_color INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	cterm_ಸಾಮಾನ್ಯ_fg_bold INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	cterm_ಸಾಮಾನ್ಯ_bg_color INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	cterm_ಸಾಮಾನ್ಯ_ul_color INIT(= 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_ಸಾಮಾನ್ಯ_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_ಸಾಮಾನ್ಯ_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_ಸಾಮಾನ್ಯ_ul_gui_color INIT(= INVALCOLOR);
#ಆದರೆ_ಅಂತ್ಯ
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
EXTERN ಪೂರ್ಣಾಂಕ	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN ಪೂರ್ಣಾಂಕ	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN ಪೂರ್ಣಾಂಕ	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN ಪೂರ್ಣಾಂಕ	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(ಖಾಲಿ, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= ಖಾಲಿ);
EXTERN win_T	*au_pending_free_win INIT(= ಖಾಲಿ);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN ಪೂರ್ಣಾಂಕ	mouse_ಅಡ್ಡಸಾಲು;
EXTERN ಪೂರ್ಣಾಂಕ	mouse_ಲಂಬಸಾಲು;
EXTERN ಪೂರ್ಣಾಂಕ	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN ಪೂರ್ಣಾಂಕ	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN ಪೂರ್ಣಾಂಕ	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#ಆದರೆ defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN ಪೂರ್ಣಾಂಕ	WantQueryMouse INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN ಪೂರ್ಣಾಂಕ	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_prev_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN ಪೂರ್ಣಾಂಕ	gui_prev_topfill INIT(= 0);
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
EXTERN ಪೂರ್ಣಾಂಕ	drag_ಸ್ಥಿತಿ_ಸಾಲು INIT(= FALSE);	// dragging the status line
EXTERN ಪೂರ್ಣಾಂಕ	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN ಪೂರ್ಣಾಂಕ	drag_sep_ಸಾಲು INIT(= FALSE);	// dragging vert separator
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
// Value set from 'diffopt'.
EXTERN ಪೂರ್ಣಾಂಕ	diff_context INIT(= 6);		// context for folds
EXTERN ಪೂರ್ಣಾಂಕ      linematch_ಸಾಲುಗಳು INIT(= 0);      // number of lines for diff line match
EXTERN ಪೂರ್ಣಾಂಕ	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN ಪೂರ್ಣಾಂಕ	diff_need_scrollbind INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN ಪೂರ್ಣಾಂಕ	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN ಪೂರ್ಣಾಂಕ	redraw_not_allowed INIT(= FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN ಪೂರ್ಣಾಂಕ	dont_parse_messages INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*ಬೇರು_menu INIT(= ಖಾಲಿ);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN ಪೂರ್ಣಾಂಕ	sys_menu INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN ಪೂರ್ಣಾಂಕ force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN ಪೂರ್ಣಾಂಕ	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN ಪೂರ್ಣಾಂಕ	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN ಪೂರ್ಣಾಂಕ	current_scrollbar;
EXTERN long_u	scrollbar_ಮೌಲ್ಯ;

// found "-rv" or "-reverse" in command line args
EXTERN ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN ಅಕ್ಷರ	*font_ವಾದ INIT(= ಖಾಲಿ);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN ಅಕ್ಷರ	*background_ವಾದ INIT(= ಖಾಲಿ);

// "-fg" or "-foreground" command line argument
EXTERN ಅಕ್ಷರ	*foreground_ವಾದ INIT(= ಖಾಲಿ);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN ಅಸ್ಥಿರ sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN ಪೂರ್ಣಾಂಕ	new_pixel_ಅಗಲ INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	new_pixel_ಎತ್ತರ INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN ಪೂರ್ಣಾಂಕ	gui_win_x INIT(= -1);
EXTERN ಪೂರ್ಣಾಂಕ	gui_win_y INIT(= -1);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
EXTERN Clipboard_T clip_ನಕ್ಷತ್ರ;	// PRIMARY selection in X11/Wayland
# ಆದರೆ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# ಇಲ್ಲದಿದ್ದರೆ
#  define clip_plus clip_ನಕ್ಷತ್ರ	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN ಪೂರ್ಣಾಂಕ	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN ಪೂರ್ಣಾಂಕ	clip_autoselect_ನಕ್ಷತ್ರ INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ	clip_autoselect_plus INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ	clip_autoselectml INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= ಖಾಲಿ);
EXTERN ಪೂರ್ಣಾಂಕ	clip_unnamed_saved INIT(= 0);
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= ಖಾಲಿ);	// previous window (may equal curwin)
#ವ್ಯಾಖ್ಯಾನಿಸು ONE_ಕಿಟಕಿ (firstwin == lastwin)
#ವ್ಯಾಖ್ಯಾನಿಸು W_ಮುಂದಿನ(wp) ((wp)->w_ಮುಂದಿನ)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#ವ್ಯಾಖ್ಯಾನಿಸು AUCMD_WIN_ಎಣಿಕೆ 5

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    ಪೂರ್ಣಾಂಕ		auc_win_ಬಳಸಿದ;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_ಎಣಿಕೆ];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN win_T    *ಮೊದಲ_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= ಖಾಲಿ);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN ಪೂರ್ಣಾಂಕ	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN ಪೂರ್ಣಾಂಕ	popup_ಬಳಸುತ್ತದೆ_mouse_move INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ	ಪಠ್ಯ_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN ಪೂರ್ಣಾಂಕ	ignore_ಪಠ್ಯ_props INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN ಪೂರ್ಣಾಂಕ	pum_will_redraw INIT(= FALSE);

/*
 * The window layout is kept in a tree of frames.  topframe points to the top
 * of the tree.
 */
EXTERN ಚೌಕಟ್ಟು_T	*topframe;	// top of the window frame tree

/*
 * Tab pages are alternative topframes.  "first_tabpage" points to the first
 * one in the list, "curtab" is the current one. "lastused_tabpage" is the
 * last used one.
 */
EXTERN tabpage_T    *ಮೊದಲ_tabpage;
EXTERN tabpage_T    *curtab;
EXTERN tabpage_T    *lastused_tabpage;
EXTERN ಪೂರ್ಣಾಂಕ	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN ಪೂರ್ಣಾಂಕ	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#ಆದರೆ defined(FEAT_TABPANEL)
EXTERN ಪೂರ್ಣಾಂಕ	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#ಆದರೆ_ಅಂತ್ಯ

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= ಖಾಲಿ);	// first buffer
EXTERN buf_T	*lastbuf INIT(= ಖಾಲಿ);	// last buffer
EXTERN buf_T	*curbuf INIT(= ಖಾಲಿ);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN ಪೂರ್ಣಾಂಕ	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN ಪೂರ್ಣಾಂಕ	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN ಪೂರ್ಣಾಂಕ	arg_had_ಕೊನೆಯ INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN ಪೂರ್ಣಾಂಕ	ru_ಲಂಬಸಾಲು;		// column for ruler
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
EXTERN ಪೂರ್ಣಾಂಕ	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	sc_ಲಂಬಸಾಲು;		// column for shown command

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TEMPDIRNAMES
# ಆದರೆ defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= ಖಾಲಿ); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= ಖಾಲಿ); // Name of Vim's own temp dir.
					   // Ends in a slash.
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN ಪೂರ್ಣಾಂಕ	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN ಪೂರ್ಣಾಂಕ	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN ಪೂರ್ಣಾಂಕ	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN ಪೂರ್ಣಾಂಕ	v_dying INIT(= 0); // internal value of v:dying
EXTERN ಪೂರ್ಣಾಂಕ	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#ಆದರೆ defined(FEAT_AUTOCHDIR)
EXTERN ಪೂರ್ಣಾಂಕ	test_autochdir INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ	*ಕೊನೆಯ_chdir_reason INIT(= ಖಾಲಿ);
#ಆದರೆ defined(EXITFREE)
EXTERN ಪೂರ್ಣಾಂಕ	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#ಆದರೆ_ಅಂತ್ಯ
// volatile because it is used in signal handler deathtrap().
EXTERN ಅಸ್ಥಿರ sig_atomic_t ತುಂಬಿದ_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN ಪೂರ್ಣಾಂಕ	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN ಪೂರ್ಣಾಂಕ	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN ಪೂರ್ಣಾಂಕ	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN ಪೂರ್ಣಾಂಕ	curbuf_ಬೀಗ INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN ಪೂರ್ಣಾಂಕ	allbuf_ಬೀಗ INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
EXTERN ಪೂರ್ಣಾಂಕ	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	silent_ಬಹುಲಕ INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಪೂರ್ಣಾಂಕ	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#ಆದರೆ_ಅಂತ್ಯ

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN ಪೂರ್ಣಾಂಕ	VIsual_ಸಕ್ರಿಯ INIT(= FALSE);
				// whether Visual mode is active
EXTERN ಪೂರ್ಣಾಂಕ	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN ಪೂರ್ಣಾಂಕ	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN ಪೂರ್ಣಾಂಕ  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN ಪೂರ್ಣಾಂಕ	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN ಪೂರ್ಣಾಂಕ	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN ಪೂರ್ಣಾಂಕ	VIsual_ಬಹುಲಕ INIT(= 'v');
				// type of Visual mode

EXTERN ಪೂರ್ಣಾಂಕ	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN ಪೂರ್ಣಾಂಕ	resel_VIsual_ಬಹುಲಕ INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_ಸಾಲು_ಎಣಿಕೆ;	// number of lines
EXTERN colnr_T	resel_VIsual_vcol;		// nr of cols or end col

/*
 * When pasting text with the middle mouse button in visual mode with
 * restart_edit set, remember where it started so we can set Insstart.
 */
EXTERN pos_T	where_paste_started;

/*
 * This flag is used to make auto-indent work right on lines where only a
 * <RETURN> or <ESC> is typed. It is set when an auto-indent is done, and
 * reset when any other editing is done on the line. If an <ESC> or <RETURN>
 * is received, and did_ai is TRUE, the line is truncated.
 */
EXTERN ಪೂರ್ಣಾಂಕ     did_ai INIT(= FALSE);

/*
 * Column of first char after autoindent.  0 when no autoindent done.  Used
 * when 'backspace' is 0, to avoid backspacing over autoindent.
 */
EXTERN colnr_T	ai_ಲಂಬಸಾಲು INIT(= 0);

/*
 * This is a character which will end a start-middle-end comment when typed as
 * the first character on a new line.  It is taken from the last character of
 * the "end" comment leader when the COM_AUTO_END flag is given for that
 * comment end in 'comments'.  It is only valid when did_ai is TRUE.
 */
EXTERN ಪೂರ್ಣಾಂಕ     ಕೊನೆ_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN ಪೂರ್ಣಾಂಕ     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN ಪೂರ್ಣಾಂಕ	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN ಪೂರ್ಣಾಂಕ	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN ಪೂರ್ಣಾಂಕ	can_si_ಹಿಂಭಾಗ INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ	old_indent INIT(= 0);	// for ^^D command in insert mode

EXTERN pos_T	saved_cursor		// w_cursor before formatting text.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DO_INIT
		    = {0, 0, 0}
#ಆದರೆ_ಅಂತ್ಯ
		    ;

/*
 * Stuff for insert mode.
 */
EXTERN pos_T	Insstart;		// This is where the latest
					// insert/append mode started.

// This is where the latest insert/append mode started. In contrast to
// Insstart, this won't be reset by certain keys and is needed for
// op_insert(), to detect correctly where inserting by the user started.
EXTERN pos_T	Insstart_orig;

/*
 * Stuff for MODE_VREPLACE state.
 */
EXTERN ಪೂರ್ಣಾಂಕ	orig_ಸಾಲು_ಎಣಿಕೆ INIT(= 0);  // Line count when "gR" started
EXTERN ಪೂರ್ಣಾಂಕ	vr_ಸಾಲುಗಳು_ಬದಲಾಯಿತು INIT(= 0); // #Lines changed by "gR" so far

#ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
// argument to SETJMP() for handling X IO errors
EXTERN JMP_BUF x_jump_env;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * These flags are set based upon 'fileencoding'.
 * Note that "enc_utf8" is also set for "unicode", because the characters are
 * internally stored as UTF-8 (to avoid trouble with NUL bytes).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_JPN	932	// japan
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_JPNU	9932	// euc-jp
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_KOR	949	// korea
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_KORU	9949	// euc-kr
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_CHS	936	// chinese
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_CHSU	9936	// euc-cn
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_CHT	950	// taiwan
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_CHTU	9950	// euc-tw
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_2BYTE	1	// 2byte-
#ವ್ಯಾಖ್ಯಾನಿಸು DBCS_DEBUG	(-1)

EXTERN ಪೂರ್ಣಾಂಕ	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN ಪೂರ್ಣಾಂಕ	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN ಪೂರ್ಣಾಂಕ	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN ಪೂರ್ಣಾಂಕ	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#ಆದರೆ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN ಪೂರ್ಣಾಂಕ	enc_codepage INIT(= -1);
EXTERN ಪೂರ್ಣಾಂಕ	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN ಅಕ್ಷರ	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T ಒಳಹರಿವು_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN ಪೂರ್ಣಾಂಕ (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN ಪೂರ್ಣಾಂಕ (*mb_ptr2len_len)(char_u *p, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN ಪೂರ್ಣಾಂಕ (*mb_char2len)(ಪೂರ್ಣಾಂಕ c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN ಪೂರ್ಣಾಂಕ (*mb_char2bytes)(ಪೂರ್ಣಾಂಕ c, char_u *buf) INIT(= latin_char2bytes);

EXTERN ಪೂರ್ಣಾಂಕ (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN ಪೂರ್ಣಾಂಕ (*mb_ptr2cells_len)(char_u *p, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2) INIT(= latin_ptr2cells_len);
EXTERN ಪೂರ್ಣಾಂಕ (*mb_char2cells)(ಪೂರ್ಣಾಂಕ c) INIT(= latin_char2cells);
EXTERN ಪೂರ್ಣಾಂಕ (*mb_off2cells)(ಚಿಹ್ನೆರಹಿತ off, ಚಿಹ್ನೆರಹಿತ max_off) INIT(= latin_off2cells);
EXTERN ಪೂರ್ಣಾಂಕ (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN ಪೂರ್ಣಾಂಕ (*mb_ತಲೆ_off)(char_u *ತಳ, char_u *p) INIT(= latin_ತಲೆ_off);

#ಆದರೆ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **inbuf, size_t *inbytesleft, ಅಕ್ಷರ **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tocode, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fromcode);
EXTERN ಪೂರ್ಣಾಂಕ (*iconv_close) (iconv_t cd);
EXTERN ಪೂರ್ಣಾಂಕ (*iconvctl) (iconv_t cd, ಪೂರ್ಣಾಂಕ request, ಶೂನ್ಯ *ವಾದ);
EXTERN ಪೂರ್ಣಾಂಕ* (*iconv_errno) (ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= ಖಾಲಿ);
/*
 * Start and end column of the preedit area in virtual columns from the start
 * of the text line.  When there is no preedit area they are set to MAXCOL.
 * "preedit_end_col" is needed for coloring the preedited string.  Drawing the
 * color between "preedit_start_col" and curpos did not work, because some XIM
 * set the cursor position to the first char of the string.
 */
EXTERN colnr_T		preedit_ಆರಂಭ_ಲಂಬಸಾಲು INIT(= MAXCOL);
EXTERN colnr_T		preedit_ಕೊನೆ_ಲಂಬಸಾಲು INIT(= MAXCOL);

// "xim_changed_while_preediting" is set when changed() can set the 'modified'
// flag even while preediting.
EXTERN ಪೂರ್ಣಾಂಕ		xim_ಬದಲಾಯಿತು_while_preediting INIT(= FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
EXTERN XIC		xic INIT(= ಖಾಲಿ);
# endif
# ifdef FEAT_GUI
EXTERN guicolor_T	xim_fg_color INIT(= INVALCOLOR);
EXTERN guicolor_T	xim_bg_color INIT(= INVALCOLOR);
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "State" is the main state of Vim.
 * There are other variables that modify the state:
 * "Visual_mode"    When State is MODE_NORMAL or MODE_INSERT.
 * "finish_op"	    When State is MODE_NORMAL, after typing the operator and
 *		    before typing the motion command.
 * "motion_force"   Last motion_force  from do_pending_operator()
 * "debug_mode"	    Debug mode.
 */
EXTERN ಪೂರ್ಣಾಂಕ	State INIT(= ಬಹುಲಕ_ಸಾಮಾನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	debug_ಬಹುಲಕ INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	ಮುಕ್ತಾಯ_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN ದೀರ್ಘ	opcount INIT(= 0);	// count for pending operator
EXTERN ಪೂರ್ಣಾಂಕ	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN ಪೂರ್ಣಾಂಕ exmode_ಸಕ್ರಿಯ INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN ಪೂರ್ಣಾಂಕ pending_exmode_ಸಕ್ರಿಯ INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN ಪೂರ್ಣಾಂಕ reg_recording INIT(= 0);	// register for recording  or zero
EXTERN ಪೂರ್ಣಾಂಕ reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN ಪೂರ್ಣಾಂಕ pending_ಕೊನೆ_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN ಪೂರ್ಣಾಂಕ ನೋಡಿದ_Modify_Other_ಕೀಲಿಗಳು INIT(= FALSE);

// The state for the modifyOtherKeys level
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    // Initially we have no clue if the protocol is on or off.
    MOKS_INITIAL,
    // Used when receiving the state and the level is not two.
    MOKS_OFF,
    // Used when receiving the state and the level is two.
    MOKS_ENABLED,
    // Used after outputting t_TE when the state was MOKS_ENABLED.  We do not
    // really know if t_TE actually disabled the protocol, the following t_TI
    // is expected to request the state, but the response may come only later.
    MOKS_DISABLED,
    // Used after outputting t_TE when the state was not MOKS_ENABLED.
    MOKS_AFTER_T_TE,
} mokstate_T;

// Set when a response to XTQMODKEYS was received.  Only works for xterm
// version 377 and later.
EXTERN mokstate_T modify_otherkeys_state INIT(= MOKS_INITIAL);

// The state for the Kitty keyboard protocol.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    // Initially we have no clue if the protocol is on or off.
    KKPS_INITIAL,
    // Used when receiving the state and the flags are zero.
    KKPS_OFF,
    // Used when receiving the state and the flags are non-zero.
    KKPS_ENABLED,
    // Used after outputting t_TE when the state was KKPS_ENABLED.  We do not
    // really know if t_TE actually disabled the protocol, the following t_TI
    // is expected to request the state, but the response may come only later.
    KKPS_DISABLED,
    // Used after outputting t_TE when the state was not KKPS_ENABLED.
    KKPS_AFTER_T_TE,
} kkpstate_T;

EXTERN kkpstate_T kitty_protocol_state INIT(= KKPS_INITIAL);

EXTERN ಪೂರ್ಣಾಂಕ no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN ಪೂರ್ಣಾಂಕ no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN ಪೂರ್ಣಾಂಕ allow_ಕೀಲಿಗಳು INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN ಪೂರ್ಣಾಂಕ no_reduce_ಕೀಲಿಗಳು INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN ಪೂರ್ಣಾಂಕ no_u_sync INIT(= 0);		// Don't call u_sync()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN ಪೂರ್ಣಾಂಕ arrow_ಬಳಸಿದ;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN ಪೂರ್ಣಾಂಕ	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN ಪೂರ್ಣಾಂಕ	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
EXTERN ಪೂರ್ಣಾಂಕ	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN ಪೂರ್ಣಾಂಕ	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN ಪೂರ್ಣಾಂಕ	sticky_cmdmod_flags INIT(= 0); // used by :execute

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	is_export INIT(= FALSE);    // :export {cmd}
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_silent INIT(= 0);	// don't print messages
EXTERN ಪೂರ್ಣಾಂಕ	emsg_silent INIT(= 0);	// don't print error messages
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN ಪೂರ್ಣಾಂಕ	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN ಪೂರ್ಣಾಂಕ	ಒಳ_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN ಪೂರ್ಣಾಂಕ	ಅದಲುಬದಲು_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN ಪೂರ್ಣಾಂಕ	ಅದಲುಬದಲು_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN ಅಕ್ಷರ	ಸಂದೇಶ_buf[ಸಂದೇಶ_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN ಪೂರ್ಣಾಂಕ	RedrawingDisabled INIT(= 0);

EXTERN ಪೂರ್ಣಾಂಕ	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN ಪೂರ್ಣಾಂಕ	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DO_INIT
		    = {ಖಾಲಿ, ಖಾಲಿ, 0, 0, 0, 0, 0, 0, 0}
#ಆದರೆ_ಅಂತ್ಯ
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN ಪೂರ್ಣಾಂಕ	typebuf_was_ಖಾಲಿಗಳು INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ	ex_ಸಾಮಾನ್ಯ_busy INIT(= 0);   // recursiveness of ex_normal()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	ಒಳ_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	ex_ಸಾಮಾನ್ಯ_ಬೀಗ INIT(= 0);   // forbid use of ex_normal()

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ	ignore_script INIT(= FALSE);  // ignore script input
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	stop_ಸೇರಿಸು_ಬಹುಲಕ;	// for ":stopinsert" and 'insertmode'

EXTERN ಪೂರ್ಣಾಂಕ	ಕೀಲಿ_Typed;		// TRUE if user typed current char
EXTERN ಪೂರ್ಣಾಂಕ	ಕೀಲಿ_Stuffed;		// TRUE if current char from stuffbuf
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
EXTERN ಪೂರ್ಣಾಂಕ	vgetc_im_ಸಕ್ರಿಯ;	// Input Method was active for last
					// character obtained from vgetc()
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN ಪೂರ್ಣಾಂಕ	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN ಪೂರ್ಣಾಂಕ	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN ಪೂರ್ಣಾಂಕ	do_redraw INIT(= FALSE);    // extra redraw once
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN ಪೂರ್ಣಾಂಕ	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN ಪೂರ್ಣಾಂಕ	redrawtime_ಎಲ್ಲೆ_ಗಣ INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	need_highlight_ಬದಲಾಯಿತು INIT(= TRUE);

#ವ್ಯಾಖ್ಯಾನಿಸು NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN ಪೂರ್ಣಾಂಕ	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= ಖಾಲಿ);   // stream to write script to
EXTERN ಪೂರ್ಣಾಂಕ	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN ಅಸ್ಥಿರ sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN ಅಸ್ಥಿರ sig_atomic_t got_sigusr1 INIT(= FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಪದ_CONSOLE
EXTERN ಪೂರ್ಣಾಂಕ	ಪದ_console INIT(= FALSE); // set to TRUE when console used
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	termcap_ಸಕ್ರಿಯ INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN ಪೂರ್ಣಾಂಕ	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN ಪೂರ್ಣಾಂಕ	searchcmdlen;		    // length of previous search cmd
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಪೂರ್ಣಾಂಕ	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_ಒಳ INIT(= ಖಾಲಿ); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_ಹೊರಹೋಗು INIT(= ಖಾಲಿ); // Set by vim_regexec()
					    // to store \z\(...\) matches
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ	did_outofmem_ಸಂದೇಶ INIT(= FALSE);
					    // set after out of memory msg
EXTERN ಪೂರ್ಣಾಂಕ	did_swapwrite_ಸಂದೇಶ INIT(= FALSE);
					    // set after swap write error msg
EXTERN ಪೂರ್ಣಾಂಕ	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN ಪೂರ್ಣಾಂಕ	global_busy INIT(= 0);	    // set when :global is executing
EXTERN ಪೂರ್ಣಾಂಕ	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN ಪೂರ್ಣಾಂಕ	need_ಆರಂಭ_insertmode INIT(= FALSE);
					    // start insert mode soon
#ಆದರೆ defined(FEAT_EVAL)
EXTERN char_u	ಕೊನೆಯ_ಬಹುಲಕ[ಬಹುಲಕ_MAX_ಉದ್ದಳತೆ] INIT(= "n"); // for ModeChanged event
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*ಕೊನೆಯ_cmdline INIT(= ಖಾಲಿ); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= ಖಾಲಿ); // command line for "."
EXTERN char_u	*new_ಕೊನೆಯ_cmdline INIT(= ಖಾಲಿ);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= ಖಾಲಿ); // fname for <afile> on cmdline
EXTERN ಪೂರ್ಣಾಂಕ	autocmd_fname_ತುಂಬಿದ;	     // autocmd_fname is full path
EXTERN ಪೂರ್ಣಾಂಕ	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_ಹೊಂದಿಕೆ INIT(= ಖಾಲಿ); // name for <amatch> on cmdline
EXTERN ಪೂರ್ಣಾಂಕ	aucmd_cmdline_ಬದಲಾಯಿತು_ಎಣಿಕೆ INIT(= 0);

EXTERN ಪೂರ್ಣಾಂಕ	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	ಕೊನೆಯ_cursormoved	      // for CursorMoved event
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DO_INIT
		    = {0, 0, 0}
#ಆದರೆ_ಅಂತ್ಯ
		    ;

EXTERN ಪೂರ್ಣಾಂಕ	postponed_ವಿಭಜಿಸು INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN ಪೂರ್ಣಾಂಕ	postponed_ವಿಭಜಿಸು_flags INIT(= 0);  // args for win_split()
EXTERN ಪೂರ್ಣಾಂಕ	postponed_ವಿಭಜಿಸು_tab INIT(= 0);  // cmdmod.cmod_tab
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಪೂರ್ಣಾಂಕ	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	g_ಟ್ಯಾಗ್_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN ಪೂರ್ಣಾಂಕ	replace_ಸ್ಥಾನಾಂತರ INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN ಪೂರ್ಣಾಂಕ	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*ಖಾಲಿಗಳು_option INIT(= (char_u *)"");

EXTERN ಪೂರ್ಣಾಂಕ  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= ಖಾಲಿ);	// message redirection file
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ  redir_reg INIT(= 0);	// message redirection register
EXTERN ಪೂರ್ಣಾಂಕ  redir_vname INIT(= 0);	// message redirection variable
EXTERN ಪೂರ್ಣಾಂಕ  redir_execute INIT(= 0);	// execute() redirection
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#ಆದರೆ_ಅಂತ್ಯ

EXTERN ಪೂರ್ಣಾಂಕ  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN ಪೂರ್ಣಾಂಕ  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN ಪೂರ್ಣಾಂಕ  wild_menu_showing INIT(= 0);
#ವ್ಯಾಖ್ಯಾನಿಸು WM_SHOWN	1		// wildmenu showing
#ವ್ಯಾಖ್ಯಾನಿಸು WM_SCROLLED	2		// wildmenu showing with scroll

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು_ರಿಜಿಸ್ಟರ್_arg INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
EXTERN ಅಕ್ಷರ	breakat_flags[256];	// which characters are in 'breakat'
#ಆದರೆ_ಅಂತ್ಯ

// These are in version.c, call init_longVersion() before use.
ಬಾಹ್ಯ ಅಕ್ಷರ *ಆವೃತ್ತಿ;
#ಆದರೆ defined(HAVE_DATE_ಸಮಯ) && defined(VMS) && defined(VAXC)
ಬಾಹ್ಯ ಅಕ್ಷರ long_ಆವೃತ್ತಿ[];
#ಇಲ್ಲದಿದ್ದರೆ
ಬಾಹ್ಯ ಅಕ್ಷರ *long_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PATHDEF
ಬಾಹ್ಯ char_u *default_vim_dir;
ಬಾಹ್ಯ char_u *default_vimruntime_dir;
ಬಾಹ್ಯ char_u *all_cflags;
ಬಾಹ್ಯ char_u *all_lflags;
# ifdef VMS
ಬಾಹ್ಯ char_u *compiler_ಆವೃತ್ತಿ;
ಬಾಹ್ಯ char_u *compiled_arch;
ಬಾಹ್ಯ char_u *compiled_vers;
# endif
ಬಾಹ್ಯ char_u *compiled_user;
ಬಾಹ್ಯ char_u *compiled_sys;
#ಆದರೆ_ಅಂತ್ಯ

EXTERN char_u	*homedir INIT(= ಖಾಲಿ);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
EXTERN ಪೂರ್ಣಾಂಕ	disable_fold_update INIT(= 0);
#ಆದರೆ_ಅಂತ್ಯ

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN ಪೂರ್ಣಾಂಕ	km_stopsel INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ	km_startsel INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN ಪೂರ್ಣಾಂಕ	cmdwin_ಫಲಿತಾಂಶ INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= ಖಾಲಿ); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= ಖಾಲಿ); // window of cmdline window or NULL

EXTERN char_u no_ಸಾಲುಗಳು_ಸಂದೇಶ[]	INIT(= N_("--No lines in buffer--"));

EXTERN ಅಕ್ಷರ typename_unknown[]	INIT(= N_("unknown"));
EXTERN ಅಕ್ಷರ typename_int[]	INIT(= "int");
EXTERN ಅಕ್ಷರ typename_longint[]	INIT(= "long int");
EXTERN ಅಕ್ಷರ typename_longlongint[]	INIT(= "long long int");
EXTERN ಅಕ್ಷರ typename_unsignedint[]	INIT(= "unsigned int");
EXTERN ಅಕ್ಷರ typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN ಅಕ್ಷರ typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN ಅಕ್ಷರ typename_ಸೂಚಕ[]	INIT(= N_("pointer"));
EXTERN ಅಕ್ಷರ typename_percent[]	INIT(= N_("percent"));
EXTERN ಅಕ್ಷರ typename_char[] INIT(= "char");
EXTERN ಅಕ್ಷರ typename_string[]	INIT(= N_("string"));
EXTERN ಅಕ್ಷರ typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN ದೀರ್ಘ	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN ರಚನೆ subs_ಅಭಿವ್ಯಕ್ತಿ_S	*substitute_instr INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#ಆದರೆ defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_ಒಳ_ICON	1
# define STL_ಒಳ_TITLE	2
EXTERN ಪೂರ್ಣಾಂಕ      stl_syntax INIT(= 0);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= ಖಾಲಿ);
EXTERN ಪೂರ್ಣಾಂಕ		balloon_Eval_For_ಪದ INIT(= FALSE);
# ಆದರೆ defined(FEAT_NETBEANS_INTG)
EXTERN ಪೂರ್ಣಾಂಕ bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
// the table is in misc2.c, because of initializations
ಬಾಹ್ಯ cursorentry_T shape_ಕೋಷ್ಟಕ[SHAPE_IDX_ಎಣಿಕೆ];
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_ತುದಿ		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_ಎಡ		2
# define OPT_PRINT_ಬಲ	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_ಸಂಖ್ಯೆ	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_ಅಂಕೆ_OPTIONS	15

EXTERN option_ಕೋಷ್ಟಕ_T printer_opts[OPT_PRINT_ಅಂಕೆ_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, ಖಾಲಿ, 0, FALSE},
	{"bottom",	TRUE, 0, ಖಾಲಿ, 0, FALSE},
	{"left",	TRUE, 0, ಖಾಲಿ, 0, FALSE},
	{"right",	TRUE, 0, ಖಾಲಿ, 0, FALSE},
	{"header",	TRUE, 0, ಖಾಲಿ, 0, FALSE},
	{"syntax",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"number",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"wrap",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"duplex",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"portrait", FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"paper",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"collate",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"jobsplit", FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"formfeed", FALSE, 0, ಖಾಲಿ, 0, FALSE},
	{"format", FALSE, 0, ಖಾಲಿ, 0, FALSE},
    }
# endif
    ;

// For prt_get_unit().
# define PRT_UNIT_NONE	-1
# define PRT_UNIT_PERC	0
# define PRT_UNIT_INCH	1
# define PRT_UNIT_MM	2
# define PRT_UNIT_ಬಿಂದು	3
# define PRT_UNIT_ಹೆಸರುಗಳು {"pc", "in", "mm", "pt"}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_ಪುಟ_ಅಂಕೆ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XCLIPBOARD
// xterm display name
EXTERN ಅಕ್ಷರ	*xterm_display INIT(= ಖಾಲಿ);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN ಪೂರ್ಣಾಂಕ	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN ಪೂರ್ಣಾಂಕ	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= ಖಾಲಿ);	// name of the server
# ifdef FEAT_X11
EXTERN ಕಿಟಕಿ	comm_ಕಿಟಕಿ INIT(= None);
EXTERN ಕಿಟಕಿ	client_ಕಿಟಕಿ INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*server_Delayed_ಆರಂಭ_Name INIT(= ಖಾಲಿ);
# elif defined(MSWIN)
EXTERN HWND	client_ಕಿಟಕಿ INIT(= 0);
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(VMS)
EXTERN ಪೂರ್ಣಾಂಕ	ಪದ_is_xterm INIT(= FALSE);	// xterm-like 'term'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
EXTERN ಅಕ್ಷರ	psepc INIT(= '\\');	// normal path separator character
EXTERN ಅಕ್ಷರ	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN ಅಕ್ಷರ	pseps[2] INIT2('\\', 0);
#ಆದರೆ_ಅಂತ್ಯ

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN ಪೂರ್ಣಾಂಕ	virtual_op INIT(= MAYBE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
// Display tick, incremented for each call to update_screen()
EXTERN disptick_T	display_tick INIT(= 0);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
// Line in which spell checking wasn't highlighted because it touched the
// cursor position in Insert mode.
EXTERN linenr_T		spell_redraw_lnum INIT(= 0);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
// Set when the cursor line needs to be redrawn.
EXTERN ಪೂರ್ಣಾಂಕ		need_cursor_ಸಾಲು_redraw INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, ಖಾಲಿ}
# endif
		    ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ *netbeansArg INIT(= ಖಾಲಿ);	// the -nb[:host:port:passwd] arg
EXTERN ಪೂರ್ಣಾಂಕ netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN ಪೂರ್ಣಾಂಕ netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN ಪೂರ್ಣಾಂಕ netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN ಪೂರ್ಣಾಂಕ netbeans_Suppress_No_ಸಾಲುಗಳು INIT(= 0); // skip "No lines in buffer"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Some messages that can be shared are included here.
 */
EXTERN ಅಕ್ಷರ ತುದಿ_bot_ಸಂದೇಶ[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN ಅಕ್ಷರ bot_ತುದಿ_ಸಂದೇಶ[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN ಅಕ್ಷರ ಸಾಲು_ಸಂದೇಶ[]	    INIT(= N_(" line "));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
EXTERN ಅಕ್ಷರ need_ಕೀಲಿ_ಸಂದೇಶ[]  INIT(= N_("Need encryption key for \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Comms. with the session manager (XSMP)
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP
EXTERN ಪೂರ್ಣಾಂಕ xsmp_icefd INIT(= -1);   // The actual connection
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
EXTERN FILE *ಸಮಯ_fd INIT(= ಖಾಲಿ);  // where to write startup timing
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN ಪೂರ್ಣಾಂಕ vim_ignored;
EXTERN ಅಕ್ಷರ *vim_ignoredp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN ಪೂರ್ಣಾಂಕ  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN ಪೂರ್ಣಾಂಕ  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN ಪೂರ್ಣಾಂಕ  disable_char_avail_for_testing INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  disable_redraw_for_testing INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  nfa_fail_for_testing INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  no_query_mouse_for_testing INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  ui_delay_for_testing INIT(= 0);
EXTERN ಪೂರ್ಣಾಂಕ  reset_ಪದ_props_on_termresponse INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN ದೀರ್ಘ override_sysinfo_uptime INIT(= -1);
EXTERN ಪೂರ್ಣಾಂಕ  override_autoload INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  override_defcompile INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  ml_get_alloc_ಸಾಲುಗಳು INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  ignore_unreachable_ಸಂಕೇತ_for_testing INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ  ಒಳ_free_unref_ವಸ್ತುಗಳು INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
EXTERN ಪೂರ್ಣಾಂಕ  did_ಕೂಡಿಸು_timer INIT(= FALSE);
EXTERN ಪೂರ್ಣಾಂಕ  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಪೂರ್ಣಾಂಕ  ಒಳಹರಿವು_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_ಬೇರು_T	*lval_ಬೇರು INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
EXTERN ಪೂರ್ಣಾಂಕ  bevalexpr_due_ಗಣ INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಸಮಯ_T ಸಮಯ_for_testing INIT(= 0);

EXTERN ಪೂರ್ಣಾಂಕ echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN ಪೂರ್ಣಾಂಕ  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN ಪೂರ್ಣಾಂಕ *eval_lavars_ಬಳಸಿದ INIT(= ಖಾಲಿ);

// Only filled for Win32.
EXTERN ಅಕ್ಷರ windows_ಆವೃತ್ತಿ[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_ಅಂಶ;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, GA_ಖಾಲಿಗಳು, GA_ಖಾಲಿಗಳು, ಖಾಲಿ,
			 {0, 0, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(char_u *), 20, ಖಾಲಿ}, 0, ಖಾಲಿ, ಖಾಲಿ}
# endif
	;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
EXTERN ಪೂರ್ಣಾಂಕ ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
EXTERN ಅಕ್ಷರ *ch_part_ಹೆಸರುಗಳು[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN ಪೂರ್ಣಾಂಕ channel_need_redraw INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN ಪೂರ್ಣಾಂಕ ch_log_output INIT(= FALSE);

EXTERN ಪೂರ್ಣಾಂಕ did_repeated_ಸಂದೇಶ INIT(= 0);
# define REPEATED_ಸಂದೇಶ_LOOKING	    1
# define REPEATED_ಸಂದೇಶ_SAFESTATE	    2
#ಆದರೆ_ಅಂತ್ಯ

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_ಗಣ);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN ಪೂರ್ಣಾಂಕ skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN ಪೂರ್ಣಾಂಕ skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN ಪೂರ್ಣಾಂಕ skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#ವ್ಯಾಖ್ಯಾನಿಸು SHOWCMD_BUFLEN (SHOWCMD_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
EXTERN ಪೂರ್ಣಾಂಕ	p_tgc_ಗಣ INIT(= FALSE);
#ಆದರೆ_ಅಂತ್ಯ

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN ಅಕ್ಷರ *wayland_display_name INIT(= ಖಾಲಿ);

// Don't connect to Wayland compositor if TRUE
EXTERN ಪೂರ್ಣಾಂಕ wayland_no_connect INIT(= FALSE);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CLIENTSERVER_ವಿಧಾನ_NONE,
# ifdef FEAT_X11
    CLIENTSERVER_ವಿಧಾನ_X11,
# endif
# ifdef MSWIN
    CLIENTSERVER_ವಿಧಾನ_MSWIN,
# endif
# ifdef FEAT_SOCKETSERVER
    CLIENTSERVER_ವಿಧಾನ_SOCKET
# endif
} clientserver_ವಿಧಾನ_T;

EXTERN clientserver_ವಿಧಾನ_T clientserver_ವಿಧಾನ

# ifdef FEAT_X11
INIT(= CLIENTSERVER_ವಿಧಾನ_X11);
# elif defined(MSWIN)
INIT(= CLIENTSERVER_ವಿಧಾನ_MSWIN);
# elif defined(FEAT_SOCKETSERVER)
INIT(= CLIENTSERVER_ವಿಧಾನ_SOCKET);
# ಇಲ್ಲದಿದ್ದರೆ
INIT(= CLIENTSERVER_ವಿಧಾನ_NONE);
# endif

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

// If the <xOSC> key should be propagated from vgetc()
EXTERN ಪೂರ್ಣಾಂಕ allow_osc_ಕೀಲಿ INIT(= 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= ಖಾಲಿ);
EXTERN bool inside_redraw_on_ಆರಂಭ_cb INIT(= false);
#ಆದರೆ_ಅಂತ್ಯ

// If greater than zero, then silence the W23/W24 warning.
EXTERN ಪೂರ್ಣಾಂಕ silence_w23_w24_ಸಂದೇಶ INIT( = 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T ಕೊನೆಯ_ಸೇರಿಸು_ga INIT5(0, 0, 1, 64, ಖಾಲಿ);
EXTERN ಪೂರ್ಣಾಂಕ	ಕೂಡಿಸು_ಕೊನೆಯ_ಸೇರಿಸು INIT(= 0);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN ಪೂರ್ಣಾಂಕ cell_ಅಗಲ INIT(= 8);
EXTERN ಪೂರ್ಣಾಂಕ cell_ಎತ್ತರ INIT(= 16);
#ಆದರೆ_ಅಂತ್ಯ
