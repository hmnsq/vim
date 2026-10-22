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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#অন্তর্ভাব "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
প্রকারনির্দেশ সংরচনা {
    VTermScreenCellAttrs	attrs;
    বর্ণ			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

প্রকারনির্দেশ সংরচনা sb_সারি_S {
    পূর্ণাঙ্ক		sb_cols;	// can differ per line
    পূর্ণাঙ্ক		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_সারি_T;

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
প্রকারনির্দেশ সংরচনা _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
সংরচনা terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#যদি defined(FEAT_GUI)
    পূর্ণাঙ্ক		tl_system;	// when non-zero used for :!cmd output
    পূর্ণাঙ্ক		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    পূর্ণাঙ্ক		tl_vterm_size_changed;

    পূর্ণাঙ্ক		tl_normal_mode; // TRUE: Terminal-Normal mode
    পূর্ণাঙ্ক		tl_channel_closing;
    পূর্ণাঙ্ক		tl_channel_closed;
    পূর্ণাঙ্ক		tl_channel_recently_closed; // still need to handle tl_finish

    পূর্ণাঙ্ক		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    শূন্য	*tl_winpty_config;
    শূন্য	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#যদি defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    পূর্ণাঙ্ক		tl_rows;
    পূর্ণাঙ্ক		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    পূর্ণাঙ্ক		tl_dirty_row_start; // MAX_ROW if nothing dirty
    পূর্ণাঙ্ক		tl_dirty_row_end;   // row below last one to update
    পূর্ণাঙ্ক		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    পূর্ণাঙ্ক		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    পূর্ণাঙ্ক		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    পূর্ণাঙ্ক		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    পূর্ণাঙ্ক		tl_scrollback_snapshot;
    পূর্ণাঙ্ক		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_শীর্ষ_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    পূর্ণাঙ্ক		tl_cursor_visible;
    পূর্ণাঙ্ক		tl_cursor_blink;
    পূর্ণাঙ্ক		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    পূর্ণাঙ্ক		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
স্থির term_T *first_term = রিক্ত;

// Terminal active in terminal_loop().
স্থির term_T *in_terminal_loop = রিক্ত;

#ifdef MSWIN
স্থির BOOL has_winpty = FALSE;
স্থির BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define চাবি_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    প্রতি ((term) = first_term; (term) != রিক্ত; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
স্থির পূর্ণাঙ্ক term_and_job_init(term_T *term, typval_T *argvar, বর্ণ **argv, jobopt_T *opt, jobopt_T *orig_opt);
স্থির পূর্ণাঙ্ক create_pty_only(term_T *term, jobopt_T *opt);
স্থির শূন্য term_report_winsize(term_T *term, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols);
স্থির শূন্য term_free_vterm(term_T *term);
#ifdef FEAT_GUI
স্থির শূন্য update_system_term(term_T *term);
#endif

স্থির শূন্য handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
স্থির পূর্ণাঙ্ক term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
স্থির char_u	*last_set_cursor_color = রিক্ত;
স্থির char_u	*desired_cursor_color = রিক্ত;
স্থির পূর্ণাঙ্ক	last_set_cursor_shape = -1;
স্থির পূর্ণাঙ্ক	desired_cursor_shape = -1;
স্থির পূর্ণাঙ্ক	last_set_cursor_blink = -1;
স্থির পূর্ণাঙ্ক	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    স্থির পূর্ণাঙ্ক
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    যদি (lhs_color != রিক্ত && rhs_color != রিক্ত)
	প্রত্যাগম STRCMP(lhs_color, rhs_color) == 0;
    প্রত্যাগম lhs_color == রিক্ত && rhs_color == রিক্ত;
}

    স্থির শূন্য
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    যদি (cursor_color_equal(*to_color, from_color))
	প্রত্যাগম;
    vim_free(*to_color);
    *to_color = (from_color == রিক্ত) ? রিক্ত : vim_strsave(from_color);
}

    স্থির char_u *
cursor_color_get(char_u *color)
{
    প্রত্যাগম (color == রিক্ত) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    স্থির পূর্ণাঙ্ক
parse_termwinsize(win_T *wp, পূর্ণাঙ্ক *rows, পূর্ণাঙ্ক *cols)
{
    পূর্ণাঙ্ক	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    যদি (*wp->w_p_tws == NUL)
	প্রত্যাগম FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    যদি (p == রিক্ত)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((বর্ণ *)wp->w_p_tws);
    *cols = atoi((বর্ণ *)p + 1);
    যদি (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    যদি (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    প্রত্যাগম minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    স্থির শূন্য
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    পূর্ণাঙ্ক rows, cols;
    পূর্ণাঙ্ক	minsize;

#ifdef FEAT_GUI
    যদি (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	প্রত্যাগম;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    যদি (minsize)
    {
	যদি (term->tl_rows < rows)
	    term->tl_rows = rows;
	যদি (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    যদি ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    অন্যথা যদি (rows != 0)
	term->tl_rows = rows;
    যদি ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    অন্যথা যদি (cols != 0)
	term->tl_cols = cols;

    যদি (!opt->jo_hidden)
    {
	যদি (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	যদি (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	যদি (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((বর্ণ *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_মান_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    শূন্য
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
    স্থির শূন্য
setup_job_options(jobopt_T *opt, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    যদি (!(opt->jo_set & JO_OUT_IO))
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
    যদি (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    যদি ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    যদি ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    স্থির শূন্য
term_flush_messages(শূন্য)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    স্থির শূন্য
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    যদি (old_curbuf != রিক্ত)
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
	বর্ণ	    **argv,
	jobopt_T    *opt,
	পূর্ণাঙ্ক	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = রিক্ত;
    পূর্ণাঙ্ক		res;
    buf_T	*newbuf;
    পূর্ণাঙ্ক		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    যদি (check_restricted() || check_secure())
	প্রত্যাগম রিক্ত;
    যদি (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_সারি_window));
	প্রত্যাগম রিক্ত;
    }

    যদি ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != রিক্ত
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != রিক্ত
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	প্রত্যাগম রিক্ত;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    যদি (term == রিক্ত)
	প্রত্যাগম রিক্ত;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, পরিমাণ(sb_সারি_T), 300);
    ga_init2(&term->tl_scrollback_postponed, পরিমাণ(sb_সারি_T), 300);
    ga_init2(&term->tl_osc_buf, পরিমাণ(বর্ণ), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    যদি (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	যদি (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    প্রত্যাগম রিক্ত;
	}
	যদি (do_ecmd(0, রিক্ত, রিক্ত, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    প্রত্যাগম রিক্ত;
	}
    }
    অন্যথা যদি (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", রিক্ত, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	যদি (buf == রিক্ত || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    প্রত্যাগম রিক্ত;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    অন্যথা
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	যদি (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_গুনতি = 1;
	}
	যদি (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_গুনতি = 1;
	}

	পূর্ণাঙ্ক cmod_split_modified = FALSE;
	যদি (vertical)
	{
	    যদি (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	যদি (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	যদি (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    প্রত্যাগম রিক্ত;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    যদি (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	যদি (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	যদি (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, রিক্ত, রিক্ত, FALSE, curbuf);

    যদি (opt->jo_term_name != রিক্ত)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    অন্যথা যদি (argv != রিক্ত)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    অন্যথা
    {
	পূর্ণাঙ্ক	i;
	size_t	len;
	char_u	*cmd, *p;

	যদি (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    যদি (cmd == রিক্ত)
		cmd = (char_u *)"";
	    অন্যথা যদি (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	অন্যথা যদি (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == রিক্ত
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == রিক্ত)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	প্রতি (i = 0; p != রিক্ত; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    যদি (i == 0)
		vim_snprintf((বর্ণ *)p, len, "!%s", cmd);
	    অন্যথা
		vim_snprintf((বর্ণ *)p, len, "!%s (%d)", cmd, i);
	    যদি (buflist_findname(p) == রিক্ত)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		ভঙ্গ;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, রিক্ত, রিক্ত, FALSE, curbuf);

    যদি (opt->jo_term_opencmd != রিক্ত)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    যদি (opt->jo_eof_chars != রিক্ত)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, পরিমাণ(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    যদি (flags & TERM_START_NOJOB)
	প্রত্যাগম curbuf;

#যদি defined(FEAT_SESSION)
    // Remember the command for the session file.
    যদি (opt->jo_term_norestore || argv != রিক্ত)
	term->tl_command = vim_strsave((char_u *)"NONE");
    অন্যথা যদি (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	যদি (cmd != রিক্ত && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    অন্যথা যদি (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != রিক্ত
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_বস্তুসমূহ(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    যদি (s == রিক্ত)
		ভঙ্গ;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    যদি (p == রিক্ত)
		ভঙ্গ;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	যদি (item == রিক্ত)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	অন্যথা
	    ga_clear(&ga);
    }
#endif

    যদি (opt->jo_term_kill != রিক্ত)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    যদি (opt->jo_term_api != রিক্ত)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    অন্যথা
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    যদি (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    যদি (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	যদি (term->tl_palette == রিক্ত)
	{
	    vim_free(term);
	    প্রত্যাগম রিক্ত;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, পরিমাণ(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    যদি (argv == রিক্ত
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != রিক্ত
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    অন্যথা
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    যদি (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	যদি (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = বার্তা_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	যদি (old_curbuf != রিক্ত)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	অন্যথা যদি (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| প্রবেশ_busy)
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
    অন্যথা
    {
	term_close_buffer(curbuf, old_curbuf);
	প্রত্যাগম রিক্ত;
    }

    apply_autocmds(EVENT_TERMINALOPEN, রিক্ত, রিক্ত, FALSE, newbuf);
    যদি (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, রিক্ত, রিক্ত, FALSE, newbuf);
    প্রত্যাগম newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    শূন্য
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    পূর্ণাঙ্ক		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = রিক্ত;
    পূর্ণাঙ্ক		scroll_save = বার্তা_scroll;

    বার্তা_scroll = FALSE;		// don't scroll here
    autowrite_all();
    বার্তা_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    যাবত্ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	যদি (ep != রিক্ত)
	{
	    যদি (ep < p)
		p = ep;
	    অন্যথা
		ep = রিক্ত;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((পূর্ণাঙ্ক)(p - cmd) == পরিমাণ(name) - 1 \
				 && STRNICMP(cmd, name, পরিমাণ(name) - 1) == 0)
	যদি (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	অন্যথা যদি (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	অন্যথা যদি (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	অন্যথা যদি (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	অন্যথা যদি (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	অন্যথা যদি (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	অন্যথা যদি (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	অন্যথা যদি (OPTARG_HAS("kill") && ep != রিক্ত)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	অন্যথা যদি (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    যদি (ep != রিক্ত)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    অন্যথা
		opt.jo_term_api = রিক্ত;
	}
	অন্যথা যদি (OPTARG_HAS("rows") && ep != রিক্ত && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((বর্ণ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	অন্যথা যদি (OPTARG_HAS("cols") && ep != রিক্ত && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((বর্ণ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	অন্যথা যদি (OPTARG_HAS("eof") && ep != রিক্ত)
	{
	    char_u *buf = রিক্ত;
	    char_u *চাবিগুলি;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    চাবিগুলি = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, রিক্ত);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(চাবিগুলি);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	অন্যথা যদি ((পূর্ণাঙ্ক)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != রিক্ত)
	{
	    পূর্ণাঙ্ক tty_type = NUL;

	    p = skiptowhite(cmd);
	    যদি (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    অন্যথা যদি (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    অন্যথা
	    {
		semsg(_(e_invalid_মান_for_argument_str), "type");
		গচ্ছ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	অন্যথা
	{
	    যদি (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    গচ্ছ theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    যদি (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	যদি (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    যদি (eap->addr_গুনতি > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_শীর্ষ | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_শীর্ষ = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    যদি (opt_shell && tofree == রিক্ত)
    {
#ifdef UNIX
	বর্ণ	**argv = রিক্ত;
	char_u	*tofree1 = রিক্ত;
	char_u	*tofree2 = রিক্ত;

	// :term ++shell command
	যদি (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(রিক্ত, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	গচ্ছ theend;
#অন্যথা
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	যদি (newcmd == রিক্ত)
	    গচ্ছ theend;
	tofree = newcmd;
	vim_snprintf((বর্ণ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# অন্যথা
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	গচ্ছ theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, রিক্ত, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    স্থির char_u *
get_terminalopt_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    // Note: Keep this in sync with ex_terminal.
    স্থির বর্ণ *(p_termopt_মানসমূহ[]) =
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

    যদি (idx < (পূর্ণাঙ্ক)ARRAY_LENGTH(p_termopt_মানসমূহ))
	প্রত্যাগম (char_u*)p_termopt_মানসমূহ[idx];
    প্রত্যাগম রিক্ত;
}

    স্থির char_u *
get_termkill_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    স্থির বর্ণ *(p_termkill_মানসমূহ[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    যদি (idx < (পূর্ণাঙ্ক)ARRAY_LENGTH(p_termkill_মানসমূহ))
	প্রত্যাগম (char_u*)p_termkill_মানসমূহ[idx];
    প্রত্যাগম রিক্ত;
}

/*
 * Command-line expansion for :terminal [options]
 */
    পূর্ণাঙ্ক
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	পূর্ণাঙ্ক	    *অঙ্ক_Matches)
{
    যদি (xp->xp_pattern > xp->xp_সারি && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, পূর্ণাঙ্ক) = রিক্ত;

	char_u *name_end = xp->xp_pattern - 1;
	যদি (name_end - xp->xp_সারি >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	যদি (cb != রিক্ত)
	{
	    প্রত্যাগম ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    অঙ্ক_Matches,
		    cb,
		    FALSE);
	}
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    অঙ্ক_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#যদি defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    পূর্ণাঙ্ক
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ধ্রুব পূর্ণাঙ্ক	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    যদি (terminal_bufs != রিক্ত && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	বর্ণ	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, পরিমাণ(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	যদি (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    যদি (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		প্রত্যাগম FAIL;
	    প্রত্যাগম put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    যদি (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	প্রত্যাগম FAIL;
# ifdef MSWIN
    যদি (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	প্রত্যাগম FAIL;
# endif
    যদি (term->tl_command != রিক্ত)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	যদি (quoted_command == রিক্ত)
	    প্রত্যাগম FAIL;

	পূর্ণাঙ্ক ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((বর্ণ *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	যদি (ret)
	    প্রত্যাগম FAIL;
    }
    যদি (put_eol(fd) != OK)
	প্রত্যাগম FAIL;

    যদি (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	প্রত্যাগম FAIL;

    যদি (terminal_bufs != রিক্ত && wp->w_buffer->b_nwindows > 1)
    {
	বর্ণ *hash_চাবি = alloc(NUMBUFLEN);

	vim_snprintf(hash_চাবি, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_চাবি, "terminal session");
    }

    প্রত্যাগম put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    পূর্ণাঙ্ক
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    প্রত্যাগম term != রিক্ত && (term->tl_command == রিক্ত
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    স্থির শূন্য
free_scrollback(term_T *term)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_সারি_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    প্রতি (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_সারি_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
স্থির term_T	*terminals_to_free = রিক্ত;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    শূন্য
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    যদি (term == রিক্ত)
	প্রত্যাগম;

    // Unlink the terminal form the list of terminals.
    যদি (first_term == term)
	first_term = term->tl_next;
    অন্যথা
	প্রতি (tp = first_term; tp->tl_next != রিক্ত; tp = tp->tl_next)
	    যদি (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		ভঙ্গ;
	    }

    যদি (term->tl_job != রিক্ত)
    {
	যদি (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, রিক্ত, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = রিক্ত;
    যদি (in_terminal_loop == term)
	in_terminal_loop = রিক্ত;
}

    শূন্য
free_unused_terminals(শূন্য)
{
    যাবত্ (terminals_to_free != রিক্ত)
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
	যদি (term->tl_out_fd != রিক্ত)
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
    স্থির ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    পূর্ণাঙ্ক		i;

    প্রতি (i = 0; i < 3; ++i)
    {
	পূর্ণাঙ্ক fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	যদি (mch_isatty(fd))
	    প্রত্যাগম parts[i];
    }
#endif
    প্রত্যাগম PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    স্থির শূন্য
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    বর্ণ   buf[চাবি_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, চাবি_BUF_LEN);

    যদি (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (পূর্ণাঙ্ক)curlen, রিক্ত);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    স্থির শূন্য
term_write_job_output(term_T *term, char_u *বার্তা_arg, size_t len_arg)
{
    char_u	*বার্তা = বার্তা_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    যদি (len > limit)
    {
	char_u *p = বার্তা + len - limit;

	p -= (*mb_head_off)(বার্তা, p);
	len -= p - বার্তা;
	বার্তা = p;
    }

    vterm_প্রবেশ_write(vterm, (বর্ণ *)বার্তা, len);

    // flush vterm buffer when vterm responded to control sequence
    যদি (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    স্থির শূন্য
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    যদি (popup_is_popup(wp))
    {
	wp->w_wrow += popup_শীর্ষ_extra(wp);
	wp->w_wcol += popup_বাম_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    অন্যথা
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    স্থির শূন্য
update_cursor(term_T *term, পূর্ণাঙ্ক redraw)
{
    যদি (term->tl_normal_mode)
	প্রত্যাগম;
#ifdef FEAT_GUI
    যদি (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    অন্যথা
#endif
    যদি (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    অন্যথা
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    যদি (redraw)
    {
	aco_save_T	aco;

	যদি (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	যদি (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, রিক্ত, রিক্ত, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    স্থির শূন্য
scrollbackline_pos_in_buf(term_T *term, পূর্ণাঙ্ক row, linenr_T *lnum, পূর্ণাঙ্ক *start_col, size_t *start_pos)
{
    sb_সারি_T	*সারিগুলি = (sb_সারি_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    পূর্ণাঙ্ক         calc_col = 0;
    পূর্ণাঙ্ক         i;

    যদি (row < 0 || row >= term->tl_scrollback.ga_len)
	প্রত্যাগম;

    যদি (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	প্রতি (i = term->tl_scrollback_scrolled + 1; i > 0 && সারিগুলি[i].continuation; --i)
	{
	    calc_pos += সারিগুলি[i - 1].sb_bytes;
	    calc_col += সারিগুলি[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	যদি (সারিগুলি[i].continuation)
	    ++i;
    }
    অন্যথা
    {
	i = 1;
	calc_lnum = 1;
    }

    প্রতি (; i <= row; ++i)
    {
	যদি (!সারিগুলি[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	অন্যথা
	{
	    calc_pos += সারিগুলি[i - 1].sb_bytes;
	    calc_col += সারিগুলি[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    যদি (start_col)
	*start_col = calc_col;
    যদি (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    স্থির শূন্য
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, পূর্ণাঙ্ক col, পূর্ণাঙ্ক *row, পূর্ণাঙ্ক *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_সারি_T	*সারিগুলি = (sb_সারি_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    পূর্ণাঙ্ক         calc_col = col;
    linenr_T    l;

    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	প্রত্যাগম;

    যদি (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	যাবত্ (calc_row < term->tl_scrollback.ga_len && সারিগুলি[calc_row].continuation)
	    ++calc_row;
    }
    অন্যথা
    {
	calc_row = 0;
	l = 1;
    }

    যাবত্ (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	যদি (!সারিগুলি[calc_row].continuation)
	    ++l;
    }

    যাবত্ (calc_row + 1 < term->tl_scrollback.ga_len && সারিগুলি[calc_row + 1].continuation
	    && calc_col >= সারিগুলি[calc_row].sb_cols)
    {
	calc_col -= সারিগুলি[calc_row].sb_cols;
	++calc_row;
    }

    যদি (row)
	*row = calc_row;
    যদি (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    শূন্য
write_to_term(buf_T *buffer, char_u *বার্তা, channel_T *channel)
{
    size_t	len = STRLEN(বার্তা);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    যদি (term->tl_out_fd != রিক্ত)
    {
	ch_log(channel, "Writing %d bytes to output file", (পূর্ণাঙ্ক)len);
	fwrite(বার্তা, len, 1, term->tl_out_fd);
	প্রত্যাগম;
    }
#endif

    যদি (term->tl_vterm == রিক্ত)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (পূর্ণাঙ্ক)len);
	প্রত্যাগম;
    }
    ch_log(channel, "writing %d bytes to terminal", (পূর্ণাঙ্ক)len);
    cursor_off();
    term_write_job_output(term, বার্তা, len);

#ifdef FEAT_GUI
    যদি (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    অন্যথা
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    যদি (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	যদি (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#যদি defined(FEAT_TABPANEL)
	    যদি (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    যদি (buffer == curbuf && curbuf->b_term != রিক্ত)
		update_cursor(curbuf->b_term, TRUE);
	}
	অন্যথা
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    স্থির পূর্ণাঙ্ক
term_send_mouse(VTerm *vterm, পূর্ণাঙ্ক button, পূর্ণাঙ্ক pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    পূর্ণাঙ্ক		    row = mouse_row - W_WINROW(curwin);
    পূর্ণাঙ্ক		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    যদি (popup_is_popup(curwin))
    {
	row -= popup_শীর্ষ_extra(curwin);
	col -= popup_বাম_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    যদি (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    প্রত্যাগম TRUE;
}

স্থির পূর্ণাঙ্ক enter_mouse_col = -1;
স্থির পূর্ণাঙ্ক enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    স্থির পূর্ণাঙ্ক
term_mouse_click(VTerm *vterm, পূর্ণাঙ্ক চাবি)
{
#যদি defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    স্থির পূর্ণাঙ্ক	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    যদি (state == রিক্ত)
	প্রত্যাগম FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    যদি (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	বিকল্প (চাবি)
	{
	পক্ষ K_LEFTDRAG:
	পক্ষ K_LEFTRELEASE:
	পক্ষ K_RIGHTDRAG:
	পক্ষ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		যদি (ignore_drag_release)
		{
		    পূর্ণাঙ্ক save_mouse_col, save_mouse_row;

		    যদি (enter_mouse_col < 0)
			ভঙ্গ;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_বাম, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	পক্ষ K_LEFTMOUSE:
	পক্ষ K_RIGHTMOUSE:
		যদি (চাবি == K_LEFTRELEASE || চাবি == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		অন্যথা
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		যদি (clip_star.available)
		{
		    পূর্ণাঙ্ক	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(চাবি),
							 &is_click, &is_drag);
		    যদি (mouse_model_popup() && button == MOUSE_বাম
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ডান;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		ভঙ্গ;

	পক্ষ K_MIDDLEMOUSE:
		যদি (clip_star.available)
		    ঢোকাও_reg('*', TRUE);
		ভঙ্গ;
	}
	enter_mouse_col = -1;
	প্রত্যাগম FALSE;
    }
#endif
    enter_mouse_col = -1;

    বিকল্প (চাবি)
    {
	পক্ষ K_LEFTMOUSE:
	পক্ষ K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); ভঙ্গ;
	পক্ষ K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); ভঙ্গ;
	পক্ষ K_LEFTRELEASE:
	পক্ষ K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); ভঙ্গ;
	পক্ষ K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); ভঙ্গ;
	পক্ষ K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); ভঙ্গ;
	পক্ষ K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); ভঙ্গ;
	পক্ষ K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); ভঙ্গ;
	পক্ষ K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); ভঙ্গ;
	পক্ষ K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); ভঙ্গ;
	পক্ষ K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); ভঙ্গ;
    }
    প্রত্যাগম TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    স্থির পূর্ণাঙ্ক
term_convert_চাবি(term_T *term, পূর্ণাঙ্ক c, পূর্ণাঙ্ক modmask, বর্ণ *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_চাবি	    চাবি = VTERM_চাবি_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    পূর্ণাঙ্ক		    other = FALSE;

    বিকল্প (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	পক্ষ K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    যদি (term->tl_conpty)
		চাবি = VTERM_চাবি_BACKSPACE;
	    অন্যথা
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    ভঙ্গ;

#ifdef MSWIN
	পক্ষ BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    যদি (term->tl_conpty)
		চাবি = VTERM_চাবি_BACKSPACE;
	    ভঙ্গ;
#endif

	পক্ষ ESC:		চাবি = VTERM_চাবি_ESCAPE; ভঙ্গ;
	পক্ষ K_DEL:		চাবি = VTERM_চাবি_DEL; ভঙ্গ;
	পক্ষ K_DOWN:		চাবি = VTERM_চাবি_DOWN; ভঙ্গ;
	পক্ষ K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_DOWN; ভঙ্গ;
	পক্ষ K_END:		চাবি = VTERM_চাবি_END; ভঙ্গ;
	পক্ষ K_S_END:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_END; ভঙ্গ;
	পক্ষ K_C_END:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_END; ভঙ্গ;
	পক্ষ K_F10:		চাবি = VTERM_চাবি_FUNCTION(10); ভঙ্গ;
	পক্ষ K_F11:		চাবি = VTERM_চাবি_FUNCTION(11); ভঙ্গ;
	পক্ষ K_F12:		চাবি = VTERM_চাবি_FUNCTION(12); ভঙ্গ;
	পক্ষ K_F1:		চাবি = VTERM_চাবি_FUNCTION(1); ভঙ্গ;
	পক্ষ K_F2:		চাবি = VTERM_চাবি_FUNCTION(2); ভঙ্গ;
	পক্ষ K_F3:		চাবি = VTERM_চাবি_FUNCTION(3); ভঙ্গ;
	পক্ষ K_F4:		চাবি = VTERM_চাবি_FUNCTION(4); ভঙ্গ;
	পক্ষ K_F5:		চাবি = VTERM_চাবি_FUNCTION(5); ভঙ্গ;
	পক্ষ K_F6:		চাবি = VTERM_চাবি_FUNCTION(6); ভঙ্গ;
	পক্ষ K_F7:		চাবি = VTERM_চাবি_FUNCTION(7); ভঙ্গ;
	পক্ষ K_F8:		চাবি = VTERM_চাবি_FUNCTION(8); ভঙ্গ;
	পক্ষ K_F9:		চাবি = VTERM_চাবি_FUNCTION(9); ভঙ্গ;
	পক্ষ K_HOME:		চাবি = VTERM_চাবি_HOME; ভঙ্গ;
	পক্ষ K_S_HOME:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_HOME; ভঙ্গ;
	পক্ষ K_C_HOME:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_HOME; ভঙ্গ;
	পক্ষ K_INS:		চাবি = VTERM_চাবি_INS; ভঙ্গ;
	পক্ষ K_K0:		চাবি = VTERM_চাবি_KP_0; ভঙ্গ;
	পক্ষ K_K1:		চাবি = VTERM_চাবি_KP_1; ভঙ্গ;
	পক্ষ K_K2:		চাবি = VTERM_চাবি_KP_2; ভঙ্গ;
	পক্ষ K_K3:		চাবি = VTERM_চাবি_KP_3; ভঙ্গ;
	পক্ষ K_K4:		চাবি = VTERM_চাবি_KP_4; ভঙ্গ;
	পক্ষ K_K5:		চাবি = VTERM_চাবি_KP_5; ভঙ্গ;
	পক্ষ K_K6:		চাবি = VTERM_চাবি_KP_6; ভঙ্গ;
	পক্ষ K_K7:		চাবি = VTERM_চাবি_KP_7; ভঙ্গ;
	পক্ষ K_K8:		চাবি = VTERM_চাবি_KP_8; ভঙ্গ;
	পক্ষ K_K9:		চাবি = VTERM_চাবি_KP_9; ভঙ্গ;
	পক্ষ K_KDEL:		চাবি = VTERM_চাবি_DEL; ভঙ্গ; // TODO
	পক্ষ K_KDIVIDE:		চাবি = VTERM_চাবি_KP_DIVIDE; ভঙ্গ;
	পক্ষ K_KEND:		চাবি = VTERM_চাবি_KP_1; ভঙ্গ; // TODO
	পক্ষ K_KENTER:		চাবি = VTERM_চাবি_KP_ENTER; ভঙ্গ;
	পক্ষ K_KHOME:		চাবি = VTERM_চাবি_KP_7; ভঙ্গ; // TODO
	পক্ষ K_KINS:		চাবি = VTERM_চাবি_KP_0; ভঙ্গ; // TODO
	পক্ষ K_KMINUS:		চাবি = VTERM_চাবি_KP_MINUS; ভঙ্গ;
	পক্ষ K_KMULTIPLY:	চাবি = VTERM_চাবি_KP_MULT; ভঙ্গ;
	পক্ষ K_KPAGEDOWN:	চাবি = VTERM_চাবি_KP_3; ভঙ্গ; // TODO
	পক্ষ K_KPAGEUP:		চাবি = VTERM_চাবি_KP_9; ভঙ্গ; // TODO
	পক্ষ K_KPLUS:		চাবি = VTERM_চাবি_KP_PLUS; ভঙ্গ;
	পক্ষ K_KPOINT:		চাবি = VTERM_চাবি_KP_PERIOD; ভঙ্গ;
	পক্ষ K_বাম:		চাবি = VTERM_চাবি_বাম; ভঙ্গ;
	পক্ষ K_S_বাম:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_বাম; ভঙ্গ;
	পক্ষ K_C_বাম:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_বাম; ভঙ্গ;
	পক্ষ K_PAGEDOWN:	চাবি = VTERM_চাবি_PAGEDOWN; ভঙ্গ;
	পক্ষ K_PAGEUP:		চাবি = VTERM_চাবি_PAGEUP; ভঙ্গ;
	পক্ষ K_ডান:		চাবি = VTERM_চাবি_ডান; ভঙ্গ;
	পক্ষ K_S_ডান:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_ডান; ভঙ্গ;
	পক্ষ K_C_ডান:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_ডান; ভঙ্গ;
	পক্ষ K_UP:		চাবি = VTERM_চাবি_UP; ভঙ্গ;
	পক্ষ K_S_UP:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_UP; ভঙ্গ;
	পক্ষ TAB:		চাবি = VTERM_চাবি_TAB; ভঙ্গ;
	পক্ষ K_S_TAB:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_TAB; ভঙ্গ;

	পক্ষ K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); ভঙ্গ;
	পক্ষ K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); ভঙ্গ;
	পক্ষ K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); ভঙ্গ;
	পক্ষ K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); ভঙ্গ;

	পক্ষ K_LEFTMOUSE:
	পক্ষ K_LEFTMOUSE_NM:
	পক্ষ K_LEFTDRAG:
	পক্ষ K_LEFTRELEASE:
	পক্ষ K_LEFTRELEASE_NM:
	পক্ষ K_MOUSEMOVE:
	পক্ষ K_MIDDLEMOUSE:
	পক্ষ K_MIDDLEDRAG:
	পক্ষ K_MIDDLERELEASE:
	পক্ষ K_RIGHTMOUSE:
	পক্ষ K_RIGHTDRAG:
	পক্ষ K_RIGHTRELEASE:	যদি (!term_mouse_click(vterm, c))
				    প্রত্যাগম 0;
				other = TRUE;
				ভঙ্গ;

	পক্ষ K_X1MOUSE:		/* TODO */ প্রত্যাগম 0;
	পক্ষ K_X1DRAG:		/* TODO */ প্রত্যাগম 0;
	পক্ষ K_X1RELEASE:	/* TODO */ প্রত্যাগম 0;
	পক্ষ K_X2MOUSE:		/* TODO */ প্রত্যাগম 0;
	পক্ষ K_X2DRAG:		/* TODO */ প্রত্যাগম 0;
	পক্ষ K_X2RELEASE:	/* TODO */ প্রত্যাগম 0;

	পক্ষ K_IGNORE:		প্রত্যাগম 0;
	পক্ষ K_NOP:		প্রত্যাগম 0;
	পক্ষ K_UNDO:		প্রত্যাগম 0;
	পক্ষ K_HELP:		প্রত্যাগম 0;
	পক্ষ K_XF1:		চাবি = VTERM_চাবি_FUNCTION(1); ভঙ্গ;
	পক্ষ K_XF2:		চাবি = VTERM_চাবি_FUNCTION(2); ভঙ্গ;
	পক্ষ K_XF3:		চাবি = VTERM_চাবি_FUNCTION(3); ভঙ্গ;
	পক্ষ K_XF4:		চাবি = VTERM_চাবি_FUNCTION(4); ভঙ্গ;
	পক্ষ K_SELECT:		প্রত্যাগম 0;
#ifdef FEAT_GUI
	পক্ষ K_VER_SCROLLBAR:	প্রত্যাগম 0;
	পক্ষ K_HOR_SCROLLBAR:	প্রত্যাগম 0;
#endif
#ifdef FEAT_GUI_TABLINE
	পক্ষ K_TABLINE:		প্রত্যাগম 0;
	পক্ষ K_TABMENU:		প্রত্যাগম 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	পক্ষ K_F21:		চাবি = VTERM_চাবি_FUNCTION(21); ভঙ্গ;
#endif
#ifdef FEAT_DND
	পক্ষ K_DROP:		প্রত্যাগম 0;
#endif
	পক্ষ K_CURSORHOLD:	প্রত্যাগম 0;
	পক্ষ K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				ভঙ্গ;
	পক্ষ K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				ভঙ্গ;
    }

    // add modifiers for the typed key
    যদি (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    যদি (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    যদি (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    যদি (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    যদি (চাবি != VTERM_চাবি_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_চাবি(vterm, চাবি, mod);
    অন্যথা যদি (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    প্রত্যাগম (পূর্ণাঙ্ক)vterm_output_read(vterm, buf, চাবি_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    স্থির পূর্ণাঙ্ক
term_job_running_check(term_T *term, পূর্ণাঙ্ক check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    যদি (term == রিক্ত
	|| term->tl_job == রিক্ত
	|| !channel_is_open(term->tl_job->jv_channel))
	প্রত্যাগম FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    যদি (check_job_status)
	job_status(job);
    প্রত্যাগম (job->jv_status == JOB_STARTED
	    || (job->jv_channel != রিক্ত && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    পূর্ণাঙ্ক
term_job_running(term_T *term)
{
    প্রত্যাগম term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    পূর্ণাঙ্ক
term_job_running_not_none(term_T *term)
{
    প্রত্যাগম term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    পূর্ণাঙ্ক
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    প্রত্যাগম term != রিক্ত
	&& term->tl_job != রিক্ত
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    পূর্ণাঙ্ক
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_বার্তা_SIZE];
    পূর্ণাঙ্ক	ret;

    dialog_বার্তা(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, রিক্ত, buff, 1);
    যদি (ret == VIM_YES)
	প্রত্যাগম OK;
    অন্যথা
	প্রত্যাগম FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    পূর্ণাঙ্ক
term_try_stop_job(buf_T *buf)
{
    পূর্ণাঙ্ক	    গুনতি;
    বর্ণ    *how = (বর্ণ *)buf->b_term->tl_kill;

#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    যদি ((how == রিক্ত || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	যদি (term_confirm_stop(buf) == OK)
	    how = "kill";
	অন্যথা
	    প্রত্যাগম FAIL;
    }
#endif
    যদি (how == রিক্ত || *how == NUL)
	প্রত্যাগম FAIL;

    job_stop(buf->b_term->tl_job, রিক্ত, how);

    // wait for up to a second for the job to die
    প্রতি (গুনতি = 0; গুনতি < 100; ++গুনতি)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	যদি (!buf_valid(buf)
		|| buf->b_term == রিক্ত
		|| buf->b_term->tl_job == রিক্ত)
	    প্রত্যাগম OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	যদি (job->jv_status >= JOB_ENDED)
	    প্রত্যাগম OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    প্রত্যাগম FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    স্থির শূন্য
add_scrollback_সারি_to_buffer(term_T *term, char_u *text, পূর্ণাঙ্ক len, পূর্ণাঙ্ক append)
{
    buf_T	*buf = term->tl_buffer;
    পূর্ণাঙ্ক		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_সারি_গুনতি;

#ifdef MSWIN
    char_u *tmp = text;
    যদি (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = রিক্ত;
	পূর্ণাঙ্ক	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (বর্ণ*)text, len + 1,
							   &ret, &length);
	যদি (ret != রিক্ত)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (বর্ণ **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    যদি (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	যদি (both == রিক্ত)
	    প্রত্যাগম;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    অন্যথা
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    যদি (tmp != text)
	vim_free(text);
#endif
    যদি (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    স্থির শূন্য
cell2cellattr(ধ্রুব VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    স্থির পূর্ণাঙ্ক
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    প্রত্যাগম a->fg.red == b->fg.red
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
    স্থির পূর্ণাঙ্ক
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, পূর্ণাঙ্ক lnum)
{
    যদি (ga_grow(&term->tl_scrollback, 1) == FAIL)
	প্রত্যাগম FALSE;

    sb_সারি_T *সারি = (sb_সারি_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    যদি (lnum > 0)
    {
	পূর্ণাঙ্ক i;

	প্রতি (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *সারি = *(সারি - 1);
	    --সারি;
	}
    }
    সারি->sb_cols = 0;
    সারি->sb_bytes = 0;
    সারি->sb_cells = রিক্ত;
    সারি->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    প্রত্যাগম OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    স্থির শূন্য
cleanup_scrollback(term_T *term)
{
    sb_সারি_T	*সারি;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_সারি_গুনতি, FALSE);
    bufline_length = STRLEN(bufline);
    যাবত্ (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	সারি = (sb_সারি_T *)gap->ga_data + gap->ga_len - 1;
	যদি (সারি->sb_bytes < 0 || (size_t)সারি->sb_bytes > bufline_length)
	    ভঙ্গ;
	bufline_length -= সারি->sb_bytes;
	যদি (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_সারি_গুনতি);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_সারি_গুনতি, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(সারি->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    যদি (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_সারি_গুনতি, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    যদি (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    স্থির শূন্য
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    পূর্ণাঙ্ক		    len;
    পূর্ণাঙ্ক		    সারিগুলি_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == রিক্ত ? রিক্ত : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    প্রতি (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	প্রতি (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    যদি (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    অন্যথা
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	যদি (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++সারিগুলি_skipped;
	অন্যথা
	{
	    যাবত্ (সারিগুলি_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--সারিগুলি_skipped;
		যদি (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_সারি_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    যদি (len == 0)
		p = রিক্ত;
	    অন্যথা
		p = ALLOC_MULT(cellattr_T, len);
	    যদি ((p != রিক্ত || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		পূর্ণাঙ্ক	    width;
		ধ্রুব VTerm_সারি_Info *lineinfo;
		sb_সারি_T   *সারি = (sb_সারি_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		প্রতি (pos.col = 0; pos.col < len; pos.col += width)
		{
		    যদি (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			যদি (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    অন্যথা
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			যদি (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			যদি (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    পূর্ণাঙ্ক	    i;
			    পূর্ণাঙ্ক	    c;

			    প্রতি (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		সারি->sb_cols = len;
		সারি->sb_bytes = ga.ga_len;
		সারি->sb_cells = p;
		সারি->sb_fill_attr = new_fill_attr;
		সারি->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		যদি (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_সারি_to_buffer(term, (char_u *)"", 0, 0);
		অন্যথা
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_সারি_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    অন্যথা
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    প্রতি (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	যদি (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_সারি_to_buffer(term, (char_u *)"", 0, 0);
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
    স্থির পূর্ণাঙ্ক
for_all_windows_and_curwin(win_T **wp, পূর্ণাঙ্ক *did_curwin)
{
    যদি (*wp == রিক্ত)
	*wp = firstwin;
    অন্যথা যদি ((*wp)->w_next != রিক্ত)
	*wp = (*wp)->w_next;
    অন্যথা যদি (!*did_curwin)
	*wp = curwin;
    অন্যথা
	প্রত্যাগম FALSE;
    যদি (*wp == curwin)
	*did_curwin = TRUE;
    প্রত্যাগম TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    শূন্য
may_move_terminal_to_buffer(term_T *term, পূর্ণাঙ্ক redraw)
{
    যদি (term->tl_vterm == রিক্ত)
	প্রত্যাগম;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    যদি (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_সারি_গুনতি
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    যদি (redraw)
    {
	win_T	    *wp = রিক্ত;
	পূর্ণাঙ্ক	    did_curwin = FALSE;

	যাবত্ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    যদি (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_সারি_গুনতি;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		যদি (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    যদি (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#যদি defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    পূর্ণাঙ্ক
term_check_timers(পূর্ণাঙ্ক next_due_arg, proftime_T *now)
{
    term_T  *term;
    পূর্ণাঙ্ক	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	যদি (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_বাম(&term->tl_timer_due, now);

	    যদি (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    অন্যথা যদি (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    প্রত্যাগম next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    স্থির শূন্য
set_terminal_mode(term_T *term, পূর্ণাঙ্ক normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    যদি (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    যদি (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    স্থির শূন্য
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    যদি (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    স্থির শূন্য
term_enter_normal_mode(শূন্য)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    পূর্ণাঙ্ক        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, রিক্ত);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    যদি (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    পূর্ণাঙ্ক
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    প্রত্যাগম term != রিক্ত && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    শূন্য
term_enter_job_mode(শূন্য)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    যদি (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    যদি (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
প্রকারনির্দেশ গণনা {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_চাবি_state_T;

স্থির reduce_চাবি_state_T  no_reduce_চাবি_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    স্থির পূর্ণাঙ্ক
vterm_using_চাবি_protocol(শূন্য)
{
    প্রত্যাগম curbuf->b_term != রিক্ত
	&& curbuf->b_term->tl_vterm != রিক্ত
	&& (vterm_is_modify_other_চাবিগুলি(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    শূন্য
check_no_reduce_চাবিগুলি(শূন্য)
{
    যদি (no_reduce_চাবি_state != NRKS_CHECK
	    || no_reduce_চাবিগুলি >= 1
	    || curbuf->b_term == রিক্ত
	    || curbuf->b_term->tl_vterm == রিক্ত)
	প্রত্যাগম;

    যদি (vterm_using_চাবি_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_চাবি_state = NRKS_SET;
	++no_reduce_চাবিগুলি;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    স্থির পূর্ণাঙ্ক
term_vgetc(শূন্য)
{
    পূর্ণাঙ্ক c;
    পূর্ণাঙ্ক save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    যদি (vterm_using_চাবি_protocol())
    {
	++no_reduce_চাবিগুলি;
	no_reduce_চাবি_state = NRKS_SET;
    }
    অন্যথা
    {
	no_reduce_চাবি_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    যদি (no_reduce_চাবি_state == NRKS_SET)
	--no_reduce_চাবিগুলি;
    no_reduce_চাবি_state = NRKS_NONE;

    প্রত্যাগম c;
}

স্থির পূর্ণাঙ্ক	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    পূর্ণাঙ্ক
send_চাবিগুলি_to_term(term_T *term, পূর্ণাঙ্ক c, পূর্ণাঙ্ক modmask, পূর্ণাঙ্ক typed)
{
    বর্ণ	বার্তা[চাবি_BUF_LEN];
    size_t	len;
    পূর্ণাঙ্ক		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    বিকল্প (c)
    {
	পক্ষ NUL:
	পক্ষ K_ZERO:
	    যদি (typed)
		stuffcharReadbuff(c);
	    প্রত্যাগম FAIL;

	পক্ষ K_TABLINE:
	    stuffcharReadbuff(c);
	    প্রত্যাগম FAIL;

	পক্ষ K_IGNORE:
	পক্ষ K_CANCEL:  // used for :normal when running out of chars
	    প্রত্যাগম FAIL;

	পক্ষ K_LEFTDRAG:
	পক্ষ K_MIDDLEDRAG:
	পক্ষ K_RIGHTDRAG:
	পক্ষ K_X1DRAG:
	পক্ষ K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	পক্ষ K_LEFTMOUSE:
	পক্ষ K_LEFTMOUSE_NM:
	পক্ষ K_LEFTRELEASE:
	পক্ষ K_LEFTRELEASE_NM:
	পক্ষ K_MOUSEMOVE:
	পক্ষ K_MIDDLEMOUSE:
	পক্ষ K_MIDDLERELEASE:
	পক্ষ K_RIGHTMOUSE:
	পক্ষ K_RIGHTRELEASE:
	পক্ষ K_X1MOUSE:
	পক্ষ K_X1RELEASE:
	পক্ষ K_X2MOUSE:
	পক্ষ K_X2RELEASE:

	পক্ষ K_MOUSEUP:
	পক্ষ K_MOUSEDOWN:
	পক্ষ K_MOUSELEFT:
	পক্ষ K_MOUSERIGHT:
	    {
		পূর্ণাঙ্ক	row = mouse_row;
		পূর্ণাঙ্ক	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		যদি (popup_is_popup(curwin))
		{
		    row -= popup_শীর্ষ_extra(curwin);
		    col -= popup_বাম_extra(curwin);
		}
#endif
		যদি (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    যদি (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    প্রত্যাগম FAIL;
		}
	    }
	    ভঙ্গ;

	পক্ষ K_COMMAND:
	পক্ষ K_SCRIPT_COMMAND:
	    প্রত্যাগম do_cmdkey_command(c, 0);
    }
    যদি (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_চাবি(term, c, modmask, বার্তা);
    যদি (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)বার্তা, (পূর্ণাঙ্ক)len, রিক্ত);

    প্রত্যাগম OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    স্থির শূন্য
term_paste_register(পূর্ণাঙ্ক prev_c UNUSED)
{
    পূর্ণাঙ্ক		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    পূর্ণাঙ্ক		type;

    যদি (add_to_showcmd(prev_c))
    যদি (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    যদি (!term_use_loop())
	// job finished while waiting for a character
	প্রত্যাগম;

    // CTRL-W "= prompt for expression to evaluate.
    যদি (c == '=' && get_রাশিমালা_register() != '=')
	প্রত্যাগম;
    যদি (!term_use_loop())
	// job finished while waiting for a character
	প্রত্যাগম;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    যদি (l == রিক্ত)
	প্রত্যাগম;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_বস্তুসমূহ(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	যদি (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = রিক্ত;
	    পূর্ণাঙ্ক	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (বর্ণ *)s,
		    (পূর্ণাঙ্ক)STRLEN(s), &ret, &length);
	    যদি (ret != রিক্ত)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (বর্ণ **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (পূর্ণাঙ্ক)STRLEN(s), রিক্ত);
#ifdef MSWIN
	যদি (tmp != s)
	    vim_free(s);
#endif

	যদি (item->li_next != রিক্ত || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, রিক্ত);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    পূর্ণাঙ্ক
terminal_is_active(শূন্য)
{
    প্রত্যাগম in_terminal_loop != রিক্ত;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    স্থির পূর্ণাঙ্ক
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    যদি (wp != রিক্ত && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    অন্যথা যদি (term->tl_highlight_name != রিক্ত)
	name = term->tl_highlight_name;
    অন্যথা
	name = (char_u*)"Terminal";

    প্রত্যাগম syn_name2id(name);
}

#যদি defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    স্থির cursorentry_T entry;
    পূর্ণাঙ্ক			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_বাম ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    যদি (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    যদি (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    যদি (term_bg != INVALCOLOR)
	*fg = term_bg;
    অন্যথা
	*fg = gui.back_pixel;

    যদি (term->tl_cursor_color == রিক্ত)
    {
	যদি (term_fg != INVALCOLOR)
	    *bg = term_fg;
	অন্যথা
	    *bg = gui.norm_pixel;
    }
    অন্যথা
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    প্রত্যাগম &entry;
}
#endif

    স্থির শূন্য
may_output_cursor_props(শূন্য)
{
    যদি (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	যদি (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	অন্যথা
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    স্থির শূন্য
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    যদি (gui.in_use)
	প্রত্যাগম;
#endif
    যদি (in_terminal_loop == term)
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
    স্থির শূন্য
prepare_restore_cursor_props(শূন্য)
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
	প্রত্যাগম;
#endif
    cursor_color_copy(&desired_cursor_color, রিক্ত);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    স্থির পূর্ণাঙ্ক
term_use_loop_check(পূর্ণাঙ্ক check_job_status)
{
    term_T *term = curbuf->b_term;

    প্রত্যাগম term != রিক্ত
	&& !term->tl_normal_mode
	&& term->tl_vterm != রিক্ত
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    পূর্ণাঙ্ক
term_use_loop(শূন্য)
{
    প্রত্যাগম term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    শূন্য
term_win_entered(শূন্য)
{
    term_T *term = curbuf->b_term;

    যদি (term == রিক্ত)
	প্রত্যাগম;

    যদি (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	যদি (State & MODE_ঢোকাও)
	    stop_ঢোকাও_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    শূন্য
term_focus_change(পূর্ণাঙ্ক in_focus)
{
    term_T *term = curbuf->b_term;

    যদি (term == রিক্ত || term->tl_vterm == রিক্ত)
	প্রত্যাগম;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    যদি (in_focus)
	vterm_state_focus_in(state);
    অন্যথা
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    স্থির পূর্ণাঙ্ক
raw_c_to_ctrl(পূর্ণাঙ্ক c)
{
    যদি ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	প্রত্যাগম c & 0x1f;
    প্রত্যাগম c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    স্থির পূর্ণাঙ্ক
ctrl_to_raw_c(পূর্ণাঙ্ক c)
{
    যদি (c < 0x20 && vterm_using_চাবি_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	প্রত্যাগম c + '@';
    }
    প্রত্যাগম c;
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
    পূর্ণাঙ্ক
terminal_loop(পূর্ণাঙ্ক blocking)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		raw_c;
    পূর্ণাঙ্ক		termwinkey = 0;
    পূর্ণাঙ্ক		ret;
#ifdef UNIX
    পূর্ণাঙ্ক		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    পূর্ণাঙ্ক		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    যদি (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_চাবি(curwin->w_p_twk, TRUE);
	যদি (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    যাবত্ (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	যদি (curbuf->b_term != রিক্ত && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    যাবত্ (must_redraw != 0)
		যদি (update_screen(0) == FAIL)
		    ভঙ্গ;
	যদি (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    ভঙ্গ;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	যদি (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    যদি (raw_c != K_IGNORE)
		vungetc(raw_c);
	    ভঙ্গ;
	}
	যদি (raw_c == K_IGNORE)
	    অনুবর্ত;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	যদি (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    যদি (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	যদি (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	যদি ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    পূর্ণাঙ্ক	    prev_c = c;
	    পূর্ণাঙ্ক	    prev_raw_c = raw_c;
	    পূর্ণাঙ্ক	    prev_mod_mask = mod_mask;

	    যদি (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    যদি (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		ভঙ্গ;

	    যদি (prev_c == Ctrl_BSL)
	    {
		যদি (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    গচ্ছ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_চাবিগুলি_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    অন্যথা যদি (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    অন্যথা যদি (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    অন্যথা যদি (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    অন্যথা যদি (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		গচ্ছ theend;
	    }
	    অন্যথা যদি (c == '"')
	    {
		term_paste_register(prev_c);
		অনুবর্ত;
	    }
	    অন্যথা যদি (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		গচ্ছ theend;
	    }
	}
#ifdef MSWIN
	যদি (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    যদি (MultiByteToWideChar(GetACP(), 0, (বর্ণ*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	যদি (send_চাবিগুলি_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    যদি (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    গচ্ছ theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = রিক্ত;
    যদি (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    যদি (curbuf->b_term != রিক্ত && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    প্রত্যাগম ret;
}

    স্থির শূন্য
may_toggle_cursor(term_T *term)
{
    যদি (in_terminal_loop != term)
	প্রত্যাগম;

    যদি (term->tl_cursor_visible)
	cursor_on();
    অন্যথা
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    স্থির পূর্ণাঙ্ক
color2index(VTermColor *color, পূর্ণাঙ্ক fg, পূর্ণাঙ্ক *boldp)
{
    পূর্ণাঙ্ক red = color->red;
    পূর্ণাঙ্ক blue = color->blue;
    পূর্ণাঙ্ক green = color->green;

    *boldp = FALSE;

    যদি (VTERM_COLOR_IS_INVALID(color))
	প্রত্যাগম 0;

    যদি (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	যদি (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    যদি (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    প্রত্যাগম index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	অন্যথা যদি (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    প্রত্যাগম (color->index & 7) + 1;
	}
	প্রত্যাগম 0;
    }

    যদি (t_colors >= 256)
    {
	যদি (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    স্থির পূর্ণাঙ্ক cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    পূর্ণাঙ্ক i;

	    যদি (red < 5)
		প্রত্যাগম 17; // 00/00/00
	    যদি (red > 245) // ff/ff/ff
		প্রত্যাগম 232;
	    প্রতি (i = 0; i < 23; ++i)
		যদি (red < cutoff[i])
		    প্রত্যাগম i + 233;
	    প্রত্যাগম 256;
	}
	{
	    স্থির পূর্ণাঙ্ক cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    পূর্ণাঙ্ক ri, gi, bi;

	    // 216-color cube
	    প্রতি (ri = 0; ri < 5; ++ri)
		যদি (red < cutoff[ri])
		    ভঙ্গ;
	    প্রতি (gi = 0; gi < 5; ++gi)
		যদি (green < cutoff[gi])
		    ভঙ্গ;
	    প্রতি (bi = 0; bi < 5; ++bi)
		যদি (blue < cutoff[bi])
		    ভঙ্গ;
	    প্রত্যাগম 17 + ri * 36 + gi * 6 + bi;
	}
    }
    প্রত্যাগম 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    স্থির পূর্ণাঙ্ক
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    পূর্ণাঙ্ক attr = 0;

    যদি (cellattrs->bold)
	attr |= HL_BOLD;
    যদি (cellattrs->underline)
	attr |= HL_UNDERLINE;
    যদি (cellattrs->italic)
	attr |= HL_ITALIC;
    যদি (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    যদি (cellattrs->reverse)
	attr |= HL_INVERSE;
    প্রত্যাগম attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    স্থির শূন্য
hl2vtermAttr(পূর্ণাঙ্ক attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    যদি (attr & HL_BOLD)
	cell->attrs.bold = 1;
    যদি (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    যদি (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    যদি (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    যদি (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    স্থির পূর্ণাঙ্ক
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    পূর্ণাঙ্ক attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    পূর্ণাঙ্ক is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    পূর্ণাঙ্ক is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    যদি (is_default_fg || is_default_bg)
    {
	যদি (wp != রিক্ত && wp->w_hlfwin_id != 0)
	{
	    যদি (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    যদি (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	অন্যথা
	{
	    যদি (is_default_fg)
		fg = &term->tl_default_color.fg;
	    যদি (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	প্রত্যাগম get_gui_attr_idx(attr, guifg, guibg);
    }
    অন্যথা
#endif
#ifdef FEAT_TERMGUICOLORS
    যদি (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	প্রত্যাগম get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    অন্যথা
#endif
    {
	পূর্ণাঙ্ক bold = MAYBE;
	পূর্ণাঙ্ক ctermfg = color2index(fg, TRUE, &bold);
	পূর্ণাঙ্ক ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	যদি (bold == TRUE)
	    attr |= HL_BOLD;

	প্রত্যাগম get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    প্রত্যাগম 0;
}

    স্থির শূন্য
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    যদি (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    স্থির পূর্ণাঙ্ক
handle_damage(VTermRect rect, শূন্য *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    প্রত্যাগম 1;
}

    স্থির শূন্য
term_scroll_up(term_T *term, পূর্ণাঙ্ক start_row, পূর্ণাঙ্ক গুনতি)
{
    win_T		 *wp = রিক্ত;
    পূর্ণাঙ্ক			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    পূর্ণাঙ্ক			 clear_attr;

    CLEAR_FIELD(attr);

    যাবত্ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	যদি (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_সারিগুলি(wp, start_row, গুনতি, FALSE, FALSE, clear_attr);
	}
    }
}

    স্থির পূর্ণাঙ্ক
handle_moverect(VTermRect dest, VTermRect src, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    পূর্ণাঙ্ক		গুনতি = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    যদি (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	যদি (dest.start_row == 0)
	    term->tl_postponed_scroll += গুনতি;
	অন্যথা
	    term_scroll_up(term, dest.start_row, গুনতি);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    প্রত্যাগম 1;
}

    স্থির পূর্ণাঙ্ক
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	পূর্ণাঙ্ক visible,
	শূন্য *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = রিক্ত;
    পূর্ণাঙ্ক		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    যাবত্ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	যদি (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    যদি (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    প্রত্যাগম 1;
}

    স্থির পূর্ণাঙ্ক
handle_settermprop(
	VTermProp prop,
	VTerm_মান *মান,
	শূন্য *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = রিক্ত;

    বিকল্প (prop)
    {
	পক্ষ VTERM_PROP_TITLE:
	    যদি (disable_vterm_title_for_testing)
		ভঙ্গ;
	    strval = vim_strnsave((char_u *)মান->string.str,
							    মান->string.len);
	    যদি (strval == রিক্ত)
		ভঙ্গ;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    যদি (*skipwhite(strval) == NUL)
		term->tl_title = রিক্ত;
	    // Same as blank
	    অন্যথা যদি (term->tl_arg0_cmd != রিক্ত
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (পূর্ণাঙ্ক)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = রিক্ত;
	    // Empty corrupted data of winpty
	    অন্যথা যদি (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = রিক্ত;
#ifdef MSWIN
	    অন্যথা যদি (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = রিক্ত;
		পূর্ণাঙ্ক	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(বর্ণ*)মান->string.str,
					(পূর্ণাঙ্ক)মান->string.len, &ret, &length);
		যদি (ret != রিক্ত)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (বর্ণ**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    অন্যথা
	    {
		term->tl_title = strval;
		strval = রিক্ত;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    যদি (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    ভঙ্গ;

	পক্ষ VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = মান->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    ভঙ্গ;

	পক্ষ VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = মান->boolean;
	    may_set_cursor_props(term);
	    ভঙ্গ;

	পক্ষ VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = মান->সংখ্যা;
	    may_set_cursor_props(term);
	    ভঙ্গ;

	পক্ষ VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)মান->string.str,
							    মান->string.len);
	    যদি (strval == রিক্ত)
		ভঙ্গ;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    ভঙ্গ;

	পক্ষ VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = মান->boolean;
	    ভঙ্গ;

	স্বভাব:
	    ভঙ্গ;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    প্রত্যাগম 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    স্থির পূর্ণাঙ্ক
handle_resize(পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    যদি (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    অন্যথা
    {
	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    প্রত্যাগম 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    স্থির শূন্য
limit_scrollback(term_T *term, garray_T *gap, পূর্ণাঙ্ক update_buffer)
{
    যদি (gap->ga_len < term->tl_buffer->b_p_twsl)
	প্রত্যাগম;

    পূর্ণাঙ্ক	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    পূর্ণাঙ্ক	i;
    sb_সারি_T *sb_সারিগুলি = (sb_সারি_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    প্রতি (i = 0; i < todo; ++i)
    {
	যদি (update_buffer && (!sb_সারিগুলি[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_সারিগুলি[i].sb_cells);
    }
    // Continue until end of wrapped line
    প্রতি (; todo < gap->ga_len && sb_সারিগুলি[todo].continuation; ++todo)
	vim_free(sb_সারিগুলি[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_সারি_T *)gap->ga_data + todo,
		পরিমাণ(sb_সারি_T) * gap->ga_len);
    যদি (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = রিক্ত;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_buffer == term->tl_buffer)
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
    স্থির পূর্ণাঙ্ক
handle_pushline(পূর্ণাঙ্ক cols, ধ্রুব VTermScreenCell *cells, পূর্ণাঙ্ক continuation, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    পূর্ণাঙ্ক		update_buffer;

    যদি (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    অন্যথা
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    যদি (ga_grow(gap, 1) == FAIL)
	প্রত্যাগম 0;

    cellattr_T	*p = রিক্ত;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		text_len;
    char_u		*text;
    sb_সারি_T	*সারি;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    প্রতি (i = 0; i < cols; ++i)
	যদি (cells[i].chars[0] != 0)
	    len = i + 1;
	অন্যথা
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    যদি (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    যদি (p != রিক্ত)
    {
	প্রতি (col = 0; col < len; col += cells[col].width)
	{
	    যদি (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		ভঙ্গ;
	    }
	    প্রতি (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    যদি (ga_grow(&ga, 1) == FAIL)
    {
	যদি (update_buffer)
	    text = (char_u *)"";
	অন্যথা
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    অন্যথা
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    যদি (update_buffer)
	add_scrollback_সারি_to_buffer(term, text, text_len, continuation);

    সারি = (sb_সারি_T *)gap->ga_data + gap->ga_len;
    সারি->sb_cols = len;
    সারি->sb_bytes = text_len;
    সারি->sb_cells = p;
    সারি->sb_fill_attr = fill_attr;
    সারি->continuation = (char_u)continuation;
    যদি (update_buffer)
    {
	সারি->sb_text = রিক্ত;
	++term->tl_scrollback_scrolled;
	যদি (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    অন্যথা
    {
	সারি->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    প্রত্যাগম 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    স্থির শূন্য
handle_postponed_scrollback(term_T *term)
{
    পূর্ণাঙ্ক i;

    যদি (term->tl_scrollback_postponed.ga_len == 0)
	প্রত্যাগম;
    ch_log(রিক্ত, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    প্রতি (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_সারি_T	*pp_সারি;
	sb_সারি_T	*সারি;

	যদি (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    ভঙ্গ;
	pp_সারি = (sb_সারি_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_সারি->sb_text;
	যদি (text == রিক্ত)
	    text = (char_u *)"";
	add_scrollback_সারি_to_buffer(term, text, (পূর্ণাঙ্ক)STRLEN(text), pp_সারি->continuation);
	vim_free(pp_সারি->sb_text);

	সারি = (sb_সারি_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	সারি->sb_cols = pp_সারি->sb_cols;
	সারি->sb_bytes = pp_সারি->sb_bytes;
	সারি->sb_cells = pp_সারি->sb_cells;
	সারি->sb_fill_attr = pp_সারি->sb_fill_attr;
	সারি->sb_text = রিক্ত;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	যদি (!pp_সারি->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    স্থির পূর্ণাঙ্ক
handle_bell(শূন্য *user UNUSED)
{
    vim_beep(BO_TERM);
    প্রত্যাগম 0;
}

স্থির VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    রিক্ত,			// sb_pushline
    রিক্ত,			// sb_popline
    রিক্ত,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    স্থির পূর্ণাঙ্ক
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    যদি (!term->tl_normal_mode)
    {
	পূর্ণাঙ্ক	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	যদি (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    পূর্ণাঙ্ক		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = রিক্ত;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    যদি (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		যদি (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    অন্যথা
#endif
	    // If this is the last normal window: exit Vim.
	    যদি (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		প্রত্যাগম TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(রিক্ত, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    যদি (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		যদি (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		যদি (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		যদি (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    যদি (pwin != রিক্ত)
		popup_close_with_retval(pwin, 0);
#endif
	    প্রত্যাগম TRUE;
	}
	যদি (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    বর্ণ    *cmd = term->tl_opencmd == রিক্ত
				? "botright sbuf %d"
				: (বর্ণ *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    বর্ণ    *buf = alloc(len);

	    যদি (buf != রিক্ত)
	    {
		ch_log(রিক্ত, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	অন্যথা
	    ch_log(রিক্ত, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    প্রত্যাগম FALSE;
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    পূর্ণাঙ্ক
may_close_term_popup(শূন্য)
{
    যদি (!popup_is_popup(curwin) || curbuf->b_term == রিক্ত
				 || term_job_running_not_none(curbuf->b_term))
	প্রত্যাগম FAIL;

    win_T *pwin = curwin;

    যদি (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    প্রত্যাগম OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    শূন্য
term_channel_closing(channel_T *ch)
{
    term_T *term;

    প্রতি (term = first_term; term != রিক্ত; term = term->tl_next)
	যদি (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    শূন্য
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    পূর্ণাঙ্ক	    did_one = FALSE;

    প্রতি (term = first_term; term != রিক্ত; term = next_term)
    {
	next_term = term->tl_next;
	যদি (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    যদি (term->tl_out_fd != রিক্ত)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = রিক্ত;
	    }
#endif

	    যদি (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		অনুবর্ত;
	    }

	    যদি (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    যদি (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	যদি (term != রিক্ত)
	{
	    যদি (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    শূন্য
term_check_channel_closed_recently(শূন্য)
{
    term_T *term;
    term_T *next_term;

    প্রতি (term = first_term; term != রিক্ত; term = next_term)
    {
	next_term = term->tl_next;
	যদি (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    যদি (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    স্থির শূন্য
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	পূর্ণাঙ্ক		max_col)
{
    পূর্ণাঙ্ক off = screen_get_current_সারি_off();

    প্রতি (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	পূর্ণাঙ্ক		c;

	যদি (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	যদি (c == NUL)
	{
	    Screen_সারিগুলি[off] = ' ';
	    যদি (enc_utf8)
		Screen_সারিগুলি_UC[off] = NUL;
	}
	অন্যথা
	{
	    যদি (enc_utf8)
	    {
		পূর্ণাঙ্ক i;

		// composing chars
		প্রতি (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_সারিগুলি_C[i][off] = cell.chars[i + 1];
		    যদি (cell.chars[i + 1] == 0)
			ভঙ্গ;
		}
		যদি (c >= 0x80 || (Screen_mco > 0
					 && Screen_সারিগুলি_C[0][off] != 0))
		{
		    Screen_সারিগুলি[off] = ' ';
		    Screen_সারিগুলি_UC[off] = c;
		}
		অন্যথা
		{
		    Screen_সারিগুলি[off] = c;
		    Screen_সারিগুলি_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    অন্যথা যদি (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		যদি (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (বর্ণ*)mb, 2, 0, 0) > 1)
		{
		    Screen_সারিগুলি[off] = mb[0];
		    Screen_সারিগুলি[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		অন্যথা
		    Screen_সারিগুলি[off] = c;
	    }
#endif
	    অন্যথা
		// This will only store the lower byte of "c".
		Screen_সারিগুলি[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	যদি (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    যদি (enc_utf8)
	    {
		Screen_সারিগুলি_UC[off] = NUL;
		Screen_সারিগুলি[off] = NUL;
	    }
	    অন্যথা যদি (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_সারিগুলি[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#যদি defined(FEAT_GUI)
    স্থির শূন্য
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    যদি (term->tl_vterm == রিক্ত)
	প্রত্যাগম;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    যাবত্ (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	পূর্ণাঙ্ক save_p_more = p_more;

	p_more = FALSE;
	বার্তা_row = Rows - 1;
	বার্তা_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    প্রতি (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	যদি (pos.row < term->tl_rows)
	{
	    পূর্ণাঙ্ক max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, রিক্ত, screen, &pos, max_col);
	}
	অন্যথা
	    pos.col = 0;

	screen_সারি(curwin, term->tl_toprow + pos.row,
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
    পূর্ণাঙ্ক
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    প্রত্যাগম term != রিক্ত && term->tl_vterm != রিক্ত && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    শূন্য
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    পূর্ণাঙ্ক		rows, cols;
    পূর্ণাঙ্ক		newrows, newcols;
    পূর্ণাঙ্ক		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    যদি (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	যদি (term->tl_postponed_scroll > 0
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
    প্রতি (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == রিক্ত ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	যদি (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	যদি (twp == রিক্ত)
	    ভঙ্গ;
    }
    যদি (newrows == 99999 || newcols == 99999)
	প্রত্যাগম; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    যদি (newrows == 0 || newcols == 0)
	প্রত্যাগম;

    যদি (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	যদি (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    প্রতি (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	যদি (pos.row < term->tl_rows)
	{
	    পূর্ণাঙ্ক max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	অন্যথা
	    pos.col = 0;

	screen_সারি(wp, wp->w_winrow + pos.row
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
    শূন্য
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    যদি (term == রিক্ত || term->tl_vterm == রিক্ত || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	প্রত্যাগম;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    পূর্ণাঙ্ক
term_is_finished(buf_T *buf)
{
    প্রত্যাগম buf->b_term != রিক্ত && buf->b_term->tl_vterm == রিক্ত;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    পূর্ণাঙ্ক
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    প্রত্যাগম term != রিক্ত && (term->tl_vterm == রিক্ত || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    শূন্য
term_change_in_curbuf(শূন্য)
{
    term_T *term = curbuf->b_term;

    যদি (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	প্রত্যাগম;

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
    পূর্ণাঙ্ক
term_get_attr(win_T *wp, linenr_T lnum, পূর্ণাঙ্ক col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_সারি_T	*সারি;
    cellattr_T	*cellattr;
    পূর্ণাঙ্ক         sb_সারি = -1;
    পূর্ণাঙ্ক         sb_col = col;

    যদি (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_সারি, &sb_col);

    যদি (sb_সারি < 0)
	cellattr = &term->tl_default_color;
    অন্যথা
    {
	সারি = (sb_সারি_T *)term->tl_scrollback.ga_data + sb_সারি;
	যদি (sb_col < 0 || sb_col >= সারি->sb_cols)
	    cellattr = &সারি->sb_fill_attr;
	অন্যথা
	    cellattr = সারি->sb_cells + sb_col;
    }
    প্রত্যাগম cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    স্থির শূন্য
cterm_color2vterm(পূর্ণাঙ্ক nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    যদি (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    অন্যথা
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
    স্থির পূর্ণাঙ্ক
get_vterm_color_from_synid(পূর্ণাঙ্ক id, VTermColor *fg, VTermColor *bg)
{
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    যদি (0
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

	যদি (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	যদি (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	অন্যথা
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	যদি (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	অন্যথা
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	প্রত্যাগম TRUE;
    }
    অন্যথা
#endif
    যদি (t_colors >= 16)
    {
	পূর্ণাঙ্ক cterm_fg = -1;
	পূর্ণাঙ্ক cterm_bg = -1;

	যদি (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	যদি (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	অন্যথা
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	যদি (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	অন্যথা
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	প্রত্যাগম TRUE;
    }

    প্রত্যাগম FALSE;
}

    শূন্য
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    শূন্য
term_update_hlfwin(win_T *wp)
{
    পূর্ণাঙ্ক id = wp->w_hlfwin_id;

    যদি (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    শূন্য
term_update_hlfwin_all(শূন্য)
{
    win_T	 *wp = রিক্ত;
    পূর্ণাঙ্ক		 did_curwin = FALSE;

    যাবত্ (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    শূন্য
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    পূর্ণাঙ্ক		    fgval, bgval;
    পূর্ণাঙ্ক		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    যদি (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    অন্যথা
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, রিক্ত);

    যদি (!get_vterm_color_from_synid(id, fg, bg))
    {
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	পূর্ণাঙ্ক tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	যদি (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    যদি (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	অন্যথা
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	যদি (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    যদি (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	অন্যথা
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    স্থির পূর্ণাঙ্ক
term_use_palette(শূন্য)
{
    যদি (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    স্থির শূন্য
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    পূর্ণাঙ্ক		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    প্রতি (; index < 16; index++)
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
    স্থির পূর্ণাঙ্ক
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    পূর্ণাঙ্ক		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    প্রতি (li = list->lv_first; li != রিক্ত && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	যদি (color_name == রিক্ত)
	    প্রত্যাগম FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	যদি (guicolor == INVALCOLOR)
	    প্রত্যাগম FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    যদি (n != 16 || li != রিক্ত)
	প্রত্যাগম FAIL;

    set_vterm_palette(vterm, rgb);

    প্রত্যাগম OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    স্থির শূন্য
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", রিক্ত, TRUE);

    যদি (var == রিক্ত)
	প্রত্যাগম;

    যদি (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == রিক্ত
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    স্থির শূন্য
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    পূর্ণাঙ্ক		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = রিক্ত;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	যদি (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    প্রত্যাগম;
	}
    }

    CLEAR_FIELD(ea);

    যদি (opt_item != রিক্ত && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != রিক্ত)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	যদি (p == রিক্ত)
	    p = dict_get_string(dict, "fileformat", FALSE);
	যদি (p != রিক্ত)
	{
	    যদি (check_ff_মান(p) == FAIL)
		ch_log(রিক্ত, "Invalid ff argument to drop: %s", p);
	    অন্যথা
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	যদি (p == রিক্ত)
	    p = dict_get_string(dict, "encoding", FALSE);
	যদি (p != রিক্ত)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    যদি (ea.cmd != রিক্ত)
	    {
		sprintf((বর্ণ *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	যদি (p != রিক্ত)
	    get_bad_opt(p, &ea);

	যদি (dict_has_চাবি(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	যদি (dict_has_চাবি(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	যদি (dict_has_চাবি(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	যদি (dict_has_চাবি(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    যদি (ea.cmd == রিক্ত)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    স্থির পূর্ণাঙ্ক
is_permitted_term_api(char_u *func, char_u *pat)
{
    প্রত্যাগম pat != রিক্ত && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    স্থির শূন্য
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    যদি (item->li_next == রিক্ত)
    {
	ch_log(channel, "Missing function arguments for call");
	প্রত্যাগম;
    }
    func = tv_get_string(&item->li_tv);

    যদি (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	প্রত্যাগম;
    }

    argvars[0].v_type = VAR_সংখ্যা;
    argvars[0].vval.v_সংখ্যা = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    যদি (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    অন্যথা
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
    স্থির size_t
url_decode(ধ্রুব বর্ণ *src, ধ্রুব size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    যাবত্ (i < len)
    {
	যদি (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	অন্যথা
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    প্রত্যাগম j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    স্থির শূন্য
sync_shell_dir(garray_T *gap)
{
    পূর্ণাঙ্ক       offset = 7;  // len of "file://" is 7
    বর্ণ      *pos = (বর্ণ *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    যাবত্ (offset < (পূর্ণাঙ্ক)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    যদি (offset >= (পূর্ণাঙ্ক)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	প্রত্যাগম;
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
    স্থির পূর্ণাঙ্ক
parse_osc(পূর্ণাঙ্ক command, VTermStringFragment frag, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == রিক্ত ? রিক্ত
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    যদি (command != 51 && (command != 7 || !p_asd))
	প্রত্যাগম 0;

    // Concatenate what was received until the final piece is found.
    যদি (ga_grow(gap, (পূর্ণাঙ্ক)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	প্রত্যাগম 1;
    }
    mch_memmove((বর্ণ *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (পূর্ণাঙ্ক)frag.len;
    যদি (!frag.final)
	প্রত্যাগম 1;

    ((বর্ণ *)gap->ga_data)[gap->ga_len] = 0;

    যদি (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	প্রত্যাগম 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = রিক্ত;
    reader.js_used = 0;
    যদি (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != রিক্ত)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	যদি (item == রিক্ত)
	    ch_log(channel, "Missing command");
	অন্যথা
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    যদি (item == রিক্ত)
		ch_log(channel, "Missing argument for %s", cmd);
	    অন্যথা যদি (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    অন্যথা যদি (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    অন্যথা
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    অন্যথা
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    প্রত্যাগম 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    স্থির পূর্ণাঙ্ক
parse_csi(
	ধ্রুব বর্ণ  *leader UNUSED,
	ধ্রুব long  args[],
	পূর্ণাঙ্ক	    argcount,
	ধ্রুব বর্ণ  *intermed UNUSED,
	বর্ণ	    command,
	শূন্য	    *user)
{
    term_T	*term = (term_T *)user;
    বর্ণ	buf[100];
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		x = 0;
    পূর্ণাঙ্ক		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    যদি (command != 't' || argcount != 1 || args[0] != 13)
	প্রত্যাগম 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#যদি defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (শূন্য)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_buffer == term->tl_buffer)
	    ভঙ্গ;
    যদি (wp != রিক্ত)
    {
#ifdef FEAT_GUI
	যদি (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	অন্যথা
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
						     (char_u *)buf, len, রিক্ত);
    প্রত্যাগম 1;
}

স্থির VTermStateFallbacks state_fallbacks = {
    রিক্ত,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    রিক্ত,		// dcs
    রিক্ত,		// apc
    রিক্ত,		// pm
    রিক্ত		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    স্থির শূন্য *
vterm_malloc(size_t size, শূন্য *data UNUSED)
{
    // make sure that the length is not zero
    প্রত্যাগম alloc_clear(size == 0 ? 1L : size);
}

    স্থির শূন্য
vterm_memfree(শূন্য *ptr, শূন্য *data UNUSED)
{
    vim_free(ptr);
}

স্থির VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    স্থির পূর্ণাঙ্ক
create_vterm(term_T *term, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_মান	    মান;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, রিক্ত);
    term->tl_vterm = vterm;
    যদি (vterm == রিক্ত)
	প্রত্যাগম FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    যদি (state == রিক্ত || screen == রিক্ত)
    {
	vterm_free(vterm);
	প্রত্যাগম FAIL;
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

    যদি (t_colors < 16)
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
    যদি (GetCaretBlinkTime() == INFINITE)
	মান.boolean = 0;
    অন্যথা
	মান.boolean = 1;
#অন্যথা
    মান.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &মান);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    প্রত্যাগম OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    স্থির শূন্য
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    পূর্ণাঙ্ক		index;

    প্রতি (index = 0; index < 16; index++)
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

    স্থির শূন্য
term_update_palette(term_T *term)
{
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette()
	    && (term->tl_palette != রিক্ত
		|| find_var((char_u *)"g:terminal_ansi_colors", রিক্ত, TRUE)
								      != রিক্ত))
    {
	যদি (term->tl_palette != রিক্ত)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	অন্যথা
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    অন্যথা
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    শূন্য
term_update_palette_all(শূন্য)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	যদি (term->tl_vterm == রিক্ত)
	    অনুবর্ত;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    শূন্য
term_update_colors_all(শূন্য)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	যদি (term->tl_vterm == রিক্ত)
	    অনুবর্ত;
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
    যদি (term->tl_status_text != রিক্ত)
	প্রত্যাগম term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    যদি (term->tl_normal_mode)
    {
	যদি (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	অন্যথা
	    txt = (char_u *)_("Terminal-finished");
    }
    অন্যথা যদি (term->tl_title != রিক্ত)
	txt = term->tl_title;
    অন্যথা যদি (term_none_open(term))
	txt = (char_u *)_("active");
    অন্যথা যদি (term_job_running(term))
	txt = (char_u *)_("running");
    অন্যথা
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    যদি (term->tl_status_text != রিক্ত)
	vim_snprintf((বর্ণ *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    প্রত্যাগম term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    শূন্য
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    পূর্ণাঙ্ক
set_ref_in_term(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    প্রতি (term = first_term; !abort && term != রিক্ত; term = term->tl_next)
	যদি (term->tl_job != রিক্ত)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
	}
    প্রত্যাগম abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    স্থির buf_T *
term_get_buf(typval_T *argvars, বর্ণ *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    যদি (buf == রিক্ত || buf->b_term == রিক্ত)
    {
	(শূন্য)tv_get_সংখ্যা(&argvars[0]);    // issue errmsg if type error
	ch_log(রিক্ত, "%s: invalid buffer argument", where);
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম buf;
}

    স্থির শূন্য
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    স্থির শূন্য
dump_term_color(FILE *fd, VTermColor *color)
{
    পূর্ণাঙ্ক index;

    যদি (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    অন্যথা যদি (color->type == 0)
	// use RGB values
	index = 255;
    অন্যথা
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (পূর্ণাঙ্ক)color->red, (পূর্ণাঙ্ক)color->green, (পূর্ণাঙ্ক)color->blue,
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
    শূন্য
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    পূর্ণাঙ্ক		max_height = 0;
    পূর্ণাঙ্ক		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    যদি (check_restricted() || check_secure())
	প্রত্যাগম;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    যদি (term->tl_vterm == রিক্ত)
    {
	emsg(_(e_job_already_finished));
	প্রত্যাগম;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	যদি (check_for_dict_arg(argvars, 2) == FAIL)
	    প্রত্যাগম;
	d = argvars[2].vval.v_dict;
	যদি (d != রিক্ত)
	{
	    max_height = dict_get_সংখ্যা(d, "rows");
	    max_width = dict_get_সংখ্যা(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    যদি (fname == রিক্ত)
	প্রত্যাগম;
    যদি (mch_stat((বর্ণ *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	প্রত্যাগম;
    }

    যদি (*fname == NUL || (fd = mch_fopen((বর্ণ *)fname, WRITEBIN)) == রিক্ত)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	প্রত্যাগম;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    প্রতি (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	পূর্ণাঙ্ক		repeat = 0;

	প্রতি (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    পূর্ণাঙ্ক		    same_attr;
	    পূর্ণাঙ্ক		    same_chars = TRUE;
	    পূর্ণাঙ্ক		    i;
	    পূর্ণাঙ্ক		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    যদি (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    প্রতি (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		পূর্ণাঙ্ক c = cell.chars[i];
		পূর্ণাঙ্ক pc = prev_cell.chars[i];
		পূর্ণাঙ্ক should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		যদি (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		যদি (c != pc)
		    same_chars = FALSE;
		যদি (should_break)
		    ভঙ্গ;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    যদি (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    অন্যথা
	    {
		যদি (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		যদি (cell.chars[0] == NUL)
		    fputs(" ", fd);
		অন্যথা
		{
		    char_u	charbuf[10];
		    পূর্ণাঙ্ক		len;

		    প্রতি (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		যদি (cell.width != prev_cell.width || !same_attr)
		{
		    যদি (cell.width == 2)
			fputs("*", fd);
		    অন্যথা
			fputs("+", fd);

		    যদি (same_attr)
		    {
			fputs("&", fd);
		    }
		    অন্যথা
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			যদি (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			অন্যথা
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			যদি (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			অন্যথা
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    যদি (cell.width == 2)
		++pos.col;
	}
	যদি (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    স্থির শূন্য
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    স্থির শূন্য
append_cell(garray_T *gap, cellattr_T *cell)
{
    যদি (ga_grow(gap, 1) == FAIL)
	প্রত্যাগম;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    স্থির শূন্য
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
    স্থির পূর্ণাঙ্ক
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    পূর্ণাঙ্ক		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {রিক্ত, 0};
    পূর্ণাঙ্ক		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    পূর্ণাঙ্ক		    max_cells = 0;
    পূর্ণাঙ্ক		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, পরিমাণ(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    প্রতি (;;)
    {
	যদি (c == EOF)
	    ভঙ্গ;
	যদি (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	অন্যথা যদি (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    যদি (ga_text.ga_data == রিক্ত)
		dump_is_corrupt(&ga_text);
	    যদি (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_সারি_T   *সারি = (sb_সারি_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		যদি (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		সারি->sb_cols = ga_cell.ga_len;
		সারি->sb_bytes = ga_text.ga_len;
		সারি->sb_cells = ga_cell.ga_data;
		সারি->sb_fill_attr = term->tl_default_color;
		সারি->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_সারি_গুনতি, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    অন্যথা
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	অন্যথা যদি (c == '|' || c == '>')
	{
	    পূর্ণাঙ্ক prev_len = ga_text.ga_len;

	    যদি (c == '>')
	    {
		যদি (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    যদি (c != EOF)
		ga_append(&ga_text, c);
	    প্রতি (;;)
	    {
		c = fgetc(fd);
		যদি (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    ভঙ্গ;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    যদি (ga_text.ga_data != রিক্ত)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		যদি (prev_char.string == রিক্ত)
		    prev_char.length = 0;
	    }

	    যদি (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    অন্যথা যদি (c == '+' || c == '*')
	    {
		পূর্ণাঙ্ক is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		যদি (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		অন্যথা যদি (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    যাবত্ (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    প্রতি (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			যদি (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			অন্যথা যদি (c == '#')
			{
			    পূর্ণাঙ্ক red, green, blue, index = 0, type;

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
			    যদি (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    যাবত্ (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    যদি (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				যদি (index == 0)
				{
				    যদি (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    অন্যথা
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    অন্যথা
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    যদি (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    অন্যথা
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			অন্যথা
			    dump_is_corrupt(&ga_text);
		    }
		}
		অন্যথা
		    dump_is_corrupt(&ga_text);
	    }
	    অন্যথা
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    যদি (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	অন্যথা যদি (c == '@')
	{
	    যদি (prev_char.string == রিক্ত)
		dump_is_corrupt(&ga_text);
	    অন্যথা
	    {
		পূর্ণাঙ্ক গুনতি = 0;

		// repeat previous character, get the count
		প্রতি (;;)
		{
		    c = fgetc(fd);
		    যদি (!SAFE_isdigit(c))
			ভঙ্গ;
		    গুনতি = গুনতি * 10 + (c - '0');
		}

		যাবত্ (গুনতি-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	অন্যথা
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    যদি (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_সারি_গুনতি, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    প্রত্যাগম max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    স্থির char_u *
get_separator(পূর্ণাঙ্ক text_width, char_u *fname)
{
    পূর্ণাঙ্ক	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    পূর্ণাঙ্ক	    fname_size;
    char_u  *p = fname;
    পূর্ণাঙ্ক	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    যদি (textline == রিক্ত)
	প্রত্যাগম রিক্ত;

    fname_size = vim_strsize(fname);
    যদি (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    অন্যথা যদি (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    যাবত্ (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    প্রতি (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    প্রতি (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    প্রত্যাগম textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    স্থির শূন্য
term_load_dump(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = রিক্ত;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = রিক্ত;
    char_u	*fname_tofree = রিক্ত;
    FILE	*fd1;
    FILE	*fd2 = রিক্ত;
    char_u	*textline = রিক্ত;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    যদি (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    যদি (fname1 == রিক্ত || (do_diff && fname2 == রিক্ত))
    {
	emsg(_(e_invalid_argument));
	প্রত্যাগম;
    }
    fd1 = mch_fopen((বর্ণ *)fname1, READBIN);
    যদি (fd1 == রিক্ত)
    {
	semsg(_(e_cant_read_file_str), fname1);
	প্রত্যাগম;
    }
    যদি (do_diff)
    {
	fd2 = mch_fopen((বর্ণ *)fname2, READBIN);
	যদি (fd2 == রিক্ত)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    প্রত্যাগম;
	}
    }

    init_job_options(&opt);
    যদি (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	গচ্ছ theend;

    যদি (opt.jo_term_name == রিক্ত)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	যদি (fname_tofree != রিক্ত)
	{
	    vim_snprintf((বর্ণ *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    যদি (opt.jo_bufnr_buf != রিক্ত)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	যদি (wp == রিক্ত)
	    semsg(_(e_invalid_argument_str), "bufnr");
	অন্যথা
	{
	    buf = curbuf;
	    যাবত্ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    অন্যথা
	// Create a new terminal window.
	buf = term_start(&argvars[0], রিক্ত, &opt, TERM_START_NOJOB);

    যদি (buf != রিক্ত && buf->b_term != রিক্ত)
    {
	পূর্ণাঙ্ক		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	পূর্ণাঙ্ক		width;
	পূর্ণাঙ্ক		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_সংখ্যা = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	যদি (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	যদি (!do_diff)
	    গচ্ছ theend;

	term->tl_শীর্ষ_diff_rows = curbuf->b_ml.ml_সারি_গুনতি;

	textline = get_separator(width, fname1);
	যদি (textline == রিক্ত)
	    গচ্ছ theend;
	যদি (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_সারি_গুনতি, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	যদি (textline == রিক্ত)
	    গচ্ছ theend;
	যদি (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_সারি_গুনতি, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_সারি_গুনতি;
	width2 = read_dump_file(fd2, &cursor_pos2);
	যদি (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    যদি (textline == রিক্ত)
		গচ্ছ theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_সারি_গুনতি - bot_lnum;

	প্রতি (lnum = 1; lnum <= term->tl_শীর্ষ_diff_rows; ++lnum)
	{
	    যদি (lnum + bot_lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    {
		// bottom part has fewer rows, fill with "-"
		প্রতি (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    অন্যথা
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		পূর্ণাঙ্ক	col;
		sb_সারি_T   *sb_সারি = (sb_সারি_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_সারি + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_সারি + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		যদি (line1 == রিক্ত)
		    ভঙ্গ;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		প্রতি (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    পূর্ণাঙ্ক len1 = utfc_ptr2len(p1);
		    পূর্ণাঙ্ক len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    যদি (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    অন্যথা যদি (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    অন্যথা যদি (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    অন্যথা যদি (cellattr1 != রিক্ত && cellattr2 != রিক্ত)
		    {
			যদি ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			অন্যথা যদি (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			অন্যথা যদি (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			অন্যথা যদি (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		যাবত্ (col < width)
		{
		    যদি (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    অন্যথা যদি (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    অন্যথা
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    যদি (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_শীর্ষ_diff_rows) == OK)
		ml_append(term->tl_শীর্ষ_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	যাবত্ (lnum + bot_lnum <= curbuf->b_ml.ml_সারি_গুনতি)
	{
	    // bottom part has more rows, fill with "+"
	    প্রতি (i = 0; i < width; ++i)
		textline[i] = '+';
	    যদি (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_শীর্ষ_diff_rows) == OK)
		ml_append(term->tl_শীর্ষ_diff_rows + lnum, textline, 0, FALSE);
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
    যদি (fd2 != রিক্ত)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    পূর্ণাঙ্ক
term_swap_diff(শূন্য)
{
    term_T	*term = curbuf->b_term;
    linenr_T	সারি_গুনতি;
    linenr_T	শীর্ষ_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_সারি_T	*sb_সারি;

    যদি (term == রিক্ত
	    || !term_is_finished(curbuf)
	    || term->tl_শীর্ষ_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	প্রত্যাগম FAIL;

    সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি;
    শীর্ষ_rows = term->tl_শীর্ষ_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = সারি_গুনতি - bot_rows;
    sb_সারি = (sb_সারি_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    প্রতি (lnum = 1; lnum <= শীর্ষ_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	যদি (p == রিক্ত)
	    প্রত্যাগম OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    প্রতি (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	যদি (p == রিক্ত)
	    প্রত্যাগম OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    যদি (p == রিক্ত)
	প্রত্যাগম OK;
    ml_append(সারি_গুনতি - শীর্ষ_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(সারি_গুনতি - শীর্ষ_rows));
    যদি (p == রিক্ত)
	প্রত্যাগম OK;
    ml_delete(সারি_গুনতি - শীর্ষ_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    যদি (শীর্ষ_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	প্রতি (lnum = 0; lnum < শীর্ষ_rows; ++lnum)
	{
	    sb_সারি_T	temp;

	    temp = *(sb_সারি + lnum);
	    *(sb_সারি + lnum) = *(sb_সারি + bot_start + lnum);
	    *(sb_সারি + bot_start + lnum) = temp;
	}
    }
    অন্যথা
    {
	size_t		size = পরিমাণ(sb_সারি_T) * term->tl_scrollback.ga_len;
	sb_সারি_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	যদি (temp != রিক্ত)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    পরিমাণ(sb_সারি_T) * bot_rows);
	    mch_memmove((sb_সারি_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + শীর্ষ_rows,
		    পরিমাণ(sb_সারি_T) * (সারি_গুনতি - শীর্ষ_rows - bot_rows));
	    mch_memmove((sb_সারি_T *)term->tl_scrollback.ga_data
						       + সারি_গুনতি - শীর্ষ_rows,
		    temp,
		    পরিমাণ(sb_সারি_T) * শীর্ষ_rows);
	    vim_free(temp);
	}
    }

    term->tl_শীর্ষ_diff_rows = bot_rows;
    term->tl_bot_diff_rows = শীর্ষ_rows;

    update_screen(UPD_NOT_VALID);
    প্রত্যাগম OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    শূন্য
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	প্রত্যাগম;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    শূন্য
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    শূন্য
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    rettv->vval.v_সংখ্যা = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    শূন্য
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক	    attr;
    size_t  i;
    char_u  *name;

    স্থির সংরচনা {
	বর্ণ	    *name;
	পূর্ণাঙ্ক	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    attr = tv_get_সংখ্যা(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    যদি (name == রিক্ত)
	প্রত্যাগম;

    যদি (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    প্রতি (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	যদি (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_সংখ্যা = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    ভঙ্গ;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    শূন্য
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getcursor()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_সংখ্যা(l, term->tl_cursor_pos.row + 1);
    list_append_সংখ্যা(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    যদি (d == রিক্ত)
	প্রত্যাগম;

    dict_add_সংখ্যা(d, "visible", term->tl_cursor_visible);
    dict_add_সংখ্যা(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_সংখ্যা(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    যদি (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    শূন্য
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getjob()");
    যদি (buf == রিক্ত)
    {
	যদি (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = রিক্ত;
	}
	অন্যথা
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_সংখ্যা = VVAL_NULL;
	}
	প্রত্যাগম;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    যদি (rettv->vval.v_job != রিক্ত)
	++rettv->vval.v_job->jv_refcount;
}

    স্থির পূর্ণাঙ্ক
get_row_সংখ্যা(typval_T *tv, term_T *term)
{
    যদি (tv->v_type == VAR_STRING
	    && tv->vval.v_string != রিক্ত
	    && STRCMP(tv->vval.v_string, ".") == 0)
	প্রত্যাগম term->tl_cursor_pos.row;
    প্রত্যাগম (পূর্ণাঙ্ক)tv_get_সংখ্যা(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    শূন্য
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    পূর্ণাঙ্ক		    row;

    rettv->v_type = VAR_STRING;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getline()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    row = get_row_সংখ্যা(&argvars[1], term);

    যদি (term->tl_vterm == রিক্ত)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	পূর্ণাঙ্ক	  sb_row = term->tl_scrollback_scrolled + row;
	sb_সারি_T *সারি;

	যদি (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    প্রত্যাগম;
	সারি = (sb_সারি_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, রিক্ত, &offset);

	// vterm is finished, get the text from the buffer
	যদি (lnum > 0 && lnum <= buf->b_ml.ml_সারি_গুনতি)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    যদি (STRLEN(p) >= offset + সারি->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, সারি->sb_bytes);
	}
    }
    অন্যথা
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	পূর্ণাঙ্ক		len;
	char_u		*p;

	যদি (row < 0 || row >= term->tl_rows)
	    প্রত্যাগম;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	যদি (p == রিক্ত)
	    প্রত্যাগম;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (বর্ণ *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    শূন্য
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getscrolled()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    rettv->vval.v_সংখ্যা = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    শূন্য
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getsize()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;

    l = rettv->vval.v_list;
    list_append_সংখ্যা(l, buf->b_term->tl_rows);
    list_append_সংখ্যা(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    শূন্য
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 2) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_setsize()");
    যদি (buf == রিক্ত)
    {
	emsg(_(e_not_terminal_buffer));
	প্রত্যাগম;
    }
    যদি (buf->b_term->tl_vterm == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    rows = tv_get_সংখ্যা(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_সংখ্যা(&argvars[2]);
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
    শূন্য
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getstatus()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;

    যদি (term_job_running(term))
	STRCPY(val, "running");
    অন্যথা
	STRCPY(val, "finished");
    যদি (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    শূন্য
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_gettitle()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;

    যদি (buf->b_term->tl_title != রিক্ত)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    শূন্য
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = রিক্ত;
    পূর্ণাঙ্ক		অঙ্ক = 0;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	অঙ্ক = tv_get_bool(&argvars[1]);

    বিকল্প (অঙ্ক)
    {
	পক্ষ 0:
	    যদি (buf->b_term->tl_job != রিক্ত)
		p = buf->b_term->tl_job->jv_tty_out;
	    ভঙ্গ;
	পক্ষ 1:
	    যদি (buf->b_term->tl_job != রিক্ত)
		p = buf->b_term->tl_job->jv_tty_in;
	    ভঙ্গ;
	স্বভাব:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    প্রত্যাগম;
    }
    যদি (p != রিক্ত)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    শূন্য
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    যদি (rettv_list_alloc(rettv) == FAIL || first_term == রিক্ত)
	প্রত্যাগম;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	যদি (tp->tl_buffer != রিক্ত)
	    যদি (list_append_সংখ্যা(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		প্রত্যাগম;
}

/*
 * "term_scrape(buf, row)" function
 */
    শূন্য
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = রিক্ত;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_সারি_T	    *সারি;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_scrape()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_সংখ্যা(&argvars[1], term);

    যদি (term->tl_vterm != রিক্ত)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	যদি (screen == রিক্ত)  // can't really happen
	    প্রত্যাগম;
	p = রিক্ত;
	সারি = রিক্ত;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, রিক্ত, &offset);

	যদি (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_সারি_গুনতি)
	    প্রত্যাগম;

	সারি = (sb_সারি_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	যদি (STRLEN(p) < offset + সারি->sb_bytes)
	    প্রত্যাগম;

	p += offset;
    }

    প্রতি (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	পূর্ণাঙ্ক		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	পূর্ণাঙ্ক		i;

	যদি (screen == রিক্ত)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    যদি (pos.col >= সারি->sb_cols)
		ভঙ্গ;
	    cellattr = সারি->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	অন্যথা
	{
	    VTermScreenCell cell;

	    যদি (vterm_screen_get_cell(screen, pos, &cell) == 0)
		ভঙ্গ;

	    mbslen = 0;
	    প্রতি (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		যদি (cell.chars[i] == 0)
		    ভঙ্গ;
		mbslen += (*utf_char2bytes)((পূর্ণাঙ্ক)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	যদি (dcell == রিক্ত)
	    ভঙ্গ;
	যদি (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    ভঙ্গ;
	}

	dict_add_string_len(dcell, "chars", mbs, (পূর্ণাঙ্ক)mbslen);

	rgblen = vim_snprintf_safelen((বর্ণ *)rgb, পরিমাণ(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (পূর্ণাঙ্ক)rgblen);
	rgblen = vim_snprintf_safelen((বর্ণ *)rgb, পরিমাণ(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (পূর্ণাঙ্ক)rgblen);

	dict_add_সংখ্যা(dcell, "attr",
				      cell2attr(term, রিক্ত, &attrs, &fg, &bg));
	dict_add_সংখ্যা(dcell, "width", width);

	++pos.col;
	যদি (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    শূন্য
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*বার্তা;
    term_T	*term;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_sendkeys()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;

    বার্তা = tv_get_string_chk(&argvars[1]);
    যদি (বার্তা == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    যদি (term->tl_vterm == রিক্ত)
	প্রত্যাগম;

    যাবত্ (*বার্তা != NUL)
    {
	পূর্ণাঙ্ক c;

	যদি (*বার্তা == K_SPECIAL && বার্তা[1] != NUL && বার্তা[2] != NUL)
	{
	    c = TO_SPECIAL(বার্তা[1], বার্তা[2]);
	    বার্তা += 3;
	}
	অন্যথা
	{
	    c = PTR2CHAR(বার্তা);
	    বার্তা += MB_CPTR2LEN(বার্তা);
	}
	send_চাবিগুলি_to_term(term, c, 0, FALSE);
    }
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    শূন্য
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    পূর্ণাঙ্ক		index;
    list_T	*list;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_getansicolors()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    যদি (term->tl_vterm == রিক্ত)
	প্রত্যাগম;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    প্রতি (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((বর্ণ *)hexbuf, পরিমাণ(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	যদি (list_append_string(list, hexbuf, (পূর্ণাঙ্ক)hexbuflen) == FAIL)
	    প্রত্যাগম;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    শূন্য
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    পূর্ণাঙ্ক		n = 0;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_setansicolors()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    যদি (term->tl_vterm == রিক্ত)
	প্রত্যাগম;

    যদি (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	প্রত্যাগম;

    যদি (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_মান_for_argument_str), "\"colors\"");
	প্রত্যাগম;
    }

    যদি (term->tl_palette == রিক্ত)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    যদি (term->tl_palette == রিক্ত)
	প্রত্যাগম;

    FOR_ALL_LIST_বস্তুসমূহ(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	যদি (color_name == রিক্ত)
	    প্রত্যাগম;

	guicolor = GUI_GET_COLOR(color_name);
	যদি (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    প্রত্যাগম;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    শূন্য
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_setapi()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    যদি (api != রিক্ত)
	term->tl_api = vim_strsave(api);
    অন্যথা
	term->tl_api = রিক্ত;
}

/*
 * "term_setrestore(buf, command)" function
 */
    শূন্য
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#যদি defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_setrestore()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    যদি (cmd != রিক্ত)
	term->tl_command = vim_strsave(cmd);
    অন্যথা
	term->tl_command = রিক্ত;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    শূন্য
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_setkill()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    যদি (how != রিক্ত)
	term->tl_kill = vim_strsave(how);
    অন্যথা
	term->tl_kill = রিক্ত;
}

/*
 * "term_start(command, options)" function
 */
    শূন্য
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    যদি (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    init_job_options(&opt);
    যদি (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	প্রত্যাগম;

    buf = term_start(&argvars[0], রিক্ত, &opt, 0);

    যদি (buf != রিক্ত && buf->b_term != রিক্ত)
	rettv->vval.v_সংখ্যা = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    শূন্য
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    buf = term_get_buf(argvars, "term_wait()");
    যদি (buf == রিক্ত)
	প্রত্যাগম;
    যদি (buf->b_term->tl_job == রিক্ত)
    {
	ch_log(রিক্ত, "term_wait(): no job to wait for");
	প্রত্যাগম;
    }
    যদি (buf->b_term->tl_job->jv_channel == রিক্ত)
	// channel is closed, nothing to do
	প্রত্যাগম;

    // Get the job status, this will detect a job that finished.
    যদি (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(রিক্ত, "term_wait(): waiting for channel to close");
	যাবত্ (buf->b_term != রিক্ত && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    যদি (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		ভঙ্গ;
	    যদি (buf->b_term == রিক্ত || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		ভঙ্গ;
	}

	term_flush_messages();
    }
    অন্যথা
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	যদি (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_সংখ্যা(&argvars[1]);
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
    শূন্য
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	যদি (term->tl_job == ch->ch_job)
	{
	    যদি (term->tl_eof_chars != রিক্ত)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(পূর্ণাঙ্ক)STRLEN(term->tl_eof_chars), রিক্ত);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, রিক্ত);
	    }
#ifdef MSWIN
	    অন্যথা
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, রিক্ত);
#endif
	}
}

#যদি defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    প্রত্যাগম term != রিক্ত ? term->tl_job : রিক্ত;
}
#endif

#যদি defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
শূন্য (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    স্থির পূর্ণাঙ্ক
dyn_conpty_init(পূর্ণাঙ্ক verbose)
{
    স্থির HMODULE	hKerneldll = রিক্ত;
    পূর্ণাঙ্ক			i;
    স্থির সংরচনা
    {
	বর্ণ	*name;
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
	{রিক্ত, রিক্ত}
    };

    যদি (!has_conpty_working())
    {
	যদি (verbose)
	    emsg(_(e_conpty_is_not_available));
	প্রত্যাগম FAIL;
    }

    // No need to initialize twice.
    যদি (hKerneldll)
	প্রত্যাগম OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    প্রতি (i = 0; conpty_entry[i].name != রিক্ত
					&& conpty_entry[i].ptr != রিক্ত; ++i)
    {
	যদি ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == রিক্ত)
	{
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = রিক্ত;
	    প্রত্যাগম FAIL;
	}
    }

    প্রত্যাগম OK;
}

    স্থির পূর্ণাঙ্ক
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	বর্ণ	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = রিক্ত;
    WCHAR	    *cmd_wchar_copy = রিক্ত;
    WCHAR	    *cwd_wchar = রিক্ত;
    WCHAR	    *env_wchar = রিক্ত;
    channel_T	    *channel = রিক্ত;
    job_T	    *job = রিক্ত;
    HANDLE	    jo = রিক্ত;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = রিক্ত;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = রিক্ত;
    HANDLE	    o_theirs = রিক্ত;
    HANDLE	    i_ours = রিক্ত;
    HANDLE	    o_ours = রিক্ত;
    বর্ণ	    *errmsg = রিক্ত;

    ga_init2(&ga_cmd, পরিমাণ(বর্ণ*), 20);
    ga_init2(&ga_env, পরিমাণ(বর্ণ*), 20);

    যদি (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    অন্যথা যদি (argvar->v_type == VAR_LIST)
    {
	যদি (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    গচ্ছ failed;
	cmd = ga_cmd.ga_data;
    }
    যদি (cmd == রিক্ত || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	গচ্ছ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, রিক্ত);

    যদি (cmd_wchar != রিক্ত)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    যদি (cmd_wchar == রিক্ত)
	গচ্ছ failed;
    যদি (opt->jo_cwd != রিক্ত)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, রিক্ত);
	যদি (cwd_wchar == রিক্ত)
	    গচ্ছ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    যদি (!CreatePipe(&i_theirs, &i_ours, রিক্ত, 0))
	গচ্ছ failed;
    যদি (!CreatePipe(&o_ours, &o_theirs, রিক্ত, 0))
	গচ্ছ failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    যদি (FAILED(hr))
	গচ্ছ failed;

    term->tl_siex.StartupInfo.cb = পরিমাণ(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_প্রবেশ = INVALID_HANDLE_মান;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_মান;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_মান;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(রিক্ত, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    যদি (!term->tl_siex.lpAttributeList)
	গচ্ছ failed;
    যদি (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	গচ্ছ failed;
    যদি (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    পরিমাণ(HPCON), রিক্ত, রিক্ত))
	গচ্ছ failed;

    channel = add_channel();
    যদি (channel == রিক্ত)
	গচ্ছ failed;

    job = job_alloc();
    যদি (job == রিক্ত)
	গচ্ছ failed;
    যদি (argvar->v_type == VAR_STRING)
    {
	পূর্ণাঙ্ক argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    অন্যথা
    {
	পূর্ণাঙ্ক argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    যদি (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    যদি (!CreateProcessW(রিক্ত, cmd_wchar_copy, রিক্ত, রিক্ত, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	গচ্ছ failed;
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

    jo = CreateJobObject(রিক্ত, রিক্ত);
    যদি (jo == রিক্ত)
	গচ্ছ failed;

    যদি (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = রিক্ত;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	গচ্ছ failed;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette())
    {
	যদি (term->tl_palette != রিক্ত)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	অন্যথা
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
    যদি (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((বর্ণ *)fname, WRITEBIN);
	যদি (term->tl_out_fd == রিক্ত)
	    semsg(_(e_cant_open_file_str), fname);
    }

    প্রত্যাগম OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    যদি (channel != রিক্ত)
	channel_clear(channel);
    যদি (job != রিক্ত)
    {
	job->jv_channel = রিক্ত;
	job_cleanup(job);
    }
    term->tl_job = রিক্ত;
    যদি (jo != রিক্ত)
	CloseHandle(jo);

    যদি (term->tl_siex.lpAttributeList != রিক্ত)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = রিক্ত;
    যদি (o_theirs != রিক্ত)
	CloseHandle(o_theirs);
    যদি (o_ours != রিক্ত)
	CloseHandle(o_ours);
    যদি (i_ours != রিক্ত)
	CloseHandle(i_ours);
    যদি (i_theirs != রিক্ত)
	CloseHandle(i_theirs);
    যদি (term->tl_conpty != রিক্ত)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = রিক্ত;
    // Propagate errors that occur in CreateProcess
    যদি (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    প্রত্যাগম FAIL;
}

    স্থির শূন্য
conpty_term_report_winsize(term_T *term, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    স্থির শূন্য
term_free_conpty(term_T *term)
{
    যদি (term->tl_siex.lpAttributeList != রিক্ত)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = রিক্ত;
    যদি (term->tl_conpty != রিক্ত)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = রিক্ত;
}

    পূর্ণাঙ্ক
use_conpty(শূন্য)
{
    প্রত্যাগম has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

শূন্য* (*winpty_config_new)(UINT64, শূন্য*);
শূন্য* (*winpty_open)(শূন্য*, শূন্য*);
শূন্য* (*winpty_spawn_config_new)(UINT64, শূন্য*, LPCWSTR, শূন্য*, শূন্য*, শূন্য*);
BOOL (*winpty_spawn)(শূন্য*, শূন্য*, HANDLE*, HANDLE*, DWORD*, শূন্য*);
শূন্য (*winpty_config_set_mouse_mode)(শূন্য*, পূর্ণাঙ্ক);
শূন্য (*winpty_config_set_initial_size)(শূন্য*, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
LPCWSTR (*winpty_conin_name)(শূন্য*);
LPCWSTR (*winpty_conout_name)(শূন্য*);
LPCWSTR (*winpty_conerr_name)(শূন্য*);
শূন্য (*winpty_free)(শূন্য*);
শূন্য (*winpty_config_free)(শূন্য*);
শূন্য (*winpty_spawn_config_free)(শূন্য*);
শূন্য (*winpty_error_free)(শূন্য*);
LPCWSTR (*winpty_error_বার্তা)(শূন্য*);
BOOL (*winpty_set_size)(শূন্য*, পূর্ণাঙ্ক, পূর্ণাঙ্ক, শূন্য*);
HANDLE (*winpty_agent_process)(শূন্য*);

# define WINPTY_DLL "winpty.dll"

স্থির HINSTANCE hWinPtyDLL = রিক্ত;

    স্থির পূর্ণাঙ্ক
dyn_winpty_init(পূর্ণাঙ্ক verbose)
{
    পূর্ণাঙ্ক i;
    স্থির সংরচনা
    {
	বর্ণ	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_বার্তা},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{রিক্ত, রিক্ত}
    };

    // No need to initialize twice.
    যদি (hWinPtyDLL)
	প্রত্যাগম OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    যদি (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((বর্ণ *)p_winptydll);
    যদি (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    যদি (!hWinPtyDLL)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	প্রত্যাগম FAIL;
    }
    প্রতি (i = 0; winpty_entry[i].name != রিক্ত
					 && winpty_entry[i].ptr != রিক্ত; ++i)
    {
	যদি ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == রিক্ত)
	{
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = রিক্ত;
	    প্রত্যাগম FAIL;
	}
    }

    প্রত্যাগম OK;
}

    স্থির পূর্ণাঙ্ক
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	বর্ণ	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = রিক্ত;
    WCHAR	    *cwd_wchar = রিক্ত;
    WCHAR	    *env_wchar = রিক্ত;
    channel_T	    *channel = রিক্ত;
    job_T	    *job = রিক্ত;
    DWORD	    error;
    HANDLE	    jo = রিক্ত;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    শূন্য	    *winpty_err = রিক্ত;
    শূন্য	    *spawn_config = রিক্ত;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = রিক্ত;

    ga_init2(&ga_cmd, পরিমাণ(বর্ণ*), 20);
    ga_init2(&ga_env, পরিমাণ(বর্ণ*), 20);

    যদি (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    অন্যথা যদি (argvar->v_type == VAR_LIST)
    {
	যদি (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    গচ্ছ failed;
	cmd = ga_cmd.ga_data;
    }
    যদি (cmd == রিক্ত || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	গচ্ছ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, রিক্ত);
    ga_clear(&ga_cmd);
    যদি (cmd_wchar == রিক্ত)
	গচ্ছ failed;
    যদি (opt->jo_cwd != রিক্ত)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, রিক্ত);
	যদি (cwd_wchar == রিক্ত)
	    গচ্ছ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    যদি (term->tl_winpty_config == রিক্ত)
	গচ্ছ failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    যদি (term->tl_winpty == রিক্ত)
	গচ্ছ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    রিক্ত,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    যদি (spawn_config == রিক্ত)
	গচ্ছ failed;

    channel = add_channel();
    যদি (channel == রিক্ত)
	গচ্ছ failed;

    job = job_alloc();
    যদি (job == রিক্ত)
	গচ্ছ failed;
    যদি (argvar->v_type == VAR_STRING)
    {
	পূর্ণাঙ্ক argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    অন্যথা
    {
	পূর্ণাঙ্ক argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    যদি (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    যদি (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	গচ্ছ failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, রিক্ত,
	    OPEN_EXISTING, 0, রিক্ত),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, রিক্ত,
	    OPEN_EXISTING, 0, রিক্ত),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, রিক্ত,
	    OPEN_EXISTING, 0, রিক্ত));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(রিক্ত, রিক্ত);
    যদি (jo == রিক্ত)
	গচ্ছ failed;

    যদি (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = রিক্ত;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	গচ্ছ failed;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette())
    {
	যদি (term->tl_palette != রিক্ত)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	অন্যথা
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
	    (short_u *)winpty_conin_name(term->tl_winpty), রিক্ত);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), রিক্ত);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    যদি (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((বর্ণ *)fname, WRITEBIN);
	যদি (term->tl_out_fd == রিক্ত)
	    semsg(_(e_cant_open_file_str), fname);
    }

    প্রত্যাগম OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    যদি (spawn_config != রিক্ত)
	winpty_spawn_config_free(spawn_config);
    যদি (channel != রিক্ত)
	channel_clear(channel);
    যদি (job != রিক্ত)
    {
	job->jv_channel = রিক্ত;
	job_cleanup(job);
    }
    term->tl_job = রিক্ত;
    যদি (jo != রিক্ত)
	CloseHandle(jo);
    যদি (term->tl_winpty != রিক্ত)
	winpty_free(term->tl_winpty);
    term->tl_winpty = রিক্ত;
    যদি (term->tl_winpty_config != রিক্ত)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = রিক্ত;
    যদি (winpty_err != রিক্ত)
    {
	বর্ণ *বার্তা = (বর্ণ *)utf16_to_enc(
				(short_u *)winpty_error_বার্তা(winpty_err), রিক্ত);

	যদি (বার্তা != রিক্ত)
	{
	    emsg(বার্তা);
	    vim_free(বার্তা);
	}
	winpty_error_free(winpty_err);
    }
    প্রত্যাগম FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	বর্ণ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    পূর্ণাঙ্ক		    use_winpty = FALSE;
    পূর্ণাঙ্ক		    use_conpty = FALSE;
    পূর্ণাঙ্ক		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    যদি (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	প্রত্যাগম dyn_winpty_init(TRUE);

    যদি (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    যদি (tty_type == NUL)
    {
	যদি (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	অন্যথা যদি (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    অন্যথা যদি (tty_type == 'w')	// winpty
    {
	যদি (has_winpty)
	    use_winpty = TRUE;
    }
    অন্যথা যদি (tty_type == 'c')	// conpty
    {
	যদি (has_conpty)
	    use_conpty = TRUE;
	অন্যথা
	    প্রত্যাগম dyn_conpty_init(TRUE);
    }

    যদি (use_conpty)
	প্রত্যাগম conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    যদি (use_winpty)
	প্রত্যাগম winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    প্রত্যাগম dyn_winpty_init(TRUE);
}

    স্থির পূর্ণাঙ্ক
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_মান;
    HANDLE	    hPipeOut = INVALID_HANDLE_মান;
    বর্ণ	    in_name[80], out_name[80];
    channel_T	    *channel = রিক্ত;

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	প্রত্যাগম FAIL;

    vim_snprintf(in_name, পরিমাণ(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, রিক্ত);
    যদি (hPipeIn == INVALID_HANDLE_মান)
	গচ্ছ failed;

    vim_snprintf(out_name, পরিমাণ(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, রিক্ত);
    যদি (hPipeOut == INVALID_HANDLE_মান)
	গচ্ছ failed;

    ConnectNamedPipe(hPipeIn, রিক্ত);
    ConnectNamedPipe(hPipeOut, রিক্ত);

    term->tl_job = job_alloc();
    যদি (term->tl_job == রিক্ত)
	গচ্ছ failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    যদি (channel == রিক্ত)
	গচ্ছ failed;
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

    প্রত্যাগম OK;

failed:
    যদি (hPipeIn != রিক্ত)
	CloseHandle(hPipeIn);
    যদি (hPipeOut != রিক্ত)
	CloseHandle(hPipeOut);
    প্রত্যাগম FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    স্থির শূন্য
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    যদি (term->tl_winpty != রিক্ত)
	winpty_free(term->tl_winpty);
    term->tl_winpty = রিক্ত;
    যদি (term->tl_winpty_config != রিক্ত)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = রিক্ত;
    যদি (term->tl_vterm != রিক্ত)
	vterm_free(term->tl_vterm);
    term->tl_vterm = রিক্ত;
}

/*
 * Report the size to the terminal.
 */
    স্থির শূন্য
term_report_winsize(term_T *term, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
    যদি (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    যদি (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, রিক্ত);
}

    পূর্ণাঙ্ক
terminal_enabled(শূন্য)
{
    প্রত্যাগম dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#অন্যথা

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	বর্ণ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = রিক্ত;

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	প্রত্যাগম FAIL;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette())
    {
	যদি (term->tl_palette != রিক্ত)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	অন্যথা
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    যদি (term->tl_job != রিক্ত)
	++term->tl_job->jv_refcount;

    প্রত্যাগম term->tl_job != রিক্ত
	&& term->tl_job->jv_channel != রিক্ত
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    স্থির পূর্ণাঙ্ক
create_pty_only(term_T *term, jobopt_T *opt)
{
    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	প্রত্যাগম FAIL;

    term->tl_job = job_alloc();
    যদি (term->tl_job == রিক্ত)
	প্রত্যাগম FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    প্রত্যাগম mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    স্থির শূন্য
term_free_vterm(term_T *term)
{
    যদি (term->tl_vterm != রিক্ত)
	vterm_free(term->tl_vterm);
    term->tl_vterm = রিক্ত;
}

/*
 * Report the size to the terminal.
 */
    স্থির শূন্য
term_report_winsize(term_T *term, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
    // Use an ioctl() to report the new window size to the job.
    যদি (term->tl_job == রিক্ত || term->tl_job->jv_channel == রিক্ত)
	প্রত্যাগম;

    পূর্ণাঙ্ক fd = -1;
    পূর্ণাঙ্ক part;

    প্রতি (part = PART_OUT; part < PART_গুনতি; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	যদি (mch_isatty(fd))
	    ভঙ্গ;
    }
    যদি (part < PART_গুনতি && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
