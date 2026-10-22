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
# ᯀᯮᯝ defined(MSWIN)
		    = 25L
# ᯀᯮᯝᯂᯪ
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
EXTERN schar_T	*ScreenLines INIT(= NULL);
EXTERN sattr_T	*ScreenAttrs INIT(= NULL);
EXTERN colnr_T  *ScreenCols INIT(= NULL);
EXTERN unsigned	*LineOffset INIT(= NULL);
EXTERN char_u	*LineWraps INIT(= NULL);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*ScreenLinesUC INIT(= NULL);	// decoded UTF-8 characters
EXTERN u8char_T	*ScreenLinesC[MAX_MCO];		// composing characters
EXTERN puur_naa_nka	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= NULL);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_ScreenLine INIT(= NULL);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN puur_naa_nka	screen_cur_row INIT(= 0);
EXTERN puur_naa_nka	screen_cur_col INIT(= 0);

#ifdef FEAT_SEARCH_EXTRA
// used for 'hlsearch' highlight matching
EXTERN match_T	screen_search_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T search_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN puur_naa_nka	no_hlsearch INIT(= FALSE);
#endif

#ifdef FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#endif

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN puur_naa_nka redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN short	*TabPageIdxs INIT(= NULL);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= NULL);
EXTERN puur_naa_nka	tabline_stl_click_count INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= NULL);
EXTERN puur_naa_nka	tabpanel_stl_click_count INIT(= 0);

#ifdef FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN short	*popup_mask INIT(= NULL);
EXTERN short	*popup_mask_next INIT(= NULL);
// Array with flags for transparent cells of current popup.
EXTERN var_na	*popup_transparent INIT(= NULL);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN puur_naa_nka	popup_mask_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_mask_tab INIT(= NULL);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN puur_naa_nka	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= NULL);
#endif

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN long	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN puur_naa_nka	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= NULL);
EXTERN schar_T	*pum_bg_lines INIT(= NULL);
EXTERN u8char_T	*pum_bg_linesUC INIT(= NULL);
EXTERN u8char_T	*pum_bg_linesC[MAX_MCO];
EXTERN puur_naa_nka	pum_bg_top INIT(= 0);
EXTERN puur_naa_nka	pum_bg_bot INIT(= 0);
EXTERN puur_naa_nka	pum_bg_cols INIT(= 0);

EXTERN puur_naa_nka	screen_Rows INIT(= 0);	    // actual size of ScreenLines[]
EXTERN puur_naa_nka	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN puur_naa_nka	mod_mask INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN puur_naa_nka	vgetc_mod_mask INIT(= 0);
EXTERN puur_naa_nka	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN puur_naa_nka	cmdline_row;
EXTERN puur_naa_nka	cmdline_col_off;
EXTERN puur_naa_nka	cmdline_width;

EXTERN puur_naa_nka	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN puur_naa_nka	redraw_mode INIT(= FALSE);	// mode must be redrawn
EXTERN puur_naa_nka	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN puur_naa_nka	mode_displayed INIT(= FALSE);	// mode is being displayed
EXTERN puur_naa_nka	no_win_do_lines_ins INIT(= FALSE); // don't insert lines
#ᯀᯮᯝ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN puur_naa_nka	cmdline_star INIT(= FALSE);	// cmdline is crypted
#endif

EXTERN puur_naa_nka	exec_from_reg INIT(= FALSE);	// executing register

EXTERN puur_naa_nka	screen_cleared INIT(= FALSE);	// screen has been cleared

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

EXTERN char_u	*edit_submode INIT(= NULL); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= NULL); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= NULL);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#ifdef FEAT_RIGHTLEFT
EXTERN puur_naa_nka	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#endif
EXTERN puur_naa_nka	msg_col;
EXTERN puur_naa_nka	msg_row;
EXTERN puur_naa_nka	msg_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN puur_naa_nka	msg_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_msg INIT(= NULL);	    // msg to be shown after redraw
EXTERN puur_naa_nka	keep_msg_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN puur_naa_nka	keep_msg_more INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN puur_naa_nka	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN puur_naa_nka	msg_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN puur_naa_nka	msg_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN puur_naa_nka	msg_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN puur_naa_nka	msg_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN puur_naa_nka	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN puur_naa_nka	info_message INIT(= FALSE); // printing informative message
EXTERN puur_naa_nka      msg_hist_off INIT(= FALSE); // don't add messages to history
#ifdef FEAT_EVAL
EXTERN puur_naa_nka	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN puur_naa_nka	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN puur_naa_nka	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_msg INIT(= NULL);
EXTERN long	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= NULL);

EXTERN puur_naa_nka	did_endif INIT(= FALSE);    // just had ":endif"
#endif
EXTERN puur_naa_nka	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#ifdef FEAT_EVAL
EXTERN puur_naa_nka	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN puur_naa_nka	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN puur_naa_nka	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN puur_naa_nka	called_vim_beep;	    // set if vim_beep() is called
EXTERN puur_naa_nka	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#endif
EXTERN puur_naa_nka	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN puur_naa_nka	called_emsg;		    // always incremented by emsg()
EXTERN puur_naa_nka	in_echowindow;		    // executing ":echowindow"
EXTERN puur_naa_nka	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN puur_naa_nka	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN puur_naa_nka	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN puur_naa_nka	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN puur_naa_nka	need_wait_return INIT(= 0); // need to wait for return later
EXTERN puur_naa_nka	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN puur_naa_nka	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN puur_naa_nka	quit_more INIT(= FALSE);    // 'q' hit at "--more--" msg
#ᯀᯮᯝ defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN puur_naa_nka	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN puur_naa_nka	intr_char INIT(= 0);	    // extra interrupt character
#endif
#ᯀᯮᯝ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN puur_naa_nka	x_no_connect INIT(= FALSE); // don't connect to X server
# ᯀᯮᯝ defined(FEAT_CLIENTSERVER)
EXTERN puur_naa_nka	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#endif
EXTERN puur_naa_nka	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN puur_naa_nka	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN puur_naa_nka	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN puur_naa_nka	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN puur_naa_nka	lines_left INIT(= -1);	    // lines left for listing
EXTERN puur_naa_nka	msg_no_more INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, parimaa_na(estack_T), 50, NULL);
#define HAVE_SOURCING_INFO  (exestack.ga_data != NULL && exestack.ga_len > 0)
// name of error message source
#define SOURCING_NAME (((estack_T *)exestack.ga_data)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#define SOURCING_LNUM (((estack_T *)exestack.ga_data)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#ifdef FEAT_EVAL
    INIT4(0, 0, 0, 0);
#ᯀᯮᯝᯂᯪ
    INIT(= {0});
#endif

#ifdef FEAT_EVAL
// whether inside compile_def_function()
EXTERN puur_naa_nka	estack_compiling INIT(= FALSE);

EXTERN puur_naa_nka	ex_nesting_level INIT(= 0);	// nesting level
EXTERN puur_naa_nka	debug_break_level INIT(= -1);	// break below this level
EXTERN puur_naa_nka	debug_did_msg INIT(= FALSE);	// did "debug mode" message
EXTERN puur_naa_nka	debug_tick INIT(= 0);		// breakpoint change count
EXTERN puur_naa_nka	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN puur_naa_nka	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_items INIT5(0, 0, parimaa_na(scriptitem_T *), 20, NULL);
# define SCRIPT_ITEM(id)    (((scriptitem_T **)script_items.ga_data)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_items.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_ITEM(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_lines.ga_data))[j]

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
EXTERN puur_naa_nka did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN puur_naa_nka need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN puur_naa_nka check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN puur_naa_nka trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN puur_naa_nka force_abort INIT(= FALSE);

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
EXTERN msglist_T **msg_list INIT(= NULL);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN puur_naa_nka suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_stack INIT(= NULL);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN puur_naa_nka	may_garbage_collect INIT(= FALSE);
EXTERN puur_naa_nka	want_garbage_collect INIT(= FALSE);
EXTERN puur_naa_nka	garbage_collect_at_exit INIT(= FALSE);


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

# define t_number		(static_types[12])
# define t_const_number		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_number_bool		(static_types[14])
# define t_const_number_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_number_float		(static_types[16])
# define t_const_number_float	(static_types[17])

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
# define t_number_or_string	(static_types[30])
# define t_const_number_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_number		(static_types[38])
# define t_const_func_number	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_number		(static_types[48])
# define t_const_func_0_number	(static_types[49])

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

# define t_list_number		(static_types[62])
# define t_const_list_number	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_job		(static_types[66])
# define t_const_list_job	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_number	(static_types[72])
# define t_const_list_list_number (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_number	(static_types[76])
# define t_const_list_list_list_number (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_number		(static_types[80])
# define t_const_dict_number	(static_types[81])

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
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 12: t_number
    {VAR_NUMBER, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_NUMBER, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 14: t_number_bool
    {VAR_NUMBER, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, NULL, NULL, NULL},
    {VAR_NUMBER, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, NULL, NULL, NULL},

    // 16: t_number_float
    {VAR_NUMBER, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, NULL, NULL, NULL},
    {VAR_NUMBER, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, NULL, NULL, NULL},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, NULL, NULL},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, NULL, NULL},

    // 26: t_job
    {VAR_JOB, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_JOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, NULL, NULL},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, NULL, NULL},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, NULL, NULL},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, NULL, NULL},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, NULL, NULL},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, NULL, NULL},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_number, NULL, NULL},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_number, NULL, NULL},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, NULL, NULL},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, NULL, NULL},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, NULL, NULL},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, NULL, NULL},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, NULL, NULL},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, NULL, NULL},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, NULL, NULL},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, NULL, NULL},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_number, NULL, NULL},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_number, NULL, NULL},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, NULL, NULL},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, NULL, NULL},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, NULL, NULL},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, NULL, NULL},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, NULL, NULL},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, NULL, NULL},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, NULL, NULL},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, NULL, NULL},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, NULL, NULL},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_number, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_number, NULL, NULL},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, NULL, NULL},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_job, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_job, NULL, NULL},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, NULL, NULL},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, NULL, NULL},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_number, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_number, NULL, NULL},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, NULL, NULL},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_number, NULL, NULL},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_number, NULL, NULL},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, NULL, NULL},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, NULL, NULL},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_number, NULL, NULL},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_number, NULL, NULL},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, NULL, NULL},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, NULL, NULL},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, NULL, NULL},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, NULL, NULL},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, NULL, NULL, NULL},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, NULL, NULL, NULL},
}
# endif
;

EXTERN puur_naa_nka	did_source_packages INIT(= FALSE);
#endif // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN puur_naa_nka	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN puur_naa_nka	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN puur_naa_nka include_none INIT(= 0);	// when 1 include "None"
EXTERN puur_naa_nka include_default INIT(= 0);	// when 1 include "default"
EXTERN puur_naa_nka include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN puur_naa_nka	highlight_match INIT(= FALSE);	// show search match pos
EXTERN linenr_T	search_match_lines;		// lines of matched string
EXTERN colnr_T	search_match_endcol;		// col nr of match end
#ifdef FEAT_SEARCH_EXTRA
EXTERN linenr_T	search_first_line INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	search_last_line INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#endif

EXTERN puur_naa_nka	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN puur_naa_nka	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN puur_naa_nka	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN puur_naa_nka	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN puur_naa_nka	highlight_attr[HLF_COUNT];  // Highl. attr for each context.
#ifdef FEAT_STL_OPT
# define USER_HIGHLIGHT
#endif
#ifdef USER_HIGHLIGHT
EXTERN puur_naa_nka	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN puur_naa_nka	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN puur_naa_nka	highlight_stlterm[9];		// On top of user
EXTERN puur_naa_nka	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#endif
#ifdef FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN puur_naa_nka	skip_term_loop INIT(= FALSE);
#endif
#ifdef FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= NULL);	// "-U" cmdline argument
#endif
EXTERN puur_naa_nka	cterm_normal_fg_color INIT(= 0);
EXTERN puur_naa_nka	cterm_normal_fg_bold INIT(= 0);
EXTERN puur_naa_nka	cterm_normal_bg_color INIT(= 0);
EXTERN puur_naa_nka	cterm_normal_ul_color INIT(= 0);
#ifdef FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_normal_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_normal_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_normal_ul_gui_color INIT(= INVALCOLOR);
#endif
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#ifdef FEAT_TERMRESPONSE
EXTERN puur_naa_nka	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#endif

EXTERN puur_naa_nka	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN puur_naa_nka	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN puur_naa_nka	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN puur_naa_nka	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(NULL, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= NULL);
EXTERN win_T	*au_pending_free_win INIT(= NULL);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN puur_naa_nka	mouse_row;
EXTERN puur_naa_nka	mouse_col;
EXTERN puur_naa_nka	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN puur_naa_nka	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN puur_naa_nka	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#ᯀᯮᯝ defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN puur_naa_nka	WantQueryMouse INIT(= FALSE);
#endif

#ifdef FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN puur_naa_nka	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_prev_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN puur_naa_nka	gui_prev_topfill INIT(= 0);
# endif
#endif

#ifdef FEAT_MOUSESHAPE
EXTERN puur_naa_nka	drag_status_line INIT(= FALSE);	// dragging the status line
EXTERN puur_naa_nka	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN puur_naa_nka	drag_sep_line INIT(= FALSE);	// dragging vert separator
#endif


#ifdef FEAT_DIFF
// Value set from 'diffopt'.
EXTERN puur_naa_nka	diff_context INIT(= 6);		// context for folds
EXTERN puur_naa_nka      linematch_lines INIT(= 0);      // number of lines for diff line match
EXTERN puur_naa_nka	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN puur_naa_nka	diff_need_scrollbind INIT(= FALSE);
#endif

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN puur_naa_nka	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN puur_naa_nka	redraw_not_allowed INIT(= FALSE);

#ifdef MESSAGE_QUEUE
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN puur_naa_nka	dont_parse_messages INIT(= FALSE);
#endif

#ifdef FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*root_menu INIT(= NULL);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN puur_naa_nka	sys_menu INIT(= FALSE);
#endif

#ifdef FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN puur_naa_nka force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN puur_naa_nka	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN puur_naa_nka	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN puur_naa_nka	current_scrollbar;
EXTERN long_u	scrollbar_value;

// found "-rv" or "-reverse" in command line args
EXTERN puur_naa_nka	found_reverse_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN var_na	*font_argument INIT(= NULL);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN var_na	*background_argument INIT(= NULL);

// "-fg" or "-foreground" command line argument
EXTERN var_na	*foreground_argument INIT(= NULL);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN ca_ncala sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN puur_naa_nka	new_pixel_width INIT(= 0);
EXTERN puur_naa_nka	new_pixel_height INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN puur_naa_nka	gui_win_x INIT(= -1);
EXTERN puur_naa_nka	gui_win_y INIT(= -1);
#endif

#ifdef FEAT_CLIPBOARD
EXTERN Clipboard_T clip_star;	// PRIMARY selection in X11/Wayland
# ᯀᯮᯝ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# ᯀᯮᯝᯂᯪ
#  define clip_plus clip_star	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#endif

#ifdef HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN puur_naa_nka	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN puur_naa_nka	clip_autoselect_star INIT(= FALSE);
EXTERN puur_naa_nka	clip_autoselect_plus INIT(= FALSE);
EXTERN puur_naa_nka	clip_autoselectml INIT(= FALSE);
EXTERN puur_naa_nka	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= NULL);
EXTERN puur_naa_nka	clip_unnamed_saved INIT(= 0);
# endif
#endif

#ifdef FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= NULL);
#endif


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= NULL);	// previous window (may equal curwin)
#define ONE_WINDOW (firstwin == lastwin)
#define W_NEXT(wp) ((wp)->w_next)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#define AUCMD_WIN_COUNT 5

typedef sa_nracanaa {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    puur_naa_nka		auc_win_used;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_COUNT];

#ifdef FEAT_PROP_POPUP
EXTERN win_T    *first_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= NULL);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN puur_naa_nka	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN puur_naa_nka	popup_uses_mouse_move INIT(= FALSE);

EXTERN puur_naa_nka	text_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN puur_naa_nka	ignore_text_props INIT(= FALSE);
#endif

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN puur_naa_nka	pum_will_redraw INIT(= FALSE);

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
EXTERN puur_naa_nka	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN puur_naa_nka	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#ᯀᯮᯝ defined(FEAT_TABPANEL)
EXTERN puur_naa_nka	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#endif

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= NULL);	// first buffer
EXTERN buf_T	*lastbuf INIT(= NULL);	// last buffer
EXTERN buf_T	*curbuf INIT(= NULL);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN puur_naa_nka	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN puur_naa_nka	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN puur_naa_nka	arg_had_last INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN puur_naa_nka	ru_col;		// column for ruler
#ifdef FEAT_STL_OPT
EXTERN puur_naa_nka	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#endif
EXTERN puur_naa_nka	sc_col;		// column for shown command

#ifdef TEMPDIRNAMES
# ᯀᯮᯝ defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= NULL); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= NULL); // Name of Vim's own temp dir.
					   // Ends in a slash.
#endif

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN puur_naa_nka	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN puur_naa_nka	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN puur_naa_nka	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN puur_naa_nka	v_dying INIT(= 0); // internal value of v:dying
EXTERN puur_naa_nka	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#ᯀᯮᯝ defined(FEAT_AUTOCHDIR)
EXTERN puur_naa_nka	test_autochdir INIT(= FALSE);
#endif
EXTERN var_na	*last_chdir_reason INIT(= NULL);
#ᯀᯮᯝ defined(EXITFREE)
EXTERN puur_naa_nka	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#endif
// volatile because it is used in signal handler deathtrap().
EXTERN ca_ncala sig_atomic_t full_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN puur_naa_nka	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN puur_naa_nka	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN puur_naa_nka	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN puur_naa_nka	curbuf_lock INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN puur_naa_nka	allbuf_lock INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#ifdef HAVE_SANDBOX
EXTERN puur_naa_nka	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#endif

EXTERN puur_naa_nka	silent_mode INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#ifdef FEAT_JOB_CHANNEL
EXTERN puur_naa_nka	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#endif

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN puur_naa_nka	VIsual_active INIT(= FALSE);
				// whether Visual mode is active
EXTERN puur_naa_nka	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN puur_naa_nka	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN puur_naa_nka  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN puur_naa_nka	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN puur_naa_nka	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN puur_naa_nka	VIsual_mode INIT(= 'v');
				// type of Visual mode

EXTERN puur_naa_nka	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN puur_naa_nka	resel_VIsual_mode INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_line_count;	// number of lines
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
EXTERN puur_naa_nka     did_ai INIT(= FALSE);

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
EXTERN puur_naa_nka     end_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN puur_naa_nka     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN puur_naa_nka	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN puur_naa_nka	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN puur_naa_nka	can_si_back INIT(= FALSE);

EXTERN puur_naa_nka	old_indent INIT(= 0);	// for ^^D command in insert mode

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
EXTERN puur_naa_nka	orig_line_count INIT(= 0);  // Line count when "gR" started
EXTERN puur_naa_nka	vr_lines_changed INIT(= 0); // #Lines changed by "gR" so far

#ᯀᯮᯝ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
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

EXTERN puur_naa_nka	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN puur_naa_nka	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN puur_naa_nka	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN puur_naa_nka	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#ᯀᯮᯝ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN puur_naa_nka	enc_codepage INIT(= -1);
EXTERN puur_naa_nka	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#endif
EXTERN puur_naa_nka	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN var_na	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T input_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN puur_naa_nka (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN puur_naa_nka (*mb_ptr2len_len)(char_u *p, puur_naa_nka size) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN puur_naa_nka (*mb_char2len)(puur_naa_nka c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN puur_naa_nka (*mb_char2bytes)(puur_naa_nka c, char_u *buf) INIT(= latin_char2bytes);

EXTERN puur_naa_nka (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN puur_naa_nka (*mb_ptr2cells_len)(char_u *p, puur_naa_nka size) INIT(= latin_ptr2cells_len);
EXTERN puur_naa_nka (*mb_char2cells)(puur_naa_nka c) INIT(= latin_char2cells);
EXTERN puur_naa_nka (*mb_off2cells)(unsigned off, unsigned max_off) INIT(= latin_off2cells);
EXTERN puur_naa_nka (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN puur_naa_nka (*mb_head_off)(char_u *base, char_u *p) INIT(= latin_head_off);

#ᯀᯮᯝ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, const var_na **inbuf, size_t *inbytesleft, var_na **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (const var_na *tocode, const var_na *fromcode);
EXTERN puur_naa_nka (*iconv_close) (iconv_t cd);
EXTERN puur_naa_nka (*iconvctl) (iconv_t cd, puur_naa_nka request, void *argument);
EXTERN puur_naa_nka* (*iconv_errno) (void);
#endif


#ifdef FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= NULL);
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
EXTERN puur_naa_nka		xim_changed_while_preediting INIT(= FALSE);
# ᯀᯮᯝᯂᯪ
EXTERN XIC		xic INIT(= NULL);
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
EXTERN puur_naa_nka	State INIT(= MODE_NORMAL);

#ifdef FEAT_EVAL
EXTERN puur_naa_nka	debug_mode INIT(= FALSE);
#endif

EXTERN puur_naa_nka	finish_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN long	opcount INIT(= 0);	// count for pending operator
EXTERN puur_naa_nka	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN puur_naa_nka exmode_active INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN puur_naa_nka pending_exmode_active INIT(= FALSE);

EXTERN puur_naa_nka ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN puur_naa_nka reg_recording INIT(= 0);	// register for recording  or zero
EXTERN puur_naa_nka reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN puur_naa_nka pending_end_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN puur_naa_nka seenModifyOtherKeys INIT(= FALSE);

// The state for the modifyOtherKeys level
typedef ga_nanaa {
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
typedef ga_nanaa {
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

EXTERN puur_naa_nka no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN puur_naa_nka no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN puur_naa_nka allow_keys INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN puur_naa_nka no_reduce_keys INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN puur_naa_nka no_u_sync INIT(= 0);		// Don't call u_sync()
#ifdef FEAT_EVAL
EXTERN puur_naa_nka u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#endif

EXTERN puur_naa_nka restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN puur_naa_nka arrow_used;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN puur_naa_nka	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN puur_naa_nka	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#ifdef USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#endif

#ifdef USE_ON_FLY_SCROLL
EXTERN puur_naa_nka	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#endif
EXTERN puur_naa_nka	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN puur_naa_nka	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN puur_naa_nka	sticky_cmdmod_flags INIT(= 0); // used by :execute

#ifdef FEAT_EVAL
EXTERN puur_naa_nka	is_export INIT(= FALSE);    // :export {cmd}
#endif

EXTERN puur_naa_nka	msg_silent INIT(= 0);	// don't print messages
EXTERN puur_naa_nka	emsg_silent INIT(= 0);	// don't print error messages
#ifdef FEAT_EVAL
EXTERN puur_naa_nka	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#endif
EXTERN puur_naa_nka	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN puur_naa_nka	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN puur_naa_nka	in_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN puur_naa_nka	swap_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN puur_naa_nka	swap_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN var_na	msg_buf[MSG_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN puur_naa_nka	RedrawingDisabled INIT(= 0);

EXTERN puur_naa_nka	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN puur_naa_nka	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#ifdef DO_INIT
		    = {NULL, NULL, 0, 0, 0, 0, 0, 0, 0}
#endif
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN puur_naa_nka	typebuf_was_empty INIT(= FALSE);

EXTERN puur_naa_nka	ex_normal_busy INIT(= 0);   // recursiveness of ex_normal()
#ifdef FEAT_EVAL
EXTERN puur_naa_nka	in_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#endif
EXTERN puur_naa_nka	ex_normal_lock INIT(= 0);   // forbid use of ex_normal()

#ifdef FEAT_EVAL
EXTERN puur_naa_nka	ignore_script INIT(= FALSE);  // ignore script input
#endif
EXTERN puur_naa_nka	stop_insert_mode;	// for ":stopinsert" and 'insertmode'

EXTERN puur_naa_nka	KeyTyped;		// TRUE if user typed current char
EXTERN puur_naa_nka	KeyStuffed;		// TRUE if current char from stuffbuf
#ifdef HAVE_INPUT_METHOD
EXTERN puur_naa_nka	vgetc_im_active;	// Input Method was active for last
					// character obtained from vgetc()
#endif
EXTERN puur_naa_nka	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN puur_naa_nka	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN puur_naa_nka	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN puur_naa_nka	do_redraw INIT(= FALSE);    // extra redraw once
#ifdef FEAT_DIFF
EXTERN puur_naa_nka	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#endif
#ifdef FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN puur_naa_nka	redrawtime_limit_set INIT(= FALSE);
#endif

EXTERN puur_naa_nka	need_highlight_changed INIT(= TRUE);

#define NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN puur_naa_nka	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= NULL);   // stream to write script to
EXTERN puur_naa_nka	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN ca_ncala sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN ca_ncala sig_atomic_t got_sigusr1 INIT(= FALSE);

#ifdef USE_TERM_CONSOLE
EXTERN puur_naa_nka	term_console INIT(= FALSE); // set to TRUE when console used
#endif
EXTERN puur_naa_nka	termcap_active INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN puur_naa_nka	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN puur_naa_nka	searchcmdlen;		    // length of previous search cmd
#ifdef FEAT_SYN_HL
EXTERN puur_naa_nka	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_in INIT(= NULL); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_out INIT(= NULL); // Set by vim_regexec()
					    // to store \z\(...\) matches
#endif

EXTERN puur_naa_nka	did_outofmem_msg INIT(= FALSE);
					    // set after out of memory msg
EXTERN puur_naa_nka	did_swapwrite_msg INIT(= FALSE);
					    // set after swap write error msg
EXTERN puur_naa_nka	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN puur_naa_nka	global_busy INIT(= 0);	    // set when :global is executing
EXTERN puur_naa_nka	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN puur_naa_nka	need_start_insertmode INIT(= FALSE);
					    // start insert mode soon
#ᯀᯮᯝ defined(FEAT_EVAL)
EXTERN char_u	last_mode[MODE_MAX_LENGTH] INIT(= "n"); // for ModeChanged event
#endif
EXTERN char_u	*last_cmdline INIT(= NULL); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= NULL); // command line for "."
EXTERN char_u	*new_last_cmdline INIT(= NULL);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= NULL); // fname for <afile> on cmdline
EXTERN puur_naa_nka	autocmd_fname_full;	     // autocmd_fname is full path
EXTERN puur_naa_nka	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_match INIT(= NULL); // name for <amatch> on cmdline
EXTERN puur_naa_nka	aucmd_cmdline_changed_count INIT(= 0);

EXTERN puur_naa_nka	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	last_cursormoved	      // for CursorMoved event
#ifdef DO_INIT
		    = {0, 0, 0}
#endif
		    ;

EXTERN puur_naa_nka	postponed_split INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN puur_naa_nka	postponed_split_flags INIT(= 0);  // args for win_split()
EXTERN puur_naa_nka	postponed_split_tab INIT(= 0);  // cmdmod.cmod_tab
#ifdef FEAT_QUICKFIX
EXTERN puur_naa_nka	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#endif
EXTERN puur_naa_nka	g_tag_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN puur_naa_nka	replace_offset INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN puur_naa_nka	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*empty_option INIT(= (char_u *)"");

EXTERN puur_naa_nka  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= NULL);	// message redirection file
#ifdef FEAT_EVAL
EXTERN puur_naa_nka  redir_reg INIT(= 0);	// message redirection register
EXTERN puur_naa_nka  redir_vname INIT(= 0);	// message redirection variable
EXTERN puur_naa_nka  redir_execute INIT(= 0);	// execute() redirection
#endif

#ifdef FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#endif

EXTERN puur_naa_nka  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN puur_naa_nka  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN puur_naa_nka  wild_menu_showing INIT(= 0);
#define WM_SHOWN	1		// wildmenu showing
#define WM_SCROLLED	2		// wildmenu showing with scroll

#ifdef MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN puur_naa_nka	found_register_arg INIT(= FALSE);
#endif

#ifdef FEAT_LINEBREAK
EXTERN var_na	breakat_flags[256];	// which characters are in 'breakat'
#endif

// These are in version.c, call init_longVersion() before use.
extern var_na *Version;
#ᯀᯮᯝ defined(HAVE_DATE_TIME) && defined(VMS) && defined(VAXC)
extern var_na longVersion[];
#ᯀᯮᯝᯂᯪ
extern var_na *longVersion;
#endif

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#ifdef HAVE_PATHDEF
extern char_u *default_vim_dir;
extern char_u *default_vimruntime_dir;
extern char_u *all_cflags;
extern char_u *all_lflags;
# ifdef VMS
extern char_u *compiler_version;
extern char_u *compiled_arch;
extern char_u *compiled_vers;
# endif
extern char_u *compiled_user;
extern char_u *compiled_sys;
#endif

EXTERN char_u	*homedir INIT(= NULL);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= NULL);

#ifdef FEAT_FOLDING
EXTERN puur_naa_nka	disable_fold_update INIT(= 0);
#endif

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN puur_naa_nka	km_stopsel INIT(= FALSE);
EXTERN puur_naa_nka	km_startsel INIT(= FALSE);

EXTERN puur_naa_nka	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN puur_naa_nka	cmdwin_result INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= NULL); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= NULL); // window of cmdline window or NULL

EXTERN char_u no_lines_msg[]	INIT(= N_("--No lines in buffer--"));

EXTERN var_na typename_unknown[]	INIT(= N_("unknown"));
EXTERN var_na typename_int[]	INIT(= "int");
EXTERN var_na typename_longint[]	INIT(= "long int");
EXTERN var_na typename_longlongint[]	INIT(= "long long int");
EXTERN var_na typename_unsignedint[]	INIT(= "unsigned int");
EXTERN var_na typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN var_na typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN var_na typename_pointer[]	INIT(= N_("pointer"));
EXTERN var_na typename_percent[]	INIT(= N_("percent"));
EXTERN var_na typename_char[] INIT(= "char");
EXTERN var_na typename_string[]	INIT(= N_("string"));
EXTERN var_na typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN long	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#ifdef FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN sa_nracanaa subs_expr_S	*substitute_instr INIT(= NULL);
#endif

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#ᯀᯮᯝ defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_IN_ICON	1
# define STL_IN_TITLE	2
EXTERN puur_naa_nka      stl_syntax INIT(= 0);
#endif

#ᯀᯮᯝ defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= NULL);
EXTERN puur_naa_nka		balloonEvalForTerm INIT(= FALSE);
# ᯀᯮᯝ defined(FEAT_NETBEANS_INTG)
EXTERN puur_naa_nka bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#endif

#ifdef CURSOR_SHAPE
// the table is in misc2.c, because of initializations
extern cursorentry_T shape_table[SHAPE_IDX_COUNT];
#endif

#ifdef FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_TOP		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_LEFT		2
# define OPT_PRINT_RIGHT	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_NUMBER	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_NUM_OPTIONS	15

EXTERN option_table_T printer_opts[OPT_PRINT_NUM_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, NULL, 0, FALSE},
	{"bottom",	TRUE, 0, NULL, 0, FALSE},
	{"left",	TRUE, 0, NULL, 0, FALSE},
	{"right",	TRUE, 0, NULL, 0, FALSE},
	{"header",	TRUE, 0, NULL, 0, FALSE},
	{"syntax",	FALSE, 0, NULL, 0, FALSE},
	{"number",	FALSE, 0, NULL, 0, FALSE},
	{"wrap",	FALSE, 0, NULL, 0, FALSE},
	{"duplex",	FALSE, 0, NULL, 0, FALSE},
	{"portrait", FALSE, 0, NULL, 0, FALSE},
	{"paper",	FALSE, 0, NULL, 0, FALSE},
	{"collate",	FALSE, 0, NULL, 0, FALSE},
	{"jobsplit", FALSE, 0, NULL, 0, FALSE},
	{"formfeed", FALSE, 0, NULL, 0, FALSE},
	{"format", FALSE, 0, NULL, 0, FALSE},
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

#ᯀᯮᯝ (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_page_num;
#endif

#ifdef FEAT_XCLIPBOARD
// xterm display name
EXTERN var_na	*xterm_display INIT(= NULL);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN puur_naa_nka	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= NULL);
#endif
#ᯀᯮᯝ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)NULL);
#endif

#ᯀᯮᯝ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN puur_naa_nka	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#endif

#ifdef FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#endif

#ᯀᯮᯝ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN puur_naa_nka	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#endif

#ifdef FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= NULL);	// name of the server
# ifdef FEAT_X11
EXTERN Window	commWindow INIT(= None);
EXTERN Window	clientWindow INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*serverDelayedStartName INIT(= NULL);
# elif defined(MSWIN)
EXTERN HWND	clientWindow INIT(= 0);
# endif
#endif

#ᯀᯮᯝ defined(UNIX) || defined(VMS)
EXTERN puur_naa_nka	term_is_xterm INIT(= FALSE);	// xterm-like 'term'
#endif

#ifdef BACKSLASH_IN_FILENAME
EXTERN var_na	psepc INIT(= '\\');	// normal path separator character
EXTERN var_na	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN var_na	pseps[2] INIT2('\\', 0);
#endif

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN puur_naa_nka	virtual_op INIT(= MAYBE);

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
EXTERN puur_naa_nka		need_cursor_line_redraw INIT(= FALSE);
#endif

#ifdef USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, NULL}
# endif
		    ;
#endif

#ifdef FEAT_NETBEANS_INTG
EXTERN var_na *netbeansArg INIT(= NULL);	// the -nb[:host:port:passwd] arg
EXTERN puur_naa_nka netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN puur_naa_nka netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN puur_naa_nka netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN puur_naa_nka netbeansSuppressNoLines INIT(= 0); // skip "No lines in buffer"
#endif

/*
 * Some messages that can be shared are included here.
 */
EXTERN var_na top_bot_msg[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN var_na bot_top_msg[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN var_na line_msg[]	    INIT(= N_(" line "));

#ifdef FEAT_CRYPT
EXTERN var_na need_key_msg[]  INIT(= N_("Need encryption key for \"%s\""));
#endif

/*
 * Comms. with the session manager (XSMP)
 */
#ifdef USE_XSMP
EXTERN puur_naa_nka xsmp_icefd INIT(= -1);   // The actual connection
#endif

#ifdef STARTUPTIME
EXTERN FILE *time_fd INIT(= NULL);  // where to write startup timing
#endif

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN puur_naa_nka vim_ignored;
EXTERN var_na *vim_ignoredp;

#ifdef FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN puur_naa_nka  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN puur_naa_nka  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN puur_naa_nka  disable_char_avail_for_testing INIT(= FALSE);
EXTERN puur_naa_nka  disable_redraw_for_testing INIT(= FALSE);
EXTERN puur_naa_nka  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN puur_naa_nka  nfa_fail_for_testing INIT(= FALSE);
EXTERN puur_naa_nka  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN puur_naa_nka  no_query_mouse_for_testing INIT(= FALSE);
EXTERN puur_naa_nka  ui_delay_for_testing INIT(= 0);
EXTERN puur_naa_nka  reset_term_props_on_termresponse INIT(= FALSE);
EXTERN puur_naa_nka  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN long override_sysinfo_uptime INIT(= -1);
EXTERN puur_naa_nka  override_autoload INIT(= FALSE);
EXTERN puur_naa_nka  override_defcompile INIT(= FALSE);
EXTERN puur_naa_nka  ml_get_alloc_lines INIT(= FALSE);
EXTERN puur_naa_nka  ignore_unreachable_code_for_testing INIT(= FALSE);

EXTERN puur_naa_nka  in_free_unref_items INIT(= FALSE);
#endif

#ifdef FEAT_TIMERS
EXTERN puur_naa_nka  did_add_timer INIT(= FALSE);
EXTERN puur_naa_nka  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#endif
#ifdef FEAT_EVAL
EXTERN puur_naa_nka  input_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_root_T	*lval_root INIT(= NULL);
#endif

#ifdef FEAT_BEVAL_TERM
EXTERN puur_naa_nka  bevalexpr_due_set INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#endif

#ifdef FEAT_EVAL
EXTERN time_T time_for_testing INIT(= 0);

EXTERN puur_naa_nka echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN puur_naa_nka  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN puur_naa_nka *eval_lavars_used INIT(= NULL);

// Only filled for Win32.
EXTERN var_na windowsVersion[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_item;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, NULL, NULL, NULL, NULL, GA_EMPTY, GA_EMPTY, NULL,
			 {0, 0, (puur_naa_nka)parimaa_na(char_u *), 20, NULL}, 0, NULL, NULL}
# endif
	;
#endif

#ifdef MSWIN
EXTERN puur_naa_nka ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= NULL);
#endif


#ᯀᯮᯝ defined(FEAT_JOB_CHANNEL)
EXTERN var_na *ch_part_names[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN puur_naa_nka channel_need_redraw INIT(= FALSE);
#endif

#ifdef FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN puur_naa_nka ch_log_output INIT(= FALSE);

EXTERN puur_naa_nka did_repeated_msg INIT(= 0);
# define REPEATED_MSG_LOOKING	    1
# define REPEATED_MSG_SAFESTATE	    2
#endif

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_SET);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN puur_naa_nka skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN puur_naa_nka skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN puur_naa_nka skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#define SHOWCMD_BUFLEN (SHOWCMD_COLS + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#ifdef FEAT_TERMGUICOLORS
EXTERN puur_naa_nka	p_tgc_set INIT(= FALSE);
#endif

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#ifdef HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#endif

#ifdef FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN var_na *wayland_display_name INIT(= NULL);

// Don't connect to Wayland compositor if TRUE
EXTERN puur_naa_nka wayland_no_connect INIT(= FALSE);

#endif

#ᯀᯮᯝ defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
typedef ga_nanaa {
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
# ᯀᯮᯝᯂᯪ
INIT(= CLIENTSERVER_METHOD_NONE);
# endif

#endif

#ifdef FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= NULL);
#endif

// If the <xOSC> key should be propagated from vgetc()
EXTERN puur_naa_nka allow_osc_key INIT(= 0);

#ifdef FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= NULL);
EXTERN bool inside_redraw_on_start_cb INIT(= false);
#endif

// If greater than zero, then silence the W23/W24 warning.
EXTERN puur_naa_nka silence_w23_w24_msg INIT( = 0);

#ifdef FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T last_insert_ga INIT5(0, 0, 1, 64, NULL);
EXTERN puur_naa_nka	add_last_insert INIT(= 0);
#endif

#ᯀᯮᯝ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN puur_naa_nka cell_width INIT(= 8);
EXTERN puur_naa_nka cell_height INIT(= 16);
#endif
