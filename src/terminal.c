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

#شامل "vim.h"

#اگر defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#شامل "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
تعریفنوع ساختار {
    VTermScreenCellAttrs	attrs;
    نویسه			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

تعریفنوع ساختار sb_سطر_S {
    صحیح		sb_cols;	// can differ per line
    صحیح		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_سطر_T;

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
تعریفنوع ساختار _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
ساختار terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#اگر defined(FEAT_GUI)
    صحیح		tl_system;	// when non-zero used for :!cmd output
    صحیح		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    صحیح		tl_vterm_size_changed;

    صحیح		tl_normal_mode; // TRUE: Terminal-Normal mode
    صحیح		tl_channel_closing;
    صحیح		tl_channel_closed;
    صحیح		tl_channel_recently_closed; // still need to handle tl_finish

    صحیح		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    تهی	*tl_winpty_config;
    تهی	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#اگر defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    صحیح		tl_rows;
    صحیح		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحیح		tl_dirty_row_start; // MAX_ROW if nothing dirty
    صحیح		tl_dirty_row_end;   // row below last one to update
    صحیح		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    صحیح		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    صحیح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحیح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحیح		tl_scrollback_snapshot;
    صحیح		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_بالاترین_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    صحیح		tl_cursor_visible;
    صحیح		tl_cursor_blink;
    صحیح		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    صحیح		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ایستا term_T *first_term = پوچ;

// Terminal active in terminal_loop().
ایستا term_T *in_terminal_loop = پوچ;

#ifdef MSWIN
ایستا BOOL has_winpty = FALSE;
ایستا BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define کلید_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    برای ((term) = first_term; (term) != پوچ; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ایستا صحیح term_and_job_init(term_T *term, typval_T *argvar, نویسه **argv, jobopt_T *opt, jobopt_T *orig_opt);
ایستا صحیح create_pty_only(term_T *term, jobopt_T *opt);
ایستا تهی term_report_winsize(term_T *term, صحیح rows, صحیح cols);
ایستا تهی term_free_vterm(term_T *term);
#ifdef FEAT_GUI
ایستا تهی update_system_term(term_T *term);
#endif

ایستا تهی handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ایستا صحیح term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ایستا char_u	*last_set_cursor_color = پوچ;
ایستا char_u	*desired_cursor_color = پوچ;
ایستا صحیح	last_set_cursor_shape = -1;
ایستا صحیح	desired_cursor_shape = -1;
ایستا صحیح	last_set_cursor_blink = -1;
ایستا صحیح	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ایستا صحیح
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    اگر (lhs_color != پوچ && rhs_color != پوچ)
	بازگشت STRCMP(lhs_color, rhs_color) == 0;
    بازگشت lhs_color == پوچ && rhs_color == پوچ;
}

    ایستا تهی
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    اگر (cursor_color_equal(*to_color, from_color))
	بازگشت;
    vim_free(*to_color);
    *to_color = (from_color == پوچ) ? پوچ : vim_strsave(from_color);
}

    ایستا char_u *
cursor_color_get(char_u *color)
{
    بازگشت (color == پوچ) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ایستا صحیح
parse_termwinsize(win_T *wp, صحیح *rows, صحیح *cols)
{
    صحیح	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    اگر (*wp->w_p_tws == NUL)
	بازگشت FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    اگر (p == پوچ)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((نویسه *)wp->w_p_tws);
    *cols = atoi((نویسه *)p + 1);
    اگر (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    اگر (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    بازگشت minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ایستا تهی
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    صحیح rows, cols;
    صحیح	minsize;

#ifdef FEAT_GUI
    اگر (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	بازگشت;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    اگر (minsize)
    {
	اگر (term->tl_rows < rows)
	    term->tl_rows = rows;
	اگر (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    اگر ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    وگرنه اگر (rows != 0)
	term->tl_rows = rows;
    اگر ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    وگرنه اگر (cols != 0)
	term->tl_cols = cols;

    اگر (!opt->jo_hidden)
    {
	اگر (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	اگر (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	اگر (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((نویسه *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_مقدار_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    تهی
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
    ایستا تهی
setup_job_options(jobopt_T *opt, صحیح rows, صحیح cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    اگر (!(opt->jo_set & JO_OUT_IO))
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
    اگر (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    اگر ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    اگر ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    ایستا تهی
term_flush_messages(تهی)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ایستا تهی
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    اگر (old_curbuf != پوچ)
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
	نویسه	    **argv,
	jobopt_T    *opt,
	صحیح	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = پوچ;
    صحیح		res;
    buf_T	*newbuf;
    صحیح		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    اگر (check_restricted() || check_secure())
	بازگشت پوچ;
    اگر (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_سطر_window));
	بازگشت پوچ;
    }

    اگر ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != پوچ
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != پوچ
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	بازگشت پوچ;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    اگر (term == پوچ)
	بازگشت پوچ;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, اندازه(sb_سطر_T), 300);
    ga_init2(&term->tl_scrollback_postponed, اندازه(sb_سطر_T), 300);
    ga_init2(&term->tl_osc_buf, اندازه(نویسه), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    اگر (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	اگر (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    بازگشت پوچ;
	}
	اگر (do_ecmd(0, پوچ, پوچ, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    بازگشت پوچ;
	}
    }
    وگرنه اگر (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", پوچ, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	اگر (buf == پوچ || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    بازگشت پوچ;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    وگرنه
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	اگر (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_شمار = 1;
	}
	اگر (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_شمار = 1;
	}

	صحیح cmod_split_modified = FALSE;
	اگر (vertical)
	{
	    اگر (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	اگر (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	اگر (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    بازگشت پوچ;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    اگر (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	اگر (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	اگر (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, پوچ, پوچ, FALSE, curbuf);

    اگر (opt->jo_term_name != پوچ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    وگرنه اگر (argv != پوچ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    وگرنه
    {
	صحیح	i;
	size_t	len;
	char_u	*cmd, *p;

	اگر (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    اگر (cmd == پوچ)
		cmd = (char_u *)"";
	    وگرنه اگر (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	وگرنه اگر (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == پوچ
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == پوچ)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	برای (i = 0; p != پوچ; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    اگر (i == 0)
		vim_snprintf((نویسه *)p, len, "!%s", cmd);
	    وگرنه
		vim_snprintf((نویسه *)p, len, "!%s (%d)", cmd, i);
	    اگر (buflist_findname(p) == پوچ)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		بشکن;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, پوچ, پوچ, FALSE, curbuf);

    اگر (opt->jo_term_opencmd != پوچ)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    اگر (opt->jo_eof_chars != پوچ)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, اندازه(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    اگر (flags & TERM_START_NOJOB)
	بازگشت curbuf;

#اگر defined(FEAT_SESSION)
    // Remember the command for the session file.
    اگر (opt->jo_term_norestore || argv != پوچ)
	term->tl_command = vim_strsave((char_u *)"NONE");
    وگرنه اگر (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	اگر (cmd != پوچ && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    وگرنه اگر (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != پوچ
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_اقلام(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    اگر (s == پوچ)
		بشکن;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    اگر (p == پوچ)
		بشکن;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	اگر (item == پوچ)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	وگرنه
	    ga_clear(&ga);
    }
#endif

    اگر (opt->jo_term_kill != پوچ)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    اگر (opt->jo_term_api != پوچ)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    وگرنه
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    اگر (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    اگر (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	اگر (term->tl_palette == پوچ)
	{
	    vim_free(term);
	    بازگشت پوچ;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, اندازه(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    اگر (argv == پوچ
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != پوچ
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    وگرنه
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    اگر (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	اگر (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = پیام_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	اگر (old_curbuf != پوچ)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	وگرنه اگر (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| درون_داد_busy)
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
    وگرنه
    {
	term_close_buffer(curbuf, old_curbuf);
	بازگشت پوچ;
    }

    apply_autocmds(EVENT_TERMINALOPEN, پوچ, پوچ, FALSE, newbuf);
    اگر (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, پوچ, پوچ, FALSE, newbuf);
    بازگشت newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    تهی
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    صحیح		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = پوچ;
    صحیح		scroll_save = پیام_scroll;

    پیام_scroll = FALSE;		// don't scroll here
    autowrite_all();
    پیام_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    وقتی (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	اگر (ep != پوچ)
	{
	    اگر (ep < p)
		p = ep;
	    وگرنه
		ep = پوچ;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((صحیح)(p - cmd) == اندازه(name) - 1 \
				 && STRNICMP(cmd, name, اندازه(name) - 1) == 0)
	اگر (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	وگرنه اگر (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	وگرنه اگر (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	وگرنه اگر (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	وگرنه اگر (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	وگرنه اگر (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	وگرنه اگر (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	وگرنه اگر (OPTARG_HAS("kill") && ep != پوچ)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	وگرنه اگر (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    اگر (ep != پوچ)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    وگرنه
		opt.jo_term_api = پوچ;
	}
	وگرنه اگر (OPTARG_HAS("rows") && ep != پوچ && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((نویسه *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وگرنه اگر (OPTARG_HAS("cols") && ep != پوچ && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((نویسه *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وگرنه اگر (OPTARG_HAS("eof") && ep != پوچ)
	{
	    char_u *buf = پوچ;
	    char_u *کلیدها;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    کلیدها = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, پوچ);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(کلیدها);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	وگرنه اگر ((صحیح)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != پوچ)
	{
	    صحیح tty_type = NUL;

	    p = skiptowhite(cmd);
	    اگر (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    وگرنه اگر (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    وگرنه
	    {
		semsg(_(e_invalid_مقدار_for_argument_str), "type");
		برو theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	وگرنه
	{
	    اگر (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    برو theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    اگر (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	اگر (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    اگر (eap->addr_شمار > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_بالاترین | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_بالاترین = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    اگر (opt_shell && tofree == پوچ)
    {
#ifdef UNIX
	نویسه	**argv = پوچ;
	char_u	*tofree1 = پوچ;
	char_u	*tofree2 = پوچ;

	// :term ++shell command
	اگر (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(پوچ, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	برو theend;
#وگرنه
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	اگر (newcmd == پوچ)
	    برو theend;
	tofree = newcmd;
	vim_snprintf((نویسه *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# وگرنه
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	برو theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, پوچ, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ایستا char_u *
get_terminalopt_name(expand_T *xp UNUSED, صحیح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ایستا نویسه *(p_termopt_مقادیر[]) =
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

    اگر (idx < (صحیح)ARRAY_LENGTH(p_termopt_مقادیر))
	بازگشت (char_u*)p_termopt_مقادیر[idx];
    بازگشت پوچ;
}

    ایستا char_u *
get_termkill_name(expand_T *xp UNUSED, صحیح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ایستا نویسه *(p_termkill_مقادیر[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    اگر (idx < (صحیح)ARRAY_LENGTH(p_termkill_مقادیر))
	بازگشت (char_u*)p_termkill_مقادیر[idx];
    بازگشت پوچ;
}

/*
 * Command-line expansion for :terminal [options]
 */
    صحیح
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحیح	    *رقم_Matches)
{
    اگر (xp->xp_pattern > xp->xp_سطر && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحیح) = پوچ;

	char_u *name_end = xp->xp_pattern - 1;
	اگر (name_end - xp->xp_سطر >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	اگر (cb != پوچ)
	{
	    بازگشت ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    رقم_Matches,
		    cb,
		    FALSE);
	}
	بازگشت FAIL;
    }
    بازگشت ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    رقم_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#اگر defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    صحیح
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحیح	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    اگر (terminal_bufs != پوچ && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	نویسه	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, اندازه(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	اگر (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    اگر (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		بازگشت FAIL;
	    بازگشت put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    اگر (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	بازگشت FAIL;
# ifdef MSWIN
    اگر (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	بازگشت FAIL;
# endif
    اگر (term->tl_command != پوچ)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	اگر (quoted_command == پوچ)
	    بازگشت FAIL;

	صحیح ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((نویسه *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	اگر (ret)
	    بازگشت FAIL;
    }
    اگر (put_eol(fd) != OK)
	بازگشت FAIL;

    اگر (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	بازگشت FAIL;

    اگر (terminal_bufs != پوچ && wp->w_buffer->b_nwindows > 1)
    {
	نویسه *hash_کلید = alloc(NUMBUFLEN);

	vim_snprintf(hash_کلید, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_کلید, "terminal session");
    }

    بازگشت put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحیح
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    بازگشت term != پوچ && (term->tl_command == پوچ
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    ایستا تهی
free_scrollback(term_T *term)
{
    صحیح i;

    برای (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_سطر_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    برای (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_سطر_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ایستا term_T	*terminals_to_free = پوچ;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    تهی
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    اگر (term == پوچ)
	بازگشت;

    // Unlink the terminal form the list of terminals.
    اگر (first_term == term)
	first_term = term->tl_next;
    وگرنه
	برای (tp = first_term; tp->tl_next != پوچ; tp = tp->tl_next)
	    اگر (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		بشکن;
	    }

    اگر (term->tl_job != پوچ)
    {
	اگر (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, پوچ, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = پوچ;
    اگر (in_terminal_loop == term)
	in_terminal_loop = پوچ;
}

    تهی
free_unused_terminals(تهی)
{
    وقتی (terminals_to_free != پوچ)
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
	اگر (term->tl_out_fd != پوچ)
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
    ایستا ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    صحیح		i;

    برای (i = 0; i < 3; ++i)
    {
	صحیح fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	اگر (mch_isatty(fd))
	    بازگشت parts[i];
    }
#endif
    بازگشت PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ایستا تهی
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    نویسه   buf[کلید_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, کلید_BUF_LEN);

    اگر (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (صحیح)curlen, پوچ);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ایستا تهی
term_write_job_output(term_T *term, char_u *پیام_arg, size_t len_arg)
{
    char_u	*پیام = پیام_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    اگر (len > limit)
    {
	char_u *p = پیام + len - limit;

	p -= (*mb_head_off)(پیام, p);
	len -= p - پیام;
	پیام = p;
    }

    vterm_درون_داد_write(vterm, (نویسه *)پیام, len);

    // flush vterm buffer when vterm responded to control sequence
    اگر (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ایستا تهی
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    اگر (popup_is_popup(wp))
    {
	wp->w_wrow += popup_بالاترین_extra(wp);
	wp->w_wcol += popup_چپ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    وگرنه
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ایستا تهی
update_cursor(term_T *term, صحیح redraw)
{
    اگر (term->tl_normal_mode)
	بازگشت;
#ifdef FEAT_GUI
    اگر (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    وگرنه
#endif
    اگر (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    وگرنه
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    اگر (redraw)
    {
	aco_save_T	aco;

	اگر (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	اگر (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, پوچ, پوچ, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ایستا تهی
scrollbackline_pos_in_buf(term_T *term, صحیح row, linenr_T *lnum, صحیح *start_col, size_t *start_pos)
{
    sb_سطر_T	*سطرها = (sb_سطر_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    صحیح         calc_col = 0;
    صحیح         i;

    اگر (row < 0 || row >= term->tl_scrollback.ga_len)
	بازگشت;

    اگر (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	برای (i = term->tl_scrollback_scrolled + 1; i > 0 && سطرها[i].continuation; --i)
	{
	    calc_pos += سطرها[i - 1].sb_bytes;
	    calc_col += سطرها[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	اگر (سطرها[i].continuation)
	    ++i;
    }
    وگرنه
    {
	i = 1;
	calc_lnum = 1;
    }

    برای (; i <= row; ++i)
    {
	اگر (!سطرها[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	وگرنه
	{
	    calc_pos += سطرها[i - 1].sb_bytes;
	    calc_col += سطرها[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    اگر (start_col)
	*start_col = calc_col;
    اگر (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ایستا تهی
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, صحیح col, صحیح *row, صحیح *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_سطر_T	*سطرها = (sb_سطر_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    صحیح         calc_col = col;
    linenr_T    l;

    اگر (lnum > buf->b_ml.ml_سطر_شمار)
	بازگشت;

    اگر (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	وقتی (calc_row < term->tl_scrollback.ga_len && سطرها[calc_row].continuation)
	    ++calc_row;
    }
    وگرنه
    {
	calc_row = 0;
	l = 1;
    }

    وقتی (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	اگر (!سطرها[calc_row].continuation)
	    ++l;
    }

    وقتی (calc_row + 1 < term->tl_scrollback.ga_len && سطرها[calc_row + 1].continuation
	    && calc_col >= سطرها[calc_row].sb_cols)
    {
	calc_col -= سطرها[calc_row].sb_cols;
	++calc_row;
    }

    اگر (row)
	*row = calc_row;
    اگر (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    تهی
write_to_term(buf_T *buffer, char_u *پیام, channel_T *channel)
{
    size_t	len = STRLEN(پیام);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    اگر (term->tl_out_fd != پوچ)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحیح)len);
	fwrite(پیام, len, 1, term->tl_out_fd);
	بازگشت;
    }
#endif

    اگر (term->tl_vterm == پوچ)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحیح)len);
	بازگشت;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحیح)len);
    cursor_off();
    term_write_job_output(term, پیام, len);

#ifdef FEAT_GUI
    اگر (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    وگرنه
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    اگر (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	اگر (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#اگر defined(FEAT_TABPANEL)
	    اگر (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    اگر (buffer == curbuf && curbuf->b_term != پوچ)
		update_cursor(curbuf->b_term, TRUE);
	}
	وگرنه
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ایستا صحیح
term_send_mouse(VTerm *vterm, صحیح button, صحیح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحیح		    row = mouse_row - W_WINROW(curwin);
    صحیح		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    اگر (popup_is_popup(curwin))
    {
	row -= popup_بالاترین_extra(curwin);
	col -= popup_چپ_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    اگر (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    بازگشت TRUE;
}

ایستا صحیح enter_mouse_col = -1;
ایستا صحیح enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ایستا صحیح
term_mouse_click(VTerm *vterm, صحیح کلید)
{
#اگر defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ایستا صحیح	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    اگر (state == پوچ)
	بازگشت FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    اگر (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	انتخاب (کلید)
	{
	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		اگر (ignore_drag_release)
		{
		    صحیح save_mouse_col, save_mouse_row;

		    اگر (enter_mouse_col < 0)
			بشکن;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_چپ, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	حالت K_LEFTMOUSE:
	حالت K_RIGHTMOUSE:
		اگر (کلید == K_LEFTRELEASE || کلید == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		وگرنه
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		اگر (clip_star.available)
		{
		    صحیح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(کلید),
							 &is_click, &is_drag);
		    اگر (mouse_model_popup() && button == MOUSE_چپ
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_راست;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		بشکن;

	حالت K_MIDDLEMOUSE:
		اگر (clip_star.available)
		    درج_reg('*', TRUE);
		بشکن;
	}
	enter_mouse_col = -1;
	بازگشت FALSE;
    }
#endif
    enter_mouse_col = -1;

    انتخاب (کلید)
    {
	حالت K_LEFTMOUSE:
	حالت K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); بشکن;
	حالت K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); بشکن;
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); بشکن;
	حالت K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); بشکن;
	حالت K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); بشکن;
	حالت K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); بشکن;
	حالت K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); بشکن;
	حالت K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); بشکن;
	حالت K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); بشکن;
	حالت K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); بشکن;
    }
    بازگشت TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ایستا صحیح
term_convert_کلید(term_T *term, صحیح c, صحیح modmask, نویسه *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_کلید	    کلید = VTERM_کلید_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحیح		    other = FALSE;

    انتخاب (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالت K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    اگر (term->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    وگرنه
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    بشکن;

#ifdef MSWIN
	حالت BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    اگر (term->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    بشکن;
#endif

	حالت ESC:		کلید = VTERM_کلید_ESCAPE; بشکن;
	حالت K_DEL:		کلید = VTERM_کلید_DEL; بشکن;
	حالت K_DOWN:		کلید = VTERM_کلید_DOWN; بشکن;
	حالت K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_DOWN; بشکن;
	حالت K_END:		کلید = VTERM_کلید_END; بشکن;
	حالت K_S_END:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_END; بشکن;
	حالت K_C_END:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_END; بشکن;
	حالت K_F10:		کلید = VTERM_کلید_FUNCTION(10); بشکن;
	حالت K_F11:		کلید = VTERM_کلید_FUNCTION(11); بشکن;
	حالت K_F12:		کلید = VTERM_کلید_FUNCTION(12); بشکن;
	حالت K_F1:		کلید = VTERM_کلید_FUNCTION(1); بشکن;
	حالت K_F2:		کلید = VTERM_کلید_FUNCTION(2); بشکن;
	حالت K_F3:		کلید = VTERM_کلید_FUNCTION(3); بشکن;
	حالت K_F4:		کلید = VTERM_کلید_FUNCTION(4); بشکن;
	حالت K_F5:		کلید = VTERM_کلید_FUNCTION(5); بشکن;
	حالت K_F6:		کلید = VTERM_کلید_FUNCTION(6); بشکن;
	حالت K_F7:		کلید = VTERM_کلید_FUNCTION(7); بشکن;
	حالت K_F8:		کلید = VTERM_کلید_FUNCTION(8); بشکن;
	حالت K_F9:		کلید = VTERM_کلید_FUNCTION(9); بشکن;
	حالت K_HOME:		کلید = VTERM_کلید_HOME; بشکن;
	حالت K_S_HOME:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_HOME; بشکن;
	حالت K_C_HOME:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_HOME; بشکن;
	حالت K_INS:		کلید = VTERM_کلید_INS; بشکن;
	حالت K_K0:		کلید = VTERM_کلید_KP_0; بشکن;
	حالت K_K1:		کلید = VTERM_کلید_KP_1; بشکن;
	حالت K_K2:		کلید = VTERM_کلید_KP_2; بشکن;
	حالت K_K3:		کلید = VTERM_کلید_KP_3; بشکن;
	حالت K_K4:		کلید = VTERM_کلید_KP_4; بشکن;
	حالت K_K5:		کلید = VTERM_کلید_KP_5; بشکن;
	حالت K_K6:		کلید = VTERM_کلید_KP_6; بشکن;
	حالت K_K7:		کلید = VTERM_کلید_KP_7; بشکن;
	حالت K_K8:		کلید = VTERM_کلید_KP_8; بشکن;
	حالت K_K9:		کلید = VTERM_کلید_KP_9; بشکن;
	حالت K_KDEL:		کلید = VTERM_کلید_DEL; بشکن; // TODO
	حالت K_KDIVIDE:		کلید = VTERM_کلید_KP_DIVIDE; بشکن;
	حالت K_KEND:		کلید = VTERM_کلید_KP_1; بشکن; // TODO
	حالت K_KENTER:		کلید = VTERM_کلید_KP_ENTER; بشکن;
	حالت K_KHOME:		کلید = VTERM_کلید_KP_7; بشکن; // TODO
	حالت K_KINS:		کلید = VTERM_کلید_KP_0; بشکن; // TODO
	حالت K_KMINUS:		کلید = VTERM_کلید_KP_MINUS; بشکن;
	حالت K_KMULTIPLY:	کلید = VTERM_کلید_KP_MULT; بشکن;
	حالت K_KPAGEDOWN:	کلید = VTERM_کلید_KP_3; بشکن; // TODO
	حالت K_KPAGEUP:		کلید = VTERM_کلید_KP_9; بشکن; // TODO
	حالت K_KPLUS:		کلید = VTERM_کلید_KP_PLUS; بشکن;
	حالت K_KPOINT:		کلید = VTERM_کلید_KP_PERIOD; بشکن;
	حالت K_چپ:		کلید = VTERM_کلید_چپ; بشکن;
	حالت K_S_چپ:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_چپ; بشکن;
	حالت K_C_چپ:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_چپ; بشکن;
	حالت K_PAGEDOWN:	کلید = VTERM_کلید_PAGEDOWN; بشکن;
	حالت K_PAGEUP:		کلید = VTERM_کلید_PAGEUP; بشکن;
	حالت K_راست:		کلید = VTERM_کلید_راست; بشکن;
	حالت K_S_راست:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_راست; بشکن;
	حالت K_C_راست:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_راست; بشکن;
	حالت K_UP:		کلید = VTERM_کلید_UP; بشکن;
	حالت K_S_UP:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_UP; بشکن;
	حالت TAB:		کلید = VTERM_کلید_TAB; بشکن;
	حالت K_S_TAB:		mod = VTERM_MOD_SHIFT;
				کلید = VTERM_کلید_TAB; بشکن;

	حالت K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); بشکن;
	حالت K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); بشکن;
	حالت K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); بشکن;
	حالت K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); بشکن;

	حالت K_LEFTMOUSE:
	حالت K_LEFTMOUSE_NM:
	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:
	حالت K_MOUSEMOVE:
	حالت K_MIDDLEMOUSE:
	حالت K_MIDDLEDRAG:
	حالت K_MIDDLERELEASE:
	حالت K_RIGHTMOUSE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:	اگر (!term_mouse_click(vterm, c))
				    بازگشت 0;
				other = TRUE;
				بشکن;

	حالت K_X1MOUSE:		/* TODO */ بازگشت 0;
	حالت K_X1DRAG:		/* TODO */ بازگشت 0;
	حالت K_X1RELEASE:	/* TODO */ بازگشت 0;
	حالت K_X2MOUSE:		/* TODO */ بازگشت 0;
	حالت K_X2DRAG:		/* TODO */ بازگشت 0;
	حالت K_X2RELEASE:	/* TODO */ بازگشت 0;

	حالت K_IGNORE:		بازگشت 0;
	حالت K_NOP:		بازگشت 0;
	حالت K_UNDO:		بازگشت 0;
	حالت K_HELP:		بازگشت 0;
	حالت K_XF1:		کلید = VTERM_کلید_FUNCTION(1); بشکن;
	حالت K_XF2:		کلید = VTERM_کلید_FUNCTION(2); بشکن;
	حالت K_XF3:		کلید = VTERM_کلید_FUNCTION(3); بشکن;
	حالت K_XF4:		کلید = VTERM_کلید_FUNCTION(4); بشکن;
	حالت K_SELECT:		بازگشت 0;
#ifdef FEAT_GUI
	حالت K_VER_SCROLLBAR:	بازگشت 0;
	حالت K_HOR_SCROLLBAR:	بازگشت 0;
#endif
#ifdef FEAT_GUI_TABLINE
	حالت K_TABLINE:		بازگشت 0;
	حالت K_TABMENU:		بازگشت 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	حالت K_F21:		کلید = VTERM_کلید_FUNCTION(21); بشکن;
#endif
#ifdef FEAT_DND
	حالت K_DROP:		بازگشت 0;
#endif
	حالت K_CURSORHOLD:	بازگشت 0;
	حالت K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				بشکن;
	حالت K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				بشکن;
    }

    // add modifiers for the typed key
    اگر (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    اگر (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    اگر (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    اگر (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    اگر (کلید != VTERM_کلید_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_کلید(vterm, کلید, mod);
    وگرنه اگر (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    بازگشت (صحیح)vterm_output_read(vterm, buf, کلید_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ایستا صحیح
term_job_running_check(term_T *term, صحیح check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    اگر (term == پوچ
	|| term->tl_job == پوچ
	|| !channel_is_open(term->tl_job->jv_channel))
	بازگشت FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    اگر (check_job_status)
	job_status(job);
    بازگشت (job->jv_status == JOB_STARTED
	    || (job->jv_channel != پوچ && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحیح
term_job_running(term_T *term)
{
    بازگشت term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحیح
term_job_running_not_none(term_T *term)
{
    بازگشت term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحیح
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    بازگشت term != پوچ
	&& term->tl_job != پوچ
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحیح
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_پیام_SIZE];
    صحیح	ret;

    dialog_پیام(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, پوچ, buff, 1);
    اگر (ret == VIM_YES)
	بازگشت OK;
    وگرنه
	بازگشت FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    صحیح
term_try_stop_job(buf_T *buf)
{
    صحیح	    شمار;
    نویسه    *how = (نویسه *)buf->b_term->tl_kill;

#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    اگر ((how == پوچ || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	اگر (term_confirm_stop(buf) == OK)
	    how = "kill";
	وگرنه
	    بازگشت FAIL;
    }
#endif
    اگر (how == پوچ || *how == NUL)
	بازگشت FAIL;

    job_stop(buf->b_term->tl_job, پوچ, how);

    // wait for up to a second for the job to die
    برای (شمار = 0; شمار < 100; ++شمار)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	اگر (!buf_valid(buf)
		|| buf->b_term == پوچ
		|| buf->b_term->tl_job == پوچ)
	    بازگشت OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	اگر (job->jv_status >= JOB_ENDED)
	    بازگشت OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    بازگشت FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ایستا تهی
add_scrollback_سطر_to_buffer(term_T *term, char_u *text, صحیح len, صحیح append)
{
    buf_T	*buf = term->tl_buffer;
    صحیح		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_سطر_شمار;

#ifdef MSWIN
    char_u *tmp = text;
    اگر (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = پوچ;
	صحیح	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (نویسه*)text, len + 1,
							   &ret, &length);
	اگر (ret != پوچ)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (نویسه **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    اگر (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	اگر (both == پوچ)
	    بازگشت;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    وگرنه
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    اگر (tmp != text)
	vim_free(text);
#endif
    اگر (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    ایستا تهی
cell2cellattr(ثابت VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ایستا صحیح
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    بازگشت a->fg.red == b->fg.red
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
    ایستا صحیح
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, صحیح lnum)
{
    اگر (ga_grow(&term->tl_scrollback, 1) == FAIL)
	بازگشت FALSE;

    sb_سطر_T *سطر = (sb_سطر_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    اگر (lnum > 0)
    {
	صحیح i;

	برای (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *سطر = *(سطر - 1);
	    --سطر;
	}
    }
    سطر->sb_cols = 0;
    سطر->sb_bytes = 0;
    سطر->sb_cells = پوچ;
    سطر->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    بازگشت OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ایستا تهی
cleanup_scrollback(term_T *term)
{
    sb_سطر_T	*سطر;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_شمار, FALSE);
    bufline_length = STRLEN(bufline);
    وقتی (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	سطر = (sb_سطر_T *)gap->ga_data + gap->ga_len - 1;
	اگر (سطر->sb_bytes < 0 || (size_t)سطر->sb_bytes > bufline_length)
	    بشکن;
	bufline_length -= سطر->sb_bytes;
	اگر (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_سطر_شمار);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_شمار, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(سطر->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    اگر (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_سطر_شمار, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    اگر (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ایستا تهی
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحیح		    len;
    صحیح		    سطرها_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == پوچ ? پوچ : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    برای (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	برای (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    اگر (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    وگرنه
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	اگر (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++سطرها_skipped;
	وگرنه
	{
	    وقتی (سطرها_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--سطرها_skipped;
		اگر (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_سطر_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    اگر (len == 0)
		p = پوچ;
	    وگرنه
		p = ALLOC_MULT(cellattr_T, len);
	    اگر ((p != پوچ || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحیح	    width;
		ثابت VTerm_سطر_Info *lineinfo;
		sb_سطر_T   *سطر = (sb_سطر_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		برای (pos.col = 0; pos.col < len; pos.col += width)
		{
		    اگر (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			اگر (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    وگرنه
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			اگر (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			اگر (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    صحیح	    i;
			    صحیح	    c;

			    برای (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		سطر->sb_cols = len;
		سطر->sb_bytes = ga.ga_len;
		سطر->sb_cells = p;
		سطر->sb_fill_attr = new_fill_attr;
		سطر->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		اگر (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_سطر_to_buffer(term, (char_u *)"", 0, 0);
		وگرنه
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_سطر_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    وگرنه
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    برای (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	اگر (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_سطر_to_buffer(term, (char_u *)"", 0, 0);
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
    ایستا صحیح
for_all_windows_and_curwin(win_T **wp, صحیح *did_curwin)
{
    اگر (*wp == پوچ)
	*wp = firstwin;
    وگرنه اگر ((*wp)->w_next != پوچ)
	*wp = (*wp)->w_next;
    وگرنه اگر (!*did_curwin)
	*wp = curwin;
    وگرنه
	بازگشت FALSE;
    اگر (*wp == curwin)
	*did_curwin = TRUE;
    بازگشت TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    تهی
may_move_terminal_to_buffer(term_T *term, صحیح redraw)
{
    اگر (term->tl_vterm == پوچ)
	بازگشت;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    اگر (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_سطر_شمار
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    اگر (redraw)
    {
	win_T	    *wp = پوچ;
	صحیح	    did_curwin = FALSE;

	وقتی (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    اگر (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_سطر_شمار;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		اگر (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    اگر (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#اگر defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    صحیح
term_check_timers(صحیح next_due_arg, proftime_T *now)
{
    term_T  *term;
    صحیح	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	اگر (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_چپ(&term->tl_timer_due, now);

	    اگر (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    وگرنه اگر (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    بازگشت next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ایستا تهی
set_terminal_mode(term_T *term, صحیح normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    اگر (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    اگر (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ایستا تهی
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    اگر (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ایستا تهی
term_enter_normal_mode(تهی)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    صحیح        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, پوچ);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    اگر (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحیح
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    بازگشت term != پوچ && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    تهی
term_enter_job_mode(تهی)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    اگر (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    اگر (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
تعریفنوع شمارش {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_کلید_state_T;

ایستا reduce_کلید_state_T  no_reduce_کلید_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ایستا صحیح
vterm_using_کلید_protocol(تهی)
{
    بازگشت curbuf->b_term != پوچ
	&& curbuf->b_term->tl_vterm != پوچ
	&& (vterm_is_modify_other_کلیدها(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    تهی
check_no_reduce_کلیدها(تهی)
{
    اگر (no_reduce_کلید_state != NRKS_CHECK
	    || no_reduce_کلیدها >= 1
	    || curbuf->b_term == پوچ
	    || curbuf->b_term->tl_vterm == پوچ)
	بازگشت;

    اگر (vterm_using_کلید_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_کلید_state = NRKS_SET;
	++no_reduce_کلیدها;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ایستا صحیح
term_vgetc(تهی)
{
    صحیح c;
    صحیح save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    اگر (vterm_using_کلید_protocol())
    {
	++no_reduce_کلیدها;
	no_reduce_کلید_state = NRKS_SET;
    }
    وگرنه
    {
	no_reduce_کلید_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    اگر (no_reduce_کلید_state == NRKS_SET)
	--no_reduce_کلیدها;
    no_reduce_کلید_state = NRKS_NONE;

    بازگشت c;
}

ایستا صحیح	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    صحیح
send_کلیدها_to_term(term_T *term, صحیح c, صحیح modmask, صحیح typed)
{
    نویسه	پیام[کلید_BUF_LEN];
    size_t	len;
    صحیح		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    انتخاب (c)
    {
	حالت NUL:
	حالت K_ZERO:
	    اگر (typed)
		stuffcharReadbuff(c);
	    بازگشت FAIL;

	حالت K_TABLINE:
	    stuffcharReadbuff(c);
	    بازگشت FAIL;

	حالت K_IGNORE:
	حالت K_CANCEL:  // used for :normal when running out of chars
	    بازگشت FAIL;

	حالت K_LEFTDRAG:
	حالت K_MIDDLEDRAG:
	حالت K_RIGHTDRAG:
	حالت K_X1DRAG:
	حالت K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	حالت K_LEFTMOUSE:
	حالت K_LEFTMOUSE_NM:
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:
	حالت K_MOUSEMOVE:
	حالت K_MIDDLEMOUSE:
	حالت K_MIDDLERELEASE:
	حالت K_RIGHTMOUSE:
	حالت K_RIGHTRELEASE:
	حالت K_X1MOUSE:
	حالت K_X1RELEASE:
	حالت K_X2MOUSE:
	حالت K_X2RELEASE:

	حالت K_MOUSEUP:
	حالت K_MOUSEDOWN:
	حالت K_MOUSELEFT:
	حالت K_MOUSERIGHT:
	    {
		صحیح	row = mouse_row;
		صحیح	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		اگر (popup_is_popup(curwin))
		{
		    row -= popup_بالاترین_extra(curwin);
		    col -= popup_چپ_extra(curwin);
		}
#endif
		اگر (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    اگر (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    بازگشت FAIL;
		}
	    }
	    بشکن;

	حالت K_COMMAND:
	حالت K_SCRIPT_COMMAND:
	    بازگشت do_cmdkey_command(c, 0);
    }
    اگر (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_کلید(term, c, modmask, پیام);
    اگر (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)پیام, (صحیح)len, پوچ);

    بازگشت OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ایستا تهی
term_paste_register(صحیح prev_c UNUSED)
{
    صحیح		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    صحیح		type;

    اگر (add_to_showcmd(prev_c))
    اگر (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    اگر (!term_use_loop())
	// job finished while waiting for a character
	بازگشت;

    // CTRL-W "= prompt for expression to evaluate.
    اگر (c == '=' && get_عبارت_register() != '=')
	بازگشت;
    اگر (!term_use_loop())
	// job finished while waiting for a character
	بازگشت;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    اگر (l == پوچ)
	بازگشت;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_اقلام(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	اگر (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = پوچ;
	    صحیح	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (نویسه *)s,
		    (صحیح)STRLEN(s), &ret, &length);
	    اگر (ret != پوچ)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (نویسه **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (صحیح)STRLEN(s), پوچ);
#ifdef MSWIN
	اگر (tmp != s)
	    vim_free(s);
#endif

	اگر (item->li_next != پوچ || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, پوچ);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    صحیح
terminal_is_active(تهی)
{
    بازگشت in_terminal_loop != پوچ;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ایستا صحیح
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    اگر (wp != پوچ && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    وگرنه اگر (term->tl_highlight_name != پوچ)
	name = term->tl_highlight_name;
    وگرنه
	name = (char_u*)"Terminal";

    بازگشت syn_name2id(name);
}

#اگر defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    ایستا cursorentry_T entry;
    صحیح			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_چپ ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    اگر (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    اگر (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    اگر (term_bg != INVALCOLOR)
	*fg = term_bg;
    وگرنه
	*fg = gui.back_pixel;

    اگر (term->tl_cursor_color == پوچ)
    {
	اگر (term_fg != INVALCOLOR)
	    *bg = term_fg;
	وگرنه
	    *bg = gui.norm_pixel;
    }
    وگرنه
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    بازگشت &entry;
}
#endif

    ایستا تهی
may_output_cursor_props(تهی)
{
    اگر (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	اگر (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	وگرنه
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ایستا تهی
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    اگر (gui.in_use)
	بازگشت;
#endif
    اگر (in_terminal_loop == term)
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
    ایستا تهی
prepare_restore_cursor_props(تهی)
{
#ifdef FEAT_GUI
    اگر (gui.in_use)
	بازگشت;
#endif
    cursor_color_copy(&desired_cursor_color, پوچ);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ایستا صحیح
term_use_loop_check(صحیح check_job_status)
{
    term_T *term = curbuf->b_term;

    بازگشت term != پوچ
	&& !term->tl_normal_mode
	&& term->tl_vterm != پوچ
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحیح
term_use_loop(تهی)
{
    بازگشت term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    تهی
term_win_entered(تهی)
{
    term_T *term = curbuf->b_term;

    اگر (term == پوچ)
	بازگشت;

    اگر (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	اگر (State & MODE_درج)
	    stop_درج_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    تهی
term_focus_change(صحیح in_focus)
{
    term_T *term = curbuf->b_term;

    اگر (term == پوچ || term->tl_vterm == پوچ)
	بازگشت;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    اگر (in_focus)
	vterm_state_focus_in(state);
    وگرنه
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ایستا صحیح
raw_c_to_ctrl(صحیح c)
{
    اگر ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	بازگشت c & 0x1f;
    بازگشت c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ایستا صحیح
ctrl_to_raw_c(صحیح c)
{
    اگر (c < 0x20 && vterm_using_کلید_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	بازگشت c + '@';
    }
    بازگشت c;
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
    صحیح
terminal_loop(صحیح blocking)
{
    صحیح		c;
    صحیح		raw_c;
    صحیح		termwinkey = 0;
    صحیح		ret;
#ifdef UNIX
    صحیح		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    صحیح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    اگر (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_کلید(curwin->w_p_twk, TRUE);
	اگر (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    وقتی (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	اگر (curbuf->b_term != پوچ && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    وقتی (must_redraw != 0)
		اگر (update_screen(0) == FAIL)
		    بشکن;
	اگر (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    بشکن;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	اگر (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    اگر (raw_c != K_IGNORE)
		vungetc(raw_c);
	    بشکن;
	}
	اگر (raw_c == K_IGNORE)
	    ادامه;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	اگر (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    اگر (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	اگر (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	اگر ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    صحیح	    prev_c = c;
	    صحیح	    prev_raw_c = raw_c;
	    صحیح	    prev_mod_mask = mod_mask;

	    اگر (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    اگر (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		بشکن;

	    اگر (prev_c == Ctrl_BSL)
	    {
		اگر (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    برو theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_کلیدها_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    وگرنه اگر (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    وگرنه اگر (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    وگرنه اگر (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    وگرنه اگر (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		برو theend;
	    }
	    وگرنه اگر (c == '"')
	    {
		term_paste_register(prev_c);
		ادامه;
	    }
	    وگرنه اگر (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		برو theend;
	    }
	}
#ifdef MSWIN
	اگر (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    اگر (MultiByteToWideChar(GetACP(), 0, (نویسه*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	اگر (send_کلیدها_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    اگر (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    برو theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = پوچ;
    اگر (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    اگر (curbuf->b_term != پوچ && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    بازگشت ret;
}

    ایستا تهی
may_toggle_cursor(term_T *term)
{
    اگر (in_terminal_loop != term)
	بازگشت;

    اگر (term->tl_cursor_visible)
	cursor_on();
    وگرنه
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ایستا صحیح
color2index(VTermColor *color, صحیح fg, صحیح *boldp)
{
    صحیح red = color->red;
    صحیح blue = color->blue;
    صحیح green = color->green;

    *boldp = FALSE;

    اگر (VTERM_COLOR_IS_INVALID(color))
	بازگشت 0;

    اگر (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	اگر (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    اگر (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    بازگشت index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	وگرنه اگر (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    بازگشت (color->index & 7) + 1;
	}
	بازگشت 0;
    }

    اگر (t_colors >= 256)
    {
	اگر (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ایستا صحیح cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    صحیح i;

	    اگر (red < 5)
		بازگشت 17; // 00/00/00
	    اگر (red > 245) // ff/ff/ff
		بازگشت 232;
	    برای (i = 0; i < 23; ++i)
		اگر (red < cutoff[i])
		    بازگشت i + 233;
	    بازگشت 256;
	}
	{
	    ایستا صحیح cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    صحیح ri, gi, bi;

	    // 216-color cube
	    برای (ri = 0; ri < 5; ++ri)
		اگر (red < cutoff[ri])
		    بشکن;
	    برای (gi = 0; gi < 5; ++gi)
		اگر (green < cutoff[gi])
		    بشکن;
	    برای (bi = 0; bi < 5; ++bi)
		اگر (blue < cutoff[bi])
		    بشکن;
	    بازگشت 17 + ri * 36 + gi * 6 + bi;
	}
    }
    بازگشت 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ایستا صحیح
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    صحیح attr = 0;

    اگر (cellattrs->bold)
	attr |= HL_BOLD;
    اگر (cellattrs->underline)
	attr |= HL_UNDERLINE;
    اگر (cellattrs->italic)
	attr |= HL_ITALIC;
    اگر (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    اگر (cellattrs->reverse)
	attr |= HL_INVERSE;
    بازگشت attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ایستا تهی
hl2vtermAttr(صحیح attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    اگر (attr & HL_BOLD)
	cell->attrs.bold = 1;
    اگر (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    اگر (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    اگر (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    اگر (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ایستا صحیح
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    صحیح attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    صحیح is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    صحیح is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    اگر (is_default_fg || is_default_bg)
    {
	اگر (wp != پوچ && wp->w_hlfwin_id != 0)
	{
	    اگر (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    اگر (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	وگرنه
	{
	    اگر (is_default_fg)
		fg = &term->tl_default_color.fg;
	    اگر (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	بازگشت get_gui_attr_idx(attr, guifg, guibg);
    }
    وگرنه
#endif
#ifdef FEAT_TERMGUICOLORS
    اگر (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	بازگشت get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    وگرنه
#endif
    {
	صحیح bold = MAYBE;
	صحیح ctermfg = color2index(fg, TRUE, &bold);
	صحیح ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	اگر (bold == TRUE)
	    attr |= HL_BOLD;

	بازگشت get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    بازگشت 0;
}

    ایستا تهی
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    اگر (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    ایستا صحیح
handle_damage(VTermRect rect, تهی *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    بازگشت 1;
}

    ایستا تهی
term_scroll_up(term_T *term, صحیح start_row, صحیح شمار)
{
    win_T		 *wp = پوچ;
    صحیح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحیح			 clear_attr;

    CLEAR_FIELD(attr);

    وقتی (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	اگر (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_سطرها(wp, start_row, شمار, FALSE, FALSE, clear_attr);
	}
    }
}

    ایستا صحیح
handle_moverect(VTermRect dest, VTermRect src, تهی *user)
{
    term_T	*term = (term_T *)user;
    صحیح		شمار = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    اگر (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	اگر (dest.start_row == 0)
	    term->tl_postponed_scroll += شمار;
	وگرنه
	    term_scroll_up(term, dest.start_row, شمار);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    بازگشت 1;
}

    ایستا صحیح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	صحیح visible,
	تهی *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = پوچ;
    صحیح		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    وقتی (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	اگر (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    اگر (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    بازگشت 1;
}

    ایستا صحیح
handle_settermprop(
	VTermProp prop,
	VTerm_مقدار *مقدار,
	تهی *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = پوچ;

    انتخاب (prop)
    {
	حالت VTERM_PROP_TITLE:
	    اگر (disable_vterm_title_for_testing)
		بشکن;
	    strval = vim_strnsave((char_u *)مقدار->string.str,
							    مقدار->string.len);
	    اگر (strval == پوچ)
		بشکن;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    اگر (*skipwhite(strval) == NUL)
		term->tl_title = پوچ;
	    // Same as blank
	    وگرنه اگر (term->tl_arg0_cmd != پوچ
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (صحیح)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = پوچ;
	    // Empty corrupted data of winpty
	    وگرنه اگر (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = پوچ;
#ifdef MSWIN
	    وگرنه اگر (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = پوچ;
		صحیح	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(نویسه*)مقدار->string.str,
					(صحیح)مقدار->string.len, &ret, &length);
		اگر (ret != پوچ)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (نویسه**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    وگرنه
	    {
		term->tl_title = strval;
		strval = پوچ;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    اگر (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    بشکن;

	حالت VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = مقدار->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    بشکن;

	حالت VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = مقدار->boolean;
	    may_set_cursor_props(term);
	    بشکن;

	حالت VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = مقدار->عدد;
	    may_set_cursor_props(term);
	    بشکن;

	حالت VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)مقدار->string.str,
							    مقدار->string.len);
	    اگر (strval == پوچ)
		بشکن;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    بشکن;

	حالت VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = مقدار->boolean;
	    بشکن;

	پیشفرض:
	    بشکن;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    بازگشت 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ایستا صحیح
handle_resize(صحیح rows, صحیح cols, تهی *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    اگر (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    وگرنه
    {
	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    بازگشت 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ایستا تهی
limit_scrollback(term_T *term, garray_T *gap, صحیح update_buffer)
{
    اگر (gap->ga_len < term->tl_buffer->b_p_twsl)
	بازگشت;

    صحیح	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    صحیح	i;
    sb_سطر_T *sb_سطرها = (sb_سطر_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    برای (i = 0; i < todo; ++i)
    {
	اگر (update_buffer && (!sb_سطرها[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_سطرها[i].sb_cells);
    }
    // Continue until end of wrapped line
    برای (; todo < gap->ga_len && sb_سطرها[todo].continuation; ++todo)
	vim_free(sb_سطرها[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_سطر_T *)gap->ga_data + todo,
		اندازه(sb_سطر_T) * gap->ga_len);
    اگر (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = پوچ;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_buffer == term->tl_buffer)
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
    ایستا صحیح
handle_pushline(صحیح cols, ثابت VTermScreenCell *cells, صحیح continuation, تهی *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    صحیح		update_buffer;

    اگر (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    وگرنه
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت 0;

    cellattr_T	*p = پوچ;
    صحیح		len = 0;
    صحیح		i;
    صحیح		c;
    صحیح		col;
    صحیح		text_len;
    char_u		*text;
    sb_سطر_T	*سطر;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    برای (i = 0; i < cols; ++i)
	اگر (cells[i].chars[0] != 0)
	    len = i + 1;
	وگرنه
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    اگر (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    اگر (p != پوچ)
    {
	برای (col = 0; col < len; col += cells[col].width)
	{
	    اگر (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		بشکن;
	    }
	    برای (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    اگر (ga_grow(&ga, 1) == FAIL)
    {
	اگر (update_buffer)
	    text = (char_u *)"";
	وگرنه
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    وگرنه
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    اگر (update_buffer)
	add_scrollback_سطر_to_buffer(term, text, text_len, continuation);

    سطر = (sb_سطر_T *)gap->ga_data + gap->ga_len;
    سطر->sb_cols = len;
    سطر->sb_bytes = text_len;
    سطر->sb_cells = p;
    سطر->sb_fill_attr = fill_attr;
    سطر->continuation = (char_u)continuation;
    اگر (update_buffer)
    {
	سطر->sb_text = پوچ;
	++term->tl_scrollback_scrolled;
	اگر (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    وگرنه
    {
	سطر->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    بازگشت 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ایستا تهی
handle_postponed_scrollback(term_T *term)
{
    صحیح i;

    اگر (term->tl_scrollback_postponed.ga_len == 0)
	بازگشت;
    ch_log(پوچ, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    برای (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_سطر_T	*pp_سطر;
	sb_سطر_T	*سطر;

	اگر (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    بشکن;
	pp_سطر = (sb_سطر_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_سطر->sb_text;
	اگر (text == پوچ)
	    text = (char_u *)"";
	add_scrollback_سطر_to_buffer(term, text, (صحیح)STRLEN(text), pp_سطر->continuation);
	vim_free(pp_سطر->sb_text);

	سطر = (sb_سطر_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	سطر->sb_cols = pp_سطر->sb_cols;
	سطر->sb_bytes = pp_سطر->sb_bytes;
	سطر->sb_cells = pp_سطر->sb_cells;
	سطر->sb_fill_attr = pp_سطر->sb_fill_attr;
	سطر->sb_text = پوچ;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	اگر (!pp_سطر->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ایستا صحیح
handle_bell(تهی *user UNUSED)
{
    vim_beep(BO_TERM);
    بازگشت 0;
}

ایستا VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    پوچ,			// sb_pushline
    پوچ,			// sb_popline
    پوچ,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ایستا صحیح
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    اگر (!term->tl_normal_mode)
    {
	صحیح	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	اگر (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    صحیح		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = پوچ;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    اگر (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		اگر (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    وگرنه
#endif
	    // If this is the last normal window: exit Vim.
	    اگر (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		بازگشت TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(پوچ, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    اگر (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		اگر (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		اگر (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		اگر (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    اگر (pwin != پوچ)
		popup_close_with_retval(pwin, 0);
#endif
	    بازگشت TRUE;
	}
	اگر (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    نویسه    *cmd = term->tl_opencmd == پوچ
				? "botright sbuf %d"
				: (نویسه *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    نویسه    *buf = alloc(len);

	    اگر (buf != پوچ)
	    {
		ch_log(پوچ, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	وگرنه
	    ch_log(پوچ, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    بازگشت FALSE;
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحیح
may_close_term_popup(تهی)
{
    اگر (!popup_is_popup(curwin) || curbuf->b_term == پوچ
				 || term_job_running_not_none(curbuf->b_term))
	بازگشت FAIL;

    win_T *pwin = curwin;

    اگر (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    بازگشت OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    تهی
term_channel_closing(channel_T *ch)
{
    term_T *term;

    برای (term = first_term; term != پوچ; term = term->tl_next)
	اگر (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    تهی
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    صحیح	    did_one = FALSE;

    برای (term = first_term; term != پوچ; term = next_term)
    {
	next_term = term->tl_next;
	اگر (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    اگر (term->tl_out_fd != پوچ)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = پوچ;
	    }
#endif

	    اگر (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		ادامه;
	    }

	    اگر (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    اگر (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	اگر (term != پوچ)
	{
	    اگر (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    تهی
term_check_channel_closed_recently(تهی)
{
    term_T *term;
    term_T *next_term;

    برای (term = first_term; term != پوچ; term = next_term)
    {
	next_term = term->tl_next;
	اگر (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    اگر (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ایستا تهی
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحیح		max_col)
{
    صحیح off = screen_get_current_سطر_off();

    برای (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	صحیح		c;

	اگر (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	اگر (c == NUL)
	{
	    Screen_سطرها[off] = ' ';
	    اگر (enc_utf8)
		Screen_سطرها_UC[off] = NUL;
	}
	وگرنه
	{
	    اگر (enc_utf8)
	    {
		صحیح i;

		// composing chars
		برای (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_سطرها_C[i][off] = cell.chars[i + 1];
		    اگر (cell.chars[i + 1] == 0)
			بشکن;
		}
		اگر (c >= 0x80 || (Screen_mco > 0
					 && Screen_سطرها_C[0][off] != 0))
		{
		    Screen_سطرها[off] = ' ';
		    Screen_سطرها_UC[off] = c;
		}
		وگرنه
		{
		    Screen_سطرها[off] = c;
		    Screen_سطرها_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    وگرنه اگر (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		اگر (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (نویسه*)mb, 2, 0, 0) > 1)
		{
		    Screen_سطرها[off] = mb[0];
		    Screen_سطرها[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		وگرنه
		    Screen_سطرها[off] = c;
	    }
#endif
	    وگرنه
		// This will only store the lower byte of "c".
		Screen_سطرها[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	اگر (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    اگر (enc_utf8)
	    {
		Screen_سطرها_UC[off] = NUL;
		Screen_سطرها[off] = NUL;
	    }
	    وگرنه اگر (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_سطرها[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#اگر defined(FEAT_GUI)
    ایستا تهی
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    اگر (term->tl_vterm == پوچ)
	بازگشت;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    وقتی (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	صحیح save_p_more = p_more;

	p_more = FALSE;
	پیام_row = Rows - 1;
	پیام_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    برای (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	اگر (pos.row < term->tl_rows)
	{
	    صحیح max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, پوچ, screen, &pos, max_col);
	}
	وگرنه
	    pos.col = 0;

	screen_سطر(curwin, term->tl_toprow + pos.row,
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
    صحیح
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    بازگشت term != پوچ && term->tl_vterm != پوچ && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    تهی
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحیح		rows, cols;
    صحیح		newrows, newcols;
    صحیح		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    اگر (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	اگر (term->tl_postponed_scroll > 0
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
    برای (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == پوچ ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	اگر (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	اگر (twp == پوچ)
	    بشکن;
    }
    اگر (newrows == 99999 || newcols == 99999)
	بازگشت; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    اگر (newrows == 0 || newcols == 0)
	بازگشت;

    اگر (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	اگر (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    برای (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	اگر (pos.row < term->tl_rows)
	{
	    صحیح max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	وگرنه
	    pos.col = 0;

	screen_سطر(wp, wp->w_winrow + pos.row
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
    تهی
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    اگر (term == پوچ || term->tl_vterm == پوچ || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	بازگشت;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحیح
term_is_finished(buf_T *buf)
{
    بازگشت buf->b_term != پوچ && buf->b_term->tl_vterm == پوچ;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحیح
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    بازگشت term != پوچ && (term->tl_vterm == پوچ || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    تهی
term_change_in_curbuf(تهی)
{
    term_T *term = curbuf->b_term;

    اگر (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	بازگشت;

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
    صحیح
term_get_attr(win_T *wp, linenr_T lnum, صحیح col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_سطر_T	*سطر;
    cellattr_T	*cellattr;
    صحیح         sb_سطر = -1;
    صحیح         sb_col = col;

    اگر (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_سطر, &sb_col);

    اگر (sb_سطر < 0)
	cellattr = &term->tl_default_color;
    وگرنه
    {
	سطر = (sb_سطر_T *)term->tl_scrollback.ga_data + sb_سطر;
	اگر (sb_col < 0 || sb_col >= سطر->sb_cols)
	    cellattr = &سطر->sb_fill_attr;
	وگرنه
	    cellattr = سطر->sb_cells + sb_col;
    }
    بازگشت cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ایستا تهی
cterm_color2vterm(صحیح nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    اگر (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    وگرنه
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
    ایستا صحیح
get_vterm_color_from_synid(صحیح id, VTermColor *fg, VTermColor *bg)
{
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    اگر (0
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

	اگر (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	اگر (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	وگرنه
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	اگر (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	وگرنه
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	بازگشت TRUE;
    }
    وگرنه
#endif
    اگر (t_colors >= 16)
    {
	صحیح cterm_fg = -1;
	صحیح cterm_bg = -1;

	اگر (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	اگر (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	وگرنه
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	اگر (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	وگرنه
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	بازگشت TRUE;
    }

    بازگشت FALSE;
}

    تهی
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    تهی
term_update_hlfwin(win_T *wp)
{
    صحیح id = wp->w_hlfwin_id;

    اگر (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    تهی
term_update_hlfwin_all(تهی)
{
    win_T	 *wp = پوچ;
    صحیح		 did_curwin = FALSE;

    وقتی (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    تهی
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    صحیح		    fgval, bgval;
    صحیح		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    اگر (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    وگرنه
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, پوچ);

    اگر (!get_vterm_color_from_synid(id, fg, bg))
    {
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحیح tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	اگر (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    اگر (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	وگرنه
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	اگر (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    اگر (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	وگرنه
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ایستا صحیح
term_use_palette(تهی)
{
    اگر (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ایستا تهی
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحیح		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    برای (; index < 16; index++)
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
    ایستا صحیح
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحیح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    برای (li = list->lv_first; li != پوچ && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	اگر (color_name == پوچ)
	    بازگشت FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	اگر (guicolor == INVALCOLOR)
	    بازگشت FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    اگر (n != 16 || li != پوچ)
	بازگشت FAIL;

    set_vterm_palette(vterm, rgb);

    بازگشت OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ایستا تهی
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", پوچ, TRUE);

    اگر (var == پوچ)
	بازگشت;

    اگر (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == پوچ
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ایستا تهی
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    صحیح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = پوچ;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    بازگشت;
	}
    }

    CLEAR_FIELD(ea);

    اگر (opt_item != پوچ && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != پوچ)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	اگر (p == پوچ)
	    p = dict_get_string(dict, "fileformat", FALSE);
	اگر (p != پوچ)
	{
	    اگر (check_ff_مقدار(p) == FAIL)
		ch_log(پوچ, "Invalid ff argument to drop: %s", p);
	    وگرنه
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	اگر (p == پوچ)
	    p = dict_get_string(dict, "encoding", FALSE);
	اگر (p != پوچ)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    اگر (ea.cmd != پوچ)
	    {
		sprintf((نویسه *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	اگر (p != پوچ)
	    get_bad_opt(p, &ea);

	اگر (dict_has_کلید(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	اگر (dict_has_کلید(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	اگر (dict_has_کلید(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	اگر (dict_has_کلید(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    اگر (ea.cmd == پوچ)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ایستا صحیح
is_permitted_term_api(char_u *func, char_u *pat)
{
    بازگشت pat != پوچ && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ایستا تهی
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    اگر (item->li_next == پوچ)
    {
	ch_log(channel, "Missing function arguments for call");
	بازگشت;
    }
    func = tv_get_string(&item->li_tv);

    اگر (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	بازگشت;
    }

    argvars[0].v_type = VAR_عدد;
    argvars[0].vval.v_عدد = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    اگر (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    وگرنه
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
    ایستا size_t
url_decode(ثابت نویسه *src, ثابت size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    وقتی (i < len)
    {
	اگر (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	وگرنه
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    بازگشت j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ایستا تهی
sync_shell_dir(garray_T *gap)
{
    صحیح       offset = 7;  // len of "file://" is 7
    نویسه      *pos = (نویسه *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    وقتی (offset < (صحیح)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    اگر (offset >= (صحیح)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	بازگشت;
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
    ایستا صحیح
parse_osc(صحیح command, VTermStringFragment frag, تهی *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == پوچ ? پوچ
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    اگر (command != 51 && (command != 7 || !p_asd))
	بازگشت 0;

    // Concatenate what was received until the final piece is found.
    اگر (ga_grow(gap, (صحیح)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	بازگشت 1;
    }
    mch_memmove((نویسه *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (صحیح)frag.len;
    اگر (!frag.final)
	بازگشت 1;

    ((نویسه *)gap->ga_data)[gap->ga_len] = 0;

    اگر (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	بازگشت 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = پوچ;
    reader.js_used = 0;
    اگر (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != پوچ)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	اگر (item == پوچ)
	    ch_log(channel, "Missing command");
	وگرنه
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    اگر (item == پوچ)
		ch_log(channel, "Missing argument for %s", cmd);
	    وگرنه اگر (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    وگرنه اگر (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    وگرنه
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    وگرنه
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    بازگشت 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ایستا صحیح
parse_csi(
	ثابت نویسه  *leader UNUSED,
	ثابت long  args[],
	صحیح	    argcount,
	ثابت نویسه  *intermed UNUSED,
	نویسه	    command,
	تهی	    *user)
{
    term_T	*term = (term_T *)user;
    نویسه	buf[100];
    صحیح		len;
    صحیح		x = 0;
    صحیح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    اگر (command != 't' || argcount != 1 || args[0] != 13)
	بازگشت 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (تهی)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_buffer == term->tl_buffer)
	    بشکن;
    اگر (wp != پوچ)
    {
#ifdef FEAT_GUI
	اگر (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	وگرنه
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
						     (char_u *)buf, len, پوچ);
    بازگشت 1;
}

ایستا VTermStateFallbacks state_fallbacks = {
    پوچ,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    پوچ,		// dcs
    پوچ,		// apc
    پوچ,		// pm
    پوچ		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ایستا تهی *
vterm_malloc(size_t size, تهی *data UNUSED)
{
    // make sure that the length is not zero
    بازگشت alloc_clear(size == 0 ? 1L : size);
}

    ایستا تهی
vterm_memfree(تهی *ptr, تهی *data UNUSED)
{
    vim_free(ptr);
}

ایستا VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ایستا صحیح
create_vterm(term_T *term, صحیح rows, صحیح cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_مقدار	    مقدار;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, پوچ);
    term->tl_vterm = vterm;
    اگر (vterm == پوچ)
	بازگشت FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    اگر (state == پوچ || screen == پوچ)
    {
	vterm_free(vterm);
	بازگشت FAIL;
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

    اگر (t_colors < 16)
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
    اگر (GetCaretBlinkTime() == INFINITE)
	مقدار.boolean = 0;
    وگرنه
	مقدار.boolean = 1;
#وگرنه
    مقدار.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &مقدار);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    بازگشت OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ایستا تهی
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحیح		index;

    برای (index = 0; index < 16; index++)
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

    ایستا تهی
term_update_palette(term_T *term)
{
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (term_use_palette()
	    && (term->tl_palette != پوچ
		|| find_var((char_u *)"g:terminal_ansi_colors", پوچ, TRUE)
								      != پوچ))
    {
	اگر (term->tl_palette != پوچ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وگرنه
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    وگرنه
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    تهی
term_update_palette_all(تهی)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	اگر (term->tl_vterm == پوچ)
	    ادامه;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    تهی
term_update_colors_all(تهی)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	اگر (term->tl_vterm == پوچ)
	    ادامه;
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
    اگر (term->tl_status_text != پوچ)
	بازگشت term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    اگر (term->tl_normal_mode)
    {
	اگر (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	وگرنه
	    txt = (char_u *)_("Terminal-finished");
    }
    وگرنه اگر (term->tl_title != پوچ)
	txt = term->tl_title;
    وگرنه اگر (term_none_open(term))
	txt = (char_u *)_("active");
    وگرنه اگر (term_job_running(term))
	txt = (char_u *)_("running");
    وگرنه
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    اگر (term->tl_status_text != پوچ)
	vim_snprintf((نویسه *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    بازگشت term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    تهی
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    صحیح
set_ref_in_term(صحیح copyID)
{
    صحیح		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    برای (term = first_term; !abort && term != پوچ; term = term->tl_next)
	اگر (term->tl_job != پوچ)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
	}
    بازگشت abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ایستا buf_T *
term_get_buf(typval_T *argvars, نویسه *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    اگر (buf == پوچ || buf->b_term == پوچ)
    {
	(تهی)tv_get_عدد(&argvars[0]);    // issue errmsg if type error
	ch_log(پوچ, "%s: invalid buffer argument", where);
	بازگشت پوچ;
    }
    بازگشت buf;
}

    ایستا تهی
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ایستا تهی
dump_term_color(FILE *fd, VTermColor *color)
{
    صحیح index;

    اگر (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    وگرنه اگر (color->type == 0)
	// use RGB values
	index = 255;
    وگرنه
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (صحیح)color->red, (صحیح)color->green, (صحیح)color->blue,
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
    تهی
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    صحیح		max_height = 0;
    صحیح		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    اگر (term->tl_vterm == پوچ)
    {
	emsg(_(e_job_already_finished));
	بازگشت;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    بازگشت;
	d = argvars[2].vval.v_dict;
	اگر (d != پوچ)
	{
	    max_height = dict_get_عدد(d, "rows");
	    max_width = dict_get_عدد(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    اگر (fname == پوچ)
	بازگشت;
    اگر (mch_stat((نویسه *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	بازگشت;
    }

    اگر (*fname == NUL || (fd = mch_fopen((نویسه *)fname, WRITEBIN)) == پوچ)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	بازگشت;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    برای (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	صحیح		repeat = 0;

	برای (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    صحیح		    same_attr;
	    صحیح		    same_chars = TRUE;
	    صحیح		    i;
	    صحیح		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    اگر (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    برای (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		صحیح c = cell.chars[i];
		صحیح pc = prev_cell.chars[i];
		صحیح should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		اگر (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		اگر (c != pc)
		    same_chars = FALSE;
		اگر (should_break)
		    بشکن;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    اگر (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    وگرنه
	    {
		اگر (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		اگر (cell.chars[0] == NUL)
		    fputs(" ", fd);
		وگرنه
		{
		    char_u	charbuf[10];
		    صحیح		len;

		    برای (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		اگر (cell.width != prev_cell.width || !same_attr)
		{
		    اگر (cell.width == 2)
			fputs("*", fd);
		    وگرنه
			fputs("+", fd);

		    اگر (same_attr)
		    {
			fputs("&", fd);
		    }
		    وگرنه
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			اگر (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			وگرنه
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			اگر (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			وگرنه
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    اگر (cell.width == 2)
		++pos.col;
	}
	اگر (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ایستا تهی
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ایستا تهی
append_cell(garray_T *gap, cellattr_T *cell)
{
    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ایستا تهی
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
    ایستا صحیح
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    صحیح		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {پوچ, 0};
    صحیح		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    صحیح		    max_cells = 0;
    صحیح		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, اندازه(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    برای (;;)
    {
	اگر (c == EOF)
	    بشکن;
	اگر (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	وگرنه اگر (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    اگر (ga_text.ga_data == پوچ)
		dump_is_corrupt(&ga_text);
	    اگر (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_سطر_T   *سطر = (sb_سطر_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		اگر (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		سطر->sb_cols = ga_cell.ga_len;
		سطر->sb_bytes = ga_text.ga_len;
		سطر->sb_cells = ga_cell.ga_data;
		سطر->sb_fill_attr = term->tl_default_color;
		سطر->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_سطر_شمار, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    وگرنه
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	وگرنه اگر (c == '|' || c == '>')
	{
	    صحیح prev_len = ga_text.ga_len;

	    اگر (c == '>')
	    {
		اگر (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    اگر (c != EOF)
		ga_append(&ga_text, c);
	    برای (;;)
	    {
		c = fgetc(fd);
		اگر (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    بشکن;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    اگر (ga_text.ga_data != پوچ)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		اگر (prev_char.string == پوچ)
		    prev_char.length = 0;
	    }

	    اگر (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    وگرنه اگر (c == '+' || c == '*')
	    {
		صحیح is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		اگر (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		وگرنه اگر (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    وقتی (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    برای (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			اگر (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			وگرنه اگر (c == '#')
			{
			    صحیح red, green, blue, index = 0, type;

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
			    اگر (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    وقتی (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    اگر (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				اگر (index == 0)
				{
				    اگر (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    وگرنه
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    وگرنه
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    اگر (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    وگرنه
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			وگرنه
			    dump_is_corrupt(&ga_text);
		    }
		}
		وگرنه
		    dump_is_corrupt(&ga_text);
	    }
	    وگرنه
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    اگر (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	وگرنه اگر (c == '@')
	{
	    اگر (prev_char.string == پوچ)
		dump_is_corrupt(&ga_text);
	    وگرنه
	    {
		صحیح شمار = 0;

		// repeat previous character, get the count
		برای (;;)
		{
		    c = fgetc(fd);
		    اگر (!SAFE_isdigit(c))
			بشکن;
		    شمار = شمار * 10 + (c - '0');
		}

		وقتی (شمار-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	وگرنه
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    اگر (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_سطر_شمار, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    بازگشت max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ایستا char_u *
get_separator(صحیح text_width, char_u *fname)
{
    صحیح	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    صحیح	    fname_size;
    char_u  *p = fname;
    صحیح	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    اگر (textline == پوچ)
	بازگشت پوچ;

    fname_size = vim_strsize(fname);
    اگر (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    وگرنه اگر (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    وقتی (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    برای (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    برای (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    بازگشت textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ایستا تهی
term_load_dump(typval_T *argvars, typval_T *rettv, صحیح do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = پوچ;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = پوچ;
    char_u	*fname_tofree = پوچ;
    FILE	*fd1;
    FILE	*fd2 = پوچ;
    char_u	*textline = پوچ;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    اگر (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    اگر (fname1 == پوچ || (do_diff && fname2 == پوچ))
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }
    fd1 = mch_fopen((نویسه *)fname1, READBIN);
    اگر (fd1 == پوچ)
    {
	semsg(_(e_cant_read_file_str), fname1);
	بازگشت;
    }
    اگر (do_diff)
    {
	fd2 = mch_fopen((نویسه *)fname2, READBIN);
	اگر (fd2 == پوچ)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    بازگشت;
	}
    }

    init_job_options(&opt);
    اگر (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	برو theend;

    اگر (opt.jo_term_name == پوچ)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	اگر (fname_tofree != پوچ)
	{
	    vim_snprintf((نویسه *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    اگر (opt.jo_bufnr_buf != پوچ)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	اگر (wp == پوچ)
	    semsg(_(e_invalid_argument_str), "bufnr");
	وگرنه
	{
	    buf = curbuf;
	    وقتی (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    وگرنه
	// Create a new terminal window.
	buf = term_start(&argvars[0], پوچ, &opt, TERM_START_NOJOB);

    اگر (buf != پوچ && buf->b_term != پوچ)
    {
	صحیح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	صحیح		width;
	صحیح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_عدد = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	اگر (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	اگر (!do_diff)
	    برو theend;

	term->tl_بالاترین_diff_rows = curbuf->b_ml.ml_سطر_شمار;

	textline = get_separator(width, fname1);
	اگر (textline == پوچ)
	    برو theend;
	اگر (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_شمار, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	اگر (textline == پوچ)
	    برو theend;
	اگر (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_شمار, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_سطر_شمار;
	width2 = read_dump_file(fd2, &cursor_pos2);
	اگر (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    اگر (textline == پوچ)
		برو theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_سطر_شمار - bot_lnum;

	برای (lnum = 1; lnum <= term->tl_بالاترین_diff_rows; ++lnum)
	{
	    اگر (lnum + bot_lnum > curbuf->b_ml.ml_سطر_شمار)
	    {
		// bottom part has fewer rows, fill with "-"
		برای (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    وگرنه
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحیح	col;
		sb_سطر_T   *sb_سطر = (sb_سطر_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_سطر + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_سطر + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		اگر (line1 == پوچ)
		    بشکن;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		برای (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    صحیح len1 = utfc_ptr2len(p1);
		    صحیح len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    اگر (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    وگرنه اگر (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    وگرنه اگر (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    وگرنه اگر (cellattr1 != پوچ && cellattr2 != پوچ)
		    {
			اگر ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			وگرنه اگر (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			وگرنه اگر (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			وگرنه اگر (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		وقتی (col < width)
		{
		    اگر (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    وگرنه اگر (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    وگرنه
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    اگر (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_بالاترین_diff_rows) == OK)
		ml_append(term->tl_بالاترین_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	وقتی (lnum + bot_lnum <= curbuf->b_ml.ml_سطر_شمار)
	{
	    // bottom part has more rows, fill with "+"
	    برای (i = 0; i < width; ++i)
		textline[i] = '+';
	    اگر (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_بالاترین_diff_rows) == OK)
		ml_append(term->tl_بالاترین_diff_rows + lnum, textline, 0, FALSE);
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
    اگر (fd2 != پوچ)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    صحیح
term_swap_diff(تهی)
{
    term_T	*term = curbuf->b_term;
    linenr_T	سطر_شمار;
    linenr_T	بالاترین_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_سطر_T	*sb_سطر;

    اگر (term == پوچ
	    || !term_is_finished(curbuf)
	    || term->tl_بالاترین_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	بازگشت FAIL;

    سطر_شمار = curbuf->b_ml.ml_سطر_شمار;
    بالاترین_rows = term->tl_بالاترین_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = سطر_شمار - bot_rows;
    sb_سطر = (sb_سطر_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    برای (lnum = 1; lnum <= بالاترین_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	اگر (p == پوچ)
	    بازگشت OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    برای (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	اگر (p == پوچ)
	    بازگشت OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    اگر (p == پوچ)
	بازگشت OK;
    ml_append(سطر_شمار - بالاترین_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(سطر_شمار - بالاترین_rows));
    اگر (p == پوچ)
	بازگشت OK;
    ml_delete(سطر_شمار - بالاترین_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    اگر (بالاترین_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	برای (lnum = 0; lnum < بالاترین_rows; ++lnum)
	{
	    sb_سطر_T	temp;

	    temp = *(sb_سطر + lnum);
	    *(sb_سطر + lnum) = *(sb_سطر + bot_start + lnum);
	    *(sb_سطر + bot_start + lnum) = temp;
	}
    }
    وگرنه
    {
	size_t		size = اندازه(sb_سطر_T) * term->tl_scrollback.ga_len;
	sb_سطر_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	اگر (temp != پوچ)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    اندازه(sb_سطر_T) * bot_rows);
	    mch_memmove((sb_سطر_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + بالاترین_rows,
		    اندازه(sb_سطر_T) * (سطر_شمار - بالاترین_rows - bot_rows));
	    mch_memmove((sb_سطر_T *)term->tl_scrollback.ga_data
						       + سطر_شمار - بالاترین_rows,
		    temp,
		    اندازه(sb_سطر_T) * بالاترین_rows);
	    vim_free(temp);
	}
    }

    term->tl_بالاترین_diff_rows = bot_rows;
    term->tl_bot_diff_rows = بالاترین_rows;

    update_screen(UPD_NOT_VALID);
    بازگشت OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    تهی
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    تهی
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    تهی
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    اگر (buf == پوچ)
	بازگشت;
    rettv->vval.v_عدد = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    تهی
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    صحیح	    attr;
    size_t  i;
    char_u  *name;

    ایستا ساختار {
	نویسه	    *name;
	صحیح	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    attr = tv_get_عدد(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    اگر (name == پوچ)
	بازگشت;

    اگر (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    برای (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	اگر (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_عدد = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    بشکن;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    تهی
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getcursor()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_عدد(l, term->tl_cursor_pos.row + 1);
    list_append_عدد(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    اگر (d == پوچ)
	بازگشت;

    dict_add_عدد(d, "visible", term->tl_cursor_visible);
    dict_add_عدد(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_عدد(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    اگر (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    تهی
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getjob()");
    اگر (buf == پوچ)
    {
	اگر (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = پوچ;
	}
	وگرنه
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NULL;
	}
	بازگشت;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    اگر (rettv->vval.v_job != پوچ)
	++rettv->vval.v_job->jv_refcount;
}

    ایستا صحیح
get_row_عدد(typval_T *tv, term_T *term)
{
    اگر (tv->v_type == VAR_STRING
	    && tv->vval.v_string != پوچ
	    && STRCMP(tv->vval.v_string, ".") == 0)
	بازگشت term->tl_cursor_pos.row;
    بازگشت (صحیح)tv_get_عدد(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    تهی
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    صحیح		    row;

    rettv->v_type = VAR_STRING;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_getline()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    row = get_row_عدد(&argvars[1], term);

    اگر (term->tl_vterm == پوچ)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	صحیح	  sb_row = term->tl_scrollback_scrolled + row;
	sb_سطر_T *سطر;

	اگر (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    بازگشت;
	سطر = (sb_سطر_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, پوچ, &offset);

	// vterm is finished, get the text from the buffer
	اگر (lnum > 0 && lnum <= buf->b_ml.ml_سطر_شمار)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    اگر (STRLEN(p) >= offset + سطر->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, سطر->sb_bytes);
	}
    }
    وگرنه
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	صحیح		len;
	char_u		*p;

	اگر (row < 0 || row >= term->tl_rows)
	    بازگشت;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	اگر (p == پوچ)
	    بازگشت;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (نویسه *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    تهی
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getscrolled()");
    اگر (buf == پوچ)
	بازگشت;
    rettv->vval.v_عدد = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    تهی
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getsize()");
    اگر (buf == پوچ)
	بازگشت;

    l = rettv->vval.v_list;
    list_append_عدد(l, buf->b_term->tl_rows);
    list_append_عدد(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    تهی
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_setsize()");
    اگر (buf == پوچ)
    {
	emsg(_(e_not_terminal_buffer));
	بازگشت;
    }
    اگر (buf->b_term->tl_vterm == پوچ)
	بازگشت;
    term = buf->b_term;
    rows = tv_get_عدد(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_عدد(&argvars[2]);
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
    تهی
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getstatus()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;

    اگر (term_job_running(term))
	STRCPY(val, "running");
    وگرنه
	STRCPY(val, "finished");
    اگر (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    تهی
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_gettitle()");
    اگر (buf == پوچ)
	بازگشت;

    اگر (buf->b_term->tl_title != پوچ)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    تهی
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = پوچ;
    صحیح		رقم = 0;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    اگر (buf == پوچ)
	بازگشت;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	رقم = tv_get_bool(&argvars[1]);

    انتخاب (رقم)
    {
	حالت 0:
	    اگر (buf->b_term->tl_job != پوچ)
		p = buf->b_term->tl_job->jv_tty_out;
	    بشکن;
	حالت 1:
	    اگر (buf->b_term->tl_job != پوچ)
		p = buf->b_term->tl_job->jv_tty_in;
	    بشکن;
	پیشفرض:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    بازگشت;
    }
    اگر (p != پوچ)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    تهی
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    اگر (rettv_list_alloc(rettv) == FAIL || first_term == پوچ)
	بازگشت;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	اگر (tp->tl_buffer != پوچ)
	    اگر (list_append_عدد(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		بازگشت;
}

/*
 * "term_scrape(buf, row)" function
 */
    تهی
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = پوچ;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_سطر_T	    *سطر;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_scrape()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_عدد(&argvars[1], term);

    اگر (term->tl_vterm != پوچ)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	اگر (screen == پوچ)  // can't really happen
	    بازگشت;
	p = پوچ;
	سطر = پوچ;
    }
    وگرنه
    {
	صحیح	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, پوچ, &offset);

	اگر (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_سطر_شمار)
	    بازگشت;

	سطر = (sb_سطر_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	اگر (STRLEN(p) < offset + سطر->sb_bytes)
	    بازگشت;

	p += offset;
    }

    برای (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	صحیح		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	صحیح		i;

	اگر (screen == پوچ)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    اگر (pos.col >= سطر->sb_cols)
		بشکن;
	    cellattr = سطر->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	وگرنه
	{
	    VTermScreenCell cell;

	    اگر (vterm_screen_get_cell(screen, pos, &cell) == 0)
		بشکن;

	    mbslen = 0;
	    برای (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		اگر (cell.chars[i] == 0)
		    بشکن;
		mbslen += (*utf_char2bytes)((صحیح)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	اگر (dcell == پوچ)
	    بشکن;
	اگر (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    بشکن;
	}

	dict_add_string_len(dcell, "chars", mbs, (صحیح)mbslen);

	rgblen = vim_snprintf_safelen((نویسه *)rgb, اندازه(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (صحیح)rgblen);
	rgblen = vim_snprintf_safelen((نویسه *)rgb, اندازه(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (صحیح)rgblen);

	dict_add_عدد(dcell, "attr",
				      cell2attr(term, پوچ, &attrs, &fg, &bg));
	dict_add_عدد(dcell, "width", width);

	++pos.col;
	اگر (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    تهی
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*پیام;
    term_T	*term;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_sendkeys()");
    اگر (buf == پوچ)
	بازگشت;

    پیام = tv_get_string_chk(&argvars[1]);
    اگر (پیام == پوچ)
	بازگشت;
    term = buf->b_term;
    اگر (term->tl_vterm == پوچ)
	بازگشت;

    وقتی (*پیام != NUL)
    {
	صحیح c;

	اگر (*پیام == K_SPECIAL && پیام[1] != NUL && پیام[2] != NUL)
	{
	    c = TO_SPECIAL(پیام[1], پیام[2]);
	    پیام += 3;
	}
	وگرنه
	{
	    c = PTR2CHAR(پیام);
	    پیام += MB_CPTR2LEN(پیام);
	}
	send_کلیدها_to_term(term, c, 0, FALSE);
    }
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    تهی
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحیح		index;
    list_T	*list;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = term_get_buf(argvars, "term_getansicolors()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    اگر (term->tl_vterm == پوچ)
	بازگشت;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    برای (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((نویسه *)hexbuf, اندازه(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	اگر (list_append_string(list, hexbuf, (صحیح)hexbuflen) == FAIL)
	    بازگشت;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    تهی
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    صحیح		n = 0;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_setansicolors()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    اگر (term->tl_vterm == پوچ)
	بازگشت;

    اگر (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_مقدار_for_argument_str), "\"colors\"");
	بازگشت;
    }

    اگر (term->tl_palette == پوچ)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    اگر (term->tl_palette == پوچ)
	بازگشت;

    FOR_ALL_LIST_اقلام(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	اگر (color_name == پوچ)
	    بازگشت;

	guicolor = GUI_GET_COLOR(color_name);
	اگر (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    بازگشت;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    تهی
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_setapi()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    اگر (api != پوچ)
	term->tl_api = vim_strsave(api);
    وگرنه
	term->tl_api = پوچ;
}

/*
 * "term_setrestore(buf, command)" function
 */
    تهی
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#اگر defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_setrestore()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    اگر (cmd != پوچ)
	term->tl_command = vim_strsave(cmd);
    وگرنه
	term->tl_command = پوچ;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    تهی
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_setkill()");
    اگر (buf == پوچ)
	بازگشت;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    اگر (how != پوچ)
	term->tl_kill = vim_strsave(how);
    وگرنه
	term->tl_kill = پوچ;
}

/*
 * "term_start(command, options)" function
 */
    تهی
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    اگر (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    init_job_options(&opt);
    اگر (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	بازگشت;

    buf = term_start(&argvars[0], پوچ, &opt, 0);

    اگر (buf != پوچ && buf->b_term != پوچ)
	rettv->vval.v_عدد = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    تهی
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    buf = term_get_buf(argvars, "term_wait()");
    اگر (buf == پوچ)
	بازگشت;
    اگر (buf->b_term->tl_job == پوچ)
    {
	ch_log(پوچ, "term_wait(): no job to wait for");
	بازگشت;
    }
    اگر (buf->b_term->tl_job->jv_channel == پوچ)
	// channel is closed, nothing to do
	بازگشت;

    // Get the job status, this will detect a job that finished.
    اگر (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(پوچ, "term_wait(): waiting for channel to close");
	وقتی (buf->b_term != پوچ && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    اگر (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		بشکن;
	    اگر (buf->b_term == پوچ || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		بشکن;
	}

	term_flush_messages();
    }
    وگرنه
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_عدد(&argvars[1]);
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
    تهی
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	اگر (term->tl_job == ch->ch_job)
	{
	    اگر (term->tl_eof_chars != پوچ)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(صحیح)STRLEN(term->tl_eof_chars), پوچ);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, پوچ);
	    }
#ifdef MSWIN
	    وگرنه
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, پوچ);
#endif
	}
}

#اگر defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    بازگشت term != پوچ ? term->tl_job : پوچ;
}
#endif

#اگر defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
تهی (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ایستا صحیح
dyn_conpty_init(صحیح verbose)
{
    ایستا HMODULE	hKerneldll = پوچ;
    صحیح			i;
    ایستا ساختار
    {
	نویسه	*name;
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
	{پوچ, پوچ}
    };

    اگر (!has_conpty_working())
    {
	اگر (verbose)
	    emsg(_(e_conpty_is_not_available));
	بازگشت FAIL;
    }

    // No need to initialize twice.
    اگر (hKerneldll)
	بازگشت OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    برای (i = 0; conpty_entry[i].name != پوچ
					&& conpty_entry[i].ptr != پوچ; ++i)
    {
	اگر ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == پوچ)
	{
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = پوچ;
	    بازگشت FAIL;
	}
    }

    بازگشت OK;
}

    ایستا صحیح
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	نویسه	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = پوچ;
    WCHAR	    *cmd_wchar_copy = پوچ;
    WCHAR	    *cwd_wchar = پوچ;
    WCHAR	    *env_wchar = پوچ;
    channel_T	    *channel = پوچ;
    job_T	    *job = پوچ;
    HANDLE	    jo = پوچ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = پوچ;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = پوچ;
    HANDLE	    o_theirs = پوچ;
    HANDLE	    i_ours = پوچ;
    HANDLE	    o_ours = پوچ;
    نویسه	    *errmsg = پوچ;

    ga_init2(&ga_cmd, اندازه(نویسه*), 20);
    ga_init2(&ga_env, اندازه(نویسه*), 20);

    اگر (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    وگرنه اگر (argvar->v_type == VAR_LIST)
    {
	اگر (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    برو failed;
	cmd = ga_cmd.ga_data;
    }
    اگر (cmd == پوچ || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	برو failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, پوچ);

    اگر (cmd_wchar != پوچ)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    اگر (cmd_wchar == پوچ)
	برو failed;
    اگر (opt->jo_cwd != پوچ)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, پوچ);
	اگر (cwd_wchar == پوچ)
	    برو failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    اگر (!CreatePipe(&i_theirs, &i_ours, پوچ, 0))
	برو failed;
    اگر (!CreatePipe(&o_ours, &o_theirs, پوچ, 0))
	برو failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    اگر (FAILED(hr))
	برو failed;

    term->tl_siex.StartupInfo.cb = اندازه(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_درون_داد = INVALID_HANDLE_مقدار;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_مقدار;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_مقدار;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(پوچ, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    اگر (!term->tl_siex.lpAttributeList)
	برو failed;
    اگر (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	برو failed;
    اگر (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    اندازه(HPCON), پوچ, پوچ))
	برو failed;

    channel = add_channel();
    اگر (channel == پوچ)
	برو failed;

    job = job_alloc();
    اگر (job == پوچ)
	برو failed;
    اگر (argvar->v_type == VAR_STRING)
    {
	صحیح argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    وگرنه
    {
	صحیح argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    اگر (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    اگر (!CreateProcessW(پوچ, cmd_wchar_copy, پوچ, پوچ, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	برو failed;
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

    jo = CreateJobObject(پوچ, پوچ);
    اگر (jo == پوچ)
	برو failed;

    اگر (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = پوچ;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    اگر (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	برو failed;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (term_use_palette())
    {
	اگر (term->tl_palette != پوچ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وگرنه
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
    اگر (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((نویسه *)fname, WRITEBIN);
	اگر (term->tl_out_fd == پوچ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    بازگشت OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    اگر (channel != پوچ)
	channel_clear(channel);
    اگر (job != پوچ)
    {
	job->jv_channel = پوچ;
	job_cleanup(job);
    }
    term->tl_job = پوچ;
    اگر (jo != پوچ)
	CloseHandle(jo);

    اگر (term->tl_siex.lpAttributeList != پوچ)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = پوچ;
    اگر (o_theirs != پوچ)
	CloseHandle(o_theirs);
    اگر (o_ours != پوچ)
	CloseHandle(o_ours);
    اگر (i_ours != پوچ)
	CloseHandle(i_ours);
    اگر (i_theirs != پوچ)
	CloseHandle(i_theirs);
    اگر (term->tl_conpty != پوچ)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = پوچ;
    // Propagate errors that occur in CreateProcess
    اگر (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    بازگشت FAIL;
}

    ایستا تهی
conpty_term_report_winsize(term_T *term, صحیح rows, صحیح cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    ایستا تهی
term_free_conpty(term_T *term)
{
    اگر (term->tl_siex.lpAttributeList != پوچ)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = پوچ;
    اگر (term->tl_conpty != پوچ)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = پوچ;
}

    صحیح
use_conpty(تهی)
{
    بازگشت has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

تهی* (*winpty_config_new)(UINT64, تهی*);
تهی* (*winpty_open)(تهی*, تهی*);
تهی* (*winpty_spawn_config_new)(UINT64, تهی*, LPCWSTR, تهی*, تهی*, تهی*);
BOOL (*winpty_spawn)(تهی*, تهی*, HANDLE*, HANDLE*, DWORD*, تهی*);
تهی (*winpty_config_set_mouse_mode)(تهی*, صحیح);
تهی (*winpty_config_set_initial_size)(تهی*, صحیح, صحیح);
LPCWSTR (*winpty_conin_name)(تهی*);
LPCWSTR (*winpty_conout_name)(تهی*);
LPCWSTR (*winpty_conerr_name)(تهی*);
تهی (*winpty_free)(تهی*);
تهی (*winpty_config_free)(تهی*);
تهی (*winpty_spawn_config_free)(تهی*);
تهی (*winpty_error_free)(تهی*);
LPCWSTR (*winpty_error_پیام)(تهی*);
BOOL (*winpty_set_size)(تهی*, صحیح, صحیح, تهی*);
HANDLE (*winpty_agent_process)(تهی*);

# define WINPTY_DLL "winpty.dll"

ایستا HINSTANCE hWinPtyDLL = پوچ;

    ایستا صحیح
dyn_winpty_init(صحیح verbose)
{
    صحیح i;
    ایستا ساختار
    {
	نویسه	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_پیام},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{پوچ, پوچ}
    };

    // No need to initialize twice.
    اگر (hWinPtyDLL)
	بازگشت OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    اگر (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((نویسه *)p_winptydll);
    اگر (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    اگر (!hWinPtyDLL)
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	بازگشت FAIL;
    }
    برای (i = 0; winpty_entry[i].name != پوچ
					 && winpty_entry[i].ptr != پوچ; ++i)
    {
	اگر ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == پوچ)
	{
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = پوچ;
	    بازگشت FAIL;
	}
    }

    بازگشت OK;
}

    ایستا صحیح
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	نویسه	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = پوچ;
    WCHAR	    *cwd_wchar = پوچ;
    WCHAR	    *env_wchar = پوچ;
    channel_T	    *channel = پوچ;
    job_T	    *job = پوچ;
    DWORD	    error;
    HANDLE	    jo = پوچ;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    تهی	    *winpty_err = پوچ;
    تهی	    *spawn_config = پوچ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = پوچ;

    ga_init2(&ga_cmd, اندازه(نویسه*), 20);
    ga_init2(&ga_env, اندازه(نویسه*), 20);

    اگر (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    وگرنه اگر (argvar->v_type == VAR_LIST)
    {
	اگر (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    برو failed;
	cmd = ga_cmd.ga_data;
    }
    اگر (cmd == پوچ || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	برو failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, پوچ);
    ga_clear(&ga_cmd);
    اگر (cmd_wchar == پوچ)
	برو failed;
    اگر (opt->jo_cwd != پوچ)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, پوچ);
	اگر (cwd_wchar == پوچ)
	    برو failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    اگر (term->tl_winpty_config == پوچ)
	برو failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    اگر (term->tl_winpty == پوچ)
	برو failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    پوچ,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    اگر (spawn_config == پوچ)
	برو failed;

    channel = add_channel();
    اگر (channel == پوچ)
	برو failed;

    job = job_alloc();
    اگر (job == پوچ)
	برو failed;
    اگر (argvar->v_type == VAR_STRING)
    {
	صحیح argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    وگرنه
    {
	صحیح argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    اگر (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    اگر (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	برو failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, پوچ,
	    OPEN_EXISTING, 0, پوچ),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, پوچ,
	    OPEN_EXISTING, 0, پوچ),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, پوچ,
	    OPEN_EXISTING, 0, پوچ));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(پوچ, پوچ);
    اگر (jo == پوچ)
	برو failed;

    اگر (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = پوچ;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    اگر (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	برو failed;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (term_use_palette())
    {
	اگر (term->tl_palette != پوچ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وگرنه
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
	    (short_u *)winpty_conin_name(term->tl_winpty), پوچ);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), پوچ);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    اگر (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((نویسه *)fname, WRITEBIN);
	اگر (term->tl_out_fd == پوچ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    بازگشت OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    اگر (spawn_config != پوچ)
	winpty_spawn_config_free(spawn_config);
    اگر (channel != پوچ)
	channel_clear(channel);
    اگر (job != پوچ)
    {
	job->jv_channel = پوچ;
	job_cleanup(job);
    }
    term->tl_job = پوچ;
    اگر (jo != پوچ)
	CloseHandle(jo);
    اگر (term->tl_winpty != پوچ)
	winpty_free(term->tl_winpty);
    term->tl_winpty = پوچ;
    اگر (term->tl_winpty_config != پوچ)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = پوچ;
    اگر (winpty_err != پوچ)
    {
	نویسه *پیام = (نویسه *)utf16_to_enc(
				(short_u *)winpty_error_پیام(winpty_err), پوچ);

	اگر (پیام != پوچ)
	{
	    emsg(پیام);
	    vim_free(پیام);
	}
	winpty_error_free(winpty_err);
    }
    بازگشت FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ایستا صحیح
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	نویسه	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    صحیح		    use_winpty = FALSE;
    صحیح		    use_conpty = FALSE;
    صحیح		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    اگر (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	بازگشت dyn_winpty_init(TRUE);

    اگر (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    اگر (tty_type == NUL)
    {
	اگر (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	وگرنه اگر (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    وگرنه اگر (tty_type == 'w')	// winpty
    {
	اگر (has_winpty)
	    use_winpty = TRUE;
    }
    وگرنه اگر (tty_type == 'c')	// conpty
    {
	اگر (has_conpty)
	    use_conpty = TRUE;
	وگرنه
	    بازگشت dyn_conpty_init(TRUE);
    }

    اگر (use_conpty)
	بازگشت conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    اگر (use_winpty)
	بازگشت winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    بازگشت dyn_winpty_init(TRUE);
}

    ایستا صحیح
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_مقدار;
    HANDLE	    hPipeOut = INVALID_HANDLE_مقدار;
    نویسه	    in_name[80], out_name[80];
    channel_T	    *channel = پوچ;

    اگر (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	بازگشت FAIL;

    vim_snprintf(in_name, اندازه(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, پوچ);
    اگر (hPipeIn == INVALID_HANDLE_مقدار)
	برو failed;

    vim_snprintf(out_name, اندازه(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, پوچ);
    اگر (hPipeOut == INVALID_HANDLE_مقدار)
	برو failed;

    ConnectNamedPipe(hPipeIn, پوچ);
    ConnectNamedPipe(hPipeOut, پوچ);

    term->tl_job = job_alloc();
    اگر (term->tl_job == پوچ)
	برو failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    اگر (channel == پوچ)
	برو failed;
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

    بازگشت OK;

failed:
    اگر (hPipeIn != پوچ)
	CloseHandle(hPipeIn);
    اگر (hPipeOut != پوچ)
	CloseHandle(hPipeOut);
    بازگشت FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ایستا تهی
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    اگر (term->tl_winpty != پوچ)
	winpty_free(term->tl_winpty);
    term->tl_winpty = پوچ;
    اگر (term->tl_winpty_config != پوچ)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = پوچ;
    اگر (term->tl_vterm != پوچ)
	vterm_free(term->tl_vterm);
    term->tl_vterm = پوچ;
}

/*
 * Report the size to the terminal.
 */
    ایستا تهی
term_report_winsize(term_T *term, صحیح rows, صحیح cols)
{
    اگر (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    اگر (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, پوچ);
}

    صحیح
terminal_enabled(تهی)
{
    بازگشت dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#وگرنه

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ایستا صحیح
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	نویسه	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = پوچ;

    اگر (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	بازگشت FAIL;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (term_use_palette())
    {
	اگر (term->tl_palette != پوچ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وگرنه
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    اگر (term->tl_job != پوچ)
	++term->tl_job->jv_refcount;

    بازگشت term->tl_job != پوچ
	&& term->tl_job->jv_channel != پوچ
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    ایستا صحیح
create_pty_only(term_T *term, jobopt_T *opt)
{
    اگر (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	بازگشت FAIL;

    term->tl_job = job_alloc();
    اگر (term->tl_job == پوچ)
	بازگشت FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    بازگشت mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ایستا تهی
term_free_vterm(term_T *term)
{
    اگر (term->tl_vterm != پوچ)
	vterm_free(term->tl_vterm);
    term->tl_vterm = پوچ;
}

/*
 * Report the size to the terminal.
 */
    ایستا تهی
term_report_winsize(term_T *term, صحیح rows, صحیح cols)
{
    // Use an ioctl() to report the new window size to the job.
    اگر (term->tl_job == پوچ || term->tl_job->jv_channel == پوچ)
	بازگشت;

    صحیح fd = -1;
    صحیح part;

    برای (part = PART_OUT; part < PART_شمار; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	اگر (mch_isatty(fd))
	    بشکن;
    }
    اگر (part < PART_شمار && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
