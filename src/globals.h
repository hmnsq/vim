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
EXTERN long	Rows			// nr of rows in the screen
#ifdef DO_INIT
# എങ്കിൽ defined(MSWIN)
		    = 25L
# അല്ലെങ്കിൽ
		    = 24L
# endif
#endif
		    ;
EXTERN long	Columns INIT(= 80);	// nr of columns in the screen

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
EXTERN schar_T	*Screen_വരികൾ INIT(= ശൂന്യ);
EXTERN sattr_T	*ScreenAttrs INIT(= ശൂന്യ);
EXTERN colnr_T  *ScreenCols INIT(= ശൂന്യ);
EXTERN unsigned	*വരി_Offset INIT(= ശൂന്യ);
EXTERN char_u	*വരി_Wraps INIT(= ശൂന്യ);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*Screen_വരികൾ_UC INIT(= ശൂന്യ);	// decoded UTF-8 characters
EXTERN u8char_T	*Screen_വരികൾ_C[MAX_MCO];		// composing characters
EXTERN പൂർണ്ണസംഖ്യ	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= ശൂന്യ);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_Screen_വരി INIT(= ശൂന്യ);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN പൂർണ്ണസംഖ്യ	screen_cur_row INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	screen_cur_col INIT(= 0);

#ifdef FEAT_തിരയൽ_EXTRA
// used for 'hlsearch' highlight matching
EXTERN match_T	screen_തിരയൽ_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T തിരയൽ_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN പൂർണ്ണസംഖ്യ	no_hlsearch INIT(= FALSE);
#endif

#ifdef FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#endif

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN പൂർണ്ണസംഖ്യ redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN short	*TabPageIdxs INIT(= ശൂന്യ);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= ശൂന്യ);
EXTERN പൂർണ്ണസംഖ്യ	tabline_stl_click_എണ്ണം INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= ശൂന്യ);
EXTERN പൂർണ്ണസംഖ്യ	tabpanel_stl_click_എണ്ണം INIT(= 0);

#ifdef FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN short	*popup_mask INIT(= ശൂന്യ);
EXTERN short	*popup_mask_next INIT(= ശൂന്യ);
// Array with flags for transparent cells of current popup.
EXTERN അക്ഷരം	*popup_transparent INIT(= ശൂന്യ);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN പൂർണ്ണസംഖ്യ	popup_mask_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_mask_tab INIT(= ശൂന്യ);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN പൂർണ്ണസംഖ്യ	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= ശൂന്യ);
#endif

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN long	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN പൂർണ്ണസംഖ്യ	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= ശൂന്യ);
EXTERN schar_T	*pum_bg_വരികൾ INIT(= ശൂന്യ);
EXTERN u8char_T	*pum_bg_വരികൾ_UC INIT(= ശൂന്യ);
EXTERN u8char_T	*pum_bg_വരികൾ_C[MAX_MCO];
EXTERN പൂർണ്ണസംഖ്യ	pum_bg_മുകൾ INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	pum_bg_bot INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	pum_bg_cols INIT(= 0);

EXTERN പൂർണ്ണസംഖ്യ	screen_Rows INIT(= 0);	    // actual size of ScreenLines[]
EXTERN പൂർണ്ണസംഖ്യ	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN പൂർണ്ണസംഖ്യ	mod_mask INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN പൂർണ്ണസംഖ്യ	vgetc_mod_mask INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN പൂർണ്ണസംഖ്യ	cmdline_row;
EXTERN പൂർണ്ണസംഖ്യ	cmdline_col_off;
EXTERN പൂർണ്ണസംഖ്യ	cmdline_width;

EXTERN പൂർണ്ണസംഖ്യ	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN പൂർണ്ണസംഖ്യ	redraw_mode INIT(= FALSE);	// mode must be redrawn
EXTERN പൂർണ്ണസംഖ്യ	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN പൂർണ്ണസംഖ്യ	mode_displayed INIT(= FALSE);	// mode is being displayed
EXTERN പൂർണ്ണസംഖ്യ	no_win_do_വരികൾ_ins INIT(= FALSE); // don't insert lines
#എങ്കിൽ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN പൂർണ്ണസംഖ്യ	cmdline_star INIT(= FALSE);	// cmdline is crypted
#endif

EXTERN പൂർണ്ണസംഖ്യ	exec_from_reg INIT(= FALSE);	// executing register

EXTERN പൂർണ്ണസംഖ്യ	screen_cleared INIT(= FALSE);	// screen has been cleared

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

EXTERN char_u	*edit_submode INIT(= ശൂന്യ); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= ശൂന്യ); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= ശൂന്യ);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#ifdef FEAT_RIGHTLEFT
EXTERN പൂർണ്ണസംഖ്യ	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#endif
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_col;
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_row;
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_സന്ദേശം INIT(= ശൂന്യ);	    // msg to be shown after redraw
EXTERN പൂർണ്ണസംഖ്യ	keep_സന്ദേശം_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN പൂർണ്ണസംഖ്യ	keep_സന്ദേശം_more INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN പൂർണ്ണസംഖ്യ	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN പൂർണ്ണസംഖ്യ	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN പൂർണ്ണസംഖ്യ	info_message INIT(= FALSE); // printing informative message
EXTERN പൂർണ്ണസംഖ്യ      സന്ദേശം_hist_off INIT(= FALSE); // don't add messages to history
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN പൂർണ്ണസംഖ്യ	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN പൂർണ്ണസംഖ്യ	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_സന്ദേശം INIT(= ശൂന്യ);
EXTERN long	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= ശൂന്യ);

EXTERN പൂർണ്ണസംഖ്യ	did_endif INIT(= FALSE);    // just had ":endif"
#endif
EXTERN പൂർണ്ണസംഖ്യ	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN പൂർണ്ണസംഖ്യ	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN പൂർണ്ണസംഖ്യ	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN പൂർണ്ണസംഖ്യ	called_vim_beep;	    // set if vim_beep() is called
EXTERN പൂർണ്ണസംഖ്യ	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#endif
EXTERN പൂർണ്ണസംഖ്യ	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN പൂർണ്ണസംഖ്യ	called_emsg;		    // always incremented by emsg()
EXTERN പൂർണ്ണസംഖ്യ	in_echowindow;		    // executing ":echowindow"
EXTERN പൂർണ്ണസംഖ്യ	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN പൂർണ്ണസംഖ്യ	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN പൂർണ്ണസംഖ്യ	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN പൂർണ്ണസംഖ്യ	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN പൂർണ്ണസംഖ്യ	need_wait_return INIT(= 0); // need to wait for return later
EXTERN പൂർണ്ണസംഖ്യ	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN പൂർണ്ണസംഖ്യ	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN പൂർണ്ണസംഖ്യ	quit_more INIT(= FALSE);    // 'q' hit at "--more--" msg
#എങ്കിൽ defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN പൂർണ്ണസംഖ്യ	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN പൂർണ്ണസംഖ്യ	intr_char INIT(= 0);	    // extra interrupt character
#endif
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN പൂർണ്ണസംഖ്യ	x_no_connect INIT(= FALSE); // don't connect to X server
# എങ്കിൽ defined(FEAT_CLIENTSERVER)
EXTERN പൂർണ്ണസംഖ്യ	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#endif
EXTERN പൂർണ്ണസംഖ്യ	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN പൂർണ്ണസംഖ്യ	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN പൂർണ്ണസംഖ്യ	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN പൂർണ്ണസംഖ്യ	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN പൂർണ്ണസംഖ്യ	വരികൾ_ഇടത് INIT(= -1);	    // lines left for listing
EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_no_more INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, വലുപ്പം(estack_T), 50, ശൂന്യ);
#define HAVE_SOURCING_INFO  (exestack.ga_data != ശൂന്യ && exestack.ga_len > 0)
// name of error message source
#define SOURCING_NAME (((estack_T *)exestack.ga_data)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#define SOURCING_LNUM (((estack_T *)exestack.ga_data)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#ifdef FEAT_EVAL
    INIT4(0, 0, 0, 0);
#അല്ലെങ്കിൽ
    INIT(= {0});
#endif

#ifdef FEAT_EVAL
// whether inside compile_def_function()
EXTERN പൂർണ്ണസംഖ്യ	estack_compiling INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ	ex_nesting_level INIT(= 0);	// nesting level
EXTERN പൂർണ്ണസംഖ്യ	debug_break_level INIT(= -1);	// break below this level
EXTERN പൂർണ്ണസംഖ്യ	debug_did_സന്ദേശം INIT(= FALSE);	// did "debug mode" message
EXTERN പൂർണ്ണസംഖ്യ	debug_tick INIT(= 0);		// breakpoint change count
EXTERN പൂർണ്ണസംഖ്യ	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN പൂർണ്ണസംഖ്യ	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_ഇനങ്ങൾ INIT5(0, 0, വലുപ്പം(scriptitem_T *), 20, ശൂന്യ);
# define SCRIPT_ITEM(id)    (((scriptitem_T **)script_ഇനങ്ങൾ.ga_data)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_ഇനങ്ങൾ.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_ITEM(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_വരികൾ.ga_data))[j]

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
EXTERN പൂർണ്ണസംഖ്യ did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN പൂർണ്ണസംഖ്യ need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN പൂർണ്ണസംഖ്യ check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN പൂർണ്ണസംഖ്യ trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN പൂർണ്ണസംഖ്യ force_abort INIT(= FALSE);

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
EXTERN msglist_T **സന്ദേശം_list INIT(= ശൂന്യ);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN പൂർണ്ണസംഖ്യ suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_അട്ടി INIT(= ശൂന്യ);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN പൂർണ്ണസംഖ്യ	may_garbage_collect INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ	want_garbage_collect INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ	garbage_collect_at_exit INIT(= FALSE);


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

# define t_സംഖ്യ		(static_types[12])
# define t_const_സംഖ്യ		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_സംഖ്യ_bool		(static_types[14])
# define t_const_സംഖ്യ_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_സംഖ്യ_float		(static_types[16])
# define t_const_സംഖ്യ_float	(static_types[17])

# define t_float			(static_types[18])
# define t_const_float		(static_types[19])

# define t_string		(static_types[20])
# define t_const_string		(static_types[21])

# define t_blob			(static_types[22])
# define t_const_blob		(static_types[23])

# define t_blob_null		(static_types[24])
# define t_const_blob_null	(static_types[25])

# define t_job			(static_types[26])
# define t_const_job		(static_types[27])

# define t_channel		(static_types[28])
# define t_const_channel		(static_types[29])

// t_number_or_string - Special value used for @#.
# define t_സംഖ്യ_or_string	(static_types[30])
# define t_const_സംഖ്യ_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_സംഖ്യ		(static_types[38])
# define t_const_func_സംഖ്യ	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_സംഖ്യ		(static_types[48])
# define t_const_func_0_സംഖ്യ	(static_types[49])

# define t_func_0_string		(static_types[50])
# define t_const_func_0_string	(static_types[51])

# define t_list_any		(static_types[52])
# define t_const_list_any	(static_types[53])

# define t_dict_any		(static_types[54])
# define t_const_dict_any	(static_types[55])

# define t_list_empty		(static_types[56])
# define t_const_list_empty	(static_types[57])

# define t_dict_empty		(static_types[58])
# define t_const_dict_empty	(static_types[59])

# define t_list_bool		(static_types[60])
# define t_const_list_bool	(static_types[61])

# define t_list_സംഖ്യ		(static_types[62])
# define t_const_list_സംഖ്യ	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_job		(static_types[66])
# define t_const_list_job	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_സംഖ്യ	(static_types[72])
# define t_const_list_list_സംഖ്യ (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_സംഖ്യ	(static_types[76])
# define t_const_list_list_list_സംഖ്യ (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_സംഖ്യ		(static_types[80])
# define t_const_dict_സംഖ്യ	(static_types[81])

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

# define t_tuple_empty		(static_types[94])
# define t_const_tuple_empty	(static_types[95])


EXTERN type_T static_types[96]
# ifdef DO_INIT
= {
    // 0: t_unknown
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 12: t_number
    {VAR_സംഖ്യ, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_സംഖ്യ, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 14: t_number_bool
    {VAR_സംഖ്യ, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_സംഖ്യ, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 16: t_number_float
    {VAR_സംഖ്യ, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_സംഖ്യ, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, ശൂന്യ, ശൂന്യ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ശൂന്യ, ശൂന്യ},

    // 26: t_job
    {VAR_JOB, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_JOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ശൂന്യ, ശൂന്യ},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ശൂന്യ, ശൂന്യ},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ശൂന്യ, ശൂന്യ},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ശൂന്യ, ശൂന്യ},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ശൂന്യ, ശൂന്യ},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ശൂന്യ, ശൂന്യ},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ശൂന്യ, ശൂന്യ},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, ശൂന്യ, ശൂന്യ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ശൂന്യ, ശൂന്യ},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ശൂന്യ, ശൂന്യ},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, ശൂന്യ, ശൂന്യ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ശൂന്യ, ശൂന്യ},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ശൂന്യ, ശൂന്യ},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, ശൂന്യ, ശൂന്യ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ശൂന്യ, ശൂന്യ},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ശൂന്യ, ശൂന്യ},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ശൂന്യ, ശൂന്യ},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_job, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_job, ശൂന്യ, ശൂന്യ},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, ശൂന്യ, ശൂന്യ},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, ശൂന്യ, ശൂന്യ},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_സംഖ്യ, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_സംഖ്യ, ശൂന്യ, ശൂന്യ},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, ശൂന്യ, ശൂന്യ},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_സംഖ്യ, ശൂന്യ, ശൂന്യ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_സംഖ്യ, ശൂന്യ, ശൂന്യ},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, ശൂന്യ, ശൂന്യ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ശൂന്യ, ശൂന്യ},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_സംഖ്യ, ശൂന്യ, ശൂന്യ},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, ശൂന്യ, ശൂന്യ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ശൂന്യ, ശൂന്യ},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, ശൂന്യ, ശൂന്യ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ശൂന്യ, ശൂന്യ},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, ശൂന്യ, ശൂന്യ, ശൂന്യ},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ശൂന്യ, ശൂന്യ, ശൂന്യ},
}
# endif
;

EXTERN പൂർണ്ണസംഖ്യ	did_source_packages INIT(= FALSE);
#endif // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN പൂർണ്ണസംഖ്യ	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN പൂർണ്ണസംഖ്യ	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN പൂർണ്ണസംഖ്യ include_none INIT(= 0);	// when 1 include "None"
EXTERN പൂർണ്ണസംഖ്യ include_default INIT(= 0);	// when 1 include "default"
EXTERN പൂർണ്ണസംഖ്യ include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN പൂർണ്ണസംഖ്യ	highlight_match INIT(= FALSE);	// show search match pos
EXTERN linenr_T	തിരയൽ_match_വരികൾ;		// lines of matched string
EXTERN colnr_T	തിരയൽ_match_endcol;		// col nr of match end
#ifdef FEAT_തിരയൽ_EXTRA
EXTERN linenr_T	തിരയൽ_first_വരി INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	തിരയൽ_last_വരി INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#endif

EXTERN പൂർണ്ണസംഖ്യ	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN പൂർണ്ണസംഖ്യ	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN പൂർണ്ണസംഖ്യ	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN പൂർണ്ണസംഖ്യ	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN പൂർണ്ണസംഖ്യ	highlight_attr[HLF_എണ്ണം];  // Highl. attr for each context.
#ifdef FEAT_STL_OPT
# define USER_HIGHLIGHT
#endif
#ifdef USER_HIGHLIGHT
EXTERN പൂർണ്ണസംഖ്യ	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN പൂർണ്ണസംഖ്യ	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN പൂർണ്ണസംഖ്യ	highlight_stlterm[9];		// On top of user
EXTERN പൂർണ്ണസംഖ്യ	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#endif
#ifdef FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN പൂർണ്ണസംഖ്യ	skip_term_loop INIT(= FALSE);
#endif
#ifdef FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= ശൂന്യ);	// "-U" cmdline argument
#endif
EXTERN പൂർണ്ണസംഖ്യ	cterm_normal_fg_color INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	cterm_normal_fg_bold INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	cterm_normal_bg_color INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	cterm_normal_ul_color INIT(= 0);
#ifdef FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_normal_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_normal_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_normal_ul_gui_color INIT(= INVALCOLOR);
#endif
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#ifdef FEAT_TERMRESPONSE
EXTERN പൂർണ്ണസംഖ്യ	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#endif

EXTERN പൂർണ്ണസംഖ്യ	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN പൂർണ്ണസംഖ്യ	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN പൂർണ്ണസംഖ്യ	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN പൂർണ്ണസംഖ്യ	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(ശൂന്യ, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= ശൂന്യ);
EXTERN win_T	*au_pending_free_win INIT(= ശൂന്യ);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN പൂർണ്ണസംഖ്യ	mouse_row;
EXTERN പൂർണ്ണസംഖ്യ	mouse_col;
EXTERN പൂർണ്ണസംഖ്യ	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN പൂർണ്ണസംഖ്യ	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN പൂർണ്ണസംഖ്യ	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#എങ്കിൽ defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN പൂർണ്ണസംഖ്യ	WantQueryMouse INIT(= FALSE);
#endif

#ifdef FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN പൂർണ്ണസംഖ്യ	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_prev_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN പൂർണ്ണസംഖ്യ	gui_prev_topfill INIT(= 0);
# endif
#endif

#ifdef FEAT_MOUSESHAPE
EXTERN പൂർണ്ണസംഖ്യ	drag_status_വരി INIT(= FALSE);	// dragging the status line
EXTERN പൂർണ്ണസംഖ്യ	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN പൂർണ്ണസംഖ്യ	drag_sep_വരി INIT(= FALSE);	// dragging vert separator
#endif


#ifdef FEAT_DIFF
// Value set from 'diffopt'.
EXTERN പൂർണ്ണസംഖ്യ	diff_context INIT(= 6);		// context for folds
EXTERN പൂർണ്ണസംഖ്യ      linematch_വരികൾ INIT(= 0);      // number of lines for diff line match
EXTERN പൂർണ്ണസംഖ്യ	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN പൂർണ്ണസംഖ്യ	diff_need_scrollbind INIT(= FALSE);
#endif

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN പൂർണ്ണസംഖ്യ	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN പൂർണ്ണസംഖ്യ	redraw_not_allowed INIT(= FALSE);

#ifdef MESSAGE_QUEUE
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN പൂർണ്ണസംഖ്യ	dont_parse_messages INIT(= FALSE);
#endif

#ifdef FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*വേര്_menu INIT(= ശൂന്യ);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN പൂർണ്ണസംഖ്യ	sys_menu INIT(= FALSE);
#endif

#ifdef FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN പൂർണ്ണസംഖ്യ force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN പൂർണ്ണസംഖ്യ	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN പൂർണ്ണസംഖ്യ	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN പൂർണ്ണസംഖ്യ	current_scrollbar;
EXTERN long_u	scrollbar_മൂല്യം;

// found "-rv" or "-reverse" in command line args
EXTERN പൂർണ്ണസംഖ്യ	found_reverse_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN അക്ഷരം	*font_argument INIT(= ശൂന്യ);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN അക്ഷരം	*background_argument INIT(= ശൂന്യ);

// "-fg" or "-foreground" command line argument
EXTERN അക്ഷരം	*foreground_argument INIT(= ശൂന്യ);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN അസ്ഥിര sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN പൂർണ്ണസംഖ്യ	new_pixel_width INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	new_pixel_height INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN പൂർണ്ണസംഖ്യ	gui_win_x INIT(= -1);
EXTERN പൂർണ്ണസംഖ്യ	gui_win_y INIT(= -1);
#endif

#ifdef FEAT_CLIPBOARD
EXTERN Clipboard_T clip_star;	// PRIMARY selection in X11/Wayland
# എങ്കിൽ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# അല്ലെങ്കിൽ
#  define clip_plus clip_star	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#endif

#ifdef HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN പൂർണ്ണസംഖ്യ	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN പൂർണ്ണസംഖ്യ	clip_autoselect_star INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ	clip_autoselect_plus INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ	clip_autoselectml INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= ശൂന്യ);
EXTERN പൂർണ്ണസംഖ്യ	clip_unnamed_saved INIT(= 0);
# endif
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= ശൂന്യ);
#endif


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= ശൂന്യ);	// previous window (may equal curwin)
#define ONE_WINDOW (firstwin == lastwin)
#define W_NEXT(wp) ((wp)->w_next)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#define AUCMD_WIN_എണ്ണം 5

തരനിർവചനം ഘടന {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    പൂർണ്ണസംഖ്യ		auc_win_used;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_എണ്ണം];

#ifdef FEAT_PROP_POPUP
EXTERN win_T    *first_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= ശൂന്യ);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN പൂർണ്ണസംഖ്യ	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN പൂർണ്ണസംഖ്യ	popup_uses_mouse_move INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ	text_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN പൂർണ്ണസംഖ്യ	ignore_text_props INIT(= FALSE);
#endif

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN പൂർണ്ണസംഖ്യ	pum_will_redraw INIT(= FALSE);

/*
 * The window layout is kept in a tree of frames.  topframe points to the top
 * of the tree.
 */
EXTERN frame_T	*topframe;	// top of the window frame tree

/*
 * Tab pages are alternative topframes.  "first_tabpage" points to the first
 * one in the list, "curtab" is the current one. "lastused_tabpage" is the
 * last used one.
 */
EXTERN tabpage_T    *first_tabpage;
EXTERN tabpage_T    *curtab;
EXTERN tabpage_T    *lastused_tabpage;
EXTERN പൂർണ്ണസംഖ്യ	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN പൂർണ്ണസംഖ്യ	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#എങ്കിൽ defined(FEAT_TABPANEL)
EXTERN പൂർണ്ണസംഖ്യ	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#endif

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= ശൂന്യ);	// first buffer
EXTERN buf_T	*lastbuf INIT(= ശൂന്യ);	// last buffer
EXTERN buf_T	*curbuf INIT(= ശൂന്യ);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN പൂർണ്ണസംഖ്യ	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN പൂർണ്ണസംഖ്യ	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN പൂർണ്ണസംഖ്യ	arg_had_last INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN പൂർണ്ണസംഖ്യ	ru_col;		// column for ruler
#ifdef FEAT_STL_OPT
EXTERN പൂർണ്ണസംഖ്യ	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#endif
EXTERN പൂർണ്ണസംഖ്യ	sc_col;		// column for shown command

#ifdef TEMPDIRNAMES
# എങ്കിൽ defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= ശൂന്യ); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= ശൂന്യ); // Name of Vim's own temp dir.
					   // Ends in a slash.
#endif

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN പൂർണ്ണസംഖ്യ	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN പൂർണ്ണസംഖ്യ	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN പൂർണ്ണസംഖ്യ	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN പൂർണ്ണസംഖ്യ	v_dying INIT(= 0); // internal value of v:dying
EXTERN പൂർണ്ണസംഖ്യ	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#എങ്കിൽ defined(FEAT_AUTOCHDIR)
EXTERN പൂർണ്ണസംഖ്യ	test_autochdir INIT(= FALSE);
#endif
EXTERN അക്ഷരം	*last_chdir_reason INIT(= ശൂന്യ);
#എങ്കിൽ defined(EXITFREE)
EXTERN പൂർണ്ണസംഖ്യ	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#endif
// volatile because it is used in signal handler deathtrap().
EXTERN അസ്ഥിര sig_atomic_t full_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN പൂർണ്ണസംഖ്യ	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN പൂർണ്ണസംഖ്യ	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN പൂർണ്ണസംഖ്യ	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN പൂർണ്ണസംഖ്യ	curbuf_lock INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN പൂർണ്ണസംഖ്യ	allbuf_lock INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#ifdef HAVE_SANDBOX
EXTERN പൂർണ്ണസംഖ്യ	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#endif

EXTERN പൂർണ്ണസംഖ്യ	silent_mode INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#ifdef FEAT_JOB_CHANNEL
EXTERN പൂർണ്ണസംഖ്യ	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#endif

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN പൂർണ്ണസംഖ്യ	VIsual_active INIT(= FALSE);
				// whether Visual mode is active
EXTERN പൂർണ്ണസംഖ്യ	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN പൂർണ്ണസംഖ്യ	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN പൂർണ്ണസംഖ്യ  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN പൂർണ്ണസംഖ്യ	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN പൂർണ്ണസംഖ്യ	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN പൂർണ്ണസംഖ്യ	VIsual_mode INIT(= 'v');
				// type of Visual mode

EXTERN പൂർണ്ണസംഖ്യ	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN പൂർണ്ണസംഖ്യ	resel_VIsual_mode INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_വരി_എണ്ണം;	// number of lines
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
EXTERN പൂർണ്ണസംഖ്യ     did_ai INIT(= FALSE);

/*
 * Column of first char after autoindent.  0 when no autoindent done.  Used
 * when 'backspace' is 0, to avoid backspacing over autoindent.
 */
EXTERN colnr_T	ai_col INIT(= 0);

/*
 * This is a character which will end a start-middle-end comment when typed as
 * the first character on a new line.  It is taken from the last character of
 * the "end" comment leader when the COM_AUTO_END flag is given for that
 * comment end in 'comments'.  It is only valid when did_ai is TRUE.
 */
EXTERN പൂർണ്ണസംഖ്യ     end_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN പൂർണ്ണസംഖ്യ     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN പൂർണ്ണസംഖ്യ	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN പൂർണ്ണസംഖ്യ	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN പൂർണ്ണസംഖ്യ	can_si_back INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ	old_indent INIT(= 0);	// for ^^D command in insert mode

EXTERN pos_T	saved_cursor		// w_cursor before formatting text.
#ifdef DO_INIT
		    = {0, 0, 0}
#endif
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
EXTERN പൂർണ്ണസംഖ്യ	orig_വരി_എണ്ണം INIT(= 0);  // Line count when "gR" started
EXTERN പൂർണ്ണസംഖ്യ	vr_വരികൾ_changed INIT(= 0); // #Lines changed by "gR" so far

#എങ്കിൽ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
// argument to SETJMP() for handling X IO errors
EXTERN JMP_BUF x_jump_env;
#endif

/*
 * These flags are set based upon 'fileencoding'.
 * Note that "enc_utf8" is also set for "unicode", because the characters are
 * internally stored as UTF-8 (to avoid trouble with NUL bytes).
 */
#define DBCS_JPN	932	// japan
#define DBCS_JPNU	9932	// euc-jp
#define DBCS_KOR	949	// korea
#define DBCS_KORU	9949	// euc-kr
#define DBCS_CHS	936	// chinese
#define DBCS_CHSU	9936	// euc-cn
#define DBCS_CHT	950	// taiwan
#define DBCS_CHTU	9950	// euc-tw
#define DBCS_2BYTE	1	// 2byte-
#define DBCS_DEBUG	(-1)

EXTERN പൂർണ്ണസംഖ്യ	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN പൂർണ്ണസംഖ്യ	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN പൂർണ്ണസംഖ്യ	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN പൂർണ്ണസംഖ്യ	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#എങ്കിൽ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN പൂർണ്ണസംഖ്യ	enc_codepage INIT(= -1);
EXTERN പൂർണ്ണസംഖ്യ	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#endif
EXTERN പൂർണ്ണസംഖ്യ	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN അക്ഷരം	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T നിവേശം_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN പൂർണ്ണസംഖ്യ (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN പൂർണ്ണസംഖ്യ (*mb_ptr2len_len)(char_u *p, പൂർണ്ണസംഖ്യ size) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN പൂർണ്ണസംഖ്യ (*mb_char2len)(പൂർണ്ണസംഖ്യ c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN പൂർണ്ണസംഖ്യ (*mb_char2bytes)(പൂർണ്ണസംഖ്യ c, char_u *buf) INIT(= latin_char2bytes);

EXTERN പൂർണ്ണസംഖ്യ (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN പൂർണ്ണസംഖ്യ (*mb_ptr2cells_len)(char_u *p, പൂർണ്ണസംഖ്യ size) INIT(= latin_ptr2cells_len);
EXTERN പൂർണ്ണസംഖ്യ (*mb_char2cells)(പൂർണ്ണസംഖ്യ c) INIT(= latin_char2cells);
EXTERN പൂർണ്ണസംഖ്യ (*mb_off2cells)(unsigned off, unsigned max_off) INIT(= latin_off2cells);
EXTERN പൂർണ്ണസംഖ്യ (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN പൂർണ്ണസംഖ്യ (*mb_head_off)(char_u *base, char_u *p) INIT(= latin_head_off);

#എങ്കിൽ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, സ്ഥിരം അക്ഷരം **inbuf, size_t *inbytesleft, അക്ഷരം **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (സ്ഥിരം അക്ഷരം *tocode, സ്ഥിരം അക്ഷരം *fromcode);
EXTERN പൂർണ്ണസംഖ്യ (*iconv_close) (iconv_t cd);
EXTERN പൂർണ്ണസംഖ്യ (*iconvctl) (iconv_t cd, പൂർണ്ണസംഖ്യ request, ശൂന്യം *argument);
EXTERN പൂർണ്ണസംഖ്യ* (*iconv_errno) (ശൂന്യം);
#endif


#ifdef FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= ശൂന്യ);
/*
 * Start and end column of the preedit area in virtual columns from the start
 * of the text line.  When there is no preedit area they are set to MAXCOL.
 * "preedit_end_col" is needed for coloring the preedited string.  Drawing the
 * color between "preedit_start_col" and curpos did not work, because some XIM
 * set the cursor position to the first char of the string.
 */
EXTERN colnr_T		preedit_start_col INIT(= MAXCOL);
EXTERN colnr_T		preedit_end_col INIT(= MAXCOL);

// "xim_changed_while_preediting" is set when changed() can set the 'modified'
// flag even while preediting.
EXTERN പൂർണ്ണസംഖ്യ		xim_changed_while_preediting INIT(= FALSE);
# അല്ലെങ്കിൽ
EXTERN XIC		xic INIT(= ശൂന്യ);
# endif
# ifdef FEAT_GUI
EXTERN guicolor_T	xim_fg_color INIT(= INVALCOLOR);
EXTERN guicolor_T	xim_bg_color INIT(= INVALCOLOR);
# endif
#endif

/*
 * "State" is the main state of Vim.
 * There are other variables that modify the state:
 * "Visual_mode"    When State is MODE_NORMAL or MODE_INSERT.
 * "finish_op"	    When State is MODE_NORMAL, after typing the operator and
 *		    before typing the motion command.
 * "motion_force"   Last motion_force  from do_pending_operator()
 * "debug_mode"	    Debug mode.
 */
EXTERN പൂർണ്ണസംഖ്യ	State INIT(= MODE_NORMAL);

#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	debug_mode INIT(= FALSE);
#endif

EXTERN പൂർണ്ണസംഖ്യ	finish_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN long	opcount INIT(= 0);	// count for pending operator
EXTERN പൂർണ്ണസംഖ്യ	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN പൂർണ്ണസംഖ്യ exmode_active INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN പൂർണ്ണസംഖ്യ pending_exmode_active INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN പൂർണ്ണസംഖ്യ reg_recording INIT(= 0);	// register for recording  or zero
EXTERN പൂർണ്ണസംഖ്യ reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN പൂർണ്ണസംഖ്യ pending_end_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN പൂർണ്ണസംഖ്യ seen_Modify_Other_താക്കോലുകൾ INIT(= FALSE);

// The state for the modifyOtherKeys level
തരനിർവചനം എണ്ണൽ {
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
തരനിർവചനം എണ്ണൽ {
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

EXTERN പൂർണ്ണസംഖ്യ no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN പൂർണ്ണസംഖ്യ no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN പൂർണ്ണസംഖ്യ allow_താക്കോലുകൾ INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN പൂർണ്ണസംഖ്യ no_reduce_താക്കോലുകൾ INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN പൂർണ്ണസംഖ്യ no_u_sync INIT(= 0);		// Don't call u_sync()
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#endif

EXTERN പൂർണ്ണസംഖ്യ restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN പൂർണ്ണസംഖ്യ arrow_used;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN പൂർണ്ണസംഖ്യ	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN പൂർണ്ണസംഖ്യ	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#ifdef USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#endif

#ifdef USE_ON_FLY_SCROLL
EXTERN പൂർണ്ണസംഖ്യ	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#endif
EXTERN പൂർണ്ണസംഖ്യ	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN പൂർണ്ണസംഖ്യ	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN പൂർണ്ണസംഖ്യ	sticky_cmdmod_flags INIT(= 0); // used by :execute

#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	is_export INIT(= FALSE);    // :export {cmd}
#endif

EXTERN പൂർണ്ണസംഖ്യ	സന്ദേശം_silent INIT(= 0);	// don't print messages
EXTERN പൂർണ്ണസംഖ്യ	emsg_silent INIT(= 0);	// don't print error messages
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#endif
EXTERN പൂർണ്ണസംഖ്യ	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN പൂർണ്ണസംഖ്യ	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN പൂർണ്ണസംഖ്യ	in_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN പൂർണ്ണസംഖ്യ	swap_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN പൂർണ്ണസംഖ്യ	swap_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN അക്ഷരം	സന്ദേശം_buf[സന്ദേശം_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN പൂർണ്ണസംഖ്യ	RedrawingDisabled INIT(= 0);

EXTERN പൂർണ്ണസംഖ്യ	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN പൂർണ്ണസംഖ്യ	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#ifdef DO_INIT
		    = {ശൂന്യ, ശൂന്യ, 0, 0, 0, 0, 0, 0, 0}
#endif
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN പൂർണ്ണസംഖ്യ	typebuf_was_empty INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ	ex_normal_busy INIT(= 0);   // recursiveness of ex_normal()
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	in_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#endif
EXTERN പൂർണ്ണസംഖ്യ	ex_normal_lock INIT(= 0);   // forbid use of ex_normal()

#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ	ignore_script INIT(= FALSE);  // ignore script input
#endif
EXTERN പൂർണ്ണസംഖ്യ	stop_തിരുകുക_mode;	// for ":stopinsert" and 'insertmode'

EXTERN പൂർണ്ണസംഖ്യ	താക്കോൽ_Typed;		// TRUE if user typed current char
EXTERN പൂർണ്ണസംഖ്യ	താക്കോൽ_Stuffed;		// TRUE if current char from stuffbuf
#ifdef HAVE_നിവേശം_METHOD
EXTERN പൂർണ്ണസംഖ്യ	vgetc_im_active;	// Input Method was active for last
					// character obtained from vgetc()
#endif
EXTERN പൂർണ്ണസംഖ്യ	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN പൂർണ്ണസംഖ്യ	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN പൂർണ്ണസംഖ്യ	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN പൂർണ്ണസംഖ്യ	do_redraw INIT(= FALSE);    // extra redraw once
#ifdef FEAT_DIFF
EXTERN പൂർണ്ണസംഖ്യ	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#endif
#ifdef FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN പൂർണ്ണസംഖ്യ	redrawtime_limit_set INIT(= FALSE);
#endif

EXTERN പൂർണ്ണസംഖ്യ	need_highlight_changed INIT(= TRUE);

#define NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN പൂർണ്ണസംഖ്യ	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= ശൂന്യ);   // stream to write script to
EXTERN പൂർണ്ണസംഖ്യ	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN അസ്ഥിര sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN അസ്ഥിര sig_atomic_t got_sigusr1 INIT(= FALSE);

#ifdef USE_TERM_CONSOLE
EXTERN പൂർണ്ണസംഖ്യ	term_console INIT(= FALSE); // set to TRUE when console used
#endif
EXTERN പൂർണ്ണസംഖ്യ	termcap_active INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN പൂർണ്ണസംഖ്യ	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN പൂർണ്ണസംഖ്യ	searchcmdlen;		    // length of previous search cmd
#ifdef FEAT_SYN_HL
EXTERN പൂർണ്ണസംഖ്യ	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_in INIT(= ശൂന്യ); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_out INIT(= ശൂന്യ); // Set by vim_regexec()
					    // to store \z\(...\) matches
#endif

EXTERN പൂർണ്ണസംഖ്യ	did_outofmem_സന്ദേശം INIT(= FALSE);
					    // set after out of memory msg
EXTERN പൂർണ്ണസംഖ്യ	did_swapwrite_സന്ദേശം INIT(= FALSE);
					    // set after swap write error msg
EXTERN പൂർണ്ണസംഖ്യ	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN പൂർണ്ണസംഖ്യ	global_busy INIT(= 0);	    // set when :global is executing
EXTERN പൂർണ്ണസംഖ്യ	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN പൂർണ്ണസംഖ്യ	need_start_insertmode INIT(= FALSE);
					    // start insert mode soon
#എങ്കിൽ defined(FEAT_EVAL)
EXTERN char_u	last_mode[MODE_MAX_LENGTH] INIT(= "n"); // for ModeChanged event
#endif
EXTERN char_u	*last_cmdline INIT(= ശൂന്യ); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= ശൂന്യ); // command line for "."
EXTERN char_u	*new_last_cmdline INIT(= ശൂന്യ);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= ശൂന്യ); // fname for <afile> on cmdline
EXTERN പൂർണ്ണസംഖ്യ	autocmd_fname_full;	     // autocmd_fname is full path
EXTERN പൂർണ്ണസംഖ്യ	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_match INIT(= ശൂന്യ); // name for <amatch> on cmdline
EXTERN പൂർണ്ണസംഖ്യ	aucmd_cmdline_changed_എണ്ണം INIT(= 0);

EXTERN പൂർണ്ണസംഖ്യ	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	last_cursormoved	      // for CursorMoved event
#ifdef DO_INIT
		    = {0, 0, 0}
#endif
		    ;

EXTERN പൂർണ്ണസംഖ്യ	postponed_split INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN പൂർണ്ണസംഖ്യ	postponed_split_flags INIT(= 0);  // args for win_split()
EXTERN പൂർണ്ണസംഖ്യ	postponed_split_tab INIT(= 0);  // cmdmod.cmod_tab
#ifdef FEAT_QUICKFIX
EXTERN പൂർണ്ണസംഖ്യ	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#endif
EXTERN പൂർണ്ണസംഖ്യ	g_tag_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN പൂർണ്ണസംഖ്യ	replace_offset INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN പൂർണ്ണസംഖ്യ	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*empty_option INIT(= (char_u *)"");

EXTERN പൂർണ്ണസംഖ്യ  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= ശൂന്യ);	// message redirection file
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ  redir_reg INIT(= 0);	// message redirection register
EXTERN പൂർണ്ണസംഖ്യ  redir_vname INIT(= 0);	// message redirection variable
EXTERN പൂർണ്ണസംഖ്യ  redir_execute INIT(= 0);	// execute() redirection
#endif

#ifdef FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#endif

EXTERN പൂർണ്ണസംഖ്യ  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN പൂർണ്ണസംഖ്യ  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN പൂർണ്ണസംഖ്യ  wild_menu_showing INIT(= 0);
#define WM_SHOWN	1		// wildmenu showing
#define WM_SCROLLED	2		// wildmenu showing with scroll

#ifdef MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN പൂർണ്ണസംഖ്യ	found_register_arg INIT(= FALSE);
#endif

#ifdef FEAT_LINEBREAK
EXTERN അക്ഷരം	breakat_flags[256];	// which characters are in 'breakat'
#endif

// These are in version.c, call init_longVersion() before use.
ബാഹ്യ അക്ഷരം *Version;
#എങ്കിൽ defined(HAVE_DATE_TIME) && defined(VMS) && defined(VAXC)
ബാഹ്യ അക്ഷരം longVersion[];
#അല്ലെങ്കിൽ
ബാഹ്യ അക്ഷരം *longVersion;
#endif

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#ifdef HAVE_PATHDEF
ബാഹ്യ char_u *default_vim_dir;
ബാഹ്യ char_u *default_vimruntime_dir;
ബാഹ്യ char_u *all_cflags;
ബാഹ്യ char_u *all_lflags;
# ifdef VMS
ബാഹ്യ char_u *compiler_version;
ബാഹ്യ char_u *compiled_arch;
ബാഹ്യ char_u *compiled_vers;
# endif
ബാഹ്യ char_u *compiled_user;
ബാഹ്യ char_u *compiled_sys;
#endif

EXTERN char_u	*homedir INIT(= ശൂന്യ);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= ശൂന്യ);

#ifdef FEAT_FOLDING
EXTERN പൂർണ്ണസംഖ്യ	disable_fold_update INIT(= 0);
#endif

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN പൂർണ്ണസംഖ്യ	km_stopsel INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ	km_startsel INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN പൂർണ്ണസംഖ്യ	cmdwin_result INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= ശൂന്യ); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= ശൂന്യ); // window of cmdline window or NULL

EXTERN char_u no_വരികൾ_സന്ദേശം[]	INIT(= N_("--No lines in buffer--"));

EXTERN അക്ഷരം typename_unknown[]	INIT(= N_("unknown"));
EXTERN അക്ഷരം typename_int[]	INIT(= "int");
EXTERN അക്ഷരം typename_longint[]	INIT(= "long int");
EXTERN അക്ഷരം typename_longlongint[]	INIT(= "long long int");
EXTERN അക്ഷരം typename_unsignedint[]	INIT(= "unsigned int");
EXTERN അക്ഷരം typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN അക്ഷരം typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN അക്ഷരം typename_pointer[]	INIT(= N_("pointer"));
EXTERN അക്ഷരം typename_percent[]	INIT(= N_("percent"));
EXTERN അക്ഷരം typename_char[] INIT(= "char");
EXTERN അക്ഷരം typename_string[]	INIT(= N_("string"));
EXTERN അക്ഷരം typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN long	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#ifdef FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN ഘടന subs_വ്യഞ്ജകം_S	*substitute_instr INIT(= ശൂന്യ);
#endif

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#എങ്കിൽ defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_IN_ICON	1
# define STL_IN_TITLE	2
EXTERN പൂർണ്ണസംഖ്യ      stl_syntax INIT(= 0);
#endif

#എങ്കിൽ defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= ശൂന്യ);
EXTERN പൂർണ്ണസംഖ്യ		balloonEvalForTerm INIT(= FALSE);
# എങ്കിൽ defined(FEAT_NETBEANS_INTG)
EXTERN പൂർണ്ണസംഖ്യ bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#endif

#ifdef CURSOR_SHAPE
// the table is in misc2.c, because of initializations
ബാഹ്യ cursorentry_T shape_table[SHAPE_IDX_എണ്ണം];
#endif

#ifdef FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_മുകൾ		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_ഇടത്		2
# define OPT_PRINT_വലത്	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_സംഖ്യ	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_അക്കം_OPTIONS	15

EXTERN option_table_T printer_opts[OPT_PRINT_അക്കം_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, ശൂന്യ, 0, FALSE},
	{"bottom",	TRUE, 0, ശൂന്യ, 0, FALSE},
	{"left",	TRUE, 0, ശൂന്യ, 0, FALSE},
	{"right",	TRUE, 0, ശൂന്യ, 0, FALSE},
	{"header",	TRUE, 0, ശൂന്യ, 0, FALSE},
	{"syntax",	FALSE, 0, ശൂന്യ, 0, FALSE},
	{"number",	FALSE, 0, ശൂന്യ, 0, FALSE},
	{"wrap",	FALSE, 0, ശൂന്യ, 0, FALSE},
	{"duplex",	FALSE, 0, ശൂന്യ, 0, FALSE},
	{"portrait", FALSE, 0, ശൂന്യ, 0, FALSE},
	{"paper",	FALSE, 0, ശൂന്യ, 0, FALSE},
	{"collate",	FALSE, 0, ശൂന്യ, 0, FALSE},
	{"jobsplit", FALSE, 0, ശൂന്യ, 0, FALSE},
	{"formfeed", FALSE, 0, ശൂന്യ, 0, FALSE},
	{"format", FALSE, 0, ശൂന്യ, 0, FALSE},
    }
# endif
    ;

// For prt_get_unit().
# define PRT_UNIT_NONE	-1
# define PRT_UNIT_PERC	0
# define PRT_UNIT_INCH	1
# define PRT_UNIT_MM	2
# define PRT_UNIT_POINT	3
# define PRT_UNIT_NAMES {"pc", "in", "mm", "pt"}
#endif

#എങ്കിൽ (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_page_അക്കം;
#endif

#ifdef FEAT_XCLIPBOARD
// xterm display name
EXTERN അക്ഷരം	*xterm_display INIT(= ശൂന്യ);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN പൂർണ്ണസംഖ്യ	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= ശൂന്യ);
#endif
#എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)ശൂന്യ);
#endif

#എങ്കിൽ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#endif

#ifdef FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#endif

#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN പൂർണ്ണസംഖ്യ	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#endif

#ifdef FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= ശൂന്യ);	// name of the server
# ifdef FEAT_X11
EXTERN Window	commWindow INIT(= None);
EXTERN Window	clientWindow INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*serverDelayedStartName INIT(= ശൂന്യ);
# elif defined(MSWIN)
EXTERN HWND	clientWindow INIT(= 0);
# endif
#endif

#എങ്കിൽ defined(UNIX) || defined(VMS)
EXTERN പൂർണ്ണസംഖ്യ	term_is_xterm INIT(= FALSE);	// xterm-like 'term'
#endif

#ifdef BACKSLASH_IN_FILENAME
EXTERN അക്ഷരം	psepc INIT(= '\\');	// normal path separator character
EXTERN അക്ഷരം	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN അക്ഷരം	pseps[2] INIT2('\\', 0);
#endif

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN പൂർണ്ണസംഖ്യ	virtual_op INIT(= MAYBE);

#ifdef FEAT_SYN_HL
// Display tick, incremented for each call to update_screen()
EXTERN disptick_T	display_tick INIT(= 0);
#endif

#ifdef FEAT_SPELL
// Line in which spell checking wasn't highlighted because it touched the
// cursor position in Insert mode.
EXTERN linenr_T		spell_redraw_lnum INIT(= 0);
#endif

#ifdef FEAT_CONCEAL
// Set when the cursor line needs to be redrawn.
EXTERN പൂർണ്ണസംഖ്യ		need_cursor_വരി_redraw INIT(= FALSE);
#endif

#ifdef USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, ശൂന്യ}
# endif
		    ;
#endif

#ifdef FEAT_NETBEANS_INTG
EXTERN അക്ഷരം *netbeansArg INIT(= ശൂന്യ);	// the -nb[:host:port:passwd] arg
EXTERN പൂർണ്ണസംഖ്യ netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN പൂർണ്ണസംഖ്യ netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN പൂർണ്ണസംഖ്യ netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN പൂർണ്ണസംഖ്യ netbeans_Suppress_No_വരികൾ INIT(= 0); // skip "No lines in buffer"
#endif

/*
 * Some messages that can be shared are included here.
 */
EXTERN അക്ഷരം മുകൾ_bot_സന്ദേശം[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN അക്ഷരം bot_മുകൾ_സന്ദേശം[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN അക്ഷരം വരി_സന്ദേശം[]	    INIT(= N_(" line "));

#ifdef FEAT_CRYPT
EXTERN അക്ഷരം need_താക്കോൽ_സന്ദേശം[]  INIT(= N_("Need encryption key for \"%s\""));
#endif

/*
 * Comms. with the session manager (XSMP)
 */
#ifdef USE_XSMP
EXTERN പൂർണ്ണസംഖ്യ xsmp_icefd INIT(= -1);   // The actual connection
#endif

#ifdef STARTUPTIME
EXTERN FILE *time_fd INIT(= ശൂന്യ);  // where to write startup timing
#endif

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN പൂർണ്ണസംഖ്യ vim_ignored;
EXTERN അക്ഷരം *vim_ignoredp;

#ifdef FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN പൂർണ്ണസംഖ്യ  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN പൂർണ്ണസംഖ്യ  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN പൂർണ്ണസംഖ്യ  disable_char_avail_for_testing INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  disable_redraw_for_testing INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  nfa_fail_for_testing INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  no_query_mouse_for_testing INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  ui_delay_for_testing INIT(= 0);
EXTERN പൂർണ്ണസംഖ്യ  reset_term_props_on_termresponse INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN long override_sysinfo_uptime INIT(= -1);
EXTERN പൂർണ്ണസംഖ്യ  override_autoload INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  override_defcompile INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  ml_get_alloc_വരികൾ INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  ignore_unreachable_code_for_testing INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ  in_free_unref_ഇനങ്ങൾ INIT(= FALSE);
#endif

#ifdef FEAT_TIMERS
EXTERN പൂർണ്ണസംഖ്യ  did_add_timer INIT(= FALSE);
EXTERN പൂർണ്ണസംഖ്യ  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#endif
#ifdef FEAT_EVAL
EXTERN പൂർണ്ണസംഖ്യ  നിവേശം_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_വേര്_T	*lval_വേര് INIT(= ശൂന്യ);
#endif

#ifdef FEAT_BEVAL_TERM
EXTERN പൂർണ്ണസംഖ്യ  bevalexpr_due_set INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#endif

#ifdef FEAT_EVAL
EXTERN time_T time_for_testing INIT(= 0);

EXTERN പൂർണ്ണസംഖ്യ echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN പൂർണ്ണസംഖ്യ  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN പൂർണ്ണസംഖ്യ *eval_lavars_used INIT(= ശൂന്യ);

// Only filled for Win32.
EXTERN അക്ഷരം windowsVersion[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_item;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, GA_EMPTY, GA_EMPTY, ശൂന്യ,
			 {0, 0, (പൂർണ്ണസംഖ്യ)വലുപ്പം(char_u *), 20, ശൂന്യ}, 0, ശൂന്യ, ശൂന്യ}
# endif
	;
#endif

#ifdef MSWIN
EXTERN പൂർണ്ണസംഖ്യ ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= ശൂന്യ);
#endif


#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
EXTERN അക്ഷരം *ch_part_names[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN പൂർണ്ണസംഖ്യ channel_need_redraw INIT(= FALSE);
#endif

#ifdef FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN പൂർണ്ണസംഖ്യ ch_log_output INIT(= FALSE);

EXTERN പൂർണ്ണസംഖ്യ did_repeated_സന്ദേശം INIT(= 0);
# define REPEATED_സന്ദേശം_LOOKING	    1
# define REPEATED_സന്ദേശം_SAFESTATE	    2
#endif

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_SET);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN പൂർണ്ണസംഖ്യ skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN പൂർണ്ണസംഖ്യ skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN പൂർണ്ണസംഖ്യ skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#define SHOWCMD_BUFLEN (SHOWCMD_COLS + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#ifdef FEAT_TERMGUICOLORS
EXTERN പൂർണ്ണസംഖ്യ	p_tgc_set INIT(= FALSE);
#endif

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#ifdef HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#endif

#ifdef FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN അക്ഷരം *wayland_display_name INIT(= ശൂന്യ);

// Don't connect to Wayland compositor if TRUE
EXTERN പൂർണ്ണസംഖ്യ wayland_no_connect INIT(= FALSE);

#endif

#എങ്കിൽ defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
തരനിർവചനം എണ്ണൽ {
    CLIENTSERVER_METHOD_NONE,
# ifdef FEAT_X11
    CLIENTSERVER_METHOD_X11,
# endif
# ifdef MSWIN
    CLIENTSERVER_METHOD_MSWIN,
# endif
# ifdef FEAT_SOCKETSERVER
    CLIENTSERVER_METHOD_SOCKET
# endif
} clientserver_method_T;

EXTERN clientserver_method_T clientserver_method

# ifdef FEAT_X11
INIT(= CLIENTSERVER_METHOD_X11);
# elif defined(MSWIN)
INIT(= CLIENTSERVER_METHOD_MSWIN);
# elif defined(FEAT_SOCKETSERVER)
INIT(= CLIENTSERVER_METHOD_SOCKET);
# അല്ലെങ്കിൽ
INIT(= CLIENTSERVER_METHOD_NONE);
# endif

#endif

#ifdef FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= ശൂന്യ);
#endif

// If the <xOSC> key should be propagated from vgetc()
EXTERN പൂർണ്ണസംഖ്യ allow_osc_താക്കോൽ INIT(= 0);

#ifdef FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= ശൂന്യ);
EXTERN bool inside_redraw_on_start_cb INIT(= false);
#endif

// If greater than zero, then silence the W23/W24 warning.
EXTERN പൂർണ്ണസംഖ്യ silence_w23_w24_സന്ദേശം INIT( = 0);

#ifdef FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T last_തിരുകുക_ga INIT5(0, 0, 1, 64, ശൂന്യ);
EXTERN പൂർണ്ണസംഖ്യ	add_last_തിരുകുക INIT(= 0);
#endif

#എങ്കിൽ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN പൂർണ്ണസംഖ്യ cell_width INIT(= 8);
EXTERN പൂർണ്ണസംഖ്യ cell_height INIT(= 16);
#endif
