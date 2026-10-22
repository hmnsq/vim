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
EXTERN ਲੰਮਾ	ਪੰਕਤੀਆਂ			// nr of rows in the screen
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DO_INIT
# ਜੇ defined(MSWIN)
		    = 25L
# ਨਹੀਂਤਾਂ
		    = 24L
# endif
#ਜੇ_ਅੰਤ
		    ;
EXTERN ਲੰਮਾ	Columns INIT(= 80);	// nr of columns in the screen

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
EXTERN schar_T	*Screen_ਸਤਰਾਂ INIT(= ਨਿਰਾ);
EXTERN sattr_T	*ScreenAttrs INIT(= ਨਿਰਾ);
EXTERN colnr_T  *Screen_ਖਾਨੇ_ਗਿਣਤੀ INIT(= ਨਿਰਾ);
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	*ਸਤਰ_ਵਿੱਥ INIT(= ਨਿਰਾ);
EXTERN char_u	*ਸਤਰ_Wraps INIT(= ਨਿਰਾ);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*Screen_ਸਤਰਾਂ_UC INIT(= ਨਿਰਾ);	// decoded UTF-8 characters
EXTERN u8char_T	*Screen_ਸਤਰਾਂ_C[MAX_MCO];		// composing characters
EXTERN ਪੂਰਨ	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= ਨਿਰਾ);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_Screen_ਸਤਰ INIT(= ਨਿਰਾ);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN ਪੂਰਨ	screen_cur_ਪੰਕਤੀ INIT(= 0);
EXTERN ਪੂਰਨ	screen_cur_ਖਾਨਾ INIT(= 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
// used for 'hlsearch' highlight matching
EXTERN ਮੇਲ_T	screen_ਖੋਜ_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T ਖੋਜ_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN ਪੂਰਨ	no_hlsearch INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#ਜੇ_ਅੰਤ

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN ਪੂਰਨ redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN ਛੋਟਾ	*Tab_ਪੰਨਾ_Idxs INIT(= ਨਿਰਾ);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= ਨਿਰਾ);
EXTERN ਪੂਰਨ	tabline_stl_click_ਗਿਣਤੀ INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= ਨਿਰਾ);
EXTERN ਪੂਰਨ	tabpanel_stl_click_ਗਿਣਤੀ INIT(= 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN ਛੋਟਾ	*popup_ਨਕਾਬ INIT(= ਨਿਰਾ);
EXTERN ਛੋਟਾ	*popup_ਨਕਾਬ_ਅਗਲਾ INIT(= ਨਿਰਾ);
// Array with flags for transparent cells of current popup.
EXTERN ਅੱਖਰ	*popup_transparent INIT(= ਨਿਰਾ);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN ਪੂਰਨ	popup_ਨਕਾਬ_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_ਨਕਾਬ_tab INIT(= ਨਿਰਾ);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN ਪੂਰਨ	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN ਲੰਮਾ	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN ਪੂਰਨ	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= ਨਿਰਾ);
EXTERN schar_T	*pum_bg_ਸਤਰਾਂ INIT(= ਨਿਰਾ);
EXTERN u8char_T	*pum_bg_ਸਤਰਾਂ_UC INIT(= ਨਿਰਾ);
EXTERN u8char_T	*pum_bg_ਸਤਰਾਂ_C[MAX_MCO];
EXTERN ਪੂਰਨ	pum_bg_ਸਿਖਰ INIT(= 0);
EXTERN ਪੂਰਨ	pum_bg_bot INIT(= 0);
EXTERN ਪੂਰਨ	pum_bg_ਖਾਨੇ_ਗਿਣਤੀ INIT(= 0);

EXTERN ਪੂਰਨ	screen_ਪੰਕਤੀਆਂ INIT(= 0);	    // actual size of ScreenLines[]
EXTERN ਪੂਰਨ	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN ਪੂਰਨ	mod_ਨਕਾਬ INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN ਪੂਰਨ	vgetc_mod_ਨਕਾਬ INIT(= 0);
EXTERN ਪੂਰਨ	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN ਪੂਰਨ	cmdline_ਪੰਕਤੀ;
EXTERN ਪੂਰਨ	cmdline_ਖਾਨਾ_off;
EXTERN ਪੂਰਨ	cmdline_ਚੌੜਾਈ;

EXTERN ਪੂਰਨ	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN ਪੂਰਨ	redraw_ਬਹੁਲਕ INIT(= FALSE);	// mode must be redrawn
EXTERN ਪੂਰਨ	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN ਪੂਰਨ	ਬਹੁਲਕ_displayed INIT(= FALSE);	// mode is being displayed
EXTERN ਪੂਰਨ	no_win_do_ਸਤਰਾਂ_ins INIT(= FALSE); // don't insert lines
#ਜੇ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN ਪੂਰਨ	cmdline_ਤਾਰਾ INIT(= FALSE);	// cmdline is crypted
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	exec_from_reg INIT(= FALSE);	// executing register

EXTERN ਪੂਰਨ	screen_cleared INIT(= FALSE);	// screen has been cleared

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

EXTERN char_u	*edit_submode INIT(= ਨਿਰਾ); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= ਨਿਰਾ); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= ਨਿਰਾ);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
EXTERN ਪੂਰਨ	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_ਖਾਨਾ;
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_ਪੰਕਤੀ;
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_ਸੁਨੇਹਾ INIT(= ਨਿਰਾ);	    // msg to be shown after redraw
EXTERN ਪੂਰਨ	keep_ਸੁਨੇਹਾ_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN ਪੂਰਨ	keep_ਸੁਨੇਹਾ_ਹੋਰ INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN ਪੂਰਨ	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN ਪੂਰਨ	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN ਪੂਰਨ	info_ਸੁਨੇਹਾ INIT(= FALSE); // printing informative message
EXTERN ਪੂਰਨ      ਸੁਨੇਹਾ_hist_off INIT(= FALSE); // don't add messages to history
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN ਪੂਰਨ	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN ਪੂਰਨ	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_ਸੁਨੇਹਾ INIT(= ਨਿਰਾ);
EXTERN ਲੰਮਾ	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= ਨਿਰਾ);

EXTERN ਪੂਰਨ	did_endif INIT(= FALSE);    // just had ":endif"
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN ਪੂਰਨ	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN ਪੂਰਨ	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN ਪੂਰਨ	called_vim_beep;	    // set if vim_beep() is called
EXTERN ਪੂਰਨ	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN ਪੂਰਨ	called_emsg;		    // always incremented by emsg()
EXTERN ਪੂਰਨ	ਵਿੱਚ_echowindow;		    // executing ":echowindow"
EXTERN ਪੂਰਨ	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN ਪੂਰਨ	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN ਪੂਰਨ	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN ਪੂਰਨ	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN ਪੂਰਨ	need_wait_return INIT(= 0); // need to wait for return later
EXTERN ਪੂਰਨ	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN ਪੂਰਨ	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN ਪੂਰਨ	quit_ਹੋਰ INIT(= FALSE);    // 'q' hit at "--more--" msg
#ਜੇ defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN ਪੂਰਨ	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN ਪੂਰਨ	intr_char INIT(= 0);	    // extra interrupt character
#ਜੇ_ਅੰਤ
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN ਪੂਰਨ	x_no_connect INIT(= FALSE); // don't connect to X server
# ਜੇ defined(FEAT_CLIENTSERVER)
EXTERN ਪੂਰਨ	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN ਪੂਰਨ	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN ਪੂਰਨ	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN ਪੂਰਨ	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN ਪੂਰਨ	ਸਤਰਾਂ_ਖੱਬਾ INIT(= -1);	    // lines left for listing
EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_no_ਹੋਰ INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, ਆਕਾਰ(estack_T), 50, ਨਿਰਾ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SOURCING_INFO  (exestack.ga_ਅੰਕੜੇ != ਨਿਰਾ && exestack.ga_len > 0)
// name of error message source
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOURCING_NAME (((estack_T *)exestack.ga_ਅੰਕੜੇ)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOURCING_LNUM (((estack_T *)exestack.ga_ਅੰਕੜੇ)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    INIT4(0, 0, 0, 0);
#ਨਹੀਂਤਾਂ
    INIT(= {0});
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// whether inside compile_def_function()
EXTERN ਪੂਰਨ	estack_compiling INIT(= FALSE);

EXTERN ਪੂਰਨ	ex_nesting_level INIT(= 0);	// nesting level
EXTERN ਪੂਰਨ	debug_break_level INIT(= -1);	// break below this level
EXTERN ਪੂਰਨ	debug_did_ਸੁਨੇਹਾ INIT(= FALSE);	// did "debug mode" message
EXTERN ਪੂਰਨ	debug_tick INIT(= 0);		// breakpoint change count
EXTERN ਪੂਰਨ	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN ਪੂਰਨ	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_ਚੀਜ਼ਾਂ INIT5(0, 0, ਆਕਾਰ(scriptitem_T *), 20, ਨਿਰਾ);
# define SCRIPT_ਵਸਤੂ(id)    (((scriptitem_T **)script_ਚੀਜ਼ਾਂ.ga_ਅੰਕੜੇ)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_ਚੀਜ਼ਾਂ.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_ਵਸਤੂ(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ))[j]

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
EXTERN ਪੂਰਨ did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN ਪੂਰਨ need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN ਪੂਰਨ check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN ਪੂਰਨ trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN ਪੂਰਨ force_abort INIT(= FALSE);

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
EXTERN msglist_T **ਸੁਨੇਹਾ_list INIT(= ਨਿਰਾ);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN ਪੂਰਨ suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_ਢੇਰ INIT(= ਨਿਰਾ);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN ਪੂਰਨ	may_garbage_collect INIT(= FALSE);
EXTERN ਪੂਰਨ	want_garbage_collect INIT(= FALSE);
EXTERN ਪੂਰਨ	garbage_collect_at_exit INIT(= FALSE);


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

# define t_ਸੰਖਿਆ		(static_types[12])
# define t_const_ਸੰਖਿਆ		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_ਸੰਖਿਆ_bool		(static_types[14])
# define t_const_ਸੰਖਿਆ_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_ਸੰਖਿਆ_float		(static_types[16])
# define t_const_ਸੰਖਿਆ_float	(static_types[17])

# define t_float			(static_types[18])
# define t_const_float		(static_types[19])

# define t_string		(static_types[20])
# define t_const_string		(static_types[21])

# define t_blob			(static_types[22])
# define t_const_blob		(static_types[23])

# define t_blob_null		(static_types[24])
# define t_const_blob_null	(static_types[25])

# define t_ਕੰਮ			(static_types[26])
# define t_const_ਕੰਮ		(static_types[27])

# define t_channel		(static_types[28])
# define t_const_channel		(static_types[29])

// t_number_or_string - Special value used for @#.
# define t_ਸੰਖਿਆ_or_string	(static_types[30])
# define t_const_ਸੰਖਿਆ_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_ਸੰਖਿਆ		(static_types[38])
# define t_const_func_ਸੰਖਿਆ	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_ਸੰਖਿਆ		(static_types[48])
# define t_const_func_0_ਸੰਖਿਆ	(static_types[49])

# define t_func_0_string		(static_types[50])
# define t_const_func_0_string	(static_types[51])

# define t_list_any		(static_types[52])
# define t_const_list_any	(static_types[53])

# define t_dict_any		(static_types[54])
# define t_const_dict_any	(static_types[55])

# define t_list_ਖਾਲੀ_ਥਾਵਾਂ		(static_types[56])
# define t_const_list_ਖਾਲੀ_ਥਾਵਾਂ	(static_types[57])

# define t_dict_ਖਾਲੀ_ਥਾਵਾਂ		(static_types[58])
# define t_const_dict_ਖਾਲੀ_ਥਾਵਾਂ	(static_types[59])

# define t_list_bool		(static_types[60])
# define t_const_list_bool	(static_types[61])

# define t_list_ਸੰਖਿਆ		(static_types[62])
# define t_const_list_ਸੰਖਿਆ	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_ਕੰਮ		(static_types[66])
# define t_const_list_ਕੰਮ	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_ਸੰਖਿਆ	(static_types[72])
# define t_const_list_list_ਸੰਖਿਆ (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_ਸੰਖਿਆ	(static_types[76])
# define t_const_list_list_list_ਸੰਖਿਆ (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_ਸੰਖਿਆ		(static_types[80])
# define t_const_dict_ਸੰਖਿਆ	(static_types[81])

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

# define t_tuple_ਖਾਲੀ_ਥਾਵਾਂ		(static_types[94])
# define t_const_tuple_ਖਾਲੀ_ਥਾਵਾਂ	(static_types[95])


EXTERN type_T static_types[96]
# ifdef DO_INIT
= {
    // 0: t_unknown
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 12: t_number
    {VAR_ਸੰਖਿਆ, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_ਸੰਖਿਆ, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 14: t_number_bool
    {VAR_ਸੰਖਿਆ, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_ਸੰਖਿਆ, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 16: t_number_float
    {VAR_ਸੰਖਿਆ, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_ਸੰਖਿਆ, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ਨਿਰਾ, ਨਿਰਾ},

    // 26: t_job
    {VAR_ਕੰਮ, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_ਕੰਮ, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ਨਿਰਾ, ਨਿਰਾ},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ਨਿਰਾ, ਨਿਰਾ},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ਨਿਰਾ, ਨਿਰਾ},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ਨਿਰਾ, ਨਿਰਾ},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ਨਿਰਾ, ਨਿਰਾ},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ਨਿਰਾ, ਨਿਰਾ},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ਨਿਰਾ, ਨਿਰਾ},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ਨਿਰਾ, ਨਿਰਾ},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ਨਿਰਾ, ਨਿਰਾ},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ਨਿਰਾ, ਨਿਰਾ},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ਨਿਰਾ, ਨਿਰਾ},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ਨਿਰਾ, ਨਿਰਾ},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ਨਿਰਾ, ਨਿਰਾ},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ਨਿਰਾ, ਨਿਰਾ},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_ਕੰਮ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ਕੰਮ, ਨਿਰਾ, ਨਿਰਾ},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, ਨਿਰਾ, ਨਿਰਾ},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, ਨਿਰਾ, ਨਿਰਾ},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, ਨਿਰਾ, ਨਿਰਾ},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ਨਿਰਾ, ਨਿਰਾ},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਨਿਰਾ},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ਨਿਰਾ, ਨਿਰਾ},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ਨਿਰਾ, ਨਿਰਾ},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ},
}
# endif
;

EXTERN ਪੂਰਨ	did_ਸਰੋਤ_packages INIT(= FALSE);
#ਜੇ_ਅੰਤ // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN ਪੂਰਨ	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN ਪੂਰਨ	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN ਪੂਰਨ include_none INIT(= 0);	// when 1 include "None"
EXTERN ਪੂਰਨ include_default INIT(= 0);	// when 1 include "default"
EXTERN ਪੂਰਨ include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN ਪੂਰਨ	highlight_ਮੇਲ INIT(= FALSE);	// show search match pos
EXTERN linenr_T	ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ;		// lines of matched string
EXTERN colnr_T	ਖੋਜ_ਮੇਲ_endcol;		// col nr of match end
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
EXTERN linenr_T	ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	ਖੋਜ_ਆਖਰੀ_ਸਤਰ INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN ਪੂਰਨ	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN ਪੂਰਨ	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN ਪੂਰਨ	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN ਪੂਰਨ	highlight_attr[HLF_ਗਿਣਤੀ];  // Highl. attr for each context.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
# define USER_HIGHLIGHT
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USER_HIGHLIGHT
EXTERN ਪੂਰਨ	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN ਪੂਰਨ	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN ਪੂਰਨ	highlight_stlterm[9];		// On top of user
EXTERN ਪੂਰਨ	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN ਪੂਰਨ	skip_ਪਦ_loop INIT(= FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= ਨਿਰਾ);	// "-U" cmdline argument
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	cterm_ਸਾਧਾਰਨ_fg_color INIT(= 0);
EXTERN ਪੂਰਨ	cterm_ਸਾਧਾਰਨ_fg_bold INIT(= 0);
EXTERN ਪੂਰਨ	cterm_ਸਾਧਾਰਨ_bg_color INIT(= 0);
EXTERN ਪੂਰਨ	cterm_ਸਾਧਾਰਨ_ul_color INIT(= 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_ਸਾਧਾਰਨ_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_ਸਾਧਾਰਨ_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_ਸਾਧਾਰਨ_ul_gui_color INIT(= INVALCOLOR);
#ਜੇ_ਅੰਤ
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
EXTERN ਪੂਰਨ	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN ਪੂਰਨ	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN ਪੂਰਨ	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN ਪੂਰਨ	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(ਨਿਰਾ, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= ਨਿਰਾ);
EXTERN win_T	*au_pending_free_win INIT(= ਨਿਰਾ);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN ਪੂਰਨ	mouse_ਪੰਕਤੀ;
EXTERN ਪੂਰਨ	mouse_ਖਾਨਾ;
EXTERN ਪੂਰਨ	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN ਪੂਰਨ	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN ਪੂਰਨ	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#ਜੇ defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN ਪੂਰਨ	WantQueryMouse INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN ਪੂਰਨ	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_prev_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN ਪੂਰਨ	gui_prev_topfill INIT(= 0);
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
EXTERN ਪੂਰਨ	drag_ਸਥਿਤੀ_ਸਤਰ INIT(= FALSE);	// dragging the status line
EXTERN ਪੂਰਨ	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN ਪੂਰਨ	drag_sep_ਸਤਰ INIT(= FALSE);	// dragging vert separator
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
// Value set from 'diffopt'.
EXTERN ਪੂਰਨ	diff_context INIT(= 6);		// context for folds
EXTERN ਪੂਰਨ      linematch_ਸਤਰਾਂ INIT(= 0);      // number of lines for diff line match
EXTERN ਪੂਰਨ	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN ਪੂਰਨ	diff_need_scrollbind INIT(= FALSE);
#ਜੇ_ਅੰਤ

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN ਪੂਰਨ	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN ਪੂਰਨ	redraw_not_allowed INIT(= FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN ਪੂਰਨ	dont_parse_messages INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*ਜੜ੍ਹ_menu INIT(= ਨਿਰਾ);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN ਪੂਰਨ	sys_menu INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN ਪੂਰਨ force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN ਪੂਰਨ	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN ਪੂਰਨ	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN ਪੂਰਨ	current_scrollbar;
EXTERN long_u	scrollbar_ਮੁੱਲ;

// found "-rv" or "-reverse" in command line args
EXTERN ਪੂਰਨ	ਮਿਲਿਆ_ਉਲਟ_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN ਅੱਖਰ	*font_ਦਲੀਲ INIT(= ਨਿਰਾ);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN ਅੱਖਰ	*background_ਦਲੀਲ INIT(= ਨਿਰਾ);

// "-fg" or "-foreground" command line argument
EXTERN ਅੱਖਰ	*foreground_ਦਲੀਲ INIT(= ਨਿਰਾ);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN ਅਸਥਿਰ sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN ਪੂਰਨ	new_pixel_ਚੌੜਾਈ INIT(= 0);
EXTERN ਪੂਰਨ	new_pixel_ਉਚਾਈ INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN ਪੂਰਨ	gui_win_x INIT(= -1);
EXTERN ਪੂਰਨ	gui_win_y INIT(= -1);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
EXTERN Clipboard_T clip_ਤਾਰਾ;	// PRIMARY selection in X11/Wayland
# ਜੇ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# ਨਹੀਂਤਾਂ
#  define clip_plus clip_ਤਾਰਾ	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN ਪੂਰਨ	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN ਪੂਰਨ	clip_autoselect_ਤਾਰਾ INIT(= FALSE);
EXTERN ਪੂਰਨ	clip_autoselect_plus INIT(= FALSE);
EXTERN ਪੂਰਨ	clip_autoselectml INIT(= FALSE);
EXTERN ਪੂਰਨ	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= ਨਿਰਾ);
EXTERN ਪੂਰਨ	clip_unnamed_saved INIT(= 0);
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= ਨਿਰਾ);	// previous window (may equal curwin)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ONE_ਖਿੜਕੀ (firstwin == lastwin)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ W_ਅਗਲਾ(wp) ((wp)->w_ਅਗਲਾ)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUCMD_WIN_ਗਿਣਤੀ 5

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    ਪੂਰਨ		auc_win_ਵਰਤਿਆ;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_ਗਿਣਤੀ];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
EXTERN win_T    *ਪਹਿਲਾ_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= ਨਿਰਾ);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN ਪੂਰਨ	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN ਪੂਰਨ	popup_ਵਰਤਦਾ_mouse_move INIT(= FALSE);

EXTERN ਪੂਰਨ	ਪਾਠ_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN ਪੂਰਨ	ignore_ਪਾਠ_props INIT(= FALSE);
#ਜੇ_ਅੰਤ

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN ਪੂਰਨ	pum_will_redraw INIT(= FALSE);

/*
 * The window layout is kept in a tree of frames.  topframe points to the top
 * of the tree.
 */
EXTERN ਚੌਖਟਾ_T	*topframe;	// top of the window frame tree

/*
 * Tab pages are alternative topframes.  "first_tabpage" points to the first
 * one in the list, "curtab" is the current one. "lastused_tabpage" is the
 * last used one.
 */
EXTERN tabpage_T    *ਪਹਿਲਾ_tabpage;
EXTERN tabpage_T    *curtab;
EXTERN tabpage_T    *lastused_tabpage;
EXTERN ਪੂਰਨ	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN ਪੂਰਨ	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#ਜੇ defined(FEAT_TABPANEL)
EXTERN ਪੂਰਨ	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#ਜੇ_ਅੰਤ

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= ਨਿਰਾ);	// first buffer
EXTERN buf_T	*lastbuf INIT(= ਨਿਰਾ);	// last buffer
EXTERN buf_T	*curbuf INIT(= ਨਿਰਾ);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN ਪੂਰਨ	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN ਪੂਰਨ	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN ਪੂਰਨ	arg_had_ਆਖਰੀ INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN ਪੂਰਨ	ru_ਖਾਨਾ;		// column for ruler
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
EXTERN ਪੂਰਨ	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	sc_ਖਾਨਾ;		// column for shown command

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TEMPDIRNAMES
# ਜੇ defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= ਨਿਰਾ); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= ਨਿਰਾ); // Name of Vim's own temp dir.
					   // Ends in a slash.
#ਜੇ_ਅੰਤ

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN ਪੂਰਨ	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN ਪੂਰਨ	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN ਪੂਰਨ	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN ਪੂਰਨ	v_dying INIT(= 0); // internal value of v:dying
EXTERN ਪੂਰਨ	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#ਜੇ defined(FEAT_AUTOCHDIR)
EXTERN ਪੂਰਨ	test_autochdir INIT(= FALSE);
#ਜੇ_ਅੰਤ
EXTERN ਅੱਖਰ	*ਆਖਰੀ_chdir_reason INIT(= ਨਿਰਾ);
#ਜੇ defined(EXITFREE)
EXTERN ਪੂਰਨ	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#ਜੇ_ਅੰਤ
// volatile because it is used in signal handler deathtrap().
EXTERN ਅਸਥਿਰ sig_atomic_t ਭਰਿਆ_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN ਪੂਰਨ	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN ਪੂਰਨ	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN ਪੂਰਨ	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN ਪੂਰਨ	curbuf_ਤਾਲਾ INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN ਪੂਰਨ	allbuf_ਤਾਲਾ INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
EXTERN ਪੂਰਨ	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	silent_ਬਹੁਲਕ INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
EXTERN ਪੂਰਨ	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#ਜੇ_ਅੰਤ

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN ਪੂਰਨ	VIsual_ਸਰਗਰਮ INIT(= FALSE);
				// whether Visual mode is active
EXTERN ਪੂਰਨ	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN ਪੂਰਨ	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN ਪੂਰਨ  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN ਪੂਰਨ	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN ਪੂਰਨ	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN ਪੂਰਨ	VIsual_ਬਹੁਲਕ INIT(= 'v');
				// type of Visual mode

EXTERN ਪੂਰਨ	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN ਪੂਰਨ	resel_VIsual_ਬਹੁਲਕ INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_ਸਤਰ_ਗਿਣਤੀ;	// number of lines
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
EXTERN ਪੂਰਨ     did_ai INIT(= FALSE);

/*
 * Column of first char after autoindent.  0 when no autoindent done.  Used
 * when 'backspace' is 0, to avoid backspacing over autoindent.
 */
EXTERN colnr_T	ai_ਖਾਨਾ INIT(= 0);

/*
 * This is a character which will end a start-middle-end comment when typed as
 * the first character on a new line.  It is taken from the last character of
 * the "end" comment leader when the COM_AUTO_END flag is given for that
 * comment end in 'comments'.  It is only valid when did_ai is TRUE.
 */
EXTERN ਪੂਰਨ     ਸਿਰਾ_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN ਪੂਰਨ     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN ਪੂਰਨ	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN ਪੂਰਨ	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN ਪੂਰਨ	can_si_ਪਿੱਠ INIT(= FALSE);

EXTERN ਪੂਰਨ	old_indent INIT(= 0);	// for ^^D command in insert mode

EXTERN pos_T	saved_cursor		// w_cursor before formatting text.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DO_INIT
		    = {0, 0, 0}
#ਜੇ_ਅੰਤ
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
EXTERN ਪੂਰਨ	orig_ਸਤਰ_ਗਿਣਤੀ INIT(= 0);  // Line count when "gR" started
EXTERN ਪੂਰਨ	vr_ਸਤਰਾਂ_ਬਦਲਿਆ INIT(= 0); // #Lines changed by "gR" so far

#ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
// argument to SETJMP() for handling X IO errors
EXTERN JMP_BUF x_jump_env;
#ਜੇ_ਅੰਤ

/*
 * These flags are set based upon 'fileencoding'.
 * Note that "enc_utf8" is also set for "unicode", because the characters are
 * internally stored as UTF-8 (to avoid trouble with NUL bytes).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_JPN	932	// japan
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_JPNU	9932	// euc-jp
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_KOR	949	// korea
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_KORU	9949	// euc-kr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_CHS	936	// chinese
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_CHSU	9936	// euc-cn
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_CHT	950	// taiwan
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_CHTU	9950	// euc-tw
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_2BYTE	1	// 2byte-
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBCS_DEBUG	(-1)

EXTERN ਪੂਰਨ	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN ਪੂਰਨ	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN ਪੂਰਨ	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN ਪੂਰਨ	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#ਜੇ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN ਪੂਰਨ	enc_codepage INIT(= -1);
EXTERN ਪੂਰਨ	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN ਅੱਖਰ	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T ਨਿਵੇਸ਼_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN ਪੂਰਨ (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN ਪੂਰਨ (*mb_ptr2len_len)(char_u *p, ਪੂਰਨ ਮਾਪ) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN ਪੂਰਨ (*mb_char2len)(ਪੂਰਨ c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN ਪੂਰਨ (*mb_char2bytes)(ਪੂਰਨ c, char_u *buf) INIT(= latin_char2bytes);

EXTERN ਪੂਰਨ (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN ਪੂਰਨ (*mb_ptr2cells_len)(char_u *p, ਪੂਰਨ ਮਾਪ) INIT(= latin_ptr2cells_len);
EXTERN ਪੂਰਨ (*mb_char2cells)(ਪੂਰਨ c) INIT(= latin_char2cells);
EXTERN ਪੂਰਨ (*mb_off2cells)(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਬਿਨਾਨਿਸ਼ਾਨੀ max_off) INIT(= latin_off2cells);
EXTERN ਪੂਰਨ (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN ਪੂਰਨ (*mb_ਸਿਰ_off)(char_u *ਅਧਾਰ, char_u *p) INIT(= latin_ਸਿਰ_off);

#ਜੇ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, ਅਚਲ ਅੱਖਰ **inbuf, size_t *inbytesleft, ਅੱਖਰ **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (ਅਚਲ ਅੱਖਰ *tocode, ਅਚਲ ਅੱਖਰ *fromcode);
EXTERN ਪੂਰਨ (*iconv_close) (iconv_t cd);
EXTERN ਪੂਰਨ (*iconvctl) (iconv_t cd, ਪੂਰਨ request, ਖਾਲੀ *ਦਲੀਲ);
EXTERN ਪੂਰਨ* (*iconv_errno) (ਖਾਲੀ);
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= ਨਿਰਾ);
/*
 * Start and end column of the preedit area in virtual columns from the start
 * of the text line.  When there is no preedit area they are set to MAXCOL.
 * "preedit_end_col" is needed for coloring the preedited string.  Drawing the
 * color between "preedit_start_col" and curpos did not work, because some XIM
 * set the cursor position to the first char of the string.
 */
EXTERN colnr_T		preedit_ਸ਼ੁਰੂ_ਖਾਨਾ INIT(= MAXCOL);
EXTERN colnr_T		preedit_ਸਿਰਾ_ਖਾਨਾ INIT(= MAXCOL);

// "xim_changed_while_preediting" is set when changed() can set the 'modified'
// flag even while preediting.
EXTERN ਪੂਰਨ		xim_ਬਦਲਿਆ_while_preediting INIT(= FALSE);
# ਨਹੀਂਤਾਂ
EXTERN XIC		xic INIT(= ਨਿਰਾ);
# endif
# ifdef FEAT_GUI
EXTERN guicolor_T	xim_fg_color INIT(= INVALCOLOR);
EXTERN guicolor_T	xim_bg_color INIT(= INVALCOLOR);
# endif
#ਜੇ_ਅੰਤ

/*
 * "State" is the main state of Vim.
 * There are other variables that modify the state:
 * "Visual_mode"    When State is MODE_NORMAL or MODE_INSERT.
 * "finish_op"	    When State is MODE_NORMAL, after typing the operator and
 *		    before typing the motion command.
 * "motion_force"   Last motion_force  from do_pending_operator()
 * "debug_mode"	    Debug mode.
 */
EXTERN ਪੂਰਨ	State INIT(= ਬਹੁਲਕ_ਸਾਧਾਰਨ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	debug_ਬਹੁਲਕ INIT(= FALSE);
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	ਅੰਤ_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN ਲੰਮਾ	opcount INIT(= 0);	// count for pending operator
EXTERN ਪੂਰਨ	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN ਪੂਰਨ exmode_ਸਰਗਰਮ INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN ਪੂਰਨ pending_exmode_ਸਰਗਰਮ INIT(= FALSE);

EXTERN ਪੂਰਨ ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN ਪੂਰਨ reg_recording INIT(= 0);	// register for recording  or zero
EXTERN ਪੂਰਨ reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN ਪੂਰਨ pending_ਸਿਰਾ_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN ਪੂਰਨ ਵੇਖਿਆ_Modify_Other_ਕੁੰਜੀਆਂ INIT(= FALSE);

// The state for the modifyOtherKeys level
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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

EXTERN ਪੂਰਨ no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN ਪੂਰਨ no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN ਪੂਰਨ allow_ਕੁੰਜੀਆਂ INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN ਪੂਰਨ no_reduce_ਕੁੰਜੀਆਂ INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN ਪੂਰਨ no_u_sync INIT(= 0);		// Don't call u_sync()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN ਪੂਰਨ arrow_ਵਰਤਿਆ;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN ਪੂਰਨ	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN ਪੂਰਨ	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
EXTERN ਪੂਰਨ	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN ਪੂਰਨ	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN ਪੂਰਨ	sticky_cmdmod_flags INIT(= 0); // used by :execute

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	is_export INIT(= FALSE);    // :export {cmd}
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	ਸੁਨੇਹਾ_silent INIT(= 0);	// don't print messages
EXTERN ਪੂਰਨ	emsg_silent INIT(= 0);	// don't print error messages
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN ਪੂਰਨ	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN ਪੂਰਨ	ਵਿੱਚ_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN ਪੂਰਨ	ਅਦਲਾ_ਬਦਲੀ_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN ਪੂਰਨ	ਅਦਲਾ_ਬਦਲੀ_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN ਅੱਖਰ	ਸੁਨੇਹਾ_buf[ਸੁਨੇਹਾ_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN ਪੂਰਨ	RedrawingDisabled INIT(= 0);

EXTERN ਪੂਰਨ	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN ਪੂਰਨ	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DO_INIT
		    = {ਨਿਰਾ, ਨਿਰਾ, 0, 0, 0, 0, 0, 0, 0}
#ਜੇ_ਅੰਤ
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN ਪੂਰਨ	typebuf_was_ਖਾਲੀ_ਥਾਵਾਂ INIT(= FALSE);

EXTERN ਪੂਰਨ	ex_ਸਾਧਾਰਨ_busy INIT(= 0);   // recursiveness of ex_normal()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	ਵਿੱਚ_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	ex_ਸਾਧਾਰਨ_ਤਾਲਾ INIT(= 0);   // forbid use of ex_normal()

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ	ignore_script INIT(= FALSE);  // ignore script input
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	stop_ਪਾਓ_ਬਹੁਲਕ;	// for ":stopinsert" and 'insertmode'

EXTERN ਪੂਰਨ	ਕੁੰਜੀ_Typed;		// TRUE if user typed current char
EXTERN ਪੂਰਨ	ਕੁੰਜੀ_Stuffed;		// TRUE if current char from stuffbuf
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
EXTERN ਪੂਰਨ	vgetc_im_ਸਰਗਰਮ;	// Input Method was active for last
					// character obtained from vgetc()
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN ਪੂਰਨ	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN ਪੂਰਨ	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN ਪੂਰਨ	do_redraw INIT(= FALSE);    // extra redraw once
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
EXTERN ਪੂਰਨ	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN ਪੂਰਨ	redrawtime_ਹੱਦਬੰਦੀ_ਸੈੱਟ INIT(= FALSE);
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	need_highlight_ਬਦਲਿਆ INIT(= TRUE);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN ਪੂਰਨ	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= ਨਿਰਾ);   // stream to write script to
EXTERN ਪੂਰਨ	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN ਅਸਥਿਰ sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN ਅਸਥਿਰ sig_atomic_t got_sigusr1 INIT(= FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਪਦ_CONSOLE
EXTERN ਪੂਰਨ	ਪਦ_console INIT(= FALSE); // set to TRUE when console used
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	termcap_ਸਰਗਰਮ INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN ਪੂਰਨ	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN ਪੂਰਨ	searchcmdlen;		    // length of previous search cmd
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
EXTERN ਪੂਰਨ	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_ਵਿੱਚ INIT(= ਨਿਰਾ); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_ਬਾਹਰਲਾ INIT(= ਨਿਰਾ); // Set by vim_regexec()
					    // to store \z\(...\) matches
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ	did_outofmem_ਸੁਨੇਹਾ INIT(= FALSE);
					    // set after out of memory msg
EXTERN ਪੂਰਨ	did_swapwrite_ਸੁਨੇਹਾ INIT(= FALSE);
					    // set after swap write error msg
EXTERN ਪੂਰਨ	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN ਪੂਰਨ	global_busy INIT(= 0);	    // set when :global is executing
EXTERN ਪੂਰਨ	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN ਪੂਰਨ	need_ਸ਼ੁਰੂ_insertmode INIT(= FALSE);
					    // start insert mode soon
#ਜੇ defined(FEAT_EVAL)
EXTERN char_u	ਆਖਰੀ_ਬਹੁਲਕ[ਬਹੁਲਕ_MAX_ਲੰਮਾਈ] INIT(= "n"); // for ModeChanged event
#ਜੇ_ਅੰਤ
EXTERN char_u	*ਆਖਰੀ_cmdline INIT(= ਨਿਰਾ); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= ਨਿਰਾ); // command line for "."
EXTERN char_u	*new_ਆਖਰੀ_cmdline INIT(= ਨਿਰਾ);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= ਨਿਰਾ); // fname for <afile> on cmdline
EXTERN ਪੂਰਨ	autocmd_fname_ਭਰਿਆ;	     // autocmd_fname is full path
EXTERN ਪੂਰਨ	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_ਮੇਲ INIT(= ਨਿਰਾ); // name for <amatch> on cmdline
EXTERN ਪੂਰਨ	aucmd_cmdline_ਬਦਲਿਆ_ਗਿਣਤੀ INIT(= 0);

EXTERN ਪੂਰਨ	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	ਆਖਰੀ_cursormoved	      // for CursorMoved event
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DO_INIT
		    = {0, 0, 0}
#ਜੇ_ਅੰਤ
		    ;

EXTERN ਪੂਰਨ	postponed_ਵੰਡੋ INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN ਪੂਰਨ	postponed_ਵੰਡੋ_flags INIT(= 0);  // args for win_split()
EXTERN ਪੂਰਨ	postponed_ਵੰਡੋ_tab INIT(= 0);  // cmdmod.cmod_tab
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
EXTERN ਪੂਰਨ	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	g_ਟੈਗ_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN ਪੂਰਨ	replace_ਵਿੱਥ INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN ਪੂਰਨ	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*ਖਾਲੀ_ਥਾਵਾਂ_option INIT(= (char_u *)"");

EXTERN ਪੂਰਨ  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= ਨਿਰਾ);	// message redirection file
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ  redir_reg INIT(= 0);	// message redirection register
EXTERN ਪੂਰਨ  redir_vname INIT(= 0);	// message redirection variable
EXTERN ਪੂਰਨ  redir_execute INIT(= 0);	// execute() redirection
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#ਜੇ_ਅੰਤ

EXTERN ਪੂਰਨ  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN ਪੂਰਨ  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN ਪੂਰਨ  wild_menu_showing INIT(= 0);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WM_SHOWN	1		// wildmenu showing
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WM_SCROLLED	2		// wildmenu showing with scroll

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN ਪੂਰਨ	ਮਿਲਿਆ_ਰਜਿਸਟਰ_arg INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
EXTERN ਅੱਖਰ	breakat_flags[256];	// which characters are in 'breakat'
#ਜੇ_ਅੰਤ

// These are in version.c, call init_longVersion() before use.
ਬਾਹਰੀ ਅੱਖਰ *ਸੰਸਕਰਣ;
#ਜੇ defined(HAVE_DATE_ਸਮਾਂ) && defined(VMS) && defined(VAXC)
ਬਾਹਰੀ ਅੱਖਰ long_ਸੰਸਕਰਣ[];
#ਨਹੀਂਤਾਂ
ਬਾਹਰੀ ਅੱਖਰ *long_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PATHDEF
ਬਾਹਰੀ char_u *default_vim_dir;
ਬਾਹਰੀ char_u *default_vimruntime_dir;
ਬਾਹਰੀ char_u *all_cflags;
ਬਾਹਰੀ char_u *all_lflags;
# ifdef VMS
ਬਾਹਰੀ char_u *compiler_ਸੰਸਕਰਣ;
ਬਾਹਰੀ char_u *compiled_arch;
ਬਾਹਰੀ char_u *compiled_vers;
# endif
ਬਾਹਰੀ char_u *compiled_user;
ਬਾਹਰੀ char_u *compiled_sys;
#ਜੇ_ਅੰਤ

EXTERN char_u	*homedir INIT(= ਨਿਰਾ);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
EXTERN ਪੂਰਨ	disable_fold_update INIT(= 0);
#ਜੇ_ਅੰਤ

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN ਪੂਰਨ	km_stopsel INIT(= FALSE);
EXTERN ਪੂਰਨ	km_startsel INIT(= FALSE);

EXTERN ਪੂਰਨ	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN ਪੂਰਨ	cmdwin_ਨਤੀਜਾ INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= ਨਿਰਾ); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= ਨਿਰਾ); // window of cmdline window or NULL

EXTERN char_u no_ਸਤਰਾਂ_ਸੁਨੇਹਾ[]	INIT(= N_("--No lines in buffer--"));

EXTERN ਅੱਖਰ typename_unknown[]	INIT(= N_("unknown"));
EXTERN ਅੱਖਰ typename_int[]	INIT(= "int");
EXTERN ਅੱਖਰ typename_longint[]	INIT(= "long int");
EXTERN ਅੱਖਰ typename_longlongint[]	INIT(= "long long int");
EXTERN ਅੱਖਰ typename_unsignedint[]	INIT(= "unsigned int");
EXTERN ਅੱਖਰ typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN ਅੱਖਰ typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN ਅੱਖਰ typename_ਸੂਚਕ[]	INIT(= N_("pointer"));
EXTERN ਅੱਖਰ typename_percent[]	INIT(= N_("percent"));
EXTERN ਅੱਖਰ typename_char[] INIT(= "char");
EXTERN ਅੱਖਰ typename_string[]	INIT(= N_("string"));
EXTERN ਅੱਖਰ typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN ਲੰਮਾ	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN ਬਣਤਰ subs_ਵਿਅੰਜਕ_S	*substitute_instr INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#ਜੇ defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_ਵਿੱਚ_ICON	1
# define STL_ਵਿੱਚ_TITLE	2
EXTERN ਪੂਰਨ      stl_syntax INIT(= 0);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= ਨਿਰਾ);
EXTERN ਪੂਰਨ		balloon_Eval_For_ਪਦ INIT(= FALSE);
# ਜੇ defined(FEAT_NETBEANS_INTG)
EXTERN ਪੂਰਨ bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
// the table is in misc2.c, because of initializations
ਬਾਹਰੀ cursorentry_T shape_ਸਾਰਣੀ[SHAPE_IDX_ਗਿਣਤੀ];
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_ਸਿਖਰ		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_ਖੱਬਾ		2
# define OPT_PRINT_ਸੱਜਾ	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_ਸੰਖਿਆ	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_ਅੰਕ_OPTIONS	15

EXTERN option_ਸਾਰਣੀ_T printer_opts[OPT_PRINT_ਅੰਕ_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, ਨਿਰਾ, 0, FALSE},
	{"bottom",	TRUE, 0, ਨਿਰਾ, 0, FALSE},
	{"left",	TRUE, 0, ਨਿਰਾ, 0, FALSE},
	{"right",	TRUE, 0, ਨਿਰਾ, 0, FALSE},
	{"header",	TRUE, 0, ਨਿਰਾ, 0, FALSE},
	{"syntax",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"number",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"wrap",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"duplex",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"portrait", FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"paper",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"collate",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"jobsplit", FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"formfeed", FALSE, 0, ਨਿਰਾ, 0, FALSE},
	{"format", FALSE, 0, ਨਿਰਾ, 0, FALSE},
    }
# endif
    ;

// For prt_get_unit().
# define PRT_UNIT_NONE	-1
# define PRT_UNIT_PERC	0
# define PRT_UNIT_INCH	1
# define PRT_UNIT_MM	2
# define PRT_UNIT_ਬਿੰਦੂ	3
# define PRT_UNIT_ਨਾਂ {"pc", "in", "mm", "pt"}
#ਜੇ_ਅੰਤ

#ਜੇ (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_ਪੰਨਾ_ਅੰਕ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XCLIPBOARD
// xterm display name
EXTERN ਅੱਖਰ	*xterm_display INIT(= ਨਿਰਾ);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN ਪੂਰਨ	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)ਨਿਰਾ);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN ਪੂਰਨ	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN ਪੂਰਨ	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= ਨਿਰਾ);	// name of the server
# ifdef FEAT_X11
EXTERN ਖਿੜਕੀ	comm_ਖਿੜਕੀ INIT(= None);
EXTERN ਖਿੜਕੀ	client_ਖਿੜਕੀ INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*server_Delayed_ਸ਼ੁਰੂ_Name INIT(= ਨਿਰਾ);
# elif defined(MSWIN)
EXTERN HWND	client_ਖਿੜਕੀ INIT(= 0);
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(VMS)
EXTERN ਪੂਰਨ	ਪਦ_is_xterm INIT(= FALSE);	// xterm-like 'term'
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
EXTERN ਅੱਖਰ	psepc INIT(= '\\');	// normal path separator character
EXTERN ਅੱਖਰ	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN ਅੱਖਰ	pseps[2] INIT2('\\', 0);
#ਜੇ_ਅੰਤ

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN ਪੂਰਨ	virtual_op INIT(= MAYBE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
// Display tick, incremented for each call to update_screen()
EXTERN disptick_T	display_tick INIT(= 0);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
// Line in which spell checking wasn't highlighted because it touched the
// cursor position in Insert mode.
EXTERN linenr_T		spell_redraw_lnum INIT(= 0);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
// Set when the cursor line needs to be redrawn.
EXTERN ਪੂਰਨ		need_cursor_ਸਤਰ_redraw INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, ਨਿਰਾ}
# endif
		    ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
EXTERN ਅੱਖਰ *netbeansArg INIT(= ਨਿਰਾ);	// the -nb[:host:port:passwd] arg
EXTERN ਪੂਰਨ netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN ਪੂਰਨ netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN ਪੂਰਨ netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN ਪੂਰਨ netbeans_Suppress_No_ਸਤਰਾਂ INIT(= 0); // skip "No lines in buffer"
#ਜੇ_ਅੰਤ

/*
 * Some messages that can be shared are included here.
 */
EXTERN ਅੱਖਰ ਸਿਖਰ_bot_ਸੁਨੇਹਾ[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN ਅੱਖਰ bot_ਸਿਖਰ_ਸੁਨੇਹਾ[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN ਅੱਖਰ ਸਤਰ_ਸੁਨੇਹਾ[]	    INIT(= N_(" line "));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
EXTERN ਅੱਖਰ need_ਕੁੰਜੀ_ਸੁਨੇਹਾ[]  INIT(= N_("Need encryption key for \"%s\""));
#ਜੇ_ਅੰਤ

/*
 * Comms. with the session manager (XSMP)
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP
EXTERN ਪੂਰਨ xsmp_icefd INIT(= -1);   // The actual connection
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
EXTERN FILE *ਸਮਾਂ_fd INIT(= ਨਿਰਾ);  // where to write startup timing
#ਜੇ_ਅੰਤ

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN ਪੂਰਨ vim_ignored;
EXTERN ਅੱਖਰ *vim_ignoredp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN ਪੂਰਨ  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN ਪੂਰਨ  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN ਪੂਰਨ  disable_char_avail_for_testing INIT(= FALSE);
EXTERN ਪੂਰਨ  disable_redraw_for_testing INIT(= FALSE);
EXTERN ਪੂਰਨ  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN ਪੂਰਨ  nfa_fail_for_testing INIT(= FALSE);
EXTERN ਪੂਰਨ  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN ਪੂਰਨ  no_query_mouse_for_testing INIT(= FALSE);
EXTERN ਪੂਰਨ  ui_delay_for_testing INIT(= 0);
EXTERN ਪੂਰਨ  reset_ਪਦ_props_on_termresponse INIT(= FALSE);
EXTERN ਪੂਰਨ  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN ਲੰਮਾ override_sysinfo_uptime INIT(= -1);
EXTERN ਪੂਰਨ  override_autoload INIT(= FALSE);
EXTERN ਪੂਰਨ  override_defcompile INIT(= FALSE);
EXTERN ਪੂਰਨ  ml_get_alloc_ਸਤਰਾਂ INIT(= FALSE);
EXTERN ਪੂਰਨ  ignore_unreachable_ਕੋਡ_for_testing INIT(= FALSE);

EXTERN ਪੂਰਨ  ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
EXTERN ਪੂਰਨ  did_ਜੋੜੋ_timer INIT(= FALSE);
EXTERN ਪੂਰਨ  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਪੂਰਨ  ਨਿਵੇਸ਼_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_ਜੜ੍ਹ_T	*lval_ਜੜ੍ਹ INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
EXTERN ਪੂਰਨ  bevalexpr_due_ਸੈੱਟ INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਸਮਾਂ_T ਸਮਾਂ_for_testing INIT(= 0);

EXTERN ਪੂਰਨ echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN ਪੂਰਨ  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN ਪੂਰਨ *eval_lavars_ਵਰਤਿਆ INIT(= ਨਿਰਾ);

// Only filled for Win32.
EXTERN ਅੱਖਰ windows_ਸੰਸਕਰਣ[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_ਵਸਤੂ;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, GA_ਖਾਲੀ_ਥਾਵਾਂ, GA_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ,
			 {0, 0, (ਪੂਰਨ)ਆਕਾਰ(char_u *), 20, ਨਿਰਾ}, 0, ਨਿਰਾ, ਨਿਰਾ}
# endif
	;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
EXTERN ਪੂਰਨ ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
EXTERN ਅੱਖਰ *ch_part_ਨਾਂ[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN ਪੂਰਨ channel_need_redraw INIT(= FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN ਪੂਰਨ ch_log_output INIT(= FALSE);

EXTERN ਪੂਰਨ did_repeated_ਸੁਨੇਹਾ INIT(= 0);
# define REPEATED_ਸੁਨੇਹਾ_LOOKING	    1
# define REPEATED_ਸੁਨੇਹਾ_SAFESTATE	    2
#ਜੇ_ਅੰਤ

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_ਸੈੱਟ);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN ਪੂਰਨ skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN ਪੂਰਨ skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN ਪੂਰਨ skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHOWCMD_BUFLEN (SHOWCMD_ਖਾਨੇ_ਗਿਣਤੀ + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
EXTERN ਪੂਰਨ	p_tgc_ਸੈੱਟ INIT(= FALSE);
#ਜੇ_ਅੰਤ

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN ਅੱਖਰ *wayland_display_name INIT(= ਨਿਰਾ);

// Don't connect to Wayland compositor if TRUE
EXTERN ਪੂਰਨ wayland_no_connect INIT(= FALSE);

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CLIENTSERVER_ਵਿਧੀ_NONE,
# ifdef FEAT_X11
    CLIENTSERVER_ਵਿਧੀ_X11,
# endif
# ifdef MSWIN
    CLIENTSERVER_ਵਿਧੀ_MSWIN,
# endif
# ifdef FEAT_SOCKETSERVER
    CLIENTSERVER_ਵਿਧੀ_SOCKET
# endif
} clientserver_ਵਿਧੀ_T;

EXTERN clientserver_ਵਿਧੀ_T clientserver_ਵਿਧੀ

# ifdef FEAT_X11
INIT(= CLIENTSERVER_ਵਿਧੀ_X11);
# elif defined(MSWIN)
INIT(= CLIENTSERVER_ਵਿਧੀ_MSWIN);
# elif defined(FEAT_SOCKETSERVER)
INIT(= CLIENTSERVER_ਵਿਧੀ_SOCKET);
# ਨਹੀਂਤਾਂ
INIT(= CLIENTSERVER_ਵਿਧੀ_NONE);
# endif

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ

// If the <xOSC> key should be propagated from vgetc()
EXTERN ਪੂਰਨ allow_osc_ਕੁੰਜੀ INIT(= 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= ਨਿਰਾ);
EXTERN bool inside_redraw_on_ਸ਼ੁਰੂ_cb INIT(= false);
#ਜੇ_ਅੰਤ

// If greater than zero, then silence the W23/W24 warning.
EXTERN ਪੂਰਨ silence_w23_w24_ਸੁਨੇਹਾ INIT( = 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T ਆਖਰੀ_ਪਾਓ_ga INIT5(0, 0, 1, 64, ਨਿਰਾ);
EXTERN ਪੂਰਨ	ਜੋੜੋ_ਆਖਰੀ_ਪਾਓ INIT(= 0);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN ਪੂਰਨ cell_ਚੌੜਾਈ INIT(= 8);
EXTERN ਪੂਰਨ cell_ਉਚਾਈ INIT(= 16);
#ਜੇ_ਅੰਤ
