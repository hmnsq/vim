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

#ضمن "vim.h"

#𐡄𐡍 defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ضمن "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
تعريفنوع بنية {
    VTermScreenCellAttrs	attrs;
    حرف			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

تعريفنوع بنية sb_line_S {
    صحيح		sb_cols;	// can differ per line
    صحيح		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_line_T;

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
تعريفنوع بنية _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
بنية terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#𐡄𐡍 defined(FEAT_GUI)
    صحيح		tl_system;	// when non-zero used for :!cmd output
    صحيح		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    صحيح		tl_vterm_size_changed;

    صحيح		tl_normal_mode; // TRUE: Terminal-Normal mode
    صحيح		tl_channel_closing;
    صحيح		tl_channel_closed;
    صحيح		tl_channel_recently_closed; // still need to handle tl_finish

    صحيح		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    فراغ	*tl_winpty_config;
    فراغ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#𐡄𐡍 defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    صحيح		tl_rows;
    صحيح		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحيح		tl_dirty_row_start; // MAX_ROW if nothing dirty
    صحيح		tl_dirty_row_end;   // row below last one to update
    صحيح		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    صحيح		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    صحيح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحيح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحيح		tl_scrollback_snapshot;
    صحيح		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_top_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    صحيح		tl_cursor_visible;
    صحيح		tl_cursor_blink;
    صحيح		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    صحيح		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ساكن term_T *first_term = عدم;

// Terminal active in terminal_loop().
ساكن term_T *in_terminal_loop = عدم;

#ifdef MSWIN
ساكن BOOL has_winpty = FALSE;
ساكن BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define KEY_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    لكل ((term) = first_term; (term) != عدم; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ساكن صحيح term_and_job_init(term_T *term, typval_T *argvar, حرف **argv, jobopt_T *opt, jobopt_T *orig_opt);
ساكن صحيح create_pty_only(term_T *term, jobopt_T *opt);
ساكن فراغ term_report_winsize(term_T *term, صحيح rows, صحيح cols);
ساكن فراغ term_free_vterm(term_T *term);
#ifdef FEAT_GUI
ساكن فراغ update_system_term(term_T *term);
#endif

ساكن فراغ handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ساكن صحيح term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ساكن char_u	*last_set_cursor_color = عدم;
ساكن char_u	*desired_cursor_color = عدم;
ساكن صحيح	last_set_cursor_shape = -1;
ساكن صحيح	desired_cursor_shape = -1;
ساكن صحيح	last_set_cursor_blink = -1;
ساكن صحيح	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ساكن صحيح
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    𐡄𐡍 (lhs_color != عدم && rhs_color != عدم)
	أرجع STRCMP(lhs_color, rhs_color) == 0;
    أرجع lhs_color == عدم && rhs_color == عدم;
}

    ساكن فراغ
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    𐡄𐡍 (cursor_color_equal(*to_color, from_color))
	أرجع;
    vim_free(*to_color);
    *to_color = (from_color == عدم) ? عدم : vim_strsave(from_color);
}

    ساكن char_u *
cursor_color_get(char_u *color)
{
    أرجع (color == عدم) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ساكن صحيح
parse_termwinsize(win_T *wp, صحيح *rows, صحيح *cols)
{
    صحيح	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    𐡄𐡍 (*wp->w_p_tws == NUL)
	أرجع FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    𐡄𐡍 (p == عدم)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((حرف *)wp->w_p_tws);
    *cols = atoi((حرف *)p + 1);
    𐡄𐡍 (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    𐡄𐡍 (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    أرجع minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ساكن فراغ
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    صحيح rows, cols;
    صحيح	minsize;

#ifdef FEAT_GUI
    𐡄𐡍 (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	أرجع;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    𐡄𐡍 (minsize)
    {
	𐡄𐡍 (term->tl_rows < rows)
	    term->tl_rows = rows;
	𐡄𐡍 (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    𐡄𐡍 ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    وإلا 𐡄𐡍 (rows != 0)
	term->tl_rows = rows;
    𐡄𐡍 ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    وإلا 𐡄𐡍 (cols != 0)
	term->tl_cols = cols;

    𐡄𐡍 (!opt->jo_hidden)
    {
	𐡄𐡍 (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	𐡄𐡍 (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	𐡄𐡍 (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((حرف *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_value_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    فراغ
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
    ساكن فراغ
setup_job_options(jobopt_T *opt, صحيح rows, صحيح cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    𐡄𐡍 (!(opt->jo_set & JO_OUT_IO))
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
    𐡄𐡍 (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    𐡄𐡍 ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    𐡄𐡍 ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    ساكن فراغ
term_flush_messages(فراغ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ساكن فراغ
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    𐡄𐡍 (old_curbuf != عدم)
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
	حرف	    **argv,
	jobopt_T    *opt,
	صحيح	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = عدم;
    صحيح		res;
    buf_T	*newbuf;
    صحيح		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    𐡄𐡍 (check_restricted() || check_secure())
	أرجع عدم;
    𐡄𐡍 (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_line_window));
	أرجع عدم;
    }

    𐡄𐡍 ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != عدم
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	أرجع عدم;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    𐡄𐡍 (term == عدم)
	أرجع عدم;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, حجم(sb_line_T), 300);
    ga_init2(&term->tl_scrollback_postponed, حجم(sb_line_T), 300);
    ga_init2(&term->tl_osc_buf, حجم(حرف), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    𐡄𐡍 (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	𐡄𐡍 (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    أرجع عدم;
	}
	𐡄𐡍 (do_ecmd(0, عدم, عدم, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    أرجع عدم;
	}
    }
    وإلا 𐡄𐡍 (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", عدم, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	𐡄𐡍 (buf == عدم || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    أرجع عدم;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    وإلا
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	𐡄𐡍 (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_count = 1;
	}
	𐡄𐡍 (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_count = 1;
	}

	صحيح cmod_split_modified = FALSE;
	𐡄𐡍 (vertical)
	{
	    𐡄𐡍 (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	𐡄𐡍 (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	𐡄𐡍 (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    أرجع عدم;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    𐡄𐡍 (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	𐡄𐡍 (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	𐡄𐡍 (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);

    𐡄𐡍 (opt->jo_term_name != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    وإلا 𐡄𐡍 (argv != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    وإلا
    {
	صحيح	i;
	size_t	len;
	char_u	*cmd, *p;

	𐡄𐡍 (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    𐡄𐡍 (cmd == عدم)
		cmd = (char_u *)"";
	    وإلا 𐡄𐡍 (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	وإلا 𐡄𐡍 (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == عدم
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == عدم)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	لكل (i = 0; p != عدم; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    𐡄𐡍 (i == 0)
		vim_snprintf((حرف *)p, len, "!%s", cmd);
	    وإلا
		vim_snprintf((حرف *)p, len, "!%s (%d)", cmd, i);
	    𐡄𐡍 (buflist_findname(p) == عدم)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		اكسر;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, عدم, عدم, FALSE, curbuf);

    𐡄𐡍 (opt->jo_term_opencmd != عدم)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    𐡄𐡍 (opt->jo_eof_chars != عدم)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, حجم(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    𐡄𐡍 (flags & TERM_START_NOJOB)
	أرجع curbuf;

#𐡄𐡍 defined(FEAT_SESSION)
    // Remember the command for the session file.
    𐡄𐡍 (opt->jo_term_norestore || argv != عدم)
	term->tl_command = vim_strsave((char_u *)"NONE");
    وإلا 𐡄𐡍 (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	𐡄𐡍 (cmd != عدم && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    وإلا 𐡄𐡍 (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ITEMS(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    𐡄𐡍 (s == عدم)
		اكسر;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    𐡄𐡍 (p == عدم)
		اكسر;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	𐡄𐡍 (item == عدم)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	وإلا
	    ga_clear(&ga);
    }
#endif

    𐡄𐡍 (opt->jo_term_kill != عدم)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    𐡄𐡍 (opt->jo_term_api != عدم)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    وإلا
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    𐡄𐡍 (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    𐡄𐡍 (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	𐡄𐡍 (term->tl_palette == عدم)
	{
	    vim_free(term);
	    أرجع عدم;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, حجم(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    𐡄𐡍 (argv == عدم
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != عدم
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    وإلا
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    𐡄𐡍 (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	𐡄𐡍 (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = msg_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	𐡄𐡍 (old_curbuf != عدم)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	وإلا 𐡄𐡍 (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| input_busy)
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
    وإلا
    {
	term_close_buffer(curbuf, old_curbuf);
	أرجع عدم;
    }

    apply_autocmds(EVENT_TERMINALOPEN, عدم, عدم, FALSE, newbuf);
    𐡄𐡍 (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, عدم, عدم, FALSE, newbuf);
    أرجع newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    فراغ
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    صحيح		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = عدم;
    صحيح		scroll_save = msg_scroll;

    msg_scroll = FALSE;		// don't scroll here
    autowrite_all();
    msg_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    طالما (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	𐡄𐡍 (ep != عدم)
	{
	    𐡄𐡍 (ep < p)
		p = ep;
	    وإلا
		ep = عدم;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((صحيح)(p - cmd) == حجم(name) - 1 \
				 && STRNICMP(cmd, name, حجم(name) - 1) == 0)
	𐡄𐡍 (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	وإلا 𐡄𐡍 (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	وإلا 𐡄𐡍 (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	وإلا 𐡄𐡍 (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	وإلا 𐡄𐡍 (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	وإلا 𐡄𐡍 (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	وإلا 𐡄𐡍 (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	وإلا 𐡄𐡍 (OPTARG_HAS("kill") && ep != عدم)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	وإلا 𐡄𐡍 (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    𐡄𐡍 (ep != عدم)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    وإلا
		opt.jo_term_api = عدم;
	}
	وإلا 𐡄𐡍 (OPTARG_HAS("rows") && ep != عدم && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا 𐡄𐡍 (OPTARG_HAS("cols") && ep != عدم && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا 𐡄𐡍 (OPTARG_HAS("eof") && ep != عدم)
	{
	    char_u *buf = عدم;
	    char_u *keys;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    keys = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, عدم);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(keys);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	وإلا 𐡄𐡍 ((صحيح)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != عدم)
	{
	    صحيح tty_type = NUL;

	    p = skiptowhite(cmd);
	    𐡄𐡍 (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    وإلا 𐡄𐡍 (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    وإلا
	    {
		semsg(_(e_invalid_value_for_argument_str), "type");
		اذهب theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	وإلا
	{
	    𐡄𐡍 (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    اذهب theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    𐡄𐡍 (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	𐡄𐡍 (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    𐡄𐡍 (eap->addr_count > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_TOP | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_top = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    𐡄𐡍 (opt_shell && tofree == عدم)
    {
#ifdef UNIX
	حرف	**argv = عدم;
	char_u	*tofree1 = عدم;
	char_u	*tofree2 = عدم;

	// :term ++shell command
	𐡄𐡍 (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(عدم, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	اذهب theend;
#وإلا
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	𐡄𐡍 (newcmd == عدم)
	    اذهب theend;
	tofree = newcmd;
	vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# وإلا
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	اذهب theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, عدم, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ساكن char_u *
get_terminalopt_name(expand_T *xp UNUSED, صحيح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ساكن حرف *(p_termopt_values[]) =
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

    𐡄𐡍 (idx < (صحيح)ARRAY_LENGTH(p_termopt_values))
	أرجع (char_u*)p_termopt_values[idx];
    أرجع عدم;
}

    ساكن char_u *
get_termkill_name(expand_T *xp UNUSED, صحيح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ساكن حرف *(p_termkill_values[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    𐡄𐡍 (idx < (صحيح)ARRAY_LENGTH(p_termkill_values))
	أرجع (char_u*)p_termkill_values[idx];
    أرجع عدم;
}

/*
 * Command-line expansion for :terminal [options]
 */
    صحيح
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحيح	    *numMatches)
{
    𐡄𐡍 (xp->xp_pattern > xp->xp_line && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحيح) = عدم;

	char_u *name_end = xp->xp_pattern - 1;
	𐡄𐡍 (name_end - xp->xp_line >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	𐡄𐡍 (cb != عدم)
	{
	    أرجع ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    numMatches,
		    cb,
		    FALSE);
	}
	أرجع FAIL;
    }
    أرجع ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    numMatches,
	    get_terminalopt_name,
	    FALSE);
}

#𐡄𐡍 defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    صحيح
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحيح	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    𐡄𐡍 (terminal_bufs != عدم && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	حرف	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, حجم(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	𐡄𐡍 (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    𐡄𐡍 (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		أرجع FAIL;
	    أرجع put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    𐡄𐡍 (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	أرجع FAIL;
# ifdef MSWIN
    𐡄𐡍 (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	أرجع FAIL;
# endif
    𐡄𐡍 (term->tl_command != عدم)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	𐡄𐡍 (quoted_command == عدم)
	    أرجع FAIL;

	صحيح ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((حرف *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	𐡄𐡍 (ret)
	    أرجع FAIL;
    }
    𐡄𐡍 (put_eol(fd) != OK)
	أرجع FAIL;

    𐡄𐡍 (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	أرجع FAIL;

    𐡄𐡍 (terminal_bufs != عدم && wp->w_buffer->b_nwindows > 1)
    {
	حرف *hash_key = alloc(NUMBUFLEN);

	vim_snprintf(hash_key, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_key, "terminal session");
    }

    أرجع put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحيح
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    أرجع term != عدم && (term->tl_command == عدم
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    ساكن فراغ
free_scrollback(term_T *term)
{
    صحيح i;

    لكل (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_line_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    لكل (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_line_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ساكن term_T	*terminals_to_free = عدم;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    فراغ
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    𐡄𐡍 (term == عدم)
	أرجع;

    // Unlink the terminal form the list of terminals.
    𐡄𐡍 (first_term == term)
	first_term = term->tl_next;
    وإلا
	لكل (tp = first_term; tp->tl_next != عدم; tp = tp->tl_next)
	    𐡄𐡍 (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		اكسر;
	    }

    𐡄𐡍 (term->tl_job != عدم)
    {
	𐡄𐡍 (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, عدم, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = عدم;
    𐡄𐡍 (in_terminal_loop == term)
	in_terminal_loop = عدم;
}

    فراغ
free_unused_terminals(فراغ)
{
    طالما (terminals_to_free != عدم)
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
	𐡄𐡍 (term->tl_out_fd != عدم)
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
    ساكن ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    صحيح		i;

    لكل (i = 0; i < 3; ++i)
    {
	صحيح fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	𐡄𐡍 (mch_isatty(fd))
	    أرجع parts[i];
    }
#endif
    أرجع PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ساكن فراغ
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    حرف   buf[KEY_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, KEY_BUF_LEN);

    𐡄𐡍 (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (صحيح)curlen, عدم);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ساكن فراغ
term_write_job_output(term_T *term, char_u *msg_arg, size_t len_arg)
{
    char_u	*msg = msg_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    𐡄𐡍 (len > limit)
    {
	char_u *p = msg + len - limit;

	p -= (*mb_head_off)(msg, p);
	len -= p - msg;
	msg = p;
    }

    vterm_input_write(vterm, (حرف *)msg, len);

    // flush vterm buffer when vterm responded to control sequence
    𐡄𐡍 (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ساكن فراغ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    𐡄𐡍 (popup_is_popup(wp))
    {
	wp->w_wrow += popup_top_extra(wp);
	wp->w_wcol += popup_left_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ساكن فراغ
update_cursor(term_T *term, صحيح redraw)
{
    𐡄𐡍 (term->tl_normal_mode)
	أرجع;
#ifdef FEAT_GUI
    𐡄𐡍 (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    وإلا
#endif
    𐡄𐡍 (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    وإلا
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    𐡄𐡍 (redraw)
    {
	aco_save_T	aco;

	𐡄𐡍 (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	𐡄𐡍 (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, عدم, عدم, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ساكن فراغ
scrollbackline_pos_in_buf(term_T *term, صحيح row, linenr_T *lnum, صحيح *start_col, size_t *start_pos)
{
    sb_line_T	*lines = (sb_line_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    صحيح         calc_col = 0;
    صحيح         i;

    𐡄𐡍 (row < 0 || row >= term->tl_scrollback.ga_len)
	أرجع;

    𐡄𐡍 (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	لكل (i = term->tl_scrollback_scrolled + 1; i > 0 && lines[i].continuation; --i)
	{
	    calc_pos += lines[i - 1].sb_bytes;
	    calc_col += lines[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	𐡄𐡍 (lines[i].continuation)
	    ++i;
    }
    وإلا
    {
	i = 1;
	calc_lnum = 1;
    }

    لكل (; i <= row; ++i)
    {
	𐡄𐡍 (!lines[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	وإلا
	{
	    calc_pos += lines[i - 1].sb_bytes;
	    calc_col += lines[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    𐡄𐡍 (start_col)
	*start_col = calc_col;
    𐡄𐡍 (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ساكن فراغ
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, صحيح col, صحيح *row, صحيح *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_line_T	*lines = (sb_line_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    صحيح         calc_col = col;
    linenr_T    l;

    𐡄𐡍 (lnum > buf->b_ml.ml_line_count)
	أرجع;

    𐡄𐡍 (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	طالما (calc_row < term->tl_scrollback.ga_len && lines[calc_row].continuation)
	    ++calc_row;
    }
    وإلا
    {
	calc_row = 0;
	l = 1;
    }

    طالما (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	𐡄𐡍 (!lines[calc_row].continuation)
	    ++l;
    }

    طالما (calc_row + 1 < term->tl_scrollback.ga_len && lines[calc_row + 1].continuation
	    && calc_col >= lines[calc_row].sb_cols)
    {
	calc_col -= lines[calc_row].sb_cols;
	++calc_row;
    }

    𐡄𐡍 (row)
	*row = calc_row;
    𐡄𐡍 (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    فراغ
write_to_term(buf_T *buffer, char_u *msg, channel_T *channel)
{
    size_t	len = STRLEN(msg);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    𐡄𐡍 (term->tl_out_fd != عدم)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحيح)len);
	fwrite(msg, len, 1, term->tl_out_fd);
	أرجع;
    }
#endif

    𐡄𐡍 (term->tl_vterm == عدم)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحيح)len);
	أرجع;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحيح)len);
    cursor_off();
    term_write_job_output(term, msg, len);

#ifdef FEAT_GUI
    𐡄𐡍 (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    وإلا
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    𐡄𐡍 (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	𐡄𐡍 (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#𐡄𐡍 defined(FEAT_TABPANEL)
	    𐡄𐡍 (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    𐡄𐡍 (buffer == curbuf && curbuf->b_term != عدم)
		update_cursor(curbuf->b_term, TRUE);
	}
	وإلا
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ساكن صحيح
term_send_mouse(VTerm *vterm, صحيح button, صحيح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    row = mouse_row - W_WINROW(curwin);
    صحيح		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    𐡄𐡍 (popup_is_popup(curwin))
    {
	row -= popup_top_extra(curwin);
	col -= popup_left_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    𐡄𐡍 (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    أرجع TRUE;
}

ساكن صحيح enter_mouse_col = -1;
ساكن صحيح enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ساكن صحيح
term_mouse_click(VTerm *vterm, صحيح key)
{
#𐡄𐡍 defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ساكن صحيح	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    𐡄𐡍 (state == عدم)
	أرجع FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    𐡄𐡍 (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	بدل (key)
	{
	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		𐡄𐡍 (ignore_drag_release)
		{
		    صحيح save_mouse_col, save_mouse_row;

		    𐡄𐡍 (enter_mouse_col < 0)
			اكسر;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_LEFT, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		𐡄𐡍 (key == K_LEFTRELEASE || key == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		وإلا
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		𐡄𐡍 (clip_star.available)
		{
		    صحيح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(key),
							 &is_click, &is_drag);
		    𐡄𐡍 (mouse_model_popup() && button == MOUSE_LEFT
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_RIGHT;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		اكسر;

	حالة K_MIDDLEMOUSE:
		𐡄𐡍 (clip_star.available)
		    insert_reg('*', TRUE);
		اكسر;
	}
	enter_mouse_col = -1;
	أرجع FALSE;
    }
#endif
    enter_mouse_col = -1;

    بدل (key)
    {
	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); اكسر;
	حالة K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); اكسر;
	حالة K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); اكسر;
	حالة K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); اكسر;
    }
    أرجع TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ساكن صحيح
term_convert_key(term_T *term, صحيح c, صحيح modmask, حرف *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTermKey	    key = VTERM_KEY_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    other = FALSE;

    بدل (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالة K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    𐡄𐡍 (term->tl_conpty)
		key = VTERM_KEY_BACKSPACE;
	    وإلا
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    اكسر;

#ifdef MSWIN
	حالة BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    𐡄𐡍 (term->tl_conpty)
		key = VTERM_KEY_BACKSPACE;
	    اكسر;
#endif

	حالة ESC:		key = VTERM_KEY_ESCAPE; اكسر;
	حالة K_DEL:		key = VTERM_KEY_DEL; اكسر;
	حالة K_DOWN:		key = VTERM_KEY_DOWN; اكسر;
	حالة K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_DOWN; اكسر;
	حالة K_END:		key = VTERM_KEY_END; اكسر;
	حالة K_S_END:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_END; اكسر;
	حالة K_C_END:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_END; اكسر;
	حالة K_F10:		key = VTERM_KEY_FUNCTION(10); اكسر;
	حالة K_F11:		key = VTERM_KEY_FUNCTION(11); اكسر;
	حالة K_F12:		key = VTERM_KEY_FUNCTION(12); اكسر;
	حالة K_F1:		key = VTERM_KEY_FUNCTION(1); اكسر;
	حالة K_F2:		key = VTERM_KEY_FUNCTION(2); اكسر;
	حالة K_F3:		key = VTERM_KEY_FUNCTION(3); اكسر;
	حالة K_F4:		key = VTERM_KEY_FUNCTION(4); اكسر;
	حالة K_F5:		key = VTERM_KEY_FUNCTION(5); اكسر;
	حالة K_F6:		key = VTERM_KEY_FUNCTION(6); اكسر;
	حالة K_F7:		key = VTERM_KEY_FUNCTION(7); اكسر;
	حالة K_F8:		key = VTERM_KEY_FUNCTION(8); اكسر;
	حالة K_F9:		key = VTERM_KEY_FUNCTION(9); اكسر;
	حالة K_HOME:		key = VTERM_KEY_HOME; اكسر;
	حالة K_S_HOME:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_HOME; اكسر;
	حالة K_C_HOME:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_HOME; اكسر;
	حالة K_INS:		key = VTERM_KEY_INS; اكسر;
	حالة K_K0:		key = VTERM_KEY_KP_0; اكسر;
	حالة K_K1:		key = VTERM_KEY_KP_1; اكسر;
	حالة K_K2:		key = VTERM_KEY_KP_2; اكسر;
	حالة K_K3:		key = VTERM_KEY_KP_3; اكسر;
	حالة K_K4:		key = VTERM_KEY_KP_4; اكسر;
	حالة K_K5:		key = VTERM_KEY_KP_5; اكسر;
	حالة K_K6:		key = VTERM_KEY_KP_6; اكسر;
	حالة K_K7:		key = VTERM_KEY_KP_7; اكسر;
	حالة K_K8:		key = VTERM_KEY_KP_8; اكسر;
	حالة K_K9:		key = VTERM_KEY_KP_9; اكسر;
	حالة K_KDEL:		key = VTERM_KEY_DEL; اكسر; // TODO
	حالة K_KDIVIDE:		key = VTERM_KEY_KP_DIVIDE; اكسر;
	حالة K_KEND:		key = VTERM_KEY_KP_1; اكسر; // TODO
	حالة K_KENTER:		key = VTERM_KEY_KP_ENTER; اكسر;
	حالة K_KHOME:		key = VTERM_KEY_KP_7; اكسر; // TODO
	حالة K_KINS:		key = VTERM_KEY_KP_0; اكسر; // TODO
	حالة K_KMINUS:		key = VTERM_KEY_KP_MINUS; اكسر;
	حالة K_KMULTIPLY:	key = VTERM_KEY_KP_MULT; اكسر;
	حالة K_KPAGEDOWN:	key = VTERM_KEY_KP_3; اكسر; // TODO
	حالة K_KPAGEUP:		key = VTERM_KEY_KP_9; اكسر; // TODO
	حالة K_KPLUS:		key = VTERM_KEY_KP_PLUS; اكسر;
	حالة K_KPOINT:		key = VTERM_KEY_KP_PERIOD; اكسر;
	حالة K_LEFT:		key = VTERM_KEY_LEFT; اكسر;
	حالة K_S_LEFT:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_LEFT; اكسر;
	حالة K_C_LEFT:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_LEFT; اكسر;
	حالة K_PAGEDOWN:	key = VTERM_KEY_PAGEDOWN; اكسر;
	حالة K_PAGEUP:		key = VTERM_KEY_PAGEUP; اكسر;
	حالة K_RIGHT:		key = VTERM_KEY_RIGHT; اكسر;
	حالة K_S_RIGHT:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_RIGHT; اكسر;
	حالة K_C_RIGHT:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_RIGHT; اكسر;
	حالة K_UP:		key = VTERM_KEY_UP; اكسر;
	حالة K_S_UP:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_UP; اكسر;
	حالة TAB:		key = VTERM_KEY_TAB; اكسر;
	حالة K_S_TAB:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_TAB; اكسر;

	حالة K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); اكسر;
	حالة K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); اكسر;
	حالة K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); اكسر;
	حالة K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); اكسر;

	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:
	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:
	حالة K_MOUSEMOVE:
	حالة K_MIDDLEMOUSE:
	حالة K_MIDDLEDRAG:
	حالة K_MIDDLERELEASE:
	حالة K_RIGHTMOUSE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:	𐡄𐡍 (!term_mouse_click(vterm, c))
				    أرجع 0;
				other = TRUE;
				اكسر;

	حالة K_X1MOUSE:		/* TODO */ أرجع 0;
	حالة K_X1DRAG:		/* TODO */ أرجع 0;
	حالة K_X1RELEASE:	/* TODO */ أرجع 0;
	حالة K_X2MOUSE:		/* TODO */ أرجع 0;
	حالة K_X2DRAG:		/* TODO */ أرجع 0;
	حالة K_X2RELEASE:	/* TODO */ أرجع 0;

	حالة K_IGNORE:		أرجع 0;
	حالة K_NOP:		أرجع 0;
	حالة K_UNDO:		أرجع 0;
	حالة K_HELP:		أرجع 0;
	حالة K_XF1:		key = VTERM_KEY_FUNCTION(1); اكسر;
	حالة K_XF2:		key = VTERM_KEY_FUNCTION(2); اكسر;
	حالة K_XF3:		key = VTERM_KEY_FUNCTION(3); اكسر;
	حالة K_XF4:		key = VTERM_KEY_FUNCTION(4); اكسر;
	حالة K_SELECT:		أرجع 0;
#ifdef FEAT_GUI
	حالة K_VER_SCROLLBAR:	أرجع 0;
	حالة K_HOR_SCROLLBAR:	أرجع 0;
#endif
#ifdef FEAT_GUI_TABLINE
	حالة K_TABLINE:		أرجع 0;
	حالة K_TABMENU:		أرجع 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	حالة K_F21:		key = VTERM_KEY_FUNCTION(21); اكسر;
#endif
#ifdef FEAT_DND
	حالة K_DROP:		أرجع 0;
#endif
	حالة K_CURSORHOLD:	أرجع 0;
	حالة K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				اكسر;
	حالة K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				اكسر;
    }

    // add modifiers for the typed key
    𐡄𐡍 (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    𐡄𐡍 (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    𐡄𐡍 (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    𐡄𐡍 (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    𐡄𐡍 (key != VTERM_KEY_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_key(vterm, key, mod);
    وإلا 𐡄𐡍 (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    أرجع (صحيح)vterm_output_read(vterm, buf, KEY_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ساكن صحيح
term_job_running_check(term_T *term, صحيح check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    𐡄𐡍 (term == عدم
	|| term->tl_job == عدم
	|| !channel_is_open(term->tl_job->jv_channel))
	أرجع FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    𐡄𐡍 (check_job_status)
	job_status(job);
    أرجع (job->jv_status == JOB_STARTED
	    || (job->jv_channel != عدم && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحيح
term_job_running(term_T *term)
{
    أرجع term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحيح
term_job_running_not_none(term_T *term)
{
    أرجع term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحيح
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    أرجع term != عدم
	&& term->tl_job != عدم
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحيح
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_MSG_SIZE];
    صحيح	ret;

    dialog_msg(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, عدم, buff, 1);
    𐡄𐡍 (ret == VIM_YES)
	أرجع OK;
    وإلا
	أرجع FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    صحيح
term_try_stop_job(buf_T *buf)
{
    صحيح	    count;
    حرف    *how = (حرف *)buf->b_term->tl_kill;

#𐡄𐡍 defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    𐡄𐡍 ((how == عدم || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	𐡄𐡍 (term_confirm_stop(buf) == OK)
	    how = "kill";
	وإلا
	    أرجع FAIL;
    }
#endif
    𐡄𐡍 (how == عدم || *how == NUL)
	أرجع FAIL;

    job_stop(buf->b_term->tl_job, عدم, how);

    // wait for up to a second for the job to die
    لكل (count = 0; count < 100; ++count)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	𐡄𐡍 (!buf_valid(buf)
		|| buf->b_term == عدم
		|| buf->b_term->tl_job == عدم)
	    أرجع OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	𐡄𐡍 (job->jv_status >= JOB_ENDED)
	    أرجع OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    أرجع FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ساكن فراغ
add_scrollback_line_to_buffer(term_T *term, char_u *text, صحيح len, صحيح append)
{
    buf_T	*buf = term->tl_buffer;
    صحيح		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_line_count;

#ifdef MSWIN
    char_u *tmp = text;
    𐡄𐡍 (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = عدم;
	صحيح	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (حرف*)text, len + 1,
							   &ret, &length);
	𐡄𐡍 (ret != عدم)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (حرف **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    𐡄𐡍 (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	𐡄𐡍 (both == عدم)
	    أرجع;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    وإلا
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    𐡄𐡍 (tmp != text)
	vim_free(text);
#endif
    𐡄𐡍 (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    ساكن فراغ
cell2cellattr(ثابت VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ساكن صحيح
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    أرجع a->fg.red == b->fg.red
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
    ساكن صحيح
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, صحيح lnum)
{
    𐡄𐡍 (ga_grow(&term->tl_scrollback, 1) == FAIL)
	أرجع FALSE;

    sb_line_T *line = (sb_line_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    𐡄𐡍 (lnum > 0)
    {
	صحيح i;

	لكل (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *line = *(line - 1);
	    --line;
	}
    }
    line->sb_cols = 0;
    line->sb_bytes = 0;
    line->sb_cells = عدم;
    line->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    أرجع OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ساكن فراغ
cleanup_scrollback(term_T *term)
{
    sb_line_T	*line;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_line_count, FALSE);
    bufline_length = STRLEN(bufline);
    طالما (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	line = (sb_line_T *)gap->ga_data + gap->ga_len - 1;
	𐡄𐡍 (line->sb_bytes < 0 || (size_t)line->sb_bytes > bufline_length)
	    اكسر;
	bufline_length -= line->sb_bytes;
	𐡄𐡍 (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_line_count);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_line_count, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(line->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    𐡄𐡍 (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_line_count, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    𐡄𐡍 (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ساكن فراغ
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحيح		    len;
    صحيح		    lines_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == عدم ? عدم : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    لكل (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	لكل (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    𐡄𐡍 (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    وإلا
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	𐡄𐡍 (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++lines_skipped;
	وإلا
	{
	    طالما (lines_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--lines_skipped;
		𐡄𐡍 (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_line_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    𐡄𐡍 (len == 0)
		p = عدم;
	    وإلا
		p = ALLOC_MULT(cellattr_T, len);
	    𐡄𐡍 ((p != عدم || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحيح	    width;
		ثابت VTermLineInfo *lineinfo;
		sb_line_T   *line = (sb_line_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		لكل (pos.col = 0; pos.col < len; pos.col += width)
		{
		    𐡄𐡍 (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			𐡄𐡍 (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    وإلا
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			𐡄𐡍 (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			𐡄𐡍 (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    صحيح	    i;
			    صحيح	    c;

			    لكل (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		line->sb_cols = len;
		line->sb_bytes = ga.ga_len;
		line->sb_cells = p;
		line->sb_fill_attr = new_fill_attr;
		line->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		𐡄𐡍 (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_line_to_buffer(term, (char_u *)"", 0, 0);
		وإلا
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_line_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    وإلا
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    لكل (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	𐡄𐡍 (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_line_to_buffer(term, (char_u *)"", 0, 0);
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
    ساكن صحيح
for_all_windows_and_curwin(win_T **wp, صحيح *did_curwin)
{
    𐡄𐡍 (*wp == عدم)
	*wp = firstwin;
    وإلا 𐡄𐡍 ((*wp)->w_next != عدم)
	*wp = (*wp)->w_next;
    وإلا 𐡄𐡍 (!*did_curwin)
	*wp = curwin;
    وإلا
	أرجع FALSE;
    𐡄𐡍 (*wp == curwin)
	*did_curwin = TRUE;
    أرجع TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    فراغ
may_move_terminal_to_buffer(term_T *term, صحيح redraw)
{
    𐡄𐡍 (term->tl_vterm == عدم)
	أرجع;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    𐡄𐡍 (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_line_count
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    𐡄𐡍 (redraw)
    {
	win_T	    *wp = عدم;
	صحيح	    did_curwin = FALSE;

	طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    𐡄𐡍 (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_line_count;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		𐡄𐡍 (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    𐡄𐡍 (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#𐡄𐡍 defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    صحيح
term_check_timers(صحيح next_due_arg, proftime_T *now)
{
    term_T  *term;
    صحيح	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	𐡄𐡍 (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_left(&term->tl_timer_due, now);

	    𐡄𐡍 (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    وإلا 𐡄𐡍 (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    أرجع next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ساكن فراغ
set_terminal_mode(term_T *term, صحيح normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    𐡄𐡍 (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    𐡄𐡍 (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ساكن فراغ
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    𐡄𐡍 (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ساكن فراغ
term_enter_normal_mode(فراغ)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    صحيح        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, عدم);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    𐡄𐡍 (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحيح
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    أرجع term != عدم && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    فراغ
term_enter_job_mode(فراغ)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    𐡄𐡍 (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    𐡄𐡍 (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
تعريفنوع تعداد {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_key_state_T;

ساكن reduce_key_state_T  no_reduce_key_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ساكن صحيح
vterm_using_key_protocol(فراغ)
{
    أرجع curbuf->b_term != عدم
	&& curbuf->b_term->tl_vterm != عدم
	&& (vterm_is_modify_other_keys(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    فراغ
check_no_reduce_keys(فراغ)
{
    𐡄𐡍 (no_reduce_key_state != NRKS_CHECK
	    || no_reduce_keys >= 1
	    || curbuf->b_term == عدم
	    || curbuf->b_term->tl_vterm == عدم)
	أرجع;

    𐡄𐡍 (vterm_using_key_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_key_state = NRKS_SET;
	++no_reduce_keys;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ساكن صحيح
term_vgetc(فراغ)
{
    صحيح c;
    صحيح save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    𐡄𐡍 (vterm_using_key_protocol())
    {
	++no_reduce_keys;
	no_reduce_key_state = NRKS_SET;
    }
    وإلا
    {
	no_reduce_key_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    𐡄𐡍 (no_reduce_key_state == NRKS_SET)
	--no_reduce_keys;
    no_reduce_key_state = NRKS_NONE;

    أرجع c;
}

ساكن صحيح	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    صحيح
send_keys_to_term(term_T *term, صحيح c, صحيح modmask, صحيح typed)
{
    حرف	msg[KEY_BUF_LEN];
    size_t	len;
    صحيح		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    بدل (c)
    {
	حالة NUL:
	حالة K_ZERO:
	    𐡄𐡍 (typed)
		stuffcharReadbuff(c);
	    أرجع FAIL;

	حالة K_TABLINE:
	    stuffcharReadbuff(c);
	    أرجع FAIL;

	حالة K_IGNORE:
	حالة K_CANCEL:  // used for :normal when running out of chars
	    أرجع FAIL;

	حالة K_LEFTDRAG:
	حالة K_MIDDLEDRAG:
	حالة K_RIGHTDRAG:
	حالة K_X1DRAG:
	حالة K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:
	حالة K_MOUSEMOVE:
	حالة K_MIDDLEMOUSE:
	حالة K_MIDDLERELEASE:
	حالة K_RIGHTMOUSE:
	حالة K_RIGHTRELEASE:
	حالة K_X1MOUSE:
	حالة K_X1RELEASE:
	حالة K_X2MOUSE:
	حالة K_X2RELEASE:

	حالة K_MOUSEUP:
	حالة K_MOUSEDOWN:
	حالة K_MOUSELEFT:
	حالة K_MOUSERIGHT:
	    {
		صحيح	row = mouse_row;
		صحيح	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		𐡄𐡍 (popup_is_popup(curwin))
		{
		    row -= popup_top_extra(curwin);
		    col -= popup_left_extra(curwin);
		}
#endif
		𐡄𐡍 (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    𐡄𐡍 (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    أرجع FAIL;
		}
	    }
	    اكسر;

	حالة K_COMMAND:
	حالة K_SCRIPT_COMMAND:
	    أرجع do_cmdkey_command(c, 0);
    }
    𐡄𐡍 (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_key(term, c, modmask, msg);
    𐡄𐡍 (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)msg, (صحيح)len, عدم);

    أرجع OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ساكن فراغ
term_paste_register(صحيح prev_c UNUSED)
{
    صحيح		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    صحيح		type;

    𐡄𐡍 (add_to_showcmd(prev_c))
    𐡄𐡍 (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    𐡄𐡍 (!term_use_loop())
	// job finished while waiting for a character
	أرجع;

    // CTRL-W "= prompt for expression to evaluate.
    𐡄𐡍 (c == '=' && get_expr_register() != '=')
	أرجع;
    𐡄𐡍 (!term_use_loop())
	// job finished while waiting for a character
	أرجع;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    𐡄𐡍 (l == عدم)
	أرجع;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ITEMS(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	𐡄𐡍 (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = عدم;
	    صحيح	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (حرف *)s,
		    (صحيح)STRLEN(s), &ret, &length);
	    𐡄𐡍 (ret != عدم)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (حرف **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (صحيح)STRLEN(s), عدم);
#ifdef MSWIN
	𐡄𐡍 (tmp != s)
	    vim_free(s);
#endif

	𐡄𐡍 (item->li_next != عدم || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, عدم);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    صحيح
terminal_is_active(فراغ)
{
    أرجع in_terminal_loop != عدم;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ساكن صحيح
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    𐡄𐡍 (wp != عدم && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    وإلا 𐡄𐡍 (term->tl_highlight_name != عدم)
	name = term->tl_highlight_name;
    وإلا
	name = (char_u*)"Terminal";

    أرجع syn_name2id(name);
}

#𐡄𐡍 defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    ساكن cursorentry_T entry;
    صحيح			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_LEFT ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    𐡄𐡍 (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    𐡄𐡍 (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    𐡄𐡍 (term_bg != INVALCOLOR)
	*fg = term_bg;
    وإلا
	*fg = gui.back_pixel;

    𐡄𐡍 (term->tl_cursor_color == عدم)
    {
	𐡄𐡍 (term_fg != INVALCOLOR)
	    *bg = term_fg;
	وإلا
	    *bg = gui.norm_pixel;
    }
    وإلا
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    أرجع &entry;
}
#endif

    ساكن فراغ
may_output_cursor_props(فراغ)
{
    𐡄𐡍 (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	𐡄𐡍 (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	وإلا
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ساكن فراغ
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    𐡄𐡍 (gui.in_use)
	أرجع;
#endif
    𐡄𐡍 (in_terminal_loop == term)
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
    ساكن فراغ
prepare_restore_cursor_props(فراغ)
{
#ifdef FEAT_GUI
    𐡄𐡍 (gui.in_use)
	أرجع;
#endif
    cursor_color_copy(&desired_cursor_color, عدم);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ساكن صحيح
term_use_loop_check(صحيح check_job_status)
{
    term_T *term = curbuf->b_term;

    أرجع term != عدم
	&& !term->tl_normal_mode
	&& term->tl_vterm != عدم
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحيح
term_use_loop(فراغ)
{
    أرجع term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    فراغ
term_win_entered(فراغ)
{
    term_T *term = curbuf->b_term;

    𐡄𐡍 (term == عدم)
	أرجع;

    𐡄𐡍 (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	𐡄𐡍 (State & MODE_INSERT)
	    stop_insert_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    فراغ
term_focus_change(صحيح in_focus)
{
    term_T *term = curbuf->b_term;

    𐡄𐡍 (term == عدم || term->tl_vterm == عدم)
	أرجع;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    𐡄𐡍 (in_focus)
	vterm_state_focus_in(state);
    وإلا
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ساكن صحيح
raw_c_to_ctrl(صحيح c)
{
    𐡄𐡍 ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	أرجع c & 0x1f;
    أرجع c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ساكن صحيح
ctrl_to_raw_c(صحيح c)
{
    𐡄𐡍 (c < 0x20 && vterm_using_key_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	أرجع c + '@';
    }
    أرجع c;
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
    صحيح
terminal_loop(صحيح blocking)
{
    صحيح		c;
    صحيح		raw_c;
    صحيح		termwinkey = 0;
    صحيح		ret;
#ifdef UNIX
    صحيح		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    صحيح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    𐡄𐡍 (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_key(curwin->w_p_twk, TRUE);
	𐡄𐡍 (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    طالما (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	𐡄𐡍 (curbuf->b_term != عدم && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    طالما (must_redraw != 0)
		𐡄𐡍 (update_screen(0) == FAIL)
		    اكسر;
	𐡄𐡍 (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    اكسر;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	𐡄𐡍 (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    𐡄𐡍 (raw_c != K_IGNORE)
		vungetc(raw_c);
	    اكسر;
	}
	𐡄𐡍 (raw_c == K_IGNORE)
	    استمر;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	𐡄𐡍 (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    𐡄𐡍 (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	𐡄𐡍 (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	𐡄𐡍 ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    صحيح	    prev_c = c;
	    صحيح	    prev_raw_c = raw_c;
	    صحيح	    prev_mod_mask = mod_mask;

	    𐡄𐡍 (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    𐡄𐡍 (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		اكسر;

	    𐡄𐡍 (prev_c == Ctrl_BSL)
	    {
		𐡄𐡍 (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    اذهب theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_keys_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    وإلا 𐡄𐡍 (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    وإلا 𐡄𐡍 (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    وإلا 𐡄𐡍 (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    وإلا 𐡄𐡍 (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		اذهب theend;
	    }
	    وإلا 𐡄𐡍 (c == '"')
	    {
		term_paste_register(prev_c);
		استمر;
	    }
	    وإلا 𐡄𐡍 (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		اذهب theend;
	    }
	}
#ifdef MSWIN
	𐡄𐡍 (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    𐡄𐡍 (MultiByteToWideChar(GetACP(), 0, (حرف*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	𐡄𐡍 (send_keys_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    𐡄𐡍 (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    اذهب theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = عدم;
    𐡄𐡍 (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    𐡄𐡍 (curbuf->b_term != عدم && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    أرجع ret;
}

    ساكن فراغ
may_toggle_cursor(term_T *term)
{
    𐡄𐡍 (in_terminal_loop != term)
	أرجع;

    𐡄𐡍 (term->tl_cursor_visible)
	cursor_on();
    وإلا
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ساكن صحيح
color2index(VTermColor *color, صحيح fg, صحيح *boldp)
{
    صحيح red = color->red;
    صحيح blue = color->blue;
    صحيح green = color->green;

    *boldp = FALSE;

    𐡄𐡍 (VTERM_COLOR_IS_INVALID(color))
	أرجع 0;

    𐡄𐡍 (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	𐡄𐡍 (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    𐡄𐡍 (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    أرجع index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	وإلا 𐡄𐡍 (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    أرجع (color->index & 7) + 1;
	}
	أرجع 0;
    }

    𐡄𐡍 (t_colors >= 256)
    {
	𐡄𐡍 (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ساكن صحيح cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    صحيح i;

	    𐡄𐡍 (red < 5)
		أرجع 17; // 00/00/00
	    𐡄𐡍 (red > 245) // ff/ff/ff
		أرجع 232;
	    لكل (i = 0; i < 23; ++i)
		𐡄𐡍 (red < cutoff[i])
		    أرجع i + 233;
	    أرجع 256;
	}
	{
	    ساكن صحيح cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    صحيح ri, gi, bi;

	    // 216-color cube
	    لكل (ri = 0; ri < 5; ++ri)
		𐡄𐡍 (red < cutoff[ri])
		    اكسر;
	    لكل (gi = 0; gi < 5; ++gi)
		𐡄𐡍 (green < cutoff[gi])
		    اكسر;
	    لكل (bi = 0; bi < 5; ++bi)
		𐡄𐡍 (blue < cutoff[bi])
		    اكسر;
	    أرجع 17 + ri * 36 + gi * 6 + bi;
	}
    }
    أرجع 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ساكن صحيح
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    صحيح attr = 0;

    𐡄𐡍 (cellattrs->bold)
	attr |= HL_BOLD;
    𐡄𐡍 (cellattrs->underline)
	attr |= HL_UNDERLINE;
    𐡄𐡍 (cellattrs->italic)
	attr |= HL_ITALIC;
    𐡄𐡍 (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    𐡄𐡍 (cellattrs->reverse)
	attr |= HL_INVERSE;
    أرجع attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ساكن فراغ
hl2vtermAttr(صحيح attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    𐡄𐡍 (attr & HL_BOLD)
	cell->attrs.bold = 1;
    𐡄𐡍 (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    𐡄𐡍 (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    𐡄𐡍 (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    𐡄𐡍 (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ساكن صحيح
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    صحيح attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    صحيح is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    صحيح is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    𐡄𐡍 (is_default_fg || is_default_bg)
    {
	𐡄𐡍 (wp != عدم && wp->w_hlfwin_id != 0)
	{
	    𐡄𐡍 (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    𐡄𐡍 (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	وإلا
	{
	    𐡄𐡍 (is_default_fg)
		fg = &term->tl_default_color.fg;
	    𐡄𐡍 (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    𐡄𐡍 (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	أرجع get_gui_attr_idx(attr, guifg, guibg);
    }
    وإلا
#endif
#ifdef FEAT_TERMGUICOLORS
    𐡄𐡍 (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	أرجع get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    وإلا
#endif
    {
	صحيح bold = MAYBE;
	صحيح ctermfg = color2index(fg, TRUE, &bold);
	صحيح ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	𐡄𐡍 (bold == TRUE)
	    attr |= HL_BOLD;

	أرجع get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    أرجع 0;
}

    ساكن فراغ
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    𐡄𐡍 (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    ساكن صحيح
handle_damage(VTermRect rect, فراغ *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    أرجع 1;
}

    ساكن فراغ
term_scroll_up(term_T *term, صحيح start_row, صحيح count)
{
    win_T		 *wp = عدم;
    صحيح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحيح			 clear_attr;

    CLEAR_FIELD(attr);

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	𐡄𐡍 (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_lines(wp, start_row, count, FALSE, FALSE, clear_attr);
	}
    }
}

    ساكن صحيح
handle_moverect(VTermRect dest, VTermRect src, فراغ *user)
{
    term_T	*term = (term_T *)user;
    صحيح		count = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    𐡄𐡍 (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	𐡄𐡍 (dest.start_row == 0)
	    term->tl_postponed_scroll += count;
	وإلا
	    term_scroll_up(term, dest.start_row, count);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    أرجع 1;
}

    ساكن صحيح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	صحيح visible,
	فراغ *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = عدم;
    صحيح		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	𐡄𐡍 (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    𐡄𐡍 (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    أرجع 1;
}

    ساكن صحيح
handle_settermprop(
	VTermProp prop,
	VTermValue *value,
	فراغ *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = عدم;

    بدل (prop)
    {
	حالة VTERM_PROP_TITLE:
	    𐡄𐡍 (disable_vterm_title_for_testing)
		اكسر;
	    strval = vim_strnsave((char_u *)value->string.str,
							    value->string.len);
	    𐡄𐡍 (strval == عدم)
		اكسر;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    𐡄𐡍 (*skipwhite(strval) == NUL)
		term->tl_title = عدم;
	    // Same as blank
	    وإلا 𐡄𐡍 (term->tl_arg0_cmd != عدم
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (صحيح)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = عدم;
	    // Empty corrupted data of winpty
	    وإلا 𐡄𐡍 (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = عدم;
#ifdef MSWIN
	    وإلا 𐡄𐡍 (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = عدم;
		صحيح	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(حرف*)value->string.str,
					(صحيح)value->string.len, &ret, &length);
		𐡄𐡍 (ret != عدم)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (حرف**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    وإلا
	    {
		term->tl_title = strval;
		strval = عدم;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    𐡄𐡍 (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    اكسر;

	حالة VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = value->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    اكسر;

	حالة VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = value->boolean;
	    may_set_cursor_props(term);
	    اكسر;

	حالة VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = value->number;
	    may_set_cursor_props(term);
	    اكسر;

	حالة VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)value->string.str,
							    value->string.len);
	    𐡄𐡍 (strval == عدم)
		اكسر;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    اكسر;

	حالة VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = value->boolean;
	    اكسر;

	افتراضي:
	    اكسر;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    أرجع 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ساكن صحيح
handle_resize(صحيح rows, صحيح cols, فراغ *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    𐡄𐡍 (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    وإلا
    {
	FOR_ALL_WINDOWS(wp)
	{
	    𐡄𐡍 (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    أرجع 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ساكن فراغ
limit_scrollback(term_T *term, garray_T *gap, صحيح update_buffer)
{
    𐡄𐡍 (gap->ga_len < term->tl_buffer->b_p_twsl)
	أرجع;

    صحيح	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    صحيح	i;
    sb_line_T *sb_lines = (sb_line_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    لكل (i = 0; i < todo; ++i)
    {
	𐡄𐡍 (update_buffer && (!sb_lines[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_lines[i].sb_cells);
    }
    // Continue until end of wrapped line
    لكل (; todo < gap->ga_len && sb_lines[todo].continuation; ++todo)
	vim_free(sb_lines[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_line_T *)gap->ga_data + todo,
		حجم(sb_line_T) * gap->ga_len);
    𐡄𐡍 (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = عدم;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    𐡄𐡍 (wp->w_buffer == term->tl_buffer)
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
    ساكن صحيح
handle_pushline(صحيح cols, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    صحيح		update_buffer;

    𐡄𐡍 (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    وإلا
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    𐡄𐡍 (ga_grow(gap, 1) == FAIL)
	أرجع 0;

    cellattr_T	*p = عدم;
    صحيح		len = 0;
    صحيح		i;
    صحيح		c;
    صحيح		col;
    صحيح		text_len;
    char_u		*text;
    sb_line_T	*line;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    لكل (i = 0; i < cols; ++i)
	𐡄𐡍 (cells[i].chars[0] != 0)
	    len = i + 1;
	وإلا
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    𐡄𐡍 (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    𐡄𐡍 (p != عدم)
    {
	لكل (col = 0; col < len; col += cells[col].width)
	{
	    𐡄𐡍 (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		اكسر;
	    }
	    لكل (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    𐡄𐡍 (ga_grow(&ga, 1) == FAIL)
    {
	𐡄𐡍 (update_buffer)
	    text = (char_u *)"";
	وإلا
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    وإلا
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    𐡄𐡍 (update_buffer)
	add_scrollback_line_to_buffer(term, text, text_len, continuation);

    line = (sb_line_T *)gap->ga_data + gap->ga_len;
    line->sb_cols = len;
    line->sb_bytes = text_len;
    line->sb_cells = p;
    line->sb_fill_attr = fill_attr;
    line->continuation = (char_u)continuation;
    𐡄𐡍 (update_buffer)
    {
	line->sb_text = عدم;
	++term->tl_scrollback_scrolled;
	𐡄𐡍 (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    وإلا
    {
	line->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    أرجع 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ساكن فراغ
handle_postponed_scrollback(term_T *term)
{
    صحيح i;

    𐡄𐡍 (term->tl_scrollback_postponed.ga_len == 0)
	أرجع;
    ch_log(عدم, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    لكل (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_line_T	*pp_line;
	sb_line_T	*line;

	𐡄𐡍 (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    اكسر;
	pp_line = (sb_line_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_line->sb_text;
	𐡄𐡍 (text == عدم)
	    text = (char_u *)"";
	add_scrollback_line_to_buffer(term, text, (صحيح)STRLEN(text), pp_line->continuation);
	vim_free(pp_line->sb_text);

	line = (sb_line_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	line->sb_cols = pp_line->sb_cols;
	line->sb_bytes = pp_line->sb_bytes;
	line->sb_cells = pp_line->sb_cells;
	line->sb_fill_attr = pp_line->sb_fill_attr;
	line->sb_text = عدم;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	𐡄𐡍 (!pp_line->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ساكن صحيح
handle_bell(فراغ *user UNUSED)
{
    vim_beep(BO_TERM);
    أرجع 0;
}

ساكن VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    عدم,			// sb_pushline
    عدم,			// sb_popline
    عدم,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ساكن صحيح
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    𐡄𐡍 (!term->tl_normal_mode)
    {
	صحيح	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	𐡄𐡍 (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    صحيح		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = عدم;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    𐡄𐡍 (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		𐡄𐡍 (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    وإلا
#endif
	    // If this is the last normal window: exit Vim.
	    𐡄𐡍 (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		أرجع TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(عدم, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    𐡄𐡍 (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		𐡄𐡍 (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		𐡄𐡍 (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		𐡄𐡍 (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    𐡄𐡍 (pwin != عدم)
		popup_close_with_retval(pwin, 0);
#endif
	    أرجع TRUE;
	}
	𐡄𐡍 (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    حرف    *cmd = term->tl_opencmd == عدم
				? "botright sbuf %d"
				: (حرف *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    حرف    *buf = alloc(len);

	    𐡄𐡍 (buf != عدم)
	    {
		ch_log(عدم, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	وإلا
	    ch_log(عدم, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    أرجع FALSE;
}

#𐡄𐡍 defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحيح
may_close_term_popup(فراغ)
{
    𐡄𐡍 (!popup_is_popup(curwin) || curbuf->b_term == عدم
				 || term_job_running_not_none(curbuf->b_term))
	أرجع FAIL;

    win_T *pwin = curwin;

    𐡄𐡍 (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    أرجع OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    فراغ
term_channel_closing(channel_T *ch)
{
    term_T *term;

    لكل (term = first_term; term != عدم; term = term->tl_next)
	𐡄𐡍 (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    فراغ
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    صحيح	    did_one = FALSE;

    لكل (term = first_term; term != عدم; term = next_term)
    {
	next_term = term->tl_next;
	𐡄𐡍 (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    𐡄𐡍 (term->tl_out_fd != عدم)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = عدم;
	    }
#endif

	    𐡄𐡍 (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		استمر;
	    }

	    𐡄𐡍 (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    𐡄𐡍 (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	𐡄𐡍 (term != عدم)
	{
	    𐡄𐡍 (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    فراغ
term_check_channel_closed_recently(فراغ)
{
    term_T *term;
    term_T *next_term;

    لكل (term = first_term; term != عدم; term = next_term)
    {
	next_term = term->tl_next;
	𐡄𐡍 (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    𐡄𐡍 (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ساكن فراغ
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحيح		max_col)
{
    صحيح off = screen_get_current_line_off();

    لكل (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	صحيح		c;

	𐡄𐡍 (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	𐡄𐡍 (c == NUL)
	{
	    ScreenLines[off] = ' ';
	    𐡄𐡍 (enc_utf8)
		ScreenLinesUC[off] = NUL;
	}
	وإلا
	{
	    𐡄𐡍 (enc_utf8)
	    {
		صحيح i;

		// composing chars
		لكل (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    ScreenLinesC[i][off] = cell.chars[i + 1];
		    𐡄𐡍 (cell.chars[i + 1] == 0)
			اكسر;
		}
		𐡄𐡍 (c >= 0x80 || (Screen_mco > 0
					 && ScreenLinesC[0][off] != 0))
		{
		    ScreenLines[off] = ' ';
		    ScreenLinesUC[off] = c;
		}
		وإلا
		{
		    ScreenLines[off] = c;
		    ScreenLinesUC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    وإلا 𐡄𐡍 (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		𐡄𐡍 (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (حرف*)mb, 2, 0, 0) > 1)
		{
		    ScreenLines[off] = mb[0];
		    ScreenLines[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		وإلا
		    ScreenLines[off] = c;
	    }
#endif
	    وإلا
		// This will only store the lower byte of "c".
		ScreenLines[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	𐡄𐡍 (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    𐡄𐡍 (enc_utf8)
	    {
		ScreenLinesUC[off] = NUL;
		ScreenLines[off] = NUL;
	    }
	    وإلا 𐡄𐡍 (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		ScreenLines[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#𐡄𐡍 defined(FEAT_GUI)
    ساكن فراغ
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    𐡄𐡍 (term->tl_vterm == عدم)
	أرجع;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    طالما (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	صحيح save_p_more = p_more;

	p_more = FALSE;
	msg_row = Rows - 1;
	msg_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    لكل (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	𐡄𐡍 (pos.row < term->tl_rows)
	{
	    صحيح max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, عدم, screen, &pos, max_col);
	}
	وإلا
	    pos.col = 0;

	screen_line(curwin, term->tl_toprow + pos.row,
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
    صحيح
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    أرجع term != عدم && term->tl_vterm != عدم && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    فراغ
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحيح		rows, cols;
    صحيح		newrows, newcols;
    صحيح		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    𐡄𐡍 (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	𐡄𐡍 (term->tl_postponed_scroll > 0
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
    لكل (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == عدم ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	𐡄𐡍 (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	𐡄𐡍 (twp == عدم)
	    اكسر;
    }
    𐡄𐡍 (newrows == 99999 || newcols == 99999)
	أرجع; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    𐡄𐡍 (newrows == 0 || newcols == 0)
	أرجع;

    𐡄𐡍 (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	𐡄𐡍 (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    لكل (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	𐡄𐡍 (pos.row < term->tl_rows)
	{
	    صحيح max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	وإلا
	    pos.col = 0;

	screen_line(wp, wp->w_winrow + pos.row
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
    فراغ
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    𐡄𐡍 (term == عدم || term->tl_vterm == عدم || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	أرجع;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحيح
term_is_finished(buf_T *buf)
{
    أرجع buf->b_term != عدم && buf->b_term->tl_vterm == عدم;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحيح
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    أرجع term != عدم && (term->tl_vterm == عدم || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    فراغ
term_change_in_curbuf(فراغ)
{
    term_T *term = curbuf->b_term;

    𐡄𐡍 (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	أرجع;

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
    صحيح
term_get_attr(win_T *wp, linenr_T lnum, صحيح col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_line_T	*line;
    cellattr_T	*cellattr;
    صحيح         sb_line = -1;
    صحيح         sb_col = col;

    𐡄𐡍 (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_line, &sb_col);

    𐡄𐡍 (sb_line < 0)
	cellattr = &term->tl_default_color;
    وإلا
    {
	line = (sb_line_T *)term->tl_scrollback.ga_data + sb_line;
	𐡄𐡍 (sb_col < 0 || sb_col >= line->sb_cols)
	    cellattr = &line->sb_fill_attr;
	وإلا
	    cellattr = line->sb_cells + sb_col;
    }
    أرجع cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ساكن فراغ
cterm_color2vterm(صحيح nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    𐡄𐡍 (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    وإلا
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
    ساكن صحيح
get_vterm_color_from_synid(صحيح id, VTermColor *fg, VTermColor *bg)
{
#𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    𐡄𐡍 (0
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

	𐡄𐡍 (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	𐡄𐡍 (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	𐡄𐡍 (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	وإلا
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	أرجع TRUE;
    }
    وإلا
#endif
    𐡄𐡍 (t_colors >= 16)
    {
	صحيح cterm_fg = -1;
	صحيح cterm_bg = -1;

	𐡄𐡍 (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	𐡄𐡍 (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	𐡄𐡍 (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	وإلا
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	أرجع TRUE;
    }

    أرجع FALSE;
}

    فراغ
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    فراغ
term_update_hlfwin(win_T *wp)
{
    صحيح id = wp->w_hlfwin_id;

    𐡄𐡍 (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
term_update_hlfwin_all(فراغ)
{
    win_T	 *wp = عدم;
    صحيح		 did_curwin = FALSE;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    فراغ
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    صحيح		    fgval, bgval;
    صحيح		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    𐡄𐡍 (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    وإلا
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, عدم);

    𐡄𐡍 (!get_vterm_color_from_synid(id, fg, bg))
    {
#𐡄𐡍 defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحيح tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	𐡄𐡍 (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#𐡄𐡍 defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    𐡄𐡍 (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	وإلا
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	𐡄𐡍 (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#𐡄𐡍 defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    𐡄𐡍 (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	وإلا
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ساكن صحيح
term_use_palette(فراغ)
{
    𐡄𐡍 (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	أرجع TRUE;
    أرجع FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ساكن فراغ
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحيح		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    لكل (; index < 16; index++)
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
    ساكن صحيح
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحيح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    لكل (li = list->lv_first; li != عدم && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	𐡄𐡍 (color_name == عدم)
	    أرجع FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	𐡄𐡍 (guicolor == INVALCOLOR)
	    أرجع FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    𐡄𐡍 (n != 16 || li != عدم)
	أرجع FAIL;

    set_vterm_palette(vterm, rgb);

    أرجع OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ساكن فراغ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE);

    𐡄𐡍 (var == عدم)
	أرجع;

    𐡄𐡍 (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == عدم
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ساكن فراغ
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    صحيح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = عدم;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	𐡄𐡍 (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    أرجع;
	}
    }

    CLEAR_FIELD(ea);

    𐡄𐡍 (opt_item != عدم && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != عدم)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	𐡄𐡍 (p == عدم)
	    p = dict_get_string(dict, "fileformat", FALSE);
	𐡄𐡍 (p != عدم)
	{
	    𐡄𐡍 (check_ff_value(p) == FAIL)
		ch_log(عدم, "Invalid ff argument to drop: %s", p);
	    وإلا
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	𐡄𐡍 (p == عدم)
	    p = dict_get_string(dict, "encoding", FALSE);
	𐡄𐡍 (p != عدم)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    𐡄𐡍 (ea.cmd != عدم)
	    {
		sprintf((حرف *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	𐡄𐡍 (p != عدم)
	    get_bad_opt(p, &ea);

	𐡄𐡍 (dict_has_key(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	𐡄𐡍 (dict_has_key(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	𐡄𐡍 (dict_has_key(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	𐡄𐡍 (dict_has_key(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    𐡄𐡍 (ea.cmd == عدم)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ساكن صحيح
is_permitted_term_api(char_u *func, char_u *pat)
{
    أرجع pat != عدم && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ساكن فراغ
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    𐡄𐡍 (item->li_next == عدم)
    {
	ch_log(channel, "Missing function arguments for call");
	أرجع;
    }
    func = tv_get_string(&item->li_tv);

    𐡄𐡍 (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	أرجع;
    }

    argvars[0].v_type = VAR_NUMBER;
    argvars[0].vval.v_number = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    𐡄𐡍 (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    وإلا
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
    ساكن size_t
url_decode(ثابت حرف *src, ثابت size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    طالما (i < len)
    {
	𐡄𐡍 (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	وإلا
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    أرجع j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ساكن فراغ
sync_shell_dir(garray_T *gap)
{
    صحيح       offset = 7;  // len of "file://" is 7
    حرف      *pos = (حرف *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    طالما (offset < (صحيح)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    𐡄𐡍 (offset >= (صحيح)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	أرجع;
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
    ساكن صحيح
parse_osc(صحيح command, VTermStringFragment frag, فراغ *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == عدم ? عدم
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    𐡄𐡍 (command != 51 && (command != 7 || !p_asd))
	أرجع 0;

    // Concatenate what was received until the final piece is found.
    𐡄𐡍 (ga_grow(gap, (صحيح)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	أرجع 1;
    }
    mch_memmove((حرف *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (صحيح)frag.len;
    𐡄𐡍 (!frag.final)
	أرجع 1;

    ((حرف *)gap->ga_data)[gap->ga_len] = 0;

    𐡄𐡍 (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	أرجع 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = عدم;
    reader.js_used = 0;
    𐡄𐡍 (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != عدم)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	𐡄𐡍 (item == عدم)
	    ch_log(channel, "Missing command");
	وإلا
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    𐡄𐡍 (item == عدم)
		ch_log(channel, "Missing argument for %s", cmd);
	    وإلا 𐡄𐡍 (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    وإلا 𐡄𐡍 (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    وإلا
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    وإلا
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    أرجع 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ساكن صحيح
parse_csi(
	ثابت حرف  *leader UNUSED,
	ثابت long  args[],
	صحيح	    argcount,
	ثابت حرف  *intermed UNUSED,
	حرف	    command,
	فراغ	    *user)
{
    term_T	*term = (term_T *)user;
    حرف	buf[100];
    صحيح		len;
    صحيح		x = 0;
    صحيح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    𐡄𐡍 (command != 't' || argcount != 1 || args[0] != 13)
	أرجع 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#𐡄𐡍 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (فراغ)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	𐡄𐡍 (wp->w_buffer == term->tl_buffer)
	    اكسر;
    𐡄𐡍 (wp != عدم)
    {
#ifdef FEAT_GUI
	𐡄𐡍 (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	وإلا
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
						     (char_u *)buf, len, عدم);
    أرجع 1;
}

ساكن VTermStateFallbacks state_fallbacks = {
    عدم,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    عدم,		// dcs
    عدم,		// apc
    عدم,		// pm
    عدم		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ساكن فراغ *
vterm_malloc(size_t size, فراغ *data UNUSED)
{
    // make sure that the length is not zero
    أرجع alloc_clear(size == 0 ? 1L : size);
}

    ساكن فراغ
vterm_memfree(فراغ *ptr, فراغ *data UNUSED)
{
    vim_free(ptr);
}

ساكن VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ساكن صحيح
create_vterm(term_T *term, صحيح rows, صحيح cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTermValue	    value;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, عدم);
    term->tl_vterm = vterm;
    𐡄𐡍 (vterm == عدم)
	أرجع FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    𐡄𐡍 (state == عدم || screen == عدم)
    {
	vterm_free(vterm);
	أرجع FAIL;
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

    𐡄𐡍 (t_colors < 16)
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
    𐡄𐡍 (GetCaretBlinkTime() == INFINITE)
	value.boolean = 0;
    وإلا
	value.boolean = 1;
#وإلا
    value.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &value);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    أرجع OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ساكن فراغ
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحيح		index;

    لكل (index = 0; index < 16; index++)
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

    ساكن فراغ
term_update_palette(term_T *term)
{
#𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    𐡄𐡍 (term_use_palette()
	    && (term->tl_palette != عدم
		|| find_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE)
								      != عدم))
    {
	𐡄𐡍 (term->tl_palette != عدم)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وإلا
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    وإلا
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    فراغ
term_update_palette_all(فراغ)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	𐡄𐡍 (term->tl_vterm == عدم)
	    استمر;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
term_update_colors_all(فراغ)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	𐡄𐡍 (term->tl_vterm == عدم)
	    استمر;
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
    𐡄𐡍 (term->tl_status_text != عدم)
	أرجع term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    𐡄𐡍 (term->tl_normal_mode)
    {
	𐡄𐡍 (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	وإلا
	    txt = (char_u *)_("Terminal-finished");
    }
    وإلا 𐡄𐡍 (term->tl_title != عدم)
	txt = term->tl_title;
    وإلا 𐡄𐡍 (term_none_open(term))
	txt = (char_u *)_("active");
    وإلا 𐡄𐡍 (term_job_running(term))
	txt = (char_u *)_("running");
    وإلا
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    𐡄𐡍 (term->tl_status_text != عدم)
	vim_snprintf((حرف *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    أرجع term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    فراغ
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    صحيح
set_ref_in_term(صحيح copyID)
{
    صحيح		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    لكل (term = first_term; !abort && term != عدم; term = term->tl_next)
	𐡄𐡍 (term->tl_job != عدم)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ساكن buf_T *
term_get_buf(typval_T *argvars, حرف *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    𐡄𐡍 (buf == عدم || buf->b_term == عدم)
    {
	(فراغ)tv_get_number(&argvars[0]);    // issue errmsg if type error
	ch_log(عدم, "%s: invalid buffer argument", where);
	أرجع عدم;
    }
    أرجع buf;
}

    ساكن فراغ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ساكن فراغ
dump_term_color(FILE *fd, VTermColor *color)
{
    صحيح index;

    𐡄𐡍 (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    وإلا 𐡄𐡍 (color->type == 0)
	// use RGB values
	index = 255;
    وإلا
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (صحيح)color->red, (صحيح)color->green, (صحيح)color->blue,
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
    فراغ
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    صحيح		max_height = 0;
    صحيح		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    𐡄𐡍 (check_restricted() || check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    𐡄𐡍 (term->tl_vterm == عدم)
    {
	emsg(_(e_job_already_finished));
	أرجع;
    }

    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	𐡄𐡍 (check_for_dict_arg(argvars, 2) == FAIL)
	    أرجع;
	d = argvars[2].vval.v_dict;
	𐡄𐡍 (d != عدم)
	{
	    max_height = dict_get_number(d, "rows");
	    max_width = dict_get_number(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (fname == عدم)
	أرجع;
    𐡄𐡍 (mch_stat((حرف *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	أرجع;
    }

    𐡄𐡍 (*fname == NUL || (fd = mch_fopen((حرف *)fname, WRITEBIN)) == عدم)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	أرجع;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    لكل (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	صحيح		repeat = 0;

	لكل (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    صحيح		    same_attr;
	    صحيح		    same_chars = TRUE;
	    صحيح		    i;
	    صحيح		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    𐡄𐡍 (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    لكل (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		صحيح c = cell.chars[i];
		صحيح pc = prev_cell.chars[i];
		صحيح should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		𐡄𐡍 (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		𐡄𐡍 (c != pc)
		    same_chars = FALSE;
		𐡄𐡍 (should_break)
		    اكسر;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    𐡄𐡍 (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    وإلا
	    {
		𐡄𐡍 (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		𐡄𐡍 (cell.chars[0] == NUL)
		    fputs(" ", fd);
		وإلا
		{
		    char_u	charbuf[10];
		    صحيح		len;

		    لكل (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		𐡄𐡍 (cell.width != prev_cell.width || !same_attr)
		{
		    𐡄𐡍 (cell.width == 2)
			fputs("*", fd);
		    وإلا
			fputs("+", fd);

		    𐡄𐡍 (same_attr)
		    {
			fputs("&", fd);
		    }
		    وإلا
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			𐡄𐡍 (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			𐡄𐡍 (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    𐡄𐡍 (cell.width == 2)
		++pos.col;
	}
	𐡄𐡍 (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ساكن فراغ
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ساكن فراغ
append_cell(garray_T *gap, cellattr_T *cell)
{
    𐡄𐡍 (ga_grow(gap, 1) == FAIL)
	أرجع;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ساكن فراغ
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
    ساكن صحيح
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    صحيح		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {عدم, 0};
    صحيح		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    صحيح		    max_cells = 0;
    صحيح		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, حجم(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    لكل (;;)
    {
	𐡄𐡍 (c == EOF)
	    اكسر;
	𐡄𐡍 (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	وإلا 𐡄𐡍 (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    𐡄𐡍 (ga_text.ga_data == عدم)
		dump_is_corrupt(&ga_text);
	    𐡄𐡍 (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_line_T   *line = (sb_line_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		𐡄𐡍 (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		line->sb_cols = ga_cell.ga_len;
		line->sb_bytes = ga_text.ga_len;
		line->sb_cells = ga_cell.ga_data;
		line->sb_fill_attr = term->tl_default_color;
		line->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_line_count, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    وإلا
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	وإلا 𐡄𐡍 (c == '|' || c == '>')
	{
	    صحيح prev_len = ga_text.ga_len;

	    𐡄𐡍 (c == '>')
	    {
		𐡄𐡍 (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    𐡄𐡍 (c != EOF)
		ga_append(&ga_text, c);
	    لكل (;;)
	    {
		c = fgetc(fd);
		𐡄𐡍 (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    اكسر;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    𐡄𐡍 (ga_text.ga_data != عدم)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		𐡄𐡍 (prev_char.string == عدم)
		    prev_char.length = 0;
	    }

	    𐡄𐡍 (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    وإلا 𐡄𐡍 (c == '+' || c == '*')
	    {
		صحيح is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		𐡄𐡍 (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		وإلا 𐡄𐡍 (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    طالما (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    لكل (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			𐡄𐡍 (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			وإلا 𐡄𐡍 (c == '#')
			{
			    صحيح red, green, blue, index = 0, type;

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
			    𐡄𐡍 (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    طالما (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    𐡄𐡍 (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				𐡄𐡍 (index == 0)
				{
				    𐡄𐡍 (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    وإلا
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    وإلا
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    𐡄𐡍 (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    وإلا
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			وإلا
			    dump_is_corrupt(&ga_text);
		    }
		}
		وإلا
		    dump_is_corrupt(&ga_text);
	    }
	    وإلا
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    𐡄𐡍 (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	وإلا 𐡄𐡍 (c == '@')
	{
	    𐡄𐡍 (prev_char.string == عدم)
		dump_is_corrupt(&ga_text);
	    وإلا
	    {
		صحيح count = 0;

		// repeat previous character, get the count
		لكل (;;)
		{
		    c = fgetc(fd);
		    𐡄𐡍 (!SAFE_isdigit(c))
			اكسر;
		    count = count * 10 + (c - '0');
		}

		طالما (count-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	وإلا
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    𐡄𐡍 (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_line_count, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    أرجع max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ساكن char_u *
get_separator(صحيح text_width, char_u *fname)
{
    صحيح	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    صحيح	    fname_size;
    char_u  *p = fname;
    صحيح	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    𐡄𐡍 (textline == عدم)
	أرجع عدم;

    fname_size = vim_strsize(fname);
    𐡄𐡍 (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    وإلا 𐡄𐡍 (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    طالما (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    لكل (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    لكل (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    أرجع textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ساكن فراغ
term_load_dump(typval_T *argvars, typval_T *rettv, صحيح do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = عدم;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = عدم;
    char_u	*fname_tofree = عدم;
    FILE	*fd1;
    FILE	*fd2 = عدم;
    char_u	*textline = عدم;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    𐡄𐡍 (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    𐡄𐡍 (fname1 == عدم || (do_diff && fname2 == عدم))
    {
	emsg(_(e_invalid_argument));
	أرجع;
    }
    fd1 = mch_fopen((حرف *)fname1, READBIN);
    𐡄𐡍 (fd1 == عدم)
    {
	semsg(_(e_cant_read_file_str), fname1);
	أرجع;
    }
    𐡄𐡍 (do_diff)
    {
	fd2 = mch_fopen((حرف *)fname2, READBIN);
	𐡄𐡍 (fd2 == عدم)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    أرجع;
	}
    }

    init_job_options(&opt);
    𐡄𐡍 (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	اذهب theend;

    𐡄𐡍 (opt.jo_term_name == عدم)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	𐡄𐡍 (fname_tofree != عدم)
	{
	    vim_snprintf((حرف *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    𐡄𐡍 (opt.jo_bufnr_buf != عدم)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	𐡄𐡍 (wp == عدم)
	    semsg(_(e_invalid_argument_str), "bufnr");
	وإلا
	{
	    buf = curbuf;
	    طالما (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    وإلا
	// Create a new terminal window.
	buf = term_start(&argvars[0], عدم, &opt, TERM_START_NOJOB);

    𐡄𐡍 (buf != عدم && buf->b_term != عدم)
    {
	صحيح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	صحيح		width;
	صحيح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_number = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	𐡄𐡍 (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	𐡄𐡍 (!do_diff)
	    اذهب theend;

	term->tl_top_diff_rows = curbuf->b_ml.ml_line_count;

	textline = get_separator(width, fname1);
	𐡄𐡍 (textline == عدم)
	    اذهب theend;
	𐡄𐡍 (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_line_count, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	𐡄𐡍 (textline == عدم)
	    اذهب theend;
	𐡄𐡍 (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_line_count, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_line_count;
	width2 = read_dump_file(fd2, &cursor_pos2);
	𐡄𐡍 (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    𐡄𐡍 (textline == عدم)
		اذهب theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_line_count - bot_lnum;

	لكل (lnum = 1; lnum <= term->tl_top_diff_rows; ++lnum)
	{
	    𐡄𐡍 (lnum + bot_lnum > curbuf->b_ml.ml_line_count)
	    {
		// bottom part has fewer rows, fill with "-"
		لكل (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    وإلا
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحيح	col;
		sb_line_T   *sb_line = (sb_line_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_line + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_line + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		𐡄𐡍 (line1 == عدم)
		    اكسر;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		لكل (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    صحيح len1 = utfc_ptr2len(p1);
		    صحيح len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    𐡄𐡍 (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    وإلا 𐡄𐡍 (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    وإلا 𐡄𐡍 (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    وإلا 𐡄𐡍 (cellattr1 != عدم && cellattr2 != عدم)
		    {
			𐡄𐡍 ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			وإلا 𐡄𐡍 (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			وإلا 𐡄𐡍 (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			وإلا 𐡄𐡍 (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		طالما (col < width)
		{
		    𐡄𐡍 (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    وإلا 𐡄𐡍 (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    وإلا
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    𐡄𐡍 (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_top_diff_rows) == OK)
		ml_append(term->tl_top_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	طالما (lnum + bot_lnum <= curbuf->b_ml.ml_line_count)
	{
	    // bottom part has more rows, fill with "+"
	    لكل (i = 0; i < width; ++i)
		textline[i] = '+';
	    𐡄𐡍 (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_top_diff_rows) == OK)
		ml_append(term->tl_top_diff_rows + lnum, textline, 0, FALSE);
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
    𐡄𐡍 (fd2 != عدم)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    صحيح
term_swap_diff(فراغ)
{
    term_T	*term = curbuf->b_term;
    linenr_T	line_count;
    linenr_T	top_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_line_T	*sb_line;

    𐡄𐡍 (term == عدم
	    || !term_is_finished(curbuf)
	    || term->tl_top_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	أرجع FAIL;

    line_count = curbuf->b_ml.ml_line_count;
    top_rows = term->tl_top_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = line_count - bot_rows;
    sb_line = (sb_line_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    لكل (lnum = 1; lnum <= top_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	𐡄𐡍 (p == عدم)
	    أرجع OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    لكل (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	𐡄𐡍 (p == عدم)
	    أرجع OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    𐡄𐡍 (p == عدم)
	أرجع OK;
    ml_append(line_count - top_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(line_count - top_rows));
    𐡄𐡍 (p == عدم)
	أرجع OK;
    ml_delete(line_count - top_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    𐡄𐡍 (top_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	لكل (lnum = 0; lnum < top_rows; ++lnum)
	{
	    sb_line_T	temp;

	    temp = *(sb_line + lnum);
	    *(sb_line + lnum) = *(sb_line + bot_start + lnum);
	    *(sb_line + bot_start + lnum) = temp;
	}
    }
    وإلا
    {
	size_t		size = حجم(sb_line_T) * term->tl_scrollback.ga_len;
	sb_line_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	𐡄𐡍 (temp != عدم)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    حجم(sb_line_T) * bot_rows);
	    mch_memmove((sb_line_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + top_rows,
		    حجم(sb_line_T) * (line_count - top_rows - bot_rows));
	    mch_memmove((sb_line_T *)term->tl_scrollback.ga_data
						       + line_count - top_rows,
		    temp,
		    حجم(sb_line_T) * top_rows);
	    vim_free(temp);
	}
    }

    term->tl_top_diff_rows = bot_rows;
    term->tl_bot_diff_rows = top_rows;

    update_screen(UPD_NOT_VALID);
    أرجع OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    فراغ
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    فراغ
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    فراغ
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    rettv->vval.v_number = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    فراغ
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    صحيح	    attr;
    size_t  i;
    char_u  *name;

    ساكن بنية {
	حرف	    *name;
	صحيح	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    𐡄𐡍 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    attr = tv_get_number(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (name == عدم)
	أرجع;

    𐡄𐡍 (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    لكل (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	𐡄𐡍 (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_number = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    اكسر;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    فراغ
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getcursor()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_number(l, term->tl_cursor_pos.row + 1);
    list_append_number(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    𐡄𐡍 (d == عدم)
	أرجع;

    dict_add_number(d, "visible", term->tl_cursor_visible);
    dict_add_number(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_number(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    𐡄𐡍 (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    فراغ
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getjob()");
    𐡄𐡍 (buf == عدم)
    {
	𐡄𐡍 (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = عدم;
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NULL;
	}
	أرجع;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    𐡄𐡍 (rettv->vval.v_job != عدم)
	++rettv->vval.v_job->jv_refcount;
}

    ساكن صحيح
get_row_number(typval_T *tv, term_T *term)
{
    𐡄𐡍 (tv->v_type == VAR_STRING
	    && tv->vval.v_string != عدم
	    && STRCMP(tv->vval.v_string, ".") == 0)
	أرجع term->tl_cursor_pos.row;
    أرجع (صحيح)tv_get_number(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    فراغ
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    صحيح		    row;

    rettv->v_type = VAR_STRING;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_getline()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    row = get_row_number(&argvars[1], term);

    𐡄𐡍 (term->tl_vterm == عدم)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	صحيح	  sb_row = term->tl_scrollback_scrolled + row;
	sb_line_T *line;

	𐡄𐡍 (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    أرجع;
	line = (sb_line_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, عدم, &offset);

	// vterm is finished, get the text from the buffer
	𐡄𐡍 (lnum > 0 && lnum <= buf->b_ml.ml_line_count)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    𐡄𐡍 (STRLEN(p) >= offset + line->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, line->sb_bytes);
	}
    }
    وإلا
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	صحيح		len;
	char_u		*p;

	𐡄𐡍 (row < 0 || row >= term->tl_rows)
	    أرجع;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	𐡄𐡍 (p == عدم)
	    أرجع;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (حرف *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    فراغ
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getscrolled()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    rettv->vval.v_number = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    فراغ
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getsize()");
    𐡄𐡍 (buf == عدم)
	أرجع;

    l = rettv->vval.v_list;
    list_append_number(l, buf->b_term->tl_rows);
    list_append_number(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    فراغ
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_setsize()");
    𐡄𐡍 (buf == عدم)
    {
	emsg(_(e_not_terminal_buffer));
	أرجع;
    }
    𐡄𐡍 (buf->b_term->tl_vterm == عدم)
	أرجع;
    term = buf->b_term;
    rows = tv_get_number(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_number(&argvars[2]);
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
    فراغ
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getstatus()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;

    𐡄𐡍 (term_job_running(term))
	STRCPY(val, "running");
    وإلا
	STRCPY(val, "finished");
    𐡄𐡍 (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    فراغ
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_gettitle()");
    𐡄𐡍 (buf == عدم)
	أرجع;

    𐡄𐡍 (buf->b_term->tl_title != عدم)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    فراغ
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = عدم;
    صحيح		num = 0;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
	num = tv_get_bool(&argvars[1]);

    بدل (num)
    {
	حالة 0:
	    𐡄𐡍 (buf->b_term->tl_job != عدم)
		p = buf->b_term->tl_job->jv_tty_out;
	    اكسر;
	حالة 1:
	    𐡄𐡍 (buf->b_term->tl_job != عدم)
		p = buf->b_term->tl_job->jv_tty_in;
	    اكسر;
	افتراضي:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    أرجع;
    }
    𐡄𐡍 (p != عدم)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    فراغ
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL || first_term == عدم)
	أرجع;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	𐡄𐡍 (tp->tl_buffer != عدم)
	    𐡄𐡍 (list_append_number(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		أرجع;
}

/*
 * "term_scrape(buf, row)" function
 */
    فراغ
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = عدم;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_line_T	    *line;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_scrape()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_number(&argvars[1], term);

    𐡄𐡍 (term->tl_vterm != عدم)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	𐡄𐡍 (screen == عدم)  // can't really happen
	    أرجع;
	p = عدم;
	line = عدم;
    }
    وإلا
    {
	صحيح	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, عدم, &offset);

	𐡄𐡍 (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_line_count)
	    أرجع;

	line = (sb_line_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	𐡄𐡍 (STRLEN(p) < offset + line->sb_bytes)
	    أرجع;

	p += offset;
    }

    لكل (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	صحيح		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	صحيح		i;

	𐡄𐡍 (screen == عدم)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    𐡄𐡍 (pos.col >= line->sb_cols)
		اكسر;
	    cellattr = line->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	وإلا
	{
	    VTermScreenCell cell;

	    𐡄𐡍 (vterm_screen_get_cell(screen, pos, &cell) == 0)
		اكسر;

	    mbslen = 0;
	    لكل (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		𐡄𐡍 (cell.chars[i] == 0)
		    اكسر;
		mbslen += (*utf_char2bytes)((صحيح)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	𐡄𐡍 (dcell == عدم)
	    اكسر;
	𐡄𐡍 (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    اكسر;
	}

	dict_add_string_len(dcell, "chars", mbs, (صحيح)mbslen);

	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (صحيح)rgblen);
	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (صحيح)rgblen);

	dict_add_number(dcell, "attr",
				      cell2attr(term, عدم, &attrs, &fg, &bg));
	dict_add_number(dcell, "width", width);

	++pos.col;
	𐡄𐡍 (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    فراغ
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*msg;
    term_T	*term;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_sendkeys()");
    𐡄𐡍 (buf == عدم)
	أرجع;

    msg = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (msg == عدم)
	أرجع;
    term = buf->b_term;
    𐡄𐡍 (term->tl_vterm == عدم)
	أرجع;

    طالما (*msg != NUL)
    {
	صحيح c;

	𐡄𐡍 (*msg == K_SPECIAL && msg[1] != NUL && msg[2] != NUL)
	{
	    c = TO_SPECIAL(msg[1], msg[2]);
	    msg += 3;
	}
	وإلا
	{
	    c = PTR2CHAR(msg);
	    msg += MB_CPTR2LEN(msg);
	}
	send_keys_to_term(term, c, 0, FALSE);
    }
}

#𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    فراغ
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحيح		index;
    list_T	*list;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    buf = term_get_buf(argvars, "term_getansicolors()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    𐡄𐡍 (term->tl_vterm == عدم)
	أرجع;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    لكل (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((حرف *)hexbuf, حجم(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	𐡄𐡍 (list_append_string(list, hexbuf, (صحيح)hexbuflen) == FAIL)
	    أرجع;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    فراغ
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    صحيح		n = 0;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_setansicolors()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    𐡄𐡍 (term->tl_vterm == عدم)
	أرجع;

    𐡄𐡍 (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	أرجع;

    𐡄𐡍 (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_value_for_argument_str), "\"colors\"");
	أرجع;
    }

    𐡄𐡍 (term->tl_palette == عدم)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    𐡄𐡍 (term->tl_palette == عدم)
	أرجع;

    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	𐡄𐡍 (color_name == عدم)
	    أرجع;

	guicolor = GUI_GET_COLOR(color_name);
	𐡄𐡍 (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    أرجع;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    فراغ
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_setapi()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (api != عدم)
	term->tl_api = vim_strsave(api);
    وإلا
	term->tl_api = عدم;
}

/*
 * "term_setrestore(buf, command)" function
 */
    فراغ
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#𐡄𐡍 defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_setrestore()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (cmd != عدم)
	term->tl_command = vim_strsave(cmd);
    وإلا
	term->tl_command = عدم;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    فراغ
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_setkill()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (how != عدم)
	term->tl_kill = vim_strsave(how);
    وإلا
	term->tl_kill = عدم;
}

/*
 * "term_start(command, options)" function
 */
    فراغ
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    init_job_options(&opt);
    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	أرجع;

    buf = term_start(&argvars[0], عدم, &opt, 0);

    𐡄𐡍 (buf != عدم && buf->b_term != عدم)
	rettv->vval.v_number = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    فراغ
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    𐡄𐡍 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	أرجع;

    buf = term_get_buf(argvars, "term_wait()");
    𐡄𐡍 (buf == عدم)
	أرجع;
    𐡄𐡍 (buf->b_term->tl_job == عدم)
    {
	ch_log(عدم, "term_wait(): no job to wait for");
	أرجع;
    }
    𐡄𐡍 (buf->b_term->tl_job->jv_channel == عدم)
	// channel is closed, nothing to do
	أرجع;

    // Get the job status, this will detect a job that finished.
    𐡄𐡍 (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(عدم, "term_wait(): waiting for channel to close");
	طالما (buf->b_term != عدم && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    𐡄𐡍 (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		اكسر;
	    𐡄𐡍 (buf->b_term == عدم || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		اكسر;
	}

	term_flush_messages();
    }
    وإلا
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_number(&argvars[1]);
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
    فراغ
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	𐡄𐡍 (term->tl_job == ch->ch_job)
	{
	    𐡄𐡍 (term->tl_eof_chars != عدم)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(صحيح)STRLEN(term->tl_eof_chars), عدم);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, عدم);
	    }
#ifdef MSWIN
	    وإلا
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, عدم);
#endif
	}
}

#𐡄𐡍 defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    أرجع term != عدم ? term->tl_job : عدم;
}
#endif

#𐡄𐡍 defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
فراغ (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ساكن صحيح
dyn_conpty_init(صحيح verbose)
{
    ساكن HMODULE	hKerneldll = عدم;
    صحيح			i;
    ساكن بنية
    {
	حرف	*name;
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
	{عدم, عدم}
    };

    𐡄𐡍 (!has_conpty_working())
    {
	𐡄𐡍 (verbose)
	    emsg(_(e_conpty_is_not_available));
	أرجع FAIL;
    }

    // No need to initialize twice.
    𐡄𐡍 (hKerneldll)
	أرجع OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    لكل (i = 0; conpty_entry[i].name != عدم
					&& conpty_entry[i].ptr != عدم; ++i)
    {
	𐡄𐡍 ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == عدم)
	{
	    𐡄𐡍 (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	حرف	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cmd_wchar_copy = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    job_T	    *job = عدم;
    HANDLE	    jo = عدم;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = عدم;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = عدم;
    HANDLE	    o_theirs = عدم;
    HANDLE	    i_ours = عدم;
    HANDLE	    o_ours = عدم;
    حرف	    *errmsg = عدم;

    ga_init2(&ga_cmd, حجم(حرف*), 20);
    ga_init2(&ga_env, حجم(حرف*), 20);

    𐡄𐡍 (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    وإلا 𐡄𐡍 (argvar->v_type == VAR_LIST)
    {
	𐡄𐡍 (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    اذهب failed;
	cmd = ga_cmd.ga_data;
    }
    𐡄𐡍 (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	اذهب failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, عدم);

    𐡄𐡍 (cmd_wchar != عدم)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    𐡄𐡍 (cmd_wchar == عدم)
	اذهب failed;
    𐡄𐡍 (opt->jo_cwd != عدم)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, عدم);
	𐡄𐡍 (cwd_wchar == عدم)
	    اذهب failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    𐡄𐡍 (!CreatePipe(&i_theirs, &i_ours, عدم, 0))
	اذهب failed;
    𐡄𐡍 (!CreatePipe(&o_ours, &o_theirs, عدم, 0))
	اذهب failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    𐡄𐡍 (FAILED(hr))
	اذهب failed;

    term->tl_siex.StartupInfo.cb = حجم(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.hStdInput = INVALID_HANDLE_VALUE;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_VALUE;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_VALUE;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(عدم, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    𐡄𐡍 (!term->tl_siex.lpAttributeList)
	اذهب failed;
    𐡄𐡍 (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	اذهب failed;
    𐡄𐡍 (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    حجم(HPCON), عدم, عدم))
	اذهب failed;

    channel = add_channel();
    𐡄𐡍 (channel == عدم)
	اذهب failed;

    job = job_alloc();
    𐡄𐡍 (job == عدم)
	اذهب failed;
    𐡄𐡍 (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    𐡄𐡍 (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    𐡄𐡍 (!CreateProcessW(عدم, cmd_wchar_copy, عدم, عدم, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	اذهب failed;
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

    jo = CreateJobObject(عدم, عدم);
    𐡄𐡍 (jo == عدم)
	اذهب failed;

    𐡄𐡍 (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    𐡄𐡍 (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	اذهب failed;

# 𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    𐡄𐡍 (term_use_palette())
    {
	𐡄𐡍 (term->tl_palette != عدم)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وإلا
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
    𐡄𐡍 (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((حرف *)fname, WRITEBIN);
	𐡄𐡍 (term->tl_out_fd == عدم)
	    semsg(_(e_cant_open_file_str), fname);
    }

    أرجع OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    𐡄𐡍 (channel != عدم)
	channel_clear(channel);
    𐡄𐡍 (job != عدم)
    {
	job->jv_channel = عدم;
	job_cleanup(job);
    }
    term->tl_job = عدم;
    𐡄𐡍 (jo != عدم)
	CloseHandle(jo);

    𐡄𐡍 (term->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = عدم;
    𐡄𐡍 (o_theirs != عدم)
	CloseHandle(o_theirs);
    𐡄𐡍 (o_ours != عدم)
	CloseHandle(o_ours);
    𐡄𐡍 (i_ours != عدم)
	CloseHandle(i_ours);
    𐡄𐡍 (i_theirs != عدم)
	CloseHandle(i_theirs);
    𐡄𐡍 (term->tl_conpty != عدم)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = عدم;
    // Propagate errors that occur in CreateProcess
    𐡄𐡍 (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    أرجع FAIL;
}

    ساكن فراغ
conpty_term_report_winsize(term_T *term, صحيح rows, صحيح cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    ساكن فراغ
term_free_conpty(term_T *term)
{
    𐡄𐡍 (term->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = عدم;
    𐡄𐡍 (term->tl_conpty != عدم)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = عدم;
}

    صحيح
use_conpty(فراغ)
{
    أرجع has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

فراغ* (*winpty_config_new)(UINT64, فراغ*);
فراغ* (*winpty_open)(فراغ*, فراغ*);
فراغ* (*winpty_spawn_config_new)(UINT64, فراغ*, LPCWSTR, فراغ*, فراغ*, فراغ*);
BOOL (*winpty_spawn)(فراغ*, فراغ*, HANDLE*, HANDLE*, DWORD*, فراغ*);
فراغ (*winpty_config_set_mouse_mode)(فراغ*, صحيح);
فراغ (*winpty_config_set_initial_size)(فراغ*, صحيح, صحيح);
LPCWSTR (*winpty_conin_name)(فراغ*);
LPCWSTR (*winpty_conout_name)(فراغ*);
LPCWSTR (*winpty_conerr_name)(فراغ*);
فراغ (*winpty_free)(فراغ*);
فراغ (*winpty_config_free)(فراغ*);
فراغ (*winpty_spawn_config_free)(فراغ*);
فراغ (*winpty_error_free)(فراغ*);
LPCWSTR (*winpty_error_msg)(فراغ*);
BOOL (*winpty_set_size)(فراغ*, صحيح, صحيح, فراغ*);
HANDLE (*winpty_agent_process)(فراغ*);

# define WINPTY_DLL "winpty.dll"

ساكن HINSTANCE hWinPtyDLL = عدم;

    ساكن صحيح
dyn_winpty_init(صحيح verbose)
{
    صحيح i;
    ساكن بنية
    {
	حرف	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_msg},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{عدم, عدم}
    };

    // No need to initialize twice.
    𐡄𐡍 (hWinPtyDLL)
	أرجع OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    𐡄𐡍 (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((حرف *)p_winptydll);
    𐡄𐡍 (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    𐡄𐡍 (!hWinPtyDLL)
    {
	𐡄𐡍 (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	أرجع FAIL;
    }
    لكل (i = 0; winpty_entry[i].name != عدم
					 && winpty_entry[i].ptr != عدم; ++i)
    {
	𐡄𐡍 ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == عدم)
	{
	    𐡄𐡍 (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	حرف	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    job_T	    *job = عدم;
    DWORD	    error;
    HANDLE	    jo = عدم;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    فراغ	    *winpty_err = عدم;
    فراغ	    *spawn_config = عدم;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = عدم;

    ga_init2(&ga_cmd, حجم(حرف*), 20);
    ga_init2(&ga_env, حجم(حرف*), 20);

    𐡄𐡍 (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    وإلا 𐡄𐡍 (argvar->v_type == VAR_LIST)
    {
	𐡄𐡍 (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    اذهب failed;
	cmd = ga_cmd.ga_data;
    }
    𐡄𐡍 (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	اذهب failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, عدم);
    ga_clear(&ga_cmd);
    𐡄𐡍 (cmd_wchar == عدم)
	اذهب failed;
    𐡄𐡍 (opt->jo_cwd != عدم)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, عدم);
	𐡄𐡍 (cwd_wchar == عدم)
	    اذهب failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    𐡄𐡍 (term->tl_winpty_config == عدم)
	اذهب failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    𐡄𐡍 (term->tl_winpty == عدم)
	اذهب failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    عدم,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    𐡄𐡍 (spawn_config == عدم)
	اذهب failed;

    channel = add_channel();
    𐡄𐡍 (channel == عدم)
	اذهب failed;

    job = job_alloc();
    𐡄𐡍 (job == عدم)
	اذهب failed;
    𐡄𐡍 (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    𐡄𐡍 (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    𐡄𐡍 (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	اذهب failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(عدم, عدم);
    𐡄𐡍 (jo == عدم)
	اذهب failed;

    𐡄𐡍 (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    𐡄𐡍 (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	اذهب failed;

# 𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    𐡄𐡍 (term_use_palette())
    {
	𐡄𐡍 (term->tl_palette != عدم)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وإلا
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
	    (short_u *)winpty_conin_name(term->tl_winpty), عدم);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), عدم);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    𐡄𐡍 (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((حرف *)fname, WRITEBIN);
	𐡄𐡍 (term->tl_out_fd == عدم)
	    semsg(_(e_cant_open_file_str), fname);
    }

    أرجع OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    𐡄𐡍 (spawn_config != عدم)
	winpty_spawn_config_free(spawn_config);
    𐡄𐡍 (channel != عدم)
	channel_clear(channel);
    𐡄𐡍 (job != عدم)
    {
	job->jv_channel = عدم;
	job_cleanup(job);
    }
    term->tl_job = عدم;
    𐡄𐡍 (jo != عدم)
	CloseHandle(jo);
    𐡄𐡍 (term->tl_winpty != عدم)
	winpty_free(term->tl_winpty);
    term->tl_winpty = عدم;
    𐡄𐡍 (term->tl_winpty_config != عدم)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = عدم;
    𐡄𐡍 (winpty_err != عدم)
    {
	حرف *msg = (حرف *)utf16_to_enc(
				(short_u *)winpty_error_msg(winpty_err), عدم);

	𐡄𐡍 (msg != عدم)
	{
	    emsg(msg);
	    vim_free(msg);
	}
	winpty_error_free(winpty_err);
    }
    أرجع FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ساكن صحيح
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    صحيح		    use_winpty = FALSE;
    صحيح		    use_conpty = FALSE;
    صحيح		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    𐡄𐡍 (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	أرجع dyn_winpty_init(TRUE);

    𐡄𐡍 (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    𐡄𐡍 (tty_type == NUL)
    {
	𐡄𐡍 (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	وإلا 𐡄𐡍 (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    وإلا 𐡄𐡍 (tty_type == 'w')	// winpty
    {
	𐡄𐡍 (has_winpty)
	    use_winpty = TRUE;
    }
    وإلا 𐡄𐡍 (tty_type == 'c')	// conpty
    {
	𐡄𐡍 (has_conpty)
	    use_conpty = TRUE;
	وإلا
	    أرجع dyn_conpty_init(TRUE);
    }

    𐡄𐡍 (use_conpty)
	أرجع conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    𐡄𐡍 (use_winpty)
	أرجع winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    أرجع dyn_winpty_init(TRUE);
}

    ساكن صحيح
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_VALUE;
    HANDLE	    hPipeOut = INVALID_HANDLE_VALUE;
    حرف	    in_name[80], out_name[80];
    channel_T	    *channel = عدم;

    𐡄𐡍 (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	أرجع FAIL;

    vim_snprintf(in_name, حجم(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, عدم);
    𐡄𐡍 (hPipeIn == INVALID_HANDLE_VALUE)
	اذهب failed;

    vim_snprintf(out_name, حجم(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, عدم);
    𐡄𐡍 (hPipeOut == INVALID_HANDLE_VALUE)
	اذهب failed;

    ConnectNamedPipe(hPipeIn, عدم);
    ConnectNamedPipe(hPipeOut, عدم);

    term->tl_job = job_alloc();
    𐡄𐡍 (term->tl_job == عدم)
	اذهب failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    𐡄𐡍 (channel == عدم)
	اذهب failed;
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

    أرجع OK;

failed:
    𐡄𐡍 (hPipeIn != عدم)
	CloseHandle(hPipeIn);
    𐡄𐡍 (hPipeOut != عدم)
	CloseHandle(hPipeOut);
    أرجع FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    𐡄𐡍 (term->tl_winpty != عدم)
	winpty_free(term->tl_winpty);
    term->tl_winpty = عدم;
    𐡄𐡍 (term->tl_winpty_config != عدم)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = عدم;
    𐡄𐡍 (term->tl_vterm != عدم)
	vterm_free(term->tl_vterm);
    term->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
term_report_winsize(term_T *term, صحيح rows, صحيح cols)
{
    𐡄𐡍 (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    𐡄𐡍 (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, عدم);
}

    صحيح
terminal_enabled(فراغ)
{
    أرجع dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#وإلا

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ساكن صحيح
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = عدم;

    𐡄𐡍 (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	أرجع FAIL;

# 𐡄𐡍 defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    𐡄𐡍 (term_use_palette())
    {
	𐡄𐡍 (term->tl_palette != عدم)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	وإلا
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    𐡄𐡍 (term->tl_job != عدم)
	++term->tl_job->jv_refcount;

    أرجع term->tl_job != عدم
	&& term->tl_job->jv_channel != عدم
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    ساكن صحيح
create_pty_only(term_T *term, jobopt_T *opt)
{
    𐡄𐡍 (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	أرجع FAIL;

    term->tl_job = job_alloc();
    𐡄𐡍 (term->tl_job == عدم)
	أرجع FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    أرجع mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
term_free_vterm(term_T *term)
{
    𐡄𐡍 (term->tl_vterm != عدم)
	vterm_free(term->tl_vterm);
    term->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
term_report_winsize(term_T *term, صحيح rows, صحيح cols)
{
    // Use an ioctl() to report the new window size to the job.
    𐡄𐡍 (term->tl_job == عدم || term->tl_job->jv_channel == عدم)
	أرجع;

    صحيح fd = -1;
    صحيح part;

    لكل (part = PART_OUT; part < PART_COUNT; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	𐡄𐡍 (mch_isatty(fd))
	    اكسر;
    }
    𐡄𐡍 (part < PART_COUNT && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
