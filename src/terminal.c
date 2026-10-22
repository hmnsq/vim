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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#അന്തര്ഭാവ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
തരനിർവചനം ഘടന {
    VTermScreenCellAttrs	attrs;
    അക്ഷരം			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

തരനിർവചനം ഘടന sb_വരി_S {
    പൂർണ്ണസംഖ്യ		sb_cols;	// can differ per line
    പൂർണ്ണസംഖ്യ		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_വരി_T;

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
തരനിർവചനം ഘടന _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
ഘടന terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#എങ്കിൽ defined(FEAT_GUI)
    പൂർണ്ണസംഖ്യ		tl_system;	// when non-zero used for :!cmd output
    പൂർണ്ണസംഖ്യ		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    പൂർണ്ണസംഖ്യ		tl_vterm_size_changed;

    പൂർണ്ണസംഖ്യ		tl_normal_mode; // TRUE: Terminal-Normal mode
    പൂർണ്ണസംഖ്യ		tl_channel_closing;
    പൂർണ്ണസംഖ്യ		tl_channel_closed;
    പൂർണ്ണസംഖ്യ		tl_channel_recently_closed; // still need to handle tl_finish

    പൂർണ്ണസംഖ്യ		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    ശൂന്യം	*tl_winpty_config;
    ശൂന്യം	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#എങ്കിൽ defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    പൂർണ്ണസംഖ്യ		tl_rows;
    പൂർണ്ണസംഖ്യ		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    പൂർണ്ണസംഖ്യ		tl_dirty_row_start; // MAX_ROW if nothing dirty
    പൂർണ്ണസംഖ്യ		tl_dirty_row_end;   // row below last one to update
    പൂർണ്ണസംഖ്യ		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    പൂർണ്ണസംഖ്യ		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    പൂർണ്ണസംഖ്യ		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    പൂർണ്ണസംഖ്യ		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    പൂർണ്ണസംഖ്യ		tl_scrollback_snapshot;
    പൂർണ്ണസംഖ്യ		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_മുകൾ_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    പൂർണ്ണസംഖ്യ		tl_cursor_visible;
    പൂർണ്ണസംഖ്യ		tl_cursor_blink;
    പൂർണ്ണസംഖ്യ		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    പൂർണ്ണസംഖ്യ		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
സ്ഥിതം term_T *first_term = ശൂന്യ;

// Terminal active in terminal_loop().
സ്ഥിതം term_T *in_terminal_loop = ശൂന്യ;

#ifdef MSWIN
സ്ഥിതം BOOL has_winpty = FALSE;
സ്ഥിതം BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define താക്കോൽ_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    വേണ്ടി ((term) = first_term; (term) != ശൂന്യ; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ term_and_job_init(term_T *term, typval_T *argvar, അക്ഷരം **argv, jobopt_T *opt, jobopt_T *orig_opt);
സ്ഥിതം പൂർണ്ണസംഖ്യ create_pty_only(term_T *term, jobopt_T *opt);
സ്ഥിതം ശൂന്യം term_report_winsize(term_T *term, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols);
സ്ഥിതം ശൂന്യം term_free_vterm(term_T *term);
#ifdef FEAT_GUI
സ്ഥിതം ശൂന്യം update_system_term(term_T *term);
#endif

സ്ഥിതം ശൂന്യം handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
സ്ഥിതം പൂർണ്ണസംഖ്യ term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
സ്ഥിതം char_u	*last_set_cursor_color = ശൂന്യ;
സ്ഥിതം char_u	*desired_cursor_color = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	last_set_cursor_shape = -1;
സ്ഥിതം പൂർണ്ണസംഖ്യ	desired_cursor_shape = -1;
സ്ഥിതം പൂർണ്ണസംഖ്യ	last_set_cursor_blink = -1;
സ്ഥിതം പൂർണ്ണസംഖ്യ	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    എങ്കിൽ (lhs_color != ശൂന്യ && rhs_color != ശൂന്യ)
	തിരികെ STRCMP(lhs_color, rhs_color) == 0;
    തിരികെ lhs_color == ശൂന്യ && rhs_color == ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    എങ്കിൽ (cursor_color_equal(*to_color, from_color))
	തിരികെ;
    vim_free(*to_color);
    *to_color = (from_color == ശൂന്യ) ? ശൂന്യ : vim_strsave(from_color);
}

    സ്ഥിതം char_u *
cursor_color_get(char_u *color)
{
    തിരികെ (color == ശൂന്യ) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_termwinsize(win_T *wp, പൂർണ്ണസംഖ്യ *rows, പൂർണ്ണസംഖ്യ *cols)
{
    പൂർണ്ണസംഖ്യ	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    എങ്കിൽ (*wp->w_p_tws == NUL)
	തിരികെ FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    എങ്കിൽ (p == ശൂന്യ)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((അക്ഷരം *)wp->w_p_tws);
    *cols = atoi((അക്ഷരം *)p + 1);
    എങ്കിൽ (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    എങ്കിൽ (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    തിരികെ minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    സ്ഥിതം ശൂന്യം
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    പൂർണ്ണസംഖ്യ rows, cols;
    പൂർണ്ണസംഖ്യ	minsize;

#ifdef FEAT_GUI
    എങ്കിൽ (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	തിരികെ;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    എങ്കിൽ (minsize)
    {
	എങ്കിൽ (term->tl_rows < rows)
	    term->tl_rows = rows;
	എങ്കിൽ (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    എങ്കിൽ ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    അല്ലെങ്കിൽ എങ്കിൽ (rows != 0)
	term->tl_rows = rows;
    എങ്കിൽ ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    അല്ലെങ്കിൽ എങ്കിൽ (cols != 0)
	term->tl_cols = cols;

    എങ്കിൽ (!opt->jo_hidden)
    {
	എങ്കിൽ (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	എങ്കിൽ (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	എങ്കിൽ (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((അക്ഷരം *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_മൂല്യം_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
setup_job_options(jobopt_T *opt, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    എങ്കിൽ (!(opt->jo_set & JO_OUT_IO))
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
    എങ്കിൽ (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    എങ്കിൽ ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    എങ്കിൽ ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    സ്ഥിതം ശൂന്യം
term_flush_messages(ശൂന്യം)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    സ്ഥിതം ശൂന്യം
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    എങ്കിൽ (old_curbuf != ശൂന്യ)
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
	അക്ഷരം	    **argv,
	jobopt_T    *opt,
	പൂർണ്ണസംഖ്യ	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res;
    buf_T	*newbuf;
    പൂർണ്ണസംഖ്യ		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ ശൂന്യ;
    എങ്കിൽ (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_വരി_window));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != ശൂന്യ
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ശൂന്യ
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	തിരികെ ശൂന്യ;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    എങ്കിൽ (term == ശൂന്യ)
	തിരികെ ശൂന്യ;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, വലുപ്പം(sb_വരി_T), 300);
    ga_init2(&term->tl_scrollback_postponed, വലുപ്പം(sb_വരി_T), 300);
    ga_init2(&term->tl_osc_buf, വലുപ്പം(അക്ഷരം), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    എങ്കിൽ (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	എങ്കിൽ (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (do_ecmd(0, ശൂന്യ, ശൂന്യ, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ശൂന്യ, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	എങ്കിൽ (buf == ശൂന്യ || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    തിരികെ ശൂന്യ;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    അല്ലെങ്കിൽ
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	എങ്കിൽ (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_എണ്ണം = 1;
	}
	എങ്കിൽ (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_എണ്ണം = 1;
	}

	പൂർണ്ണസംഖ്യ cmod_split_modified = FALSE;
	എങ്കിൽ (vertical)
	{
	    എങ്കിൽ (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	എങ്കിൽ (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	എങ്കിൽ (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    തിരികെ ശൂന്യ;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    എങ്കിൽ (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	എങ്കിൽ (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	എങ്കിൽ (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

    എങ്കിൽ (opt->jo_term_name != ശൂന്യ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argv != ശൂന്യ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	i;
	size_t	len;
	char_u	*cmd, *p;

	എങ്കിൽ (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    എങ്കിൽ (cmd == ശൂന്യ)
		cmd = (char_u *)"";
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	അല്ലെങ്കിൽ എങ്കിൽ (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ശൂന്യ
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == ശൂന്യ)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	വേണ്ടി (i = 0; p != ശൂന്യ; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    എങ്കിൽ (i == 0)
		vim_snprintf((അക്ഷരം *)p, len, "!%s", cmd);
	    അല്ലെങ്കിൽ
		vim_snprintf((അക്ഷരം *)p, len, "!%s (%d)", cmd, i);
	    എങ്കിൽ (buflist_findname(p) == ശൂന്യ)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		നിർത്തുക;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

    എങ്കിൽ (opt->jo_term_opencmd != ശൂന്യ)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    എങ്കിൽ (opt->jo_eof_chars != ശൂന്യ)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, വലുപ്പം(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    എങ്കിൽ (flags & TERM_START_NOJOB)
	തിരികെ curbuf;

#എങ്കിൽ defined(FEAT_SESSION)
    // Remember the command for the session file.
    എങ്കിൽ (opt->jo_term_norestore || argv != ശൂന്യ)
	term->tl_command = vim_strsave((char_u *)"NONE");
    അല്ലെങ്കിൽ എങ്കിൽ (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	എങ്കിൽ (cmd != ശൂന്യ && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ശൂന്യ
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ഇനങ്ങൾ(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    എങ്കിൽ (s == ശൂന്യ)
		നിർത്തുക;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    എങ്കിൽ (p == ശൂന്യ)
		നിർത്തുക;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	എങ്കിൽ (item == ശൂന്യ)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	അല്ലെങ്കിൽ
	    ga_clear(&ga);
    }
#endif

    എങ്കിൽ (opt->jo_term_kill != ശൂന്യ)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    എങ്കിൽ (opt->jo_term_api != ശൂന്യ)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    അല്ലെങ്കിൽ
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    എങ്കിൽ (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    എങ്കിൽ (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	എങ്കിൽ (term->tl_palette == ശൂന്യ)
	{
	    vim_free(term);
	    തിരികെ ശൂന്യ;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, വലുപ്പം(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    എങ്കിൽ (argv == ശൂന്യ
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ശൂന്യ
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    അല്ലെങ്കിൽ
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    എങ്കിൽ (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	എങ്കിൽ (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = സന്ദേശം_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	എങ്കിൽ (old_curbuf != ശൂന്യ)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| നിവേശം_busy)
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
    അല്ലെങ്കിൽ
    {
	term_close_buffer(curbuf, old_curbuf);
	തിരികെ ശൂന്യ;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ശൂന്യ, ശൂന്യ, FALSE, newbuf);
    എങ്കിൽ (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ശൂന്യ, ശൂന്യ, FALSE, newbuf);
    തിരികെ newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    ശൂന്യം
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    പൂർണ്ണസംഖ്യ		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		scroll_save = സന്ദേശം_scroll;

    സന്ദേശം_scroll = FALSE;		// don't scroll here
    autowrite_all();
    സന്ദേശം_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    ആകുമ്പോൾ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	എങ്കിൽ (ep != ശൂന്യ)
	{
	    എങ്കിൽ (ep < p)
		p = ep;
	    അല്ലെങ്കിൽ
		ep = ശൂന്യ;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((പൂർണ്ണസംഖ്യ)(p - cmd) == വലുപ്പം(name) - 1 \
				 && STRNICMP(cmd, name, വലുപ്പം(name) - 1) == 0)
	എങ്കിൽ (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("kill") && ep != ശൂന്യ)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    എങ്കിൽ (ep != ശൂന്യ)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    അല്ലെങ്കിൽ
		opt.jo_term_api = ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("rows") && ep != ശൂന്യ && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((അക്ഷരം *)ep + 1);
	    p = skiptowhite(cmd);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("cols") && ep != ശൂന്യ && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((അക്ഷരം *)ep + 1);
	    p = skiptowhite(cmd);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (OPTARG_HAS("eof") && ep != ശൂന്യ)
	{
	    char_u *buf = ശൂന്യ;
	    char_u *താക്കോലുകൾ;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    താക്കോലുകൾ = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ശൂന്യ);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(താക്കോലുകൾ);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ tty_type = NUL;

	    p = skiptowhite(cmd);
	    എങ്കിൽ (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str), "type");
		പോകുക theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    പോകുക theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    എങ്കിൽ (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	എങ്കിൽ (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    എങ്കിൽ (eap->addr_എണ്ണം > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_മുകൾ | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_മുകൾ = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    എങ്കിൽ (opt_shell && tofree == ശൂന്യ)
    {
#ifdef UNIX
	അക്ഷരം	**argv = ശൂന്യ;
	char_u	*tofree1 = ശൂന്യ;
	char_u	*tofree2 = ശൂന്യ;

	// :term ++shell command
	എങ്കിൽ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(ശൂന്യ, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	പോകുക theend;
#അല്ലെങ്കിൽ
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	എങ്കിൽ (newcmd == ശൂന്യ)
	    പോകുക theend;
	tofree = newcmd;
	vim_snprintf((അക്ഷരം *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# അല്ലെങ്കിൽ
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	പോകുക theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, ശൂന്യ, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    സ്ഥിതം char_u *
get_terminalopt_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    // Note: Keep this in sync with ex_terminal.
    സ്ഥിതം അക്ഷരം *(p_termopt_മൂല്യങ്ങൾ[]) =
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

    എങ്കിൽ (idx < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_termopt_മൂല്യങ്ങൾ))
	തിരികെ (char_u*)p_termopt_മൂല്യങ്ങൾ[idx];
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം char_u *
get_termkill_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    സ്ഥിതം അക്ഷരം *(p_termkill_മൂല്യങ്ങൾ[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    എങ്കിൽ (idx < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_termkill_മൂല്യങ്ങൾ))
	തിരികെ (char_u*)p_termkill_മൂല്യങ്ങൾ[idx];
    തിരികെ ശൂന്യ;
}

/*
 * Command-line expansion for :terminal [options]
 */
    പൂർണ്ണസംഖ്യ
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	പൂർണ്ണസംഖ്യ	    *അക്കം_Matches)
{
    എങ്കിൽ (xp->xp_pattern > xp->xp_വരി && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, പൂർണ്ണസംഖ്യ) = ശൂന്യ;

	char_u *name_end = xp->xp_pattern - 1;
	എങ്കിൽ (name_end - xp->xp_വരി >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	എങ്കിൽ (cb != ശൂന്യ)
	{
	    തിരികെ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    അക്കം_Matches,
		    cb,
		    FALSE);
	}
	തിരികെ FAIL;
    }
    തിരികെ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    അക്കം_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#എങ്കിൽ defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    പൂർണ്ണസംഖ്യ
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    സ്ഥിരം പൂർണ്ണസംഖ്യ	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    എങ്കിൽ (terminal_bufs != ശൂന്യ && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	അക്ഷരം	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, വലുപ്പം(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	എങ്കിൽ (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    എങ്കിൽ (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		തിരികെ FAIL;
	    തിരികെ put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    എങ്കിൽ (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	തിരികെ FAIL;
# ifdef MSWIN
    എങ്കിൽ (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	തിരികെ FAIL;
# endif
    എങ്കിൽ (term->tl_command != ശൂന്യ)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	എങ്കിൽ (quoted_command == ശൂന്യ)
	    തിരികെ FAIL;

	പൂർണ്ണസംഖ്യ ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((അക്ഷരം *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	എങ്കിൽ (ret)
	    തിരികെ FAIL;
    }
    എങ്കിൽ (put_eol(fd) != OK)
	തിരികെ FAIL;

    എങ്കിൽ (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	തിരികെ FAIL;

    എങ്കിൽ (terminal_bufs != ശൂന്യ && wp->w_buffer->b_nwindows > 1)
    {
	അക്ഷരം *hash_താക്കോൽ = alloc(NUMBUFLEN);

	vim_snprintf(hash_താക്കോൽ, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_താക്കോൽ, "terminal session");
    }

    തിരികെ put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    പൂർണ്ണസംഖ്യ
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    തിരികെ term != ശൂന്യ && (term->tl_command == ശൂന്യ
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    സ്ഥിതം ശൂന്യം
free_scrollback(term_T *term)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_വരി_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    വേണ്ടി (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_വരി_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
സ്ഥിതം term_T	*terminals_to_free = ശൂന്യ;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    ശൂന്യം
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    എങ്കിൽ (term == ശൂന്യ)
	തിരികെ;

    // Unlink the terminal form the list of terminals.
    എങ്കിൽ (first_term == term)
	first_term = term->tl_next;
    അല്ലെങ്കിൽ
	വേണ്ടി (tp = first_term; tp->tl_next != ശൂന്യ; tp = tp->tl_next)
	    എങ്കിൽ (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		നിർത്തുക;
	    }

    എങ്കിൽ (term->tl_job != ശൂന്യ)
    {
	എങ്കിൽ (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, ശൂന്യ, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = ശൂന്യ;
    എങ്കിൽ (in_terminal_loop == term)
	in_terminal_loop = ശൂന്യ;
}

    ശൂന്യം
free_unused_terminals(ശൂന്യം)
{
    ആകുമ്പോൾ (terminals_to_free != ശൂന്യ)
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
	എങ്കിൽ (term->tl_out_fd != ശൂന്യ)
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
    സ്ഥിതം ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < 3; ++i)
    {
	പൂർണ്ണസംഖ്യ fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	എങ്കിൽ (mch_isatty(fd))
	    തിരികെ parts[i];
    }
#endif
    തിരികെ PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    സ്ഥിതം ശൂന്യം
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    അക്ഷരം   buf[താക്കോൽ_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, താക്കോൽ_BUF_LEN);

    എങ്കിൽ (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (പൂർണ്ണസംഖ്യ)curlen, ശൂന്യ);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    സ്ഥിതം ശൂന്യം
term_write_job_output(term_T *term, char_u *സന്ദേശം_arg, size_t len_arg)
{
    char_u	*സന്ദേശം = സന്ദേശം_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    എങ്കിൽ (len > limit)
    {
	char_u *p = സന്ദേശം + len - limit;

	p -= (*mb_head_off)(സന്ദേശം, p);
	len -= p - സന്ദേശം;
	സന്ദേശം = p;
    }

    vterm_നിവേശം_write(vterm, (അക്ഷരം *)സന്ദേശം, len);

    // flush vterm buffer when vterm responded to control sequence
    എങ്കിൽ (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    സ്ഥിതം ശൂന്യം
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (popup_is_popup(wp))
    {
	wp->w_wrow += popup_മുകൾ_extra(wp);
	wp->w_wcol += popup_ഇടത്_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    അല്ലെങ്കിൽ
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    സ്ഥിതം ശൂന്യം
update_cursor(term_T *term, പൂർണ്ണസംഖ്യ redraw)
{
    എങ്കിൽ (term->tl_normal_mode)
	തിരികെ;
#ifdef FEAT_GUI
    എങ്കിൽ (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    അല്ലെങ്കിൽ
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    എങ്കിൽ (redraw)
    {
	aco_save_T	aco;

	എങ്കിൽ (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, ശൂന്യ, ശൂന്യ, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    സ്ഥിതം ശൂന്യം
scrollbackline_pos_in_buf(term_T *term, പൂർണ്ണസംഖ്യ row, linenr_T *lnum, പൂർണ്ണസംഖ്യ *start_col, size_t *start_pos)
{
    sb_വരി_T	*വരികൾ = (sb_വരി_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    പൂർണ്ണസംഖ്യ         calc_col = 0;
    പൂർണ്ണസംഖ്യ         i;

    എങ്കിൽ (row < 0 || row >= term->tl_scrollback.ga_len)
	തിരികെ;

    എങ്കിൽ (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	വേണ്ടി (i = term->tl_scrollback_scrolled + 1; i > 0 && വരികൾ[i].continuation; --i)
	{
	    calc_pos += വരികൾ[i - 1].sb_bytes;
	    calc_col += വരികൾ[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	എങ്കിൽ (വരികൾ[i].continuation)
	    ++i;
    }
    അല്ലെങ്കിൽ
    {
	i = 1;
	calc_lnum = 1;
    }

    വേണ്ടി (; i <= row; ++i)
    {
	എങ്കിൽ (!വരികൾ[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	അല്ലെങ്കിൽ
	{
	    calc_pos += വരികൾ[i - 1].sb_bytes;
	    calc_col += വരികൾ[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    എങ്കിൽ (start_col)
	*start_col = calc_col;
    എങ്കിൽ (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    സ്ഥിതം ശൂന്യം
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ *row, പൂർണ്ണസംഖ്യ *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_വരി_T	*വരികൾ = (sb_വരി_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    പൂർണ്ണസംഖ്യ         calc_col = col;
    linenr_T    l;

    എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ;

    എങ്കിൽ (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	ആകുമ്പോൾ (calc_row < term->tl_scrollback.ga_len && വരികൾ[calc_row].continuation)
	    ++calc_row;
    }
    അല്ലെങ്കിൽ
    {
	calc_row = 0;
	l = 1;
    }

    ആകുമ്പോൾ (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	എങ്കിൽ (!വരികൾ[calc_row].continuation)
	    ++l;
    }

    ആകുമ്പോൾ (calc_row + 1 < term->tl_scrollback.ga_len && വരികൾ[calc_row + 1].continuation
	    && calc_col >= വരികൾ[calc_row].sb_cols)
    {
	calc_col -= വരികൾ[calc_row].sb_cols;
	++calc_row;
    }

    എങ്കിൽ (row)
	*row = calc_row;
    എങ്കിൽ (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    ശൂന്യം
write_to_term(buf_T *buffer, char_u *സന്ദേശം, channel_T *channel)
{
    size_t	len = STRLEN(സന്ദേശം);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    എങ്കിൽ (term->tl_out_fd != ശൂന്യ)
    {
	ch_log(channel, "Writing %d bytes to output file", (പൂർണ്ണസംഖ്യ)len);
	fwrite(സന്ദേശം, len, 1, term->tl_out_fd);
	തിരികെ;
    }
#endif

    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (പൂർണ്ണസംഖ്യ)len);
	തിരികെ;
    }
    ch_log(channel, "writing %d bytes to terminal", (പൂർണ്ണസംഖ്യ)len);
    cursor_off();
    term_write_job_output(term, സന്ദേശം, len);

#ifdef FEAT_GUI
    എങ്കിൽ (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    അല്ലെങ്കിൽ
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    എങ്കിൽ (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	എങ്കിൽ (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#എങ്കിൽ defined(FEAT_TABPANEL)
	    എങ്കിൽ (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    എങ്കിൽ (buffer == curbuf && curbuf->b_term != ശൂന്യ)
		update_cursor(curbuf->b_term, TRUE);
	}
	അല്ലെങ്കിൽ
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_send_mouse(VTerm *vterm, പൂർണ്ണസംഖ്യ button, പൂർണ്ണസംഖ്യ pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    പൂർണ്ണസംഖ്യ		    row = mouse_row - W_WINROW(curwin);
    പൂർണ്ണസംഖ്യ		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (popup_is_popup(curwin))
    {
	row -= popup_മുകൾ_extra(curwin);
	col -= popup_ഇടത്_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    എങ്കിൽ (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    തിരികെ TRUE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ enter_mouse_col = -1;
സ്ഥിതം പൂർണ്ണസംഖ്യ enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_mouse_click(VTerm *vterm, പൂർണ്ണസംഖ്യ താക്കോൽ)
{
#എങ്കിൽ defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    എങ്കിൽ (state == ശൂന്യ)
	തിരികെ FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    എങ്കിൽ (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	തിരഞ്ഞെടുപ്പ് (താക്കോൽ)
	{
	അവസ്ഥ K_LEFTDRAG:
	അവസ്ഥ K_LEFTRELEASE:
	അവസ്ഥ K_RIGHTDRAG:
	അവസ്ഥ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		എങ്കിൽ (ignore_drag_release)
		{
		    പൂർണ്ണസംഖ്യ save_mouse_col, save_mouse_row;

		    എങ്കിൽ (enter_mouse_col < 0)
			നിർത്തുക;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_ഇടത്, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	അവസ്ഥ K_LEFTMOUSE:
	അവസ്ഥ K_RIGHTMOUSE:
		എങ്കിൽ (താക്കോൽ == K_LEFTRELEASE || താക്കോൽ == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		അല്ലെങ്കിൽ
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		എങ്കിൽ (clip_star.available)
		{
		    പൂർണ്ണസംഖ്യ	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(താക്കോൽ),
							 &is_click, &is_drag);
		    എങ്കിൽ (mouse_model_popup() && button == MOUSE_ഇടത്
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_വലത്;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		നിർത്തുക;

	അവസ്ഥ K_MIDDLEMOUSE:
		എങ്കിൽ (clip_star.available)
		    തിരുകുക_reg('*', TRUE);
		നിർത്തുക;
	}
	enter_mouse_col = -1;
	തിരികെ FALSE;
    }
#endif
    enter_mouse_col = -1;

    തിരഞ്ഞെടുപ്പ് (താക്കോൽ)
    {
	അവസ്ഥ K_LEFTMOUSE:
	അവസ്ഥ K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); നിർത്തുക;
	അവസ്ഥ K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); നിർത്തുക;
	അവസ്ഥ K_LEFTRELEASE:
	അവസ്ഥ K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); നിർത്തുക;
	അവസ്ഥ K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); നിർത്തുക;
	അവസ്ഥ K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); നിർത്തുക;
	അവസ്ഥ K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); നിർത്തുക;
	അവസ്ഥ K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); നിർത്തുക;
	അവസ്ഥ K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); നിർത്തുക;
	അവസ്ഥ K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); നിർത്തുക;
	അവസ്ഥ K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); നിർത്തുക;
    }
    തിരികെ TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_convert_താക്കോൽ(term_T *term, പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ modmask, അക്ഷരം *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_താക്കോൽ	    താക്കോൽ = VTERM_താക്കോൽ_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    പൂർണ്ണസംഖ്യ		    other = FALSE;

    തിരഞ്ഞെടുപ്പ് (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	അവസ്ഥ K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    എങ്കിൽ (term->tl_conpty)
		താക്കോൽ = VTERM_താക്കോൽ_BACKSPACE;
	    അല്ലെങ്കിൽ
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    നിർത്തുക;

#ifdef MSWIN
	അവസ്ഥ BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    എങ്കിൽ (term->tl_conpty)
		താക്കോൽ = VTERM_താക്കോൽ_BACKSPACE;
	    നിർത്തുക;
#endif

	അവസ്ഥ ESC:		താക്കോൽ = VTERM_താക്കോൽ_ESCAPE; നിർത്തുക;
	അവസ്ഥ K_DEL:		താക്കോൽ = VTERM_താക്കോൽ_DEL; നിർത്തുക;
	അവസ്ഥ K_DOWN:		താക്കോൽ = VTERM_താക്കോൽ_DOWN; നിർത്തുക;
	അവസ്ഥ K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_DOWN; നിർത്തുക;
	അവസ്ഥ K_END:		താക്കോൽ = VTERM_താക്കോൽ_END; നിർത്തുക;
	അവസ്ഥ K_S_END:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_END; നിർത്തുക;
	അവസ്ഥ K_C_END:		mod = VTERM_MOD_CTRL;
				താക്കോൽ = VTERM_താക്കോൽ_END; നിർത്തുക;
	അവസ്ഥ K_F10:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(10); നിർത്തുക;
	അവസ്ഥ K_F11:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(11); നിർത്തുക;
	അവസ്ഥ K_F12:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(12); നിർത്തുക;
	അവസ്ഥ K_F1:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(1); നിർത്തുക;
	അവസ്ഥ K_F2:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(2); നിർത്തുക;
	അവസ്ഥ K_F3:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(3); നിർത്തുക;
	അവസ്ഥ K_F4:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(4); നിർത്തുക;
	അവസ്ഥ K_F5:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(5); നിർത്തുക;
	അവസ്ഥ K_F6:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(6); നിർത്തുക;
	അവസ്ഥ K_F7:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(7); നിർത്തുക;
	അവസ്ഥ K_F8:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(8); നിർത്തുക;
	അവസ്ഥ K_F9:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(9); നിർത്തുക;
	അവസ്ഥ K_HOME:		താക്കോൽ = VTERM_താക്കോൽ_HOME; നിർത്തുക;
	അവസ്ഥ K_S_HOME:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_HOME; നിർത്തുക;
	അവസ്ഥ K_C_HOME:		mod = VTERM_MOD_CTRL;
				താക്കോൽ = VTERM_താക്കോൽ_HOME; നിർത്തുക;
	അവസ്ഥ K_INS:		താക്കോൽ = VTERM_താക്കോൽ_INS; നിർത്തുക;
	അവസ്ഥ K_K0:		താക്കോൽ = VTERM_താക്കോൽ_KP_0; നിർത്തുക;
	അവസ്ഥ K_K1:		താക്കോൽ = VTERM_താക്കോൽ_KP_1; നിർത്തുക;
	അവസ്ഥ K_K2:		താക്കോൽ = VTERM_താക്കോൽ_KP_2; നിർത്തുക;
	അവസ്ഥ K_K3:		താക്കോൽ = VTERM_താക്കോൽ_KP_3; നിർത്തുക;
	അവസ്ഥ K_K4:		താക്കോൽ = VTERM_താക്കോൽ_KP_4; നിർത്തുക;
	അവസ്ഥ K_K5:		താക്കോൽ = VTERM_താക്കോൽ_KP_5; നിർത്തുക;
	അവസ്ഥ K_K6:		താക്കോൽ = VTERM_താക്കോൽ_KP_6; നിർത്തുക;
	അവസ്ഥ K_K7:		താക്കോൽ = VTERM_താക്കോൽ_KP_7; നിർത്തുക;
	അവസ്ഥ K_K8:		താക്കോൽ = VTERM_താക്കോൽ_KP_8; നിർത്തുക;
	അവസ്ഥ K_K9:		താക്കോൽ = VTERM_താക്കോൽ_KP_9; നിർത്തുക;
	അവസ്ഥ K_KDEL:		താക്കോൽ = VTERM_താക്കോൽ_DEL; നിർത്തുക; // TODO
	അവസ്ഥ K_KDIVIDE:		താക്കോൽ = VTERM_താക്കോൽ_KP_DIVIDE; നിർത്തുക;
	അവസ്ഥ K_KEND:		താക്കോൽ = VTERM_താക്കോൽ_KP_1; നിർത്തുക; // TODO
	അവസ്ഥ K_KENTER:		താക്കോൽ = VTERM_താക്കോൽ_KP_ENTER; നിർത്തുക;
	അവസ്ഥ K_KHOME:		താക്കോൽ = VTERM_താക്കോൽ_KP_7; നിർത്തുക; // TODO
	അവസ്ഥ K_KINS:		താക്കോൽ = VTERM_താക്കോൽ_KP_0; നിർത്തുക; // TODO
	അവസ്ഥ K_KMINUS:		താക്കോൽ = VTERM_താക്കോൽ_KP_MINUS; നിർത്തുക;
	അവസ്ഥ K_KMULTIPLY:	താക്കോൽ = VTERM_താക്കോൽ_KP_MULT; നിർത്തുക;
	അവസ്ഥ K_KPAGEDOWN:	താക്കോൽ = VTERM_താക്കോൽ_KP_3; നിർത്തുക; // TODO
	അവസ്ഥ K_KPAGEUP:		താക്കോൽ = VTERM_താക്കോൽ_KP_9; നിർത്തുക; // TODO
	അവസ്ഥ K_KPLUS:		താക്കോൽ = VTERM_താക്കോൽ_KP_PLUS; നിർത്തുക;
	അവസ്ഥ K_KPOINT:		താക്കോൽ = VTERM_താക്കോൽ_KP_PERIOD; നിർത്തുക;
	അവസ്ഥ K_ഇടത്:		താക്കോൽ = VTERM_താക്കോൽ_ഇടത്; നിർത്തുക;
	അവസ്ഥ K_S_ഇടത്:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_ഇടത്; നിർത്തുക;
	അവസ്ഥ K_C_ഇടത്:		mod = VTERM_MOD_CTRL;
				താക്കോൽ = VTERM_താക്കോൽ_ഇടത്; നിർത്തുക;
	അവസ്ഥ K_PAGEDOWN:	താക്കോൽ = VTERM_താക്കോൽ_PAGEDOWN; നിർത്തുക;
	അവസ്ഥ K_PAGEUP:		താക്കോൽ = VTERM_താക്കോൽ_PAGEUP; നിർത്തുക;
	അവസ്ഥ K_വലത്:		താക്കോൽ = VTERM_താക്കോൽ_വലത്; നിർത്തുക;
	അവസ്ഥ K_S_വലത്:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_വലത്; നിർത്തുക;
	അവസ്ഥ K_C_വലത്:		mod = VTERM_MOD_CTRL;
				താക്കോൽ = VTERM_താക്കോൽ_വലത്; നിർത്തുക;
	അവസ്ഥ K_UP:		താക്കോൽ = VTERM_താക്കോൽ_UP; നിർത്തുക;
	അവസ്ഥ K_S_UP:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_UP; നിർത്തുക;
	അവസ്ഥ TAB:		താക്കോൽ = VTERM_താക്കോൽ_TAB; നിർത്തുക;
	അവസ്ഥ K_S_TAB:		mod = VTERM_MOD_SHIFT;
				താക്കോൽ = VTERM_താക്കോൽ_TAB; നിർത്തുക;

	അവസ്ഥ K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); നിർത്തുക;
	അവസ്ഥ K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); നിർത്തുക;
	അവസ്ഥ K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); നിർത്തുക;
	അവസ്ഥ K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); നിർത്തുക;

	അവസ്ഥ K_LEFTMOUSE:
	അവസ്ഥ K_LEFTMOUSE_NM:
	അവസ്ഥ K_LEFTDRAG:
	അവസ്ഥ K_LEFTRELEASE:
	അവസ്ഥ K_LEFTRELEASE_NM:
	അവസ്ഥ K_MOUSEMOVE:
	അവസ്ഥ K_MIDDLEMOUSE:
	അവസ്ഥ K_MIDDLEDRAG:
	അവസ്ഥ K_MIDDLERELEASE:
	അവസ്ഥ K_RIGHTMOUSE:
	അവസ്ഥ K_RIGHTDRAG:
	അവസ്ഥ K_RIGHTRELEASE:	എങ്കിൽ (!term_mouse_click(vterm, c))
				    തിരികെ 0;
				other = TRUE;
				നിർത്തുക;

	അവസ്ഥ K_X1MOUSE:		/* TODO */ തിരികെ 0;
	അവസ്ഥ K_X1DRAG:		/* TODO */ തിരികെ 0;
	അവസ്ഥ K_X1RELEASE:	/* TODO */ തിരികെ 0;
	അവസ്ഥ K_X2MOUSE:		/* TODO */ തിരികെ 0;
	അവസ്ഥ K_X2DRAG:		/* TODO */ തിരികെ 0;
	അവസ്ഥ K_X2RELEASE:	/* TODO */ തിരികെ 0;

	അവസ്ഥ K_IGNORE:		തിരികെ 0;
	അവസ്ഥ K_NOP:		തിരികെ 0;
	അവസ്ഥ K_UNDO:		തിരികെ 0;
	അവസ്ഥ K_HELP:		തിരികെ 0;
	അവസ്ഥ K_XF1:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(1); നിർത്തുക;
	അവസ്ഥ K_XF2:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(2); നിർത്തുക;
	അവസ്ഥ K_XF3:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(3); നിർത്തുക;
	അവസ്ഥ K_XF4:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(4); നിർത്തുക;
	അവസ്ഥ K_SELECT:		തിരികെ 0;
#ifdef FEAT_GUI
	അവസ്ഥ K_VER_SCROLLBAR:	തിരികെ 0;
	അവസ്ഥ K_HOR_SCROLLBAR:	തിരികെ 0;
#endif
#ifdef FEAT_GUI_TABLINE
	അവസ്ഥ K_TABLINE:		തിരികെ 0;
	അവസ്ഥ K_TABMENU:		തിരികെ 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	അവസ്ഥ K_F21:		താക്കോൽ = VTERM_താക്കോൽ_FUNCTION(21); നിർത്തുക;
#endif
#ifdef FEAT_DND
	അവസ്ഥ K_DROP:		തിരികെ 0;
#endif
	അവസ്ഥ K_CURSORHOLD:	തിരികെ 0;
	അവസ്ഥ K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				നിർത്തുക;
	അവസ്ഥ K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				നിർത്തുക;
    }

    // add modifiers for the typed key
    എങ്കിൽ (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    എങ്കിൽ (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    എങ്കിൽ (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    എങ്കിൽ (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    എങ്കിൽ (താക്കോൽ != VTERM_താക്കോൽ_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_താക്കോൽ(vterm, താക്കോൽ, mod);
    അല്ലെങ്കിൽ എങ്കിൽ (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    തിരികെ (പൂർണ്ണസംഖ്യ)vterm_output_read(vterm, buf, താക്കോൽ_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_job_running_check(term_T *term, പൂർണ്ണസംഖ്യ check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    എങ്കിൽ (term == ശൂന്യ
	|| term->tl_job == ശൂന്യ
	|| !channel_is_open(term->tl_job->jv_channel))
	തിരികെ FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    എങ്കിൽ (check_job_status)
	job_status(job);
    തിരികെ (job->jv_status == JOB_STARTED
	    || (job->jv_channel != ശൂന്യ && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    പൂർണ്ണസംഖ്യ
term_job_running(term_T *term)
{
    തിരികെ term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    പൂർണ്ണസംഖ്യ
term_job_running_not_none(term_T *term)
{
    തിരികെ term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    പൂർണ്ണസംഖ്യ
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    തിരികെ term != ശൂന്യ
	&& term->tl_job != ശൂന്യ
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    പൂർണ്ണസംഖ്യ
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_സന്ദേശം_SIZE];
    പൂർണ്ണസംഖ്യ	ret;

    dialog_സന്ദേശം(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ശൂന്യ, buff, 1);
    എങ്കിൽ (ret == VIM_YES)
	തിരികെ OK;
    അല്ലെങ്കിൽ
	തിരികെ FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    പൂർണ്ണസംഖ്യ
term_try_stop_job(buf_T *buf)
{
    പൂർണ്ണസംഖ്യ	    എണ്ണം;
    അക്ഷരം    *how = (അക്ഷരം *)buf->b_term->tl_kill;

#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    എങ്കിൽ ((how == ശൂന്യ || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	എങ്കിൽ (term_confirm_stop(buf) == OK)
	    how = "kill";
	അല്ലെങ്കിൽ
	    തിരികെ FAIL;
    }
#endif
    എങ്കിൽ (how == ശൂന്യ || *how == NUL)
	തിരികെ FAIL;

    job_stop(buf->b_term->tl_job, ശൂന്യ, how);

    // wait for up to a second for the job to die
    വേണ്ടി (എണ്ണം = 0; എണ്ണം < 100; ++എണ്ണം)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	എങ്കിൽ (!buf_valid(buf)
		|| buf->b_term == ശൂന്യ
		|| buf->b_term->tl_job == ശൂന്യ)
	    തിരികെ OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	എങ്കിൽ (job->jv_status >= JOB_ENDED)
	    തിരികെ OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    തിരികെ FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    സ്ഥിതം ശൂന്യം
add_scrollback_വരി_to_buffer(term_T *term, char_u *text, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ append)
{
    buf_T	*buf = term->tl_buffer;
    പൂർണ്ണസംഖ്യ		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_വരി_എണ്ണം;

#ifdef MSWIN
    char_u *tmp = text;
    എങ്കിൽ (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (അക്ഷരം*)text, len + 1,
							   &ret, &length);
	എങ്കിൽ (ret != ശൂന്യ)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (അക്ഷരം **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    എങ്കിൽ (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	എങ്കിൽ (both == ശൂന്യ)
	    തിരികെ;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    അല്ലെങ്കിൽ
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    എങ്കിൽ (tmp != text)
	vim_free(text);
#endif
    എങ്കിൽ (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    സ്ഥിതം ശൂന്യം
cell2cellattr(സ്ഥിരം VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    തിരികെ a->fg.red == b->fg.red
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, പൂർണ്ണസംഖ്യ lnum)
{
    എങ്കിൽ (ga_grow(&term->tl_scrollback, 1) == FAIL)
	തിരികെ FALSE;

    sb_വരി_T *വരി = (sb_വരി_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    എങ്കിൽ (lnum > 0)
    {
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *വരി = *(വരി - 1);
	    --വരി;
	}
    }
    വരി->sb_cols = 0;
    വരി->sb_bytes = 0;
    വരി->sb_cells = ശൂന്യ;
    വരി->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    തിരികെ OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    സ്ഥിതം ശൂന്യം
cleanup_scrollback(term_T *term)
{
    sb_വരി_T	*വരി;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_വരി_എണ്ണം, FALSE);
    bufline_length = STRLEN(bufline);
    ആകുമ്പോൾ (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	വരി = (sb_വരി_T *)gap->ga_data + gap->ga_len - 1;
	എങ്കിൽ (വരി->sb_bytes < 0 || (size_t)വരി->sb_bytes > bufline_length)
	    നിർത്തുക;
	bufline_length -= വരി->sb_bytes;
	എങ്കിൽ (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_വരി_എണ്ണം);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_വരി_എണ്ണം, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(വരി->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    എങ്കിൽ (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_വരി_എണ്ണം, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    എങ്കിൽ (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    സ്ഥിതം ശൂന്യം
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    പൂർണ്ണസംഖ്യ		    len;
    പൂർണ്ണസംഖ്യ		    വരികൾ_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == ശൂന്യ ? ശൂന്യ : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    വേണ്ടി (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	വേണ്ടി (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    എങ്കിൽ (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    അല്ലെങ്കിൽ
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	എങ്കിൽ (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++വരികൾ_skipped;
	അല്ലെങ്കിൽ
	{
	    ആകുമ്പോൾ (വരികൾ_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--വരികൾ_skipped;
		എങ്കിൽ (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_വരി_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    എങ്കിൽ (len == 0)
		p = ശൂന്യ;
	    അല്ലെങ്കിൽ
		p = ALLOC_MULT(cellattr_T, len);
	    എങ്കിൽ ((p != ശൂന്യ || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		പൂർണ്ണസംഖ്യ	    width;
		സ്ഥിരം VTerm_വരി_Info *lineinfo;
		sb_വരി_T   *വരി = (sb_വരി_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		വേണ്ടി (pos.col = 0; pos.col < len; pos.col += width)
		{
		    എങ്കിൽ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			എങ്കിൽ (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    അല്ലെങ്കിൽ
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			എങ്കിൽ (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			എങ്കിൽ (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    പൂർണ്ണസംഖ്യ	    i;
			    പൂർണ്ണസംഖ്യ	    c;

			    വേണ്ടി (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		വരി->sb_cols = len;
		വരി->sb_bytes = ga.ga_len;
		വരി->sb_cells = p;
		വരി->sb_fill_attr = new_fill_attr;
		വരി->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_വരി_to_buffer(term, (char_u *)"", 0, 0);
		അല്ലെങ്കിൽ
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_വരി_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    അല്ലെങ്കിൽ
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    വേണ്ടി (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	എങ്കിൽ (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_വരി_to_buffer(term, (char_u *)"", 0, 0);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
for_all_windows_and_curwin(win_T **wp, പൂർണ്ണസംഖ്യ *did_curwin)
{
    എങ്കിൽ (*wp == ശൂന്യ)
	*wp = firstwin;
    അല്ലെങ്കിൽ എങ്കിൽ ((*wp)->w_next != ശൂന്യ)
	*wp = (*wp)->w_next;
    അല്ലെങ്കിൽ എങ്കിൽ (!*did_curwin)
	*wp = curwin;
    അല്ലെങ്കിൽ
	തിരികെ FALSE;
    എങ്കിൽ (*wp == curwin)
	*did_curwin = TRUE;
    തിരികെ TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    ശൂന്യം
may_move_terminal_to_buffer(term_T *term, പൂർണ്ണസംഖ്യ redraw)
{
    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	തിരികെ;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    എങ്കിൽ (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_വരി_എണ്ണം
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    എങ്കിൽ (redraw)
    {
	win_T	    *wp = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	    did_curwin = FALSE;

	ആകുമ്പോൾ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    എങ്കിൽ (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_വരി_എണ്ണം;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		എങ്കിൽ (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    എങ്കിൽ (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#എങ്കിൽ defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    പൂർണ്ണസംഖ്യ
term_check_timers(പൂർണ്ണസംഖ്യ next_due_arg, proftime_T *now)
{
    term_T  *term;
    പൂർണ്ണസംഖ്യ	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	എങ്കിൽ (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_ഇടത്(&term->tl_timer_due, now);

	    എങ്കിൽ (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    തിരികെ next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    സ്ഥിതം ശൂന്യം
set_terminal_mode(term_T *term, പൂർണ്ണസംഖ്യ normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    എങ്കിൽ (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    എങ്കിൽ (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    സ്ഥിതം ശൂന്യം
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    എങ്കിൽ (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    സ്ഥിതം ശൂന്യം
term_enter_normal_mode(ശൂന്യം)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    പൂർണ്ണസംഖ്യ        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, ശൂന്യ);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    എങ്കിൽ (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    പൂർണ്ണസംഖ്യ
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    തിരികെ term != ശൂന്യ && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    ശൂന്യം
term_enter_job_mode(ശൂന്യം)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    എങ്കിൽ (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
തരനിർവചനം എണ്ണൽ {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_താക്കോൽ_state_T;

സ്ഥിതം reduce_താക്കോൽ_state_T  no_reduce_താക്കോൽ_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vterm_using_താക്കോൽ_protocol(ശൂന്യം)
{
    തിരികെ curbuf->b_term != ശൂന്യ
	&& curbuf->b_term->tl_vterm != ശൂന്യ
	&& (vterm_is_modify_other_താക്കോലുകൾ(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    ശൂന്യം
check_no_reduce_താക്കോലുകൾ(ശൂന്യം)
{
    എങ്കിൽ (no_reduce_താക്കോൽ_state != NRKS_CHECK
	    || no_reduce_താക്കോലുകൾ >= 1
	    || curbuf->b_term == ശൂന്യ
	    || curbuf->b_term->tl_vterm == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (vterm_using_താക്കോൽ_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_താക്കോൽ_state = NRKS_SET;
	++no_reduce_താക്കോലുകൾ;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_vgetc(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ c;
    പൂർണ്ണസംഖ്യ save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    എങ്കിൽ (vterm_using_താക്കോൽ_protocol())
    {
	++no_reduce_താക്കോലുകൾ;
	no_reduce_താക്കോൽ_state = NRKS_SET;
    }
    അല്ലെങ്കിൽ
    {
	no_reduce_താക്കോൽ_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    എങ്കിൽ (no_reduce_താക്കോൽ_state == NRKS_SET)
	--no_reduce_താക്കോലുകൾ;
    no_reduce_താക്കോൽ_state = NRKS_NONE;

    തിരികെ c;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    പൂർണ്ണസംഖ്യ
send_താക്കോലുകൾ_to_term(term_T *term, പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ modmask, പൂർണ്ണസംഖ്യ typed)
{
    അക്ഷരം	സന്ദേശം[താക്കോൽ_BUF_LEN];
    size_t	len;
    പൂർണ്ണസംഖ്യ		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ NUL:
	അവസ്ഥ K_ZERO:
	    എങ്കിൽ (typed)
		stuffcharReadbuff(c);
	    തിരികെ FAIL;

	അവസ്ഥ K_TABLINE:
	    stuffcharReadbuff(c);
	    തിരികെ FAIL;

	അവസ്ഥ K_IGNORE:
	അവസ്ഥ K_CANCEL:  // used for :normal when running out of chars
	    തിരികെ FAIL;

	അവസ്ഥ K_LEFTDRAG:
	അവസ്ഥ K_MIDDLEDRAG:
	അവസ്ഥ K_RIGHTDRAG:
	അവസ്ഥ K_X1DRAG:
	അവസ്ഥ K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	അവസ്ഥ K_LEFTMOUSE:
	അവസ്ഥ K_LEFTMOUSE_NM:
	അവസ്ഥ K_LEFTRELEASE:
	അവസ്ഥ K_LEFTRELEASE_NM:
	അവസ്ഥ K_MOUSEMOVE:
	അവസ്ഥ K_MIDDLEMOUSE:
	അവസ്ഥ K_MIDDLERELEASE:
	അവസ്ഥ K_RIGHTMOUSE:
	അവസ്ഥ K_RIGHTRELEASE:
	അവസ്ഥ K_X1MOUSE:
	അവസ്ഥ K_X1RELEASE:
	അവസ്ഥ K_X2MOUSE:
	അവസ്ഥ K_X2RELEASE:

	അവസ്ഥ K_MOUSEUP:
	അവസ്ഥ K_MOUSEDOWN:
	അവസ്ഥ K_MOUSELEFT:
	അവസ്ഥ K_MOUSERIGHT:
	    {
		പൂർണ്ണസംഖ്യ	row = mouse_row;
		പൂർണ്ണസംഖ്യ	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		എങ്കിൽ (popup_is_popup(curwin))
		{
		    row -= popup_മുകൾ_extra(curwin);
		    col -= popup_ഇടത്_extra(curwin);
		}
#endif
		എങ്കിൽ (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    എങ്കിൽ (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    തിരികെ FAIL;
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ K_COMMAND:
	അവസ്ഥ K_SCRIPT_COMMAND:
	    തിരികെ do_cmdkey_command(c, 0);
    }
    എങ്കിൽ (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_താക്കോൽ(term, c, modmask, സന്ദേശം);
    എങ്കിൽ (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)സന്ദേശം, (പൂർണ്ണസംഖ്യ)len, ശൂന്യ);

    തിരികെ OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    സ്ഥിതം ശൂന്യം
term_paste_register(പൂർണ്ണസംഖ്യ prev_c UNUSED)
{
    പൂർണ്ണസംഖ്യ		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    പൂർണ്ണസംഖ്യ		type;

    എങ്കിൽ (add_to_showcmd(prev_c))
    എങ്കിൽ (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    എങ്കിൽ (!term_use_loop())
	// job finished while waiting for a character
	തിരികെ;

    // CTRL-W "= prompt for expression to evaluate.
    എങ്കിൽ (c == '=' && get_വ്യഞ്ജകം_register() != '=')
	തിരികെ;
    എങ്കിൽ (!term_use_loop())
	// job finished while waiting for a character
	തിരികെ;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	എങ്കിൽ (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (അക്ഷരം *)s,
		    (പൂർണ്ണസംഖ്യ)STRLEN(s), &ret, &length);
	    എങ്കിൽ (ret != ശൂന്യ)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (അക്ഷരം **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (പൂർണ്ണസംഖ്യ)STRLEN(s), ശൂന്യ);
#ifdef MSWIN
	എങ്കിൽ (tmp != s)
	    vim_free(s);
#endif

	എങ്കിൽ (item->li_next != ശൂന്യ || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, ശൂന്യ);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    പൂർണ്ണസംഖ്യ
terminal_is_active(ശൂന്യം)
{
    തിരികെ in_terminal_loop != ശൂന്യ;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    എങ്കിൽ (wp != ശൂന്യ && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    അല്ലെങ്കിൽ എങ്കിൽ (term->tl_highlight_name != ശൂന്യ)
	name = term->tl_highlight_name;
    അല്ലെങ്കിൽ
	name = (char_u*)"Terminal";

    തിരികെ syn_name2id(name);
}

#എങ്കിൽ defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    സ്ഥിതം cursorentry_T entry;
    പൂർണ്ണസംഖ്യ			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ഇടത് ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    എങ്കിൽ (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    എങ്കിൽ (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    എങ്കിൽ (term_bg != INVALCOLOR)
	*fg = term_bg;
    അല്ലെങ്കിൽ
	*fg = gui.back_pixel;

    എങ്കിൽ (term->tl_cursor_color == ശൂന്യ)
    {
	എങ്കിൽ (term_fg != INVALCOLOR)
	    *bg = term_fg;
	അല്ലെങ്കിൽ
	    *bg = gui.norm_pixel;
    }
    അല്ലെങ്കിൽ
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    തിരികെ &entry;
}
#endif

    സ്ഥിതം ശൂന്യം
may_output_cursor_props(ശൂന്യം)
{
    എങ്കിൽ (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	എങ്കിൽ (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	അല്ലെങ്കിൽ
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    സ്ഥിതം ശൂന്യം
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    എങ്കിൽ (gui.in_use)
	തിരികെ;
#endif
    എങ്കിൽ (in_terminal_loop == term)
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
    സ്ഥിതം ശൂന്യം
prepare_restore_cursor_props(ശൂന്യം)
{
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	തിരികെ;
#endif
    cursor_color_copy(&desired_cursor_color, ശൂന്യ);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_use_loop_check(പൂർണ്ണസംഖ്യ check_job_status)
{
    term_T *term = curbuf->b_term;

    തിരികെ term != ശൂന്യ
	&& !term->tl_normal_mode
	&& term->tl_vterm != ശൂന്യ
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    പൂർണ്ണസംഖ്യ
term_use_loop(ശൂന്യം)
{
    തിരികെ term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    ശൂന്യം
term_win_entered(ശൂന്യം)
{
    term_T *term = curbuf->b_term;

    എങ്കിൽ (term == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	എങ്കിൽ (State & MODE_തിരുകുക)
	    stop_തിരുകുക_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    ശൂന്യം
term_focus_change(പൂർണ്ണസംഖ്യ in_focus)
{
    term_T *term = curbuf->b_term;

    എങ്കിൽ (term == ശൂന്യ || term->tl_vterm == ശൂന്യ)
	തിരികെ;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    എങ്കിൽ (in_focus)
	vterm_state_focus_in(state);
    അല്ലെങ്കിൽ
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
raw_c_to_ctrl(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	തിരികെ c & 0x1f;
    തിരികെ c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ctrl_to_raw_c(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c < 0x20 && vterm_using_താക്കോൽ_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	തിരികെ c + '@';
    }
    തിരികെ c;
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
    പൂർണ്ണസംഖ്യ
terminal_loop(പൂർണ്ണസംഖ്യ blocking)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		raw_c;
    പൂർണ്ണസംഖ്യ		termwinkey = 0;
    പൂർണ്ണസംഖ്യ		ret;
#ifdef UNIX
    പൂർണ്ണസംഖ്യ		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    പൂർണ്ണസംഖ്യ		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    എങ്കിൽ (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_താക്കോൽ(curwin->w_p_twk, TRUE);
	എങ്കിൽ (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    ആകുമ്പോൾ (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	എങ്കിൽ (curbuf->b_term != ശൂന്യ && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    ആകുമ്പോൾ (must_redraw != 0)
		എങ്കിൽ (update_screen(0) == FAIL)
		    നിർത്തുക;
	എങ്കിൽ (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    നിർത്തുക;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	എങ്കിൽ (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    എങ്കിൽ (raw_c != K_IGNORE)
		vungetc(raw_c);
	    നിർത്തുക;
	}
	എങ്കിൽ (raw_c == K_IGNORE)
	    തുടരുക;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	എങ്കിൽ (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    എങ്കിൽ (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	എങ്കിൽ (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	എങ്കിൽ ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    പൂർണ്ണസംഖ്യ	    prev_c = c;
	    പൂർണ്ണസംഖ്യ	    prev_raw_c = raw_c;
	    പൂർണ്ണസംഖ്യ	    prev_mod_mask = mod_mask;

	    എങ്കിൽ (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    എങ്കിൽ (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		നിർത്തുക;

	    എങ്കിൽ (prev_c == Ctrl_BSL)
	    {
		എങ്കിൽ (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    പോകുക theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_താക്കോലുകൾ_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		പോകുക theend;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == '"')
	    {
		term_paste_register(prev_c);
		തുടരുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		പോകുക theend;
	    }
	}
#ifdef MSWIN
	എങ്കിൽ (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    എങ്കിൽ (MultiByteToWideChar(GetACP(), 0, (അക്ഷരം*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	എങ്കിൽ (send_താക്കോലുകൾ_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    എങ്കിൽ (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    പോകുക theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = ശൂന്യ;
    എങ്കിൽ (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    എങ്കിൽ (curbuf->b_term != ശൂന്യ && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    തിരികെ ret;
}

    സ്ഥിതം ശൂന്യം
may_toggle_cursor(term_T *term)
{
    എങ്കിൽ (in_terminal_loop != term)
	തിരികെ;

    എങ്കിൽ (term->tl_cursor_visible)
	cursor_on();
    അല്ലെങ്കിൽ
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
color2index(VTermColor *color, പൂർണ്ണസംഖ്യ fg, പൂർണ്ണസംഖ്യ *boldp)
{
    പൂർണ്ണസംഖ്യ red = color->red;
    പൂർണ്ണസംഖ്യ blue = color->blue;
    പൂർണ്ണസംഖ്യ green = color->green;

    *boldp = FALSE;

    എങ്കിൽ (VTERM_COLOR_IS_INVALID(color))
	തിരികെ 0;

    എങ്കിൽ (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	എങ്കിൽ (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    എങ്കിൽ (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    തിരികെ index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	അല്ലെങ്കിൽ എങ്കിൽ (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    തിരികെ (color->index & 7) + 1;
	}
	തിരികെ 0;
    }

    എങ്കിൽ (t_colors >= 256)
    {
	എങ്കിൽ (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    സ്ഥിതം പൂർണ്ണസംഖ്യ cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    പൂർണ്ണസംഖ്യ i;

	    എങ്കിൽ (red < 5)
		തിരികെ 17; // 00/00/00
	    എങ്കിൽ (red > 245) // ff/ff/ff
		തിരികെ 232;
	    വേണ്ടി (i = 0; i < 23; ++i)
		എങ്കിൽ (red < cutoff[i])
		    തിരികെ i + 233;
	    തിരികെ 256;
	}
	{
	    സ്ഥിതം പൂർണ്ണസംഖ്യ cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    പൂർണ്ണസംഖ്യ ri, gi, bi;

	    // 216-color cube
	    വേണ്ടി (ri = 0; ri < 5; ++ri)
		എങ്കിൽ (red < cutoff[ri])
		    നിർത്തുക;
	    വേണ്ടി (gi = 0; gi < 5; ++gi)
		എങ്കിൽ (green < cutoff[gi])
		    നിർത്തുക;
	    വേണ്ടി (bi = 0; bi < 5; ++bi)
		എങ്കിൽ (blue < cutoff[bi])
		    നിർത്തുക;
	    തിരികെ 17 + ri * 36 + gi * 6 + bi;
	}
    }
    തിരികെ 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    പൂർണ്ണസംഖ്യ attr = 0;

    എങ്കിൽ (cellattrs->bold)
	attr |= HL_BOLD;
    എങ്കിൽ (cellattrs->underline)
	attr |= HL_UNDERLINE;
    എങ്കിൽ (cellattrs->italic)
	attr |= HL_ITALIC;
    എങ്കിൽ (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    എങ്കിൽ (cellattrs->reverse)
	attr |= HL_INVERSE;
    തിരികെ attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    സ്ഥിതം ശൂന്യം
hl2vtermAttr(പൂർണ്ണസംഖ്യ attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    എങ്കിൽ (attr & HL_BOLD)
	cell->attrs.bold = 1;
    എങ്കിൽ (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    എങ്കിൽ (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    എങ്കിൽ (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    എങ്കിൽ (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    പൂർണ്ണസംഖ്യ attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    പൂർണ്ണസംഖ്യ is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    പൂർണ്ണസംഖ്യ is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    എങ്കിൽ (is_default_fg || is_default_bg)
    {
	എങ്കിൽ (wp != ശൂന്യ && wp->w_hlfwin_id != 0)
	{
	    എങ്കിൽ (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    എങ്കിൽ (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (is_default_fg)
		fg = &term->tl_default_color.fg;
	    എങ്കിൽ (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	തിരികെ get_gui_attr_idx(attr, guifg, guibg);
    }
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	തിരികെ get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    അല്ലെങ്കിൽ
#endif
    {
	പൂർണ്ണസംഖ്യ bold = MAYBE;
	പൂർണ്ണസംഖ്യ ctermfg = color2index(fg, TRUE, &bold);
	പൂർണ്ണസംഖ്യ ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	എങ്കിൽ (bold == TRUE)
	    attr |= HL_BOLD;

	തിരികെ get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    തിരികെ 0;
}

    സ്ഥിതം ശൂന്യം
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    എങ്കിൽ (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_damage(VTermRect rect, ശൂന്യം *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    തിരികെ 1;
}

    സ്ഥിതം ശൂന്യം
term_scroll_up(term_T *term, പൂർണ്ണസംഖ്യ start_row, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    win_T		 *wp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    പൂർണ്ണസംഖ്യ			 clear_attr;

    CLEAR_FIELD(attr);

    ആകുമ്പോൾ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	എങ്കിൽ (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_വരികൾ(wp, start_row, എണ്ണം, FALSE, FALSE, clear_attr);
	}
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_moverect(VTermRect dest, VTermRect src, ശൂന്യം *user)
{
    term_T	*term = (term_T *)user;
    പൂർണ്ണസംഖ്യ		എണ്ണം = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    എങ്കിൽ (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	എങ്കിൽ (dest.start_row == 0)
	    term->tl_postponed_scroll += എണ്ണം;
	അല്ലെങ്കിൽ
	    term_scroll_up(term, dest.start_row, എണ്ണം);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	പൂർണ്ണസംഖ്യ visible,
	ശൂന്യം *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    ആകുമ്പോൾ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	എങ്കിൽ (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    എങ്കിൽ (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_settermprop(
	VTermProp prop,
	VTerm_മൂല്യം *മൂല്യം,
	ശൂന്യം *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = ശൂന്യ;

    തിരഞ്ഞെടുപ്പ് (prop)
    {
	അവസ്ഥ VTERM_PROP_TITLE:
	    എങ്കിൽ (disable_vterm_title_for_testing)
		നിർത്തുക;
	    strval = vim_strnsave((char_u *)മൂല്യം->string.str,
							    മൂല്യം->string.len);
	    എങ്കിൽ (strval == ശൂന്യ)
		നിർത്തുക;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    എങ്കിൽ (*skipwhite(strval) == NUL)
		term->tl_title = ശൂന്യ;
	    // Same as blank
	    അല്ലെങ്കിൽ എങ്കിൽ (term->tl_arg0_cmd != ശൂന്യ
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (പൂർണ്ണസംഖ്യ)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = ശൂന്യ;
	    // Empty corrupted data of winpty
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = ശൂന്യ;
#ifdef MSWIN
	    അല്ലെങ്കിൽ എങ്കിൽ (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ശൂന്യ;
		പൂർണ്ണസംഖ്യ	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(അക്ഷരം*)മൂല്യം->string.str,
					(പൂർണ്ണസംഖ്യ)മൂല്യം->string.len, &ret, &length);
		എങ്കിൽ (ret != ശൂന്യ)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (അക്ഷരം**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    അല്ലെങ്കിൽ
	    {
		term->tl_title = strval;
		strval = ശൂന്യ;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    എങ്കിൽ (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    നിർത്തുക;

	അവസ്ഥ VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = മൂല്യം->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    നിർത്തുക;

	അവസ്ഥ VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = മൂല്യം->boolean;
	    may_set_cursor_props(term);
	    നിർത്തുക;

	അവസ്ഥ VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = മൂല്യം->സംഖ്യ;
	    may_set_cursor_props(term);
	    നിർത്തുക;

	അവസ്ഥ VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)മൂല്യം->string.str,
							    മൂല്യം->string.len);
	    എങ്കിൽ (strval == ശൂന്യ)
		നിർത്തുക;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    നിർത്തുക;

	അവസ്ഥ VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = മൂല്യം->boolean;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    തിരികെ 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_resize(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, ശൂന്യം *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    എങ്കിൽ (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    അല്ലെങ്കിൽ
    {
	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    തിരികെ 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    സ്ഥിതം ശൂന്യം
limit_scrollback(term_T *term, garray_T *gap, പൂർണ്ണസംഖ്യ update_buffer)
{
    എങ്കിൽ (gap->ga_len < term->tl_buffer->b_p_twsl)
	തിരികെ;

    പൂർണ്ണസംഖ്യ	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    പൂർണ്ണസംഖ്യ	i;
    sb_വരി_T *sb_വരികൾ = (sb_വരി_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    വേണ്ടി (i = 0; i < todo; ++i)
    {
	എങ്കിൽ (update_buffer && (!sb_വരികൾ[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_വരികൾ[i].sb_cells);
    }
    // Continue until end of wrapped line
    വേണ്ടി (; todo < gap->ga_len && sb_വരികൾ[todo].continuation; ++todo)
	vim_free(sb_വരികൾ[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_വരി_T *)gap->ga_data + todo,
		വലുപ്പം(sb_വരി_T) * gap->ga_len);
    എങ്കിൽ (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ശൂന്യ;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp->w_buffer == term->tl_buffer)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_pushline(പൂർണ്ണസംഖ്യ cols, സ്ഥിരം VTermScreenCell *cells, പൂർണ്ണസംഖ്യ continuation, ശൂന്യം *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    പൂർണ്ണസംഖ്യ		update_buffer;

    എങ്കിൽ (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ 0;

    cellattr_T	*p = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		text_len;
    char_u		*text;
    sb_വരി_T	*വരി;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    വേണ്ടി (i = 0; i < cols; ++i)
	എങ്കിൽ (cells[i].chars[0] != 0)
	    len = i + 1;
	അല്ലെങ്കിൽ
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    എങ്കിൽ (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    എങ്കിൽ (p != ശൂന്യ)
    {
	വേണ്ടി (col = 0; col < len; col += cells[col].width)
	{
	    എങ്കിൽ (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		നിർത്തുക;
	    }
	    വേണ്ടി (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
    {
	എങ്കിൽ (update_buffer)
	    text = (char_u *)"";
	അല്ലെങ്കിൽ
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    അല്ലെങ്കിൽ
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    എങ്കിൽ (update_buffer)
	add_scrollback_വരി_to_buffer(term, text, text_len, continuation);

    വരി = (sb_വരി_T *)gap->ga_data + gap->ga_len;
    വരി->sb_cols = len;
    വരി->sb_bytes = text_len;
    വരി->sb_cells = p;
    വരി->sb_fill_attr = fill_attr;
    വരി->continuation = (char_u)continuation;
    എങ്കിൽ (update_buffer)
    {
	വരി->sb_text = ശൂന്യ;
	++term->tl_scrollback_scrolled;
	എങ്കിൽ (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    അല്ലെങ്കിൽ
    {
	വരി->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    തിരികെ 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    സ്ഥിതം ശൂന്യം
handle_postponed_scrollback(term_T *term)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (term->tl_scrollback_postponed.ga_len == 0)
	തിരികെ;
    ch_log(ശൂന്യ, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    വേണ്ടി (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_വരി_T	*pp_വരി;
	sb_വരി_T	*വരി;

	എങ്കിൽ (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    നിർത്തുക;
	pp_വരി = (sb_വരി_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_വരി->sb_text;
	എങ്കിൽ (text == ശൂന്യ)
	    text = (char_u *)"";
	add_scrollback_വരി_to_buffer(term, text, (പൂർണ്ണസംഖ്യ)STRLEN(text), pp_വരി->continuation);
	vim_free(pp_വരി->sb_text);

	വരി = (sb_വരി_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	വരി->sb_cols = pp_വരി->sb_cols;
	വരി->sb_bytes = pp_വരി->sb_bytes;
	വരി->sb_cells = pp_വരി->sb_cells;
	വരി->sb_fill_attr = pp_വരി->sb_fill_attr;
	വരി->sb_text = ശൂന്യ;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	എങ്കിൽ (!pp_വരി->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_bell(ശൂന്യം *user UNUSED)
{
    vim_beep(BO_TERM);
    തിരികെ 0;
}

സ്ഥിതം VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ശൂന്യ,			// sb_pushline
    ശൂന്യ,			// sb_popline
    ശൂന്യ,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    എങ്കിൽ (!term->tl_normal_mode)
    {
	പൂർണ്ണസംഖ്യ	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	എങ്കിൽ (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    പൂർണ്ണസംഖ്യ		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = ശൂന്യ;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    എങ്കിൽ (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		എങ്കിൽ (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    അല്ലെങ്കിൽ
#endif
	    // If this is the last normal window: exit Vim.
	    എങ്കിൽ (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		തിരികെ TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ശൂന്യ, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    എങ്കിൽ (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		എങ്കിൽ (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		എങ്കിൽ (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		എങ്കിൽ (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    എങ്കിൽ (pwin != ശൂന്യ)
		popup_close_with_retval(pwin, 0);
#endif
	    തിരികെ TRUE;
	}
	എങ്കിൽ (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    അക്ഷരം    *cmd = term->tl_opencmd == ശൂന്യ
				? "botright sbuf %d"
				: (അക്ഷരം *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    അക്ഷരം    *buf = alloc(len);

	    എങ്കിൽ (buf != ശൂന്യ)
	    {
		ch_log(ശൂന്യ, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	അല്ലെങ്കിൽ
	    ch_log(ശൂന്യ, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    പൂർണ്ണസംഖ്യ
may_close_term_popup(ശൂന്യം)
{
    എങ്കിൽ (!popup_is_popup(curwin) || curbuf->b_term == ശൂന്യ
				 || term_job_running_not_none(curbuf->b_term))
	തിരികെ FAIL;

    win_T *pwin = curwin;

    എങ്കിൽ (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    തിരികെ OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    ശൂന്യം
term_channel_closing(channel_T *ch)
{
    term_T *term;

    വേണ്ടി (term = first_term; term != ശൂന്യ; term = term->tl_next)
	എങ്കിൽ (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    ശൂന്യം
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    പൂർണ്ണസംഖ്യ	    did_one = FALSE;

    വേണ്ടി (term = first_term; term != ശൂന്യ; term = next_term)
    {
	next_term = term->tl_next;
	എങ്കിൽ (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    എങ്കിൽ (term->tl_out_fd != ശൂന്യ)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = ശൂന്യ;
	    }
#endif

	    എങ്കിൽ (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		തുടരുക;
	    }

	    എങ്കിൽ (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    എങ്കിൽ (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	എങ്കിൽ (term != ശൂന്യ)
	{
	    എങ്കിൽ (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    ശൂന്യം
term_check_channel_closed_recently(ശൂന്യം)
{
    term_T *term;
    term_T *next_term;

    വേണ്ടി (term = first_term; term != ശൂന്യ; term = next_term)
    {
	next_term = term->tl_next;
	എങ്കിൽ (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    എങ്കിൽ (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    സ്ഥിതം ശൂന്യം
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	പൂർണ്ണസംഖ്യ		max_col)
{
    പൂർണ്ണസംഖ്യ off = screen_get_current_വരി_off();

    വേണ്ടി (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	പൂർണ്ണസംഖ്യ		c;

	എങ്കിൽ (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	എങ്കിൽ (c == NUL)
	{
	    Screen_വരികൾ[off] = ' ';
	    എങ്കിൽ (enc_utf8)
		Screen_വരികൾ_UC[off] = NUL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (enc_utf8)
	    {
		പൂർണ്ണസംഖ്യ i;

		// composing chars
		വേണ്ടി (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_വരികൾ_C[i][off] = cell.chars[i + 1];
		    എങ്കിൽ (cell.chars[i + 1] == 0)
			നിർത്തുക;
		}
		എങ്കിൽ (c >= 0x80 || (Screen_mco > 0
					 && Screen_വരികൾ_C[0][off] != 0))
		{
		    Screen_വരികൾ[off] = ' ';
		    Screen_വരികൾ_UC[off] = c;
		}
		അല്ലെങ്കിൽ
		{
		    Screen_വരികൾ[off] = c;
		    Screen_വരികൾ_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		എങ്കിൽ (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (അക്ഷരം*)mb, 2, 0, 0) > 1)
		{
		    Screen_വരികൾ[off] = mb[0];
		    Screen_വരികൾ[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		അല്ലെങ്കിൽ
		    Screen_വരികൾ[off] = c;
	    }
#endif
	    അല്ലെങ്കിൽ
		// This will only store the lower byte of "c".
		Screen_വരികൾ[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	എങ്കിൽ (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    എങ്കിൽ (enc_utf8)
	    {
		Screen_വരികൾ_UC[off] = NUL;
		Screen_വരികൾ[off] = NUL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_വരികൾ[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#എങ്കിൽ defined(FEAT_GUI)
    സ്ഥിതം ശൂന്യം
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	തിരികെ;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    ആകുമ്പോൾ (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	പൂർണ്ണസംഖ്യ save_p_more = p_more;

	p_more = FALSE;
	സന്ദേശം_row = Rows - 1;
	സന്ദേശം_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    വേണ്ടി (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	എങ്കിൽ (pos.row < term->tl_rows)
	{
	    പൂർണ്ണസംഖ്യ max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, ശൂന്യ, screen, &pos, max_col);
	}
	അല്ലെങ്കിൽ
	    pos.col = 0;

	screen_വരി(curwin, term->tl_toprow + pos.row,
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
    പൂർണ്ണസംഖ്യ
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    തിരികെ term != ശൂന്യ && term->tl_vterm != ശൂന്യ && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    ശൂന്യം
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    പൂർണ്ണസംഖ്യ		rows, cols;
    പൂർണ്ണസംഖ്യ		newrows, newcols;
    പൂർണ്ണസംഖ്യ		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    എങ്കിൽ (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	എങ്കിൽ (term->tl_postponed_scroll > 0
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
    വേണ്ടി (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ശൂന്യ ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	എങ്കിൽ (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	എങ്കിൽ (twp == ശൂന്യ)
	    നിർത്തുക;
    }
    എങ്കിൽ (newrows == 99999 || newcols == 99999)
	തിരികെ; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    എങ്കിൽ (newrows == 0 || newcols == 0)
	തിരികെ;

    എങ്കിൽ (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	എങ്കിൽ (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    വേണ്ടി (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	എങ്കിൽ (pos.row < term->tl_rows)
	{
	    പൂർണ്ണസംഖ്യ max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	അല്ലെങ്കിൽ
	    pos.col = 0;

	screen_വരി(wp, wp->w_winrow + pos.row
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
    ശൂന്യം
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    എങ്കിൽ (term == ശൂന്യ || term->tl_vterm == ശൂന്യ || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	തിരികെ;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    പൂർണ്ണസംഖ്യ
term_is_finished(buf_T *buf)
{
    തിരികെ buf->b_term != ശൂന്യ && buf->b_term->tl_vterm == ശൂന്യ;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    പൂർണ്ണസംഖ്യ
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    തിരികെ term != ശൂന്യ && (term->tl_vterm == ശൂന്യ || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    ശൂന്യം
term_change_in_curbuf(ശൂന്യം)
{
    term_T *term = curbuf->b_term;

    എങ്കിൽ (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	തിരികെ;

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
    പൂർണ്ണസംഖ്യ
term_get_attr(win_T *wp, linenr_T lnum, പൂർണ്ണസംഖ്യ col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_വരി_T	*വരി;
    cellattr_T	*cellattr;
    പൂർണ്ണസംഖ്യ         sb_വരി = -1;
    പൂർണ്ണസംഖ്യ         sb_col = col;

    എങ്കിൽ (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_വരി, &sb_col);

    എങ്കിൽ (sb_വരി < 0)
	cellattr = &term->tl_default_color;
    അല്ലെങ്കിൽ
    {
	വരി = (sb_വരി_T *)term->tl_scrollback.ga_data + sb_വരി;
	എങ്കിൽ (sb_col < 0 || sb_col >= വരി->sb_cols)
	    cellattr = &വരി->sb_fill_attr;
	അല്ലെങ്കിൽ
	    cellattr = വരി->sb_cells + sb_col;
    }
    തിരികെ cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    സ്ഥിതം ശൂന്യം
cterm_color2vterm(പൂർണ്ണസംഖ്യ nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    എങ്കിൽ (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_vterm_color_from_synid(പൂർണ്ണസംഖ്യ id, VTermColor *fg, VTermColor *bg)
{
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    എങ്കിൽ (0
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

	എങ്കിൽ (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	എങ്കിൽ (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	അല്ലെങ്കിൽ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	എങ്കിൽ (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	അല്ലെങ്കിൽ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	തിരികെ TRUE;
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (t_colors >= 16)
    {
	പൂർണ്ണസംഖ്യ cterm_fg = -1;
	പൂർണ്ണസംഖ്യ cterm_bg = -1;

	എങ്കിൽ (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	എങ്കിൽ (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	അല്ലെങ്കിൽ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	എങ്കിൽ (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	അല്ലെങ്കിൽ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	തിരികെ TRUE;
    }

    തിരികെ FALSE;
}

    ശൂന്യം
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    ശൂന്യം
term_update_hlfwin(win_T *wp)
{
    പൂർണ്ണസംഖ്യ id = wp->w_hlfwin_id;

    എങ്കിൽ (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ശൂന്യം
term_update_hlfwin_all(ശൂന്യം)
{
    win_T	 *wp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		 did_curwin = FALSE;

    ആകുമ്പോൾ (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    ശൂന്യം
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    പൂർണ്ണസംഖ്യ		    fgval, bgval;
    പൂർണ്ണസംഖ്യ		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    എങ്കിൽ (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    അല്ലെങ്കിൽ
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, ശൂന്യ);

    എങ്കിൽ (!get_vterm_color_from_synid(id, fg, bg))
    {
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	പൂർണ്ണസംഖ്യ tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	എങ്കിൽ (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    എങ്കിൽ (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	അല്ലെങ്കിൽ
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	എങ്കിൽ (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    എങ്കിൽ (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	അല്ലെങ്കിൽ
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_use_palette(ശൂന്യം)
{
    എങ്കിൽ (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    സ്ഥിതം ശൂന്യം
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    പൂർണ്ണസംഖ്യ		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    വേണ്ടി (; index < 16; index++)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    പൂർണ്ണസംഖ്യ		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    വേണ്ടി (li = list->lv_first; li != ശൂന്യ && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	എങ്കിൽ (color_name == ശൂന്യ)
	    തിരികെ FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	എങ്കിൽ (guicolor == INVALCOLOR)
	    തിരികെ FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    എങ്കിൽ (n != 16 || li != ശൂന്യ)
	തിരികെ FAIL;

    set_vterm_palette(vterm, rgb);

    തിരികെ OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    സ്ഥിതം ശൂന്യം
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", ശൂന്യ, TRUE);

    എങ്കിൽ (var == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ശൂന്യ
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    സ്ഥിതം ശൂന്യം
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    പൂർണ്ണസംഖ്യ		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ശൂന്യ;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	എങ്കിൽ (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    തിരികെ;
	}
    }

    CLEAR_FIELD(ea);

    എങ്കിൽ (opt_item != ശൂന്യ && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != ശൂന്യ)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	എങ്കിൽ (p == ശൂന്യ)
	    p = dict_get_string(dict, "fileformat", FALSE);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    എങ്കിൽ (check_ff_മൂല്യം(p) == FAIL)
		ch_log(ശൂന്യ, "Invalid ff argument to drop: %s", p);
	    അല്ലെങ്കിൽ
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	എങ്കിൽ (p == ശൂന്യ)
	    p = dict_get_string(dict, "encoding", FALSE);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    എങ്കിൽ (ea.cmd != ശൂന്യ)
	    {
		sprintf((അക്ഷരം *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	എങ്കിൽ (p != ശൂന്യ)
	    get_bad_opt(p, &ea);

	എങ്കിൽ (dict_has_താക്കോൽ(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	എങ്കിൽ (dict_has_താക്കോൽ(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	എങ്കിൽ (dict_has_താക്കോൽ(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	എങ്കിൽ (dict_has_താക്കോൽ(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    എങ്കിൽ (ea.cmd == ശൂന്യ)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_permitted_term_api(char_u *func, char_u *pat)
{
    തിരികെ pat != ശൂന്യ && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    സ്ഥിതം ശൂന്യം
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    എങ്കിൽ (item->li_next == ശൂന്യ)
    {
	ch_log(channel, "Missing function arguments for call");
	തിരികെ;
    }
    func = tv_get_string(&item->li_tv);

    എങ്കിൽ (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	തിരികെ;
    }

    argvars[0].v_type = VAR_സംഖ്യ;
    argvars[0].vval.v_സംഖ്യ = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    എങ്കിൽ (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    അല്ലെങ്കിൽ
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
    സ്ഥിതം size_t
url_decode(സ്ഥിരം അക്ഷരം *src, സ്ഥിരം size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    ആകുമ്പോൾ (i < len)
    {
	എങ്കിൽ (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	അല്ലെങ്കിൽ
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    തിരികെ j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    സ്ഥിതം ശൂന്യം
sync_shell_dir(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ       offset = 7;  // len of "file://" is 7
    അക്ഷരം      *pos = (അക്ഷരം *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    ആകുമ്പോൾ (offset < (പൂർണ്ണസംഖ്യ)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    എങ്കിൽ (offset >= (പൂർണ്ണസംഖ്യ)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	തിരികെ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_osc(പൂർണ്ണസംഖ്യ command, VTermStringFragment frag, ശൂന്യം *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == ശൂന്യ ? ശൂന്യ
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    എങ്കിൽ (command != 51 && (command != 7 || !p_asd))
	തിരികെ 0;

    // Concatenate what was received until the final piece is found.
    എങ്കിൽ (ga_grow(gap, (പൂർണ്ണസംഖ്യ)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	തിരികെ 1;
    }
    mch_memmove((അക്ഷരം *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (പൂർണ്ണസംഖ്യ)frag.len;
    എങ്കിൽ (!frag.final)
	തിരികെ 1;

    ((അക്ഷരം *)gap->ga_data)[gap->ga_len] = 0;

    എങ്കിൽ (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	തിരികെ 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = ശൂന്യ;
    reader.js_used = 0;
    എങ്കിൽ (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ശൂന്യ)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	എങ്കിൽ (item == ശൂന്യ)
	    ch_log(channel, "Missing command");
	അല്ലെങ്കിൽ
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    എങ്കിൽ (item == ശൂന്യ)
		ch_log(channel, "Missing argument for %s", cmd);
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    അല്ലെങ്കിൽ
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    അല്ലെങ്കിൽ
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    തിരികെ 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_csi(
	സ്ഥിരം അക്ഷരം  *leader UNUSED,
	സ്ഥിരം long  args[],
	പൂർണ്ണസംഖ്യ	    argcount,
	സ്ഥിരം അക്ഷരം  *intermed UNUSED,
	അക്ഷരം	    command,
	ശൂന്യം	    *user)
{
    term_T	*term = (term_T *)user;
    അക്ഷരം	buf[100];
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		x = 0;
    പൂർണ്ണസംഖ്യ		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    എങ്കിൽ (command != 't' || argcount != 1 || args[0] != 13)
	തിരികെ 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#എങ്കിൽ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (ശൂന്യം)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_buffer == term->tl_buffer)
	    നിർത്തുക;
    എങ്കിൽ (wp != ശൂന്യ)
    {
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	അല്ലെങ്കിൽ
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
						     (char_u *)buf, len, ശൂന്യ);
    തിരികെ 1;
}

സ്ഥിതം VTermStateFallbacks state_fallbacks = {
    ശൂന്യ,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ശൂന്യ,		// dcs
    ശൂന്യ,		// apc
    ശൂന്യ,		// pm
    ശൂന്യ		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    സ്ഥിതം ശൂന്യം *
vterm_malloc(size_t size, ശൂന്യം *data UNUSED)
{
    // make sure that the length is not zero
    തിരികെ alloc_clear(size == 0 ? 1L : size);
}

    സ്ഥിതം ശൂന്യം
vterm_memfree(ശൂന്യം *ptr, ശൂന്യം *data UNUSED)
{
    vim_free(ptr);
}

സ്ഥിതം VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
create_vterm(term_T *term, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_മൂല്യം	    മൂല്യം;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, ശൂന്യ);
    term->tl_vterm = vterm;
    എങ്കിൽ (vterm == ശൂന്യ)
	തിരികെ FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    എങ്കിൽ (state == ശൂന്യ || screen == ശൂന്യ)
    {
	vterm_free(vterm);
	തിരികെ FAIL;
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

    എങ്കിൽ (t_colors < 16)
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
    എങ്കിൽ (GetCaretBlinkTime() == INFINITE)
	മൂല്യം.boolean = 0;
    അല്ലെങ്കിൽ
	മൂല്യം.boolean = 1;
#അല്ലെങ്കിൽ
    മൂല്യം.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &മൂല്യം);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    തിരികെ OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    സ്ഥിതം ശൂന്യം
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    പൂർണ്ണസംഖ്യ		index;

    വേണ്ടി (index = 0; index < 16; index++)
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

    സ്ഥിതം ശൂന്യം
term_update_palette(term_T *term)
{
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (term_use_palette()
	    && (term->tl_palette != ശൂന്യ
		|| find_var((char_u *)"g:terminal_ansi_colors", ശൂന്യ, TRUE)
								      != ശൂന്യ))
    {
	എങ്കിൽ (term->tl_palette != ശൂന്യ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	അല്ലെങ്കിൽ
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    അല്ലെങ്കിൽ
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    ശൂന്യം
term_update_palette_all(ശൂന്യം)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	    തുടരുക;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ശൂന്യം
term_update_colors_all(ശൂന്യം)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	    തുടരുക;
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
    എങ്കിൽ (term->tl_status_text != ശൂന്യ)
	തിരികെ term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    എങ്കിൽ (term->tl_normal_mode)
    {
	എങ്കിൽ (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	അല്ലെങ്കിൽ
	    txt = (char_u *)_("Terminal-finished");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (term->tl_title != ശൂന്യ)
	txt = term->tl_title;
    അല്ലെങ്കിൽ എങ്കിൽ (term_none_open(term))
	txt = (char_u *)_("active");
    അല്ലെങ്കിൽ എങ്കിൽ (term_job_running(term))
	txt = (char_u *)_("running");
    അല്ലെങ്കിൽ
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    എങ്കിൽ (term->tl_status_text != ശൂന്യ)
	vim_snprintf((അക്ഷരം *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    തിരികെ term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    ശൂന്യം
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_term(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    വേണ്ടി (term = first_term; !abort && term != ശൂന്യ; term = term->tl_next)
	എങ്കിൽ (term->tl_job != ശൂന്യ)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
    തിരികെ abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    സ്ഥിതം buf_T *
term_get_buf(typval_T *argvars, അക്ഷരം *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    എങ്കിൽ (buf == ശൂന്യ || buf->b_term == ശൂന്യ)
    {
	(ശൂന്യം)tv_get_സംഖ്യ(&argvars[0]);    // issue errmsg if type error
	ch_log(ശൂന്യ, "%s: invalid buffer argument", where);
	തിരികെ ശൂന്യ;
    }
    തിരികെ buf;
}

    സ്ഥിതം ശൂന്യം
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    സ്ഥിതം ശൂന്യം
dump_term_color(FILE *fd, VTermColor *color)
{
    പൂർണ്ണസംഖ്യ index;

    എങ്കിൽ (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    അല്ലെങ്കിൽ എങ്കിൽ (color->type == 0)
	// use RGB values
	index = 255;
    അല്ലെങ്കിൽ
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (പൂർണ്ണസംഖ്യ)color->red, (പൂർണ്ണസംഖ്യ)color->green, (പൂർണ്ണസംഖ്യ)color->blue,
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
    ശൂന്യം
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    പൂർണ്ണസംഖ്യ		max_height = 0;
    പൂർണ്ണസംഖ്യ		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
    {
	emsg(_(e_job_already_finished));
	തിരികെ;
    }

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	എങ്കിൽ (check_for_dict_arg(argvars, 2) == FAIL)
	    തിരികെ;
	d = argvars[2].vval.v_dict;
	എങ്കിൽ (d != ശൂന്യ)
	{
	    max_height = dict_get_സംഖ്യ(d, "rows");
	    max_width = dict_get_സംഖ്യ(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	തിരികെ;
    }

    എങ്കിൽ (*fname == NUL || (fd = mch_fopen((അക്ഷരം *)fname, WRITEBIN)) == ശൂന്യ)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	തിരികെ;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    വേണ്ടി (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	പൂർണ്ണസംഖ്യ		repeat = 0;

	വേണ്ടി (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    പൂർണ്ണസംഖ്യ		    same_attr;
	    പൂർണ്ണസംഖ്യ		    same_chars = TRUE;
	    പൂർണ്ണസംഖ്യ		    i;
	    പൂർണ്ണസംഖ്യ		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    എങ്കിൽ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    വേണ്ടി (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		പൂർണ്ണസംഖ്യ c = cell.chars[i];
		പൂർണ്ണസംഖ്യ pc = prev_cell.chars[i];
		പൂർണ്ണസംഖ്യ should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		എങ്കിൽ (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		എങ്കിൽ (c != pc)
		    same_chars = FALSE;
		എങ്കിൽ (should_break)
		    നിർത്തുക;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    എങ്കിൽ (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		എങ്കിൽ (cell.chars[0] == NUL)
		    fputs(" ", fd);
		അല്ലെങ്കിൽ
		{
		    char_u	charbuf[10];
		    പൂർണ്ണസംഖ്യ		len;

		    വേണ്ടി (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		എങ്കിൽ (cell.width != prev_cell.width || !same_attr)
		{
		    എങ്കിൽ (cell.width == 2)
			fputs("*", fd);
		    അല്ലെങ്കിൽ
			fputs("+", fd);

		    എങ്കിൽ (same_attr)
		    {
			fputs("&", fd);
		    }
		    അല്ലെങ്കിൽ
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			എങ്കിൽ (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			അല്ലെങ്കിൽ
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			എങ്കിൽ (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			അല്ലെങ്കിൽ
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    എങ്കിൽ (cell.width == 2)
		++pos.col;
	}
	എങ്കിൽ (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    സ്ഥിതം ശൂന്യം
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    സ്ഥിതം ശൂന്യം
append_cell(garray_T *gap, cellattr_T *cell)
{
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    പൂർണ്ണസംഖ്യ		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {ശൂന്യ, 0};
    പൂർണ്ണസംഖ്യ		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    പൂർണ്ണസംഖ്യ		    max_cells = 0;
    പൂർണ്ണസംഖ്യ		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, വലുപ്പം(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    വേണ്ടി (;;)
    {
	എങ്കിൽ (c == EOF)
	    നിർത്തുക;
	എങ്കിൽ (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    എങ്കിൽ (ga_text.ga_data == ശൂന്യ)
		dump_is_corrupt(&ga_text);
	    എങ്കിൽ (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_വരി_T   *വരി = (sb_വരി_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		എങ്കിൽ (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		വരി->sb_cols = ga_cell.ga_len;
		വരി->sb_bytes = ga_text.ga_len;
		വരി->sb_cells = ga_cell.ga_data;
		വരി->sb_fill_attr = term->tl_default_color;
		വരി->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_വരി_എണ്ണം, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    അല്ലെങ്കിൽ
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '|' || c == '>')
	{
	    പൂർണ്ണസംഖ്യ prev_len = ga_text.ga_len;

	    എങ്കിൽ (c == '>')
	    {
		എങ്കിൽ (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    എങ്കിൽ (c != EOF)
		ga_append(&ga_text, c);
	    വേണ്ടി (;;)
	    {
		c = fgetc(fd);
		എങ്കിൽ (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    നിർത്തുക;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    എങ്കിൽ (ga_text.ga_data != ശൂന്യ)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		എങ്കിൽ (prev_char.string == ശൂന്യ)
		    prev_char.length = 0;
	    }

	    എങ്കിൽ (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == '+' || c == '*')
	    {
		പൂർണ്ണസംഖ്യ is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		എങ്കിൽ (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    ആകുമ്പോൾ (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    വേണ്ടി (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			എങ്കിൽ (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			അല്ലെങ്കിൽ എങ്കിൽ (c == '#')
			{
			    പൂർണ്ണസംഖ്യ red, green, blue, index = 0, type;

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
			    എങ്കിൽ (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    ആകുമ്പോൾ (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    എങ്കിൽ (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				എങ്കിൽ (index == 0)
				{
				    എങ്കിൽ (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    അല്ലെങ്കിൽ
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    അല്ലെങ്കിൽ
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    എങ്കിൽ (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    അല്ലെങ്കിൽ
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			അല്ലെങ്കിൽ
			    dump_is_corrupt(&ga_text);
		    }
		}
		അല്ലെങ്കിൽ
		    dump_is_corrupt(&ga_text);
	    }
	    അല്ലെങ്കിൽ
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    എങ്കിൽ (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '@')
	{
	    എങ്കിൽ (prev_char.string == ശൂന്യ)
		dump_is_corrupt(&ga_text);
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ എണ്ണം = 0;

		// repeat previous character, get the count
		വേണ്ടി (;;)
		{
		    c = fgetc(fd);
		    എങ്കിൽ (!SAFE_isdigit(c))
			നിർത്തുക;
		    എണ്ണം = എണ്ണം * 10 + (c - '0');
		}

		ആകുമ്പോൾ (എണ്ണം-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    എങ്കിൽ (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_വരി_എണ്ണം, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    തിരികെ max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    സ്ഥിതം char_u *
get_separator(പൂർണ്ണസംഖ്യ text_width, char_u *fname)
{
    പൂർണ്ണസംഖ്യ	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    പൂർണ്ണസംഖ്യ	    fname_size;
    char_u  *p = fname;
    പൂർണ്ണസംഖ്യ	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    എങ്കിൽ (textline == ശൂന്യ)
	തിരികെ ശൂന്യ;

    fname_size = vim_strsize(fname);
    എങ്കിൽ (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    ആകുമ്പോൾ (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    വേണ്ടി (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    വേണ്ടി (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    തിരികെ textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    സ്ഥിതം ശൂന്യം
term_load_dump(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ശൂന്യ;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ശൂന്യ;
    char_u	*fname_tofree = ശൂന്യ;
    FILE	*fd1;
    FILE	*fd2 = ശൂന്യ;
    char_u	*textline = ശൂന്യ;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    എങ്കിൽ (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    എങ്കിൽ (fname1 == ശൂന്യ || (do_diff && fname2 == ശൂന്യ))
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }
    fd1 = mch_fopen((അക്ഷരം *)fname1, READBIN);
    എങ്കിൽ (fd1 == ശൂന്യ)
    {
	semsg(_(e_cant_read_file_str), fname1);
	തിരികെ;
    }
    എങ്കിൽ (do_diff)
    {
	fd2 = mch_fopen((അക്ഷരം *)fname2, READBIN);
	എങ്കിൽ (fd2 == ശൂന്യ)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    തിരികെ;
	}
    }

    init_job_options(&opt);
    എങ്കിൽ (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	പോകുക theend;

    എങ്കിൽ (opt.jo_term_name == ശൂന്യ)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	എങ്കിൽ (fname_tofree != ശൂന്യ)
	{
	    vim_snprintf((അക്ഷരം *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    എങ്കിൽ (opt.jo_bufnr_buf != ശൂന്യ)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	എങ്കിൽ (wp == ശൂന്യ)
	    semsg(_(e_invalid_argument_str), "bufnr");
	അല്ലെങ്കിൽ
	{
	    buf = curbuf;
	    ആകുമ്പോൾ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    അല്ലെങ്കിൽ
	// Create a new terminal window.
	buf = term_start(&argvars[0], ശൂന്യ, &opt, TERM_START_NOJOB);

    എങ്കിൽ (buf != ശൂന്യ && buf->b_term != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	പൂർണ്ണസംഖ്യ		width;
	പൂർണ്ണസംഖ്യ		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_സംഖ്യ = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	എങ്കിൽ (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	എങ്കിൽ (!do_diff)
	    പോകുക theend;

	term->tl_മുകൾ_diff_rows = curbuf->b_ml.ml_വരി_എണ്ണം;

	textline = get_separator(width, fname1);
	എങ്കിൽ (textline == ശൂന്യ)
	    പോകുക theend;
	എങ്കിൽ (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_വരി_എണ്ണം, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	എങ്കിൽ (textline == ശൂന്യ)
	    പോകുക theend;
	എങ്കിൽ (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_വരി_എണ്ണം, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	width2 = read_dump_file(fd2, &cursor_pos2);
	എങ്കിൽ (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    എങ്കിൽ (textline == ശൂന്യ)
		പോകുക theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_വരി_എണ്ണം - bot_lnum;

	വേണ്ടി (lnum = 1; lnum <= term->tl_മുകൾ_diff_rows; ++lnum)
	{
	    എങ്കിൽ (lnum + bot_lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	    {
		// bottom part has fewer rows, fill with "-"
		വേണ്ടി (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    അല്ലെങ്കിൽ
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		പൂർണ്ണസംഖ്യ	col;
		sb_വരി_T   *sb_വരി = (sb_വരി_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_വരി + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_വരി + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		എങ്കിൽ (line1 == ശൂന്യ)
		    നിർത്തുക;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		വേണ്ടി (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    പൂർണ്ണസംഖ്യ len1 = utfc_ptr2len(p1);
		    പൂർണ്ണസംഖ്യ len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    എങ്കിൽ (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    അല്ലെങ്കിൽ എങ്കിൽ (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    അല്ലെങ്കിൽ എങ്കിൽ (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    അല്ലെങ്കിൽ എങ്കിൽ (cellattr1 != ശൂന്യ && cellattr2 != ശൂന്യ)
		    {
			എങ്കിൽ ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			അല്ലെങ്കിൽ എങ്കിൽ (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			അല്ലെങ്കിൽ എങ്കിൽ (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			അല്ലെങ്കിൽ എങ്കിൽ (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		ആകുമ്പോൾ (col < width)
		{
		    എങ്കിൽ (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    അല്ലെങ്കിൽ എങ്കിൽ (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    അല്ലെങ്കിൽ
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    എങ്കിൽ (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_മുകൾ_diff_rows) == OK)
		ml_append(term->tl_മുകൾ_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	ആകുമ്പോൾ (lnum + bot_lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    // bottom part has more rows, fill with "+"
	    വേണ്ടി (i = 0; i < width; ++i)
		textline[i] = '+';
	    എങ്കിൽ (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_മുകൾ_diff_rows) == OK)
		ml_append(term->tl_മുകൾ_diff_rows + lnum, textline, 0, FALSE);
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
    എങ്കിൽ (fd2 != ശൂന്യ)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    പൂർണ്ണസംഖ്യ
term_swap_diff(ശൂന്യം)
{
    term_T	*term = curbuf->b_term;
    linenr_T	വരി_എണ്ണം;
    linenr_T	മുകൾ_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_വരി_T	*sb_വരി;

    എങ്കിൽ (term == ശൂന്യ
	    || !term_is_finished(curbuf)
	    || term->tl_മുകൾ_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	തിരികെ FAIL;

    വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;
    മുകൾ_rows = term->tl_മുകൾ_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = വരി_എണ്ണം - bot_rows;
    sb_വരി = (sb_വരി_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    വേണ്ടി (lnum = 1; lnum <= മുകൾ_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    വേണ്ടി (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ OK;
    ml_append(വരി_എണ്ണം - മുകൾ_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(വരി_എണ്ണം - മുകൾ_rows));
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ OK;
    ml_delete(വരി_എണ്ണം - മുകൾ_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    എങ്കിൽ (മുകൾ_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	വേണ്ടി (lnum = 0; lnum < മുകൾ_rows; ++lnum)
	{
	    sb_വരി_T	temp;

	    temp = *(sb_വരി + lnum);
	    *(sb_വരി + lnum) = *(sb_വരി + bot_start + lnum);
	    *(sb_വരി + bot_start + lnum) = temp;
	}
    }
    അല്ലെങ്കിൽ
    {
	size_t		size = വലുപ്പം(sb_വരി_T) * term->tl_scrollback.ga_len;
	sb_വരി_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	എങ്കിൽ (temp != ശൂന്യ)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    വലുപ്പം(sb_വരി_T) * bot_rows);
	    mch_memmove((sb_വരി_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + മുകൾ_rows,
		    വലുപ്പം(sb_വരി_T) * (വരി_എണ്ണം - മുകൾ_rows - bot_rows));
	    mch_memmove((sb_വരി_T *)term->tl_scrollback.ga_data
						       + വരി_എണ്ണം - മുകൾ_rows,
		    temp,
		    വലുപ്പം(sb_വരി_T) * മുകൾ_rows);
	    vim_free(temp);
	}
    }

    term->tl_മുകൾ_diff_rows = bot_rows;
    term->tl_bot_diff_rows = മുകൾ_rows;

    update_screen(UPD_NOT_VALID);
    തിരികെ OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    ശൂന്യം
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    ശൂന്യം
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    ശൂന്യം
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    rettv->vval.v_സംഖ്യ = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    ശൂന്യം
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ	    attr;
    size_t  i;
    char_u  *name;

    സ്ഥിതം ഘടന {
	അക്ഷരം	    *name;
	പൂർണ്ണസംഖ്യ	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    attr = tv_get_സംഖ്യ(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    വേണ്ടി (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	എങ്കിൽ (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_സംഖ്യ = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    നിർത്തുക;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    ശൂന്യം
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getcursor()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_സംഖ്യ(l, term->tl_cursor_pos.row + 1);
    list_append_സംഖ്യ(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ;

    dict_add_സംഖ്യ(d, "visible", term->tl_cursor_visible);
    dict_add_സംഖ്യ(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_സംഖ്യ(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    എങ്കിൽ (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    ശൂന്യം
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getjob()");
    എങ്കിൽ (buf == ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_സംഖ്യ = VVAL_NULL;
	}
	തിരികെ;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    എങ്കിൽ (rettv->vval.v_job != ശൂന്യ)
	++rettv->vval.v_job->jv_refcount;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_row_സംഖ്യ(typval_T *tv, term_T *term)
{
    എങ്കിൽ (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ശൂന്യ
	    && STRCMP(tv->vval.v_string, ".") == 0)
	തിരികെ term->tl_cursor_pos.row;
    തിരികെ (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    ശൂന്യം
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    പൂർണ്ണസംഖ്യ		    row;

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_getline()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    row = get_row_സംഖ്യ(&argvars[1], term);

    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	പൂർണ്ണസംഖ്യ	  sb_row = term->tl_scrollback_scrolled + row;
	sb_വരി_T *വരി;

	എങ്കിൽ (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    തിരികെ;
	വരി = (sb_വരി_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ശൂന്യ, &offset);

	// vterm is finished, get the text from the buffer
	എങ്കിൽ (lnum > 0 && lnum <= buf->b_ml.ml_വരി_എണ്ണം)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    എങ്കിൽ (STRLEN(p) >= offset + വരി->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, വരി->sb_bytes);
	}
    }
    അല്ലെങ്കിൽ
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	പൂർണ്ണസംഖ്യ		len;
	char_u		*p;

	എങ്കിൽ (row < 0 || row >= term->tl_rows)
	    തിരികെ;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (അക്ഷരം *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    ശൂന്യം
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getscrolled()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    rettv->vval.v_സംഖ്യ = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    ശൂന്യം
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getsize()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    l = rettv->vval.v_list;
    list_append_സംഖ്യ(l, buf->b_term->tl_rows);
    list_append_സംഖ്യ(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    ശൂന്യം
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_setsize()");
    എങ്കിൽ (buf == ശൂന്യ)
    {
	emsg(_(e_not_terminal_buffer));
	തിരികെ;
    }
    എങ്കിൽ (buf->b_term->tl_vterm == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    rows = tv_get_സംഖ്യ(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_സംഖ്യ(&argvars[2]);
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
    ശൂന്യം
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getstatus()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;

    എങ്കിൽ (term_job_running(term))
	STRCPY(val, "running");
    അല്ലെങ്കിൽ
	STRCPY(val, "finished");
    എങ്കിൽ (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    ശൂന്യം
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_gettitle()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (buf->b_term->tl_title != ശൂന്യ)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    ശൂന്യം
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		അക്കം = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	അക്കം = tv_get_bool(&argvars[1]);

    തിരഞ്ഞെടുപ്പ് (അക്കം)
    {
	അവസ്ഥ 0:
	    എങ്കിൽ (buf->b_term->tl_job != ശൂന്യ)
		p = buf->b_term->tl_job->jv_tty_out;
	    നിർത്തുക;
	അവസ്ഥ 1:
	    എങ്കിൽ (buf->b_term->tl_job != ശൂന്യ)
		p = buf->b_term->tl_job->jv_tty_in;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    തിരികെ;
    }
    എങ്കിൽ (p != ശൂന്യ)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    ശൂന്യം
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL || first_term == ശൂന്യ)
	തിരികെ;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	എങ്കിൽ (tp->tl_buffer != ശൂന്യ)
	    എങ്കിൽ (list_append_സംഖ്യ(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		തിരികെ;
}

/*
 * "term_scrape(buf, row)" function
 */
    ശൂന്യം
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ശൂന്യ;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_വരി_T	    *വരി;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_scrape()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_സംഖ്യ(&argvars[1], term);

    എങ്കിൽ (term->tl_vterm != ശൂന്യ)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	എങ്കിൽ (screen == ശൂന്യ)  // can't really happen
	    തിരികെ;
	p = ശൂന്യ;
	വരി = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ശൂന്യ, &offset);

	എങ്കിൽ (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_വരി_എണ്ണം)
	    തിരികെ;

	വരി = (sb_വരി_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	എങ്കിൽ (STRLEN(p) < offset + വരി->sb_bytes)
	    തിരികെ;

	p += offset;
    }

    വേണ്ടി (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	പൂർണ്ണസംഖ്യ		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	പൂർണ്ണസംഖ്യ		i;

	എങ്കിൽ (screen == ശൂന്യ)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    എങ്കിൽ (pos.col >= വരി->sb_cols)
		നിർത്തുക;
	    cellattr = വരി->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	അല്ലെങ്കിൽ
	{
	    VTermScreenCell cell;

	    എങ്കിൽ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		നിർത്തുക;

	    mbslen = 0;
	    വേണ്ടി (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		എങ്കിൽ (cell.chars[i] == 0)
		    നിർത്തുക;
		mbslen += (*utf_char2bytes)((പൂർണ്ണസംഖ്യ)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	എങ്കിൽ (dcell == ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    നിർത്തുക;
	}

	dict_add_string_len(dcell, "chars", mbs, (പൂർണ്ണസംഖ്യ)mbslen);

	rgblen = vim_snprintf_safelen((അക്ഷരം *)rgb, വലുപ്പം(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (പൂർണ്ണസംഖ്യ)rgblen);
	rgblen = vim_snprintf_safelen((അക്ഷരം *)rgb, വലുപ്പം(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (പൂർണ്ണസംഖ്യ)rgblen);

	dict_add_സംഖ്യ(dcell, "attr",
				      cell2attr(term, ശൂന്യ, &attrs, &fg, &bg));
	dict_add_സംഖ്യ(dcell, "width", width);

	++pos.col;
	എങ്കിൽ (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    ശൂന്യം
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*സന്ദേശം;
    term_T	*term;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_sendkeys()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    സന്ദേശം = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (സന്ദേശം == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	തിരികെ;

    ആകുമ്പോൾ (*സന്ദേശം != NUL)
    {
	പൂർണ്ണസംഖ്യ c;

	എങ്കിൽ (*സന്ദേശം == K_SPECIAL && സന്ദേശം[1] != NUL && സന്ദേശം[2] != NUL)
	{
	    c = TO_SPECIAL(സന്ദേശം[1], സന്ദേശം[2]);
	    സന്ദേശം += 3;
	}
	അല്ലെങ്കിൽ
	{
	    c = PTR2CHAR(സന്ദേശം);
	    സന്ദേശം += MB_CPTR2LEN(സന്ദേശം);
	}
	send_താക്കോലുകൾ_to_term(term, c, 0, FALSE);
    }
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    ശൂന്യം
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    പൂർണ്ണസംഖ്യ		index;
    list_T	*list;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = term_get_buf(argvars, "term_getansicolors()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	തിരികെ;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    വേണ്ടി (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((അക്ഷരം *)hexbuf, വലുപ്പം(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	എങ്കിൽ (list_append_string(list, hexbuf, (പൂർണ്ണസംഖ്യ)hexbuflen) == FAIL)
	    തിരികെ;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    ശൂന്യം
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		n = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_setansicolors()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    എങ്കിൽ (term->tl_vterm == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_മൂല്യം_for_argument_str), "\"colors\"");
	തിരികെ;
    }

    എങ്കിൽ (term->tl_palette == ശൂന്യ)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    എങ്കിൽ (term->tl_palette == ശൂന്യ)
	തിരികെ;

    FOR_ALL_LIST_ഇനങ്ങൾ(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	എങ്കിൽ (color_name == ശൂന്യ)
	    തിരികെ;

	guicolor = GUI_GET_COLOR(color_name);
	എങ്കിൽ (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    തിരികെ;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    ശൂന്യം
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_setapi()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (api != ശൂന്യ)
	term->tl_api = vim_strsave(api);
    അല്ലെങ്കിൽ
	term->tl_api = ശൂന്യ;
}

/*
 * "term_setrestore(buf, command)" function
 */
    ശൂന്യം
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#എങ്കിൽ defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_setrestore()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (cmd != ശൂന്യ)
	term->tl_command = vim_strsave(cmd);
    അല്ലെങ്കിൽ
	term->tl_command = ശൂന്യ;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    ശൂന്യം
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_setkill()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (how != ശൂന്യ)
	term->tl_kill = vim_strsave(how);
    അല്ലെങ്കിൽ
	term->tl_kill = ശൂന്യ;
}

/*
 * "term_start(command, options)" function
 */
    ശൂന്യം
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    init_job_options(&opt);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	തിരികെ;

    buf = term_start(&argvars[0], ശൂന്യ, &opt, 0);

    എങ്കിൽ (buf != ശൂന്യ && buf->b_term != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    ശൂന്യം
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    buf = term_get_buf(argvars, "term_wait()");
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (buf->b_term->tl_job == ശൂന്യ)
    {
	ch_log(ശൂന്യ, "term_wait(): no job to wait for");
	തിരികെ;
    }
    എങ്കിൽ (buf->b_term->tl_job->jv_channel == ശൂന്യ)
	// channel is closed, nothing to do
	തിരികെ;

    // Get the job status, this will detect a job that finished.
    എങ്കിൽ (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ശൂന്യ, "term_wait(): waiting for channel to close");
	ആകുമ്പോൾ (buf->b_term != ശൂന്യ && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    എങ്കിൽ (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		നിർത്തുക;
	    എങ്കിൽ (buf->b_term == ശൂന്യ || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		നിർത്തുക;
	}

	term_flush_messages();
    }
    അല്ലെങ്കിൽ
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_സംഖ്യ(&argvars[1]);
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
    ശൂന്യം
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	എങ്കിൽ (term->tl_job == ch->ch_job)
	{
	    എങ്കിൽ (term->tl_eof_chars != ശൂന്യ)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(പൂർണ്ണസംഖ്യ)STRLEN(term->tl_eof_chars), ശൂന്യ);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, ശൂന്യ);
	    }
#ifdef MSWIN
	    അല്ലെങ്കിൽ
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, ശൂന്യ);
#endif
	}
}

#എങ്കിൽ defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    തിരികെ term != ശൂന്യ ? term->tl_job : ശൂന്യ;
}
#endif

#എങ്കിൽ defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
ശൂന്യം (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    സ്ഥിതം പൂർണ്ണസംഖ്യ
dyn_conpty_init(പൂർണ്ണസംഖ്യ verbose)
{
    സ്ഥിതം HMODULE	hKerneldll = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			i;
    സ്ഥിതം ഘടന
    {
	അക്ഷരം	*name;
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
	{ശൂന്യ, ശൂന്യ}
    };

    എങ്കിൽ (!has_conpty_working())
    {
	എങ്കിൽ (verbose)
	    emsg(_(e_conpty_is_not_available));
	തിരികെ FAIL;
    }

    // No need to initialize twice.
    എങ്കിൽ (hKerneldll)
	തിരികെ OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    വേണ്ടി (i = 0; conpty_entry[i].name != ശൂന്യ
					&& conpty_entry[i].ptr != ശൂന്യ; ++i)
    {
	എങ്കിൽ ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == ശൂന്യ)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = ശൂന്യ;
	    തിരികെ FAIL;
	}
    }

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	അക്ഷരം	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ശൂന്യ;
    WCHAR	    *cmd_wchar_copy = ശൂന്യ;
    WCHAR	    *cwd_wchar = ശൂന്യ;
    WCHAR	    *env_wchar = ശൂന്യ;
    channel_T	    *channel = ശൂന്യ;
    job_T	    *job = ശൂന്യ;
    HANDLE	    jo = ശൂന്യ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ശൂന്യ;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ശൂന്യ;
    HANDLE	    o_theirs = ശൂന്യ;
    HANDLE	    i_ours = ശൂന്യ;
    HANDLE	    o_ours = ശൂന്യ;
    അക്ഷരം	    *errmsg = ശൂന്യ;

    ga_init2(&ga_cmd, വലുപ്പം(അക്ഷരം*), 20);
    ga_init2(&ga_env, വലുപ്പം(അക്ഷരം*), 20);

    എങ്കിൽ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvar->v_type == VAR_LIST)
    {
	എങ്കിൽ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    പോകുക failed;
	cmd = ga_cmd.ga_data;
    }
    എങ്കിൽ (cmd == ശൂന്യ || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	പോകുക failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ശൂന്യ);

    എങ്കിൽ (cmd_wchar != ശൂന്യ)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    എങ്കിൽ (cmd_wchar == ശൂന്യ)
	പോകുക failed;
    എങ്കിൽ (opt->jo_cwd != ശൂന്യ)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ശൂന്യ);
	എങ്കിൽ (cwd_wchar == ശൂന്യ)
	    പോകുക failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    എങ്കിൽ (!CreatePipe(&i_theirs, &i_ours, ശൂന്യ, 0))
	പോകുക failed;
    എങ്കിൽ (!CreatePipe(&o_ours, &o_theirs, ശൂന്യ, 0))
	പോകുക failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    എങ്കിൽ (FAILED(hr))
	പോകുക failed;

    term->tl_siex.StartupInfo.cb = വലുപ്പം(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_നിവേശം = INVALID_HANDLE_മൂല്യം;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_മൂല്യം;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_മൂല്യം;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ശൂന്യ, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    എങ്കിൽ (!term->tl_siex.lpAttributeList)
	പോകുക failed;
    എങ്കിൽ (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	പോകുക failed;
    എങ്കിൽ (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    വലുപ്പം(HPCON), ശൂന്യ, ശൂന്യ))
	പോകുക failed;

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
	പോകുക failed;

    job = job_alloc();
    എങ്കിൽ (job == ശൂന്യ)
	പോകുക failed;
    എങ്കിൽ (argvar->v_type == VAR_STRING)
    {
	പൂർണ്ണസംഖ്യ argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    എങ്കിൽ (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    എങ്കിൽ (!CreateProcessW(ശൂന്യ, cmd_wchar_copy, ശൂന്യ, ശൂന്യ, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	പോകുക failed;
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

    jo = CreateJobObject(ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (jo == ശൂന്യ)
	പോകുക failed;

    എങ്കിൽ (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ശൂന്യ;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    എങ്കിൽ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	പോകുക failed;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (term_use_palette())
    {
	എങ്കിൽ (term->tl_palette != ശൂന്യ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	അല്ലെങ്കിൽ
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
    എങ്കിൽ (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((അക്ഷരം *)fname, WRITEBIN);
	എങ്കിൽ (term->tl_out_fd == ശൂന്യ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    തിരികെ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    എങ്കിൽ (channel != ശൂന്യ)
	channel_clear(channel);
    എങ്കിൽ (job != ശൂന്യ)
    {
	job->jv_channel = ശൂന്യ;
	job_cleanup(job);
    }
    term->tl_job = ശൂന്യ;
    എങ്കിൽ (jo != ശൂന്യ)
	CloseHandle(jo);

    എങ്കിൽ (term->tl_siex.lpAttributeList != ശൂന്യ)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ശൂന്യ;
    എങ്കിൽ (o_theirs != ശൂന്യ)
	CloseHandle(o_theirs);
    എങ്കിൽ (o_ours != ശൂന്യ)
	CloseHandle(o_ours);
    എങ്കിൽ (i_ours != ശൂന്യ)
	CloseHandle(i_ours);
    എങ്കിൽ (i_theirs != ശൂന്യ)
	CloseHandle(i_theirs);
    എങ്കിൽ (term->tl_conpty != ശൂന്യ)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ശൂന്യ;
    // Propagate errors that occur in CreateProcess
    എങ്കിൽ (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    തിരികെ FAIL;
}

    സ്ഥിതം ശൂന്യം
conpty_term_report_winsize(term_T *term, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    സ്ഥിതം ശൂന്യം
term_free_conpty(term_T *term)
{
    എങ്കിൽ (term->tl_siex.lpAttributeList != ശൂന്യ)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ശൂന്യ;
    എങ്കിൽ (term->tl_conpty != ശൂന്യ)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
use_conpty(ശൂന്യം)
{
    തിരികെ has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

ശൂന്യം* (*winpty_config_new)(UINT64, ശൂന്യം*);
ശൂന്യം* (*winpty_open)(ശൂന്യം*, ശൂന്യം*);
ശൂന്യം* (*winpty_spawn_config_new)(UINT64, ശൂന്യം*, LPCWSTR, ശൂന്യം*, ശൂന്യം*, ശൂന്യം*);
BOOL (*winpty_spawn)(ശൂന്യം*, ശൂന്യം*, HANDLE*, HANDLE*, DWORD*, ശൂന്യം*);
ശൂന്യം (*winpty_config_set_mouse_mode)(ശൂന്യം*, പൂർണ്ണസംഖ്യ);
ശൂന്യം (*winpty_config_set_initial_size)(ശൂന്യം*, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
LPCWSTR (*winpty_conin_name)(ശൂന്യം*);
LPCWSTR (*winpty_conout_name)(ശൂന്യം*);
LPCWSTR (*winpty_conerr_name)(ശൂന്യം*);
ശൂന്യം (*winpty_free)(ശൂന്യം*);
ശൂന്യം (*winpty_config_free)(ശൂന്യം*);
ശൂന്യം (*winpty_spawn_config_free)(ശൂന്യം*);
ശൂന്യം (*winpty_error_free)(ശൂന്യം*);
LPCWSTR (*winpty_error_സന്ദേശം)(ശൂന്യം*);
BOOL (*winpty_set_size)(ശൂന്യം*, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, ശൂന്യം*);
HANDLE (*winpty_agent_process)(ശൂന്യം*);

# define WINPTY_DLL "winpty.dll"

സ്ഥിതം HINSTANCE hWinPtyDLL = ശൂന്യ;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
dyn_winpty_init(പൂർണ്ണസംഖ്യ verbose)
{
    പൂർണ്ണസംഖ്യ i;
    സ്ഥിതം ഘടന
    {
	അക്ഷരം	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_സന്ദേശം},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ശൂന്യ, ശൂന്യ}
    };

    // No need to initialize twice.
    എങ്കിൽ (hWinPtyDLL)
	തിരികെ OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    എങ്കിൽ (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((അക്ഷരം *)p_winptydll);
    എങ്കിൽ (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    എങ്കിൽ (!hWinPtyDLL)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	തിരികെ FAIL;
    }
    വേണ്ടി (i = 0; winpty_entry[i].name != ശൂന്യ
					 && winpty_entry[i].ptr != ശൂന്യ; ++i)
    {
	എങ്കിൽ ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == ശൂന്യ)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = ശൂന്യ;
	    തിരികെ FAIL;
	}
    }

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	അക്ഷരം	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ശൂന്യ;
    WCHAR	    *cwd_wchar = ശൂന്യ;
    WCHAR	    *env_wchar = ശൂന്യ;
    channel_T	    *channel = ശൂന്യ;
    job_T	    *job = ശൂന്യ;
    DWORD	    error;
    HANDLE	    jo = ശൂന്യ;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    ശൂന്യം	    *winpty_err = ശൂന്യ;
    ശൂന്യം	    *spawn_config = ശൂന്യ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ശൂന്യ;

    ga_init2(&ga_cmd, വലുപ്പം(അക്ഷരം*), 20);
    ga_init2(&ga_env, വലുപ്പം(അക്ഷരം*), 20);

    എങ്കിൽ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvar->v_type == VAR_LIST)
    {
	എങ്കിൽ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    പോകുക failed;
	cmd = ga_cmd.ga_data;
    }
    എങ്കിൽ (cmd == ശൂന്യ || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	പോകുക failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ശൂന്യ);
    ga_clear(&ga_cmd);
    എങ്കിൽ (cmd_wchar == ശൂന്യ)
	പോകുക failed;
    എങ്കിൽ (opt->jo_cwd != ശൂന്യ)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ശൂന്യ);
	എങ്കിൽ (cwd_wchar == ശൂന്യ)
	    പോകുക failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    എങ്കിൽ (term->tl_winpty_config == ശൂന്യ)
	പോകുക failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    എങ്കിൽ (term->tl_winpty == ശൂന്യ)
	പോകുക failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ശൂന്യ,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    എങ്കിൽ (spawn_config == ശൂന്യ)
	പോകുക failed;

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
	പോകുക failed;

    job = job_alloc();
    എങ്കിൽ (job == ശൂന്യ)
	പോകുക failed;
    എങ്കിൽ (argvar->v_type == VAR_STRING)
    {
	പൂർണ്ണസംഖ്യ argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    എങ്കിൽ (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    എങ്കിൽ (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	പോകുക failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, ശൂന്യ,
	    OPEN_EXISTING, 0, ശൂന്യ),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, ശൂന്യ,
	    OPEN_EXISTING, 0, ശൂന്യ),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, ശൂന്യ,
	    OPEN_EXISTING, 0, ശൂന്യ));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (jo == ശൂന്യ)
	പോകുക failed;

    എങ്കിൽ (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ശൂന്യ;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    എങ്കിൽ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	പോകുക failed;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (term_use_palette())
    {
	എങ്കിൽ (term->tl_palette != ശൂന്യ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	അല്ലെങ്കിൽ
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
	    (short_u *)winpty_conin_name(term->tl_winpty), ശൂന്യ);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), ശൂന്യ);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    എങ്കിൽ (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((അക്ഷരം *)fname, WRITEBIN);
	എങ്കിൽ (term->tl_out_fd == ശൂന്യ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    തിരികെ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    എങ്കിൽ (spawn_config != ശൂന്യ)
	winpty_spawn_config_free(spawn_config);
    എങ്കിൽ (channel != ശൂന്യ)
	channel_clear(channel);
    എങ്കിൽ (job != ശൂന്യ)
    {
	job->jv_channel = ശൂന്യ;
	job_cleanup(job);
    }
    term->tl_job = ശൂന്യ;
    എങ്കിൽ (jo != ശൂന്യ)
	CloseHandle(jo);
    എങ്കിൽ (term->tl_winpty != ശൂന്യ)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ശൂന്യ;
    എങ്കിൽ (term->tl_winpty_config != ശൂന്യ)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ശൂന്യ;
    എങ്കിൽ (winpty_err != ശൂന്യ)
    {
	അക്ഷരം *സന്ദേശം = (അക്ഷരം *)utf16_to_enc(
				(short_u *)winpty_error_സന്ദേശം(winpty_err), ശൂന്യ);

	എങ്കിൽ (സന്ദേശം != ശൂന്യ)
	{
	    emsg(സന്ദേശം);
	    vim_free(സന്ദേശം);
	}
	winpty_error_free(winpty_err);
    }
    തിരികെ FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	അക്ഷരം	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    പൂർണ്ണസംഖ്യ		    use_winpty = FALSE;
    പൂർണ്ണസംഖ്യ		    use_conpty = FALSE;
    പൂർണ്ണസംഖ്യ		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    എങ്കിൽ (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	തിരികെ dyn_winpty_init(TRUE);

    എങ്കിൽ (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    എങ്കിൽ (tty_type == NUL)
    {
	എങ്കിൽ (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tty_type == 'w')	// winpty
    {
	എങ്കിൽ (has_winpty)
	    use_winpty = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tty_type == 'c')	// conpty
    {
	എങ്കിൽ (has_conpty)
	    use_conpty = TRUE;
	അല്ലെങ്കിൽ
	    തിരികെ dyn_conpty_init(TRUE);
    }

    എങ്കിൽ (use_conpty)
	തിരികെ conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    എങ്കിൽ (use_winpty)
	തിരികെ winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    തിരികെ dyn_winpty_init(TRUE);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_മൂല്യം;
    HANDLE	    hPipeOut = INVALID_HANDLE_മൂല്യം;
    അക്ഷരം	    in_name[80], out_name[80];
    channel_T	    *channel = ശൂന്യ;

    എങ്കിൽ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	തിരികെ FAIL;

    vim_snprintf(in_name, വലുപ്പം(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ശൂന്യ);
    എങ്കിൽ (hPipeIn == INVALID_HANDLE_മൂല്യം)
	പോകുക failed;

    vim_snprintf(out_name, വലുപ്പം(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ശൂന്യ);
    എങ്കിൽ (hPipeOut == INVALID_HANDLE_മൂല്യം)
	പോകുക failed;

    ConnectNamedPipe(hPipeIn, ശൂന്യ);
    ConnectNamedPipe(hPipeOut, ശൂന്യ);

    term->tl_job = job_alloc();
    എങ്കിൽ (term->tl_job == ശൂന്യ)
	പോകുക failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
	പോകുക failed;
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

    തിരികെ OK;

failed:
    എങ്കിൽ (hPipeIn != ശൂന്യ)
	CloseHandle(hPipeIn);
    എങ്കിൽ (hPipeOut != ശൂന്യ)
	CloseHandle(hPipeOut);
    തിരികെ FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    സ്ഥിതം ശൂന്യം
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    എങ്കിൽ (term->tl_winpty != ശൂന്യ)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ശൂന്യ;
    എങ്കിൽ (term->tl_winpty_config != ശൂന്യ)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ശൂന്യ;
    എങ്കിൽ (term->tl_vterm != ശൂന്യ)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ശൂന്യ;
}

/*
 * Report the size to the terminal.
 */
    സ്ഥിതം ശൂന്യം
term_report_winsize(term_T *term, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
    എങ്കിൽ (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    എങ്കിൽ (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
terminal_enabled(ശൂന്യം)
{
    തിരികെ dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#അല്ലെങ്കിൽ

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	അക്ഷരം	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = ശൂന്യ;

    എങ്കിൽ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	തിരികെ FAIL;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (term_use_palette())
    {
	എങ്കിൽ (term->tl_palette != ശൂന്യ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	അല്ലെങ്കിൽ
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    എങ്കിൽ (term->tl_job != ശൂന്യ)
	++term->tl_job->jv_refcount;

    തിരികെ term->tl_job != ശൂന്യ
	&& term->tl_job->jv_channel != ശൂന്യ
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
create_pty_only(term_T *term, jobopt_T *opt)
{
    എങ്കിൽ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	തിരികെ FAIL;

    term->tl_job = job_alloc();
    എങ്കിൽ (term->tl_job == ശൂന്യ)
	തിരികെ FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    തിരികെ mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    സ്ഥിതം ശൂന്യം
term_free_vterm(term_T *term)
{
    എങ്കിൽ (term->tl_vterm != ശൂന്യ)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ശൂന്യ;
}

/*
 * Report the size to the terminal.
 */
    സ്ഥിതം ശൂന്യം
term_report_winsize(term_T *term, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
    // Use an ioctl() to report the new window size to the job.
    എങ്കിൽ (term->tl_job == ശൂന്യ || term->tl_job->jv_channel == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ fd = -1;
    പൂർണ്ണസംഖ്യ part;

    വേണ്ടി (part = PART_OUT; part < PART_എണ്ണം; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	എങ്കിൽ (mch_isatty(fd))
	    നിർത്തുക;
    }
    എങ്കിൽ (part < PART_എണ്ണം && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
