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
EXTERN طویل	صفیں			// nr of rows in the screen
#اگر_تعریف_شدہ DO_INIT
# اگر defined(MSWIN)
		    = 25L
# ورنہ
		    = 24L
# endif
#اگر_ختم
		    ;
EXTERN طویل	Columns INIT(= 80);	// nr of columns in the screen

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
EXTERN schar_T	*Screen_سطریں INIT(= تہی);
EXTERN sattr_T	*ScreenAttrs INIT(= تہی);
EXTERN colnr_T  *Screen_ستون_تعداد INIT(= تہی);
EXTERN غیرعلامتی	*سطر_انحراف INIT(= تہی);
EXTERN char_u	*سطر_Wraps INIT(= تہی);	// line wraps to next line

/*
 * When using Unicode characters (in UTF-8 encoding) the character in
 * ScreenLinesUC[] contains the Unicode for the character at this position, or
 * NUL when the character in ScreenLines[] is to be used (ASCII char).
 * The composing characters are to be drawn on top of the original character.
 * ScreenLinesC[0][off] is only to be used when ScreenLinesUC[off] != 0.
 * Note: These three are only allocated when enc_utf8 is set!
 */
EXTERN u8char_T	*Screen_سطریں_UC INIT(= تہی);	// decoded UTF-8 characters
EXTERN u8char_T	*Screen_سطریں_C[MAX_MCO];		// composing characters
EXTERN صحیح	Screen_mco INIT(= 0);		// value of p_mco used when
						// allocating ScreenLinesC[]

// Only used for euc-jp: Second byte of a character that starts with 0x8e.
// These are single-width.
EXTERN schar_T	*ScreenLines2 INIT(= تہی);

/*
 * One screen line to be displayed.  Points into ScreenLines.
 */
EXTERN schar_T	*current_Screen_سطر INIT(= تہی);

/*
 * Last known cursor position.
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
EXTERN صحیح	screen_cur_صف INIT(= 0);
EXTERN صحیح	screen_cur_ستون INIT(= 0);

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
// used for 'hlsearch' highlight matching
EXTERN مطابقت_T	screen_تلاش_hl;

// last lnum where CurSearch was displayed
EXTERN linenr_T تلاش_hl_has_cursor_lnum INIT(= 0);

// don't use 'hlsearch' temporarily
EXTERN صحیح	no_hlsearch INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
EXTERN foldinfo_T win_foldinfo;	// info for 'foldcolumn'
#اگر_ختم

// Flag that is set when drawing for a callback, not from the main command
// loop.
EXTERN صحیح redrawing_for_callback INIT(= 0);

/*
 * Indexes for tab page line:
 *	N > 0 for label of tab page N
 *	N == 0 for no label
 *	N < 0 for closing tab page -N
 *	N == -999 for closing current tab page
 */
EXTERN مختصر	*Tab_صفحہ_Idxs INIT(= تہی);

// Click regions for 'tabline' (%[FuncName]).
EXTERN stl_click_region_T *tabline_stl_click INIT(= تہی);
EXTERN صحیح	tabline_stl_click_گنتی INIT(= 0);

// Click regions for 'tabpanel' (%[FuncName]).
EXTERN stl_click_region_T *tabpanel_stl_click INIT(= تہی);
EXTERN صحیح	tabpanel_stl_click_گنتی INIT(= 0);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
// Array with size Rows x Columns containing zindex of popups.
EXTERN مختصر	*popup_نقاب INIT(= تہی);
EXTERN مختصر	*popup_نقاب_اگلا INIT(= تہی);
// Array with flags for transparent cells of current popup.
EXTERN حرف	*popup_transparent INIT(= تہی);

// Flag set to TRUE when popup_mask needs to be updated.
EXTERN صحیح	popup_نقاب_refresh INIT(= TRUE);

// Tab that was used to fill popup_mask.
EXTERN tabpage_T *popup_نقاب_tab INIT(= تہی);

// Zindex in for screen_char(): if lower than the value in "popup_mask"
// drawing the character is skipped.
EXTERN صحیح	screen_zindex INIT(= 0);

// Currently drawing popup with opacity window, or NULL.
EXTERN win_T	*screen_opacity_popup INIT(= تہی);
#اگر_ختم

// Pum opacity level (0 = fully transparent, 100 = fully opaque).
// Set via 'pumopt' opacity: key.
EXTERN طویل	p_po INIT(= 100);

// Blend value for popup menu opacity (0 = off, 1-99 = blend level).
// Set during pum drawing when pum opacity is active.
EXTERN صحیح	screen_pum_blend INIT(= 0);

// Saved background screen content for pum opacity blending.
EXTERN sattr_T	*pum_bg_attrs INIT(= تہی);
EXTERN schar_T	*pum_bg_سطریں INIT(= تہی);
EXTERN u8char_T	*pum_bg_سطریں_UC INIT(= تہی);
EXTERN u8char_T	*pum_bg_سطریں_C[MAX_MCO];
EXTERN صحیح	pum_bg_چوٹی INIT(= 0);
EXTERN صحیح	pum_bg_bot INIT(= 0);
EXTERN صحیح	pum_bg_ستون_تعداد INIT(= 0);

EXTERN صحیح	screen_صفیں INIT(= 0);	    // actual size of ScreenLines[]
EXTERN صحیح	screen_Columns INIT(= 0);   // actual size of ScreenLines[]

/*
 * When vgetc() is called, it sets mod_mask to the set of modifiers that are
 * held down based on the MOD_MASK_* symbols that are read first.
 */
EXTERN صحیح	mod_نقاب INIT(= 0);		// current key modifiers

// The value of "mod_mask" and the unmodified character in vgetc() after it has
// called vgetorpeek() enough times.
EXTERN صحیح	vgetc_mod_نقاب INIT(= 0);
EXTERN صحیح	vgetc_char INIT(= 0);

/*
 * Cmdline_row is the row where the command line starts, just below the
 * last window.
 * When the cmdline gets longer than the available space the screen gets
 * scrolled up. After a CTRL-D (show matches), after hitting ':' after
 * "hit return", and for the :global command, the command line is
 * temporarily moved.  The old position is restored with the next call to
 * update_screen().
 */
EXTERN صحیح	cmdline_صف;
EXTERN صحیح	cmdline_ستون_off;
EXTERN صحیح	cmdline_چوڑائی;

EXTERN صحیح	redraw_cmdline INIT(= FALSE);	// cmdline must be redrawn
EXTERN صحیح	redraw_منوال INIT(= FALSE);	// mode must be redrawn
EXTERN صحیح	clear_cmdline INIT(= FALSE);	// cmdline must be cleared
EXTERN صحیح	منوال_displayed INIT(= FALSE);	// mode is being displayed
EXTERN صحیح	no_win_do_سطریں_ins INIT(= FALSE); // don't insert lines
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
EXTERN صحیح	cmdline_ستارہ INIT(= FALSE);	// cmdline is crypted
#اگر_ختم

EXTERN صحیح	exec_from_reg INIT(= FALSE);	// executing register

EXTERN صحیح	screen_cleared INIT(= FALSE);	// screen has been cleared

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

EXTERN char_u	*edit_submode INIT(= تہی); // msg for CTRL-X submode
EXTERN char_u	*edit_submode_pre INIT(= تہی); // prepended to edit_submode
EXTERN char_u	*edit_submode_extra INIT(= تہی);// appended to edit_submode
EXTERN hlf_T	edit_submode_highl;	// highl. method for extra info

/*
 * Functions for putting characters in the command line,
 * while keeping ScreenLines[] updated.
 */
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
EXTERN صحیح	cmdmsg_rl INIT(= FALSE);    // cmdline is drawn right to left
#اگر_ختم
EXTERN صحیح	پیغام_ستون;
EXTERN صحیح	پیغام_صف;
EXTERN صحیح	پیغام_scrolled;	// Number of screen lines that windows have
				// scrolled because of printing messages.
EXTERN صحیح	پیغام_scrolled_ign INIT(= FALSE);
				// when TRUE don't set need_wait_return in
				// msg_puts_attr() when msg_scrolled is
				// non-zero

EXTERN char_u	*keep_پیغام INIT(= تہی);	    // msg to be shown after redraw
EXTERN صحیح	keep_پیغام_attr INIT(= 0);    // highlight attr for keep_msg
EXTERN صحیح	keep_پیغام_مزید INIT(= FALSE); // keep_msg was set by msgmore()
EXTERN صحیح	need_fileinfo INIT(= FALSE);// do fileinfo() after redraw
EXTERN صحیح	پیغام_scroll INIT(= FALSE);   // msg_start() will scroll
EXTERN صحیح	پیغام_didout INIT(= FALSE);   // msg_outstr() was used in line
EXTERN صحیح	پیغام_didany INIT(= FALSE);   // msg_outstr() was used at all
EXTERN صحیح	پیغام_nowait INIT(= FALSE);   // don't wait for this msg
EXTERN صحیح	emsg_off INIT(= 0);	    // don't display errors for now,
					    // unless 'debug' is set.
EXTERN صحیح	info_پیغام INIT(= FALSE); // printing informative message
EXTERN صحیح      پیغام_hist_off INIT(= FALSE); // don't add messages to history
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	need_clr_eos INIT(= FALSE); // need to clear text before
					    // displaying a message.
EXTERN صحیح	emsg_skip INIT(= 0);	    // don't display errors for
					    // expression that is skipped
EXTERN صحیح	emsg_severe INIT(= FALSE);  // use message of next of several
					    // emsg() calls for throw
// used by assert_fails()
EXTERN char_u	*emsg_assert_fails_پیغام INIT(= تہی);
EXTERN طویل	emsg_assert_fails_lnum INIT(= 0);
EXTERN char_u	*emsg_assert_fails_context INIT(= تہی);

EXTERN صحیح	did_endif INIT(= FALSE);    // just had ":endif"
#اگر_ختم
EXTERN صحیح	did_emsg;		    // incremented by emsg() when a
					    // message is displayed or thrown
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	did_emsg_silent INIT(= 0);  // incremented by emsg() when
					    // emsg_silent was set and did_emsg
					    // is not incremented
EXTERN صحیح	did_emsg_def;		    // set by emsg() when emsg_silent
					    // is set before calling a function
EXTERN صحیح	did_emsg_cumul;		    // cumulative did_emsg, increased
					    // when did_emsg is reset.
EXTERN صحیح	called_vim_beep;	    // set if vim_beep() is called
EXTERN صحیح	uncaught_emsg;		    // number of times emsg() was
					    // called and did show a message
#اگر_ختم
EXTERN صحیح	did_emsg_syntax;	    // did_emsg set because of a
					    // syntax error
EXTERN صحیح	called_emsg;		    // always incremented by emsg()
EXTERN صحیح	اندر_echowindow;		    // executing ":echowindow"
EXTERN صحیح	ex_exitval INIT(= 0);	    // exit value for ex mode
EXTERN صحیح	emsg_on_display INIT(= FALSE);	// there is an error message
EXTERN صحیح	rc_did_emsg INIT(= FALSE);  // vim_regcomp() called emsg()

EXTERN صحیح	no_wait_return INIT(= 0);   // don't wait for return for now
EXTERN صحیح	need_wait_return INIT(= 0); // need to wait for return later
EXTERN صحیح	did_wait_return INIT(= FALSE);	// wait_return() was used and
						// nothing written since then
EXTERN صحیح	need_maketitle INIT(= TRUE); // call maketitle() soon

EXTERN صحیح	quit_مزید INIT(= FALSE);    // 'q' hit at "--more--" msg
#اگر defined(UNIX) || defined(VMS) || defined(MACOS_X) || defined(AMIGA)
EXTERN صحیح	newline_on_exit INIT(= FALSE);	// did msg in altern. screen
EXTERN صحیح	intr_char INIT(= 0);	    // extra interrupt character
#اگر_ختم
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
EXTERN صحیح	x_no_connect INIT(= FALSE); // don't connect to X server
# اگر defined(FEAT_CLIENTSERVER)
EXTERN صحیح	x_force_connect INIT(= FALSE);	// Do connect to X server.
						// Overrules x_no_connect and
						// "exclude" in 'clipboard'.
# endif
#اگر_ختم
EXTERN صحیح	ex_keep_indent INIT(= FALSE); // getexmodeline(): keep indent
EXTERN صحیح	vgetc_busy INIT(= 0);	      // when inside vgetc() then > 0

EXTERN صحیح	didset_vim INIT(= FALSE);	    // did set $VIM ourselves
EXTERN صحیح	didset_vimruntime INIT(= FALSE);    // idem for $VIMRUNTIME

/*
 * Lines left before a "more" message.	Ex mode needs to be able to reset this
 * after you type something.
 */
EXTERN صحیح	سطریں_بایاں INIT(= -1);	    // lines left for listing
EXTERN صحیح	پیغام_no_مزید INIT(= FALSE);  // don't use more prompt, truncate
					    // messages

/*
 * Stack of execution contexts.  Each entry is an estack_T.
 * Current context is at ga_len - 1.
 */
EXTERN garray_T	exestack INIT5(0, 0, حجم(estack_T), 50, تہی);
#تعریف_کرو HAVE_SOURCING_INFO  (exestack.ga_ڈیٹا != تہی && exestack.ga_len > 0)
// name of error message source
#تعریف_کرو SOURCING_NAME (((estack_T *)exestack.ga_ڈیٹا)[exestack.ga_len - 1].es_name)
// line number in the message source or zero
#تعریف_کرو SOURCING_LNUM (((estack_T *)exestack.ga_ڈیٹا)[exestack.ga_len - 1].es_lnum)

// Script CTX being sourced or was sourced to define the current function.
EXTERN sctx_T	current_sctx
#اگر_تعریف_شدہ FEAT_EVAL
    INIT4(0, 0, 0, 0);
#ورنہ
    INIT(= {0});
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
// whether inside compile_def_function()
EXTERN صحیح	estack_compiling INIT(= FALSE);

EXTERN صحیح	ex_nesting_level INIT(= 0);	// nesting level
EXTERN صحیح	debug_break_level INIT(= -1);	// break below this level
EXTERN صحیح	debug_did_پیغام INIT(= FALSE);	// did "debug mode" message
EXTERN صحیح	debug_tick INIT(= 0);		// breakpoint change count
EXTERN صحیح	debug_backtrace_level INIT(= 0); // breakpoint backtrace level
# ifdef FEAT_PROFILE
EXTERN صحیح	do_profiling INIT(= PROF_NONE);	// PROF_ values
# endif
EXTERN garray_T script_اشیاء INIT5(0, 0, حجم(scriptitem_T *), 20, تہی);
# define SCRIPT_شے(id)    (((scriptitem_T **)script_اشیاء.ga_ڈیٹا)[(id) - 1])
# define SCRIPT_ID_VALID(id)    ((id) > 0 && (id) <= script_اشیاء.ga_len)
# define SCRIPT_SV(id)		(SCRIPT_شے(id)->sn_vars)
# define SCRIPT_VARS(id)	(SCRIPT_SV(id)->sv_dict.dv_hashtab)

# define FUNCLINE(fp, j)	((char_u **)(fp->uf_سطریں.ga_ڈیٹا))[j]

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
EXTERN صحیح did_throw INIT(= FALSE);

/*
 * need_rethrow: set to TRUE when a throw that cannot be handled in do_cmdline()
 * must be propagated to the cstack of the previously called do_cmdline().
 */
EXTERN صحیح need_rethrow INIT(= FALSE);

/*
 * check_cstack: set to TRUE when a ":finish" or ":return" that cannot be
 * handled in do_cmdline() must be propagated to the cstack of the previously
 * called do_cmdline().
 */
EXTERN صحیح check_cstack INIT(= FALSE);

/*
 * Number of nested try conditionals (across function calls and ":source"
 * commands).
 */
EXTERN صحیح trylevel INIT(= 0);

/*
 * When "force_abort" is TRUE, always skip commands after an error message,
 * even after the outermost ":endif", ":endwhile" or ":endfor" or for a
 * function without the "abort" flag.  It is set to TRUE when "trylevel" is
 * non-zero (and ":silent!" was not used) or an exception is being thrown at
 * the time an error is detected.  It is set to FALSE when "trylevel" gets
 * zero again and there was no error or interrupt or throw.
 */
EXTERN صحیح force_abort INIT(= FALSE);

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
EXTERN msglist_T **پیغام_list INIT(= تہی);

/*
 * suppress_errthrow: When TRUE, don't convert an error to an exception.  Used
 * when displaying the interrupt message or reporting an exception that is still
 * uncaught at the top level (which has already been discarded then).  Also used
 * for the error message when no exception can be thrown.
 */
EXTERN صحیح suppress_errthrow INIT(= FALSE);

/*
 * The stack of all caught and not finished exceptions.  The exception on the
 * top of the stack is the one got by evaluation of v:exception.  The complete
 * stack of all caught and pending exceptions is embedded in the various
 * cstacks; the pending exceptions, however, are not on the caught stack.
 */
EXTERN except_T *caught_ڈھیر INIT(= تہی);

/*
 * Garbage collection can only take place when we are sure there are no Lists
 * or Dictionaries being used internally.  This is flagged with
 * "may_garbage_collect" when we are at the toplevel.
 * "want_garbage_collect" is set by the garbagecollect() function, which means
 * we do garbage collection before waiting for a char at the toplevel.
 * "garbage_collect_at_exit" indicates garbagecollect(1) was called.
 */
EXTERN صحیح	may_garbage_collect INIT(= FALSE);
EXTERN صحیح	want_garbage_collect INIT(= FALSE);
EXTERN صحیح	garbage_collect_at_exit INIT(= FALSE);


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

# define t_نمبر		(static_types[12])
# define t_const_نمبر		(static_types[13])

// t_number_bool - number that can be used as a bool
# define t_نمبر_bool		(static_types[14])
# define t_const_نمبر_bool	(static_types[15])

// t_number_float - number that can be used as a float
# define t_نمبر_float		(static_types[16])
# define t_const_نمبر_float	(static_types[17])

# define t_float			(static_types[18])
# define t_const_float		(static_types[19])

# define t_string		(static_types[20])
# define t_const_string		(static_types[21])

# define t_blob			(static_types[22])
# define t_const_blob		(static_types[23])

# define t_blob_null		(static_types[24])
# define t_const_blob_null	(static_types[25])

# define t_کام			(static_types[26])
# define t_const_کام		(static_types[27])

# define t_channel		(static_types[28])
# define t_const_channel		(static_types[29])

// t_number_or_string - Special value used for @#.
# define t_نمبر_or_string	(static_types[30])
# define t_const_نمبر_or_string (static_types[31])

// t_func_unknown - function with any arguments and no or unknown return value
# define t_func_unknown		(static_types[32])
# define t_const_func_unknown	(static_types[33])

// t_func_void - function with any arguments and no return value
# define t_func_void		(static_types[34])
# define t_const_func_void	(static_types[35])

# define t_func_any		(static_types[36])
# define t_const_func_any	(static_types[37])

# define t_func_نمبر		(static_types[38])
# define t_const_func_نمبر	(static_types[39])

# define t_func_string		(static_types[40])
# define t_const_func_string	(static_types[41])

# define t_func_bool		(static_types[42])
# define t_const_func_bool	(static_types[43])

// t_func_0_void - function without arguments and nor return value
# define t_func_0_void		(static_types[44])
# define t_const_func_0_void	(static_types[45])

# define t_func_0_any		(static_types[46])
# define t_const_func_0_any	(static_types[47])

# define t_func_0_نمبر		(static_types[48])
# define t_const_func_0_نمبر	(static_types[49])

# define t_func_0_string		(static_types[50])
# define t_const_func_0_string	(static_types[51])

# define t_list_any		(static_types[52])
# define t_const_list_any	(static_types[53])

# define t_dict_any		(static_types[54])
# define t_const_dict_any	(static_types[55])

# define t_list_خالی_خانے		(static_types[56])
# define t_const_list_خالی_خانے	(static_types[57])

# define t_dict_خالی_خانے		(static_types[58])
# define t_const_dict_خالی_خانے	(static_types[59])

# define t_list_bool		(static_types[60])
# define t_const_list_bool	(static_types[61])

# define t_list_نمبر		(static_types[62])
# define t_const_list_نمبر	(static_types[63])

# define t_list_string		(static_types[64])
# define t_const_list_string	(static_types[65])

# define t_list_کام		(static_types[66])
# define t_const_list_کام	(static_types[67])

# define t_list_dict_any		(static_types[68])
# define t_const_list_dict_any	(static_types[69])

# define t_list_list_any		(static_types[70])
# define t_const_list_list_any	(static_types[71])

# define t_list_list_نمبر	(static_types[72])
# define t_const_list_list_نمبر (static_types[73])

# define t_list_list_string	(static_types[74])
# define t_const_list_list_string (static_types[75])

# define t_list_list_list_نمبر	(static_types[76])
# define t_const_list_list_list_نمبر (static_types[77])

# define t_dict_bool		(static_types[78])
# define t_const_dict_bool	(static_types[79])

# define t_dict_نمبر		(static_types[80])
# define t_const_dict_نمبر	(static_types[81])

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

# define t_tuple_خالی_خانے		(static_types[94])
# define t_const_tuple_خالی_خانے	(static_types[95])


EXTERN type_T static_types[96]
# ifdef DO_INIT
= {
    // 0: t_unknown
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_UNKNOWN, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 2: t_any
    {VAR_ANY, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_ANY, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 4: t_void
    {VAR_VOID, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_VOID, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 6: t_bool
    {VAR_BOOL, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_BOOL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 8: t_null
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 10: t_none
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_SPECIAL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 12: t_number
    {VAR_نمبر, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_نمبر, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 14: t_number_bool
    {VAR_نمبر, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK, تہی, تہی, تہی},
    {VAR_نمبر, 0, 0, TTFLAG_STATIC|TTFLAG_BOOL_OK|TTFLAG_CONST, تہی, تہی, تہی},

    // 16: t_number_float
    {VAR_نمبر, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK, تہی, تہی, تہی},
    {VAR_نمبر, 0, 0, TTFLAG_STATIC|TTFLAG_FLOAT_OK|TTFLAG_CONST, تہی, تہی, تہی},

    // 18: t_float
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_FLOAT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 20: t_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 22: t_blob
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 24: t_blob_null
    {VAR_BLOB, 0, 0, TTFLAG_STATIC, &t_void, تہی, تہی},
    {VAR_BLOB, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, تہی, تہی},

    // 26: t_job
    {VAR_کام, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_کام, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 28: t_channel
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_CHANNEL, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 30: t_number_or_string
    {VAR_STRING, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_STRING, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 32: t_func_unknown
    {VAR_FUNC, -1, -1, TTFLAG_STATIC, &t_unknown, تہی, تہی},
    {VAR_FUNC, -1, -1, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, تہی, تہی},

    // 34: t_func_void
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_void, تہی, تہی},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, تہی, تہی},

    // 36: t_func_any
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_any, تہی, تہی},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, تہی, تہی},

    // 38: t_func_number
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_نمبر, تہی, تہی},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_نمبر, تہی, تہی},

    // 40: t_func_string
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_string, تہی, تہی},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, تہی, تہی},

    // 42: t_func_bool
    {VAR_FUNC, -1, 0, TTFLAG_STATIC, &t_bool, تہی, تہی},
    {VAR_FUNC, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, تہی, تہی},

    // 44: t_func_0_void
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_void, تہی, تہی},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_void, تہی, تہی},

    // 46: t_func_0_any
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_any, تہی, تہی},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, تہی, تہی},

    // 48: t_func_0_number
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_نمبر, تہی, تہی},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_نمبر, تہی, تہی},

    // 50: t_func_0_string
    {VAR_FUNC, 0, 0, TTFLAG_STATIC, &t_string, تہی, تہی},
    {VAR_FUNC, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, تہی, تہی},

    // 52: t_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_any, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, تہی, تہی},

    // 54: t_dict_any
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_any, تہی, تہی},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_any, تہی, تہی},

    // 56: t_list_empty
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_unknown, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, تہی, تہی},

    // 58: t_dict_empty
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_unknown, تہی, تہی},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_unknown, تہی, تہی},

    // 60: t_list_bool
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_bool, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, تہی, تہی},

    // 62: t_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_نمبر, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_نمبر, تہی, تہی},

    // 64: t_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_string, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, تہی, تہی},

    // 66: t_list_job
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_کام, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_کام, تہی, تہی},

    // 68: t_list_dict_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_dict_any, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_dict_any, تہی, تہی},

    // 70: t_list_list_any
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_any, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_any, تہی, تہی},

    // 74: t_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_نمبر, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_نمبر, تہی, تہی},

    // 74: t_list_list_string
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_string, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_string, تہی, تہی},

    // 76: t_list_list_list_number
    {VAR_LIST, 0, 0, TTFLAG_STATIC, &t_list_list_نمبر, تہی, تہی},
    {VAR_LIST, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_list_list_نمبر, تہی, تہی},

    // 78: t_dict_bool
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_bool, تہی, تہی},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, تہی, تہی},

    // 80: t_dict_number
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_نمبر, تہی, تہی},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_نمبر, تہی, تہی},

    // 82: t_dict_string
    {VAR_DICT, 0, 0, TTFLAG_STATIC, &t_string, تہی, تہی},
    {VAR_DICT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_string, تہی, تہی},

    // 84: t_super (VAR_CLASS with tt_member set to &t_bool
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, &t_bool, تہی, تہی},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, &t_bool, تہی, تہی},

    // 86: t_object_any
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_OBJECT, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 88: t_class
    {VAR_CLASS, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_CLASS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 90: t_typealias
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_TYPEALIAS, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 92: t_tuple_any
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_TUPLE, -1, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},

    // 94: t_tuple_empty
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC, تہی, تہی, تہی},
    {VAR_TUPLE, 0, 0, TTFLAG_STATIC|TTFLAG_CONST, تہی, تہی, تہی},
}
# endif
;

EXTERN صحیح	did_ماخذ_packages INIT(= FALSE);
#اگر_ختم // FEAT_EVAL

// Magic number used for hashitem "hi_key" value indicating a deleted item.
// Only the address is used.
EXTERN char_u	hash_removed;


EXTERN صحیح	scroll_region INIT(= FALSE); // term supports scroll region
EXTERN صحیح	t_colors INIT(= 0);	    // int value of T_CCO

// Flags to indicate an additional string for highlight name completion.
EXTERN صحیح include_none INIT(= 0);	// when 1 include "None"
EXTERN صحیح include_default INIT(= 0);	// when 1 include "default"
EXTERN صحیح include_link INIT(= 0);	// when 2 include "link" and "clear"

/*
 * When highlight_match is TRUE, highlight a match, starting at the cursor
 * position.  Search_match_lines is the number of lines after the match (0 for
 * a match within one line), search_match_endcol the column number of the
 * character just after the match in the last line.
 */
EXTERN صحیح	highlight_مطابقت INIT(= FALSE);	// show search match pos
EXTERN linenr_T	تلاش_مطابقت_سطریں;		// lines of matched string
EXTERN colnr_T	تلاش_مطابقت_endcol;		// col nr of match end
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
EXTERN linenr_T	تلاش_پہلا_سطر INIT(= 0);	  // for :{FIRST},{last}s/pat
EXTERN linenr_T	تلاش_آخری_سطر INIT(= MAXLNUM); // for :{first},{LAST}s/pat
#اگر_ختم

EXTERN صحیح	no_smartcase INIT(= FALSE);	// don't use 'smartcase' once

EXTERN صحیح	need_check_timestamps INIT(= FALSE); // need to check file
						     // timestamps asap
EXTERN صحیح	did_check_timestamps INIT(= FALSE); // did check timestamps
						    // recently
EXTERN صحیح	no_check_timestamps INIT(= 0);	// Don't check timestamps

EXTERN صحیح	highlight_attr[HLF_گنتی];  // Highl. attr for each context.
#اگر_تعریف_شدہ FEAT_STL_OPT
# define USER_HIGHLIGHT
#اگر_ختم
#اگر_تعریف_شدہ USER_HIGHLIGHT
EXTERN صحیح	highlight_user[9];		// User[1-9] attributes
# ifdef FEAT_STL_OPT
EXTERN صحیح	highlight_stlnc[9];		// On top of user
#  ifdef FEAT_TERMINAL
EXTERN صحیح	highlight_stlterm[9];		// On top of user
EXTERN صحیح	highlight_stltermnc[9];		// On top of user
#  endif
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
		// When TRUE skip calling terminal_loop() once.  Used when
		// typing ':' at the more prompt.
EXTERN صحیح	skip_رکن_loop INIT(= FALSE);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
EXTERN char_u	*use_gvimrc INIT(= تہی);	// "-U" cmdline argument
#اگر_ختم
EXTERN صحیح	cterm_معمول_fg_color INIT(= 0);
EXTERN صحیح	cterm_معمول_fg_bold INIT(= 0);
EXTERN صحیح	cterm_معمول_bg_color INIT(= 0);
EXTERN صحیح	cterm_معمول_ul_color INIT(= 0);
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
EXTERN guicolor_T cterm_معمول_fg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_معمول_bg_gui_color INIT(= INVALCOLOR);
EXTERN guicolor_T cterm_معمول_ul_gui_color INIT(= INVALCOLOR);
#اگر_ختم
EXTERN guicolor_T fallback_fg_rgb INIT(= INVALCOLOR); // RGB fallback foreground color from guifg, ctermfg or deduced from 'background'
EXTERN guicolor_T fallback_bg_rgb INIT(= INVALCOLOR); // RGB fallback background color from guibg, ctermbg or deduced from 'background'
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
EXTERN صحیح	is_mac_terminal INIT(= FALSE);  // recognized Terminal.app
#اگر_ختم

EXTERN صحیح	autocmd_busy INIT(= FALSE);	// Is apply_autocmds() busy?
EXTERN صحیح	autocmd_no_enter INIT(= FALSE); // Buf/WinEnter autocmds disabled
EXTERN صحیح	autocmd_no_leave INIT(= FALSE); // Buf/WinLeave autocmds disabled
EXTERN صحیح	tabpage_move_disallowed INIT(= FALSE);  // moving tabpages around disallowed

// When deleting the current buffer, another one must be loaded.  If we know
// which one is preferred, au_new_curbuf is set to it
EXTERN bufref_T	au_new_curbuf INIT3(تہی, 0, 0);

// When deleting a buffer/window and autocmd_busy is TRUE, do not free the
// buffer/window. but link it in the list starting with
// au_pending_free_buf/ap_pending_free_win, using b_next/w_next.
// Free the buffer/window when autocmd_busy is being set to FALSE.
EXTERN buf_T	*au_pending_free_buf INIT(= تہی);
EXTERN win_T	*au_pending_free_win INIT(= تہی);

/*
 * Mouse coordinates, set by check_termcode()
 */
EXTERN صحیح	mouse_صف;
EXTERN صحیح	mouse_ستون;
EXTERN صحیح	mouse_past_bottom INIT(= FALSE);// mouse below last line
EXTERN صحیح	mouse_past_eol INIT(= FALSE);	// mouse right of line
EXTERN صحیح	mouse_dragging INIT(= 0);	// extending Visual area with
						// mouse dragging
#اگر defined(FEAT_MOUSE_DEC)
/*
 * When the DEC mouse has been pressed but not yet released we enable
 * automatic queries for the mouse position.
 */
EXTERN صحیح	WantQueryMouse INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
// When the window layout is about to be changed, need_mouse_correct is set,
// so that gui_mouse_correct() is called afterwards, to correct the mouse
// pointer when focus-follow-mouse is being used.
EXTERN صحیح	need_mouse_correct INIT(= FALSE);

// When double clicking, topline must be the same
EXTERN linenr_T gui_پچھلا_topline INIT(= 0);
# ifdef FEAT_DIFF
EXTERN صحیح	gui_پچھلا_topfill INIT(= 0);
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSESHAPE
EXTERN صحیح	drag_حالت_سطر INIT(= FALSE);	// dragging the status line
EXTERN صحیح	postponed_mouseshape INIT(= FALSE); // postponed updating the
						    // mouse pointer shape
EXTERN صحیح	drag_sep_سطر INIT(= FALSE);	// dragging vert separator
#اگر_ختم


#اگر_تعریف_شدہ FEAT_DIFF
// Value set from 'diffopt'.
EXTERN صحیح	diff_context INIT(= 6);		// context for folds
EXTERN صحیح      linematch_سطریں INIT(= 0);      // number of lines for diff line match
EXTERN صحیح	diff_foldcolumn INIT(= 2);	// 'foldcolumn' for diff mode
EXTERN صحیح	diff_need_scrollbind INIT(= FALSE);
#اگر_ختم

// While redrawing the screen this flag is set.  It means the screen size
// ('lines' and 'rows') must not be changed and prevents recursive updating.
EXTERN صحیح	updating_screen INIT(= FALSE);

// While computing a statusline and the like we do not want any w_redr_type or
// must_redraw to be set.
EXTERN صحیح	redraw_not_allowed INIT(= FALSE);

#اگر_تعریف_شدہ پیغام_قطار
// While closing windows or buffers messages should not be handled to avoid
// using invalid windows or buffers.
EXTERN صحیح	dont_parse_messages INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
// The root of the menu hierarchy.
EXTERN vimmenu_T	*جڑ_menu INIT(= تہی);
/*
 * While defining the system menu, sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
EXTERN صحیح	sys_menu INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
# ifdef FEAT_MENU
// Menu item just selected, set by check_termcode()
EXTERN vimmenu_T	*current_menu;

// Set to TRUE after adding/removing menus to ensure they are updated
EXTERN صحیح force_menu_update INIT(= FALSE);
# endif
# ifdef FEAT_GUI_TABLINE
// Tab in tab pages line just selected, set by check_termcode()
EXTERN صحیح	    current_tab;

// Menu entry in tab pages line menu just selected, set by check_termcode()
EXTERN صحیح	    current_tabmenu;
#  define TABLINE_MENU_CLOSE	1
#  define TABLINE_MENU_NEW	2
#  define TABLINE_MENU_OPEN	3
# endif

// Scrollbar moved and new value, set by check_termcode()
EXTERN صحیح	current_scrollbar;
EXTERN long_u	scrollbar_قدر;

// found "-rv" or "-reverse" in command line args
EXTERN صحیح	ملا_الٹ_arg INIT(= FALSE);

// "-fn" or "-font" command line argument
EXTERN حرف	*font_دلیل INIT(= تہی);

# ifdef FEAT_GUI_GTK
// "-bg" or "-background" command line argument
EXTERN حرف	*background_دلیل INIT(= تہی);

// "-fg" or "-foreground" command line argument
EXTERN حرف	*foreground_دلیل INIT(= تہی);
# endif

/*
 * While executing external commands or in Ex mode, should not insert GUI
 * events in the input buffer: Set hold_gui_events to non-zero.
 *
 * volatile because it is used in signal handler sig_sysmouse().
 */
EXTERN متلون sig_atomic_t hold_gui_events INIT(= 0);

/*
 * When resizing the shell is postponed, remember the new size, and call
 * gui_resize_shell() later.
 */
EXTERN صحیح	new_pixel_چوڑائی INIT(= 0);
EXTERN صحیح	new_pixel_اونچائی INIT(= 0);

// Window position from ":winpos", to be used when opening the GUI window.
EXTERN صحیح	gui_win_x INIT(= -1);
EXTERN صحیح	gui_win_y INIT(= -1);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD
EXTERN Clipboard_T clip_ستارہ;	// PRIMARY selection in X11/Wayland
# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
EXTERN Clipboard_T clip_plus;	// CLIPBOARD selection in X11/Wayland
# ورنہ
#  define clip_plus clip_ستارہ	// there is only one clipboard
#  define ONE_CLIPBOARD
# endif
#اگر_ختم

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
# define CLIP_UNNAMED      1
# define CLIP_UNNAMED_PLUS 2
EXTERN صحیح	clip_unnamed INIT(= 0); // above two values or'ed

# ifdef FEAT_CLIPBOARD
EXTERN صحیح	clip_autoselect_ستارہ INIT(= FALSE);
EXTERN صحیح	clip_autoselect_plus INIT(= FALSE);
EXTERN صحیح	clip_autoselectml INIT(= FALSE);
EXTERN صحیح	clip_html INIT(= FALSE);
EXTERN regprog_T *clip_exclude_prog INIT(= تہی);
EXTERN صحیح	clip_unnamed_saved INIT(= 0);
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
EXTERN char_u	*clip_provider INIT(= تہی);
#اگر_ختم


/*
 * All regular windows are linked in a list. "firstwin" points to the first
 * entry, "lastwin" to the last entry (can be the same as firstwin) and
 * "curwin" to the currently active window.
 * When switching tabs these swapped with the pointers in "tabpage_T".
 */
EXTERN win_T	*firstwin;		// first window
EXTERN win_T	*lastwin;		// last window
EXTERN win_T	*prevwin INIT(= تہی);	// previous window (may equal curwin)
#تعریف_کرو ONE_کھڑکی (firstwin == lastwin)
#تعریف_کرو W_اگلا(wp) ((wp)->w_اگلا)

EXTERN win_T	*curwin;	// currently active window

// When executing autocommands for a buffer that is not in any window, a
// special window is created to handle the side effects.  When autocommands
// nest we may need more than one.  Allow for up to five, if more are needed
// something crazy is happening.
#تعریف_کرو AUCMD_WIN_گنتی 5

قسمتعریف ساخت {
    // Window used in aucmd_prepbuf().  When not NULL the window has been
    // allocated.
    win_T	*auc_win;
    صحیح		auc_win_مستعمل;	// This auc_win is being used.
} aucmdwin_T;

EXTERN aucmdwin_T aucmd_win[AUCMD_WIN_گنتی];

#اگر_تعریف_شدہ FEAT_PROP_POPUP
EXTERN win_T    *پہلا_popupwin;		// first global popup window
EXTERN win_T	*popup_dragwin INIT(= تہی);	// popup window being dragged

// Set to TRUE if there is any visible popup window.
EXTERN صحیح	popup_visible INIT(= FALSE);

// Set to TRUE if a visible popup window may use a MOUSE_MOVE event
EXTERN صحیح	popup_استعمال_کرتا_mouse_move INIT(= FALSE);

EXTERN صحیح	متن_prop_frozen INIT(= 0);

// when TRUE computing the cursor position ignores text properties.
EXTERN صحیح	ignore_متن_props INIT(= FALSE);
#اگر_ختم

// When set the popup menu will redraw soon using the pum_win_ values. Do not
// draw over the poup menu area to avoid flicker.
EXTERN صحیح	pum_will_redraw INIT(= FALSE);

/*
 * The window layout is kept in a tree of frames.  topframe points to the top
 * of the tree.
 */
EXTERN چوکھٹا_T	*topframe;	// top of the window frame tree

/*
 * Tab pages are alternative topframes.  "first_tabpage" points to the first
 * one in the list, "curtab" is the current one. "lastused_tabpage" is the
 * last used one.
 */
EXTERN tabpage_T    *پہلا_tabpage;
EXTERN tabpage_T    *curtab;
EXTERN tabpage_T    *lastused_tabpage;
EXTERN صحیح	    redraw_tabline INIT(= FALSE);  // need to redraw tabline
EXTERN صحیح	    redraw_vseps INIT(= FALSE);	   // need to redraw vseps

#اگر defined(FEAT_TABPANEL)
EXTERN صحیح	    redraw_tabpanel INIT(= FALSE);  // need to redraw tabpanel
#اگر_ختم

/*
 * All buffers are linked in a list. 'firstbuf' points to the first entry,
 * 'lastbuf' to the last entry and 'curbuf' to the currently active buffer.
 */
EXTERN buf_T	*firstbuf INIT(= تہی);	// first buffer
EXTERN buf_T	*lastbuf INIT(= تہی);	// last buffer
EXTERN buf_T	*curbuf INIT(= تہی);	// currently active buffer

// Flag that is set when switching off 'swapfile'.  It means that all blocks
// are to be loaded into memory.  Shouldn't be global...
EXTERN صحیح	mf_dont_release INIT(= FALSE);	// don't release blocks

/*
 * List of files being edited (global argument list).  curwin->w_alist points
 * to this when the window is using the global argument list.
 */
EXTERN alist_T	global_alist;		    // global argument list
EXTERN صحیح	max_alist_id INIT(= 0);	    // the previous argument list id
EXTERN صحیح	arg_had_آخری INIT(= FALSE); // accessed last file in
					    // global_alist

EXTERN صحیح	ru_ستون;		// column for ruler
#اگر_تعریف_شدہ FEAT_STL_OPT
EXTERN صحیح	ru_wid;		// 'rulerfmt' width of ruler when non-zero
#اگر_ختم
EXTERN صحیح	sc_ستون;		// column for shown command

#اگر_تعریف_شدہ TEMPDIRNAMES
# اگر defined(UNIX) && defined(HAVE_FLOCK) \
	&& (defined(HAVE_DIRFD) || defined(__hpux))
EXTERN DIR	*vim_tempdir_dp INIT(= تہی); // File descriptor of temp dir
# endif
EXTERN char_u	*vim_tempdir INIT(= تہی); // Name of Vim's own temp dir.
					   // Ends in a slash.
#اگر_ختم

/*
 * When starting or exiting some things are done differently (e.g. screen
 * updating).
 */
EXTERN صحیح	starting INIT(= NO_SCREEN);
				// first NO_SCREEN, then NO_BUFFERS and then
				// set to 0 when starting up finished
EXTERN صحیح	exiting INIT(= FALSE);
				// TRUE when planning to exit Vim.  Might
				// still keep on running if there is a changed
				// buffer.
EXTERN صحیح	really_exiting INIT(= FALSE);
				// TRUE when we are sure to exit, e.g., after
				// a deadly signal
EXTERN صحیح	v_dying INIT(= 0); // internal value of v:dying
EXTERN صحیح	stdout_isatty INIT(= TRUE);	// is stdout a terminal?

#اگر defined(FEAT_AUTOCHDIR)
EXTERN صحیح	test_autochdir INIT(= FALSE);
#اگر_ختم
EXTERN حرف	*آخری_chdir_reason INIT(= تہی);
#اگر defined(EXITFREE)
EXTERN صحیح	entered_free_all_mem INIT(= FALSE);
				// TRUE when in or after free_all_mem()
#اگر_ختم
// volatile because it is used in signal handler deathtrap().
EXTERN متلون sig_atomic_t بھرا_screen INIT(= FALSE);
				// TRUE when doing full-screen output
				// otherwise only writing some messages

EXTERN صحیح	restricted INIT(= FALSE);
				// TRUE when started as "rvim"
EXTERN صحیح	secure INIT(= FALSE);
				// non-zero when only "safe" commands are
				// allowed, e.g. when sourcing .exrc or .vimrc
				// in current directory

EXTERN صحیح	textlock INIT(= 0);
				// non-zero when changing text and jumping to
				// another window or editing another buffer is
				// not allowed

EXTERN صحیح	curbuf_تالا INIT(= 0);
				// non-zero when the current buffer can't be
				// changed.  Used for FileChangedRO.
EXTERN صحیح	allbuf_تالا INIT(= 0);
				// non-zero when no buffer name can be
				// changed, no buffer can be deleted and
				// current directory can't be changed.
				// Used for SwapExists et al.
#اگر_تعریف_شدہ HAVE_SANDBOX
EXTERN صحیح	sandbox INIT(= 0);
				// Non-zero when evaluating an expression in a
				// "sandbox".  Several things are not allowed
				// then.
#اگر_ختم

EXTERN صحیح	silent_منوال INIT(= FALSE);
				// set to TRUE when "-s" commandline argument
				// used for ex
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
EXTERN صحیح	use_stdio_channel INIT(= FALSE);
				// set to TRUE for the "--stdio-channel"
				// commandline argument
#اگر_ختم

EXTERN pos_T	VIsual;		// start position of active Visual selection
EXTERN صحیح	VIsual_فعال INIT(= FALSE);
				// whether Visual mode is active
EXTERN صحیح	VIsual_select INIT(= FALSE);
				// whether Select mode is active
EXTERN صحیح	VIsual_select_reg INIT(= 0);
				// register name for Select mode
EXTERN صحیح  VIsual_select_exclu_adj INIT(= FALSE);
				// whether incremented cursor during exclusive selection
EXTERN صحیح	restart_VIsual_select INIT(= 0);
				// restart Select mode when next cmd finished
EXTERN صحیح	VIsual_reselect;
				// whether to restart the selection after a
				// Select mode mapping or menu

EXTERN صحیح	VIsual_منوال INIT(= 'v');
				// type of Visual mode

EXTERN صحیح	redo_VIsual_busy INIT(= FALSE);
				// TRUE when redoing Visual

/*
 * The Visual area is remembered for reselection.
 */
EXTERN صحیح	resel_VIsual_منوال INIT(= NUL);	// 'v', 'V', or Ctrl-V
EXTERN linenr_T	resel_VIsual_سطر_گنتی;	// number of lines
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
EXTERN صحیح     did_ai INIT(= FALSE);

/*
 * Column of first char after autoindent.  0 when no autoindent done.  Used
 * when 'backspace' is 0, to avoid backspacing over autoindent.
 */
EXTERN colnr_T	ai_ستون INIT(= 0);

/*
 * This is a character which will end a start-middle-end comment when typed as
 * the first character on a new line.  It is taken from the last character of
 * the "end" comment leader when the COM_AUTO_END flag is given for that
 * comment end in 'comments'.  It is only valid when did_ai is TRUE.
 */
EXTERN صحیح     انتہا_comment_pending INIT(= NUL);

/*
 * This flag is set after a ":syncbind" to let the check_scrollbind() function
 * know that it should not attempt to perform scrollbinding due to the scroll
 * that was a result of the ":syncbind." (Otherwise, check_scrollbind() will
 * undo some of the work done by ":syncbind.")  -ralston
 */
EXTERN صحیح     did_syncbind INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN صحیح	did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent will be removed.
 */
EXTERN صحیح	can_si INIT(= FALSE);

/*
 * This flag is set after an "O" command. If the next typed character is a '{'
 * one indent will be removed.
 */
EXTERN صحیح	can_si_عقب INIT(= FALSE);

EXTERN صحیح	old_indent INIT(= 0);	// for ^^D command in insert mode

EXTERN pos_T	saved_cursor		// w_cursor before formatting text.
#اگر_تعریف_شدہ DO_INIT
		    = {0, 0, 0}
#اگر_ختم
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
EXTERN صحیح	orig_سطر_گنتی INIT(= 0);  // Line count when "gR" started
EXTERN صحیح	vr_سطریں_بدلا INIT(= 0); // #Lines changed by "gR" so far

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
// argument to SETJMP() for handling X IO errors
EXTERN JMP_BUF x_jump_env;
#اگر_ختم

/*
 * These flags are set based upon 'fileencoding'.
 * Note that "enc_utf8" is also set for "unicode", because the characters are
 * internally stored as UTF-8 (to avoid trouble with NUL bytes).
 */
#تعریف_کرو DBCS_JPN	932	// japan
#تعریف_کرو DBCS_JPNU	9932	// euc-jp
#تعریف_کرو DBCS_KOR	949	// korea
#تعریف_کرو DBCS_KORU	9949	// euc-kr
#تعریف_کرو DBCS_CHS	936	// chinese
#تعریف_کرو DBCS_CHSU	9936	// euc-cn
#تعریف_کرو DBCS_CHT	950	// taiwan
#تعریف_کرو DBCS_CHTU	9950	// euc-tw
#تعریف_کرو DBCS_2BYTE	1	// 2byte-
#تعریف_کرو DBCS_DEBUG	(-1)

EXTERN صحیح	enc_dbcs INIT(= 0);		// One of DBCS_xxx values if
						// DBCS encoding
EXTERN صحیح	enc_unicode INIT(= 0);	// 2: UCS-2 or UTF-16, 4: UCS-4
EXTERN صحیح	enc_utf8 INIT(= FALSE);		// UTF-8 encoded Unicode
EXTERN صحیح	enc_latin1like INIT(= TRUE);	// 'encoding' is latin1 comp.
#اگر defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
// Codepage nr of 'encoding'.  Negative means it's not been set yet, zero
// means 'encoding' is not a valid codepage.
EXTERN صحیح	enc_codepage INIT(= -1);
EXTERN صحیح	enc_latin9 INIT(= FALSE);	// 'encoding' is latin9
#اگر_ختم
EXTERN صحیح	has_mbyte INIT(= 0);		// any multi-byte encoding

/*
 * To speed up BYTELEN() we fill a table with the byte lengths whenever
 * enc_utf8 or enc_dbcs changes.
 */
EXTERN حرف	mb_bytelen_tab[256];

// Variables that tell what conversion is used for keyboard input and display
// output.
EXTERN vimconv_T داخلہ_conv;			// type of input conversion
EXTERN vimconv_T output_conv;			// type of output conversion

/*
 * Function pointers, used to quickly get to the right function.  Each has
 * three possible values: latin_ (8-bit), utfc_ or utf_ (utf-8) and dbcs_
 * (DBCS).
 * The value is set in mb_init();
 */
// Length of char in bytes, including any following composing chars.
// NUL has length zero.
EXTERN صحیح (*mb_ptr2len)(char_u *p) INIT(= latin_ptr2len);

// idem, with limit on string length
EXTERN صحیح (*mb_ptr2len_len)(char_u *p, صحیح سائز) INIT(= latin_ptr2len_len);

// byte length of char
EXTERN صحیح (*mb_char2len)(صحیح c) INIT(= latin_char2len);

// Convert char "c" to bytes in "buf", return the length.  "buf" must have room
// for at least 6 bytes.
EXTERN صحیح (*mb_char2bytes)(صحیح c, char_u *buf) INIT(= latin_char2bytes);

EXTERN صحیح (*mb_ptr2cells)(char_u *p) INIT(= latin_ptr2cells);
EXTERN صحیح (*mb_ptr2cells_len)(char_u *p, صحیح سائز) INIT(= latin_ptr2cells_len);
EXTERN صحیح (*mb_char2cells)(صحیح c) INIT(= latin_char2cells);
EXTERN صحیح (*mb_off2cells)(غیرعلامتی off, غیرعلامتی max_off) INIT(= latin_off2cells);
EXTERN صحیح (*mb_ptr2char)(char_u *p) INIT(= latin_ptr2char);

// Byte offset from "p" to the start of a character, including any composing
// characters. "base" must be the start of the string, which must be NUL
// terminated.
EXTERN صحیح (*mb_سر_off)(char_u *بنیاد, char_u *p) INIT(= latin_سر_off);

#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
// Pointers to functions and variables to be loaded at runtime
EXTERN size_t (*iconv) (iconv_t cd, ثابت حرف **inbuf, size_t *inbytesleft, حرف **outbuf, size_t *outbytesleft);
EXTERN iconv_t (*iconv_open) (ثابت حرف *tocode, ثابت حرف *fromcode);
EXTERN صحیح (*iconv_close) (iconv_t cd);
EXTERN صحیح (*iconvctl) (iconv_t cd, صحیح request, خالی *دلیل);
EXTERN صحیح* (*iconv_errno) (خالی);
#اگر_ختم


#اگر_تعریف_شدہ FEAT_XIM
# ifdef FEAT_GUI_GTK
EXTERN GtkIMContext	*xic INIT(= تہی);
/*
 * Start and end column of the preedit area in virtual columns from the start
 * of the text line.  When there is no preedit area they are set to MAXCOL.
 * "preedit_end_col" is needed for coloring the preedited string.  Drawing the
 * color between "preedit_start_col" and curpos did not work, because some XIM
 * set the cursor position to the first char of the string.
 */
EXTERN colnr_T		preedit_آغاز_ستون INIT(= MAXCOL);
EXTERN colnr_T		preedit_انتہا_ستون INIT(= MAXCOL);

// "xim_changed_while_preediting" is set when changed() can set the 'modified'
// flag even while preediting.
EXTERN صحیح		xim_بدلا_while_preediting INIT(= FALSE);
# ورنہ
EXTERN XIC		xic INIT(= تہی);
# endif
# ifdef FEAT_GUI
EXTERN guicolor_T	xim_fg_color INIT(= INVALCOLOR);
EXTERN guicolor_T	xim_bg_color INIT(= INVALCOLOR);
# endif
#اگر_ختم

/*
 * "State" is the main state of Vim.
 * There are other variables that modify the state:
 * "Visual_mode"    When State is MODE_NORMAL or MODE_INSERT.
 * "finish_op"	    When State is MODE_NORMAL, after typing the operator and
 *		    before typing the motion command.
 * "motion_force"   Last motion_force  from do_pending_operator()
 * "debug_mode"	    Debug mode.
 */
EXTERN صحیح	State INIT(= منوال_معمول);

#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	debug_منوال INIT(= FALSE);
#اگر_ختم

EXTERN صحیح	اختتام_op INIT(= FALSE);// TRUE while an operator is pending
EXTERN طویل	opcount INIT(= 0);	// count for pending operator
EXTERN صحیح	motion_force INIT(= 0); // motion force for pending operator

/*
 * Ex mode (Q) state
 */
EXTERN صحیح exmode_فعال INIT(= 0);	// zero, EXMODE_NORMAL or EXMODE_VIM

// Flag set when main_loop() should exit when entering Ex mode.
EXTERN صحیح pending_exmode_فعال INIT(= FALSE);

EXTERN صحیح ex_no_reprint INIT(= FALSE); // no need to print after z or p

EXTERN صحیح reg_recording INIT(= 0);	// register for recording  or zero
EXTERN صحیح reg_executing INIT(= 0);	// register being executed or zero
// Flag set when peeking a character and found the end of executed register
EXTERN صحیح pending_انتہا_reg_executing INIT(= FALSE);

// Set when a modifyOtherKeys sequence was seen, then simplified mappings will
// no longer be used.  To be combined with modify_otherkeys_state.
EXTERN صحیح نظر_آیا_Modify_Other_کلیدیں INIT(= FALSE);

// The state for the modifyOtherKeys level
قسمتعریف شمار {
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
قسمتعریف شمار {
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

EXTERN صحیح no_mapping INIT(= FALSE);	// currently no mapping allowed
EXTERN صحیح no_zero_mapping INIT(= 0);	// mapping zero not allowed
EXTERN صحیح allow_کلیدیں INIT(= FALSE);	// allow key codes when no_mapping
					// is set
EXTERN صحیح no_reduce_کلیدیں INIT(= FALSE);  // do not apply Ctrl, Shift and Alt
					  // to the key
EXTERN صحیح no_u_sync INIT(= 0);		// Don't call u_sync()
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح u_sync_once INIT(= 0);	// Call u_sync() once when evaluating
					// an expression.
#اگر_ختم

EXTERN صحیح restart_edit INIT(= 0);	// call edit when next cmd finished
EXTERN صحیح arrow_مستعمل;			// Normally FALSE, set to TRUE after
					// hitting cursor key in insert mode.
					// Used by vgetorpeek() to decide when
					// to call u_sync()
EXTERN صحیح	ins_at_eol INIT(= FALSE); // put cursor after eol when
					  // restarting edit after CTRL-O

EXTERN صحیح	no_abbr INIT(= TRUE);	// TRUE when no abbreviations loaded

#اگر_تعریف_شدہ USE_EXE_NAME
EXTERN char_u	*exe_name;		// the name of the executable
#اگر_ختم

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
EXTERN صحیح	dont_scroll INIT(= FALSE);// don't use scrollbars when TRUE
#اگر_ختم
EXTERN صحیح	mapped_ctrl_c INIT(= FALSE); // modes where CTRL-C is mapped
EXTERN صحیح	ctrl_c_interrupts INIT(= TRUE);	// CTRL-C sets got_int

EXTERN cmdmod_T	cmdmod;			// Ex command modifiers
EXTERN صحیح	sticky_cmdmod_flags INIT(= 0); // used by :execute

#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	is_export INIT(= FALSE);    // :export {cmd}
#اگر_ختم

EXTERN صحیح	پیغام_silent INIT(= 0);	// don't print messages
EXTERN صحیح	emsg_silent INIT(= 0);	// don't print error messages
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	emsg_silent_def INIT(= 0);  // value of emsg_silent when a :def
					    // function is called
#اگر_ختم
EXTERN صحیح	emsg_noredir INIT(= 0);	// don't redirect error messages
EXTERN صحیح	cmd_silent INIT(= FALSE); // don't echo the command line

EXTERN صحیح	اندر_assert_fails INIT(= FALSE);	// assert_fails() active

EXTERN صحیح	تبادلہ_exists_action INIT(= SEA_NONE);
					// For dialog when swap file already
					// exists.
EXTERN صحیح	تبادلہ_exists_did_quit INIT(= FALSE);
					// Selected "quit" at the dialog.

EXTERN char_u	*IObuff;		// sprintf's are done in this buffer,
					// size is IOSIZE
EXTERN char_u	*NameBuff;		// file names are expanded in this
					// buffer, size is MAXPATHL
EXTERN حرف	پیغام_buf[پیغام_BUF_LEN];	// small buffer for messages

// When non-zero, postpone redrawing.
EXTERN صحیح	RedrawingDisabled INIT(= 0);

EXTERN صحیح	readonlymode INIT(= FALSE); // Set to TRUE for "view"
EXTERN صحیح	recoverymode INIT(= FALSE); // Set to TRUE for "-r" option

EXTERN typebuf_T typebuf		// typeahead buffer
#اگر_تعریف_شدہ DO_INIT
		    = {تہی, تہی, 0, 0, 0, 0, 0, 0, 0}
#اگر_ختم
		    ;
// Flag used to indicate that vgetorpeek() returned a char like Esc when the
// :normal argument was exhausted.
EXTERN صحیح	typebuf_was_خالی_خانے INIT(= FALSE);

EXTERN صحیح	ex_معمول_busy INIT(= 0);   // recursiveness of ex_normal()
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	اندر_feedkeys INIT(= 0);	    // ex_normal_busy set in feedkeys()
#اگر_ختم
EXTERN صحیح	ex_معمول_تالا INIT(= 0);   // forbid use of ex_normal()

#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح	ignore_script INIT(= FALSE);  // ignore script input
#اگر_ختم
EXTERN صحیح	stop_داخل_کرو_منوال;	// for ":stopinsert" and 'insertmode'

EXTERN صحیح	کلید_Typed;		// TRUE if user typed current char
EXTERN صحیح	کلید_Stuffed;		// TRUE if current char from stuffbuf
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
EXTERN صحیح	vgetc_im_فعال;	// Input Method was active for last
					// character obtained from vgetc()
#اگر_ختم
EXTERN صحیح	maptick INIT(= 0);	// tick for each non-mapped char

EXTERN صحیح	must_redraw INIT(= 0);	    // type of redraw necessary
EXTERN صحیح	skip_redraw INIT(= FALSE);  // skip redraw once
EXTERN صحیح	do_redraw INIT(= FALSE);    // extra redraw once
#اگر_تعریف_شدہ FEAT_DIFF
EXTERN صحیح	need_diff_redraw INIT(= 0); // need to call diff_redraw()
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RELTIME
// flag set when 'redrawtime' timeout has been set
EXTERN صحیح	redrawtime_حد_بندی_سیٹ INIT(= FALSE);
#اگر_ختم

EXTERN صحیح	need_highlight_بدلا INIT(= TRUE);

#تعریف_کرو NSCRIPT 15
EXTERN FILE	*scriptin[NSCRIPT];	    // streams to read script from
EXTERN صحیح	curscript INIT(= 0);	    // index in scriptin[]
EXTERN FILE	*scriptout  INIT(= تہی);   // stream to write script to
EXTERN صحیح	read_cmd_fd INIT(= 0);	    // fd to read commands from

// Set to TRUE when an interrupt signal occurred.
// Volatile because it is used in signal handler catch_sigint().
EXTERN متلون sig_atomic_t got_int INIT(= FALSE);

// Set to TRUE when SIGUSR1 signal was detected.
// Volatile because it is used in signal handler catch_sigint().
EXTERN متلون sig_atomic_t got_sigusr1 INIT(= FALSE);

#اگر_تعریف_شدہ USE_رکن_CONSOLE
EXTERN صحیح	رکن_console INIT(= FALSE); // set to TRUE when console used
#اگر_ختم
EXTERN صحیح	termcap_فعال INIT(= FALSE);	// set by starttermcap()
EXTERN tmode_T	cur_tmode INIT(= TMODE_COOK);	// input terminal mode
EXTERN صحیح	bangredo INIT(= FALSE);	    // set to TRUE with ! command
EXTERN صحیح	searchcmdlen;		    // length of previous search cmd
#اگر_تعریف_شدہ FEAT_SYN_HL
EXTERN صحیح	reg_do_extmatch INIT(= 0);  // Used when compiling regexp:
					    // REX_SET to allow \z\(...\),
					    // REX_USE to allow \z\1 et al.
EXTERN reg_extmatch_T *re_extmatch_اندر INIT(= تہی); // Used by vim_regexec():
					    // strings for \z\1...\z\9
EXTERN reg_extmatch_T *re_extmatch_برآمد INIT(= تہی); // Set by vim_regexec()
					    // to store \z\(...\) matches
#اگر_ختم

EXTERN صحیح	did_outofmem_پیغام INIT(= FALSE);
					    // set after out of memory msg
EXTERN صحیح	did_swapwrite_پیغام INIT(= FALSE);
					    // set after swap write error msg
EXTERN صحیح	undo_off INIT(= FALSE);	    // undo switched off for now
EXTERN صحیح	global_busy INIT(= 0);	    // set when :global is executing
EXTERN صحیح	listcmd_busy INIT(= FALSE); // set when :argdo, :windo or
					    // :bufdo is executing
EXTERN صحیح	need_آغاز_insertmode INIT(= FALSE);
					    // start insert mode soon
#اگر defined(FEAT_EVAL)
EXTERN char_u	آخری_منوال[منوال_MAX_لمبائی] INIT(= "n"); // for ModeChanged event
#اگر_ختم
EXTERN char_u	*آخری_cmdline INIT(= تہی); // last command line (for ":)
EXTERN char_u	*repeat_cmdline INIT(= تہی); // command line for "."
EXTERN char_u	*new_آخری_cmdline INIT(= تہی);	// new value for last_cmdline
						//
EXTERN char_u	*autocmd_fname INIT(= تہی); // fname for <afile> on cmdline
EXTERN صحیح	autocmd_fname_بھرا;	     // autocmd_fname is full path
EXTERN صحیح	autocmd_bufnr INIT(= 0);     // fnum for <abuf> on cmdline
EXTERN char_u	*autocmd_مطابقت INIT(= تہی); // name for <amatch> on cmdline
EXTERN صحیح	aucmd_cmdline_بدلا_گنتی INIT(= 0);

EXTERN صحیح	did_cursorhold INIT(= TRUE);  // set when CursorHold t'gerd
EXTERN pos_T	آخری_cursormoved	      // for CursorMoved event
#اگر_تعریف_شدہ DO_INIT
		    = {0, 0, 0}
#اگر_ختم
		    ;

EXTERN صحیح	postponed_بانٹو INIT(= 0);  // for CTRL-W CTRL-] command
EXTERN صحیح	postponed_بانٹو_flags INIT(= 0);  // args for win_split()
EXTERN صحیح	postponed_بانٹو_tab INIT(= 0);  // cmdmod.cmod_tab
#اگر_تعریف_شدہ FEAT_QUICKFIX
EXTERN صحیح	g_do_tagpreview INIT(= 0);  // for tag preview commands:
					    // height of preview window
#اگر_ختم
EXTERN صحیح	g_لیبل_at_cursor INIT(= FALSE); // whether the tag command comes
					    // from the command line (0) or was
					    // invoked as a normal command (1)

EXTERN صحیح	replace_انحراف INIT(= 0);   // offset for replace_push()

EXTERN char_u	*escape_chars INIT(= (char_u *)" \t\\\"|");
					    // need backslash in cmd line

EXTERN صحیح	keep_help_flag INIT(= FALSE); // doing :ta from help file

/*
 * When a string option is NULL (which only happens in out-of-memory
 * situations), it is set to empty_option, to avoid having to check for NULL
 * everywhere.
 */
EXTERN char_u	*خالی_خانے_option INIT(= (char_u *)"");

EXTERN صحیح  redir_off INIT(= FALSE);	// no redirection for a moment
EXTERN FILE *redir_fd INIT(= تہی);	// message redirection file
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح  redir_reg INIT(= 0);	// message redirection register
EXTERN صحیح  redir_vname INIT(= 0);	// message redirection variable
EXTERN صحیح  redir_execute INIT(= 0);	// execute() redirection
#اگر_ختم

#اگر_تعریف_شدہ FEAT_LANGMAP
EXTERN char_u	langmap_mapchar[256];	// mapping for language keys
#اگر_ختم

EXTERN صحیح  save_p_ls INIT(= -1);	// Save 'laststatus' setting
EXTERN صحیح  save_p_wmh INIT(= -1);	// Save 'winminheight' setting
EXTERN صحیح  wild_menu_showing INIT(= 0);
#تعریف_کرو WM_SHOWN	1		// wildmenu showing
#تعریف_کرو WM_SCROLLED	2		// wildmenu showing with scroll

#اگر_تعریف_شدہ MSWIN
EXTERN char_u	toupper_tab[256];	// table for toupper()
EXTERN char_u	tolower_tab[256];	// table for tolower()
EXTERN صحیح	ملا_رجسٹر_arg INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_LINEBREAK
EXTERN حرف	breakat_flags[256];	// which characters are in 'breakat'
#اگر_ختم

// These are in version.c, call init_longVersion() before use.
خارجی حرف *نسخہ;
#اگر defined(HAVE_DATE_وقت) && defined(VMS) && defined(VAXC)
خارجی حرف long_نسخہ[];
#ورنہ
خارجی حرف *long_نسخہ;
#اگر_ختم

/*
 * Some file names are stored in pathdef.c, which is generated from the
 * Makefile to make their value depend on the Makefile.
 */
#اگر_تعریف_شدہ HAVE_PATHDEF
خارجی char_u *default_vim_dir;
خارجی char_u *default_vimruntime_dir;
خارجی char_u *all_cflags;
خارجی char_u *all_lflags;
# ifdef VMS
خارجی char_u *compiler_نسخہ;
خارجی char_u *compiled_arch;
خارجی char_u *compiled_vers;
# endif
خارجی char_u *compiled_user;
خارجی char_u *compiled_sys;
#اگر_ختم

EXTERN char_u	*homedir INIT(= تہی);

// When a window has a local directory, the absolute path of the global
// current directory is stored here (in allocated memory).  If the current
// directory is not a local directory, globaldir is NULL.
EXTERN char_u	*globaldir INIT(= تہی);

#اگر_تعریف_شدہ FEAT_FOLDING
EXTERN صحیح	disable_fold_update INIT(= 0);
#اگر_ختم

// Whether 'keymodel' contains "stopsel" and "startsel".
EXTERN صحیح	km_stopsel INIT(= FALSE);
EXTERN صحیح	km_startsel INIT(= FALSE);

EXTERN صحیح	cmdwin_type INIT(= 0);	// type of cmdline window or 0
EXTERN صحیح	cmdwin_نتیجہ INIT(= 0); // result of cmdline window or 0
EXTERN buf_T	*cmdwin_buf INIT(= تہی); // buffer of cmdline window or NULL
EXTERN win_T	*cmdwin_win INIT(= تہی); // window of cmdline window or NULL

EXTERN char_u no_سطریں_پیغام[]	INIT(= N_("--No lines in buffer--"));

EXTERN حرف typename_unknown[]	INIT(= N_("unknown"));
EXTERN حرف typename_int[]	INIT(= "int");
EXTERN حرف typename_longint[]	INIT(= "long int");
EXTERN حرف typename_longlongint[]	INIT(= "long long int");
EXTERN حرف typename_unsignedint[]	INIT(= "unsigned int");
EXTERN حرف typename_unsignedlongint[]	INIT(= "unsigned long int");
EXTERN حرف typename_unsignedlonglongint[]	INIT(= "unsigned long long int");
EXTERN حرف typename_اشارہ_گر[]	INIT(= N_("pointer"));
EXTERN حرف typename_percent[]	INIT(= N_("percent"));
EXTERN حرف typename_char[] INIT(= "char");
EXTERN حرف typename_string[]	INIT(= N_("string"));
EXTERN حرف typename_float[]	INIT(= "float");

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 * Also used for ":spellrepall".
 */
EXTERN طویل	sub_nsubs;	// total number of substitutions
EXTERN linenr_T	sub_nlines;	// total number of lines changed

#اگر_تعریف_شدہ FEAT_EVAL
// Used when a compiled :substitute has an expression.
EXTERN ساخت subs_اظہاریہ_S	*substitute_instr INIT(= تہی);
#اگر_ختم

// table to store parsed 'wildmode'
EXTERN char_u	wim_flags[4];

#اگر defined(FEAT_STL_OPT)
// whether titlestring and iconstring contains statusline syntax
# define STL_اندر_ICON	1
# define STL_اندر_TITLE	2
EXTERN صحیح      stl_syntax INIT(= 0);
#اگر_ختم

#اگر defined(FEAT_BEVAL) && !defined(NO_X11_INCLUDES)
EXTERN BalloonEval	*balloonEval INIT(= تہی);
EXTERN صحیح		balloon_Eval_For_رکن INIT(= FALSE);
# اگر defined(FEAT_NETBEANS_INTG)
EXTERN صحیح bevalServers INIT(= 0);
#  define BEVAL_NETBEANS		0x01
# endif
#اگر_ختم

#اگر_تعریف_شدہ CURSOR_SHAPE
// the table is in misc2.c, because of initializations
خارجی cursorentry_T shape_جدول[SHAPE_IDX_گنتی];
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PRINTER
/*
 * Printer stuff shared between hardcopy.c and machine-specific printing code.
 */
# define OPT_PRINT_چوٹی		0
# define OPT_PRINT_BOT		1
# define OPT_PRINT_بایاں		2
# define OPT_PRINT_دایاں	3
# define OPT_PRINT_HEADERHEIGHT	4
# define OPT_PRINT_SYNTAX	5
# define OPT_PRINT_نمبر	6
# define OPT_PRINT_WRAP		7
# define OPT_PRINT_DUPLEX	8
# define OPT_PRINT_PORTRAIT	9
# define OPT_PRINT_PAPER	10
# define OPT_PRINT_COLLATE	11
# define OPT_PRINT_JOBSPLIT	12
# define OPT_PRINT_FORMFEED	13
# define OPT_PRINT_FORMAT	14

# define OPT_PRINT_عدد_OPTIONS	15

EXTERN option_جدول_T printer_opts[OPT_PRINT_عدد_OPTIONS]
# ifdef DO_INIT
    = {
	{"top",	TRUE, 0, تہی, 0, FALSE},
	{"bottom",	TRUE, 0, تہی, 0, FALSE},
	{"left",	TRUE, 0, تہی, 0, FALSE},
	{"right",	TRUE, 0, تہی, 0, FALSE},
	{"header",	TRUE, 0, تہی, 0, FALSE},
	{"syntax",	FALSE, 0, تہی, 0, FALSE},
	{"number",	FALSE, 0, تہی, 0, FALSE},
	{"wrap",	FALSE, 0, تہی, 0, FALSE},
	{"duplex",	FALSE, 0, تہی, 0, FALSE},
	{"portrait", FALSE, 0, تہی, 0, FALSE},
	{"paper",	FALSE, 0, تہی, 0, FALSE},
	{"collate",	FALSE, 0, تہی, 0, FALSE},
	{"jobsplit", FALSE, 0, تہی, 0, FALSE},
	{"formfeed", FALSE, 0, تہی, 0, FALSE},
	{"format", FALSE, 0, تہی, 0, FALSE},
    }
# endif
    ;

// For prt_get_unit().
# define PRT_UNIT_NONE	-1
# define PRT_UNIT_PERC	0
# define PRT_UNIT_INCH	1
# define PRT_UNIT_MM	2
# define PRT_UNIT_نقطہ	3
# define PRT_UNIT_نام {"pc", "in", "mm", "pt"}
#اگر_ختم

#اگر (defined(FEAT_PRINTER) && defined(FEAT_STL_OPT)) \
	    || defined(FEAT_GUI_TABLINE)
// Page number used for %N in 'pageheader' and 'guitablabel'.
EXTERN linenr_T printer_صفحہ_عدد;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_XCLIPBOARD
// xterm display name
EXTERN حرف	*xterm_display INIT(= تہی);

// whether xterm_display was allocated, when FALSE it points into argv[]
EXTERN صحیح	xterm_display_allocated INIT(= FALSE);

// xterm display pointer
EXTERN Display	*xterm_dpy INIT(= تہی);
#اگر_ختم
#اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
EXTERN XtAppContext app_context INIT(= (XtAppContext)تہی);
#اگر_ختم

#اگر defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
EXTERN guint32	gtk_socket_id INIT(= 0);
EXTERN صحیح	echo_wid_arg INIT(= FALSE);	// --echo-wid argument
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_MSWIN
/*
 * The value of the --windowid argument.
 * For embedding gvim inside another application.
 */
EXTERN long_u	win_socket_id INIT(= 0);
#اگر_ختم

#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
EXTERN صحیح	typebuf_was_filled INIT(= FALSE); // received text from client
						  // or from feedkeys()
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
EXTERN char_u	*serverName INIT(= تہی);	// name of the server
# ifdef FEAT_X11
EXTERN کھڑکی	comm_کھڑکی INIT(= None);
EXTERN کھڑکی	client_کھڑکی INIT(= None);
EXTERN Atom	commProperty INIT(= None);
EXTERN char_u	*server_Delayed_آغاز_Name INIT(= تہی);
# elif defined(MSWIN)
EXTERN HWND	client_کھڑکی INIT(= 0);
# endif
#اگر_ختم

#اگر defined(UNIX) || defined(VMS)
EXTERN صحیح	رکن_is_xterm INIT(= FALSE);	// xterm-like 'term'
#اگر_ختم

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
EXTERN حرف	psepc INIT(= '\\');	// normal path separator character
EXTERN حرف	psepcN INIT(= '/');	// abnormal path separator character
// normal path separator string
EXTERN حرف	pseps[2] INIT2('\\', 0);
#اگر_ختم

// Set to TRUE when an operator is being executed with virtual editing, MAYBE
// when no operator is being executed, FALSE otherwise.
EXTERN صحیح	virtual_op INIT(= MAYBE);

#اگر_تعریف_شدہ FEAT_SYN_HL
// Display tick, incremented for each call to update_screen()
EXTERN disptick_T	display_tick INIT(= 0);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SPELL
// Line in which spell checking wasn't highlighted because it touched the
// cursor position in Insert mode.
EXTERN linenr_T		spell_redraw_lnum INIT(= 0);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CONCEAL
// Set when the cursor line needs to be redrawn.
EXTERN صحیح		need_cursor_سطر_redraw INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ USE_MCH_ERRMSG
// Grow array to collect error messages in until they can be displayed.
EXTERN garray_T error_ga
# ifdef DO_INIT
		    = {0, 0, 0, 0, تہی}
# endif
		    ;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
EXTERN حرف *netbeansArg INIT(= تہی);	// the -nb[:host:port:passwd] arg
EXTERN صحیح netbeansFireChanges INIT(= 1); // send buffer changes if != 0
EXTERN صحیح netbeansForcedQuit INIT(= 0);// don't write modified files
EXTERN صحیح netbeansReadFile INIT(= 1);	// OK to read from disk if != 0
EXTERN صحیح netbeans_Suppress_No_سطریں INIT(= 0); // skip "No lines in buffer"
#اگر_ختم

/*
 * Some messages that can be shared are included here.
 */
EXTERN حرف چوٹی_bot_پیغام[]   INIT(= N_("search hit TOP, continuing at BOTTOM"));
EXTERN حرف bot_چوٹی_پیغام[]   INIT(= N_("search hit BOTTOM, continuing at TOP"));

EXTERN حرف سطر_پیغام[]	    INIT(= N_(" line "));

#اگر_تعریف_شدہ FEAT_CRYPT
EXTERN حرف need_کلید_پیغام[]  INIT(= N_("Need encryption key for \"%s\""));
#اگر_ختم

/*
 * Comms. with the session manager (XSMP)
 */
#اگر_تعریف_شدہ USE_XSMP
EXTERN صحیح xsmp_icefd INIT(= -1);   // The actual connection
#اگر_ختم

#اگر_تعریف_شدہ STARTUPTIME
EXTERN FILE *وقت_fd INIT(= تہی);  // where to write startup timing
#اگر_ختم

/*
 * Some compilers warn for not using a return value, but in some situations we
 * can't do anything useful with the value.  Assign to this variable to avoid
 * the warning.
 */
EXTERN صحیح vim_ignored;
EXTERN حرف *vim_ignoredp;

#اگر_تعریف_شدہ FEAT_EVAL
// set by alloc_fail(): ID
EXTERN alloc_id_T  alloc_fail_id INIT(= aid_none);
// set by alloc_fail(), when zero alloc() returns NULL
EXTERN صحیح  alloc_fail_countdown INIT(= -1);
// set by alloc_fail(), number of times alloc() returns NULL
EXTERN صحیح  alloc_fail_repeat INIT(= 0);

// flags set by test_override()
EXTERN صحیح  disable_char_avail_for_testing INIT(= FALSE);
EXTERN صحیح  disable_redraw_for_testing INIT(= FALSE);
EXTERN صحیح  ignore_redraw_flag_for_testing INIT(= FALSE);
EXTERN صحیح  nfa_fail_for_testing INIT(= FALSE);
EXTERN صحیح  disable_syn_idlist_cache_for_testing INIT(= FALSE);
EXTERN صحیح  no_query_mouse_for_testing INIT(= FALSE);
EXTERN صحیح  ui_delay_for_testing INIT(= 0);
EXTERN صحیح  reset_رکن_props_on_termresponse INIT(= FALSE);
EXTERN صحیح  disable_vterm_title_for_testing INIT(= FALSE);
EXTERN طویل override_sysinfo_uptime INIT(= -1);
EXTERN صحیح  override_autoload INIT(= FALSE);
EXTERN صحیح  override_defcompile INIT(= FALSE);
EXTERN صحیح  ml_get_alloc_سطریں INIT(= FALSE);
EXTERN صحیح  ignore_unreachable_رمز_for_testing INIT(= FALSE);

EXTERN صحیح  اندر_free_unref_اشیاء INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TIMERS
EXTERN صحیح  did_جوڑو_timer INIT(= FALSE);
EXTERN صحیح  timer_busy INIT(= 0);   // when timer is inside vgetc() then > 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
EXTERN صحیح  داخلہ_busy INIT(= 0);   // when inside get_user_input() then > 0

EXTERN lval_جڑ_T	*lval_جڑ INIT(= تہی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_BEVAL_رکن
EXTERN صحیح  bevalexpr_due_سیٹ INIT(= FALSE);
EXTERN proftime_T bevalexpr_due;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
EXTERN وقت_T وقت_for_testing INIT(= 0);

EXTERN صحیح echo_attr INIT(= 0);   // attributes used for ":echo"

// Abort conversion to string after a recursion error.
EXTERN صحیح  did_echo_string_emsg INIT(= FALSE);

// Used for checking if local variables or arguments used in a lambda.
EXTERN صحیح *eval_lavars_مستعمل INIT(= تہی);

// Only filled for Win32.
EXTERN حرف windows_نسخہ[20] INIT(= {0});

// Used for lv_first in a non-materialized range() list.
EXTERN listitem_T range_list_شے;

// Passed to an eval() function to enable evaluation.
EXTERN evalarg_T EVALARG_EVALUATE
# ifdef DO_INIT
	= {EVAL_EVALUATE, 0, تہی, تہی, تہی, تہی, GA_خالی_خانے, GA_خالی_خانے, تہی,
			 {0, 0, (صحیح)حجم(char_u *), 20, تہی}, 0, تہی, تہی}
# endif
	;
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
EXTERN صحیح ctrl_break_was_pressed INIT(= FALSE);
EXTERN HINSTANCE g_hinst INIT(= تہی);
#اگر_ختم


#اگر defined(FEAT_کام_CHANNEL)
EXTERN حرف *ch_part_نام[]
# ifdef DO_INIT
		= {"sock", "out", "err", "in"}
# endif
		;

// Whether a redraw is needed for appending a line to a buffer.
EXTERN صحیح channel_need_redraw INIT(= FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
// This flag is set when outputting a terminal control code and reset in
// out_flush() when characters have been written.
EXTERN صحیح ch_log_output INIT(= FALSE);

EXTERN صحیح did_repeated_پیغام INIT(= 0);
# define REPEATED_پیغام_LOOKING	    1
# define REPEATED_پیغام_SAFESTATE	    2
#اگر_ختم

// While executing a regexp and set to OPTION_MAGIC_ON or OPTION_MAGIC_OFF this
// overrules p_magic.  Otherwise set to OPTION_MAGIC_NOT_SET.
EXTERN optmagic_T magic_overruled INIT(= OPTION_MAGIC_NOT_سیٹ);

// Skip win_fix_cursor() call for 'splitkeep' when cmdwin is closed.
EXTERN صحیح skip_win_fix_cursor INIT(= FALSE);
// Skip win_fix_scroll() call for 'splitkeep' when closing tab page.
EXTERN صحیح skip_win_fix_scroll INIT(= FALSE);
// Skip update_topline() call while executing win_fix_scroll().
EXTERN صحیح skip_update_topline INIT(= FALSE);

// 'showcmd' buffer shared between normal.c and statusline code
#تعریف_کرو SHOWCMD_BUFLEN (SHOWCMD_ستون_تعداد + 1 + 30)
EXTERN char_u showcmd_buf[SHOWCMD_BUFLEN];

#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
EXTERN صحیح	p_tgc_سیٹ INIT(= FALSE);
#اگر_ختم

// If we've already warned about missing/unavailable clipboard
EXTERN bool did_warn_clipboard INIT(= FALSE);

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
EXTERN clipmethod_T clipmethod INIT(= CLIPMETHOD_NONE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_WAYLAND

// Wayland display name for global connection (ex. wayland-0). Can be NULL
EXTERN حرف *wayland_display_name INIT(= تہی);

// Don't connect to Wayland compositor if TRUE
EXTERN صحیح wayland_no_connect INIT(= FALSE);

#اگر_ختم

#اگر defined(FEAT_CLIENTSERVER)

// Backend for clientserver functionality
قسمتعریف شمار {
    CLIENTSERVER_طریقہ_NONE,
# ifdef FEAT_X11
    CLIENTSERVER_طریقہ_X11,
# endif
# ifdef MSWIN
    CLIENTSERVER_طریقہ_MSWIN,
# endif
# ifdef FEAT_SOCKETSERVER
    CLIENTSERVER_طریقہ_SOCKET
# endif
} clientserver_طریقہ_T;

EXTERN clientserver_طریقہ_T clientserver_طریقہ

# ifdef FEAT_X11
INIT(= CLIENTSERVER_طریقہ_X11);
# elif defined(MSWIN)
INIT(= CLIENTSERVER_طریقہ_MSWIN);
# elif defined(FEAT_SOCKETSERVER)
INIT(= CLIENTSERVER_طریقہ_SOCKET);
# ورنہ
INIT(= CLIENTSERVER_طریقہ_NONE);
# endif

#اگر_ختم

#اگر_تعریف_شدہ FEAT_SOCKETSERVER
// Path to socket of last client that communicated with us
EXTERN char_u *client_socket INIT(= تہی);
#اگر_ختم

// If the <xOSC> key should be propagated from vgetc()
EXTERN صحیح allow_osc_کلید INIT(= 0);

#اگر_تعریف_شدہ FEAT_EVAL
// Global singly linked list of redraw listeners
EXTERN redraw_listener_T *redraw_listeners INIT(= تہی);
EXTERN bool inside_redraw_on_آغاز_cb INIT(= false);
#اگر_ختم

// If greater than zero, then silence the W23/W24 warning.
EXTERN صحیح silence_w23_w24_پیغام INIT( = 0);

#اگر_تعریف_شدہ FEAT_EVAL
// Used by TextPutPost/TextPutPre autocommands for the '.' register. If
// "add_last_insert" is == 1, then "stuff_inserted" will add the last inserted
// text to "last_insert_ga".
EXTERN garray_T آخری_داخل_کرو_ga INIT5(0, 0, 1, 64, تہی);
EXTERN صحیح	جوڑو_آخری_داخل_کرو INIT(= 0);
#اگر_ختم

#اگر defined(FEAT_IMAGE) || defined(FEAT_EVAL)
// Cached value, re calculated when window size changes. Defaults to 8x16
// pixels. Note that these are in logical pixels (when using GUI).
EXTERN صحیح cell_چوڑائی INIT(= 8);
EXTERN صحیح cell_اونچائی INIT(= 16);
#اگر_ختم
