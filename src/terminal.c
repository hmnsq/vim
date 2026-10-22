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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ಅನ್ತರ್ಭಾವ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    VTermScreenCellAttrs	attrs;
    ವರ್ಣ			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ sb_ಸಾಲು_S {
    ಪೂರ್ಣಾಙ್ಕ		sb_cols;	// can differ per line
    ಪೂರ್ಣಾಙ್ಕ		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_ಸಾಲು_T;

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
ಸಂರಚನಾ terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#ಯದಿ defined(FEAT_GUI)
    ಪೂರ್ಣಾಙ್ಕ		tl_system;	// when non-zero used for :!cmd output
    ಪೂರ್ಣಾಙ್ಕ		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    ಪೂರ್ಣಾಙ್ಕ		tl_vterm_size_changed;

    ಪೂರ್ಣಾಙ್ಕ		tl_normal_mode; // TRUE: Terminal-Normal mode
    ಪೂರ್ಣಾಙ್ಕ		tl_channel_closing;
    ಪೂರ್ಣಾಙ್ಕ		tl_channel_closed;
    ಪೂರ್ಣಾಙ್ಕ		tl_channel_recently_closed; // still need to handle tl_finish

    ಪೂರ್ಣಾಙ್ಕ		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    ಶೂನ್ಯ	*tl_winpty_config;
    ಶೂನ್ಯ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#ಯದಿ defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    ಪೂರ್ಣಾಙ್ಕ		tl_rows;
    ಪೂರ್ಣಾಙ್ಕ		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    ಪೂರ್ಣಾಙ್ಕ		tl_dirty_row_start; // MAX_ROW if nothing dirty
    ಪೂರ್ಣಾಙ್ಕ		tl_dirty_row_end;   // row below last one to update
    ಪೂರ್ಣಾಙ್ಕ		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    ಪೂರ್ಣಾಙ್ಕ		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    ಪೂರ್ಣಾಙ್ಕ		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    ಪೂರ್ಣಾಙ್ಕ		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    ಪೂರ್ಣಾಙ್ಕ		tl_scrollback_snapshot;
    ಪೂರ್ಣಾಙ್ಕ		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_ತುದಿ_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    ಪೂರ್ಣಾಙ್ಕ		tl_cursor_visible;
    ಪೂರ್ಣಾಙ್ಕ		tl_cursor_blink;
    ಪೂರ್ಣಾಙ್ಕ		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    ಪೂರ್ಣಾಙ್ಕ		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ಸ್ಥಿರ term_T *first_term = ರಿಕ್ತ;

// Terminal active in terminal_loop().
ಸ್ಥಿರ term_T *in_terminal_loop = ರಿಕ್ತ;

#ifdef MSWIN
ಸ್ಥಿರ BOOL has_winpty = FALSE;
ಸ್ಥಿರ BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define ಕೀಲಿ_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    ಪ್ರತಿ ((term) = first_term; (term) != ರಿಕ್ತ; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ term_and_job_init(term_T *term, typval_T *argvar, ವರ್ಣ **argv, jobopt_T *opt, jobopt_T *orig_opt);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ create_pty_only(term_T *term, jobopt_T *opt);
ಸ್ಥಿರ ಶೂನ್ಯ term_report_winsize(term_T *term, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols);
ಸ್ಥಿರ ಶೂನ್ಯ term_free_vterm(term_T *term);
#ifdef FEAT_GUI
ಸ್ಥಿರ ಶೂನ್ಯ update_system_term(term_T *term);
#endif

ಸ್ಥಿರ ಶೂನ್ಯ handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ಸ್ಥಿರ char_u	*last_set_cursor_color = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*desired_cursor_color = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_set_cursor_shape = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	desired_cursor_shape = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_set_cursor_blink = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    ಯದಿ (lhs_color != ರಿಕ್ತ && rhs_color != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ STRCMP(lhs_color, rhs_color) == 0;
    ಪ್ರತ್ಯಾಗಮ lhs_color == ರಿಕ್ತ && rhs_color == ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    ಯದಿ (cursor_color_equal(*to_color, from_color))
	ಪ್ರತ್ಯಾಗಮ;
    vim_free(*to_color);
    *to_color = (from_color == ರಿಕ್ತ) ? ರಿಕ್ತ : vim_strsave(from_color);
}

    ಸ್ಥಿರ char_u *
cursor_color_get(char_u *color)
{
    ಪ್ರತ್ಯಾಗಮ (color == ರಿಕ್ತ) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_termwinsize(win_T *wp, ಪೂರ್ಣಾಙ್ಕ *rows, ಪೂರ್ಣಾಙ್ಕ *cols)
{
    ಪೂರ್ಣಾಙ್ಕ	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    ಯದಿ (*wp->w_p_tws == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    ಯದಿ (p == ರಿಕ್ತ)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((ವರ್ಣ *)wp->w_p_tws);
    *cols = atoi((ವರ್ಣ *)p + 1);
    ಯದಿ (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    ಯದಿ (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    ಪ್ರತ್ಯಾಗಮ minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    ಪೂರ್ಣಾಙ್ಕ rows, cols;
    ಪೂರ್ಣಾಙ್ಕ	minsize;

#ifdef FEAT_GUI
    ಯದಿ (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    ಯದಿ (minsize)
    {
	ಯದಿ (term->tl_rows < rows)
	    term->tl_rows = rows;
	ಯದಿ (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    ಯದಿ ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    ಅನ್ಯಥಾ ಯದಿ (rows != 0)
	term->tl_rows = rows;
    ಯದಿ ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    ಅನ್ಯಥಾ ಯದಿ (cols != 0)
	term->tl_cols = cols;

    ಯದಿ (!opt->jo_hidden)
    {
	ಯದಿ (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	ಯದಿ (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	ಯದಿ (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((ವರ್ಣ *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
setup_job_options(jobopt_T *opt, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    ಯದಿ (!(opt->jo_set & JO_OUT_IO))
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
    ಯದಿ (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    ಯದಿ ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    ಯದಿ ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_flush_messages(ಶೂನ್ಯ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    ಯದಿ (old_curbuf != ರಿಕ್ತ)
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
	ವರ್ಣ	    **argv,
	jobopt_T    *opt,
	ಪೂರ್ಣಾಙ್ಕ	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res;
    buf_T	*newbuf;
    ಪೂರ್ಣಾಙ್ಕ		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_ಸಾಲು_window));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != ರಿಕ್ತ
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ರಿಕ್ತ
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    ಯದಿ (term == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, ಪರಿಮಾಣ(sb_ಸಾಲು_T), 300);
    ga_init2(&term->tl_scrollback_postponed, ಪರಿಮಾಣ(sb_ಸಾಲು_T), 300);
    ga_init2(&term->tl_osc_buf, ಪರಿಮಾಣ(ವರ್ಣ), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    ಯದಿ (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	ಯದಿ (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (do_ecmd(0, ರಿಕ್ತ, ರಿಕ್ತ, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ರಿಕ್ತ, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	ಯದಿ (buf == ರಿಕ್ತ || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    ಅನ್ಯಥಾ
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	ಯದಿ (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_ಎಣಿಕೆ = 1;
	}
	ಯದಿ (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_ಎಣಿಕೆ = 1;
	}

	ಪೂರ್ಣಾಙ್ಕ cmod_split_modified = FALSE;
	ಯದಿ (vertical)
	{
	    ಯದಿ (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	ಯದಿ (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	ಯದಿ (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    ಯದಿ (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	ಯದಿ (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	ಯದಿ (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);

    ಯದಿ (opt->jo_term_name != ರಿಕ್ತ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    ಅನ್ಯಥಾ ಯದಿ (argv != ರಿಕ್ತ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	i;
	size_t	len;
	char_u	*cmd, *p;

	ಯದಿ (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    ಯದಿ (cmd == ರಿಕ್ತ)
		cmd = (char_u *)"";
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	ಅನ್ಯಥಾ ಯದಿ (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ರಿಕ್ತ
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == ರಿಕ್ತ)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	ಪ್ರತಿ (i = 0; p != ರಿಕ್ತ; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    ಯದಿ (i == 0)
		vim_snprintf((ವರ್ಣ *)p, len, "!%s", cmd);
	    ಅನ್ಯಥಾ
		vim_snprintf((ವರ್ಣ *)p, len, "!%s (%d)", cmd, i);
	    ಯದಿ (buflist_findname(p) == ರಿಕ್ತ)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		ಭಙ್ಗ;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);

    ಯದಿ (opt->jo_term_opencmd != ರಿಕ್ತ)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    ಯದಿ (opt->jo_eof_chars != ರಿಕ್ತ)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, ಪರಿಮಾಣ(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    ಯದಿ (flags & TERM_START_NOJOB)
	ಪ್ರತ್ಯಾಗಮ curbuf;

#ಯದಿ defined(FEAT_SESSION)
    // Remember the command for the session file.
    ಯದಿ (opt->jo_term_norestore || argv != ರಿಕ್ತ)
	term->tl_command = vim_strsave((char_u *)"NONE");
    ಅನ್ಯಥಾ ಯದಿ (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	ಯದಿ (cmd != ರಿಕ್ತ && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    ಅನ್ಯಥಾ ಯದಿ (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ರಿಕ್ತ
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    ಯದಿ (s == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    ಯದಿ (p == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	ಯದಿ (item == ರಿಕ್ತ)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	ಅನ್ಯಥಾ
	    ga_clear(&ga);
    }
#endif

    ಯದಿ (opt->jo_term_kill != ರಿಕ್ತ)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    ಯದಿ (opt->jo_term_api != ರಿಕ್ತ)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    ಅನ್ಯಥಾ
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    ಯದಿ (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    ಯದಿ (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	ಯದಿ (term->tl_palette == ರಿಕ್ತ)
	{
	    vim_free(term);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, ಪರಿಮಾಣ(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    ಯದಿ (argv == ರಿಕ್ತ
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ರಿಕ್ತ
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    ಅನ್ಯಥಾ
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    ಯದಿ (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	ಯದಿ (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = ಸಂದೇಶ_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	ಯದಿ (old_curbuf != ರಿಕ್ತ)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	ಅನ್ಯಥಾ ಯದಿ (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| ಒಳಹರಿವು_busy)
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
    ಅನ್ಯಥಾ
    {
	term_close_buffer(curbuf, old_curbuf);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, newbuf);
    ಯದಿ (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, newbuf);
    ಪ್ರತ್ಯಾಗಮ newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    ಶೂನ್ಯ
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    ಪೂರ್ಣಾಙ್ಕ		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		scroll_save = ಸಂದೇಶ_scroll;

    ಸಂದೇಶ_scroll = FALSE;		// don't scroll here
    autowrite_all();
    ಸಂದೇಶ_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    ಯಾವತ್ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	ಯದಿ (ep != ರಿಕ್ತ)
	{
	    ಯದಿ (ep < p)
		p = ep;
	    ಅನ್ಯಥಾ
		ep = ರಿಕ್ತ;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((ಪೂರ್ಣಾಙ್ಕ)(p - cmd) == ಪರಿಮಾಣ(name) - 1 \
				 && STRNICMP(cmd, name, ಪರಿಮಾಣ(name) - 1) == 0)
	ಯದಿ (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("kill") && ep != ರಿಕ್ತ)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    ಯದಿ (ep != ರಿಕ್ತ)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    ಅನ್ಯಥಾ
		opt.jo_term_api = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("rows") && ep != ರಿಕ್ತ && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((ವರ್ಣ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("cols") && ep != ರಿಕ್ತ && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((ವರ್ಣ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ಅನ್ಯಥಾ ಯದಿ (OPTARG_HAS("eof") && ep != ರಿಕ್ತ)
	{
	    char_u *buf = ರಿಕ್ತ;
	    char_u *ಕೀಲಿಗಳು;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    ಕೀಲಿಗಳು = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ರಿಕ್ತ);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(ಕೀಲಿಗಳು);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	ಅನ್ಯಥಾ ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ tty_type = NUL;

	    p = skiptowhite(cmd);
	    ಯದಿ (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    ಅನ್ಯಥಾ
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "type");
		ಗಚ್ಛ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    ಗಚ್ಛ theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    ಯದಿ (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	ಯದಿ (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_ತುದಿ | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_ತುದಿ = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    ಯದಿ (opt_shell && tofree == ರಿಕ್ತ)
    {
#ifdef UNIX
	ವರ್ಣ	**argv = ರಿಕ್ತ;
	char_u	*tofree1 = ರಿಕ್ತ;
	char_u	*tofree2 = ರಿಕ್ತ;

	// :term ++shell command
	ಯದಿ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(ರಿಕ್ತ, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	ಗಚ್ಛ theend;
#ಅನ್ಯಥಾ
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	ಯದಿ (newcmd == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	tofree = newcmd;
	vim_snprintf((ವರ್ಣ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# ಅನ್ಯಥಾ
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	ಗಚ್ಛ theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, ರಿಕ್ತ, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ಸ್ಥಿರ char_u *
get_terminalopt_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    // Note: Keep this in sync with ex_terminal.
    ಸ್ಥಿರ ವರ್ಣ *(p_termopt_ಮೌಲ್ಯಗಳು[]) =
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

    ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_termopt_ಮೌಲ್ಯಗಳು))
	ಪ್ರತ್ಯಾಗಮ (char_u*)p_termopt_ಮೌಲ್ಯಗಳು[idx];
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ char_u *
get_termkill_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ಸ್ಥಿರ ವರ್ಣ *(p_termkill_ಮೌಲ್ಯಗಳು[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_termkill_ಮೌಲ್ಯಗಳು))
	ಪ್ರತ್ಯಾಗಮ (char_u*)p_termkill_ಮೌಲ್ಯಗಳು[idx];
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Command-line expansion for :terminal [options]
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ಪೂರ್ಣಾಙ್ಕ	    *ಅಂಕೆ_Matches)
{
    ಯದಿ (xp->xp_pattern > xp->xp_ಸಾಲು && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, ಪೂರ್ಣಾಙ್ಕ) = ರಿಕ್ತ;

	char_u *name_end = xp->xp_pattern - 1;
	ಯದಿ (name_end - xp->xp_ಸಾಲು >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	ಯದಿ (cb != ರಿಕ್ತ)
	{
	    ಪ್ರತ್ಯಾಗಮ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ಅಂಕೆ_Matches,
		    cb,
		    FALSE);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ಅಂಕೆ_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#ಯದಿ defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    ಯದಿ (terminal_bufs != ರಿಕ್ತ && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	ವರ್ಣ	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, ಪರಿಮಾಣ(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	ಯದಿ (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    ಯದಿ (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಪ್ರತ್ಯಾಗಮ put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    ಯದಿ (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
# ifdef MSWIN
    ಯದಿ (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
# endif
    ಯದಿ (term->tl_command != ರಿಕ್ತ)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	ಯದಿ (quoted_command == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪೂರ್ಣಾಙ್ಕ ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((ವರ್ಣ *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	ಯದಿ (ret)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (put_eol(fd) != OK)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (terminal_bufs != ರಿಕ್ತ && wp->w_buffer->b_nwindows > 1)
    {
	ವರ್ಣ *hash_ಕೀಲಿ = alloc(NUMBUFLEN);

	vim_snprintf(hash_ಕೀಲಿ, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_ಕೀಲಿ, "terminal session");
    }

    ಪ್ರತ್ಯಾಗಮ put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ && (term->tl_command == ರಿಕ್ತ
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_scrollback(term_T *term)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_ಸಾಲು_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    ಪ್ರತಿ (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_ಸಾಲು_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ಸ್ಥಿರ term_T	*terminals_to_free = ರಿಕ್ತ;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    ಶೂನ್ಯ
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    ಯದಿ (term == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Unlink the terminal form the list of terminals.
    ಯದಿ (first_term == term)
	first_term = term->tl_next;
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (tp = first_term; tp->tl_next != ರಿಕ್ತ; tp = tp->tl_next)
	    ಯದಿ (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		ಭಙ್ಗ;
	    }

    ಯದಿ (term->tl_job != ರಿಕ್ತ)
    {
	ಯದಿ (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, ರಿಕ್ತ, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = ರಿಕ್ತ;
    ಯದಿ (in_terminal_loop == term)
	in_terminal_loop = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
free_unused_terminals(ಶೂನ್ಯ)
{
    ಯಾವತ್ (terminals_to_free != ರಿಕ್ತ)
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
	ಯದಿ (term->tl_out_fd != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < 3; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	ಯದಿ (mch_isatty(fd))
	    ಪ್ರತ್ಯಾಗಮ parts[i];
    }
#endif
    ಪ್ರತ್ಯಾಗಮ PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    ವರ್ಣ   buf[ಕೀಲಿ_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, ಕೀಲಿ_BUF_LEN);

    ಯದಿ (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (ಪೂರ್ಣಾಙ್ಕ)curlen, ರಿಕ್ತ);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_write_job_output(term_T *term, char_u *ಸಂದೇಶ_arg, size_t len_arg)
{
    char_u	*ಸಂದೇಶ = ಸಂದೇಶ_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    ಯದಿ (len > limit)
    {
	char_u *p = ಸಂದೇಶ + len - limit;

	p -= (*mb_head_off)(ಸಂದೇಶ, p);
	len -= p - ಸಂದೇಶ;
	ಸಂದೇಶ = p;
    }

    vterm_ಒಳಹರಿವು_write(vterm, (ವರ್ಣ *)ಸಂದೇಶ, len);

    // flush vterm buffer when vterm responded to control sequence
    ಯದಿ (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    ಯದಿ (popup_is_popup(wp))
    {
	wp->w_wrow += popup_ತುದಿ_extra(wp);
	wp->w_wcol += popup_ಎಡ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    ಅನ್ಯಥಾ
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_cursor(term_T *term, ಪೂರ್ಣಾಙ್ಕ redraw)
{
    ಯದಿ (term->tl_normal_mode)
	ಪ್ರತ್ಯಾಗಮ;
#ifdef FEAT_GUI
    ಯದಿ (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    ಅನ್ಯಥಾ
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    ಯದಿ (redraw)
    {
	aco_save_T	aco;

	ಯದಿ (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
scrollbackline_pos_in_buf(term_T *term, ಪೂರ್ಣಾಙ್ಕ row, linenr_T *lnum, ಪೂರ್ಣಾಙ್ಕ *start_col, size_t *start_pos)
{
    sb_ಸಾಲು_T	*ಸಾಲುಗಳು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    ಪೂರ್ಣಾಙ್ಕ         calc_col = 0;
    ಪೂರ್ಣಾಙ್ಕ         i;

    ಯದಿ (row < 0 || row >= term->tl_scrollback.ga_len)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	ಪ್ರತಿ (i = term->tl_scrollback_scrolled + 1; i > 0 && ಸಾಲುಗಳು[i].continuation; --i)
	{
	    calc_pos += ಸಾಲುಗಳು[i - 1].sb_bytes;
	    calc_col += ಸಾಲುಗಳು[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	ಯದಿ (ಸಾಲುಗಳು[i].continuation)
	    ++i;
    }
    ಅನ್ಯಥಾ
    {
	i = 1;
	calc_lnum = 1;
    }

    ಪ್ರತಿ (; i <= row; ++i)
    {
	ಯದಿ (!ಸಾಲುಗಳು[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	ಅನ್ಯಥಾ
	{
	    calc_pos += ಸಾಲುಗಳು[i - 1].sb_bytes;
	    calc_col += ಸಾಲುಗಳು[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    ಯದಿ (start_col)
	*start_col = calc_col;
    ಯದಿ (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ *row, ಪೂರ್ಣಾಙ್ಕ *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_ಸಾಲು_T	*ಸಾಲುಗಳು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    ಪೂರ್ಣಾಙ್ಕ         calc_col = col;
    linenr_T    l;

    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	ಯಾವತ್ (calc_row < term->tl_scrollback.ga_len && ಸಾಲುಗಳು[calc_row].continuation)
	    ++calc_row;
    }
    ಅನ್ಯಥಾ
    {
	calc_row = 0;
	l = 1;
    }

    ಯಾವತ್ (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	ಯದಿ (!ಸಾಲುಗಳು[calc_row].continuation)
	    ++l;
    }

    ಯಾವತ್ (calc_row + 1 < term->tl_scrollback.ga_len && ಸಾಲುಗಳು[calc_row + 1].continuation
	    && calc_col >= ಸಾಲುಗಳು[calc_row].sb_cols)
    {
	calc_col -= ಸಾಲುಗಳು[calc_row].sb_cols;
	++calc_row;
    }

    ಯದಿ (row)
	*row = calc_row;
    ಯದಿ (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    ಶೂನ್ಯ
write_to_term(buf_T *buffer, char_u *ಸಂದೇಶ, channel_T *channel)
{
    size_t	len = STRLEN(ಸಂದೇಶ);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    ಯದಿ (term->tl_out_fd != ರಿಕ್ತ)
    {
	ch_log(channel, "Writing %d bytes to output file", (ಪೂರ್ಣಾಙ್ಕ)len);
	fwrite(ಸಂದೇಶ, len, 1, term->tl_out_fd);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (ಪೂರ್ಣಾಙ್ಕ)len);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ch_log(channel, "writing %d bytes to terminal", (ಪೂರ್ಣಾಙ್ಕ)len);
    cursor_off();
    term_write_job_output(term, ಸಂದೇಶ, len);

#ifdef FEAT_GUI
    ಯದಿ (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    ಅನ್ಯಥಾ
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    ಯದಿ (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	ಯದಿ (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#ಯದಿ defined(FEAT_TABPANEL)
	    ಯದಿ (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    ಯದಿ (buffer == curbuf && curbuf->b_term != ರಿಕ್ತ)
		update_cursor(curbuf->b_term, TRUE);
	}
	ಅನ್ಯಥಾ
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_send_mouse(VTerm *vterm, ಪೂರ್ಣಾಙ್ಕ button, ಪೂರ್ಣಾಙ್ಕ pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    ಪೂರ್ಣಾಙ್ಕ		    row = mouse_row - W_WINROW(curwin);
    ಪೂರ್ಣಾಙ್ಕ		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (popup_is_popup(curwin))
    {
	row -= popup_ತುದಿ_extra(curwin);
	col -= popup_ಎಡ_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    ಯದಿ (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ enter_mouse_col = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_mouse_click(VTerm *vterm, ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ)
{
#ಯದಿ defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    ಯದಿ (state == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    ಯದಿ (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	ವಿಕಲ್ಪ (ಕೀಲಿ)
	{
	ಪಕ್ಷ K_LEFTDRAG:
	ಪಕ್ಷ K_LEFTRELEASE:
	ಪಕ್ಷ K_RIGHTDRAG:
	ಪಕ್ಷ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ಯದಿ (ignore_drag_release)
		{
		    ಪೂರ್ಣಾಙ್ಕ save_mouse_col, save_mouse_row;

		    ಯದಿ (enter_mouse_col < 0)
			ಭಙ್ಗ;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_ಎಡ, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	ಪಕ್ಷ K_LEFTMOUSE:
	ಪಕ್ಷ K_RIGHTMOUSE:
		ಯದಿ (ಕೀಲಿ == K_LEFTRELEASE || ಕೀಲಿ == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		ಅನ್ಯಥಾ
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		ಯದಿ (clip_star.available)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(ಕೀಲಿ),
							 &is_click, &is_drag);
		    ಯದಿ (mouse_model_popup() && button == MOUSE_ಎಡ
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ಬಲ;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		ಭಙ್ಗ;

	ಪಕ್ಷ K_MIDDLEMOUSE:
		ಯದಿ (clip_star.available)
		    ಸೇರಿಸು_reg('*', TRUE);
		ಭಙ್ಗ;
	}
	enter_mouse_col = -1;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif
    enter_mouse_col = -1;

    ವಿಕಲ್ಪ (ಕೀಲಿ)
    {
	ಪಕ್ಷ K_LEFTMOUSE:
	ಪಕ್ಷ K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_LEFTRELEASE:
	ಪಕ್ಷ K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); ಭಙ್ಗ;
	ಪಕ್ಷ K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); ಭಙ್ಗ;
	ಪಕ್ಷ K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); ಭಙ್ಗ;
	ಪಕ್ಷ K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_convert_ಕೀಲಿ(term_T *term, ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ modmask, ವರ್ಣ *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_ಕೀಲಿ	    ಕೀಲಿ = VTERM_ಕೀಲಿ_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    ಪೂರ್ಣಾಙ್ಕ		    other = FALSE;

    ವಿಕಲ್ಪ (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	ಪಕ್ಷ K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ಯದಿ (term->tl_conpty)
		ಕೀಲಿ = VTERM_ಕೀಲಿ_BACKSPACE;
	    ಅನ್ಯಥಾ
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    ಭಙ್ಗ;

#ifdef MSWIN
	ಪಕ್ಷ BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ಯದಿ (term->tl_conpty)
		ಕೀಲಿ = VTERM_ಕೀಲಿ_BACKSPACE;
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ ESC:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ESCAPE; ಭಙ್ಗ;
	ಪಕ್ಷ K_DEL:		ಕೀಲಿ = VTERM_ಕೀಲಿ_DEL; ಭಙ್ಗ;
	ಪಕ್ಷ K_DOWN:		ಕೀಲಿ = VTERM_ಕೀಲಿ_DOWN; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_DOWN; ಭಙ್ಗ;
	ಪಕ್ಷ K_END:		ಕೀಲಿ = VTERM_ಕೀಲಿ_END; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_END:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_END; ಭಙ್ಗ;
	ಪಕ್ಷ K_C_END:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_END; ಭಙ್ಗ;
	ಪಕ್ಷ K_F10:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(10); ಭಙ್ಗ;
	ಪಕ್ಷ K_F11:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(11); ಭಙ್ಗ;
	ಪಕ್ಷ K_F12:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(12); ಭಙ್ಗ;
	ಪಕ್ಷ K_F1:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(1); ಭಙ್ಗ;
	ಪಕ್ಷ K_F2:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(2); ಭಙ್ಗ;
	ಪಕ್ಷ K_F3:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(3); ಭಙ್ಗ;
	ಪಕ್ಷ K_F4:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(4); ಭಙ್ಗ;
	ಪಕ್ಷ K_F5:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(5); ಭಙ್ಗ;
	ಪಕ್ಷ K_F6:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(6); ಭಙ್ಗ;
	ಪಕ್ಷ K_F7:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(7); ಭಙ್ಗ;
	ಪಕ್ಷ K_F8:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(8); ಭಙ್ಗ;
	ಪಕ್ಷ K_F9:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(9); ಭಙ್ಗ;
	ಪಕ್ಷ K_HOME:		ಕೀಲಿ = VTERM_ಕೀಲಿ_HOME; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_HOME:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_HOME; ಭಙ್ಗ;
	ಪಕ್ಷ K_C_HOME:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_HOME; ಭಙ್ಗ;
	ಪಕ್ಷ K_INS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_INS; ಭಙ್ಗ;
	ಪಕ್ಷ K_K0:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_0; ಭಙ್ಗ;
	ಪಕ್ಷ K_K1:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_1; ಭಙ್ಗ;
	ಪಕ್ಷ K_K2:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_2; ಭಙ್ಗ;
	ಪಕ್ಷ K_K3:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_3; ಭಙ್ಗ;
	ಪಕ್ಷ K_K4:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_4; ಭಙ್ಗ;
	ಪಕ್ಷ K_K5:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_5; ಭಙ್ಗ;
	ಪಕ್ಷ K_K6:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_6; ಭಙ್ಗ;
	ಪಕ್ಷ K_K7:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_7; ಭಙ್ಗ;
	ಪಕ್ಷ K_K8:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_8; ಭಙ್ಗ;
	ಪಕ್ಷ K_K9:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_9; ಭಙ್ಗ;
	ಪಕ್ಷ K_KDEL:		ಕೀಲಿ = VTERM_ಕೀಲಿ_DEL; ಭಙ್ಗ; // TODO
	ಪಕ್ಷ K_KDIVIDE:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_DIVIDE; ಭಙ್ಗ;
	ಪಕ್ಷ K_KEND:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_1; ಭಙ್ಗ; // TODO
	ಪಕ್ಷ K_KENTER:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_ENTER; ಭಙ್ಗ;
	ಪಕ್ಷ K_KHOME:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_7; ಭಙ್ಗ; // TODO
	ಪಕ್ಷ K_KINS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_0; ಭಙ್ಗ; // TODO
	ಪಕ್ಷ K_KMINUS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_MINUS; ಭಙ್ಗ;
	ಪಕ್ಷ K_KMULTIPLY:	ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_MULT; ಭಙ್ಗ;
	ಪಕ್ಷ K_KPAGEDOWN:	ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_3; ಭಙ್ಗ; // TODO
	ಪಕ್ಷ K_KPAGEUP:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_9; ಭಙ್ಗ; // TODO
	ಪಕ್ಷ K_KPLUS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_PLUS; ಭಙ್ಗ;
	ಪಕ್ಷ K_KPOINT:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_PERIOD; ಭಙ್ಗ;
	ಪಕ್ಷ K_ಎಡ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಎಡ; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_ಎಡ:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಎಡ; ಭಙ್ಗ;
	ಪಕ್ಷ K_C_ಎಡ:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಎಡ; ಭಙ್ಗ;
	ಪಕ್ಷ K_PAGEDOWN:	ಕೀಲಿ = VTERM_ಕೀಲಿ_PAGEDOWN; ಭಙ್ಗ;
	ಪಕ್ಷ K_PAGEUP:		ಕೀಲಿ = VTERM_ಕೀಲಿ_PAGEUP; ಭಙ್ಗ;
	ಪಕ್ಷ K_ಬಲ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಬಲ; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_ಬಲ:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಬಲ; ಭಙ್ಗ;
	ಪಕ್ಷ K_C_ಬಲ:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಬಲ; ಭಙ್ಗ;
	ಪಕ್ಷ K_UP:		ಕೀಲಿ = VTERM_ಕೀಲಿ_UP; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_UP:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_UP; ಭಙ್ಗ;
	ಪಕ್ಷ TAB:		ಕೀಲಿ = VTERM_ಕೀಲಿ_TAB; ಭಙ್ಗ;
	ಪಕ್ಷ K_S_TAB:		mod = VTERM_MOD_SHIFT;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_TAB; ಭಙ್ಗ;

	ಪಕ್ಷ K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); ಭಙ್ಗ;
	ಪಕ್ಷ K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); ಭಙ್ಗ;

	ಪಕ್ಷ K_LEFTMOUSE:
	ಪಕ್ಷ K_LEFTMOUSE_NM:
	ಪಕ್ಷ K_LEFTDRAG:
	ಪಕ್ಷ K_LEFTRELEASE:
	ಪಕ್ಷ K_LEFTRELEASE_NM:
	ಪಕ್ಷ K_MOUSEMOVE:
	ಪಕ್ಷ K_MIDDLEMOUSE:
	ಪಕ್ಷ K_MIDDLEDRAG:
	ಪಕ್ಷ K_MIDDLERELEASE:
	ಪಕ್ಷ K_RIGHTMOUSE:
	ಪಕ್ಷ K_RIGHTDRAG:
	ಪಕ್ಷ K_RIGHTRELEASE:	ಯದಿ (!term_mouse_click(vterm, c))
				    ಪ್ರತ್ಯಾಗಮ 0;
				other = TRUE;
				ಭಙ್ಗ;

	ಪಕ್ಷ K_X1MOUSE:		/* TODO */ ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_X1DRAG:		/* TODO */ ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_X1RELEASE:	/* TODO */ ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_X2MOUSE:		/* TODO */ ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_X2DRAG:		/* TODO */ ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_X2RELEASE:	/* TODO */ ಪ್ರತ್ಯಾಗಮ 0;

	ಪಕ್ಷ K_IGNORE:		ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_NOP:		ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_UNDO:		ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_HELP:		ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_XF1:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(1); ಭಙ್ಗ;
	ಪಕ್ಷ K_XF2:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(2); ಭಙ್ಗ;
	ಪಕ್ಷ K_XF3:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(3); ಭಙ್ಗ;
	ಪಕ್ಷ K_XF4:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(4); ಭಙ್ಗ;
	ಪಕ್ಷ K_SELECT:		ಪ್ರತ್ಯಾಗಮ 0;
#ifdef FEAT_GUI
	ಪಕ್ಷ K_VER_SCROLLBAR:	ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_HOR_SCROLLBAR:	ಪ್ರತ್ಯಾಗಮ 0;
#endif
#ifdef FEAT_GUI_TABLINE
	ಪಕ್ಷ K_TABLINE:		ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_TABMENU:		ಪ್ರತ್ಯಾಗಮ 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	ಪಕ್ಷ K_F21:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(21); ಭಙ್ಗ;
#endif
#ifdef FEAT_DND
	ಪಕ್ಷ K_DROP:		ಪ್ರತ್ಯಾಗಮ 0;
#endif
	ಪಕ್ಷ K_CURSORHOLD:	ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				ಭಙ್ಗ;
	ಪಕ್ಷ K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				ಭಙ್ಗ;
    }

    // add modifiers for the typed key
    ಯದಿ (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    ಯದಿ (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    ಯದಿ (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    ಯದಿ (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    ಯದಿ (ಕೀಲಿ != VTERM_ಕೀಲಿ_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_ಕೀಲಿ(vterm, ಕೀಲಿ, mod);
    ಅನ್ಯಥಾ ಯದಿ (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vterm_output_read(vterm, buf, ಕೀಲಿ_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_job_running_check(term_T *term, ಪೂರ್ಣಾಙ್ಕ check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ಯದಿ (term == ರಿಕ್ತ
	|| term->tl_job == ರಿಕ್ತ
	|| !channel_is_open(term->tl_job->jv_channel))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    ಯದಿ (check_job_status)
	job_status(job);
    ಪ್ರತ್ಯಾಗಮ (job->jv_status == JOB_STARTED
	    || (job->jv_channel != ರಿಕ್ತ && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_job_running(term_T *term)
{
    ಪ್ರತ್ಯಾಗಮ term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    ಪೂರ್ಣಾಙ್ಕ
term_job_running_not_none(term_T *term)
{
    ಪ್ರತ್ಯಾಗಮ term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    ಪೂರ್ಣಾಙ್ಕ
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ
	&& term->tl_job != ರಿಕ್ತ
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    ಪೂರ್ಣಾಙ್ಕ
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_ಸಂದೇಶ_SIZE];
    ಪೂರ್ಣಾಙ್ಕ	ret;

    dialog_ಸಂದೇಶ(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ರಿಕ್ತ, buff, 1);
    ಯದಿ (ret == VIM_YES)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_try_stop_job(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ;
    ವರ್ಣ    *how = (ವರ್ಣ *)buf->b_term->tl_kill;

#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    ಯದಿ ((how == ರಿಕ್ತ || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	ಯದಿ (term_confirm_stop(buf) == OK)
	    how = "kill";
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
#endif
    ಯದಿ (how == ರಿಕ್ತ || *how == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    job_stop(buf->b_term->tl_job, ರಿಕ್ತ, how);

    // wait for up to a second for the job to die
    ಪ್ರತಿ (ಎಣಿಕೆ = 0; ಎಣಿಕೆ < 100; ++ಎಣಿಕೆ)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	ಯದಿ (!buf_valid(buf)
		|| buf->b_term == ರಿಕ್ತ
		|| buf->b_term->tl_job == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	ಯದಿ (job->jv_status >= JOB_ENDED)
	    ಪ್ರತ್ಯಾಗಮ OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_scrollback_ಸಾಲು_to_buffer(term_T *term, char_u *text, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ append)
{
    buf_T	*buf = term->tl_buffer;
    ಪೂರ್ಣಾಙ್ಕ		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

#ifdef MSWIN
    char_u *tmp = text;
    ಯದಿ (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (ವರ್ಣ*)text, len + 1,
							   &ret, &length);
	ಯದಿ (ret != ರಿಕ್ತ)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (ವರ್ಣ **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    ಯದಿ (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	ಯದಿ (both == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    ಅನ್ಯಥಾ
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    ಯದಿ (tmp != text)
	vim_free(text);
#endif
    ಯದಿ (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cell2cellattr(ಧ್ರುವ VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    ಪ್ರತ್ಯಾಗಮ a->fg.red == b->fg.red
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, ಪೂರ್ಣಾಙ್ಕ lnum)
{
    ಯದಿ (ga_grow(&term->tl_scrollback, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    sb_ಸಾಲು_T *ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    ಯದಿ (lnum > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *ಸಾಲು = *(ಸಾಲು - 1);
	    --ಸಾಲು;
	}
    }
    ಸಾಲು->sb_cols = 0;
    ಸಾಲು->sb_bytes = 0;
    ಸಾಲು->sb_cells = ರಿಕ್ತ;
    ಸಾಲು->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_scrollback(term_T *term)
{
    sb_ಸಾಲು_T	*ಸಾಲು;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, FALSE);
    bufline_length = STRLEN(bufline);
    ಯಾವತ್ (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	ಸಾಲು = (sb_ಸಾಲು_T *)gap->ga_data + gap->ga_len - 1;
	ಯದಿ (ಸಾಲು->sb_bytes < 0 || (size_t)ಸಾಲು->sb_bytes > bufline_length)
	    ಭಙ್ಗ;
	bufline_length -= ಸಾಲು->sb_bytes;
	ಯದಿ (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(ಸಾಲು->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    ಯದಿ (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    ಯದಿ (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    ಪೂರ್ಣಾಙ್ಕ		    len;
    ಪೂರ್ಣಾಙ್ಕ		    ಸಾಲುಗಳು_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == ರಿಕ್ತ ? ರಿಕ್ತ : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    ಪ್ರತಿ (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	ಪ್ರತಿ (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    ಯದಿ (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    ಅನ್ಯಥಾ
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	ಯದಿ (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++ಸಾಲುಗಳು_skipped;
	ಅನ್ಯಥಾ
	{
	    ಯಾವತ್ (ಸಾಲುಗಳು_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--ಸಾಲುಗಳು_skipped;
		ಯದಿ (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_ಸಾಲು_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    ಯದಿ (len == 0)
		p = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		p = ALLOC_MULT(cellattr_T, len);
	    ಯದಿ ((p != ರಿಕ್ತ || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		ಪೂರ್ಣಾಙ್ಕ	    width;
		ಧ್ರುವ VTerm_ಸಾಲು_Info *lineinfo;
		sb_ಸಾಲು_T   *ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		ಪ್ರತಿ (pos.col = 0; pos.col < len; pos.col += width)
		{
		    ಯದಿ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			ಯದಿ (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    ಅನ್ಯಥಾ
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			ಯದಿ (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			ಯದಿ (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    ಪೂರ್ಣಾಙ್ಕ	    i;
			    ಪೂರ್ಣಾಙ್ಕ	    c;

			    ಪ್ರತಿ (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		ಸಾಲು->sb_cols = len;
		ಸಾಲು->sb_bytes = ga.ga_len;
		ಸಾಲು->sb_cells = p;
		ಸಾಲು->sb_fill_attr = new_fill_attr;
		ಸಾಲು->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		ಯದಿ (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_ಸಾಲು_to_buffer(term, (char_u *)"", 0, 0);
		ಅನ್ಯಥಾ
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_ಸಾಲು_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    ಅನ್ಯಥಾ
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    ಪ್ರತಿ (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	ಯದಿ (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_ಸಾಲು_to_buffer(term, (char_u *)"", 0, 0);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
for_all_windows_and_curwin(win_T **wp, ಪೂರ್ಣಾಙ್ಕ *did_curwin)
{
    ಯದಿ (*wp == ರಿಕ್ತ)
	*wp = firstwin;
    ಅನ್ಯಥಾ ಯದಿ ((*wp)->w_next != ರಿಕ್ತ)
	*wp = (*wp)->w_next;
    ಅನ್ಯಥಾ ಯದಿ (!*did_curwin)
	*wp = curwin;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (*wp == curwin)
	*did_curwin = TRUE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    ಶೂನ್ಯ
may_move_terminal_to_buffer(term_T *term, ಪೂರ್ಣಾಙ್ಕ redraw)
{
    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    ಯದಿ (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    ಯದಿ (redraw)
    {
	win_T	    *wp = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	    did_curwin = FALSE;

	ಯಾವತ್ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    ಯದಿ (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		ಯದಿ (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    ಯದಿ (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#ಯದಿ defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_check_timers(ಪೂರ್ಣಾಙ್ಕ next_due_arg, proftime_T *now)
{
    term_T  *term;
    ಪೂರ್ಣಾಙ್ಕ	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	ಯದಿ (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_ಎಡ(&term->tl_timer_due, now);

	    ಯದಿ (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    ಪ್ರತ್ಯಾಗಮ next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_terminal_mode(term_T *term, ಪೂರ್ಣಾಙ್ಕ normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    ಯದಿ (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    ಯದಿ (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    ಯದಿ (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_enter_normal_mode(ಶೂನ್ಯ)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    ಪೂರ್ಣಾಙ್ಕ        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, ರಿಕ್ತ);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    ಯದಿ (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    ಶೂನ್ಯ
term_enter_job_mode(ಶೂನ್ಯ)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    ಯದಿ (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    ಯದಿ (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_ಕೀಲಿ_state_T;

ಸ್ಥಿರ reduce_ಕೀಲಿ_state_T  no_reduce_ಕೀಲಿ_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vterm_using_ಕೀಲಿ_protocol(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curbuf->b_term != ರಿಕ್ತ
	&& curbuf->b_term->tl_vterm != ರಿಕ್ತ
	&& (vterm_is_modify_other_ಕೀಲಿಗಳು(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    ಶೂನ್ಯ
check_no_reduce_ಕೀಲಿಗಳು(ಶೂನ್ಯ)
{
    ಯದಿ (no_reduce_ಕೀಲಿ_state != NRKS_CHECK
	    || no_reduce_ಕೀಲಿಗಳು >= 1
	    || curbuf->b_term == ರಿಕ್ತ
	    || curbuf->b_term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (vterm_using_ಕೀಲಿ_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_ಕೀಲಿ_state = NRKS_SET;
	++no_reduce_ಕೀಲಿಗಳು;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_vgetc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ c;
    ಪೂರ್ಣಾಙ್ಕ save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    ಯದಿ (vterm_using_ಕೀಲಿ_protocol())
    {
	++no_reduce_ಕೀಲಿಗಳು;
	no_reduce_ಕೀಲಿ_state = NRKS_SET;
    }
    ಅನ್ಯಥಾ
    {
	no_reduce_ಕೀಲಿ_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    ಯದಿ (no_reduce_ಕೀಲಿ_state == NRKS_SET)
	--no_reduce_ಕೀಲಿಗಳು;
    no_reduce_ಕೀಲಿ_state = NRKS_NONE;

    ಪ್ರತ್ಯಾಗಮ c;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    ಪೂರ್ಣಾಙ್ಕ
send_ಕೀಲಿಗಳು_to_term(term_T *term, ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ modmask, ಪೂರ್ಣಾಙ್ಕ typed)
{
    ವರ್ಣ	ಸಂದೇಶ[ಕೀಲಿ_BUF_LEN];
    size_t	len;
    ಪೂರ್ಣಾಙ್ಕ		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ NUL:
	ಪಕ್ಷ K_ZERO:
	    ಯದಿ (typed)
		stuffcharReadbuff(c);
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ K_TABLINE:
	    stuffcharReadbuff(c);
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ K_IGNORE:
	ಪಕ್ಷ K_CANCEL:  // used for :normal when running out of chars
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ K_LEFTDRAG:
	ಪಕ್ಷ K_MIDDLEDRAG:
	ಪಕ್ಷ K_RIGHTDRAG:
	ಪಕ್ಷ K_X1DRAG:
	ಪಕ್ಷ K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	ಪಕ್ಷ K_LEFTMOUSE:
	ಪಕ್ಷ K_LEFTMOUSE_NM:
	ಪಕ್ಷ K_LEFTRELEASE:
	ಪಕ್ಷ K_LEFTRELEASE_NM:
	ಪಕ್ಷ K_MOUSEMOVE:
	ಪಕ್ಷ K_MIDDLEMOUSE:
	ಪಕ್ಷ K_MIDDLERELEASE:
	ಪಕ್ಷ K_RIGHTMOUSE:
	ಪಕ್ಷ K_RIGHTRELEASE:
	ಪಕ್ಷ K_X1MOUSE:
	ಪಕ್ಷ K_X1RELEASE:
	ಪಕ್ಷ K_X2MOUSE:
	ಪಕ್ಷ K_X2RELEASE:

	ಪಕ್ಷ K_MOUSEUP:
	ಪಕ್ಷ K_MOUSEDOWN:
	ಪಕ್ಷ K_MOUSELEFT:
	ಪಕ್ಷ K_MOUSERIGHT:
	    {
		ಪೂರ್ಣಾಙ್ಕ	row = mouse_row;
		ಪೂರ್ಣಾಙ್ಕ	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		ಯದಿ (popup_is_popup(curwin))
		{
		    row -= popup_ತುದಿ_extra(curwin);
		    col -= popup_ಎಡ_extra(curwin);
		}
#endif
		ಯದಿ (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    ಯದಿ (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_COMMAND:
	ಪಕ್ಷ K_SCRIPT_COMMAND:
	    ಪ್ರತ್ಯಾಗಮ do_cmdkey_command(c, 0);
    }
    ಯದಿ (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_ಕೀಲಿ(term, c, modmask, ಸಂದೇಶ);
    ಯದಿ (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)ಸಂದೇಶ, (ಪೂರ್ಣಾಙ್ಕ)len, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_paste_register(ಪೂರ್ಣಾಙ್ಕ prev_c UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    ಪೂರ್ಣಾಙ್ಕ		type;

    ಯದಿ (add_to_showcmd(prev_c))
    ಯದಿ (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    ಯದಿ (!term_use_loop())
	// job finished while waiting for a character
	ಪ್ರತ್ಯಾಗಮ;

    // CTRL-W "= prompt for expression to evaluate.
    ಯದಿ (c == '=' && get_ಅಭಿವ್ಯಕ್ತಿ_register() != '=')
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!term_use_loop())
	// job finished while waiting for a character
	ಪ್ರತ್ಯಾಗಮ;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	ಯದಿ (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (ವರ್ಣ *)s,
		    (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s), &ret, &length);
	    ಯದಿ (ret != ರಿಕ್ತ)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (ವರ್ಣ **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s), ರಿಕ್ತ);
#ifdef MSWIN
	ಯದಿ (tmp != s)
	    vim_free(s);
#endif

	ಯದಿ (item->li_next != ರಿಕ್ತ || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, ರಿಕ್ತ);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    ಪೂರ್ಣಾಙ್ಕ
terminal_is_active(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ in_terminal_loop != ರಿಕ್ತ;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    ಯದಿ (wp != ರಿಕ್ತ && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    ಅನ್ಯಥಾ ಯದಿ (term->tl_highlight_name != ರಿಕ್ತ)
	name = term->tl_highlight_name;
    ಅನ್ಯಥಾ
	name = (char_u*)"Terminal";

    ಪ್ರತ್ಯಾಗಮ syn_name2id(name);
}

#ಯದಿ defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    ಸ್ಥಿರ cursorentry_T entry;
    ಪೂರ್ಣಾಙ್ಕ			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ಎಡ ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    ಯದಿ (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    ಯದಿ (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    ಯದಿ (term_bg != INVALCOLOR)
	*fg = term_bg;
    ಅನ್ಯಥಾ
	*fg = gui.back_pixel;

    ಯದಿ (term->tl_cursor_color == ರಿಕ್ತ)
    {
	ಯದಿ (term_fg != INVALCOLOR)
	    *bg = term_fg;
	ಅನ್ಯಥಾ
	    *bg = gui.norm_pixel;
    }
    ಅನ್ಯಥಾ
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    ಪ್ರತ್ಯಾಗಮ &entry;
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
may_output_cursor_props(ಶೂನ್ಯ)
{
    ಯದಿ (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	ಯದಿ (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	ಅನ್ಯಥಾ
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    ಯದಿ (in_terminal_loop == term)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_restore_cursor_props(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    cursor_color_copy(&desired_cursor_color, ರಿಕ್ತ);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_use_loop_check(ಪೂರ್ಣಾಙ್ಕ check_job_status)
{
    term_T *term = curbuf->b_term;

    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ
	&& !term->tl_normal_mode
	&& term->tl_vterm != ರಿಕ್ತ
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_use_loop(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    ಶೂನ್ಯ
term_win_entered(ಶೂನ್ಯ)
{
    term_T *term = curbuf->b_term;

    ಯದಿ (term == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	ಯದಿ (State & MODE_ಸೇರಿಸು)
	    stop_ಸೇರಿಸು_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    ಶೂನ್ಯ
term_focus_change(ಪೂರ್ಣಾಙ್ಕ in_focus)
{
    term_T *term = curbuf->b_term;

    ಯದಿ (term == ರಿಕ್ತ || term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    ಯದಿ (in_focus)
	vterm_state_focus_in(state);
    ಅನ್ಯಥಾ
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
raw_c_to_ctrl(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	ಪ್ರತ್ಯಾಗಮ c & 0x1f;
    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ctrl_to_raw_c(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c < 0x20 && vterm_using_ಕೀಲಿ_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	ಪ್ರತ್ಯಾಗಮ c + '@';
    }
    ಪ್ರತ್ಯಾಗಮ c;
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
    ಪೂರ್ಣಾಙ್ಕ
terminal_loop(ಪೂರ್ಣಾಙ್ಕ blocking)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		raw_c;
    ಪೂರ್ಣಾಙ್ಕ		termwinkey = 0;
    ಪೂರ್ಣಾಙ್ಕ		ret;
#ifdef UNIX
    ಪೂರ್ಣಾಙ್ಕ		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    ಪೂರ್ಣಾಙ್ಕ		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    ಯದಿ (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_ಕೀಲಿ(curwin->w_p_twk, TRUE);
	ಯದಿ (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    ಯಾವತ್ (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	ಯದಿ (curbuf->b_term != ರಿಕ್ತ && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    ಯಾವತ್ (must_redraw != 0)
		ಯದಿ (update_screen(0) == FAIL)
		    ಭಙ್ಗ;
	ಯದಿ (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    ಭಙ್ಗ;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	ಯದಿ (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    ಯದಿ (raw_c != K_IGNORE)
		vungetc(raw_c);
	    ಭಙ್ಗ;
	}
	ಯದಿ (raw_c == K_IGNORE)
	    ಅನುವರ್ತ;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	ಯದಿ (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    ಯದಿ (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	ಯದಿ (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	ಯದಿ ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    prev_c = c;
	    ಪೂರ್ಣಾಙ್ಕ	    prev_raw_c = raw_c;
	    ಪೂರ್ಣಾಙ್ಕ	    prev_mod_mask = mod_mask;

	    ಯದಿ (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    ಯದಿ (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		ಭಙ್ಗ;

	    ಯದಿ (prev_c == Ctrl_BSL)
	    {
		ಯದಿ (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    ಗಚ್ಛ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_ಕೀಲಿಗಳು_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		ಗಚ್ಛ theend;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == '"')
	    {
		term_paste_register(prev_c);
		ಅನುವರ್ತ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		ಗಚ್ಛ theend;
	    }
	}
#ifdef MSWIN
	ಯದಿ (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    ಯದಿ (MultiByteToWideChar(GetACP(), 0, (ವರ್ಣ*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	ಯದಿ (send_ಕೀಲಿಗಳು_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    ಯದಿ (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    ಗಚ್ಛ theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = ರಿಕ್ತ;
    ಯದಿ (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    ಯದಿ (curbuf->b_term != ರಿಕ್ತ && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_toggle_cursor(term_T *term)
{
    ಯದಿ (in_terminal_loop != term)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (term->tl_cursor_visible)
	cursor_on();
    ಅನ್ಯಥಾ
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
color2index(VTermColor *color, ಪೂರ್ಣಾಙ್ಕ fg, ಪೂರ್ಣಾಙ್ಕ *boldp)
{
    ಪೂರ್ಣಾಙ್ಕ red = color->red;
    ಪೂರ್ಣಾಙ್ಕ blue = color->blue;
    ಪೂರ್ಣಾಙ್ಕ green = color->green;

    *boldp = FALSE;

    ಯದಿ (VTERM_COLOR_IS_INVALID(color))
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	ಯದಿ (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    ಯದಿ (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    ಪ್ರತ್ಯಾಗಮ index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	ಅನ್ಯಥಾ ಯದಿ (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    ಪ್ರತ್ಯಾಗಮ (color->index & 7) + 1;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (t_colors >= 256)
    {
	ಯದಿ (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಯದಿ (red < 5)
		ಪ್ರತ್ಯಾಗಮ 17; // 00/00/00
	    ಯದಿ (red > 245) // ff/ff/ff
		ಪ್ರತ್ಯಾಗಮ 232;
	    ಪ್ರತಿ (i = 0; i < 23; ++i)
		ಯದಿ (red < cutoff[i])
		    ಪ್ರತ್ಯಾಗಮ i + 233;
	    ಪ್ರತ್ಯಾಗಮ 256;
	}
	{
	    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    ಪೂರ್ಣಾಙ್ಕ ri, gi, bi;

	    // 216-color cube
	    ಪ್ರತಿ (ri = 0; ri < 5; ++ri)
		ಯದಿ (red < cutoff[ri])
		    ಭಙ್ಗ;
	    ಪ್ರತಿ (gi = 0; gi < 5; ++gi)
		ಯದಿ (green < cutoff[gi])
		    ಭಙ್ಗ;
	    ಪ್ರತಿ (bi = 0; bi < 5; ++bi)
		ಯದಿ (blue < cutoff[bi])
		    ಭಙ್ಗ;
	    ಪ್ರತ್ಯಾಗಮ 17 + ri * 36 + gi * 6 + bi;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    ಪೂರ್ಣಾಙ್ಕ attr = 0;

    ಯದಿ (cellattrs->bold)
	attr |= HL_BOLD;
    ಯದಿ (cellattrs->underline)
	attr |= HL_UNDERLINE;
    ಯದಿ (cellattrs->italic)
	attr |= HL_ITALIC;
    ಯದಿ (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    ಯದಿ (cellattrs->reverse)
	attr |= HL_INVERSE;
    ಪ್ರತ್ಯಾಗಮ attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl2vtermAttr(ಪೂರ್ಣಾಙ್ಕ attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    ಯದಿ (attr & HL_BOLD)
	cell->attrs.bold = 1;
    ಯದಿ (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    ಯದಿ (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    ಯದಿ (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    ಯದಿ (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    ಪೂರ್ಣಾಙ್ಕ attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    ಪೂರ್ಣಾಙ್ಕ is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    ಪೂರ್ಣಾಙ್ಕ is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    ಯದಿ (is_default_fg || is_default_bg)
    {
	ಯದಿ (wp != ರಿಕ್ತ && wp->w_hlfwin_id != 0)
	{
	    ಯದಿ (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    ಯದಿ (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (is_default_fg)
		fg = &term->tl_default_color.fg;
	    ಯದಿ (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	ಪ್ರತ್ಯಾಗಮ get_gui_attr_idx(attr, guifg, guibg);
    }
    ಅನ್ಯಥಾ
#endif
#ifdef FEAT_TERMGUICOLORS
    ಯದಿ (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	ಪ್ರತ್ಯಾಗಮ get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    ಅನ್ಯಥಾ
#endif
    {
	ಪೂರ್ಣಾಙ್ಕ bold = MAYBE;
	ಪೂರ್ಣಾಙ್ಕ ctermfg = color2index(fg, TRUE, &bold);
	ಪೂರ್ಣಾಙ್ಕ ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	ಯದಿ (bold == TRUE)
	    attr |= HL_BOLD;

	ಪ್ರತ್ಯಾಗಮ get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    ಯದಿ (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_damage(VTermRect rect, ಶೂನ್ಯ *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
term_scroll_up(term_T *term, ಪೂರ್ಣಾಙ್ಕ start_row, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    win_T		 *wp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    ಪೂರ್ಣಾಙ್ಕ			 clear_attr;

    CLEAR_FIELD(attr);

    ಯಾವತ್ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ಯದಿ (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_ಸಾಲುಗಳು(wp, start_row, ಎಣಿಕೆ, FALSE, FALSE, clear_attr);
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_moverect(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user)
{
    term_T	*term = (term_T *)user;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    ಯದಿ (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	ಯದಿ (dest.start_row == 0)
	    term->tl_postponed_scroll += ಎಣಿಕೆ;
	ಅನ್ಯಥಾ
	    term_scroll_up(term, dest.start_row, ಎಣಿಕೆ);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	ಪೂರ್ಣಾಙ್ಕ visible,
	ಶೂನ್ಯ *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    ಯಾವತ್ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ಯದಿ (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    ಯದಿ (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_settermprop(
	VTermProp prop,
	VTerm_ಮೌಲ್ಯ *ಮೌಲ್ಯ,
	ಶೂನ್ಯ *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = ರಿಕ್ತ;

    ವಿಕಲ್ಪ (prop)
    {
	ಪಕ್ಷ VTERM_PROP_TITLE:
	    ಯದಿ (disable_vterm_title_for_testing)
		ಭಙ್ಗ;
	    strval = vim_strnsave((char_u *)ಮೌಲ್ಯ->string.str,
							    ಮೌಲ್ಯ->string.len);
	    ಯದಿ (strval == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    ಯದಿ (*skipwhite(strval) == NUL)
		term->tl_title = ರಿಕ್ತ;
	    // Same as blank
	    ಅನ್ಯಥಾ ಯದಿ (term->tl_arg0_cmd != ರಿಕ್ತ
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (ಪೂರ್ಣಾಙ್ಕ)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = ರಿಕ್ತ;
	    // Empty corrupted data of winpty
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = ರಿಕ್ತ;
#ifdef MSWIN
	    ಅನ್ಯಥಾ ಯದಿ (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ರಿಕ್ತ;
		ಪೂರ್ಣಾಙ್ಕ	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(ವರ್ಣ*)ಮೌಲ್ಯ->string.str,
					(ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ->string.len, &ret, &length);
		ಯದಿ (ret != ರಿಕ್ತ)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (ವರ್ಣ**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    ಅನ್ಯಥಾ
	    {
		term->tl_title = strval;
		strval = ರಿಕ್ತ;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    ಯದಿ (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = ಮೌಲ್ಯ->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    ಭಙ್ಗ;

	ಪಕ್ಷ VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = ಮೌಲ್ಯ->boolean;
	    may_set_cursor_props(term);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = ಮೌಲ್ಯ->ಸಂಖ್ಯೆ;
	    may_set_cursor_props(term);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)ಮೌಲ್ಯ->string.str,
							    ಮೌಲ್ಯ->string.len);
	    ಯದಿ (strval == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = ಮೌಲ್ಯ->boolean;
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_resize(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, ಶೂನ್ಯ *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    ಯದಿ (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    ಅನ್ಯಥಾ
    {
	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
limit_scrollback(term_T *term, garray_T *gap, ಪೂರ್ಣಾಙ್ಕ update_buffer)
{
    ಯದಿ (gap->ga_len < term->tl_buffer->b_p_twsl)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    ಪೂರ್ಣಾಙ್ಕ	i;
    sb_ಸಾಲು_T *sb_ಸಾಲುಗಳು = (sb_ಸಾಲು_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    ಪ್ರತಿ (i = 0; i < todo; ++i)
    {
	ಯದಿ (update_buffer && (!sb_ಸಾಲುಗಳು[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_ಸಾಲುಗಳು[i].sb_cells);
    }
    // Continue until end of wrapped line
    ಪ್ರತಿ (; todo < gap->ga_len && sb_ಸಾಲುಗಳು[todo].continuation; ++todo)
	vim_free(sb_ಸಾಲುಗಳು[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_ಸಾಲು_T *)gap->ga_data + todo,
		ಪರಿಮಾಣ(sb_ಸಾಲು_T) * gap->ga_len);
    ಯದಿ (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ರಿಕ್ತ;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp->w_buffer == term->tl_buffer)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_pushline(ಪೂರ್ಣಾಙ್ಕ cols, ಧ್ರುವ VTermScreenCell *cells, ಪೂರ್ಣಾಙ್ಕ continuation, ಶೂನ್ಯ *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    ಪೂರ್ಣಾಙ್ಕ		update_buffer;

    ಯದಿ (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    cellattr_T	*p = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		text_len;
    char_u		*text;
    sb_ಸಾಲು_T	*ಸಾಲು;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    ಪ್ರತಿ (i = 0; i < cols; ++i)
	ಯದಿ (cells[i].chars[0] != 0)
	    len = i + 1;
	ಅನ್ಯಥಾ
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    ಯದಿ (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    ಯದಿ (p != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (col = 0; col < len; col += cells[col].width)
	{
	    ಯದಿ (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		ಭಙ್ಗ;
	    }
	    ಪ್ರತಿ (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    ಯದಿ (ga_grow(&ga, 1) == FAIL)
    {
	ಯದಿ (update_buffer)
	    text = (char_u *)"";
	ಅನ್ಯಥಾ
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    ಅನ್ಯಥಾ
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    ಯದಿ (update_buffer)
	add_scrollback_ಸಾಲು_to_buffer(term, text, text_len, continuation);

    ಸಾಲು = (sb_ಸಾಲು_T *)gap->ga_data + gap->ga_len;
    ಸಾಲು->sb_cols = len;
    ಸಾಲು->sb_bytes = text_len;
    ಸಾಲು->sb_cells = p;
    ಸಾಲು->sb_fill_attr = fill_attr;
    ಸಾಲು->continuation = (char_u)continuation;
    ಯದಿ (update_buffer)
    {
	ಸಾಲು->sb_text = ರಿಕ್ತ;
	++term->tl_scrollback_scrolled;
	ಯದಿ (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    ಅನ್ಯಥಾ
    {
	ಸಾಲು->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_postponed_scrollback(term_T *term)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (term->tl_scrollback_postponed.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ch_log(ರಿಕ್ತ, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    ಪ್ರತಿ (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_ಸಾಲು_T	*pp_ಸಾಲು;
	sb_ಸಾಲು_T	*ಸಾಲು;

	ಯದಿ (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    ಭಙ್ಗ;
	pp_ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_ಸಾಲು->sb_text;
	ಯದಿ (text == ರಿಕ್ತ)
	    text = (char_u *)"";
	add_scrollback_ಸಾಲು_to_buffer(term, text, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(text), pp_ಸಾಲು->continuation);
	vim_free(pp_ಸಾಲು->sb_text);

	ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	ಸಾಲು->sb_cols = pp_ಸಾಲು->sb_cols;
	ಸಾಲು->sb_bytes = pp_ಸಾಲು->sb_bytes;
	ಸಾಲು->sb_cells = pp_ಸಾಲು->sb_cells;
	ಸಾಲು->sb_fill_attr = pp_ಸಾಲು->sb_fill_attr;
	ಸಾಲು->sb_text = ರಿಕ್ತ;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	ಯದಿ (!pp_ಸಾಲು->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_bell(ಶೂನ್ಯ *user UNUSED)
{
    vim_beep(BO_TERM);
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ರಿಕ್ತ,			// sb_pushline
    ರಿಕ್ತ,			// sb_popline
    ರಿಕ್ತ,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    ಯದಿ (!term->tl_normal_mode)
    {
	ಪೂರ್ಣಾಙ್ಕ	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	ಯದಿ (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    ಪೂರ್ಣಾಙ್ಕ		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = ರಿಕ್ತ;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    ಯದಿ (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		ಯದಿ (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    ಅನ್ಯಥಾ
#endif
	    // If this is the last normal window: exit Vim.
	    ಯದಿ (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ರಿಕ್ತ, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    ಯದಿ (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		ಯದಿ (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		ಯದಿ (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		ಯದಿ (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    ಯದಿ (pwin != ರಿಕ್ತ)
		popup_close_with_retval(pwin, 0);
#endif
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	ಯದಿ (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    ವರ್ಣ    *cmd = term->tl_opencmd == ರಿಕ್ತ
				? "botright sbuf %d"
				: (ವರ್ಣ *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    ವರ್ಣ    *buf = alloc(len);

	    ಯದಿ (buf != ರಿಕ್ತ)
	    {
		ch_log(ರಿಕ್ತ, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	ಅನ್ಯಥಾ
	    ch_log(ರಿಕ್ತ, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_close_term_popup(ಶೂನ್ಯ)
{
    ಯದಿ (!popup_is_popup(curwin) || curbuf->b_term == ರಿಕ್ತ
				 || term_job_running_not_none(curbuf->b_term))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    win_T *pwin = curwin;

    ಯದಿ (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    ಶೂನ್ಯ
term_channel_closing(channel_T *ch)
{
    term_T *term;

    ಪ್ರತಿ (term = first_term; term != ರಿಕ್ತ; term = term->tl_next)
	ಯದಿ (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    ಶೂನ್ಯ
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    ಪೂರ್ಣಾಙ್ಕ	    did_one = FALSE;

    ಪ್ರತಿ (term = first_term; term != ರಿಕ್ತ; term = next_term)
    {
	next_term = term->tl_next;
	ಯದಿ (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    ಯದಿ (term->tl_out_fd != ರಿಕ್ತ)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = ರಿಕ್ತ;
	    }
#endif

	    ಯದಿ (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		ಅನುವರ್ತ;
	    }

	    ಯದಿ (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    ಯದಿ (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	ಯದಿ (term != ರಿಕ್ತ)
	{
	    ಯದಿ (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    ಶೂನ್ಯ
term_check_channel_closed_recently(ಶೂನ್ಯ)
{
    term_T *term;
    term_T *next_term;

    ಪ್ರತಿ (term = first_term; term != ರಿಕ್ತ; term = next_term)
    {
	next_term = term->tl_next;
	ಯದಿ (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    ಯದಿ (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	ಪೂರ್ಣಾಙ್ಕ		max_col)
{
    ಪೂರ್ಣಾಙ್ಕ off = screen_get_current_ಸಾಲು_off();

    ಪ್ರತಿ (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	ಪೂರ್ಣಾಙ್ಕ		c;

	ಯದಿ (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	ಯದಿ (c == NUL)
	{
	    Screen_ಸಾಲುಗಳು[off] = ' ';
	    ಯದಿ (enc_utf8)
		Screen_ಸಾಲುಗಳು_UC[off] = NUL;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (enc_utf8)
	    {
		ಪೂರ್ಣಾಙ್ಕ i;

		// composing chars
		ಪ್ರತಿ (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_ಸಾಲುಗಳು_C[i][off] = cell.chars[i + 1];
		    ಯದಿ (cell.chars[i + 1] == 0)
			ಭಙ್ಗ;
		}
		ಯದಿ (c >= 0x80 || (Screen_mco > 0
					 && Screen_ಸಾಲುಗಳು_C[0][off] != 0))
		{
		    Screen_ಸಾಲುಗಳು[off] = ' ';
		    Screen_ಸಾಲುಗಳು_UC[off] = c;
		}
		ಅನ್ಯಥಾ
		{
		    Screen_ಸಾಲುಗಳು[off] = c;
		    Screen_ಸಾಲುಗಳು_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		ಯದಿ (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (ವರ್ಣ*)mb, 2, 0, 0) > 1)
		{
		    Screen_ಸಾಲುಗಳು[off] = mb[0];
		    Screen_ಸಾಲುಗಳು[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		ಅನ್ಯಥಾ
		    Screen_ಸಾಲುಗಳು[off] = c;
	    }
#endif
	    ಅನ್ಯಥಾ
		// This will only store the lower byte of "c".
		Screen_ಸಾಲುಗಳು[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	ಯದಿ (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    ಯದಿ (enc_utf8)
	    {
		Screen_ಸಾಲುಗಳು_UC[off] = NUL;
		Screen_ಸಾಲುಗಳು[off] = NUL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_ಸಾಲುಗಳು[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#ಯದಿ defined(FEAT_GUI)
    ಸ್ಥಿರ ಶೂನ್ಯ
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    ಯಾವತ್ (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	ಪೂರ್ಣಾಙ್ಕ save_p_more = p_more;

	p_more = FALSE;
	ಸಂದೇಶ_row = Rows - 1;
	ಸಂದೇಶ_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    ಪ್ರತಿ (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	ಯದಿ (pos.row < term->tl_rows)
	{
	    ಪೂರ್ಣಾಙ್ಕ max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, ರಿಕ್ತ, screen, &pos, max_col);
	}
	ಅನ್ಯಥಾ
	    pos.col = 0;

	screen_ಸಾಲು(curwin, term->tl_toprow + pos.row,
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
    ಪೂರ್ಣಾಙ್ಕ
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ && term->tl_vterm != ರಿಕ್ತ && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    ಶೂನ್ಯ
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    ಪೂರ್ಣಾಙ್ಕ		rows, cols;
    ಪೂರ್ಣಾಙ್ಕ		newrows, newcols;
    ಪೂರ್ಣಾಙ್ಕ		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    ಯದಿ (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	ಯದಿ (term->tl_postponed_scroll > 0
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
    ಪ್ರತಿ (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ರಿಕ್ತ ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	ಯದಿ (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	ಯದಿ (twp == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }
    ಯದಿ (newrows == 99999 || newcols == 99999)
	ಪ್ರತ್ಯಾಗಮ; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    ಯದಿ (newrows == 0 || newcols == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	ಯದಿ (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    ಪ್ರತಿ (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	ಯದಿ (pos.row < term->tl_rows)
	{
	    ಪೂರ್ಣಾಙ್ಕ max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	ಅನ್ಯಥಾ
	    pos.col = 0;

	screen_ಸಾಲು(wp, wp->w_winrow + pos.row
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
    ಶೂನ್ಯ
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    ಯದಿ (term == ರಿಕ್ತ || term->tl_vterm == ರಿಕ್ತ || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	ಪ್ರತ್ಯಾಗಮ;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_is_finished(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf->b_term != ರಿಕ್ತ && buf->b_term->tl_vterm == ರಿಕ್ತ;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ && (term->tl_vterm == ರಿಕ್ತ || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    ಶೂನ್ಯ
term_change_in_curbuf(ಶೂನ್ಯ)
{
    term_T *term = curbuf->b_term;

    ಯದಿ (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಪೂರ್ಣಾಙ್ಕ
term_get_attr(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_ಸಾಲು_T	*ಸಾಲು;
    cellattr_T	*cellattr;
    ಪೂರ್ಣಾಙ್ಕ         sb_ಸಾಲು = -1;
    ಪೂರ್ಣಾಙ್ಕ         sb_col = col;

    ಯದಿ (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_ಸಾಲು, &sb_col);

    ಯದಿ (sb_ಸಾಲು < 0)
	cellattr = &term->tl_default_color;
    ಅನ್ಯಥಾ
    {
	ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data + sb_ಸಾಲು;
	ಯದಿ (sb_col < 0 || sb_col >= ಸಾಲು->sb_cols)
	    cellattr = &ಸಾಲು->sb_fill_attr;
	ಅನ್ಯಥಾ
	    cellattr = ಸಾಲು->sb_cells + sb_col;
    }
    ಪ್ರತ್ಯಾಗಮ cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cterm_color2vterm(ಪೂರ್ಣಾಙ್ಕ nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    ಯದಿ (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_vterm_color_from_synid(ಪೂರ್ಣಾಙ್ಕ id, VTermColor *fg, VTermColor *bg)
{
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    ಯದಿ (0
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

	ಯದಿ (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	ಯದಿ (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	ಅನ್ಯಥಾ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ಯದಿ (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	ಅನ್ಯಥಾ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (t_colors >= 16)
    {
	ಪೂರ್ಣಾಙ್ಕ cterm_fg = -1;
	ಪೂರ್ಣಾಙ್ಕ cterm_bg = -1;

	ಯದಿ (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	ಯದಿ (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	ಅನ್ಯಥಾ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ಯದಿ (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	ಅನ್ಯಥಾ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಶೂನ್ಯ
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    ಶೂನ್ಯ
term_update_hlfwin(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ id = wp->w_hlfwin_id;

    ಯದಿ (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ಶೂನ್ಯ
term_update_hlfwin_all(ಶೂನ್ಯ)
{
    win_T	 *wp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		 did_curwin = FALSE;

    ಯಾವತ್ (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    ಶೂನ್ಯ
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    ಪೂರ್ಣಾಙ್ಕ		    fgval, bgval;
    ಪೂರ್ಣಾಙ್ಕ		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    ಯದಿ (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    ಅನ್ಯಥಾ
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, ರಿಕ್ತ);

    ಯದಿ (!get_vterm_color_from_synid(id, fg, bg))
    {
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ಪೂರ್ಣಾಙ್ಕ tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	ಯದಿ (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ಯದಿ (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	ಅನ್ಯಥಾ
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	ಯದಿ (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ಯದಿ (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	ಅನ್ಯಥಾ
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_use_palette(ಶೂನ್ಯ)
{
    ಯದಿ (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    ಪೂರ್ಣಾಙ್ಕ		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    ಪ್ರತಿ (; index < 16; index++)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    ಪೂರ್ಣಾಙ್ಕ		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    ಪ್ರತಿ (li = list->lv_first; li != ರಿಕ್ತ && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ಯದಿ (color_name == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	ಯದಿ (guicolor == INVALCOLOR)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    ಯದಿ (n != 16 || li != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    set_vterm_palette(vterm, rgb);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", ರಿಕ್ತ, TRUE);

    ಯದಿ (var == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ರಿಕ್ತ
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    ಪೂರ್ಣಾಙ್ಕ		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ರಿಕ್ತ;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಯದಿ (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    CLEAR_FIELD(ea);

    ಯದಿ (opt_item != ರಿಕ್ತ && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != ರಿಕ್ತ)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	ಯದಿ (p == ರಿಕ್ತ)
	    p = dict_get_string(dict, "fileformat", FALSE);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಯದಿ (check_ff_ಮೌಲ್ಯ(p) == FAIL)
		ch_log(ರಿಕ್ತ, "Invalid ff argument to drop: %s", p);
	    ಅನ್ಯಥಾ
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	ಯದಿ (p == ರಿಕ್ತ)
	    p = dict_get_string(dict, "encoding", FALSE);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    ಯದಿ (ea.cmd != ರಿಕ್ತ)
	    {
		sprintf((ವರ್ಣ *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	ಯದಿ (p != ರಿಕ್ತ)
	    get_bad_opt(p, &ea);

	ಯದಿ (dict_has_ಕೀಲಿ(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	ಯದಿ (dict_has_ಕೀಲಿ(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	ಯದಿ (dict_has_ಕೀಲಿ(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	ಯದಿ (dict_has_ಕೀಲಿ(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    ಯದಿ (ea.cmd == ರಿಕ್ತ)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_permitted_term_api(char_u *func, char_u *pat)
{
    ಪ್ರತ್ಯಾಗಮ pat != ರಿಕ್ತ && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    ಯದಿ (item->li_next == ರಿಕ್ತ)
    {
	ch_log(channel, "Missing function arguments for call");
	ಪ್ರತ್ಯಾಗಮ;
    }
    func = tv_get_string(&item->li_tv);

    ಯದಿ (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	ಪ್ರತ್ಯಾಗಮ;
    }

    argvars[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argvars[0].vval.v_ಸಂಖ್ಯೆ = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    ಯದಿ (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ size_t
url_decode(ಧ್ರುವ ವರ್ಣ *src, ಧ್ರುವ size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    ಯಾವತ್ (i < len)
    {
	ಯದಿ (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	ಅನ್ಯಥಾ
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    ಪ್ರತ್ಯಾಗಮ j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sync_shell_dir(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ       offset = 7;  // len of "file://" is 7
    ವರ್ಣ      *pos = (ವರ್ಣ *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    ಯಾವತ್ (offset < (ಪೂರ್ಣಾಙ್ಕ)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    ಯದಿ (offset >= (ಪೂರ್ಣಾಙ್ಕ)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_osc(ಪೂರ್ಣಾಙ್ಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == ರಿಕ್ತ ? ರಿಕ್ತ
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    ಯದಿ (command != 51 && (command != 7 || !p_asd))
	ಪ್ರತ್ಯಾಗಮ 0;

    // Concatenate what was received until the final piece is found.
    ಯದಿ (ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    mch_memmove((ವರ್ಣ *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (ಪೂರ್ಣಾಙ್ಕ)frag.len;
    ಯದಿ (!frag.final)
	ಪ್ರತ್ಯಾಗಮ 1;

    ((ವರ್ಣ *)gap->ga_data)[gap->ga_len] = 0;

    ಯದಿ (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = ರಿಕ್ತ;
    reader.js_used = 0;
    ಯದಿ (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ರಿಕ್ತ)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	ಯದಿ (item == ರಿಕ್ತ)
	    ch_log(channel, "Missing command");
	ಅನ್ಯಥಾ
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    ಯದಿ (item == ರಿಕ್ತ)
		ch_log(channel, "Missing argument for %s", cmd);
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    ಅನ್ಯಥಾ
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    ಅನ್ಯಥಾ
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_csi(
	ಧ್ರುವ ವರ್ಣ  *leader UNUSED,
	ಧ್ರುವ long  args[],
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	ಧ್ರುವ ವರ್ಣ  *intermed UNUSED,
	ವರ್ಣ	    command,
	ಶೂನ್ಯ	    *user)
{
    term_T	*term = (term_T *)user;
    ವರ್ಣ	buf[100];
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		x = 0;
    ಪೂರ್ಣಾಙ್ಕ		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    ಯದಿ (command != 't' || argcount != 1 || args[0] != 13)
	ಪ್ರತ್ಯಾಗಮ 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#ಯದಿ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (ಶೂನ್ಯ)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer == term->tl_buffer)
	    ಭಙ್ಗ;
    ಯದಿ (wp != ರಿಕ್ತ)
    {
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	ಅನ್ಯಥಾ
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
						     (char_u *)buf, len, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ VTermStateFallbacks state_fallbacks = {
    ರಿಕ್ತ,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ರಿಕ್ತ,		// dcs
    ರಿಕ್ತ,		// apc
    ರಿಕ್ತ,		// pm
    ರಿಕ್ತ		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
vterm_malloc(size_t size, ಶೂನ್ಯ *data UNUSED)
{
    // make sure that the length is not zero
    ಪ್ರತ್ಯಾಗಮ alloc_clear(size == 0 ? 1L : size);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vterm_memfree(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *data UNUSED)
{
    vim_free(ptr);
}

ಸ್ಥಿರ VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
create_vterm(term_T *term, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_ಮೌಲ್ಯ	    ಮೌಲ್ಯ;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, ರಿಕ್ತ);
    term->tl_vterm = vterm;
    ಯದಿ (vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    ಯದಿ (state == ರಿಕ್ತ || screen == ರಿಕ್ತ)
    {
	vterm_free(vterm);
	ಪ್ರತ್ಯಾಗಮ FAIL;
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

    ಯದಿ (t_colors < 16)
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
    ಯದಿ (GetCaretBlinkTime() == INFINITE)
	ಮೌಲ್ಯ.boolean = 0;
    ಅನ್ಯಥಾ
	ಮೌಲ್ಯ.boolean = 1;
#ಅನ್ಯಥಾ
    ಮೌಲ್ಯ.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &ಮೌಲ್ಯ);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    ಪೂರ್ಣಾಙ್ಕ		index;

    ಪ್ರತಿ (index = 0; index < 16; index++)
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

    ಸ್ಥಿರ ಶೂನ್ಯ
term_update_palette(term_T *term)
{
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಯದಿ (term_use_palette()
	    && (term->tl_palette != ರಿಕ್ತ
		|| find_var((char_u *)"g:terminal_ansi_colors", ರಿಕ್ತ, TRUE)
								      != ರಿಕ್ತ))
    {
	ಯದಿ (term->tl_palette != ರಿಕ್ತ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ಅನ್ಯಥಾ
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    ಅನ್ಯಥಾ
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    ಶೂನ್ಯ
term_update_palette_all(ಶೂನ್ಯ)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ಶೂನ್ಯ
term_update_colors_all(ಶೂನ್ಯ)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
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
    ಯದಿ (term->tl_status_text != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    ಯದಿ (term->tl_normal_mode)
    {
	ಯದಿ (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	ಅನ್ಯಥಾ
	    txt = (char_u *)_("Terminal-finished");
    }
    ಅನ್ಯಥಾ ಯದಿ (term->tl_title != ರಿಕ್ತ)
	txt = term->tl_title;
    ಅನ್ಯಥಾ ಯದಿ (term_none_open(term))
	txt = (char_u *)_("active");
    ಅನ್ಯಥಾ ಯದಿ (term_job_running(term))
	txt = (char_u *)_("running");
    ಅನ್ಯಥಾ
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    ಯದಿ (term->tl_status_text != ರಿಕ್ತ)
	vim_snprintf((ವರ್ಣ *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    ಪ್ರತ್ಯಾಗಮ term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    ಶೂನ್ಯ
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_term(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    ಪ್ರತಿ (term = first_term; !abort && term != ರಿಕ್ತ; term = term->tl_next)
	ಯದಿ (term->tl_job != ರಿಕ್ತ)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ಸ್ಥಿರ buf_T *
term_get_buf(typval_T *argvars, ವರ್ಣ *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    ಯದಿ (buf == ರಿಕ್ತ || buf->b_term == ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);    // issue errmsg if type error
	ch_log(ರಿಕ್ತ, "%s: invalid buffer argument", where);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dump_term_color(FILE *fd, VTermColor *color)
{
    ಪೂರ್ಣಾಙ್ಕ index;

    ಯದಿ (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    ಅನ್ಯಥಾ ಯದಿ (color->type == 0)
	// use RGB values
	index = 255;
    ಅನ್ಯಥಾ
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (ಪೂರ್ಣಾಙ್ಕ)color->red, (ಪೂರ್ಣಾಙ್ಕ)color->green, (ಪೂರ್ಣಾಙ್ಕ)color->blue,
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
    ಶೂನ್ಯ
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    ಪೂರ್ಣಾಙ್ಕ		max_height = 0;
    ಪೂರ್ಣಾಙ್ಕ		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
    {
	emsg(_(e_job_already_finished));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ಯದಿ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	d = argvars[2].vval.v_dict;
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    max_height = dict_get_ಸಂಖ್ಯೆ(d, "rows");
	    max_width = dict_get_ಸಂಖ್ಯೆ(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (*fname == NUL || (fd = mch_fopen((ವರ್ಣ *)fname, WRITEBIN)) == ರಿಕ್ತ)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	ಪ್ರತ್ಯಾಗಮ;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    ಪ್ರತಿ (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	ಪೂರ್ಣಾಙ್ಕ		repeat = 0;

	ಪ್ರತಿ (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    ಪೂರ್ಣಾಙ್ಕ		    same_attr;
	    ಪೂರ್ಣಾಙ್ಕ		    same_chars = TRUE;
	    ಪೂರ್ಣಾಙ್ಕ		    i;
	    ಪೂರ್ಣಾಙ್ಕ		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    ಯದಿ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    ಪ್ರತಿ (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		ಪೂರ್ಣಾಙ್ಕ c = cell.chars[i];
		ಪೂರ್ಣಾಙ್ಕ pc = prev_cell.chars[i];
		ಪೂರ್ಣಾಙ್ಕ should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		ಯದಿ (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		ಯದಿ (c != pc)
		    same_chars = FALSE;
		ಯದಿ (should_break)
		    ಭಙ್ಗ;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    ಯದಿ (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		ಯದಿ (cell.chars[0] == NUL)
		    fputs(" ", fd);
		ಅನ್ಯಥಾ
		{
		    char_u	charbuf[10];
		    ಪೂರ್ಣಾಙ್ಕ		len;

		    ಪ್ರತಿ (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		ಯದಿ (cell.width != prev_cell.width || !same_attr)
		{
		    ಯದಿ (cell.width == 2)
			fputs("*", fd);
		    ಅನ್ಯಥಾ
			fputs("+", fd);

		    ಯದಿ (same_attr)
		    {
			fputs("&", fd);
		    }
		    ಅನ್ಯಥಾ
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			ಯದಿ (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			ಅನ್ಯಥಾ
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			ಯದಿ (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			ಅನ್ಯಥಾ
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    ಯದಿ (cell.width == 2)
		++pos.col;
	}
	ಯದಿ (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
append_cell(garray_T *gap, cellattr_T *cell)
{
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    ಪೂರ್ಣಾಙ್ಕ		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {ರಿಕ್ತ, 0};
    ಪೂರ್ಣಾಙ್ಕ		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    ಪೂರ್ಣಾಙ್ಕ		    max_cells = 0;
    ಪೂರ್ಣಾಙ್ಕ		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, ಪರಿಮಾಣ(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (c == EOF)
	    ಭಙ್ಗ;
	ಯದಿ (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	ಅನ್ಯಥಾ ಯದಿ (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    ಯದಿ (ga_text.ga_data == ರಿಕ್ತ)
		dump_is_corrupt(&ga_text);
	    ಯದಿ (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_ಸಾಲು_T   *ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		ಯದಿ (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		ಸಾಲು->sb_cols = ga_cell.ga_len;
		ಸಾಲು->sb_bytes = ga_text.ga_len;
		ಸಾಲು->sb_cells = ga_cell.ga_data;
		ಸಾಲು->sb_fill_attr = term->tl_default_color;
		ಸಾಲು->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    ಅನ್ಯಥಾ
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	ಅನ್ಯಥಾ ಯದಿ (c == '|' || c == '>')
	{
	    ಪೂರ್ಣಾಙ್ಕ prev_len = ga_text.ga_len;

	    ಯದಿ (c == '>')
	    {
		ಯದಿ (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    ಯದಿ (c != EOF)
		ga_append(&ga_text, c);
	    ಪ್ರತಿ (;;)
	    {
		c = fgetc(fd);
		ಯದಿ (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    ಭಙ್ಗ;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    ಯದಿ (ga_text.ga_data != ರಿಕ್ತ)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		ಯದಿ (prev_char.string == ರಿಕ್ತ)
		    prev_char.length = 0;
	    }

	    ಯದಿ (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == '+' || c == '*')
	    {
		ಪೂರ್ಣಾಙ್ಕ is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		ಯದಿ (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		ಅನ್ಯಥಾ ಯದಿ (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    ಯಾವತ್ (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    ಪ್ರತಿ (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			ಯದಿ (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			ಅನ್ಯಥಾ ಯದಿ (c == '#')
			{
			    ಪೂರ್ಣಾಙ್ಕ red, green, blue, index = 0, type;

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
			    ಯದಿ (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    ಯಾವತ್ (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    ಯದಿ (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				ಯದಿ (index == 0)
				{
				    ಯದಿ (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    ಅನ್ಯಥಾ
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    ಅನ್ಯಥಾ
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    ಯದಿ (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    ಅನ್ಯಥಾ
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			ಅನ್ಯಥಾ
			    dump_is_corrupt(&ga_text);
		    }
		}
		ಅನ್ಯಥಾ
		    dump_is_corrupt(&ga_text);
	    }
	    ಅನ್ಯಥಾ
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    ಯದಿ (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	ಅನ್ಯಥಾ ಯದಿ (c == '@')
	{
	    ಯದಿ (prev_char.string == ರಿಕ್ತ)
		dump_is_corrupt(&ga_text);
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

		// repeat previous character, get the count
		ಪ್ರತಿ (;;)
		{
		    c = fgetc(fd);
		    ಯದಿ (!SAFE_isdigit(c))
			ಭಙ್ಗ;
		    ಎಣಿಕೆ = ಎಣಿಕೆ * 10 + (c - '0');
		}

		ಯಾವತ್ (ಎಣಿಕೆ-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    ಯದಿ (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    ಪ್ರತ್ಯಾಗಮ max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ಸ್ಥಿರ char_u *
get_separator(ಪೂರ್ಣಾಙ್ಕ text_width, char_u *fname)
{
    ಪೂರ್ಣಾಙ್ಕ	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    ಪೂರ್ಣಾಙ್ಕ	    fname_size;
    char_u  *p = fname;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    ಯದಿ (textline == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    fname_size = vim_strsize(fname);
    ಯದಿ (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    ಅನ್ಯಥಾ ಯದಿ (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    ಯಾವತ್ (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    ಪ್ರತಿ (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    ಪ್ರತಿ (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    ಪ್ರತ್ಯಾಗಮ textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_load_dump(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ರಿಕ್ತ;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ರಿಕ್ತ;
    char_u	*fname_tofree = ರಿಕ್ತ;
    FILE	*fd1;
    FILE	*fd2 = ರಿಕ್ತ;
    char_u	*textline = ರಿಕ್ತ;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    ಯದಿ (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    ಯದಿ (fname1 == ರಿಕ್ತ || (do_diff && fname2 == ರಿಕ್ತ))
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }
    fd1 = mch_fopen((ವರ್ಣ *)fname1, READBIN);
    ಯದಿ (fd1 == ರಿಕ್ತ)
    {
	semsg(_(e_cant_read_file_str), fname1);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (do_diff)
    {
	fd2 = mch_fopen((ವರ್ಣ *)fname2, READBIN);
	ಯದಿ (fd2 == ರಿಕ್ತ)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    init_job_options(&opt);
    ಯದಿ (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	ಗಚ್ಛ theend;

    ಯದಿ (opt.jo_term_name == ರಿಕ್ತ)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	ಯದಿ (fname_tofree != ರಿಕ್ತ)
	{
	    vim_snprintf((ವರ್ಣ *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    ಯದಿ (opt.jo_bufnr_buf != ರಿಕ್ತ)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	ಯದಿ (wp == ರಿಕ್ತ)
	    semsg(_(e_invalid_argument_str), "bufnr");
	ಅನ್ಯಥಾ
	{
	    buf = curbuf;
	    ಯಾವತ್ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    ಅನ್ಯಥಾ
	// Create a new terminal window.
	buf = term_start(&argvars[0], ರಿಕ್ತ, &opt, TERM_START_NOJOB);

    ಯದಿ (buf != ರಿಕ್ತ && buf->b_term != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	ಪೂರ್ಣಾಙ್ಕ		width;
	ಪೂರ್ಣಾಙ್ಕ		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	ಯದಿ (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	ಯದಿ (!do_diff)
	    ಗಚ್ಛ theend;

	term->tl_ತುದಿ_diff_rows = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

	textline = get_separator(width, fname1);
	ಯದಿ (textline == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	ಯದಿ (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	ಯದಿ (textline == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	ಯದಿ (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	width2 = read_dump_file(fd2, &cursor_pos2);
	ಯದಿ (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    ಯದಿ (textline == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - bot_lnum;

	ಪ್ರತಿ (lnum = 1; lnum <= term->tl_ತುದಿ_diff_rows; ++lnum)
	{
	    ಯದಿ (lnum + bot_lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		// bottom part has fewer rows, fill with "-"
		ಪ್ರತಿ (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    ಅನ್ಯಥಾ
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		ಪೂರ್ಣಾಙ್ಕ	col;
		sb_ಸಾಲು_T   *sb_ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_ಸಾಲು + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_ಸಾಲು + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		ಯದಿ (line1 == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		ಪ್ರತಿ (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    ಪೂರ್ಣಾಙ್ಕ len1 = utfc_ptr2len(p1);
		    ಪೂರ್ಣಾಙ್ಕ len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    ಯದಿ (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    ಅನ್ಯಥಾ ಯದಿ (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    ಅನ್ಯಥಾ ಯದಿ (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    ಅನ್ಯಥಾ ಯದಿ (cellattr1 != ರಿಕ್ತ && cellattr2 != ರಿಕ್ತ)
		    {
			ಯದಿ ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			ಅನ್ಯಥಾ ಯದಿ (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			ಅನ್ಯಥಾ ಯದಿ (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			ಅನ್ಯಥಾ ಯದಿ (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		ಯಾವತ್ (col < width)
		{
		    ಯದಿ (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    ಅನ್ಯಥಾ ಯದಿ (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    ಅನ್ಯಥಾ
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    ಯದಿ (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_ತುದಿ_diff_rows) == OK)
		ml_append(term->tl_ತುದಿ_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	ಯಾವತ್ (lnum + bot_lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // bottom part has more rows, fill with "+"
	    ಪ್ರತಿ (i = 0; i < width; ++i)
		textline[i] = '+';
	    ಯದಿ (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_ತುದಿ_diff_rows) == OK)
		ml_append(term->tl_ತುದಿ_diff_rows + lnum, textline, 0, FALSE);
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
    ಯದಿ (fd2 != ರಿಕ್ತ)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_swap_diff(ಶೂನ್ಯ)
{
    term_T	*term = curbuf->b_term;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    linenr_T	ತುದಿ_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_ಸಾಲು_T	*sb_ಸಾಲು;

    ಯದಿ (term == ರಿಕ್ತ
	    || !term_is_finished(curbuf)
	    || term->tl_ತುದಿ_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ತುದಿ_rows = term->tl_ತುದಿ_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = ಸಾಲು_ಎಣಿಕೆ - bot_rows;
    sb_ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    ಪ್ರತಿ (lnum = 1; lnum <= ತುದಿ_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    ಪ್ರತಿ (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;
    ml_append(ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_rows));
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;
    ml_delete(ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    ಯದಿ (ತುದಿ_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	ಪ್ರತಿ (lnum = 0; lnum < ತುದಿ_rows; ++lnum)
	{
	    sb_ಸಾಲು_T	temp;

	    temp = *(sb_ಸಾಲು + lnum);
	    *(sb_ಸಾಲು + lnum) = *(sb_ಸಾಲು + bot_start + lnum);
	    *(sb_ಸಾಲು + bot_start + lnum) = temp;
	}
    }
    ಅನ್ಯಥಾ
    {
	size_t		size = ಪರಿಮಾಣ(sb_ಸಾಲು_T) * term->tl_scrollback.ga_len;
	sb_ಸಾಲು_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	ಯದಿ (temp != ರಿಕ್ತ)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    ಪರಿಮಾಣ(sb_ಸಾಲು_T) * bot_rows);
	    mch_memmove((sb_ಸಾಲು_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + ತುದಿ_rows,
		    ಪರಿಮಾಣ(sb_ಸಾಲು_T) * (ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_rows - bot_rows));
	    mch_memmove((sb_ಸಾಲು_T *)term->tl_scrollback.ga_data
						       + ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_rows,
		    temp,
		    ಪರಿಮಾಣ(sb_ಸಾಲು_T) * ತುದಿ_rows);
	    vim_free(temp);
	}
    }

    term->tl_ತುದಿ_diff_rows = bot_rows;
    term->tl_bot_diff_rows = ತುದಿ_rows;

    update_screen(UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    ಶೂನ್ಯ
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    ಶೂನ್ಯ
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    ಶೂನ್ಯ
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    ಶೂನ್ಯ
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ	    attr;
    size_t  i;
    char_u  *name;

    ಸ್ಥಿರ ಸಂರಚನಾ {
	ವರ್ಣ	    *name;
	ಪೂರ್ಣಾಙ್ಕ	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    attr = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    ಪ್ರತಿ (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	ಯದಿ (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    ಭಙ್ಗ;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    ಶೂನ್ಯ
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getcursor()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_ಸಂಖ್ಯೆ(l, term->tl_cursor_pos.row + 1);
    list_append_ಸಂಖ್ಯೆ(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    dict_add_ಸಂಖ್ಯೆ(d, "visible", term->tl_cursor_visible);
    dict_add_ಸಂಖ್ಯೆ(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_ಸಂಖ್ಯೆ(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    ಯದಿ (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    ಶೂನ್ಯ
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getjob()");
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    ಯದಿ (rettv->vval.v_job != ರಿಕ್ತ)
	++rettv->vval.v_job->jv_refcount;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_row_ಸಂಖ್ಯೆ(typval_T *tv, term_T *term)
{
    ಯದಿ (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ರಿಕ್ತ
	    && STRCMP(tv->vval.v_string, ".") == 0)
	ಪ್ರತ್ಯಾಗಮ term->tl_cursor_pos.row;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    ಶೂನ್ಯ
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    ಪೂರ್ಣಾಙ್ಕ		    row;

    rettv->v_type = VAR_STRING;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getline()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    row = get_row_ಸಂಖ್ಯೆ(&argvars[1], term);

    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	ಪೂರ್ಣಾಙ್ಕ	  sb_row = term->tl_scrollback_scrolled + row;
	sb_ಸಾಲು_T *ಸಾಲು;

	ಯದಿ (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    ಪ್ರತ್ಯಾಗಮ;
	ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ರಿಕ್ತ, &offset);

	// vterm is finished, get the text from the buffer
	ಯದಿ (lnum > 0 && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    ಯದಿ (STRLEN(p) >= offset + ಸಾಲು->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, ಸಾಲು->sb_bytes);
	}
    }
    ಅನ್ಯಥಾ
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	ಪೂರ್ಣಾಙ್ಕ		len;
	char_u		*p;

	ಯದಿ (row < 0 || row >= term->tl_rows)
	    ಪ್ರತ್ಯಾಗಮ;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (ವರ್ಣ *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    ಶೂನ್ಯ
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getscrolled()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    ಶೂನ್ಯ
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getsize()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    l = rettv->vval.v_list;
    list_append_ಸಂಖ್ಯೆ(l, buf->b_term->tl_rows);
    list_append_ಸಂಖ್ಯೆ(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    ಶೂನ್ಯ
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_setsize()");
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	emsg(_(e_not_terminal_buffer));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (buf->b_term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    rows = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
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
    ಶೂನ್ಯ
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getstatus()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;

    ಯದಿ (term_job_running(term))
	STRCPY(val, "running");
    ಅನ್ಯಥಾ
	STRCPY(val, "finished");
    ಯದಿ (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    ಶೂನ್ಯ
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_gettitle()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (buf->b_term->tl_title != ರಿಕ್ತ)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    ಶೂನ್ಯ
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	ಅಂಕೆ = tv_get_bool(&argvars[1]);

    ವಿಕಲ್ಪ (ಅಂಕೆ)
    {
	ಪಕ್ಷ 0:
	    ಯದಿ (buf->b_term->tl_job != ರಿಕ್ತ)
		p = buf->b_term->tl_job->jv_tty_out;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 1:
	    ಯದಿ (buf->b_term->tl_job != ರಿಕ್ತ)
		p = buf->b_term->tl_job->jv_tty_in;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (p != ರಿಕ್ತ)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    ಶೂನ್ಯ
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL || first_term == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	ಯದಿ (tp->tl_buffer != ರಿಕ್ತ)
	    ಯದಿ (list_append_ಸಂಖ್ಯೆ(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
}

/*
 * "term_scrape(buf, row)" function
 */
    ಶೂನ್ಯ
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ರಿಕ್ತ;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_ಸಾಲು_T	    *ಸಾಲು;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_scrape()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_ಸಂಖ್ಯೆ(&argvars[1], term);

    ಯದಿ (term->tl_vterm != ರಿಕ್ತ)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	ಯದಿ (screen == ರಿಕ್ತ)  // can't really happen
	    ಪ್ರತ್ಯಾಗಮ;
	p = ರಿಕ್ತ;
	ಸಾಲು = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ರಿಕ್ತ, &offset);

	ಯದಿ (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಪ್ರತ್ಯಾಗಮ;

	ಸಾಲು = (sb_ಸಾಲು_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	ಯದಿ (STRLEN(p) < offset + ಸಾಲು->sb_bytes)
	    ಪ್ರತ್ಯಾಗಮ;

	p += offset;
    }

    ಪ್ರತಿ (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	ಪೂರ್ಣಾಙ್ಕ		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	ಪೂರ್ಣಾಙ್ಕ		i;

	ಯದಿ (screen == ರಿಕ್ತ)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    ಯದಿ (pos.col >= ಸಾಲು->sb_cols)
		ಭಙ್ಗ;
	    cellattr = ಸಾಲು->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	ಅನ್ಯಥಾ
	{
	    VTermScreenCell cell;

	    ಯದಿ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		ಭಙ್ಗ;

	    mbslen = 0;
	    ಪ್ರತಿ (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		ಯದಿ (cell.chars[i] == 0)
		    ಭಙ್ಗ;
		mbslen += (*utf_char2bytes)((ಪೂರ್ಣಾಙ್ಕ)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	ಯದಿ (dcell == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    ಭಙ್ಗ;
	}

	dict_add_string_len(dcell, "chars", mbs, (ಪೂರ್ಣಾಙ್ಕ)mbslen);

	rgblen = vim_snprintf_safelen((ವರ್ಣ *)rgb, ಪರಿಮಾಣ(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (ಪೂರ್ಣಾಙ್ಕ)rgblen);
	rgblen = vim_snprintf_safelen((ವರ್ಣ *)rgb, ಪರಿಮಾಣ(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (ಪೂರ್ಣಾಙ್ಕ)rgblen);

	dict_add_ಸಂಖ್ಯೆ(dcell, "attr",
				      cell2attr(term, ರಿಕ್ತ, &attrs, &fg, &bg));
	dict_add_ಸಂಖ್ಯೆ(dcell, "width", width);

	++pos.col;
	ಯದಿ (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    ಶೂನ್ಯ
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*ಸಂದೇಶ;
    term_T	*term;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_sendkeys()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಸಂದೇಶ = tv_get_string_chk(&argvars[1]);
    ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (*ಸಂದೇಶ != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ c;

	ಯದಿ (*ಸಂದೇಶ == K_SPECIAL && ಸಂದೇಶ[1] != NUL && ಸಂದೇಶ[2] != NUL)
	{
	    c = TO_SPECIAL(ಸಂದೇಶ[1], ಸಂದೇಶ[2]);
	    ಸಂದೇಶ += 3;
	}
	ಅನ್ಯಥಾ
	{
	    c = PTR2CHAR(ಸಂದೇಶ);
	    ಸಂದೇಶ += MB_CPTR2LEN(ಸಂದೇಶ);
	}
	send_ಕೀಲಿಗಳು_to_term(term, c, 0, FALSE);
    }
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    ಶೂನ್ಯ
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    ಪೂರ್ಣಾಙ್ಕ		index;
    list_T	*list;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_getansicolors()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    ಪ್ರತಿ (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((ವರ್ಣ *)hexbuf, ಪರಿಮಾಣ(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	ಯದಿ (list_append_string(list, hexbuf, (ಪೂರ್ಣಾಙ್ಕ)hexbuflen) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    ಶೂನ್ಯ
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		n = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_setansicolors()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    ಯದಿ (term->tl_vterm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "\"colors\"");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (term->tl_palette == ರಿಕ್ತ)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    ಯದಿ (term->tl_palette == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ಯದಿ (color_name == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	guicolor = GUI_GET_COLOR(color_name);
	ಯದಿ (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    ಶೂನ್ಯ
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_setapi()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    ಯದಿ (api != ರಿಕ್ತ)
	term->tl_api = vim_strsave(api);
    ಅನ್ಯಥಾ
	term->tl_api = ರಿಕ್ತ;
}

/*
 * "term_setrestore(buf, command)" function
 */
    ಶೂನ್ಯ
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ಯದಿ defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_setrestore()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    ಯದಿ (cmd != ರಿಕ್ತ)
	term->tl_command = vim_strsave(cmd);
    ಅನ್ಯಥಾ
	term->tl_command = ರಿಕ್ತ;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    ಶೂನ್ಯ
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_setkill()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    ಯದಿ (how != ರಿಕ್ತ)
	term->tl_kill = vim_strsave(how);
    ಅನ್ಯಥಾ
	term->tl_kill = ರಿಕ್ತ;
}

/*
 * "term_start(command, options)" function
 */
    ಶೂನ್ಯ
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    init_job_options(&opt);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_start(&argvars[0], ರಿಕ್ತ, &opt, 0);

    ಯದಿ (buf != ರಿಕ್ತ && buf->b_term != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    ಶೂನ್ಯ
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = term_get_buf(argvars, "term_wait()");
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (buf->b_term->tl_job == ರಿಕ್ತ)
    {
	ch_log(ರಿಕ್ತ, "term_wait(): no job to wait for");
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (buf->b_term->tl_job->jv_channel == ರಿಕ್ತ)
	// channel is closed, nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    // Get the job status, this will detect a job that finished.
    ಯದಿ (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ರಿಕ್ತ, "term_wait(): waiting for channel to close");
	ಯಾವತ್ (buf->b_term != ರಿಕ್ತ && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    ಯದಿ (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		ಭಙ್ಗ;
	    ಯದಿ (buf->b_term == ರಿಕ್ತ || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		ಭಙ್ಗ;
	}

	term_flush_messages();
    }
    ಅನ್ಯಥಾ
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
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
    ಶೂನ್ಯ
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	ಯದಿ (term->tl_job == ch->ch_job)
	{
	    ಯದಿ (term->tl_eof_chars != ರಿಕ್ತ)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(ಪೂರ್ಣಾಙ್ಕ)STRLEN(term->tl_eof_chars), ರಿಕ್ತ);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, ರಿಕ್ತ);
	    }
#ifdef MSWIN
	    ಅನ್ಯಥಾ
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, ರಿಕ್ತ);
#endif
	}
}

#ಯದಿ defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    ಪ್ರತ್ಯಾಗಮ term != ರಿಕ್ತ ? term->tl_job : ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
ಶೂನ್ಯ (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dyn_conpty_init(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಸ್ಥಿರ HMODULE	hKerneldll = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			i;
    ಸ್ಥಿರ ಸಂರಚನಾ
    {
	ವರ್ಣ	*name;
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
	{ರಿಕ್ತ, ರಿಕ್ತ}
    };

    ಯದಿ (!has_conpty_working())
    {
	ಯದಿ (verbose)
	    emsg(_(e_conpty_is_not_available));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // No need to initialize twice.
    ಯದಿ (hKerneldll)
	ಪ್ರತ್ಯಾಗಮ OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    ಪ್ರತಿ (i = 0; conpty_entry[i].name != ರಿಕ್ತ
					&& conpty_entry[i].ptr != ರಿಕ್ತ; ++i)
    {
	ಯದಿ ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == ರಿಕ್ತ)
	{
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ವರ್ಣ	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ರಿಕ್ತ;
    WCHAR	    *cmd_wchar_copy = ರಿಕ್ತ;
    WCHAR	    *cwd_wchar = ರಿಕ್ತ;
    WCHAR	    *env_wchar = ರಿಕ್ತ;
    channel_T	    *channel = ರಿಕ್ತ;
    job_T	    *job = ರಿಕ್ತ;
    HANDLE	    jo = ರಿಕ್ತ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ರಿಕ್ತ;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ರಿಕ್ತ;
    HANDLE	    o_theirs = ರಿಕ್ತ;
    HANDLE	    i_ours = ರಿಕ್ತ;
    HANDLE	    o_ours = ರಿಕ್ತ;
    ವರ್ಣ	    *errmsg = ರಿಕ್ತ;

    ga_init2(&ga_cmd, ಪರಿಮಾಣ(ವರ್ಣ*), 20);
    ga_init2(&ga_env, ಪರಿಮಾಣ(ವರ್ಣ*), 20);

    ಯದಿ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvar->v_type == VAR_LIST)
    {
	ಯದಿ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ಗಚ್ಛ failed;
	cmd = ga_cmd.ga_data;
    }
    ಯದಿ (cmd == ರಿಕ್ತ || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	ಗಚ್ಛ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ರಿಕ್ತ);

    ಯದಿ (cmd_wchar != ರಿಕ್ತ)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    ಯದಿ (cmd_wchar == ರಿಕ್ತ)
	ಗಚ್ಛ failed;
    ಯದಿ (opt->jo_cwd != ರಿಕ್ತ)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ರಿಕ್ತ);
	ಯದಿ (cwd_wchar == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    ಯದಿ (!CreatePipe(&i_theirs, &i_ours, ರಿಕ್ತ, 0))
	ಗಚ್ಛ failed;
    ಯದಿ (!CreatePipe(&o_ours, &o_theirs, ರಿಕ್ತ, 0))
	ಗಚ್ಛ failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    ಯದಿ (FAILED(hr))
	ಗಚ್ಛ failed;

    term->tl_siex.StartupInfo.cb = ಪರಿಮಾಣ(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_ಒಳಹರಿವು = INVALID_HANDLE_ಮೌಲ್ಯ;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_ಮೌಲ್ಯ;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_ಮೌಲ್ಯ;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ರಿಕ್ತ, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    ಯದಿ (!term->tl_siex.lpAttributeList)
	ಗಚ್ಛ failed;
    ಯದಿ (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	ಗಚ್ಛ failed;
    ಯದಿ (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    ಪರಿಮಾಣ(HPCON), ರಿಕ್ತ, ರಿಕ್ತ))
	ಗಚ್ಛ failed;

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    job = job_alloc();
    ಯದಿ (job == ರಿಕ್ತ)
	ಗಚ್ಛ failed;
    ಯದಿ (argvar->v_type == VAR_STRING)
    {
	ಪೂರ್ಣಾಙ್ಕ argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    ಯದಿ (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    ಯದಿ (!CreateProcessW(ರಿಕ್ತ, cmd_wchar_copy, ರಿಕ್ತ, ರಿಕ್ತ, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	ಗಚ್ಛ failed;
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

    jo = CreateJobObject(ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (jo == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    ಯದಿ (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ರಿಕ್ತ;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ಯದಿ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ಗಚ್ಛ failed;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಯದಿ (term_use_palette())
    {
	ಯದಿ (term->tl_palette != ರಿಕ್ತ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ಅನ್ಯಥಾ
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
    ಯದಿ (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((ವರ್ಣ *)fname, WRITEBIN);
	ಯದಿ (term->tl_out_fd == ರಿಕ್ತ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ಪ್ರತ್ಯಾಗಮ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    ಯದಿ (channel != ರಿಕ್ತ)
	channel_clear(channel);
    ಯದಿ (job != ರಿಕ್ತ)
    {
	job->jv_channel = ರಿಕ್ತ;
	job_cleanup(job);
    }
    term->tl_job = ರಿಕ್ತ;
    ಯದಿ (jo != ರಿಕ್ತ)
	CloseHandle(jo);

    ಯದಿ (term->tl_siex.lpAttributeList != ರಿಕ್ತ)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ರಿಕ್ತ;
    ಯದಿ (o_theirs != ರಿಕ್ತ)
	CloseHandle(o_theirs);
    ಯದಿ (o_ours != ರಿಕ್ತ)
	CloseHandle(o_ours);
    ಯದಿ (i_ours != ರಿಕ್ತ)
	CloseHandle(i_ours);
    ಯದಿ (i_theirs != ರಿಕ್ತ)
	CloseHandle(i_theirs);
    ಯದಿ (term->tl_conpty != ರಿಕ್ತ)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ರಿಕ್ತ;
    // Propagate errors that occur in CreateProcess
    ಯದಿ (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
conpty_term_report_winsize(term_T *term, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
term_free_conpty(term_T *term)
{
    ಯದಿ (term->tl_siex.lpAttributeList != ರಿಕ್ತ)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ರಿಕ್ತ;
    ಯದಿ (term->tl_conpty != ರಿಕ್ತ)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
use_conpty(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

ಶೂನ್ಯ* (*winpty_config_new)(UINT64, ಶೂನ್ಯ*);
ಶೂನ್ಯ* (*winpty_open)(ಶೂನ್ಯ*, ಶೂನ್ಯ*);
ಶೂನ್ಯ* (*winpty_spawn_config_new)(UINT64, ಶೂನ್ಯ*, LPCWSTR, ಶೂನ್ಯ*, ಶೂನ್ಯ*, ಶೂನ್ಯ*);
BOOL (*winpty_spawn)(ಶೂನ್ಯ*, ಶೂನ್ಯ*, HANDLE*, HANDLE*, DWORD*, ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_config_set_mouse_mode)(ಶೂನ್ಯ*, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ (*winpty_config_set_initial_size)(ಶೂನ್ಯ*, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
LPCWSTR (*winpty_conin_name)(ಶೂನ್ಯ*);
LPCWSTR (*winpty_conout_name)(ಶೂನ್ಯ*);
LPCWSTR (*winpty_conerr_name)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_free)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_config_free)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_spawn_config_free)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_error_free)(ಶೂನ್ಯ*);
LPCWSTR (*winpty_error_ಸಂದೇಶ)(ಶೂನ್ಯ*);
BOOL (*winpty_set_size)(ಶೂನ್ಯ*, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ*);
HANDLE (*winpty_agent_process)(ಶೂನ್ಯ*);

# define WINPTY_DLL "winpty.dll"

ಸ್ಥಿರ HINSTANCE hWinPtyDLL = ರಿಕ್ತ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dyn_winpty_init(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಸ್ಥಿರ ಸಂರಚನಾ
    {
	ವರ್ಣ	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_ಸಂದೇಶ},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ರಿಕ್ತ, ರಿಕ್ತ}
    };

    // No need to initialize twice.
    ಯದಿ (hWinPtyDLL)
	ಪ್ರತ್ಯಾಗಮ OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    ಯದಿ (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((ವರ್ಣ *)p_winptydll);
    ಯದಿ (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    ಯದಿ (!hWinPtyDLL)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (i = 0; winpty_entry[i].name != ರಿಕ್ತ
					 && winpty_entry[i].ptr != ರಿಕ್ತ; ++i)
    {
	ಯದಿ ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == ರಿಕ್ತ)
	{
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ವರ್ಣ	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ರಿಕ್ತ;
    WCHAR	    *cwd_wchar = ರಿಕ್ತ;
    WCHAR	    *env_wchar = ರಿಕ್ತ;
    channel_T	    *channel = ರಿಕ್ತ;
    job_T	    *job = ರಿಕ್ತ;
    DWORD	    error;
    HANDLE	    jo = ರಿಕ್ತ;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    ಶೂನ್ಯ	    *winpty_err = ರಿಕ್ತ;
    ಶೂನ್ಯ	    *spawn_config = ರಿಕ್ತ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ರಿಕ್ತ;

    ga_init2(&ga_cmd, ಪರಿಮಾಣ(ವರ್ಣ*), 20);
    ga_init2(&ga_env, ಪರಿಮಾಣ(ವರ್ಣ*), 20);

    ಯದಿ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvar->v_type == VAR_LIST)
    {
	ಯದಿ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ಗಚ್ಛ failed;
	cmd = ga_cmd.ga_data;
    }
    ಯದಿ (cmd == ರಿಕ್ತ || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	ಗಚ್ಛ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ರಿಕ್ತ);
    ga_clear(&ga_cmd);
    ಯದಿ (cmd_wchar == ರಿಕ್ತ)
	ಗಚ್ಛ failed;
    ಯದಿ (opt->jo_cwd != ರಿಕ್ತ)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ರಿಕ್ತ);
	ಯದಿ (cwd_wchar == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    ಯದಿ (term->tl_winpty_config == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    ಯದಿ (term->tl_winpty == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ರಿಕ್ತ,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    ಯದಿ (spawn_config == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    job = job_alloc();
    ಯದಿ (job == ರಿಕ್ತ)
	ಗಚ್ಛ failed;
    ಯದಿ (argvar->v_type == VAR_STRING)
    {
	ಪೂರ್ಣಾಙ್ಕ argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    ಯದಿ (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    ಯದಿ (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	ಗಚ್ಛ failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, ರಿಕ್ತ,
	    OPEN_EXISTING, 0, ರಿಕ್ತ),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, ರಿಕ್ತ,
	    OPEN_EXISTING, 0, ರಿಕ್ತ),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, ರಿಕ್ತ,
	    OPEN_EXISTING, 0, ರಿಕ್ತ));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (jo == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    ಯದಿ (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ರಿಕ್ತ;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ಯದಿ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ಗಚ್ಛ failed;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಯದಿ (term_use_palette())
    {
	ಯದಿ (term->tl_palette != ರಿಕ್ತ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ಅನ್ಯಥಾ
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
	    (short_u *)winpty_conin_name(term->tl_winpty), ರಿಕ್ತ);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), ರಿಕ್ತ);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ಯದಿ (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((ವರ್ಣ *)fname, WRITEBIN);
	ಯದಿ (term->tl_out_fd == ರಿಕ್ತ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ಪ್ರತ್ಯಾಗಮ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    ಯದಿ (spawn_config != ರಿಕ್ತ)
	winpty_spawn_config_free(spawn_config);
    ಯದಿ (channel != ರಿಕ್ತ)
	channel_clear(channel);
    ಯದಿ (job != ರಿಕ್ತ)
    {
	job->jv_channel = ರಿಕ್ತ;
	job_cleanup(job);
    }
    term->tl_job = ರಿಕ್ತ;
    ಯದಿ (jo != ರಿಕ್ತ)
	CloseHandle(jo);
    ಯದಿ (term->tl_winpty != ರಿಕ್ತ)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ರಿಕ್ತ;
    ಯದಿ (term->tl_winpty_config != ರಿಕ್ತ)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ರಿಕ್ತ;
    ಯದಿ (winpty_err != ರಿಕ್ತ)
    {
	ವರ್ಣ *ಸಂದೇಶ = (ವರ್ಣ *)utf16_to_enc(
				(short_u *)winpty_error_ಸಂದೇಶ(winpty_err), ರಿಕ್ತ);

	ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
	{
	    emsg(ಸಂದೇಶ);
	    vim_free(ಸಂದೇಶ);
	}
	winpty_error_free(winpty_err);
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ವರ್ಣ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    ಪೂರ್ಣಾಙ್ಕ		    use_winpty = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    use_conpty = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    ಯದಿ (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	ಪ್ರತ್ಯಾಗಮ dyn_winpty_init(TRUE);

    ಯದಿ (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    ಯದಿ (tty_type == NUL)
    {
	ಯದಿ (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    ಅನ್ಯಥಾ ಯದಿ (tty_type == 'w')	// winpty
    {
	ಯದಿ (has_winpty)
	    use_winpty = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (tty_type == 'c')	// conpty
    {
	ಯದಿ (has_conpty)
	    use_conpty = TRUE;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ dyn_conpty_init(TRUE);
    }

    ಯದಿ (use_conpty)
	ಪ್ರತ್ಯಾಗಮ conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    ಯದಿ (use_winpty)
	ಪ್ರತ್ಯಾಗಮ winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    ಪ್ರತ್ಯಾಗಮ dyn_winpty_init(TRUE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_ಮೌಲ್ಯ;
    HANDLE	    hPipeOut = INVALID_HANDLE_ಮೌಲ್ಯ;
    ವರ್ಣ	    in_name[80], out_name[80];
    channel_T	    *channel = ರಿಕ್ತ;

    ಯದಿ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    vim_snprintf(in_name, ಪರಿಮಾಣ(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ರಿಕ್ತ);
    ಯದಿ (hPipeIn == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಗಚ್ಛ failed;

    vim_snprintf(out_name, ಪರಿಮಾಣ(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ರಿಕ್ತ);
    ಯದಿ (hPipeOut == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಗಚ್ಛ failed;

    ConnectNamedPipe(hPipeIn, ರಿಕ್ತ);
    ConnectNamedPipe(hPipeOut, ರಿಕ್ತ);

    term->tl_job = job_alloc();
    ಯದಿ (term->tl_job == ರಿಕ್ತ)
	ಗಚ್ಛ failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
	ಗಚ್ಛ failed;
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

    ಪ್ರತ್ಯಾಗಮ OK;

failed:
    ಯದಿ (hPipeIn != ರಿಕ್ತ)
	CloseHandle(hPipeIn);
    ಯದಿ (hPipeOut != ರಿಕ್ತ)
	CloseHandle(hPipeOut);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    ಯದಿ (term->tl_winpty != ರಿಕ್ತ)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ರಿಕ್ತ;
    ಯದಿ (term->tl_winpty_config != ರಿಕ್ತ)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ರಿಕ್ತ;
    ಯದಿ (term->tl_vterm != ರಿಕ್ತ)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ರಿಕ್ತ;
}

/*
 * Report the size to the terminal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_report_winsize(term_T *term, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
    ಯದಿ (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    ಯದಿ (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
terminal_enabled(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#ಅನ್ಯಥಾ

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ವರ್ಣ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = ರಿಕ್ತ;

    ಯದಿ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಯದಿ (term_use_palette())
    {
	ಯದಿ (term->tl_palette != ರಿಕ್ತ)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ಅನ್ಯಥಾ
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    ಯದಿ (term->tl_job != ರಿಕ್ತ)
	++term->tl_job->jv_refcount;

    ಪ್ರತ್ಯಾಗಮ term->tl_job != ರಿಕ್ತ
	&& term->tl_job->jv_channel != ರಿಕ್ತ
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
create_pty_only(term_T *term, jobopt_T *opt)
{
    ಯದಿ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    term->tl_job = job_alloc();
    ಯದಿ (term->tl_job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    ಪ್ರತ್ಯಾಗಮ mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_free_vterm(term_T *term)
{
    ಯದಿ (term->tl_vterm != ರಿಕ್ತ)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ರಿಕ್ತ;
}

/*
 * Report the size to the terminal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
term_report_winsize(term_T *term, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
    // Use an ioctl() to report the new window size to the job.
    ಯದಿ (term->tl_job == ರಿಕ್ತ || term->tl_job->jv_channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ fd = -1;
    ಪೂರ್ಣಾಙ್ಕ part;

    ಪ್ರತಿ (part = PART_OUT; part < PART_ಎಣಿಕೆ; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	ಯದಿ (mch_isatty(fd))
	    ಭಙ್ಗ;
    }
    ಯದಿ (part < PART_ಎಣಿಕೆ && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
