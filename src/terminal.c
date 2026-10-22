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
প্রকারলেখ কাঠামো {
    VTermScreenCellAttrs	attrs;
    অক্ষর			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

প্রকারলেখ কাঠামো sb_সারি_S {
    পূর্ণসংখ্যা		sb_cols;	// can differ per line
    পূর্ণসংখ্যা		sb_bytes;	// length in bytes of text
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
প্রকারলেখ কাঠামো _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
কাঠামো terminal_S {
    term_T	*tl_পরবর্তী;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_বাফার;
#যদি defined(FEAT_GUI)
    পূর্ণসংখ্যা		tl_system;	// when non-zero used for :!cmd output
    পূর্ণসংখ্যা		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    পূর্ণসংখ্যা		tl_vterm_আকার_changed;

    পূর্ণসংখ্যা		tl_normal_mode; // TRUE: Terminal-Normal mode
    পূর্ণসংখ্যা		tl_channel_closing;
    পূর্ণসংখ্যা		tl_channel_closed;
    পূর্ণসংখ্যা		tl_channel_recently_closed; // still need to handle tl_finish

    পূর্ণসংখ্যা		tl_finish;
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
    পূর্ণসংখ্যা		tl_rows;
    পূর্ণসংখ্যা		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    পূর্ণসংখ্যা		tl_dirty_row_start; // MAX_ROW if nothing dirty
    পূর্ণসংখ্যা		tl_dirty_row_end;   // row below last one to update
    পূর্ণসংখ্যা		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    পূর্ণসংখ্যা		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    পূর্ণসংখ্যা		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    পূর্ণসংখ্যা		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    পূর্ণসংখ্যা		tl_scrollback_snapshot;
    পূর্ণসংখ্যা		tl_বাফার_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_শীর্ষ_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    পূর্ণসংখ্যা		tl_cursor_visible;
    পূর্ণসংখ্যা		tl_cursor_blink;
    পূর্ণসংখ্যা		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    পূর্ণসংখ্যা		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
স্থির term_T *first_term = ফাঁকা;

// Terminal active in terminal_loop().
স্থির term_T *in_terminal_loop = ফাঁকা;

#ifdef MSWIN
স্থির BOOL has_winpty = FALSE;
স্থির BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define চাবি_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    জন্য ((term) = first_term; (term) != ফাঁকা; (term) = (term)->tl_পরবর্তী)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
স্থির পূর্ণসংখ্যা term_and_job_init(term_T *term, typval_T *argvar, অক্ষর **argv, jobopt_T *opt, jobopt_T *orig_opt);
স্থির পূর্ণসংখ্যা create_pty_only(term_T *term, jobopt_T *opt);
স্থির শূন্য term_report_winsize(term_T *term, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols);
স্থির শূন্য term_free_vterm(term_T *term);
#ifdef FEAT_GUI
স্থির শূন্য update_system_term(term_T *term);
#endif

স্থির শূন্য handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
স্থির পূর্ণসংখ্যা term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
স্থির char_u	*last_set_cursor_color = ফাঁকা;
স্থির char_u	*desired_cursor_color = ফাঁকা;
স্থির পূর্ণসংখ্যা	last_set_cursor_shape = -1;
স্থির পূর্ণসংখ্যা	desired_cursor_shape = -1;
স্থির পূর্ণসংখ্যা	last_set_cursor_blink = -1;
স্থির পূর্ণসংখ্যা	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    স্থির পূর্ণসংখ্যা
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    যদি (lhs_color != ফাঁকা && rhs_color != ফাঁকা)
	ফেরত STRCMP(lhs_color, rhs_color) == 0;
    ফেরত lhs_color == ফাঁকা && rhs_color == ফাঁকা;
}

    স্থির শূন্য
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    যদি (cursor_color_equal(*to_color, from_color))
	ফেরত;
    vim_free(*to_color);
    *to_color = (from_color == ফাঁকা) ? ফাঁকা : vim_strsave(from_color);
}

    স্থির char_u *
cursor_color_get(char_u *color)
{
    ফেরত (color == ফাঁকা) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    স্থির পূর্ণসংখ্যা
parse_termwinsize(win_T *wp, পূর্ণসংখ্যা *rows, পূর্ণসংখ্যা *cols)
{
    পূর্ণসংখ্যা	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    যদি (*wp->w_p_tws == NUL)
	ফেরত FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    যদি (p == ফাঁকা)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((অক্ষর *)wp->w_p_tws);
    *cols = atoi((অক্ষর *)p + 1);
    যদি (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    যদি (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    ফেরত minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    স্থির শূন্য
set_term_and_win_আকার(term_T *term, jobopt_T *opt)
{
    পূর্ণসংখ্যা rows, cols;
    পূর্ণসংখ্যা	minsize;

#ifdef FEAT_GUI
    যদি (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	ফেরত;
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

	    vim_snprintf((অক্ষর *)buf, 100, "%dx%d",
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
setup_job_options(jobopt_T *opt, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    যদি (!(opt->jo_set & JO_OUT_IO))
#endif
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_OUT] = JIO_বাফার;
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
	opt->jo_io[PART_ERR] = JIO_বাফার;
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
term_close_বাফার(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    যদি (old_curbuf != ফাঁকা)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_বাফার = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_বাফার(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);
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
	অক্ষর	    **argv,
	jobopt_T    *opt,
	পূর্ণসংখ্যা	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = ফাঁকা;
    পূর্ণসংখ্যা		res;
    buf_T	*newbuf;
    পূর্ণসংখ্যা		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    যদি (check_restricted() || check_secure())
	ফেরত ফাঁকা;
    যদি (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_সারি_window));
	ফেরত ফাঁকা;
    }

    যদি ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != ফাঁকা
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ফাঁকা
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	ফেরত ফাঁকা;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    যদি (term == ফাঁকা)
	ফেরত ফাঁকা;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, আকার(sb_সারি_T), 300);
    ga_init2(&term->tl_scrollback_postponed, আকার(sb_সারি_T), 300);
    ga_init2(&term->tl_osc_buf, আকার(অক্ষর), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    যদি (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	যদি (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    ফেরত ফাঁকা;
	}
	যদি (do_ecmd(0, ফাঁকা, ফাঁকা, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_বাফার) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা যদি (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ফাঁকা, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	যদি (buf == ফাঁকা || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    ফেরত ফাঁকা;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_বাফার = buf;
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

	পূর্ণসংখ্যা cmod_split_modified = FALSE;
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
	    ফেরত ফাঁকা;
	}
    }
    term->tl_বাফার = curbuf;
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
    term->tl_পরবর্তী = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, ফাঁকা, ফাঁকা, FALSE, curbuf);

    যদি (opt->jo_term_name != ফাঁকা)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    অন্যথা যদি (argv != ফাঁকা)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    অন্যথা
    {
	পূর্ণসংখ্যা	i;
	size_t	len;
	char_u	*cmd, *p;

	যদি (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    যদি (cmd == ফাঁকা)
		cmd = (char_u *)"";
	    অন্যথা যদি (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	অন্যথা যদি (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ফাঁকা
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == ফাঁকা)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	জন্য (i = 0; p != ফাঁকা; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    যদি (i == 0)
		vim_snprintf((অক্ষর *)p, len, "!%s", cmd);
	    অন্যথা
		vim_snprintf((অক্ষর *)p, len, "!%s (%d)", cmd, i);
	    যদি (buflist_findname(p) == ফাঁকা)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		বিরতি;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ফাঁকা, ফাঁকা, FALSE, curbuf);

    যদি (opt->jo_term_opencmd != ফাঁকা)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    যদি (opt->jo_eof_chars != ফাঁকা)
	term->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    set_string_option_direct((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    set_term_and_win_আকার(term, opt);
#ifdef MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, আকার(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    যদি (flags & TERM_START_NOJOB)
	ফেরত curbuf;

#যদি defined(FEAT_SESSION)
    // Remember the command for the session file.
    যদি (opt->jo_term_norestore || argv != ফাঁকা)
	term->tl_command = vim_strsave((char_u *)"NONE");
    অন্যথা যদি (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	যদি (cmd != ফাঁকা && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    অন্যথা যদি (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ফাঁকা
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_বস্তুসমূহ(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    যদি (s == ফাঁকা)
		বিরতি;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    যদি (p == ফাঁকা)
		বিরতি;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	যদি (item == ফাঁকা)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_তথ্য;
	}
	অন্যথা
	    ga_clear(&ga);
    }
#endif

    যদি (opt->jo_term_kill != ফাঁকা)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    যদি (opt->jo_term_api != ফাঁকা)
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
	যদি (term->tl_palette == ফাঁকা)
	{
	    vim_free(term);
	    ফেরত ফাঁকা;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, আকার(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    যদি (argv == ফাঁকা
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ফাঁকা
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    অন্যথা
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    যদি (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_আকার(term->tl_vterm, &term->tl_rows, &term->tl_cols);
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

	যদি (old_curbuf != ফাঁকা)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_বাফার = curbuf;
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
	term_close_বাফার(curbuf, old_curbuf);
	ফেরত ফাঁকা;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ফাঁকা, ফাঁকা, FALSE, newbuf);
    যদি (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ফাঁকা, ফাঁকা, FALSE, newbuf);
    ফেরত newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    শূন্য
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    পূর্ণসংখ্যা		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ফাঁকা;
    পূর্ণসংখ্যা		scroll_save = বার্তা_scroll;

    বার্তা_scroll = FALSE;		// don't scroll here
    autowrite_all();
    বার্তা_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    যখন (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	যদি (ep != ফাঁকা)
	{
	    যদি (ep < p)
		p = ep;
	    অন্যথা
		ep = ফাঁকা;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((পূর্ণসংখ্যা)(p - cmd) == আকার(name) - 1 \
				 && STRNICMP(cmd, name, আকার(name) - 1) == 0)
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
	অন্যথা যদি (OPTARG_HAS("kill") && ep != ফাঁকা)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	অন্যথা যদি (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    যদি (ep != ফাঁকা)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    অন্যথা
		opt.jo_term_api = ফাঁকা;
	}
	অন্যথা যদি (OPTARG_HAS("rows") && ep != ফাঁকা && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((অক্ষর *)ep + 1);
	    p = skiptowhite(cmd);
	}
	অন্যথা যদি (OPTARG_HAS("cols") && ep != ফাঁকা && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((অক্ষর *)ep + 1);
	    p = skiptowhite(cmd);
	}
	অন্যথা যদি (OPTARG_HAS("eof") && ep != ফাঁকা)
	{
	    char_u *buf = ফাঁকা;
	    char_u *চাবিগুলি;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    চাবিগুলি = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ফাঁকা);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(চাবিগুলি);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	অন্যথা যদি ((পূর্ণসংখ্যা)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ফাঁকা)
	{
	    পূর্ণসংখ্যা tty_type = NUL;

	    p = skiptowhite(cmd);
	    যদি (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    অন্যথা যদি (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    অন্যথা
	    {
		semsg(_(e_invalid_মান_for_argument_str), "type");
		যাও theend;
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
	    যাও theend;
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
	opt.jo_io[PART_IN] = JIO_বাফার;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_শীর্ষ = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    যদি (opt_shell && tofree == ফাঁকা)
    {
#ifdef UNIX
	অক্ষর	**argv = ফাঁকা;
	char_u	*tofree1 = ফাঁকা;
	char_u	*tofree2 = ফাঁকা;

	// :term ++shell command
	যদি (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(ফাঁকা, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	যাও theend;
#অন্যথা
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	যদি (newcmd == ফাঁকা)
	    যাও theend;
	tofree = newcmd;
	vim_snprintf((অক্ষর *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# অন্যথা
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	যাও theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, ফাঁকা, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    স্থির char_u *
get_terminalopt_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    // Note: Keep this in sync with ex_terminal.
    স্থির অক্ষর *(p_termopt_মানসমূহ[]) =
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

    যদি (idx < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(p_termopt_মানসমূহ))
	ফেরত (char_u*)p_termopt_মানসমূহ[idx];
    ফেরত ফাঁকা;
}

    স্থির char_u *
get_termkill_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    স্থির অক্ষর *(p_termkill_মানসমূহ[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    যদি (idx < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(p_termkill_মানসমূহ))
	ফেরত (char_u*)p_termkill_মানসমূহ[idx];
    ফেরত ফাঁকা;
}

/*
 * Command-line expansion for :terminal [options]
 */
    পূর্ণসংখ্যা
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	পূর্ণসংখ্যা	    *অঙ্ক_Matches)
{
    যদি (xp->xp_pattern > xp->xp_সারি && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, পূর্ণসংখ্যা) = ফাঁকা;

	char_u *name_end = xp->xp_pattern - 1;
	যদি (name_end - xp->xp_সারি >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	যদি (cb != ফাঁকা)
	{
	    ফেরত ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    অঙ্ক_Matches,
		    cb,
		    FALSE);
	}
	ফেরত FAIL;
    }
    ফেরত ExpandGeneric(
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
    পূর্ণসংখ্যা
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ধ্রুবক পূর্ণসংখ্যা	bufnr = wp->w_বাফার->b_fnum;
    term_T	*term = wp->w_বাফার->b_term;

    যদি (terminal_bufs != ফাঁকা && wp->w_বাফার->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	অক্ষর	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, আকার(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	যদি (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    যদি (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		ফেরত FAIL;
	    ফেরত put_eol(fd);
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
	ফেরত FAIL;
# ifdef MSWIN
    যদি (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	ফেরত FAIL;
# endif
    যদি (term->tl_command != ফাঁকা)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	যদি (quoted_command == ফাঁকা)
	    ফেরত FAIL;

	পূর্ণসংখ্যা ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((অক্ষর *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	যদি (ret)
	    ফেরত FAIL;
    }
    যদি (put_eol(fd) != OK)
	ফেরত FAIL;

    যদি (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	ফেরত FAIL;

    যদি (terminal_bufs != ফাঁকা && wp->w_বাফার->b_nwindows > 1)
    {
	অক্ষর *hash_চাবি = alloc(NUMBUFLEN);

	vim_snprintf(hash_চাবি, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_চাবি, "terminal session");
    }

    ফেরত put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    পূর্ণসংখ্যা
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    ফেরত term != ফাঁকা && (term->tl_command == ফাঁকা
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    স্থির শূন্য
free_scrollback(term_T *term)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_সারি_T *)term->tl_scrollback.ga_তথ্য + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    জন্য (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_সারি_T *)term->tl_scrollback_postponed.ga_তথ্য + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
স্থির term_T	*terminals_to_free = ফাঁকা;

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

    যদি (term == ফাঁকা)
	ফেরত;

    // Unlink the terminal form the list of terminals.
    যদি (first_term == term)
	first_term = term->tl_পরবর্তী;
    অন্যথা
	জন্য (tp = first_term; tp->tl_পরবর্তী != ফাঁকা; tp = tp->tl_পরবর্তী)
	    যদি (tp->tl_পরবর্তী == term)
	    {
		tp->tl_পরবর্তী = term->tl_পরবর্তী;
		বিরতি;
	    }

    যদি (term->tl_job != ফাঁকা)
    {
	যদি (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, ফাঁকা, "kill");
	job_unref(term->tl_job);
    }
    term->tl_পরবর্তী = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = ফাঁকা;
    যদি (in_terminal_loop == term)
	in_terminal_loop = ফাঁকা;
}

    শূন্য
free_unused_terminals(শূন্য)
{
    যখন (terminals_to_free != ফাঁকা)
    {
	term_T	    *term = terminals_to_free;

	terminals_to_free = term->tl_পরবর্তী;

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
	যদি (term->tl_out_fd != ফাঁকা)
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
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < 3; ++i)
    {
	পূর্ণসংখ্যা fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	যদি (mch_isatty(fd))
	    ফেরত parts[i];
    }
#endif
    ফেরত PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    স্থির শূন্য
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    অক্ষর   buf[চাবি_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, চাবি_BUF_LEN);

    যদি (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (পূর্ণসংখ্যা)curlen, ফাঁকা);
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
    size_t	prevlen = vterm_output_get_বাফার_current(vterm);
    size_t	limit = term->tl_বাফার->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    যদি (len > limit)
    {
	char_u *p = বার্তা + len - limit;

	p -= (*mb_শীর্ষ_off)(বার্তা, p);
	len -= p - বার্তা;
	বার্তা = p;
    }

    vterm_প্রবেশ_write(vterm, (অক্ষর *)বার্তা, len);

    // flush vterm buffer when vterm responded to control sequence
    যদি (prevlen != vterm_output_get_বাফার_current(vterm))
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
update_cursor(term_T *term, পূর্ণসংখ্যা redraw)
{
    যদি (term->tl_normal_mode)
	ফেরত;
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

	যদি (term->tl_বাফার == curbuf && term->tl_cursor_visible)
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
	++term->tl_বাফার->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, ফাঁকা, ফাঁকা, FALSE, term->tl_বাফার);
	aucmd_restbuf(&aco);

	--term->tl_বাফার->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    স্থির শূন্য
scrollbackline_pos_in_buf(term_T *term, পূর্ণসংখ্যা row, linenr_T *lnum, পূর্ণসংখ্যা *start_col, size_t *start_pos)
{
    sb_সারি_T	*সারিগুলি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য;
    linenr_T    calc_lnum = term->tl_বাফার_scrolled;
    size_t      calc_pos = 0;
    পূর্ণসংখ্যা         calc_col = 0;
    পূর্ণসংখ্যা         i;

    যদি (row < 0 || row >= term->tl_scrollback.ga_len)
	ফেরত;

    যদি (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	জন্য (i = term->tl_scrollback_scrolled + 1; i > 0 && সারিগুলি[i].continuation; --i)
	{
	    calc_pos += সারিগুলি[i - 1].sb_bytes;
	    calc_col += সারিগুলি[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_বাফার_scrolled + 1;
	// Do not count this line's bytes/cols twice
	যদি (সারিগুলি[i].continuation)
	    ++i;
    }
    অন্যথা
    {
	i = 1;
	calc_lnum = 1;
    }

    জন্য (; i <= row; ++i)
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
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, পূর্ণসংখ্যা col, পূর্ণসংখ্যা *row, পূর্ণসংখ্যা *wrapped_col)
{
    buf_T	*buf = term->tl_বাফার;
    sb_সারি_T	*সারিগুলি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    পূর্ণসংখ্যা         calc_col = col;
    linenr_T    l;

    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	ফেরত;

    যদি (lnum > term->tl_বাফার_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_বাফার_scrolled + 1;

	যখন (calc_row < term->tl_scrollback.ga_len && সারিগুলি[calc_row].continuation)
	    ++calc_row;
    }
    অন্যথা
    {
	calc_row = 0;
	l = 1;
    }

    যখন (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	যদি (!সারিগুলি[calc_row].continuation)
	    ++l;
    }

    যখন (calc_row + 1 < term->tl_scrollback.ga_len && সারিগুলি[calc_row + 1].continuation
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
write_to_term(buf_T *বাফার, char_u *বার্তা, channel_T *channel)
{
    size_t	len = STRLEN(বার্তা);
    term_T	*term = বাফার->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    যদি (term->tl_out_fd != ফাঁকা)
    {
	ch_log(channel, "Writing %d bytes to output file", (পূর্ণসংখ্যা)len);
	fwrite(বার্তা, len, 1, term->tl_out_fd);
	ফেরত;
    }
#endif

    যদি (term->tl_vterm == ফাঁকা)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (পূর্ণসংখ্যা)len);
	ফেরত;
    }
    ch_log(channel, "writing %d bytes to terminal", (পূর্ণসংখ্যা)len);
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
	যদি (বাফার == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#যদি defined(FEAT_TABPANEL)
	    যদি (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    যদি (বাফার == curbuf && curbuf->b_term != ফাঁকা)
		update_cursor(curbuf->b_term, TRUE);
	}
	অন্যথা
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    স্থির পূর্ণসংখ্যা
term_send_mouse(VTerm *vterm, পূর্ণসংখ্যা button, পূর্ণসংখ্যা pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    পূর্ণসংখ্যা		    row = mouse_row - W_WINROW(curwin);
    পূর্ণসংখ্যা		    col = mouse_col - curwin->w_wincol;

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
    ফেরত TRUE;
}

স্থির পূর্ণসংখ্যা enter_mouse_col = -1;
স্থির পূর্ণসংখ্যা enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    স্থির পূর্ণসংখ্যা
term_mouse_click(VTerm *vterm, পূর্ণসংখ্যা চাবি)
{
#যদি defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    স্থির পূর্ণসংখ্যা	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    যদি (state == ফাঁকা)
	ফেরত FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    যদি (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	নির্বাচন (চাবি)
	{
	ক্ষেত্র K_LEFTDRAG:
	ক্ষেত্র K_LEFTRELEASE:
	ক্ষেত্র K_RIGHTDRAG:
	ক্ষেত্র K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		যদি (ignore_drag_release)
		{
		    পূর্ণসংখ্যা save_mouse_col, save_mouse_row;

		    যদি (enter_mouse_col < 0)
			বিরতি;

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
	ক্ষেত্র K_LEFTMOUSE:
	ক্ষেত্র K_RIGHTMOUSE:
		যদি (চাবি == K_LEFTRELEASE || চাবি == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		অন্যথা
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		যদি (clip_star.available)
		{
		    পূর্ণসংখ্যা	    button, is_click, is_drag;

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
		বিরতি;

	ক্ষেত্র K_MIDDLEMOUSE:
		যদি (clip_star.available)
		    ঢোকাও_reg('*', TRUE);
		বিরতি;
	}
	enter_mouse_col = -1;
	ফেরত FALSE;
    }
#endif
    enter_mouse_col = -1;

    নির্বাচন (চাবি)
    {
	ক্ষেত্র K_LEFTMOUSE:
	ক্ষেত্র K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); বিরতি;
	ক্ষেত্র K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); বিরতি;
	ক্ষেত্র K_LEFTRELEASE:
	ক্ষেত্র K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); বিরতি;
	ক্ষেত্র K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); বিরতি;
	ক্ষেত্র K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); বিরতি;
	ক্ষেত্র K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); বিরতি;
	ক্ষেত্র K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); বিরতি;
	ক্ষেত্র K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); বিরতি;
	ক্ষেত্র K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); বিরতি;
	ক্ষেত্র K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); বিরতি;
    }
    ফেরত TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    স্থির পূর্ণসংখ্যা
term_convert_চাবি(term_T *term, পূর্ণসংখ্যা c, পূর্ণসংখ্যা modmask, অক্ষর *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_চাবি	    চাবি = VTERM_চাবি_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    পূর্ণসংখ্যা		    other = FALSE;

    নির্বাচন (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	ক্ষেত্র K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    যদি (term->tl_conpty)
		চাবি = VTERM_চাবি_BACKSPACE;
	    অন্যথা
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    বিরতি;

#ifdef MSWIN
	ক্ষেত্র BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    যদি (term->tl_conpty)
		চাবি = VTERM_চাবি_BACKSPACE;
	    বিরতি;
#endif

	ক্ষেত্র ESC:		চাবি = VTERM_চাবি_ESCAPE; বিরতি;
	ক্ষেত্র K_DEL:		চাবি = VTERM_চাবি_DEL; বিরতি;
	ক্ষেত্র K_DOWN:		চাবি = VTERM_চাবি_DOWN; বিরতি;
	ক্ষেত্র K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_DOWN; বিরতি;
	ক্ষেত্র K_END:		চাবি = VTERM_চাবি_END; বিরতি;
	ক্ষেত্র K_S_END:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_END; বিরতি;
	ক্ষেত্র K_C_END:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_END; বিরতি;
	ক্ষেত্র K_F10:		চাবি = VTERM_চাবি_FUNCTION(10); বিরতি;
	ক্ষেত্র K_F11:		চাবি = VTERM_চাবি_FUNCTION(11); বিরতি;
	ক্ষেত্র K_F12:		চাবি = VTERM_চাবি_FUNCTION(12); বিরতি;
	ক্ষেত্র K_F1:		চাবি = VTERM_চাবি_FUNCTION(1); বিরতি;
	ক্ষেত্র K_F2:		চাবি = VTERM_চাবি_FUNCTION(2); বিরতি;
	ক্ষেত্র K_F3:		চাবি = VTERM_চাবি_FUNCTION(3); বিরতি;
	ক্ষেত্র K_F4:		চাবি = VTERM_চাবি_FUNCTION(4); বিরতি;
	ক্ষেত্র K_F5:		চাবি = VTERM_চাবি_FUNCTION(5); বিরতি;
	ক্ষেত্র K_F6:		চাবি = VTERM_চাবি_FUNCTION(6); বিরতি;
	ক্ষেত্র K_F7:		চাবি = VTERM_চাবি_FUNCTION(7); বিরতি;
	ক্ষেত্র K_F8:		চাবি = VTERM_চাবি_FUNCTION(8); বিরতি;
	ক্ষেত্র K_F9:		চাবি = VTERM_চাবি_FUNCTION(9); বিরতি;
	ক্ষেত্র K_HOME:		চাবি = VTERM_চাবি_HOME; বিরতি;
	ক্ষেত্র K_S_HOME:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_HOME; বিরতি;
	ক্ষেত্র K_C_HOME:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_HOME; বিরতি;
	ক্ষেত্র K_INS:		চাবি = VTERM_চাবি_INS; বিরতি;
	ক্ষেত্র K_K0:		চাবি = VTERM_চাবি_KP_0; বিরতি;
	ক্ষেত্র K_K1:		চাবি = VTERM_চাবি_KP_1; বিরতি;
	ক্ষেত্র K_K2:		চাবি = VTERM_চাবি_KP_2; বিরতি;
	ক্ষেত্র K_K3:		চাবি = VTERM_চাবি_KP_3; বিরতি;
	ক্ষেত্র K_K4:		চাবি = VTERM_চাবি_KP_4; বিরতি;
	ক্ষেত্র K_K5:		চাবি = VTERM_চাবি_KP_5; বিরতি;
	ক্ষেত্র K_K6:		চাবি = VTERM_চাবি_KP_6; বিরতি;
	ক্ষেত্র K_K7:		চাবি = VTERM_চাবি_KP_7; বিরতি;
	ক্ষেত্র K_K8:		চাবি = VTERM_চাবি_KP_8; বিরতি;
	ক্ষেত্র K_K9:		চাবি = VTERM_চাবি_KP_9; বিরতি;
	ক্ষেত্র K_KDEL:		চাবি = VTERM_চাবি_DEL; বিরতি; // TODO
	ক্ষেত্র K_KDIVIDE:		চাবি = VTERM_চাবি_KP_DIVIDE; বিরতি;
	ক্ষেত্র K_KEND:		চাবি = VTERM_চাবি_KP_1; বিরতি; // TODO
	ক্ষেত্র K_KENTER:		চাবি = VTERM_চাবি_KP_ENTER; বিরতি;
	ক্ষেত্র K_KHOME:		চাবি = VTERM_চাবি_KP_7; বিরতি; // TODO
	ক্ষেত্র K_KINS:		চাবি = VTERM_চাবি_KP_0; বিরতি; // TODO
	ক্ষেত্র K_KMINUS:		চাবি = VTERM_চাবি_KP_MINUS; বিরতি;
	ক্ষেত্র K_KMULTIPLY:	চাবি = VTERM_চাবি_KP_MULT; বিরতি;
	ক্ষেত্র K_KPAGEDOWN:	চাবি = VTERM_চাবি_KP_3; বিরতি; // TODO
	ক্ষেত্র K_KPAGEUP:		চাবি = VTERM_চাবি_KP_9; বিরতি; // TODO
	ক্ষেত্র K_KPLUS:		চাবি = VTERM_চাবি_KP_PLUS; বিরতি;
	ক্ষেত্র K_KPOINT:		চাবি = VTERM_চাবি_KP_PERIOD; বিরতি;
	ক্ষেত্র K_বাম:		চাবি = VTERM_চাবি_বাম; বিরতি;
	ক্ষেত্র K_S_বাম:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_বাম; বিরতি;
	ক্ষেত্র K_C_বাম:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_বাম; বিরতি;
	ক্ষেত্র K_PAGEDOWN:	চাবি = VTERM_চাবি_PAGEDOWN; বিরতি;
	ক্ষেত্র K_PAGEUP:		চাবি = VTERM_চাবি_PAGEUP; বিরতি;
	ক্ষেত্র K_ডান:		চাবি = VTERM_চাবি_ডান; বিরতি;
	ক্ষেত্র K_S_ডান:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_ডান; বিরতি;
	ক্ষেত্র K_C_ডান:		mod = VTERM_MOD_CTRL;
				চাবি = VTERM_চাবি_ডান; বিরতি;
	ক্ষেত্র K_UP:		চাবি = VTERM_চাবি_UP; বিরতি;
	ক্ষেত্র K_S_UP:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_UP; বিরতি;
	ক্ষেত্র TAB:		চাবি = VTERM_চাবি_TAB; বিরতি;
	ক্ষেত্র K_S_TAB:		mod = VTERM_MOD_SHIFT;
				চাবি = VTERM_চাবি_TAB; বিরতি;

	ক্ষেত্র K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); বিরতি;
	ক্ষেত্র K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); বিরতি;
	ক্ষেত্র K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); বিরতি;
	ক্ষেত্র K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); বিরতি;

	ক্ষেত্র K_LEFTMOUSE:
	ক্ষেত্র K_LEFTMOUSE_NM:
	ক্ষেত্র K_LEFTDRAG:
	ক্ষেত্র K_LEFTRELEASE:
	ক্ষেত্র K_LEFTRELEASE_NM:
	ক্ষেত্র K_MOUSEMOVE:
	ক্ষেত্র K_MIDDLEMOUSE:
	ক্ষেত্র K_MIDDLEDRAG:
	ক্ষেত্র K_MIDDLERELEASE:
	ক্ষেত্র K_RIGHTMOUSE:
	ক্ষেত্র K_RIGHTDRAG:
	ক্ষেত্র K_RIGHTRELEASE:	যদি (!term_mouse_click(vterm, c))
				    ফেরত 0;
				other = TRUE;
				বিরতি;

	ক্ষেত্র K_X1MOUSE:		/* TODO */ ফেরত 0;
	ক্ষেত্র K_X1DRAG:		/* TODO */ ফেরত 0;
	ক্ষেত্র K_X1RELEASE:	/* TODO */ ফেরত 0;
	ক্ষেত্র K_X2MOUSE:		/* TODO */ ফেরত 0;
	ক্ষেত্র K_X2DRAG:		/* TODO */ ফেরত 0;
	ক্ষেত্র K_X2RELEASE:	/* TODO */ ফেরত 0;

	ক্ষেত্র K_IGNORE:		ফেরত 0;
	ক্ষেত্র K_NOP:		ফেরত 0;
	ক্ষেত্র K_UNDO:		ফেরত 0;
	ক্ষেত্র K_HELP:		ফেরত 0;
	ক্ষেত্র K_XF1:		চাবি = VTERM_চাবি_FUNCTION(1); বিরতি;
	ক্ষেত্র K_XF2:		চাবি = VTERM_চাবি_FUNCTION(2); বিরতি;
	ক্ষেত্র K_XF3:		চাবি = VTERM_চাবি_FUNCTION(3); বিরতি;
	ক্ষেত্র K_XF4:		চাবি = VTERM_চাবি_FUNCTION(4); বিরতি;
	ক্ষেত্র K_SELECT:		ফেরত 0;
#ifdef FEAT_GUI
	ক্ষেত্র K_VER_SCROLLBAR:	ফেরত 0;
	ক্ষেত্র K_HOR_SCROLLBAR:	ফেরত 0;
#endif
#ifdef FEAT_GUI_TABLINE
	ক্ষেত্র K_TABLINE:		ফেরত 0;
	ক্ষেত্র K_TABMENU:		ফেরত 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	ক্ষেত্র K_F21:		চাবি = VTERM_চাবি_FUNCTION(21); বিরতি;
#endif
#ifdef FEAT_DND
	ক্ষেত্র K_DROP:		ফেরত 0;
#endif
	ক্ষেত্র K_CURSORHOLD:	ফেরত 0;
	ক্ষেত্র K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				বিরতি;
	ক্ষেত্র K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				বিরতি;
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
    ফেরত (পূর্ণসংখ্যা)vterm_output_read(vterm, buf, চাবি_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    স্থির পূর্ণসংখ্যা
term_job_running_check(term_T *term, পূর্ণসংখ্যা check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    যদি (term == ফাঁকা
	|| term->tl_job == ফাঁকা
	|| !channel_is_open(term->tl_job->jv_channel))
	ফেরত FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    যদি (check_job_status)
	job_status(job);
    ফেরত (job->jv_status == JOB_STARTED
	    || (job->jv_channel != ফাঁকা && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    পূর্ণসংখ্যা
term_job_running(term_T *term)
{
    ফেরত term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    পূর্ণসংখ্যা
term_job_running_not_none(term_T *term)
{
    ফেরত term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    পূর্ণসংখ্যা
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ফেরত term != ফাঁকা
	&& term->tl_job != ফাঁকা
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    পূর্ণসংখ্যা
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_বার্তা_আকার];
    পূর্ণসংখ্যা	ret;

    dialog_বার্তা(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ফাঁকা, buff, 1);
    যদি (ret == VIM_YES)
	ফেরত OK;
    অন্যথা
	ফেরত FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    পূর্ণসংখ্যা
term_try_stop_job(buf_T *buf)
{
    পূর্ণসংখ্যা	    গুনতি;
    অক্ষর    *how = (অক্ষর *)buf->b_term->tl_kill;

#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    যদি ((how == ফাঁকা || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	যদি (term_confirm_stop(buf) == OK)
	    how = "kill";
	অন্যথা
	    ফেরত FAIL;
    }
#endif
    যদি (how == ফাঁকা || *how == NUL)
	ফেরত FAIL;

    job_stop(buf->b_term->tl_job, ফাঁকা, how);

    // wait for up to a second for the job to die
    জন্য (গুনতি = 0; গুনতি < 100; ++গুনতি)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	যদি (!buf_valid(buf)
		|| buf->b_term == ফাঁকা
		|| buf->b_term->tl_job == ফাঁকা)
	    ফেরত OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	যদি (job->jv_status >= JOB_ENDED)
	    ফেরত OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    ফেরত FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    স্থির শূন্য
add_scrollback_সারি_to_বাফার(term_T *term, char_u *text, পূর্ণসংখ্যা len, পূর্ণসংখ্যা append)
{
    buf_T	*buf = term->tl_বাফার;
    পূর্ণসংখ্যা		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_সারি_গুনতি;

#ifdef MSWIN
    char_u *tmp = text;
    যদি (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ফাঁকা;
	পূর্ণসংখ্যা	দৈর্ঘ্য = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (অক্ষর*)text, len + 1,
							   &ret, &দৈর্ঘ্য);
	যদি (ret != ফাঁকা)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, দৈর্ঘ্য, (অক্ষর **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    যদি (append)
    {
	char_u *পূর্ব_text = ml_get_buf(buf, lnum, FALSE);
	size_t পূর্ব_len = STRLEN(পূর্ব_text);

	char_u *both = alloc(len + পূর্ব_len + 2);
	যদি (both == ফাঁকা)
	    ফেরত;
	vim_strncpy(both, পূর্ব_text, পূর্ব_len + 1);
	vim_strncpy(both + পূর্ব_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_বাফার;
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
	curbuf = curwin->w_বাফার;
    }
}

    স্থির শূন্য
cell2cellattr(ধ্রুবক VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    স্থির পূর্ণসংখ্যা
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    ফেরত a->fg.red == b->fg.red
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
    স্থির পূর্ণসংখ্যা
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, পূর্ণসংখ্যা lnum)
{
    যদি (ga_grow(&term->tl_scrollback, 1) == FAIL)
	ফেরত FALSE;

    sb_সারি_T *সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য
	+ term->tl_scrollback.ga_len;

    যদি (lnum > 0)
    {
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *সারি = *(সারি - 1);
	    --সারি;
	}
    }
    সারি->sb_cols = 0;
    সারি->sb_bytes = 0;
    সারি->sb_cells = ফাঁকা;
    সারি->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    ফেরত OK;
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
    size_t      bufline_দৈর্ঘ্য;

    curbuf = term->tl_বাফার;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_সারি_গুনতি, FALSE);
    bufline_দৈর্ঘ্য = STRLEN(bufline);
    যখন (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	সারি = (sb_সারি_T *)gap->ga_তথ্য + gap->ga_len - 1;
	যদি (সারি->sb_bytes < 0 || (size_t)সারি->sb_bytes > bufline_দৈর্ঘ্য)
	    বিরতি;
	bufline_দৈর্ঘ্য -= সারি->sb_bytes;
	যদি (!bufline_দৈর্ঘ্য)
	{
	    ml_delete(curbuf->b_ml.ml_সারি_গুনতি);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_সারি_গুনতি, FALSE);
	    bufline_দৈর্ঘ্য = STRLEN(bufline);
	}
	vim_free(সারি->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    যদি (bufline_দৈর্ঘ্য < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_দৈর্ঘ্য);
	ml_replace(curbuf->b_ml.ml_সারি_গুনতি, shortened, FALSE);
    }
    curbuf = curwin->w_বাফার;
    যদি (curbuf == term->tl_বাফার)
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
    পূর্ণসংখ্যা		    len;
    পূর্ণসংখ্যা		    সারিগুলি_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == ফাঁকা ? ফাঁকা : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    জন্য (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	জন্য (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
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
	    যখন (সারিগুলি_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--সারিগুলি_skipped;
		যদি (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_সারি_to_বাফার(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    যদি (len == 0)
		p = ফাঁকা;
	    অন্যথা
		p = ALLOC_MULT(cellattr_T, len);
	    যদি ((p != ফাঁকা || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		পূর্ণসংখ্যা	    width;
		ধ্রুবক VTerm_সারি_Info *lineinfo;
		sb_সারি_T   *সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		জন্য (pos.col = 0; pos.col < len; pos.col += width)
		{
		    যদি (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			যদি (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_তথ্য + ga.ga_len);
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
			    পূর্ণসংখ্যা	    i;
			    পূর্ণসংখ্যা	    c;

			    জন্য (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_তথ্য + ga.ga_len);
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
		    add_scrollback_সারি_to_বাফার(term, (char_u *)"", 0, 0);
		অন্যথা
		{
		    *((char_u *)ga.ga_তথ্য + ga.ga_len) = NUL;
		    add_scrollback_সারি_to_বাফার(term, ga.ga_তথ্য, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    অন্যথা
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    জন্য (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	যদি (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_সারি_to_বাফার(term, (char_u *)"", 0, 0);
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
    স্থির পূর্ণসংখ্যা
for_all_windows_and_curwin(win_T **wp, পূর্ণসংখ্যা *did_curwin)
{
    যদি (*wp == ফাঁকা)
	*wp = firstwin;
    অন্যথা যদি ((*wp)->w_পরবর্তী != ফাঁকা)
	*wp = (*wp)->w_পরবর্তী;
    অন্যথা যদি (!*did_curwin)
	*wp = curwin;
    অন্যথা
	ফেরত FALSE;
    যদি (*wp == curwin)
	*did_curwin = TRUE;
    ফেরত TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    শূন্য
may_move_terminal_to_বাফার(term_T *term, পূর্ণসংখ্যা redraw)
{
    যদি (term->tl_vterm == ফাঁকা)
	ফেরত;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    যদি (term->tl_dirty_snapshot || term->tl_বাফার->b_ml.ml_সারি_গুনতি
					       <= term->tl_বাফার_scrolled)
	update_snapshot(term);

    যদি (redraw)
    {
	win_T	    *wp = ফাঁকা;
	পূর্ণসংখ্যা	    did_curwin = FALSE;

	যখন (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    যদি (wp->w_বাফার == term->tl_বাফার)
	    {
		wp->w_cursor.lnum = term->tl_বাফার->b_ml.ml_সারি_গুনতি;
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
    পূর্ণসংখ্যা
term_check_timers(পূর্ণসংখ্যা পরবর্তী_due_arg, proftime_T *now)
{
    term_T  *term;
    পূর্ণসংখ্যা	    পরবর্তী_due = পরবর্তী_due_arg;

    FOR_ALL_TERMS(term)
    {
	যদি (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_বাম(&term->tl_timer_due, now);

	    যদি (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_বাফার(term, FALSE);
	    }
	    অন্যথা যদি (পরবর্তী_due == -1 || পরবর্তী_due > this_due)
		পরবর্তী_due = this_due;
	}
    }

    ফেরত পরবর্তী_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    স্থির শূন্য
set_terminal_mode(term_T *term, পূর্ণসংখ্যা normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    যদি (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    যদি (term->tl_বাফার == curbuf)
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
	may_move_terminal_to_বাফার(term, TRUE);
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
    পূর্ণসংখ্যা        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_বাফার(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_বাফার_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, ফাঁকা);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    যদি (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_বাফার_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    পূর্ণসংখ্যা
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    ফেরত term != ফাঁকা && term->tl_normal_mode;
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
প্রকারলেখ গণনা {
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
    স্থির পূর্ণসংখ্যা
vterm_using_চাবি_protocol(শূন্য)
{
    ফেরত curbuf->b_term != ফাঁকা
	&& curbuf->b_term->tl_vterm != ফাঁকা
	&& (vterm_is_modify_other_চাবিগুলি(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    শূন্য
check_no_reduce_চাবিগুলি(শূন্য)
{
    যদি (no_reduce_চাবি_state != NRKS_CHECK
	    || no_reduce_চাবিগুলি >= 1
	    || curbuf->b_term == ফাঁকা
	    || curbuf->b_term->tl_vterm == ফাঁকা)
	ফেরত;

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
    স্থির পূর্ণসংখ্যা
term_vgetc(শূন্য)
{
    পূর্ণসংখ্যা c;
    পূর্ণসংখ্যা save_State = State;

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

    ফেরত c;
}

স্থির পূর্ণসংখ্যা	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    পূর্ণসংখ্যা
send_চাবিগুলি_to_term(term_T *term, পূর্ণসংখ্যা c, পূর্ণসংখ্যা modmask, পূর্ণসংখ্যা typed)
{
    অক্ষর	বার্তা[চাবি_BUF_LEN];
    size_t	len;
    পূর্ণসংখ্যা		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    নির্বাচন (c)
    {
	ক্ষেত্র NUL:
	ক্ষেত্র K_ZERO:
	    যদি (typed)
		stuffcharReadbuff(c);
	    ফেরত FAIL;

	ক্ষেত্র K_TABLINE:
	    stuffcharReadbuff(c);
	    ফেরত FAIL;

	ক্ষেত্র K_IGNORE:
	ক্ষেত্র K_CANCEL:  // used for :normal when running out of chars
	    ফেরত FAIL;

	ক্ষেত্র K_LEFTDRAG:
	ক্ষেত্র K_MIDDLEDRAG:
	ক্ষেত্র K_RIGHTDRAG:
	ক্ষেত্র K_X1DRAG:
	ক্ষেত্র K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	ক্ষেত্র K_LEFTMOUSE:
	ক্ষেত্র K_LEFTMOUSE_NM:
	ক্ষেত্র K_LEFTRELEASE:
	ক্ষেত্র K_LEFTRELEASE_NM:
	ক্ষেত্র K_MOUSEMOVE:
	ক্ষেত্র K_MIDDLEMOUSE:
	ক্ষেত্র K_MIDDLERELEASE:
	ক্ষেত্র K_RIGHTMOUSE:
	ক্ষেত্র K_RIGHTRELEASE:
	ক্ষেত্র K_X1MOUSE:
	ক্ষেত্র K_X1RELEASE:
	ক্ষেত্র K_X2MOUSE:
	ক্ষেত্র K_X2RELEASE:

	ক্ষেত্র K_MOUSEUP:
	ক্ষেত্র K_MOUSEDOWN:
	ক্ষেত্র K_MOUSELEFT:
	ক্ষেত্র K_MOUSERIGHT:
	    {
		পূর্ণসংখ্যা	row = mouse_row;
		পূর্ণসংখ্যা	col = mouse_col;

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
		    ফেরত FAIL;
		}
	    }
	    বিরতি;

	ক্ষেত্র K_COMMAND:
	ক্ষেত্র K_SCRIPT_COMMAND:
	    ফেরত do_cmdkey_command(c, 0);
    }
    যদি (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_চাবি(term, c, modmask, বার্তা);
    যদি (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)বার্তা, (পূর্ণসংখ্যা)len, ফাঁকা);

    ফেরত OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    স্থির শূন্য
term_paste_register(পূর্ণসংখ্যা পূর্ব_c UNUSED)
{
    পূর্ণসংখ্যা		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    পূর্ণসংখ্যা		type;

    যদি (add_to_showcmd(পূর্ব_c))
    যদি (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    যদি (!term_use_loop())
	// job finished while waiting for a character
	ফেরত;

    // CTRL-W "= prompt for expression to evaluate.
    যদি (c == '=' && get_রাশিমালা_register() != '=')
	ফেরত;
    যদি (!term_use_loop())
	// job finished while waiting for a character
	ফেরত;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    যদি (l == ফাঁকা)
	ফেরত;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_বস্তুসমূহ(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	যদি (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ফাঁকা;
	    পূর্ণসংখ্যা	দৈর্ঘ্য = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (অক্ষর *)s,
		    (পূর্ণসংখ্যা)STRLEN(s), &ret, &দৈর্ঘ্য);
	    যদি (ret != ফাঁকা)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, দৈর্ঘ্য, (অক্ষর **)&s, &দৈর্ঘ্য, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (পূর্ণসংখ্যা)STRLEN(s), ফাঁকা);
#ifdef MSWIN
	যদি (tmp != s)
	    vim_free(s);
#endif

	যদি (item->li_পরবর্তী != ফাঁকা || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, ফাঁকা);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    পূর্ণসংখ্যা
terminal_is_active(শূন্য)
{
    ফেরত in_terminal_loop != ফাঁকা;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    স্থির পূর্ণসংখ্যা
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    যদি (wp != ফাঁকা && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    অন্যথা যদি (term->tl_highlight_name != ফাঁকা)
	name = term->tl_highlight_name;
    অন্যথা
	name = (char_u*)"Terminal";

    ফেরত syn_name2id(name);
}

#যদি defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    স্থির cursorentry_T entry;
    পূর্ণসংখ্যা			 id;
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

    যদি (term->tl_cursor_color == ফাঁকা)
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

    ফেরত &entry;
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
	ফেরত;
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
	ফেরত;
#endif
    cursor_color_copy(&desired_cursor_color, ফাঁকা);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    স্থির পূর্ণসংখ্যা
term_use_loop_check(পূর্ণসংখ্যা check_job_status)
{
    term_T *term = curbuf->b_term;

    ফেরত term != ফাঁকা
	&& !term->tl_normal_mode
	&& term->tl_vterm != ফাঁকা
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    পূর্ণসংখ্যা
term_use_loop(শূন্য)
{
    ফেরত term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    শূন্য
term_win_entered(শূন্য)
{
    term_T *term = curbuf->b_term;

    যদি (term == ফাঁকা)
	ফেরত;

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
term_focus_change(পূর্ণসংখ্যা in_focus)
{
    term_T *term = curbuf->b_term;

    যদি (term == ফাঁকা || term->tl_vterm == ফাঁকা)
	ফেরত;

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
    স্থির পূর্ণসংখ্যা
raw_c_to_ctrl(পূর্ণসংখ্যা c)
{
    যদি ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	ফেরত c & 0x1f;
    ফেরত c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    স্থির পূর্ণসংখ্যা
ctrl_to_raw_c(পূর্ণসংখ্যা c)
{
    যদি (c < 0x20 && vterm_using_চাবি_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	ফেরত c + '@';
    }
    ফেরত c;
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
    পূর্ণসংখ্যা
terminal_loop(পূর্ণসংখ্যা blocking)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		raw_c;
    পূর্ণসংখ্যা		termwinkey = 0;
    পূর্ণসংখ্যা		ret;
#ifdef UNIX
    পূর্ণসংখ্যা		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    পূর্ণসংখ্যা		restore_cursor = FALSE;

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

    যখন (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	যদি (curbuf->b_term != ফাঁকা && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    যখন (must_redraw != 0)
		যদি (update_screen(0) == FAIL)
		    বিরতি;
	যদি (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    বিরতি;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	যদি (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    যদি (raw_c != K_IGNORE)
		vungetc(raw_c);
	    বিরতি;
	}
	যদি (raw_c == K_IGNORE)
	    চালু;
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
	    পূর্ণসংখ্যা	    পূর্ব_c = c;
	    পূর্ণসংখ্যা	    পূর্ব_raw_c = raw_c;
	    পূর্ণসংখ্যা	    পূর্ব_mod_mask = mod_mask;

	    যদি (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    যদি (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		বিরতি;

	    যদি (পূর্ব_c == Ctrl_BSL)
	    {
		যদি (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    যাও theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_চাবিগুলি_to_term(curbuf->b_term, পূর্ব_raw_c, পূর্ব_mod_mask,
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
		যাও theend;
	    }
	    অন্যথা যদি (c == '"')
	    {
		term_paste_register(পূর্ব_c);
		চালু;
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
		যাও theend;
	    }
	}
#ifdef MSWIN
	যদি (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    যদি (MultiByteToWideChar(GetACP(), 0, (অক্ষর*)mb, 2, &wc, 1) > 0)
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
	    যাও theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = ফাঁকা;
    যদি (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    যদি (curbuf->b_term != ফাঁকা && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_বাফার(curbuf->b_term, FALSE);

    ফেরত ret;
}

    স্থির শূন্য
may_toggle_cursor(term_T *term)
{
    যদি (in_terminal_loop != term)
	ফেরত;

    যদি (term->tl_cursor_visible)
	cursor_on();
    অন্যথা
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    স্থির পূর্ণসংখ্যা
color2index(VTermColor *color, পূর্ণসংখ্যা fg, পূর্ণসংখ্যা *boldp)
{
    পূর্ণসংখ্যা red = color->red;
    পূর্ণসংখ্যা blue = color->blue;
    পূর্ণসংখ্যা green = color->green;

    *boldp = FALSE;

    যদি (VTERM_COLOR_IS_INVALID(color))
	ফেরত 0;

    যদি (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	যদি (color->সূচক < t_colors)
	{
	    uint8_t সূচক = color->সূচক;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    যদি (সূচক < 16)
		সূচক = cterm_ansi_idx[সূচক];
#endif
	    ফেরত সূচক + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	অন্যথা যদি (t_colors == 8 && fg && color->সূচক < 16)
	{
	    *boldp = TRUE;
	    ফেরত (color->সূচক & 7) + 1;
	}
	ফেরত 0;
    }

    যদি (t_colors >= 256)
    {
	যদি (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    স্থির পূর্ণসংখ্যা cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    পূর্ণসংখ্যা i;

	    যদি (red < 5)
		ফেরত 17; // 00/00/00
	    যদি (red > 245) // ff/ff/ff
		ফেরত 232;
	    জন্য (i = 0; i < 23; ++i)
		যদি (red < cutoff[i])
		    ফেরত i + 233;
	    ফেরত 256;
	}
	{
	    স্থির পূর্ণসংখ্যা cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    পূর্ণসংখ্যা ri, gi, bi;

	    // 216-color cube
	    জন্য (ri = 0; ri < 5; ++ri)
		যদি (red < cutoff[ri])
		    বিরতি;
	    জন্য (gi = 0; gi < 5; ++gi)
		যদি (green < cutoff[gi])
		    বিরতি;
	    জন্য (bi = 0; bi < 5; ++bi)
		যদি (blue < cutoff[bi])
		    বিরতি;
	    ফেরত 17 + ri * 36 + gi * 6 + bi;
	}
    }
    ফেরত 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    স্থির পূর্ণসংখ্যা
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    পূর্ণসংখ্যা attr = 0;

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
    ফেরত attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    স্থির শূন্য
hl2vtermAttr(পূর্ণসংখ্যা attr, cellattr_T *cell)
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
    স্থির পূর্ণসংখ্যা
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    পূর্ণসংখ্যা attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    পূর্ণসংখ্যা is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    পূর্ণসংখ্যা is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    যদি (is_default_fg || is_default_bg)
    {
	যদি (wp != ফাঁকা && wp->w_hlfwin_id != 0)
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
	ফেরত get_gui_attr_idx(attr, guifg, guibg);
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
	ফেরত get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    অন্যথা
#endif
    {
	পূর্ণসংখ্যা bold = MAYBE;
	পূর্ণসংখ্যা ctermfg = color2index(fg, TRUE, &bold);
	পূর্ণসংখ্যা ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	যদি (bold == TRUE)
	    attr |= HL_BOLD;

	ফেরত get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    ফেরত 0;
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

    স্থির পূর্ণসংখ্যা
handle_damage(VTermRect rect, শূন্য *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_বাফার, UPD_SOME_VALID);
    ফেরত 1;
}

    স্থির শূন্য
term_scroll_up(term_T *term, পূর্ণসংখ্যা start_row, পূর্ণসংখ্যা গুনতি)
{
    win_T		 *wp = ফাঁকা;
    পূর্ণসংখ্যা			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    পূর্ণসংখ্যা			 clear_attr;

    CLEAR_FIELD(attr);

    যখন (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	যদি (wp->w_বাফার == term->tl_বাফার)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_সারিগুলি(wp, start_row, গুনতি, FALSE, FALSE, clear_attr);
	}
    }
}

    স্থির পূর্ণসংখ্যা
handle_moverect(VTermRect dest, VTermRect src, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    পূর্ণসংখ্যা		গুনতি = src.start_row - dest.start_row;

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
    redraw_buf_later(term->tl_বাফার, UPD_NOT_VALID);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	পূর্ণসংখ্যা visible,
	শূন্য *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = ফাঁকা;
    পূর্ণসংখ্যা		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    যখন (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	যদি (wp->w_বাফার == term->tl_বাফার)
	    position_cursor(wp, &pos);
    }
    যদি (term->tl_বাফার == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
handle_settermprop(
	VTermProp prop,
	VTerm_মান *মান,
	শূন্য *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = ফাঁকা;

    নির্বাচন (prop)
    {
	ক্ষেত্র VTERM_PROP_TITLE:
	    যদি (disable_vterm_title_for_testing)
		বিরতি;
	    strval = vim_strnsave((char_u *)মান->string.str,
							    মান->string.len);
	    যদি (strval == ফাঁকা)
		বিরতি;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    যদি (*skipwhite(strval) == NUL)
		term->tl_title = ফাঁকা;
	    // Same as blank
	    অন্যথা যদি (term->tl_arg0_cmd != ফাঁকা
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (পূর্ণসংখ্যা)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = ফাঁকা;
	    // Empty corrupted data of winpty
	    অন্যথা যদি (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = ফাঁকা;
#ifdef MSWIN
	    অন্যথা যদি (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ফাঁকা;
		পূর্ণসংখ্যা	দৈর্ঘ্য = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(অক্ষর*)মান->string.str,
					(পূর্ণসংখ্যা)মান->string.len, &ret, &দৈর্ঘ্য);
		যদি (ret != ফাঁকা)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, দৈর্ঘ্য, (অক্ষর**)&term->tl_title,
					&দৈর্ঘ্য, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    অন্যথা
	    {
		term->tl_title = strval;
		strval = ফাঁকা;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    যদি (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    বিরতি;

	ক্ষেত্র VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = মান->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    বিরতি;

	ক্ষেত্র VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = মান->boolean;
	    may_set_cursor_props(term);
	    বিরতি;

	ক্ষেত্র VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = মান->সংখ্যা;
	    may_set_cursor_props(term);
	    বিরতি;

	ক্ষেত্র VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)মান->string.str,
							    মান->string.len);
	    যদি (strval == ফাঁকা)
		বিরতি;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    বিরতি;

	ক্ষেত্র VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = মান->boolean;
	    বিরতি;

	পূর্বনির্ধারিত:
	    বিরতি;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    ফেরত 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    স্থির পূর্ণসংখ্যা
handle_resize(পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    যদি (term->tl_vterm_আকার_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_আকার_changed = FALSE;
    অন্যথা
    {
	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_বাফার == term->tl_বাফার)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_বাফার, UPD_NOT_VALID);
    }
    ফেরত 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    স্থির শূন্য
limit_scrollback(term_T *term, garray_T *gap, পূর্ণসংখ্যা update_বাফার)
{
    যদি (gap->ga_len < term->tl_বাফার->b_p_twsl)
	ফেরত;

    পূর্ণসংখ্যা	todo = MAX(term->tl_বাফার->b_p_twsl / 10,
				     gap->ga_len - term->tl_বাফার->b_p_twsl);
    পূর্ণসংখ্যা	i;
    sb_সারি_T *sb_সারিগুলি = (sb_সারি_T *)gap->ga_তথ্য;

    curbuf = term->tl_বাফার;
    জন্য (i = 0; i < todo; ++i)
    {
	যদি (update_বাফার && (!sb_সারিগুলি[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_বাফার_scrolled;
	}
	vim_free(sb_সারিগুলি[i].sb_cells);
    }
    // Continue until end of wrapped line
    জন্য (; todo < gap->ga_len && sb_সারিগুলি[todo].continuation; ++todo)
	vim_free(sb_সারিগুলি[todo].sb_cells);
    curbuf = curwin->w_বাফার;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_তথ্য,
		(sb_সারি_T *)gap->ga_তথ্য + todo,
		আকার(sb_সারি_T) * gap->ga_len);
    যদি (update_বাফার)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ফাঁকা;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_বাফার == term->tl_বাফার)
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
    স্থির পূর্ণসংখ্যা
handle_pushline(পূর্ণসংখ্যা cols, ধ্রুবক VTermScreenCell *cells, পূর্ণসংখ্যা continuation, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    পূর্ণসংখ্যা		update_বাফার;

    যদি (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_বাফার = FALSE;
    }
    অন্যথা
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_বাফার = TRUE;
    }

    limit_scrollback(term, gap, update_বাফার);

    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত 0;

    cellattr_T	*p = ফাঁকা;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		text_len;
    char_u		*text;
    sb_সারি_T	*সারি;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    জন্য (i = 0; i < cols; ++i)
	যদি (cells[i].chars[0] != 0)
	    len = i + 1;
	অন্যথা
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    যদি (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    যদি (p != ফাঁকা)
    {
	জন্য (col = 0; col < len; col += cells[col].width)
	{
	    যদি (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		বিরতি;
	    }
	    জন্য (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_তথ্য + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    যদি (ga_grow(&ga, 1) == FAIL)
    {
	যদি (update_বাফার)
	    text = (char_u *)"";
	অন্যথা
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    অন্যথা
    {
	text = ga.ga_তথ্য;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    যদি (update_বাফার)
	add_scrollback_সারি_to_বাফার(term, text, text_len, continuation);

    সারি = (sb_সারি_T *)gap->ga_তথ্য + gap->ga_len;
    সারি->sb_cols = len;
    সারি->sb_bytes = text_len;
    সারি->sb_cells = p;
    সারি->sb_fill_attr = fill_attr;
    সারি->continuation = (char_u)continuation;
    যদি (update_বাফার)
    {
	সারি->sb_text = ফাঁকা;
	++term->tl_scrollback_scrolled;
	যদি (!continuation)
	    ++term->tl_বাফার_scrolled;
	ga_clear(&ga);  // free the text
    }
    অন্যথা
    {
	সারি->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    ফেরত 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    স্থির শূন্য
handle_postponed_scrollback(term_T *term)
{
    পূর্ণসংখ্যা i;

    যদি (term->tl_scrollback_postponed.ga_len == 0)
	ফেরত;
    ch_log(ফাঁকা, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    জন্য (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_সারি_T	*pp_সারি;
	sb_সারি_T	*সারি;

	যদি (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    বিরতি;
	pp_সারি = (sb_সারি_T *)term->tl_scrollback_postponed.ga_তথ্য + i;

	text = pp_সারি->sb_text;
	যদি (text == ফাঁকা)
	    text = (char_u *)"";
	add_scrollback_সারি_to_বাফার(term, text, (পূর্ণসংখ্যা)STRLEN(text), pp_সারি->continuation);
	vim_free(pp_সারি->sb_text);

	সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য
						 + term->tl_scrollback.ga_len;
	সারি->sb_cols = pp_সারি->sb_cols;
	সারি->sb_bytes = pp_সারি->sb_bytes;
	সারি->sb_cells = pp_সারি->sb_cells;
	সারি->sb_fill_attr = pp_সারি->sb_fill_attr;
	সারি->sb_text = ফাঁকা;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	যদি (!pp_সারি->continuation)
	    ++term->tl_বাফার_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    স্থির পূর্ণসংখ্যা
handle_bell(শূন্য *user UNUSED)
{
    vim_beep(BO_TERM);
    ফেরত 0;
}

স্থির VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ফাঁকা,			// sb_pushline
    ফাঁকা,			// sb_popline
    ফাঁকা,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    স্থির পূর্ণসংখ্যা
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    যদি (!term->tl_normal_mode)
    {
	পূর্ণসংখ্যা	fnum = term->tl_বাফার->b_fnum;

	cleanup_vterm(term);

	যদি (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    পূর্ণসংখ্যা		do_set_w_locked = term->tl_বাফার->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = ফাঁকা;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    যদি (popup_is_popup(curwin) && curbuf == term->tl_বাফার)
	    {
		pwin = curwin;
		যদি (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    অন্যথা
#endif
	    // If this is the last normal window: exit Vim.
	    যদি (term->tl_বাফার->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		ফেরত TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ফাঁকা, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_বাফার);
	    যদি (curbuf == term->tl_বাফার)
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
	    যদি (pwin != ফাঁকা)
		popup_close_with_retval(pwin, 0);
#endif
	    ফেরত TRUE;
	}
	যদি (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_বাফার->b_nwindows == 0)
	{
	    অক্ষর    *cmd = term->tl_opencmd == ফাঁকা
				? "botright sbuf %d"
				: (অক্ষর *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    অক্ষর    *buf = alloc(len);

	    যদি (buf != ফাঁকা)
	    {
		ch_log(ফাঁকা, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	অন্যথা
	    ch_log(ফাঁকা, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_বাফার, UPD_NOT_VALID);
    ফেরত FALSE;
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    পূর্ণসংখ্যা
may_close_term_popup(শূন্য)
{
    যদি (!popup_is_popup(curwin) || curbuf->b_term == ফাঁকা
				 || term_job_running_not_none(curbuf->b_term))
	ফেরত FAIL;

    win_T *pwin = curwin;

    যদি (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    ফেরত OK;
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

    জন্য (term = first_term; term != ফাঁকা; term = term->tl_পরবর্তী)
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
    term_T *পরবর্তী_term;
    পূর্ণসংখ্যা	    did_one = FALSE;

    জন্য (term = first_term; term != ফাঁকা; term = পরবর্তী_term)
    {
	পরবর্তী_term = term->tl_পরবর্তী;
	যদি (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    যদি (term->tl_out_fd != ফাঁকা)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = ফাঁকা;
	    }
#endif

	    যদি (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		চালু;
	    }

	    যদি (term_after_channel_closed(term))
		পরবর্তী_term = first_term;
	}
    }

    যদি (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	যদি (term != ফাঁকা)
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
    term_T *পরবর্তী_term;

    জন্য (term = first_term; term != ফাঁকা; term = পরবর্তী_term)
    {
	পরবর্তী_term = term->tl_পরবর্তী;
	যদি (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    যদি (term_after_channel_closed(term))
		// start over, the list may have changed
		পরবর্তী_term = first_term;
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
	পূর্ণসংখ্যা		max_col)
{
    পূর্ণসংখ্যা off = screen_get_current_সারি_off();

    জন্য (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	পূর্ণসংখ্যা		c;

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
		পূর্ণসংখ্যা i;

		// composing chars
		জন্য (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_সারিগুলি_C[i][off] = cell.chars[i + 1];
		    যদি (cell.chars[i + 1] == 0)
			বিরতি;
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
					       (অক্ষর*)mb, 2, 0, 0) > 1)
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

    যদি (term->tl_vterm == ফাঁকা)
	ফেরত;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    যখন (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	পূর্ণসংখ্যা save_p_more = p_more;

	p_more = FALSE;
	বার্তা_row = Rows - 1;
	বার্তা_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    জন্য (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	যদি (pos.row < term->tl_rows)
	{
	    পূর্ণসংখ্যা max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, ফাঁকা, screen, &pos, max_col);
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
    পূর্ণসংখ্যা
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_বাফার->b_term;

    ফেরত term != ফাঁকা && term->tl_vterm != ফাঁকা && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    শূন্য
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_বাফার->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    পূর্ণসংখ্যা		rows, cols;
    পূর্ণসংখ্যা		newrows, newcols;
    পূর্ণসংখ্যা		minsize;
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
    জন্য (twp = firstwin; ; twp = twp->w_পরবর্তী)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ফাঁকা ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	যদি (wwp->w_বাফার == term->tl_বাফার)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	যদি (twp == ফাঁকা)
	    বিরতি;
    }
    যদি (newrows == 99999 || newcols == 99999)
	ফেরত; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    যদি (newrows == 0 || newcols == 0)
	ফেরত;

    যদি (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_আকার_changed = TRUE;
	vterm_set_আকার(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	যদি (term != in_terminal_loop)
	    may_move_terminal_to_বাফার(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    জন্য (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	যদি (pos.row < term->tl_rows)
	{
	    পূর্ণসংখ্যা max_col = MIN(wp->w_width, term->tl_cols);

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
    term_T	*term = wp->w_বাফার->b_term;

    যদি (term == ফাঁকা || term->tl_vterm == ফাঁকা || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	ফেরত;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    পূর্ণসংখ্যা
term_is_finished(buf_T *buf)
{
    ফেরত buf->b_term != ফাঁকা && buf->b_term->tl_vterm == ফাঁকা;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    পূর্ণসংখ্যা
term_show_বাফার(buf_T *buf)
{
    term_T *term = buf->b_term;

    ফেরত term != ফাঁকা && (term->tl_vterm == ফাঁকা || term->tl_normal_mode);
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
	ফেরত;

    free_scrollback(term);
    redraw_buf_later(term->tl_বাফার, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    set_string_option_direct((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    পূর্ণসংখ্যা
term_get_attr(win_T *wp, linenr_T lnum, পূর্ণসংখ্যা col)
{
    buf_T	*buf = wp->w_বাফার;
    term_T	*term = buf->b_term;
    sb_সারি_T	*সারি;
    cellattr_T	*cellattr;
    পূর্ণসংখ্যা         sb_সারি = -1;
    পূর্ণসংখ্যা         sb_col = col;

    যদি (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_সারি, &sb_col);

    যদি (sb_সারি < 0)
	cellattr = &term->tl_default_color;
    অন্যথা
    {
	সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য + sb_সারি;
	যদি (sb_col < 0 || sb_col >= সারি->sb_cols)
	    cellattr = &সারি->sb_fill_attr;
	অন্যথা
	    cellattr = সারি->sb_cells + sb_col;
    }
    ফেরত cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    স্থির শূন্য
cterm_color2vterm(পূর্ণসংখ্যা nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->সূচক);
    যদি (rgb->সূচক == 0)
	rgb->type = VTERM_COLOR_RGB;
    অন্যথা
    {
	rgb->type = VTERM_COLOR_INDEXED;
	--rgb->সূচক;
    }
}

/*
 * Initialize vterm color from the synID.
 * Returns TRUE if color is set to "fg" and "bg".
 * Otherwise returns FALSE.
 */
    স্থির পূর্ণসংখ্যা
get_vterm_color_from_synid(পূর্ণসংখ্যা id, VTermColor *fg, VTermColor *bg)
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

	ফেরত TRUE;
    }
    অন্যথা
#endif
    যদি (t_colors >= 16)
    {
	পূর্ণসংখ্যা cterm_fg = -1;
	পূর্ণসংখ্যা cterm_bg = -1;

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

	ফেরত TRUE;
    }

    ফেরত FALSE;
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
    পূর্ণসংখ্যা id = wp->w_hlfwin_id;

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
    win_T	 *wp = ফাঁকা;
    পূর্ণসংখ্যা		 did_curwin = FALSE;

    যখন (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    শূন্য
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    পূর্ণসংখ্যা		    fgval, bgval;
    পূর্ণসংখ্যা		    id;

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
    id = term_get_highlight_id(term, ফাঁকা);

    যদি (!get_vterm_color_from_synid(id, fg, bg))
    {
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	পূর্ণসংখ্যা tmp;
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
    স্থির পূর্ণসংখ্যা
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
	ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    স্থির শূন্য
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    পূর্ণসংখ্যা		সূচক = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    জন্য (; সূচক < 16; সূচক++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (unsigned)(rgb[সূচক] >> 16);
	color.green = (unsigned)(rgb[সূচক] >> 8) & 255;
	color.blue = (unsigned)rgb[সূচক] & 255;
	color.সূচক = 0;
	vterm_state_set_palette_color(state, সূচক, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    স্থির পূর্ণসংখ্যা
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    পূর্ণসংখ্যা		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    জন্য (li = list->lv_first; li != ফাঁকা && n < 16; li = li->li_পরবর্তী, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	যদি (color_name == ফাঁকা)
	    ফেরত FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	যদি (guicolor == INVALCOLOR)
	    ফেরত FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    যদি (n != 16 || li != ফাঁকা)
	ফেরত FAIL;

    set_vterm_palette(vterm, rgb);

    ফেরত OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    স্থির শূন্য
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", ফাঁকা, TRUE);

    যদি (var == ফাঁকা)
	ফেরত;

    যদি (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ফাঁকা
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
    listitem_T	*opt_item = item->li_পরবর্তী;
    পূর্ণসংখ্যা		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ফাঁকা;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	যদি (wp->w_বাফার->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    ফেরত;
	}
    }

    CLEAR_FIELD(ea);

    যদি (opt_item != ফাঁকা && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != ফাঁকা)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	যদি (p == ফাঁকা)
	    p = dict_get_string(dict, "fileformat", FALSE);
	যদি (p != ফাঁকা)
	{
	    যদি (check_ff_মান(p) == FAIL)
		ch_log(ফাঁকা, "Invalid ff argument to drop: %s", p);
	    অন্যথা
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	যদি (p == ফাঁকা)
	    p = dict_get_string(dict, "encoding", FALSE);
	যদি (p != ফাঁকা)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    যদি (ea.cmd != ফাঁকা)
	    {
		sprintf((অক্ষর *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	যদি (p != ফাঁকা)
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
    যদি (ea.cmd == ফাঁকা)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    স্থির পূর্ণসংখ্যা
is_permitted_term_api(char_u *func, char_u *pat)
{
    ফেরত pat != ফাঁকা && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
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

    যদি (item->li_পরবর্তী == ফাঁকা)
    {
	ch_log(channel, "Missing function arguments for call");
	ফেরত;
    }
    func = tv_get_string(&item->li_tv);

    যদি (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	ফেরত;
    }

    argvars[0].v_type = VAR_সংখ্যা;
    argvars[0].vval.v_সংখ্যা = term->tl_বাফার->b_fnum;
    argvars[1] = item->li_পরবর্তী->li_tv;
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
url_decode(ধ্রুবক অক্ষর *src, ধ্রুবক size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    যখন (i < len)
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
    ফেরত j;
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
    পূর্ণসংখ্যা       offset = 7;  // len of "file://" is 7
    অক্ষর      *pos = (অক্ষর *)gap->ga_তথ্য + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    যখন (offset < (পূর্ণসংখ্যা)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    যদি (offset >= (পূর্ণসংখ্যা)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_তথ্য);
	ফেরত;
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
    স্থির পূর্ণসংখ্যা
parse_osc(পূর্ণসংখ্যা command, VTermStringFragment frag, শূন্য *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == ফাঁকা ? ফাঁকা
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    যদি (command != 51 && (command != 7 || !p_asd))
	ফেরত 0;

    // Concatenate what was received until the final piece is found.
    যদি (ga_grow(gap, (পূর্ণসংখ্যা)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	ফেরত 1;
    }
    mch_memmove((অক্ষর *)gap->ga_তথ্য + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (পূর্ণসংখ্যা)frag.len;
    যদি (!frag.final)
	ফেরত 1;

    ((অক্ষর *)gap->ga_তথ্য)[gap->ga_len] = 0;

    যদি (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	ফেরত 1;
    }

    reader.js_buf = gap->ga_তথ্য;
    reader.js_fill = ফাঁকা;
    reader.js_used = 0;
    যদি (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ফাঁকা)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	যদি (item == ফাঁকা)
	    ch_log(channel, "Missing command");
	অন্যথা
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_বাফার->b_locked;

	    item = item->li_পরবর্তী;
	    যদি (item == ফাঁকা)
		ch_log(channel, "Missing argument for %s", cmd);
	    অন্যথা যদি (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    অন্যথা যদি (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    অন্যথা
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_বাফার->b_locked;
	}
    }
    অন্যথা
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    ফেরত 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    স্থির পূর্ণসংখ্যা
parse_csi(
	ধ্রুবক অক্ষর  *leader UNUSED,
	ধ্রুবক long  args[],
	পূর্ণসংখ্যা	    argcount,
	ধ্রুবক অক্ষর  *intermed UNUSED,
	অক্ষর	    command,
	শূন্য	    *user)
{
    term_T	*term = (term_T *)user;
    অক্ষর	buf[100];
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		x = 0;
    পূর্ণসংখ্যা		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    যদি (command != 't' || argcount != 1 || args[0] != 13)
	ফেরত 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#যদি defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (শূন্য)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_বাফার == term->tl_বাফার)
	    বিরতি;
    যদি (wp != ফাঁকা)
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
						     (char_u *)buf, len, ফাঁকা);
    ফেরত 1;
}

স্থির VTermStateFallbacks state_fallbacks = {
    ফাঁকা,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ফাঁকা,		// dcs
    ফাঁকা,		// apc
    ফাঁকা,		// pm
    ফাঁকা		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    স্থির শূন্য *
vterm_malloc(size_t আকার_2, শূন্য *তথ্য UNUSED)
{
    // make sure that the length is not zero
    ফেরত alloc_clear(আকার_2 == 0 ? 1L : আকার_2);
}

    স্থির শূন্য
vterm_memfree(শূন্য *ptr, শূন্য *তথ্য UNUSED)
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
    স্থির পূর্ণসংখ্যা
create_vterm(term_T *term, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_মান	    মান;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, ফাঁকা);
    term->tl_vterm = vterm;
    যদি (vterm == ফাঁকা)
	ফেরত FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    যদি (state == ফাঁকা || screen == ফাঁকা)
    {
	vterm_free(vterm);
	ফেরত FAIL;
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

    ফেরত OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    স্থির শূন্য
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    পূর্ণসংখ্যা		সূচক;

    জন্য (সূচক = 0; সূচক < 16; সূচক++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_INDEXED;
	ansi_color2rgb(সূচক, &color.red, &color.green, &color.blue,
		&color.সূচক);
	// The first valid index starts at 1.
	color.সূচক -= 1;

	vterm_state_set_palette_color(state, সূচক, &color);
    }
}

    স্থির শূন্য
term_update_palette(term_T *term)
{
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette()
	    && (term->tl_palette != ফাঁকা
		|| find_var((char_u *)"g:terminal_ansi_colors", ফাঁকা, TRUE)
								      != ফাঁকা))
    {
	যদি (term->tl_palette != ফাঁকা)
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
	যদি (term->tl_vterm == ফাঁকা)
	    চালু;
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
	যদি (term->tl_vterm == ফাঁকা)
	    চালু;
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
    যদি (term->tl_status_text != ফাঁকা)
	ফেরত term->tl_status_text;

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
    অন্যথা যদি (term->tl_title != ফাঁকা)
	txt = term->tl_title;
    অন্যথা যদি (term_none_open(term))
	txt = (char_u *)_("active");
    অন্যথা যদি (term_job_running(term))
	txt = (char_u *)_("running");
    অন্যথা
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_বাফার);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    যদি (term->tl_status_text != ফাঁকা)
	vim_snprintf((অক্ষর *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    ফেরত term->tl_status_text;
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
    পূর্ণসংখ্যা
set_ref_in_term(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    জন্য (term = first_term; !abort && term != ফাঁকা; term = term->tl_পরবর্তী)
	যদি (term->tl_job != ফাঁকা)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
	}
    ফেরত abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    স্থির buf_T *
term_get_buf(typval_T *argvars, অক্ষর *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    যদি (buf == ফাঁকা || buf->b_term == ফাঁকা)
    {
	(শূন্য)tv_get_সংখ্যা(&argvars[0]);    // issue errmsg if type error
	ch_log(ফাঁকা, "%s: invalid buffer argument", where);
	ফেরত ফাঁকা;
    }
    ফেরত buf;
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
    পূর্ণসংখ্যা সূচক;

    যদি (VTERM_COLOR_IS_INDEXED(color))
	সূচক = color->সূচক + 1;
    অন্যথা যদি (color->type == 0)
	// use RGB values
	সূচক = 255;
    অন্যথা
	// default color
	সূচক = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (পূর্ণসংখ্যা)color->red, (পূর্ণসংখ্যা)color->green, (পূর্ণসংখ্যা)color->blue,
	    সূচক);
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
    পূর্ণসংখ্যা		max_height = 0;
    পূর্ণসংখ্যা		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell পূর্ব_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    যদি (term->tl_vterm == ফাঁকা)
    {
	emsg(_(e_job_already_finished));
	ফেরত;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	যদি (check_for_dict_arg(argvars, 2) == FAIL)
	    ফেরত;
	d = argvars[2].vval.v_dict;
	যদি (d != ফাঁকা)
	{
	    max_height = dict_get_সংখ্যা(d, "rows");
	    max_width = dict_get_সংখ্যা(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    যদি (fname == ফাঁকা)
	ফেরত;
    যদি (mch_stat((অক্ষর *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	ফেরত;
    }

    যদি (*fname == NUL || (fd = mch_fopen((অক্ষর *)fname, WRITEBIN)) == ফাঁকা)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	ফেরত;
    }

    clear_cell(&পূর্ব_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    জন্য (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	পূর্ণসংখ্যা		repeat = 0;

	জন্য (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    পূর্ণসংখ্যা		    same_attr;
	    পূর্ণসংখ্যা		    same_chars = TRUE;
	    পূর্ণসংখ্যা		    i;
	    পূর্ণসংখ্যা		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    যদি (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    জন্য (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		পূর্ণসংখ্যা c = cell.chars[i];
		পূর্ণসংখ্যা pc = পূর্ব_cell.chars[i];
		পূর্ণসংখ্যা should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		যদি (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		যদি (c != pc)
		    same_chars = FALSE;
		যদি (should_break)
		    বিরতি;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&পূর্ব_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &পূর্ব_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &পূর্ব_cell.bg);
	    যদি (same_chars && cell.width == পূর্ব_cell.width && same_attr
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
		    পূর্ণসংখ্যা		len;

		    জন্য (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		যদি (cell.width != পূর্ব_cell.width || !same_attr)
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
			যদি (vterm_color_is_equal(&cell.fg, &পূর্ব_cell.fg))
			    fputs("&", fd);
			অন্যথা
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			যদি (vterm_color_is_equal(&cell.bg, &পূর্ব_cell.bg))
			    fputs("&", fd);
			অন্যথা
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		পূর্ব_cell = cell;
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
	ফেরত;

    *(((cellattr_T *)gap->ga_তথ্য) + gap->ga_len) = *cell;
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
    স্থির পূর্ণসংখ্যা
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    পূর্ণসংখ্যা		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    পূর্ব_char = {ফাঁকা, 0};
    পূর্ণসংখ্যা		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    পূর্ণসংখ্যা		    max_cells = 0;
    পূর্ণসংখ্যা		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, আকার(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    জন্য (;;)
    {
	যদি (c == EOF)
	    বিরতি;
	যদি (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	অন্যথা যদি (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    যদি (ga_text.ga_তথ্য == ফাঁকা)
		dump_is_corrupt(&ga_text);
	    যদি (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_সারি_T   *সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য
						  + term->tl_scrollback.ga_len;

		যদি (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		সারি->sb_cols = ga_cell.ga_len;
		সারি->sb_bytes = ga_text.ga_len;
		সারি->sb_cells = ga_cell.ga_তথ্য;
		সারি->sb_fill_attr = term->tl_default_color;
		সারি->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_সারি_গুনতি, ga_text.ga_তথ্য,
							ga_text.ga_len, FALSE);
	    }
	    অন্যথা
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	অন্যথা যদি (c == '|' || c == '>')
	{
	    পূর্ণসংখ্যা পূর্ব_len = ga_text.ga_len;

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
	    জন্য (;;)
	    {
		c = fgetc(fd);
		যদি (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    বিরতি;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(পূর্ব_char);
	    যদি (ga_text.ga_তথ্য != ফাঁকা)
	    {
		পূর্ব_char.দৈর্ঘ্য = (size_t)(ga_text.ga_len - পূর্ব_len);
		পূর্ব_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_তথ্য) + পূর্ব_len,
		    পূর্ব_char.দৈর্ঘ্য);
		যদি (পূর্ব_char.string == ফাঁকা)
		    পূর্ব_char.দৈর্ঘ্য = 0;
	    }

	    যদি (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    অন্যথা যদি (c == '+' || c == '*')
	    {
		পূর্ণসংখ্যা is_bg;

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
		    যখন (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    জন্য (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			যদি (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			অন্যথা যদি (c == '#')
			{
			    পূর্ণসংখ্যা red, green, blue, সূচক = 0, type;

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
			    যখন (SAFE_isdigit(c))
			    {
				সূচক = সূচক * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    যদি (সূচক == 0 || সূচক == 255)
			    {
				type = VTERM_COLOR_RGB;
				যদি (সূচক == 0)
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
				সূচক -= 1;
			    }
			    যদি (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.সূচক = সূচক;
			    }
			    অন্যথা
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.সূচক = সূচক;
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
	    যদি (পূর্ব_char.string == ফাঁকা)
		dump_is_corrupt(&ga_text);
	    অন্যথা
	    {
		পূর্ণসংখ্যা গুনতি = 0;

		// repeat previous character, get the count
		জন্য (;;)
		{
		    c = fgetc(fd);
		    যদি (!SAFE_isdigit(c))
			বিরতি;
		    গুনতি = গুনতি * 10 + (c - '0');
		}

		যখন (গুনতি-- > 0)
		{
		    ga_concat_len(&ga_text, পূর্ব_char.string, পূর্ব_char.দৈর্ঘ্য);
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
	ml_append(curbuf->b_ml.ml_সারি_গুনতি, ga_text.ga_তথ্য,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(পূর্ব_char.string);

    ফেরত max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    স্থির char_u *
get_separator(পূর্ণসংখ্যা text_width, char_u *fname)
{
    পূর্ণসংখ্যা	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    পূর্ণসংখ্যা	    fname_আকার;
    char_u  *p = fname;
    পূর্ণসংখ্যা	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    যদি (textline == ফাঁকা)
	ফেরত ফাঁকা;

    fname_আকার = vim_strsize(fname);
    যদি (fname_আকার < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_আকার + 8);
    }
    অন্যথা যদি (fname_আকার > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_আকার = vim_strsize(p);
    }
    // skip characters until the name fits
    যখন (fname_আকার > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_আকার = vim_strsize(p);
    }

    জন্য (i = 0; i < (width - fname_আকার) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    জন্য (i = 1; i < (width - fname_আকার) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    ফেরত textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    স্থির শূন্য
term_load_dump(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ফাঁকা;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ফাঁকা;
    char_u	*fname_tofree = ফাঁকা;
    FILE	*fd1;
    FILE	*fd2 = ফাঁকা;
    char_u	*textline = ফাঁকা;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    যদি (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    যদি (fname1 == ফাঁকা || (do_diff && fname2 == ফাঁকা))
    {
	emsg(_(e_invalid_argument));
	ফেরত;
    }
    fd1 = mch_fopen((অক্ষর *)fname1, READBIN);
    যদি (fd1 == ফাঁকা)
    {
	semsg(_(e_cant_read_file_str), fname1);
	ফেরত;
    }
    যদি (do_diff)
    {
	fd2 = mch_fopen((অক্ষর *)fname2, READBIN);
	যদি (fd2 == ফাঁকা)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    ফেরত;
	}
    }

    init_job_options(&opt);
    যদি (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	যাও theend;

    যদি (opt.jo_term_name == ফাঁকা)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	যদি (fname_tofree != ফাঁকা)
	{
	    vim_snprintf((অক্ষর *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    যদি (opt.jo_bufnr_buf != ফাঁকা)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	যদি (wp == ফাঁকা)
	    semsg(_(e_invalid_argument_str), "bufnr");
	অন্যথা
	{
	    buf = curbuf;
	    যখন (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    অন্যথা
	// Create a new terminal window.
	buf = term_start(&argvars[0], ফাঁকা, &opt, TERM_START_NOJOB);

    যদি (buf != ফাঁকা && buf->b_term != ফাঁকা)
    {
	পূর্ণসংখ্যা		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	পূর্ণসংখ্যা		width;
	পূর্ণসংখ্যা		width2;
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
	    যাও theend;

	term->tl_শীর্ষ_diff_rows = curbuf->b_ml.ml_সারি_গুনতি;

	textline = get_separator(width, fname1);
	যদি (textline == ফাঁকা)
	    যাও theend;
	যদি (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_সারি_গুনতি, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	যদি (textline == ফাঁকা)
	    যাও theend;
	যদি (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_সারি_গুনতি, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_সারি_গুনতি;
	width2 = read_dump_file(fd2, &cursor_pos2);
	যদি (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    যদি (textline == ফাঁকা)
		যাও theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_সারি_গুনতি - bot_lnum;

	জন্য (lnum = 1; lnum <= term->tl_শীর্ষ_diff_rows; ++lnum)
	{
	    যদি (lnum + bot_lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    {
		// bottom part has fewer rows, fill with "-"
		জন্য (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    অন্যথা
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		পূর্ণসংখ্যা	col;
		sb_সারি_T   *sb_সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য;
		cellattr_T *cellattr1 = (sb_সারি + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_সারি + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		যদি (line1 == ফাঁকা)
		    বিরতি;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		জন্য (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    পূর্ণসংখ্যা len1 = utfc_ptr2len(p1);
		    পূর্ণসংখ্যা len2 = utfc_ptr2len(p2);

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
		    অন্যথা যদি (cellattr1 != ফাঁকা && cellattr2 != ফাঁকা)
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

		যখন (col < width)
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

	যখন (lnum + bot_lnum <= curbuf->b_ml.ml_সারি_গুনতি)
	{
	    // bottom part has more rows, fill with "+"
	    জন্য (i = 0; i < width; ++i)
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
    যদি (fd2 != ফাঁকা)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    পূর্ণসংখ্যা
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

    যদি (term == ফাঁকা
	    || !term_is_finished(curbuf)
	    || term->tl_শীর্ষ_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	ফেরত FAIL;

    সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি;
    শীর্ষ_rows = term->tl_শীর্ষ_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = সারি_গুনতি - bot_rows;
    sb_সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য;

    // move lines from top to above the bottom part
    জন্য (lnum = 1; lnum <= শীর্ষ_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	যদি (p == ফাঁকা)
	    ফেরত OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    জন্য (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	যদি (p == ফাঁকা)
	    ফেরত OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    যদি (p == ফাঁকা)
	ফেরত OK;
    ml_append(সারি_গুনতি - শীর্ষ_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(সারি_গুনতি - শীর্ষ_rows));
    যদি (p == ফাঁকা)
	ফেরত OK;
    ml_delete(সারি_গুনতি - শীর্ষ_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    যদি (শীর্ষ_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	জন্য (lnum = 0; lnum < শীর্ষ_rows; ++lnum)
	{
	    sb_সারি_T	অস্থায়ী;

	    অস্থায়ী = *(sb_সারি + lnum);
	    *(sb_সারি + lnum) = *(sb_সারি + bot_start + lnum);
	    *(sb_সারি + bot_start + lnum) = অস্থায়ী;
	}
    }
    অন্যথা
    {
	size_t		আকার_2 = আকার(sb_সারি_T) * term->tl_scrollback.ga_len;
	sb_সারি_T	*অস্থায়ী = alloc(আকার_2);

	// need to copy cell properties into temp memory
	যদি (অস্থায়ী != ফাঁকা)
	{
	    mch_memmove(অস্থায়ী, term->tl_scrollback.ga_তথ্য, আকার_2);
	    mch_memmove(term->tl_scrollback.ga_তথ্য,
		    অস্থায়ী + bot_start,
		    আকার(sb_সারি_T) * bot_rows);
	    mch_memmove((sb_সারি_T *)term->tl_scrollback.ga_তথ্য + bot_rows,
		    অস্থায়ী + শীর্ষ_rows,
		    আকার(sb_সারি_T) * (সারি_গুনতি - শীর্ষ_rows - bot_rows));
	    mch_memmove((sb_সারি_T *)term->tl_scrollback.ga_তথ্য
						       + সারি_গুনতি - শীর্ষ_rows,
		    অস্থায়ী,
		    আকার(sb_সারি_T) * শীর্ষ_rows);
	    vim_free(অস্থায়ী);
	}
    }

    term->tl_শীর্ষ_diff_rows = bot_rows;
    term->tl_bot_diff_rows = শীর্ষ_rows;

    update_screen(UPD_NOT_VALID);
    ফেরত OK;
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
	ফেরত;

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
	ফেরত;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    শূন্য
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    যদি (buf == ফাঁকা)
	ফেরত;
    rettv->vval.v_সংখ্যা = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    শূন্য
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা	    attr;
    size_t  i;
    char_u  *name;

    স্থির কাঠামো {
	অক্ষর	    *name;
	পূর্ণসংখ্যা	    attr;
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
	ফেরত;

    attr = tv_get_সংখ্যা(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    যদি (name == ফাঁকা)
	ফেরত;

    যদি (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    জন্য (i = 0; i < ARRAY_দৈর্ঘ্য(attrs); ++i)
	যদি (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_সংখ্যা = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    বিরতি;
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
	ফেরত;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getcursor()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_সংখ্যা(l, term->tl_cursor_pos.row + 1);
    list_append_সংখ্যা(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    যদি (d == ফাঁকা)
	ফেরত;

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

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getjob()");
    যদি (buf == ফাঁকা)
    {
	যদি (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = ফাঁকা;
	}
	অন্যথা
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_সংখ্যা = VVAL_NULL;
	}
	ফেরত;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    যদি (rettv->vval.v_job != ফাঁকা)
	++rettv->vval.v_job->jv_refcount;
}

    স্থির পূর্ণসংখ্যা
get_row_সংখ্যা(typval_T *tv, term_T *term)
{
    যদি (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ফাঁকা
	    && STRCMP(tv->vval.v_string, ".") == 0)
	ফেরত term->tl_cursor_pos.row;
    ফেরত (পূর্ণসংখ্যা)tv_get_সংখ্যা(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    শূন্য
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    পূর্ণসংখ্যা		    row;

    rettv->v_type = VAR_STRING;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_getline()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    row = get_row_সংখ্যা(&argvars[1], term);

    যদি (term->tl_vterm == ফাঁকা)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	পূর্ণসংখ্যা	  sb_row = term->tl_scrollback_scrolled + row;
	sb_সারি_T *সারি;

	যদি (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    ফেরত;
	সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ফাঁকা, &offset);

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
	পূর্ণসংখ্যা		len;
	char_u		*p;

	যদি (row < 0 || row >= term->tl_rows)
	    ফেরত;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	যদি (p == ফাঁকা)
	    ফেরত;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (অক্ষর *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    শূন্য
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getscrolled()");
    যদি (buf == ফাঁকা)
	ফেরত;
    rettv->vval.v_সংখ্যা = buf->b_term->tl_বাফার_scrolled;
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
	ফেরত;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getsize()");
    যদি (buf == ফাঁকা)
	ফেরত;

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
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 2) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_setsize()");
    যদি (buf == ফাঁকা)
    {
	emsg(_(e_not_terminal_বাফার));
	ফেরত;
    }
    যদি (buf->b_term->tl_vterm == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    rows = tv_get_সংখ্যা(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_সংখ্যা(&argvars[2]);
    cols = cols <= 0 ? term->tl_cols : cols;
    vterm_set_আকার(term->tl_vterm, rows, cols);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_আকার(term->tl_vterm, &term->tl_rows, &term->tl_cols);
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

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getstatus()");
    যদি (buf == ফাঁকা)
	ফেরত;
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

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_gettitle()");
    যদি (buf == ফাঁকা)
	ফেরত;

    যদি (buf->b_term->tl_title != ফাঁকা)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    শূন্য
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ফাঁকা;
    পূর্ণসংখ্যা		অঙ্ক = 0;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    যদি (buf == ফাঁকা)
	ফেরত;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	অঙ্ক = tv_get_bool(&argvars[1]);

    নির্বাচন (অঙ্ক)
    {
	ক্ষেত্র 0:
	    যদি (buf->b_term->tl_job != ফাঁকা)
		p = buf->b_term->tl_job->jv_tty_out;
	    বিরতি;
	ক্ষেত্র 1:
	    যদি (buf->b_term->tl_job != ফাঁকা)
		p = buf->b_term->tl_job->jv_tty_in;
	    বিরতি;
	পূর্বনির্ধারিত:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    ফেরত;
    }
    যদি (p != ফাঁকা)
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

    যদি (rettv_list_alloc(rettv) == FAIL || first_term == ফাঁকা)
	ফেরত;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	যদি (tp->tl_বাফার != ফাঁকা)
	    যদি (list_append_সংখ্যা(l,
				   (varnumber_T)tp->tl_বাফার->b_fnum) == FAIL)
		ফেরত;
}

/*
 * "term_scrape(buf, row)" function
 */
    শূন্য
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ফাঁকা;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_সারি_T	    *সারি;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_scrape()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_সংখ্যা(&argvars[1], term);

    যদি (term->tl_vterm != ফাঁকা)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	যদি (screen == ফাঁকা)  // can't really happen
	    ফেরত;
	p = ফাঁকা;
	সারি = ফাঁকা;
    }
    অন্যথা
    {
	পূর্ণসংখ্যা	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ফাঁকা, &offset);

	যদি (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_সারি_গুনতি)
	    ফেরত;

	সারি = (sb_সারি_T *)term->tl_scrollback.ga_তথ্য + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	যদি (STRLEN(p) < offset + সারি->sb_bytes)
	    ফেরত;

	p += offset;
    }

    জন্য (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	পূর্ণসংখ্যা		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	পূর্ণসংখ্যা		i;

	যদি (screen == ফাঁকা)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    যদি (pos.col >= সারি->sb_cols)
		বিরতি;
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
		বিরতি;

	    mbslen = 0;
	    জন্য (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		যদি (cell.chars[i] == 0)
		    বিরতি;
		mbslen += (*utf_char2bytes)((পূর্ণসংখ্যা)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	যদি (dcell == ফাঁকা)
	    বিরতি;
	যদি (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    বিরতি;
	}

	dict_add_string_len(dcell, "chars", mbs, (পূর্ণসংখ্যা)mbslen);

	rgblen = vim_snprintf_safelen((অক্ষর *)rgb, আকার(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (পূর্ণসংখ্যা)rgblen);
	rgblen = vim_snprintf_safelen((অক্ষর *)rgb, আকার(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (পূর্ণসংখ্যা)rgblen);

	dict_add_সংখ্যা(dcell, "attr",
				      cell2attr(term, ফাঁকা, &attrs, &fg, &bg));
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
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_sendkeys()");
    যদি (buf == ফাঁকা)
	ফেরত;

    বার্তা = tv_get_string_chk(&argvars[1]);
    যদি (বার্তা == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    যদি (term->tl_vterm == ফাঁকা)
	ফেরত;

    যখন (*বার্তা != NUL)
    {
	পূর্ণসংখ্যা c;

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
    পূর্ণসংখ্যা		সূচক;
    list_T	*list;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = term_get_buf(argvars, "term_getansicolors()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    যদি (term->tl_vterm == ফাঁকা)
	ফেরত;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    জন্য (সূচক = 0; সূচক < 16; সূচক++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, সূচক, &color);
	hexbuflen = vim_snprintf_safelen((অক্ষর *)hexbuf, আকার(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	যদি (list_append_string(list, hexbuf, (পূর্ণসংখ্যা)hexbuflen) == FAIL)
	    ফেরত;
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
    পূর্ণসংখ্যা		n = 0;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_setansicolors()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    যদি (term->tl_vterm == ফাঁকা)
	ফেরত;

    যদি (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_মান_for_argument_str), "\"colors\"");
	ফেরত;
    }

    যদি (term->tl_palette == ফাঁকা)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    যদি (term->tl_palette == ফাঁকা)
	ফেরত;

    FOR_ALL_LIST_বস্তুসমূহ(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	যদি (color_name == ফাঁকা)
	    ফেরত;

	guicolor = GUI_GET_COLOR(color_name);
	যদি (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    ফেরত;
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
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_setapi()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    যদি (api != ফাঁকা)
	term->tl_api = vim_strsave(api);
    অন্যথা
	term->tl_api = ফাঁকা;
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
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_setrestore()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    যদি (cmd != ফাঁকা)
	term->tl_command = vim_strsave(cmd);
    অন্যথা
	term->tl_command = ফাঁকা;
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
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_setkill()");
    যদি (buf == ফাঁকা)
	ফেরত;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    যদি (how != ফাঁকা)
	term->tl_kill = vim_strsave(how);
    অন্যথা
	term->tl_kill = ফাঁকা;
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
	ফেরত;

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
	ফেরত;

    buf = term_start(&argvars[0], ফাঁকা, &opt, 0);

    যদি (buf != ফাঁকা && buf->b_term != ফাঁকা)
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
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    buf = term_get_buf(argvars, "term_wait()");
    যদি (buf == ফাঁকা)
	ফেরত;
    যদি (buf->b_term->tl_job == ফাঁকা)
    {
	ch_log(ফাঁকা, "term_wait(): no job to wait for");
	ফেরত;
    }
    যদি (buf->b_term->tl_job->jv_channel == ফাঁকা)
	// channel is closed, nothing to do
	ফেরত;

    // Get the job status, this will detect a job that finished.
    যদি (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ফাঁকা, "term_wait(): waiting for channel to close");
	যখন (buf->b_term != ফাঁকা && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    যদি (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		বিরতি;
	    যদি (buf->b_term == ফাঁকা || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		বিরতি;
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
	    যদি (term->tl_eof_chars != ফাঁকা)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(পূর্ণসংখ্যা)STRLEN(term->tl_eof_chars), ফাঁকা);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, ফাঁকা);
	    }
#ifdef MSWIN
	    অন্যথা
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, ফাঁকা);
#endif
	}
}

#যদি defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    ফেরত term != ফাঁকা ? term->tl_job : ফাঁকা;
}
#endif

#যদি defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, আকার_T, PVOID, PSIZE_T);
শূন্য (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    স্থির পূর্ণসংখ্যা
dyn_conpty_init(পূর্ণসংখ্যা verbose)
{
    স্থির HMODULE	hKerneldll = ফাঁকা;
    পূর্ণসংখ্যা			i;
    স্থির কাঠামো
    {
	অক্ষর	*name;
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
	{ফাঁকা, ফাঁকা}
    };

    যদি (!has_conpty_working())
    {
	যদি (verbose)
	    emsg(_(e_conpty_is_not_available));
	ফেরত FAIL;
    }

    // No need to initialize twice.
    যদি (hKerneldll)
	ফেরত OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    জন্য (i = 0; conpty_entry[i].name != ফাঁকা
					&& conpty_entry[i].ptr != ফাঁকা; ++i)
    {
	যদি ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == ফাঁকা)
	{
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = ফাঁকা;
	    ফেরত FAIL;
	}
    }

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	অক্ষর	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ফাঁকা;
    WCHAR	    *cmd_wchar_copy = ফাঁকা;
    WCHAR	    *cwd_wchar = ফাঁকা;
    WCHAR	    *env_wchar = ফাঁকা;
    channel_T	    *channel = ফাঁকা;
    job_T	    *job = ফাঁকা;
    HANDLE	    jo = ফাঁকা;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ফাঁকা;
    HRESULT	    hr;
    COORD	    consize;
    আকার_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ফাঁকা;
    HANDLE	    o_theirs = ফাঁকা;
    HANDLE	    i_ours = ফাঁকা;
    HANDLE	    o_ours = ফাঁকা;
    অক্ষর	    *errmsg = ফাঁকা;

    ga_init2(&ga_cmd, আকার(অক্ষর*), 20);
    ga_init2(&ga_env, আকার(অক্ষর*), 20);

    যদি (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    অন্যথা যদি (argvar->v_type == VAR_LIST)
    {
	যদি (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    যাও failed;
	cmd = ga_cmd.ga_তথ্য;
    }
    যদি (cmd == ফাঁকা || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	যাও failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ফাঁকা);

    যদি (cmd_wchar != ফাঁকা)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    যদি (cmd_wchar == ফাঁকা)
	যাও failed;
    যদি (opt->jo_cwd != ফাঁকা)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ফাঁকা);
	যদি (cwd_wchar == ফাঁকা)
	    যাও failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_তথ্য;

    যদি (!CreatePipe(&i_theirs, &i_ours, ফাঁকা, 0))
	যাও failed;
    যদি (!CreatePipe(&o_ours, &o_theirs, ফাঁকা, 0))
	যাও failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    যদি (FAILED(hr))
	যাও failed;

    term->tl_siex.StartupInfo.cb = আকার(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_প্রবেশ = INVALID_HANDLE_মান;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_মান;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_মান;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ফাঁকা, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    যদি (!term->tl_siex.lpAttributeList)
	যাও failed;
    যদি (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	যাও failed;
    যদি (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    আকার(HPCON), ফাঁকা, ফাঁকা))
	যাও failed;

    channel = add_channel();
    যদি (channel == ফাঁকা)
	যাও failed;

    job = job_alloc();
    যদি (job == ফাঁকা)
	যাও failed;
    যদি (argvar->v_type == VAR_STRING)
    {
	পূর্ণসংখ্যা argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    যদি (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    যদি (!CreateProcessW(ফাঁকা, cmd_wchar_copy, ফাঁকা, ফাঁকা, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	যাও failed;
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

    jo = CreateJobObject(ফাঁকা, ফাঁকা);
    যদি (jo == ফাঁকা)
	যাও failed;

    যদি (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ফাঁকা;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	যাও failed;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette())
    {
	যদি (term->tl_palette != ফাঁকা)
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
	term->tl_out_fd = mch_fopen((অক্ষর *)fname, WRITEBIN);
	যদি (term->tl_out_fd == ফাঁকা)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ফেরত OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    যদি (channel != ফাঁকা)
	channel_clear(channel);
    যদি (job != ফাঁকা)
    {
	job->jv_channel = ফাঁকা;
	job_cleanup(job);
    }
    term->tl_job = ফাঁকা;
    যদি (jo != ফাঁকা)
	CloseHandle(jo);

    যদি (term->tl_siex.lpAttributeList != ফাঁকা)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ফাঁকা;
    যদি (o_theirs != ফাঁকা)
	CloseHandle(o_theirs);
    যদি (o_ours != ফাঁকা)
	CloseHandle(o_ours);
    যদি (i_ours != ফাঁকা)
	CloseHandle(i_ours);
    যদি (i_theirs != ফাঁকা)
	CloseHandle(i_theirs);
    যদি (term->tl_conpty != ফাঁকা)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ফাঁকা;
    // Propagate errors that occur in CreateProcess
    যদি (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    ফেরত FAIL;
}

    স্থির শূন্য
conpty_term_report_winsize(term_T *term, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    স্থির শূন্য
term_free_conpty(term_T *term)
{
    যদি (term->tl_siex.lpAttributeList != ফাঁকা)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ফাঁকা;
    যদি (term->tl_conpty != ফাঁকা)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ফাঁকা;
}

    পূর্ণসংখ্যা
use_conpty(শূন্য)
{
    ফেরত has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

শূন্য* (*winpty_config_new)(UINT64, শূন্য*);
শূন্য* (*winpty_open)(শূন্য*, শূন্য*);
শূন্য* (*winpty_spawn_config_new)(UINT64, শূন্য*, LPCWSTR, শূন্য*, শূন্য*, শূন্য*);
BOOL (*winpty_spawn)(শূন্য*, শূন্য*, HANDLE*, HANDLE*, DWORD*, শূন্য*);
শূন্য (*winpty_config_set_mouse_mode)(শূন্য*, পূর্ণসংখ্যা);
শূন্য (*winpty_config_set_initial_আকার)(শূন্য*, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
LPCWSTR (*winpty_conin_name)(শূন্য*);
LPCWSTR (*winpty_conout_name)(শূন্য*);
LPCWSTR (*winpty_conerr_name)(শূন্য*);
শূন্য (*winpty_free)(শূন্য*);
শূন্য (*winpty_config_free)(শূন্য*);
শূন্য (*winpty_spawn_config_free)(শূন্য*);
শূন্য (*winpty_error_free)(শূন্য*);
LPCWSTR (*winpty_error_বার্তা)(শূন্য*);
BOOL (*winpty_set_আকার)(শূন্য*, পূর্ণসংখ্যা, পূর্ণসংখ্যা, শূন্য*);
HANDLE (*winpty_agent_process)(শূন্য*);

# define WINPTY_DLL "winpty.dll"

স্থির HINSTANCE hWinPtyDLL = ফাঁকা;

    স্থির পূর্ণসংখ্যা
dyn_winpty_init(পূর্ণসংখ্যা verbose)
{
    পূর্ণসংখ্যা i;
    স্থির কাঠামো
    {
	অক্ষর	    *name;
	FARPROC	    *ptr;
    } winpty_entry[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_set_mouse_mode},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_set_initial_আকার},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_বার্তা},
	{"winpty_set_size", (FARPROC*)&winpty_set_আকার},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ফাঁকা, ফাঁকা}
    };

    // No need to initialize twice.
    যদি (hWinPtyDLL)
	ফেরত OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    যদি (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((অক্ষর *)p_winptydll);
    যদি (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    যদি (!hWinPtyDLL)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	ফেরত FAIL;
    }
    জন্য (i = 0; winpty_entry[i].name != ফাঁকা
					 && winpty_entry[i].ptr != ফাঁকা; ++i)
    {
	যদি ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == ফাঁকা)
	{
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = ফাঁকা;
	    ফেরত FAIL;
	}
    }

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	অক্ষর	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ফাঁকা;
    WCHAR	    *cwd_wchar = ফাঁকা;
    WCHAR	    *env_wchar = ফাঁকা;
    channel_T	    *channel = ফাঁকা;
    job_T	    *job = ফাঁকা;
    DWORD	    error;
    HANDLE	    jo = ফাঁকা;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    শূন্য	    *winpty_err = ফাঁকা;
    শূন্য	    *spawn_config = ফাঁকা;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ফাঁকা;

    ga_init2(&ga_cmd, আকার(অক্ষর*), 20);
    ga_init2(&ga_env, আকার(অক্ষর*), 20);

    যদি (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    অন্যথা যদি (argvar->v_type == VAR_LIST)
    {
	যদি (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    যাও failed;
	cmd = ga_cmd.ga_তথ্য;
    }
    যদি (cmd == ফাঁকা || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	যাও failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ফাঁকা);
    ga_clear(&ga_cmd);
    যদি (cmd_wchar == ফাঁকা)
	যাও failed;
    যদি (opt->jo_cwd != ফাঁকা)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ফাঁকা);
	যদি (cwd_wchar == ফাঁকা)
	    যাও failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_তথ্য;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    যদি (term->tl_winpty_config == ফাঁকা)
	যাও failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_আকার(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    যদি (term->tl_winpty == ফাঁকা)
	যাও failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ফাঁকা,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    যদি (spawn_config == ফাঁকা)
	যাও failed;

    channel = add_channel();
    যদি (channel == ফাঁকা)
	যাও failed;

    job = job_alloc();
    যদি (job == ফাঁকা)
	যাও failed;
    যদি (argvar->v_type == VAR_STRING)
    {
	পূর্ণসংখ্যা argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    যদি (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    যদি (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	যাও failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, ফাঁকা,
	    OPEN_EXISTING, 0, ফাঁকা),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, ফাঁকা,
	    OPEN_EXISTING, 0, ফাঁকা),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, ফাঁকা,
	    OPEN_EXISTING, 0, ফাঁকা));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(ফাঁকা, ফাঁকা);
    যদি (jo == ফাঁকা)
	যাও failed;

    যদি (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ফাঁকা;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	যাও failed;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette())
    {
	যদি (term->tl_palette != ফাঁকা)
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
	    (short_u *)winpty_conin_name(term->tl_winpty), ফাঁকা);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), ফাঁকা);
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
	term->tl_out_fd = mch_fopen((অক্ষর *)fname, WRITEBIN);
	যদি (term->tl_out_fd == ফাঁকা)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ফেরত OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    যদি (spawn_config != ফাঁকা)
	winpty_spawn_config_free(spawn_config);
    যদি (channel != ফাঁকা)
	channel_clear(channel);
    যদি (job != ফাঁকা)
    {
	job->jv_channel = ফাঁকা;
	job_cleanup(job);
    }
    term->tl_job = ফাঁকা;
    যদি (jo != ফাঁকা)
	CloseHandle(jo);
    যদি (term->tl_winpty != ফাঁকা)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ফাঁকা;
    যদি (term->tl_winpty_config != ফাঁকা)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ফাঁকা;
    যদি (winpty_err != ফাঁকা)
    {
	অক্ষর *বার্তা = (অক্ষর *)utf16_to_enc(
				(short_u *)winpty_error_বার্তা(winpty_err), ফাঁকা);

	যদি (বার্তা != ফাঁকা)
	{
	    emsg(বার্তা);
	    vim_free(বার্তা);
	}
	winpty_error_free(winpty_err);
    }
    ফেরত FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	অক্ষর	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    পূর্ণসংখ্যা		    use_winpty = FALSE;
    পূর্ণসংখ্যা		    use_conpty = FALSE;
    পূর্ণসংখ্যা		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    যদি (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	ফেরত dyn_winpty_init(TRUE);

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
	    ফেরত dyn_conpty_init(TRUE);
    }

    যদি (use_conpty)
	ফেরত conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    যদি (use_winpty)
	ফেরত winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    ফেরত dyn_winpty_init(TRUE);
}

    স্থির পূর্ণসংখ্যা
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_মান;
    HANDLE	    hPipeOut = INVALID_HANDLE_মান;
    অক্ষর	    in_name[80], out_name[80];
    channel_T	    *channel = ফাঁকা;

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ফেরত FAIL;

    vim_snprintf(in_name, আকার(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ফাঁকা);
    যদি (hPipeIn == INVALID_HANDLE_মান)
	যাও failed;

    vim_snprintf(out_name, আকার(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ফাঁকা);
    যদি (hPipeOut == INVALID_HANDLE_মান)
	যাও failed;

    ConnectNamedPipe(hPipeIn, ফাঁকা);
    ConnectNamedPipe(hPipeOut, ফাঁকা);

    term->tl_job = job_alloc();
    যদি (term->tl_job == ফাঁকা)
	যাও failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    যদি (channel == ফাঁকা)
	যাও failed;
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

    ফেরত OK;

failed:
    যদি (hPipeIn != ফাঁকা)
	CloseHandle(hPipeIn);
    যদি (hPipeOut != ফাঁকা)
	CloseHandle(hPipeOut);
    ফেরত FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    স্থির শূন্য
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    যদি (term->tl_winpty != ফাঁকা)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ফাঁকা;
    যদি (term->tl_winpty_config != ফাঁকা)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ফাঁকা;
    যদি (term->tl_vterm != ফাঁকা)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ফাঁকা;
}

/*
 * Report the size to the terminal.
 */
    স্থির শূন্য
term_report_winsize(term_T *term, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
    যদি (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    যদি (term->tl_winpty)
	winpty_set_আকার(term->tl_winpty, cols, rows, ফাঁকা);
}

    পূর্ণসংখ্যা
terminal_enabled(শূন্য)
{
    ফেরত dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
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
    স্থির পূর্ণসংখ্যা
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	অক্ষর	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = ফাঁকা;

    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ফেরত FAIL;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (term_use_palette())
    {
	যদি (term->tl_palette != ফাঁকা)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	অন্যথা
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    যদি (term->tl_job != ফাঁকা)
	++term->tl_job->jv_refcount;

    ফেরত term->tl_job != ফাঁকা
	&& term->tl_job->jv_channel != ফাঁকা
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    স্থির পূর্ণসংখ্যা
create_pty_only(term_T *term, jobopt_T *opt)
{
    যদি (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ফেরত FAIL;

    term->tl_job = job_alloc();
    যদি (term->tl_job == ফাঁকা)
	ফেরত FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    ফেরত mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    স্থির শূন্য
term_free_vterm(term_T *term)
{
    যদি (term->tl_vterm != ফাঁকা)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ফাঁকা;
}

/*
 * Report the size to the terminal.
 */
    স্থির শূন্য
term_report_winsize(term_T *term, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
    // Use an ioctl() to report the new window size to the job.
    যদি (term->tl_job == ফাঁকা || term->tl_job->jv_channel == ফাঁকা)
	ফেরত;

    পূর্ণসংখ্যা fd = -1;
    পূর্ণসংখ্যা part;

    জন্য (part = PART_OUT; part < PART_গুনতি; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	যদি (mch_isatty(fd))
	    বিরতি;
    }
    যদি (part < PART_গুনতি && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
