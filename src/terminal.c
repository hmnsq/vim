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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#அந்தர்பாவ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
வகைவரை கட்டமைப்பு {
    VTermScreenCellAttrs	attrs;
    எழுத்து			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

வகைவரை கட்டமைப்பு sb_வரி_S {
    முழு		sb_cols;	// can differ per line
    முழு		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_வரி_T;

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
வகைவரை கட்டமைப்பு _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
கட்டமைப்பு terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#எனில் defined(FEAT_GUI)
    முழு		tl_system;	// when non-zero used for :!cmd output
    முழு		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    முழு		tl_vterm_size_changed;

    முழு		tl_normal_mode; // TRUE: Terminal-Normal mode
    முழு		tl_channel_closing;
    முழு		tl_channel_closed;
    முழு		tl_channel_recently_closed; // still need to handle tl_finish

    முழு		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    வெறுமை	*tl_winpty_config;
    வெறுமை	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#எனில் defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    முழு		tl_rows;
    முழு		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    முழு		tl_dirty_row_start; // MAX_ROW if nothing dirty
    முழு		tl_dirty_row_end;   // row below last one to update
    முழு		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    முழு		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    முழு		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    முழு		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    முழு		tl_scrollback_snapshot;
    முழு		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_உச்சி_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    முழு		tl_cursor_visible;
    முழு		tl_cursor_blink;
    முழு		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    முழு		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
நிலையான term_T *first_term = வெற்று;

// Terminal active in terminal_loop().
நிலையான term_T *in_terminal_loop = வெற்று;

#ifdef MSWIN
நிலையான BOOL has_winpty = FALSE;
நிலையான BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define சாவி_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    க்கு ((term) = first_term; (term) != வெற்று; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
நிலையான முழு term_and_job_init(term_T *term, typval_T *argvar, எழுத்து **argv, jobopt_T *opt, jobopt_T *orig_opt);
நிலையான முழு create_pty_only(term_T *term, jobopt_T *opt);
நிலையான வெறுமை term_report_winsize(term_T *term, முழு rows, முழு cols);
நிலையான வெறுமை term_free_vterm(term_T *term);
#ifdef FEAT_GUI
நிலையான வெறுமை update_system_term(term_T *term);
#endif

நிலையான வெறுமை handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
நிலையான முழு term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
நிலையான char_u	*last_set_cursor_color = வெற்று;
நிலையான char_u	*desired_cursor_color = வெற்று;
நிலையான முழு	last_set_cursor_shape = -1;
நிலையான முழு	desired_cursor_shape = -1;
நிலையான முழு	last_set_cursor_blink = -1;
நிலையான முழு	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    நிலையான முழு
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    எனில் (lhs_color != வெற்று && rhs_color != வெற்று)
	திரும்பு STRCMP(lhs_color, rhs_color) == 0;
    திரும்பு lhs_color == வெற்று && rhs_color == வெற்று;
}

    நிலையான வெறுமை
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    எனில் (cursor_color_equal(*to_color, from_color))
	திரும்பு;
    vim_free(*to_color);
    *to_color = (from_color == வெற்று) ? வெற்று : vim_strsave(from_color);
}

    நிலையான char_u *
cursor_color_get(char_u *color)
{
    திரும்பு (color == வெற்று) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    நிலையான முழு
parse_termwinsize(win_T *wp, முழு *rows, முழு *cols)
{
    முழு	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    எனில் (*wp->w_p_tws == NUL)
	திரும்பு FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    எனில் (p == வெற்று)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((எழுத்து *)wp->w_p_tws);
    *cols = atoi((எழுத்து *)p + 1);
    எனில் (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    எனில் (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    திரும்பு minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    நிலையான வெறுமை
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    முழு rows, cols;
    முழு	minsize;

#ifdef FEAT_GUI
    எனில் (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	திரும்பு;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    எனில் (minsize)
    {
	எனில் (term->tl_rows < rows)
	    term->tl_rows = rows;
	எனில் (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    எனில் ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    இல்லையெனில் எனில் (rows != 0)
	term->tl_rows = rows;
    எனில் ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    இல்லையெனில் எனில் (cols != 0)
	term->tl_cols = cols;

    எனில் (!opt->jo_hidden)
    {
	எனில் (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	எனில் (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	எனில் (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((எழுத்து *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_மதிப்பு_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    வெறுமை
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
    நிலையான வெறுமை
setup_job_options(jobopt_T *opt, முழு rows, முழு cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    எனில் (!(opt->jo_set & JO_OUT_IO))
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
    எனில் (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    எனில் ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    எனில் ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    நிலையான வெறுமை
term_flush_messages(வெறுமை)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    நிலையான வெறுமை
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    எனில் (old_curbuf != வெற்று)
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
	எழுத்து	    **argv,
	jobopt_T    *opt,
	முழு	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = வெற்று;
    முழு		res;
    buf_T	*newbuf;
    முழு		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    எனில் (check_restricted() || check_secure())
	திரும்பு வெற்று;
    எனில் (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_வரி_window));
	திரும்பு வெற்று;
    }

    எனில் ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != வெற்று
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != வெற்று
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	திரும்பு வெற்று;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    எனில் (term == வெற்று)
	திரும்பு வெற்று;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, அளவு(sb_வரி_T), 300);
    ga_init2(&term->tl_scrollback_postponed, அளவு(sb_வரி_T), 300);
    ga_init2(&term->tl_osc_buf, அளவு(எழுத்து), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    எனில் (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	எனில் (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    திரும்பு வெற்று;
	}
	எனில் (do_ecmd(0, வெற்று, வெற்று, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில் எனில் (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", வெற்று, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	எனில் (buf == வெற்று || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    திரும்பு வெற்று;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    இல்லையெனில்
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	எனில் (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_மொத்தம் = 1;
	}
	எனில் (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_மொத்தம் = 1;
	}

	முழு cmod_split_modified = FALSE;
	எனில் (vertical)
	{
	    எனில் (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	எனில் (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	எனில் (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    திரும்பு வெற்று;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    எனில் (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	எனில் (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	எனில் (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, வெற்று, வெற்று, FALSE, curbuf);

    எனில் (opt->jo_term_name != வெற்று)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    இல்லையெனில் எனில் (argv != வெற்று)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    இல்லையெனில்
    {
	முழு	i;
	size_t	len;
	char_u	*cmd, *p;

	எனில் (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    எனில் (cmd == வெற்று)
		cmd = (char_u *)"";
	    இல்லையெனில் எனில் (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	இல்லையெனில் எனில் (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == வெற்று
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == வெற்று)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	க்கு (i = 0; p != வெற்று; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    எனில் (i == 0)
		vim_snprintf((எழுத்து *)p, len, "!%s", cmd);
	    இல்லையெனில்
		vim_snprintf((எழுத்து *)p, len, "!%s (%d)", cmd, i);
	    எனில் (buflist_findname(p) == வெற்று)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		நிறுத்து;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, வெற்று, வெற்று, FALSE, curbuf);

    எனில் (opt->jo_term_opencmd != வெற்று)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    எனில் (opt->jo_eof_chars != வெற்று)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, அளவு(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    எனில் (flags & TERM_START_NOJOB)
	திரும்பு curbuf;

#எனில் defined(FEAT_SESSION)
    // Remember the command for the session file.
    எனில் (opt->jo_term_norestore || argv != வெற்று)
	term->tl_command = vim_strsave((char_u *)"NONE");
    இல்லையெனில் எனில் (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	எனில் (cmd != வெற்று && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    இல்லையெனில் எனில் (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != வெற்று
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_பொருட்கள்(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    எனில் (s == வெற்று)
		நிறுத்து;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    எனில் (p == வெற்று)
		நிறுத்து;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	எனில் (item == வெற்று)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	இல்லையெனில்
	    ga_clear(&ga);
    }
#endif

    எனில் (opt->jo_term_kill != வெற்று)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    எனில் (opt->jo_term_api != வெற்று)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    இல்லையெனில்
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    எனில் (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    எனில் (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	எனில் (term->tl_palette == வெற்று)
	{
	    vim_free(term);
	    திரும்பு வெற்று;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, அளவு(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    எனில் (argv == வெற்று
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != வெற்று
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    இல்லையெனில்
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    எனில் (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	எனில் (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = செய்தி_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	எனில் (old_curbuf != வெற்று)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	இல்லையெனில் எனில் (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| உள்ளீடு_busy)
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
    இல்லையெனில்
    {
	term_close_buffer(curbuf, old_curbuf);
	திரும்பு வெற்று;
    }

    apply_autocmds(EVENT_TERMINALOPEN, வெற்று, வெற்று, FALSE, newbuf);
    எனில் (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, வெற்று, வெற்று, FALSE, newbuf);
    திரும்பு newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    வெறுமை
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    முழு		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = வெற்று;
    முழு		scroll_save = செய்தி_scroll;

    செய்தி_scroll = FALSE;		// don't scroll here
    autowrite_all();
    செய்தி_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    போது (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	எனில் (ep != வெற்று)
	{
	    எனில் (ep < p)
		p = ep;
	    இல்லையெனில்
		ep = வெற்று;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((முழு)(p - cmd) == அளவு(name) - 1 \
				 && STRNICMP(cmd, name, அளவு(name) - 1) == 0)
	எனில் (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	இல்லையெனில் எனில் (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	இல்லையெனில் எனில் (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	இல்லையெனில் எனில் (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	இல்லையெனில் எனில் (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	இல்லையெனில் எனில் (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	இல்லையெனில் எனில் (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	இல்லையெனில் எனில் (OPTARG_HAS("kill") && ep != வெற்று)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	இல்லையெனில் எனில் (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    எனில் (ep != வெற்று)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    இல்லையெனில்
		opt.jo_term_api = வெற்று;
	}
	இல்லையெனில் எனில் (OPTARG_HAS("rows") && ep != வெற்று && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((எழுத்து *)ep + 1);
	    p = skiptowhite(cmd);
	}
	இல்லையெனில் எனில் (OPTARG_HAS("cols") && ep != வெற்று && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((எழுத்து *)ep + 1);
	    p = skiptowhite(cmd);
	}
	இல்லையெனில் எனில் (OPTARG_HAS("eof") && ep != வெற்று)
	{
	    char_u *buf = வெற்று;
	    char_u *சாவிகள்;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    சாவிகள் = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, வெற்று);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(சாவிகள்);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	இல்லையெனில் எனில் ((முழு)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != வெற்று)
	{
	    முழு tty_type = NUL;

	    p = skiptowhite(cmd);
	    எனில் (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    இல்லையெனில் எனில் (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    இல்லையெனில்
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str), "type");
		செல் theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	இல்லையெனில்
	{
	    எனில் (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    செல் theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    எனில் (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	எனில் (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    எனில் (eap->addr_மொத்தம் > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_உச்சி | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_உச்சி = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    எனில் (opt_shell && tofree == வெற்று)
    {
#ifdef UNIX
	எழுத்து	**argv = வெற்று;
	char_u	*tofree1 = வெற்று;
	char_u	*tofree2 = வெற்று;

	// :term ++shell command
	எனில் (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(வெற்று, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	செல் theend;
#இல்லையெனில்
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	எனில் (newcmd == வெற்று)
	    செல் theend;
	tofree = newcmd;
	vim_snprintf((எழுத்து *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# இல்லையெனில்
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	செல் theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, வெற்று, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    நிலையான char_u *
get_terminalopt_name(expand_T *xp UNUSED, முழு idx)
{
    // Note: Keep this in sync with ex_terminal.
    நிலையான எழுத்து *(p_termopt_மதிப்புகள்[]) =
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

    எனில் (idx < (முழு)ARRAY_LENGTH(p_termopt_மதிப்புகள்))
	திரும்பு (char_u*)p_termopt_மதிப்புகள்[idx];
    திரும்பு வெற்று;
}

    நிலையான char_u *
get_termkill_name(expand_T *xp UNUSED, முழு idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    நிலையான எழுத்து *(p_termkill_மதிப்புகள்[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    எனில் (idx < (முழு)ARRAY_LENGTH(p_termkill_மதிப்புகள்))
	திரும்பு (char_u*)p_termkill_மதிப்புகள்[idx];
    திரும்பு வெற்று;
}

/*
 * Command-line expansion for :terminal [options]
 */
    முழு
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	முழு	    *இலக்கம்_Matches)
{
    எனில் (xp->xp_pattern > xp->xp_வரி && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, முழு) = வெற்று;

	char_u *name_end = xp->xp_pattern - 1;
	எனில் (name_end - xp->xp_வரி >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	எனில் (cb != வெற்று)
	{
	    திரும்பு ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    இலக்கம்_Matches,
		    cb,
		    FALSE);
	}
	திரும்பு FAIL;
    }
    திரும்பு ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    இலக்கம்_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#எனில் defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    முழு
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    மாறிலி முழு	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    எனில் (terminal_bufs != வெற்று && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	எழுத்து	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, அளவு(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	எனில் (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    எனில் (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		திரும்பு FAIL;
	    திரும்பு put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    எனில் (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	திரும்பு FAIL;
# ifdef MSWIN
    எனில் (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	திரும்பு FAIL;
# endif
    எனில் (term->tl_command != வெற்று)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	எனில் (quoted_command == வெற்று)
	    திரும்பு FAIL;

	முழு ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((எழுத்து *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	எனில் (ret)
	    திரும்பு FAIL;
    }
    எனில் (put_eol(fd) != OK)
	திரும்பு FAIL;

    எனில் (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	திரும்பு FAIL;

    எனில் (terminal_bufs != வெற்று && wp->w_buffer->b_nwindows > 1)
    {
	எழுத்து *hash_சாவி = alloc(NUMBUFLEN);

	vim_snprintf(hash_சாவி, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_சாவி, "terminal session");
    }

    திரும்பு put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    முழு
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    திரும்பு term != வெற்று && (term->tl_command == வெற்று
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    நிலையான வெறுமை
free_scrollback(term_T *term)
{
    முழு i;

    க்கு (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_வரி_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    க்கு (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_வரி_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
நிலையான term_T	*terminals_to_free = வெற்று;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    வெறுமை
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    எனில் (term == வெற்று)
	திரும்பு;

    // Unlink the terminal form the list of terminals.
    எனில் (first_term == term)
	first_term = term->tl_next;
    இல்லையெனில்
	க்கு (tp = first_term; tp->tl_next != வெற்று; tp = tp->tl_next)
	    எனில் (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		நிறுத்து;
	    }

    எனில் (term->tl_job != வெற்று)
    {
	எனில் (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, வெற்று, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = வெற்று;
    எனில் (in_terminal_loop == term)
	in_terminal_loop = வெற்று;
}

    வெறுமை
free_unused_terminals(வெறுமை)
{
    போது (terminals_to_free != வெற்று)
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
	எனில் (term->tl_out_fd != வெற்று)
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
    நிலையான ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    முழு		i;

    க்கு (i = 0; i < 3; ++i)
    {
	முழு fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	எனில் (mch_isatty(fd))
	    திரும்பு parts[i];
    }
#endif
    திரும்பு PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    நிலையான வெறுமை
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    எழுத்து   buf[சாவி_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, சாவி_BUF_LEN);

    எனில் (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (முழு)curlen, வெற்று);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    நிலையான வெறுமை
term_write_job_output(term_T *term, char_u *செய்தி_arg, size_t len_arg)
{
    char_u	*செய்தி = செய்தி_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    எனில் (len > limit)
    {
	char_u *p = செய்தி + len - limit;

	p -= (*mb_head_off)(செய்தி, p);
	len -= p - செய்தி;
	செய்தி = p;
    }

    vterm_உள்ளீடு_write(vterm, (எழுத்து *)செய்தி, len);

    // flush vterm buffer when vterm responded to control sequence
    எனில் (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    நிலையான வெறுமை
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    எனில் (popup_is_popup(wp))
    {
	wp->w_wrow += popup_உச்சி_extra(wp);
	wp->w_wcol += popup_இடது_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    இல்லையெனில்
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    நிலையான வெறுமை
update_cursor(term_T *term, முழு redraw)
{
    எனில் (term->tl_normal_mode)
	திரும்பு;
#ifdef FEAT_GUI
    எனில் (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    இல்லையெனில்
#endif
    எனில் (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    இல்லையெனில்
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    எனில் (redraw)
    {
	aco_save_T	aco;

	எனில் (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	எனில் (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, வெற்று, வெற்று, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    நிலையான வெறுமை
scrollbackline_pos_in_buf(term_T *term, முழு row, linenr_T *lnum, முழு *start_col, size_t *start_pos)
{
    sb_வரி_T	*வரிகள் = (sb_வரி_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    முழு         calc_col = 0;
    முழு         i;

    எனில் (row < 0 || row >= term->tl_scrollback.ga_len)
	திரும்பு;

    எனில் (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	க்கு (i = term->tl_scrollback_scrolled + 1; i > 0 && வரிகள்[i].continuation; --i)
	{
	    calc_pos += வரிகள்[i - 1].sb_bytes;
	    calc_col += வரிகள்[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	எனில் (வரிகள்[i].continuation)
	    ++i;
    }
    இல்லையெனில்
    {
	i = 1;
	calc_lnum = 1;
    }

    க்கு (; i <= row; ++i)
    {
	எனில் (!வரிகள்[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	இல்லையெனில்
	{
	    calc_pos += வரிகள்[i - 1].sb_bytes;
	    calc_col += வரிகள்[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    எனில் (start_col)
	*start_col = calc_col;
    எனில் (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    நிலையான வெறுமை
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, முழு col, முழு *row, முழு *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_வரி_T	*வரிகள் = (sb_வரி_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    முழு         calc_col = col;
    linenr_T    l;

    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு;

    எனில் (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	போது (calc_row < term->tl_scrollback.ga_len && வரிகள்[calc_row].continuation)
	    ++calc_row;
    }
    இல்லையெனில்
    {
	calc_row = 0;
	l = 1;
    }

    போது (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	எனில் (!வரிகள்[calc_row].continuation)
	    ++l;
    }

    போது (calc_row + 1 < term->tl_scrollback.ga_len && வரிகள்[calc_row + 1].continuation
	    && calc_col >= வரிகள்[calc_row].sb_cols)
    {
	calc_col -= வரிகள்[calc_row].sb_cols;
	++calc_row;
    }

    எனில் (row)
	*row = calc_row;
    எனில் (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    வெறுமை
write_to_term(buf_T *buffer, char_u *செய்தி, channel_T *channel)
{
    size_t	len = STRLEN(செய்தி);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    எனில் (term->tl_out_fd != வெற்று)
    {
	ch_log(channel, "Writing %d bytes to output file", (முழு)len);
	fwrite(செய்தி, len, 1, term->tl_out_fd);
	திரும்பு;
    }
#endif

    எனில் (term->tl_vterm == வெற்று)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (முழு)len);
	திரும்பு;
    }
    ch_log(channel, "writing %d bytes to terminal", (முழு)len);
    cursor_off();
    term_write_job_output(term, செய்தி, len);

#ifdef FEAT_GUI
    எனில் (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    இல்லையெனில்
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    எனில் (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	எனில் (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#எனில் defined(FEAT_TABPANEL)
	    எனில் (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    எனில் (buffer == curbuf && curbuf->b_term != வெற்று)
		update_cursor(curbuf->b_term, TRUE);
	}
	இல்லையெனில்
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    நிலையான முழு
term_send_mouse(VTerm *vterm, முழு button, முழு pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    முழு		    row = mouse_row - W_WINROW(curwin);
    முழு		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    எனில் (popup_is_popup(curwin))
    {
	row -= popup_உச்சி_extra(curwin);
	col -= popup_இடது_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    எனில் (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    திரும்பு TRUE;
}

நிலையான முழு enter_mouse_col = -1;
நிலையான முழு enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    நிலையான முழு
term_mouse_click(VTerm *vterm, முழு சாவி)
{
#எனில் defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    நிலையான முழு	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    எனில் (state == வெற்று)
	திரும்பு FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    எனில் (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	தேர்வு (சாவி)
	{
	நிலை K_LEFTDRAG:
	நிலை K_LEFTRELEASE:
	நிலை K_RIGHTDRAG:
	நிலை K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		எனில் (ignore_drag_release)
		{
		    முழு save_mouse_col, save_mouse_row;

		    எனில் (enter_mouse_col < 0)
			நிறுத்து;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_இடது, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	நிலை K_LEFTMOUSE:
	நிலை K_RIGHTMOUSE:
		எனில் (சாவி == K_LEFTRELEASE || சாவி == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		இல்லையெனில்
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		எனில் (clip_star.available)
		{
		    முழு	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(சாவி),
							 &is_click, &is_drag);
		    எனில் (mouse_model_popup() && button == MOUSE_இடது
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_வலது;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		நிறுத்து;

	நிலை K_MIDDLEMOUSE:
		எனில் (clip_star.available)
		    செருகு_reg('*', TRUE);
		நிறுத்து;
	}
	enter_mouse_col = -1;
	திரும்பு FALSE;
    }
#endif
    enter_mouse_col = -1;

    தேர்வு (சாவி)
    {
	நிலை K_LEFTMOUSE:
	நிலை K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); நிறுத்து;
	நிலை K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); நிறுத்து;
	நிலை K_LEFTRELEASE:
	நிலை K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); நிறுத்து;
	நிலை K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); நிறுத்து;
	நிலை K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); நிறுத்து;
	நிலை K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); நிறுத்து;
	நிலை K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); நிறுத்து;
	நிலை K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); நிறுத்து;
	நிலை K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); நிறுத்து;
	நிலை K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); நிறுத்து;
    }
    திரும்பு TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    நிலையான முழு
term_convert_சாவி(term_T *term, முழு c, முழு modmask, எழுத்து *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_சாவி	    சாவி = VTERM_சாவி_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    முழு		    other = FALSE;

    தேர்வு (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	நிலை K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    எனில் (term->tl_conpty)
		சாவி = VTERM_சாவி_BACKSPACE;
	    இல்லையெனில்
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    நிறுத்து;

#ifdef MSWIN
	நிலை BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    எனில் (term->tl_conpty)
		சாவி = VTERM_சாவி_BACKSPACE;
	    நிறுத்து;
#endif

	நிலை ESC:		சாவி = VTERM_சாவி_ESCAPE; நிறுத்து;
	நிலை K_DEL:		சாவி = VTERM_சாவி_DEL; நிறுத்து;
	நிலை K_DOWN:		சாவி = VTERM_சாவி_DOWN; நிறுத்து;
	நிலை K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_DOWN; நிறுத்து;
	நிலை K_END:		சாவி = VTERM_சாவி_END; நிறுத்து;
	நிலை K_S_END:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_END; நிறுத்து;
	நிலை K_C_END:		mod = VTERM_MOD_CTRL;
				சாவி = VTERM_சாவி_END; நிறுத்து;
	நிலை K_F10:		சாவி = VTERM_சாவி_FUNCTION(10); நிறுத்து;
	நிலை K_F11:		சாவி = VTERM_சாவி_FUNCTION(11); நிறுத்து;
	நிலை K_F12:		சாவி = VTERM_சாவி_FUNCTION(12); நிறுத்து;
	நிலை K_F1:		சாவி = VTERM_சாவி_FUNCTION(1); நிறுத்து;
	நிலை K_F2:		சாவி = VTERM_சாவி_FUNCTION(2); நிறுத்து;
	நிலை K_F3:		சாவி = VTERM_சாவி_FUNCTION(3); நிறுத்து;
	நிலை K_F4:		சாவி = VTERM_சாவி_FUNCTION(4); நிறுத்து;
	நிலை K_F5:		சாவி = VTERM_சாவி_FUNCTION(5); நிறுத்து;
	நிலை K_F6:		சாவி = VTERM_சாவி_FUNCTION(6); நிறுத்து;
	நிலை K_F7:		சாவி = VTERM_சாவி_FUNCTION(7); நிறுத்து;
	நிலை K_F8:		சாவி = VTERM_சாவி_FUNCTION(8); நிறுத்து;
	நிலை K_F9:		சாவி = VTERM_சாவி_FUNCTION(9); நிறுத்து;
	நிலை K_HOME:		சாவி = VTERM_சாவி_HOME; நிறுத்து;
	நிலை K_S_HOME:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_HOME; நிறுத்து;
	நிலை K_C_HOME:		mod = VTERM_MOD_CTRL;
				சாவி = VTERM_சாவி_HOME; நிறுத்து;
	நிலை K_INS:		சாவி = VTERM_சாவி_INS; நிறுத்து;
	நிலை K_K0:		சாவி = VTERM_சாவி_KP_0; நிறுத்து;
	நிலை K_K1:		சாவி = VTERM_சாவி_KP_1; நிறுத்து;
	நிலை K_K2:		சாவி = VTERM_சாவி_KP_2; நிறுத்து;
	நிலை K_K3:		சாவி = VTERM_சாவி_KP_3; நிறுத்து;
	நிலை K_K4:		சாவி = VTERM_சாவி_KP_4; நிறுத்து;
	நிலை K_K5:		சாவி = VTERM_சாவி_KP_5; நிறுத்து;
	நிலை K_K6:		சாவி = VTERM_சாவி_KP_6; நிறுத்து;
	நிலை K_K7:		சாவி = VTERM_சாவி_KP_7; நிறுத்து;
	நிலை K_K8:		சாவி = VTERM_சாவி_KP_8; நிறுத்து;
	நிலை K_K9:		சாவி = VTERM_சாவி_KP_9; நிறுத்து;
	நிலை K_KDEL:		சாவி = VTERM_சாவி_DEL; நிறுத்து; // TODO
	நிலை K_KDIVIDE:		சாவி = VTERM_சாவி_KP_DIVIDE; நிறுத்து;
	நிலை K_KEND:		சாவி = VTERM_சாவி_KP_1; நிறுத்து; // TODO
	நிலை K_KENTER:		சாவி = VTERM_சாவி_KP_ENTER; நிறுத்து;
	நிலை K_KHOME:		சாவி = VTERM_சாவி_KP_7; நிறுத்து; // TODO
	நிலை K_KINS:		சாவி = VTERM_சாவி_KP_0; நிறுத்து; // TODO
	நிலை K_KMINUS:		சாவி = VTERM_சாவி_KP_MINUS; நிறுத்து;
	நிலை K_KMULTIPLY:	சாவி = VTERM_சாவி_KP_MULT; நிறுத்து;
	நிலை K_KPAGEDOWN:	சாவி = VTERM_சாவி_KP_3; நிறுத்து; // TODO
	நிலை K_KPAGEUP:		சாவி = VTERM_சாவி_KP_9; நிறுத்து; // TODO
	நிலை K_KPLUS:		சாவி = VTERM_சாவி_KP_PLUS; நிறுத்து;
	நிலை K_KPOINT:		சாவி = VTERM_சாவி_KP_PERIOD; நிறுத்து;
	நிலை K_இடது:		சாவி = VTERM_சாவி_இடது; நிறுத்து;
	நிலை K_S_இடது:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_இடது; நிறுத்து;
	நிலை K_C_இடது:		mod = VTERM_MOD_CTRL;
				சாவி = VTERM_சாவி_இடது; நிறுத்து;
	நிலை K_PAGEDOWN:	சாவி = VTERM_சாவி_PAGEDOWN; நிறுத்து;
	நிலை K_PAGEUP:		சாவி = VTERM_சாவி_PAGEUP; நிறுத்து;
	நிலை K_வலது:		சாவி = VTERM_சாவி_வலது; நிறுத்து;
	நிலை K_S_வலது:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_வலது; நிறுத்து;
	நிலை K_C_வலது:		mod = VTERM_MOD_CTRL;
				சாவி = VTERM_சாவி_வலது; நிறுத்து;
	நிலை K_UP:		சாவி = VTERM_சாவி_UP; நிறுத்து;
	நிலை K_S_UP:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_UP; நிறுத்து;
	நிலை TAB:		சாவி = VTERM_சாவி_TAB; நிறுத்து;
	நிலை K_S_TAB:		mod = VTERM_MOD_SHIFT;
				சாவி = VTERM_சாவி_TAB; நிறுத்து;

	நிலை K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); நிறுத்து;
	நிலை K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); நிறுத்து;
	நிலை K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); நிறுத்து;
	நிலை K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); நிறுத்து;

	நிலை K_LEFTMOUSE:
	நிலை K_LEFTMOUSE_NM:
	நிலை K_LEFTDRAG:
	நிலை K_LEFTRELEASE:
	நிலை K_LEFTRELEASE_NM:
	நிலை K_MOUSEMOVE:
	நிலை K_MIDDLEMOUSE:
	நிலை K_MIDDLEDRAG:
	நிலை K_MIDDLERELEASE:
	நிலை K_RIGHTMOUSE:
	நிலை K_RIGHTDRAG:
	நிலை K_RIGHTRELEASE:	எனில் (!term_mouse_click(vterm, c))
				    திரும்பு 0;
				other = TRUE;
				நிறுத்து;

	நிலை K_X1MOUSE:		/* TODO */ திரும்பு 0;
	நிலை K_X1DRAG:		/* TODO */ திரும்பு 0;
	நிலை K_X1RELEASE:	/* TODO */ திரும்பு 0;
	நிலை K_X2MOUSE:		/* TODO */ திரும்பு 0;
	நிலை K_X2DRAG:		/* TODO */ திரும்பு 0;
	நிலை K_X2RELEASE:	/* TODO */ திரும்பு 0;

	நிலை K_IGNORE:		திரும்பு 0;
	நிலை K_NOP:		திரும்பு 0;
	நிலை K_UNDO:		திரும்பு 0;
	நிலை K_HELP:		திரும்பு 0;
	நிலை K_XF1:		சாவி = VTERM_சாவி_FUNCTION(1); நிறுத்து;
	நிலை K_XF2:		சாவி = VTERM_சாவி_FUNCTION(2); நிறுத்து;
	நிலை K_XF3:		சாவி = VTERM_சாவி_FUNCTION(3); நிறுத்து;
	நிலை K_XF4:		சாவி = VTERM_சாவி_FUNCTION(4); நிறுத்து;
	நிலை K_SELECT:		திரும்பு 0;
#ifdef FEAT_GUI
	நிலை K_VER_SCROLLBAR:	திரும்பு 0;
	நிலை K_HOR_SCROLLBAR:	திரும்பு 0;
#endif
#ifdef FEAT_GUI_TABLINE
	நிலை K_TABLINE:		திரும்பு 0;
	நிலை K_TABMENU:		திரும்பு 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	நிலை K_F21:		சாவி = VTERM_சாவி_FUNCTION(21); நிறுத்து;
#endif
#ifdef FEAT_DND
	நிலை K_DROP:		திரும்பு 0;
#endif
	நிலை K_CURSORHOLD:	திரும்பு 0;
	நிலை K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				நிறுத்து;
	நிலை K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				நிறுத்து;
    }

    // add modifiers for the typed key
    எனில் (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    எனில் (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    எனில் (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    எனில் (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    எனில் (சாவி != VTERM_சாவி_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_சாவி(vterm, சாவி, mod);
    இல்லையெனில் எனில் (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    திரும்பு (முழு)vterm_output_read(vterm, buf, சாவி_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    நிலையான முழு
term_job_running_check(term_T *term, முழு check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    எனில் (term == வெற்று
	|| term->tl_job == வெற்று
	|| !channel_is_open(term->tl_job->jv_channel))
	திரும்பு FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    எனில் (check_job_status)
	job_status(job);
    திரும்பு (job->jv_status == JOB_STARTED
	    || (job->jv_channel != வெற்று && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    முழு
term_job_running(term_T *term)
{
    திரும்பு term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    முழு
term_job_running_not_none(term_T *term)
{
    திரும்பு term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    முழு
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    திரும்பு term != வெற்று
	&& term->tl_job != வெற்று
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    முழு
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_செய்தி_SIZE];
    முழு	ret;

    dialog_செய்தி(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, வெற்று, buff, 1);
    எனில் (ret == VIM_YES)
	திரும்பு OK;
    இல்லையெனில்
	திரும்பு FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    முழு
term_try_stop_job(buf_T *buf)
{
    முழு	    மொத்தம்;
    எழுத்து    *how = (எழுத்து *)buf->b_term->tl_kill;

#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    எனில் ((how == வெற்று || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	எனில் (term_confirm_stop(buf) == OK)
	    how = "kill";
	இல்லையெனில்
	    திரும்பு FAIL;
    }
#endif
    எனில் (how == வெற்று || *how == NUL)
	திரும்பு FAIL;

    job_stop(buf->b_term->tl_job, வெற்று, how);

    // wait for up to a second for the job to die
    க்கு (மொத்தம் = 0; மொத்தம் < 100; ++மொத்தம்)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	எனில் (!buf_valid(buf)
		|| buf->b_term == வெற்று
		|| buf->b_term->tl_job == வெற்று)
	    திரும்பு OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	எனில் (job->jv_status >= JOB_ENDED)
	    திரும்பு OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    திரும்பு FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    நிலையான வெறுமை
add_scrollback_வரி_to_buffer(term_T *term, char_u *text, முழு len, முழு append)
{
    buf_T	*buf = term->tl_buffer;
    முழு		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_வரி_மொத்தம்;

#ifdef MSWIN
    char_u *tmp = text;
    எனில் (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = வெற்று;
	முழு	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (எழுத்து*)text, len + 1,
							   &ret, &length);
	எனில் (ret != வெற்று)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (எழுத்து **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    எனில் (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	எனில் (both == வெற்று)
	    திரும்பு;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    இல்லையெனில்
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    எனில் (tmp != text)
	vim_free(text);
#endif
    எனில் (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    நிலையான வெறுமை
cell2cellattr(மாறிலி VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    நிலையான முழு
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    திரும்பு a->fg.red == b->fg.red
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
    நிலையான முழு
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, முழு lnum)
{
    எனில் (ga_grow(&term->tl_scrollback, 1) == FAIL)
	திரும்பு FALSE;

    sb_வரி_T *வரி = (sb_வரி_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    எனில் (lnum > 0)
    {
	முழு i;

	க்கு (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *வரி = *(வரி - 1);
	    --வரி;
	}
    }
    வரி->sb_cols = 0;
    வரி->sb_bytes = 0;
    வரி->sb_cells = வெற்று;
    வரி->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    திரும்பு OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    நிலையான வெறுமை
cleanup_scrollback(term_T *term)
{
    sb_வரி_T	*வரி;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_வரி_மொத்தம், FALSE);
    bufline_length = STRLEN(bufline);
    போது (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	வரி = (sb_வரி_T *)gap->ga_data + gap->ga_len - 1;
	எனில் (வரி->sb_bytes < 0 || (size_t)வரி->sb_bytes > bufline_length)
	    நிறுத்து;
	bufline_length -= வரி->sb_bytes;
	எனில் (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_வரி_மொத்தம்);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_வரி_மொத்தம், FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(வரி->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    எனில் (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_வரி_மொத்தம், shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    எனில் (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    நிலையான வெறுமை
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    முழு		    len;
    முழு		    வரிகள்_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == வெற்று ? வெற்று : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    க்கு (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	க்கு (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    எனில் (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    இல்லையெனில்
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	எனில் (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++வரிகள்_skipped;
	இல்லையெனில்
	{
	    போது (வரிகள்_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--வரிகள்_skipped;
		எனில் (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_வரி_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    எனில் (len == 0)
		p = வெற்று;
	    இல்லையெனில்
		p = ALLOC_MULT(cellattr_T, len);
	    எனில் ((p != வெற்று || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		முழு	    width;
		மாறிலி VTerm_வரி_Info *lineinfo;
		sb_வரி_T   *வரி = (sb_வரி_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		க்கு (pos.col = 0; pos.col < len; pos.col += width)
		{
		    எனில் (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			எனில் (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    இல்லையெனில்
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			எனில் (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			எனில் (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    முழு	    i;
			    முழு	    c;

			    க்கு (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		வரி->sb_cols = len;
		வரி->sb_bytes = ga.ga_len;
		வரி->sb_cells = p;
		வரி->sb_fill_attr = new_fill_attr;
		வரி->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		எனில் (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_வரி_to_buffer(term, (char_u *)"", 0, 0);
		இல்லையெனில்
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_வரி_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    இல்லையெனில்
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    க்கு (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	எனில் (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_வரி_to_buffer(term, (char_u *)"", 0, 0);
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
    நிலையான முழு
for_all_windows_and_curwin(win_T **wp, முழு *did_curwin)
{
    எனில் (*wp == வெற்று)
	*wp = firstwin;
    இல்லையெனில் எனில் ((*wp)->w_next != வெற்று)
	*wp = (*wp)->w_next;
    இல்லையெனில் எனில் (!*did_curwin)
	*wp = curwin;
    இல்லையெனில்
	திரும்பு FALSE;
    எனில் (*wp == curwin)
	*did_curwin = TRUE;
    திரும்பு TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    வெறுமை
may_move_terminal_to_buffer(term_T *term, முழு redraw)
{
    எனில் (term->tl_vterm == வெற்று)
	திரும்பு;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    எனில் (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_வரி_மொத்தம்
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    எனில் (redraw)
    {
	win_T	    *wp = வெற்று;
	முழு	    did_curwin = FALSE;

	போது (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    எனில் (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_வரி_மொத்தம்;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		எனில் (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    எனில் (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#எனில் defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    முழு
term_check_timers(முழு next_due_arg, proftime_T *now)
{
    term_T  *term;
    முழு	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	எனில் (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_இடது(&term->tl_timer_due, now);

	    எனில் (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    இல்லையெனில் எனில் (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    திரும்பு next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    நிலையான வெறுமை
set_terminal_mode(term_T *term, முழு normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    எனில் (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    எனில் (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    நிலையான வெறுமை
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    எனில் (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    நிலையான வெறுமை
term_enter_normal_mode(வெறுமை)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    முழு        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, வெற்று);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    எனில் (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    முழு
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    திரும்பு term != வெற்று && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    வெறுமை
term_enter_job_mode(வெறுமை)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    எனில் (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    எனில் (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
வகைவரை எண்ணிக்கை {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_சாவி_state_T;

நிலையான reduce_சாவி_state_T  no_reduce_சாவி_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    நிலையான முழு
vterm_using_சாவி_protocol(வெறுமை)
{
    திரும்பு curbuf->b_term != வெற்று
	&& curbuf->b_term->tl_vterm != வெற்று
	&& (vterm_is_modify_other_சாவிகள்(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    வெறுமை
check_no_reduce_சாவிகள்(வெறுமை)
{
    எனில் (no_reduce_சாவி_state != NRKS_CHECK
	    || no_reduce_சாவிகள் >= 1
	    || curbuf->b_term == வெற்று
	    || curbuf->b_term->tl_vterm == வெற்று)
	திரும்பு;

    எனில் (vterm_using_சாவி_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_சாவி_state = NRKS_SET;
	++no_reduce_சாவிகள்;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    நிலையான முழு
term_vgetc(வெறுமை)
{
    முழு c;
    முழு save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    எனில் (vterm_using_சாவி_protocol())
    {
	++no_reduce_சாவிகள்;
	no_reduce_சாவி_state = NRKS_SET;
    }
    இல்லையெனில்
    {
	no_reduce_சாவி_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    எனில் (no_reduce_சாவி_state == NRKS_SET)
	--no_reduce_சாவிகள்;
    no_reduce_சாவி_state = NRKS_NONE;

    திரும்பு c;
}

நிலையான முழு	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    முழு
send_சாவிகள்_to_term(term_T *term, முழு c, முழு modmask, முழு typed)
{
    எழுத்து	செய்தி[சாவி_BUF_LEN];
    size_t	len;
    முழு		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    தேர்வு (c)
    {
	நிலை NUL:
	நிலை K_ZERO:
	    எனில் (typed)
		stuffcharReadbuff(c);
	    திரும்பு FAIL;

	நிலை K_TABLINE:
	    stuffcharReadbuff(c);
	    திரும்பு FAIL;

	நிலை K_IGNORE:
	நிலை K_CANCEL:  // used for :normal when running out of chars
	    திரும்பு FAIL;

	நிலை K_LEFTDRAG:
	நிலை K_MIDDLEDRAG:
	நிலை K_RIGHTDRAG:
	நிலை K_X1DRAG:
	நிலை K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	நிலை K_LEFTMOUSE:
	நிலை K_LEFTMOUSE_NM:
	நிலை K_LEFTRELEASE:
	நிலை K_LEFTRELEASE_NM:
	நிலை K_MOUSEMOVE:
	நிலை K_MIDDLEMOUSE:
	நிலை K_MIDDLERELEASE:
	நிலை K_RIGHTMOUSE:
	நிலை K_RIGHTRELEASE:
	நிலை K_X1MOUSE:
	நிலை K_X1RELEASE:
	நிலை K_X2MOUSE:
	நிலை K_X2RELEASE:

	நிலை K_MOUSEUP:
	நிலை K_MOUSEDOWN:
	நிலை K_MOUSELEFT:
	நிலை K_MOUSERIGHT:
	    {
		முழு	row = mouse_row;
		முழு	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		எனில் (popup_is_popup(curwin))
		{
		    row -= popup_உச்சி_extra(curwin);
		    col -= popup_இடது_extra(curwin);
		}
#endif
		எனில் (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    எனில் (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    திரும்பு FAIL;
		}
	    }
	    நிறுத்து;

	நிலை K_COMMAND:
	நிலை K_SCRIPT_COMMAND:
	    திரும்பு do_cmdkey_command(c, 0);
    }
    எனில் (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_சாவி(term, c, modmask, செய்தி);
    எனில் (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)செய்தி, (முழு)len, வெற்று);

    திரும்பு OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    நிலையான வெறுமை
term_paste_register(முழு prev_c UNUSED)
{
    முழு		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    முழு		type;

    எனில் (add_to_showcmd(prev_c))
    எனில் (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    எனில் (!term_use_loop())
	// job finished while waiting for a character
	திரும்பு;

    // CTRL-W "= prompt for expression to evaluate.
    எனில் (c == '=' && get_கோவை_register() != '=')
	திரும்பு;
    எனில் (!term_use_loop())
	// job finished while waiting for a character
	திரும்பு;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    எனில் (l == வெற்று)
	திரும்பு;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_பொருட்கள்(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	எனில் (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = வெற்று;
	    முழு	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (எழுத்து *)s,
		    (முழு)STRLEN(s), &ret, &length);
	    எனில் (ret != வெற்று)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (எழுத்து **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (முழு)STRLEN(s), வெற்று);
#ifdef MSWIN
	எனில் (tmp != s)
	    vim_free(s);
#endif

	எனில் (item->li_next != வெற்று || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, வெற்று);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    முழு
terminal_is_active(வெறுமை)
{
    திரும்பு in_terminal_loop != வெற்று;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    நிலையான முழு
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    எனில் (wp != வெற்று && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    இல்லையெனில் எனில் (term->tl_highlight_name != வெற்று)
	name = term->tl_highlight_name;
    இல்லையெனில்
	name = (char_u*)"Terminal";

    திரும்பு syn_name2id(name);
}

#எனில் defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    நிலையான cursorentry_T entry;
    முழு			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_இடது ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    எனில் (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    எனில் (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    எனில் (term_bg != INVALCOLOR)
	*fg = term_bg;
    இல்லையெனில்
	*fg = gui.back_pixel;

    எனில் (term->tl_cursor_color == வெற்று)
    {
	எனில் (term_fg != INVALCOLOR)
	    *bg = term_fg;
	இல்லையெனில்
	    *bg = gui.norm_pixel;
    }
    இல்லையெனில்
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    திரும்பு &entry;
}
#endif

    நிலையான வெறுமை
may_output_cursor_props(வெறுமை)
{
    எனில் (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	எனில் (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	இல்லையெனில்
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    நிலையான வெறுமை
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    எனில் (gui.in_use)
	திரும்பு;
#endif
    எனில் (in_terminal_loop == term)
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
    நிலையான வெறுமை
prepare_restore_cursor_props(வெறுமை)
{
#ifdef FEAT_GUI
    எனில் (gui.in_use)
	திரும்பு;
#endif
    cursor_color_copy(&desired_cursor_color, வெற்று);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    நிலையான முழு
term_use_loop_check(முழு check_job_status)
{
    term_T *term = curbuf->b_term;

    திரும்பு term != வெற்று
	&& !term->tl_normal_mode
	&& term->tl_vterm != வெற்று
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    முழு
term_use_loop(வெறுமை)
{
    திரும்பு term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    வெறுமை
term_win_entered(வெறுமை)
{
    term_T *term = curbuf->b_term;

    எனில் (term == வெற்று)
	திரும்பு;

    எனில் (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	எனில் (State & MODE_செருகு)
	    stop_செருகு_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    வெறுமை
term_focus_change(முழு in_focus)
{
    term_T *term = curbuf->b_term;

    எனில் (term == வெற்று || term->tl_vterm == வெற்று)
	திரும்பு;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    எனில் (in_focus)
	vterm_state_focus_in(state);
    இல்லையெனில்
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    நிலையான முழு
raw_c_to_ctrl(முழு c)
{
    எனில் ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	திரும்பு c & 0x1f;
    திரும்பு c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    நிலையான முழு
ctrl_to_raw_c(முழு c)
{
    எனில் (c < 0x20 && vterm_using_சாவி_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	திரும்பு c + '@';
    }
    திரும்பு c;
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
    முழு
terminal_loop(முழு blocking)
{
    முழு		c;
    முழு		raw_c;
    முழு		termwinkey = 0;
    முழு		ret;
#ifdef UNIX
    முழு		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    முழு		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    எனில் (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_சாவி(curwin->w_p_twk, TRUE);
	எனில் (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    போது (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	எனில் (curbuf->b_term != வெற்று && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    போது (must_redraw != 0)
		எனில் (update_screen(0) == FAIL)
		    நிறுத்து;
	எனில் (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    நிறுத்து;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	எனில் (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    எனில் (raw_c != K_IGNORE)
		vungetc(raw_c);
	    நிறுத்து;
	}
	எனில் (raw_c == K_IGNORE)
	    தொடர்;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	எனில் (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    எனில் (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	எனில் (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	எனில் ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    முழு	    prev_c = c;
	    முழு	    prev_raw_c = raw_c;
	    முழு	    prev_mod_mask = mod_mask;

	    எனில் (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    எனில் (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		நிறுத்து;

	    எனில் (prev_c == Ctrl_BSL)
	    {
		எனில் (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    செல் theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_சாவிகள்_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    இல்லையெனில் எனில் (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    இல்லையெனில் எனில் (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    இல்லையெனில் எனில் (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    இல்லையெனில் எனில் (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		செல் theend;
	    }
	    இல்லையெனில் எனில் (c == '"')
	    {
		term_paste_register(prev_c);
		தொடர்;
	    }
	    இல்லையெனில் எனில் (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		செல் theend;
	    }
	}
#ifdef MSWIN
	எனில் (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    எனில் (MultiByteToWideChar(GetACP(), 0, (எழுத்து*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	எனில் (send_சாவிகள்_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    எனில் (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    செல் theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = வெற்று;
    எனில் (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    எனில் (curbuf->b_term != வெற்று && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    திரும்பு ret;
}

    நிலையான வெறுமை
may_toggle_cursor(term_T *term)
{
    எனில் (in_terminal_loop != term)
	திரும்பு;

    எனில் (term->tl_cursor_visible)
	cursor_on();
    இல்லையெனில்
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    நிலையான முழு
color2index(VTermColor *color, முழு fg, முழு *boldp)
{
    முழு red = color->red;
    முழு blue = color->blue;
    முழு green = color->green;

    *boldp = FALSE;

    எனில் (VTERM_COLOR_IS_INVALID(color))
	திரும்பு 0;

    எனில் (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	எனில் (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    எனில் (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    திரும்பு index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	இல்லையெனில் எனில் (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    திரும்பு (color->index & 7) + 1;
	}
	திரும்பு 0;
    }

    எனில் (t_colors >= 256)
    {
	எனில் (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    நிலையான முழு cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    முழு i;

	    எனில் (red < 5)
		திரும்பு 17; // 00/00/00
	    எனில் (red > 245) // ff/ff/ff
		திரும்பு 232;
	    க்கு (i = 0; i < 23; ++i)
		எனில் (red < cutoff[i])
		    திரும்பு i + 233;
	    திரும்பு 256;
	}
	{
	    நிலையான முழு cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    முழு ri, gi, bi;

	    // 216-color cube
	    க்கு (ri = 0; ri < 5; ++ri)
		எனில் (red < cutoff[ri])
		    நிறுத்து;
	    க்கு (gi = 0; gi < 5; ++gi)
		எனில் (green < cutoff[gi])
		    நிறுத்து;
	    க்கு (bi = 0; bi < 5; ++bi)
		எனில் (blue < cutoff[bi])
		    நிறுத்து;
	    திரும்பு 17 + ri * 36 + gi * 6 + bi;
	}
    }
    திரும்பு 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    நிலையான முழு
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    முழு attr = 0;

    எனில் (cellattrs->bold)
	attr |= HL_BOLD;
    எனில் (cellattrs->underline)
	attr |= HL_UNDERLINE;
    எனில் (cellattrs->italic)
	attr |= HL_ITALIC;
    எனில் (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    எனில் (cellattrs->reverse)
	attr |= HL_INVERSE;
    திரும்பு attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    நிலையான வெறுமை
hl2vtermAttr(முழு attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    எனில் (attr & HL_BOLD)
	cell->attrs.bold = 1;
    எனில் (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    எனில் (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    எனில் (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    எனில் (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    நிலையான முழு
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    முழு attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    முழு is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    முழு is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    எனில் (is_default_fg || is_default_bg)
    {
	எனில் (wp != வெற்று && wp->w_hlfwin_id != 0)
	{
	    எனில் (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    எனில் (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	இல்லையெனில்
	{
	    எனில் (is_default_fg)
		fg = &term->tl_default_color.fg;
	    எனில் (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	திரும்பு get_gui_attr_idx(attr, guifg, guibg);
    }
    இல்லையெனில்
#endif
#ifdef FEAT_TERMGUICOLORS
    எனில் (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	திரும்பு get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    இல்லையெனில்
#endif
    {
	முழு bold = MAYBE;
	முழு ctermfg = color2index(fg, TRUE, &bold);
	முழு ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	எனில் (bold == TRUE)
	    attr |= HL_BOLD;

	திரும்பு get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    திரும்பு 0;
}

    நிலையான வெறுமை
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    எனில் (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    நிலையான முழு
handle_damage(VTermRect rect, வெறுமை *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    திரும்பு 1;
}

    நிலையான வெறுமை
term_scroll_up(term_T *term, முழு start_row, முழு மொத்தம்)
{
    win_T		 *wp = வெற்று;
    முழு			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    முழு			 clear_attr;

    CLEAR_FIELD(attr);

    போது (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	எனில் (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_வரிகள்(wp, start_row, மொத்தம், FALSE, FALSE, clear_attr);
	}
    }
}

    நிலையான முழு
handle_moverect(VTermRect dest, VTermRect src, வெறுமை *user)
{
    term_T	*term = (term_T *)user;
    முழு		மொத்தம் = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    எனில் (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	எனில் (dest.start_row == 0)
	    term->tl_postponed_scroll += மொத்தம்;
	இல்லையெனில்
	    term_scroll_up(term, dest.start_row, மொத்தம்);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    திரும்பு 1;
}

    நிலையான முழு
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	முழு visible,
	வெறுமை *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = வெற்று;
    முழு		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    போது (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	எனில் (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    எனில் (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    திரும்பு 1;
}

    நிலையான முழு
handle_settermprop(
	VTermProp prop,
	VTerm_மதிப்பு *மதிப்பு,
	வெறுமை *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = வெற்று;

    தேர்வு (prop)
    {
	நிலை VTERM_PROP_TITLE:
	    எனில் (disable_vterm_title_for_testing)
		நிறுத்து;
	    strval = vim_strnsave((char_u *)மதிப்பு->string.str,
							    மதிப்பு->string.len);
	    எனில் (strval == வெற்று)
		நிறுத்து;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    எனில் (*skipwhite(strval) == NUL)
		term->tl_title = வெற்று;
	    // Same as blank
	    இல்லையெனில் எனில் (term->tl_arg0_cmd != வெற்று
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (முழு)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = வெற்று;
	    // Empty corrupted data of winpty
	    இல்லையெனில் எனில் (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = வெற்று;
#ifdef MSWIN
	    இல்லையெனில் எனில் (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = வெற்று;
		முழு	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(எழுத்து*)மதிப்பு->string.str,
					(முழு)மதிப்பு->string.len, &ret, &length);
		எனில் (ret != வெற்று)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (எழுத்து**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    இல்லையெனில்
	    {
		term->tl_title = strval;
		strval = வெற்று;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    எனில் (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    நிறுத்து;

	நிலை VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = மதிப்பு->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    நிறுத்து;

	நிலை VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = மதிப்பு->boolean;
	    may_set_cursor_props(term);
	    நிறுத்து;

	நிலை VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = மதிப்பு->எண்;
	    may_set_cursor_props(term);
	    நிறுத்து;

	நிலை VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)மதிப்பு->string.str,
							    மதிப்பு->string.len);
	    எனில் (strval == வெற்று)
		நிறுத்து;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    நிறுத்து;

	நிலை VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = மதிப்பு->boolean;
	    நிறுத்து;

	இயல்பு:
	    நிறுத்து;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    திரும்பு 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    நிலையான முழு
handle_resize(முழு rows, முழு cols, வெறுமை *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    எனில் (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    இல்லையெனில்
    {
	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    திரும்பு 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    நிலையான வெறுமை
limit_scrollback(term_T *term, garray_T *gap, முழு update_buffer)
{
    எனில் (gap->ga_len < term->tl_buffer->b_p_twsl)
	திரும்பு;

    முழு	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    முழு	i;
    sb_வரி_T *sb_வரிகள் = (sb_வரி_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    க்கு (i = 0; i < todo; ++i)
    {
	எனில் (update_buffer && (!sb_வரிகள்[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_வரிகள்[i].sb_cells);
    }
    // Continue until end of wrapped line
    க்கு (; todo < gap->ga_len && sb_வரிகள்[todo].continuation; ++todo)
	vim_free(sb_வரிகள்[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_வரி_T *)gap->ga_data + todo,
		அளவு(sb_வரி_T) * gap->ga_len);
    எனில் (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = வெற்று;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp->w_buffer == term->tl_buffer)
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
    நிலையான முழு
handle_pushline(முழு cols, மாறிலி VTermScreenCell *cells, முழு continuation, வெறுமை *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    முழு		update_buffer;

    எனில் (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    இல்லையெனில்
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு 0;

    cellattr_T	*p = வெற்று;
    முழு		len = 0;
    முழு		i;
    முழு		c;
    முழு		col;
    முழு		text_len;
    char_u		*text;
    sb_வரி_T	*வரி;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    க்கு (i = 0; i < cols; ++i)
	எனில் (cells[i].chars[0] != 0)
	    len = i + 1;
	இல்லையெனில்
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    எனில் (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    எனில் (p != வெற்று)
    {
	க்கு (col = 0; col < len; col += cells[col].width)
	{
	    எனில் (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		நிறுத்து;
	    }
	    க்கு (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    எனில் (ga_grow(&ga, 1) == FAIL)
    {
	எனில் (update_buffer)
	    text = (char_u *)"";
	இல்லையெனில்
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    இல்லையெனில்
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    எனில் (update_buffer)
	add_scrollback_வரி_to_buffer(term, text, text_len, continuation);

    வரி = (sb_வரி_T *)gap->ga_data + gap->ga_len;
    வரி->sb_cols = len;
    வரி->sb_bytes = text_len;
    வரி->sb_cells = p;
    வரி->sb_fill_attr = fill_attr;
    வரி->continuation = (char_u)continuation;
    எனில் (update_buffer)
    {
	வரி->sb_text = வெற்று;
	++term->tl_scrollback_scrolled;
	எனில் (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    இல்லையெனில்
    {
	வரி->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    திரும்பு 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    நிலையான வெறுமை
handle_postponed_scrollback(term_T *term)
{
    முழு i;

    எனில் (term->tl_scrollback_postponed.ga_len == 0)
	திரும்பு;
    ch_log(வெற்று, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    க்கு (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_வரி_T	*pp_வரி;
	sb_வரி_T	*வரி;

	எனில் (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    நிறுத்து;
	pp_வரி = (sb_வரி_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_வரி->sb_text;
	எனில் (text == வெற்று)
	    text = (char_u *)"";
	add_scrollback_வரி_to_buffer(term, text, (முழு)STRLEN(text), pp_வரி->continuation);
	vim_free(pp_வரி->sb_text);

	வரி = (sb_வரி_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	வரி->sb_cols = pp_வரி->sb_cols;
	வரி->sb_bytes = pp_வரி->sb_bytes;
	வரி->sb_cells = pp_வரி->sb_cells;
	வரி->sb_fill_attr = pp_வரி->sb_fill_attr;
	வரி->sb_text = வெற்று;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	எனில் (!pp_வரி->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    நிலையான முழு
handle_bell(வெறுமை *user UNUSED)
{
    vim_beep(BO_TERM);
    திரும்பு 0;
}

நிலையான VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    வெற்று,			// sb_pushline
    வெற்று,			// sb_popline
    வெற்று,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    நிலையான முழு
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    எனில் (!term->tl_normal_mode)
    {
	முழு	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	எனில் (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    முழு		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = வெற்று;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    எனில் (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		எனில் (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    இல்லையெனில்
#endif
	    // If this is the last normal window: exit Vim.
	    எனில் (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		திரும்பு TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(வெற்று, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    எனில் (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		எனில் (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		எனில் (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		எனில் (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    எனில் (pwin != வெற்று)
		popup_close_with_retval(pwin, 0);
#endif
	    திரும்பு TRUE;
	}
	எனில் (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    எழுத்து    *cmd = term->tl_opencmd == வெற்று
				? "botright sbuf %d"
				: (எழுத்து *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    எழுத்து    *buf = alloc(len);

	    எனில் (buf != வெற்று)
	    {
		ch_log(வெற்று, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	இல்லையெனில்
	    ch_log(வெற்று, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    திரும்பு FALSE;
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    முழு
may_close_term_popup(வெறுமை)
{
    எனில் (!popup_is_popup(curwin) || curbuf->b_term == வெற்று
				 || term_job_running_not_none(curbuf->b_term))
	திரும்பு FAIL;

    win_T *pwin = curwin;

    எனில் (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    திரும்பு OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    வெறுமை
term_channel_closing(channel_T *ch)
{
    term_T *term;

    க்கு (term = first_term; term != வெற்று; term = term->tl_next)
	எனில் (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    வெறுமை
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    முழு	    did_one = FALSE;

    க்கு (term = first_term; term != வெற்று; term = next_term)
    {
	next_term = term->tl_next;
	எனில் (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    எனில் (term->tl_out_fd != வெற்று)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = வெற்று;
	    }
#endif

	    எனில் (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		தொடர்;
	    }

	    எனில் (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    எனில் (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	எனில் (term != வெற்று)
	{
	    எனில் (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    வெறுமை
term_check_channel_closed_recently(வெறுமை)
{
    term_T *term;
    term_T *next_term;

    க்கு (term = first_term; term != வெற்று; term = next_term)
    {
	next_term = term->tl_next;
	எனில் (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    எனில் (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    நிலையான வெறுமை
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	முழு		max_col)
{
    முழு off = screen_get_current_வரி_off();

    க்கு (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	முழு		c;

	எனில் (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	எனில் (c == NUL)
	{
	    Screen_வரிகள்[off] = ' ';
	    எனில் (enc_utf8)
		Screen_வரிகள்_UC[off] = NUL;
	}
	இல்லையெனில்
	{
	    எனில் (enc_utf8)
	    {
		முழு i;

		// composing chars
		க்கு (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_வரிகள்_C[i][off] = cell.chars[i + 1];
		    எனில் (cell.chars[i + 1] == 0)
			நிறுத்து;
		}
		எனில் (c >= 0x80 || (Screen_mco > 0
					 && Screen_வரிகள்_C[0][off] != 0))
		{
		    Screen_வரிகள்[off] = ' ';
		    Screen_வரிகள்_UC[off] = c;
		}
		இல்லையெனில்
		{
		    Screen_வரிகள்[off] = c;
		    Screen_வரிகள்_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    இல்லையெனில் எனில் (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		எனில் (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (எழுத்து*)mb, 2, 0, 0) > 1)
		{
		    Screen_வரிகள்[off] = mb[0];
		    Screen_வரிகள்[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		இல்லையெனில்
		    Screen_வரிகள்[off] = c;
	    }
#endif
	    இல்லையெனில்
		// This will only store the lower byte of "c".
		Screen_வரிகள்[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	எனில் (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    எனில் (enc_utf8)
	    {
		Screen_வரிகள்_UC[off] = NUL;
		Screen_வரிகள்[off] = NUL;
	    }
	    இல்லையெனில் எனில் (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_வரிகள்[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#எனில் defined(FEAT_GUI)
    நிலையான வெறுமை
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    எனில் (term->tl_vterm == வெற்று)
	திரும்பு;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    போது (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	முழு save_p_more = p_more;

	p_more = FALSE;
	செய்தி_row = Rows - 1;
	செய்தி_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    க்கு (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	எனில் (pos.row < term->tl_rows)
	{
	    முழு max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, வெற்று, screen, &pos, max_col);
	}
	இல்லையெனில்
	    pos.col = 0;

	screen_வரி(curwin, term->tl_toprow + pos.row,
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
    முழு
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    திரும்பு term != வெற்று && term->tl_vterm != வெற்று && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    வெறுமை
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    முழு		rows, cols;
    முழு		newrows, newcols;
    முழு		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    எனில் (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	எனில் (term->tl_postponed_scroll > 0
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
    க்கு (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == வெற்று ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	எனில் (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	எனில் (twp == வெற்று)
	    நிறுத்து;
    }
    எனில் (newrows == 99999 || newcols == 99999)
	திரும்பு; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    எனில் (newrows == 0 || newcols == 0)
	திரும்பு;

    எனில் (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	எனில் (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    க்கு (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	எனில் (pos.row < term->tl_rows)
	{
	    முழு max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	இல்லையெனில்
	    pos.col = 0;

	screen_வரி(wp, wp->w_winrow + pos.row
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
    வெறுமை
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    எனில் (term == வெற்று || term->tl_vterm == வெற்று || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	திரும்பு;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    முழு
term_is_finished(buf_T *buf)
{
    திரும்பு buf->b_term != வெற்று && buf->b_term->tl_vterm == வெற்று;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    முழு
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    திரும்பு term != வெற்று && (term->tl_vterm == வெற்று || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    வெறுமை
term_change_in_curbuf(வெறுமை)
{
    term_T *term = curbuf->b_term;

    எனில் (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	திரும்பு;

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
    முழு
term_get_attr(win_T *wp, linenr_T lnum, முழு col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_வரி_T	*வரி;
    cellattr_T	*cellattr;
    முழு         sb_வரி = -1;
    முழு         sb_col = col;

    எனில் (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_வரி, &sb_col);

    எனில் (sb_வரி < 0)
	cellattr = &term->tl_default_color;
    இல்லையெனில்
    {
	வரி = (sb_வரி_T *)term->tl_scrollback.ga_data + sb_வரி;
	எனில் (sb_col < 0 || sb_col >= வரி->sb_cols)
	    cellattr = &வரி->sb_fill_attr;
	இல்லையெனில்
	    cellattr = வரி->sb_cells + sb_col;
    }
    திரும்பு cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    நிலையான வெறுமை
cterm_color2vterm(முழு nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    எனில் (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    இல்லையெனில்
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
    நிலையான முழு
get_vterm_color_from_synid(முழு id, VTermColor *fg, VTermColor *bg)
{
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    எனில் (0
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

	எனில் (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	எனில் (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	இல்லையெனில்
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	எனில் (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	இல்லையெனில்
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	திரும்பு TRUE;
    }
    இல்லையெனில்
#endif
    எனில் (t_colors >= 16)
    {
	முழு cterm_fg = -1;
	முழு cterm_bg = -1;

	எனில் (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	எனில் (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	இல்லையெனில்
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	எனில் (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	இல்லையெனில்
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	திரும்பு TRUE;
    }

    திரும்பு FALSE;
}

    வெறுமை
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    வெறுமை
term_update_hlfwin(win_T *wp)
{
    முழு id = wp->w_hlfwin_id;

    எனில் (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    வெறுமை
term_update_hlfwin_all(வெறுமை)
{
    win_T	 *wp = வெற்று;
    முழு		 did_curwin = FALSE;

    போது (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    வெறுமை
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    முழு		    fgval, bgval;
    முழு		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    எனில் (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    இல்லையெனில்
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, வெற்று);

    எனில் (!get_vterm_color_from_synid(id, fg, bg))
    {
#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	முழு tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	எனில் (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    எனில் (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	இல்லையெனில்
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	எனில் (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    எனில் (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	இல்லையெனில்
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    நிலையான முழு
term_use_palette(வெறுமை)
{
    எனில் (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    நிலையான வெறுமை
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    முழு		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    க்கு (; index < 16; index++)
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
    நிலையான முழு
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    முழு		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    க்கு (li = list->lv_first; li != வெற்று && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	எனில் (color_name == வெற்று)
	    திரும்பு FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	எனில் (guicolor == INVALCOLOR)
	    திரும்பு FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    எனில் (n != 16 || li != வெற்று)
	திரும்பு FAIL;

    set_vterm_palette(vterm, rgb);

    திரும்பு OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    நிலையான வெறுமை
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", வெற்று, TRUE);

    எனில் (var == வெற்று)
	திரும்பு;

    எனில் (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == வெற்று
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    நிலையான வெறுமை
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    முழு		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = வெற்று;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	எனில் (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    திரும்பு;
	}
    }

    CLEAR_FIELD(ea);

    எனில் (opt_item != வெற்று && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != வெற்று)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	எனில் (p == வெற்று)
	    p = dict_get_string(dict, "fileformat", FALSE);
	எனில் (p != வெற்று)
	{
	    எனில் (check_ff_மதிப்பு(p) == FAIL)
		ch_log(வெற்று, "Invalid ff argument to drop: %s", p);
	    இல்லையெனில்
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	எனில் (p == வெற்று)
	    p = dict_get_string(dict, "encoding", FALSE);
	எனில் (p != வெற்று)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    எனில் (ea.cmd != வெற்று)
	    {
		sprintf((எழுத்து *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	எனில் (p != வெற்று)
	    get_bad_opt(p, &ea);

	எனில் (dict_has_சாவி(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	எனில் (dict_has_சாவி(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	எனில் (dict_has_சாவி(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	எனில் (dict_has_சாவி(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    எனில் (ea.cmd == வெற்று)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    நிலையான முழு
is_permitted_term_api(char_u *func, char_u *pat)
{
    திரும்பு pat != வெற்று && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    நிலையான வெறுமை
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    எனில் (item->li_next == வெற்று)
    {
	ch_log(channel, "Missing function arguments for call");
	திரும்பு;
    }
    func = tv_get_string(&item->li_tv);

    எனில் (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	திரும்பு;
    }

    argvars[0].v_type = VAR_எண்;
    argvars[0].vval.v_எண் = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    எனில் (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    இல்லையெனில்
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
    நிலையான size_t
url_decode(மாறிலி எழுத்து *src, மாறிலி size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    போது (i < len)
    {
	எனில் (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	இல்லையெனில்
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    திரும்பு j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    நிலையான வெறுமை
sync_shell_dir(garray_T *gap)
{
    முழு       offset = 7;  // len of "file://" is 7
    எழுத்து      *pos = (எழுத்து *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    போது (offset < (முழு)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    எனில் (offset >= (முழு)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	திரும்பு;
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
    நிலையான முழு
parse_osc(முழு command, VTermStringFragment frag, வெறுமை *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == வெற்று ? வெற்று
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    எனில் (command != 51 && (command != 7 || !p_asd))
	திரும்பு 0;

    // Concatenate what was received until the final piece is found.
    எனில் (ga_grow(gap, (முழு)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	திரும்பு 1;
    }
    mch_memmove((எழுத்து *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (முழு)frag.len;
    எனில் (!frag.final)
	திரும்பு 1;

    ((எழுத்து *)gap->ga_data)[gap->ga_len] = 0;

    எனில் (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	திரும்பு 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = வெற்று;
    reader.js_used = 0;
    எனில் (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != வெற்று)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	எனில் (item == வெற்று)
	    ch_log(channel, "Missing command");
	இல்லையெனில்
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    எனில் (item == வெற்று)
		ch_log(channel, "Missing argument for %s", cmd);
	    இல்லையெனில் எனில் (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    இல்லையெனில் எனில் (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    இல்லையெனில்
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    இல்லையெனில்
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    திரும்பு 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    நிலையான முழு
parse_csi(
	மாறிலி எழுத்து  *leader UNUSED,
	மாறிலி long  args[],
	முழு	    argcount,
	மாறிலி எழுத்து  *intermed UNUSED,
	எழுத்து	    command,
	வெறுமை	    *user)
{
    term_T	*term = (term_T *)user;
    எழுத்து	buf[100];
    முழு		len;
    முழு		x = 0;
    முழு		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    எனில் (command != 't' || argcount != 1 || args[0] != 13)
	திரும்பு 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#எனில் defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (வெறுமை)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_buffer == term->tl_buffer)
	    நிறுத்து;
    எனில் (wp != வெற்று)
    {
#ifdef FEAT_GUI
	எனில் (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	இல்லையெனில்
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
						     (char_u *)buf, len, வெற்று);
    திரும்பு 1;
}

நிலையான VTermStateFallbacks state_fallbacks = {
    வெற்று,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    வெற்று,		// dcs
    வெற்று,		// apc
    வெற்று,		// pm
    வெற்று		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    நிலையான வெறுமை *
vterm_malloc(size_t size, வெறுமை *data UNUSED)
{
    // make sure that the length is not zero
    திரும்பு alloc_clear(size == 0 ? 1L : size);
}

    நிலையான வெறுமை
vterm_memfree(வெறுமை *ptr, வெறுமை *data UNUSED)
{
    vim_free(ptr);
}

நிலையான VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    நிலையான முழு
create_vterm(term_T *term, முழு rows, முழு cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_மதிப்பு	    மதிப்பு;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, வெற்று);
    term->tl_vterm = vterm;
    எனில் (vterm == வெற்று)
	திரும்பு FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    எனில் (state == வெற்று || screen == வெற்று)
    {
	vterm_free(vterm);
	திரும்பு FAIL;
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

    எனில் (t_colors < 16)
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
    எனில் (GetCaretBlinkTime() == INFINITE)
	மதிப்பு.boolean = 0;
    இல்லையெனில்
	மதிப்பு.boolean = 1;
#இல்லையெனில்
    மதிப்பு.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &மதிப்பு);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    திரும்பு OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    நிலையான வெறுமை
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    முழு		index;

    க்கு (index = 0; index < 16; index++)
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

    நிலையான வெறுமை
term_update_palette(term_T *term)
{
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    எனில் (term_use_palette()
	    && (term->tl_palette != வெற்று
		|| find_var((char_u *)"g:terminal_ansi_colors", வெற்று, TRUE)
								      != வெற்று))
    {
	எனில் (term->tl_palette != வெற்று)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	இல்லையெனில்
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    இல்லையெனில்
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    வெறுமை
term_update_palette_all(வெறுமை)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	எனில் (term->tl_vterm == வெற்று)
	    தொடர்;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    வெறுமை
term_update_colors_all(வெறுமை)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	எனில் (term->tl_vterm == வெற்று)
	    தொடர்;
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
    எனில் (term->tl_status_text != வெற்று)
	திரும்பு term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    எனில் (term->tl_normal_mode)
    {
	எனில் (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	இல்லையெனில்
	    txt = (char_u *)_("Terminal-finished");
    }
    இல்லையெனில் எனில் (term->tl_title != வெற்று)
	txt = term->tl_title;
    இல்லையெனில் எனில் (term_none_open(term))
	txt = (char_u *)_("active");
    இல்லையெனில் எனில் (term_job_running(term))
	txt = (char_u *)_("running");
    இல்லையெனில்
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    எனில் (term->tl_status_text != வெற்று)
	vim_snprintf((எழுத்து *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    திரும்பு term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    வெறுமை
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    முழு
set_ref_in_term(முழு copyID)
{
    முழு		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    க்கு (term = first_term; !abort && term != வெற்று; term = term->tl_next)
	எனில் (term->tl_job != வெற்று)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
	}
    திரும்பு abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    நிலையான buf_T *
term_get_buf(typval_T *argvars, எழுத்து *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    எனில் (buf == வெற்று || buf->b_term == வெற்று)
    {
	(வெறுமை)tv_get_எண்(&argvars[0]);    // issue errmsg if type error
	ch_log(வெற்று, "%s: invalid buffer argument", where);
	திரும்பு வெற்று;
    }
    திரும்பு buf;
}

    நிலையான வெறுமை
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    நிலையான வெறுமை
dump_term_color(FILE *fd, VTermColor *color)
{
    முழு index;

    எனில் (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    இல்லையெனில் எனில் (color->type == 0)
	// use RGB values
	index = 255;
    இல்லையெனில்
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (முழு)color->red, (முழு)color->green, (முழு)color->blue,
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
    வெறுமை
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    முழு		max_height = 0;
    முழு		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    எனில் (term->tl_vterm == வெற்று)
    {
	emsg(_(e_job_already_finished));
	திரும்பு;
    }

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	எனில் (check_for_dict_arg(argvars, 2) == FAIL)
	    திரும்பு;
	d = argvars[2].vval.v_dict;
	எனில் (d != வெற்று)
	{
	    max_height = dict_get_எண்(d, "rows");
	    max_width = dict_get_எண்(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    எனில் (fname == வெற்று)
	திரும்பு;
    எனில் (mch_stat((எழுத்து *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	திரும்பு;
    }

    எனில் (*fname == NUL || (fd = mch_fopen((எழுத்து *)fname, WRITEBIN)) == வெற்று)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	திரும்பு;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    க்கு (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	முழு		repeat = 0;

	க்கு (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    முழு		    same_attr;
	    முழு		    same_chars = TRUE;
	    முழு		    i;
	    முழு		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    எனில் (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    க்கு (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		முழு c = cell.chars[i];
		முழு pc = prev_cell.chars[i];
		முழு should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		எனில் (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		எனில் (c != pc)
		    same_chars = FALSE;
		எனில் (should_break)
		    நிறுத்து;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    எனில் (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    இல்லையெனில்
	    {
		எனில் (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		எனில் (cell.chars[0] == NUL)
		    fputs(" ", fd);
		இல்லையெனில்
		{
		    char_u	charbuf[10];
		    முழு		len;

		    க்கு (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		எனில் (cell.width != prev_cell.width || !same_attr)
		{
		    எனில் (cell.width == 2)
			fputs("*", fd);
		    இல்லையெனில்
			fputs("+", fd);

		    எனில் (same_attr)
		    {
			fputs("&", fd);
		    }
		    இல்லையெனில்
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			எனில் (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			இல்லையெனில்
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			எனில் (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			இல்லையெனில்
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    எனில் (cell.width == 2)
		++pos.col;
	}
	எனில் (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    நிலையான வெறுமை
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    நிலையான வெறுமை
append_cell(garray_T *gap, cellattr_T *cell)
{
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    நிலையான வெறுமை
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
    நிலையான முழு
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    முழு		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {வெற்று, 0};
    முழு		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    முழு		    max_cells = 0;
    முழு		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, அளவு(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    க்கு (;;)
    {
	எனில் (c == EOF)
	    நிறுத்து;
	எனில் (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	இல்லையெனில் எனில் (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    எனில் (ga_text.ga_data == வெற்று)
		dump_is_corrupt(&ga_text);
	    எனில் (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_வரி_T   *வரி = (sb_வரி_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		எனில் (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		வரி->sb_cols = ga_cell.ga_len;
		வரி->sb_bytes = ga_text.ga_len;
		வரி->sb_cells = ga_cell.ga_data;
		வரி->sb_fill_attr = term->tl_default_color;
		வரி->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_வரி_மொத்தம், ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    இல்லையெனில்
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	இல்லையெனில் எனில் (c == '|' || c == '>')
	{
	    முழு prev_len = ga_text.ga_len;

	    எனில் (c == '>')
	    {
		எனில் (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    எனில் (c != EOF)
		ga_append(&ga_text, c);
	    க்கு (;;)
	    {
		c = fgetc(fd);
		எனில் (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    நிறுத்து;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    எனில் (ga_text.ga_data != வெற்று)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		எனில் (prev_char.string == வெற்று)
		    prev_char.length = 0;
	    }

	    எனில் (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    இல்லையெனில் எனில் (c == '+' || c == '*')
	    {
		முழு is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		எனில் (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		இல்லையெனில் எனில் (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    போது (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    க்கு (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			எனில் (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			இல்லையெனில் எனில் (c == '#')
			{
			    முழு red, green, blue, index = 0, type;

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
			    எனில் (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    போது (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    எனில் (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				எனில் (index == 0)
				{
				    எனில் (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    இல்லையெனில்
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    இல்லையெனில்
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    எனில் (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    இல்லையெனில்
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			இல்லையெனில்
			    dump_is_corrupt(&ga_text);
		    }
		}
		இல்லையெனில்
		    dump_is_corrupt(&ga_text);
	    }
	    இல்லையெனில்
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    எனில் (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	இல்லையெனில் எனில் (c == '@')
	{
	    எனில் (prev_char.string == வெற்று)
		dump_is_corrupt(&ga_text);
	    இல்லையெனில்
	    {
		முழு மொத்தம் = 0;

		// repeat previous character, get the count
		க்கு (;;)
		{
		    c = fgetc(fd);
		    எனில் (!SAFE_isdigit(c))
			நிறுத்து;
		    மொத்தம் = மொத்தம் * 10 + (c - '0');
		}

		போது (மொத்தம்-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	இல்லையெனில்
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    எனில் (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_வரி_மொத்தம், ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    திரும்பு max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    நிலையான char_u *
get_separator(முழு text_width, char_u *fname)
{
    முழு	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    முழு	    fname_size;
    char_u  *p = fname;
    முழு	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    எனில் (textline == வெற்று)
	திரும்பு வெற்று;

    fname_size = vim_strsize(fname);
    எனில் (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    இல்லையெனில் எனில் (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    போது (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    க்கு (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    க்கு (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    திரும்பு textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    நிலையான வெறுமை
term_load_dump(typval_T *argvars, typval_T *rettv, முழு do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = வெற்று;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = வெற்று;
    char_u	*fname_tofree = வெற்று;
    FILE	*fd1;
    FILE	*fd2 = வெற்று;
    char_u	*textline = வெற்று;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    எனில் (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    எனில் (fname1 == வெற்று || (do_diff && fname2 == வெற்று))
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }
    fd1 = mch_fopen((எழுத்து *)fname1, READBIN);
    எனில் (fd1 == வெற்று)
    {
	semsg(_(e_cant_read_file_str), fname1);
	திரும்பு;
    }
    எனில் (do_diff)
    {
	fd2 = mch_fopen((எழுத்து *)fname2, READBIN);
	எனில் (fd2 == வெற்று)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    திரும்பு;
	}
    }

    init_job_options(&opt);
    எனில் (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	செல் theend;

    எனில் (opt.jo_term_name == வெற்று)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	எனில் (fname_tofree != வெற்று)
	{
	    vim_snprintf((எழுத்து *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    எனில் (opt.jo_bufnr_buf != வெற்று)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	எனில் (wp == வெற்று)
	    semsg(_(e_invalid_argument_str), "bufnr");
	இல்லையெனில்
	{
	    buf = curbuf;
	    போது (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    இல்லையெனில்
	// Create a new terminal window.
	buf = term_start(&argvars[0], வெற்று, &opt, TERM_START_NOJOB);

    எனில் (buf != வெற்று && buf->b_term != வெற்று)
    {
	முழு		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	முழு		width;
	முழு		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_எண் = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	எனில் (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	எனில் (!do_diff)
	    செல் theend;

	term->tl_உச்சி_diff_rows = curbuf->b_ml.ml_வரி_மொத்தம்;

	textline = get_separator(width, fname1);
	எனில் (textline == வெற்று)
	    செல் theend;
	எனில் (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_வரி_மொத்தம், textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	எனில் (textline == வெற்று)
	    செல் theend;
	எனில் (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_வரி_மொத்தம், textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	width2 = read_dump_file(fd2, &cursor_pos2);
	எனில் (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    எனில் (textline == வெற்று)
		செல் theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_வரி_மொத்தம் - bot_lnum;

	க்கு (lnum = 1; lnum <= term->tl_உச்சி_diff_rows; ++lnum)
	{
	    எனில் (lnum + bot_lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	    {
		// bottom part has fewer rows, fill with "-"
		க்கு (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    இல்லையெனில்
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		முழு	col;
		sb_வரி_T   *sb_வரி = (sb_வரி_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_வரி + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_வரி + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		எனில் (line1 == வெற்று)
		    நிறுத்து;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		க்கு (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    முழு len1 = utfc_ptr2len(p1);
		    முழு len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    எனில் (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    இல்லையெனில் எனில் (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    இல்லையெனில் எனில் (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    இல்லையெனில் எனில் (cellattr1 != வெற்று && cellattr2 != வெற்று)
		    {
			எனில் ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			இல்லையெனில் எனில் (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			இல்லையெனில் எனில் (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			இல்லையெனில் எனில் (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		போது (col < width)
		{
		    எனில் (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    இல்லையெனில் எனில் (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    இல்லையெனில்
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    எனில் (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_உச்சி_diff_rows) == OK)
		ml_append(term->tl_உச்சி_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	போது (lnum + bot_lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    // bottom part has more rows, fill with "+"
	    க்கு (i = 0; i < width; ++i)
		textline[i] = '+';
	    எனில் (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_உச்சி_diff_rows) == OK)
		ml_append(term->tl_உச்சி_diff_rows + lnum, textline, 0, FALSE);
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
    எனில் (fd2 != வெற்று)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    முழு
term_swap_diff(வெறுமை)
{
    term_T	*term = curbuf->b_term;
    linenr_T	வரி_மொத்தம்;
    linenr_T	உச்சி_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_வரி_T	*sb_வரி;

    எனில் (term == வெற்று
	    || !term_is_finished(curbuf)
	    || term->tl_உச்சி_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	திரும்பு FAIL;

    வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;
    உச்சி_rows = term->tl_உச்சி_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = வரி_மொத்தம் - bot_rows;
    sb_வரி = (sb_வரி_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    க்கு (lnum = 1; lnum <= உச்சி_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	எனில் (p == வெற்று)
	    திரும்பு OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    க்கு (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	எனில் (p == வெற்று)
	    திரும்பு OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    எனில் (p == வெற்று)
	திரும்பு OK;
    ml_append(வரி_மொத்தம் - உச்சி_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(வரி_மொத்தம் - உச்சி_rows));
    எனில் (p == வெற்று)
	திரும்பு OK;
    ml_delete(வரி_மொத்தம் - உச்சி_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    எனில் (உச்சி_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	க்கு (lnum = 0; lnum < உச்சி_rows; ++lnum)
	{
	    sb_வரி_T	temp;

	    temp = *(sb_வரி + lnum);
	    *(sb_வரி + lnum) = *(sb_வரி + bot_start + lnum);
	    *(sb_வரி + bot_start + lnum) = temp;
	}
    }
    இல்லையெனில்
    {
	size_t		size = அளவு(sb_வரி_T) * term->tl_scrollback.ga_len;
	sb_வரி_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	எனில் (temp != வெற்று)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    அளவு(sb_வரி_T) * bot_rows);
	    mch_memmove((sb_வரி_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + உச்சி_rows,
		    அளவு(sb_வரி_T) * (வரி_மொத்தம் - உச்சி_rows - bot_rows));
	    mch_memmove((sb_வரி_T *)term->tl_scrollback.ga_data
						       + வரி_மொத்தம் - உச்சி_rows,
		    temp,
		    அளவு(sb_வரி_T) * உச்சி_rows);
	    vim_free(temp);
	}
    }

    term->tl_உச்சி_diff_rows = bot_rows;
    term->tl_bot_diff_rows = உச்சி_rows;

    update_screen(UPD_NOT_VALID);
    திரும்பு OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    வெறுமை
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    வெறுமை
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    வெறுமை
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    எனில் (buf == வெற்று)
	திரும்பு;
    rettv->vval.v_எண் = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    வெறுமை
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    முழு	    attr;
    size_t  i;
    char_u  *name;

    நிலையான கட்டமைப்பு {
	எழுத்து	    *name;
	முழு	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    attr = tv_get_எண்(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    எனில் (name == வெற்று)
	திரும்பு;

    எனில் (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    க்கு (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	எனில் (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_எண் = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    நிறுத்து;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    வெறுமை
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getcursor()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_எண்(l, term->tl_cursor_pos.row + 1);
    list_append_எண்(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    எனில் (d == வெற்று)
	திரும்பு;

    dict_add_எண்(d, "visible", term->tl_cursor_visible);
    dict_add_எண்(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_எண்(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    எனில் (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    வெறுமை
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getjob()");
    எனில் (buf == வெற்று)
    {
	எனில் (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = வெற்று;
	}
	இல்லையெனில்
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_எண் = VVAL_NULL;
	}
	திரும்பு;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    எனில் (rettv->vval.v_job != வெற்று)
	++rettv->vval.v_job->jv_refcount;
}

    நிலையான முழு
get_row_எண்(typval_T *tv, term_T *term)
{
    எனில் (tv->v_type == VAR_STRING
	    && tv->vval.v_string != வெற்று
	    && STRCMP(tv->vval.v_string, ".") == 0)
	திரும்பு term->tl_cursor_pos.row;
    திரும்பு (முழு)tv_get_எண்(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    வெறுமை
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    முழு		    row;

    rettv->v_type = VAR_STRING;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_getline()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    row = get_row_எண்(&argvars[1], term);

    எனில் (term->tl_vterm == வெற்று)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	முழு	  sb_row = term->tl_scrollback_scrolled + row;
	sb_வரி_T *வரி;

	எனில் (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    திரும்பு;
	வரி = (sb_வரி_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, வெற்று, &offset);

	// vterm is finished, get the text from the buffer
	எனில் (lnum > 0 && lnum <= buf->b_ml.ml_வரி_மொத்தம்)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    எனில் (STRLEN(p) >= offset + வரி->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, வரி->sb_bytes);
	}
    }
    இல்லையெனில்
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	முழு		len;
	char_u		*p;

	எனில் (row < 0 || row >= term->tl_rows)
	    திரும்பு;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	எனில் (p == வெற்று)
	    திரும்பு;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (எழுத்து *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    வெறுமை
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getscrolled()");
    எனில் (buf == வெற்று)
	திரும்பு;
    rettv->vval.v_எண் = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    வெறுமை
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getsize()");
    எனில் (buf == வெற்று)
	திரும்பு;

    l = rettv->vval.v_list;
    list_append_எண்(l, buf->b_term->tl_rows);
    list_append_எண்(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    வெறுமை
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_setsize()");
    எனில் (buf == வெற்று)
    {
	emsg(_(e_not_terminal_buffer));
	திரும்பு;
    }
    எனில் (buf->b_term->tl_vterm == வெற்று)
	திரும்பு;
    term = buf->b_term;
    rows = tv_get_எண்(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_எண்(&argvars[2]);
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
    வெறுமை
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getstatus()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;

    எனில் (term_job_running(term))
	STRCPY(val, "running");
    இல்லையெனில்
	STRCPY(val, "finished");
    எனில் (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    வெறுமை
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_gettitle()");
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் (buf->b_term->tl_title != வெற்று)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    வெறுமை
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = வெற்று;
    முழு		இலக்கம் = 0;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    எனில் (buf == வெற்று)
	திரும்பு;
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	இலக்கம் = tv_get_bool(&argvars[1]);

    தேர்வு (இலக்கம்)
    {
	நிலை 0:
	    எனில் (buf->b_term->tl_job != வெற்று)
		p = buf->b_term->tl_job->jv_tty_out;
	    நிறுத்து;
	நிலை 1:
	    எனில் (buf->b_term->tl_job != வெற்று)
		p = buf->b_term->tl_job->jv_tty_in;
	    நிறுத்து;
	இயல்பு:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    திரும்பு;
    }
    எனில் (p != வெற்று)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    வெறுமை
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    எனில் (rettv_list_alloc(rettv) == FAIL || first_term == வெற்று)
	திரும்பு;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	எனில் (tp->tl_buffer != வெற்று)
	    எனில் (list_append_எண்(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		திரும்பு;
}

/*
 * "term_scrape(buf, row)" function
 */
    வெறுமை
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = வெற்று;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_வரி_T	    *வரி;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_scrape()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_எண்(&argvars[1], term);

    எனில் (term->tl_vterm != வெற்று)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	எனில் (screen == வெற்று)  // can't really happen
	    திரும்பு;
	p = வெற்று;
	வரி = வெற்று;
    }
    இல்லையெனில்
    {
	முழு	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, வெற்று, &offset);

	எனில் (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_வரி_மொத்தம்)
	    திரும்பு;

	வரி = (sb_வரி_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	எனில் (STRLEN(p) < offset + வரி->sb_bytes)
	    திரும்பு;

	p += offset;
    }

    க்கு (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	முழு		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	முழு		i;

	எனில் (screen == வெற்று)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    எனில் (pos.col >= வரி->sb_cols)
		நிறுத்து;
	    cellattr = வரி->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	இல்லையெனில்
	{
	    VTermScreenCell cell;

	    எனில் (vterm_screen_get_cell(screen, pos, &cell) == 0)
		நிறுத்து;

	    mbslen = 0;
	    க்கு (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		எனில் (cell.chars[i] == 0)
		    நிறுத்து;
		mbslen += (*utf_char2bytes)((முழு)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	எனில் (dcell == வெற்று)
	    நிறுத்து;
	எனில் (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    நிறுத்து;
	}

	dict_add_string_len(dcell, "chars", mbs, (முழு)mbslen);

	rgblen = vim_snprintf_safelen((எழுத்து *)rgb, அளவு(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (முழு)rgblen);
	rgblen = vim_snprintf_safelen((எழுத்து *)rgb, அளவு(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (முழு)rgblen);

	dict_add_எண்(dcell, "attr",
				      cell2attr(term, வெற்று, &attrs, &fg, &bg));
	dict_add_எண்(dcell, "width", width);

	++pos.col;
	எனில் (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    வெறுமை
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*செய்தி;
    term_T	*term;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_sendkeys()");
    எனில் (buf == வெற்று)
	திரும்பு;

    செய்தி = tv_get_string_chk(&argvars[1]);
    எனில் (செய்தி == வெற்று)
	திரும்பு;
    term = buf->b_term;
    எனில் (term->tl_vterm == வெற்று)
	திரும்பு;

    போது (*செய்தி != NUL)
    {
	முழு c;

	எனில் (*செய்தி == K_SPECIAL && செய்தி[1] != NUL && செய்தி[2] != NUL)
	{
	    c = TO_SPECIAL(செய்தி[1], செய்தி[2]);
	    செய்தி += 3;
	}
	இல்லையெனில்
	{
	    c = PTR2CHAR(செய்தி);
	    செய்தி += MB_CPTR2LEN(செய்தி);
	}
	send_சாவிகள்_to_term(term, c, 0, FALSE);
    }
}

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    வெறுமை
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    முழு		index;
    list_T	*list;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = term_get_buf(argvars, "term_getansicolors()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    எனில் (term->tl_vterm == வெற்று)
	திரும்பு;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    க்கு (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((எழுத்து *)hexbuf, அளவு(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	எனில் (list_append_string(list, hexbuf, (முழு)hexbuflen) == FAIL)
	    திரும்பு;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    வெறுமை
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    முழு		n = 0;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_setansicolors()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    எனில் (term->tl_vterm == வெற்று)
	திரும்பு;

    எனில் (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_மதிப்பு_for_argument_str), "\"colors\"");
	திரும்பு;
    }

    எனில் (term->tl_palette == வெற்று)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    எனில் (term->tl_palette == வெற்று)
	திரும்பு;

    FOR_ALL_LIST_பொருட்கள்(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	எனில் (color_name == வெற்று)
	    திரும்பு;

	guicolor = GUI_GET_COLOR(color_name);
	எனில் (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    திரும்பு;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    வெறுமை
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_setapi()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    எனில் (api != வெற்று)
	term->tl_api = vim_strsave(api);
    இல்லையெனில்
	term->tl_api = வெற்று;
}

/*
 * "term_setrestore(buf, command)" function
 */
    வெறுமை
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#எனில் defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_setrestore()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    எனில் (cmd != வெற்று)
	term->tl_command = vim_strsave(cmd);
    இல்லையெனில்
	term->tl_command = வெற்று;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    வெறுமை
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_setkill()");
    எனில் (buf == வெற்று)
	திரும்பு;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    எனில் (how != வெற்று)
	term->tl_kill = vim_strsave(how);
    இல்லையெனில்
	term->tl_kill = வெற்று;
}

/*
 * "term_start(command, options)" function
 */
    வெறுமை
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    எனில் (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    init_job_options(&opt);
    எனில் (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	திரும்பு;

    buf = term_start(&argvars[0], வெற்று, &opt, 0);

    எனில் (buf != வெற்று && buf->b_term != வெற்று)
	rettv->vval.v_எண் = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    வெறுமை
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    buf = term_get_buf(argvars, "term_wait()");
    எனில் (buf == வெற்று)
	திரும்பு;
    எனில் (buf->b_term->tl_job == வெற்று)
    {
	ch_log(வெற்று, "term_wait(): no job to wait for");
	திரும்பு;
    }
    எனில் (buf->b_term->tl_job->jv_channel == வெற்று)
	// channel is closed, nothing to do
	திரும்பு;

    // Get the job status, this will detect a job that finished.
    எனில் (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(வெற்று, "term_wait(): waiting for channel to close");
	போது (buf->b_term != வெற்று && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    எனில் (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		நிறுத்து;
	    எனில் (buf->b_term == வெற்று || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		நிறுத்து;
	}

	term_flush_messages();
    }
    இல்லையெனில்
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_எண்(&argvars[1]);
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
    வெறுமை
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	எனில் (term->tl_job == ch->ch_job)
	{
	    எனில் (term->tl_eof_chars != வெற்று)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(முழு)STRLEN(term->tl_eof_chars), வெற்று);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, வெற்று);
	    }
#ifdef MSWIN
	    இல்லையெனில்
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, வெற்று);
#endif
	}
}

#எனில் defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    திரும்பு term != வெற்று ? term->tl_job : வெற்று;
}
#endif

#எனில் defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
வெறுமை (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    நிலையான முழு
dyn_conpty_init(முழு verbose)
{
    நிலையான HMODULE	hKerneldll = வெற்று;
    முழு			i;
    நிலையான கட்டமைப்பு
    {
	எழுத்து	*name;
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
	{வெற்று, வெற்று}
    };

    எனில் (!has_conpty_working())
    {
	எனில் (verbose)
	    emsg(_(e_conpty_is_not_available));
	திரும்பு FAIL;
    }

    // No need to initialize twice.
    எனில் (hKerneldll)
	திரும்பு OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    க்கு (i = 0; conpty_entry[i].name != வெற்று
					&& conpty_entry[i].ptr != வெற்று; ++i)
    {
	எனில் ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == வெற்று)
	{
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = வெற்று;
	    திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

    நிலையான முழு
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	எழுத்து	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = வெற்று;
    WCHAR	    *cmd_wchar_copy = வெற்று;
    WCHAR	    *cwd_wchar = வெற்று;
    WCHAR	    *env_wchar = வெற்று;
    channel_T	    *channel = வெற்று;
    job_T	    *job = வெற்று;
    HANDLE	    jo = வெற்று;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = வெற்று;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = வெற்று;
    HANDLE	    o_theirs = வெற்று;
    HANDLE	    i_ours = வெற்று;
    HANDLE	    o_ours = வெற்று;
    எழுத்து	    *errmsg = வெற்று;

    ga_init2(&ga_cmd, அளவு(எழுத்து*), 20);
    ga_init2(&ga_env, அளவு(எழுத்து*), 20);

    எனில் (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    இல்லையெனில் எனில் (argvar->v_type == VAR_LIST)
    {
	எனில் (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    செல் failed;
	cmd = ga_cmd.ga_data;
    }
    எனில் (cmd == வெற்று || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	செல் failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, வெற்று);

    எனில் (cmd_wchar != வெற்று)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    எனில் (cmd_wchar == வெற்று)
	செல் failed;
    எனில் (opt->jo_cwd != வெற்று)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, வெற்று);
	எனில் (cwd_wchar == வெற்று)
	    செல் failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    எனில் (!CreatePipe(&i_theirs, &i_ours, வெற்று, 0))
	செல் failed;
    எனில் (!CreatePipe(&o_ours, &o_theirs, வெற்று, 0))
	செல் failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    எனில் (FAILED(hr))
	செல் failed;

    term->tl_siex.StartupInfo.cb = அளவு(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_உள்ளீடு = INVALID_HANDLE_மதிப்பு;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_மதிப்பு;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_மதிப்பு;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(வெற்று, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    எனில் (!term->tl_siex.lpAttributeList)
	செல் failed;
    எனில் (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	செல் failed;
    எனில் (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    அளவு(HPCON), வெற்று, வெற்று))
	செல் failed;

    channel = add_channel();
    எனில் (channel == வெற்று)
	செல் failed;

    job = job_alloc();
    எனில் (job == வெற்று)
	செல் failed;
    எனில் (argvar->v_type == VAR_STRING)
    {
	முழு argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    இல்லையெனில்
    {
	முழு argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    எனில் (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    எனில் (!CreateProcessW(வெற்று, cmd_wchar_copy, வெற்று, வெற்று, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	செல் failed;
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

    jo = CreateJobObject(வெற்று, வெற்று);
    எனில் (jo == வெற்று)
	செல் failed;

    எனில் (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = வெற்று;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    எனில் (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	செல் failed;

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    எனில் (term_use_palette())
    {
	எனில் (term->tl_palette != வெற்று)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	இல்லையெனில்
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
    எனில் (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((எழுத்து *)fname, WRITEBIN);
	எனில் (term->tl_out_fd == வெற்று)
	    semsg(_(e_cant_open_file_str), fname);
    }

    திரும்பு OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    எனில் (channel != வெற்று)
	channel_clear(channel);
    எனில் (job != வெற்று)
    {
	job->jv_channel = வெற்று;
	job_cleanup(job);
    }
    term->tl_job = வெற்று;
    எனில் (jo != வெற்று)
	CloseHandle(jo);

    எனில் (term->tl_siex.lpAttributeList != வெற்று)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = வெற்று;
    எனில் (o_theirs != வெற்று)
	CloseHandle(o_theirs);
    எனில் (o_ours != வெற்று)
	CloseHandle(o_ours);
    எனில் (i_ours != வெற்று)
	CloseHandle(i_ours);
    எனில் (i_theirs != வெற்று)
	CloseHandle(i_theirs);
    எனில் (term->tl_conpty != வெற்று)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = வெற்று;
    // Propagate errors that occur in CreateProcess
    எனில் (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    திரும்பு FAIL;
}

    நிலையான வெறுமை
conpty_term_report_winsize(term_T *term, முழு rows, முழு cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    நிலையான வெறுமை
term_free_conpty(term_T *term)
{
    எனில் (term->tl_siex.lpAttributeList != வெற்று)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = வெற்று;
    எனில் (term->tl_conpty != வெற்று)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = வெற்று;
}

    முழு
use_conpty(வெறுமை)
{
    திரும்பு has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

வெறுமை* (*winpty_config_new)(UINT64, வெறுமை*);
வெறுமை* (*winpty_open)(வெறுமை*, வெறுமை*);
வெறுமை* (*winpty_spawn_config_new)(UINT64, வெறுமை*, LPCWSTR, வெறுமை*, வெறுமை*, வெறுமை*);
BOOL (*winpty_spawn)(வெறுமை*, வெறுமை*, HANDLE*, HANDLE*, DWORD*, வெறுமை*);
வெறுமை (*winpty_config_set_mouse_mode)(வெறுமை*, முழு);
வெறுமை (*winpty_config_set_initial_size)(வெறுமை*, முழு, முழு);
LPCWSTR (*winpty_conin_name)(வெறுமை*);
LPCWSTR (*winpty_conout_name)(வெறுமை*);
LPCWSTR (*winpty_conerr_name)(வெறுமை*);
வெறுமை (*winpty_free)(வெறுமை*);
வெறுமை (*winpty_config_free)(வெறுமை*);
வெறுமை (*winpty_spawn_config_free)(வெறுமை*);
வெறுமை (*winpty_error_free)(வெறுமை*);
LPCWSTR (*winpty_error_செய்தி)(வெறுமை*);
BOOL (*winpty_set_size)(வெறுமை*, முழு, முழு, வெறுமை*);
HANDLE (*winpty_agent_process)(வெறுமை*);

# define WINPTY_DLL "winpty.dll"

நிலையான HINSTANCE hWinPtyDLL = வெற்று;

    நிலையான முழு
dyn_winpty_init(முழு verbose)
{
    முழு i;
    நிலையான கட்டமைப்பு
    {
	எழுத்து	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_செய்தி},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{வெற்று, வெற்று}
    };

    // No need to initialize twice.
    எனில் (hWinPtyDLL)
	திரும்பு OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    எனில் (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((எழுத்து *)p_winptydll);
    எனில் (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    எனில் (!hWinPtyDLL)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	திரும்பு FAIL;
    }
    க்கு (i = 0; winpty_entry[i].name != வெற்று
					 && winpty_entry[i].ptr != வெற்று; ++i)
    {
	எனில் ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == வெற்று)
	{
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = வெற்று;
	    திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

    நிலையான முழு
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	எழுத்து	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = வெற்று;
    WCHAR	    *cwd_wchar = வெற்று;
    WCHAR	    *env_wchar = வெற்று;
    channel_T	    *channel = வெற்று;
    job_T	    *job = வெற்று;
    DWORD	    error;
    HANDLE	    jo = வெற்று;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    வெறுமை	    *winpty_err = வெற்று;
    வெறுமை	    *spawn_config = வெற்று;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = வெற்று;

    ga_init2(&ga_cmd, அளவு(எழுத்து*), 20);
    ga_init2(&ga_env, அளவு(எழுத்து*), 20);

    எனில் (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    இல்லையெனில் எனில் (argvar->v_type == VAR_LIST)
    {
	எனில் (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    செல் failed;
	cmd = ga_cmd.ga_data;
    }
    எனில் (cmd == வெற்று || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	செல் failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, வெற்று);
    ga_clear(&ga_cmd);
    எனில் (cmd_wchar == வெற்று)
	செல் failed;
    எனில் (opt->jo_cwd != வெற்று)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, வெற்று);
	எனில் (cwd_wchar == வெற்று)
	    செல் failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    எனில் (term->tl_winpty_config == வெற்று)
	செல் failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    எனில் (term->tl_winpty == வெற்று)
	செல் failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    வெற்று,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    எனில் (spawn_config == வெற்று)
	செல் failed;

    channel = add_channel();
    எனில் (channel == வெற்று)
	செல் failed;

    job = job_alloc();
    எனில் (job == வெற்று)
	செல் failed;
    எனில் (argvar->v_type == VAR_STRING)
    {
	முழு argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    இல்லையெனில்
    {
	முழு argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    எனில் (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    எனில் (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	செல் failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, வெற்று,
	    OPEN_EXISTING, 0, வெற்று),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, வெற்று,
	    OPEN_EXISTING, 0, வெற்று),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, வெற்று,
	    OPEN_EXISTING, 0, வெற்று));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(வெற்று, வெற்று);
    எனில் (jo == வெற்று)
	செல் failed;

    எனில் (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = வெற்று;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    எனில் (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	செல் failed;

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    எனில் (term_use_palette())
    {
	எனில் (term->tl_palette != வெற்று)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	இல்லையெனில்
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
	    (short_u *)winpty_conin_name(term->tl_winpty), வெற்று);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), வெற்று);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    எனில் (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((எழுத்து *)fname, WRITEBIN);
	எனில் (term->tl_out_fd == வெற்று)
	    semsg(_(e_cant_open_file_str), fname);
    }

    திரும்பு OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    எனில் (spawn_config != வெற்று)
	winpty_spawn_config_free(spawn_config);
    எனில் (channel != வெற்று)
	channel_clear(channel);
    எனில் (job != வெற்று)
    {
	job->jv_channel = வெற்று;
	job_cleanup(job);
    }
    term->tl_job = வெற்று;
    எனில் (jo != வெற்று)
	CloseHandle(jo);
    எனில் (term->tl_winpty != வெற்று)
	winpty_free(term->tl_winpty);
    term->tl_winpty = வெற்று;
    எனில் (term->tl_winpty_config != வெற்று)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = வெற்று;
    எனில் (winpty_err != வெற்று)
    {
	எழுத்து *செய்தி = (எழுத்து *)utf16_to_enc(
				(short_u *)winpty_error_செய்தி(winpty_err), வெற்று);

	எனில் (செய்தி != வெற்று)
	{
	    emsg(செய்தி);
	    vim_free(செய்தி);
	}
	winpty_error_free(winpty_err);
    }
    திரும்பு FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    நிலையான முழு
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	எழுத்து	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    முழு		    use_winpty = FALSE;
    முழு		    use_conpty = FALSE;
    முழு		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    எனில் (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	திரும்பு dyn_winpty_init(TRUE);

    எனில் (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    எனில் (tty_type == NUL)
    {
	எனில் (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	இல்லையெனில் எனில் (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    இல்லையெனில் எனில் (tty_type == 'w')	// winpty
    {
	எனில் (has_winpty)
	    use_winpty = TRUE;
    }
    இல்லையெனில் எனில் (tty_type == 'c')	// conpty
    {
	எனில் (has_conpty)
	    use_conpty = TRUE;
	இல்லையெனில்
	    திரும்பு dyn_conpty_init(TRUE);
    }

    எனில் (use_conpty)
	திரும்பு conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    எனில் (use_winpty)
	திரும்பு winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    திரும்பு dyn_winpty_init(TRUE);
}

    நிலையான முழு
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_மதிப்பு;
    HANDLE	    hPipeOut = INVALID_HANDLE_மதிப்பு;
    எழுத்து	    in_name[80], out_name[80];
    channel_T	    *channel = வெற்று;

    எனில் (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	திரும்பு FAIL;

    vim_snprintf(in_name, அளவு(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, வெற்று);
    எனில் (hPipeIn == INVALID_HANDLE_மதிப்பு)
	செல் failed;

    vim_snprintf(out_name, அளவு(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, வெற்று);
    எனில் (hPipeOut == INVALID_HANDLE_மதிப்பு)
	செல் failed;

    ConnectNamedPipe(hPipeIn, வெற்று);
    ConnectNamedPipe(hPipeOut, வெற்று);

    term->tl_job = job_alloc();
    எனில் (term->tl_job == வெற்று)
	செல் failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    எனில் (channel == வெற்று)
	செல் failed;
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

    திரும்பு OK;

failed:
    எனில் (hPipeIn != வெற்று)
	CloseHandle(hPipeIn);
    எனில் (hPipeOut != வெற்று)
	CloseHandle(hPipeOut);
    திரும்பு FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    நிலையான வெறுமை
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    எனில் (term->tl_winpty != வெற்று)
	winpty_free(term->tl_winpty);
    term->tl_winpty = வெற்று;
    எனில் (term->tl_winpty_config != வெற்று)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = வெற்று;
    எனில் (term->tl_vterm != வெற்று)
	vterm_free(term->tl_vterm);
    term->tl_vterm = வெற்று;
}

/*
 * Report the size to the terminal.
 */
    நிலையான வெறுமை
term_report_winsize(term_T *term, முழு rows, முழு cols)
{
    எனில் (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    எனில் (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, வெற்று);
}

    முழு
terminal_enabled(வெறுமை)
{
    திரும்பு dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#இல்லையெனில்

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    நிலையான முழு
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	எழுத்து	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = வெற்று;

    எனில் (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	திரும்பு FAIL;

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    எனில் (term_use_palette())
    {
	எனில் (term->tl_palette != வெற்று)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	இல்லையெனில்
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    எனில் (term->tl_job != வெற்று)
	++term->tl_job->jv_refcount;

    திரும்பு term->tl_job != வெற்று
	&& term->tl_job->jv_channel != வெற்று
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    நிலையான முழு
create_pty_only(term_T *term, jobopt_T *opt)
{
    எனில் (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	திரும்பு FAIL;

    term->tl_job = job_alloc();
    எனில் (term->tl_job == வெற்று)
	திரும்பு FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    திரும்பு mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    நிலையான வெறுமை
term_free_vterm(term_T *term)
{
    எனில் (term->tl_vterm != வெற்று)
	vterm_free(term->tl_vterm);
    term->tl_vterm = வெற்று;
}

/*
 * Report the size to the terminal.
 */
    நிலையான வெறுமை
term_report_winsize(term_T *term, முழு rows, முழு cols)
{
    // Use an ioctl() to report the new window size to the job.
    எனில் (term->tl_job == வெற்று || term->tl_job->jv_channel == வெற்று)
	திரும்பு;

    முழு fd = -1;
    முழு part;

    க்கு (part = PART_OUT; part < PART_மொத்தம்; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	எனில் (mch_isatty(fd))
	    நிறுத்து;
    }
    எனில் (part < PART_மொத்தம் && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
