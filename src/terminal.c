/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Terminal window support, see ":help :terminal".
 *
 * There are three parts:
 * 1. Generic code for all systems.
 *    Uses libvterm for the terminal emulator.
 * 2. The MS-Windows implementation.
 *    Uses winpty.
 * 3. The Unix-like implementation.
 *    Uses pseudo-tty's (pty's).
 *
 * For each terminal one VTerm is constructed.  This uses libvterm.  A copy of
 * this library is in the libvterm directory.
 *
 * When a terminal window is opened, a job is started that will be connected to
 * the terminal emulator.
 *
 * If the terminal window has keyboard focus, typed keys are converted to the
 * terminal encoding and writing to the job over a channel.
 *
 * If the job produces output, it is written to the terminal emulator.  The
 * terminal emulator invokes callbacks when its screen content changes.  The
 * line range is stored in tl_dirty_row_start and tl_dirty_row_end.  Once in a
 * while, if the terminal window is visible, the screen contents is drawn.
 *
 * When the job ends the text is put in a buffer.  Redrawing then happens from
 * that buffer, attributes come from the scrollback buffer tl_scrollback.
 * When the buffer is changed it is turned into a normal buffer, the attributes
 * in tl_scrollback are no longer used.
 */

#अन्तर्भाव "vim.h"

#जर defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#अन्तर्भाव "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
प्रकारव्याख्या संरचना {
    VTermScreenCellAttrs	attrs;
    अक्षर			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

प्रकारव्याख्या संरचना sb_ओळ_S {
    पूर्णांक		sb_cols;	// can differ per line
    पूर्णांक		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_ओळ_T;

#ifdef MSWIN
# ifndef HPCON
#  define HPCON VOID*
# endif
# ifndef EXTENDED_STARTUPINFO_PRESENT
#  define EXTENDED_STARTUPINFO_PRESENT 0x00080000
# endif
# ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#  define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
# endif
प्रकारव्याख्या संरचना _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
संरचना terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#जर defined(FEAT_GUI)
    पूर्णांक		tl_system;	// when non-zero used for :!cmd output
    पूर्णांक		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    पूर्णांक		tl_vterm_size_changed;

    पूर्णांक		tl_normal_mode; // TRUE: Terminal-Normal mode
    पूर्णांक		tl_channel_closing;
    पूर्णांक		tl_channel_closed;
    पूर्णांक		tl_channel_recently_closed; // still need to handle tl_finish

    पूर्णांक		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    शून्य	*tl_winpty_config;
    शून्य	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    खाता	*tl_out_fd;
#endif
#जर defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    पूर्णांक		tl_rows;
    पूर्णांक		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    पूर्णांक		tl_dirty_row_start; // MAX_ROW if nothing dirty
    पूर्णांक		tl_dirty_row_end;   // row below last one to update
    पूर्णांक		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    पूर्णांक		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    पूर्णांक		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    पूर्णांक		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    पूर्णांक		tl_scrollback_snapshot;
    पूर्णांक		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_शीर्ष_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    पूर्णांक		tl_cursor_visible;
    पूर्णांक		tl_cursor_blink;
    पूर्णांक		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    पूर्णांक		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
स्थिर term_T *first_term = रिक्त;

// Terminal active in terminal_loop().
स्थिर term_T *in_terminal_loop = रिक्त;

#ifdef MSWIN
स्थिर BOOL has_winpty = FALSE;
स्थिर BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define किल्ली_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    साठी ((term) = first_term; (term) != रिक्त; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
स्थिर पूर्णांक term_and_job_init(term_T *term, typval_T *argvar, अक्षर **argv, jobopt_T *opt, jobopt_T *orig_opt);
स्थिर पूर्णांक create_pty_only(term_T *term, jobopt_T *opt);
स्थिर शून्य term_report_winsize(term_T *term, पूर्णांक rows, पूर्णांक cols);
स्थिर शून्य term_free_vterm(term_T *term);
#ifdef FEAT_GUI
स्थिर शून्य update_system_term(term_T *term);
#endif

स्थिर शून्य handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
स्थिर पूर्णांक term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
स्थिर char_u	*last_set_cursor_color = रिक्त;
स्थिर char_u	*desired_cursor_color = रिक्त;
स्थिर पूर्णांक	last_set_cursor_shape = -1;
स्थिर पूर्णांक	desired_cursor_shape = -1;
स्थिर पूर्णांक	last_set_cursor_blink = -1;
स्थिर पूर्णांक	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    स्थिर पूर्णांक
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    जर (lhs_color != रिक्त && rhs_color != रिक्त)
	परत STRCMP(lhs_color, rhs_color) == 0;
    परत lhs_color == रिक्त && rhs_color == रिक्त;
}

    स्थिर शून्य
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    जर (cursor_color_equal(*to_color, from_color))
	परत;
    vim_free(*to_color);
    *to_color = (from_color == रिक्त) ? रिक्त : vim_strsave(from_color);
}

    स्थिर char_u *
cursor_color_get(char_u *color)
{
    परत (color == रिक्त) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    स्थिर पूर्णांक
parse_termwinsize(win_T *wp, पूर्णांक *rows, पूर्णांक *cols)
{
    पूर्णांक	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    जर (*wp->w_p_tws == NUL)
	परत FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    जर (p == रिक्त)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = म_से_प((अक्षर *)wp->w_p_tws);
    *cols = म_से_प((अक्षर *)p + 1);
    जर (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    जर (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    परत minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    स्थिर शून्य
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    पूर्णांक rows, cols;
    पूर्णांक	minsize;

#ifdef FEAT_GUI
    जर (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	परत;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    जर (minsize)
    {
	जर (term->tl_rows < rows)
	    term->tl_rows = rows;
	जर (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    जर ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    अन्यथा जर (rows != 0)
	term->tl_rows = rows;
    जर ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    अन्यथा जर (cols != 0)
	term->tl_cols = cols;

    जर (!opt->jo_hidden)
    {
	जर (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	जर (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	जर (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((अक्षर *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_मूल्य_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    शून्य
init_job_options(jobopt_T *opt)
{
    clear_job_options(opt);

    opt->jo_mode = CH_MODE_RAW;
    opt->jo_out_mode = CH_MODE_RAW;
    opt->jo_err_mode = CH_MODE_RAW;
    opt->jo_set = JO_MODE | JO_OUT_MODE | JO_ERR_MODE;
}

/*
 * Set job options mandatory for a terminal job.
 */
    स्थिर शून्य
setup_job_options(jobopt_T *opt, पूर्णांक rows, पूर्णांक cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    जर (!(opt->jo_set & JO_OUT_IO))
#endif
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_OUT] = JIO_BUFFER;
	opt->jo_io_buf[PART_OUT] = curbuf->b_fnum;
	opt->jo_modifiable[PART_OUT] = 0;
	opt->jo_set |= JO_OUT_IO + JO_OUT_BUF + JO_OUT_MODIFIABLE;
    }

#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    जर (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    जर ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    जर ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    स्थिर शून्य
term_flush_messages(शून्य)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    स्थिर शून्य
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    जर (old_curbuf != रिक्त)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_buffer = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
term_start(
	typval_T    *argvar,
	अक्षर	    **argv,
	jobopt_T    *opt,
	पूर्णांक	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = रिक्त;
    पूर्णांक		res;
    buf_T	*newbuf;
    पूर्णांक		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    जर (check_restricted() || check_secure())
	परत रिक्त;
    जर (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_ओळ_window));
	परत रिक्त;
    }

    जर ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != रिक्त
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != रिक्त
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	परत रिक्त;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    जर (term == रिक्त)
	परत रिक्त;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, आकार(sb_ओळ_T), 300);
    ga_init2(&term->tl_scrollback_postponed, आकार(sb_ओळ_T), 300);
    ga_init2(&term->tl_osc_buf, आकार(अक्षर), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    जर (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	जर (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    परत रिक्त;
	}
	जर (do_ecmd(0, रिक्त, रिक्त, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    परत रिक्त;
	}
    }
    अन्यथा जर (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", रिक्त, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	जर (buf == रिक्त || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    परत रिक्त;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    अन्यथा
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	जर (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_मोजणी = 1;
	}
	जर (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_मोजणी = 1;
	}

	पूर्णांक cmod_split_modified = FALSE;
	जर (vertical)
	{
	    जर (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	जर (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	जर (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    परत रिक्त;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    जर (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	जर (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	जर (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, रिक्त, रिक्त, FALSE, curbuf);

    जर (opt->jo_term_name != रिक्त)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    अन्यथा जर (argv != रिक्त)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    अन्यथा
    {
	पूर्णांक	i;
	माप_प्रकार	len;
	char_u	*cmd, *p;

	जर (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    जर (cmd == रिक्त)
		cmd = (char_u *)"";
	    अन्यथा जर (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	अन्यथा जर (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == रिक्त
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == रिक्त)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	साठी (i = 0; p != रिक्त; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    जर (i == 0)
		vim_snprintf((अक्षर *)p, len, "!%s", cmd);
	    अन्यथा
		vim_snprintf((अक्षर *)p, len, "!%s (%d)", cmd, i);
	    जर (buflist_findname(p) == रिक्त)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		थांबा;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, रिक्त, रिक्त, FALSE, curbuf);

    जर (opt->jo_term_opencmd != रिक्त)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    जर (opt->jo_eof_chars != रिक्त)
	term->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    set_string_option_direct((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    set_term_and_win_size(term, opt);
#ifdef MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, आकार(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    जर (flags & TERM_START_NOJOB)
	परत curbuf;

#जर defined(FEAT_SESSION)
    // Remember the command for the session file.
    जर (opt->jo_term_norestore || argv != रिक्त)
	term->tl_command = vim_strsave((char_u *)"NONE");
    अन्यथा जर (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	जर (cmd != रिक्त && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    अन्यथा जर (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != रिक्त
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_वस्तुसंच(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    जर (s == रिक्त)
		थांबा;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    जर (p == रिक्त)
		थांबा;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	जर (item == रिक्त)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	अन्यथा
	    ga_clear(&ga);
    }
#endif

    जर (opt->jo_term_kill != रिक्त)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    जर (opt->jo_term_api != रिक्त)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    अन्यथा
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    जर (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    जर (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	जर (term->tl_palette == रिक्त)
	{
	    vim_free(term);
	    परत रिक्त;
	}
	स_नकल(term->tl_palette, opt->jo_ansi_colors, आकार(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    जर (argv == रिक्त
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != रिक्त
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    अन्यथा
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    जर (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	जर (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = संदेश_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	जर (old_curbuf != रिक्त)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	अन्यथा जर (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| आदान_busy)
	{
	    char_u ignore[4];

	    // When waiting for input need to return and possibly end up in
	    // terminal_loop() instead.
	    ignore[0] = K_SPECIAL;
	    ignore[1] = KS_EXTRA;
	    ignore[2] = KE_IGNORE;
	    ignore[3] = NUL;
	    ins_typebuf(ignore, REMAP_NONE, 0, TRUE, FALSE);
	    typebuf_was_filled = TRUE;
	}
    }
    अन्यथा
    {
	term_close_buffer(curbuf, old_curbuf);
	परत रिक्त;
    }

    apply_autocmds(EVENT_TERMINALOPEN, रिक्त, रिक्त, FALSE, newbuf);
    जर (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, रिक्त, रिक्त, FALSE, newbuf);
    परत newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    शून्य
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    पूर्णांक		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = रिक्त;
    पूर्णांक		scroll_save = संदेश_scroll;

    संदेश_scroll = FALSE;		// don't scroll here
    autowrite_all();
    संदेश_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    पर्यंत (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	जर (ep != रिक्त)
	{
	    जर (ep < p)
		p = ep;
	    अन्यथा
		ep = रिक्त;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((पूर्णांक)(p - cmd) == आकार(name) - 1 \
				 && STRNICMP(cmd, name, आकार(name) - 1) == 0)
	जर (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	अन्यथा जर (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	अन्यथा जर (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	अन्यथा जर (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	अन्यथा जर (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	अन्यथा जर (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	अन्यथा जर (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	अन्यथा जर (OPTARG_HAS("kill") && ep != रिक्त)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	अन्यथा जर (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    जर (ep != रिक्त)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    अन्यथा
		opt.jo_term_api = रिक्त;
	}
	अन्यथा जर (OPTARG_HAS("rows") && ep != रिक्त && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = म_से_प((अक्षर *)ep + 1);
	    p = skiptowhite(cmd);
	}
	अन्यथा जर (OPTARG_HAS("cols") && ep != रिक्त && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = म_से_प((अक्षर *)ep + 1);
	    p = skiptowhite(cmd);
	}
	अन्यथा जर (OPTARG_HAS("eof") && ep != रिक्त)
	{
	    char_u *buf = रिक्त;
	    char_u *किल्ल्या;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    किल्ल्या = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, रिक्त);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(किल्ल्या);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	अन्यथा जर ((पूर्णांक)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != रिक्त)
	{
	    पूर्णांक tty_type = NUL;

	    p = skiptowhite(cmd);
	    जर (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    अन्यथा जर (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    अन्यथा
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str), "type");
		जा theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	अन्यथा
	{
	    जर (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    जा theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    जर (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	जर (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    जर (eap->addr_मोजणी > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_शीर्ष | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_शीर्ष = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    जर (opt_shell && tofree == रिक्त)
    {
#ifdef UNIX
	अक्षर	**argv = रिक्त;
	char_u	*tofree1 = रिक्त;
	char_u	*tofree2 = रिक्त;

	// :term ++shell command
	जर (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(रिक्त, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	जा theend;
#अन्यथा
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	जर (newcmd == रिक्त)
	    जा theend;
	tofree = newcmd;
	vim_snprintf((अक्षर *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# अन्यथा
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	जा theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, रिक्त, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    स्थिर char_u *
get_terminalopt_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    // Note: Keep this in sync with ex_terminal.
    स्थिर अक्षर *(p_termopt_मूल्ये[]) =
    {
	"close",
	"noclose",
	"open",
	"curwin",
	"hidden",
	"norestore",
	"shell",
	"kill=",
	"rows=",
	"cols=",
	"eof=",
	"type=",
	"api=",
    };

    जर (idx < (पूर्णांक)ARRAY_LENGTH(p_termopt_मूल्ये))
	परत (char_u*)p_termopt_मूल्ये[idx];
    परत रिक्त;
}

    स्थिर char_u *
get_termkill_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    स्थिर अक्षर *(p_termkill_मूल्ये[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    जर (idx < (पूर्णांक)ARRAY_LENGTH(p_termkill_मूल्ये))
	परत (char_u*)p_termkill_मूल्ये[idx];
    परत रिक्त;
}

/*
 * Command-line expansion for :terminal [options]
 */
    पूर्णांक
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	पूर्णांक	    *अंक_Matches)
{
    जर (xp->xp_pattern > xp->xp_ओळ && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, पूर्णांक) = रिक्त;

	char_u *name_end = xp->xp_pattern - 1;
	जर (name_end - xp->xp_ओळ >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	जर (cb != रिक्त)
	{
	    परत ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    अंक_Matches,
		    cb,
		    FALSE);
	}
	परत FAIL;
    }
    परत ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    अंक_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#जर defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    पूर्णांक
term_write_session(खाता *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    स्थिरांक पूर्णांक	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    जर (terminal_bufs != रिक्त && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	अक्षर	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, आकार(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	जर (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    जर (ख_लिखो(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		परत FAIL;
	    परत put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    जर (ख_लिखो(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	परत FAIL;
# ifdef MSWIN
    जर (ख_लिखो(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	परत FAIL;
# endif
    जर (term->tl_command != रिक्त)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	जर (quoted_command == रिक्त)
	    परत FAIL;

	पूर्णांक ret = ख_माला_दो(".. ' ' .. ", fd) < 0
		    || ख_माला_दो((अक्षर *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	जर (ret)
	    परत FAIL;
    }
    जर (put_eol(fd) != OK)
	परत FAIL;

    जर (ख_लिखो(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	परत FAIL;

    जर (terminal_bufs != रिक्त && wp->w_buffer->b_nwindows > 1)
    {
	अक्षर *hash_किल्ली = alloc(NUMBUFLEN);

	vim_snprintf(hash_किल्ली, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_किल्ली, "terminal session");
    }

    परत put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    पूर्णांक
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    परत term != रिक्त && (term->tl_command == रिक्त
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    स्थिर शून्य
free_scrollback(term_T *term)
{
    पूर्णांक i;

    साठी (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_ओळ_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    साठी (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_ओळ_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
स्थिर term_T	*terminals_to_free = रिक्त;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    शून्य
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    जर (term == रिक्त)
	परत;

    // Unlink the terminal form the list of terminals.
    जर (first_term == term)
	first_term = term->tl_next;
    अन्यथा
	साठी (tp = first_term; tp->tl_next != रिक्त; tp = tp->tl_next)
	    जर (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		थांबा;
	    }

    जर (term->tl_job != रिक्त)
    {
	जर (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, रिक्त, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = रिक्त;
    जर (in_terminal_loop == term)
	in_terminal_loop = रिक्त;
}

    शून्य
free_unused_terminals(शून्य)
{
    पर्यंत (terminals_to_free != रिक्त)
    {
	term_T	    *term = terminals_to_free;

	terminals_to_free = term->tl_next;

	free_scrollback(term);
	ga_clear(&term->tl_osc_buf);

	term_free_vterm(term);
	vim_free(term->tl_api);
	vim_free(term->tl_title);
#ifdef FEAT_SESSION
	vim_free(term->tl_command);
#endif
	vim_free(term->tl_kill);
	vim_free(term->tl_status_text);
	vim_free(term->tl_opencmd);
	vim_free(term->tl_eof_chars);
	vim_free(term->tl_arg0_cmd);
#ifdef MSWIN
	जर (term->tl_out_fd != रिक्त)
	    ख_बंद(term->tl_out_fd);
#endif
	vim_free(term->tl_highlight_name);
	vim_free(term->tl_cursor_color);
	vim_free(term->tl_palette);
	vim_free(term);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    स्थिर ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    पूर्णांक		i;

    साठी (i = 0; i < 3; ++i)
    {
	पूर्णांक fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	जर (mch_isatty(fd))
	    परत parts[i];
    }
#endif
    परत PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    स्थिर शून्य
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    अक्षर   buf[किल्ली_BUF_LEN];
    माप_प्रकार curlen = vterm_output_read(vterm, buf, किल्ली_BUF_LEN);

    जर (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (पूर्णांक)curlen, रिक्त);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    स्थिर शून्य
term_write_job_output(term_T *term, char_u *संदेश_arg, माप_प्रकार len_arg)
{
    char_u	*संदेश = संदेश_arg;
    माप_प्रकार	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    माप_प्रकार	prevlen = vterm_output_get_buffer_current(vterm);
    माप_प्रकार	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    जर (len > limit)
    {
	char_u *p = संदेश + len - limit;

	p -= (*mb_head_off)(संदेश, p);
	len -= p - संदेश;
	संदेश = p;
    }

    vterm_आदान_write(vterm, (अक्षर *)संदेश, len);

    // flush vterm buffer when vterm responded to control sequence
    जर (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    स्थिर शून्य
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    जर (popup_is_popup(wp))
    {
	wp->w_wrow += popup_शीर्ष_extra(wp);
	wp->w_wcol += popup_डावा_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    अन्यथा
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    स्थिर शून्य
update_cursor(term_T *term, पूर्णांक redraw)
{
    जर (term->tl_normal_mode)
	परत;
#ifdef FEAT_GUI
    जर (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    अन्यथा
#endif
    जर (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    अन्यथा
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    जर (redraw)
    {
	aco_save_T	aco;

	जर (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	जर (gui.in_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#endif
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++term->tl_buffer->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, रिक्त, रिक्त, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    स्थिर शून्य
scrollbackline_pos_in_buf(term_T *term, पूर्णांक row, linenr_T *lnum, पूर्णांक *start_col, माप_प्रकार *start_pos)
{
    sb_ओळ_T	*ओळी = (sb_ओळ_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    माप_प्रकार      calc_pos = 0;
    पूर्णांक         calc_col = 0;
    पूर्णांक         i;

    जर (row < 0 || row >= term->tl_scrollback.ga_len)
	परत;

    जर (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	साठी (i = term->tl_scrollback_scrolled + 1; i > 0 && ओळी[i].continuation; --i)
	{
	    calc_pos += ओळी[i - 1].sb_bytes;
	    calc_col += ओळी[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	जर (ओळी[i].continuation)
	    ++i;
    }
    अन्यथा
    {
	i = 1;
	calc_lnum = 1;
    }

    साठी (; i <= row; ++i)
    {
	जर (!ओळी[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	अन्यथा
	{
	    calc_pos += ओळी[i - 1].sb_bytes;
	    calc_col += ओळी[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    जर (start_col)
	*start_col = calc_col;
    जर (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    स्थिर शून्य
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, पूर्णांक col, पूर्णांक *row, पूर्णांक *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_ओळ_T	*ओळी = (sb_ओळ_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    पूर्णांक         calc_col = col;
    linenr_T    l;

    जर (lnum > buf->b_ml.ml_ओळ_मोजणी)
	परत;

    जर (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	पर्यंत (calc_row < term->tl_scrollback.ga_len && ओळी[calc_row].continuation)
	    ++calc_row;
    }
    अन्यथा
    {
	calc_row = 0;
	l = 1;
    }

    पर्यंत (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	जर (!ओळी[calc_row].continuation)
	    ++l;
    }

    पर्यंत (calc_row + 1 < term->tl_scrollback.ga_len && ओळी[calc_row + 1].continuation
	    && calc_col >= ओळी[calc_row].sb_cols)
    {
	calc_col -= ओळी[calc_row].sb_cols;
	++calc_row;
    }

    जर (row)
	*row = calc_row;
    जर (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    शून्य
write_to_term(buf_T *buffer, char_u *संदेश, channel_T *channel)
{
    माप_प्रकार	len = STRLEN(संदेश);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    जर (term->tl_out_fd != रिक्त)
    {
	ch_log(channel, "Writing %d bytes to output file", (पूर्णांक)len);
	ख_डालो(संदेश, len, 1, term->tl_out_fd);
	परत;
    }
#endif

    जर (term->tl_vterm == रिक्त)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (पूर्णांक)len);
	परत;
    }
    ch_log(channel, "writing %d bytes to terminal", (पूर्णांक)len);
    cursor_off();
    term_write_job_output(term, संदेश, len);

#ifdef FEAT_GUI
    जर (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    अन्यथा
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    जर (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	जर (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#जर defined(FEAT_TABPANEL)
	    जर (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    जर (buffer == curbuf && curbuf->b_term != रिक्त)
		update_cursor(curbuf->b_term, TRUE);
	}
	अन्यथा
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    स्थिर पूर्णांक
term_send_mouse(VTerm *vterm, पूर्णांक button, पूर्णांक pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    पूर्णांक		    row = mouse_row - W_WINROW(curwin);
    पूर्णांक		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    जर (popup_is_popup(curwin))
    {
	row -= popup_शीर्ष_extra(curwin);
	col -= popup_डावा_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    जर (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    परत TRUE;
}

स्थिर पूर्णांक enter_mouse_col = -1;
स्थिर पूर्णांक enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    स्थिर पूर्णांक
term_mouse_click(VTerm *vterm, पूर्णांक किल्ली)
{
#जर defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    स्थिर पूर्णांक	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    जर (state == रिक्त)
	परत FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    जर (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	निवड (किल्ली)
	{
	स्थिती K_LEFTDRAG:
	स्थिती K_LEFTRELEASE:
	स्थिती K_RIGHTDRAG:
	स्थिती K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		जर (ignore_drag_release)
		{
		    पूर्णांक save_mouse_col, save_mouse_row;

		    जर (enter_mouse_col < 0)
			थांबा;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_डावा, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	स्थिती K_LEFTMOUSE:
	स्थिती K_RIGHTMOUSE:
		जर (किल्ली == K_LEFTRELEASE || किल्ली == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		अन्यथा
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		जर (clip_star.available)
		{
		    पूर्णांक	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(किल्ली),
							 &is_click, &is_drag);
		    जर (mouse_model_popup() && button == MOUSE_डावा
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_उजवा;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		थांबा;

	स्थिती K_MIDDLEMOUSE:
		जर (clip_star.available)
		    घाला_reg('*', TRUE);
		थांबा;
	}
	enter_mouse_col = -1;
	परत FALSE;
    }
#endif
    enter_mouse_col = -1;

    निवड (किल्ली)
    {
	स्थिती K_LEFTMOUSE:
	स्थिती K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); थांबा;
	स्थिती K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); थांबा;
	स्थिती K_LEFTRELEASE:
	स्थिती K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); थांबा;
	स्थिती K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); थांबा;
	स्थिती K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); थांबा;
	स्थिती K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); थांबा;
	स्थिती K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); थांबा;
	स्थिती K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); थांबा;
	स्थिती K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); थांबा;
	स्थिती K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); थांबा;
    }
    परत TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    स्थिर पूर्णांक
term_convert_किल्ली(term_T *term, पूर्णांक c, पूर्णांक modmask, अक्षर *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_किल्ली	    किल्ली = VTERM_किल्ली_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    पूर्णांक		    other = FALSE;

    निवड (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	स्थिती K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    जर (term->tl_conpty)
		किल्ली = VTERM_किल्ली_BACKSPACE;
	    अन्यथा
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    थांबा;

#ifdef MSWIN
	स्थिती BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    जर (term->tl_conpty)
		किल्ली = VTERM_किल्ली_BACKSPACE;
	    थांबा;
#endif

	स्थिती ESC:		किल्ली = VTERM_किल्ली_ESCAPE; थांबा;
	स्थिती K_DEL:		किल्ली = VTERM_किल्ली_DEL; थांबा;
	स्थिती K_DOWN:		किल्ली = VTERM_किल्ली_DOWN; थांबा;
	स्थिती K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_DOWN; थांबा;
	स्थिती K_END:		किल्ली = VTERM_किल्ली_END; थांबा;
	स्थिती K_S_END:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_END; थांबा;
	स्थिती K_C_END:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_END; थांबा;
	स्थिती K_F10:		किल्ली = VTERM_किल्ली_FUNCTION(10); थांबा;
	स्थिती K_F11:		किल्ली = VTERM_किल्ली_FUNCTION(11); थांबा;
	स्थिती K_F12:		किल्ली = VTERM_किल्ली_FUNCTION(12); थांबा;
	स्थिती K_F1:		किल्ली = VTERM_किल्ली_FUNCTION(1); थांबा;
	स्थिती K_F2:		किल्ली = VTERM_किल्ली_FUNCTION(2); थांबा;
	स्थिती K_F3:		किल्ली = VTERM_किल्ली_FUNCTION(3); थांबा;
	स्थिती K_F4:		किल्ली = VTERM_किल्ली_FUNCTION(4); थांबा;
	स्थिती K_F5:		किल्ली = VTERM_किल्ली_FUNCTION(5); थांबा;
	स्थिती K_F6:		किल्ली = VTERM_किल्ली_FUNCTION(6); थांबा;
	स्थिती K_F7:		किल्ली = VTERM_किल्ली_FUNCTION(7); थांबा;
	स्थिती K_F8:		किल्ली = VTERM_किल्ली_FUNCTION(8); थांबा;
	स्थिती K_F9:		किल्ली = VTERM_किल्ली_FUNCTION(9); थांबा;
	स्थिती K_HOME:		किल्ली = VTERM_किल्ली_HOME; थांबा;
	स्थिती K_S_HOME:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_HOME; थांबा;
	स्थिती K_C_HOME:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_HOME; थांबा;
	स्थिती K_INS:		किल्ली = VTERM_किल्ली_INS; थांबा;
	स्थिती K_K0:		किल्ली = VTERM_किल्ली_KP_0; थांबा;
	स्थिती K_K1:		किल्ली = VTERM_किल्ली_KP_1; थांबा;
	स्थिती K_K2:		किल्ली = VTERM_किल्ली_KP_2; थांबा;
	स्थिती K_K3:		किल्ली = VTERM_किल्ली_KP_3; थांबा;
	स्थिती K_K4:		किल्ली = VTERM_किल्ली_KP_4; थांबा;
	स्थिती K_K5:		किल्ली = VTERM_किल्ली_KP_5; थांबा;
	स्थिती K_K6:		किल्ली = VTERM_किल्ली_KP_6; थांबा;
	स्थिती K_K7:		किल्ली = VTERM_किल्ली_KP_7; थांबा;
	स्थिती K_K8:		किल्ली = VTERM_किल्ली_KP_8; थांबा;
	स्थिती K_K9:		किल्ली = VTERM_किल्ली_KP_9; थांबा;
	स्थिती K_KDEL:		किल्ली = VTERM_किल्ली_DEL; थांबा; // TODO
	स्थिती K_KDIVIDE:		किल्ली = VTERM_किल्ली_KP_DIVIDE; थांबा;
	स्थिती K_KEND:		किल्ली = VTERM_किल्ली_KP_1; थांबा; // TODO
	स्थिती K_KENTER:		किल्ली = VTERM_किल्ली_KP_ENTER; थांबा;
	स्थिती K_KHOME:		किल्ली = VTERM_किल्ली_KP_7; थांबा; // TODO
	स्थिती K_KINS:		किल्ली = VTERM_किल्ली_KP_0; थांबा; // TODO
	स्थिती K_KMINUS:		किल्ली = VTERM_किल्ली_KP_MINUS; थांबा;
	स्थिती K_KMULTIPLY:	किल्ली = VTERM_किल्ली_KP_MULT; थांबा;
	स्थिती K_KPAGEDOWN:	किल्ली = VTERM_किल्ली_KP_3; थांबा; // TODO
	स्थिती K_KPAGEUP:		किल्ली = VTERM_किल्ली_KP_9; थांबा; // TODO
	स्थिती K_KPLUS:		किल्ली = VTERM_किल्ली_KP_PLUS; थांबा;
	स्थिती K_KPOINT:		किल्ली = VTERM_किल्ली_KP_PERIOD; थांबा;
	स्थिती K_डावा:		किल्ली = VTERM_किल्ली_डावा; थांबा;
	स्थिती K_S_डावा:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_डावा; थांबा;
	स्थिती K_C_डावा:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_डावा; थांबा;
	स्थिती K_PAGEDOWN:	किल्ली = VTERM_किल्ली_PAGEDOWN; थांबा;
	स्थिती K_PAGEUP:		किल्ली = VTERM_किल्ली_PAGEUP; थांबा;
	स्थिती K_उजवा:		किल्ली = VTERM_किल्ली_उजवा; थांबा;
	स्थिती K_S_उजवा:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_उजवा; थांबा;
	स्थिती K_C_उजवा:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_उजवा; थांबा;
	स्थिती K_UP:		किल्ली = VTERM_किल्ली_UP; थांबा;
	स्थिती K_S_UP:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_UP; थांबा;
	स्थिती TAB:		किल्ली = VTERM_किल्ली_TAB; थांबा;
	स्थिती K_S_TAB:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_TAB; थांबा;

	स्थिती K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); थांबा;
	स्थिती K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); थांबा;
	स्थिती K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); थांबा;
	स्थिती K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); थांबा;

	स्थिती K_LEFTMOUSE:
	स्थिती K_LEFTMOUSE_NM:
	स्थिती K_LEFTDRAG:
	स्थिती K_LEFTRELEASE:
	स्थिती K_LEFTRELEASE_NM:
	स्थिती K_MOUSEMOVE:
	स्थिती K_MIDDLEMOUSE:
	स्थिती K_MIDDLEDRAG:
	स्थिती K_MIDDLERELEASE:
	स्थिती K_RIGHTMOUSE:
	स्थिती K_RIGHTDRAG:
	स्थिती K_RIGHTRELEASE:	जर (!term_mouse_click(vterm, c))
				    परत 0;
				other = TRUE;
				थांबा;

	स्थिती K_X1MOUSE:		/* TODO */ परत 0;
	स्थिती K_X1DRAG:		/* TODO */ परत 0;
	स्थिती K_X1RELEASE:	/* TODO */ परत 0;
	स्थिती K_X2MOUSE:		/* TODO */ परत 0;
	स्थिती K_X2DRAG:		/* TODO */ परत 0;
	स्थिती K_X2RELEASE:	/* TODO */ परत 0;

	स्थिती K_IGNORE:		परत 0;
	स्थिती K_NOP:		परत 0;
	स्थिती K_UNDO:		परत 0;
	स्थिती K_HELP:		परत 0;
	स्थिती K_XF1:		किल्ली = VTERM_किल्ली_FUNCTION(1); थांबा;
	स्थिती K_XF2:		किल्ली = VTERM_किल्ली_FUNCTION(2); थांबा;
	स्थिती K_XF3:		किल्ली = VTERM_किल्ली_FUNCTION(3); थांबा;
	स्थिती K_XF4:		किल्ली = VTERM_किल्ली_FUNCTION(4); थांबा;
	स्थिती K_SELECT:		परत 0;
#ifdef FEAT_GUI
	स्थिती K_VER_SCROLLBAR:	परत 0;
	स्थिती K_HOR_SCROLLBAR:	परत 0;
#endif
#ifdef FEAT_GUI_TABLINE
	स्थिती K_TABLINE:		परत 0;
	स्थिती K_TABMENU:		परत 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	स्थिती K_F21:		किल्ली = VTERM_किल्ली_FUNCTION(21); थांबा;
#endif
#ifdef FEAT_DND
	स्थिती K_DROP:		परत 0;
#endif
	स्थिती K_CURSORHOLD:	परत 0;
	स्थिती K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				थांबा;
	स्थिती K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				थांबा;
    }

    // add modifiers for the typed key
    जर (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    जर (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    जर (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    जर (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    जर (किल्ली != VTERM_किल्ली_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_किल्ली(vterm, किल्ली, mod);
    अन्यथा जर (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    परत (पूर्णांक)vterm_output_read(vterm, buf, किल्ली_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    स्थिर पूर्णांक
term_job_running_check(term_T *term, पूर्णांक check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    जर (term == रिक्त
	|| term->tl_job == रिक्त
	|| !channel_is_open(term->tl_job->jv_channel))
	परत FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    जर (check_job_status)
	job_status(job);
    परत (job->jv_status == JOB_STARTED
	    || (job->jv_channel != रिक्त && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    पूर्णांक
term_job_running(term_T *term)
{
    परत term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    पूर्णांक
term_job_running_not_none(term_T *term)
{
    परत term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    पूर्णांक
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    परत term != रिक्त
	&& term->tl_job != रिक्त
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    पूर्णांक
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_संदेश_SIZE];
    पूर्णांक	ret;

    dialog_संदेश(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 1);
    जर (ret == VIM_YES)
	परत OK;
    अन्यथा
	परत FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    पूर्णांक
term_try_stop_job(buf_T *buf)
{
    पूर्णांक	    मोजणी;
    अक्षर    *how = (अक्षर *)buf->b_term->tl_kill;

#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    जर ((how == रिक्त || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	जर (term_confirm_stop(buf) == OK)
	    how = "kill";
	अन्यथा
	    परत FAIL;
    }
#endif
    जर (how == रिक्त || *how == NUL)
	परत FAIL;

    job_stop(buf->b_term->tl_job, रिक्त, how);

    // wait for up to a second for the job to die
    साठी (मोजणी = 0; मोजणी < 100; ++मोजणी)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	जर (!buf_valid(buf)
		|| buf->b_term == रिक्त
		|| buf->b_term->tl_job == रिक्त)
	    परत OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	जर (job->jv_status >= JOB_ENDED)
	    परत OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    परत FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    स्थिर शून्य
add_scrollback_ओळ_to_buffer(term_T *term, char_u *text, पूर्णांक len, पूर्णांक append)
{
    buf_T	*buf = term->tl_buffer;
    पूर्णांक		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_ओळ_मोजणी;

#ifdef MSWIN
    char_u *tmp = text;
    जर (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = रिक्त;
	पूर्णांक	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (अक्षर*)text, len + 1,
							   &ret, &length);
	जर (ret != रिक्त)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (अक्षर **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    जर (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	माप_प्रकार prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	जर (both == रिक्त)
	    परत;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    अन्यथा
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    जर (tmp != text)
	vim_free(text);
#endif
    जर (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    स्थिर शून्य
cell2cellattr(स्थिरांक VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    स्थिर पूर्णांक
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    परत a->fg.red == b->fg.red
	&& a->fg.green == b->fg.green
	&& a->fg.blue == b->fg.blue
	&& a->bg.red == b->bg.red
	&& a->bg.green == b->bg.green
	&& a->bg.blue == b->bg.blue;
}

/*
 * Add an empty scrollback line to "term".  When "lnum" is not zero, add the
 * line at this position.  Otherwise at the end.
 */
    स्थिर पूर्णांक
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, पूर्णांक lnum)
{
    जर (ga_grow(&term->tl_scrollback, 1) == FAIL)
	परत FALSE;

    sb_ओळ_T *ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    जर (lnum > 0)
    {
	पूर्णांक i;

	साठी (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *ओळ = *(ओळ - 1);
	    --ओळ;
	}
    }
    ओळ->sb_cols = 0;
    ओळ->sb_bytes = 0;
    ओळ->sb_cells = रिक्त;
    ओळ->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    परत OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    स्थिर शून्य
cleanup_scrollback(term_T *term)
{
    sb_ओळ_T	*ओळ;
    garray_T	*gap;
    char_u	*bufline;
    माप_प्रकार      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ओळ_मोजणी, FALSE);
    bufline_length = STRLEN(bufline);
    पर्यंत (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	ओळ = (sb_ओळ_T *)gap->ga_data + gap->ga_len - 1;
	जर (ओळ->sb_bytes < 0 || (माप_प्रकार)ओळ->sb_bytes > bufline_length)
	    थांबा;
	bufline_length -= ओळ->sb_bytes;
	जर (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_ओळ_मोजणी);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ओळ_मोजणी, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(ओळ->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    जर (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_ओळ_मोजणी, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    जर (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    स्थिर शून्य
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    पूर्णांक		    len;
    पूर्णांक		    ओळी_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == रिक्त ? रिक्त : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    साठी (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	साठी (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    जर (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    अन्यथा
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	जर (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++ओळी_skipped;
	अन्यथा
	{
	    पर्यंत (ओळी_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--ओळी_skipped;
		जर (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_ओळ_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    जर (len == 0)
		p = रिक्त;
	    अन्यथा
		p = ALLOC_MULT(cellattr_T, len);
	    जर ((p != रिक्त || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		पूर्णांक	    width;
		स्थिरांक VTerm_ओळ_Info *lineinfo;
		sb_ओळ_T   *ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		साठी (pos.col = 0; pos.col < len; pos.col += width)
		{
		    जर (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			जर (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    अन्यथा
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			जर (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			जर (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    पूर्णांक	    i;
			    पूर्णांक	    c;

			    साठी (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		ओळ->sb_cols = len;
		ओळ->sb_bytes = ga.ga_len;
		ओळ->sb_cells = p;
		ओळ->sb_fill_attr = new_fill_attr;
		ओळ->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		जर (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_ओळ_to_buffer(term, (char_u *)"", 0, 0);
		अन्यथा
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_ओळ_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    अन्यथा
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    साठी (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	जर (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_ओळ_to_buffer(term, (char_u *)"", 0, 0);
	    ++term->tl_scrollback_snapshot;
	}
    }

    term->tl_dirty_snapshot = FALSE;
#ifdef FEAT_TIMERS
    term->tl_timer_set = FALSE;
#endif
}

/*
 * Loop over all windows in the current tab, and also curwin, which is not
 * encountered when using a terminal in a popup window.
 * Return TRUE if "*wp" was set to the next window.
 */
    स्थिर पूर्णांक
for_all_windows_and_curwin(win_T **wp, पूर्णांक *did_curwin)
{
    जर (*wp == रिक्त)
	*wp = firstwin;
    अन्यथा जर ((*wp)->w_next != रिक्त)
	*wp = (*wp)->w_next;
    अन्यथा जर (!*did_curwin)
	*wp = curwin;
    अन्यथा
	परत FALSE;
    जर (*wp == curwin)
	*did_curwin = TRUE;
    परत TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    शून्य
may_move_terminal_to_buffer(term_T *term, पूर्णांक redraw)
{
    जर (term->tl_vterm == रिक्त)
	परत;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    जर (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_ओळ_मोजणी
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    जर (redraw)
    {
	win_T	    *wp = रिक्त;
	पूर्णांक	    did_curwin = FALSE;

	पर्यंत (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    जर (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_ओळ_मोजणी;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		जर (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    जर (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#जर defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    पूर्णांक
term_check_timers(पूर्णांक next_due_arg, proftime_T *now)
{
    term_T  *term;
    पूर्णांक	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	जर (term->tl_timer_set && !term->tl_normal_mode)
	{
	    दीर्घ    this_due = proftime_time_डावा(&term->tl_timer_due, now);

	    जर (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    अन्यथा जर (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    परत next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    स्थिर शून्य
set_terminal_mode(term_T *term, पूर्णांक normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    जर (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    जर (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    स्थिर शून्य
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    जर (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    स्थिर शून्य
term_enter_normal_mode(शून्य)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    पूर्णांक        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, रिक्त);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    जर (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    पूर्णांक
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    परत term != रिक्त && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    शून्य
term_enter_job_mode(शून्य)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    जर (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    जर (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
प्रकारव्याख्या गणना {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_किल्ली_state_T;

स्थिर reduce_किल्ली_state_T  no_reduce_किल्ली_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    स्थिर पूर्णांक
vterm_using_किल्ली_protocol(शून्य)
{
    परत curbuf->b_term != रिक्त
	&& curbuf->b_term->tl_vterm != रिक्त
	&& (vterm_is_modify_other_किल्ल्या(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    शून्य
check_no_reduce_किल्ल्या(शून्य)
{
    जर (no_reduce_किल्ली_state != NRKS_CHECK
	    || no_reduce_किल्ल्या >= 1
	    || curbuf->b_term == रिक्त
	    || curbuf->b_term->tl_vterm == रिक्त)
	परत;

    जर (vterm_using_किल्ली_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_किल्ली_state = NRKS_SET;
	++no_reduce_किल्ल्या;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    स्थिर पूर्णांक
term_vgetc(शून्य)
{
    पूर्णांक c;
    पूर्णांक save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    जर (vterm_using_किल्ली_protocol())
    {
	++no_reduce_किल्ल्या;
	no_reduce_किल्ली_state = NRKS_SET;
    }
    अन्यथा
    {
	no_reduce_किल्ली_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    जर (no_reduce_किल्ली_state == NRKS_SET)
	--no_reduce_किल्ल्या;
    no_reduce_किल्ली_state = NRKS_NONE;

    परत c;
}

स्थिर पूर्णांक	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    पूर्णांक
send_किल्ल्या_to_term(term_T *term, पूर्णांक c, पूर्णांक modmask, पूर्णांक typed)
{
    अक्षर	संदेश[किल्ली_BUF_LEN];
    माप_प्रकार	len;
    पूर्णांक		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    निवड (c)
    {
	स्थिती NUL:
	स्थिती K_ZERO:
	    जर (typed)
		stuffcharReadbuff(c);
	    परत FAIL;

	स्थिती K_TABLINE:
	    stuffcharReadbuff(c);
	    परत FAIL;

	स्थिती K_IGNORE:
	स्थिती K_CANCEL:  // used for :normal when running out of chars
	    परत FAIL;

	स्थिती K_LEFTDRAG:
	स्थिती K_MIDDLEDRAG:
	स्थिती K_RIGHTDRAG:
	स्थिती K_X1DRAG:
	स्थिती K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	स्थिती K_LEFTMOUSE:
	स्थिती K_LEFTMOUSE_NM:
	स्थिती K_LEFTRELEASE:
	स्थिती K_LEFTRELEASE_NM:
	स्थिती K_MOUSEMOVE:
	स्थिती K_MIDDLEMOUSE:
	स्थिती K_MIDDLERELEASE:
	स्थिती K_RIGHTMOUSE:
	स्थिती K_RIGHTRELEASE:
	स्थिती K_X1MOUSE:
	स्थिती K_X1RELEASE:
	स्थिती K_X2MOUSE:
	स्थिती K_X2RELEASE:

	स्थिती K_MOUSEUP:
	स्थिती K_MOUSEDOWN:
	स्थिती K_MOUSELEFT:
	स्थिती K_MOUSERIGHT:
	    {
		पूर्णांक	row = mouse_row;
		पूर्णांक	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		जर (popup_is_popup(curwin))
		{
		    row -= popup_शीर्ष_extra(curwin);
		    col -= popup_डावा_extra(curwin);
		}
#endif
		जर (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    जर (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    परत FAIL;
		}
	    }
	    थांबा;

	स्थिती K_COMMAND:
	स्थिती K_SCRIPT_COMMAND:
	    परत do_cmdkey_command(c, 0);
    }
    जर (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_किल्ली(term, c, modmask, संदेश);
    जर (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)संदेश, (पूर्णांक)len, रिक्त);

    परत OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    स्थिर शून्य
term_paste_register(पूर्णांक prev_c UNUSED)
{
    पूर्णांक		c;
    list_T	*l;
    listitem_T	*item;
    दीर्घ	reglen = 0;
    पूर्णांक		type;

    जर (add_to_showcmd(prev_c))
    जर (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    जर (!term_use_loop())
	// job finished while waiting for a character
	परत;

    // CTRL-W "= prompt for expression to evaluate.
    जर (c == '=' && get_अभिव्यक्ती_register() != '=')
	परत;
    जर (!term_use_loop())
	// job finished while waiting for a character
	परत;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    जर (l == रिक्त)
	परत;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_वस्तुसंच(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	जर (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = रिक्त;
	    पूर्णांक	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (अक्षर *)s,
		    (पूर्णांक)STRLEN(s), &ret, &length);
	    जर (ret != रिक्त)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (अक्षर **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (पूर्णांक)STRLEN(s), रिक्त);
#ifdef MSWIN
	जर (tmp != s)
	    vim_free(s);
#endif

	जर (item->li_next != रिक्त || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, रिक्त);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    पूर्णांक
terminal_is_active(शून्य)
{
    परत in_terminal_loop != रिक्त;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    स्थिर पूर्णांक
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    जर (wp != रिक्त && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    अन्यथा जर (term->tl_highlight_name != रिक्त)
	name = term->tl_highlight_name;
    अन्यथा
	name = (char_u*)"Terminal";

    परत syn_name2id(name);
}

#जर defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    स्थिर cursorentry_T entry;
    पूर्णांक			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_डावा ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    जर (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    जर (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    जर (term_bg != INVALCOLOR)
	*fg = term_bg;
    अन्यथा
	*fg = gui.back_pixel;

    जर (term->tl_cursor_color == रिक्त)
    {
	जर (term_fg != INVALCOLOR)
	    *bg = term_fg;
	अन्यथा
	    *bg = gui.norm_pixel;
    }
    अन्यथा
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    परत &entry;
}
#endif

    स्थिर शून्य
may_output_cursor_props(शून्य)
{
    जर (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	जर (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	अन्यथा
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    स्थिर शून्य
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    जर (gui.in_use)
	परत;
#endif
    जर (in_terminal_loop == term)
    {
	cursor_color_copy(&desired_cursor_color, term->tl_cursor_color);
	desired_cursor_shape = term->tl_cursor_shape;
	desired_cursor_blink = term->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    स्थिर शून्य
prepare_restore_cursor_props(शून्य)
{
#ifdef FEAT_GUI
    जर (gui.in_use)
	परत;
#endif
    cursor_color_copy(&desired_cursor_color, रिक्त);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    स्थिर पूर्णांक
term_use_loop_check(पूर्णांक check_job_status)
{
    term_T *term = curbuf->b_term;

    परत term != रिक्त
	&& !term->tl_normal_mode
	&& term->tl_vterm != रिक्त
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    पूर्णांक
term_use_loop(शून्य)
{
    परत term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    शून्य
term_win_entered(शून्य)
{
    term_T *term = curbuf->b_term;

    जर (term == रिक्त)
	परत;

    जर (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	जर (State & MODE_घाला)
	    stop_घाला_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    शून्य
term_focus_change(पूर्णांक in_focus)
{
    term_T *term = curbuf->b_term;

    जर (term == रिक्त || term->tl_vterm == रिक्त)
	परत;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    जर (in_focus)
	vterm_state_focus_in(state);
    अन्यथा
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    स्थिर पूर्णांक
raw_c_to_ctrl(पूर्णांक c)
{
    जर ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	परत c & 0x1f;
    परत c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    स्थिर पूर्णांक
ctrl_to_raw_c(पूर्णांक c)
{
    जर (c < 0x20 && vterm_using_किल्ली_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	परत c + '@';
    }
    परत c;
}

/*
 * Wait for input and send it to the job.
 * When "blocking" is TRUE wait for a character to be typed.  Otherwise return
 * when there is no more typahead.
 * Return when the start of a CTRL-W command is typed or anything else that
 * should be handled as a Normal mode command.
 * Returns OK if a typed character is to be handled in Normal mode, FAIL if
 * the terminal was closed.
 */
    पूर्णांक
terminal_loop(पूर्णांक blocking)
{
    पूर्णांक		c;
    पूर्णांक		raw_c;
    पूर्णांक		termwinkey = 0;
    पूर्णांक		ret;
#ifdef UNIX
    पूर्णांक		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    पूर्णांक		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    जर (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_किल्ली(curwin->w_p_twk, TRUE);
	जर (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    पर्यंत (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	जर (curbuf->b_term != रिक्त && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    पर्यंत (must_redraw != 0)
		जर (update_screen(0) == FAIL)
		    थांबा;
	जर (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    थांबा;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	जर (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    जर (raw_c != K_IGNORE)
		vungetc(raw_c);
	    थांबा;
	}
	जर (raw_c == K_IGNORE)
	    सुरूठेवा;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	जर (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    जर (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	जर (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	जर ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    पूर्णांक	    prev_c = c;
	    पूर्णांक	    prev_raw_c = raw_c;
	    पूर्णांक	    prev_mod_mask = mod_mask;

	    जर (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    जर (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		थांबा;

	    जर (prev_c == Ctrl_BSL)
	    {
		जर (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    जा theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_किल्ल्या_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    अन्यथा जर (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    अन्यथा जर (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    अन्यथा जर (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    अन्यथा जर (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		जा theend;
	    }
	    अन्यथा जर (c == '"')
	    {
		term_paste_register(prev_c);
		सुरूठेवा;
	    }
	    अन्यथा जर (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		जा theend;
	    }
	}
#ifdef MSWIN
	जर (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (अचिन्हित)raw_c >> 8;
	    mb[1] = raw_c;
	    जर (MultiByteToWideChar(GetACP(), 0, (अक्षर*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	जर (send_किल्ल्या_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    जर (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    जा theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = रिक्त;
    जर (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    जर (curbuf->b_term != रिक्त && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    परत ret;
}

    स्थिर शून्य
may_toggle_cursor(term_T *term)
{
    जर (in_terminal_loop != term)
	परत;

    जर (term->tl_cursor_visible)
	cursor_on();
    अन्यथा
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    स्थिर पूर्णांक
color2index(VTermColor *color, पूर्णांक fg, पूर्णांक *boldp)
{
    पूर्णांक red = color->red;
    पूर्णांक blue = color->blue;
    पूर्णांक green = color->green;

    *boldp = FALSE;

    जर (VTERM_COLOR_IS_INVALID(color))
	परत 0;

    जर (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	जर (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    जर (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    परत index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	अन्यथा जर (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    परत (color->index & 7) + 1;
	}
	परत 0;
    }

    जर (t_colors >= 256)
    {
	जर (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    स्थिर पूर्णांक cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    पूर्णांक i;

	    जर (red < 5)
		परत 17; // 00/00/00
	    जर (red > 245) // ff/ff/ff
		परत 232;
	    साठी (i = 0; i < 23; ++i)
		जर (red < cutoff[i])
		    परत i + 233;
	    परत 256;
	}
	{
	    स्थिर पूर्णांक cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    पूर्णांक ri, gi, bi;

	    // 216-color cube
	    साठी (ri = 0; ri < 5; ++ri)
		जर (red < cutoff[ri])
		    थांबा;
	    साठी (gi = 0; gi < 5; ++gi)
		जर (green < cutoff[gi])
		    थांबा;
	    साठी (bi = 0; bi < 5; ++bi)
		जर (blue < cutoff[bi])
		    थांबा;
	    परत 17 + ri * 36 + gi * 6 + bi;
	}
    }
    परत 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    स्थिर पूर्णांक
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    पूर्णांक attr = 0;

    जर (cellattrs->bold)
	attr |= HL_BOLD;
    जर (cellattrs->underline)
	attr |= HL_UNDERLINE;
    जर (cellattrs->italic)
	attr |= HL_ITALIC;
    जर (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    जर (cellattrs->reverse)
	attr |= HL_INVERSE;
    परत attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    स्थिर शून्य
hl2vtermAttr(पूर्णांक attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    जर (attr & HL_BOLD)
	cell->attrs.bold = 1;
    जर (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    जर (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    जर (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    जर (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    स्थिर पूर्णांक
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    पूर्णांक attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    पूर्णांक is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    पूर्णांक is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    जर (is_default_fg || is_default_bg)
    {
	जर (wp != रिक्त && wp->w_hlfwin_id != 0)
	{
	    जर (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    जर (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	अन्यथा
	{
	    जर (is_default_fg)
		fg = &term->tl_default_color.fg;
	    जर (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	परत get_gui_attr_idx(attr, guifg, guibg);
    }
    अन्यथा
#endif
#ifdef FEAT_TERMGUICOLORS
    जर (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	परत get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    अन्यथा
#endif
    {
	पूर्णांक bold = MAYBE;
	पूर्णांक ctermfg = color2index(fg, TRUE, &bold);
	पूर्णांक ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	जर (bold == TRUE)
	    attr |= HL_BOLD;

	परत get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    परत 0;
}

    स्थिर शून्य
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    जर (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    स्थिर पूर्णांक
handle_damage(VTermRect rect, शून्य *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    परत 1;
}

    स्थिर शून्य
term_scroll_up(term_T *term, पूर्णांक start_row, पूर्णांक मोजणी)
{
    win_T		 *wp = रिक्त;
    पूर्णांक			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    पूर्णांक			 clear_attr;

    CLEAR_FIELD(attr);

    पर्यंत (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	जर (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_ओळी(wp, start_row, मोजणी, FALSE, FALSE, clear_attr);
	}
    }
}

    स्थिर पूर्णांक
handle_moverect(VTermRect dest, VTermRect src, शून्य *user)
{
    term_T	*term = (term_T *)user;
    पूर्णांक		मोजणी = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    जर (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	जर (dest.start_row == 0)
	    term->tl_postponed_scroll += मोजणी;
	अन्यथा
	    term_scroll_up(term, dest.start_row, मोजणी);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    परत 1;
}

    स्थिर पूर्णांक
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	पूर्णांक visible,
	शून्य *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = रिक्त;
    पूर्णांक		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    पर्यंत (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	जर (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    जर (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    परत 1;
}

    स्थिर पूर्णांक
handle_settermprop(
	VTermProp prop,
	VTerm_मूल्य *मूल्य,
	शून्य *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = रिक्त;

    निवड (prop)
    {
	स्थिती VTERM_PROP_TITLE:
	    जर (disable_vterm_title_for_testing)
		थांबा;
	    strval = vim_strnsave((char_u *)मूल्य->string.str,
							    मूल्य->string.len);
	    जर (strval == रिक्त)
		थांबा;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    जर (*skipwhite(strval) == NUL)
		term->tl_title = रिक्त;
	    // Same as blank
	    अन्यथा जर (term->tl_arg0_cmd != रिक्त
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (पूर्णांक)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = रिक्त;
	    // Empty corrupted data of winpty
	    अन्यथा जर (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = रिक्त;
#ifdef MSWIN
	    अन्यथा जर (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = रिक्त;
		पूर्णांक	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(अक्षर*)मूल्य->string.str,
					(पूर्णांक)मूल्य->string.len, &ret, &length);
		जर (ret != रिक्त)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (अक्षर**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    अन्यथा
	    {
		term->tl_title = strval;
		strval = रिक्त;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    जर (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    थांबा;

	स्थिती VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = मूल्य->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    थांबा;

	स्थिती VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = मूल्य->boolean;
	    may_set_cursor_props(term);
	    थांबा;

	स्थिती VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = मूल्य->संख्या;
	    may_set_cursor_props(term);
	    थांबा;

	स्थिती VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)मूल्य->string.str,
							    मूल्य->string.len);
	    जर (strval == रिक्त)
		थांबा;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    थांबा;

	स्थिती VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = मूल्य->boolean;
	    थांबा;

	पूर्वनिर्धारित:
	    थांबा;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    परत 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    स्थिर पूर्णांक
handle_resize(पूर्णांक rows, पूर्णांक cols, शून्य *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    जर (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    अन्यथा
    {
	FOR_ALL_WINDOWS(wp)
	{
	    जर (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    परत 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    स्थिर शून्य
limit_scrollback(term_T *term, garray_T *gap, पूर्णांक update_buffer)
{
    जर (gap->ga_len < term->tl_buffer->b_p_twsl)
	परत;

    पूर्णांक	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    पूर्णांक	i;
    sb_ओळ_T *sb_ओळी = (sb_ओळ_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    साठी (i = 0; i < todo; ++i)
    {
	जर (update_buffer && (!sb_ओळी[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_ओळी[i].sb_cells);
    }
    // Continue until end of wrapped line
    साठी (; todo < gap->ga_len && sb_ओळी[todo].continuation; ++todo)
	vim_free(sb_ओळी[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_ओळ_T *)gap->ga_data + todo,
		आकार(sb_ओळ_T) * gap->ga_len);
    जर (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = रिक्त;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    जर (wp->w_buffer == term->tl_buffer)
	    {
		curwin = wp;
		check_cursor();
		update_topline();
	    }
	}
	curwin = curwin_save;
    }
}

/*
 * Handle a line that is pushed off the top of the screen.
 */
    स्थिर पूर्णांक
handle_pushline(पूर्णांक cols, स्थिरांक VTermScreenCell *cells, पूर्णांक continuation, शून्य *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    पूर्णांक		update_buffer;

    जर (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    अन्यथा
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    जर (ga_grow(gap, 1) == FAIL)
	परत 0;

    cellattr_T	*p = रिक्त;
    पूर्णांक		len = 0;
    पूर्णांक		i;
    पूर्णांक		c;
    पूर्णांक		col;
    पूर्णांक		text_len;
    char_u		*text;
    sb_ओळ_T	*ओळ;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    साठी (i = 0; i < cols; ++i)
	जर (cells[i].chars[0] != 0)
	    len = i + 1;
	अन्यथा
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    जर (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    जर (p != रिक्त)
    {
	साठी (col = 0; col < len; col += cells[col].width)
	{
	    जर (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		थांबा;
	    }
	    साठी (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    जर (ga_grow(&ga, 1) == FAIL)
    {
	जर (update_buffer)
	    text = (char_u *)"";
	अन्यथा
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    अन्यथा
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    जर (update_buffer)
	add_scrollback_ओळ_to_buffer(term, text, text_len, continuation);

    ओळ = (sb_ओळ_T *)gap->ga_data + gap->ga_len;
    ओळ->sb_cols = len;
    ओळ->sb_bytes = text_len;
    ओळ->sb_cells = p;
    ओळ->sb_fill_attr = fill_attr;
    ओळ->continuation = (char_u)continuation;
    जर (update_buffer)
    {
	ओळ->sb_text = रिक्त;
	++term->tl_scrollback_scrolled;
	जर (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    अन्यथा
    {
	ओळ->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    परत 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    स्थिर शून्य
handle_postponed_scrollback(term_T *term)
{
    पूर्णांक i;

    जर (term->tl_scrollback_postponed.ga_len == 0)
	परत;
    ch_log(रिक्त, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    साठी (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_ओळ_T	*pp_ओळ;
	sb_ओळ_T	*ओळ;

	जर (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    थांबा;
	pp_ओळ = (sb_ओळ_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_ओळ->sb_text;
	जर (text == रिक्त)
	    text = (char_u *)"";
	add_scrollback_ओळ_to_buffer(term, text, (पूर्णांक)STRLEN(text), pp_ओळ->continuation);
	vim_free(pp_ओळ->sb_text);

	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	ओळ->sb_cols = pp_ओळ->sb_cols;
	ओळ->sb_bytes = pp_ओळ->sb_bytes;
	ओळ->sb_cells = pp_ओळ->sb_cells;
	ओळ->sb_fill_attr = pp_ओळ->sb_fill_attr;
	ओळ->sb_text = रिक्त;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	जर (!pp_ओळ->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    स्थिर पूर्णांक
handle_bell(शून्य *user UNUSED)
{
    vim_beep(BO_TERM);
    परत 0;
}

स्थिर VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    रिक्त,			// sb_pushline
    रिक्त,			// sb_popline
    रिक्त,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    स्थिर पूर्णांक
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    जर (!term->tl_normal_mode)
    {
	पूर्णांक	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	जर (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    पूर्णांक		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = रिक्त;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    जर (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		जर (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    अन्यथा
#endif
	    // If this is the last normal window: exit Vim.
	    जर (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		परत TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(रिक्त, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    जर (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		जर (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		जर (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		जर (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    जर (pwin != रिक्त)
		popup_close_with_retval(pwin, 0);
#endif
	    परत TRUE;
	}
	जर (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    अक्षर    *cmd = term->tl_opencmd == रिक्त
				? "botright sbuf %d"
				: (अक्षर *)term->tl_opencmd;
	    माप_प्रकार  len = म_माप(cmd) + 50;
	    अक्षर    *buf = alloc(len);

	    जर (buf != रिक्त)
	    {
		ch_log(रिक्त, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	अन्यथा
	    ch_log(रिक्त, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    परत FALSE;
}

#जर defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    पूर्णांक
may_close_term_popup(शून्य)
{
    जर (!popup_is_popup(curwin) || curbuf->b_term == रिक्त
				 || term_job_running_not_none(curbuf->b_term))
	परत FAIL;

    win_T *pwin = curwin;

    जर (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    परत OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    शून्य
term_channel_closing(channel_T *ch)
{
    term_T *term;

    साठी (term = first_term; term != रिक्त; term = term->tl_next)
	जर (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    शून्य
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    पूर्णांक	    did_one = FALSE;

    साठी (term = first_term; term != रिक्त; term = next_term)
    {
	next_term = term->tl_next;
	जर (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    जर (term->tl_out_fd != रिक्त)
	    {
		ख_बंद(term->tl_out_fd);
		term->tl_out_fd = रिक्त;
	    }
#endif

	    जर (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		सुरूठेवा;
	    }

	    जर (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    जर (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	जर (term != रिक्त)
	{
	    जर (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    शून्य
term_check_channel_closed_recently(शून्य)
{
    term_T *term;
    term_T *next_term;

    साठी (term = first_term; term != रिक्त; term = next_term)
    {
	next_term = term->tl_next;
	जर (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    जर (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    स्थिर शून्य
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	पूर्णांक		max_col)
{
    पूर्णांक off = screen_get_current_ओळ_off();

    साठी (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	पूर्णांक		c;

	जर (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	जर (c == NUL)
	{
	    Screen_ओळी[off] = ' ';
	    जर (enc_utf8)
		Screen_ओळी_UC[off] = NUL;
	}
	अन्यथा
	{
	    जर (enc_utf8)
	    {
		पूर्णांक i;

		// composing chars
		साठी (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_ओळी_C[i][off] = cell.chars[i + 1];
		    जर (cell.chars[i + 1] == 0)
			थांबा;
		}
		जर (c >= 0x80 || (Screen_mco > 0
					 && Screen_ओळी_C[0][off] != 0))
		{
		    Screen_ओळी[off] = ' ';
		    Screen_ओळी_UC[off] = c;
		}
		अन्यथा
		{
		    Screen_ओळी[off] = c;
		    Screen_ओळी_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    अन्यथा जर (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		जर (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (अक्षर*)mb, 2, 0, 0) > 1)
		{
		    Screen_ओळी[off] = mb[0];
		    Screen_ओळी[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		अन्यथा
		    Screen_ओळी[off] = c;
	    }
#endif
	    अन्यथा
		// This will only store the lower byte of "c".
		Screen_ओळी[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	जर (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    जर (enc_utf8)
	    {
		Screen_ओळी_UC[off] = NUL;
		Screen_ओळी[off] = NUL;
	    }
	    अन्यथा जर (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_ओळी[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#जर defined(FEAT_GUI)
    स्थिर शून्य
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    जर (term->tl_vterm == रिक्त)
	परत;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    पर्यंत (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	पूर्णांक save_p_more = p_more;

	p_more = FALSE;
	संदेश_row = Rows - 1;
	संदेश_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    साठी (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	जर (pos.row < term->tl_rows)
	{
	    पूर्णांक max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, रिक्त, screen, &pos, max_col);
	}
	अन्यथा
	    pos.col = 0;

	screen_ओळ(curwin, term->tl_toprow + pos.row,
		firstwin->w_wincol, pos.col, topframe->fr_width, -1, 0);
    }

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}
#endif

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    पूर्णांक
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    परत term != रिक्त && term->tl_vterm != रिक्त && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    शून्य
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    पूर्णांक		rows, cols;
    पूर्णांक		newrows, newcols;
    पूर्णांक		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    जर (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	जर (term->tl_postponed_scroll > 0
			      && term->tl_postponed_scroll < term->tl_rows / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    term_scroll_up(term, 0, term->tl_postponed_scroll);
	term->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &rows, &cols);

    newrows = 99999;
    newcols = 99999;
    साठी (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == रिक्त ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	जर (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	जर (twp == रिक्त)
	    थांबा;
    }
    जर (newrows == 99999 || newcols == 99999)
	परत; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    जर (newrows == 0 || newcols == 0)
	परत;

    जर (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	जर (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    साठी (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	जर (pos.row < term->tl_rows)
	{
	    पूर्णांक max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	अन्यथा
	    pos.col = 0;

	screen_ओळ(wp, wp->w_winrow + pos.row
#ifdef FEAT_MENU
				+ winbar_height(wp)
#endif
				, wp->w_wincol, pos.col, wp->w_width, -1,
#ifdef FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#endif
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    शून्य
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    जर (term == रिक्त || term->tl_vterm == रिक्त || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	परत;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    पूर्णांक
term_is_finished(buf_T *buf)
{
    परत buf->b_term != रिक्त && buf->b_term->tl_vterm == रिक्त;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    पूर्णांक
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    परत term != रिक्त && (term->tl_vterm == रिक्त || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    शून्य
term_change_in_curbuf(शून्य)
{
    term_T *term = curbuf->b_term;

    जर (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	परत;

    free_scrollback(term);
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    set_string_option_direct((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    पूर्णांक
term_get_attr(win_T *wp, linenr_T lnum, पूर्णांक col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_ओळ_T	*ओळ;
    cellattr_T	*cellattr;
    पूर्णांक         sb_ओळ = -1;
    पूर्णांक         sb_col = col;

    जर (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_ओळ, &sb_col);

    जर (sb_ओळ < 0)
	cellattr = &term->tl_default_color;
    अन्यथा
    {
	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data + sb_ओळ;
	जर (sb_col < 0 || sb_col >= ओळ->sb_cols)
	    cellattr = &ओळ->sb_fill_attr;
	अन्यथा
	    cellattr = ओळ->sb_cells + sb_col;
    }
    परत cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    स्थिर शून्य
cterm_color2vterm(पूर्णांक nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    जर (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    अन्यथा
    {
	rgb->type = VTERM_COLOR_INDEXED;
	--rgb->index;
    }
}

/*
 * Initialize vterm color from the synID.
 * Returns TRUE if color is set to "fg" and "bg".
 * Otherwise returns FALSE.
 */
    स्थिर पूर्णांक
get_vterm_color_from_synid(पूर्णांक id, VTermColor *fg, VTermColor *bg)
{
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    जर (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
#  ifdef FEAT_VTP
	    // Finally get INVALCOLOR on this execution path
	    || (!p_tgc && t_colors >= 256)
#  endif
# endif
       )
    {
	guicolor_T fg_rgb = INVALCOLOR;
	guicolor_T bg_rgb = INVALCOLOR;

	जर (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	जर (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (अचिन्हित)(rgb >> 16);
	    fg->green = (अचिन्हित)(rgb >> 8) & 255;
	    fg->blue = (अचिन्हित)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	अन्यथा
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	जर (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (अचिन्हित)(rgb >> 16);
	    bg->green = (अचिन्हित)(rgb >> 8) & 255;
	    bg->blue = (अचिन्हित)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	अन्यथा
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	परत TRUE;
    }
    अन्यथा
#endif
    जर (t_colors >= 16)
    {
	पूर्णांक cterm_fg = -1;
	पूर्णांक cterm_bg = -1;

	जर (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	जर (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	अन्यथा
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	जर (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	अन्यथा
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	परत TRUE;
    }

    परत FALSE;
}

    शून्य
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    शून्य
term_update_hlfwin(win_T *wp)
{
    पूर्णांक id = wp->w_hlfwin_id;

    जर (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    शून्य
term_update_hlfwin_all(शून्य)
{
    win_T	 *wp = रिक्त;
    पूर्णांक		 did_curwin = FALSE;

    पर्यंत (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    शून्य
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    पूर्णांक		    fgval, bgval;
    पूर्णांक		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    जर (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    अन्यथा
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, रिक्त);

    जर (!get_vterm_color_from_synid(id, fg, bg))
    {
#जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	पूर्णांक tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	जर (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    जर (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	अन्यथा
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	जर (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    जर (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	अन्यथा
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    स्थिर पूर्णांक
term_use_palette(शून्य)
{
    जर (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	परत TRUE;
    परत FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    स्थिर शून्य
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    पूर्णांक		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    साठी (; index < 16; index++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (अचिन्हित)(rgb[index] >> 16);
	color.green = (अचिन्हित)(rgb[index] >> 8) & 255;
	color.blue = (अचिन्हित)rgb[index] & 255;
	color.index = 0;
	vterm_state_set_palette_color(state, index, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    स्थिर पूर्णांक
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    पूर्णांक		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    साठी (li = list->lv_first; li != रिक्त && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	जर (color_name == रिक्त)
	    परत FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	जर (guicolor == INVALCOLOR)
	    परत FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    जर (n != 16 || li != रिक्त)
	परत FAIL;

    set_vterm_palette(vterm, rgb);

    परत OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    स्थिर शून्य
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", रिक्त, TRUE);

    जर (var == रिक्त)
	परत;

    जर (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == रिक्त
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    स्थिर शून्य
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    पूर्णांक		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = रिक्त;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	जर (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    परत;
	}
    }

    CLEAR_FIELD(ea);

    जर (opt_item != रिक्त && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != रिक्त)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	जर (p == रिक्त)
	    p = dict_get_string(dict, "fileformat", FALSE);
	जर (p != रिक्त)
	{
	    जर (check_ff_मूल्य(p) == FAIL)
		ch_log(रिक्त, "Invalid ff argument to drop: %s", p);
	    अन्यथा
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	जर (p == रिक्त)
	    p = dict_get_string(dict, "encoding", FALSE);
	जर (p != रिक्त)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    जर (ea.cmd != रिक्त)
	    {
		प्र_लिखो((अक्षर *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	जर (p != रिक्त)
	    get_bad_opt(p, &ea);

	जर (dict_has_किल्ली(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	जर (dict_has_किल्ली(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	जर (dict_has_किल्ली(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	जर (dict_has_किल्ली(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    जर (ea.cmd == रिक्त)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    स्थिर पूर्णांक
is_permitted_term_api(char_u *func, char_u *pat)
{
    परत pat != रिक्त && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    स्थिर शून्य
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    जर (item->li_next == रिक्त)
    {
	ch_log(channel, "Missing function arguments for call");
	परत;
    }
    func = tv_get_string(&item->li_tv);

    जर (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	परत;
    }

    argvars[0].v_type = VAR_संख्या;
    argvars[0].vval.v_संख्या = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    जर (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    अन्यथा
	ch_log(channel, "Calling function %s failed", func);
}

/*
 * URL decoding (also know as Percent-encoding).
 *
 * Note this function currently is only used for decoding shell's
 * OSC 7 escape sequence which we can assume all bytes are valid
 * UTF-8 bytes. Thus we don't need to deal with invalid UTF-8
 * encoding bytes like 0xfe, 0xff.
 */
    स्थिर माप_प्रकार
url_decode(स्थिरांक अक्षर *src, स्थिरांक माप_प्रकार len, char_u *dst)
{
    माप_प्रकार i = 0, j = 0;

    पर्यंत (i < len)
    {
	जर (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	अन्यथा
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    परत j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    स्थिर शून्य
sync_shell_dir(garray_T *gap)
{
    पूर्णांक       offset = 7;  // len of "file://" is 7
    अक्षर      *pos = (अक्षर *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    पर्यंत (offset < (पूर्णांक)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    जर (offset >= (पूर्णांक)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	परत;
    }

    new_dir = alloc(gap->ga_len - offset + 1);
    url_decode(pos, gap->ga_len-offset, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_WINDOW);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    स्थिर पूर्णांक
parse_osc(पूर्णांक command, VTermStringFragment frag, शून्य *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == रिक्त ? रिक्त
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    जर (command != 51 && (command != 7 || !p_asd))
	परत 0;

    // Concatenate what was received until the final piece is found.
    जर (ga_grow(gap, (पूर्णांक)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	परत 1;
    }
    mch_memmove((अक्षर *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (पूर्णांक)frag.len;
    जर (!frag.final)
	परत 1;

    ((अक्षर *)gap->ga_data)[gap->ga_len] = 0;

    जर (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	परत 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = रिक्त;
    reader.js_used = 0;
    जर (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != रिक्त)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	जर (item == रिक्त)
	    ch_log(channel, "Missing command");
	अन्यथा
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    जर (item == रिक्त)
		ch_log(channel, "Missing argument for %s", cmd);
	    अन्यथा जर (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    अन्यथा जर (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    अन्यथा
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    अन्यथा
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    परत 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    स्थिर पूर्णांक
parse_csi(
	स्थिरांक अक्षर  *leader UNUSED,
	स्थिरांक दीर्घ  args[],
	पूर्णांक	    argcount,
	स्थिरांक अक्षर  *intermed UNUSED,
	अक्षर	    command,
	शून्य	    *user)
{
    term_T	*term = (term_T *)user;
    अक्षर	buf[100];
    पूर्णांक		len;
    पूर्णांक		x = 0;
    पूर्णांक		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    जर (command != 't' || argcount != 1 || args[0] != 13)
	परत 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#जर defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (शून्य)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	जर (wp->w_buffer == term->tl_buffer)
	    थांबा;
    जर (wp != रिक्त)
    {
#ifdef FEAT_GUI
	जर (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	अन्यथा
#endif
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(term->tl_job->jv_channel, get_tty_part(term),
						     (char_u *)buf, len, रिक्त);
    परत 1;
}

स्थिर VTermStateFallbacks state_fallbacks = {
    रिक्त,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    रिक्त,		// dcs
    रिक्त,		// apc
    रिक्त,		// pm
    रिक्त		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    स्थिर शून्य *
vterm_malloc(माप_प्रकार size, शून्य *data UNUSED)
{
    // make sure that the length is not zero
    परत alloc_clear(size == 0 ? 1L : size);
}

    स्थिर शून्य
vterm_memfree(शून्य *ptr, शून्य *data UNUSED)
{
    vim_free(ptr);
}

स्थिर VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    स्थिर पूर्णांक
create_vterm(term_T *term, पूर्णांक rows, पूर्णांक cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_मूल्य	    मूल्य;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, रिक्त);
    term->tl_vterm = vterm;
    जर (vterm == रिक्त)
	परत FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    जर (state == रिक्त || screen == रिक्त)
    {
	vterm_free(vterm);
	परत FAIL;
    }

    vterm_screen_set_callbacks(screen, &screen_callbacks, term);
    vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_set_utf8(vterm, 1);

    term_init_default_colors(term);

    vterm_state_set_default_colors(
	    state,
	    &term->tl_default_color.fg,
	    &term->tl_default_color.bg);

    जर (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_set_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#ifdef MSWIN
    जर (GetCaretBlinkTime() == INFINITE)
	मूल्य.boolean = 0;
    अन्यथा
	मूल्य.boolean = 1;
#अन्यथा
    मूल्य.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &मूल्य);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    परत OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    स्थिर शून्य
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    पूर्णांक		index;

    साठी (index = 0; index < 16; index++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_INDEXED;
	ansi_color2rgb(index, &color.red, &color.green, &color.blue,
		&color.index);
	// The first valid index starts at 1.
	color.index -= 1;

	vterm_state_set_palette_color(state, index, &color);
    }
}

    स्थिर शून्य
term_update_palette(term_T *term)
{
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    जर (term_use_palette()
	    && (term->tl_palette != रिक्त
		|| find_var((char_u *)"g:terminal_ansi_colors", रिक्त, TRUE)
								      != रिक्त))
    {
	जर (term->tl_palette != रिक्त)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	अन्यथा
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    अन्यथा
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    शून्य
term_update_palette_all(शून्य)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	जर (term->tl_vterm == रिक्त)
	    सुरूठेवा;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    शून्य
term_update_colors_all(शून्य)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	जर (term->tl_vterm == रिक्त)
	    सुरूठेवा;
	term_init_default_colors(term);
	vterm_state_set_default_colors(
		vterm_obtain_state(term->tl_vterm),
		&term->tl_default_color.fg,
		&term->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
term_get_status_text(term_T *term)
{
    जर (term->tl_status_text != रिक्त)
	परत term->tl_status_text;

    char_u *txt;
    माप_प्रकार len;
    char_u *fname;

    जर (term->tl_normal_mode)
    {
	जर (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	अन्यथा
	    txt = (char_u *)_("Terminal-finished");
    }
    अन्यथा जर (term->tl_title != रिक्त)
	txt = term->tl_title;
    अन्यथा जर (term_none_open(term))
	txt = (char_u *)_("active");
    अन्यथा जर (term_job_running(term))
	txt = (char_u *)_("running");
    अन्यथा
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    जर (term->tl_status_text != रिक्त)
	vim_snprintf((अक्षर *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    परत term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    शून्य
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    पूर्णांक
set_ref_in_term(पूर्णांक copyID)
{
    पूर्णांक		पात = FALSE;
    term_T	*term;
    typval_T	tv;

    साठी (term = first_term; !पात && term != रिक्त; term = term->tl_next)
	जर (term->tl_job != रिक्त)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    पात = पात || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    परत पात;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    स्थिर buf_T *
term_get_buf(typval_T *argvars, अक्षर *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    जर (buf == रिक्त || buf->b_term == रिक्त)
    {
	(शून्य)tv_get_संख्या(&argvars[0]);    // issue errmsg if type error
	ch_log(रिक्त, "%s: invalid buffer argument", where);
	परत रिक्त;
    }
    परत buf;
}

    स्थिर शून्य
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    स्थिर शून्य
dump_term_color(खाता *fd, VTermColor *color)
{
    पूर्णांक index;

    जर (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    अन्यथा जर (color->type == 0)
	// use RGB values
	index = 255;
    अन्यथा
	// default color
	index = 0;
    ख_लिखो(fd, "%02x%02x%02x%d",
	    (पूर्णांक)color->red, (पूर्णांक)color->green, (पूर्णांक)color->blue,
	    index);
}

/*
 * "term_dumpwrite(buf, filename, options)" function
 *
 * Each screen cell in full is:
 *    |{characters}+{attributes}#{fg-color}{color-idx}#{bg-color}{color-idx}
 * {characters} is a space for an empty cell
 * For a double-width character "+" is changed to "*" and the next cell is
 * skipped.
 * {attributes} is the decimal value of HL_BOLD + HL_UNDERLINE, etc.
 *			  when "&" use the same as the previous cell.
 * {fg-color} is hex RGB, when "&" use the same as the previous cell.
 * {bg-color} is hex RGB, when "&" use the same as the previous cell.
 * {color-idx} is a number from 0 to 255
 *
 * Screen cell with same width, attributes and color as the previous one:
 *    |{characters}
 *
 * To use the color of the previous cell, use "&" instead of {color}-{idx}.
 *
 * Repeating the previous screen cell:
 *    @{count}
 */
    शून्य
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    पूर्णांक		max_height = 0;
    पूर्णांक		max_width = 0;
    stat_T	st;
    खाता	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    जर (term->tl_vterm == रिक्त)
    {
	emsg(_(e_job_already_finished));
	परत;
    }

    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	जर (check_for_dict_arg(argvars, 2) == FAIL)
	    परत;
	d = argvars[2].vval.v_dict;
	जर (d != रिक्त)
	{
	    max_height = dict_get_संख्या(d, "rows");
	    max_width = dict_get_संख्या(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    जर (fname == रिक्त)
	परत;
    जर (mch_stat((अक्षर *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	परत;
    }

    जर (*fname == NUL || (fd = mch_fopen((अक्षर *)fname, WRITEBIN)) == रिक्त)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	परत;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    साठी (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	पूर्णांक		repeat = 0;

	साठी (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    पूर्णांक		    same_attr;
	    पूर्णांक		    same_chars = TRUE;
	    पूर्णांक		    i;
	    पूर्णांक		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    जर (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    साठी (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		पूर्णांक c = cell.chars[i];
		पूर्णांक pc = prev_cell.chars[i];
		पूर्णांक should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		जर (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		जर (c != pc)
		    same_chars = FALSE;
		जर (should_break)
		    थांबा;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    जर (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    अन्यथा
	    {
		जर (repeat > 0)
		{
		    ख_लिखो(fd, "@%d", repeat);
		    repeat = 0;
		}
		ख_माला_दो(is_cursor_pos ? ">" : "|", fd);

		जर (cell.chars[0] == NUL)
		    ख_माला_दो(" ", fd);
		अन्यथा
		{
		    char_u	charbuf[10];
		    पूर्णांक		len;

		    साठी (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			ख_डालो(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		जर (cell.width != prev_cell.width || !same_attr)
		{
		    जर (cell.width == 2)
			ख_माला_दो("*", fd);
		    अन्यथा
			ख_माला_दो("+", fd);

		    जर (same_attr)
		    {
			ख_माला_दो("&", fd);
		    }
		    अन्यथा
		    {
			ख_लिखो(fd, "%d", vtermAttr2hl(&cell.attrs));
			जर (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    ख_माला_दो("&", fd);
			अन्यथा
			{
			    ख_माला_दो("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			जर (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    ख_माला_दो("&", fd);
			अन्यथा
			{
			    ख_माला_दो("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    जर (cell.width == 2)
		++pos.col;
	}
	जर (repeat > 0)
	    ख_लिखो(fd, "@%d", repeat);
	ख_माला_दो("\n", fd);
    }

    ख_बंद(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    स्थिर शून्य
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    स्थिर शून्य
append_cell(garray_T *gap, cellattr_T *cell)
{
    जर (ga_grow(gap, 1) == FAIL)
	परत;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    स्थिर शून्य
clear_cellattr(cellattr_T *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_DEFAULT_BG;
}

/*
 * Read the dump file from "fd" and append lines to the current buffer.
 * Return the cell width of the longest line.
 */
    स्थिर पूर्णांक
read_dump_file(खाता *fd, VTermPos *cursor_pos)
{
    पूर्णांक		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {रिक्त, 0};
    पूर्णांक		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    पूर्णांक		    max_cells = 0;
    पूर्णांक		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, आकार(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = ख_अक्षर_लो(fd);
    साठी (;;)
    {
	जर (c == खातापूर्ण)
	    थांबा;
	जर (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = ख_अक्षर_लो(fd);
	}
	अन्यथा जर (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    जर (ga_text.ga_data == रिक्त)
		dump_is_corrupt(&ga_text);
	    जर (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_ओळ_T   *ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		जर (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		ओळ->sb_cols = ga_cell.ga_len;
		ओळ->sb_bytes = ga_text.ga_len;
		ओळ->sb_cells = ga_cell.ga_data;
		ओळ->sb_fill_attr = term->tl_default_color;
		ओळ->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_ओळ_मोजणी, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    अन्यथा
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = ख_अक्षर_लो(fd);
	}
	अन्यथा जर (c == '|' || c == '>')
	{
	    पूर्णांक prev_len = ga_text.ga_len;

	    जर (c == '>')
	    {
		जर (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = ख_अक्षर_लो(fd);
	    जर (c != खातापूर्ण)
		ga_append(&ga_text, c);
	    साठी (;;)
	    {
		c = ख_अक्षर_लो(fd);
		जर (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == खातापूर्ण || c == '\n')
		    थांबा;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    जर (ga_text.ga_data != रिक्त)
	    {
		prev_char.length = (माप_प्रकार)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		जर (prev_char.string == रिक्त)
		    prev_char.length = 0;
	    }

	    जर (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    अन्यथा जर (c == '+' || c == '*')
	    {
		पूर्णांक is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = ख_अक्षर_लो(fd);
		जर (c == '&')
		{
		    // use same attr as previous cell
		    c = ख_अक्षर_लो(fd);
		}
		अन्यथा जर (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    पर्यंत (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = ख_अक्षर_लो(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    साठी (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			जर (c == '&')
			{
			    // use same color as previous cell
			    c = ख_अक्षर_लो(fd);
			}
			अन्यथा जर (c == '#')
			{
			    पूर्णांक red, green, blue, index = 0, type;

			    c = ख_अक्षर_लो(fd);
			    red = hex2nr(c);
			    c = ख_अक्षर_लो(fd);
			    red = (red << 4) + hex2nr(c);
			    c = ख_अक्षर_लो(fd);
			    green = hex2nr(c);
			    c = ख_अक्षर_लो(fd);
			    green = (green << 4) + hex2nr(c);
			    c = ख_अक्षर_लो(fd);
			    blue = hex2nr(c);
			    c = ख_अक्षर_लो(fd);
			    blue = (blue << 4) + hex2nr(c);
			    c = ख_अक्षर_लो(fd);
			    जर (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    पर्यंत (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = ख_अक्षर_लो(fd);
			    }
			    जर (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				जर (index == 0)
				{
				    जर (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    अन्यथा
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    अन्यथा
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    जर (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    अन्यथा
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			अन्यथा
			    dump_is_corrupt(&ga_text);
		    }
		}
		अन्यथा
		    dump_is_corrupt(&ga_text);
	    }
	    अन्यथा
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    जर (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	अन्यथा जर (c == '@')
	{
	    जर (prev_char.string == रिक्त)
		dump_is_corrupt(&ga_text);
	    अन्यथा
	    {
		पूर्णांक मोजणी = 0;

		// repeat previous character, get the count
		साठी (;;)
		{
		    c = ख_अक्षर_लो(fd);
		    जर (!SAFE_isdigit(c))
			थांबा;
		    मोजणी = मोजणी * 10 + (c - '0');
		}

		पर्यंत (मोजणी-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	अन्यथा
	{
	    dump_is_corrupt(&ga_text);
	    c = ख_अक्षर_लो(fd);
	}
    }

    जर (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_ओळ_मोजणी, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    परत max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    स्थिर char_u *
get_separator(पूर्णांक text_width, char_u *fname)
{
    पूर्णांक	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    पूर्णांक	    fname_size;
    char_u  *p = fname;
    पूर्णांक	    i;
    माप_प्रकार  off;

    textline = alloc(width + STRLEN(fname) + 1);
    जर (textline == रिक्त)
	परत रिक्त;

    fname_size = vim_strsize(fname);
    जर (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    अन्यथा जर (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    पर्यंत (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    साठी (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    साठी (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    परत textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    स्थिर शून्य
term_load_dump(typval_T *argvars, typval_T *rettv, पूर्णांक do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = रिक्त;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = रिक्त;
    char_u	*fname_tofree = रिक्त;
    खाता	*fd1;
    खाता	*fd2 = रिक्त;
    char_u	*textline = रिक्त;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    जर (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    जर (fname1 == रिक्त || (do_diff && fname2 == रिक्त))
    {
	emsg(_(e_invalid_argument));
	परत;
    }
    fd1 = mch_fopen((अक्षर *)fname1, READBIN);
    जर (fd1 == रिक्त)
    {
	semsg(_(e_cant_read_file_str), fname1);
	परत;
    }
    जर (do_diff)
    {
	fd2 = mch_fopen((अक्षर *)fname2, READBIN);
	जर (fd2 == रिक्त)
	{
	    ख_बंद(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    परत;
	}
    }

    init_job_options(&opt);
    जर (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	जा theend;

    जर (opt.jo_term_name == रिक्त)
    {
	माप_प्रकार len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	जर (fname_tofree != रिक्त)
	{
	    vim_snprintf((अक्षर *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    जर (opt.jo_bufnr_buf != रिक्त)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	जर (wp == रिक्त)
	    semsg(_(e_invalid_argument_str), "bufnr");
	अन्यथा
	{
	    buf = curbuf;
	    पर्यंत (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    अन्यथा
	// Create a new terminal window.
	buf = term_start(&argvars[0], रिक्त, &opt, TERM_START_NOJOB);

    जर (buf != रिक्त && buf->b_term != रिक्त)
    {
	पूर्णांक		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	पूर्णांक		width;
	पूर्णांक		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_संख्या = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	जर (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	जर (!do_diff)
	    जा theend;

	term->tl_शीर्ष_diff_rows = curbuf->b_ml.ml_ओळ_मोजणी;

	textline = get_separator(width, fname1);
	जर (textline == रिक्त)
	    जा theend;
	जर (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ओळ_मोजणी, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	जर (textline == रिक्त)
	    जा theend;
	जर (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ओळ_मोजणी, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	width2 = read_dump_file(fd2, &cursor_pos2);
	जर (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    जर (textline == रिक्त)
		जा theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_ओळ_मोजणी - bot_lnum;

	साठी (lnum = 1; lnum <= term->tl_शीर्ष_diff_rows; ++lnum)
	{
	    जर (lnum + bot_lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	    {
		// bottom part has fewer rows, fill with "-"
		साठी (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    अन्यथा
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		पूर्णांक	col;
		sb_ओळ_T   *sb_ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_ओळ + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_ओळ + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		जर (line1 == रिक्त)
		    थांबा;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		साठी (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    पूर्णांक len1 = utfc_ptr2len(p1);
		    पूर्णांक len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    जर (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    अन्यथा जर (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    अन्यथा जर (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    अन्यथा जर (cellattr1 != रिक्त && cellattr2 != रिक्त)
		    {
			जर ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			अन्यथा जर (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			अन्यथा जर (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			अन्यथा जर (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		पर्यंत (col < width)
		{
		    जर (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    अन्यथा जर (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    अन्यथा
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    जर (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_शीर्ष_diff_rows) == OK)
		ml_append(term->tl_शीर्ष_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	पर्यंत (lnum + bot_lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    // bottom part has more rows, fill with "+"
	    साठी (i = 0; i < width; ++i)
		textline[i] = '+';
	    जर (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_शीर्ष_diff_rows) == OK)
		ml_append(term->tl_शीर्ष_diff_rows + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	term->tl_cols = width;

	// looks better without wrapping
	curwin->w_p_wrap = 0;
    }

theend:
    vim_free(textline);
    vim_free(fname_tofree);
    ख_बंद(fd1);
    जर (fd2 != रिक्त)
	ख_बंद(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    पूर्णांक
term_swap_diff(शून्य)
{
    term_T	*term = curbuf->b_term;
    linenr_T	ओळ_मोजणी;
    linenr_T	शीर्ष_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_ओळ_T	*sb_ओळ;

    जर (term == रिक्त
	    || !term_is_finished(curbuf)
	    || term->tl_शीर्ष_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	परत FAIL;

    ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
    शीर्ष_rows = term->tl_शीर्ष_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = ओळ_मोजणी - bot_rows;
    sb_ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    साठी (lnum = 1; lnum <= शीर्ष_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	जर (p == रिक्त)
	    परत OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    साठी (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	जर (p == रिक्त)
	    परत OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    जर (p == रिक्त)
	परत OK;
    ml_append(ओळ_मोजणी - शीर्ष_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(ओळ_मोजणी - शीर्ष_rows));
    जर (p == रिक्त)
	परत OK;
    ml_delete(ओळ_मोजणी - शीर्ष_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    जर (शीर्ष_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	साठी (lnum = 0; lnum < शीर्ष_rows; ++lnum)
	{
	    sb_ओळ_T	temp;

	    temp = *(sb_ओळ + lnum);
	    *(sb_ओळ + lnum) = *(sb_ओळ + bot_start + lnum);
	    *(sb_ओळ + bot_start + lnum) = temp;
	}
    }
    अन्यथा
    {
	माप_प्रकार		size = आकार(sb_ओळ_T) * term->tl_scrollback.ga_len;
	sb_ओळ_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	जर (temp != रिक्त)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    आकार(sb_ओळ_T) * bot_rows);
	    mch_memmove((sb_ओळ_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + शीर्ष_rows,
		    आकार(sb_ओळ_T) * (ओळ_मोजणी - शीर्ष_rows - bot_rows));
	    mch_memmove((sb_ओळ_T *)term->tl_scrollback.ga_data
						       + ओळ_मोजणी - शीर्ष_rows,
		    temp,
		    आकार(sb_ओळ_T) * शीर्ष_rows);
	    vim_free(temp);
	}
    }

    term->tl_शीर्ष_diff_rows = bot_rows;
    term->tl_bot_diff_rows = शीर्ष_rows;

    update_screen(UPD_NOT_VALID);
    परत OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    शून्य
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	परत;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    शून्य
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    शून्य
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    जर (buf == रिक्त)
	परत;
    rettv->vval.v_संख्या = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    शून्य
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक	    attr;
    माप_प्रकार  i;
    char_u  *name;

    स्थिर संरचना {
	अक्षर	    *name;
	पूर्णांक	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    attr = tv_get_संख्या(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    जर (name == रिक्त)
	परत;

    जर (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    साठी (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	जर (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_संख्या = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    थांबा;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    शून्य
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getcursor()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_संख्या(l, term->tl_cursor_pos.row + 1);
    list_append_संख्या(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    जर (d == रिक्त)
	परत;

    dict_add_संख्या(d, "visible", term->tl_cursor_visible);
    dict_add_संख्या(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_संख्या(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    जर (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    शून्य
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getjob()");
    जर (buf == रिक्त)
    {
	जर (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = रिक्त;
	}
	अन्यथा
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_संख्या = VVAL_NULL;
	}
	परत;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    जर (rettv->vval.v_job != रिक्त)
	++rettv->vval.v_job->jv_refcount;
}

    स्थिर पूर्णांक
get_row_संख्या(typval_T *tv, term_T *term)
{
    जर (tv->v_type == VAR_STRING
	    && tv->vval.v_string != रिक्त
	    && STRCMP(tv->vval.v_string, ".") == 0)
	परत term->tl_cursor_pos.row;
    परत (पूर्णांक)tv_get_संख्या(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    शून्य
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    पूर्णांक		    row;

    rettv->v_type = VAR_STRING;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_getline()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    row = get_row_संख्या(&argvars[1], term);

    जर (term->tl_vterm == रिक्त)
    {
	linenr_T  lnum = 0;
	माप_प्रकार	  offset = 0;
	पूर्णांक	  sb_row = term->tl_scrollback_scrolled + row;
	sb_ओळ_T *ओळ;

	जर (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    परत;
	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, रिक्त, &offset);

	// vterm is finished, get the text from the buffer
	जर (lnum > 0 && lnum <= buf->b_ml.ml_ओळ_मोजणी)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    जर (STRLEN(p) >= offset + ओळ->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, ओळ->sb_bytes);
	}
    }
    अन्यथा
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	पूर्णांक		len;
	char_u		*p;

	जर (row < 0 || row >= term->tl_rows)
	    परत;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	जर (p == रिक्त)
	    परत;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (अक्षर *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    शून्य
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getscrolled()");
    जर (buf == रिक्त)
	परत;
    rettv->vval.v_संख्या = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    शून्य
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getsize()");
    जर (buf == रिक्त)
	परत;

    l = rettv->vval.v_list;
    list_append_संख्या(l, buf->b_term->tl_rows);
    list_append_संख्या(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    शून्य
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_संख्या_arg(argvars, 2) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_setsize()");
    जर (buf == रिक्त)
    {
	emsg(_(e_not_terminal_buffer));
	परत;
    }
    जर (buf->b_term->tl_vterm == रिक्त)
	परत;
    term = buf->b_term;
    rows = tv_get_संख्या(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_संख्या(&argvars[2]);
    cols = cols <= 0 ? term->tl_cols : cols;
    vterm_set_size(term->tl_vterm, rows, cols);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
    term_report_winsize(term, term->tl_rows, term->tl_cols);
}

/*
 * "term_getstatus(buf)" function
 */
    शून्य
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getstatus()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;

    जर (term_job_running(term))
	STRCPY(val, "running");
    अन्यथा
	STRCPY(val, "finished");
    जर (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    शून्य
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_gettitle()");
    जर (buf == रिक्त)
	परत;

    जर (buf->b_term->tl_title != रिक्त)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    शून्य
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = रिक्त;
    पूर्णांक		अंक = 0;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	परत;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    जर (buf == रिक्त)
	परत;
    जर (argvars[1].v_type != VAR_UNKNOWN)
	अंक = tv_get_bool(&argvars[1]);

    निवड (अंक)
    {
	स्थिती 0:
	    जर (buf->b_term->tl_job != रिक्त)
		p = buf->b_term->tl_job->jv_tty_out;
	    थांबा;
	स्थिती 1:
	    जर (buf->b_term->tl_job != रिक्त)
		p = buf->b_term->tl_job->jv_tty_in;
	    थांबा;
	पूर्वनिर्धारित:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    परत;
    }
    जर (p != रिक्त)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    शून्य
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    जर (rettv_list_alloc(rettv) == FAIL || first_term == रिक्त)
	परत;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	जर (tp->tl_buffer != रिक्त)
	    जर (list_append_संख्या(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		परत;
}

/*
 * "term_scrape(buf, row)" function
 */
    शून्य
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = रिक्त;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_ओळ_T	    *ओळ;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_scrape()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_संख्या(&argvars[1], term);

    जर (term->tl_vterm != रिक्त)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	जर (screen == रिक्त)  // can't really happen
	    परत;
	p = रिक्त;
	ओळ = रिक्त;
    }
    अन्यथा
    {
	पूर्णांक	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	माप_प्रकार	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, रिक्त, &offset);

	जर (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_ओळ_मोजणी)
	    परत;

	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	जर (STRLEN(p) < offset + ओळ->sb_bytes)
	    परत;

	p += offset;
    }

    साठी (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	पूर्णांक		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	माप_प्रकार		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	माप_प्रकार		mbslen;
	पूर्णांक		i;

	जर (screen == रिक्त)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    जर (pos.col >= ओळ->sb_cols)
		थांबा;
	    cellattr = ओळ->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	अन्यथा
	{
	    VTermScreenCell cell;

	    जर (vterm_screen_get_cell(screen, pos, &cell) == 0)
		थांबा;

	    mbslen = 0;
	    साठी (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		जर (cell.chars[i] == 0)
		    थांबा;
		mbslen += (*utf_char2bytes)((पूर्णांक)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	जर (dcell == रिक्त)
	    थांबा;
	जर (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    थांबा;
	}

	dict_add_string_len(dcell, "chars", mbs, (पूर्णांक)mbslen);

	rgblen = vim_snprintf_safelen((अक्षर *)rgb, आकार(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (पूर्णांक)rgblen);
	rgblen = vim_snprintf_safelen((अक्षर *)rgb, आकार(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (पूर्णांक)rgblen);

	dict_add_संख्या(dcell, "attr",
				      cell2attr(term, रिक्त, &attrs, &fg, &bg));
	dict_add_संख्या(dcell, "width", width);

	++pos.col;
	जर (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    शून्य
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*संदेश;
    term_T	*term;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_sendkeys()");
    जर (buf == रिक्त)
	परत;

    संदेश = tv_get_string_chk(&argvars[1]);
    जर (संदेश == रिक्त)
	परत;
    term = buf->b_term;
    जर (term->tl_vterm == रिक्त)
	परत;

    पर्यंत (*संदेश != NUL)
    {
	पूर्णांक c;

	जर (*संदेश == K_SPECIAL && संदेश[1] != NUL && संदेश[2] != NUL)
	{
	    c = TO_SPECIAL(संदेश[1], संदेश[2]);
	    संदेश += 3;
	}
	अन्यथा
	{
	    c = PTR2CHAR(संदेश);
	    संदेश += MB_CPTR2LEN(संदेश);
	}
	send_किल्ल्या_to_term(term, c, 0, FALSE);
    }
}

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    शून्य
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    पूर्णांक		index;
    list_T	*list;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = term_get_buf(argvars, "term_getansicolors()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    जर (term->tl_vterm == रिक्त)
	परत;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    साठी (index = 0; index < 16; index++)
    {
	माप_प्रकार	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((अक्षर *)hexbuf, आकार(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	जर (list_append_string(list, hexbuf, (पूर्णांक)hexbuflen) == FAIL)
	    परत;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    शून्य
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    पूर्णांक		n = 0;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_setansicolors()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    जर (term->tl_vterm == रिक्त)
	परत;

    जर (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	परत;

    जर (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_मूल्य_for_argument_str), "\"colors\"");
	परत;
    }

    जर (term->tl_palette == रिक्त)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    जर (term->tl_palette == रिक्त)
	परत;

    FOR_ALL_LIST_वस्तुसंच(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	जर (color_name == रिक्त)
	    परत;

	guicolor = GUI_GET_COLOR(color_name);
	जर (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    परत;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    शून्य
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_setapi()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    जर (api != रिक्त)
	term->tl_api = vim_strsave(api);
    अन्यथा
	term->tl_api = रिक्त;
}

/*
 * "term_setrestore(buf, command)" function
 */
    शून्य
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#जर defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_setrestore()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    जर (cmd != रिक्त)
	term->tl_command = vim_strsave(cmd);
    अन्यथा
	term->tl_command = रिक्त;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    शून्य
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_setkill()");
    जर (buf == रिक्त)
	परत;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    जर (how != रिक्त)
	term->tl_kill = vim_strsave(how);
    अन्यथा
	term->tl_kill = रिक्त;
}

/*
 * "term_start(command, options)" function
 */
    शून्य
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    जर (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    init_job_options(&opt);
    जर (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	परत;

    buf = term_start(&argvars[0], रिक्त, &opt, 0);

    जर (buf != रिक्त && buf->b_term != रिक्त)
	rettv->vval.v_संख्या = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    शून्य
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    buf = term_get_buf(argvars, "term_wait()");
    जर (buf == रिक्त)
	परत;
    जर (buf->b_term->tl_job == रिक्त)
    {
	ch_log(रिक्त, "term_wait(): no job to wait for");
	परत;
    }
    जर (buf->b_term->tl_job->jv_channel == रिक्त)
	// channel is closed, nothing to do
	परत;

    // Get the job status, this will detect a job that finished.
    जर (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(रिक्त, "term_wait(): waiting for channel to close");
	पर्यंत (buf->b_term != रिक्त && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    जर (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		थांबा;
	    जर (buf->b_term == रिक्त || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		थांबा;
	}

	term_flush_messages();
    }
    अन्यथा
    {
	दीर्घ रुको = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	जर (argvars[1].v_type != VAR_UNKNOWN)
	    रुको = tv_get_संख्या(&argvars[1]);
	ui_delay(रुको, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	term_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    शून्य
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	जर (term->tl_job == ch->ch_job)
	{
	    जर (term->tl_eof_chars != रिक्त)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(पूर्णांक)STRLEN(term->tl_eof_chars), रिक्त);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, रिक्त);
	    }
#ifdef MSWIN
	    अन्यथा
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, रिक्त);
#endif
	}
}

#जर defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    परत term != रिक्त ? term->tl_job : रिक्त;
}
#endif

#जर defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
शून्य (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    स्थिर पूर्णांक
dyn_conpty_init(पूर्णांक verbose)
{
    स्थिर HMODULE	hKerneldll = रिक्त;
    पूर्णांक			i;
    स्थिर संरचना
    {
	अक्षर	*name;
	FARPROC	*ptr;
    } conpty_entry[] =
    {
	{"CreatePseudoConsole", (FARPROC*)&pCreatePseudoConsole},
	{"ResizePseudoConsole", (FARPROC*)&pResizePseudoConsole},
	{"ClosePseudoConsole", (FARPROC*)&pClosePseudoConsole},
	{"InitializeProcThreadAttributeList",
				(FARPROC*)&pInitializeProcThreadAttributeList},
	{"UpdateProcThreadAttribute",
				(FARPROC*)&pUpdateProcThreadAttribute},
	{"DeleteProcThreadAttributeList",
				(FARPROC*)&pDeleteProcThreadAttributeList},
	{रिक्त, रिक्त}
    };

    जर (!has_conpty_working())
    {
	जर (verbose)
	    emsg(_(e_conpty_is_not_available));
	परत FAIL;
    }

    // No need to initialize twice.
    जर (hKerneldll)
	परत OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    साठी (i = 0; conpty_entry[i].name != रिक्त
					&& conpty_entry[i].ptr != रिक्त; ++i)
    {
	जर ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == रिक्त)
	{
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = रिक्त;
	    परत FAIL;
	}
    }

    परत OK;
}

    स्थिर पूर्णांक
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	अक्षर	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = रिक्त;
    WCHAR	    *cmd_wchar_copy = रिक्त;
    WCHAR	    *cwd_wchar = रिक्त;
    WCHAR	    *env_wchar = रिक्त;
    channel_T	    *channel = रिक्त;
    job_T	    *job = रिक्त;
    HANDLE	    jo = रिक्त;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = रिक्त;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = रिक्त;
    HANDLE	    o_theirs = रिक्त;
    HANDLE	    i_ours = रिक्त;
    HANDLE	    o_ours = रिक्त;
    अक्षर	    *errmsg = रिक्त;

    ga_init2(&ga_cmd, आकार(अक्षर*), 20);
    ga_init2(&ga_env, आकार(अक्षर*), 20);

    जर (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    अन्यथा जर (argvar->v_type == VAR_LIST)
    {
	जर (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    जा failed;
	cmd = ga_cmd.ga_data;
    }
    जर (cmd == रिक्त || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	जा failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, रिक्त);

    जर (cmd_wchar != रिक्त)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    जर (cmd_wchar == रिक्त)
	जा failed;
    जर (opt->jo_cwd != रिक्त)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, रिक्त);
	जर (cwd_wchar == रिक्त)
	    जा failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    जर (!CreatePipe(&i_theirs, &i_ours, रिक्त, 0))
	जा failed;
    जर (!CreatePipe(&o_ours, &o_theirs, रिक्त, 0))
	जा failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    जर (FAILED(hr))
	जा failed;

    term->tl_siex.StartupInfo.cb = आकार(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_आदान = INVALID_HANDLE_मूल्य;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_मूल्य;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_मूल्य;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(रिक्त, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    जर (!term->tl_siex.lpAttributeList)
	जा failed;
    जर (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	जा failed;
    जर (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    आकार(HPCON), रिक्त, रिक्त))
	जा failed;

    channel = add_channel();
    जर (channel == रिक्त)
	जा failed;

    job = job_alloc();
    जर (job == रिक्त)
	जा failed;
    जर (argvar->v_type == VAR_STRING)
    {
	पूर्णांक argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    अन्यथा
    {
	पूर्णांक argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    जर (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    जर (!CreateProcessW(रिक्त, cmd_wchar_copy, रिक्त, रिक्त, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	जा failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_set_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = CreateJobObject(रिक्त, रिक्त);
    जर (jo == रिक्त)
	जा failed;

    जर (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = रिक्त;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    जर (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	जा failed;

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    जर (term_use_palette())
    {
	जर (term->tl_palette != रिक्त)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	अन्यथा
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    channel_set_job(channel, job, opt);
    job_set_options(job, opt);

    job->jv_channel = channel;
    job->jv_proc_info = proc_info;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;
    job->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    जर (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((अक्षर *)fname, WRITEBIN);
	जर (term->tl_out_fd == रिक्त)
	    semsg(_(e_cant_open_file_str), fname);
    }

    परत OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    जर (channel != रिक्त)
	channel_clear(channel);
    जर (job != रिक्त)
    {
	job->jv_channel = रिक्त;
	job_cleanup(job);
    }
    term->tl_job = रिक्त;
    जर (jo != रिक्त)
	CloseHandle(jo);

    जर (term->tl_siex.lpAttributeList != रिक्त)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = रिक्त;
    जर (o_theirs != रिक्त)
	CloseHandle(o_theirs);
    जर (o_ours != रिक्त)
	CloseHandle(o_ours);
    जर (i_ours != रिक्त)
	CloseHandle(i_ours);
    जर (i_theirs != रिक्त)
	CloseHandle(i_theirs);
    जर (term->tl_conpty != रिक्त)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = रिक्त;
    // Propagate errors that occur in CreateProcess
    जर (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    परत FAIL;
}

    स्थिर शून्य
conpty_term_report_winsize(term_T *term, पूर्णांक rows, पूर्णांक cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    स्थिर शून्य
term_free_conpty(term_T *term)
{
    जर (term->tl_siex.lpAttributeList != रिक्त)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = रिक्त;
    जर (term->tl_conpty != रिक्त)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = रिक्त;
}

    पूर्णांक
use_conpty(शून्य)
{
    परत has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

शून्य* (*winpty_config_new)(UINT64, शून्य*);
शून्य* (*winpty_open)(शून्य*, शून्य*);
शून्य* (*winpty_spawn_config_new)(UINT64, शून्य*, LPCWSTR, शून्य*, शून्य*, शून्य*);
BOOL (*winpty_spawn)(शून्य*, शून्य*, HANDLE*, HANDLE*, DWORD*, शून्य*);
शून्य (*winpty_config_set_mouse_mode)(शून्य*, पूर्णांक);
शून्य (*winpty_config_set_initial_size)(शून्य*, पूर्णांक, पूर्णांक);
LPCWSTR (*winpty_conin_name)(शून्य*);
LPCWSTR (*winpty_conout_name)(शून्य*);
LPCWSTR (*winpty_conerr_name)(शून्य*);
शून्य (*winpty_free)(शून्य*);
शून्य (*winpty_config_free)(शून्य*);
शून्य (*winpty_spawn_config_free)(शून्य*);
शून्य (*winpty_error_free)(शून्य*);
LPCWSTR (*winpty_error_संदेश)(शून्य*);
BOOL (*winpty_set_size)(शून्य*, पूर्णांक, पूर्णांक, शून्य*);
HANDLE (*winpty_agent_process)(शून्य*);

# define WINPTY_DLL "winpty.dll"

स्थिर HINSTANCE hWinPtyDLL = रिक्त;

    स्थिर पूर्णांक
dyn_winpty_init(पूर्णांक verbose)
{
    पूर्णांक i;
    स्थिर संरचना
    {
	अक्षर	    *name;
	FARPROC	    *ptr;
    } winpty_entry[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_set_mouse_mode},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_set_initial_size},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_संदेश},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{रिक्त, रिक्त}
    };

    // No need to initialize twice.
    जर (hWinPtyDLL)
	परत OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    जर (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((अक्षर *)p_winptydll);
    जर (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    जर (!hWinPtyDLL)
    {
	जर (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	परत FAIL;
    }
    साठी (i = 0; winpty_entry[i].name != रिक्त
					 && winpty_entry[i].ptr != रिक्त; ++i)
    {
	जर ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == रिक्त)
	{
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = रिक्त;
	    परत FAIL;
	}
    }

    परत OK;
}

    स्थिर पूर्णांक
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	अक्षर	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = रिक्त;
    WCHAR	    *cwd_wchar = रिक्त;
    WCHAR	    *env_wchar = रिक्त;
    channel_T	    *channel = रिक्त;
    job_T	    *job = रिक्त;
    DWORD	    error;
    HANDLE	    jo = रिक्त;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    शून्य	    *winpty_err = रिक्त;
    शून्य	    *spawn_config = रिक्त;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = रिक्त;

    ga_init2(&ga_cmd, आकार(अक्षर*), 20);
    ga_init2(&ga_env, आकार(अक्षर*), 20);

    जर (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    अन्यथा जर (argvar->v_type == VAR_LIST)
    {
	जर (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    जा failed;
	cmd = ga_cmd.ga_data;
    }
    जर (cmd == रिक्त || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	जा failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, रिक्त);
    ga_clear(&ga_cmd);
    जर (cmd_wchar == रिक्त)
	जा failed;
    जर (opt->jo_cwd != रिक्त)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, रिक्त);
	जर (cwd_wchar == रिक्त)
	    जा failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    जर (term->tl_winpty_config == रिक्त)
	जा failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    जर (term->tl_winpty == रिक्त)
	जा failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    रिक्त,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    जर (spawn_config == रिक्त)
	जा failed;

    channel = add_channel();
    जर (channel == रिक्त)
	जा failed;

    job = job_alloc();
    जर (job == रिक्त)
	जा failed;
    जर (argvar->v_type == VAR_STRING)
    {
	पूर्णांक argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    अन्यथा
    {
	पूर्णांक argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    जर (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    जर (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	जा failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, रिक्त,
	    OPEN_EXISTING, 0, रिक्त),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, रिक्त,
	    OPEN_EXISTING, 0, रिक्त),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, रिक्त,
	    OPEN_EXISTING, 0, रिक्त));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(रिक्त, रिक्त);
    जर (jo == रिक्त)
	जा failed;

    जर (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = रिक्त;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    जर (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	जा failed;

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    जर (term_use_palette())
    {
	जर (term->tl_palette != रिक्त)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	अन्यथा
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    channel_set_job(channel, job, opt);
    job_set_options(job, opt);

    job->jv_channel = channel;
    job->jv_proc_info.hProcess = child_process_handle;
    job->jv_proc_info.dwProcessId = GetProcessId(child_process_handle);
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;
    job->jv_tty_in = utf16_to_enc(
	    (short_u *)winpty_conin_name(term->tl_winpty), रिक्त);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), रिक्त);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    जर (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((अक्षर *)fname, WRITEBIN);
	जर (term->tl_out_fd == रिक्त)
	    semsg(_(e_cant_open_file_str), fname);
    }

    परत OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    जर (spawn_config != रिक्त)
	winpty_spawn_config_free(spawn_config);
    जर (channel != रिक्त)
	channel_clear(channel);
    जर (job != रिक्त)
    {
	job->jv_channel = रिक्त;
	job_cleanup(job);
    }
    term->tl_job = रिक्त;
    जर (jo != रिक्त)
	CloseHandle(jo);
    जर (term->tl_winpty != रिक्त)
	winpty_free(term->tl_winpty);
    term->tl_winpty = रिक्त;
    जर (term->tl_winpty_config != रिक्त)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = रिक्त;
    जर (winpty_err != रिक्त)
    {
	अक्षर *संदेश = (अक्षर *)utf16_to_enc(
				(short_u *)winpty_error_संदेश(winpty_err), रिक्त);

	जर (संदेश != रिक्त)
	{
	    emsg(संदेश);
	    vim_free(संदेश);
	}
	winpty_error_free(winpty_err);
    }
    परत FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	अक्षर	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    पूर्णांक		    use_winpty = FALSE;
    पूर्णांक		    use_conpty = FALSE;
    पूर्णांक		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    जर (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	परत dyn_winpty_init(TRUE);

    जर (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    जर (tty_type == NUL)
    {
	जर (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	अन्यथा जर (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    अन्यथा जर (tty_type == 'w')	// winpty
    {
	जर (has_winpty)
	    use_winpty = TRUE;
    }
    अन्यथा जर (tty_type == 'c')	// conpty
    {
	जर (has_conpty)
	    use_conpty = TRUE;
	अन्यथा
	    परत dyn_conpty_init(TRUE);
    }

    जर (use_conpty)
	परत conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    जर (use_winpty)
	परत winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    परत dyn_winpty_init(TRUE);
}

    स्थिर पूर्णांक
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_मूल्य;
    HANDLE	    hPipeOut = INVALID_HANDLE_मूल्य;
    अक्षर	    in_name[80], out_name[80];
    channel_T	    *channel = रिक्त;

    जर (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	परत FAIL;

    vim_snprintf(in_name, आकार(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, रिक्त);
    जर (hPipeIn == INVALID_HANDLE_मूल्य)
	जा failed;

    vim_snprintf(out_name, आकार(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, रिक्त);
    जर (hPipeOut == INVALID_HANDLE_मूल्य)
	जा failed;

    ConnectNamedPipe(hPipeIn, रिक्त);
    ConnectNamedPipe(hPipeOut, रिक्त);

    term->tl_job = job_alloc();
    जर (term->tl_job == रिक्त)
	जा failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    जर (channel == रिक्त)
	जा failed;
    term->tl_job->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_set_pipes(channel,
	(sock_T)hPipeIn,
	(sock_T)hPipeOut,
	(sock_T)hPipeOut);
    channel_set_job(channel, term->tl_job, options);
    term->tl_job->jv_tty_in = vim_strsave((char_u*)in_name);
    term->tl_job->jv_tty_out = vim_strsave((char_u*)out_name);

    परत OK;

failed:
    जर (hPipeIn != रिक्त)
	CloseHandle(hPipeIn);
    जर (hPipeOut != रिक्त)
	CloseHandle(hPipeOut);
    परत FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    स्थिर शून्य
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    जर (term->tl_winpty != रिक्त)
	winpty_free(term->tl_winpty);
    term->tl_winpty = रिक्त;
    जर (term->tl_winpty_config != रिक्त)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = रिक्त;
    जर (term->tl_vterm != रिक्त)
	vterm_free(term->tl_vterm);
    term->tl_vterm = रिक्त;
}

/*
 * Report the size to the terminal.
 */
    स्थिर शून्य
term_report_winsize(term_T *term, पूर्णांक rows, पूर्णांक cols)
{
    जर (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    जर (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, रिक्त);
}

    पूर्णांक
terminal_enabled(शून्य)
{
    परत dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#अन्यथा

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	अक्षर	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = रिक्त;

    जर (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	परत FAIL;

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    जर (term_use_palette())
    {
	जर (term->tl_palette != रिक्त)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	अन्यथा
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    जर (term->tl_job != रिक्त)
	++term->tl_job->jv_refcount;

    परत term->tl_job != रिक्त
	&& term->tl_job->jv_channel != रिक्त
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    स्थिर पूर्णांक
create_pty_only(term_T *term, jobopt_T *opt)
{
    जर (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	परत FAIL;

    term->tl_job = job_alloc();
    जर (term->tl_job == रिक्त)
	परत FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    परत mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    स्थिर शून्य
term_free_vterm(term_T *term)
{
    जर (term->tl_vterm != रिक्त)
	vterm_free(term->tl_vterm);
    term->tl_vterm = रिक्त;
}

/*
 * Report the size to the terminal.
 */
    स्थिर शून्य
term_report_winsize(term_T *term, पूर्णांक rows, पूर्णांक cols)
{
    // Use an ioctl() to report the new window size to the job.
    जर (term->tl_job == रिक्त || term->tl_job->jv_channel == रिक्त)
	परत;

    पूर्णांक fd = -1;
    पूर्णांक part;

    साठी (part = PART_OUT; part < PART_मोजणी; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	जर (mch_isatty(fd))
	    थांबा;
    }
    जर (part < PART_मोजणी && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
