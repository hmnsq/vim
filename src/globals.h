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
# सचे defined(MSWIN)
		    = 25L
# अञ्ञथा
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
EXTERN schar_T	*Screen_पङ्क्तयः INIT(= रिक्त);
EXTERN sattr_T	*ScreenAttrs INIT(= रिक्त);
EXTERN colnr_T  *ScreenCols INIT(= रिक्त);
EXTERN unsigned	*पङ्क्तिः_Offset INIT(= रिक्त);
EXTERN char_u	*पङ्क्तिः_Wraps INIT(= रिक्त);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*Screen_पङ्क्तयः_UC INIT(= रिक्त);	// decoded UTF-8 characters
EXTERN u8char_T	*Screen_पङ्क्तयः_C[MAX_MCO];		// composing characters
EXTERN पूर्णाङ्क	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= रिक्त);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_Screen_पङ्क्तिः INIT(= रिक्त);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN पूर्णाङ्क	screen_cur_row INIT(= 0);
EXTERN पूर्णाङ्क	screen_cur_col INIT(= 0);

#ifdef FEAT_अन्वेषणम्_EXTRA
// used for 'hlsearch' highlight matching
EXTERN match_T	screen_अन्वेषणम्_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T अन्वेषणम्_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN पूर्णाङ्क	no_hlsearch INIT(= FALSE);
#endif

#ifdef FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#endif

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN पूर्णाङ्क redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN short	*TabPageIdxs INIT(= रिक्त);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= रिक्त);
EXTERN पूर्णाङ्क	tabline_stl_click_गणनम् INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= रिक्त);
EXTERN पूर्णाङ्क	tabpanel_stl_click_गणनम् INIT(= 0);

#ifdef FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN short	*popup_mask INIT(= रिक्त);
EXTERN short	*popup_mask_next INIT(= रिक्त);
// Array with flags for transparent cells of current popup.
EXTERN वर्ण	*popup_transparent INIT(= रिक्त);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN पूर्णाङ्क	popup_mask_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_mask_tab INIT(= रिक्त);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN पूर्णाङ्क	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= रिक्त);
#endif

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN long	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN पूर्णाङ्क	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= रिक्त);
EXTERN schar_T	*pum_bg_पङ्क्तयः INIT(= रिक्त);
EXTERN u8char_T	*pum_bg_पङ्क्तयः_UC INIT(= रिक्त);
EXTERN u8char_T	*pum_bg_पङ्क्तयः_C[MAX_MCO];
EXTERN पूर्णाङ्क	pum_bg_शिखरम् INIT(= 0);
EXTERN पूर्णाङ्क	pum_bg_bot INIT(= 0);
EXTERN पूर्णाङ्क	pum_bg_cols INIT(= 0);

EXTERN पूर्णाङ्क	screen_Rows INIT(= 0);	    // actual size of ScreenLines[]
EXTERN पूर्णाङ्क	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN पूर्णाङ्क	mod_mask INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN पूर्णाङ्क	vgetc_mod_mask INIT(= 0);
EXTERN पूर्णाङ्क	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN पूर्णाङ्क	cmdline_row;
EXTERN पूर्णाङ्क	cmdline_col_off;
EXTERN पूर्णाङ्क	cmdline_width;

EXTERN पूर्णाङ्क	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN पूर्णाङ्क	redraw_mode INIT(= FALSE);	// mode must be redrawn
EXTERN पूर्णाङ्क	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN पूर्णाङ्क	mode_displayed INIT(= FALSE);	// mode is being displayed
EXTERN पूर्णाङ्क	no_win_do_पङ्क्तयः_ins INIT(= FALSE); // don't insert lines
#सचे defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN पूर्णाङ्क	cmdline_star INIT(= FALSE);	// cmdline is crypted
#endif

EXTERN पूर्णाङ्क	exec_from_reg INIT(= FALSE);	// executing register

EXTERN पूर्णाङ्क	screen_cleared INIT(= FALSE);	// screen has been cleared

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

EXTERN char_u	*edit_submode INIT(= रिक्त); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= रिक्त); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= रिक्त);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#ifdef FEAT_RIGHTLEFT
EXTERN पूर्णाङ्क	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#endif
EXTERN पूर्णाङ्क	सन्देशः_col;
EXTERN पूर्णाङ्क	सन्देशः_row;
EXTERN पूर्णाङ्क	सन्देशः_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN पूर्णाङ्क	सन्देशः_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_सन्देशः INIT(= रिक्त);	    // msg to be shown after redraw
EXTERN पूर्णाङ्क	keep_सन्देशः_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN पूर्णाङ्क	keep_सन्देशः_more INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN पूर्णाङ्क	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN पूर्णाङ्क	सन्देशः_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN पूर्णाङ्क	सन्देशः_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN पूर्णाङ्क	सन्देशः_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN पूर्णाङ्क	सन्देशः_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN पूर्णाङ्क	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN पूर्णाङ्क	info_message INIT(= FALSE); // printing informative message
EXTERN पूर्णाङ्क      सन्देशः_hist_off INIT(= FALSE); // don't add messages to history
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN पूर्णाङ्क	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN पूर्णाङ्क	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_सन्देशः INIT(= रिक्त);
EXTERN long	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= रिक्त);

EXTERN पूर्णाङ्क	did_endif INIT(= FALSE);    // just had ":endif"
#endif
EXTERN पूर्णाङ्क	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN पूर्णाङ्क	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN पूर्णाङ्क	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN पूर्णाङ्क	called_vim_beep;	    // set if vim_beep() is called
EXTERN पूर्णाङ्क	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#endif
EXTERN पूर्णाङ्क	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN पूर्णाङ्क	called_emsg;		    // always incremented by emsg()
EXTERN पूर्णाङ्क	in_echowindow;		    // executing ":echowindow"
EXTERN पूर्णाङ्क	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN पूर्णाङ्क	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN पूर्णाङ्क	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN पूर्णाङ्क	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN पूर्णाङ्क	need_wait_return INIT(= 0); // need to wait for return later
EXTERN पूर्णाङ्क	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN पूर्णाङ्क	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN पूर्णाङ्क	quit_more INIT(= FALSE);    // 'q' hit at "--more--" msg
#सचे defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN पूर्णाङ्क	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN पूर्णाङ्क	intr_char INIT(= 0);	    // extra interrupt character
#endif
#सचे (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN पूर्णाङ्क	x_no_connect INIT(= FALSE); // don't connect to X server
# सचे defined(FEAT_CLIENTSERVER)
EXTERN पूर्णाङ्क	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#endif
EXTERN पूर्णाङ्क	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN पूर्णाङ्क	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN पूर्णाङ्क	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN पूर्णाङ्क	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN पूर्णाङ्क	पङ्क्तयः_वामः INIT(= -1);	    // lines left for listing
EXTERN पूर्णाङ्क	सन्देशः_no_more INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, परिमाण(estack_T), 50, रिक्त);
#define HAVE_SOURCING_INFO  (exestack.ga_data != रिक्त && exestack.ga_len > 0)
// name of error message source
#define SOURCING_NAME (((estack_T *)exestack.ga_data)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#define SOURCING_LNUM (((estack_T *)exestack.ga_data)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#ifdef FEAT_EVAL
    INIT4(0, 0, 0, 0);
#अञ्ञथा
    INIT(= {0});
#endif

#ifdef FEAT_EVAL
// whether inside compile_def_function()
EXTERN पूर्णाङ्क	estack_compiling INIT(= FALSE);

EXTERN पूर्णाङ्क	ex_nesting_level INIT(= 0);	// nesting level
EXTERN पूर्णाङ्क	debug_break_level INIT(= -1);	// break below this level
EXTERN पूर्णाङ्क	debug_did_सन्देशः INIT(= FALSE);	// did "debug mode" message
EXTERN पूर्णाङ्क	debug_tick INIT(= 0);		// breakpoint change count
EXTERN पूर्णाङ्क	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN पूर्णाङ्क	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_वस्तूनि INIT5(0, 0, परिमाण(scriptitem_T *), 20, रिक्त);
# define SCRIPT_ITEM(id)    (((scriptitem_T **)script_वस्तूनि.ga_data)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_वस्तूनि.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_ITEM(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_पङ्क्तयः.ga_data))[j]

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
EXTERN पूर्णाङ्क did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN पूर्णाङ्क need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN पूर्णाङ्क check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN पूर्णाङ्क trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN पूर्णाङ्क force_abort INIT(= FALSE);

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
EXTERN msglist_T **सन्देशः_list INIT(= रिक्त);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN पूर्णाङ्क suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_राशिः INIT(= रिक्त);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN पूर्णाङ्क	may_garbage_collect INIT(= FALSE);
EXTERN पूर्णाङ्क	want_garbage_collect INIT(= FALSE);
EXTERN पूर्णाङ्क	garbage_collect_at_exit INIT(= FALSE);


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

# define t_सङ्ख्या		(static_types[12])
# define t_const_सङ्ख्या		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_सङ्ख्या_bool		(static_types[14])
# define t_const_सङ्ख्या_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_सङ्ख्या_float		(static_types[16])
# define t_const_सङ्ख्या_float	(static_types[17])

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
# define t_सङ्ख्या_or_string	(static_types[30])
# define t_const_सङ्ख्या_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_सङ्ख्या		(static_types[38])
# define t_const_func_सङ्ख्या	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_सङ्ख्या		(static_types[48])
# define t_const_func_0_सङ्ख्या	(static_types[49])

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

# define t_list_सङ्ख्या		(static_types[62])
# define t_const_list_सङ्ख्या	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_job		(static_types[66])
# define t_const_list_job	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_सङ्ख्या	(static_types[72])
# define t_const_list_list_सङ्ख्या (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_सङ्ख्या	(static_types[76])
# define t_const_list_list_list_सङ्ख्या (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_सङ्ख्या		(static_types[80])
# define t_const_dict_सङ्ख्या	(static_types[81])

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
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 12: t_number
    {VAR_सङ्ख्या, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_सङ्ख्या, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 14: t_number_bool
    {VAR_सङ्ख्या, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, रिक्त, रिक्त, रिक्त},
    {VAR_सङ्ख्या, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 16: t_number_float
    {VAR_सङ्ख्या, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, रिक्त, रिक्त, रिक्त},
    {VAR_सङ्ख्या, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, रिक्त, रिक्त},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, रिक्त, रिक्त},

    // 26: t_job
    {VAR_JOB, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_JOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, रिक्त, रिक्त},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, रिक्त, रिक्त},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, रिक्त, रिक्त},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, रिक्त, रिक्त},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, रिक्त, रिक्त},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, रिक्त, रिक्त},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_सङ्ख्या, रिक्त, रिक्त},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_सङ्ख्या, रिक्त, रिक्त},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, रिक्त, रिक्त},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, रिक्त, रिक्त},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, रिक्त, रिक्त},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, रिक्त, रिक्त},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, रिक्त, रिक्त},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, रिक्त, रिक्त},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, रिक्त, रिक्त},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, रिक्त, रिक्त},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_सङ्ख्या, रिक्त, रिक्त},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_सङ्ख्या, रिक्त, रिक्त},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, रिक्त, रिक्त},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, रिक्त, रिक्त},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, रिक्त, रिक्त},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, रिक्त, रिक्त},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, रिक्त, रिक्त},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, रिक्त, रिक्त},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, रिक्त, रिक्त},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, रिक्त, रिक्त},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, रिक्त, रिक्त},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_सङ्ख्या, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_सङ्ख्या, रिक्त, रिक्त},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, रिक्त, रिक्त},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_job, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_job, रिक्त, रिक्त},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, रिक्त, रिक्त},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, रिक्त, रिक्त},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_सङ्ख्या, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_सङ्ख्या, रिक्त, रिक्त},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, रिक्त, रिक्त},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_सङ्ख्या, रिक्त, रिक्त},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_सङ्ख्या, रिक्त, रिक्त},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, रिक्त, रिक्त},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, रिक्त, रिक्त},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_सङ्ख्या, रिक्त, रिक्त},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_सङ्ख्या, रिक्त, रिक्त},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, रिक्त, रिक्त},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, रिक्त, रिक्त},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, रिक्त, रिक्त},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, रिक्त, रिक्त},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, रिक्त, रिक्त, रिक्त},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, रिक्त, रिक्त, रिक्त},
}
# endif
;

EXTERN पूर्णाङ्क	did_source_packages INIT(= FALSE);
#endif // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN पूर्णाङ्क	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN पूर्णाङ्क	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN पूर्णाङ्क include_none INIT(= 0);	// when 1 include "None"
EXTERN पूर्णाङ्क include_default INIT(= 0);	// when 1 include "default"
EXTERN पूर्णाङ्क include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN पूर्णाङ्क	highlight_match INIT(= FALSE);	// show search match pos
EXTERN linenr_T	अन्वेषणम्_match_पङ्क्तयः;		// lines of matched string
EXTERN colnr_T	अन्वेषणम्_match_endcol;		// col nr of match end
#ifdef FEAT_अन्वेषणम्_EXTRA
EXTERN linenr_T	अन्वेषणम्_first_पङ्क्तिः INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	अन्वेषणम्_last_पङ्क्तिः INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#endif

EXTERN पूर्णाङ्क	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN पूर्णाङ्क	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN पूर्णाङ्क	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN पूर्णाङ्क	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN पूर्णाङ्क	highlight_attr[HLF_गणनम्];  // Highl. attr for each context.
#ifdef FEAT_STL_OPT
# define USER_HIGHLIGHT
#endif
#ifdef USER_HIGHLIGHT
EXTERN पूर्णाङ्क	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN पूर्णाङ्क	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN पूर्णाङ्क	highlight_stlterm[9];		// On top of user
EXTERN पूर्णाङ्क	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#endif
#ifdef FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN पूर्णाङ्क	skip_term_loop INIT(= FALSE);
#endif
#ifdef FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= रिक्त);	// "-U" cmdline argument
#endif
EXTERN पूर्णाङ्क	cterm_normal_fg_color INIT(= 0);
EXTERN पूर्णाङ्क	cterm_normal_fg_bold INIT(= 0);
EXTERN पूर्णाङ्क	cterm_normal_bg_color INIT(= 0);
EXTERN पूर्णाङ्क	cterm_normal_ul_color INIT(= 0);
#ifdef FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_normal_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_normal_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_normal_ul_gui_color INIT(= INVALCOLOR);
#endif
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#ifdef FEAT_TERMRESPONSE
EXTERN पूर्णाङ्क	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#endif

EXTERN पूर्णाङ्क	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN पूर्णाङ्क	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN पूर्णाङ्क	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN पूर्णाङ्क	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(रिक्त, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= रिक्त);
EXTERN win_T	*au_pending_free_win INIT(= रिक्त);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN पूर्णाङ्क	mouse_row;
EXTERN पूर्णाङ्क	mouse_col;
EXTERN पूर्णाङ्क	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN पूर्णाङ्क	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN पूर्णाङ्क	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#सचे defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN पूर्णाङ्क	WantQueryMouse INIT(= FALSE);
#endif

#ifdef FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN पूर्णाङ्क	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_prev_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN पूर्णाङ्क	gui_prev_topfill INIT(= 0);
# endif
#endif

#ifdef FEAT_MOUSESHAPE
EXTERN पूर्णाङ्क	drag_status_पङ्क्तिः INIT(= FALSE);	// dragging the status line
EXTERN पूर्णाङ्क	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN पूर्णाङ्क	drag_sep_पङ्क्तिः INIT(= FALSE);	// dragging vert separator
#endif


#ifdef FEAT_DIFF
// Value set from 'diffopt'.
EXTERN पूर्णाङ्क	diff_context INIT(= 6);		// context for folds
EXTERN पूर्णाङ्क      linematch_पङ्क्तयः INIT(= 0);      // number of lines for diff line match
EXTERN पूर्णाङ्क	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN पूर्णाङ्क	diff_need_scrollbind INIT(= FALSE);
#endif

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN पूर्णाङ्क	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN पूर्णाङ्क	redraw_not_allowed INIT(= FALSE);

#ifdef MESSAGE_QUEUE
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN पूर्णाङ्क	dont_parse_messages INIT(= FALSE);
#endif

#ifdef FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*मूलम्_menu INIT(= रिक्त);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN पूर्णाङ्क	sys_menu INIT(= FALSE);
#endif

#ifdef FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN पूर्णाङ्क force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN पूर्णाङ्क	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN पूर्णाङ्क	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN पूर्णाङ्क	current_scrollbar;
EXTERN long_u	scrollbar_मानम्;

// found "-rv" or "-reverse" in command line args
EXTERN पूर्णाङ्क	found_reverse_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN वर्ण	*font_argument INIT(= रिक्त);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN वर्ण	*background_argument INIT(= रिक्त);

// "-fg" or "-foreground" command line argument
EXTERN वर्ण	*foreground_argument INIT(= रिक्त);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN चञ्चल sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN पूर्णाङ्क	new_pixel_width INIT(= 0);
EXTERN पूर्णाङ्क	new_pixel_height INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN पूर्णाङ्क	gui_win_x INIT(= -1);
EXTERN पूर्णाङ्क	gui_win_y INIT(= -1);
#endif

#ifdef FEAT_CLIPBOARD
EXTERN Clipboard_T clip_star;	// PRIMARY selection in X11/Wayland
# सचे defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# अञ्ञथा
#  define clip_plus clip_star	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#endif

#ifdef HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN पूर्णाङ्क	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN पूर्णाङ्क	clip_autoselect_star INIT(= FALSE);
EXTERN पूर्णाङ्क	clip_autoselect_plus INIT(= FALSE);
EXTERN पूर्णाङ्क	clip_autoselectml INIT(= FALSE);
EXTERN पूर्णाङ्क	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= रिक्त);
EXTERN पूर्णाङ्क	clip_unnamed_saved INIT(= 0);
# endif
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= रिक्त);
#endif


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= रिक्त);	// previous window (may equal curwin)
#define ONE_WINDOW (firstwin == lastwin)
#define W_NEXT(wp) ((wp)->w_next)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#define AUCMD_WIN_गणनम् 5

प्रकारनिर्देश संरचना {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    पूर्णाङ्क		auc_win_used;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_गणनम्];

#ifdef FEAT_PROP_POPUP
EXTERN win_T    *first_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= रिक्त);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN पूर्णाङ्क	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN पूर्णाङ्क	popup_uses_mouse_move INIT(= FALSE);

EXTERN पूर्णाङ्क	text_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN पूर्णाङ्क	ignore_text_props INIT(= FALSE);
#endif

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN पूर्णाङ्क	pum_will_redraw INIT(= FALSE);

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
EXTERN पूर्णाङ्क	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN पूर्णाङ्क	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#सचे defined(FEAT_TABPANEL)
EXTERN पूर्णाङ्क	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#endif

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= रिक्त);	// first buffer
EXTERN buf_T	*lastbuf INIT(= रिक्त);	// last buffer
EXTERN buf_T	*curbuf INIT(= रिक्त);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN पूर्णाङ्क	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN पूर्णाङ्क	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN पूर्णाङ्क	arg_had_last INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN पूर्णाङ्क	ru_col;		// column for ruler
#ifdef FEAT_STL_OPT
EXTERN पूर्णाङ्क	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#endif
EXTERN पूर्णाङ्क	sc_col;		// column for shown command

#ifdef TEMPDIRNAMES
# सचे defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= रिक्त); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= रिक्त); // Name of Vim's own temp dir.
					   // Ends in a slash.
#endif

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN पूर्णाङ्क	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN पूर्णाङ्क	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN पूर्णाङ्क	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN पूर्णाङ्क	v_dying INIT(= 0); // internal value of v:dying
EXTERN पूर्णाङ्क	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#सचे defined(FEAT_AUTOCHDIR)
EXTERN पूर्णाङ्क	test_autochdir INIT(= FALSE);
#endif
EXTERN वर्ण	*last_chdir_reason INIT(= रिक्त);
#सचे defined(EXITFREE)
EXTERN पूर्णाङ्क	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#endif
// volatile because it is used in signal handler deathtrap().
EXTERN चञ्चल sig_atomic_t full_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN पूर्णाङ्क	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN पूर्णाङ्क	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN पूर्णाङ्क	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN पूर्णाङ्क	curbuf_lock INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN पूर्णाङ्क	allbuf_lock INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#ifdef HAVE_SANDBOX
EXTERN पूर्णाङ्क	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#endif

EXTERN पूर्णाङ्क	silent_mode INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#ifdef FEAT_JOB_CHANNEL
EXTERN पूर्णाङ्क	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#endif

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN पूर्णाङ्क	VIsual_active INIT(= FALSE);
				// whether Visual mode is active
EXTERN पूर्णाङ्क	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN पूर्णाङ्क	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN पूर्णाङ्क  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN पूर्णाङ्क	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN पूर्णाङ्क	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN पूर्णाङ्क	VIsual_mode INIT(= 'v');
				// type of Visual mode

EXTERN पूर्णाङ्क	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN पूर्णाङ्क	resel_VIsual_mode INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_पङ्क्तिः_गणनम्;	// number of lines
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
EXTERN पूर्णाङ्क     did_ai INIT(= FALSE);

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
EXTERN पूर्णाङ्क     end_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN पूर्णाङ्क     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN पूर्णाङ्क	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN पूर्णाङ्क	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN पूर्णाङ्क	can_si_back INIT(= FALSE);

EXTERN पूर्णाङ्क	old_indent INIT(= 0);	// for ^^D command in insert mode

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
EXTERN पूर्णाङ्क	orig_पङ्क्तिः_गणनम् INIT(= 0);  // Line count when "gR" started
EXTERN पूर्णाङ्क	vr_पङ्क्तयः_changed INIT(= 0); // #Lines changed by "gR" so far

#सचे defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
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

EXTERN पूर्णाङ्क	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN पूर्णाङ्क	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN पूर्णाङ्क	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN पूर्णाङ्क	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#सचे defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN पूर्णाङ्क	enc_codepage INIT(= -1);
EXTERN पूर्णाङ्क	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#endif
EXTERN पूर्णाङ्क	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN वर्ण	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T निवेशः_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN पूर्णाङ्क (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN पूर्णाङ्क (*mb_ptr2len_len)(char_u *p, पूर्णाङ्क size) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN पूर्णाङ्क (*mb_char2len)(पूर्णाङ्क c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN पूर्णाङ्क (*mb_char2bytes)(पूर्णाङ्क c, char_u *buf) INIT(= latin_char2bytes);

EXTERN पूर्णाङ्क (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN पूर्णाङ्क (*mb_ptr2cells_len)(char_u *p, पूर्णाङ्क size) INIT(= latin_ptr2cells_len);
EXTERN पूर्णाङ्क (*mb_char2cells)(पूर्णाङ्क c) INIT(= latin_char2cells);
EXTERN पूर्णाङ्क (*mb_off2cells)(unsigned off, unsigned max_off) INIT(= latin_off2cells);
EXTERN पूर्णाङ्क (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN पूर्णाङ्क (*mb_head_off)(char_u *base, char_u *p) INIT(= latin_head_off);

#सचे defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, ध्रुव वर्ण **inbuf, size_t *inbytesleft, वर्ण **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (ध्रुव वर्ण *tocode, ध्रुव वर्ण *fromcode);
EXTERN पूर्णाङ्क (*iconv_close) (iconv_t cd);
EXTERN पूर्णाङ्क (*iconvctl) (iconv_t cd, पूर्णाङ्क request, शून्य *argument);
EXTERN पूर्णाङ्क* (*iconv_errno) (शून्य);
#endif


#ifdef FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= रिक्त);
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
EXTERN पूर्णाङ्क		xim_changed_while_preediting INIT(= FALSE);
# अञ्ञथा
EXTERN XIC		xic INIT(= रिक्त);
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
EXTERN पूर्णाङ्क	State INIT(= MODE_NORMAL);

#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	debug_mode INIT(= FALSE);
#endif

EXTERN पूर्णाङ्क	finish_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN long	opcount INIT(= 0);	// count for pending operator
EXTERN पूर्णाङ्क	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN पूर्णाङ्क exmode_active INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN पूर्णाङ्क pending_exmode_active INIT(= FALSE);

EXTERN पूर्णाङ्क ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN पूर्णाङ्क reg_recording INIT(= 0);	// register for recording  or zero
EXTERN पूर्णाङ्क reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN पूर्णाङ्क pending_end_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN पूर्णाङ्क seen_Modify_Other_कुञ्चिकाः INIT(= FALSE);

// The state for the modifyOtherKeys level
प्रकारनिर्देश गणना {
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
प्रकारनिर्देश गणना {
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

EXTERN पूर्णाङ्क no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN पूर्णाङ्क no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN पूर्णाङ्क allow_कुञ्चिकाः INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN पूर्णाङ्क no_reduce_कुञ्चिकाः INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN पूर्णाङ्क no_u_sync INIT(= 0);		// Don't call u_sync()
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#endif

EXTERN पूर्णाङ्क restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN पूर्णाङ्क arrow_used;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN पूर्णाङ्क	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN पूर्णाङ्क	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#ifdef USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#endif

#ifdef USE_ON_FLY_SCROLL
EXTERN पूर्णाङ्क	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#endif
EXTERN पूर्णाङ्क	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN पूर्णाङ्क	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN पूर्णाङ्क	sticky_cmdmod_flags INIT(= 0); // used by :execute

#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	is_export INIT(= FALSE);    // :export {cmd}
#endif

EXTERN पूर्णाङ्क	सन्देशः_silent INIT(= 0);	// don't print messages
EXTERN पूर्णाङ्क	emsg_silent INIT(= 0);	// don't print error messages
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#endif
EXTERN पूर्णाङ्क	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN पूर्णाङ्क	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN पूर्णाङ्क	in_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN पूर्णाङ्क	swap_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN पूर्णाङ्क	swap_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN वर्ण	सन्देशः_buf[सन्देशः_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN पूर्णाङ्क	RedrawingDisabled INIT(= 0);

EXTERN पूर्णाङ्क	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN पूर्णाङ्क	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#ifdef DO_INIT
		    = {रिक्त, रिक्त, 0, 0, 0, 0, 0, 0, 0}
#endif
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN पूर्णाङ्क	typebuf_was_empty INIT(= FALSE);

EXTERN पूर्णाङ्क	ex_normal_busy INIT(= 0);   // recursiveness of ex_normal()
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	in_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#endif
EXTERN पूर्णाङ्क	ex_normal_lock INIT(= 0);   // forbid use of ex_normal()

#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क	ignore_script INIT(= FALSE);  // ignore script input
#endif
EXTERN पूर्णाङ्क	stop_निवेशय_mode;	// for ":stopinsert" and 'insertmode'

EXTERN पूर्णाङ्क	कुञ्चिका_Typed;		// TRUE if user typed current char
EXTERN पूर्णाङ्क	कुञ्चिका_Stuffed;		// TRUE if current char from stuffbuf
#ifdef HAVE_निवेशः_METHOD
EXTERN पूर्णाङ्क	vgetc_im_active;	// Input Method was active for last
					// character obtained from vgetc()
#endif
EXTERN पूर्णाङ्क	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN पूर्णाङ्क	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN पूर्णाङ्क	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN पूर्णाङ्क	do_redraw INIT(= FALSE);    // extra redraw once
#ifdef FEAT_DIFF
EXTERN पूर्णाङ्क	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#endif
#ifdef FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN पूर्णाङ्क	redrawtime_limit_set INIT(= FALSE);
#endif

EXTERN पूर्णाङ्क	need_highlight_changed INIT(= TRUE);

#define NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN पूर्णाङ्क	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= रिक्त);   // stream to write script to
EXTERN पूर्णाङ्क	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN चञ्चल sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN चञ्चल sig_atomic_t got_sigusr1 INIT(= FALSE);

#ifdef USE_TERM_CONSOLE
EXTERN पूर्णाङ्क	term_console INIT(= FALSE); // set to TRUE when console used
#endif
EXTERN पूर्णाङ्क	termcap_active INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN पूर्णाङ्क	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN पूर्णाङ्क	searchcmdlen;		    // length of previous search cmd
#ifdef FEAT_SYN_HL
EXTERN पूर्णाङ्क	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_in INIT(= रिक्त); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_out INIT(= रिक्त); // Set by vim_regexec()
					    // to store \z\(...\) matches
#endif

EXTERN पूर्णाङ्क	did_outofmem_सन्देशः INIT(= FALSE);
					    // set after out of memory msg
EXTERN पूर्णाङ्क	did_swapwrite_सन्देशः INIT(= FALSE);
					    // set after swap write error msg
EXTERN पूर्णाङ्क	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN पूर्णाङ्क	global_busy INIT(= 0);	    // set when :global is executing
EXTERN पूर्णाङ्क	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN पूर्णाङ्क	need_start_insertmode INIT(= FALSE);
					    // start insert mode soon
#सचे defined(FEAT_EVAL)
EXTERN char_u	last_mode[MODE_MAX_LENGTH] INIT(= "n"); // for ModeChanged event
#endif
EXTERN char_u	*last_cmdline INIT(= रिक्त); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= रिक्त); // command line for "."
EXTERN char_u	*new_last_cmdline INIT(= रिक्त);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= रिक्त); // fname for <afile> on cmdline
EXTERN पूर्णाङ्क	autocmd_fname_full;	     // autocmd_fname is full path
EXTERN पूर्णाङ्क	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_match INIT(= रिक्त); // name for <amatch> on cmdline
EXTERN पूर्णाङ्क	aucmd_cmdline_changed_गणनम् INIT(= 0);

EXTERN पूर्णाङ्क	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	last_cursormoved	      // for CursorMoved event
#ifdef DO_INIT
		    = {0, 0, 0}
#endif
		    ;

EXTERN पूर्णाङ्क	postponed_split INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN पूर्णाङ्क	postponed_split_flags INIT(= 0);  // args for win_split()
EXTERN पूर्णाङ्क	postponed_split_tab INIT(= 0);  // cmdmod.cmod_tab
#ifdef FEAT_QUICKFIX
EXTERN पूर्णाङ्क	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#endif
EXTERN पूर्णाङ्क	g_tag_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN पूर्णाङ्क	replace_offset INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN पूर्णाङ्क	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*empty_option INIT(= (char_u *)"");

EXTERN पूर्णाङ्क  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= रिक्त);	// message redirection file
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क  redir_reg INIT(= 0);	// message redirection register
EXTERN पूर्णाङ्क  redir_vname INIT(= 0);	// message redirection variable
EXTERN पूर्णाङ्क  redir_execute INIT(= 0);	// execute() redirection
#endif

#ifdef FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#endif

EXTERN पूर्णाङ्क  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN पूर्णाङ्क  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN पूर्णाङ्क  wild_menu_showing INIT(= 0);
#define WM_SHOWN	1		// wildmenu showing
#define WM_SCROLLED	2		// wildmenu showing with scroll

#ifdef MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN पूर्णाङ्क	found_register_arg INIT(= FALSE);
#endif

#ifdef FEAT_LINEBREAK
EXTERN वर्ण	breakat_flags[256];	// which characters are in 'breakat'
#endif

// These are in version.c, call init_longVersion() before use.
बाह्य वर्ण *Version;
#सचे defined(HAVE_DATE_TIME) && defined(VMS) && defined(VAXC)
बाह्य वर्ण longVersion[];
#अञ्ञथा
बाह्य वर्ण *longVersion;
#endif

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#ifdef HAVE_PATHDEF
बाह्य char_u *default_vim_dir;
बाह्य char_u *default_vimruntime_dir;
बाह्य char_u *all_cflags;
बाह्य char_u *all_lflags;
# ifdef VMS
बाह्य char_u *compiler_version;
बाह्य char_u *compiled_arch;
बाह्य char_u *compiled_vers;
# endif
बाह्य char_u *compiled_user;
बाह्य char_u *compiled_sys;
#endif

EXTERN char_u	*homedir INIT(= रिक्त);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= रिक्त);

#ifdef FEAT_FOLDING
EXTERN पूर्णाङ्क	disable_fold_update INIT(= 0);
#endif

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN पूर्णाङ्क	km_stopsel INIT(= FALSE);
EXTERN पूर्णाङ्क	km_startsel INIT(= FALSE);

EXTERN पूर्णाङ्क	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN पूर्णाङ्क	cmdwin_result INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= रिक्त); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= रिक्त); // window of cmdline window or NULL

EXTERN char_u no_पङ्क्तयः_सन्देशः[]	INIT(= N_("--No lines in buffer--"));

EXTERN वर्ण typename_unknown[]	INIT(= N_("unknown"));
EXTERN वर्ण typename_int[]	INIT(= "int");
EXTERN वर्ण typename_longint[]	INIT(= "long int");
EXTERN वर्ण typename_longlongint[]	INIT(= "long long int");
EXTERN वर्ण typename_unsignedint[]	INIT(= "unsigned int");
EXTERN वर्ण typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN वर्ण typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN वर्ण typename_pointer[]	INIT(= N_("pointer"));
EXTERN वर्ण typename_percent[]	INIT(= N_("percent"));
EXTERN वर्ण typename_char[] INIT(= "char");
EXTERN वर्ण typename_string[]	INIT(= N_("string"));
EXTERN वर्ण typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN long	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#ifdef FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN संरचना subs_व्यञ्जकम्_S	*substitute_instr INIT(= रिक्त);
#endif

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#सचे defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_IN_ICON	1
# define STL_IN_TITLE	2
EXTERN पूर्णाङ्क      stl_syntax INIT(= 0);
#endif

#सचे defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= रिक्त);
EXTERN पूर्णाङ्क		balloonEvalForTerm INIT(= FALSE);
# सचे defined(FEAT_NETBEANS_INTG)
EXTERN पूर्णाङ्क bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#endif

#ifdef CURSOR_SHAPE
// the table is in misc2.c, because of initializations
बाह्य cursorentry_T shape_table[SHAPE_IDX_गणनम्];
#endif

#ifdef FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_शिखरम्		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_वामः		2
# define OPT_PRINT_दक्षिणः	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_सङ्ख्या	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_अङ्कः_OPTIONS	15

EXTERN option_table_T printer_opts[OPT_PRINT_अङ्कः_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, रिक्त, 0, FALSE},
	{"bottom",	TRUE, 0, रिक्त, 0, FALSE},
	{"left",	TRUE, 0, रिक्त, 0, FALSE},
	{"right",	TRUE, 0, रिक्त, 0, FALSE},
	{"header",	TRUE, 0, रिक्त, 0, FALSE},
	{"syntax",	FALSE, 0, रिक्त, 0, FALSE},
	{"number",	FALSE, 0, रिक्त, 0, FALSE},
	{"wrap",	FALSE, 0, रिक्त, 0, FALSE},
	{"duplex",	FALSE, 0, रिक्त, 0, FALSE},
	{"portrait", FALSE, 0, रिक्त, 0, FALSE},
	{"paper",	FALSE, 0, रिक्त, 0, FALSE},
	{"collate",	FALSE, 0, रिक्त, 0, FALSE},
	{"jobsplit", FALSE, 0, रिक्त, 0, FALSE},
	{"formfeed", FALSE, 0, रिक्त, 0, FALSE},
	{"format", FALSE, 0, रिक्त, 0, FALSE},
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

#सचे (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_page_अङ्कः;
#endif

#ifdef FEAT_XCLIPBOARD
// xterm display name
EXTERN वर्ण	*xterm_display INIT(= रिक्त);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN पूर्णाङ्क	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= रिक्त);
#endif
#सचे defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)रिक्त);
#endif

#सचे defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN पूर्णाङ्क	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#endif

#ifdef FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#endif

#सचे defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN पूर्णाङ्क	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#endif

#ifdef FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= रिक्त);	// name of the server
# ifdef FEAT_X11
EXTERN Window	commWindow INIT(= None);
EXTERN Window	clientWindow INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*serverDelayedStartName INIT(= रिक्त);
# elif defined(MSWIN)
EXTERN HWND	clientWindow INIT(= 0);
# endif
#endif

#सचे defined(UNIX) || defined(VMS)
EXTERN पूर्णाङ्क	term_is_xterm INIT(= FALSE);	// xterm-like 'term'
#endif

#ifdef BACKSLASH_IN_FILENAME
EXTERN वर्ण	psepc INIT(= '\\');	// normal path separator character
EXTERN वर्ण	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN वर्ण	pseps[2] INIT2('\\', 0);
#endif

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN पूर्णाङ्क	virtual_op INIT(= MAYBE);

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
EXTERN पूर्णाङ्क		need_cursor_पङ्क्तिः_redraw INIT(= FALSE);
#endif

#ifdef USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, रिक्त}
# endif
		    ;
#endif

#ifdef FEAT_NETBEANS_INTG
EXTERN वर्ण *netbeansArg INIT(= रिक्त);	// the -nb[:host:port:passwd] arg
EXTERN पूर्णाङ्क netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN पूर्णाङ्क netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN पूर्णाङ्क netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN पूर्णाङ्क netbeans_Suppress_No_पङ्क्तयः INIT(= 0); // skip "No lines in buffer"
#endif

/*
 * Some messages that can be shared are included here.
 */
EXTERN वर्ण शिखरम्_bot_सन्देशः[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN वर्ण bot_शिखरम्_सन्देशः[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN वर्ण पङ्क्तिः_सन्देशः[]	    INIT(= N_(" line "));

#ifdef FEAT_CRYPT
EXTERN वर्ण need_कुञ्चिका_सन्देशः[]  INIT(= N_("Need encryption key for \"%s\""));
#endif

/*
 * Comms. with the session manager (XSMP)
 */
#ifdef USE_XSMP
EXTERN पूर्णाङ्क xsmp_icefd INIT(= -1);   // The actual connection
#endif

#ifdef STARTUPTIME
EXTERN FILE *time_fd INIT(= रिक्त);  // where to write startup timing
#endif

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN पूर्णाङ्क vim_ignored;
EXTERN वर्ण *vim_ignoredp;

#ifdef FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN पूर्णाङ्क  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN पूर्णाङ्क  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN पूर्णाङ्क  disable_char_avail_for_testing INIT(= FALSE);
EXTERN पूर्णाङ्क  disable_redraw_for_testing INIT(= FALSE);
EXTERN पूर्णाङ्क  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN पूर्णाङ्क  nfa_fail_for_testing INIT(= FALSE);
EXTERN पूर्णाङ्क  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN पूर्णाङ्क  no_query_mouse_for_testing INIT(= FALSE);
EXTERN पूर्णाङ्क  ui_delay_for_testing INIT(= 0);
EXTERN पूर्णाङ्क  reset_term_props_on_termresponse INIT(= FALSE);
EXTERN पूर्णाङ्क  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN long override_sysinfo_uptime INIT(= -1);
EXTERN पूर्णाङ्क  override_autoload INIT(= FALSE);
EXTERN पूर्णाङ्क  override_defcompile INIT(= FALSE);
EXTERN पूर्णाङ्क  ml_get_alloc_पङ्क्तयः INIT(= FALSE);
EXTERN पूर्णाङ्क  ignore_unreachable_code_for_testing INIT(= FALSE);

EXTERN पूर्णाङ्क  in_free_unref_वस्तूनि INIT(= FALSE);
#endif

#ifdef FEAT_TIMERS
EXTERN पूर्णाङ्क  did_add_timer INIT(= FALSE);
EXTERN पूर्णाङ्क  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#endif
#ifdef FEAT_EVAL
EXTERN पूर्णाङ्क  निवेशः_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_मूलम्_T	*lval_मूलम् INIT(= रिक्त);
#endif

#ifdef FEAT_BEVAL_TERM
EXTERN पूर्णाङ्क  bevalexpr_due_set INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#endif

#ifdef FEAT_EVAL
EXTERN time_T time_for_testing INIT(= 0);

EXTERN पूर्णाङ्क echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN पूर्णाङ्क  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN पूर्णाङ्क *eval_lavars_used INIT(= रिक्त);

// Only filled for Win32.
EXTERN वर्ण windowsVersion[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_item;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, रिक्त, रिक्त, रिक्त, रिक्त, GA_EMPTY, GA_EMPTY, रिक्त,
			 {0, 0, (पूर्णाङ्क)परिमाण(char_u *), 20, रिक्त}, 0, रिक्त, रिक्त}
# endif
	;
#endif

#ifdef MSWIN
EXTERN पूर्णाङ्क ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= रिक्त);
#endif


#सचे defined(FEAT_JOB_CHANNEL)
EXTERN वर्ण *ch_part_names[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN पूर्णाङ्क channel_need_redraw INIT(= FALSE);
#endif

#ifdef FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN पूर्णाङ्क ch_log_output INIT(= FALSE);

EXTERN पूर्णाङ्क did_repeated_सन्देशः INIT(= 0);
# define REPEATED_सन्देशः_LOOKING	    1
# define REPEATED_सन्देशः_SAFESTATE	    2
#endif

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_SET);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN पूर्णाङ्क skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN पूर्णाङ्क skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN पूर्णाङ्क skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#define SHOWCMD_BUFLEN (SHOWCMD_COLS + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#ifdef FEAT_TERMGUICOLORS
EXTERN पूर्णाङ्क	p_tgc_set INIT(= FALSE);
#endif

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#ifdef HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#endif

#ifdef FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN वर्ण *wayland_display_name INIT(= रिक्त);

// Don't connect to Wayland compositor if TRUE
EXTERN पूर्णाङ्क wayland_no_connect INIT(= FALSE);

#endif

#सचे defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
प्रकारनिर्देश गणना {
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
# अञ्ञथा
INIT(= CLIENTSERVER_METHOD_NONE);
# endif

#endif

#ifdef FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= रिक्त);
#endif

// If the <xOSC> key should be propagated from vgetc()
EXTERN पूर्णाङ्क allow_osc_कुञ्चिका INIT(= 0);

#ifdef FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= रिक्त);
EXTERN bool inside_redraw_on_start_cb INIT(= false);
#endif

// If greater than zero, then silence the W23/W24 warning.
EXTERN पूर्णाङ्क silence_w23_w24_सन्देशः INIT( = 0);

#ifdef FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T last_निवेशय_ga INIT5(0, 0, 1, 64, रिक्त);
EXTERN पूर्णाङ्क	add_last_निवेशय INIT(= 0);
#endif

#सचे defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN पूर्णाङ्क cell_width INIT(= 8);
EXTERN पूर्णाङ्क cell_height INIT(= 16);
#endif
