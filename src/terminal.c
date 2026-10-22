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

#यदि defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#अन्तर्भाव "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
प्रकारनिर्देश संरचना {
    VTermScreenCellAttrs	attrs;
    वर्ण			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

प्रकारनिर्देश संरचना sb_ओळ_S {
    पूर्णाङ्क		sb_cols;	// can differ per line
    पूर्णाङ्क		sb_bytes;	// length in bytes of text
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
प्रकारनिर्देश संरचना _DYN_STARTUPINFOEXW
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
#यदि defined(FEAT_GUI)
    पूर्णाङ्क		tl_system;	// when non-zero used for :!cmd output
    पूर्णाङ्क		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    पूर्णाङ्क		tl_vterm_size_changed;

    पूर्णाङ्क		tl_normal_mode; // TRUE: Terminal-Normal mode
    पूर्णाङ्क		tl_channel_closing;
    पूर्णाङ्क		tl_channel_closed;
    पूर्णाङ्क		tl_channel_recently_closed; // still need to handle tl_finish

    पूर्णाङ्क		tl_finish;
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

    FILE	*tl_out_fd;
#endif
#यदि defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    पूर्णाङ्क		tl_rows;
    पूर्णाङ्क		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    पूर्णाङ्क		tl_dirty_row_start; // MAX_ROW if nothing dirty
    पूर्णाङ्क		tl_dirty_row_end;   // row below last one to update
    पूर्णाङ्क		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    पूर्णाङ्क		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    पूर्णाङ्क		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    पूर्णाङ्क		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    पूर्णाङ्क		tl_scrollback_snapshot;
    पूर्णाङ्क		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_शीर्ष_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    पूर्णाङ्क		tl_cursor_visible;
    पूर्णाङ्क		tl_cursor_blink;
    पूर्णाङ्क		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    पूर्णाङ्क		tl_using_altscreen;
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
    प्रति ((term) = first_term; (term) != रिक्त; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
स्थिर पूर्णाङ्क term_and_job_init(term_T *term, typval_T *argvar, वर्ण **argv, jobopt_T *opt, jobopt_T *orig_opt);
स्थिर पूर्णाङ्क create_pty_only(term_T *term, jobopt_T *opt);
स्थिर शून्य term_report_winsize(term_T *term, पूर्णाङ्क rows, पूर्णाङ्क cols);
स्थिर शून्य term_free_vterm(term_T *term);
#ifdef FEAT_GUI
स्थिर शून्य update_system_term(term_T *term);
#endif

स्थिर शून्य handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
स्थिर पूर्णाङ्क term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
स्थिर char_u	*last_set_cursor_color = रिक्त;
स्थिर char_u	*desired_cursor_color = रिक्त;
स्थिर पूर्णाङ्क	last_set_cursor_shape = -1;
स्थिर पूर्णाङ्क	desired_cursor_shape = -1;
स्थिर पूर्णाङ्क	last_set_cursor_blink = -1;
स्थिर पूर्णाङ्क	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    स्थिर पूर्णाङ्क
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    यदि (lhs_color != रिक्त && rhs_color != रिक्त)
	प्रत्यागम STRCMP(lhs_color, rhs_color) == 0;
    प्रत्यागम lhs_color == रिक्त && rhs_color == रिक्त;
}

    स्थिर शून्य
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    यदि (cursor_color_equal(*to_color, from_color))
	प्रत्यागम;
    vim_free(*to_color);
    *to_color = (from_color == रिक्त) ? रिक्त : vim_strsave(from_color);
}

    स्थिर char_u *
cursor_color_get(char_u *color)
{
    प्रत्यागम (color == रिक्त) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    स्थिर पूर्णाङ्क
parse_termwinsize(win_T *wp, पूर्णाङ्क *rows, पूर्णाङ्क *cols)
{
    पूर्णाङ्क	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    यदि (*wp->w_p_tws == NUL)
	प्रत्यागम FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    यदि (p == रिक्त)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((वर्ण *)wp->w_p_tws);
    *cols = atoi((वर्ण *)p + 1);
    यदि (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    यदि (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    प्रत्यागम minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    स्थिर शून्य
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    पूर्णाङ्क rows, cols;
    पूर्णाङ्क	minsize;

#ifdef FEAT_GUI
    यदि (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	प्रत्यागम;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    यदि (minsize)
    {
	यदि (term->tl_rows < rows)
	    term->tl_rows = rows;
	यदि (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    यदि ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    अन्यथा यदि (rows != 0)
	term->tl_rows = rows;
    यदि ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    अन्यथा यदि (cols != 0)
	term->tl_cols = cols;

    यदि (!opt->jo_hidden)
    {
	यदि (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	यदि (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	यदि (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((वर्ण *)buf, 100, "%dx%d",
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
setup_job_options(jobopt_T *opt, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    यदि (!(opt->jo_set & JO_OUT_IO))
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
    यदि (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    यदि ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    यदि ((opt->jo_set2 & JO2_TERM_COLS) == 0)
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
    यदि (old_curbuf != रिक्त)
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
	वर्ण	    **argv,
	jobopt_T    *opt,
	पूर्णाङ्क	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = रिक्त;
    पूर्णाङ्क		res;
    buf_T	*newbuf;
    पूर्णाङ्क		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    यदि (check_restricted() || check_secure())
	प्रत्यागम रिक्त;
    यदि (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_ओळ_window));
	प्रत्यागम रिक्त;
    }

    यदि ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != रिक्त
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != रिक्त
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम रिक्त;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    यदि (term == रिक्त)
	प्रत्यागम रिक्त;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, परिमाण(sb_ओळ_T), 300);
    ga_init2(&term->tl_scrollback_postponed, परिमाण(sb_ओळ_T), 300);
    ga_init2(&term->tl_osc_buf, परिमाण(वर्ण), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    यदि (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	यदि (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    प्रत्यागम रिक्त;
	}
	यदि (do_ecmd(0, रिक्त, रिक्त, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    प्रत्यागम रिक्त;
	}
    }
    अन्यथा यदि (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", रिक्त, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	यदि (buf == रिक्त || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    प्रत्यागम रिक्त;
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
	यदि (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_मोजणी = 1;
	}
	यदि (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_मोजणी = 1;
	}

	पूर्णाङ्क cmod_split_modified = FALSE;
	यदि (vertical)
	{
	    यदि (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	यदि (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	यदि (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    प्रत्यागम रिक्त;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    यदि (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	यदि (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	यदि (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, रिक्त, रिक्त, FALSE, curbuf);

    यदि (opt->jo_term_name != रिक्त)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    अन्यथा यदि (argv != रिक्त)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    अन्यथा
    {
	पूर्णाङ्क	i;
	size_t	len;
	char_u	*cmd, *p;

	यदि (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    यदि (cmd == रिक्त)
		cmd = (char_u *)"";
	    अन्यथा यदि (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	अन्यथा यदि (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == रिक्त
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == रिक्त)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	प्रति (i = 0; p != रिक्त; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    यदि (i == 0)
		vim_snprintf((वर्ण *)p, len, "!%s", cmd);
	    अन्यथा
		vim_snprintf((वर्ण *)p, len, "!%s (%d)", cmd, i);
	    यदि (buflist_findname(p) == रिक्त)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		भङ्ग;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, रिक्त, रिक्त, FALSE, curbuf);

    यदि (opt->jo_term_opencmd != रिक्त)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    यदि (opt->jo_eof_chars != रिक्त)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, परिमाण(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    यदि (flags & TERM_START_NOJOB)
	प्रत्यागम curbuf;

#यदि defined(FEAT_SESSION)
    // Remember the command for the session file.
    यदि (opt->jo_term_norestore || argv != रिक्त)
	term->tl_command = vim_strsave((char_u *)"NONE");
    अन्यथा यदि (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	यदि (cmd != रिक्त && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    अन्यथा यदि (argvar->v_type == VAR_LIST
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

	    यदि (s == रिक्त)
		भङ्ग;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    यदि (p == रिक्त)
		भङ्ग;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	यदि (item == रिक्त)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	अन्यथा
	    ga_clear(&ga);
    }
#endif

    यदि (opt->jo_term_kill != रिक्त)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    यदि (opt->jo_term_api != रिक्त)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    अन्यथा
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    यदि (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    यदि (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	यदि (term->tl_palette == रिक्त)
	{
	    vim_free(term);
	    प्रत्यागम रिक्त;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, परिमाण(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    यदि (argv == रिक्त
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != रिक्त
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    अन्यथा
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    यदि (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	यदि (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = संदेश_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	यदि (old_curbuf != रिक्त)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	अन्यथा यदि (vgetc_busy
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
	प्रत्यागम रिक्त;
    }

    apply_autocmds(EVENT_TERMINALOPEN, रिक्त, रिक्त, FALSE, newbuf);
    यदि (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, रिक्त, रिक्त, FALSE, newbuf);
    प्रत्यागम newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    शून्य
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    पूर्णाङ्क		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = रिक्त;
    पूर्णाङ्क		scroll_save = संदेश_scroll;

    संदेश_scroll = FALSE;		// don't scroll here
    autowrite_all();
    संदेश_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    यावत् (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	यदि (ep != रिक्त)
	{
	    यदि (ep < p)
		p = ep;
	    अन्यथा
		ep = रिक्त;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((पूर्णाङ्क)(p - cmd) == परिमाण(name) - 1 \
				 && STRNICMP(cmd, name, परिमाण(name) - 1) == 0)
	यदि (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	अन्यथा यदि (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	अन्यथा यदि (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	अन्यथा यदि (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	अन्यथा यदि (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	अन्यथा यदि (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	अन्यथा यदि (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	अन्यथा यदि (OPTARG_HAS("kill") && ep != रिक्त)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	अन्यथा यदि (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    यदि (ep != रिक्त)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    अन्यथा
		opt.jo_term_api = रिक्त;
	}
	अन्यथा यदि (OPTARG_HAS("rows") && ep != रिक्त && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((वर्ण *)ep + 1);
	    p = skiptowhite(cmd);
	}
	अन्यथा यदि (OPTARG_HAS("cols") && ep != रिक्त && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((वर्ण *)ep + 1);
	    p = skiptowhite(cmd);
	}
	अन्यथा यदि (OPTARG_HAS("eof") && ep != रिक्त)
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
	अन्यथा यदि ((पूर्णाङ्क)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != रिक्त)
	{
	    पूर्णाङ्क tty_type = NUL;

	    p = skiptowhite(cmd);
	    यदि (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    अन्यथा यदि (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    अन्यथा
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str), "type");
		गच्छ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	अन्यथा
	{
	    यदि (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    गच्छ theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    यदि (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	यदि (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    यदि (eap->addr_मोजणी > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_शीर्ष | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_शीर्ष = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    यदि (opt_shell && tofree == रिक्त)
    {
#ifdef UNIX
	वर्ण	**argv = रिक्त;
	char_u	*tofree1 = रिक्त;
	char_u	*tofree2 = रिक्त;

	// :term ++shell command
	यदि (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(रिक्त, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	गच्छ theend;
#अन्यथा
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	यदि (newcmd == रिक्त)
	    गच्छ theend;
	tofree = newcmd;
	vim_snprintf((वर्ण *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# अन्यथा
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	गच्छ theend;
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
get_terminalopt_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    // Note: Keep this in sync with ex_terminal.
    स्थिर वर्ण *(p_termopt_मूल्ये[]) =
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

    यदि (idx < (पूर्णाङ्क)ARRAY_LENGTH(p_termopt_मूल्ये))
	प्रत्यागम (char_u*)p_termopt_मूल्ये[idx];
    प्रत्यागम रिक्त;
}

    स्थिर char_u *
get_termkill_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    स्थिर वर्ण *(p_termkill_मूल्ये[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    यदि (idx < (पूर्णाङ्क)ARRAY_LENGTH(p_termkill_मूल्ये))
	प्रत्यागम (char_u*)p_termkill_मूल्ये[idx];
    प्रत्यागम रिक्त;
}

/*
 * Command-line expansion for :terminal [options]
 */
    पूर्णाङ्क
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	पूर्णाङ्क	    *अंक_Matches)
{
    यदि (xp->xp_pattern > xp->xp_ओळ && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, पूर्णाङ्क) = रिक्त;

	char_u *name_end = xp->xp_pattern - 1;
	यदि (name_end - xp->xp_ओळ >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	यदि (cb != रिक्त)
	{
	    प्रत्यागम ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    अंक_Matches,
		    cb,
		    FALSE);
	}
	प्रत्यागम FAIL;
    }
    प्रत्यागम ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    अंक_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#यदि defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    पूर्णाङ्क
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ध्रुव पूर्णाङ्क	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    यदि (terminal_bufs != रिक्त && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	वर्ण	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, परिमाण(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	यदि (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    यदि (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		प्रत्यागम FAIL;
	    प्रत्यागम put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    यदि (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	प्रत्यागम FAIL;
# ifdef MSWIN
    यदि (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	प्रत्यागम FAIL;
# endif
    यदि (term->tl_command != रिक्त)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	यदि (quoted_command == रिक्त)
	    प्रत्यागम FAIL;

	पूर्णाङ्क ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((वर्ण *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	यदि (ret)
	    प्रत्यागम FAIL;
    }
    यदि (put_eol(fd) != OK)
	प्रत्यागम FAIL;

    यदि (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	प्रत्यागम FAIL;

    यदि (terminal_bufs != रिक्त && wp->w_buffer->b_nwindows > 1)
    {
	वर्ण *hash_किल्ली = alloc(NUMBUFLEN);

	vim_snprintf(hash_किल्ली, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_किल्ली, "terminal session");
    }

    प्रत्यागम put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    पूर्णाङ्क
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    प्रत्यागम term != रिक्त && (term->tl_command == रिक्त
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    स्थिर शून्य
free_scrollback(term_T *term)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_ओळ_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    प्रति (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
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

    यदि (term == रिक्त)
	प्रत्यागम;

    // Unlink the terminal form the list of terminals.
    यदि (first_term == term)
	first_term = term->tl_next;
    अन्यथा
	प्रति (tp = first_term; tp->tl_next != रिक्त; tp = tp->tl_next)
	    यदि (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		भङ्ग;
	    }

    यदि (term->tl_job != रिक्त)
    {
	यदि (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, रिक्त, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = रिक्त;
    यदि (in_terminal_loop == term)
	in_terminal_loop = रिक्त;
}

    शून्य
free_unused_terminals(शून्य)
{
    यावत् (terminals_to_free != रिक्त)
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
	यदि (term->tl_out_fd != रिक्त)
	    fclose(term->tl_out_fd);
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
    पूर्णाङ्क		i;

    प्रति (i = 0; i < 3; ++i)
    {
	पूर्णाङ्क fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	यदि (mch_isatty(fd))
	    प्रत्यागम parts[i];
    }
#endif
    प्रत्यागम PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    स्थिर शून्य
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    वर्ण   buf[किल्ली_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, किल्ली_BUF_LEN);

    यदि (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (पूर्णाङ्क)curlen, रिक्त);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    स्थिर शून्य
term_write_job_output(term_T *term, char_u *संदेश_arg, size_t len_arg)
{
    char_u	*संदेश = संदेश_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    यदि (len > limit)
    {
	char_u *p = संदेश + len - limit;

	p -= (*mb_head_off)(संदेश, p);
	len -= p - संदेश;
	संदेश = p;
    }

    vterm_आदान_write(vterm, (वर्ण *)संदेश, len);

    // flush vterm buffer when vterm responded to control sequence
    यदि (prevlen != vterm_output_get_buffer_current(vterm))
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
    यदि (popup_is_popup(wp))
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
update_cursor(term_T *term, पूर्णाङ्क redraw)
{
    यदि (term->tl_normal_mode)
	प्रत्यागम;
#ifdef FEAT_GUI
    यदि (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    अन्यथा
#endif
    यदि (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    अन्यथा
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    यदि (redraw)
    {
	aco_save_T	aco;

	यदि (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	यदि (gui.in_use)
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
scrollbackline_pos_in_buf(term_T *term, पूर्णाङ्क row, linenr_T *lnum, पूर्णाङ्क *start_col, size_t *start_pos)
{
    sb_ओळ_T	*ओळी = (sb_ओळ_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    पूर्णाङ्क         calc_col = 0;
    पूर्णाङ्क         i;

    यदि (row < 0 || row >= term->tl_scrollback.ga_len)
	प्रत्यागम;

    यदि (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	प्रति (i = term->tl_scrollback_scrolled + 1; i > 0 && ओळी[i].continuation; --i)
	{
	    calc_pos += ओळी[i - 1].sb_bytes;
	    calc_col += ओळी[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	यदि (ओळी[i].continuation)
	    ++i;
    }
    अन्यथा
    {
	i = 1;
	calc_lnum = 1;
    }

    प्रति (; i <= row; ++i)
    {
	यदि (!ओळी[i].continuation)
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
    यदि (start_col)
	*start_col = calc_col;
    यदि (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    स्थिर शून्य
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, पूर्णाङ्क col, पूर्णाङ्क *row, पूर्णाङ्क *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_ओळ_T	*ओळी = (sb_ओळ_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    पूर्णाङ्क         calc_col = col;
    linenr_T    l;

    यदि (lnum > buf->b_ml.ml_ओळ_मोजणी)
	प्रत्यागम;

    यदि (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	यावत् (calc_row < term->tl_scrollback.ga_len && ओळी[calc_row].continuation)
	    ++calc_row;
    }
    अन्यथा
    {
	calc_row = 0;
	l = 1;
    }

    यावत् (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	यदि (!ओळी[calc_row].continuation)
	    ++l;
    }

    यावत् (calc_row + 1 < term->tl_scrollback.ga_len && ओळी[calc_row + 1].continuation
	    && calc_col >= ओळी[calc_row].sb_cols)
    {
	calc_col -= ओळी[calc_row].sb_cols;
	++calc_row;
    }

    यदि (row)
	*row = calc_row;
    यदि (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    शून्य
write_to_term(buf_T *buffer, char_u *संदेश, channel_T *channel)
{
    size_t	len = STRLEN(संदेश);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    यदि (term->tl_out_fd != रिक्त)
    {
	ch_log(channel, "Writing %d bytes to output file", (पूर्णाङ्क)len);
	fwrite(संदेश, len, 1, term->tl_out_fd);
	प्रत्यागम;
    }
#endif

    यदि (term->tl_vterm == रिक्त)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (पूर्णाङ्क)len);
	प्रत्यागम;
    }
    ch_log(channel, "writing %d bytes to terminal", (पूर्णाङ्क)len);
    cursor_off();
    term_write_job_output(term, संदेश, len);

#ifdef FEAT_GUI
    यदि (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    अन्यथा
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    यदि (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	यदि (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#यदि defined(FEAT_TABPANEL)
	    यदि (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    यदि (buffer == curbuf && curbuf->b_term != रिक्त)
		update_cursor(curbuf->b_term, TRUE);
	}
	अन्यथा
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    स्थिर पूर्णाङ्क
term_send_mouse(VTerm *vterm, पूर्णाङ्क button, पूर्णाङ्क pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    पूर्णाङ्क		    row = mouse_row - W_WINROW(curwin);
    पूर्णाङ्क		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    यदि (popup_is_popup(curwin))
    {
	row -= popup_शीर्ष_extra(curwin);
	col -= popup_डावा_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    यदि (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    प्रत्यागम TRUE;
}

स्थिर पूर्णाङ्क enter_mouse_col = -1;
स्थिर पूर्णाङ्क enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    स्थिर पूर्णाङ्क
term_mouse_click(VTerm *vterm, पूर्णाङ्क किल्ली)
{
#यदि defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    स्थिर पूर्णाङ्क	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    यदि (state == रिक्त)
	प्रत्यागम FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    यदि (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	विकल्प (किल्ली)
	{
	पक्ष K_LEFTDRAG:
	पक्ष K_LEFTRELEASE:
	पक्ष K_RIGHTDRAG:
	पक्ष K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		यदि (ignore_drag_release)
		{
		    पूर्णाङ्क save_mouse_col, save_mouse_row;

		    यदि (enter_mouse_col < 0)
			भङ्ग;

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
	पक्ष K_LEFTMOUSE:
	पक्ष K_RIGHTMOUSE:
		यदि (किल्ली == K_LEFTRELEASE || किल्ली == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		अन्यथा
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		यदि (clip_star.available)
		{
		    पूर्णाङ्क	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(किल्ली),
							 &is_click, &is_drag);
		    यदि (mouse_model_popup() && button == MOUSE_डावा
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_उजवा;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		भङ्ग;

	पक्ष K_MIDDLEMOUSE:
		यदि (clip_star.available)
		    घाला_reg('*', TRUE);
		भङ्ग;
	}
	enter_mouse_col = -1;
	प्रत्यागम FALSE;
    }
#endif
    enter_mouse_col = -1;

    विकल्प (किल्ली)
    {
	पक्ष K_LEFTMOUSE:
	पक्ष K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); भङ्ग;
	पक्ष K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); भङ्ग;
	पक्ष K_LEFTRELEASE:
	पक्ष K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); भङ्ग;
	पक्ष K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); भङ्ग;
	पक्ष K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); भङ्ग;
	पक्ष K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); भङ्ग;
	पक्ष K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); भङ्ग;
	पक्ष K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); भङ्ग;
	पक्ष K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); भङ्ग;
	पक्ष K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); भङ्ग;
    }
    प्रत्यागम TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    स्थिर पूर्णाङ्क
term_convert_किल्ली(term_T *term, पूर्णाङ्क c, पूर्णाङ्क modmask, वर्ण *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_किल्ली	    किल्ली = VTERM_किल्ली_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    पूर्णाङ्क		    other = FALSE;

    विकल्प (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	पक्ष K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    यदि (term->tl_conpty)
		किल्ली = VTERM_किल्ली_BACKSPACE;
	    अन्यथा
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    भङ्ग;

#ifdef MSWIN
	पक्ष BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    यदि (term->tl_conpty)
		किल्ली = VTERM_किल्ली_BACKSPACE;
	    भङ्ग;
#endif

	पक्ष ESC:		किल्ली = VTERM_किल्ली_ESCAPE; भङ्ग;
	पक्ष K_DEL:		किल्ली = VTERM_किल्ली_DEL; भङ्ग;
	पक्ष K_DOWN:		किल्ली = VTERM_किल्ली_DOWN; भङ्ग;
	पक्ष K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_DOWN; भङ्ग;
	पक्ष K_END:		किल्ली = VTERM_किल्ली_END; भङ्ग;
	पक्ष K_S_END:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_END; भङ्ग;
	पक्ष K_C_END:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_END; भङ्ग;
	पक्ष K_F10:		किल्ली = VTERM_किल्ली_FUNCTION(10); भङ्ग;
	पक्ष K_F11:		किल्ली = VTERM_किल्ली_FUNCTION(11); भङ्ग;
	पक्ष K_F12:		किल्ली = VTERM_किल्ली_FUNCTION(12); भङ्ग;
	पक्ष K_F1:		किल्ली = VTERM_किल्ली_FUNCTION(1); भङ्ग;
	पक्ष K_F2:		किल्ली = VTERM_किल्ली_FUNCTION(2); भङ्ग;
	पक्ष K_F3:		किल्ली = VTERM_किल्ली_FUNCTION(3); भङ्ग;
	पक्ष K_F4:		किल्ली = VTERM_किल्ली_FUNCTION(4); भङ्ग;
	पक्ष K_F5:		किल्ली = VTERM_किल्ली_FUNCTION(5); भङ्ग;
	पक्ष K_F6:		किल्ली = VTERM_किल्ली_FUNCTION(6); भङ्ग;
	पक्ष K_F7:		किल्ली = VTERM_किल्ली_FUNCTION(7); भङ्ग;
	पक्ष K_F8:		किल्ली = VTERM_किल्ली_FUNCTION(8); भङ्ग;
	पक्ष K_F9:		किल्ली = VTERM_किल्ली_FUNCTION(9); भङ्ग;
	पक्ष K_HOME:		किल्ली = VTERM_किल्ली_HOME; भङ्ग;
	पक्ष K_S_HOME:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_HOME; भङ्ग;
	पक्ष K_C_HOME:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_HOME; भङ्ग;
	पक्ष K_INS:		किल्ली = VTERM_किल्ली_INS; भङ्ग;
	पक्ष K_K0:		किल्ली = VTERM_किल्ली_KP_0; भङ्ग;
	पक्ष K_K1:		किल्ली = VTERM_किल्ली_KP_1; भङ्ग;
	पक्ष K_K2:		किल्ली = VTERM_किल्ली_KP_2; भङ्ग;
	पक्ष K_K3:		किल्ली = VTERM_किल्ली_KP_3; भङ्ग;
	पक्ष K_K4:		किल्ली = VTERM_किल्ली_KP_4; भङ्ग;
	पक्ष K_K5:		किल्ली = VTERM_किल्ली_KP_5; भङ्ग;
	पक्ष K_K6:		किल्ली = VTERM_किल्ली_KP_6; भङ्ग;
	पक्ष K_K7:		किल्ली = VTERM_किल्ली_KP_7; भङ्ग;
	पक्ष K_K8:		किल्ली = VTERM_किल्ली_KP_8; भङ्ग;
	पक्ष K_K9:		किल्ली = VTERM_किल्ली_KP_9; भङ्ग;
	पक्ष K_KDEL:		किल्ली = VTERM_किल्ली_DEL; भङ्ग; // TODO
	पक्ष K_KDIVIDE:		किल्ली = VTERM_किल्ली_KP_DIVIDE; भङ्ग;
	पक्ष K_KEND:		किल्ली = VTERM_किल्ली_KP_1; भङ्ग; // TODO
	पक्ष K_KENTER:		किल्ली = VTERM_किल्ली_KP_ENTER; भङ्ग;
	पक्ष K_KHOME:		किल्ली = VTERM_किल्ली_KP_7; भङ्ग; // TODO
	पक्ष K_KINS:		किल्ली = VTERM_किल्ली_KP_0; भङ्ग; // TODO
	पक्ष K_KMINUS:		किल्ली = VTERM_किल्ली_KP_MINUS; भङ्ग;
	पक्ष K_KMULTIPLY:	किल्ली = VTERM_किल्ली_KP_MULT; भङ्ग;
	पक्ष K_KPAGEDOWN:	किल्ली = VTERM_किल्ली_KP_3; भङ्ग; // TODO
	पक्ष K_KPAGEUP:		किल्ली = VTERM_किल्ली_KP_9; भङ्ग; // TODO
	पक्ष K_KPLUS:		किल्ली = VTERM_किल्ली_KP_PLUS; भङ्ग;
	पक्ष K_KPOINT:		किल्ली = VTERM_किल्ली_KP_PERIOD; भङ्ग;
	पक्ष K_डावा:		किल्ली = VTERM_किल्ली_डावा; भङ्ग;
	पक्ष K_S_डावा:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_डावा; भङ्ग;
	पक्ष K_C_डावा:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_डावा; भङ्ग;
	पक्ष K_PAGEDOWN:	किल्ली = VTERM_किल्ली_PAGEDOWN; भङ्ग;
	पक्ष K_PAGEUP:		किल्ली = VTERM_किल्ली_PAGEUP; भङ्ग;
	पक्ष K_उजवा:		किल्ली = VTERM_किल्ली_उजवा; भङ्ग;
	पक्ष K_S_उजवा:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_उजवा; भङ्ग;
	पक्ष K_C_उजवा:		mod = VTERM_MOD_CTRL;
				किल्ली = VTERM_किल्ली_उजवा; भङ्ग;
	पक्ष K_UP:		किल्ली = VTERM_किल्ली_UP; भङ्ग;
	पक्ष K_S_UP:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_UP; भङ्ग;
	पक्ष TAB:		किल्ली = VTERM_किल्ली_TAB; भङ्ग;
	पक्ष K_S_TAB:		mod = VTERM_MOD_SHIFT;
				किल्ली = VTERM_किल्ली_TAB; भङ्ग;

	पक्ष K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); भङ्ग;
	पक्ष K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); भङ्ग;
	पक्ष K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); भङ्ग;
	पक्ष K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); भङ्ग;

	पक्ष K_LEFTMOUSE:
	पक्ष K_LEFTMOUSE_NM:
	पक्ष K_LEFTDRAG:
	पक्ष K_LEFTRELEASE:
	पक्ष K_LEFTRELEASE_NM:
	पक्ष K_MOUSEMOVE:
	पक्ष K_MIDDLEMOUSE:
	पक्ष K_MIDDLEDRAG:
	पक्ष K_MIDDLERELEASE:
	पक्ष K_RIGHTMOUSE:
	पक्ष K_RIGHTDRAG:
	पक्ष K_RIGHTRELEASE:	यदि (!term_mouse_click(vterm, c))
				    प्रत्यागम 0;
				other = TRUE;
				भङ्ग;

	पक्ष K_X1MOUSE:		/* TODO */ प्रत्यागम 0;
	पक्ष K_X1DRAG:		/* TODO */ प्रत्यागम 0;
	पक्ष K_X1RELEASE:	/* TODO */ प्रत्यागम 0;
	पक्ष K_X2MOUSE:		/* TODO */ प्रत्यागम 0;
	पक्ष K_X2DRAG:		/* TODO */ प्रत्यागम 0;
	पक्ष K_X2RELEASE:	/* TODO */ प्रत्यागम 0;

	पक्ष K_IGNORE:		प्रत्यागम 0;
	पक्ष K_NOP:		प्रत्यागम 0;
	पक्ष K_UNDO:		प्रत्यागम 0;
	पक्ष K_HELP:		प्रत्यागम 0;
	पक्ष K_XF1:		किल्ली = VTERM_किल्ली_FUNCTION(1); भङ्ग;
	पक्ष K_XF2:		किल्ली = VTERM_किल्ली_FUNCTION(2); भङ्ग;
	पक्ष K_XF3:		किल्ली = VTERM_किल्ली_FUNCTION(3); भङ्ग;
	पक्ष K_XF4:		किल्ली = VTERM_किल्ली_FUNCTION(4); भङ्ग;
	पक्ष K_SELECT:		प्रत्यागम 0;
#ifdef FEAT_GUI
	पक्ष K_VER_SCROLLBAR:	प्रत्यागम 0;
	पक्ष K_HOR_SCROLLBAR:	प्रत्यागम 0;
#endif
#ifdef FEAT_GUI_TABLINE
	पक्ष K_TABLINE:		प्रत्यागम 0;
	पक्ष K_TABMENU:		प्रत्यागम 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	पक्ष K_F21:		किल्ली = VTERM_किल्ली_FUNCTION(21); भङ्ग;
#endif
#ifdef FEAT_DND
	पक्ष K_DROP:		प्रत्यागम 0;
#endif
	पक्ष K_CURSORHOLD:	प्रत्यागम 0;
	पक्ष K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				भङ्ग;
	पक्ष K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				भङ्ग;
    }

    // add modifiers for the typed key
    यदि (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    यदि (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    यदि (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    यदि (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    यदि (किल्ली != VTERM_किल्ली_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_किल्ली(vterm, किल्ली, mod);
    अन्यथा यदि (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    प्रत्यागम (पूर्णाङ्क)vterm_output_read(vterm, buf, किल्ली_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    स्थिर पूर्णाङ्क
term_job_running_check(term_T *term, पूर्णाङ्क check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    यदि (term == रिक्त
	|| term->tl_job == रिक्त
	|| !channel_is_open(term->tl_job->jv_channel))
	प्रत्यागम FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    यदि (check_job_status)
	job_status(job);
    प्रत्यागम (job->jv_status == JOB_STARTED
	    || (job->jv_channel != रिक्त && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    पूर्णाङ्क
term_job_running(term_T *term)
{
    प्रत्यागम term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    पूर्णाङ्क
term_job_running_not_none(term_T *term)
{
    प्रत्यागम term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    पूर्णाङ्क
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    प्रत्यागम term != रिक्त
	&& term->tl_job != रिक्त
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    पूर्णाङ्क
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_संदेश_SIZE];
    पूर्णाङ्क	ret;

    dialog_संदेश(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 1);
    यदि (ret == VIM_YES)
	प्रत्यागम OK;
    अन्यथा
	प्रत्यागम FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    पूर्णाङ्क
term_try_stop_job(buf_T *buf)
{
    पूर्णाङ्क	    मोजणी;
    वर्ण    *how = (वर्ण *)buf->b_term->tl_kill;

#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    यदि ((how == रिक्त || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	यदि (term_confirm_stop(buf) == OK)
	    how = "kill";
	अन्यथा
	    प्रत्यागम FAIL;
    }
#endif
    यदि (how == रिक्त || *how == NUL)
	प्रत्यागम FAIL;

    job_stop(buf->b_term->tl_job, रिक्त, how);

    // wait for up to a second for the job to die
    प्रति (मोजणी = 0; मोजणी < 100; ++मोजणी)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	यदि (!buf_valid(buf)
		|| buf->b_term == रिक्त
		|| buf->b_term->tl_job == रिक्त)
	    प्रत्यागम OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	यदि (job->jv_status >= JOB_ENDED)
	    प्रत्यागम OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    प्रत्यागम FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    स्थिर शून्य
add_scrollback_ओळ_to_buffer(term_T *term, char_u *text, पूर्णाङ्क len, पूर्णाङ्क append)
{
    buf_T	*buf = term->tl_buffer;
    पूर्णाङ्क		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_ओळ_मोजणी;

#ifdef MSWIN
    char_u *tmp = text;
    यदि (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = रिक्त;
	पूर्णाङ्क	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (वर्ण*)text, len + 1,
							   &ret, &length);
	यदि (ret != रिक्त)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (वर्ण **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    यदि (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	यदि (both == रिक्त)
	    प्रत्यागम;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    अन्यथा
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    यदि (tmp != text)
	vim_free(text);
#endif
    यदि (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    स्थिर शून्य
cell2cellattr(ध्रुव VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    स्थिर पूर्णाङ्क
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    प्रत्यागम a->fg.red == b->fg.red
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
    स्थिर पूर्णाङ्क
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, पूर्णाङ्क lnum)
{
    यदि (ga_grow(&term->tl_scrollback, 1) == FAIL)
	प्रत्यागम FALSE;

    sb_ओळ_T *ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    यदि (lnum > 0)
    {
	पूर्णाङ्क i;

	प्रति (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
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
    प्रत्यागम OK;
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
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ओळ_मोजणी, FALSE);
    bufline_length = STRLEN(bufline);
    यावत् (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	ओळ = (sb_ओळ_T *)gap->ga_data + gap->ga_len - 1;
	यदि (ओळ->sb_bytes < 0 || (size_t)ओळ->sb_bytes > bufline_length)
	    भङ्ग;
	bufline_length -= ओळ->sb_bytes;
	यदि (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_ओळ_मोजणी);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ओळ_मोजणी, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(ओळ->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    यदि (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_ओळ_मोजणी, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    यदि (curbuf == term->tl_buffer)
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
    पूर्णाङ्क		    len;
    पूर्णाङ्क		    ओळी_skipped = 0;
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
    प्रति (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	प्रति (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    यदि (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    अन्यथा
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	यदि (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++ओळी_skipped;
	अन्यथा
	{
	    यावत् (ओळी_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--ओळी_skipped;
		यदि (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_ओळ_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    यदि (len == 0)
		p = रिक्त;
	    अन्यथा
		p = ALLOC_MULT(cellattr_T, len);
	    यदि ((p != रिक्त || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		पूर्णाङ्क	    width;
		ध्रुव VTerm_ओळ_Info *lineinfo;
		sb_ओळ_T   *ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		प्रति (pos.col = 0; pos.col < len; pos.col += width)
		{
		    यदि (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			यदि (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    अन्यथा
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			यदि (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			यदि (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    पूर्णाङ्क	    i;
			    पूर्णाङ्क	    c;

			    प्रति (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
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

		यदि (ga_grow(&ga, 1) == FAIL)
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
    प्रति (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	यदि (add_empty_scrollback(term, &fill_attr, 0) == OK)
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
    स्थिर पूर्णाङ्क
for_all_windows_and_curwin(win_T **wp, पूर्णाङ्क *did_curwin)
{
    यदि (*wp == रिक्त)
	*wp = firstwin;
    अन्यथा यदि ((*wp)->w_next != रिक्त)
	*wp = (*wp)->w_next;
    अन्यथा यदि (!*did_curwin)
	*wp = curwin;
    अन्यथा
	प्रत्यागम FALSE;
    यदि (*wp == curwin)
	*did_curwin = TRUE;
    प्रत्यागम TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    शून्य
may_move_terminal_to_buffer(term_T *term, पूर्णाङ्क redraw)
{
    यदि (term->tl_vterm == रिक्त)
	प्रत्यागम;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    यदि (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_ओळ_मोजणी
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    यदि (redraw)
    {
	win_T	    *wp = रिक्त;
	पूर्णाङ्क	    did_curwin = FALSE;

	यावत् (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    यदि (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_ओळ_मोजणी;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		यदि (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    यदि (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#यदि defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    पूर्णाङ्क
term_check_timers(पूर्णाङ्क next_due_arg, proftime_T *now)
{
    term_T  *term;
    पूर्णाङ्क	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	यदि (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_डावा(&term->tl_timer_due, now);

	    यदि (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    अन्यथा यदि (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    प्रत्यागम next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    स्थिर शून्य
set_terminal_mode(term_T *term, पूर्णाङ्क normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    यदि (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    यदि (term->tl_buffer == curbuf)
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
    यदि (term->tl_finish != TL_FINISH_CLOSE)
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
    पूर्णाङ्क        col;

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
    यदि (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    पूर्णाङ्क
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    प्रत्यागम term != रिक्त && term->tl_normal_mode;
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

    यदि (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    यदि (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
प्रकारनिर्देश गणना {
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
    स्थिर पूर्णाङ्क
vterm_using_किल्ली_protocol(शून्य)
{
    प्रत्यागम curbuf->b_term != रिक्त
	&& curbuf->b_term->tl_vterm != रिक्त
	&& (vterm_is_modify_other_किल्ल्या(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    शून्य
check_no_reduce_किल्ल्या(शून्य)
{
    यदि (no_reduce_किल्ली_state != NRKS_CHECK
	    || no_reduce_किल्ल्या >= 1
	    || curbuf->b_term == रिक्त
	    || curbuf->b_term->tl_vterm == रिक्त)
	प्रत्यागम;

    यदि (vterm_using_किल्ली_protocol())
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
    स्थिर पूर्णाङ्क
term_vgetc(शून्य)
{
    पूर्णाङ्क c;
    पूर्णाङ्क save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    यदि (vterm_using_किल्ली_protocol())
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

    यदि (no_reduce_किल्ली_state == NRKS_SET)
	--no_reduce_किल्ल्या;
    no_reduce_किल्ली_state = NRKS_NONE;

    प्रत्यागम c;
}

स्थिर पूर्णाङ्क	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    पूर्णाङ्क
send_किल्ल्या_to_term(term_T *term, पूर्णाङ्क c, पूर्णाङ्क modmask, पूर्णाङ्क typed)
{
    वर्ण	संदेश[किल्ली_BUF_LEN];
    size_t	len;
    पूर्णाङ्क		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    विकल्प (c)
    {
	पक्ष NUL:
	पक्ष K_ZERO:
	    यदि (typed)
		stuffcharReadbuff(c);
	    प्रत्यागम FAIL;

	पक्ष K_TABLINE:
	    stuffcharReadbuff(c);
	    प्रत्यागम FAIL;

	पक्ष K_IGNORE:
	पक्ष K_CANCEL:  // used for :normal when running out of chars
	    प्रत्यागम FAIL;

	पक्ष K_LEFTDRAG:
	पक्ष K_MIDDLEDRAG:
	पक्ष K_RIGHTDRAG:
	पक्ष K_X1DRAG:
	पक्ष K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	पक्ष K_LEFTMOUSE:
	पक्ष K_LEFTMOUSE_NM:
	पक्ष K_LEFTRELEASE:
	पक्ष K_LEFTRELEASE_NM:
	पक्ष K_MOUSEMOVE:
	पक्ष K_MIDDLEMOUSE:
	पक्ष K_MIDDLERELEASE:
	पक्ष K_RIGHTMOUSE:
	पक्ष K_RIGHTRELEASE:
	पक्ष K_X1MOUSE:
	पक्ष K_X1RELEASE:
	पक्ष K_X2MOUSE:
	पक्ष K_X2RELEASE:

	पक्ष K_MOUSEUP:
	पक्ष K_MOUSEDOWN:
	पक्ष K_MOUSELEFT:
	पक्ष K_MOUSERIGHT:
	    {
		पूर्णाङ्क	row = mouse_row;
		पूर्णाङ्क	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		यदि (popup_is_popup(curwin))
		{
		    row -= popup_शीर्ष_extra(curwin);
		    col -= popup_डावा_extra(curwin);
		}
#endif
		यदि (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    यदि (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    प्रत्यागम FAIL;
		}
	    }
	    भङ्ग;

	पक्ष K_COMMAND:
	पक्ष K_SCRIPT_COMMAND:
	    प्रत्यागम do_cmdkey_command(c, 0);
    }
    यदि (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_किल्ली(term, c, modmask, संदेश);
    यदि (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)संदेश, (पूर्णाङ्क)len, रिक्त);

    प्रत्यागम OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    स्थिर शून्य
term_paste_register(पूर्णाङ्क prev_c UNUSED)
{
    पूर्णाङ्क		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    पूर्णाङ्क		type;

    यदि (add_to_showcmd(prev_c))
    यदि (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    यदि (!term_use_loop())
	// job finished while waiting for a character
	प्रत्यागम;

    // CTRL-W "= prompt for expression to evaluate.
    यदि (c == '=' && get_अभिव्यक्ती_register() != '=')
	प्रत्यागम;
    यदि (!term_use_loop())
	// job finished while waiting for a character
	प्रत्यागम;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    यदि (l == रिक्त)
	प्रत्यागम;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_वस्तुसंच(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	यदि (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = रिक्त;
	    पूर्णाङ्क	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (वर्ण *)s,
		    (पूर्णाङ्क)STRLEN(s), &ret, &length);
	    यदि (ret != रिक्त)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (वर्ण **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (पूर्णाङ्क)STRLEN(s), रिक्त);
#ifdef MSWIN
	यदि (tmp != s)
	    vim_free(s);
#endif

	यदि (item->li_next != रिक्त || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, रिक्त);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    पूर्णाङ्क
terminal_is_active(शून्य)
{
    प्रत्यागम in_terminal_loop != रिक्त;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    स्थिर पूर्णाङ्क
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    यदि (wp != रिक्त && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    अन्यथा यदि (term->tl_highlight_name != रिक्त)
	name = term->tl_highlight_name;
    अन्यथा
	name = (char_u*)"Terminal";

    प्रत्यागम syn_name2id(name);
}

#यदि defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    स्थिर cursorentry_T entry;
    पूर्णाङ्क			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_डावा ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    यदि (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    यदि (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    यदि (term_bg != INVALCOLOR)
	*fg = term_bg;
    अन्यथा
	*fg = gui.back_pixel;

    यदि (term->tl_cursor_color == रिक्त)
    {
	यदि (term_fg != INVALCOLOR)
	    *bg = term_fg;
	अन्यथा
	    *bg = gui.norm_pixel;
    }
    अन्यथा
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    प्रत्यागम &entry;
}
#endif

    स्थिर शून्य
may_output_cursor_props(शून्य)
{
    यदि (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	यदि (desired_cursor_shape == -1 || desired_cursor_blink == -1)
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
    यदि (gui.in_use)
	प्रत्यागम;
#endif
    यदि (in_terminal_loop == term)
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
    यदि (gui.in_use)
	प्रत्यागम;
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
    स्थिर पूर्णाङ्क
term_use_loop_check(पूर्णाङ्क check_job_status)
{
    term_T *term = curbuf->b_term;

    प्रत्यागम term != रिक्त
	&& !term->tl_normal_mode
	&& term->tl_vterm != रिक्त
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    पूर्णाङ्क
term_use_loop(शून्य)
{
    प्रत्यागम term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    शून्य
term_win_entered(शून्य)
{
    term_T *term = curbuf->b_term;

    यदि (term == रिक्त)
	प्रत्यागम;

    यदि (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	यदि (State & MODE_घाला)
	    stop_घाला_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    शून्य
term_focus_change(पूर्णाङ्क in_focus)
{
    term_T *term = curbuf->b_term;

    यदि (term == रिक्त || term->tl_vterm == रिक्त)
	प्रत्यागम;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    यदि (in_focus)
	vterm_state_focus_in(state);
    अन्यथा
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    स्थिर पूर्णाङ्क
raw_c_to_ctrl(पूर्णाङ्क c)
{
    यदि ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	प्रत्यागम c & 0x1f;
    प्रत्यागम c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    स्थिर पूर्णाङ्क
ctrl_to_raw_c(पूर्णाङ्क c)
{
    यदि (c < 0x20 && vterm_using_किल्ली_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	प्रत्यागम c + '@';
    }
    प्रत्यागम c;
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
    पूर्णाङ्क
terminal_loop(पूर्णाङ्क blocking)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		raw_c;
    पूर्णाङ्क		termwinkey = 0;
    पूर्णाङ्क		ret;
#ifdef UNIX
    पूर्णाङ्क		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    पूर्णाङ्क		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    यदि (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_किल्ली(curwin->w_p_twk, TRUE);
	यदि (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    यावत् (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	यदि (curbuf->b_term != रिक्त && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    यावत् (must_redraw != 0)
		यदि (update_screen(0) == FAIL)
		    भङ्ग;
	यदि (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    भङ्ग;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	यदि (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    यदि (raw_c != K_IGNORE)
		vungetc(raw_c);
	    भङ्ग;
	}
	यदि (raw_c == K_IGNORE)
	    अनुवर्त;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	यदि (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    यदि (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	यदि (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	यदि ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    पूर्णाङ्क	    prev_c = c;
	    पूर्णाङ्क	    prev_raw_c = raw_c;
	    पूर्णाङ्क	    prev_mod_mask = mod_mask;

	    यदि (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    यदि (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		भङ्ग;

	    यदि (prev_c == Ctrl_BSL)
	    {
		यदि (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    गच्छ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_किल्ल्या_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    अन्यथा यदि (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    अन्यथा यदि (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    अन्यथा यदि (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    अन्यथा यदि (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		गच्छ theend;
	    }
	    अन्यथा यदि (c == '"')
	    {
		term_paste_register(prev_c);
		अनुवर्त;
	    }
	    अन्यथा यदि (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		गच्छ theend;
	    }
	}
#ifdef MSWIN
	यदि (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    यदि (MultiByteToWideChar(GetACP(), 0, (वर्ण*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	यदि (send_किल्ल्या_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    यदि (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    गच्छ theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = रिक्त;
    यदि (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    यदि (curbuf->b_term != रिक्त && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    प्रत्यागम ret;
}

    स्थिर शून्य
may_toggle_cursor(term_T *term)
{
    यदि (in_terminal_loop != term)
	प्रत्यागम;

    यदि (term->tl_cursor_visible)
	cursor_on();
    अन्यथा
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    स्थिर पूर्णाङ्क
color2index(VTermColor *color, पूर्णाङ्क fg, पूर्णाङ्क *boldp)
{
    पूर्णाङ्क red = color->red;
    पूर्णाङ्क blue = color->blue;
    पूर्णाङ्क green = color->green;

    *boldp = FALSE;

    यदि (VTERM_COLOR_IS_INVALID(color))
	प्रत्यागम 0;

    यदि (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	यदि (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    यदि (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    प्रत्यागम index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	अन्यथा यदि (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    प्रत्यागम (color->index & 7) + 1;
	}
	प्रत्यागम 0;
    }

    यदि (t_colors >= 256)
    {
	यदि (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    स्थिर पूर्णाङ्क cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    पूर्णाङ्क i;

	    यदि (red < 5)
		प्रत्यागम 17; // 00/00/00
	    यदि (red > 245) // ff/ff/ff
		प्रत्यागम 232;
	    प्रति (i = 0; i < 23; ++i)
		यदि (red < cutoff[i])
		    प्रत्यागम i + 233;
	    प्रत्यागम 256;
	}
	{
	    स्थिर पूर्णाङ्क cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    पूर्णाङ्क ri, gi, bi;

	    // 216-color cube
	    प्रति (ri = 0; ri < 5; ++ri)
		यदि (red < cutoff[ri])
		    भङ्ग;
	    प्रति (gi = 0; gi < 5; ++gi)
		यदि (green < cutoff[gi])
		    भङ्ग;
	    प्रति (bi = 0; bi < 5; ++bi)
		यदि (blue < cutoff[bi])
		    भङ्ग;
	    प्रत्यागम 17 + ri * 36 + gi * 6 + bi;
	}
    }
    प्रत्यागम 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    स्थिर पूर्णाङ्क
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    पूर्णाङ्क attr = 0;

    यदि (cellattrs->bold)
	attr |= HL_BOLD;
    यदि (cellattrs->underline)
	attr |= HL_UNDERLINE;
    यदि (cellattrs->italic)
	attr |= HL_ITALIC;
    यदि (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    यदि (cellattrs->reverse)
	attr |= HL_INVERSE;
    प्रत्यागम attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    स्थिर शून्य
hl2vtermAttr(पूर्णाङ्क attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    यदि (attr & HL_BOLD)
	cell->attrs.bold = 1;
    यदि (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    यदि (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    यदि (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    यदि (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    स्थिर पूर्णाङ्क
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    पूर्णाङ्क attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    पूर्णाङ्क is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    पूर्णाङ्क is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    यदि (is_default_fg || is_default_bg)
    {
	यदि (wp != रिक्त && wp->w_hlfwin_id != 0)
	{
	    यदि (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    यदि (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	अन्यथा
	{
	    यदि (is_default_fg)
		fg = &term->tl_default_color.fg;
	    यदि (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    यदि (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	प्रत्यागम get_gui_attr_idx(attr, guifg, guibg);
    }
    अन्यथा
#endif
#ifdef FEAT_TERMGUICOLORS
    यदि (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	प्रत्यागम get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    अन्यथा
#endif
    {
	पूर्णाङ्क bold = MAYBE;
	पूर्णाङ्क ctermfg = color2index(fg, TRUE, &bold);
	पूर्णाङ्क ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	यदि (bold == TRUE)
	    attr |= HL_BOLD;

	प्रत्यागम get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    प्रत्यागम 0;
}

    स्थिर शून्य
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    यदि (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    स्थिर पूर्णाङ्क
handle_damage(VTermRect rect, शून्य *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    प्रत्यागम 1;
}

    स्थिर शून्य
term_scroll_up(term_T *term, पूर्णाङ्क start_row, पूर्णाङ्क मोजणी)
{
    win_T		 *wp = रिक्त;
    पूर्णाङ्क			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    पूर्णाङ्क			 clear_attr;

    CLEAR_FIELD(attr);

    यावत् (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	यदि (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_ओळी(wp, start_row, मोजणी, FALSE, FALSE, clear_attr);
	}
    }
}

    स्थिर पूर्णाङ्क
handle_moverect(VTermRect dest, VTermRect src, शून्य *user)
{
    term_T	*term = (term_T *)user;
    पूर्णाङ्क		मोजणी = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    यदि (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	यदि (dest.start_row == 0)
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
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	पूर्णाङ्क visible,
	शून्य *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = रिक्त;
    पूर्णाङ्क		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    यावत् (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	यदि (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    यदि (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
handle_settermprop(
	VTermProp prop,
	VTerm_मूल्य *मूल्य,
	शून्य *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = रिक्त;

    विकल्प (prop)
    {
	पक्ष VTERM_PROP_TITLE:
	    यदि (disable_vterm_title_for_testing)
		भङ्ग;
	    strval = vim_strnsave((char_u *)मूल्य->string.str,
							    मूल्य->string.len);
	    यदि (strval == रिक्त)
		भङ्ग;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    यदि (*skipwhite(strval) == NUL)
		term->tl_title = रिक्त;
	    // Same as blank
	    अन्यथा यदि (term->tl_arg0_cmd != रिक्त
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (पूर्णाङ्क)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = रिक्त;
	    // Empty corrupted data of winpty
	    अन्यथा यदि (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = रिक्त;
#ifdef MSWIN
	    अन्यथा यदि (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = रिक्त;
		पूर्णाङ्क	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(वर्ण*)मूल्य->string.str,
					(पूर्णाङ्क)मूल्य->string.len, &ret, &length);
		यदि (ret != रिक्त)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (वर्ण**)&term->tl_title,
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
	    यदि (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    भङ्ग;

	पक्ष VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = मूल्य->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    भङ्ग;

	पक्ष VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = मूल्य->boolean;
	    may_set_cursor_props(term);
	    भङ्ग;

	पक्ष VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = मूल्य->संख्या;
	    may_set_cursor_props(term);
	    भङ्ग;

	पक्ष VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)मूल्य->string.str,
							    मूल्य->string.len);
	    यदि (strval == रिक्त)
		भङ्ग;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    भङ्ग;

	पक्ष VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = मूल्य->boolean;
	    भङ्ग;

	स्वभाव:
	    भङ्ग;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    प्रत्यागम 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    स्थिर पूर्णाङ्क
handle_resize(पूर्णाङ्क rows, पूर्णाङ्क cols, शून्य *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    यदि (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    अन्यथा
    {
	FOR_ALL_WINDOWS(wp)
	{
	    यदि (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    प्रत्यागम 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    स्थिर शून्य
limit_scrollback(term_T *term, garray_T *gap, पूर्णाङ्क update_buffer)
{
    यदि (gap->ga_len < term->tl_buffer->b_p_twsl)
	प्रत्यागम;

    पूर्णाङ्क	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    पूर्णाङ्क	i;
    sb_ओळ_T *sb_ओळी = (sb_ओळ_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    प्रति (i = 0; i < todo; ++i)
    {
	यदि (update_buffer && (!sb_ओळी[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_ओळी[i].sb_cells);
    }
    // Continue until end of wrapped line
    प्रति (; todo < gap->ga_len && sb_ओळी[todo].continuation; ++todo)
	vim_free(sb_ओळी[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_ओळ_T *)gap->ga_data + todo,
		परिमाण(sb_ओळ_T) * gap->ga_len);
    यदि (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = रिक्त;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    यदि (wp->w_buffer == term->tl_buffer)
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
    स्थिर पूर्णाङ्क
handle_pushline(पूर्णाङ्क cols, ध्रुव VTermScreenCell *cells, पूर्णाङ्क continuation, शून्य *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    पूर्णाङ्क		update_buffer;

    यदि (term->tl_normal_mode)
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

    यदि (ga_grow(gap, 1) == FAIL)
	प्रत्यागम 0;

    cellattr_T	*p = रिक्त;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;
    पूर्णाङ्क		col;
    पूर्णाङ्क		text_len;
    char_u		*text;
    sb_ओळ_T	*ओळ;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    प्रति (i = 0; i < cols; ++i)
	यदि (cells[i].chars[0] != 0)
	    len = i + 1;
	अन्यथा
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    यदि (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    यदि (p != रिक्त)
    {
	प्रति (col = 0; col < len; col += cells[col].width)
	{
	    यदि (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		भङ्ग;
	    }
	    प्रति (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    यदि (ga_grow(&ga, 1) == FAIL)
    {
	यदि (update_buffer)
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
    यदि (update_buffer)
	add_scrollback_ओळ_to_buffer(term, text, text_len, continuation);

    ओळ = (sb_ओळ_T *)gap->ga_data + gap->ga_len;
    ओळ->sb_cols = len;
    ओळ->sb_bytes = text_len;
    ओळ->sb_cells = p;
    ओळ->sb_fill_attr = fill_attr;
    ओळ->continuation = (char_u)continuation;
    यदि (update_buffer)
    {
	ओळ->sb_text = रिक्त;
	++term->tl_scrollback_scrolled;
	यदि (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    अन्यथा
    {
	ओळ->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    प्रत्यागम 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    स्थिर शून्य
handle_postponed_scrollback(term_T *term)
{
    पूर्णाङ्क i;

    यदि (term->tl_scrollback_postponed.ga_len == 0)
	प्रत्यागम;
    ch_log(रिक्त, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    प्रति (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_ओळ_T	*pp_ओळ;
	sb_ओळ_T	*ओळ;

	यदि (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    भङ्ग;
	pp_ओळ = (sb_ओळ_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_ओळ->sb_text;
	यदि (text == रिक्त)
	    text = (char_u *)"";
	add_scrollback_ओळ_to_buffer(term, text, (पूर्णाङ्क)STRLEN(text), pp_ओळ->continuation);
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
	यदि (!pp_ओळ->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    स्थिर पूर्णाङ्क
handle_bell(शून्य *user UNUSED)
{
    vim_beep(BO_TERM);
    प्रत्यागम 0;
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
    स्थिर पूर्णाङ्क
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    यदि (!term->tl_normal_mode)
    {
	पूर्णाङ्क	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	यदि (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    पूर्णाङ्क		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = रिक्त;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    यदि (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		यदि (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    अन्यथा
#endif
	    // If this is the last normal window: exit Vim.
	    यदि (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		प्रत्यागम TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(रिक्त, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    यदि (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		यदि (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		यदि (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		यदि (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    यदि (pwin != रिक्त)
		popup_close_with_retval(pwin, 0);
#endif
	    प्रत्यागम TRUE;
	}
	यदि (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    वर्ण    *cmd = term->tl_opencmd == रिक्त
				? "botright sbuf %d"
				: (वर्ण *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    वर्ण    *buf = alloc(len);

	    यदि (buf != रिक्त)
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
    प्रत्यागम FALSE;
}

#यदि defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    पूर्णाङ्क
may_close_term_popup(शून्य)
{
    यदि (!popup_is_popup(curwin) || curbuf->b_term == रिक्त
				 || term_job_running_not_none(curbuf->b_term))
	प्रत्यागम FAIL;

    win_T *pwin = curwin;

    यदि (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    प्रत्यागम OK;
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

    प्रति (term = first_term; term != रिक्त; term = term->tl_next)
	यदि (term->tl_job == ch->ch_job && !term->tl_channel_closed)
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
    पूर्णाङ्क	    did_one = FALSE;

    प्रति (term = first_term; term != रिक्त; term = next_term)
    {
	next_term = term->tl_next;
	यदि (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    यदि (term->tl_out_fd != रिक्त)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = रिक्त;
	    }
#endif

	    यदि (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		अनुवर्त;
	    }

	    यदि (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    यदि (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	यदि (term != रिक्त)
	{
	    यदि (term->tl_job == ch->ch_job)
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

    प्रति (term = first_term; term != रिक्त; term = next_term)
    {
	next_term = term->tl_next;
	यदि (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    यदि (term_after_channel_closed(term))
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
	पूर्णाङ्क		max_col)
{
    पूर्णाङ्क off = screen_get_current_ओळ_off();

    प्रति (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	पूर्णाङ्क		c;

	यदि (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	यदि (c == NUL)
	{
	    Screen_ओळी[off] = ' ';
	    यदि (enc_utf8)
		Screen_ओळी_UC[off] = NUL;
	}
	अन्यथा
	{
	    यदि (enc_utf8)
	    {
		पूर्णाङ्क i;

		// composing chars
		प्रति (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_ओळी_C[i][off] = cell.chars[i + 1];
		    यदि (cell.chars[i + 1] == 0)
			भङ्ग;
		}
		यदि (c >= 0x80 || (Screen_mco > 0
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
	    अन्यथा यदि (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		यदि (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (वर्ण*)mb, 2, 0, 0) > 1)
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
	यदि (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    यदि (enc_utf8)
	    {
		Screen_ओळी_UC[off] = NUL;
		Screen_ओळी[off] = NUL;
	    }
	    अन्यथा यदि (!has_mbyte)
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

#यदि defined(FEAT_GUI)
    स्थिर शून्य
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    यदि (term->tl_vterm == रिक्त)
	प्रत्यागम;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    यावत् (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	पूर्णाङ्क save_p_more = p_more;

	p_more = FALSE;
	संदेश_row = Rows - 1;
	संदेश_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    प्रति (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	यदि (pos.row < term->tl_rows)
	{
	    पूर्णाङ्क max_col = MIN(topframe->fr_width, term->tl_cols);

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
    पूर्णाङ्क
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    प्रत्यागम term != रिक्त && term->tl_vterm != रिक्त && !term->tl_normal_mode;
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
    पूर्णाङ्क		rows, cols;
    पूर्णाङ्क		newrows, newcols;
    पूर्णाङ्क		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    यदि (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	यदि (term->tl_postponed_scroll > 0
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
    प्रति (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == रिक्त ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	यदि (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	यदि (twp == रिक्त)
	    भङ्ग;
    }
    यदि (newrows == 99999 || newcols == 99999)
	प्रत्यागम; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    यदि (newrows == 0 || newcols == 0)
	प्रत्यागम;

    यदि (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	यदि (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    प्रति (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	यदि (pos.row < term->tl_rows)
	{
	    पूर्णाङ्क max_col = MIN(wp->w_width, term->tl_cols);

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

    यदि (term == रिक्त || term->tl_vterm == रिक्त || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	प्रत्यागम;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    पूर्णाङ्क
term_is_finished(buf_T *buf)
{
    प्रत्यागम buf->b_term != रिक्त && buf->b_term->tl_vterm == रिक्त;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    पूर्णाङ्क
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    प्रत्यागम term != रिक्त && (term->tl_vterm == रिक्त || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    शून्य
term_change_in_curbuf(शून्य)
{
    term_T *term = curbuf->b_term;

    यदि (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	प्रत्यागम;

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
    पूर्णाङ्क
term_get_attr(win_T *wp, linenr_T lnum, पूर्णाङ्क col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_ओळ_T	*ओळ;
    cellattr_T	*cellattr;
    पूर्णाङ्क         sb_ओळ = -1;
    पूर्णाङ्क         sb_col = col;

    यदि (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_ओळ, &sb_col);

    यदि (sb_ओळ < 0)
	cellattr = &term->tl_default_color;
    अन्यथा
    {
	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data + sb_ओळ;
	यदि (sb_col < 0 || sb_col >= ओळ->sb_cols)
	    cellattr = &ओळ->sb_fill_attr;
	अन्यथा
	    cellattr = ओळ->sb_cells + sb_col;
    }
    प्रत्यागम cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    स्थिर शून्य
cterm_color2vterm(पूर्णाङ्क nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    यदि (rgb->index == 0)
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
    स्थिर पूर्णाङ्क
get_vterm_color_from_synid(पूर्णाङ्क id, VTermColor *fg, VTermColor *bg)
{
#यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    यदि (0
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

	यदि (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	यदि (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	अन्यथा
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	यदि (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	अन्यथा
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	प्रत्यागम TRUE;
    }
    अन्यथा
#endif
    यदि (t_colors >= 16)
    {
	पूर्णाङ्क cterm_fg = -1;
	पूर्णाङ्क cterm_bg = -1;

	यदि (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	यदि (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	अन्यथा
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	यदि (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	अन्यथा
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
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
    पूर्णाङ्क id = wp->w_hlfwin_id;

    यदि (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
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
    पूर्णाङ्क		 did_curwin = FALSE;

    यावत् (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    शून्य
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    पूर्णाङ्क		    fgval, bgval;
    पूर्णाङ्क		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    यदि (*p_bg == 'l')
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

    यदि (!get_vterm_color_from_synid(id, fg, bg))
    {
#यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	पूर्णाङ्क tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	यदि (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    यदि (!gui.in_use)
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

	यदि (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    यदि (!gui.in_use)
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

#यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    स्थिर पूर्णाङ्क
term_use_palette(शून्य)
{
    यदि (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    स्थिर शून्य
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    पूर्णाङ्क		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    प्रति (; index < 16; index++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (unsigned)(rgb[index] >> 16);
	color.green = (unsigned)(rgb[index] >> 8) & 255;
	color.blue = (unsigned)rgb[index] & 255;
	color.index = 0;
	vterm_state_set_palette_color(state, index, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    स्थिर पूर्णाङ्क
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    पूर्णाङ्क		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    प्रति (li = list->lv_first; li != रिक्त && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	यदि (color_name == रिक्त)
	    प्रत्यागम FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	यदि (guicolor == INVALCOLOR)
	    प्रत्यागम FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    यदि (n != 16 || li != रिक्त)
	प्रत्यागम FAIL;

    set_vterm_palette(vterm, rgb);

    प्रत्यागम OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    स्थिर शून्य
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", रिक्त, TRUE);

    यदि (var == रिक्त)
	प्रत्यागम;

    यदि (var->di_tv.v_type != VAR_LIST
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
    पूर्णाङ्क		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = रिक्त;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	यदि (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    प्रत्यागम;
	}
    }

    CLEAR_FIELD(ea);

    यदि (opt_item != रिक्त && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != रिक्त)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	यदि (p == रिक्त)
	    p = dict_get_string(dict, "fileformat", FALSE);
	यदि (p != रिक्त)
	{
	    यदि (check_ff_मूल्य(p) == FAIL)
		ch_log(रिक्त, "Invalid ff argument to drop: %s", p);
	    अन्यथा
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	यदि (p == रिक्त)
	    p = dict_get_string(dict, "encoding", FALSE);
	यदि (p != रिक्त)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    यदि (ea.cmd != रिक्त)
	    {
		sprintf((वर्ण *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	यदि (p != रिक्त)
	    get_bad_opt(p, &ea);

	यदि (dict_has_किल्ली(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	यदि (dict_has_किल्ली(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	यदि (dict_has_किल्ली(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	यदि (dict_has_किल्ली(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    यदि (ea.cmd == रिक्त)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    स्थिर पूर्णाङ्क
is_permitted_term_api(char_u *func, char_u *pat)
{
    प्रत्यागम pat != रिक्त && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
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

    यदि (item->li_next == रिक्त)
    {
	ch_log(channel, "Missing function arguments for call");
	प्रत्यागम;
    }
    func = tv_get_string(&item->li_tv);

    यदि (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	प्रत्यागम;
    }

    argvars[0].v_type = VAR_संख्या;
    argvars[0].vval.v_संख्या = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    यदि (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
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
    स्थिर size_t
url_decode(ध्रुव वर्ण *src, ध्रुव size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    यावत् (i < len)
    {
	यदि (src[i] == '%' && i + 2 < len)
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
    प्रत्यागम j;
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
    पूर्णाङ्क       offset = 7;  // len of "file://" is 7
    वर्ण      *pos = (वर्ण *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    यावत् (offset < (पूर्णाङ्क)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    यदि (offset >= (पूर्णाङ्क)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	प्रत्यागम;
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
    स्थिर पूर्णाङ्क
parse_osc(पूर्णाङ्क command, VTermStringFragment frag, शून्य *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == रिक्त ? रिक्त
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    यदि (command != 51 && (command != 7 || !p_asd))
	प्रत्यागम 0;

    // Concatenate what was received until the final piece is found.
    यदि (ga_grow(gap, (पूर्णाङ्क)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	प्रत्यागम 1;
    }
    mch_memmove((वर्ण *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (पूर्णाङ्क)frag.len;
    यदि (!frag.final)
	प्रत्यागम 1;

    ((वर्ण *)gap->ga_data)[gap->ga_len] = 0;

    यदि (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	प्रत्यागम 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = रिक्त;
    reader.js_used = 0;
    यदि (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != रिक्त)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	यदि (item == रिक्त)
	    ch_log(channel, "Missing command");
	अन्यथा
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    यदि (item == रिक्त)
		ch_log(channel, "Missing argument for %s", cmd);
	    अन्यथा यदि (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    अन्यथा यदि (STRCMP(cmd, "call") == 0)
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
    प्रत्यागम 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    स्थिर पूर्णाङ्क
parse_csi(
	ध्रुव वर्ण  *leader UNUSED,
	ध्रुव long  args[],
	पूर्णाङ्क	    argcount,
	ध्रुव वर्ण  *intermed UNUSED,
	वर्ण	    command,
	शून्य	    *user)
{
    term_T	*term = (term_T *)user;
    वर्ण	buf[100];
    पूर्णाङ्क		len;
    पूर्णाङ्क		x = 0;
    पूर्णाङ्क		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    यदि (command != 't' || argcount != 1 || args[0] != 13)
	प्रत्यागम 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#यदि defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (शून्य)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_buffer == term->tl_buffer)
	    भङ्ग;
    यदि (wp != रिक्त)
    {
#ifdef FEAT_GUI
	यदि (gui.in_use)
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
    प्रत्यागम 1;
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
vterm_malloc(size_t size, शून्य *data UNUSED)
{
    // make sure that the length is not zero
    प्रत्यागम alloc_clear(size == 0 ? 1L : size);
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
    स्थिर पूर्णाङ्क
create_vterm(term_T *term, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_मूल्य	    मूल्य;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, रिक्त);
    term->tl_vterm = vterm;
    यदि (vterm == रिक्त)
	प्रत्यागम FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    यदि (state == रिक्त || screen == रिक्त)
    {
	vterm_free(vterm);
	प्रत्यागम FAIL;
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

    यदि (t_colors < 16)
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
    यदि (GetCaretBlinkTime() == INFINITE)
	मूल्य.boolean = 0;
    अन्यथा
	मूल्य.boolean = 1;
#अन्यथा
    मूल्य.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &मूल्य);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    प्रत्यागम OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    स्थिर शून्य
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    पूर्णाङ्क		index;

    प्रति (index = 0; index < 16; index++)
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
#यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    यदि (term_use_palette()
	    && (term->tl_palette != रिक्त
		|| find_var((char_u *)"g:terminal_ansi_colors", रिक्त, TRUE)
								      != रिक्त))
    {
	यदि (term->tl_palette != रिक्त)
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
	यदि (term->tl_vterm == रिक्त)
	    अनुवर्त;
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
	यदि (term->tl_vterm == रिक्त)
	    अनुवर्त;
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
    यदि (term->tl_status_text != रिक्त)
	प्रत्यागम term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    यदि (term->tl_normal_mode)
    {
	यदि (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	अन्यथा
	    txt = (char_u *)_("Terminal-finished");
    }
    अन्यथा यदि (term->tl_title != रिक्त)
	txt = term->tl_title;
    अन्यथा यदि (term_none_open(term))
	txt = (char_u *)_("active");
    अन्यथा यदि (term_job_running(term))
	txt = (char_u *)_("running");
    अन्यथा
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    यदि (term->tl_status_text != रिक्त)
	vim_snprintf((वर्ण *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    प्रत्यागम term->tl_status_text;
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
    पूर्णाङ्क
set_ref_in_term(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    प्रति (term = first_term; !abort && term != रिक्त; term = term->tl_next)
	यदि (term->tl_job != रिक्त)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    प्रत्यागम abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    स्थिर buf_T *
term_get_buf(typval_T *argvars, वर्ण *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    यदि (buf == रिक्त || buf->b_term == रिक्त)
    {
	(शून्य)tv_get_संख्या(&argvars[0]);    // issue errmsg if type error
	ch_log(रिक्त, "%s: invalid buffer argument", where);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम buf;
}

    स्थिर शून्य
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    स्थिर शून्य
dump_term_color(FILE *fd, VTermColor *color)
{
    पूर्णाङ्क index;

    यदि (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    अन्यथा यदि (color->type == 0)
	// use RGB values
	index = 255;
    अन्यथा
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (पूर्णाङ्क)color->red, (पूर्णाङ्क)color->green, (पूर्णाङ्क)color->blue,
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
    पूर्णाङ्क		max_height = 0;
    पूर्णाङ्क		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    यदि (term->tl_vterm == रिक्त)
    {
	emsg(_(e_job_already_finished));
	प्रत्यागम;
    }

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	यदि (check_for_dict_arg(argvars, 2) == FAIL)
	    प्रत्यागम;
	d = argvars[2].vval.v_dict;
	यदि (d != रिक्त)
	{
	    max_height = dict_get_संख्या(d, "rows");
	    max_width = dict_get_संख्या(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    यदि (fname == रिक्त)
	प्रत्यागम;
    यदि (mch_stat((वर्ण *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	प्रत्यागम;
    }

    यदि (*fname == NUL || (fd = mch_fopen((वर्ण *)fname, WRITEBIN)) == रिक्त)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	प्रत्यागम;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    प्रति (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	पूर्णाङ्क		repeat = 0;

	प्रति (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    पूर्णाङ्क		    same_attr;
	    पूर्णाङ्क		    same_chars = TRUE;
	    पूर्णाङ्क		    i;
	    पूर्णाङ्क		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    यदि (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    प्रति (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		पूर्णाङ्क c = cell.chars[i];
		पूर्णाङ्क pc = prev_cell.chars[i];
		पूर्णाङ्क should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		यदि (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		यदि (c != pc)
		    same_chars = FALSE;
		यदि (should_break)
		    भङ्ग;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    यदि (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    अन्यथा
	    {
		यदि (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		यदि (cell.chars[0] == NUL)
		    fputs(" ", fd);
		अन्यथा
		{
		    char_u	charbuf[10];
		    पूर्णाङ्क		len;

		    प्रति (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		यदि (cell.width != prev_cell.width || !same_attr)
		{
		    यदि (cell.width == 2)
			fputs("*", fd);
		    अन्यथा
			fputs("+", fd);

		    यदि (same_attr)
		    {
			fputs("&", fd);
		    }
		    अन्यथा
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			यदि (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			अन्यथा
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			यदि (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			अन्यथा
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    यदि (cell.width == 2)
		++pos.col;
	}
	यदि (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
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
    यदि (ga_grow(gap, 1) == FAIL)
	प्रत्यागम;

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
    स्थिर पूर्णाङ्क
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    पूर्णाङ्क		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {रिक्त, 0};
    पूर्णाङ्क		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    पूर्णाङ्क		    max_cells = 0;
    पूर्णाङ्क		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, परिमाण(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    प्रति (;;)
    {
	यदि (c == EOF)
	    भङ्ग;
	यदि (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	अन्यथा यदि (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    यदि (ga_text.ga_data == रिक्त)
		dump_is_corrupt(&ga_text);
	    यदि (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_ओळ_T   *ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		यदि (max_cells < ga_cell.ga_len)
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

	    c = fgetc(fd);
	}
	अन्यथा यदि (c == '|' || c == '>')
	{
	    पूर्णाङ्क prev_len = ga_text.ga_len;

	    यदि (c == '>')
	    {
		यदि (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    यदि (c != EOF)
		ga_append(&ga_text, c);
	    प्रति (;;)
	    {
		c = fgetc(fd);
		यदि (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    भङ्ग;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    यदि (ga_text.ga_data != रिक्त)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		यदि (prev_char.string == रिक्त)
		    prev_char.length = 0;
	    }

	    यदि (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    अन्यथा यदि (c == '+' || c == '*')
	    {
		पूर्णाङ्क is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		यदि (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		अन्यथा यदि (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    यावत् (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    प्रति (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			यदि (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			अन्यथा यदि (c == '#')
			{
			    पूर्णाङ्क red, green, blue, index = 0, type;

			    c = fgetc(fd);
			    red = hex2nr(c);
			    c = fgetc(fd);
			    red = (red << 4) + hex2nr(c);
			    c = fgetc(fd);
			    green = hex2nr(c);
			    c = fgetc(fd);
			    green = (green << 4) + hex2nr(c);
			    c = fgetc(fd);
			    blue = hex2nr(c);
			    c = fgetc(fd);
			    blue = (blue << 4) + hex2nr(c);
			    c = fgetc(fd);
			    यदि (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    यावत् (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    यदि (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				यदि (index == 0)
				{
				    यदि (is_bg)
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
			    यदि (is_bg)
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
	    यदि (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	अन्यथा यदि (c == '@')
	{
	    यदि (prev_char.string == रिक्त)
		dump_is_corrupt(&ga_text);
	    अन्यथा
	    {
		पूर्णाङ्क मोजणी = 0;

		// repeat previous character, get the count
		प्रति (;;)
		{
		    c = fgetc(fd);
		    यदि (!SAFE_isdigit(c))
			भङ्ग;
		    मोजणी = मोजणी * 10 + (c - '0');
		}

		यावत् (मोजणी-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	अन्यथा
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    यदि (ga_text.ga_len > 0)
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

    प्रत्यागम max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    स्थिर char_u *
get_separator(पूर्णाङ्क text_width, char_u *fname)
{
    पूर्णाङ्क	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    पूर्णाङ्क	    fname_size;
    char_u  *p = fname;
    पूर्णाङ्क	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    यदि (textline == रिक्त)
	प्रत्यागम रिक्त;

    fname_size = vim_strsize(fname);
    यदि (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    अन्यथा यदि (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    यावत् (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    प्रति (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    प्रति (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    प्रत्यागम textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    स्थिर शून्य
term_load_dump(typval_T *argvars, typval_T *rettv, पूर्णाङ्क do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = रिक्त;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = रिक्त;
    char_u	*fname_tofree = रिक्त;
    FILE	*fd1;
    FILE	*fd2 = रिक्त;
    char_u	*textline = रिक्त;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    यदि (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    यदि (fname1 == रिक्त || (do_diff && fname2 == रिक्त))
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }
    fd1 = mch_fopen((वर्ण *)fname1, READBIN);
    यदि (fd1 == रिक्त)
    {
	semsg(_(e_cant_read_file_str), fname1);
	प्रत्यागम;
    }
    यदि (do_diff)
    {
	fd2 = mch_fopen((वर्ण *)fname2, READBIN);
	यदि (fd2 == रिक्त)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    प्रत्यागम;
	}
    }

    init_job_options(&opt);
    यदि (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	गच्छ theend;

    यदि (opt.jo_term_name == रिक्त)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	यदि (fname_tofree != रिक्त)
	{
	    vim_snprintf((वर्ण *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    यदि (opt.jo_bufnr_buf != रिक्त)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	यदि (wp == रिक्त)
	    semsg(_(e_invalid_argument_str), "bufnr");
	अन्यथा
	{
	    buf = curbuf;
	    यावत् (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    अन्यथा
	// Create a new terminal window.
	buf = term_start(&argvars[0], रिक्त, &opt, TERM_START_NOJOB);

    यदि (buf != रिक्त && buf->b_term != रिक्त)
    {
	पूर्णाङ्क		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	पूर्णाङ्क		width;
	पूर्णाङ्क		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_संख्या = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	यदि (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	यदि (!do_diff)
	    गच्छ theend;

	term->tl_शीर्ष_diff_rows = curbuf->b_ml.ml_ओळ_मोजणी;

	textline = get_separator(width, fname1);
	यदि (textline == रिक्त)
	    गच्छ theend;
	यदि (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ओळ_मोजणी, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	यदि (textline == रिक्त)
	    गच्छ theend;
	यदि (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ओळ_मोजणी, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	width2 = read_dump_file(fd2, &cursor_pos2);
	यदि (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    यदि (textline == रिक्त)
		गच्छ theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_ओळ_मोजणी - bot_lnum;

	प्रति (lnum = 1; lnum <= term->tl_शीर्ष_diff_rows; ++lnum)
	{
	    यदि (lnum + bot_lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	    {
		// bottom part has fewer rows, fill with "-"
		प्रति (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    अन्यथा
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		पूर्णाङ्क	col;
		sb_ओळ_T   *sb_ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_ओळ + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_ओळ + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		यदि (line1 == रिक्त)
		    भङ्ग;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		प्रति (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    पूर्णाङ्क len1 = utfc_ptr2len(p1);
		    पूर्णाङ्क len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    यदि (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    अन्यथा यदि (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    अन्यथा यदि (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    अन्यथा यदि (cellattr1 != रिक्त && cellattr2 != रिक्त)
		    {
			यदि ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			अन्यथा यदि (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			अन्यथा यदि (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			अन्यथा यदि (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		यावत् (col < width)
		{
		    यदि (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    अन्यथा यदि (*p1 == NUL)
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
	    यदि (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_शीर्ष_diff_rows) == OK)
		ml_append(term->tl_शीर्ष_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	यावत् (lnum + bot_lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    // bottom part has more rows, fill with "+"
	    प्रति (i = 0; i < width; ++i)
		textline[i] = '+';
	    यदि (add_empty_scrollback(term, &term->tl_default_color,
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
    fclose(fd1);
    यदि (fd2 != रिक्त)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    पूर्णाङ्क
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

    यदि (term == रिक्त
	    || !term_is_finished(curbuf)
	    || term->tl_शीर्ष_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	प्रत्यागम FAIL;

    ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
    शीर्ष_rows = term->tl_शीर्ष_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = ओळ_मोजणी - bot_rows;
    sb_ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    प्रति (lnum = 1; lnum <= शीर्ष_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	यदि (p == रिक्त)
	    प्रत्यागम OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    प्रति (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	यदि (p == रिक्त)
	    प्रत्यागम OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    यदि (p == रिक्त)
	प्रत्यागम OK;
    ml_append(ओळ_मोजणी - शीर्ष_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(ओळ_मोजणी - शीर्ष_rows));
    यदि (p == रिक्त)
	प्रत्यागम OK;
    ml_delete(ओळ_मोजणी - शीर्ष_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    यदि (शीर्ष_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	प्रति (lnum = 0; lnum < शीर्ष_rows; ++lnum)
	{
	    sb_ओळ_T	temp;

	    temp = *(sb_ओळ + lnum);
	    *(sb_ओळ + lnum) = *(sb_ओळ + bot_start + lnum);
	    *(sb_ओळ + bot_start + lnum) = temp;
	}
    }
    अन्यथा
    {
	size_t		size = परिमाण(sb_ओळ_T) * term->tl_scrollback.ga_len;
	sb_ओळ_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	यदि (temp != रिक्त)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    परिमाण(sb_ओळ_T) * bot_rows);
	    mch_memmove((sb_ओळ_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + शीर्ष_rows,
		    परिमाण(sb_ओळ_T) * (ओळ_मोजणी - शीर्ष_rows - bot_rows));
	    mch_memmove((sb_ओळ_T *)term->tl_scrollback.ga_data
						       + ओळ_मोजणी - शीर्ष_rows,
		    temp,
		    परिमाण(sb_ओळ_T) * शीर्ष_rows);
	    vim_free(temp);
	}
    }

    term->tl_शीर्ष_diff_rows = bot_rows;
    term->tl_bot_diff_rows = शीर्ष_rows;

    update_screen(UPD_NOT_VALID);
    प्रत्यागम OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    शून्य
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    शून्य
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    शून्य
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    rettv->vval.v_संख्या = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    शून्य
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क	    attr;
    size_t  i;
    char_u  *name;

    स्थिर संरचना {
	वर्ण	    *name;
	पूर्णाङ्क	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    attr = tv_get_संख्या(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    यदि (name == रिक्त)
	प्रत्यागम;

    यदि (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    प्रति (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	यदि (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_संख्या = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    भङ्ग;
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

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getcursor()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_संख्या(l, term->tl_cursor_pos.row + 1);
    list_append_संख्या(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    यदि (d == रिक्त)
	प्रत्यागम;

    dict_add_संख्या(d, "visible", term->tl_cursor_visible);
    dict_add_संख्या(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_संख्या(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    यदि (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    शून्य
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getjob()");
    यदि (buf == रिक्त)
    {
	यदि (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = रिक्त;
	}
	अन्यथा
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_संख्या = VVAL_NULL;
	}
	प्रत्यागम;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    यदि (rettv->vval.v_job != रिक्त)
	++rettv->vval.v_job->jv_refcount;
}

    स्थिर पूर्णाङ्क
get_row_संख्या(typval_T *tv, term_T *term)
{
    यदि (tv->v_type == VAR_STRING
	    && tv->vval.v_string != रिक्त
	    && STRCMP(tv->vval.v_string, ".") == 0)
	प्रत्यागम term->tl_cursor_pos.row;
    प्रत्यागम (पूर्णाङ्क)tv_get_संख्या(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    शून्य
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    पूर्णाङ्क		    row;

    rettv->v_type = VAR_STRING;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getline()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    row = get_row_संख्या(&argvars[1], term);

    यदि (term->tl_vterm == रिक्त)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	पूर्णाङ्क	  sb_row = term->tl_scrollback_scrolled + row;
	sb_ओळ_T *ओळ;

	यदि (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    प्रत्यागम;
	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, रिक्त, &offset);

	// vterm is finished, get the text from the buffer
	यदि (lnum > 0 && lnum <= buf->b_ml.ml_ओळ_मोजणी)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    यदि (STRLEN(p) >= offset + ओळ->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, ओळ->sb_bytes);
	}
    }
    अन्यथा
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	पूर्णाङ्क		len;
	char_u		*p;

	यदि (row < 0 || row >= term->tl_rows)
	    प्रत्यागम;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	यदि (p == रिक्त)
	    प्रत्यागम;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (वर्ण *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    शून्य
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getscrolled()");
    यदि (buf == रिक्त)
	प्रत्यागम;
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

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getsize()");
    यदि (buf == रिक्त)
	प्रत्यागम;

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

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_संख्या_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_setsize()");
    यदि (buf == रिक्त)
    {
	emsg(_(e_not_terminal_buffer));
	प्रत्यागम;
    }
    यदि (buf->b_term->tl_vterm == रिक्त)
	प्रत्यागम;
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

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getstatus()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;

    यदि (term_job_running(term))
	STRCPY(val, "running");
    अन्यथा
	STRCPY(val, "finished");
    यदि (term->tl_normal_mode)
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

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_gettitle()");
    यदि (buf == रिक्त)
	प्रत्यागम;

    यदि (buf->b_term->tl_title != रिक्त)
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
    पूर्णाङ्क		अंक = 0;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    यदि (argvars[1].v_type != VAR_UNKNOWN)
	अंक = tv_get_bool(&argvars[1]);

    विकल्प (अंक)
    {
	पक्ष 0:
	    यदि (buf->b_term->tl_job != रिक्त)
		p = buf->b_term->tl_job->jv_tty_out;
	    भङ्ग;
	पक्ष 1:
	    यदि (buf->b_term->tl_job != रिक्त)
		p = buf->b_term->tl_job->jv_tty_in;
	    भङ्ग;
	स्वभाव:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    प्रत्यागम;
    }
    यदि (p != रिक्त)
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

    यदि (rettv_list_alloc(rettv) == FAIL || first_term == रिक्त)
	प्रत्यागम;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	यदि (tp->tl_buffer != रिक्त)
	    यदि (list_append_संख्या(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		प्रत्यागम;
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

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_scrape()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_संख्या(&argvars[1], term);

    यदि (term->tl_vterm != रिक्त)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	यदि (screen == रिक्त)  // can't really happen
	    प्रत्यागम;
	p = रिक्त;
	ओळ = रिक्त;
    }
    अन्यथा
    {
	पूर्णाङ्क	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, रिक्त, &offset);

	यदि (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_ओळ_मोजणी)
	    प्रत्यागम;

	ओळ = (sb_ओळ_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	यदि (STRLEN(p) < offset + ओळ->sb_bytes)
	    प्रत्यागम;

	p += offset;
    }

    प्रति (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	पूर्णाङ्क		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	पूर्णाङ्क		i;

	यदि (screen == रिक्त)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    यदि (pos.col >= ओळ->sb_cols)
		भङ्ग;
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

	    यदि (vterm_screen_get_cell(screen, pos, &cell) == 0)
		भङ्ग;

	    mbslen = 0;
	    प्रति (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		यदि (cell.chars[i] == 0)
		    भङ्ग;
		mbslen += (*utf_char2bytes)((पूर्णाङ्क)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	यदि (dcell == रिक्त)
	    भङ्ग;
	यदि (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    भङ्ग;
	}

	dict_add_string_len(dcell, "chars", mbs, (पूर्णाङ्क)mbslen);

	rgblen = vim_snprintf_safelen((वर्ण *)rgb, परिमाण(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (पूर्णाङ्क)rgblen);
	rgblen = vim_snprintf_safelen((वर्ण *)rgb, परिमाण(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (पूर्णाङ्क)rgblen);

	dict_add_संख्या(dcell, "attr",
				      cell2attr(term, रिक्त, &attrs, &fg, &bg));
	dict_add_संख्या(dcell, "width", width);

	++pos.col;
	यदि (width == 2)
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

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_sendkeys()");
    यदि (buf == रिक्त)
	प्रत्यागम;

    संदेश = tv_get_string_chk(&argvars[1]);
    यदि (संदेश == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    यदि (term->tl_vterm == रिक्त)
	प्रत्यागम;

    यावत् (*संदेश != NUL)
    {
	पूर्णाङ्क c;

	यदि (*संदेश == K_SPECIAL && संदेश[1] != NUL && संदेश[2] != NUL)
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

#यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    पूर्णाङ्क		index;
    list_T	*list;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_getansicolors()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    यदि (term->tl_vterm == रिक्त)
	प्रत्यागम;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    प्रति (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((वर्ण *)hexbuf, परिमाण(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	यदि (list_append_string(list, hexbuf, (पूर्णाङ्क)hexbuflen) == FAIL)
	    प्रत्यागम;
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
    पूर्णाङ्क		n = 0;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_setansicolors()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    यदि (term->tl_vterm == रिक्त)
	प्रत्यागम;

    यदि (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    यदि (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_मूल्य_for_argument_str), "\"colors\"");
	प्रत्यागम;
    }

    यदि (term->tl_palette == रिक्त)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    यदि (term->tl_palette == रिक्त)
	प्रत्यागम;

    FOR_ALL_LIST_वस्तुसंच(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	यदि (color_name == रिक्त)
	    प्रत्यागम;

	guicolor = GUI_GET_COLOR(color_name);
	यदि (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    प्रत्यागम;
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

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_setapi()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    यदि (api != रिक्त)
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
#यदि defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_setrestore()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    यदि (cmd != रिक्त)
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

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_setkill()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    यदि (how != रिक्त)
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

    यदि (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    init_job_options(&opt);
    यदि (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	प्रत्यागम;

    buf = term_start(&argvars[0], रिक्त, &opt, 0);

    यदि (buf != रिक्त && buf->b_term != रिक्त)
	rettv->vval.v_संख्या = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    शून्य
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    buf = term_get_buf(argvars, "term_wait()");
    यदि (buf == रिक्त)
	प्रत्यागम;
    यदि (buf->b_term->tl_job == रिक्त)
    {
	ch_log(रिक्त, "term_wait(): no job to wait for");
	प्रत्यागम;
    }
    यदि (buf->b_term->tl_job->jv_channel == रिक्त)
	// channel is closed, nothing to do
	प्रत्यागम;

    // Get the job status, this will detect a job that finished.
    यदि (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(रिक्त, "term_wait(): waiting for channel to close");
	यावत् (buf->b_term != रिक्त && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    यदि (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		भङ्ग;
	    यदि (buf->b_term == रिक्त || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		भङ्ग;
	}

	term_flush_messages();
    }
    अन्यथा
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	यदि (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_संख्या(&argvars[1]);
	ui_delay(wait, TRUE);

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
	यदि (term->tl_job == ch->ch_job)
	{
	    यदि (term->tl_eof_chars != रिक्त)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(पूर्णाङ्क)STRLEN(term->tl_eof_chars), रिक्त);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, रिक्त);
	    }
#ifdef MSWIN
	    अन्यथा
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, रिक्त);
#endif
	}
}

#यदि defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    प्रत्यागम term != रिक्त ? term->tl_job : रिक्त;
}
#endif

#यदि defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
शून्य (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    स्थिर पूर्णाङ्क
dyn_conpty_init(पूर्णाङ्क verbose)
{
    स्थिर HMODULE	hKerneldll = रिक्त;
    पूर्णाङ्क			i;
    स्थिर संरचना
    {
	वर्ण	*name;
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

    यदि (!has_conpty_working())
    {
	यदि (verbose)
	    emsg(_(e_conpty_is_not_available));
	प्रत्यागम FAIL;
    }

    // No need to initialize twice.
    यदि (hKerneldll)
	प्रत्यागम OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    प्रति (i = 0; conpty_entry[i].name != रिक्त
					&& conpty_entry[i].ptr != रिक्त; ++i)
    {
	यदि ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == रिक्त)
	{
	    यदि (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = रिक्त;
	    प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	वर्ण	    **argv UNUSED,
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
    वर्ण	    *errmsg = रिक्त;

    ga_init2(&ga_cmd, परिमाण(वर्ण*), 20);
    ga_init2(&ga_env, परिमाण(वर्ण*), 20);

    यदि (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    अन्यथा यदि (argvar->v_type == VAR_LIST)
    {
	यदि (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    गच्छ failed;
	cmd = ga_cmd.ga_data;
    }
    यदि (cmd == रिक्त || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	गच्छ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, रिक्त);

    यदि (cmd_wchar != रिक्त)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    यदि (cmd_wchar == रिक्त)
	गच्छ failed;
    यदि (opt->jo_cwd != रिक्त)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, रिक्त);
	यदि (cwd_wchar == रिक्त)
	    गच्छ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    यदि (!CreatePipe(&i_theirs, &i_ours, रिक्त, 0))
	गच्छ failed;
    यदि (!CreatePipe(&o_ours, &o_theirs, रिक्त, 0))
	गच्छ failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    यदि (FAILED(hr))
	गच्छ failed;

    term->tl_siex.StartupInfo.cb = परिमाण(term->tl_siex);

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
    यदि (!term->tl_siex.lpAttributeList)
	गच्छ failed;
    यदि (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	गच्छ failed;
    यदि (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    परिमाण(HPCON), रिक्त, रिक्त))
	गच्छ failed;

    channel = add_channel();
    यदि (channel == रिक्त)
	गच्छ failed;

    job = job_alloc();
    यदि (job == रिक्त)
	गच्छ failed;
    यदि (argvar->v_type == VAR_STRING)
    {
	पूर्णाङ्क argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    अन्यथा
    {
	पूर्णाङ्क argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    यदि (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    यदि (!CreateProcessW(रिक्त, cmd_wchar_copy, रिक्त, रिक्त, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	गच्छ failed;
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
    यदि (jo == रिक्त)
	गच्छ failed;

    यदि (!AssignProcessToJobObject(jo, proc_info.hProcess))
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

    यदि (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	गच्छ failed;

# यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    यदि (term_use_palette())
    {
	यदि (term->tl_palette != रिक्त)
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
    यदि (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((वर्ण *)fname, WRITEBIN);
	यदि (term->tl_out_fd == रिक्त)
	    semsg(_(e_cant_open_file_str), fname);
    }

    प्रत्यागम OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    यदि (channel != रिक्त)
	channel_clear(channel);
    यदि (job != रिक्त)
    {
	job->jv_channel = रिक्त;
	job_cleanup(job);
    }
    term->tl_job = रिक्त;
    यदि (jo != रिक्त)
	CloseHandle(jo);

    यदि (term->tl_siex.lpAttributeList != रिक्त)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = रिक्त;
    यदि (o_theirs != रिक्त)
	CloseHandle(o_theirs);
    यदि (o_ours != रिक्त)
	CloseHandle(o_ours);
    यदि (i_ours != रिक्त)
	CloseHandle(i_ours);
    यदि (i_theirs != रिक्त)
	CloseHandle(i_theirs);
    यदि (term->tl_conpty != रिक्त)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = रिक्त;
    // Propagate errors that occur in CreateProcess
    यदि (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    प्रत्यागम FAIL;
}

    स्थिर शून्य
conpty_term_report_winsize(term_T *term, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    स्थिर शून्य
term_free_conpty(term_T *term)
{
    यदि (term->tl_siex.lpAttributeList != रिक्त)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = रिक्त;
    यदि (term->tl_conpty != रिक्त)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = रिक्त;
}

    पूर्णाङ्क
use_conpty(शून्य)
{
    प्रत्यागम has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

शून्य* (*winpty_config_new)(UINT64, शून्य*);
शून्य* (*winpty_open)(शून्य*, शून्य*);
शून्य* (*winpty_spawn_config_new)(UINT64, शून्य*, LPCWSTR, शून्य*, शून्य*, शून्य*);
BOOL (*winpty_spawn)(शून्य*, शून्य*, HANDLE*, HANDLE*, DWORD*, शून्य*);
शून्य (*winpty_config_set_mouse_mode)(शून्य*, पूर्णाङ्क);
शून्य (*winpty_config_set_initial_size)(शून्य*, पूर्णाङ्क, पूर्णाङ्क);
LPCWSTR (*winpty_conin_name)(शून्य*);
LPCWSTR (*winpty_conout_name)(शून्य*);
LPCWSTR (*winpty_conerr_name)(शून्य*);
शून्य (*winpty_free)(शून्य*);
शून्य (*winpty_config_free)(शून्य*);
शून्य (*winpty_spawn_config_free)(शून्य*);
शून्य (*winpty_error_free)(शून्य*);
LPCWSTR (*winpty_error_संदेश)(शून्य*);
BOOL (*winpty_set_size)(शून्य*, पूर्णाङ्क, पूर्णाङ्क, शून्य*);
HANDLE (*winpty_agent_process)(शून्य*);

# define WINPTY_DLL "winpty.dll"

स्थिर HINSTANCE hWinPtyDLL = रिक्त;

    स्थिर पूर्णाङ्क
dyn_winpty_init(पूर्णाङ्क verbose)
{
    पूर्णाङ्क i;
    स्थिर संरचना
    {
	वर्ण	    *name;
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
    यदि (hWinPtyDLL)
	प्रत्यागम OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    यदि (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((वर्ण *)p_winptydll);
    यदि (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    यदि (!hWinPtyDLL)
    {
	यदि (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	प्रत्यागम FAIL;
    }
    प्रति (i = 0; winpty_entry[i].name != रिक्त
					 && winpty_entry[i].ptr != रिक्त; ++i)
    {
	यदि ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == रिक्त)
	{
	    यदि (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = रिक्त;
	    प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	वर्ण	    **argv UNUSED,
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

    ga_init2(&ga_cmd, परिमाण(वर्ण*), 20);
    ga_init2(&ga_env, परिमाण(वर्ण*), 20);

    यदि (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    अन्यथा यदि (argvar->v_type == VAR_LIST)
    {
	यदि (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    गच्छ failed;
	cmd = ga_cmd.ga_data;
    }
    यदि (cmd == रिक्त || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	गच्छ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, रिक्त);
    ga_clear(&ga_cmd);
    यदि (cmd_wchar == रिक्त)
	गच्छ failed;
    यदि (opt->jo_cwd != रिक्त)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, रिक्त);
	यदि (cwd_wchar == रिक्त)
	    गच्छ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    यदि (term->tl_winpty_config == रिक्त)
	गच्छ failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    यदि (term->tl_winpty == रिक्त)
	गच्छ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    रिक्त,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    यदि (spawn_config == रिक्त)
	गच्छ failed;

    channel = add_channel();
    यदि (channel == रिक्त)
	गच्छ failed;

    job = job_alloc();
    यदि (job == रिक्त)
	गच्छ failed;
    यदि (argvar->v_type == VAR_STRING)
    {
	पूर्णाङ्क argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    अन्यथा
    {
	पूर्णाङ्क argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    यदि (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    यदि (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	गच्छ failed;

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
    यदि (jo == रिक्त)
	गच्छ failed;

    यदि (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = रिक्त;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    यदि (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	गच्छ failed;

# यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    यदि (term_use_palette())
    {
	यदि (term->tl_palette != रिक्त)
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
    यदि (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((वर्ण *)fname, WRITEBIN);
	यदि (term->tl_out_fd == रिक्त)
	    semsg(_(e_cant_open_file_str), fname);
    }

    प्रत्यागम OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    यदि (spawn_config != रिक्त)
	winpty_spawn_config_free(spawn_config);
    यदि (channel != रिक्त)
	channel_clear(channel);
    यदि (job != रिक्त)
    {
	job->jv_channel = रिक्त;
	job_cleanup(job);
    }
    term->tl_job = रिक्त;
    यदि (jo != रिक्त)
	CloseHandle(jo);
    यदि (term->tl_winpty != रिक्त)
	winpty_free(term->tl_winpty);
    term->tl_winpty = रिक्त;
    यदि (term->tl_winpty_config != रिक्त)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = रिक्त;
    यदि (winpty_err != रिक्त)
    {
	वर्ण *संदेश = (वर्ण *)utf16_to_enc(
				(short_u *)winpty_error_संदेश(winpty_err), रिक्त);

	यदि (संदेश != रिक्त)
	{
	    emsg(संदेश);
	    vim_free(संदेश);
	}
	winpty_error_free(winpty_err);
    }
    प्रत्यागम FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	वर्ण	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    पूर्णाङ्क		    use_winpty = FALSE;
    पूर्णाङ्क		    use_conpty = FALSE;
    पूर्णाङ्क		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    यदि (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	प्रत्यागम dyn_winpty_init(TRUE);

    यदि (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    यदि (tty_type == NUL)
    {
	यदि (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	अन्यथा यदि (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    अन्यथा यदि (tty_type == 'w')	// winpty
    {
	यदि (has_winpty)
	    use_winpty = TRUE;
    }
    अन्यथा यदि (tty_type == 'c')	// conpty
    {
	यदि (has_conpty)
	    use_conpty = TRUE;
	अन्यथा
	    प्रत्यागम dyn_conpty_init(TRUE);
    }

    यदि (use_conpty)
	प्रत्यागम conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    यदि (use_winpty)
	प्रत्यागम winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    प्रत्यागम dyn_winpty_init(TRUE);
}

    स्थिर पूर्णाङ्क
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_मूल्य;
    HANDLE	    hPipeOut = INVALID_HANDLE_मूल्य;
    वर्ण	    in_name[80], out_name[80];
    channel_T	    *channel = रिक्त;

    यदि (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	प्रत्यागम FAIL;

    vim_snprintf(in_name, परिमाण(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, रिक्त);
    यदि (hPipeIn == INVALID_HANDLE_मूल्य)
	गच्छ failed;

    vim_snprintf(out_name, परिमाण(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, रिक्त);
    यदि (hPipeOut == INVALID_HANDLE_मूल्य)
	गच्छ failed;

    ConnectNamedPipe(hPipeIn, रिक्त);
    ConnectNamedPipe(hPipeOut, रिक्त);

    term->tl_job = job_alloc();
    यदि (term->tl_job == रिक्त)
	गच्छ failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    यदि (channel == रिक्त)
	गच्छ failed;
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

    प्रत्यागम OK;

failed:
    यदि (hPipeIn != रिक्त)
	CloseHandle(hPipeIn);
    यदि (hPipeOut != रिक्त)
	CloseHandle(hPipeOut);
    प्रत्यागम FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    स्थिर शून्य
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    यदि (term->tl_winpty != रिक्त)
	winpty_free(term->tl_winpty);
    term->tl_winpty = रिक्त;
    यदि (term->tl_winpty_config != रिक्त)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = रिक्त;
    यदि (term->tl_vterm != रिक्त)
	vterm_free(term->tl_vterm);
    term->tl_vterm = रिक्त;
}

/*
 * Report the size to the terminal.
 */
    स्थिर शून्य
term_report_winsize(term_T *term, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
    यदि (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    यदि (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, रिक्त);
}

    पूर्णाङ्क
terminal_enabled(शून्य)
{
    प्रत्यागम dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
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
    स्थिर पूर्णाङ्क
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	वर्ण	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = रिक्त;

    यदि (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	प्रत्यागम FAIL;

# यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    यदि (term_use_palette())
    {
	यदि (term->tl_palette != रिक्त)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	अन्यथा
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    यदि (term->tl_job != रिक्त)
	++term->tl_job->jv_refcount;

    प्रत्यागम term->tl_job != रिक्त
	&& term->tl_job->jv_channel != रिक्त
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    स्थिर पूर्णाङ्क
create_pty_only(term_T *term, jobopt_T *opt)
{
    यदि (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	प्रत्यागम FAIL;

    term->tl_job = job_alloc();
    यदि (term->tl_job == रिक्त)
	प्रत्यागम FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    प्रत्यागम mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    स्थिर शून्य
term_free_vterm(term_T *term)
{
    यदि (term->tl_vterm != रिक्त)
	vterm_free(term->tl_vterm);
    term->tl_vterm = रिक्त;
}

/*
 * Report the size to the terminal.
 */
    स्थिर शून्य
term_report_winsize(term_T *term, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
    // Use an ioctl() to report the new window size to the job.
    यदि (term->tl_job == रिक्त || term->tl_job->jv_channel == रिक्त)
	प्रत्यागम;

    पूर्णाङ्क fd = -1;
    पूर्णाङ्क part;

    प्रति (part = PART_OUT; part < PART_मोजणी; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	यदि (mch_isatty(fd))
	    भङ्ग;
    }
    यदि (part < PART_मोजणी && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
