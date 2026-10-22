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
EXTERN దీర్ఘ	అడ్డువరుసలు			// nr of rows in the screen
#నిర్వచితమైతే DO_INIT
# ఒకవేళ defined(MSWIN)
		    = 25L
# లేకపోతే
		    = 24L
# endif
#అయితే_ముగింపు
		    ;
EXTERN దీర్ఘ	Columns INIT(= 80);	// nr of columns in the screen

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
EXTERN schar_T	*Screen_పంక్తులు INIT(= ఖాళీ);
EXTERN sattr_T	*ScreenAttrs INIT(= ఖాళీ);
EXTERN colnr_T  *Screen_నిలువుల_సంఖ్య INIT(= ఖాళీ);
EXTERN గుర్తులేని	*పంక్తి_స్థానభ్రంశం INIT(= ఖాళీ);
EXTERN char_u	*పంక్తి_Wraps INIT(= ఖాళీ);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*Screen_పంక్తులు_UC INIT(= ఖాళీ);	// decoded UTF-8 characters
EXTERN u8char_T	*Screen_పంక్తులు_C[MAX_MCO];		// composing characters
EXTERN పూర్ణాంకం	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= ఖాళీ);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_Screen_పంక్తి INIT(= ఖాళీ);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN పూర్ణాంకం	screen_cur_అడ్డువరుస INIT(= 0);
EXTERN పూర్ణాంకం	screen_cur_నిలువు INIT(= 0);

#నిర్వచితమైతే FEAT_శోధన_EXTRA
// used for 'hlsearch' highlight matching
EXTERN సరిపోలిక_T	screen_శోధన_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T శోధన_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN పూర్ణాంకం	no_hlsearch INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#అయితే_ముగింపు

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN పూర్ణాంకం redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN పొట్టి	*Tab_పుట_Idxs INIT(= ఖాళీ);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= ఖాళీ);
EXTERN పూర్ణాంకం	tabline_stl_click_లెక్క INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= ఖాళీ);
EXTERN పూర్ణాంకం	tabpanel_stl_click_లెక్క INIT(= 0);

#నిర్వచితమైతే FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN పొట్టి	*popup_ముసుగు INIT(= ఖాళీ);
EXTERN పొట్టి	*popup_ముసుగు_తదుపరి INIT(= ఖాళీ);
// Array with flags for transparent cells of current popup.
EXTERN అక్షరం	*popup_transparent INIT(= ఖాళీ);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN పూర్ణాంకం	popup_ముసుగు_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_ముసుగు_tab INIT(= ఖాళీ);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN పూర్ణాంకం	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= ఖాళీ);
#అయితే_ముగింపు

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN దీర్ఘ	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN పూర్ణాంకం	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= ఖాళీ);
EXTERN schar_T	*pum_bg_పంక్తులు INIT(= ఖాళీ);
EXTERN u8char_T	*pum_bg_పంక్తులు_UC INIT(= ఖాళీ);
EXTERN u8char_T	*pum_bg_పంక్తులు_C[MAX_MCO];
EXTERN పూర్ణాంకం	pum_bg_శిఖరం INIT(= 0);
EXTERN పూర్ణాంకం	pum_bg_bot INIT(= 0);
EXTERN పూర్ణాంకం	pum_bg_నిలువుల_సంఖ్య INIT(= 0);

EXTERN పూర్ణాంకం	screen_అడ్డువరుసలు INIT(= 0);	    // actual size of ScreenLines[]
EXTERN పూర్ణాంకం	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN పూర్ణాంకం	mod_ముసుగు INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN పూర్ణాంకం	vgetc_mod_ముసుగు INIT(= 0);
EXTERN పూర్ణాంకం	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN పూర్ణాంకం	cmdline_అడ్డువరుస;
EXTERN పూర్ణాంకం	cmdline_నిలువు_off;
EXTERN పూర్ణాంకం	cmdline_వెడల్పు;

EXTERN పూర్ణాంకం	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN పూర్ణాంకం	redraw_బహుళకం INIT(= FALSE);	// mode must be redrawn
EXTERN పూర్ణాంకం	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN పూర్ణాంకం	బహుళకం_displayed INIT(= FALSE);	// mode is being displayed
EXTERN పూర్ణాంకం	no_win_do_పంక్తులు_ins INIT(= FALSE); // don't insert lines
#ఒకవేళ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN పూర్ణాంకం	cmdline_నక్షత్రం INIT(= FALSE);	// cmdline is crypted
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	exec_from_reg INIT(= FALSE);	// executing register

EXTERN పూర్ణాంకం	screen_cleared INIT(= FALSE);	// screen has been cleared

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

EXTERN char_u	*edit_submode INIT(= ఖాళీ); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= ఖాళీ); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= ఖాళీ);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#నిర్వచితమైతే FEAT_RIGHTLEFT
EXTERN పూర్ణాంకం	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	సందేశం_నిలువు;
EXTERN పూర్ణాంకం	సందేశం_అడ్డువరుస;
EXTERN పూర్ణాంకం	సందేశం_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN పూర్ణాంకం	సందేశం_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_సందేశం INIT(= ఖాళీ);	    // msg to be shown after redraw
EXTERN పూర్ణాంకం	keep_సందేశం_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN పూర్ణాంకం	keep_సందేశం_మరిన్ని INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN పూర్ణాంకం	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN పూర్ణాంకం	సందేశం_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN పూర్ణాంకం	సందేశం_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN పూర్ణాంకం	సందేశం_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN పూర్ణాంకం	సందేశం_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN పూర్ణాంకం	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN పూర్ణాంకం	info_సందేశం INIT(= FALSE); // printing informative message
EXTERN పూర్ణాంకం      సందేశం_hist_off INIT(= FALSE); // don't add messages to history
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN పూర్ణాంకం	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN పూర్ణాంకం	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_సందేశం INIT(= ఖాళీ);
EXTERN దీర్ఘ	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= ఖాళీ);

EXTERN పూర్ణాంకం	did_endif INIT(= FALSE);    // just had ":endif"
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN పూర్ణాంకం	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN పూర్ణాంకం	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN పూర్ణాంకం	called_vim_beep;	    // set if vim_beep() is called
EXTERN పూర్ణాంకం	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN పూర్ణాంకం	called_emsg;		    // always incremented by emsg()
EXTERN పూర్ణాంకం	లోపల_echowindow;		    // executing ":echowindow"
EXTERN పూర్ణాంకం	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN పూర్ణాంకం	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN పూర్ణాంకం	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN పూర్ణాంకం	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN పూర్ణాంకం	need_wait_return INIT(= 0); // need to wait for return later
EXTERN పూర్ణాంకం	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN పూర్ణాంకం	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN పూర్ణాంకం	quit_మరిన్ని INIT(= FALSE);    // 'q' hit at "--more--" msg
#ఒకవేళ defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN పూర్ణాంకం	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN పూర్ణాంకం	intr_char INIT(= 0);	    // extra interrupt character
#అయితే_ముగింపు
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN పూర్ణాంకం	x_no_connect INIT(= FALSE); // don't connect to X server
# ఒకవేళ defined(FEAT_CLIENTSERVER)
EXTERN పూర్ణాంకం	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN పూర్ణాంకం	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN పూర్ణాంకం	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN పూర్ణాంకం	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN పూర్ణాంకం	పంక్తులు_ఎడమ INIT(= -1);	    // lines left for listing
EXTERN పూర్ణాంకం	సందేశం_no_మరిన్ని INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, పరిమాణం(estack_T), 50, ఖాళీ);
#నిర్వచించు HAVE_SOURCING_INFO  (exestack.ga_దత్తాంశం != ఖాళీ && exestack.ga_len > 0)
// name of error message source
#నిర్వచించు SOURCING_NAME (((estack_T *)exestack.ga_దత్తాంశం)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#నిర్వచించు SOURCING_LNUM (((estack_T *)exestack.ga_దత్తాంశం)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#నిర్వచితమైతే FEAT_EVAL
    INIT4(0, 0, 0, 0);
#లేకపోతే
    INIT(= {0});
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
// whether inside compile_def_function()
EXTERN పూర్ణాంకం	estack_compiling INIT(= FALSE);

EXTERN పూర్ణాంకం	ex_nesting_level INIT(= 0);	// nesting level
EXTERN పూర్ణాంకం	debug_break_level INIT(= -1);	// break below this level
EXTERN పూర్ణాంకం	debug_did_సందేశం INIT(= FALSE);	// did "debug mode" message
EXTERN పూర్ణాంకం	debug_tick INIT(= 0);		// breakpoint change count
EXTERN పూర్ణాంకం	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN పూర్ణాంకం	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_వస్తువులు INIT5(0, 0, పరిమాణం(scriptitem_T *), 20, ఖాళీ);
# define SCRIPT_అంశం(id)    (((scriptitem_T **)script_వస్తువులు.ga_దత్తాంశం)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_వస్తువులు.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_అంశం(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_పంక్తులు.ga_దత్తాంశం))[j]

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
EXTERN పూర్ణాంకం did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN పూర్ణాంకం need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN పూర్ణాంకం check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN పూర్ణాంకం trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN పూర్ణాంకం force_abort INIT(= FALSE);

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
EXTERN msglist_T **సందేశం_list INIT(= ఖాళీ);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN పూర్ణాంకం suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_దొంతర INIT(= ఖాళీ);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN పూర్ణాంకం	may_garbage_collect INIT(= FALSE);
EXTERN పూర్ణాంకం	want_garbage_collect INIT(= FALSE);
EXTERN పూర్ణాంకం	garbage_collect_at_exit INIT(= FALSE);


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

# define t_సంఖ్య		(static_types[12])
# define t_const_సంఖ్య		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_సంఖ్య_bool		(static_types[14])
# define t_const_సంఖ్య_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_సంఖ్య_float		(static_types[16])
# define t_const_సంఖ్య_float	(static_types[17])

# define t_float			(static_types[18])
# define t_const_float		(static_types[19])

# define t_string		(static_types[20])
# define t_const_string		(static_types[21])

# define t_blob			(static_types[22])
# define t_const_blob		(static_types[23])

# define t_blob_null		(static_types[24])
# define t_const_blob_null	(static_types[25])

# define t_పని			(static_types[26])
# define t_const_పని		(static_types[27])

# define t_channel		(static_types[28])
# define t_const_channel		(static_types[29])

// t_number_or_string - Special value used for @#.
# define t_సంఖ్య_or_string	(static_types[30])
# define t_const_సంఖ్య_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_సంఖ్య		(static_types[38])
# define t_const_func_సంఖ్య	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_సంఖ్య		(static_types[48])
# define t_const_func_0_సంఖ్య	(static_types[49])

# define t_func_0_string		(static_types[50])
# define t_const_func_0_string	(static_types[51])

# define t_list_any		(static_types[52])
# define t_const_list_any	(static_types[53])

# define t_dict_any		(static_types[54])
# define t_const_dict_any	(static_types[55])

# define t_list_ఖాళీలు		(static_types[56])
# define t_const_list_ఖాళీలు	(static_types[57])

# define t_dict_ఖాళీలు		(static_types[58])
# define t_const_dict_ఖాళీలు	(static_types[59])

# define t_list_bool		(static_types[60])
# define t_const_list_bool	(static_types[61])

# define t_list_సంఖ్య		(static_types[62])
# define t_const_list_సంఖ్య	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_పని		(static_types[66])
# define t_const_list_పని	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_సంఖ్య	(static_types[72])
# define t_const_list_list_సంఖ్య (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_సంఖ్య	(static_types[76])
# define t_const_list_list_list_సంఖ్య (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_సంఖ్య		(static_types[80])
# define t_const_dict_సంఖ్య	(static_types[81])

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

# define t_tuple_ఖాళీలు		(static_types[94])
# define t_const_tuple_ఖాళీలు	(static_types[95])


EXTERN type_T static_types[96]
# ifdef DO_INIT
= {
    // 0: t_unknown
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 12: t_number
    {VAR_సంఖ్య, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_సంఖ్య, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 14: t_number_bool
    {VAR_సంఖ్య, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_సంఖ్య, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 16: t_number_float
    {VAR_సంఖ్య, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_సంఖ్య, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, ఖాళీ, ఖాళీ},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ఖాళీ, ఖాళీ},

    // 26: t_job
    {VAR_పని, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_పని, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, ఖాళీ, ఖాళీ},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ఖాళీ, ఖాళీ},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, ఖాళీ, ఖాళీ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ఖాళీ, ఖాళీ},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, ఖాళీ, ఖాళీ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ఖాళీ, ఖాళీ},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_సంఖ్య, ఖాళీ, ఖాళీ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_సంఖ్య, ఖాళీ, ఖాళీ},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, ఖాళీ, ఖాళీ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ఖాళీ, ఖాళీ},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, ఖాళీ, ఖాళీ},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ఖాళీ, ఖాళీ},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, ఖాళీ, ఖాళీ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, ఖాళీ, ఖాళీ},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, ఖాళీ, ఖాళీ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ఖాళీ, ఖాళీ},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_సంఖ్య, ఖాళీ, ఖాళీ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_సంఖ్య, ఖాళీ, ఖాళీ},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, ఖాళీ, ఖాళీ},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ఖాళీ, ఖాళీ},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ఖాళీ, ఖాళీ},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, ఖాళీ, ఖాళీ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, ఖాళీ, ఖాళీ},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ఖాళీ, ఖాళీ},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, ఖాళీ, ఖాళీ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, ఖాళీ, ఖాళీ},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ఖాళీ, ఖాళీ},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_సంఖ్య, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_సంఖ్య, ఖాళీ, ఖాళీ},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ఖాళీ, ఖాళీ},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_పని, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_పని, ఖాళీ, ఖాళీ},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, ఖాళీ, ఖాళీ},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, ఖాళీ, ఖాళీ},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_సంఖ్య, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_సంఖ్య, ఖాళీ, ఖాళీ},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, ఖాళీ, ఖాళీ},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_సంఖ్య, ఖాళీ, ఖాళీ},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_సంఖ్య, ఖాళీ, ఖాళీ},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, ఖాళీ, ఖాళీ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ఖాళీ, ఖాళీ},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_సంఖ్య, ఖాళీ, ఖాళీ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_సంఖ్య, ఖాళీ, ఖాళీ},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, ఖాళీ, ఖాళీ},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, ఖాళీ, ఖాళీ},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, ఖాళీ, ఖాళీ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, ఖాళీ, ఖాళీ},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, ఖాళీ, ఖాళీ, ఖాళీ},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, ఖాళీ, ఖాళీ, ఖాళీ},
}
# endif
;

EXTERN పూర్ణాంకం	did_ఆధారం_packages INIT(= FALSE);
#అయితే_ముగింపు // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN పూర్ణాంకం	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN పూర్ణాంకం	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN పూర్ణాంకం include_none INIT(= 0);	// when 1 include "None"
EXTERN పూర్ణాంకం include_default INIT(= 0);	// when 1 include "default"
EXTERN పూర్ణాంకం include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN పూర్ణాంకం	highlight_సరిపోలిక INIT(= FALSE);	// show search match pos
EXTERN linenr_T	శోధన_సరిపోలిక_పంక్తులు;		// lines of matched string
EXTERN colnr_T	శోధన_సరిపోలిక_endcol;		// col nr of match end
#నిర్వచితమైతే FEAT_శోధన_EXTRA
EXTERN linenr_T	శోధన_మొదటి_పంక్తి INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	శోధన_చివరి_పంక్తి INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN పూర్ణాంకం	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN పూర్ణాంకం	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN పూర్ణాంకం	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN పూర్ణాంకం	highlight_attr[HLF_లెక్క];  // Highl. attr for each context.
#నిర్వచితమైతే FEAT_STL_OPT
# define USER_HIGHLIGHT
#అయితే_ముగింపు
#నిర్వచితమైతే USER_HIGHLIGHT
EXTERN పూర్ణాంకం	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN పూర్ణాంకం	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN పూర్ణాంకం	highlight_stlterm[9];		// On top of user
EXTERN పూర్ణాంకం	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN పూర్ణాంకం	skip_పదం_loop INIT(= FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= ఖాళీ);	// "-U" cmdline argument
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	cterm_సాధారణ_fg_color INIT(= 0);
EXTERN పూర్ణాంకం	cterm_సాధారణ_fg_bold INIT(= 0);
EXTERN పూర్ణాంకం	cterm_సాధారణ_bg_color INIT(= 0);
EXTERN పూర్ణాంకం	cterm_సాధారణ_ul_color INIT(= 0);
#నిర్వచితమైతే FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_సాధారణ_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_సాధారణ_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_సాధారణ_ul_gui_color INIT(= INVALCOLOR);
#అయితే_ముగింపు
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#నిర్వచితమైతే FEAT_TERMRESPONSE
EXTERN పూర్ణాంకం	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN పూర్ణాంకం	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN పూర్ణాంకం	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN పూర్ణాంకం	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(ఖాళీ, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= ఖాళీ);
EXTERN win_T	*au_pending_free_win INIT(= ఖాళీ);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN పూర్ణాంకం	mouse_అడ్డువరుస;
EXTERN పూర్ణాంకం	mouse_నిలువు;
EXTERN పూర్ణాంకం	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN పూర్ణాంకం	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN పూర్ణాంకం	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#ఒకవేళ defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN పూర్ణాంకం	WantQueryMouse INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN పూర్ణాంకం	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_prev_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN పూర్ణాంకం	gui_prev_topfill INIT(= 0);
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSESHAPE
EXTERN పూర్ణాంకం	drag_స్థితి_పంక్తి INIT(= FALSE);	// dragging the status line
EXTERN పూర్ణాంకం	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN పూర్ణాంకం	drag_sep_పంక్తి INIT(= FALSE);	// dragging vert separator
#అయితే_ముగింపు


#నిర్వచితమైతే FEAT_DIFF
// Value set from 'diffopt'.
EXTERN పూర్ణాంకం	diff_context INIT(= 6);		// context for folds
EXTERN పూర్ణాంకం      linematch_పంక్తులు INIT(= 0);      // number of lines for diff line match
EXTERN పూర్ణాంకం	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN పూర్ణాంకం	diff_need_scrollbind INIT(= FALSE);
#అయితే_ముగింపు

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN పూర్ణాంకం	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN పూర్ణాంకం	redraw_not_allowed INIT(= FALSE);

#నిర్వచితమైతే సందేశం_వరుస
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN పూర్ణాంకం	dont_parse_messages INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*మూలం_menu INIT(= ఖాళీ);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN పూర్ణాంకం	sys_menu INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN పూర్ణాంకం force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN పూర్ణాంకం	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN పూర్ణాంకం	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN పూర్ణాంకం	current_scrollbar;
EXTERN long_u	scrollbar_విలువ;

// found "-rv" or "-reverse" in command line args
EXTERN పూర్ణాంకం	దొరికింది_తిరగేయి_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN అక్షరం	*font_వాదం INIT(= ఖాళీ);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN అక్షరం	*background_వాదం INIT(= ఖాళీ);

// "-fg" or "-foreground" command line argument
EXTERN అక్షరం	*foreground_వాదం INIT(= ఖాళీ);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN అస్థిర sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN పూర్ణాంకం	new_pixel_వెడల్పు INIT(= 0);
EXTERN పూర్ణాంకం	new_pixel_ఎత్తు INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN పూర్ణాంకం	gui_win_x INIT(= -1);
EXTERN పూర్ణాంకం	gui_win_y INIT(= -1);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD
EXTERN Clipboard_T clip_నక్షత్రం;	// PRIMARY selection in X11/Wayland
# ఒకవేళ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# లేకపోతే
#  define clip_plus clip_నక్షత్రం	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN పూర్ణాంకం	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN పూర్ణాంకం	clip_autoselect_నక్షత్రం INIT(= FALSE);
EXTERN పూర్ణాంకం	clip_autoselect_plus INIT(= FALSE);
EXTERN పూర్ణాంకం	clip_autoselectml INIT(= FALSE);
EXTERN పూర్ణాంకం	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= ఖాళీ);
EXTERN పూర్ణాంకం	clip_unnamed_saved INIT(= 0);
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= ఖాళీ);
#అయితే_ముగింపు


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= ఖాళీ);	// previous window (may equal curwin)
#నిర్వచించు ONE_కిటికీ (firstwin == lastwin)
#నిర్వచించు W_తదుపరి(wp) ((wp)->w_తదుపరి)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#నిర్వచించు AUCMD_WIN_లెక్క 5

రకనిర్వచనం నిర్మాణం {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    పూర్ణాంకం		auc_win_వాడిన;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_లెక్క];

#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN win_T    *మొదటి_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= ఖాళీ);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN పూర్ణాంకం	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN పూర్ణాంకం	popup_ఉపయోగిస్తుంది_mouse_move INIT(= FALSE);

EXTERN పూర్ణాంకం	పాఠ్యం_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN పూర్ణాంకం	ignore_పాఠ్యం_props INIT(= FALSE);
#అయితే_ముగింపు

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN పూర్ణాంకం	pum_will_redraw INIT(= FALSE);

/*
 * The window layout is kept in a tree of frames.  topframe points to the top
 * of the tree.
 */
EXTERN చట్రం_T	*topframe;	// top of the window frame tree

/*
 * Tab pages are alternative topframes.  "first_tabpage" points to the first
 * one in the list, "curtab" is the current one. "lastused_tabpage" is the
 * last used one.
 */
EXTERN tabpage_T    *మొదటి_tabpage;
EXTERN tabpage_T    *curtab;
EXTERN tabpage_T    *lastused_tabpage;
EXTERN పూర్ణాంకం	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN పూర్ణాంకం	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#ఒకవేళ defined(FEAT_TABPANEL)
EXTERN పూర్ణాంకం	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#అయితే_ముగింపు

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= ఖాళీ);	// first buffer
EXTERN buf_T	*lastbuf INIT(= ఖాళీ);	// last buffer
EXTERN buf_T	*curbuf INIT(= ఖాళీ);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN పూర్ణాంకం	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN పూర్ణాంకం	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN పూర్ణాంకం	arg_had_చివరి INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN పూర్ణాంకం	ru_నిలువు;		// column for ruler
#నిర్వచితమైతే FEAT_STL_OPT
EXTERN పూర్ణాంకం	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	sc_నిలువు;		// column for shown command

#నిర్వచితమైతే TEMPDIRNAMES
# ఒకవేళ defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= ఖాళీ); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= ఖాళీ); // Name of Vim's own temp dir.
					   // Ends in a slash.
#అయితే_ముగింపు

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN పూర్ణాంకం	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN పూర్ణాంకం	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN పూర్ణాంకం	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN పూర్ణాంకం	v_dying INIT(= 0); // internal value of v:dying
EXTERN పూర్ణాంకం	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#ఒకవేళ defined(FEAT_AUTOCHDIR)
EXTERN పూర్ణాంకం	test_autochdir INIT(= FALSE);
#అయితే_ముగింపు
EXTERN అక్షరం	*చివరి_chdir_reason INIT(= ఖాళీ);
#ఒకవేళ defined(EXITFREE)
EXTERN పూర్ణాంకం	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#అయితే_ముగింపు
// volatile because it is used in signal handler deathtrap().
EXTERN అస్థిర sig_atomic_t నిండిన_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN పూర్ణాంకం	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN పూర్ణాంకం	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN పూర్ణాంకం	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN పూర్ణాంకం	curbuf_తాళం INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN పూర్ణాంకం	allbuf_తాళం INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#నిర్వచితమైతే HAVE_SANDBOX
EXTERN పూర్ణాంకం	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	silent_బహుళకం INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN పూర్ణాంకం	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#అయితే_ముగింపు

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN పూర్ణాంకం	VIsual_చురుకైన INIT(= FALSE);
				// whether Visual mode is active
EXTERN పూర్ణాంకం	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN పూర్ణాంకం	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN పూర్ణాంకం  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN పూర్ణాంకం	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN పూర్ణాంకం	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN పూర్ణాంకం	VIsual_బహుళకం INIT(= 'v');
				// type of Visual mode

EXTERN పూర్ణాంకం	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN పూర్ణాంకం	resel_VIsual_బహుళకం INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_పంక్తి_లెక్క;	// number of lines
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
EXTERN పూర్ణాంకం     did_ai INIT(= FALSE);

/*
 * Column of first char after autoindent.  0 when no autoindent done.  Used
 * when 'backspace' is 0, to avoid backspacing over autoindent.
 */
EXTERN colnr_T	ai_నిలువు INIT(= 0);

/*
 * This is a character which will end a start-middle-end comment when typed as
 * the first character on a new line.  It is taken from the last character of
 * the "end" comment leader when the COM_AUTO_END flag is given for that
 * comment end in 'comments'.  It is only valid when did_ai is TRUE.
 */
EXTERN పూర్ణాంకం     చివర_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN పూర్ణాంకం     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN పూర్ణాంకం	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN పూర్ణాంకం	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN పూర్ణాంకం	can_si_వెనుక INIT(= FALSE);

EXTERN పూర్ణాంకం	old_indent INIT(= 0);	// for ^^D command in insert mode

EXTERN pos_T	saved_cursor		// w_cursor before formatting text.
#నిర్వచితమైతే DO_INIT
		    = {0, 0, 0}
#అయితే_ముగింపు
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
EXTERN పూర్ణాంకం	orig_పంక్తి_లెక్క INIT(= 0);  // Line count when "gR" started
EXTERN పూర్ణాంకం	vr_పంక్తులు_మారింది INIT(= 0); // #Lines changed by "gR" so far

#ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
// argument to SETJMP() for handling X IO errors
EXTERN JMP_BUF x_jump_env;
#అయితే_ముగింపు

/*
 * These flags are set based upon 'fileencoding'.
 * Note that "enc_utf8" is also set for "unicode", because the characters are
 * internally stored as UTF-8 (to avoid trouble with NUL bytes).
 */
#నిర్వచించు DBCS_JPN	932	// japan
#నిర్వచించు DBCS_JPNU	9932	// euc-jp
#నిర్వచించు DBCS_KOR	949	// korea
#నిర్వచించు DBCS_KORU	9949	// euc-kr
#నిర్వచించు DBCS_CHS	936	// chinese
#నిర్వచించు DBCS_CHSU	9936	// euc-cn
#నిర్వచించు DBCS_CHT	950	// taiwan
#నిర్వచించు DBCS_CHTU	9950	// euc-tw
#నిర్వచించు DBCS_2BYTE	1	// 2byte-
#నిర్వచించు DBCS_DEBUG	(-1)

EXTERN పూర్ణాంకం	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN పూర్ణాంకం	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN పూర్ణాంకం	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN పూర్ణాంకం	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#ఒకవేళ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN పూర్ణాంకం	enc_codepage INIT(= -1);
EXTERN పూర్ణాంకం	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN అక్షరం	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T ఆదానం_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN పూర్ణాంకం (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN పూర్ణాంకం (*mb_ptr2len_len)(char_u *p, పూర్ణాంకం కొలత) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN పూర్ణాంకం (*mb_char2len)(పూర్ణాంకం c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN పూర్ణాంకం (*mb_char2bytes)(పూర్ణాంకం c, char_u *buf) INIT(= latin_char2bytes);

EXTERN పూర్ణాంకం (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN పూర్ణాంకం (*mb_ptr2cells_len)(char_u *p, పూర్ణాంకం కొలత) INIT(= latin_ptr2cells_len);
EXTERN పూర్ణాంకం (*mb_char2cells)(పూర్ణాంకం c) INIT(= latin_char2cells);
EXTERN పూర్ణాంకం (*mb_off2cells)(గుర్తులేని off, గుర్తులేని max_off) INIT(= latin_off2cells);
EXTERN పూర్ణాంకం (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN పూర్ణాంకం (*mb_తల_off)(char_u *పునాది, char_u *p) INIT(= latin_తల_off);

#ఒకవేళ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, స్థిరరాశి అక్షరం **inbuf, size_t *inbytesleft, అక్షరం **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (స్థిరరాశి అక్షరం *tocode, స్థిరరాశి అక్షరం *fromcode);
EXTERN పూర్ణాంకం (*iconv_close) (iconv_t cd);
EXTERN పూర్ణాంకం (*iconvctl) (iconv_t cd, పూర్ణాంకం request, శూన్య *వాదం);
EXTERN పూర్ణాంకం* (*iconv_errno) (శూన్య);
#అయితే_ముగింపు


#నిర్వచితమైతే FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= ఖాళీ);
/*
 * Start and end column of the preedit area in virtual columns from the start
 * of the text line.  When there is no preedit area they are set to MAXCOL.
 * "preedit_end_col" is needed for coloring the preedited string.  Drawing the
 * color between "preedit_start_col" and curpos did not work, because some XIM
 * set the cursor position to the first char of the string.
 */
EXTERN colnr_T		preedit_ప్రారంభం_నిలువు INIT(= MAXCOL);
EXTERN colnr_T		preedit_చివర_నిలువు INIT(= MAXCOL);

// "xim_changed_while_preediting" is set when changed() can set the 'modified'
// flag even while preediting.
EXTERN పూర్ణాంకం		xim_మారింది_while_preediting INIT(= FALSE);
# లేకపోతే
EXTERN XIC		xic INIT(= ఖాళీ);
# endif
# ifdef FEAT_GUI
EXTERN guicolor_T	xim_fg_color INIT(= INVALCOLOR);
EXTERN guicolor_T	xim_bg_color INIT(= INVALCOLOR);
# endif
#అయితే_ముగింపు

/*
 * "State" is the main state of Vim.
 * There are other variables that modify the state:
 * "Visual_mode"    When State is MODE_NORMAL or MODE_INSERT.
 * "finish_op"	    When State is MODE_NORMAL, after typing the operator and
 *		    before typing the motion command.
 * "motion_force"   Last motion_force  from do_pending_operator()
 * "debug_mode"	    Debug mode.
 */
EXTERN పూర్ణాంకం	State INIT(= బహుళకం_సాధారణ);

#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	debug_బహుళకం INIT(= FALSE);
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	ముగింపు_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN దీర్ఘ	opcount INIT(= 0);	// count for pending operator
EXTERN పూర్ణాంకం	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN పూర్ణాంకం exmode_చురుకైన INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN పూర్ణాంకం pending_exmode_చురుకైన INIT(= FALSE);

EXTERN పూర్ణాంకం ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN పూర్ణాంకం reg_recording INIT(= 0);	// register for recording  or zero
EXTERN పూర్ణాంకం reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN పూర్ణాంకం pending_చివర_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN పూర్ణాంకం చూసిన_Modify_Other_కీలకాలు INIT(= FALSE);

// The state for the modifyOtherKeys level
రకనిర్వచనం గణన {
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
రకనిర్వచనం గణన {
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

EXTERN పూర్ణాంకం no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN పూర్ణాంకం no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN పూర్ణాంకం allow_కీలకాలు INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN పూర్ణాంకం no_reduce_కీలకాలు INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN పూర్ణాంకం no_u_sync INIT(= 0);		// Don't call u_sync()
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#అయితే_ముగింపు

EXTERN పూర్ణాంకం restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN పూర్ణాంకం arrow_వాడిన;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN పూర్ణాంకం	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN పూర్ణాంకం	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#నిర్వచితమైతే USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ON_FLY_SCROLL
EXTERN పూర్ణాంకం	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN పూర్ణాంకం	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN పూర్ణాంకం	sticky_cmdmod_flags INIT(= 0); // used by :execute

#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	is_export INIT(= FALSE);    // :export {cmd}
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	సందేశం_silent INIT(= 0);	// don't print messages
EXTERN పూర్ణాంకం	emsg_silent INIT(= 0);	// don't print error messages
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN పూర్ణాంకం	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN పూర్ణాంకం	లోపల_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN పూర్ణాంకం	మార్పిడి_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN పూర్ణాంకం	మార్పిడి_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN అక్షరం	సందేశం_buf[సందేశం_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN పూర్ణాంకం	RedrawingDisabled INIT(= 0);

EXTERN పూర్ణాంకం	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN పూర్ణాంకం	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#నిర్వచితమైతే DO_INIT
		    = {ఖాళీ, ఖాళీ, 0, 0, 0, 0, 0, 0, 0}
#అయితే_ముగింపు
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN పూర్ణాంకం	typebuf_was_ఖాళీలు INIT(= FALSE);

EXTERN పూర్ణాంకం	ex_సాధారణ_busy INIT(= 0);   // recursiveness of ex_normal()
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	లోపల_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	ex_సాధారణ_తాళం INIT(= 0);   // forbid use of ex_normal()

#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం	ignore_script INIT(= FALSE);  // ignore script input
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	stop_చొప్పించు_బహుళకం;	// for ":stopinsert" and 'insertmode'

EXTERN పూర్ణాంకం	కీలకం_Typed;		// TRUE if user typed current char
EXTERN పూర్ణాంకం	కీలకం_Stuffed;		// TRUE if current char from stuffbuf
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
EXTERN పూర్ణాంకం	vgetc_im_చురుకైన;	// Input Method was active for last
					// character obtained from vgetc()
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN పూర్ణాంకం	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN పూర్ణాంకం	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN పూర్ణాంకం	do_redraw INIT(= FALSE);    // extra redraw once
#నిర్వచితమైతే FEAT_DIFF
EXTERN పూర్ణాంకం	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN పూర్ణాంకం	redrawtime_హద్దు_సమితి INIT(= FALSE);
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	need_highlight_మారింది INIT(= TRUE);

#నిర్వచించు NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN పూర్ణాంకం	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= ఖాళీ);   // stream to write script to
EXTERN పూర్ణాంకం	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN అస్థిర sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN అస్థిర sig_atomic_t got_sigusr1 INIT(= FALSE);

#నిర్వచితమైతే USE_పదం_CONSOLE
EXTERN పూర్ణాంకం	పదం_console INIT(= FALSE); // set to TRUE when console used
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	termcap_చురుకైన INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN పూర్ణాంకం	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN పూర్ణాంకం	searchcmdlen;		    // length of previous search cmd
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN పూర్ణాంకం	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_లోపల INIT(= ఖాళీ); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_బహిర్గతం INIT(= ఖాళీ); // Set by vim_regexec()
					    // to store \z\(...\) matches
#అయితే_ముగింపు

EXTERN పూర్ణాంకం	did_outofmem_సందేశం INIT(= FALSE);
					    // set after out of memory msg
EXTERN పూర్ణాంకం	did_swapwrite_సందేశం INIT(= FALSE);
					    // set after swap write error msg
EXTERN పూర్ణాంకం	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN పూర్ణాంకం	global_busy INIT(= 0);	    // set when :global is executing
EXTERN పూర్ణాంకం	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN పూర్ణాంకం	need_ప్రారంభం_insertmode INIT(= FALSE);
					    // start insert mode soon
#ఒకవేళ defined(FEAT_EVAL)
EXTERN char_u	చివరి_బహుళకం[బహుళకం_MAX_నిడివి] INIT(= "n"); // for ModeChanged event
#అయితే_ముగింపు
EXTERN char_u	*చివరి_cmdline INIT(= ఖాళీ); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= ఖాళీ); // command line for "."
EXTERN char_u	*new_చివరి_cmdline INIT(= ఖాళీ);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= ఖాళీ); // fname for <afile> on cmdline
EXTERN పూర్ణాంకం	autocmd_fname_నిండిన;	     // autocmd_fname is full path
EXTERN పూర్ణాంకం	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_సరిపోలిక INIT(= ఖాళీ); // name for <amatch> on cmdline
EXTERN పూర్ణాంకం	aucmd_cmdline_మారింది_లెక్క INIT(= 0);

EXTERN పూర్ణాంకం	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	చివరి_cursormoved	      // for CursorMoved event
#నిర్వచితమైతే DO_INIT
		    = {0, 0, 0}
#అయితే_ముగింపు
		    ;

EXTERN పూర్ణాంకం	postponed_విభజించు INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN పూర్ణాంకం	postponed_విభజించు_flags INIT(= 0);  // args for win_split()
EXTERN పూర్ణాంకం	postponed_విభజించు_tab INIT(= 0);  // cmdmod.cmod_tab
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN పూర్ణాంకం	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	g_ట్యాగ్_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN పూర్ణాంకం	replace_స్థానభ్రంశం INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN పూర్ణాంకం	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*ఖాళీలు_option INIT(= (char_u *)"");

EXTERN పూర్ణాంకం  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= ఖాళీ);	// message redirection file
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం  redir_reg INIT(= 0);	// message redirection register
EXTERN పూర్ణాంకం  redir_vname INIT(= 0);	// message redirection variable
EXTERN పూర్ణాంకం  redir_execute INIT(= 0);	// execute() redirection
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#అయితే_ముగింపు

EXTERN పూర్ణాంకం  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN పూర్ణాంకం  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN పూర్ణాంకం  wild_menu_showing INIT(= 0);
#నిర్వచించు WM_SHOWN	1		// wildmenu showing
#నిర్వచించు WM_SCROLLED	2		// wildmenu showing with scroll

#నిర్వచితమైతే MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN పూర్ణాంకం	దొరికింది_రిజిస్టరు_arg INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_LINEBREAK
EXTERN అక్షరం	breakat_flags[256];	// which characters are in 'breakat'
#అయితే_ముగింపు

// These are in version.c, call init_longVersion() before use.
బాహ్య అక్షరం *సంస్కరణ;
#ఒకవేళ defined(HAVE_DATE_సమయం) && defined(VMS) && defined(VAXC)
బాహ్య అక్షరం long_సంస్కరణ[];
#లేకపోతే
బాహ్య అక్షరం *long_సంస్కరణ;
#అయితే_ముగింపు

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#నిర్వచితమైతే HAVE_PATHDEF
బాహ్య char_u *default_vim_dir;
బాహ్య char_u *default_vimruntime_dir;
బాహ్య char_u *all_cflags;
బాహ్య char_u *all_lflags;
# ifdef VMS
బాహ్య char_u *compiler_సంస్కరణ;
బాహ్య char_u *compiled_arch;
బాహ్య char_u *compiled_vers;
# endif
బాహ్య char_u *compiled_user;
బాహ్య char_u *compiled_sys;
#అయితే_ముగింపు

EXTERN char_u	*homedir INIT(= ఖాళీ);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= ఖాళీ);

#నిర్వచితమైతే FEAT_FOLDING
EXTERN పూర్ణాంకం	disable_fold_update INIT(= 0);
#అయితే_ముగింపు

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN పూర్ణాంకం	km_stopsel INIT(= FALSE);
EXTERN పూర్ణాంకం	km_startsel INIT(= FALSE);

EXTERN పూర్ణాంకం	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN పూర్ణాంకం	cmdwin_ఫలితం INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= ఖాళీ); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= ఖాళీ); // window of cmdline window or NULL

EXTERN char_u no_పంక్తులు_సందేశం[]	INIT(= N_("--No lines in buffer--"));

EXTERN అక్షరం typename_unknown[]	INIT(= N_("unknown"));
EXTERN అక్షరం typename_int[]	INIT(= "int");
EXTERN అక్షరం typename_longint[]	INIT(= "long int");
EXTERN అక్షరం typename_longlongint[]	INIT(= "long long int");
EXTERN అక్షరం typename_unsignedint[]	INIT(= "unsigned int");
EXTERN అక్షరం typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN అక్షరం typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN అక్షరం typename_సూచిక[]	INIT(= N_("pointer"));
EXTERN అక్షరం typename_percent[]	INIT(= N_("percent"));
EXTERN అక్షరం typename_char[] INIT(= "char");
EXTERN అక్షరం typename_string[]	INIT(= N_("string"));
EXTERN అక్షరం typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN దీర్ఘ	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#నిర్వచితమైతే FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN నిర్మాణం subs_సమాసం_S	*substitute_instr INIT(= ఖాళీ);
#అయితే_ముగింపు

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#ఒకవేళ defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_లోపల_ICON	1
# define STL_లోపల_TITLE	2
EXTERN పూర్ణాంకం      stl_syntax INIT(= 0);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= ఖాళీ);
EXTERN పూర్ణాంకం		balloon_Eval_For_పదం INIT(= FALSE);
# ఒకవేళ defined(FEAT_NETBEANS_INTG)
EXTERN పూర్ణాంకం bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే CURSOR_SHAPE
// the table is in misc2.c, because of initializations
బాహ్య cursorentry_T shape_పట్టిక[SHAPE_IDX_లెక్క];
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_శిఖరం		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_ఎడమ		2
# define OPT_PRINT_కుడి	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_సంఖ్య	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_అంకె_OPTIONS	15

EXTERN option_పట్టిక_T printer_opts[OPT_PRINT_అంకె_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, ఖాళీ, 0, FALSE},
	{"bottom",	TRUE, 0, ఖాళీ, 0, FALSE},
	{"left",	TRUE, 0, ఖాళీ, 0, FALSE},
	{"right",	TRUE, 0, ఖాళీ, 0, FALSE},
	{"header",	TRUE, 0, ఖాళీ, 0, FALSE},
	{"syntax",	FALSE, 0, ఖాళీ, 0, FALSE},
	{"number",	FALSE, 0, ఖాళీ, 0, FALSE},
	{"wrap",	FALSE, 0, ఖాళీ, 0, FALSE},
	{"duplex",	FALSE, 0, ఖాళీ, 0, FALSE},
	{"portrait", FALSE, 0, ఖాళీ, 0, FALSE},
	{"paper",	FALSE, 0, ఖాళీ, 0, FALSE},
	{"collate",	FALSE, 0, ఖాళీ, 0, FALSE},
	{"jobsplit", FALSE, 0, ఖాళీ, 0, FALSE},
	{"formfeed", FALSE, 0, ఖాళీ, 0, FALSE},
	{"format", FALSE, 0, ఖాళీ, 0, FALSE},
    }
# endif
    ;

// For prt_get_unit().
# define PRT_UNIT_NONE	-1
# define PRT_UNIT_PERC	0
# define PRT_UNIT_INCH	1
# define PRT_UNIT_MM	2
# define PRT_UNIT_బిందువు	3
# define PRT_UNIT_పేర్లు {"pc", "in", "mm", "pt"}
#అయితే_ముగింపు

#ఒకవేళ (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_పుట_అంకె;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_XCLIPBOARD
// xterm display name
EXTERN అక్షరం	*xterm_display INIT(= ఖాళీ);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN పూర్ణాంకం	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= ఖాళీ);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)ఖాళీ);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN పూర్ణాంకం	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN పూర్ణాంకం	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= ఖాళీ);	// name of the server
# ifdef FEAT_X11
EXTERN కిటికీ	comm_కిటికీ INIT(= None);
EXTERN కిటికీ	client_కిటికీ INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*server_Delayed_ప్రారంభం_Name INIT(= ఖాళీ);
# elif defined(MSWIN)
EXTERN HWND	client_కిటికీ INIT(= 0);
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX) || defined(VMS)
EXTERN పూర్ణాంకం	పదం_is_xterm INIT(= FALSE);	// xterm-like 'term'
#అయితే_ముగింపు

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
EXTERN అక్షరం	psepc INIT(= '\\');	// normal path separator character
EXTERN అక్షరం	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN అక్షరం	pseps[2] INIT2('\\', 0);
#అయితే_ముగింపు

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN పూర్ణాంకం	virtual_op INIT(= MAYBE);

#నిర్వచితమైతే FEAT_SYN_HL
// Display tick, incremented for each call to update_screen()
EXTERN disptick_T	display_tick INIT(= 0);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SPELL
// Line in which spell checking wasn't highlighted because it touched the
// cursor position in Insert mode.
EXTERN linenr_T		spell_redraw_lnum INIT(= 0);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CONCEAL
// Set when the cursor line needs to be redrawn.
EXTERN పూర్ణాంకం		need_cursor_పంక్తి_redraw INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, ఖాళీ}
# endif
		    ;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం *netbeansArg INIT(= ఖాళీ);	// the -nb[:host:port:passwd] arg
EXTERN పూర్ణాంకం netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN పూర్ణాంకం netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN పూర్ణాంకం netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN పూర్ణాంకం netbeans_Suppress_No_పంక్తులు INIT(= 0); // skip "No lines in buffer"
#అయితే_ముగింపు

/*
 * Some messages that can be shared are included here.
 */
EXTERN అక్షరం శిఖరం_bot_సందేశం[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN అక్షరం bot_శిఖరం_సందేశం[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN అక్షరం పంక్తి_సందేశం[]	    INIT(= N_(" line "));

#నిర్వచితమైతే FEAT_CRYPT
EXTERN అక్షరం need_కీలకం_సందేశం[]  INIT(= N_("Need encryption key for \"%s\""));
#అయితే_ముగింపు

/*
 * Comms. with the session manager (XSMP)
 */
#నిర్వచితమైతే USE_XSMP
EXTERN పూర్ణాంకం xsmp_icefd INIT(= -1);   // The actual connection
#అయితే_ముగింపు

#నిర్వచితమైతే STARTUPTIME
EXTERN FILE *సమయం_fd INIT(= ఖాళీ);  // where to write startup timing
#అయితే_ముగింపు

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN పూర్ణాంకం vim_ignored;
EXTERN అక్షరం *vim_ignoredp;

#నిర్వచితమైతే FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN పూర్ణాంకం  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN పూర్ణాంకం  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN పూర్ణాంకం  disable_char_avail_for_testing INIT(= FALSE);
EXTERN పూర్ణాంకం  disable_redraw_for_testing INIT(= FALSE);
EXTERN పూర్ణాంకం  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN పూర్ణాంకం  nfa_fail_for_testing INIT(= FALSE);
EXTERN పూర్ణాంకం  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN పూర్ణాంకం  no_query_mouse_for_testing INIT(= FALSE);
EXTERN పూర్ణాంకం  ui_delay_for_testing INIT(= 0);
EXTERN పూర్ణాంకం  reset_పదం_props_on_termresponse INIT(= FALSE);
EXTERN పూర్ణాంకం  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN దీర్ఘ override_sysinfo_uptime INIT(= -1);
EXTERN పూర్ణాంకం  override_autoload INIT(= FALSE);
EXTERN పూర్ణాంకం  override_defcompile INIT(= FALSE);
EXTERN పూర్ణాంకం  ml_get_alloc_పంక్తులు INIT(= FALSE);
EXTERN పూర్ణాంకం  ignore_unreachable_సంకేతం_for_testing INIT(= FALSE);

EXTERN పూర్ణాంకం  లోపల_free_unref_వస్తువులు INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TIMERS
EXTERN పూర్ణాంకం  did_కూడు_timer INIT(= FALSE);
EXTERN పూర్ణాంకం  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN పూర్ణాంకం  ఆదానం_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_మూలం_T	*lval_మూలం INIT(= ఖాళీ);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_BEVAL_పదం
EXTERN పూర్ణాంకం  bevalexpr_due_సమితి INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
EXTERN సమయం_T సమయం_for_testing INIT(= 0);

EXTERN పూర్ణాంకం echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN పూర్ణాంకం  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN పూర్ణాంకం *eval_lavars_వాడిన INIT(= ఖాళీ);

// Only filled for Win32.
EXTERN అక్షరం windows_సంస్కరణ[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_అంశం;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, GA_ఖాళీలు, GA_ఖాళీలు, ఖాళీ,
			 {0, 0, (పూర్ణాంకం)పరిమాణం(char_u *), 20, ఖాళీ}, 0, ఖాళీ, ఖాళీ}
# endif
	;
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
EXTERN పూర్ణాంకం ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= ఖాళీ);
#అయితే_ముగింపు


#ఒకవేళ defined(FEAT_పని_CHANNEL)
EXTERN అక్షరం *ch_part_పేర్లు[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN పూర్ణాంకం channel_need_redraw INIT(= FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN పూర్ణాంకం ch_log_output INIT(= FALSE);

EXTERN పూర్ణాంకం did_repeated_సందేశం INIT(= 0);
# define REPEATED_సందేశం_LOOKING	    1
# define REPEATED_సందేశం_SAFESTATE	    2
#అయితే_ముగింపు

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_సమితి);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN పూర్ణాంకం skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN పూర్ణాంకం skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN పూర్ణాంకం skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#నిర్వచించు SHOWCMD_BUFLEN (SHOWCMD_నిలువుల_సంఖ్య + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#నిర్వచితమైతే FEAT_TERMGUICOLORS
EXTERN పూర్ణాంకం	p_tgc_సమితి INIT(= FALSE);
#అయితే_ముగింపు

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#నిర్వచితమైతే HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN అక్షరం *wayland_display_name INIT(= ఖాళీ);

// Don't connect to Wayland compositor if TRUE
EXTERN పూర్ణాంకం wayland_no_connect INIT(= FALSE);

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
రకనిర్వచనం గణన {
    CLIENTSERVER_పద్ధతి_NONE,
# ifdef FEAT_X11
    CLIENTSERVER_పద్ధతి_X11,
# endif
# ifdef MSWIN
    CLIENTSERVER_పద్ధతి_MSWIN,
# endif
# ifdef FEAT_SOCKETSERVER
    CLIENTSERVER_పద్ధతి_SOCKET
# endif
} clientserver_పద్ధతి_T;

EXTERN clientserver_పద్ధతి_T clientserver_పద్ధతి

# ifdef FEAT_X11
INIT(= CLIENTSERVER_పద్ధతి_X11);
# elif defined(MSWIN)
INIT(= CLIENTSERVER_పద్ధతి_MSWIN);
# elif defined(FEAT_SOCKETSERVER)
INIT(= CLIENTSERVER_పద్ధతి_SOCKET);
# లేకపోతే
INIT(= CLIENTSERVER_పద్ధతి_NONE);
# endif

#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= ఖాళీ);
#అయితే_ముగింపు

// If the <xOSC> key should be propagated from vgetc()
EXTERN పూర్ణాంకం allow_osc_కీలకం INIT(= 0);

#నిర్వచితమైతే FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= ఖాళీ);
EXTERN bool inside_redraw_on_ప్రారంభం_cb INIT(= false);
#అయితే_ముగింపు

// If greater than zero, then silence the W23/W24 warning.
EXTERN పూర్ణాంకం silence_w23_w24_సందేశం INIT( = 0);

#నిర్వచితమైతే FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T చివరి_చొప్పించు_ga INIT5(0, 0, 1, 64, ఖాళీ);
EXTERN పూర్ణాంకం	కూడు_చివరి_చొప్పించు INIT(= 0);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN పూర్ణాంకం cell_వెడల్పు INIT(= 8);
EXTERN పూర్ణాంకం cell_ఎత్తు INIT(= 16);
#అయితే_ముగింపు
