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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ଅନ୍ତର୍ଭାଵ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    VTermScreenCellAttrs	attrs;
    ଅକ୍ଷର			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sb_ଧାଡ଼ି_S {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sb_cols;	// can differ per line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_ଧାଡ଼ି_T;

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
ସଂରଚନା terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#ଯଦି defined(FEAT_GUI)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_system;	// when non-zero used for :!cmd output
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_vterm_size_changed;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_normal_mode; // TRUE: Terminal-Normal mode
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_channel_closing;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_channel_closed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_channel_recently_closed; // still need to handle tl_finish

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    ଶୂନ୍ୟ	*tl_winpty_config;
    ଶୂନ୍ୟ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#ଯଦି defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_rows;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_dirty_row_start; // MAX_ROW if nothing dirty
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_dirty_row_end;   // row below last one to update
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_scrollback_snapshot;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_ଶୀର୍ଷ_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_cursor_visible;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_cursor_blink;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ସ୍ଥିର term_T *first_term = ଖାଲି;

// Terminal active in terminal_loop().
ସ୍ଥିର term_T *in_terminal_loop = ଖାଲି;

#ifdef MSWIN
ସ୍ଥିର BOOL has_winpty = FALSE;
ସ୍ଥିର BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define ଚାବି_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    ପାଇଁ ((term) = first_term; (term) != ଖାଲି; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା term_and_job_init(term_T *term, typval_T *argvar, ଅକ୍ଷର **argv, jobopt_T *opt, jobopt_T *orig_opt);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା create_pty_only(term_T *term, jobopt_T *opt);
ସ୍ଥିର ଶୂନ୍ୟ term_report_winsize(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols);
ସ୍ଥିର ଶୂନ୍ୟ term_free_vterm(term_T *term);
#ifdef FEAT_GUI
ସ୍ଥିର ଶୂନ୍ୟ update_system_term(term_T *term);
#endif

ସ୍ଥିର ଶୂନ୍ୟ handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ସ୍ଥିର char_u	*last_set_cursor_color = ଖାଲି;
ସ୍ଥିର char_u	*desired_cursor_color = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_set_cursor_shape = -1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	desired_cursor_shape = -1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_set_cursor_blink = -1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    ଯଦି (lhs_color != ଖାଲି && rhs_color != ଖାଲି)
	ଫେରନ୍ତୁ STRCMP(lhs_color, rhs_color) == 0;
    ଫେରନ୍ତୁ lhs_color == ଖାଲି && rhs_color == ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    ଯଦି (cursor_color_equal(*to_color, from_color))
	ଫେରନ୍ତୁ;
    vim_free(*to_color);
    *to_color = (from_color == ଖାଲି) ? ଖାଲି : vim_strsave(from_color);
}

    ସ୍ଥିର char_u *
cursor_color_get(char_u *color)
{
    ଫେରନ୍ତୁ (color == ଖାଲି) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_termwinsize(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cols)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    ଯଦି (*wp->w_p_tws == NUL)
	ଫେରନ୍ତୁ FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    ଯଦି (p == ଖାଲି)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((ଅକ୍ଷର *)wp->w_p_tws);
    *cols = atoi((ଅକ୍ଷର *)p + 1);
    ଯଦି (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    ଯଦି (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    ଫେରନ୍ତୁ minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, cols;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	minsize;

#ifdef FEAT_GUI
    ଯଦି (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	ଫେରନ୍ତୁ;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    ଯଦି (minsize)
    {
	ଯଦି (term->tl_rows < rows)
	    term->tl_rows = rows;
	ଯଦି (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    ଯଦି ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    ଅନ୍ୟଥା ଯଦି (rows != 0)
	term->tl_rows = rows;
    ଯଦି ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    ଅନ୍ୟଥା ଯଦି (cols != 0)
	term->tl_cols = cols;

    ଯଦି (!opt->jo_hidden)
    {
	ଯଦି (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	ଯଦି (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	ଯଦି (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((ଅକ୍ଷର *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_ମୂଲ୍ୟ_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
setup_job_options(jobopt_T *opt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    ଯଦି (!(opt->jo_set & JO_OUT_IO))
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
    ଯଦି (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    ଯଦି ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    ଯଦି ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_flush_messages(ଶୂନ୍ୟ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    ଯଦି (old_curbuf != ଖାଲି)
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
	ଅକ୍ଷର	    **argv,
	jobopt_T    *opt,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    buf_T	*newbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_ଧାଡ଼ି_window));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != ଖାଲି
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ଖାଲି
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    ଯଦି (term == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, ଆକାର(sb_ଧାଡ଼ି_T), 300);
    ga_init2(&term->tl_scrollback_postponed, ଆକାର(sb_ଧାଡ଼ି_T), 300);
    ga_init2(&term->tl_osc_buf, ଆକାର(ଅକ୍ଷର), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    ଯଦି (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	ଯଦି (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (do_ecmd(0, ଖାଲି, ଖାଲି, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା ଯଦି (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ଖାଲି, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	ଯଦି (buf == ଖାଲି || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    ଅନ୍ୟଥା
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	ଯଦି (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_ଗଣନା = 1;
	}
	ଯଦି (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_ଗଣନା = 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmod_split_modified = FALSE;
	ଯଦି (vertical)
	{
	    ଯଦି (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	ଯଦି (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	ଯଦି (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    ଯଦି (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	ଯଦି (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	ଯଦି (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, ଖାଲି, ଖାଲି, FALSE, curbuf);

    ଯଦି (opt->jo_term_name != ଖାଲି)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    ଅନ୍ୟଥା ଯଦି (argv != ଖାଲି)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	size_t	len;
	char_u	*cmd, *p;

	ଯଦି (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    ଯଦି (cmd == ଖାଲି)
		cmd = (char_u *)"";
	    ଅନ୍ୟଥା ଯଦି (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	ଅନ୍ୟଥା ଯଦି (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ଖାଲି
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == ଖାଲି)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	ପାଇଁ (i = 0; p != ଖାଲି; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    ଯଦି (i == 0)
		vim_snprintf((ଅକ୍ଷର *)p, len, "!%s", cmd);
	    ଅନ୍ୟଥା
		vim_snprintf((ଅକ୍ଷର *)p, len, "!%s (%d)", cmd, i);
	    ଯଦି (buflist_findname(p) == ଖାଲି)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		ଭାଙ୍ଗ;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);

    ଯଦି (opt->jo_term_opencmd != ଖାଲି)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    ଯଦି (opt->jo_eof_chars != ଖାଲି)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, ଆକାର(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    ଯଦି (flags & TERM_START_NOJOB)
	ଫେରନ୍ତୁ curbuf;

#ଯଦି defined(FEAT_SESSION)
    // Remember the command for the session file.
    ଯଦି (opt->jo_term_norestore || argv != ଖାଲି)
	term->tl_command = vim_strsave((char_u *)"NONE");
    ଅନ୍ୟଥା ଯଦି (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	ଯଦି (cmd != ଖାଲି && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    ଅନ୍ୟଥା ଯଦି (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ଖାଲି
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    ଯଦି (s == ଖାଲି)
		ଭାଙ୍ଗ;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    ଯଦି (p == ଖାଲି)
		ଭାଙ୍ଗ;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	ଯଦି (item == ଖାଲି)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	ଅନ୍ୟଥା
	    ga_clear(&ga);
    }
#endif

    ଯଦି (opt->jo_term_kill != ଖାଲି)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    ଯଦି (opt->jo_term_api != ଖାଲି)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    ଅନ୍ୟଥା
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    ଯଦି (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    ଯଦି (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	ଯଦି (term->tl_palette == ଖାଲି)
	{
	    vim_free(term);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, ଆକାର(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    ଯଦି (argv == ଖାଲି
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ଖାଲି
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    ଅନ୍ୟଥା
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    ଯଦି (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	ଯଦି (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = ସନ୍ଦେଶ_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	ଯଦି (old_curbuf != ଖାଲି)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	ଅନ୍ୟଥା ଯଦି (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| ନିବେଶ_busy)
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
    ଅନ୍ୟଥା
    {
	term_close_buffer(curbuf, old_curbuf);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ଖାଲି, ଖାଲି, FALSE, newbuf);
    ଯଦି (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ଖାଲି, ଖାଲି, FALSE, newbuf);
    ଫେରନ୍ତୁ newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    ଶୂନ୍ୟ
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scroll_save = ସନ୍ଦେଶ_scroll;

    ସନ୍ଦେଶ_scroll = FALSE;		// don't scroll here
    autowrite_all();
    ସନ୍ଦେଶ_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    ଯେତେବେଳେ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	ଯଦି (ep != ଖାଲି)
	{
	    ଯଦି (ep < p)
		p = ep;
	    ଅନ୍ୟଥା
		ep = ଖାଲି;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - cmd) == ଆକାର(name) - 1 \
				 && STRNICMP(cmd, name, ଆକାର(name) - 1) == 0)
	ଯଦି (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("kill") && ep != ଖାଲି)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    ଯଦି (ep != ଖାଲି)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    ଅନ୍ୟଥା
		opt.jo_term_api = ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("rows") && ep != ଖାଲି && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((ଅକ୍ଷର *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("cols") && ep != ଖାଲି && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((ଅକ୍ଷର *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ଅନ୍ୟଥା ଯଦି (OPTARG_HAS("eof") && ep != ଖାଲି)
	{
	    char_u *buf = ଖାଲି;
	    char_u *ଚାବିଗୁଡ଼ିକ;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    ଚାବିଗୁଡ଼ିକ = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ଖାଲି);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(ଚାବିଗୁଡ଼ିକ);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	ଅନ୍ୟଥା ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tty_type = NUL;

	    p = skiptowhite(cmd);
	    ଯଦି (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "type");
		ଯାଅ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	ଅନ୍ୟଥା
	{
	    ଯଦି (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    ଯାଅ theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    ଯଦି (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	ଯଦି (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    ଯଦି (eap->addr_ଗଣନା > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_ଶୀର୍ଷ | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_ଶୀର୍ଷ = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    ଯଦି (opt_shell && tofree == ଖାଲି)
    {
#ifdef UNIX
	ଅକ୍ଷର	**argv = ଖାଲି;
	char_u	*tofree1 = ଖାଲି;
	char_u	*tofree2 = ଖାଲି;

	// :term ++shell command
	ଯଦି (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(ଖାଲି, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	ଯାଅ theend;
#ଅନ୍ୟଥା
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	ଯଦି (newcmd == ଖାଲି)
	    ଯାଅ theend;
	tofree = newcmd;
	vim_snprintf((ଅକ୍ଷର *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# ଅନ୍ୟଥା
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	ଯାଅ theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, ଖାଲି, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ସ୍ଥିର char_u *
get_terminalopt_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // Note: Keep this in sync with ex_terminal.
    ସ୍ଥିର ଅକ୍ଷର *(p_termopt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) =
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

    ଯଦି (idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_termopt_ମୂଲ୍ୟଗୁଡ଼ିକ))
	ଫେରନ୍ତୁ (char_u*)p_termopt_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର char_u *
get_termkill_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ସ୍ଥିର ଅକ୍ଷର *(p_termkill_ମୂଲ୍ୟଗୁଡ଼ିକ[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    ଯଦି (idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_termkill_ମୂଲ୍ୟଗୁଡ଼ିକ))
	ଫେରନ୍ତୁ (char_u*)p_termkill_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Command-line expansion for :terminal [options]
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *ଅଙ୍କ_Matches)
{
    ଯଦି (xp->xp_pattern > xp->xp_ଧାଡ଼ି && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) = ଖାଲି;

	char_u *name_end = xp->xp_pattern - 1;
	ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	ଯଦି (cb != ଖାଲି)
	{
	    ଫେରନ୍ତୁ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ଅଙ୍କ_Matches,
		    cb,
		    FALSE);
	}
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ଅଙ୍କ_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#ଯଦି defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    ଯଦି (terminal_bufs != ଖାଲି && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	ଅକ୍ଷର	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, ଆକାର(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	ଯଦି (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    ଯଦି (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		ଫେରନ୍ତୁ FAIL;
	    ଫେରନ୍ତୁ put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    ଯଦି (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	ଫେରନ୍ତୁ FAIL;
# ifdef MSWIN
    ଯଦି (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	ଫେରନ୍ତୁ FAIL;
# endif
    ଯଦି (term->tl_command != ଖାଲି)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	ଯଦି (quoted_command == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((ଅକ୍ଷର *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	ଯଦି (ret)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (put_eol(fd) != OK)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (terminal_bufs != ଖାଲି && wp->w_buffer->b_nwindows > 1)
    {
	ଅକ୍ଷର *hash_ଚାବି = alloc(NUMBUFLEN);

	vim_snprintf(hash_ଚାବି, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_ଚାବି, "terminal session");
    }

    ଫେରନ୍ତୁ put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    ଫେରନ୍ତୁ term != ଖାଲି && (term->tl_command == ଖାଲି
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_scrollback(term_T *term)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    ପାଇଁ (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_ଧାଡ଼ି_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ସ୍ଥିର term_T	*terminals_to_free = ଖାଲି;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    ଶୂନ୍ୟ
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    ଯଦି (term == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Unlink the terminal form the list of terminals.
    ଯଦି (first_term == term)
	first_term = term->tl_next;
    ଅନ୍ୟଥା
	ପାଇଁ (tp = first_term; tp->tl_next != ଖାଲି; tp = tp->tl_next)
	    ଯଦି (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		ଭାଙ୍ଗ;
	    }

    ଯଦି (term->tl_job != ଖାଲି)
    {
	ଯଦି (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, ଖାଲି, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = ଖାଲି;
    ଯଦି (in_terminal_loop == term)
	in_terminal_loop = ଖାଲି;
}

    ଶୂନ୍ୟ
free_unused_terminals(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (terminals_to_free != ଖାଲି)
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
	ଯଦି (term->tl_out_fd != ଖାଲି)
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
    ସ୍ଥିର ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < 3; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	ଯଦି (mch_isatty(fd))
	    ଫେରନ୍ତୁ parts[i];
    }
#endif
    ଫେରନ୍ତୁ PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    ଅକ୍ଷର   buf[ଚାବି_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, ଚାବି_BUF_LEN);

    ଯଦି (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlen, ଖାଲି);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_write_job_output(term_T *term, char_u *ସନ୍ଦେଶ_arg, size_t len_arg)
{
    char_u	*ସନ୍ଦେଶ = ସନ୍ଦେଶ_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    ଯଦି (len > limit)
    {
	char_u *p = ସନ୍ଦେଶ + len - limit;

	p -= (*mb_head_off)(ସନ୍ଦେଶ, p);
	len -= p - ସନ୍ଦେଶ;
	ସନ୍ଦେଶ = p;
    }

    vterm_ନିବେଶ_write(vterm, (ଅକ୍ଷର *)ସନ୍ଦେଶ, len);

    // flush vterm buffer when vterm responded to control sequence
    ଯଦି (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ସ୍ଥିର ଶୂନ୍ୟ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    ଯଦି (popup_is_popup(wp))
    {
	wp->w_wrow += popup_ଶୀର୍ଷ_extra(wp);
	wp->w_wcol += popup_ବାମ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    ଅନ୍ୟଥା
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ସ୍ଥିର ଶୂନ୍ୟ
update_cursor(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା redraw)
{
    ଯଦି (term->tl_normal_mode)
	ଫେରନ୍ତୁ;
#ifdef FEAT_GUI
    ଯଦି (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    ଅନ୍ୟଥା
#endif
    ଯଦି (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    ଅନ୍ୟଥା
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    ଯଦି (redraw)
    {
	aco_save_T	aco;

	ଯଦି (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	ଯଦି (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, ଖାଲି, ଖାଲି, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
scrollbackline_pos_in_buf(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, linenr_T *lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *start_col, size_t *start_pos)
{
    sb_ଧାଡ଼ି_T	*ଧାଡ଼ିଗୁଡ଼ିକ = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା         calc_col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା         i;

    ଯଦି (row < 0 || row >= term->tl_scrollback.ga_len)
	ଫେରନ୍ତୁ;

    ଯଦି (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	ପାଇଁ (i = term->tl_scrollback_scrolled + 1; i > 0 && ଧାଡ଼ିଗୁଡ଼ିକ[i].continuation; --i)
	{
	    calc_pos += ଧାଡ଼ିଗୁଡ଼ିକ[i - 1].sb_bytes;
	    calc_col += ଧାଡ଼ିଗୁଡ଼ିକ[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ[i].continuation)
	    ++i;
    }
    ଅନ୍ୟଥା
    {
	i = 1;
	calc_lnum = 1;
    }

    ପାଇଁ (; i <= row; ++i)
    {
	ଯଦି (!ଧାଡ଼ିଗୁଡ଼ିକ[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	ଅନ୍ୟଥା
	{
	    calc_pos += ଧାଡ଼ିଗୁଡ଼ିକ[i - 1].sb_bytes;
	    calc_col += ଧାଡ଼ିଗୁଡ଼ିକ[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    ଯଦି (start_col)
	*start_col = calc_col;
    ଯଦି (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ସ୍ଥିର ଶୂନ୍ୟ
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_ଧାଡ଼ି_T	*ଧାଡ଼ିଗୁଡ଼ିକ = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା         calc_col = col;
    linenr_T    l;

    ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ;

    ଯଦି (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	ଯେତେବେଳେ (calc_row < term->tl_scrollback.ga_len && ଧାଡ଼ିଗୁଡ଼ିକ[calc_row].continuation)
	    ++calc_row;
    }
    ଅନ୍ୟଥା
    {
	calc_row = 0;
	l = 1;
    }

    ଯେତେବେଳେ (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	ଯଦି (!ଧାଡ଼ିଗୁଡ଼ିକ[calc_row].continuation)
	    ++l;
    }

    ଯେତେବେଳେ (calc_row + 1 < term->tl_scrollback.ga_len && ଧାଡ଼ିଗୁଡ଼ିକ[calc_row + 1].continuation
	    && calc_col >= ଧାଡ଼ିଗୁଡ଼ିକ[calc_row].sb_cols)
    {
	calc_col -= ଧାଡ଼ିଗୁଡ଼ିକ[calc_row].sb_cols;
	++calc_row;
    }

    ଯଦି (row)
	*row = calc_row;
    ଯଦି (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    ଶୂନ୍ୟ
write_to_term(buf_T *buffer, char_u *ସନ୍ଦେଶ, channel_T *channel)
{
    size_t	len = STRLEN(ସନ୍ଦେଶ);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    ଯଦି (term->tl_out_fd != ଖାଲି)
    {
	ch_log(channel, "Writing %d bytes to output file", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
	fwrite(ସନ୍ଦେଶ, len, 1, term->tl_out_fd);
	ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (term->tl_vterm == ଖାଲି)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
	ଫେରନ୍ତୁ;
    }
    ch_log(channel, "writing %d bytes to terminal", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
    cursor_off();
    term_write_job_output(term, ସନ୍ଦେଶ, len);

#ifdef FEAT_GUI
    ଯଦି (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    ଅନ୍ୟଥା
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    ଯଦି (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	ଯଦି (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#ଯଦି defined(FEAT_TABPANEL)
	    ଯଦି (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    ଯଦି (buffer == curbuf && curbuf->b_term != ଖାଲି)
		update_cursor(curbuf->b_term, TRUE);
	}
	ଅନ୍ୟଥା
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_send_mouse(VTerm *vterm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    row = mouse_row - W_WINROW(curwin);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    ଯଦି (popup_is_popup(curwin))
    {
	row -= popup_ଶୀର୍ଷ_extra(curwin);
	col -= popup_ବାମ_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    ଯଦି (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା enter_mouse_col = -1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_mouse_click(VTerm *vterm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି)
{
#ଯଦି defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    ଯଦି (state == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    ଯଦି (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	ବାଛ (ଚାବି)
	{
	ସ୍ଥିତି K_LEFTDRAG:
	ସ୍ଥିତି K_LEFTRELEASE:
	ସ୍ଥିତି K_RIGHTDRAG:
	ସ୍ଥିତି K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ଯଦି (ignore_drag_release)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_mouse_col, save_mouse_row;

		    ଯଦି (enter_mouse_col < 0)
			ଭାଙ୍ଗ;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_ବାମ, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	ସ୍ଥିତି K_LEFTMOUSE:
	ସ୍ଥିତି K_RIGHTMOUSE:
		ଯଦି (ଚାବି == K_LEFTRELEASE || ଚାବି == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		ଅନ୍ୟଥା
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		ଯଦି (clip_star.available)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(ଚାବି),
							 &is_click, &is_drag);
		    ଯଦି (mouse_model_popup() && button == MOUSE_ବାମ
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ଡାହାଣ;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି K_MIDDLEMOUSE:
		ଯଦି (clip_star.available)
		    ଭର୍ତ୍ତି_reg('*', TRUE);
		ଭାଙ୍ଗ;
	}
	enter_mouse_col = -1;
	ଫେରନ୍ତୁ FALSE;
    }
#endif
    enter_mouse_col = -1;

    ବାଛ (ଚାବି)
    {
	ସ୍ଥିତି K_LEFTMOUSE:
	ସ୍ଥିତି K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_LEFTRELEASE:
	ସ୍ଥିତି K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_convert_ଚାବି(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modmask, ଅକ୍ଷର *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_ଚାବି	    ଚାବି = VTERM_ଚାବି_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    other = FALSE;

    ବାଛ (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	ସ୍ଥିତି K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ଯଦି (term->tl_conpty)
		ଚାବି = VTERM_ଚାବି_BACKSPACE;
	    ଅନ୍ୟଥା
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    ଭାଙ୍ଗ;

#ifdef MSWIN
	ସ୍ଥିତି BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ଯଦି (term->tl_conpty)
		ଚାବି = VTERM_ଚାବି_BACKSPACE;
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି ESC:		ଚାବି = VTERM_ଚାବି_ESCAPE; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_DEL:		ଚାବି = VTERM_ଚାବି_DEL; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_DOWN:		ଚାବି = VTERM_ଚାବି_DOWN; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_DOWN; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_END:		ଚାବି = VTERM_ଚାବି_END; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_END:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_END; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_C_END:		mod = VTERM_MOD_CTRL;
				ଚାବି = VTERM_ଚାବି_END; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F10:		ଚାବି = VTERM_ଚାବି_FUNCTION(10); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F11:		ଚାବି = VTERM_ଚାବି_FUNCTION(11); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F12:		ଚାବି = VTERM_ଚାବି_FUNCTION(12); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F1:		ଚାବି = VTERM_ଚାବି_FUNCTION(1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F2:		ଚାବି = VTERM_ଚାବି_FUNCTION(2); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F3:		ଚାବି = VTERM_ଚାବି_FUNCTION(3); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F4:		ଚାବି = VTERM_ଚାବି_FUNCTION(4); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F5:		ଚାବି = VTERM_ଚାବି_FUNCTION(5); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F6:		ଚାବି = VTERM_ଚାବି_FUNCTION(6); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F7:		ଚାବି = VTERM_ଚାବି_FUNCTION(7); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F8:		ଚାବି = VTERM_ଚାବି_FUNCTION(8); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F9:		ଚାବି = VTERM_ଚାବି_FUNCTION(9); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_HOME:		ଚାବି = VTERM_ଚାବି_HOME; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_HOME:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_HOME; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_C_HOME:		mod = VTERM_MOD_CTRL;
				ଚାବି = VTERM_ଚାବି_HOME; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_INS:		ଚାବି = VTERM_ଚାବି_INS; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K0:		ଚାବି = VTERM_ଚାବି_KP_0; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K1:		ଚାବି = VTERM_ଚାବି_KP_1; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K2:		ଚାବି = VTERM_ଚାବି_KP_2; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K3:		ଚାବି = VTERM_ଚାବି_KP_3; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K4:		ଚାବି = VTERM_ଚାବି_KP_4; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K5:		ଚାବି = VTERM_ଚାବି_KP_5; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K6:		ଚାବି = VTERM_ଚାବି_KP_6; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K7:		ଚାବି = VTERM_ଚାବି_KP_7; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K8:		ଚାବି = VTERM_ଚାବି_KP_8; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_K9:		ଚାବି = VTERM_ଚାବି_KP_9; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_KDEL:		ଚାବି = VTERM_ଚାବି_DEL; ଭାଙ୍ଗ; // TODO
	ସ୍ଥିତି K_KDIVIDE:		ଚାବି = VTERM_ଚାବି_KP_DIVIDE; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_KEND:		ଚାବି = VTERM_ଚାବି_KP_1; ଭାଙ୍ଗ; // TODO
	ସ୍ଥିତି K_KENTER:		ଚାବି = VTERM_ଚାବି_KP_ENTER; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_KHOME:		ଚାବି = VTERM_ଚାବି_KP_7; ଭାଙ୍ଗ; // TODO
	ସ୍ଥିତି K_KINS:		ଚାବି = VTERM_ଚାବି_KP_0; ଭାଙ୍ଗ; // TODO
	ସ୍ଥିତି K_KMINUS:		ଚାବି = VTERM_ଚାବି_KP_MINUS; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_KMULTIPLY:	ଚାବି = VTERM_ଚାବି_KP_MULT; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_KPAGEDOWN:	ଚାବି = VTERM_ଚାବି_KP_3; ଭାଙ୍ଗ; // TODO
	ସ୍ଥିତି K_KPAGEUP:		ଚାବି = VTERM_ଚାବି_KP_9; ଭାଙ୍ଗ; // TODO
	ସ୍ଥିତି K_KPLUS:		ଚାବି = VTERM_ଚାବି_KP_PLUS; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_KPOINT:		ଚାବି = VTERM_ଚାବି_KP_PERIOD; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_ବାମ:		ଚାବି = VTERM_ଚାବି_ବାମ; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_ବାମ:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_ବାମ; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_C_ବାମ:		mod = VTERM_MOD_CTRL;
				ଚାବି = VTERM_ଚାବି_ବାମ; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_PAGEDOWN:	ଚାବି = VTERM_ଚାବି_PAGEDOWN; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_PAGEUP:		ଚାବି = VTERM_ଚାବି_PAGEUP; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_ଡାହାଣ:		ଚାବି = VTERM_ଚାବି_ଡାହାଣ; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_ଡାହାଣ:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_ଡାହାଣ; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_C_ଡାହାଣ:		mod = VTERM_MOD_CTRL;
				ଚାବି = VTERM_ଚାବି_ଡାହାଣ; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_UP:		ଚାବି = VTERM_ଚାବି_UP; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_UP:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_UP; ଭାଙ୍ଗ;
	ସ୍ଥିତି TAB:		ଚାବି = VTERM_ଚାବି_TAB; ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_TAB:		mod = VTERM_MOD_SHIFT;
				ଚାବି = VTERM_ଚାବି_TAB; ଭାଙ୍ଗ;

	ସ୍ଥିତି K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); ଭାଙ୍ଗ;

	ସ୍ଥିତି K_LEFTMOUSE:
	ସ୍ଥିତି K_LEFTMOUSE_NM:
	ସ୍ଥିତି K_LEFTDRAG:
	ସ୍ଥିତି K_LEFTRELEASE:
	ସ୍ଥିତି K_LEFTRELEASE_NM:
	ସ୍ଥିତି K_MOUSEMOVE:
	ସ୍ଥିତି K_MIDDLEMOUSE:
	ସ୍ଥିତି K_MIDDLEDRAG:
	ସ୍ଥିତି K_MIDDLERELEASE:
	ସ୍ଥିତି K_RIGHTMOUSE:
	ସ୍ଥିତି K_RIGHTDRAG:
	ସ୍ଥିତି K_RIGHTRELEASE:	ଯଦି (!term_mouse_click(vterm, c))
				    ଫେରନ୍ତୁ 0;
				other = TRUE;
				ଭାଙ୍ଗ;

	ସ୍ଥିତି K_X1MOUSE:		/* TODO */ ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_X1DRAG:		/* TODO */ ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_X1RELEASE:	/* TODO */ ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_X2MOUSE:		/* TODO */ ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_X2DRAG:		/* TODO */ ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_X2RELEASE:	/* TODO */ ଫେରନ୍ତୁ 0;

	ସ୍ଥିତି K_IGNORE:		ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_NOP:		ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_UNDO:		ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_HELP:		ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_XF1:		ଚାବି = VTERM_ଚାବି_FUNCTION(1); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_XF2:		ଚାବି = VTERM_ଚାବି_FUNCTION(2); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_XF3:		ଚାବି = VTERM_ଚାବି_FUNCTION(3); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_XF4:		ଚାବି = VTERM_ଚାବି_FUNCTION(4); ଭାଙ୍ଗ;
	ସ୍ଥିତି K_SELECT:		ଫେରନ୍ତୁ 0;
#ifdef FEAT_GUI
	ସ୍ଥିତି K_VER_SCROLLBAR:	ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_HOR_SCROLLBAR:	ଫେରନ୍ତୁ 0;
#endif
#ifdef FEAT_GUI_TABLINE
	ସ୍ଥିତି K_TABLINE:		ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_TABMENU:		ଫେରନ୍ତୁ 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	ସ୍ଥିତି K_F21:		ଚାବି = VTERM_ଚାବି_FUNCTION(21); ଭାଙ୍ଗ;
#endif
#ifdef FEAT_DND
	ସ୍ଥିତି K_DROP:		ଫେରନ୍ତୁ 0;
#endif
	ସ୍ଥିତି K_CURSORHOLD:	ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				ଭାଙ୍ଗ;
	ସ୍ଥିତି K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				ଭାଙ୍ଗ;
    }

    // add modifiers for the typed key
    ଯଦି (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    ଯଦି (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    ଯଦି (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    ଯଦି (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    ଯଦି (ଚାବି != VTERM_ଚାବି_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_ଚାବି(vterm, ଚାବି, mod);
    ଅନ୍ୟଥା ଯଦି (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vterm_output_read(vterm, buf, ଚାବି_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_job_running_check(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ଯଦି (term == ଖାଲି
	|| term->tl_job == ଖାଲି
	|| !channel_is_open(term->tl_job->jv_channel))
	ଫେରନ୍ତୁ FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    ଯଦି (check_job_status)
	job_status(job);
    ଫେରନ୍ତୁ (job->jv_status == JOB_STARTED
	    || (job->jv_channel != ଖାଲି && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_job_running(term_T *term)
{
    ଫେରନ୍ତୁ term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_job_running_not_none(term_T *term)
{
    ଫେରନ୍ତୁ term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ଫେରନ୍ତୁ term != ଖାଲି
	&& term->tl_job != ଖାଲି
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_ସନ୍ଦେଶ_SIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ret;

    dialog_ସନ୍ଦେଶ(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ଖାଲି, buff, 1);
    ଯଦି (ret == VIM_YES)
	ଫେରନ୍ତୁ OK;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_try_stop_job(buf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2;
    ଅକ୍ଷର    *how = (ଅକ୍ଷର *)buf->b_term->tl_kill;

#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    ଯଦି ((how == ଖାଲି || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	ଯଦି (term_confirm_stop(buf) == OK)
	    how = "kill";
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FAIL;
    }
#endif
    ଯଦି (how == ଖାଲି || *how == NUL)
	ଫେରନ୍ତୁ FAIL;

    job_stop(buf->b_term->tl_job, ଖାଲି, how);

    // wait for up to a second for the job to die
    ପାଇଁ (ଗଣନା_2 = 0; ଗଣନା_2 < 100; ++ଗଣନା_2)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	ଯଦି (!buf_valid(buf)
		|| buf->b_term == ଖାଲି
		|| buf->b_term->tl_job == ଖାଲି)
	    ଫେରନ୍ତୁ OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	ଯଦି (job->jv_status >= JOB_ENDED)
	    ଫେରନ୍ତୁ OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_scrollback_ଧାଡ଼ି_to_buffer(term_T *term, char_u *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା append)
{
    buf_T	*buf = term->tl_buffer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

#ifdef MSWIN
    char_u *tmp = text;
    ଯଦି (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (ଅକ୍ଷର*)text, len + 1,
							   &ret, &length);
	ଯଦି (ret != ଖାଲି)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (ଅକ୍ଷର **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    ଯଦି (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	ଯଦି (both == ଖାଲି)
	    ଫେରନ୍ତୁ;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    ଅନ୍ୟଥା
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    ଯଦି (tmp != text)
	vim_free(text);
#endif
    ଯଦି (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
cell2cellattr(ଅଚଳ VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    ଫେରନ୍ତୁ a->fg.red == b->fg.red
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum)
{
    ଯଦି (ga_grow(&term->tl_scrollback, 1) == FAIL)
	ଫେରନ୍ତୁ FALSE;

    sb_ଧାଡ଼ି_T *ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    ଯଦି (lnum > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *ଧାଡ଼ି = *(ଧାଡ଼ି - 1);
	    --ଧାଡ଼ି;
	}
    }
    ଧାଡ଼ି->sb_cols = 0;
    ଧାଡ଼ି->sb_bytes = 0;
    ଧାଡ଼ି->sb_cells = ଖାଲି;
    ଧାଡ଼ି->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cleanup_scrollback(term_T *term)
{
    sb_ଧାଡ଼ି_T	*ଧାଡ଼ି;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, FALSE);
    bufline_length = STRLEN(bufline);
    ଯେତେବେଳେ (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)gap->ga_data + gap->ga_len - 1;
	ଯଦି (ଧାଡ଼ି->sb_bytes < 0 || (size_t)ଧାଡ଼ି->sb_bytes > bufline_length)
	    ଭାଙ୍ଗ;
	bufline_length -= ଧାଡ଼ି->sb_bytes;
	ଯଦି (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(ଧାଡ଼ି->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    ଯଦି (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    ଯଦି (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଧାଡ଼ିଗୁଡ଼ିକ_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == ଖାଲି ? ଖାଲି : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    ପାଇଁ (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	ପାଇଁ (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    ଯଦି (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    ଅନ୍ୟଥା
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	ଯଦି (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++ଧାଡ଼ିଗୁଡ଼ିକ_skipped;
	ଅନ୍ୟଥା
	{
	    ଯେତେବେଳେ (ଧାଡ଼ିଗୁଡ଼ିକ_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--ଧାଡ଼ିଗୁଡ଼ିକ_skipped;
		ଯଦି (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_ଧାଡ଼ି_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    ଯଦି (len == 0)
		p = ଖାଲି;
	    ଅନ୍ୟଥା
		p = ALLOC_MULT(cellattr_T, len);
	    ଯଦି ((p != ଖାଲି || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width;
		ଅଚଳ VTerm_ଧାଡ଼ି_Info *lineinfo;
		sb_ଧାଡ଼ି_T   *ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		ପାଇଁ (pos.col = 0; pos.col < len; pos.col += width)
		{
		    ଯଦି (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			ଯଦି (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    ଅନ୍ୟଥା
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			ଯଦି (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			ଯଦି (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

			    ପାଇଁ (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		ଧାଡ଼ି->sb_cols = len;
		ଧାଡ଼ି->sb_bytes = ga.ga_len;
		ଧାଡ଼ି->sb_cells = p;
		ଧାଡ଼ି->sb_fill_attr = new_fill_attr;
		ଧାଡ଼ି->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		ଯଦି (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_ଧାଡ଼ି_to_buffer(term, (char_u *)"", 0, 0);
		ଅନ୍ୟଥା
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_ଧାଡ଼ି_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    ଅନ୍ୟଥା
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    ପାଇଁ (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	ଯଦି (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_ଧାଡ଼ି_to_buffer(term, (char_u *)"", 0, 0);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
for_all_windows_and_curwin(win_T **wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *did_curwin)
{
    ଯଦି (*wp == ଖାଲି)
	*wp = firstwin;
    ଅନ୍ୟଥା ଯଦି ((*wp)->w_next != ଖାଲି)
	*wp = (*wp)->w_next;
    ଅନ୍ୟଥା ଯଦି (!*did_curwin)
	*wp = curwin;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (*wp == curwin)
	*did_curwin = TRUE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    ଶୂନ୍ୟ
may_move_terminal_to_buffer(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା redraw)
{
    ଯଦି (term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    ଯଦି (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    ଯଦି (redraw)
    {
	win_T	    *wp = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_curwin = FALSE;

	ଯେତେବେଳେ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    ଯଦି (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		ଯଦି (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    ଯଦି (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#ଯଦି defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_check_timers(ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_due_arg, proftime_T *now)
{
    term_T  *term;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	ଯଦି (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_ବାମ(&term->tl_timer_due, now);

	    ଯଦି (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    ଅନ୍ୟଥା ଯଦି (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    ଫେରନ୍ତୁ next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_terminal_mode(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    ଯଦି (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    ଯଦି (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    ଯଦି (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_enter_normal_mode(ଶୂନ୍ୟ)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, ଖାଲି);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    ଯଦି (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    ଫେରନ୍ତୁ term != ଖାଲି && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    ଶୂନ୍ୟ
term_enter_job_mode(ଶୂନ୍ୟ)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    ଯଦି (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    ଯଦି (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_ଚାବି_state_T;

ସ୍ଥିର reduce_ଚାବି_state_T  no_reduce_ଚାବି_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vterm_using_ଚାବି_protocol(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curbuf->b_term != ଖାଲି
	&& curbuf->b_term->tl_vterm != ଖାଲି
	&& (vterm_is_modify_other_ଚାବିଗୁଡ଼ିକ(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    ଶୂନ୍ୟ
check_no_reduce_ଚାବିଗୁଡ଼ିକ(ଶୂନ୍ୟ)
{
    ଯଦି (no_reduce_ଚାବି_state != NRKS_CHECK
	    || no_reduce_ଚାବିଗୁଡ଼ିକ >= 1
	    || curbuf->b_term == ଖାଲି
	    || curbuf->b_term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (vterm_using_ଚାବି_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_ଚାବି_state = NRKS_SET;
	++no_reduce_ଚାବିଗୁଡ଼ିକ;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_vgetc(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    ଯଦି (vterm_using_ଚାବି_protocol())
    {
	++no_reduce_ଚାବିଗୁଡ଼ିକ;
	no_reduce_ଚାବି_state = NRKS_SET;
    }
    ଅନ୍ୟଥା
    {
	no_reduce_ଚାବି_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    ଯଦି (no_reduce_ଚାବି_state == NRKS_SET)
	--no_reduce_ଚାବିଗୁଡ଼ିକ;
    no_reduce_ଚାବି_state = NRKS_NONE;

    ଫେରନ୍ତୁ c;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
send_ଚାବିଗୁଡ଼ିକ_to_term(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modmask, ପୂର୍ଣ୍ଣସଂଖ୍ୟା typed)
{
    ଅକ୍ଷର	ସନ୍ଦେଶ[ଚାବି_BUF_LEN];
    size_t	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    ବାଛ (c)
    {
	ସ୍ଥିତି NUL:
	ସ୍ଥିତି K_ZERO:
	    ଯଦି (typed)
		stuffcharReadbuff(c);
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି K_TABLINE:
	    stuffcharReadbuff(c);
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି K_IGNORE:
	ସ୍ଥିତି K_CANCEL:  // used for :normal when running out of chars
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି K_LEFTDRAG:
	ସ୍ଥିତି K_MIDDLEDRAG:
	ସ୍ଥିତି K_RIGHTDRAG:
	ସ୍ଥିତି K_X1DRAG:
	ସ୍ଥିତି K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	ସ୍ଥିତି K_LEFTMOUSE:
	ସ୍ଥିତି K_LEFTMOUSE_NM:
	ସ୍ଥିତି K_LEFTRELEASE:
	ସ୍ଥିତି K_LEFTRELEASE_NM:
	ସ୍ଥିତି K_MOUSEMOVE:
	ସ୍ଥିତି K_MIDDLEMOUSE:
	ସ୍ଥିତି K_MIDDLERELEASE:
	ସ୍ଥିତି K_RIGHTMOUSE:
	ସ୍ଥିତି K_RIGHTRELEASE:
	ସ୍ଥିତି K_X1MOUSE:
	ସ୍ଥିତି K_X1RELEASE:
	ସ୍ଥିତି K_X2MOUSE:
	ସ୍ଥିତି K_X2RELEASE:

	ସ୍ଥିତି K_MOUSEUP:
	ସ୍ଥିତି K_MOUSEDOWN:
	ସ୍ଥିତି K_MOUSELEFT:
	ସ୍ଥିତି K_MOUSERIGHT:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row = mouse_row;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		ଯଦି (popup_is_popup(curwin))
		{
		    row -= popup_ଶୀର୍ଷ_extra(curwin);
		    col -= popup_ବାମ_extra(curwin);
		}
#endif
		ଯଦି (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    ଯଦି (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_COMMAND:
	ସ୍ଥିତି K_SCRIPT_COMMAND:
	    ଫେରନ୍ତୁ do_cmdkey_command(c, 0);
    }
    ଯଦି (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_ଚାବି(term, c, modmask, ସନ୍ଦେଶ);
    ଯଦି (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)ସନ୍ଦେଶ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, ଖାଲି);

    ଫେରନ୍ତୁ OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_paste_register(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_c UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;

    ଯଦି (add_to_showcmd(prev_c))
    ଯଦି (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    ଯଦି (!term_use_loop())
	// job finished while waiting for a character
	ଫେରନ୍ତୁ;

    // CTRL-W "= prompt for expression to evaluate.
    ଯଦି (c == '=' && get_ପଦସମଷ୍ଟି_register() != '=')
	ଫେରନ୍ତୁ;
    ଯଦି (!term_use_loop())
	// job finished while waiting for a character
	ଫେରନ୍ତୁ;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	ଯଦି (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (ଅକ୍ଷର *)s,
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s), &ret, &length);
	    ଯଦି (ret != ଖାଲି)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (ଅକ୍ଷର **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s), ଖାଲି);
#ifdef MSWIN
	ଯଦି (tmp != s)
	    vim_free(s);
#endif

	ଯଦି (item->li_next != ଖାଲି || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, ଖାଲି);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
terminal_is_active(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ in_terminal_loop != ଖାଲି;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    ଯଦି (wp != ଖାଲି && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    ଅନ୍ୟଥା ଯଦି (term->tl_highlight_name != ଖାଲି)
	name = term->tl_highlight_name;
    ଅନ୍ୟଥା
	name = (char_u*)"Terminal";

    ଫେରନ୍ତୁ syn_name2id(name);
}

#ଯଦି defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    ସ୍ଥିର cursorentry_T entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ବାମ ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    ଯଦି (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    ଯଦି (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    ଯଦି (term_bg != INVALCOLOR)
	*fg = term_bg;
    ଅନ୍ୟଥା
	*fg = gui.back_pixel;

    ଯଦି (term->tl_cursor_color == ଖାଲି)
    {
	ଯଦି (term_fg != INVALCOLOR)
	    *bg = term_fg;
	ଅନ୍ୟଥା
	    *bg = gui.norm_pixel;
    }
    ଅନ୍ୟଥା
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    ଫେରନ୍ତୁ &entry;
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
may_output_cursor_props(ଶୂନ୍ୟ)
{
    ଯଦି (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	ଯଦି (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	ଅନ୍ୟଥା
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (in_terminal_loop == term)
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
    ସ୍ଥିର ଶୂନ୍ୟ
prepare_restore_cursor_props(ଶୂନ୍ୟ)
{
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
#endif
    cursor_color_copy(&desired_cursor_color, ଖାଲି);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_use_loop_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_job_status)
{
    term_T *term = curbuf->b_term;

    ଫେରନ୍ତୁ term != ଖାଲି
	&& !term->tl_normal_mode
	&& term->tl_vterm != ଖାଲି
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_use_loop(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    ଶୂନ୍ୟ
term_win_entered(ଶୂନ୍ୟ)
{
    term_T *term = curbuf->b_term;

    ଯଦି (term == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	    stop_ଭର୍ତ୍ତି_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    ଶୂନ୍ୟ
term_focus_change(ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_focus)
{
    term_T *term = curbuf->b_term;

    ଯଦି (term == ଖାଲି || term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    ଯଦି (in_focus)
	vterm_state_focus_in(state);
    ଅନ୍ୟଥା
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
raw_c_to_ctrl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	ଫେରନ୍ତୁ c & 0x1f;
    ଫେରନ୍ତୁ c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ctrl_to_raw_c(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c < 0x20 && vterm_using_ଚାବି_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	ଫେରନ୍ତୁ c + '@';
    }
    ଫେରନ୍ତୁ c;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
terminal_loop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା blocking)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		raw_c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		termwinkey = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
#ifdef UNIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    ଯଦି (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_ଚାବି(curwin->w_p_twk, TRUE);
	ଯଦି (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    ଯେତେବେଳେ (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	ଯଦି (curbuf->b_term != ଖାଲି && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    ଯେତେବେଳେ (must_redraw != 0)
		ଯଦି (update_screen(0) == FAIL)
		    ଭାଙ୍ଗ;
	ଯଦି (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    ଭାଙ୍ଗ;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	ଯଦି (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    ଯଦି (raw_c != K_IGNORE)
		vungetc(raw_c);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (raw_c == K_IGNORE)
	    ଜାରି;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	ଯଦି (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    ଯଦି (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	ଯଦି (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	ଯଦି ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_c = c;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_raw_c = raw_c;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_mod_mask = mod_mask;

	    ଯଦି (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    ଯଦି (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		ଭାଙ୍ଗ;

	    ଯଦି (prev_c == Ctrl_BSL)
	    {
		ଯଦି (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    ଯାଅ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_ଚାବିଗୁଡ଼ିକ_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    ଅନ୍ୟଥା ଯଦି (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    ଅନ୍ୟଥା ଯଦି (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    ଅନ୍ୟଥା ଯଦି (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    ଅନ୍ୟଥା ଯଦି (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		ଯାଅ theend;
	    }
	    ଅନ୍ୟଥା ଯଦି (c == '"')
	    {
		term_paste_register(prev_c);
		ଜାରି;
	    }
	    ଅନ୍ୟଥା ଯଦି (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		ଯାଅ theend;
	    }
	}
#ifdef MSWIN
	ଯଦି (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    ଯଦି (MultiByteToWideChar(GetACP(), 0, (ଅକ୍ଷର*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	ଯଦି (send_ଚାବିଗୁଡ଼ିକ_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    ଯଦି (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    ଯାଅ theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = ଖାଲି;
    ଯଦି (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    ଯଦି (curbuf->b_term != ଖାଲି && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ଶୂନ୍ୟ
may_toggle_cursor(term_T *term)
{
    ଯଦି (in_terminal_loop != term)
	ଫେରନ୍ତୁ;

    ଯଦି (term->tl_cursor_visible)
	cursor_on();
    ଅନ୍ୟଥା
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
color2index(VTermColor *color, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *boldp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା red = color->red;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା blue = color->blue;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା green = color->green;

    *boldp = FALSE;

    ଯଦି (VTERM_COLOR_IS_INVALID(color))
	ଫେରନ୍ତୁ 0;

    ଯଦି (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	ଯଦି (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    ଯଦି (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    ଫେରନ୍ତୁ index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	ଅନ୍ୟଥା ଯଦି (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    ଫେରନ୍ତୁ (color->index & 7) + 1;
	}
	ଫେରନ୍ତୁ 0;
    }

    ଯଦି (t_colors >= 256)
    {
	ଯଦି (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    ଯଦି (red < 5)
		ଫେରନ୍ତୁ 17; // 00/00/00
	    ଯଦି (red > 245) // ff/ff/ff
		ଫେରନ୍ତୁ 232;
	    ପାଇଁ (i = 0; i < 23; ++i)
		ଯଦି (red < cutoff[i])
		    ଫେରନ୍ତୁ i + 233;
	    ଫେରନ୍ତୁ 256;
	}
	{
	    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ri, gi, bi;

	    // 216-color cube
	    ପାଇଁ (ri = 0; ri < 5; ++ri)
		ଯଦି (red < cutoff[ri])
		    ଭାଙ୍ଗ;
	    ପାଇଁ (gi = 0; gi < 5; ++gi)
		ଯଦି (green < cutoff[gi])
		    ଭାଙ୍ଗ;
	    ପାଇଁ (bi = 0; bi < 5; ++bi)
		ଯଦି (blue < cutoff[bi])
		    ଭାଙ୍ଗ;
	    ଫେରନ୍ତୁ 17 + ri * 36 + gi * 6 + bi;
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr = 0;

    ଯଦି (cellattrs->bold)
	attr |= HL_BOLD;
    ଯଦି (cellattrs->underline)
	attr |= HL_UNDERLINE;
    ଯଦି (cellattrs->italic)
	attr |= HL_ITALIC;
    ଯଦି (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    ଯଦି (cellattrs->reverse)
	attr |= HL_INVERSE;
    ଫେରନ୍ତୁ attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hl2vtermAttr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    ଯଦି (attr & HL_BOLD)
	cell->attrs.bold = 1;
    ଯଦି (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    ଯଦି (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    ଯଦି (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    ଯଦି (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    ଯଦି (is_default_fg || is_default_bg)
    {
	ଯଦି (wp != ଖାଲି && wp->w_hlfwin_id != 0)
	{
	    ଯଦି (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    ଯଦି (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (is_default_fg)
		fg = &term->tl_default_color.fg;
	    ଯଦି (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	ଫେରନ୍ତୁ get_gui_attr_idx(attr, guifg, guibg);
    }
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_TERMGUICOLORS
    ଯଦି (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	ଫେରନ୍ତୁ get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    ଅନ୍ୟଥା
#endif
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold = MAYBE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctermfg = color2index(fg, TRUE, &bold);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	ଯଦି (bold == TRUE)
	    attr |= HL_BOLD;

	ଫେରନ୍ତୁ get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    ଯଦି (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_damage(VTermRect rect, ଶୂନ୍ୟ *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ଶୂନ୍ୟ
term_scroll_up(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    win_T		 *wp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			 clear_attr;

    CLEAR_FIELD(attr);

    ଯେତେବେଳେ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ଯଦି (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_ଧାଡ଼ିଗୁଡ଼ିକ(wp, start_row, ଗଣନା_2, FALSE, FALSE, clear_attr);
	}
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_moverect(VTermRect dest, VTermRect src, ଶୂନ୍ୟ *user)
{
    term_T	*term = (term_T *)user;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    ଯଦି (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	ଯଦି (dest.start_row == 0)
	    term->tl_postponed_scroll += ଗଣନା_2;
	ଅନ୍ୟଥା
	    term_scroll_up(term, dest.start_row, ଗଣନା_2);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା visible,
	ଶୂନ୍ୟ *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    ଯେତେବେଳେ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ଯଦି (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    ଯଦି (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_settermprop(
	VTermProp prop,
	VTerm_ମୂଲ୍ୟ *ମୂଲ୍ୟ,
	ଶୂନ୍ୟ *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = ଖାଲି;

    ବାଛ (prop)
    {
	ସ୍ଥିତି VTERM_PROP_TITLE:
	    ଯଦି (disable_vterm_title_for_testing)
		ଭାଙ୍ଗ;
	    strval = vim_strnsave((char_u *)ମୂଲ୍ୟ->string.str,
							    ମୂଲ୍ୟ->string.len);
	    ଯଦି (strval == ଖାଲି)
		ଭାଙ୍ଗ;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    ଯଦି (*skipwhite(strval) == NUL)
		term->tl_title = ଖାଲି;
	    // Same as blank
	    ଅନ୍ୟଥା ଯଦି (term->tl_arg0_cmd != ଖାଲି
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = ଖାଲି;
	    // Empty corrupted data of winpty
	    ଅନ୍ୟଥା ଯଦି (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = ଖାଲି;
#ifdef MSWIN
	    ଅନ୍ୟଥା ଯଦି (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ଖାଲି;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(ଅକ୍ଷର*)ମୂଲ୍ୟ->string.str,
					(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ->string.len, &ret, &length);
		ଯଦି (ret != ଖାଲି)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (ଅକ୍ଷର**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    ଅନ୍ୟଥା
	    {
		term->tl_title = strval;
		strval = ଖାଲି;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    ଯଦି (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = ମୂଲ୍ୟ->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = ମୂଲ୍ୟ->boolean;
	    may_set_cursor_props(term);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = ମୂଲ୍ୟ->ସଂଖ୍ୟା;
	    may_set_cursor_props(term);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)ମୂଲ୍ୟ->string.str,
							    ମୂଲ୍ୟ->string.len);
	    ଯଦି (strval == ଖାଲି)
		ଭାଙ୍ଗ;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = ମୂଲ୍ୟ->boolean;
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    ଫେରନ୍ତୁ 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_resize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଶୂନ୍ୟ *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    ଯଦି (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    ଅନ୍ୟଥା
    {
	FOR_ALL_WINDOWS(wp)
	{
	    ଯଦି (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    ଫେରନ୍ତୁ 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
limit_scrollback(term_T *term, garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା update_buffer)
{
    ଯଦି (gap->ga_len < term->tl_buffer->b_p_twsl)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    sb_ଧାଡ଼ି_T *sb_ଧାଡ଼ିଗୁଡ଼ିକ = (sb_ଧାଡ଼ି_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    ପାଇଁ (i = 0; i < todo; ++i)
    {
	ଯଦି (update_buffer && (!sb_ଧାଡ଼ିଗୁଡ଼ିକ[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_ଧାଡ଼ିଗୁଡ଼ିକ[i].sb_cells);
    }
    // Continue until end of wrapped line
    ପାଇଁ (; todo < gap->ga_len && sb_ଧାଡ଼ିଗୁଡ଼ିକ[todo].continuation; ++todo)
	vim_free(sb_ଧାଡ଼ିଗୁଡ଼ିକ[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_ଧାଡ଼ି_T *)gap->ga_data + todo,
		ଆକାର(sb_ଧାଡ଼ି_T) * gap->ga_len);
    ଯଦି (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ଖାଲି;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    ଯଦି (wp->w_buffer == term->tl_buffer)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_pushline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଅଚଳ VTermScreenCell *cells, ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuation, ଶୂନ୍ୟ *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		update_buffer;

    ଯଦି (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    ଅନ୍ୟଥା
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ 0;

    cellattr_T	*p = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_len;
    char_u		*text;
    sb_ଧାଡ଼ି_T	*ଧାଡ଼ି;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    ପାଇଁ (i = 0; i < cols; ++i)
	ଯଦି (cells[i].chars[0] != 0)
	    len = i + 1;
	ଅନ୍ୟଥା
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    ଯଦି (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    ଯଦି (p != ଖାଲି)
    {
	ପାଇଁ (col = 0; col < len; col += cells[col].width)
	{
	    ଯଦି (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		ଭାଙ୍ଗ;
	    }
	    ପାଇଁ (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    ଯଦି (ga_grow(&ga, 1) == FAIL)
    {
	ଯଦି (update_buffer)
	    text = (char_u *)"";
	ଅନ୍ୟଥା
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    ଅନ୍ୟଥା
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    ଯଦି (update_buffer)
	add_scrollback_ଧାଡ଼ି_to_buffer(term, text, text_len, continuation);

    ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)gap->ga_data + gap->ga_len;
    ଧାଡ଼ି->sb_cols = len;
    ଧାଡ଼ି->sb_bytes = text_len;
    ଧାଡ଼ି->sb_cells = p;
    ଧାଡ଼ି->sb_fill_attr = fill_attr;
    ଧାଡ଼ି->continuation = (char_u)continuation;
    ଯଦି (update_buffer)
    {
	ଧାଡ଼ି->sb_text = ଖାଲି;
	++term->tl_scrollback_scrolled;
	ଯଦି (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    ଅନ୍ୟଥା
    {
	ଧାଡ଼ି->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    ଫେରନ୍ତୁ 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_postponed_scrollback(term_T *term)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (term->tl_scrollback_postponed.ga_len == 0)
	ଫେରନ୍ତୁ;
    ch_log(ଖାଲି, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    ପାଇଁ (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_ଧାଡ଼ି_T	*pp_ଧାଡ଼ି;
	sb_ଧାଡ଼ି_T	*ଧାଡ଼ି;

	ଯଦି (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    ଭାଙ୍ଗ;
	pp_ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_ଧାଡ଼ି->sb_text;
	ଯଦି (text == ଖାଲି)
	    text = (char_u *)"";
	add_scrollback_ଧାଡ଼ି_to_buffer(term, text, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(text), pp_ଧାଡ଼ି->continuation);
	vim_free(pp_ଧାଡ଼ି->sb_text);

	ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	ଧାଡ଼ି->sb_cols = pp_ଧାଡ଼ି->sb_cols;
	ଧାଡ଼ି->sb_bytes = pp_ଧାଡ଼ି->sb_bytes;
	ଧାଡ଼ି->sb_cells = pp_ଧାଡ଼ି->sb_cells;
	ଧାଡ଼ି->sb_fill_attr = pp_ଧାଡ଼ି->sb_fill_attr;
	ଧାଡ଼ି->sb_text = ଖାଲି;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	ଯଦି (!pp_ଧାଡ଼ି->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_bell(ଶୂନ୍ୟ *user UNUSED)
{
    vim_beep(BO_TERM);
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ଖାଲି,			// sb_pushline
    ଖାଲି,			// sb_popline
    ଖାଲି,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    ଯଦି (!term->tl_normal_mode)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	ଯଦି (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = ଖାଲି;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    ଯଦି (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		ଯଦି (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    ଅନ୍ୟଥା
#endif
	    // If this is the last normal window: exit Vim.
	    ଯଦି (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		ଫେରନ୍ତୁ TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ଖାଲି, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    ଯଦି (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		ଯଦି (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		ଯଦି (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		ଯଦି (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    ଯଦି (pwin != ଖାଲି)
		popup_close_with_retval(pwin, 0);
#endif
	    ଫେରନ୍ତୁ TRUE;
	}
	ଯଦି (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    ଅକ୍ଷର    *cmd = term->tl_opencmd == ଖାଲି
				? "botright sbuf %d"
				: (ଅକ୍ଷର *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    ଅକ୍ଷର    *buf = alloc(len);

	    ଯଦି (buf != ଖାଲି)
	    {
		ch_log(ଖାଲି, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	ଅନ୍ୟଥା
	    ch_log(ଖାଲି, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_close_term_popup(ଶୂନ୍ୟ)
{
    ଯଦି (!popup_is_popup(curwin) || curbuf->b_term == ଖାଲି
				 || term_job_running_not_none(curbuf->b_term))
	ଫେରନ୍ତୁ FAIL;

    win_T *pwin = curwin;

    ଯଦି (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    ଶୂନ୍ୟ
term_channel_closing(channel_T *ch)
{
    term_T *term;

    ପାଇଁ (term = first_term; term != ଖାଲି; term = term->tl_next)
	ଯଦି (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    ଶୂନ୍ୟ
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_one = FALSE;

    ପାଇଁ (term = first_term; term != ଖାଲି; term = next_term)
    {
	next_term = term->tl_next;
	ଯଦି (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    ଯଦି (term->tl_out_fd != ଖାଲି)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = ଖାଲି;
	    }
#endif

	    ଯଦି (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		ଜାରି;
	    }

	    ଯଦି (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    ଯଦି (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	ଯଦି (term != ଖାଲି)
	{
	    ଯଦି (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    ଶୂନ୍ୟ
term_check_channel_closed_recently(ଶୂନ୍ୟ)
{
    term_T *term;
    term_T *next_term;

    ପାଇଁ (term = first_term; term != ଖାଲି; term = next_term)
    {
	next_term = term->tl_next;
	ଯଦି (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    ଯଦି (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = screen_get_current_ଧାଡ଼ି_off();

    ପାଇଁ (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

	ଯଦି (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	ଯଦି (c == NUL)
	{
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = ' ';
	    ଯଦି (enc_utf8)
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = NUL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (enc_utf8)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		// composing chars
		ପାଇଁ (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_C[i][off] = cell.chars[i + 1];
		    ଯଦି (cell.chars[i + 1] == 0)
			ଭାଙ୍ଗ;
		}
		ଯଦି (c >= 0x80 || (Screen_mco > 0
					 && Screen_ଧାଡ଼ିଗୁଡ଼ିକ_C[0][off] != 0))
		{
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = ' ';
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = c;
		}
		ଅନ୍ୟଥା
		{
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = c;
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    ଅନ୍ୟଥା ଯଦି (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		ଯଦି (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (ଅକ୍ଷର*)mb, 2, 0, 0) > 1)
		{
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = mb[0];
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		ଅନ୍ୟଥା
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = c;
	    }
#endif
	    ଅନ୍ୟଥା
		// This will only store the lower byte of "c".
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	ଯଦି (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    ଯଦି (enc_utf8)
	    {
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = NUL;
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = NUL;
	    }
	    ଅନ୍ୟଥା ଯଦି (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#ଯଦି defined(FEAT_GUI)
    ସ୍ଥିର ଶୂନ୍ୟ
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    ଯଦି (term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    ଯେତେବେଳେ (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_more = p_more;

	p_more = FALSE;
	ସନ୍ଦେଶ_row = Rows - 1;
	ସନ୍ଦେଶ_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    ପାଇଁ (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	ଯଦି (pos.row < term->tl_rows)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, ଖାଲି, screen, &pos, max_col);
	}
	ଅନ୍ୟଥା
	    pos.col = 0;

	screen_ଧାଡ଼ି(curwin, term->tl_toprow + pos.row,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    ଫେରନ୍ତୁ term != ଖାଲି && term->tl_vterm != ଖାଲି && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    ଶୂନ୍ୟ
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rows, cols;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newrows, newcols;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    ଯଦି (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	ଯଦି (term->tl_postponed_scroll > 0
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
    ପାଇଁ (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ଖାଲି ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	ଯଦି (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	ଯଦି (twp == ଖାଲି)
	    ଭାଙ୍ଗ;
    }
    ଯଦି (newrows == 99999 || newcols == 99999)
	ଫେରନ୍ତୁ; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    ଯଦି (newrows == 0 || newcols == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	ଯଦି (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    ପାଇଁ (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	ଯଦି (pos.row < term->tl_rows)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	ଅନ୍ୟଥା
	    pos.col = 0;

	screen_ଧାଡ଼ି(wp, wp->w_winrow + pos.row
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
    ଶୂନ୍ୟ
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    ଯଦି (term == ଖାଲି || term->tl_vterm == ଖାଲି || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	ଫେରନ୍ତୁ;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_is_finished(buf_T *buf)
{
    ଫେରନ୍ତୁ buf->b_term != ଖାଲି && buf->b_term->tl_vterm == ଖାଲି;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    ଫେରନ୍ତୁ term != ଖାଲି && (term->tl_vterm == ଖାଲି || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    ଶୂନ୍ୟ
term_change_in_curbuf(ଶୂନ୍ୟ)
{
    term_T *term = curbuf->b_term;

    ଯଦି (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	ଫେରନ୍ତୁ;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_get_attr(win_T *wp, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_ଧାଡ଼ି_T	*ଧାଡ଼ି;
    cellattr_T	*cellattr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା         sb_ଧାଡ଼ି = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା         sb_col = col;

    ଯଦି (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_ଧାଡ଼ି, &sb_col);

    ଯଦି (sb_ଧାଡ଼ି < 0)
	cellattr = &term->tl_default_color;
    ଅନ୍ୟଥା
    {
	ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data + sb_ଧାଡ଼ି;
	ଯଦି (sb_col < 0 || sb_col >= ଧାଡ଼ି->sb_cols)
	    cellattr = &ଧାଡ଼ି->sb_fill_attr;
	ଅନ୍ୟଥା
	    cellattr = ଧାଡ଼ି->sb_cells + sb_col;
    }
    ଫେରନ୍ତୁ cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cterm_color2vterm(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    ଯଦି (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_vterm_color_from_synid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, VTermColor *fg, VTermColor *bg)
{
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    ଯଦି (0
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

	ଯଦି (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	ଯଦି (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	ଅନ୍ୟଥା
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ଯଦି (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	ଅନ୍ୟଥା
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ଫେରନ୍ତୁ TRUE;
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି (t_colors >= 16)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cterm_fg = -1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cterm_bg = -1;

	ଯଦି (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	ଯଦି (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	ଅନ୍ୟଥା
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ଯଦି (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	ଅନ୍ୟଥା
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
}

    ଶୂନ୍ୟ
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    ଶୂନ୍ୟ
term_update_hlfwin(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = wp->w_hlfwin_id;

    ଯଦି (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ଶୂନ୍ୟ
term_update_hlfwin_all(ଶୂନ୍ୟ)
{
    win_T	 *wp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 did_curwin = FALSE;

    ଯେତେବେଳେ (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    ଶୂନ୍ୟ
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    fgval, bgval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    ଯଦି (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    ଅନ୍ୟଥା
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, ଖାଲି);

    ଯଦି (!get_vterm_color_from_synid(id, fg, bg))
    {
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	ଯଦି (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ଯଦି (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	ଅନ୍ୟଥା
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	ଯଦି (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ଯଦି (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	ଅନ୍ୟଥା
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_use_palette(ଶୂନ୍ୟ)
{
    ଯଦି (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    ପାଇଁ (; index < 16; index++)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    ପାଇଁ (li = list->lv_first; li != ଖାଲି && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ଯଦି (color_name == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	ଯଦି (guicolor == INVALCOLOR)
	    ଫେରନ୍ତୁ FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    ଯଦି (n != 16 || li != ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    set_vterm_palette(vterm, rgb);

    ଫେରନ୍ତୁ OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", ଖାଲି, TRUE);

    ଯଦି (var == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ଖାଲି
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ଖାଲି;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ଯଦି (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    ଫେରନ୍ତୁ;
	}
    }

    CLEAR_FIELD(ea);

    ଯଦି (opt_item != ଖାଲି && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != ଖାଲି)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	ଯଦି (p == ଖାଲି)
	    p = dict_get_string(dict, "fileformat", FALSE);
	ଯଦି (p != ଖାଲି)
	{
	    ଯଦି (check_ff_ମୂଲ୍ୟ(p) == FAIL)
		ch_log(ଖାଲି, "Invalid ff argument to drop: %s", p);
	    ଅନ୍ୟଥା
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	ଯଦି (p == ଖାଲି)
	    p = dict_get_string(dict, "encoding", FALSE);
	ଯଦି (p != ଖାଲି)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    ଯଦି (ea.cmd != ଖାଲି)
	    {
		sprintf((ଅକ୍ଷର *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	ଯଦି (p != ଖାଲି)
	    get_bad_opt(p, &ea);

	ଯଦି (dict_has_ଚାବି(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	ଯଦି (dict_has_ଚାବି(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	ଯଦି (dict_has_ଚାବି(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	ଯଦି (dict_has_ଚାବି(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    ଯଦି (ea.cmd == ଖାଲି)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_permitted_term_api(char_u *func, char_u *pat)
{
    ଫେରନ୍ତୁ pat != ଖାଲି && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    ଯଦି (item->li_next == ଖାଲି)
    {
	ch_log(channel, "Missing function arguments for call");
	ଫେରନ୍ତୁ;
    }
    func = tv_get_string(&item->li_tv);

    ଯଦି (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	ଫେରନ୍ତୁ;
    }

    argvars[0].v_type = VAR_ସଂଖ୍ୟା;
    argvars[0].vval.v_ସଂଖ୍ୟା = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    ଯଦି (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    ଅନ୍ୟଥା
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
    ସ୍ଥିର size_t
url_decode(ଅଚଳ ଅକ୍ଷର *src, ଅଚଳ size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    ଯେତେବେଳେ (i < len)
    {
	ଯଦି (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	ଅନ୍ୟଥା
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    ଫେରନ୍ତୁ j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sync_shell_dir(garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା       offset = 7;  // len of "file://" is 7
    ଅକ୍ଷର      *pos = (ଅକ୍ଷର *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    ଯେତେବେଳେ (offset < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    ଯଦି (offset >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_osc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା command, VTermStringFragment frag, ଶୂନ୍ୟ *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == ଖାଲି ? ଖାଲି
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    ଯଦି (command != 51 && (command != 7 || !p_asd))
	ଫେରନ୍ତୁ 0;

    // Concatenate what was received until the final piece is found.
    ଯଦି (ga_grow(gap, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	ଫେରନ୍ତୁ 1;
    }
    mch_memmove((ଅକ୍ଷର *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frag.len;
    ଯଦି (!frag.final)
	ଫେରନ୍ତୁ 1;

    ((ଅକ୍ଷର *)gap->ga_data)[gap->ga_len] = 0;

    ଯଦି (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	ଫେରନ୍ତୁ 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = ଖାଲି;
    reader.js_used = 0;
    ଯଦି (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ଖାଲି)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	ଯଦି (item == ଖାଲି)
	    ch_log(channel, "Missing command");
	ଅନ୍ୟଥା
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    ଯଦି (item == ଖାଲି)
		ch_log(channel, "Missing argument for %s", cmd);
	    ଅନ୍ୟଥା ଯଦି (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    ଅନ୍ୟଥା ଯଦି (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    ଅନ୍ୟଥା
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    ଅନ୍ୟଥା
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    ଫେରନ୍ତୁ 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_csi(
	ଅଚଳ ଅକ୍ଷର  *leader UNUSED,
	ଅଚଳ long  args[],
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	ଅଚଳ ଅକ୍ଷର  *intermed UNUSED,
	ଅକ୍ଷର	    command,
	ଶୂନ୍ୟ	    *user)
{
    term_T	*term = (term_T *)user;
    ଅକ୍ଷର	buf[100];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    ଯଦି (command != 't' || argcount != 1 || args[0] != 13)
	ଫେରନ୍ତୁ 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#ଯଦି defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (ଶୂନ୍ୟ)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_buffer == term->tl_buffer)
	    ଭାଙ୍ଗ;
    ଯଦି (wp != ଖାଲି)
    {
#ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	ଅନ୍ୟଥା
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
						     (char_u *)buf, len, ଖାଲି);
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର VTermStateFallbacks state_fallbacks = {
    ଖାଲି,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ଖାଲି,		// dcs
    ଖାଲି,		// apc
    ଖାଲି,		// pm
    ଖାଲି		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ସ୍ଥିର ଶୂନ୍ୟ *
vterm_malloc(size_t size, ଶୂନ୍ୟ *data UNUSED)
{
    // make sure that the length is not zero
    ଫେରନ୍ତୁ alloc_clear(size == 0 ? 1L : size);
}

    ସ୍ଥିର ଶୂନ୍ୟ
vterm_memfree(ଶୂନ୍ୟ *ptr, ଶୂନ୍ୟ *data UNUSED)
{
    vim_free(ptr);
}

ସ୍ଥିର VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
create_vterm(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_ମୂଲ୍ୟ	    ମୂଲ୍ୟ;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, ଖାଲି);
    term->tl_vterm = vterm;
    ଯଦି (vterm == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    ଯଦି (state == ଖାଲି || screen == ଖାଲି)
    {
	vterm_free(vterm);
	ଫେରନ୍ତୁ FAIL;
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

    ଯଦି (t_colors < 16)
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
    ଯଦି (GetCaretBlinkTime() == INFINITE)
	ମୂଲ୍ୟ.boolean = 0;
    ଅନ୍ୟଥା
	ମୂଲ୍ୟ.boolean = 1;
#ଅନ୍ୟଥା
    ମୂଲ୍ୟ.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &ମୂଲ୍ୟ);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    ଫେରନ୍ତୁ OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		index;

    ପାଇଁ (index = 0; index < 16; index++)
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

    ସ୍ଥିର ଶୂନ୍ୟ
term_update_palette(term_T *term)
{
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ଯଦି (term_use_palette()
	    && (term->tl_palette != ଖାଲି
		|| find_var((char_u *)"g:terminal_ansi_colors", ଖାଲି, TRUE)
								      != ଖାଲି))
    {
	ଯଦି (term->tl_palette != ଖାଲି)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ଅନ୍ୟଥା
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    ଅନ୍ୟଥା
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    ଶୂନ୍ୟ
term_update_palette_all(ଶୂନ୍ୟ)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	ଯଦି (term->tl_vterm == ଖାଲି)
	    ଜାରି;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ଶୂନ୍ୟ
term_update_colors_all(ଶୂନ୍ୟ)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	ଯଦି (term->tl_vterm == ଖାଲି)
	    ଜାରି;
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
    ଯଦି (term->tl_status_text != ଖାଲି)
	ଫେରନ୍ତୁ term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    ଯଦି (term->tl_normal_mode)
    {
	ଯଦି (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	ଅନ୍ୟଥା
	    txt = (char_u *)_("Terminal-finished");
    }
    ଅନ୍ୟଥା ଯଦି (term->tl_title != ଖାଲି)
	txt = term->tl_title;
    ଅନ୍ୟଥା ଯଦି (term_none_open(term))
	txt = (char_u *)_("active");
    ଅନ୍ୟଥା ଯଦି (term_job_running(term))
	txt = (char_u *)_("running");
    ଅନ୍ୟଥା
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    ଯଦି (term->tl_status_text != ଖାଲି)
	vim_snprintf((ଅକ୍ଷର *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    ଫେରନ୍ତୁ term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    ଶୂନ୍ୟ
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_term(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    ପାଇଁ (term = first_term; !abort && term != ଖାଲି; term = term->tl_next)
	ଯଦି (term->tl_job != ଖାଲି)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
	}
    ଫେରନ୍ତୁ abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ସ୍ଥିର buf_T *
term_get_buf(typval_T *argvars, ଅକ୍ଷର *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    ଯଦି (buf == ଖାଲି || buf->b_term == ଖାଲି)
    {
	(ଶୂନ୍ୟ)tv_get_ସଂଖ୍ୟା(&argvars[0]);    // issue errmsg if type error
	ch_log(ଖାଲି, "%s: invalid buffer argument", where);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ buf;
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ସ୍ଥିର ଶୂନ୍ୟ
dump_term_color(FILE *fd, VTermColor *color)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;

    ଯଦି (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    ଅନ୍ୟଥା ଯଦି (color->type == 0)
	// use RGB values
	index = 255;
    ଅନ୍ୟଥା
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)color->red, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)color->green, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)color->blue,
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
    ଶୂନ୍ୟ
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_height = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    ଯଦି (term->tl_vterm == ଖାଲି)
    {
	emsg(_(e_job_already_finished));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ଯଦି (check_for_dict_arg(argvars, 2) == FAIL)
	    ଫେରନ୍ତୁ;
	d = argvars[2].vval.v_dict;
	ଯଦି (d != ଖାଲି)
	{
	    max_height = dict_get_ସଂଖ୍ୟା(d, "rows");
	    max_width = dict_get_ସଂଖ୍ୟା(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*fname == NUL || (fd = mch_fopen((ଅକ୍ଷର *)fname, WRITEBIN)) == ଖାଲି)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	ଫେରନ୍ତୁ;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    ପାଇଁ (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeat = 0;

	ପାଇଁ (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    same_attr;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    same_chars = TRUE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    ଯଦି (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    ପାଇଁ (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = cell.chars[i];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pc = prev_cell.chars[i];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		ଯଦି (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		ଯଦି (c != pc)
		    same_chars = FALSE;
		ଯଦି (should_break)
		    ଭାଙ୍ଗ;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    ଯଦି (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		ଯଦି (cell.chars[0] == NUL)
		    fputs(" ", fd);
		ଅନ୍ୟଥା
		{
		    char_u	charbuf[10];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

		    ପାଇଁ (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		ଯଦି (cell.width != prev_cell.width || !same_attr)
		{
		    ଯଦି (cell.width == 2)
			fputs("*", fd);
		    ଅନ୍ୟଥା
			fputs("+", fd);

		    ଯଦି (same_attr)
		    {
			fputs("&", fd);
		    }
		    ଅନ୍ୟଥା
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			ଯଦି (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			ଅନ୍ୟଥା
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			ଯଦି (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			ଅନ୍ୟଥା
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    ଯଦି (cell.width == 2)
		++pos.col;
	}
	ଯଦି (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ସ୍ଥିର ଶୂନ୍ୟ
append_cell(garray_T *gap, cellattr_T *cell)
{
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {ଖାଲି, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    max_cells = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, ଆକାର(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    ପାଇଁ (;;)
    {
	ଯଦି (c == EOF)
	    ଭାଙ୍ଗ;
	ଯଦି (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	ଅନ୍ୟଥା ଯଦି (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    ଯଦି (ga_text.ga_data == ଖାଲି)
		dump_is_corrupt(&ga_text);
	    ଯଦି (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_ଧାଡ଼ି_T   *ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		ଯଦି (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		ଧାଡ଼ି->sb_cols = ga_cell.ga_len;
		ଧାଡ଼ି->sb_bytes = ga_text.ga_len;
		ଧାଡ଼ି->sb_cells = ga_cell.ga_data;
		ଧାଡ଼ି->sb_fill_attr = term->tl_default_color;
		ଧାଡ଼ି->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    ଅନ୍ୟଥା
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	ଅନ୍ୟଥା ଯଦି (c == '|' || c == '>')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_len = ga_text.ga_len;

	    ଯଦି (c == '>')
	    {
		ଯଦି (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    ଯଦି (c != EOF)
		ga_append(&ga_text, c);
	    ପାଇଁ (;;)
	    {
		c = fgetc(fd);
		ଯଦି (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    ଭାଙ୍ଗ;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    ଯଦି (ga_text.ga_data != ଖାଲି)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		ଯଦି (prev_char.string == ଖାଲି)
		    prev_char.length = 0;
	    }

	    ଯଦି (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    ଅନ୍ୟଥା ଯଦି (c == '+' || c == '*')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		ଯଦି (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		ଅନ୍ୟଥା ଯଦି (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    ଯେତେବେଳେ (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    ପାଇଁ (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			ଯଦି (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			ଅନ୍ୟଥା ଯଦି (c == '#')
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା red, green, blue, index = 0, type;

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
			    ଯଦି (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    ଯେତେବେଳେ (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    ଯଦି (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				ଯଦି (index == 0)
				{
				    ଯଦି (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    ଅନ୍ୟଥା
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    ଅନ୍ୟଥା
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    ଯଦି (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    ଅନ୍ୟଥା
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			ଅନ୍ୟଥା
			    dump_is_corrupt(&ga_text);
		    }
		}
		ଅନ୍ୟଥା
		    dump_is_corrupt(&ga_text);
	    }
	    ଅନ୍ୟଥା
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    ଯଦି (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	ଅନ୍ୟଥା ଯଦି (c == '@')
	{
	    ଯଦି (prev_char.string == ଖାଲି)
		dump_is_corrupt(&ga_text);
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

		// repeat previous character, get the count
		ପାଇଁ (;;)
		{
		    c = fgetc(fd);
		    ଯଦି (!SAFE_isdigit(c))
			ଭାଙ୍ଗ;
		    ଗଣନା_2 = ଗଣନା_2 * 10 + (c - '0');
		}

		ଯେତେବେଳେ (ଗଣନା_2-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    ଯଦି (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    ଫେରନ୍ତୁ max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ସ୍ଥିର char_u *
get_separator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା text_width, char_u *fname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    fname_size;
    char_u  *p = fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    ଯଦି (textline == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    fname_size = vim_strsize(fname);
    ଯଦି (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    ଅନ୍ୟଥା ଯଦି (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    ଯେତେବେଳେ (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    ପାଇଁ (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    ପାଇଁ (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    ଫେରନ୍ତୁ textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_load_dump(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ଖାଲି;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ଖାଲି;
    char_u	*fname_tofree = ଖାଲି;
    FILE	*fd1;
    FILE	*fd2 = ଖାଲି;
    char_u	*textline = ଖାଲି;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    ଯଦି (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    ଯଦି (fname1 == ଖାଲି || (do_diff && fname2 == ଖାଲି))
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }
    fd1 = mch_fopen((ଅକ୍ଷର *)fname1, READBIN);
    ଯଦି (fd1 == ଖାଲି)
    {
	semsg(_(e_cant_read_file_str), fname1);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (do_diff)
    {
	fd2 = mch_fopen((ଅକ୍ଷର *)fname2, READBIN);
	ଯଦି (fd2 == ଖାଲି)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    ଫେରନ୍ତୁ;
	}
    }

    init_job_options(&opt);
    ଯଦି (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	ଯାଅ theend;

    ଯଦି (opt.jo_term_name == ଖାଲି)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	ଯଦି (fname_tofree != ଖାଲି)
	{
	    vim_snprintf((ଅକ୍ଷର *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    ଯଦି (opt.jo_bufnr_buf != ଖାଲି)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	ଯଦି (wp == ଖାଲି)
	    semsg(_(e_invalid_argument_str), "bufnr");
	ଅନ୍ୟଥା
	{
	    buf = curbuf;
	    ଯେତେବେଳେ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    ଅନ୍ୟଥା
	// Create a new terminal window.
	buf = term_start(&argvars[0], ଖାଲି, &opt, TERM_START_NOJOB);

    ଯଦି (buf != ଖାଲି && buf->b_term != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_ସଂଖ୍ୟା = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	ଯଦି (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	ଯଦି (!do_diff)
	    ଯାଅ theend;

	term->tl_ଶୀର୍ଷ_diff_rows = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

	textline = get_separator(width, fname1);
	ଯଦି (textline == ଖାଲି)
	    ଯାଅ theend;
	ଯଦି (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	ଯଦି (textline == ଖାଲି)
	    ଯାଅ theend;
	ଯଦି (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	width2 = read_dump_file(fd2, &cursor_pos2);
	ଯଦି (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    ଯଦି (textline == ଖାଲି)
		ଯାଅ theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - bot_lnum;

	ପାଇଁ (lnum = 1; lnum <= term->tl_ଶୀର୍ଷ_diff_rows; ++lnum)
	{
	    ଯଦି (lnum + bot_lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		// bottom part has fewer rows, fill with "-"
		ପାଇଁ (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    ଅନ୍ୟଥା
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col;
		sb_ଧାଡ଼ି_T   *sb_ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_ଧାଡ଼ି + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_ଧାଡ଼ି + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		ଯଦି (line1 == ଖାଲି)
		    ଭାଙ୍ଗ;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		ପାଇଁ (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len1 = utfc_ptr2len(p1);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    ଯଦି (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    ଅନ୍ୟଥା ଯଦି (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    ଅନ୍ୟଥା ଯଦି (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    ଅନ୍ୟଥା ଯଦି (cellattr1 != ଖାଲି && cellattr2 != ଖାଲି)
		    {
			ଯଦି ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			ଅନ୍ୟଥା ଯଦି (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			ଅନ୍ୟଥା ଯଦି (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			ଅନ୍ୟଥା ଯଦି (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		ଯେତେବେଳେ (col < width)
		{
		    ଯଦି (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    ଅନ୍ୟଥା ଯଦି (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    ଅନ୍ୟଥା
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    ଯଦି (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_ଶୀର୍ଷ_diff_rows) == OK)
		ml_append(term->tl_ଶୀର୍ଷ_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	ଯେତେବେଳେ (lnum + bot_lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    // bottom part has more rows, fill with "+"
	    ପାଇଁ (i = 0; i < width; ++i)
		textline[i] = '+';
	    ଯଦି (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_ଶୀର୍ଷ_diff_rows) == OK)
		ml_append(term->tl_ଶୀର୍ଷ_diff_rows + lnum, textline, 0, FALSE);
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
    ଯଦି (fd2 != ଖାଲି)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_swap_diff(ଶୂନ୍ୟ)
{
    term_T	*term = curbuf->b_term;
    linenr_T	ଧାଡ଼ି_ଗଣନା;
    linenr_T	ଶୀର୍ଷ_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_ଧାଡ଼ି_T	*sb_ଧାଡ଼ି;

    ଯଦି (term == ଖାଲି
	    || !term_is_finished(curbuf)
	    || term->tl_ଶୀର୍ଷ_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	ଫେରନ୍ତୁ FAIL;

    ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଶୀର୍ଷ_rows = term->tl_ଶୀର୍ଷ_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = ଧାଡ଼ି_ଗଣନା - bot_rows;
    sb_ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    ପାଇଁ (lnum = 1; lnum <= ଶୀର୍ଷ_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    ପାଇଁ (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ OK;
    ml_append(ଧାଡ଼ି_ଗଣନା - ଶୀର୍ଷ_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(ଧାଡ଼ି_ଗଣନା - ଶୀର୍ଷ_rows));
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ OK;
    ml_delete(ଧାଡ଼ି_ଗଣନା - ଶୀର୍ଷ_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    ଯଦି (ଶୀର୍ଷ_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	ପାଇଁ (lnum = 0; lnum < ଶୀର୍ଷ_rows; ++lnum)
	{
	    sb_ଧାଡ଼ି_T	temp;

	    temp = *(sb_ଧାଡ଼ି + lnum);
	    *(sb_ଧାଡ଼ି + lnum) = *(sb_ଧାଡ଼ି + bot_start + lnum);
	    *(sb_ଧାଡ଼ି + bot_start + lnum) = temp;
	}
    }
    ଅନ୍ୟଥା
    {
	size_t		size = ଆକାର(sb_ଧାଡ଼ି_T) * term->tl_scrollback.ga_len;
	sb_ଧାଡ଼ି_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	ଯଦି (temp != ଖାଲି)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    ଆକାର(sb_ଧାଡ଼ି_T) * bot_rows);
	    mch_memmove((sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + ଶୀର୍ଷ_rows,
		    ଆକାର(sb_ଧାଡ଼ି_T) * (ଧାଡ଼ି_ଗଣନା - ଶୀର୍ଷ_rows - bot_rows));
	    mch_memmove((sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data
						       + ଧାଡ଼ି_ଗଣନା - ଶୀର୍ଷ_rows,
		    temp,
		    ଆକାର(sb_ଧାଡ଼ି_T) * ଶୀର୍ଷ_rows);
	    vim_free(temp);
	}
    }

    term->tl_ଶୀର୍ଷ_diff_rows = bot_rows;
    term->tl_bot_diff_rows = ଶୀର୍ଷ_rows;

    update_screen(UPD_NOT_VALID);
    ଫେରନ୍ତୁ OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    ଶୂନ୍ୟ
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    ଶୂନ୍ୟ
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    ଶୂନ୍ୟ
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    rettv->vval.v_ସଂଖ୍ୟା = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    ଶୂନ୍ୟ
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr;
    size_t  i;
    char_u  *name;

    ସ୍ଥିର ସଂରଚନା {
	ଅକ୍ଷର	    *name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    attr = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    ପାଇଁ (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	ଯଦି (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_ସଂଖ୍ୟା = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    ଭାଙ୍ଗ;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    ଶୂନ୍ୟ
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getcursor()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_ସଂଖ୍ୟା(l, term->tl_cursor_pos.row + 1);
    list_append_ସଂଖ୍ୟା(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ;

    dict_add_ସଂଖ୍ୟା(d, "visible", term->tl_cursor_visible);
    dict_add_ସଂଖ୍ୟା(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_ସଂଖ୍ୟା(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    ଯଦି (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    ଶୂନ୍ୟ
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getjob()");
    ଯଦି (buf == ଖାଲି)
    {
	ଯଦି (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
	}
	ଫେରନ୍ତୁ;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    ଯଦି (rettv->vval.v_job != ଖାଲି)
	++rettv->vval.v_job->jv_refcount;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_row_ସଂଖ୍ୟା(typval_T *tv, term_T *term)
{
    ଯଦି (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ଖାଲି
	    && STRCMP(tv->vval.v_string, ".") == 0)
	ଫେରନ୍ତୁ term->tl_cursor_pos.row;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    ଶୂନ୍ୟ
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    row;

    rettv->v_type = VAR_STRING;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getline()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    row = get_row_ସଂଖ୍ୟା(&argvars[1], term);

    ଯଦି (term->tl_vterm == ଖାଲି)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  sb_row = term->tl_scrollback_scrolled + row;
	sb_ଧାଡ଼ି_T *ଧାଡ଼ି;

	ଯଦି (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    ଫେରନ୍ତୁ;
	ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ଖାଲି, &offset);

	// vterm is finished, get the text from the buffer
	ଯଦି (lnum > 0 && lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    ଯଦି (STRLEN(p) >= offset + ଧାଡ଼ି->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, ଧାଡ଼ି->sb_bytes);
	}
    }
    ଅନ୍ୟଥା
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	char_u		*p;

	ଯଦି (row < 0 || row >= term->tl_rows)
	    ଫେରନ୍ତୁ;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (ଅକ୍ଷର *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    ଶୂନ୍ୟ
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getscrolled()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    rettv->vval.v_ସଂଖ୍ୟା = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    ଶୂନ୍ୟ
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getsize()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    l = rettv->vval.v_list;
    list_append_ସଂଖ୍ୟା(l, buf->b_term->tl_rows);
    list_append_ସଂଖ୍ୟା(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    ଶୂନ୍ୟ
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_setsize()");
    ଯଦି (buf == ଖାଲି)
    {
	emsg(_(e_not_terminal_buffer));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (buf->b_term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    rows = tv_get_ସଂଖ୍ୟା(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_ସଂଖ୍ୟା(&argvars[2]);
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
    ଶୂନ୍ୟ
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getstatus()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;

    ଯଦି (term_job_running(term))
	STRCPY(val, "running");
    ଅନ୍ୟଥା
	STRCPY(val, "finished");
    ଯଦି (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    ଶୂନ୍ୟ
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_gettitle()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (buf->b_term->tl_title != ଖାଲି)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    ଶୂନ୍ୟ
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ = 0;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	ଅଙ୍କ = tv_get_bool(&argvars[1]);

    ବାଛ (ଅଙ୍କ)
    {
	ସ୍ଥିତି 0:
	    ଯଦି (buf->b_term->tl_job != ଖାଲି)
		p = buf->b_term->tl_job->jv_tty_out;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 1:
	    ଯଦି (buf->b_term->tl_job != ଖାଲି)
		p = buf->b_term->tl_job->jv_tty_in;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    ଫେରନ୍ତୁ;
    }
    ଯଦି (p != ଖାଲି)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    ଶୂନ୍ୟ
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    ଯଦି (rettv_list_alloc(rettv) == FAIL || first_term == ଖାଲି)
	ଫେରନ୍ତୁ;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	ଯଦି (tp->tl_buffer != ଖାଲି)
	    ଯଦି (list_append_ସଂଖ୍ୟା(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		ଫେରନ୍ତୁ;
}

/*
 * "term_scrape(buf, row)" function
 */
    ଶୂନ୍ୟ
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ଖାଲି;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_ଧାଡ଼ି_T	    *ଧାଡ଼ି;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_scrape()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_ସଂଖ୍ୟା(&argvars[1], term);

    ଯଦି (term->tl_vterm != ଖାଲି)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	ଯଦି (screen == ଖାଲି)  // can't really happen
	    ଫେରନ୍ତୁ;
	p = ଖାଲି;
	ଧାଡ଼ି = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ଖାଲି, &offset);

	ଯଦି (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଫେରନ୍ତୁ;

	ଧାଡ଼ି = (sb_ଧାଡ଼ି_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	ଯଦି (STRLEN(p) < offset + ଧାଡ଼ି->sb_bytes)
	    ଫେରନ୍ତୁ;

	p += offset;
    }

    ପାଇଁ (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	ଯଦି (screen == ଖାଲି)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    ଯଦି (pos.col >= ଧାଡ଼ି->sb_cols)
		ଭାଙ୍ଗ;
	    cellattr = ଧାଡ଼ି->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	ଅନ୍ୟଥା
	{
	    VTermScreenCell cell;

	    ଯଦି (vterm_screen_get_cell(screen, pos, &cell) == 0)
		ଭାଙ୍ଗ;

	    mbslen = 0;
	    ପାଇଁ (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		ଯଦି (cell.chars[i] == 0)
		    ଭାଙ୍ଗ;
		mbslen += (*utf_char2bytes)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	ଯଦି (dcell == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    ଭାଙ୍ଗ;
	}

	dict_add_string_len(dcell, "chars", mbs, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mbslen);

	rgblen = vim_snprintf_safelen((ଅକ୍ଷର *)rgb, ଆକାର(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rgblen);
	rgblen = vim_snprintf_safelen((ଅକ୍ଷର *)rgb, ଆକାର(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rgblen);

	dict_add_ସଂଖ୍ୟା(dcell, "attr",
				      cell2attr(term, ଖାଲି, &attrs, &fg, &bg));
	dict_add_ସଂଖ୍ୟା(dcell, "width", width);

	++pos.col;
	ଯଦି (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    ଶୂନ୍ୟ
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*ସନ୍ଦେଶ;
    term_T	*term;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_sendkeys()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ସନ୍ଦେଶ = tv_get_string_chk(&argvars[1]);
    ଯଦି (ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    ଯଦି (term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (*ସନ୍ଦେଶ != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

	ଯଦି (*ସନ୍ଦେଶ == K_SPECIAL && ସନ୍ଦେଶ[1] != NUL && ସନ୍ଦେଶ[2] != NUL)
	{
	    c = TO_SPECIAL(ସନ୍ଦେଶ[1], ସନ୍ଦେଶ[2]);
	    ସନ୍ଦେଶ += 3;
	}
	ଅନ୍ୟଥା
	{
	    c = PTR2CHAR(ସନ୍ଦେଶ);
	    ସନ୍ଦେଶ += MB_CPTR2LEN(ସନ୍ଦେଶ);
	}
	send_ଚାବିଗୁଡ଼ିକ_to_term(term, c, 0, FALSE);
    }
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    ଶୂନ୍ୟ
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		index;
    list_T	*list;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_getansicolors()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    ଯଦି (term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    ପାଇଁ (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((ଅକ୍ଷର *)hexbuf, ଆକାର(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	ଯଦି (list_append_string(list, hexbuf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)hexbuflen) == FAIL)
	    ଫେରନ୍ତୁ;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    ଶୂନ୍ୟ
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_setansicolors()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    ଯଦି (term->tl_vterm == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "\"colors\"");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (term->tl_palette == ଖାଲି)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    ଯଦି (term->tl_palette == ଖାଲି)
	ଫେରନ୍ତୁ;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ଯଦି (color_name == ଖାଲି)
	    ଫେରନ୍ତୁ;

	guicolor = GUI_GET_COLOR(color_name);
	ଯଦି (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    ଫେରନ୍ତୁ;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    ଶୂନ୍ୟ
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_setapi()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    ଯଦି (api != ଖାଲି)
	term->tl_api = vim_strsave(api);
    ଅନ୍ୟଥା
	term->tl_api = ଖାଲି;
}

/*
 * "term_setrestore(buf, command)" function
 */
    ଶୂନ୍ୟ
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ଯଦି defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_setrestore()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    ଯଦି (cmd != ଖାଲି)
	term->tl_command = vim_strsave(cmd);
    ଅନ୍ୟଥା
	term->tl_command = ଖାଲି;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    ଶୂନ୍ୟ
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_setkill()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    ଯଦି (how != ଖାଲି)
	term->tl_kill = vim_strsave(how);
    ଅନ୍ୟଥା
	term->tl_kill = ଖାଲି;
}

/*
 * "term_start(command, options)" function
 */
    ଶୂନ୍ୟ
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    init_job_options(&opt);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	ଫେରନ୍ତୁ;

    buf = term_start(&argvars[0], ଖାଲି, &opt, 0);

    ଯଦି (buf != ଖାଲି && buf->b_term != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    ଶୂନ୍ୟ
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    buf = term_get_buf(argvars, "term_wait()");
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (buf->b_term->tl_job == ଖାଲି)
    {
	ch_log(ଖାଲି, "term_wait(): no job to wait for");
	ଫେରନ୍ତୁ;
    }
    ଯଦି (buf->b_term->tl_job->jv_channel == ଖାଲି)
	// channel is closed, nothing to do
	ଫେରନ୍ତୁ;

    // Get the job status, this will detect a job that finished.
    ଯଦି (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ଖାଲି, "term_wait(): waiting for channel to close");
	ଯେତେବେଳେ (buf->b_term != ଖାଲି && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    ଯଦି (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		ଭାଙ୍ଗ;
	    ଯଦି (buf->b_term == ଖାଲି || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		ଭାଙ୍ଗ;
	}

	term_flush_messages();
    }
    ଅନ୍ୟଥା
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_ସଂଖ୍ୟା(&argvars[1]);
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
    ଶୂନ୍ୟ
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	ଯଦି (term->tl_job == ch->ch_job)
	{
	    ଯଦି (term->tl_eof_chars != ଖାଲି)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(term->tl_eof_chars), ଖାଲି);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, ଖାଲି);
	    }
#ifdef MSWIN
	    ଅନ୍ୟଥା
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, ଖାଲି);
#endif
	}
}

#ଯଦି defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    ଫେରନ୍ତୁ term != ଖାଲି ? term->tl_job : ଖାଲି;
}
#endif

#ଯଦି defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
ଶୂନ୍ୟ (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dyn_conpty_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ସ୍ଥିର HMODULE	hKerneldll = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    ସ୍ଥିର ସଂରଚନା
    {
	ଅକ୍ଷର	*name;
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
	{ଖାଲି, ଖାଲି}
    };

    ଯଦି (!has_conpty_working())
    {
	ଯଦି (verbose)
	    emsg(_(e_conpty_is_not_available));
	ଫେରନ୍ତୁ FAIL;
    }

    // No need to initialize twice.
    ଯଦି (hKerneldll)
	ଫେରନ୍ତୁ OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    ପାଇଁ (i = 0; conpty_entry[i].name != ଖାଲି
					&& conpty_entry[i].ptr != ଖାଲି; ++i)
    {
	ଯଦି ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == ଖାଲି)
	{
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = ଖାଲି;
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ଅକ୍ଷର	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ଖାଲି;
    WCHAR	    *cmd_wchar_copy = ଖାଲି;
    WCHAR	    *cwd_wchar = ଖାଲି;
    WCHAR	    *env_wchar = ଖାଲି;
    channel_T	    *channel = ଖାଲି;
    job_T	    *job = ଖାଲି;
    HANDLE	    jo = ଖାଲି;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ଖାଲି;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ଖାଲି;
    HANDLE	    o_theirs = ଖାଲି;
    HANDLE	    i_ours = ଖାଲି;
    HANDLE	    o_ours = ଖାଲି;
    ଅକ୍ଷର	    *errmsg = ଖାଲି;

    ga_init2(&ga_cmd, ଆକାର(ଅକ୍ଷର*), 20);
    ga_init2(&ga_env, ଆକାର(ଅକ୍ଷର*), 20);

    ଯଦି (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ଅନ୍ୟଥା ଯଦି (argvar->v_type == VAR_LIST)
    {
	ଯଦି (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ଯାଅ failed;
	cmd = ga_cmd.ga_data;
    }
    ଯଦି (cmd == ଖାଲି || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	ଯାଅ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ଖାଲି);

    ଯଦି (cmd_wchar != ଖାଲି)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    ଯଦି (cmd_wchar == ଖାଲି)
	ଯାଅ failed;
    ଯଦି (opt->jo_cwd != ଖାଲି)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ଖାଲି);
	ଯଦି (cwd_wchar == ଖାଲି)
	    ଯାଅ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    ଯଦି (!CreatePipe(&i_theirs, &i_ours, ଖାଲି, 0))
	ଯାଅ failed;
    ଯଦି (!CreatePipe(&o_ours, &o_theirs, ଖାଲି, 0))
	ଯାଅ failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    ଯଦି (FAILED(hr))
	ଯାଅ failed;

    term->tl_siex.StartupInfo.cb = ଆକାର(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_ନିବେଶ = INVALID_HANDLE_ମୂଲ୍ୟ;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_ମୂଲ୍ୟ;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_ମୂଲ୍ୟ;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ଖାଲି, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    ଯଦି (!term->tl_siex.lpAttributeList)
	ଯାଅ failed;
    ଯଦି (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	ଯାଅ failed;
    ଯଦି (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    ଆକାର(HPCON), ଖାଲି, ଖାଲି))
	ଯାଅ failed;

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
	ଯାଅ failed;

    job = job_alloc();
    ଯଦି (job == ଖାଲି)
	ଯାଅ failed;
    ଯଦି (argvar->v_type == VAR_STRING)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    ଯଦି (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    ଯଦି (!CreateProcessW(ଖାଲି, cmd_wchar_copy, ଖାଲି, ଖାଲି, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	ଯାଅ failed;
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

    jo = CreateJobObject(ଖାଲି, ଖାଲି);
    ଯଦି (jo == ଖାଲି)
	ଯାଅ failed;

    ଯଦି (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ଖାଲି;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ଯଦି (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ଯାଅ failed;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ଯଦି (term_use_palette())
    {
	ଯଦି (term->tl_palette != ଖାଲି)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ଅନ୍ୟଥା
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
    ଯଦି (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((ଅକ୍ଷର *)fname, WRITEBIN);
	ଯଦି (term->tl_out_fd == ଖାଲି)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ଫେରନ୍ତୁ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    ଯଦି (channel != ଖାଲି)
	channel_clear(channel);
    ଯଦି (job != ଖାଲି)
    {
	job->jv_channel = ଖାଲି;
	job_cleanup(job);
    }
    term->tl_job = ଖାଲି;
    ଯଦି (jo != ଖାଲି)
	CloseHandle(jo);

    ଯଦି (term->tl_siex.lpAttributeList != ଖାଲି)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ଖାଲି;
    ଯଦି (o_theirs != ଖାଲି)
	CloseHandle(o_theirs);
    ଯଦି (o_ours != ଖାଲି)
	CloseHandle(o_ours);
    ଯଦି (i_ours != ଖାଲି)
	CloseHandle(i_ours);
    ଯଦି (i_theirs != ଖାଲି)
	CloseHandle(i_theirs);
    ଯଦି (term->tl_conpty != ଖାଲି)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ଖାଲି;
    // Propagate errors that occur in CreateProcess
    ଯଦି (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    ଫେରନ୍ତୁ FAIL;
}

    ସ୍ଥିର ଶୂନ୍ୟ
conpty_term_report_winsize(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    ସ୍ଥିର ଶୂନ୍ୟ
term_free_conpty(term_T *term)
{
    ଯଦି (term->tl_siex.lpAttributeList != ଖାଲି)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ଖାଲି;
    ଯଦି (term->tl_conpty != ଖାଲି)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_conpty(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

ଶୂନ୍ୟ* (*winpty_config_new)(UINT64, ଶୂନ୍ୟ*);
ଶୂନ୍ୟ* (*winpty_open)(ଶୂନ୍ୟ*, ଶୂନ୍ୟ*);
ଶୂନ୍ୟ* (*winpty_spawn_config_new)(UINT64, ଶୂନ୍ୟ*, LPCWSTR, ଶୂନ୍ୟ*, ଶୂନ୍ୟ*, ଶୂନ୍ୟ*);
BOOL (*winpty_spawn)(ଶୂନ୍ୟ*, ଶୂନ୍ୟ*, HANDLE*, HANDLE*, DWORD*, ଶୂନ୍ୟ*);
ଶୂନ୍ୟ (*winpty_config_set_mouse_mode)(ଶୂନ୍ୟ*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ (*winpty_config_set_initial_size)(ଶୂନ୍ୟ*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
LPCWSTR (*winpty_conin_name)(ଶୂନ୍ୟ*);
LPCWSTR (*winpty_conout_name)(ଶୂନ୍ୟ*);
LPCWSTR (*winpty_conerr_name)(ଶୂନ୍ୟ*);
ଶୂନ୍ୟ (*winpty_free)(ଶୂନ୍ୟ*);
ଶୂନ୍ୟ (*winpty_config_free)(ଶୂନ୍ୟ*);
ଶୂନ୍ୟ (*winpty_spawn_config_free)(ଶୂନ୍ୟ*);
ଶୂନ୍ୟ (*winpty_error_free)(ଶୂନ୍ୟ*);
LPCWSTR (*winpty_error_ସନ୍ଦେଶ)(ଶୂନ୍ୟ*);
BOOL (*winpty_set_size)(ଶୂନ୍ୟ*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ*);
HANDLE (*winpty_agent_process)(ଶୂନ୍ୟ*);

# define WINPTY_DLL "winpty.dll"

ସ୍ଥିର HINSTANCE hWinPtyDLL = ଖାଲି;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dyn_winpty_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ସ୍ଥିର ସଂରଚନା
    {
	ଅକ୍ଷର	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_ସନ୍ଦେଶ},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ଖାଲି, ଖାଲି}
    };

    // No need to initialize twice.
    ଯଦି (hWinPtyDLL)
	ଫେରନ୍ତୁ OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    ଯଦି (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((ଅକ୍ଷର *)p_winptydll);
    ଯଦି (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    ଯଦି (!hWinPtyDLL)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	ଫେରନ୍ତୁ FAIL;
    }
    ପାଇଁ (i = 0; winpty_entry[i].name != ଖାଲି
					 && winpty_entry[i].ptr != ଖାଲି; ++i)
    {
	ଯଦି ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == ଖାଲି)
	{
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = ଖାଲି;
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ଅକ୍ଷର	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ଖାଲି;
    WCHAR	    *cwd_wchar = ଖାଲି;
    WCHAR	    *env_wchar = ଖାଲି;
    channel_T	    *channel = ଖାଲି;
    job_T	    *job = ଖାଲି;
    DWORD	    error;
    HANDLE	    jo = ଖାଲି;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    ଶୂନ୍ୟ	    *winpty_err = ଖାଲି;
    ଶୂନ୍ୟ	    *spawn_config = ଖାଲି;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ଖାଲି;

    ga_init2(&ga_cmd, ଆକାର(ଅକ୍ଷର*), 20);
    ga_init2(&ga_env, ଆକାର(ଅକ୍ଷର*), 20);

    ଯଦି (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ଅନ୍ୟଥା ଯଦି (argvar->v_type == VAR_LIST)
    {
	ଯଦି (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ଯାଅ failed;
	cmd = ga_cmd.ga_data;
    }
    ଯଦି (cmd == ଖାଲି || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	ଯାଅ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ଖାଲି);
    ga_clear(&ga_cmd);
    ଯଦି (cmd_wchar == ଖାଲି)
	ଯାଅ failed;
    ଯଦି (opt->jo_cwd != ଖାଲି)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ଖାଲି);
	ଯଦି (cwd_wchar == ଖାଲି)
	    ଯାଅ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    ଯଦି (term->tl_winpty_config == ଖାଲି)
	ଯାଅ failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    ଯଦି (term->tl_winpty == ଖାଲି)
	ଯାଅ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ଖାଲି,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    ଯଦି (spawn_config == ଖାଲି)
	ଯାଅ failed;

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
	ଯାଅ failed;

    job = job_alloc();
    ଯଦି (job == ଖାଲି)
	ଯାଅ failed;
    ଯଦି (argvar->v_type == VAR_STRING)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    ଯଦି (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    ଯଦି (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	ଯାଅ failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, ଖାଲି,
	    OPEN_EXISTING, 0, ଖାଲି),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, ଖାଲି,
	    OPEN_EXISTING, 0, ଖାଲି),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, ଖାଲି,
	    OPEN_EXISTING, 0, ଖାଲି));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(ଖାଲି, ଖାଲି);
    ଯଦି (jo == ଖାଲି)
	ଯାଅ failed;

    ଯଦି (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ଖାଲି;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ଯଦି (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ଯାଅ failed;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ଯଦି (term_use_palette())
    {
	ଯଦି (term->tl_palette != ଖାଲି)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ଅନ୍ୟଥା
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
	    (short_u *)winpty_conin_name(term->tl_winpty), ଖାଲି);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), ଖାଲି);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ଯଦି (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((ଅକ୍ଷର *)fname, WRITEBIN);
	ଯଦି (term->tl_out_fd == ଖାଲି)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ଫେରନ୍ତୁ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    ଯଦି (spawn_config != ଖାଲି)
	winpty_spawn_config_free(spawn_config);
    ଯଦି (channel != ଖାଲି)
	channel_clear(channel);
    ଯଦି (job != ଖାଲି)
    {
	job->jv_channel = ଖାଲି;
	job_cleanup(job);
    }
    term->tl_job = ଖାଲି;
    ଯଦି (jo != ଖାଲି)
	CloseHandle(jo);
    ଯଦି (term->tl_winpty != ଖାଲି)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ଖାଲି;
    ଯଦି (term->tl_winpty_config != ଖାଲି)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ଖାଲି;
    ଯଦି (winpty_err != ଖାଲି)
    {
	ଅକ୍ଷର *ସନ୍ଦେଶ = (ଅକ୍ଷର *)utf16_to_enc(
				(short_u *)winpty_error_ସନ୍ଦେଶ(winpty_err), ଖାଲି);

	ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
	{
	    emsg(ସନ୍ଦେଶ);
	    vim_free(ସନ୍ଦେଶ);
	}
	winpty_error_free(winpty_err);
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ଅକ୍ଷର	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    use_winpty = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    use_conpty = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    ଯଦି (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	ଫେରନ୍ତୁ dyn_winpty_init(TRUE);

    ଯଦି (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    ଯଦି (tty_type == NUL)
    {
	ଯଦି (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	ଅନ୍ୟଥା ଯଦି (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    ଅନ୍ୟଥା ଯଦି (tty_type == 'w')	// winpty
    {
	ଯଦି (has_winpty)
	    use_winpty = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (tty_type == 'c')	// conpty
    {
	ଯଦି (has_conpty)
	    use_conpty = TRUE;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ dyn_conpty_init(TRUE);
    }

    ଯଦି (use_conpty)
	ଫେରନ୍ତୁ conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    ଯଦି (use_winpty)
	ଫେରନ୍ତୁ winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    ଫେରନ୍ତୁ dyn_winpty_init(TRUE);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_ମୂଲ୍ୟ;
    HANDLE	    hPipeOut = INVALID_HANDLE_ମୂଲ୍ୟ;
    ଅକ୍ଷର	    in_name[80], out_name[80];
    channel_T	    *channel = ଖାଲି;

    ଯଦି (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    vim_snprintf(in_name, ଆକାର(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ଖାଲି);
    ଯଦି (hPipeIn == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଯାଅ failed;

    vim_snprintf(out_name, ଆକାର(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ଖାଲି);
    ଯଦି (hPipeOut == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଯାଅ failed;

    ConnectNamedPipe(hPipeIn, ଖାଲି);
    ConnectNamedPipe(hPipeOut, ଖାଲି);

    term->tl_job = job_alloc();
    ଯଦି (term->tl_job == ଖାଲି)
	ଯାଅ failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
	ଯାଅ failed;
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

    ଫେରନ୍ତୁ OK;

failed:
    ଯଦି (hPipeIn != ଖାଲି)
	CloseHandle(hPipeIn);
    ଯଦି (hPipeOut != ଖାଲି)
	CloseHandle(hPipeOut);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    ଯଦି (term->tl_winpty != ଖାଲି)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ଖାଲି;
    ଯଦି (term->tl_winpty_config != ଖାଲି)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ଖାଲି;
    ଯଦି (term->tl_vterm != ଖାଲି)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ଖାଲି;
}

/*
 * Report the size to the terminal.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_report_winsize(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
    ଯଦି (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    ଯଦି (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
terminal_enabled(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#ଅନ୍ୟଥା

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	ଅକ୍ଷର	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = ଖାଲି;

    ଯଦି (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ଫେରନ୍ତୁ FAIL;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ଯଦି (term_use_palette())
    {
	ଯଦି (term->tl_palette != ଖାଲି)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ଅନ୍ୟଥା
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    ଯଦି (term->tl_job != ଖାଲି)
	++term->tl_job->jv_refcount;

    ଫେରନ୍ତୁ term->tl_job != ଖାଲି
	&& term->tl_job->jv_channel != ଖାଲି
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
create_pty_only(term_T *term, jobopt_T *opt)
{
    ଯଦି (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    term->tl_job = job_alloc();
    ଯଦି (term->tl_job == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    ଫେରନ୍ତୁ mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_free_vterm(term_T *term)
{
    ଯଦି (term->tl_vterm != ଖାଲି)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ଖାଲି;
}

/*
 * Report the size to the terminal.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
term_report_winsize(term_T *term, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
    // Use an ioctl() to report the new window size to the job.
    ଯଦି (term->tl_job == ଖାଲି || term->tl_job->jv_channel == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା part;

    ପାଇଁ (part = PART_OUT; part < PART_ଗଣନା; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	ଯଦି (mch_isatty(fd))
	    ଭାଙ୍ଗ;
    }
    ଯଦି (part < PART_ଗଣନା && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
